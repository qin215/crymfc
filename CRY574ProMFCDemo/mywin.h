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


// 集贤通用库版本,如果有更改请升级版本号
#define JX_COMMON_LIB_VERSION	"1.6"

/* 
 * 获取 uart.ini 中, 某个段中的 key 对应的 string 值
 */
FUNC_DLL_EXPORT Boolean get_config_string_value(const TCHAR *segment, const TCHAR *key, const TCHAR *default_value, TCHAR *out_buf, int len);

/* 
 * 获取 uart.ini 中, 某个段中的 key 对应的 integer 值
 */
FUNC_DLL_EXPORT Boolean get_config_int_value(const TCHAR *segment, const TCHAR *key, int *pvalue, int default_value);

/* 
 * 获取当前程序路径
 */
TCHAR* get_program_path(TCHAR *out_buf, int len);

/*
 * 打开cmd console窗口,printf语句打印到此处
 */
FUNC_DLL_EXPORT void enable_console_window();

/*
 * 打开文件记录log
 */
FUNC_DLL_EXPORT void enable_log_file();

void close_logfile();

/*
 * 判断目录是否存在，如果不存在，则建立；如果相同的文件名存在，则删除文件，建立同名目录
 */
Boolean ensure_directory_exist(LPCTSTR path);

TCHAR* HAL_get_current_dt_filename(void);


#ifdef __cplusplus
}
#endif

#endif
