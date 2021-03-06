#ifndef QCLOUD_IOT_EXPORT_LOG_H_
#define QCLOUD_IOT_EXPORT_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#define _IN_           
#define _OU_     

#ifndef Boolean
#define Boolean BOOL
#endif

/**
 * 日志输出等级
 */
typedef enum 
{
    QC_DEBUG,
    QC_INFO,
    QC_WARN,
    QC_ERROR
} QC_LOG_LEVEL;

/**
 * 全局日志级别输出标志, 只输出小于或等于该等级的日志信息
 */
extern QC_LOG_LEVEL g_log_level;

typedef Boolean (*LogMessageHandler)(LPCTSTR message);

/**
 * @brief
 *
 * @param
 */
void IOT_Log_Set_Level(QC_LOG_LEVEL level);

/**
 * @brief 获取当前日志等级
 *
 * @return
 */
QC_LOG_LEVEL IOT_Log_Get_Level();

/**
 * @brief 设置日志回调函数，用户接管日志内容用于输出到文件等操作
 *
 * @param handler 回调函数指针
 *
 */
void IOT_Log_Set_MessageHandler(LogMessageHandler handler);

/**
 * @brief 日志打印函数，默认打印到标准输出，当用户设置日志打印handler时，回调handler
 *
 * @param file 源文件名
 * @param func 函数名
 * @param line 行号
 * @param level 日志等级
 */
void Log_writter(LPCSTR file, LPCSTR func, const int line, const int level, LPCTSTR fmt, ...);

#define Log_d(args, ...) Log_writter(__FILE__, __FUNCTION__, __LINE__, QC_DEBUG, args, ##__VA_ARGS__)
#define Log_i(args, ...) Log_writter(__FILE__, __FUNCTION__, __LINE__, QC_INFO, args, ##__VA_ARGS__)
#define Log_w(args, ...) Log_writter(__FILE__, __FUNCTION__, __LINE__, QC_WARN, args, ##__VA_ARGS__)
#define Log_e(args, ...) Log_writter(__FILE__, __FUNCTION__, __LINE__, QC_ERROR, args, ##__VA_ARGS__)

#ifdef IOT_DEBUG
	#define IOT_FUNC_ENTRY    \
		{\
		printf("FUNC_ENTRY:   %s L#%d \n", __FUNCTION__, __LINE__);  \
		}
	#define IOT_FUNC_EXIT    \
		{\
		printf("FUNC_EXIT:   %s L#%d \n", __FUNCTION__, __LINE__);  \
		return;\
		}
	#define IOT_FUNC_EXIT_RC(x)    \
		{\
		printf("FUNC_EXIT:   %s L#%d Return Code : %ld \n", __FUNCTION__, __LINE__, (long)(x));  \
		return x; \
		}
#else
	#define IOT_FUNC_ENTRY
	#define IOT_FUNC_EXIT 			\
		{\
			return;\
		}
	#define IOT_FUNC_EXIT_RC(x)     \
		{\
			return x; \
		}
#endif

//
void HAL_Printf(_IN_ LPCTSTR format, ...);

//
int HAL_Snprintf(_IN_ TCHAR *str, const int len, LPCTSTR fmt, ...);
 
//
TCHAR* HAL_Timer_current(void) ;

/*
 * 自定义记录log函数
 */
Boolean MyLogMessageHandler(LPCTSTR message);

void print_buffer_data_help(void *buff, UINT32 len, int line);

#define print_buffer_data(x, y) print_buffer_data_help(x, y, __LINE__)

#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_LOG_H_ */
