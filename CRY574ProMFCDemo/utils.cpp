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
	NULL,
	NULL,
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