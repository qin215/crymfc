#include "stdafx.h"
#include <stddef.h>
#include <stdio.h>
#include <windows.h>
#include "CRY574ProAPIWrapper.h"
#include "CRY574ProMFCDemo.h"
#include "CRY574ProMFCDemoDlg.h"
#include "afxdialogex.h"
#include "protocol.h"
#include "mywin.h"
#include "uart_cmd.h"

int process_data_buffer(kal_uint8 *pbuff, int len);
void dlg_send_uart_msg_to_ui(int cmd);

/*
 * 串口回应处理
 */
void ua800_do_with_uart_rsp(buf_t *b)
{
	char *p = NULL;

//	print_buffer(b);

	b->pbuff[b->len] = '\0';

	//Log_d("recv str:%s", b->pbuff);
//	sz_print_pkt("recv bindata:", (kal_uint8 *)b->pbuff, b->len);
	b->len = process_data_buffer((kal_uint8 *)b->pbuff, b->len);
}

// 返回剩余的字节数
int process_data_buffer(kal_uint8 *pbuff, int len)
{
	int nlen = len;
	kal_uint8 *tmp;
	kal_uint8 *p;
	int count = 0;
	int i;

	for (i = 0; i < len; i++)
	{
		if (pbuff[i] >= 'a' && pbuff[i] <= 'z')
		{
			pbuff[i] = pbuff[i] - ('a' - 'A');			// to upper case
		}
	}

	if (strstr((const char *)pbuff, "READY"))
	{
		// get the ready string from uart.
		Log_d(_T("get ready cmd from uart."));
		dlg_send_uart_msg_to_ui(UART_USER_CMD_START_TESTING);
	}


	return 0;
}



#define T5506_RACECMD_SEND_COUNT 3


/*
 * 构造race cmd数据包
 */
static buf_t * t5506_build_race_cmd(buf_t *b, kal_uint16 race_cmd)
{
	kal_uint8 *pbuf = NULL;

	char *open_cmd = "open\r\n";

	if (race_cmd == OPEN_BOX)
	{
		b->len = strlen(open_cmd);
		pbuf = (kal_uint8 *)open_cmd;
	}
	else
	{
		ASSERT(FALSE);
	}

	memcpy(b->pbuff, pbuf, b->len);

	return b;
}

/*
 * 发送 RACE CMD 给耳机
 */
Boolean t5506_send_uart_cmd(kal_uint16 racecmd)
{
	buf_t *b;
	int i;
	DWORD dwWait;
	BOOL ret = FALSE;


	b = alloc_buffer();
	if (!b)
	{
		return FALSE;
	}

	for (i = 0; i < T5506_RACECMD_SEND_COUNT; i++)
	{
		t5506_build_race_cmd(b, racecmd);
		uart_send_buf(b, NULL);
	}

	free_buffer(b);

	return ret;
}

/*
 * 发送串口指令到UI线程
 */
void dlg_send_uart_msg_to_ui(int cmd)
{
	CWnd * pDlg = AfxGetMainWnd();

	if (pDlg)
	{
		::PostMessage(pDlg->m_hWnd, WM_UART_USER_CMD, (WPARAM)cmd, (LPARAM)0);
	}
}
