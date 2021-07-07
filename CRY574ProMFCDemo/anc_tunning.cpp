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

UINT32 process_anc_gain_bindata(race_cmd_t *pcmd , int data_len, int *pside)
{
	if (pcmd->frame_cmd != RACE_CMD_WRITE_ANC_GAIN)
	{
		print_buffer_data((void *)pcmd, data_len);
		return FALSE;
	}

	race_rsp_anc_gain_t *pAnc = &pcmd->anc_gain_rsp;
	if (pAnc->status != 0)
	{
		Log_e(_T("spp writing anc gain status(%d) error"), pAnc->status);
		return FALSE;
	}
	else
	{
		Log_d(_T("spp writing anc gain status(%d) ok"), pAnc->status);
		return TRUE;
	}
}

/*
 * 写gain是否成功
 */
BOOL parse_writing_anc_gain_rsp(CString& strRSP)
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
		Log_e(_T("qin no memory!"));
		AfxMessageBox(_T("没有内存"));
		PostQuitMessage(0);			// 退出
	}

	int binlen = String2HexData(strRSP, pbuff);
	if (binlen <= 0)
	{
		Log_e(_T("qin data format error!raw data=%s"), strRSP);
		goto done;				// 返回值错误，进行重发
	}

	onewire_frame_t *pFrame = onewire_get_one_rsp_frame(RACE_CMD_FRAME_START, RACE_CMD_RSP, pbuff, &binlen);
	if (pFrame)
	{
		BOOL valid = FALSE;
	
		uint16_t len = pFrame->len;
		len += 4;			// added length & header & type
		if (len <= binlen)
		{
			valid = TRUE;
		}

		if (!valid)
		{
			Log_e(_T("qin spp format error, raw data=%s"), strRSP);
			goto done;
		}

		race_cmd_t *pcmd = (race_cmd_t *)pFrame;
		ret = process_anc_gain_bindata(pcmd, len, NULL);
		
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

// 写gain是否成功
BOOL write_agent_anc_gain()
{
	const char anc_gain[] = "05 5A 10 00 06 0E 00 0C D8 DC C8 00 D8 DC C8 00 00 00 00 00";		// write anc gain
	char* pcRecv = DBG_NEW char[4096];
	INT retcode;
	int i;
	BOOL ret = FALSE;

	for (i = 0; i < 3; i++)
	{
		retcode = CRYBT_SPPCommand(anc_gain, pcRecv, 1000, TRUE);
		Log_d(_T("send spp cmd retcode=%d"), retcode);
		CString strSPPRecv(pcRecv);
		CString strInfo;

		ret = parse_writing_anc_gain_rsp(strSPPRecv);
		strInfo.Format(_T("check sw version Recv: %s"),strSPPRecv);
		dlg_update_ui(strInfo);

		if (ret)
		{
			break;
		}
	}

	//
	delete pcRecv;
	pcRecv = NULL;

	return ret;
}


// 写gain是否成功
BOOL write_partner_anc_gain()
{
	const char anc_gain[] = "05 5A 10 00 06 0E 00 0C D8 DC C8 00 D8 DC C8 00 00 00 00 00";		// write anc gain
	BOOL ret;

	ret = send_partner_relay_cmd_help(anc_gain, process_anc_gain_bindata);
	return ret;
}

