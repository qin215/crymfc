#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <direct.h>
#include <sys/stat.h>
#include <errno.h>

#include "mywin.h"
#include "qcloud_iot_export_log.h"

/*
 * �����ڴ�
 */
void *HAL_Malloc(_IN_ size_t size)
{
    return malloc(size);
}

/*
 * �ͷ��ڴ�
 */
void HAL_Free(_IN_ void *ptr)
{
    free(ptr);
}

/*
 * ��ʽ����ӡ
 */
void HAL_Printf(_IN_ LPCTSTR fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    _vtprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}

/*
 * ��ȫ��ʽ����ӡ
 */
int HAL_Snprintf(_IN_ TCHAR *str, const int len, LPCTSTR fmt, ...)
{
    va_list args;
    int rc;

    va_start(args, fmt);
    rc = _vsntprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

/*
 * ��ӡ��ǰ���ں�ʱ��Ļ�����
 */
static TCHAR now_time_str[32] = {0};

/*
 * ���ص�ǰ�����ں�ʱ��buffer, ��Ӻ���֧��
 */
TCHAR* HAL_Timer_current(void) 
{
	struct tm *newtime;
	char am_pm[] = "AM";
	__time64_t long_time;

	SYSTEMTIME tmSys;
	GetLocalTime(&tmSys);

	_stprintf(now_time_str,_T("%d-%02d-%02d %02d:%02d:%02d.%03d "), tmSys.wYear, tmSys.wMonth, tmSys.wDay, 
		tmSys.wHour, tmSys.wMinute, tmSys.wSecond, tmSys.wMilliseconds);

	return now_time_str;
}

/*
 * ���ִ�sβ��ƥ��str�ִ�
 */
char *strrstr(char *s, char *str)
{
	char *p; 
	int len = strlen(s);
	int n = strlen(str);

	for (p = s + len - 1; p >= s; p--) 
	{
		if ((*p == *str) && (memcmp(p, str, n) == 0)) 
		{
			return p;
		}
	}

	return NULL;
}

/*
 * ��cmd console����,printf����ӡ���˴�
 */
FUNC_DLL_EXPORT void enable_console_window()
{
	AllocConsole();
	freopen( "CONOUT$","w",stdout);
	printf("hello");			// for testing.
}


/*
 * �ж�Ŀ¼�Ƿ���ڣ���������ڣ������������ͬ���ļ������ڣ���ɾ���ļ�������ͬ��Ŀ¼
 */
Boolean ensure_directory_exist(LPCTSTR  path)
{
	CFileFind ff;

	CString dirpath(path);
	dirpath += _T("\\*.*");
	if (!ff.FindFile(dirpath))
	{
		CreateDirectory(path, NULL);
	}

	return TRUE;
}


static void sz_print_data_buf(unsigned char *data, int dataLen)
{
	int i = 0;

	for (; i < dataLen; i++)
	{
		/*ÿ�з���������ʾ��ÿ������ʾ8���ֽڣ�ÿ����ʾ16���ֽ�*/
		if (0 == i)
		{
			printf(" %02x ", data[i] & 0xff);
			continue;
		}
		else if (0 == (i % (8 * 2)))
		{
			printf("\n ");
		}
		else if (0 == (i % 8))
		{
			printf("\t");
		}
		else
		{
			/*none*/
		}

		printf("%02x ", data[i] & 0xff);
	}

	printf("\n");
}


/*��ӡ������*/
void sz_print_pkt(char *str, unsigned char *data, int dataLen)
{
	printf("%s", str);
	sz_print_data_buf(data, dataLen);
}

/*
 * �Ե�ǰʱ��������ļ���׺��
 */
TCHAR* HAL_get_current_dt_filename(void) 
{
	SYSTEMTIME tmSys;
	GetLocalTime(&tmSys);

	_stprintf(now_time_str, _T("%d-%02d-%02d_%02d_%02d_%02d "), tmSys.wYear, tmSys.wMonth, tmSys.wDay, 
		tmSys.wHour, tmSys.wMinute, tmSys.wSecond);

	return now_time_str;
}
