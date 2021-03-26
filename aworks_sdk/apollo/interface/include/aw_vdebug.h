/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief 可视化调试组件
 *
 * \note  该组件打印通过串口输出
 *
 * 使用本服务需要包含头文件
 * \code
 * #include aw_vdebug.h
 * \endcode
 *
 * \par 简单示例
 * \code
 *  #include "aworks.h"
 *  #include "aw_vdebug.h"
 *
 *  int age = 18;
 *  aw_kprintf("I'm %d years old!\n", age);
 *  AW_INFOF(("*Info: Hello World!\n"));
 *  AW_WARNF(("*Wanf: Hello World!\n"));
 *  AW_ERRF(("*ERR: Hello World!\n"));
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-23  orz, first implementation
 * \endinternal
 */

#ifndef __AW_VDEBUG_H
#define __AW_VDEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdlib.h>

/**
 * \addtogroup grp_aw_if_vdebug
 * \copydoc aw_vdebug.h
 * @{
 */

/*******************************************************************************
    debug macro defines
*******************************************************************************/

/** \brief 一般用于 logo 打印输出, 使用前需要定义 AW_VDEBUG, 若想屏蔽输出, 
 *         则只需关闭AW_VDEBUG 定义即可
 */
#ifdef AW_VDEBUG
#define AW_LOGF(msg)    (void)aw_kprintf msg
#else
#define AW_LOGF(msg)
#endif

/** \brief 一般用于info 打印 输出, 使用前需要定义 AW_VDEBUG_INFO, 若想屏蔽输出,
 *         则只需关闭AW_VDEBUG_INFO 定义即可
 */
#ifdef AW_VDEBUG_INFO
#define AW_INFOF(info)  AW_LOGF(info)
#else
#define AW_INFOF(info)
#endif

/** \brief 一般用于调试打印输出 , 使用前需要定义 AW_VDEBUG_DEBUG, 若想屏蔽输出, 
 *         则只需关闭AW_VDEBUG_DEBUG 定义即可
 */
#ifdef AW_VDEBUG_DEBUG
#define AW_DBGF(info)   AW_LOGF(info)
#else
#define AW_DBGF(info)
#endif

/** \brief 一般用于警告打印输出 , 使用前需要定义 AW_VDEBUG_WARN, 若想屏蔽输出, 
 *         则只需关闭AW_VDEBUG_WARN 定义即可
 */
#ifdef AW_VDEBUG_WARN
#define AW_WARNF(info)  AW_LOGF(info)
#else
#define AW_WARNF(info)
#endif

/** \brief 一般用于错误打印输出 , 使用前需要定义 AW_VDEBUG_ERROR, 若想屏蔽输出, 
 *         则只需关闭AW_VDEBUG_ERROR 定义即可
 */
#ifdef AW_VDEBUG_ERROR
#define AW_ERRF(info)   AW_LOGF(info)
#else
#define AW_ERRF(info)
#endif

/**
 * \brief 将可变参数格式化字符串到一个指定长度的字符串缓冲区中
 *
 * \param[in] buf       保存格式化字符串的缓冲区
 * \param[in] sz        输出字符串数组大小
 * \param[in] fmt       格式化字符串
 * \param[in] args      可变参数函数列表
 *
 * \retval 字符串长度
 *
 * \par 示例
 * \code
 *  #include "aworks.h"
 *  #include "aw_vdebug.h"
 *
 *  aw_local void  __my_printf(const char *format, ...)
 *  {
 *      char  buf[128];
 *      int   len = 0;
 *
 *      va_list  vlist;
 *
 *      va_start(vlist, format);
 *      len = aw_vsnprintf(buf, sizeof(buf), format, vlist);
 *      va_end(vlist);
 *
 *      aw_kprintf("%s", buf);
 *  }
 * \endcode
 */
int aw_vsnprintf (char *buf, size_t sz, const char *fmt, va_list args);

/**
 * \brief 使用aw_mem_alloc申请缓冲区并格式化字符串到一个指定长度的字符串缓冲区中
 *
 * \param[in] pp_buffer     保存格式化字符串的缓冲区
 * \param[in] fmt           格式化字符串
 * \param[in] ...           格式化字符串的参数列表
 *
 * \retval 字符串长度
 *
 * \par 示例
 * \code
 *  #include "aworks.h"
 *  #include "aw_vdebug.h"
 *  #include "aw_mem.h"
 *
 *  char *str = NULL;
 *  int   len = 0;
 *  len = aw_asprintf(&str, "I'm %d years old!\n", 18);
 *  if ( str ) {
 *      aw_kprintf( "%s", str );
 *      aw_mem_free( str );
 *  }
 * \endcode
 */
int aw_asprintf(char **pp_buffer, const char *fmt, ...);

/**
 * \brief 将可变参数格式化输出到一个动态申请的字符串数组中
 *
 * \param[in,out] pp_buffer 保存格式化字符串的缓冲区地址
 * \param[in]     fmt       格式化字符串
 * \param[in]     args      可变参数函数列表
 *
 * \retval 字符串长度
 *
 * \par 示例
 * \code
 *  #include "aworks.h"
 *  #include "aw_vdebug.h"
 *  #include "aw_mem.h"
 *
 *  aw_local void  __my_printf(const char *format, ...)
 *  {
 *      char *buf = NULL;
 *      int   len = 0;
 *
 *      va_list  vlist;
 *
 *      va_start(vlist, format);
 *      len = aw_vasprintf(&buf, format, vlist);
 *      va_end(vlist);
 *
 *      if (buf != NULL) {
 *          aw_kprintf( "%s", str );
 *          aw_mem_free( buf );
 *      }
 *
 *      aw_kprintf("%s", buf);
 *  }
 * \endcode
 */
int aw_vasprintf(char **pp_buffer, const char *fmt, va_list args);

/**
 * \brief 格式化字符串到一个指定长度的字符串缓冲区中
 *
 * \param[in] buf   保存格式化字符串的缓冲区
 * \param[in] sz    缓冲区大小
 * \param[in] fmt   格式化字符串
 * \param[in] ...   格式化字符串的参数列表
 *
 * \retval number of char put into buffer
 *
 * \par 示例
 * \code
 *  #include "aworks.h"
 *  #include "aw_vdebug.h"
 *  #include "aw_serial.h"
 *  #include <string.h>
 *
 *  char buf[64];
 *  int age = 18;
 *  aw_snprintf(&buf[0], sizeof(buf), "I'm %d years old!\n", age);
 *  aw_serial_write(COM1, &buf[0], strlen(buf));
 * \endcode
 */
#ifndef AW_SNPRINTF_DEFINED
extern int aw_snprintf (char *buf, size_t sz, const char *fmt, ...);
#endif  /* AW_SNPRINTF_DEFINED */

/**
 * \brief 内核格式化打印函数
 *
 * 本函数将格式化字符串输出到当前的控制台。
 *
 * \param[in] fmt 格式化字符串
 * \param[in] ... 格式化字符串的参数列表
 *
 * \retval 输出到控制台的字符个数
 *
 * \par 示例
 * \code
 *  #include "aworks.h"
 *  #include "aw_vdebug.h"
 *
 *  int age = 18;
 *  aw_kprintf("I'm %d years old!\n", age);
 * \endcode
 */
#ifndef AW_KPRINTF_DEFINED
extern int aw_kprintf (const char *fmt, ...);
#endif  /* AW_KPRINTF_DEFINED */

/**
 * \brief 初始化一个输出打印设备
 *
 * \param[in]   fil     输出句柄
 * \param[in]   f_putc  输出字符回调接口
 * \param[in]   f_puts  输出字符串回调接口
 * \param[in]   str     格式化字符串
 * \param[in]   arp     可变参数函数列表
 *
 * \retval 字符串长度
 */
extern int aw_vfprintf_do (void       *fil,
                           int       (*f_putc) (const char  c, void *fil),
                           int       (*f_puts) (const char *s, void *fil),
                           const char *str,
                           va_list     arp);

/* @} grp_aw_if_vdebug */

#ifdef __cplusplus
}
#endif

#endif /* __AW_VDEBUG_H */

/* end of file */
