/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-24  win, first implementation
 * \endinternal
 */

#ifndef __AWSA_SOC_INTERNAL_H
#define __AWSA_SOC_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/** \brief core middle layer */
#include "core/awsa_core_internal.h"

/** \brief AWorks platform */
#include "aw_delayed_work.h"
#include "aw_gpio.h"
#include "awbus_lite.h"

/**
 * \brief get maximum between two number
 */
#ifndef max
#define max(x, y)   (((x) < (y)) ? (y) : (x))
#endif

/**
 * \brief get minimum between two number
 */
#ifndef min
#define min(x, y)   (((x) < (y)) ? (x) : (y))
#endif

/** \brief rename */
#define AWSA_SOC_ARRAY_SIZE  AWSA_CORE_ARRAY_SIZE

/** \brief rename */
#define awsa_soc_fls         awsa_core_fls

#include "soc/awsa_soc_dapm.h"
#include "soc/awsa_soc.h"
#include "soc/awsa_soc_dai.h"
#include "soc/awsa_soc_jack.h"
#include "soc/awsa_soc_debug.h"
#include "soc/awsa_soc_decl.h"

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_SOC_INTERNAL_H */

/* end of file */
