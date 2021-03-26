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
 * \brief ���ӻ��������
 *
 * \note  �������ӡͨ���������
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ�
 * \code
 * #include aw_vdebug.h
 * \endcode
 *
 * \par ��ʾ��
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
 * // �������ݴ���ӡ�����
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

/** \brief һ������ logo ��ӡ���, ʹ��ǰ��Ҫ���� AW_VDEBUG, �����������, 
 *         ��ֻ��ر�AW_VDEBUG ���弴��
 */
#ifdef AW_VDEBUG
#define AW_LOGF(msg)    (void)aw_kprintf msg
#else
#define AW_LOGF(msg)
#endif

/** \brief һ������info ��ӡ ���, ʹ��ǰ��Ҫ���� AW_VDEBUG_INFO, �����������,
 *         ��ֻ��ر�AW_VDEBUG_INFO ���弴��
 */
#ifdef AW_VDEBUG_INFO
#define AW_INFOF(info)  AW_LOGF(info)
#else
#define AW_INFOF(info)
#endif

/** \brief һ�����ڵ��Դ�ӡ��� , ʹ��ǰ��Ҫ���� AW_VDEBUG_DEBUG, �����������, 
 *         ��ֻ��ر�AW_VDEBUG_DEBUG ���弴��
 */
#ifdef AW_VDEBUG_DEBUG
#define AW_DBGF(info)   AW_LOGF(info)
#else
#define AW_DBGF(info)
#endif

/** \brief һ�����ھ����ӡ��� , ʹ��ǰ��Ҫ���� AW_VDEBUG_WARN, �����������, 
 *         ��ֻ��ر�AW_VDEBUG_WARN ���弴��
 */
#ifdef AW_VDEBUG_WARN
#define AW_WARNF(info)  AW_LOGF(info)
#else
#define AW_WARNF(info)
#endif

/** \brief һ�����ڴ����ӡ��� , ʹ��ǰ��Ҫ���� AW_VDEBUG_ERROR, �����������, 
 *         ��ֻ��ر�AW_VDEBUG_ERROR ���弴��
 */
#ifdef AW_VDEBUG_ERROR
#define AW_ERRF(info)   AW_LOGF(info)
#else
#define AW_ERRF(info)
#endif

/**
 * \brief ���ɱ������ʽ���ַ�����һ��ָ�����ȵ��ַ�����������
 *
 * \param[in] buf       �����ʽ���ַ����Ļ�����
 * \param[in] sz        ����ַ��������С
 * \param[in] fmt       ��ʽ���ַ���
 * \param[in] args      �ɱ���������б�
 *
 * \retval �ַ�������
 *
 * \par ʾ��
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
 * \brief ʹ��aw_mem_alloc���뻺��������ʽ���ַ�����һ��ָ�����ȵ��ַ�����������
 *
 * \param[in] pp_buffer     �����ʽ���ַ����Ļ�����
 * \param[in] fmt           ��ʽ���ַ���
 * \param[in] ...           ��ʽ���ַ����Ĳ����б�
 *
 * \retval �ַ�������
 *
 * \par ʾ��
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
 * \brief ���ɱ������ʽ�������һ����̬������ַ���������
 *
 * \param[in,out] pp_buffer �����ʽ���ַ����Ļ�������ַ
 * \param[in]     fmt       ��ʽ���ַ���
 * \param[in]     args      �ɱ���������б�
 *
 * \retval �ַ�������
 *
 * \par ʾ��
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
 * \brief ��ʽ���ַ�����һ��ָ�����ȵ��ַ�����������
 *
 * \param[in] buf   �����ʽ���ַ����Ļ�����
 * \param[in] sz    ��������С
 * \param[in] fmt   ��ʽ���ַ���
 * \param[in] ...   ��ʽ���ַ����Ĳ����б�
 *
 * \retval number of char put into buffer
 *
 * \par ʾ��
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
 * \brief �ں˸�ʽ����ӡ����
 *
 * ����������ʽ���ַ����������ǰ�Ŀ���̨��
 *
 * \param[in] fmt ��ʽ���ַ���
 * \param[in] ... ��ʽ���ַ����Ĳ����б�
 *
 * \retval ���������̨���ַ�����
 *
 * \par ʾ��
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
 * \brief ��ʼ��һ�������ӡ�豸
 *
 * \param[in]   fil     ������
 * \param[in]   f_putc  ����ַ��ص��ӿ�
 * \param[in]   f_puts  ����ַ����ص��ӿ�
 * \param[in]   str     ��ʽ���ַ���
 * \param[in]   arp     �ɱ���������б�
 *
 * \retval �ַ�������
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
