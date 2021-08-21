#include "stdafx.h"

#include "ServerCmd.h"
#include "data_buff.h"
#include "uart_cmd.h"
#include "mywin.h"
#include "qcloud_iot_export_log.h"

buff_queue_t uart_tx_queue;
buff_queue_t uart_rx_queue;
buf_t uart_rx_buff;
static Boolean uart_busy;
static net_line_t *pnet;

HANDLE		UARTHandle[MAX_PORT_NUM] = {0};
COMSTAT		comstat[MAX_PORT_NUM] = {0};
static BOOL breadytoread[MAX_PORT_NUM] = {0};
static BOOL breadytowrite[MAX_PORT_NUM] = {0};
static OVERLAPPED com_ov;
static HANDLE hWriteEvent;

static OVERLAPPED com_ov_ex[MAX_PORT_NUM];
static HANDLE hWriteEvent_ex[MAX_PORT_NUM];
buf_t g_uart_rx_buff[MAX_PORT_NUM];

static Boolean m_bUartThreadExit = FALSE; //add by cbk 20180829
//static int m_port = 0;//add by cbk 20180829


static uart_rsp_cb_t fn_uart_rsp_func;
static uart_rsp_ex_cb_t fn_uart_rsp_ex_func;			// 通用的，增加端口号

Boolean init_uart_buff()
{
	Boolean ret = FALSE;
	static char uart_buff[1024 * 10];

	init_mem_pool(NULL);
	init_buff_queue(&uart_tx_queue, "uart tx");
	init_buff_queue(&uart_rx_queue, "uart rx");

	init_buffer(&uart_rx_buff);

	uart_rx_buff.size = sizeof(uart_buff);
	uart_rx_buff.pbuff = uart_buff;
	
#ifdef WIN32
	if ((ret = win32_UART_Open(HX_WIN32_UART_PORT)))
	{
		win32_start_thread(win32_uart_recv_thread, NULL);

		return TRUE;
	}
	else 
	{
		Log_e(_T("open uart%d failed!\n"), HX_WIN32_UART_PORT);
		return FALSE;
	}
#endif

	return TRUE;
}

static  char g_uart_buff[MAX_PORT_NUM][BUF_SIZE] = {0};

/************************************************************************/
/* 
功能：初始化指定串口数据
port:串口号
返回值：成功：TRUE，失败:FALSE
*/
/************************************************************************/
Boolean init_uart_buff_ex()
{
	int i;

	init_mem_pool(NULL);

	for (i = 0; i < MAX_PORT_NUM; i++)
	{
		init_buffer(&g_uart_rx_buff[i]);

		g_uart_rx_buff[i].size = sizeof(g_uart_buff[i]);
		g_uart_rx_buff[i].pbuff = g_uart_buff[i];
		clear_uart_rx_buff(i);
	}

	return TRUE;
}

/************************************************************************/
/* 
功能：清除指定串口数据，其实是把接收串口数据的队列清空
port:串口号
*/
/************************************************************************/
void clear_uart_rx_buff(int port)
{
	memset(g_uart_rx_buff[port].pbuff, 0, g_uart_rx_buff[port].size);
	g_uart_rx_buff[port].len = 0;
}

/************************************************************************/
/* 
功能：读取串口数据
port:串口号
out_buf：接收串口数据buff
buf_len: out_buf的长度
actual_len：实际读取数据的长度
返回值：成功：TRUE，失败:FALSE
*/
/************************************************************************/
Boolean get_uart_rx_buff(int port, unsigned char *out_buf, int buf_len, int *actual_len)
{
	Boolean bRet = TRUE;
	int data_len = g_uart_rx_buff[port].len;

	if (data_len > buf_len)
	{
		data_len = buf_len;
		bRet = FALSE;
	}

	memcpy(out_buf, g_uart_rx_buff[port].pbuff, data_len);
	*actual_len = data_len;

	return bRet;
}

/************************************************************************/
/* 
功能：打开指定串口
port:串口号
返回值：成功：TRUE，失败:FALSE
*/
/************************************************************************/
Boolean init_serial_port(int port)
{
	Boolean ret = FALSE;
	static int nPort = 0;

#ifdef WIN32
	if ((ret = win32_UART_Open_ex(port)))
	{
		nPort = port;
		m_bUartThreadExit = FALSE;
		//init_uart_buff_ex(port);
		win32_start_thread(win32_uart_recv_thread_ex, &nPort);

		return TRUE;
	}
	else 
	{
		Log_e(_T("open uart%d failed!\n"), HX_WIN32_UART_PORT);
		return FALSE;
	}
#endif
	return TRUE;
}


static Boolean add_uart_rx_to_queue()
{
	Boolean ret;
	
	ret = add_buf(&uart_rx_queue, &uart_rx_buff);
	init_buffer(&uart_rx_buff);	

	return ret;
}

/*
 * 返回值为FALSE, 表示需要进行下一步处理
 */
Boolean input_char(int input)
{
	Boolean ret;

	ret = add_char(&uart_rx_buff, input);
	
	if (!ret)
	{
		if (!add_uart_rx_to_queue())
		{
			Log_d(_T("uart rx queue overflow11\n"));
		}

		add_char(&uart_rx_buff, input);
	}
#if defined(__HX_IOT_SUPPORT__)
	if (uart_rx_buff.len >= hx_frame_length)
	{
		if (!add_uart_rx_to_queue())
		{
			Log_d(_T("uart rx queue overflow22\n"));
		}

		ret = FALSE;
	}
#endif

	return ret;
}

/*
 * 检查最后一个串口接收包，加到网络发送队列中
 */
Boolean add_rx_buffer()
{
	Boolean ret = FALSE;

	if (uart_busy)
	{
		return TRUE;
	}

	if (uart_rx_buff.len == 0)
	{
		Log_d(_T("uart rx buffer is empty!"));
		return FALSE;
	}

	if (pnet == NULL)
	{
		init_buffer(&uart_rx_buff);		// 未注册到网络链路上,丢掉数据
		return FALSE;
	}

	//Log_d(_T("add_rx_buffer\r\n");
	uart_busy = TRUE;

	ret = add_buf(&pnet->tx_queue, &uart_rx_buff);
//	if (ret)		// 添加不成功，直接丢弃
	{
		init_buffer(&uart_rx_buff);
	}
	uart_busy = FALSE;

	return ret;
}

/*
 * 将buf添加到uart发送队列中。
 */
Boolean add_uart_tx_buffer()
{
	Boolean ret = FALSE;

	if (uart_rx_buff.len == 0)
	{
		Log_d(_T("uart rx buffer is empty!"));
		return FALSE;
	}

	ret = add_buf(&uart_tx_queue, &uart_rx_buff);
	if (ret)
	{
		init_buffer(&uart_rx_buff);
	}

	return ret;
}


void uart_send_callback(Boolean bOk)
{
	buf_t *b;

	Log_d(_T("uart_send_callback @%d\n"), __LINE__);
	if (!bOk)
	{
		return;
	}

	b = remove_buf(&uart_tx_queue);
	if (!b)
	{
		//assert(FALSE);
		Log_d(_T("uart_send_callback b == NULL @%d\n"), __LINE__);
	}

	// 发送队列中还有数据，继续发送
	if (get_buf(&uart_tx_queue))
	{
		send_uart();
	}
}

Boolean uart_send_buf(buf_t *b, send_cb fn)
{
	char tmp[256];
	int n;

	n = (b->len >= sizeof(tmp))? (sizeof(tmp) - 1) : b->len;

	memset(tmp, 0, sizeof(tmp));
	memcpy(tmp, b->pbuff, n);

	b->count++;

	Log_d(_T("++++++++++++\r\n"));
	print_buffer(b);
	Log_d(_T("++++++++++++\r\nsend UART len = %d, count = %d\n"), b->len, b->count);
#ifdef WIN32
	win32_UART_PutBytes(HX_WIN32_UART_PORT, (unsigned char *)b->pbuff, b->len);
#endif
	if (fn)
	{
		fn(TRUE);
	}

	return TRUE;
}


void send_uart()
{
	buf_t *b;

	b = get_buf(&uart_tx_queue);
	if (!b)
	{
		Log_d(_T("null buf!\n"));
		return;
	}

#ifdef WIN32
	uart_send_buf(b, uart_send_callback);
#else
	print_buffer(b);
	SerialWrite((U8 *)(b->pbuff), b->len);
	uart_send_callback(TRUE);
#endif
}


UINT win32_uart_recv_thread(LPVOID pParam)
{
	int status;
	int len;
	fm_packet_t *pack;
	DWORD dwEvtMask = 0;
	DWORD dwErrorFlags;
	COMSTAT ComStat;
	BOOL bResult;
	unsigned char temp_buf[BUF_SIZE] = {0};

	init_buffer(&uart_rx_buff);

	SetCommMask(UARTHandle[HX_WIN32_UART_PORT], EV_RXCHAR | EV_TXEMPTY );//有哪些串口事件需要监视？

#if 0
	ZeroMemory(&com_ov, sizeof(com_ov));
	com_ov.hEvent = CreateEvent(
		NULL,   // default security attributes 
		TRUE,   // manual-reset event 
		FALSE,  // not signaled 
		NULL    // no name
		);

	assert(com_ov.hEvent);
#endif	

	for (;;)
	{
		bResult = WaitCommEvent(UARTHandle[HX_WIN32_UART_PORT], &dwEvtMask, &com_ov);// 等待串口通信事件的发生

		if (!bResult)
		{    
			// If WaitCommEvent() returns FALSE, process the last error to determin    
			// the reason..    
			switch (dwErrorFlags = GetLastError())    
			{    
				case ERROR_IO_PENDING:     
					{    
						// This is a normal return value if there are no bytes    
						// to read at the port.
						// Do nothing and continue 
						WaitForSingleObject(com_ov.hEvent, INFINITE);

						break;   
					}   
				case 87:   
					{   
						// Under Windows NT, this value is returned for some reason.    
						// I have not investigated why, but it is also a valid reply    
						// Also do nothing and continue.    
						break;   
					}   
				default:   
					{   
						// All other error codes indicate a serious error has    
						// occured.  Process this error.    
						break;   
					}   
			}   
		}   
		
		ResetEvent(com_ov.hEvent);

		if ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR)
		{ // 缓冲区中有数据到达
			COMSTAT ComStat ;
			DWORD dwLength;
			DWORD dwError = 0;
			int error;
			fm_packet_t *pack;

			ClearCommError(UARTHandle[HX_WIN32_UART_PORT], &dwErrorFlags, &ComStat);

			dwLength = ComStat.cbInQue ; //输入缓冲区有多少数据？
			if (dwLength > 0)
			{
				BOOL fReadStat ;
				DWORD dwBytesRead;

				fReadStat = ReadFile(UARTHandle[HX_WIN32_UART_PORT], &uart_rx_buff.pbuff[uart_rx_buff.len], BUF_SIZE - 1 - uart_rx_buff.len, &dwBytesRead, &com_ov);

				if (!fReadStat)
				{
					DWORD dwError = GetLastError();

					if (dwError == ERROR_IO_PENDING)
					{
						while (!GetOverlappedResult(UARTHandle[HX_WIN32_UART_PORT], &com_ov, &dwBytesRead, TRUE))
						{
							dwError = GetLastError();
							if (dwError == ERROR_IO_INCOMPLETE)
							{
								continue;
							}
						}
					}
				}

				if (dwBytesRead == 0)		// 异常情况处理
				{
					Log_e(_T("dwLength=%d, fReadStat=%d, uart read error=%d\n"), dwLength, fReadStat, dwError);
					continue;
				}

				uart_rx_buff.len += dwBytesRead;
				Log_d(_T("==========\r\nuart read len = %d\n"), uart_rx_buff.len);
				uart_rx_buff.pbuff[uart_rx_buff.len] = '\0';
				//print_buffer(&uart_rx_buff);
				Log_d(_T("==========\r\n"));

#ifdef __HEKR_CLOUD_SUPPORT__ 
				pack = get_packet(&uart_rx_buff, &error);
				pack = build_echo_packet(pack);
				if (pack)
				{
					win32_UART_PutBytes(HX_WIN32_UART_PORT, (unsigned char *)pack, pack->len);
				}
				else
				{
					Log_d(_T(("not packet!\r\n"));
				}
#elif defined(__OBD_TEST__)
				obd_log_uart_rx_data(&uart_rx_buff);
#elif defined(__HOMI_TEST__) || defined(__HXWL_TEST__)
				homi_do_with_uart_rsp(&uart_rx_buff);
#elif defined(__TUYA_UART_TEST__)
				tuya_parse_uart_data_bufffer(&uart_rx_buff);
#endif
				if (fn_uart_rsp_func)
				{
					fn_uart_rsp_func(&uart_rx_buff);
				}

				init_buffer(&uart_rx_buff);
			}
		}
		else if ((dwEvtMask & EV_TXEMPTY) == EV_TXEMPTY) 
		{
			Log_d(_T("uart data sent!\n"));
			SetEvent(hWriteEvent);
		}
	}
	
#if 0
	for (;;)
	{
		len = win32_UART_GetBytes(HX_WIN32_UART_PORT, &uart_rx_buff.pbuff[uart_rx_buff.len], BUF_SIZE - uart_rx_buff.len, &status);
		if (len > 0)
		{
			uart_rx_buff.len += len;

			Log_d(_T("uart read len = %d\n", len);

#if defined(__HX_IOT_SUPPORT__)					
			hx_do_with_uart_frame();

#elif defined(__HEKR_CLOUD_SUPPORT__)
			add_rx_buffer();
			pack = parse_data_buff();
			if (pack)
			{
				hekr_add_task_rsp_from_uart(pack);
			}
#else
			//{
			//	add_rx_buffer();
			//	net_send_tx_queue(app_res.tcpServerSock, FALSE, wifi2uart_tcp_send_cb);
			//}
#endif
		}
	}
#endif

	return 0;
}

/************************************************************************/
/* 
功能：接收串口数据线程
pParam:传入指定串口号，表明接收此串口的数据
返回值：成功：KAL_TRUE，失败:KAL_FALSE
*/
/************************************************************************/
UINT win32_uart_recv_thread_ex(LPVOID pParam)
{
	int status;
	int len;
	fm_packet_t *pack;
	DWORD dwEvtMask = 0;
	DWORD dwErrorFlags;
	COMSTAT ComStat;
	BOOL bResult;
	unsigned char temp_buf[BUF_SIZE] = {0};
	int port = *(int *)pParam;

	init_buffer(&g_uart_rx_buff[port]);

	SetCommMask(UARTHandle[port], EV_RXCHAR | EV_TXEMPTY );//有哪些串口事件需要监视？

	for (;;)
	{
		bResult = WaitCommEvent(UARTHandle[port], &dwEvtMask, &com_ov_ex[port]);// 等待串口通信事件的发生

		if (!bResult)
		{    
			// If WaitCommEvent() returns FALSE, process the last error to determin    
			// the reason..    
			switch (dwErrorFlags = GetLastError())    
			{    
			case ERROR_IO_PENDING:     
				{    
					// This is a normal return value if there are no bytes    
					// to read at the port.
					// Do nothing and continue 
					WaitForSingleObject(com_ov_ex[port].hEvent, INFINITE);

					break;   
				}   
			case 87:   
				{   
					// Under Windows NT, this value is returned for some reason.    
					// I have not investigated why, but it is also a valid reply    
					// Also do nothing and continue.    
					break;   
				}   
			default:   
				{   
					// All other error codes indicate a serious error has    
					// occured.  Process this error.    
					break;   
				}   
			}   
		}   

		ResetEvent(com_ov_ex[port].hEvent);

		if ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR)
		{ // 缓冲区中有数据到达
			COMSTAT ComStat ;
			DWORD dwLength;
			DWORD dwError;
			int error;
			fm_packet_t *pack;
			buf_t *ptr = &g_uart_rx_buff[port];

			ClearCommError(UARTHandle[port], &dwErrorFlags, &ComStat);

			dwLength = ComStat.cbInQue ; //输入缓冲区有多少数据？
			if (dwLength > 0)
			{
				BOOL fReadStat ;
				DWORD dwBytesRead;

				fReadStat = ReadFile(UARTHandle[port], temp_buf, sizeof(temp_buf), &dwBytesRead, &com_ov_ex[port]);
				if (!fReadStat)
				{
					DWORD dwError = GetLastError();

					if (dwError == ERROR_IO_PENDING)
					{
						while (!GetOverlappedResult(UARTHandle[port], &com_ov_ex[port], &dwBytesRead, TRUE))
						{
							dwError = GetLastError();
							if (dwError == ERROR_IO_INCOMPLETE)
							{
								continue;
							}
						}
					}
				}

				if (dwBytesRead < (BUF_SIZE - ptr->len))
				{
					memcpy(ptr->pbuff + ptr->len, temp_buf, dwBytesRead);
					ptr->len += dwBytesRead;
				}

				Log_d(_T("==========\r\nuart read len = %d\n"), ptr->len);
				ptr->pbuff[ptr->len] = '\0';
				print_buffer(ptr);
				Log_d(_T("==========\r\n"));

				//obd_log_uart_rx_data_ex(ptr, port);

				if (fn_uart_rsp_ex_func)
				{
					fn_uart_rsp_ex_func(ptr, port);
				}
			}
		}
		else if ((dwEvtMask & EV_TXEMPTY) == EV_TXEMPTY) 
		{
			Log_d(_T("uart data sent!\n"));
			SetEvent(hWriteEvent_ex[port]);
		}
	}

	return 0;
}


kal_uint16 win32_UART_GetBytes(int port, kal_uint8 *Buffaddr, kal_uint16 Length, kal_uint8 *status)
{
	int real_read = 0;
	int i = 0;
	
	if (status) 
	{
		*status = 0;
	}
	
	if (!ReadFile(UARTHandle[port],
			(LPVOID)Buffaddr,
			(DWORD)Length,
			(LPDWORD)&real_read,
			NULL))
	{
		Log_d(_T("read file error %d\n"), GetLastError());
		return 0;
	}

    if (real_read < Length)
    {
        breadytoread[port] = TRUE;
    }
    else
    {
        breadytoread[port] = FALSE;
    }
	
	return real_read;
}

void win32_UART_Close(int port)
{
    CloseHandle(UARTHandle[port]);
}

/*关闭指定串口*/
void win32_UART_Close_Ex(int port)
{
    //CloseHandle(UARTHandle[port]);
	m_bUartThreadExit = TRUE;
	//win32_stop_thread();
	if (UARTHandle[port])
	{
		CloseHandle(UARTHandle[port]);
	}
	//m_port = 0;
}

void win32_CheckReadyToRead(int port)
{
	COMSTAT	comstat;
	DWORD	errors;
	
	if (ClearCommError(UARTHandle[port], &errors, &comstat))
	{
		if (comstat.cbInQue)        
		{
			breadytoread[port] = FALSE;
		}
	}        
}

void win32_CheckReadyToWrite(int port)
{
	COMSTAT	comstat;
	DWORD	errors;
	
	if (ClearCommError(UARTHandle[port], &errors, &comstat))
	{
		if(comstat.fCtsHold == 0 && comstat.fXoffHold == 0 && comstat.fXoffSent == 0)
		{            
			breadytowrite[port] = FALSE;
		}
	}            
}

void win32_QueryUARTStatus()
{
	int i = 0;
	//ready to read    
	for (i = 0; i < MAX_PORT_NUM; i++)
	{
		if (breadytoread[i])
		{
			win32_CheckReadyToRead(i);
		}

		if (breadytowrite[i])
		{
			win32_CheckReadyToWrite(i);
		}
	}    
}

int w32uart_available(int port)
{
	DWORD dummy;
	
	if (!ClearCommError(UARTHandle[port], &dummy, &comstat[port]))
	{
		return 0;
	}
	
	return comstat[port].cbInQue;
}

kal_uint16 win32_UART_PutBytes(int port, kal_uint8 *Buffaddr, kal_uint16 Length)
{
    ///
#if 0
    int wbytes;
	BOOL fWriteStat;
	DWORD dwError;
	DWORD dwBytesWritten;
    DWORD dwRet ;
#endif
	OVERLAPPED osWrite = {0};
	DWORD dwWritten;
	DWORD dwRes;
	BOOL fRes;

   // Create this write operation's OVERLAPPED structure's hEvent.
   osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
   if (osWrite.hEvent == NULL)
      // error creating overlapped event handle
      return FALSE;

    // Issue write.
   if (!WriteFile(UARTHandle[port], Buffaddr, Length, &dwWritten, &osWrite)) 
   {
      if (GetLastError() != ERROR_IO_PENDING) 
	  { 
         // WriteFile failed, but isn't delayed. Report error and abort.
         fRes = FALSE;
      }
      else
      {
         // Write is pending.
         dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
         switch(dwRes)
         {
            // OVERLAPPED structure's event has been signaled. 
            case WAIT_OBJECT_0:
                 if (!GetOverlappedResult(UARTHandle[port], &osWrite, &dwWritten, FALSE))
                 {
                       fRes = FALSE;
                 }
                 else
                  // Write operation completed successfully.
                  fRes = TRUE;
                 break;
            
            default:
                 // An error has occurred in WaitForSingleObject.
                 // This usually indicates a problem with the
                // OVERLAPPED structure's event handle.
                 fRes = FALSE;
                 break;
         }
      }
   }
   else
   {
      // WriteFile completed immediately.
      fRes = TRUE;
   	}

   Log_d(_T("tolen = %d, write len = %d\r\n"), Length, dwWritten);

   CloseHandle(osWrite.hEvent);
   return fRes;
   
#if 0
    fWriteStat = WriteFile(UARTHandle[port], Buffaddr, Length, &wbytes, &com_ov);
	if (!fWriteStat) 
	{
		dwError = GetLastError();
		Log_d(_T("dwError = %d\r\n", dwError);
		if (dwError == ERROR_IO_PENDING)
		{
			while(!GetOverlappedResult(UARTHandle[port], &com_ov, &dwBytesWritten, TRUE )) 
			{
				dwError = GetLastError();

				if (dwError == ERROR_IO_INCOMPLETE)
				{
					wbytes += dwBytesWritten;
					continue; 
				}
			}
		}
	}

	dwRet = WaitForSingleObject(hWriteEvent, INFINITE);
	if (dwRet == WAIT_ABANDONED)
	{
		Log_d(_T("event abandon!\r\n");
	}
	else if (dwRet == WAIT_OBJECT_0)
	{
		Log_d(_T("get signal\r\n");
	}
	else
	{

	}

	ResetEvent(hWriteEvent);

    if (wbytes < Length)
    {
        breadytowrite[port] = TRUE;
    }
    else
    {
        breadytowrite[port] = FALSE;
    }

	PurgeComm(UARTHandle[port], PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);	   
        
    return wbytes;
#endif	
}

kal_bool win32_UART_Open(int port)
{
	DCB dcb;
	HANDLE hCom;
	TCHAR strbuf[256], destbuf[256];
	COMSTAT	comstat;
	DWORD	errors;
	COMMTIMEOUTS timeouts;
	int baudrate = CBR_38400;
	int flowCtrl = 0;  //0:none, 1:hw , 2:sw flow control
	TCHAR path[1024];
	TCHAR tempbuf[1024];
	TCHAR *name;	
	int len;

#if 1
	_stprintf(strbuf, _T("UART%d"), port);
	
	if (!get_config_string_value(strbuf, _TEXT("COM_PORT"), _TEXT("COM1"), tempbuf, sizeof(tempbuf)))
	{
		_tcscpy(tempbuf, _T("COM1"));
	}

	_stprintf(destbuf, _T("\\\\.\\%s"), tempbuf);

	if (!get_config_int_value(strbuf, _TEXT("flowcontrol"), &flowCtrl, 0))
	{
		port = 0;
	}

	if (!get_config_int_value(strbuf, _TEXT("baud_rate"), &baudrate, 115200))
	{
		baudrate = 115200;
	}

#else

	if (!SearchPath(NULL, "stubmfc.exe", NULL, sizeof(path), path, &name))
	{
		return 0;
	}
    
	memset(tempbuf, 0, sizeof(tempbuf));
	strncpy(tempbuf, path, strlen(path) - strlen(name));
	sprintf(path, "%suart.ini", tempbuf);

	sprintf(strbuf, "UART%d", port);
	len = sprintf(destbuf, ("\\\\.\\"));
	
	GetPrivateProfileString(strbuf, "COM_PORT", "COM1", &destbuf[len], sizeof(destbuf) - len, path);
	flowCtrl = GetPrivateProfileInt(strbuf, "flowcontrol", 0, path);
	baudrate = GetPrivateProfileInt(strbuf, "baud_rate", 115200 , path);
#endif
	
	hCom = CreateFile(destbuf,
                    GENERIC_READ |  GENERIC_WRITE ,
                    0,    // must be opened with exclusive-access
                    NULL, // no security attributes
                    OPEN_EXISTING, // must use OPEN_EXISTING
                    FILE_FLAG_OVERLAPPED,    // not overlapped I/O
                    NULL  // hTemplate must be NULL for comm devices
                    );

	if (hCom == INVALID_HANDLE_VALUE)// create fail
	{
		return KAL_FALSE;
	}
	
	//Turn off fAbortOnError first!	
	ClearCommError(hCom, &errors, &comstat);

#if 0
    if (!GetCommState(hCom, &dcb))
	{
		CloseHandle(hCom);
		hCom = NULL;
		return KAL_FALSE;
	}
	
	dcb.DCBlength = sizeof(DCB);
    dcb.Parity = NOPARITY;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.BaudRate = 921600;// baudrate;		///check!
    dcb.fBinary = TRUE;
    dcb.fParity = FALSE;

	if (flowCtrl == 1)  //hardware flow control
	{
		dcb.fOutxCtsFlow = TRUE;
	}
	else
	{
		dcb.fOutxCtsFlow = FALSE;
	}

    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fDsrSensitivity = FALSE;
    dcb.fTXContinueOnXoff = FALSE;
	dcb.fErrorChar = FALSE;
    dcb.fNull = FALSE;
	dcb.fAbortOnError = FALSE;

	if (flowCtrl == 2)  //software flow control
	{
		dcb.fOutX = TRUE;
		dcb.fInX = TRUE;
		dcb.XonChar = 0x11;
		dcb.XoffChar = 0x13;
	}
	else
	{
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;
		dcb.XonChar = 0;
		dcb.XoffChar = 0;
	}   
	
	if (flowCtrl == 2 || flowCtrl == 0) //sw or none flow control
	{
		dcb.fRtsControl = RTS_CONTROL_ENABLE;
	}
	else if (flowCtrl == 1)   //hw flow control
	{
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	}

    dcb.ErrorChar = 0;
    dcb.EofChar = 0;
    dcb.EvtChar = 0;	
#else
	//DCB dcb;

	FillMemory(&dcb, sizeof(dcb), 0);
	dcb.DCBlength = sizeof(dcb);
	_stprintf(tempbuf, _T("%d,N,8,1"), baudrate);
	if (!BuildCommDCB(tempbuf, &dcb)) 
	{   
	  // Couldn't build the DCB. Usually a problem
	  // with the communications specification string.
	  Log_d(_T("build dcb failed!\r\n"));
	  return FALSE;
	}

#endif

	if (!SetCommState(hCom, &dcb))
    {
        CloseHandle(hCom);		
		return KAL_FALSE;
    }	

	if (SetupComm(hCom, 8192, 8192)==FALSE )
	{
		CloseHandle(hCom);		
		return KAL_FALSE;
	}

	PurgeComm(hCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);	   
				
#if 1
	timeouts.ReadIntervalTimeout = 100;
	timeouts.ReadTotalTimeoutConstant = 100;
	timeouts.ReadTotalTimeoutMultiplier = 400; 

	timeouts.WriteTotalTimeoutMultiplier = 3;
	timeouts.WriteTotalTimeoutConstant = 2;
	
	if (SetCommTimeouts(hCom, &timeouts) == FALSE ) 
	{
		CloseHandle(hCom);		
		return KAL_FALSE;
	}
#endif

	
	UARTHandle[port]  = hCom;
	breadytoread[port] = TRUE;

	ZeroMemory(&com_ov, sizeof(com_ov));
	com_ov.hEvent = CreateEvent(
		NULL,   // default security attributes 
		TRUE,   // manual-reset event 
		FALSE,  // not signaled 
		NULL    // no name
	);

	assert(com_ov.hEvent);

	hWriteEvent = CreateEvent(
		NULL,   // default security attributes 
		TRUE,   // manual-reset event 
		FALSE,  // not signaled 
		NULL    // no name
	);
	assert(hWriteEvent);
	
	return KAL_TRUE;	
}


/* 
 * 功能：打开指定串口
 * port:串口号
 * 返回值：成功：KAL_TRUE，失败:KAL_FALSE
 * 波特率在本段中确定
 * [UART2]
 *		COM_PORT = COM3
 *		flowcontrol = 0
 *		baud_rate = 115200
 */
kal_bool win32_UART_Open_ex(int port)
{
	DCB dcb;
	HANDLE hCom;
	TCHAR strbuf[256], destbuf[256];
	COMSTAT	comstat;
	DWORD	errors;
	COMMTIMEOUTS timeouts;
	int baudrate = CBR_38400;
	int flowCtrl = 0;  //0:none, 1:hw , 2:sw flow control
	TCHAR path[1024];
	TCHAR tempbuf[1024];
	TCHAR *name;	
	int len;

	memset(destbuf, 0, sizeof(destbuf));
	_stprintf(destbuf, _T("\\\\.\\COM%d"), port);
	flowCtrl = 0;
	baudrate = CBR_9600;

	_stprintf(strbuf, _T("UART%d"), 2);
	if (!get_config_string_value(strbuf, _TEXT("COM_PORT"), _TEXT("COM1"), tempbuf, sizeof(tempbuf)))
	{
		_tcscpy(tempbuf, _T("COM1"));
	}

	if (!get_config_int_value(strbuf, _TEXT("baud_rate"), &baudrate, CBR_9600))
	{
		baudrate = CBR_9600;
	}

	hCom = CreateFile(destbuf,
		GENERIC_READ /*| GENERIC_WRITE*/,
		0,    // must be opened with exclusive-access
		NULL, // no security attributes
		OPEN_EXISTING, // must use OPEN_EXISTING
		FILE_FLAG_OVERLAPPED,    // not overlapped I/O
		NULL  // hTemplate must be NULL for comm devices
		);

	if (hCom == INVALID_HANDLE_VALUE)// create fail
	{
		return KAL_FALSE;
	}

	//Turn off fAbortOnError first!	
	ClearCommError(hCom, &errors, &comstat);

	//DCB dcb;

	FillMemory(&dcb, sizeof(dcb), 0);
	dcb.DCBlength = sizeof(dcb);
	_stprintf(tempbuf, _T("%d,N,8,1"), baudrate);
	if (!BuildCommDCB(tempbuf, &dcb)) 
	{   
		// Couldn't build the DCB. Usually a problem
		// with the communications specification string.
		Log_d(_T("build dcb failed!\r\n"));
		return FALSE;
	}

	if (!SetCommState(hCom, &dcb))
	{
		CloseHandle(hCom);		
		return KAL_FALSE;
	}	

	if (SetupComm(hCom, 8192, 8192)==FALSE )
	{
		CloseHandle(hCom);		
		return KAL_FALSE;
	}

	PurgeComm(hCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);	   

#if 1
	timeouts.ReadIntervalTimeout = 100;
	timeouts.ReadTotalTimeoutConstant = 100;
	timeouts.ReadTotalTimeoutMultiplier = 400; 

	timeouts.WriteTotalTimeoutMultiplier = 3;
	timeouts.WriteTotalTimeoutConstant = 2;

	if (SetCommTimeouts(hCom, &timeouts) == FALSE ) 
	{
		CloseHandle(hCom);		
		return KAL_FALSE;
	}
#endif


	UARTHandle[port]  = hCom;
	breadytoread[port] = TRUE;

	ZeroMemory(&com_ov_ex[port], sizeof(com_ov_ex[port]));
	com_ov_ex[port].hEvent = CreateEvent(
		NULL,   // default security attributes 
		TRUE,   // manual-reset event 
		FALSE,  // not signaled 
		NULL    // no name
		);

	assert(com_ov_ex[port].hEvent);

	hWriteEvent_ex[port] = CreateEvent(
		NULL,   // default security attributes 
		TRUE,   // manual-reset event 
		FALSE,  // not signaled 
		NULL    // no name
		);
	assert(hWriteEvent_ex[port]);

	return KAL_TRUE;	
}

/*
 * 清除串口
 */
kal_bool win32_clear_uart_data(int port)
{
	HANDLE h;

	if (port < 0 || port >= MAX_PORT_NUM)
	{
		return FALSE;
	}

	h = UARTHandle[port];
	
	if (!PurgeComm(h, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
	{
		Log_d(_T("Purge comm error!"));
	}
}


static U8 local_mac[6] = {0X00, 0x12, 0x23, 0x34, 0x45, 0x56};
U8 get_local_mac(U8 *mac, U8 len)
{
	U8 status = 0;
	
    if (len > 6) 
	{
        Log_d(_T("over local mac len: (%d), please check\n"), len);
        status = -1;
    } else if (len < 6) 
   {
        Log_d(_T("less local mac len: (%d), please check\n"), len);
        status = -1;
    } else {
        memcpy(mac, local_mac, 6);
    }
	
    return status;
}

/*
 * 注册串口数据回调函数
 */
void register_uart_rsp_func(uart_rsp_cb_t fn)
{
	fn_uart_rsp_func = fn;
}

/*
 * 注册串口数据回调函数
 */
void register_uart_rsp_ex_func(uart_rsp_ex_cb_t fn)
{
	fn_uart_rsp_ex_func = fn;
}



void win32_start_thread(AFX_THREADPROC proc, LPVOID param)
{
	/*m_pUartRecvThread = */::AfxBeginThread(proc, param); //接下来做啥就直接调用pThead就行.
}