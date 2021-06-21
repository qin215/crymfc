#include "stdafx.h"
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>

#include "mywin.h"

CFile log_file;

#define MAX_LOG_FILESIZE	(8 * 1024 * 1024) 			// 300k
#define MAX_CHECK_TIMEOUT	(60 * 1000)					// 1 Minute

static TCHAR log_filename[128];
static int filename_index;

static int get_log_filesize();
static void check_logfile();

#define LOG_DIR	_T("log\\")

static LogMessageHandler sg_log_message_handler = NULL;


void IOT_Log_Set_MessageHandler(LogMessageHandler handler) 
{
	sg_log_message_handler = handler;
}

#define MAX_LOG_MSG_LEN  1024

const TCHAR *level_str[] =
{
	_T("QC_DEBUG"),
	_T("QC_INFO"),
	_T("QC_WARN"),
	_T("QC_ERROR")
};

void Log_writter(LPCSTR file, LPCSTR func, const int line, const int level, LPCTSTR fmt, ...)
{
	va_list ap;
	TCHAR tfile[128];
	TCHAR tfunc[128];

	if (level < g_log_level) 
	{
		return;
	}

	MultiByteToWideChar(CP_ACP, 0, file, -1, tfile, sizeof(tfile) / sizeof(TCHAR));
	MultiByteToWideChar(CP_ACP, 0, func, -1, tfunc, sizeof(tfunc) / sizeof(TCHAR));

	if (sg_log_message_handler) 
	{
		TCHAR sg_text_buf[MAX_LOG_MSG_LEN + 1];
		TCHAR *tmp_buf = sg_text_buf;
		TCHAR *o = tmp_buf;
		memset(tmp_buf, 0, sizeof(sg_text_buf));

		o += HAL_Snprintf(o, sizeof(sg_text_buf), _T("%s|%s|%s(%d): "), level_str[level], HAL_Timer_current(), tfunc, line);

		va_start(ap, fmt);
		o += _vsntprintf(o, MAX_LOG_MSG_LEN - 2 - lstrlen(tmp_buf), fmt, ap);
		va_end(ap);

		lstrcat(tmp_buf, _T("\n"));

		if (sg_log_message_handler(tmp_buf)) 
		{
			return;
		}
	}

	HAL_Printf(_T("%s|%s|%s(%d): "), level_str[level], HAL_Timer_current(), tfunc, line);

	va_start(ap, fmt);
	_vtprintf(fmt, ap);
	va_end(ap);

	HAL_Printf(_T("\r\n"));
}


/*
 * 获取当前时间戳，ms单位
 */
__int64 get_current_timestamp_ms()
{
	SYSTEMTIME tmSys;

	GetLocalTime(&tmSys);
	CTime tm3(tmSys);

	__int64 tmDst = __int64(tm3.GetTime())*1000 + tmSys.wMilliseconds;

	return tmDst;
}

CString get_file_name(void)
{
	CString path;
	TCHAR szFileFullName[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDriver[_MAX_DRIVE];
	TCHAR szTempPathName[_MAX_PATH];

	GetModuleFileName(NULL, szTempPathName,_MAX_PATH);
	_wsplitpath(szTempPathName, szDriver, szDir, NULL, NULL);
	_wmakepath(szFileFullName, szDriver, szDir, NULL, NULL);

	path = szFileFullName;
	//end add
	path += LOG_DIR;

	ensure_directory_exist(path);
	
	CString filename;

	filename.Format(_T("\\log_%s.txt"), HAL_get_current_dt_filename());

	path += filename;

	return path;
}


/*
 * log 回调函数
 */
Boolean MyLogMessageHandler(LPCTSTR message)
{
	int len;
	static __int64 timestamp = 0;
	__int64 tmp;
	static int count;
	char tmpbuf[1024];

	try 
	{
		if (log_file.m_hFile != CFile::hFileNull)
		{
			len = lstrlen(message);
			len = WideCharToMultiByte(CP_ACP, 0, message, len, tmpbuf, sizeof(tmpbuf), NULL, NULL);
			log_file.Write(tmpbuf, len);
			log_file.Flush();
		}
		else
		{
			CString filepath = get_file_name();
			log_file.Open(filepath,  CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);

			
			len = lstrlen(message);
			len = WideCharToMultiByte(CP_ACP, 0, message, len, tmpbuf, sizeof(tmpbuf), NULL, NULL);
			log_file.Write(tmpbuf, len);
			log_file.Flush();
		}

	} catch (CFileException e)
	{
		printf("open logfile exception!");
		ASSERT(FALSE);
	}
	
	count = 0;
	tmp = get_current_timestamp_ms();

	if (timestamp == 0)
	{
		timestamp = tmp;
	}
	else
	{
		if (tmp - timestamp >= MAX_CHECK_TIMEOUT)	// 超时检测
		{
			check_logfile();
			timestamp = tmp;
		}
	}

	return FALSE;
}


/*
 * 获取当前log file文件大小
 */
static int get_log_filesize()
{
	return (int)(log_file.GetLength());
}


/*
 * 关闭logfile
 */
void close_logfile()
{
	try 
	{
		if (log_file.m_hFile != CFile::hFileNull)
		{
			log_file.Close();
		}
	}
	catch (CFileException e)
	{
		printf("close logfile exception!");
	}
	
}

/*
 * 检查logfile 大小, 如果大于预定的log大小，则关闭
 */
static void check_logfile()
{
	int filesize;

	filesize = get_log_filesize();
	printf("filesize=%d\n", filesize);
	
	if (filesize >= MAX_LOG_FILESIZE)
	{
		close_logfile();
		filename_index++;
	}
}

 QC_LOG_LEVEL g_log_level;

/*
 * 打开文件记录log
 */
FUNC_DLL_EXPORT void enable_log_file()
{
	IOT_Log_Set_MessageHandler(MyLogMessageHandler);
	IOT_Log_Set_Level(QC_DEBUG);
}

void IOT_Log_Set_Level(QC_LOG_LEVEL level)
{
	g_log_level = level;
}
