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

static BYTE partner_id = 0xff;

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

#if 0
		if (len <= binlen)
		{
			valid = TRUE;
		}

		if (!valid)
		{
			Log_e(_T("qin spp format error, raw data=%s"), strRSP);
			goto done;
		}
#endif

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


UINT32 get_partner_id(CString& strRSP)
{
	int nlen = strRSP.GetLength();
	UCHAR *pbuff;
	UINT32 ret = 0;
	int i;

	if (nlen == 0)
	{
		Log_e(_T("qin nlen == 0!"));
		return 0;
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

	if (pbuff[0] == RACE_CMD_FRAME_START && pbuff[1] == RACE_CMD_RSP)
	{
		race_cmd_t *pcmd = (race_cmd_t *)pbuff;

		if (pcmd->frame_cmd != RACE_CMD_GET_PARTNER_ID)
		{
			Log_e(_T("race cmd id(%x) is not equal to 0xd000 error"), pcmd->frame_cmd);
			goto done;
		}
		int payload_len = pcmd->frame_len - sizeof(pcmd->frame_cmd);
		if (payload_len % 2 != 0)
		{
			Log_e(_T("race cmd payload len(%d) is not even error"), payload_len);
			goto done;
		}

		for (i = 0; i < payload_len; i += 2)
		{
			if (pcmd->payload[i] == TWS_PARTNER_ID)
			{
				ret = pcmd->payload[ i+ 1];
				break;
			}
		}

		if (i == payload_len)
		{
			Log_e(_T("not found TWS partner id error"));
		}
		else
		{
			Log_d(_T("TWS partner id = 0x%x"), ret);
		}
	}
	else
	{
		Log_e(_T("RACE cmd format error"));
	}

done:
	delete pbuff;

	// 删除多余的0
	strRSP = strRSP.Left(3 * 15);
	Log_d(_T("recv spp data='%s'"), strRSP);

	return ret;
}



/*
 * 返回指针
 */
UINT32 send_partner_relay_cmd_help(const char *partner_cmd, race_cmd_rsp_callback_func cbFunc)
{
	const char partner_id_cmd[] = "05 5A 02 00 00 0D";		// 获取partner id
	char* pcRecv = DBG_NEW char[4096];
	int i;
	int retcode;
	BYTE id;
	UINT32 ret = (UINT32)NULL;
	BYTE *pPartner = NULL;
	int len;
	INT side = 0;

	if (partner_id == 0xFF)
	{
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

		partner_id = id;
	}
	else
	{
		id = partner_id;
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

	ret = cbFunc((race_cmd_t *)pPartner, len, &side);

	delete pPartner;

release_race_cmd:
	delete pcmd;

done:
	delete pcRecv;

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


void reset_partner_id()
{
	partner_id = 0xff;
}