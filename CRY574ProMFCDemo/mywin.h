#ifndef _MYWIN_H_
#define _MYWIN_H_

#ifdef WIN32
#include <tchar.h>
#include "windows.h"
#include <BaseTsd.h>
#else
#define TCHAR char
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include "qcloud_iot_export_log.h"

#ifndef Boolean
#define Boolean BOOL
#endif

//#define DLL_EXPORT 1

#ifdef DLL_EXPORT
#ifdef __cplusplus
#define FUNC_DLL_EXPORT extern "C" __declspec(dllexport) 
#else
#define FUNC_DLL_EXPORT __declspec(dllexport) 
#endif

#else
#define FUNC_DLL_EXPORT
#endif


// ����ͨ�ÿ�汾,����и����������汾��
#define JX_COMMON_LIB_VERSION	"1.6"

/* 
 * ��ȡ uart.ini ��, ĳ�����е� key ��Ӧ�� string ֵ
 */
FUNC_DLL_EXPORT Boolean get_config_string_value(const TCHAR *segment, const TCHAR *key, const TCHAR *default_value, TCHAR *out_buf, int len);

/* 
 * ��ȡ uart.ini ��, ĳ�����е� key ��Ӧ�� integer ֵ
 */
FUNC_DLL_EXPORT Boolean get_config_int_value(const TCHAR *segment, const TCHAR *key, int *pvalue, int default_value);

/* 
 * ��ȡ��ǰ����·��
 */
TCHAR* get_program_path(TCHAR *out_buf, int len);

/*
 * ��cmd console����,printf����ӡ���˴�
 */
FUNC_DLL_EXPORT void enable_console_window();

/*
 * ���ļ���¼log
 */
FUNC_DLL_EXPORT void enable_log_file();

void close_logfile();

/*
 * �ж�Ŀ¼�Ƿ���ڣ���������ڣ������������ͬ���ļ������ڣ���ɾ���ļ�������ͬ��Ŀ¼
 */
Boolean ensure_directory_exist(LPCTSTR path);

TCHAR* HAL_get_current_dt_filename(void);


#ifdef __cplusplus
}
#endif

#endif
