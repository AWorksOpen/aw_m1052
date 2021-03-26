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
 * \brief kernel output interface
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-23  orz, first implementation
 * \endinternal
 */

#ifndef __AW_K_OUTPUT_H
#define __AW_K_OUTPUT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name aw_koutput
 * @{
 */

extern void  *gp_kout_file;
extern int  (*gpfunc_kputc)(const char  c, void *fil);
extern int  (*gpfunc_kputs)(const char *s, void *fil);

void aw_koutput_set (void  *fil,
                     int  (*f_putc) (const char  c, void *fil),
                     int  (*f_puts) (const char *s, void *fil));

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* __AW_K_OUTPUT_H */

/* end of file */
