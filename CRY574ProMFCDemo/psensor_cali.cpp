#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ServerCmd.h"
#include "data_buff.h"
#include "uart_cmd.h"
#include "qcloud_iot_export_log.h"
#include "CRY574ProAPIWrapper.h"
#include "CRY574ProMFCDemo.h"
#include "CRY574ProMFCDemoDlg.h"
#include "afxdialogex.h"
#include "protocol.h"
#include "mywin.h"


/*
 * psensor SPP校准文件
 */

static int earphone_side;
static BOOL cali_ret;

static int earphone_cali_status;

static int partner_cali_status = 0;
static int agent_cali_status = 0;

/*
 * 获取psensor校准指令返回值
 */
static UINT32 process_psensor_cali_cmd_bindata(race_cmd_t *pcmd , int data_len, int *pside)
{
	if (pcmd->frame_cmd != CUSTOMER_RACE_CMD)
	{
		print_buffer_data((void *)pcmd, data_len);
		return 0;
	}

	race_rsp_rawdata_t *prawdata = &pcmd->psensor_rawdata_rsp;
	cali_ret = prawdata->result;
	if (prawdata->result != 1)
	{
		Log_e(_T("spp enter calibrate mode status(%d) error"), prawdata->result);
		return 0;
	}

	if (pside)
	{
		*pside = prawdata->side;
	}

	earphone_side = prawdata->side;



	return 1;
}


/*
 * 获取psensor校准的状态
 */
static UINT32 process_psensor_query_cali_status_bindata(race_cmd_t *pcmd , int data_len, int *pside)
{
	if (pcmd->frame_cmd != CUSTOMER_RACE_CMD)
	{
		print_buffer_data((void *)pcmd, data_len);
		return 0;
	}

	race_rsp_rawdata_t *prawdata = &pcmd->psensor_rawdata_rsp;

	earphone_cali_status = prawdata->result;
	Log_e(_T("spp get query calibrate status(%d)"), prawdata->result);
	

	if (pside)
	{
		*pside = prawdata->side;
	}

	earphone_side = prawdata->side;

	return 1;
}



// 发送出入耳校准指令
int send_psensor_calibrate_cmd(int cmd)
{
	char cali_cmd[] = "05 5A 05 00 00 20 30 0B 30";
	CString buffer(cali_cmd);
	UCHAR bindata[32];
	int binlen;
	int ret;
	BOOL val = TRUE;

	binlen = String2HexData(buffer, bindata);

	bindata[binlen - 1] = cmd & 0xff;
	bindata[binlen - 3] = cmd & 0xff;

	CHAR new_cmd[32];
	memset(new_cmd, 0, sizeof(new_cmd));
	Binary2HexData(bindata, binlen, (UCHAR *)new_cmd, sizeof(new_cmd));

	agent_cali_status = -1;
	partner_cali_status = -1;

	cali_ret = FALSE;
	earphone_side = -1;

#if 1
	ret = send_race_cmd_to_partner(new_cmd, process_psensor_cali_cmd_bindata);
	if (ret == TRUE)
	{
		if (earphone_side == ONEWIRE_LEFT_CHANNEL)
		{
			Log_d(_T("left earphone enter calibrate mode status(%d)"), cali_ret);
		}
		else if (earphone_side == ONEWIRE_RIGHT_CHANNEL)
		{
			Log_d(_T("right earphone enter calibrate mode status(%d)"), cali_ret);
		}

		val = TRUE;
	}
	else
	{
		Log_e(_T("send agent calibrate cmd failed!\n"));
		val = FALSE;
	}
#endif
	cali_ret = FALSE;
	earphone_side = -1;
	ret = send_race_cmd_to_agent(new_cmd, process_psensor_cali_cmd_bindata);
	if (ret == TRUE)
	{
		if (earphone_side == ONEWIRE_LEFT_CHANNEL)
		{
			Log_d(_T("left earphone enter calibrate mode status(%d)"), cali_ret);
		}
		else if (earphone_side == ONEWIRE_RIGHT_CHANNEL)
		{
			Log_d(_T("right earphone enter calibrate mode status(%d)"), cali_ret);
		}

		val = TRUE;
	}
	else
	{
		Log_e(_T("send partner calibrate cmd failed!\n"));
		val = FALSE;
	}

	return val;
}

// 获取校准状态
int check_psensor_calibrate_status()
{
	char query_cali_status[] = "05 5A 05 00 00 20 22 0B 22";
	int ret;

	earphone_side = -1;
	earphone_cali_status = 0;

#if 1
	if (partner_cali_status != PSENSOR_QUERY_CALI_SUCCESS || partner_cali_status == -1)
	{
		ret = send_race_cmd_to_partner(query_cali_status, process_psensor_query_cali_status_bindata);
		if (ret == TRUE)
		{
			if (earphone_side == ONEWIRE_LEFT_CHANNEL)
			{
				Log_d(_T("left earphone calibrate status(%d)"), earphone_cali_status);
				partner_cali_status = earphone_cali_status;
			}
			else if (earphone_side == ONEWIRE_RIGHT_CHANNEL)
			{
				Log_d(_T("right earphone calibrate status(%d)"), earphone_cali_status);
				partner_cali_status = earphone_cali_status;
			}
		}
		else
		{
			Log_e(_T("send partner calibrate cmd failed!\n"));
		}
	}
#endif

	earphone_side = -1;
	earphone_cali_status = 0;
	if (agent_cali_status == PSENSOR_QUERY_CALI_DOING || agent_cali_status == -1)
	{
		ret = send_race_cmd_to_agent(query_cali_status, process_psensor_query_cali_status_bindata);
		if (ret == TRUE)
		{
			if (earphone_side == ONEWIRE_LEFT_CHANNEL)
			{
				Log_d(_T("left earphone enter calibrate mode status(%d)"), earphone_cali_status);
				agent_cali_status = earphone_cali_status;
			}
			else if (earphone_side == ONEWIRE_RIGHT_CHANNEL)
			{
				Log_d(_T("right earphone enter calibrate mode status(%d)"), earphone_cali_status);
				agent_cali_status = earphone_cali_status;
			}
		}
		else
		{
			Log_e(_T("send agent calibrate cmd failed!\n"));
		}
	}

	if ((agent_cali_status == PSENSOR_QUERY_CALI_DOING) || (partner_cali_status == PSENSOR_QUERY_CALI_DOING))
	{
		return PSENSOR_QUERY_CALI_DOING;
	}

	if ((agent_cali_status == PSENSOR_QUERY_CALI_FAIL) || (partner_cali_status == PSENSOR_QUERY_CALI_FAIL))
	{
		return PSENSOR_QUERY_CALI_FAIL;
	}
	
	return PSENSOR_QUERY_CALI_SUCCESS;
}


#define RETRY_COUNT 10
/*
 * psensor 校准过程
 */
BOOL process_psensor_calibrate()
{
	int i;
	BOOL ret = FALSE;
	int val;
	BOOL bStop = FALSE;
	CString info(_T("校准出耳"));

	dlg_update_status_ui(STATE_PSENSOR_SEND_CALIBRATE_OUT_EAR, info);
	if (send_psensor_calibrate_cmd(PSENSOR_RACE_CAL_CT) == 0)
	{
		dlg_update_status_data(STATE_PSENSOR_CALIBRATE_DONE, (void *)NULL);
		return FALSE;
	}

	info = _T("查询出耳校准状态");
	dlg_update_status_ui(STATE_PSENSOR_QUERY_CALIBRATE_OUT_EAR, info);
	for (i = 0; i < RETRY_COUNT; i++)
	{
		val = check_psensor_calibrate_status();

		if (val == PSENSOR_QUERY_CALI_SUCCESS)
		{
			ret = TRUE;
			bStop = TRUE;
		}
		else if (val == PSENSOR_QUERY_CALI_FAIL)
		{
			ret = FALSE;
			bStop = TRUE;
		}

		if (bStop)
		{
			break;
		}

		Sleep(50);
	}

	if (!ret)
	{
		dlg_update_status_data(STATE_PSENSOR_CALIBRATE_DONE, (void *)NULL);
		return FALSE;
	}

	AfxMessageBox(_T("入耳校准"));

	info = _T("校准入耳");
	dlg_update_status_ui(STATE_PSENSOR_SEND_CALIBRATE_IN_EAR, info);
	ret = FALSE;
	bStop = FALSE;
	if (send_psensor_calibrate_cmd(PSENSOR_RACE_CAL_G2) == 0)
	{
		dlg_update_status_data(STATE_PSENSOR_CALIBRATE_DONE, (void *)NULL);
		return FALSE;
	}

	info = _T("查询入耳校准状态");
	dlg_update_status_ui(STATE_PSENSOR_QUERY_CALIBRATE_IN_EAR, info);
	for (i = 0; i < RETRY_COUNT; i++)
	{
		val = check_psensor_calibrate_status();
		if (val == PSENSOR_QUERY_CALI_SUCCESS)
		{
			ret = TRUE;
			bStop = TRUE;
		}
		else if (val == PSENSOR_QUERY_CALI_FAIL)
		{
			ret = FALSE;
			bStop = TRUE;
		}

		if (bStop)
		{
			break;
		}

		Sleep(50);
	}

	dlg_update_status_data(STATE_PSENSOR_CALIBRATE_DONE, (void *)ret);

	return ret;
}