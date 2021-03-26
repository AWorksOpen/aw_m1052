/** 
 * 功能说明：
 *     1.基本类型定义。
 *
 * 修改历史：
 *     1.2017-3-18 李先静<lixianjing@zlg.cn> 创建。
 */

#ifndef TYPES_DEF_H
#define TYPES_DEF_H 

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>

#if defined(WIN32) || defined(DEVICE_BUILD)
typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
#else
#ifndef _INT8_T_DECLARED
typedef __int8_t int8_t;
#define _INT8_T_DECLARED
#endif
#ifndef _UINT8_T_DECLARED
typedef __uint8_t uint8_t;
#define _UINT8_T_DECLARED
#include <stdint.h>
#endif

#ifndef _INT16_T_DECLARED
typedef __int16_t int16_t;
#define _INT16_T_DECLARED
#endif
#ifndef _UINT16_T_DECLARED
typedef __uint16_t uint16_t;
#define _UINT16_T_DECLARED
#endif

#ifndef _INT32_T_DECLARED
typedef __int32_t int32_t;
#define _INT32_T_DECLARED
#endif
#ifndef _UINT32_T_DECLARED
typedef __uint32_t uint32_t;
#define _UINT32_T_DECLARED
#endif
#endif//WIN32

#ifdef __cplusplus
#   define BEGIN_C_DECLS extern "C" {
#   define END_C_DECLS }
#else
#   define BEGIN_C_DECLS
#   define END_C_DECLS
#endif

#define public_for_test public

#ifdef WIN32 
#   include <Windows.h>
#   include <direct.h>
#   define socket_t SOCKET
#   define DIRECTORY_SEPARATOR "\\"
#   define DIRECTORY_SEPARATOR_CHAR '\\'
#	define getcwd _getcwd
#	define unlink _unlink
#   define snprintf _snprintf
#   define mkdir(s,p) _mkdir(s)
#	define strcasecmp _stricmp
#   define msleep(ms) Sleep(ms)
#   define inline 
#   define __func__ __FILE__
#else
#   define socket_t int
#   include <unistd.h>
#   include <sys/time.h>
#   include <sys/stat.h>
#   define DIRECTORY_SEPARATOR "/"
#   define DIRECTORY_SEPARATOR_CHAR '/'
#   define msleep(ms) usleep(1000*ms)
#endif

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE 
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifdef NDEBUG
#   define return_if_fail(p) if(!(p)) {return;}
#   define return_value_if_fail(p, value) if(!(p)) {return (value);}
#elif defined(LogWarn)
#   define return_if_fail(p) if(!(p)) \
    {LogWarn("%s:%d "#p"\n", __FUNCTION__, __LINE__); return;}
#   define return_value_if_fail(p, value) if(!(p)) \
    {LogWarn("%s:%d "#p"\n", __FUNCTION__, __LINE__); return (value);}
#else
#   define return_if_fail(p) if(!(p)) \
    {printf("%s:%d "#p"\n", __FUNCTION__, __LINE__); return;}
#   define return_value_if_fail(p, value) if(!(p)) \
    {printf("%s:%d "#p"\n", __FUNCTION__, __LINE__); return (value);}
#endif


#endif

