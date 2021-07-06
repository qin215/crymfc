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


UINT32 process_sw_version_bindata(race_cmd_t *pcmd , int data_len, int *pside)
{
	if (pcmd->frame_cmd != RACE_CMD_GET_SW_VERSION)
	{
		print_buffer_data((void *)pcmd, data_len);
		return 0;
	}

	race_rsp_sw_version_t *psw_version = &pcmd->sw_ver_rsp;
	if (psw_version->status != 0)
	{
		Log_e(_T("spp get sw version status(%d) error"), psw_version->status);
		return 0;
	}

	int nlen = psw_version->str_len;
	TCHAR *strVersion = (TCHAR *) DBG_NEW TCHAR[nlen + 1];
	if (!strVersion)
	{
		Log_e(_T("no memory error!"));
		return 0;
	}

	CHAR *chVersion = (CHAR *)DBG_NEW CHAR[nlen + 1];
	if (!chVersion)
	{
		delete strVersion;
		Log_e(_T("no memory error!"));
		return 0;
	}

	memset(chVersion, 0, nlen + 1);
	memset(strVersion, 0, (nlen + 1) * sizeof(TCHAR));

	memcpy(chVersion, psw_version->version, nlen);
	MultiByteToWideChar(CP_ACP, 0, (LPCCH)chVersion, -1, strVersion, nlen + 1);
	delete chVersion;

	return (UINT32)strVersion;
}

/*
 * ��ȡ����������汾��
 */
TCHAR* parse_sw_version_rsp(CString& strRSP)
{
	int nlen = strRSP.GetLength();
	UCHAR *pbuff;
	TCHAR *pVersion = NULL;

	Log_d(_T("qin spp orgin rsp str ='%s'"), strRSP);
	if (nlen == 0)
	{
		return NULL;
	}

	pbuff = DBG_NEW UCHAR[nlen];
	if (!pbuff)
	{
		Log_e(_T("qin no memory!"));
		AfxMessageBox(_T("û���ڴ�"));
		PostQuitMessage(0);			// �˳�
	}

	int binlen = String2HexData(strRSP, pbuff);
	if (binlen <= 0)
	{
		Log_e(_T("qin data format error!raw data=%s"), strRSP);
		goto done;				// ����ֵ���󣬽����ط�
	}

	onewire_frame_t *pFrame = onewire_get_one_rsp_frame(RACE_CMD_FRAME_START, RACE_CMD_RELAY_RSP, pbuff, &binlen);
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
		pVersion = (TCHAR *)process_sw_version_bindata(pcmd, len, NULL);
		
		// ɾ�������0
		strRSP = strRSP.Left(3 * len);
		Log_d(_T("process recv spp data='%s'"), strRSP);
	}
	else
	{
		Log_e(_T("spp rsp error, raw data=%s"), strRSP);
	}

done:
	delete pbuff;

	return pVersion;
}


// ��ȡ�û��汾��
TCHAR * get_agent_sw_version()
{
	const char sw_version_data[] = "05 5A 04 00 07 1C 01 FF";		// ��ȡ����汾��
	char* pcRecv = DBG_NEW char[4096];
	INT retcode;
	int i;
	TCHAR *pVersion = NULL;

	for (i = 0; i < 3; i++)
	{
		retcode = CRYBT_SPPCommand(sw_version_data, pcRecv, 1000, TRUE);
		Log_d(_T("send spp cmd retcode=%d"), retcode);
		CString strSPPRecv(pcRecv);
		CString strInfo;

		pVersion = parse_sw_version_rsp(strSPPRecv);
		strInfo.Format(_T("check sw version Recv: %s"),strSPPRecv);
		dlg_update_ui(strInfo);

		if (pVersion)
		{
			break;
		}
	}

	//
	delete pcRecv;
	pcRecv = NULL;

	return pVersion;
}

/*
 * ����ָ��
 */
UINT32 send_partner_relay_cmd_help(const char *partner_cmd, race_cmd_rsp_callback_func cbFunc)
{
	const char partner_id_cmd[] = "05 5A 02 00 00 0D";		// ��ȡpartner id
	char* pcRecv = DBG_NEW char[4096];
	int i;
	int retcode;
	BYTE id;
	UINT32 ret = (UINT32)NULL;
	BYTE *pPartner = NULL;
	int len;
	INT side = 0;

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

	// ����partner����
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


// ��ȡ�û��汾��
TCHAR * get_partner_sw_version()
{
	const char sw_version_data[] = "05 5A 04 00 07 1C 01 FF";		// ��ȡ����汾��
	TCHAR *pVersion = NULL;

	pVersion = (TCHAR *)send_partner_relay_cmd_help(sw_version_data, process_sw_version_bindata);
	return pVersion;
}


void check_software_version()
{
	TCHAR *pAgentVersion = get_agent_sw_version();
	TCHAR *pPartnerVersion = get_partner_sw_version();
	tws_sw_version_t *pVersion;

	pVersion = (tws_sw_version_t *) DBG_NEW tws_sw_version_t[1];
	if (!pVersion)
	{
		Log_e(_T("no memory error!"));
		if (pAgentVersion)
		{
			delete pAgentVersion;
		}

		if (pPartnerVersion)
		{
			delete pPartnerVersion;
		}

		return;
	}

	pVersion->pAgent = pAgentVersion;
	pVersion->pPartner = pPartnerVersion;
	/* ���ݴ��ݸ����߳� */
	dlg_update_status_data(STATE_TWS_VERSION_DATA, (void *)pVersion);
}
