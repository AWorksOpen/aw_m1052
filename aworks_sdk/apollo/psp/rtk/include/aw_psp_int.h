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
 * \brief aworks1.0 中断标准接口头文件
 *
 * \internal
 * \par modification history:
 * - 1.00 12-08-27  zen, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_INT_H
#define __AW_PSP_INT_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "rtk.h"

/******************************************************************************/
/** \brief seek for the right int libraray by CPU, CPU is from apollo.h  */
#define AW_PSP_INT_CPU_LOCK_DECL(key)         RTK_CRITICAL_STATUS_DECL(key)
#define AW_PSP_INT_CPU_LOCK_DECL_STATIC(key)  static RTK_CRITICAL_STATUS_DECL(key)
#define AW_PSP_INT_CPU_LOCK(key)       RTK_ENTER_CRITICAL((key))
#define AW_PSP_INT_CPU_UNLOCK(key)     RTK_EXIT_CRITICAL(key)

/******************************************************************************/
#define AW_PSP_INT_CONTEXT()    rtk_is_int_context()
#define AW_PSP_FAULT_CONTEXT()  rtk_is_fault_context()

#define ENTER_INT_CONTEXT()     RTK_ENTER_INT_CONTEXT()
#define EXIT_INT_CONTEXT()      RTK_EXIT_INT_CONTEXT()

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AW_PSP_INT_H */

/* end of file */
