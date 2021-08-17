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

static int ep_side;

static race_rsp_rawdata_t left_rawdata;
static race_rsp_rawdata_t right_rawdata;

static psensor_cali_data_t left_cali_data;
static psensor_cali_data_t right_cali_data;

/*
 * 获取psensor的raw data.
 */
static UINT32 process_psensor_rawdata_bindata(race_cmd_t *pcmd , int data_len, int *pside)
{
	if (pcmd->frame_cmd != CUSTOMER_RACE_CMD)
	{
		print_buffer_data((void *)pcmd, data_len);
		return 0;
	}

	race_rsp_rawdata_t *prawdata = &pcmd->psensor_rawdata_rsp;
	if (prawdata->result != 1)
	{
		Log_e(_T("spp get sw version status(%d) error"), prawdata->result);
		return 0;
	}

	if (pside)
	{
		*pside = prawdata->side;
	}

	ep_side = prawdata->side;

	if (prawdata->side == ONEWIRE_LEFT_CHANNEL)
	{
		left_rawdata = *prawdata;
	}
	else if (prawdata->side == ONEWIRE_RIGHT_CHANNEL)
	{
		right_rawdata = *prawdata;
	}
	else
	{
		Log_e(_T("raw data side(%d) error"), prawdata->side);
		return 0;
	}

	return 1;
}

/*
 * 获取主从耳的psensor raw data.
 */
void get_psensor_rawdata()
{
	const char raw_data_cmd[] = "05 5A 05 00 00 20 31 00 31";
	int ret;

	memset(&left_rawdata, 0, sizeof(left_rawdata));
	memset(&right_rawdata, 0, sizeof(left_rawdata)); 
	ep_side = -1;

	ret = send_race_cmd_to_partner(raw_data_cmd, process_psensor_rawdata_bindata);
	if (ret == TRUE)
	{
		if (ep_side == ONEWIRE_LEFT_CHANNEL)
		{
			Log_d(_T("left rawdata: %d, side=%d"), left_rawdata.raw_data, ep_side);
		}
		else if (ep_side == ONEWIRE_RIGHT_CHANNEL)
		{
			Log_d(_T("right rawdata: %d, side=%d"), right_rawdata.raw_data, ep_side);
		}
	}
	else
	{
		Log_e(_T("get partner rawdata failed!\n"));
	}

	ret = send_race_cmd_to_agent(raw_data_cmd, process_psensor_rawdata_bindata);

	if (ret == TRUE)
	{
		if (ep_side == ONEWIRE_LEFT_CHANNEL)
		{
			Log_d(_T("left rawdata: %d, side=%d"), left_rawdata.raw_data, ep_side);
		}
		else if (ep_side == ONEWIRE_RIGHT_CHANNEL)
		{
			Log_d(_T("right rawdata: %d, side=%d"), right_rawdata.raw_data, ep_side);
		}
	}
	else
	{
		Log_e(_T("get agent rawdata failed!\n"));
	}
}


// 第二版本，发送一次指令
BOOL check_psensor_calibrated_2()
{
	const char tws_cali_data_cmd[] = "05 5A 05 00 00 20 00 0B 30";
	BOOL ret = FALSE;
	int i;

	left_cali_data.cali_flag = FALSE;
	right_cali_data.cali_flag = FALSE;

	char* pcRecv = DBG_NEW char[4096];
	INT retcode;

	// 入耳数据
	retcode = CRYBT_SPPCommand(tws_cali_data_cmd, pcRecv, 1000, TRUE);
	Log_d(_T("send spp cmd retcode=%d"), retcode);
	CString strSPPRecv(pcRecv);
	CString strInfo;
	psensor_cali_data_t left;
	psensor_cali_data_t right;

	for (i = 0; i < 3; i++)		// 重试3次
	{
		ret = parse_spp_rsp_data_2(strSPPRecv, &left, &right);
		strInfo.Format(_T("SPP TWS Recv:%s"),strSPPRecv);
		dlg_update_ui(strInfo);
		if (!ret)
		{
			strInfo.Format(_T("spp TWS CMD error, please check sw version > V2.9"));
			Log_e(_T("retry count=%d, spp TWS CMD error, please check sw version, recv rsp='%s'"), i, strSPPRecv);
			dlg_update_ui(strInfo);
		}
		else
		{
			break;
		}
	}

	if (i == 3)
	{
		goto done;
	}

	/* 数据传递给主线程 */
	UCHAR *data_ptr = DBG_NEW UCHAR[2 * sizeof(psensor_cali_data_t)];

	if (!data_ptr)
	{
		Log_e(_T("no memory!"));
		ASSERT(FALSE);
	}

	memcpy(data_ptr, &left, sizeof(psensor_cali_data_t));
	UCHAR *tmp = data_ptr + sizeof(psensor_cali_data_t);
	memcpy(tmp, &right, sizeof(psensor_cali_data_t));

	left_cali_data = left;
	right_cali_data = right;

	dlg_update_status_data(STATE_TWS_CALI_DATA, (void *)data_ptr);
	ret = TRUE;
done:
	delete pcRecv;

	return ret;
}


static int calculate_low_threshold(psensor_cali_data_t *pdata)
{
	int delta;

	delta = pdata->gray_value - pdata->base_value;
	return pdata->base_value + delta * 7 / 10;
}

static BOOL check_ep_psensor_rawdata(race_rsp_rawdata_t *prawdata, psensor_cali_data_t *pcalidata)
{
	BOOL ret = FALSE;

	if (pcalidata->cali_flag == FALSE || prawdata->result == FALSE)
	{
		Log_e(_T("%s ep is not calibrated! OR no left raw data\n"), prawdata->side == ONEWIRE_LEFT_CHANNEL ? _T("left") :  _T("right"));
	}
	else
	{
		int low_threhold = calculate_low_threshold(pcalidata);

		if (prawdata->raw_data >= low_threhold)
		{
			Log_e(_T("bda(%s), psensor %s data error: cali_base=%d(0x%x), cali_gray=%d(0x%x), raw_data=%d(0x%x), low_threhold=%d(0x%x)\n"), 
				current_bt_device,
				prawdata->side == ONEWIRE_LEFT_CHANNEL ? _T("left") :  _T("right"),
				pcalidata->base_value, pcalidata->base_value,
				pcalidata->gray_value, pcalidata->gray_value,
				prawdata->raw_data, prawdata->raw_data, 
				low_threhold, low_threhold);
		}
		else
		{
			Log_d(_T("bda(%s),psensor %s data OK: cali_base=%d(0x%x), cali_gray=%d(0x%x), raw_data=%d(0x%x), low_threhold=%d(0x%x)\n"), 
				current_bt_device,
				prawdata->side == ONEWIRE_LEFT_CHANNEL ? _T("left") :  _T("right"),
				pcalidata->base_value, pcalidata->base_value,
				pcalidata->gray_value, pcalidata->gray_value,
				prawdata->raw_data, prawdata->raw_data, 
				low_threhold, low_threhold);
			ret = TRUE;
		}
	}

	return ret;
}


/*
 *  判断raw data 出耳情况下是否处于 出耳阈值以下
 */
BOOL check_psensor_rawdata()
{
	BOOL left_result = FALSE;
	BOOL right_result = FALSE;

	left_result = check_ep_psensor_rawdata(&left_rawdata, &left_cali_data);
	right_result = check_ep_psensor_rawdata(&right_rawdata, &right_cali_data);

	return left_result && right_result;
}

