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

#define CONFIG_FILE_NAME _T("config.ini")

/* 
 * 获取 uart.ini 中, 某个段中的 key 对应的 string 值
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

	_tcscpy(ptr + 1, CONFIG_FILE_NAME);

	GetPrivateProfileString(segment, key, default_value, out_buf, len, lpszPath);

	return TRUE;
}


/* 
 * 获取 uart.ini 中, 某个段中的 key 对应的 integer 值
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

	_tcscpy(ptr + 1, CONFIG_FILE_NAME);

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



/* 
 * 获取 uart.ini 中, 某个段中的 key 对应的 integer 值
 */
FUNC_DLL_EXPORT Boolean get_config_int_value_v2(const TCHAR *filename, const TCHAR *segment, const TCHAR *key, int *pvalue, int default_value)
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

	_tcscpy(ptr + 1, filename);

	*pvalue = GetPrivateProfileInt(segment, key, default_value, lpszPath);

	return TRUE;
}


FUNC_DLL_EXPORT Boolean set_config_int_value_v2(const TCHAR *filename, const TCHAR *segment, const TCHAR *key, int *pvalue, int default_value)
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

	_tcscpy(ptr + 1, filename);

	*pvalue = GetPrivateProfileInt(segment, key, default_value, lpszPath);

	return TRUE;
}