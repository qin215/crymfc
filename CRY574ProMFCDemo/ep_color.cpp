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


static int left_ep_color;			// 左耳颜色
static int right_ep_color;			// 右耳颜色
static int ep_side;


const TCHAR * get_ep_color_string(int color)
{
	if (color == EP_COLOR_BLACK)
	{
		return _T("Black");
	}

	if (color == EP_COLOR_WHITE)
	{
		return _T("White");
	}

	return _T("error");
}


/*
 * 获取psensor的raw data.
 */
static UINT32 process_ep_color_bindata(race_cmd_t *pcmd , int data_len, int *pside)
{
	if (pcmd->frame_cmd != CUSTOMER_RACE_CMD)
	{
		print_buffer_data((void *)pcmd, data_len);
		return 0;
	}

	race_rsp_color_t *pcolordata = &pcmd->ep_color_rsp;
	if (pcolordata->result != 1)
	{
		Log_e(_T("spp get color status(%d) error"), pcolordata->result);
		return 0;
	}

	if (pside)
	{
		*pside = pcolordata->side;
	}

	ep_side = pcolordata->side;

	if (pcolordata->side == ONEWIRE_LEFT_CHANNEL)
	{
		left_ep_color = pcolordata->color & 0xff;
	}
	else if (pcolordata->side == ONEWIRE_RIGHT_CHANNEL)
	{
		right_ep_color = pcolordata->color & 0xff;
	}
	else
	{
		Log_e(_T("raw data side(%d) error"), pcolordata->side);
		return 0;
	}

	return 1;
}


/*
 * 获取主从耳的耳机颜色
 */
void check_ep_color()
{
	const char ep_color_cmd[] = "05 5A 05 00 00 20 33 00 33";
	int ret;

	left_ep_color = EP_COLOR_INVALID_VALUE;
	right_ep_color = EP_COLOR_INVALID_VALUE;
	
	ret = send_race_cmd_to_partner(ep_color_cmd, process_ep_color_bindata);
	if (ret == TRUE)
	{
		if (ep_side == ONEWIRE_LEFT_CHANNEL)
		{
			Log_d(_T("left color: %s, side=%d"), get_ep_color_string(left_ep_color), ep_side);
		}
		else if (ep_side == ONEWIRE_RIGHT_CHANNEL)
		{
			Log_d(_T("right color: %s, side=%d"), get_ep_color_string(right_ep_color), ep_side);
		}
	}
	else
	{
		Log_e(_T("get partner rawdata failed!\n"));
	}

	ret = send_race_cmd_to_agent(ep_color_cmd, process_ep_color_bindata);
	if (ret == TRUE)
	{
		if (ep_side == ONEWIRE_LEFT_CHANNEL)
		{
			Log_d(_T("left color: %s, side=%d"), get_ep_color_string(left_ep_color), ep_side);
		}
		else if (ep_side == ONEWIRE_RIGHT_CHANNEL)
		{
			Log_d(_T("right color: %s, side=%d"), get_ep_color_string(right_ep_color), ep_side);
		}
	}
	else
	{
		Log_e(_T("get agent rawdata failed!\n"));
	}

	/* 数据传递给主线程 */
	int *data_ptr = DBG_NEW int[2];

	if (!data_ptr)
	{
		Log_e(_T("no memory!"));
		ASSERT(FALSE);
	}

	*data_ptr = left_ep_color;
	*(data_ptr + 1) = right_ep_color;

	dlg_update_status_data(STATE_TWS_EP_COLOR, (void *)data_ptr);
}
