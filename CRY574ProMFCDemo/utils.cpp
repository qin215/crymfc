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

const TCHAR *setting_name[TEST_NR] =
{
	_T("psensor"),			// 0
	_T("user_mode"),
	_T("software_version"),
	_T("ep_color"),
	_T("psensor_calibrate"),
	NULL,
	NULL,
	NULL,					// 7

	_T("write_anc_gain"),
	_T("factory_reset"),
};



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

	if (out_len < len * 3)
	{
		return -1;
	}

	for (i = 0, index = 0; i < len; i++)
	{
		index += sprintf_s((char *)&outBuff[index], out_len - index, "%02X ", inBuff[i]);
	}

	return index;
}


/*
 * 获取测试设置项
 */
int get_test_item_setting_bitmap()
{
	int value = 0;
	int i;
	int bitmap = 0;

	for (i = 0; i < TEST_NR; i++)
	{
		if (!setting_name[i])
		{
			continue;
		}

		if (!get_config_int_value(_T("setting"), setting_name[i], &value, 1))
		{
			Log_e(_T("Get setting(%s) failed, use default value(%d)"), setting_name[i], 1);
		}
		else
		{
			Log_d(_T("Get setting(%s) ok, use value(%d)"), setting_name[i], value);
		}

		if (value == 1)
		{
			bitmap |= (1 << i);
		}
	}

	return bitmap;
}

/*
 * 获取实际的设置值
 */
int get_test_item_setting_value(const TCHAR *pkey)
{
	int value = 0;

	if (!get_config_int_value(_T("data"), pkey, &value, 1))
	{
		Log_e(_T("Get data(%s) failed, use default value(%d)"), pkey, 1);
	}
	else
	{
		Log_d(_T("Get data(%s) ok, use value(%d)"), pkey, value);
	}

	return value;
}


#define ishex(x)	(((x) >= '0' && (x) <= '9') || ((x) >= 'a' && (x) <= 'f') || ((x) >= 'A' && (x) <= 'F'))
#define isblank(x)   ((x) == ' ' || (x) == '\t')


int myChar2Int( int c) 
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}

	if (c >= 'a' && c <= 'f')
	{
		return 10 + c - 'a';
	}

	if (c >= 'A' && c <= 'F')
	{
		return 10 + c - 'A';
	}

	return 0;
}

int str2bindata(const char *str, char * outBuffer, int buf_len)
{
	int len, i;
	int count;
	int v;
	bool high_quad = true;

	len = strlen(str);
	count = 0;

	for (i = 0; i < len; i++)
	{
		if (!ishex(str[i]))
		{
			if (!high_quad)
			{
				printf("format error!\n");
				return count;
			}

			continue;
		}

		if (high_quad)
		{
			v = myChar2Int(str[i]);
			v <<= 4;
			high_quad = false;
		}
		else
		{
			v |= myChar2Int(str[i]);
			outBuffer[count++] = v & 0xff;

			if (count >= buf_len)
			{
				break;
			}

			high_quad = true;
		}
	}

	
	return count;
}



static char bin_buffer[32];
static char tx_buffer[32];

/*
 * 返回 frame 起始地址
 */
race_cmd_t * init_racecmd_rsp_packet(uint8_t frame, uint16_t cmd)
{
	race_cmd_t *pcmd;

	pcmd = (race_cmd_t *)tx_buffer;

	pcmd->frame_start = RACE_CMD_FRAME_START;
	pcmd->frame_type = frame;
	pcmd->frame_len = sizeof(tx_buffer);
	pcmd->frame_cmd = cmd;

	return pcmd;
}

/*
 * 返回整个包大小
 */
int cons_sw_version_payload_buffer(race_cmd_t *pcmd, const char *str_ver)
{
	race_rsp_sw_version_t *pVersion;

	pVersion = &pcmd->sw_ver_rsp;
	pVersion->status = 1;
	pVersion->role = 1;
	pVersion->recv_count = 0;
	pVersion->str_len = strlen(str_ver);

	if (pcmd->frame_len - 4 - 4 < pVersion->str_len)
	{
		pVersion->status = 0;		// 返回失败，buffer不够
		pVersion->str_len = 0;
	}
	else
	{
		memcpy(pVersion->version, str_ver, pVersion->str_len);
	}

	pcmd->frame_len =  2 + 4 + pVersion->str_len;


	return pcmd->frame_len + 4;
}


int process_racecmd_string(const char *pstr)
{
	char *buff;
	int len = sizeof(bin_buffer);
	race_cmd_t *pcmd;

	buff = bin_buffer;

	len = str2bindata(pstr, buff, len);
	printf("len = %d", len);
	print_buffer_data(buff, len);

	pcmd = (race_cmd_t *)buff;

	switch (pcmd->frame_cmd)
	{
	case RACE_CMD_GET_SW_VERSION:
		{
			int pack_len;
			race_cmd_t *pcmd;

			printf("get sw version!\n");

			pcmd = init_racecmd_rsp_packet(RACE_CMD_RSP, RACE_CMD_GET_SW_VERSION);
			pack_len = cons_sw_version_payload_buffer(pcmd, "2.2");

			break;
		}

	case CUSTOMER_RACE_CMD:
		{

			break;
		}

	default:
		{
			printf("cmd=%x!\n", pcmd->frame_cmd);
			break;
		}
	}

	return 0;
}


void test0()
{
	const char *hex = "  05 5a 04 00 07 1c 01 ff   ";
	char bin[32];
	int len;
	race_cmd_t *pcmd;

	len = str2bindata(hex, bin, sizeof(bin));
	printf("len = %d", len);

	print_buffer_data(bin, len);

	pcmd = (race_cmd_t *)bin;

	if (pcmd->frame_cmd == RACE_CMD_GET_SW_VERSION)
	{
		printf("get sw version!\n");
	}
	else
	{
		printf("no sw version!\n");
	}

	process_racecmd_string(hex);

}
