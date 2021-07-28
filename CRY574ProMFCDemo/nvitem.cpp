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
 * 读取特定的NVR值
 */


// 获取用户版本号
TCHAR * get_agent_nvitem_version(UINT16 nvkey_id, int data_len)
{
	char read_nvkey_data[32] = "05 5A 06 00 00 0A";		// 读取软件版本号
	char* pcRecv = DBG_NEW char[4096];
	INT retcode;
	int i;
	int len;

	len = strlen(read_nvkey_data);
	sprintf(&read_nvkey_data[len], " %02X %02X FF FF", nvkey_id & 0xff, (nvkey_id >> 8) & 0xff);


	for (i = 0; i < 1; i++)
	{
		CString cmd(read_nvkey_data);
		Log_d(_T("send spp cmd retcode='%s'"), cmd);
		retcode = CRYBT_SPPCommand(read_nvkey_data, pcRecv, 1000, TRUE);
		Log_d(_T("send spp cmd retcode=%d"), retcode);
		CString strSPPRecv(pcRecv);
		CString strInfo;

		Log_d(_T("Get nvkey spp rsp:'%s'"), strSPPRecv);
		strInfo.Format(_T("check nvkey Recv: %s"), strSPPRecv);
		dlg_update_ui(strInfo);
	}

	//
	delete pcRecv;
	pcRecv = NULL;

	return NULL;
}



UINT32 process_nvkey_bindata(race_cmd_t *pcmd , int data_len, int *pside)
{
	if (pcmd->frame_cmd != RACE_CMD_READ_NVKEY)
	{
		print_buffer_data((void *)pcmd, data_len);
		return 0;
	}

	race_rsp_nvkey_t *pnvkey = &pcmd->nvkey_value_rsp;
	if (pnvkey->nvkey_len == 0)
	{
		Log_e(_T("spp get nvkey  status error"));
		return 0;
	}

	int nlen = pnvkey->nvkey_len;
	
	print_buffer_data(&pnvkey->data[0], nlen);

	return (UINT32)0;
}

// 获取付耳NVKEY值
TCHAR * get_partner_nvkey(UINT16 nvkey_id, int data_len)
{
	char read_nvkey_data[32] = "05 5A 06 00 00 0A";		// 读取软件版本号
	int len;

	len = strlen(read_nvkey_data);
	sprintf(&read_nvkey_data[len], " %02X %02X FF FF", nvkey_id & 0xff, (nvkey_id >> 8) & 0xff);
	TCHAR *pVersion = NULL;

	pVersion = (TCHAR *)send_partner_relay_cmd_help(read_nvkey_data, process_nvkey_bindata);
	return pVersion;
}


void test_read_nvkey()
{
	get_agent_nvitem_version(0xF501, 16);
	get_agent_nvitem_version(0xF502, 16);
	get_agent_nvitem_version(0xFE05, 16);

	get_partner_nvkey(0xF501, 16);
	get_partner_nvkey(0xF502, 16);
	get_partner_nvkey(0xFE05, 16);


	get_agent_nvitem_version(0xF23B, 16);
	get_partner_nvkey(0xF23B, 16);


}