/*******************************************************************************
*                                 AWORKS
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Modbus编译相关头文件
 *
 * \internal
 * \par modification history]
 * - 1.02 2015-05-21  cod,  fix code style and comments, a little improve.
 * - 1.01 2012-12-04  liuweiyun, Modify for aworks1.0, fix code style
 * - 1.00 2012-05-13  ygy, first implementation
 * \endinternal
 */

#ifndef __AW_MB_COMILER_H /* { */
#define __AW_MB_COMILER_H

/**
 * \addtogroup grp_aw_if_mb_compiler
 * \copydoc aw_mb_compiler.h
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_common.h"
#include "aw_assert.h"
#include "aw_types.h"
#include "string.h"    /* for memcpy() and memset() */

#define AW_MB_CPU_BIG_ENDIAN  0   /**< \brief CPU为小端格式 */

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_compiler */

#endif /* __AW_MB_COMILER_H */
/* end of file */
