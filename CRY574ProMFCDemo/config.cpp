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
 * ��ȡ uart.ini ��, ĳ�����е� key ��Ӧ�� string ֵ
 */
FUNC_DLL_EXPORT Boolean get_config_string_value(const TCHAR *segment, const TCHAR *key, const TCHAR *default_value, TCHAR *out_buf, int len)
{
	HMODULE  hHandle;
	TCHAR lpszPath[MAX_PATH];
	DWORD count;
	LPTSTR ptr;

	hHandle = GetModuleHandle(NULL);
	if (!hHandle)
	{
		return FALSE;
	}

	count = GetModuleFileName(hHandle, lpszPath, MAX_PATH);
	if (count == 0)
	{
		return FALSE;
	}

	ptr = _tcsrchr(lpszPath, '\\');
	if (!ptr)
	{
		return FALSE;
	}

	_tcscpy(ptr + 1, _TEXT("config.ini"));

	GetPrivateProfileString(segment, key, default_value, out_buf, len, lpszPath);

	return TRUE;
}


/* 
 * ��ȡ uart.ini ��, ĳ�����е� key ��Ӧ�� integer ֵ
 */
FUNC_DLL_EXPORT Boolean get_config_int_value(const TCHAR *segment, const TCHAR *key, int *pvalue, int default_value)
{
	HMODULE  hHandle;
	TCHAR lpszPath[MAX_PATH];
	DWORD count;
	LPTSTR ptr;

	hHandle = GetModuleHandle(NULL);
	if (!hHandle)
	{
		return FALSE;
	}

	count = GetModuleFileName(hHandle, lpszPath, MAX_PATH);
	if (count == 0)
	{
		return FALSE;
	}

	ptr = _tcsrchr(lpszPath, '\\');
	if (!ptr)
	{
		return FALSE;
	}

	_tcscpy(ptr + 1, _TEXT("uart.ini"));

	*pvalue = GetPrivateProfileInt(segment, key, default_value, lpszPath);

	return TRUE;
}


TCHAR* get_program_path(TCHAR *out_buf, int len)
{
	HMODULE  hHandle;
	TCHAR lpszPath[MAX_PATH];
	DWORD count;
	LPTSTR ptr;
	int n;

	hHandle = GetModuleHandle(NULL);
	if (!hHandle)
	{
		return NULL;
	}

	count = GetModuleFileName(hHandle, lpszPath, MAX_PATH);
	if (count == 0)
	{
		return NULL;
	}

	ptr = _tcsrchr(lpszPath, '\\');
	if (!ptr)
	{
		return NULL;
	}

	*(ptr + 1) = '\0';

	n = (ptr + 1 - lpszPath) / sizeof(TCHAR);

	if (n > len )
	{
		n = len;
	}

	_tcscpy(out_buf, lpszPath);
	
	return &out_buf[n];
}