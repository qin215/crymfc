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
 * 分配内存
 */
void *HAL_Malloc(_IN_ size_t size)
{
    return malloc(size);
}

/*
 * 释放内存
 */
void HAL_Free(_IN_ void *ptr)
{
    free(ptr);
}

/*
 * 格式化打印
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
 * 安全格式化打印
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
 * 打印当前日期和时间的缓冲区
 */
static TCHAR now_time_str[32] = {0};

/*
 * 返回当前的日期和时间buffer, 添加毫秒支持
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
 * 从字串s尾部匹配str字串
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
 * 打开cmd console窗口,printf语句打印到此处
 */
FUNC_DLL_EXPORT void enable_console_window()
{
	AllocConsole();
	freopen( "CONOUT$","w",stdout);
	printf("hello");			// for testing.
}


/*
 * 判断目录是否存在，如果不存在，则建立；如果相同的文件名存在，则删除文件，建立同名目录
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
		/*每行分两大列显示，每大列显示8个字节，每行显示16个字节*/
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


/*打印出报文*/
void sz_print_pkt(char *str, unsigned char *data, int dataLen)
{
	printf("%s", str);
	sz_print_data_buf(data, dataLen);
}

/*
 * 以当前时间戳生成文件后缀名
 */
TCHAR* HAL_get_current_dt_filename(void) 
{
	SYSTEMTIME tmSys;
	GetLocalTime(&tmSys);

	_stprintf(now_time_str, _T("%d-%02d-%02d_%02d_%02d_%02d "), tmSys.wYear, tmSys.wMonth, tmSys.wDay, 
		tmSys.wHour, tmSys.wMinute, tmSys.wSecond);

	return now_time_str;
}
