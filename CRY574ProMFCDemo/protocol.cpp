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



BOOL bStopped = TRUE;
BOOL bRunning = FALSE;

CWinThread *pWorkThread;
CString current_bt_device;
CString current_bt_name(_T("Philips TAT5506"));

tws_mode_t get_agent_mode();
tws_mode_t get_partner_mode();

UINT32 parse_race_cmd_rsp(race_cmd_t *pdata, int data_len, int *pside)
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
		Log_e(_T("qin spp format error!"));
		return SPP_RSP_ERROR;
	}

	*pside = pFrame->side;

	return pFrame->param[0];
}

CString begin_inquiry_bt_device()
{
	int nInqCount = 0;
	CString strInfo;
	int maxRssi = -1000;
	CString btDevice;
	int retcode;
		
	strInfo.Format(_T("正在搜寻蓝牙设备"));
	dlg_update_ui(strInfo);

	retcode = CRYBT_InquiryAllOneTime(5, FALSE, nInqCount);
	Log_d(_T("CRYBT_InquiryAllOneTime retcode=%d"), retcode);
	strInfo.Format(_T("%d BT devices has been inquiried"), nInqCount);
	dlg_update_ui(strInfo);

	for (int i = 0 ;i < nInqCount; i++)
	{
		char* pcMac = DBG_NEW char[256];
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
	Log_d(_T("found bt device(%s) rssi=%d"), btDevice, maxRssi);
	dlg_update_ui(strInfo);
	

	return btDevice;
}


BOOL connect_bt_device(const CString& device)
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL ret = FALSE;
	char* pcMac = DBG_NEW char[64];
	int len = device.GetLength();
	CString info;
	info.Format(_T("正在连接蓝牙:%s"), device);

	for (int i = 0;i<len;i++)
	{
		pcMac[i] = device[i] & 0xff;
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
	Log_d(_T("CRYBT_MacConnect retcode=%d"), retCode);
	dlg_update_ui(info);

	delete [] pcMac;
	pcMac = NULL;

	return ret;
}


BOOL check_bt_name()
{
	int retCode;
	char* pcName = DBG_NEW char[256];
	CString strinfo;
	BOOL ret = FALSE;

	retCode = CRYBT_QuiryName(pcName);
	if (retCode == API_OK)
	{
		CString strName(pcName);
		strinfo.Format(_T("蓝牙名: %s"),strName);

		if (strName == current_bt_name)
		{
			ret = TRUE;
		}
	} 
	else
	{
		strinfo.Format(_T("error code is %d"),retCode);
	}

	Log_d(_T("CRYBT_QuiryName retcode=%d"), retCode);

	dlg_update_ui(strinfo);
	delete pcName;
	pcName = NULL;

	return ret;
}

BOOL connect_bt_spp(const CString& device)
{
	char* pcMac = DBG_NEW char[64];
	int len = device.GetLength();
	CString info;
	BOOL ret = FALSE;

	for (int i = 0;i<len;i++)
	{
		pcMac[i] = device[i] & 0xff;
	}
	pcMac[len] = '\0';

	Sleep(1000);

	int retCode = CRYBT_ConnectSPP(pcMac);
	if (retCode == API_OK)
	{
		info.Format(_T("Connected SPP"));
		ret = TRUE;
	} 
	else
	{
		info.Format(_T("error code is %d"),retCode);
		Log_d(_T("connect spp retcode=%d"), retCode);
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
	INT retcode;
	int side = 0;

	char* pcRecv = DBG_NEW char[4096];
	retcode = CRYBT_SPPCommand(data, pcRecv, 1000, TRUE);
	Log_d(_T("send spp cmd retcode=%d"), retcode);
	CString strSPPRecv(pcRecv);

	BOOL ok = parse_spp_rsp_data(strSPPRecv, &side);
	if (ok)
	{
		//AfxMessageBox(_T("光感已校准"));
		ret = TRUE;
		dlg_update_status_ui(STATE_CALI_STATUS, _T("已校准"));
		Log_d(_T("device %s is calibrated"), current_bt_device);
	}
	else
	{
		dlg_update_status_ui(STATE_FAIL);
		dlg_update_status_ui(STATE_CALI_STATUS, _T("未校准"));
		Log_d(_T("device %s is NOT calibrated"), current_bt_device);
	//	AfxMessageBox(_T("光感未校准"));
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

	char* pcRecv = DBG_NEW char[4096];
	INT retcode;
	int side = 0;


	// 入耳数据
	retcode = CRYBT_SPPCommand(near_hi_data, pcRecv, 1000, TRUE);
	Log_d(_T("send spp cmd retcode=%d"), retcode);
	CString strSPPRecv(pcRecv);
	CString strInfo;

	UINT32 near_data = parse_spp_rsp_data(strSPPRecv, &side);
	strInfo.Format(_T("SPP NEAR HIGH8 Recv:%s"),strSPPRecv);
	dlg_update_ui(strInfo);
	near_data <<= 8;

	retcode = CRYBT_SPPCommand(near_low_data, pcRecv, 1000, TRUE);
	Log_d(_T("send spp cmd retcode=%d"), retcode);
	strSPPRecv = CString(pcRecv);

	near_data |= parse_spp_rsp_data(strSPPRecv, &side);
	strInfo.Format(_T("SPP NEAR LOW8 Recv:%s"),strSPPRecv);
	dlg_update_ui(strInfo);
	// 入耳数据 结束

	// 出耳数据
	UINT32 far_data;
	retcode = CRYBT_SPPCommand(far_hi_data, pcRecv, 1000, TRUE);
	Log_d(_T("send spp cmd retcode=%d"), retcode);
	strSPPRecv = CString(pcRecv);

	far_data = parse_spp_rsp_data(strSPPRecv, &side);
	strInfo.Format(_T("SPP far high8 Recv:%s"),strSPPRecv);
	dlg_update_ui(strInfo);

	far_data <<= 8;

	retcode = CRYBT_SPPCommand(far_low_data, pcRecv, 1000, TRUE);
	Log_d(_T("send spp cmd retcode=%d"), retcode);
	strSPPRecv = CString(pcRecv);
	far_data |= parse_spp_rsp_data(strSPPRecv, &side);
	strInfo.Format(_T("SPP far low8 Recv:%s"),strSPPRecv);
	dlg_update_ui(strInfo);
	

	CString prompt;
	
	if ((near_data < far_data) || (near_data - far_data <= 0x100))
	{
		prompt.Format(_T("入耳校准值:0X%04X, 出耳校准值：0X%04X, 校准失败！"), near_data, far_data);
		dlg_update_status_ui(STATE_CALI_VALUE, prompt);
		dlg_update_status_ui(STATE_FAIL);
		Log_e(_T("bda(%s) in ear value=0x%04x, out ear value=0x%04x, FAILED."), current_bt_device, near_data, far_data);
		//AfxMessageBox(prompt);
	}
	else
	{
		prompt.Format(_T("入耳校准值:0X%04X, 出耳校准值：0X%04X, 校准成功！"), near_data, far_data);
		Log_d(_T("bda(%s) in ear value=0x%04x, out ear value=0x%04x, SUCCESS."), current_bt_device, near_data, far_data);
		dlg_update_status_ui(STATE_CALI_VALUE, prompt);
		dlg_update_status_ui(STATE_SUCCESS);
	}

	//
	delete pcRecv;
	pcRecv = NULL;
}


UINT32 parse_spp_rsp_data(CString& strRSP, int *pside)
{
	int nlen = strRSP.GetLength();
	UCHAR *pbuff;
	UINT32 ret;

	if (nlen == 0)
	{
		Log_e(_T("qin nlen == 0!"));
		return SPP_RSP_ERROR;
	}

	pbuff = DBG_NEW UCHAR[nlen];
	if (!pbuff)
	{
		bStopped = TRUE;
		Log_e(_T("qin no memory error!"));
		PostQuitMessage(0);
	}

	int binlen = String2HexData(strRSP, pbuff);
	if (binlen <= 0)
	{
		Log_e(_T("origin spp str=%s"), strRSP);
		goto done;
	}

	ret = parse_race_cmd_rsp((race_cmd_t *)pbuff, binlen, pside);

done:
	delete pbuff;

	// 删除多余的0
	strRSP = strRSP.Left(3 * 15);
	Log_d(_T("recv spp data='%s'"), strRSP);

	return ret;
}


BOOL parse_spp_rsp_data_2(CString& strRSP, psensor_cali_data_t *left_earphone, 
							psensor_cali_data_t *right_earphone)
{
	int nlen = strRSP.GetLength();
	UCHAR *pbuff;
	BOOL ret = FALSE;

	Log_d(_T("qin spp orgin rsp str ='%s'"), strRSP);
	if (nlen == 0)
	{
		return FALSE;
	}

	pbuff = DBG_NEW UCHAR[nlen];
	if (!pbuff)
	{
		bStopped = TRUE;
		Log_e(_T("qin no memory!"));
		AfxMessageBox(_T("没有内存"));
		PostQuitMessage(0);			// 退出
	}

	int binlen = String2HexData(strRSP, pbuff);
	if (binlen <= 0)
	{
		bStopped = TRUE;
		Log_e(_T("qin data format error!raw data=%s"), strRSP);
		//AfxMessageBox(_T("返回值错误"));
		//delete pbuff;
		//PostQuitMessage(0);

		goto done;				// 返回值错误，进行重发
	}

	onewire_frame_t *pFrame = onewire_get_one_rsp_frame(RACE_CMD_FRAME_START, RACE_CMD_RSP, pbuff, &binlen);
	if (pFrame)
	{
		BOOL valid = FALSE;
		// 05 5A 05 00 00 20 00 0B 03

		uint16_t len = pFrame->len;
		len += 4;			// added length & header & type

#if 0
		if (len <= binlen)
		{
			valid = TRUE;
		}

		if (!valid)
		{
			printf("qin spp format error!");
			Log_e(_T("qin spp format error, raw data=%s"), strRSP);
			goto done;
		}
#endif
	
		if (pFrame->param[0] == 0)
		{
			Log_e(_T("qin spp rsp error, earphone sw not support!, raw data=%s"), strRSP);
			goto done;
		}

		UCHAR *ptr = &pFrame->param[0];
		ptr++;

		*left_earphone = *((struct psensor_cali_struct *)ptr);
		ptr += sizeof(struct psensor_cali_struct);
		*right_earphone = *((struct psensor_cali_struct *)ptr);
		ret = TRUE;

		// 删除多余的0
		strRSP = strRSP.Left(3 * len);
		Log_d(_T("process recv spp data='%s'"), strRSP);
	}
	else
	{
		Log_e(_T("spp rsp error, raw data=%s"), strRSP);
	}

done:
	delete pbuff;


	return ret;
}


/*
 * 更新主线程中的UI显示
 */
void dlg_update_ui(const CString& promptinfo)
{
	//CString *pInfoText = DBG_NEW CString(promptinfo);
	static TCHAR strBuff[128];

	lstrcpy(strBuff, promptinfo);
	CCRY574ProMFCDemoDlg * pDlg = (CCRY574ProMFCDemoDlg *)AfxGetMainWnd();
	if (pDlg)
	{
		::PostMessage(pDlg->m_hWnd, WM_UPDATE_STATIC, (WPARAM)strBuff, FALSE);
	}
	else
	{

	}
}

/*
 * 更新主线程中的UI显示
 */
void dlg_update_status_ui(INT state, const CString& info)
{
	CCRY574ProMFCDemoDlg * pDlg = (CCRY574ProMFCDemoDlg *)AfxGetMainWnd();
	static TCHAR strBuff[128];

	if (pDlg)
	{
#if 0
		CString *pInfoText = DBG_NEW CString(info);
		if (pInfoText == NULL)
		{
			ASSERT(FALSE);
		}
#endif
		lstrcpy(strBuff, info);
		::PostMessage(pDlg->m_hWnd, WM_UPDATE_STATUS, (WPARAM)state, (LPARAM)strBuff);
	}
}


/*
 * 更新主线程中的UI显示
 */
void dlg_update_status_data(INT state, void *data_ptr)
{
	CCRY574ProMFCDemoDlg * pDlg = (CCRY574ProMFCDemoDlg *)AfxGetMainWnd();
	if (pDlg)
	{
		::PostMessage(pDlg->m_hWnd, WM_UPDATE_STATUS, (WPARAM)state, (LPARAM)data_ptr);
	}
}


// 判断agent是否为用户模式
tws_mode_t get_agent_mode()
{
	const char customer_ui_data[] = "05 5A 05 00 00 20 00 0B 12";		// 检查customer ui
	char* pcRecv = DBG_NEW char[4096];
	INT retcode;
	UINT32 bCustomerUi;
	UINT32 bProductMode;
	BOOL ret = FALSE;
	int i;
	int side = 0;
	tws_mode_t mode;
	UINT32 sppret;

	for (i = 0; i < 3; i++)
	{
		retcode = CRYBT_SPPCommand(customer_ui_data, pcRecv, 1000, TRUE);
		Log_d(_T("send spp cmd retcode=%d"), retcode);
		CString strSPPRecv(pcRecv);
		CString strInfo;

		sppret = parse_spp_rsp_data(strSPPRecv, &side);
		strInfo.Format(_T("check customer ui Recv: %s"),strSPPRecv);
		dlg_update_ui(strInfo);

		bCustomerUi = (sppret >> CUSTOMER_UI_INDEX) & 0x1;
		bProductMode = (sppret >> CUSTOMER_PRODUCT_INDEX) & 0x1;

		Log_d(_T("agent customer ui(%d), product mode(%d), side(%d)=%s"), bCustomerUi, bProductMode, side, get_tws_side_str(side));
		ret = (bCustomerUi == TRUE) && (bProductMode == FALSE);
		if (ret)
		{
			break;
		}
	}

	mode.tws_side = side & 0xff;
	if ((bCustomerUi == TRUE) && (bProductMode == FALSE))
	{
		mode.tws_mode = TWS_USER_MODE;
	}
	else if ((bCustomerUi == FALSE) && (bProductMode == TRUE))
	{
		mode.tws_mode = TWS_PRODUCT_MODE;
	}
	else
	{
		mode.tws_mode = TWS_ERROR_MODE;
	}

	//
	delete pcRecv;
	pcRecv = NULL;

	return mode;
}


// 构建从耳race cmd 转发包
UCHAR * cons_relay_race_cmd(UCHAR id, LPCCH cmd_str)
{
	race_cmd_t *pcmd;
	int plen = 0;
	UCHAR *ret = NULL;

	pcmd = (race_cmd_t *) DBG_NEW UCHAR[128];
	if (!pcmd)
	{
		Log_e(_T("no memory error"));
		return NULL;
	}

	pcmd->frame_start = RACE_CMD_FRAME_START;
	pcmd->frame_type = RACE_CMD_REQ;
	pcmd->frame_cmd = 0x0D01;
	pcmd->payload[plen++] = TWS_PARTNER_ID;
	pcmd->payload[plen++] = id;

	TCHAR tbuff[128];
	MultiByteToWideChar(CP_ACP, 0, cmd_str, -1, tbuff, sizeof(tbuff) / sizeof(TCHAR));
	CString strCmd(tbuff);
	UCHAR binData[128];
	int binlen = String2HexData(strCmd, binData);
	int left = 128 - sizeof(race_cmd_t) - 2;
	if (left < binlen) 
	{
		Log_e(_T("left room %d is less than needed room %d"), left, binlen);
		goto done;
	}

	memcpy(&pcmd->payload[plen], binData, binlen);
	plen += binlen;
	pcmd->frame_len = plen + sizeof(pcmd->frame_cmd);

	int cmd_str_len = (pcmd->frame_len + 4) * 4;
	UCHAR *pcmd_str = (UCHAR *) DBG_NEW UCHAR[cmd_str_len];
	if (!pcmd_str) 
	{
		Log_e(_T("no memroy error"));
		goto done;
	}

	if (Binary2HexData((const UCHAR *)pcmd, pcmd->frame_len + 4, pcmd_str, cmd_str_len) < 0)
	{
		Log_e(_T("convert binary to hex data error"));
		delete pcmd_str;
		goto done;
	}
	else
	{
		int slen = strlen((LPCCH)pcmd_str);
		TCHAR *ptchr_buf = (TCHAR *) DBG_NEW TCHAR[slen + 1];

		if (!ptchr_buf)
		{
			Log_e(_T("no memory error, len=%d"), slen);
		}

		memset(ptchr_buf, 0, (slen + 1) * sizeof(TCHAR));
		MultiByteToWideChar(CP_ACP, 0, (LPCCH)pcmd_str, -1, ptchr_buf, slen + 1);

		Log_d(_T("cons hex cmd ok= '%s'"), ptchr_buf);

		delete ptchr_buf;
	}

	ret = pcmd_str;

done:
	delete pcmd;

	return ret;
}

BYTE *get_partner_rsp_str(BYTE id, const CString& strRsp, int *plen)
{
	int nlen = strRsp.GetLength();
	UCHAR *pbuff;
	BYTE *ret = NULL;

	if (nlen == 0)
	{
		Log_e(_T("qin nlen == 0!"));
		return NULL;
	}

	pbuff = DBG_NEW UCHAR[nlen];
	if (!pbuff)
	{
		Log_e(_T("qin no memory error!"));
		goto done;
	}

	int binlen = String2HexData(strRsp, pbuff);
	if (binlen <= 0)
	{
		Log_e(_T("origin spp str=%s"), strRsp);
		goto done;
	}

	// 从BUFFER中找到回复帧数据
	onewire_frame_t *ptr = onewire_get_one_rsp_frame(RACE_CMD_FRAME_START, RACE_CMD_RELAY_RSP, pbuff, &binlen);
	if (ptr)
	{
		race_cmd_t *pcmd = (race_cmd_t *)ptr;

		if (pcmd->frame_cmd != RACE_CMD_RELAY_PARTER_CMD)
		{
			Log_e(_T("race cmd id(%x) is not equal to 0xd000"), pcmd->frame_cmd);
			goto done;
		}

		relay_rsp_t *prsp = &pcmd->u.rsp;
		if (prsp->dst_type != 0x5)
		{
			Log_e(_T("relay dst type(%d) error"), prsp->dst_type);
			goto done;
		}
		
		if (prsp->dst_id != id) 
		{
			Log_e(_T("relay dst id error, dst id=%d, id=%d"), prsp->dst_id, id);
			goto done;
		}

		// ok. 获取正确的rsp响应
		int part_rsp_len = pcmd->frame_len - sizeof(relay_rsp_t) - sizeof(pcmd->frame_cmd) + 1;
		ret = (BYTE *) DBG_NEW BYTE[part_rsp_len];
		if (!ret)
		{
			Log_e(_T("no memory error"));
			goto done;
		}

		memcpy(ret, prsp->partner_data, part_rsp_len);
		*plen = part_rsp_len;
	}
	else
	{
		Log_e(_T("RACE cmd format error"));
	}

done:
	delete pbuff;

	// 删除多余的0

	return ret;
}

UINT32 send_partner_relay_cmd(const char *partner_cmd, int *pside)
{
	const char partner_id_cmd[] = "05 5A 02 00 00 0D";		// 获取partner id
	char* pcRecv = DBG_NEW char[4096];
	int i;
	int retcode;
	BYTE id;
	UINT32 ret = SPP_RSP_ERROR;
	BYTE *pPartner = NULL;
	int len;

	for (i = 0; i < 3; i++)
	{
		retcode = CRYBT_SPPCommand(partner_id_cmd, pcRecv, 1000, TRUE);
		Log_d(_T("send retry (%d) spp cmd retcode=%d"), i, retcode);
		CString strSPPRecv(pcRecv);
		Log_d(_T("recv partner id rsp(%s)"), strSPPRecv);

		id = get_partner_id(strSPPRecv);
		if (id != 0)
		{
			break;
		}
	}

	if (i == 3)
	{
		Log_e(_T("not get partner id"));
		goto done;
	}

	UCHAR *pcmd = cons_relay_race_cmd(id, partner_cmd);
	if (!pcmd)
	{
		Log_e(_T("construct partner cmd error"));
		goto done;
	}

	for (i = 0; i < 3; i++)
	{
		retcode = CRYBT_SPPCommand((const char *)pcmd, pcRecv, 1000, TRUE);
		Log_d(_T("send retry (%d) spp cmd retcode=%d"), i, retcode);
		CString strSPPRecv(pcRecv);
		
		Log_d(_T("Get relay spp rsp:'%s'"), strSPPRecv);
		pPartner = get_partner_rsp_str(id, strSPPRecv, &len);
		if (!pPartner)
		{
			Log_e(_T("get partner retry(%d) data error"), i);
		}
		else
		{
			break;
		}
	}
	if (i == 3)
	{
		goto release_race_cmd;
	}

	// 解析partner数据
	Log_d(_T("get partner data len=%d"), len);
	print_buffer_data(pPartner, len);
	ret = parse_race_cmd_rsp((race_cmd_t *)pPartner, len, pside);

	delete pPartner;

release_race_cmd:
	delete pcmd;

done:
	delete pcRecv;

	return ret;
}

const TCHAR * get_tws_side_str(int side)
{
	if (side == LEFT_CHANNEL)
	{
		return _T("left");
	}
	else if (side == RIGHT_CHANNEL)
	{
		return _T("right");
	}
	else
	{
		return _T("Unkown");
	}
}


// 判断partner是否为用户模式
tws_mode_t get_partner_mode()
{
	const char customer_ui_data[] = "05 5A 05 00 00 20 00 0B 12";		// 检查customer ui
	UINT32 partner_customer_ui;
	UINT32 partner_product_mode;
	UINT32 value;
	int side = 0;
	tws_mode_t mode;

	value = send_partner_relay_cmd(customer_ui_data, &side);

	partner_customer_ui = (value >> CUSTOMER_UI_INDEX) & 0x1;
	partner_product_mode = (value >> CUSTOMER_PRODUCT_INDEX) & 0x1;

	Log_d(_T("partner customer ui(%d), product mode(%d), side(%d)=%s"), partner_customer_ui, partner_product_mode, side, get_tws_side_str(side));

	mode.tws_side = side & 0xff;

	if ((partner_customer_ui == TRUE) && (partner_product_mode == FALSE))
	{
		mode.tws_mode = TWS_USER_MODE;
	}
	else if ((partner_customer_ui == FALSE) && (partner_product_mode == TRUE))
	{
		mode.tws_mode = TWS_PRODUCT_MODE;
	}
	else
	{
		mode.tws_mode = TWS_ERROR_MODE;
	}

	return mode;
}


/*
 * id1/id2 帧头识别关键字节
 */
kal_uint8 *get_rsp_frame(BYTE id1, BYTE id2, kal_uint8* ptr, int len, uint8_t **next_frame, int *left_len)
{
	int i;
	const CHAR *p;
	const onewire_frame_t *pframe = NULL;
	int frame_len;
	
	for (i = 0, p = (const CHAR *)ptr; i < len; i++, p++)
	{
		 if (*p == id1 && (*(p + 1) == id2))
		 {
			pframe = (const onewire_frame_t *)p;

			frame_len = pframe->len + sizeof(pframe->len) + 2; 		// the length of frame length \ header \ type 
			if (frame_len <= (len - i))
			{
				*next_frame = (uint8_t *)(p + frame_len);
				*left_len = len - i - frame_len;

				return (kal_uint8 *)pframe;
			}
			
			// 不够一帧
			*next_frame = (uint8_t *)p;
			*left_len = len - i;
			return NULL;
		 }
	}

	// 未找到帧头
	*next_frame = NULL;
	*left_len = 0;
	
	return NULL;
}


/*
 * buffer中找到关键字所表示的帧数据
 */
onewire_frame_t * onewire_get_one_rsp_frame(BYTE id1, BYTE id2, kal_uint8 * protocol_buffer, int *plen)
{
	int left;
	uint8_t *next;
	onewire_frame_t *p;
	int buffer_index = *plen;

	print_buffer_data(protocol_buffer, buffer_index);

	p = (onewire_frame_t *)get_rsp_frame(id1, id2, protocol_buffer, buffer_index, &next, &left);
	if (!p)
	{
		if (next != NULL)
		{
			memmove(protocol_buffer, next, left);
			buffer_index = left;
		}
		else
		{
			memset(protocol_buffer, 0, sizeof(protocol_buffer));
			buffer_index = 0;
		}

		*plen = buffer_index;

		return NULL;
	}
	else
	{
		print_buffer_data((void *)p, p->len + 4);
	}

	buffer_index = left;
	*plen = buffer_index;

	return p;
}

void check_tws_mode()
{
	tws_mode_t agent;
	tws_mode_t partner;

	agent = get_agent_mode();
	partner = get_partner_mode();

	/* 数据传递给主线程 */
	UCHAR *data_ptr = DBG_NEW UCHAR[2 * sizeof(tws_mode_t)];

	if (!data_ptr)
	{
		Log_e(_T("no memory!"));
		ASSERT(FALSE);
	}

	memcpy(data_ptr, &agent, sizeof(tws_mode_t));
	UCHAR *tmp = data_ptr + sizeof(tws_mode_t);
	memcpy(tmp, &partner, sizeof(tws_mode_t));

	dlg_update_status_data(STATE_TWS_MODE_DATA, (void *)data_ptr);
}




// 线程运行程序
//CRYBT_InitializePro
// CRYBT_ResetDongle
//
UINT thread_process(LPVOID)
{
	CString info;
	int retcode;
	int ret = -1;
	int i;
	CString btdevice;

	current_bt_device.Empty();

	dlg_update_status_ui(STATE_PROCESS);
	bRunning = TRUE;

	reset_partner_id();

	//retcode = CRYBT_ResetDongle();
	//Log_d(_T("reset dongle retcode=%d"), retcode);

	for (i = 0; i < 10; i++)
	{
		btdevice = begin_inquiry_bt_device();
		if (bStopped)
		{
			i = 10;
			dlg_update_status_ui(STATE_ABORT);
			break;
		}

		if (btdevice.IsEmpty())
		{
			continue;
		}

		Log_d(_T("bt device mac: %s"), btdevice);
		if (!connect_bt_device(btdevice))
		{
			if ((retcode = CRYBT_Disconnect()) == API_OK)
			{
				info.Format(_T("Disconnected"));
			} 
			else
			{
				info.Format(_T("error code is %d"),retcode);
			}
			dlg_update_ui(info);
			Log_d(_T("CRYBT_Disconnect retcode=%d"), retcode);
			continue;
		}

		if (check_bt_name())
		{
			current_bt_device = btdevice;
			break;
		}
		else
		{
			goto disconn_bt;
		}
	}

	if (i == 10)
	{
		goto disconn_bt;
	}

	// TODO: 在此添加控件通知处理程序代码
	retcode = CRYBT_SetDefaultProfile(1);
	Log_d(_T("CRYBT_SetDefaultProfile retcode=%d"), retcode);
	dlg_update_ui(_T("配置SPP成功"));

	if (!connect_bt_spp(btdevice))
	{
		Log_e(_T("connect device(%s) failed!"), btdevice);
		dlg_update_status_ui(STATE_ABORT);
		goto disconn_bt;
	}

	Log_d(_T("begin to test bda(%s)"), current_bt_device);

	int test_items = get_test_item_setting_bitmap();

	if (test_items & (1 << TEST_PSENSOR_INDEX))
	{
		get_psensor_rawdata();			// 先获取psensor raw data.
		check_psensor_calibrated_2();
		//check_psensor_rawdata();
	}
	
	if (test_items & (1 << TEST_SW_VERSION_INDEX))
	{
		check_software_version();
	}

	if (test_items & (1 << TEST_USER_MODE_INDEX))
	{
		check_tws_mode();		// 最后一个
	}

	if (test_items & (1 << TEST_EP_COLOR_INDEX))
	{
		check_ep_color();
	}

	if (test_items & (1 << TEST_WRITE_ANC_GAIN_INDEX))
	{
	//	write_agent_anc_gain();			// 保险先去掉
	//	write_partner_anc_gain();
	}

	if (test_items & (1 << TEST_FACTORY_RESET_INDEX))
	{
		send_system_factory_cmd();
	}

	Log_d(_T("test bda(%s) end."), current_bt_device);

	ret = 0;


	if ((retcode = CRYBT_DisConnectSPP()) == API_OK)
	{
		info.Format(_T("Disconnect SPP"));
	} 
	else
	{
		info.Format(_T("error code is %d"),retcode);
	}

	Log_d(_T("CRYBT_DisConnectSPP retcode=%d"), retcode);

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
	Log_d(_T("CRYBT_Disconnect retcode=%d"), retcode);

	//retcode = CRYBT_ResetDongle();
	//Log_d(_T("CRYBT_ResetDongle retcode=%d"), retcode);

	dlg_update_ui(_T("done!"));
	dlg_update_status_ui(STATE_DONE);

	t5506_send_uart_cmd(OPEN_BOX);

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
