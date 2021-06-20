#include "stdafx.h"
#include <stddef.h>
#include <stdio.h>
#include <windows.h>
#include "CRY574ProAPIWrapper.h"
#include "CRY574ProMFCDemo.h"
#include "CRY574ProMFCDemoDlg.h"
#include "afxdialogex.h"
#include "protocol.h"

#pragma pack(push)
#pragma pack(1)
typedef struct onewire_struct 
{
	uint8_t header;
	uint8_t type;
	uint16_t len;
	uint16_t cmd;
	uint8_t event;
	uint8_t side;
	uint8_t param[1];
}  onewire_frame_t;
#pragma pack(pop)

BOOL bStopped = TRUE;
BOOL bRunning = FALSE;

CWinThread *pWorkThread;




int Char2Int(TCHAR c)
{
	switch(c)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return c-'0';
		break;

	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		return c-'a'+10;
		break;
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		return c-'A'+10;
		break;
	default:
		return 0xFFFFFFFF;

	}

}

BYTE Hex2Char(BYTE data)
{
	BYTE hex= data&0x0f;

	if(hex>=0x00 && hex<=0x09)
		hex += '0';
	else if(hex>=0x0a && hex<=0x0f)
		hex += ('A'-0x0a);

	return hex;
}

BYTE Char2Hex(BYTE hex) 
{
	if (hex>='a' && hex<='f')
	{
		hex -= 0x20;
	}

	if ((hex>='0' && hex<='9') || (hex>='A' && hex<='F'))
	{
		if(hex>='0' && hex<='9')
			hex -= '0';
		if(hex>='A' && hex<='F')
			hex -= ('A' - 0x0A);
		return hex;
	}

	return 0;
}

int String2HexData(const CString &in_str, UCHAR * outBuffer)
{
	int iLen, i;
	int count;
	TCHAR * p;

	CString str = in_str;

	iLen = str.GetLength();

	str.Replace(_T(" "), _T(""));

	p = str.GetBuffer(iLen);
	for(i = 0, count = 0; i < iLen / 2; i++, count++)
	{
		int temp;

		*outBuffer = 0;
		temp = Char2Int(*p);
		if( temp == 0xFFFFFFFF )
			return count;
		*outBuffer = temp;
		p++;
		*outBuffer <<= 4;
		temp = Char2Int(*p);
		if( temp == 0xFFFFFFFF )
			return count;
		*outBuffer |= temp;
		p++;
		*outBuffer++;
	}

	return count;
}

/*
 * 二进制数据转为hex数据
 */
int Binary2HexData(const UCHAR * inBuff, const int len, UCHAR *outBuff, int out_len)
{
	int i;
	int index;

	if (out_len < len * 2)
	{
		return -1;
	}

	for (i = 0, index = 0; i < len; i++)
	{
		index += sprintf((char *)&outBuff[index], "%02X", inBuff[i]);
	}

	return index;
}


UINT32 parse_race_cmd_rsp(const uint8_t *pdata, int data_len)
{
	onewire_frame_t *pFrame = (onewire_frame_t *)pdata;
	BOOL valid = FALSE;
	// 05 5A 05 00 06 0E 00 0B 03

	uint16_t len = pFrame->len;
	len += 4;			// added length & header & type
	if (len <= data_len)
	{
		valid = TRUE;
	}

	if (!valid)
	{
		printf("qin spp format error!");
		return 0;
	}

	return pFrame->param[0];
}

CString begin_inquiry_bt_device()
{
	int nInqCount = 0;
	CString strInfo;
	int maxRssi = -1000;
	CString btDevice;
		
	strInfo.Format(_T("正在搜寻蓝牙设备"));
	dlg_update_ui(strInfo);

	CRYBT_InquiryAllOneTime(5, FALSE, nInqCount);

	strInfo.Format(_T("%d BT devices has been inquiried"), nInqCount);
	dlg_update_ui(strInfo);

	for (int i = 0 ;i < nInqCount; i++)
	{
		char* pcMac = new char[256];
		int nRssi = 0;
		CRYBT_GetInquiryMac(i, pcMac);
		CRYBT_GetInquiryRssi(i, nRssi);

		CString strMac(pcMac);

		strInfo.Format(_T("MAC:%s RSSI:%d"), strMac, nRssi);
		dlg_update_ui(strInfo);

		if (nRssi > maxRssi)
		{
			btDevice = strMac;
			maxRssi = nRssi;
		}

		delete pcMac;
		pcMac = NULL;
	}

	strInfo.Format(_T("MAX RSSI:%d, MAC:%s "), maxRssi, btDevice);
	dlg_update_ui(strInfo);

	return btDevice;
}


BOOL connect_bt_device(const CString& device)
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL ret = FALSE;
	char* pcMac = new char[64];
	int len = device.GetLength();
	CString info;
	info.Format(_T("正在连接蓝牙:%s"), device);

	for (int i = 0;i<len;i++)
	{
		pcMac[i] = device[i];
	}
	pcMac[len] = '\0';

	int retCode = CRYBT_MacConnect(pcMac);
	if (retCode == API_OK)
	{
		info.Format(_T("蓝牙(%s)已连接"), device);
		ret = TRUE;
	} 
	else
	{
		info.Format(_T("连接蓝牙(%s)错误： %d"),device, retCode);
	}

	dlg_update_ui(info);

	delete [] pcMac;
	pcMac = NULL;

	return ret;
}


BOOL check_bt_name()
{
	int retCode;
	char* pcName = new char[256];
	CString strinfo;
	BOOL ret = FALSE;

	retCode = CRYBT_QuiryName(pcName);
	if (retCode == API_OK)
	{
		CString strName(pcName);
		strinfo.Format(_T("蓝牙名: %s"),strName);

		if (strName == (_T("Philips TAT5506")))
		{
			ret = TRUE;
		}
	} 
	else
	{
		strinfo.Format(_T("error code is %d"),retCode);
	}

	dlg_update_ui(strinfo);
	delete pcName;
	pcName = NULL;

	return ret;
}



BOOL connect_bt_spp(const CString& device)
{
	char* pcMac = new char[64];
	int len = device.GetLength();
	CString info;
	BOOL ret = FALSE;

	for (int i = 0;i<len;i++)
	{
		pcMac[i] = device[i];
	}
	pcMac[len] = '\0';

	int retCode = CRYBT_ConnectSPP(pcMac);
	if (retCode == API_OK)
	{
		info.Format(_T("Connected SPP"));
		ret = TRUE;
	} 
	else
	{
		info.Format(_T("error code is %d"),retCode);
	}

	dlg_update_ui(info);

	delete []pcMac;
	pcMac = NULL;

	return ret;
}

BOOL check_psensor_calibrated()
{
	const char data[] = "05 5A 05 00 06 0E 00 0B 17";
	BOOL ret = FALSE;

	char* pcRecv = new char[4096];
	CRYBT_SPPCommand(data, pcRecv, 1000, TRUE);
	CString strSPPRecv(pcRecv);

	BOOL ok = parse_spp_rsp_data(strSPPRecv);
	if (ok)
	{
		//AfxMessageBox(_T("光感已校准"));
		ret = TRUE;
		dlg_update_status_ui(STATE_CALI_STATUS, _T("已校准"));
	}
	else
	{
		//AfxMessageBox(_T("光感未校准"));
		dlg_update_status_ui(STATE_FAIL);
		dlg_update_status_ui(STATE_CALI_STATUS, _T("未校准"));
	}

	CString strInfo;
	strInfo.Format(_T("SPP Recv:%s"),strSPPRecv);
	dlg_update_ui(strInfo);

	delete pcRecv;
	pcRecv = NULL;

	return ret;
}

void check_psensor_cali_value()
{
	const char near_hi_data[] = "05 5A 05 00 06 0E 00 0B 1B";
	const char near_low_data[] = "05 5A 05 00 06 0E 00 0B 1C";

	const char far_hi_data[] = "05 5A 05 00 06 0E 00 0B 1D";
	const char far_low_data[] = "05 5A 05 00 06 0E 00 0B 1E";

	char* pcRecv = new char[4096];

	// 入耳数据
	CRYBT_SPPCommand(near_hi_data, pcRecv, 1000, TRUE);
	CString strSPPRecv(pcRecv);
	CString strInfo;

	UINT32 near_data = parse_spp_rsp_data(strSPPRecv);
	strInfo.Format(_T("SPP NEAR HIGH8 Recv:%s"),strSPPRecv);
	dlg_update_ui(strInfo);
	near_data <<= 8;

	CRYBT_SPPCommand(near_low_data, pcRecv, 1000, TRUE);
	strSPPRecv = CString(pcRecv);

	near_data |= parse_spp_rsp_data(strSPPRecv);
	strInfo.Format(_T("SPP NEAR LOW8 Recv:%s"),strSPPRecv);
	dlg_update_ui(strInfo);
	// 入耳数据 结束

	// 出耳数据
	UINT32 far_data = CRYBT_SPPCommand(far_hi_data, pcRecv, 1000, TRUE);
	strSPPRecv = CString(pcRecv);

	far_data = parse_spp_rsp_data(strSPPRecv);
	strInfo.Format(_T("SPP far high8 Recv:%s"),strSPPRecv);
	dlg_update_ui(strInfo);

	far_data <<= 8;

	CRYBT_SPPCommand(far_low_data, pcRecv, 1000, TRUE);
	strSPPRecv = CString(pcRecv);
	far_data |= parse_spp_rsp_data(strSPPRecv);
	strInfo.Format(_T("SPP far low8 Recv:%s"),strSPPRecv);
	dlg_update_ui(strInfo);
	

	CString prompt;
	
	if ((near_data < far_data) || (near_data - far_data <= 0x100))
	{
		prompt.Format(_T("入耳校准值:0X%04X, 出耳校准值：0X%04X, 校准失败！"), near_data, far_data);
		dlg_update_status_ui(STATE_FAIL);
	}
	else
	{
		prompt.Format(_T("入耳校准值:0X%04X, 出耳校准值：0X%04X, 校准成功！"), near_data, far_data);
		dlg_update_status_ui(STATE_CALI_VALUE, prompt);
		dlg_update_status_ui(STATE_SUCCESS);
	}

	//AfxMessageBox(prompt);

	delete pcRecv;
	pcRecv = NULL;
}


UINT32 parse_spp_rsp_data(CString& strRSP)
{
	int nlen = strRSP.GetLength();
	UCHAR *pbuff = new UCHAR[nlen];
	UINT32 ret;

	if (!pbuff)
	{
		bStopped = TRUE;
		AfxMessageBox(_T("没有内存"));
		PostQuitMessage(0);
	}

	int binlen = String2HexData(strRSP, pbuff);

	if (binlen <= 0)
	{
		bStopped = TRUE;
		AfxMessageBox(_T("返回值错误"));
		PostQuitMessage(0);
	}

	ret = parse_race_cmd_rsp(pbuff, binlen);

	delete pbuff;

	// 删除多余的0
	strRSP = strRSP.Left(3 * 15);

	return ret;
}




// 线程运行程序
UINT thread_process(LPVOID)
{
	CString btdevice;
	CString info;
	int retcode;
	int ret = -1;
	int i;


	dlg_update_status_ui(STATE_PROCESS);
	bRunning = TRUE;
	if (CRYBT_InitializePro() == TRUE)
	{
		dlg_update_ui(_T("初始化完成"));
	}
	else
	{
		dlg_update_ui(_T("初始化失败"));
		bRunning = FALSE;
		dlg_update_status_ui(STATE_ERROR);
		dlg_update_status_ui(STATE_DONE);
		return ret;
	}

	CRYBT_ResetDongle();

	for (i = 0; i < 10; i++)
	{
		btdevice = begin_inquiry_bt_device();
		if (btdevice.IsEmpty())
		{
			continue;
		}

		if (!connect_bt_device(btdevice))
		{
			continue;
		}

		if (check_bt_name())
		{
			break;
		}
		else
		{
			CRYBT_DisConnectSPP();
		}

		if (bStopped)
		{
			i = 10;
			dlg_update_status_ui(STATE_ABORT);
			break;
		}
	}

	if (i == 10)
	{
		goto disconn_bt;
	}

	// TODO: 在此添加控件通知处理程序代码
	CRYBT_SetDefaultProfile(1);
	dlg_update_ui(_T("配置SPP成功"));

	if (bStopped || !connect_bt_spp(btdevice))
	{
		dlg_update_status_ui(STATE_ABORT);
		goto disconn_bt;
	}

	if (!bStopped && check_psensor_calibrated())
	{
		check_psensor_cali_value();
	}

	ret = 0;

disconn_spp:
	if ((retcode = CRYBT_DisConnectSPP()) == API_OK)
	{
		info.Format(_T("Disconnect SPP"));
	} 
	else
	{
		info.Format(_T("error code is %d"),retcode);
	}

	dlg_update_ui(info);

disconn_bt:
	if ((retcode = CRYBT_Disconnect()) == API_OK)
	{
		info.Format(_T("Disconnected"));
	} 
	else
	{
		info.Format(_T("error code is %d"),retcode);
	}
	dlg_update_ui(info);

close_bt:
	CRYBT_Release();
	
	dlg_update_ui(_T("done!"));
	dlg_update_status_ui(STATE_DONE);

	bRunning = FALSE;
	return ret;
}


//
// 蓝牙光感查询校准程序
// 1. 初始化
// 2. inquiry
// 3. pairing
// 4. spp enable
// 5. spp connect
// 6. psensor cali flag check
// 7. psensor thershold value check
// 8. spp disconnect
// 9. bt close.
INT32 psensor_check_process()
{
	while (bRunning)
	{
		Sleep(500);
	}

	bStopped = FALSE;

	pWorkThread = AfxBeginThread((AFX_THREADPROC)thread_process, 0);

	return -1;
}


/*
 * 更新主线程中的UI显示
 */
void dlg_update_ui(const CString& promptinfo)
{
	CString *pInfoText = new CString(promptinfo);

	CCRY574ProMFCDemoDlg * pDlg = (CCRY574ProMFCDemoDlg *)AfxGetMainWnd();
	if (pDlg)
	{
		::PostMessage(pDlg->m_hWnd, WM_UPDATE_STATIC, (WPARAM)pInfoText, TRUE);
	}
	else
	{
		delete pInfoText;
	}
}

/*
 * 更新主线程中的UI显示
 */
void dlg_update_status_ui(INT state, const CString& info)
{
	CCRY574ProMFCDemoDlg * pDlg = (CCRY574ProMFCDemoDlg *)AfxGetMainWnd();
	if (pDlg)
	{
		CString *pInfoText = new CString(info);
		if (pInfoText == NULL)
		{
			ASSERT(FALSE);
		}

		::PostMessage(pDlg->m_hWnd, WM_UPDATE_STATUS, (WPARAM)state, (LPARAM)pInfoText);
	}
}