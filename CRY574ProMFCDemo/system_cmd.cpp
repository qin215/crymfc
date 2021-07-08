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
/*
 * 压力测试恢复出厂
 */


/*
 * race cmd 回应解析处理
 */
BOOL parse_race_cmd_rsp_help(CString& strRSP, race_cmd_rsp_callback_func callback)
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
		int side = 0;
	
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
		ret = callback(pcmd, len, &side);
		
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


// 给agent发送指令
BOOL send_race_cmd_to_agent(const char *pcmd, race_cmd_rsp_callback_func callback)
{
	//const char anc_gain[] = "05 5A 10 00 06 0E 00 0C D8 DC C8 00 D8 DC C8 00 00 00 00 00";		// write anc gain
	char* pcRecv = DBG_NEW char[4096];
	INT retcode;
	int i;
	BOOL ret = FALSE;
	CString raceCmd(pcmd);
	
	for (i = 0; i < 3; i++)
	{
		Log_d(_T("send race cmd, retry(%d):%s"), i, raceCmd);
		retcode = CRYBT_SPPCommand(pcmd, pcRecv, 1000, TRUE);
		Log_d(_T("send spp cmd retcode=%d"), retcode);
		CString strSPPRecv(pcRecv);
		CString strInfo;

		ret = parse_race_cmd_rsp_help(strSPPRecv, callback);
		strInfo.Format(_T("get race cmd Recv, retry(%d): %s"), i, strSPPRecv);
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


// 给partner发送指令
BOOL send_race_cmd_to_partner(const char *pcmd, race_cmd_rsp_callback_func callback)
{
	//const char anc_gain[] = "05 5A 10 00 06 0E 00 0C D8 DC C8 00 D8 DC C8 00 00 00 00 00";		// write anc gain
	BOOL ret;

	ret = send_partner_relay_cmd_help(pcmd, callback);
	return ret;
}



/*
 * 恢复出厂设置
 */
void send_system_factory_cmd()
{
	const char factory_cmd[] = "05 5A 05 00 00 20 0B 00 0B";
	int ret;

	ret = send_race_cmd_to_partner(factory_cmd, parse_race_cmd_rsp);
	Log_d(_T("factory partner status: %d"), ret);
	ret = send_race_cmd_to_agent(factory_cmd, parse_race_cmd_rsp);
	Log_d(_T("factory agent status: %d"), ret);
}

