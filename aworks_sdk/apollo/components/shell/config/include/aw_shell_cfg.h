/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * @file
 * @brief
 *
 * @internal
 * @par History
 * - 170619, vih, first implementation.
 * @endinternal
 */

#ifndef __AW_SHELL_CFG_H
#define __AW_SHELL_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef AXIO_AWORKS
#include "aw_shell_autoconf.h"
#endif

/**
 * \addtogroup grp_aw_if_shell_cfg
 * \copydoc aw_shell_cfg.h
 * @{
 */

/** \brief shell 使能 */ 
#ifdef AXIO_AWORKS
#ifdef CONFIG_SHELL_ENABLE
#define AW_SHELL_ENABLED
#endif
#else
#define AW_SHELL_ENABLED
#endif


/** \brief shell line buf size */
#ifdef AXIO_AWORKS
#ifdef CONFIG_SHELL_LINE_SIZE
#define AW_SHELL_LINE_SIZE            CONFIG_SHELL_LINE_SIZE
#endif
#else
#define AW_SHELL_LINE_SIZE            79
#endif

/** \brief Use cursor */
#ifdef AXIO_AWORKS
#ifdef CONFIG_SHELL_USE_CURSOR
#define AW_SHELL_USE_CURSOR
#endif
#else
#define AW_SHELL_USE_CURSOR
#endif

/** \brief Use color */
#ifdef AXIO_AWORKS
#ifdef CONFIG_SHELL_USE_COLOR
#define AW_SHELL_USE_COLOR
#endif
#else
#define AW_SHELL_USE_COLOR
#endif

/** \brief Use string completion */
#ifdef AXIO_AWORKS
#ifdef CONFIG_SHELL_USE_COMPLETE
#define AW_SHELL_USE_COMPLETE
#endif
#else
#define AW_SHELL_USE_COMPLETE
#endif

/** \brief Use chart */
#ifdef AXIO_AWORKS
#ifdef CONFIG_SHELL_USE_CHART
#define AW_SHELL_USE_CHART
#endif
#else
#define AW_SHELL_USE_CHART
#endif


/** \brief Use command set */
#ifdef AXIO_AWORKS
#ifdef CONFIG_SHELL_USE_CMDSET
#define AW_SHELL_USE_CMDSET
#endif
#else
#define AW_SHELL_USE_CMDSET
#endif

/** \brief Use history */
#ifdef AXIO_AWORKS
#ifdef CONFIG_SHELL_USE_HISTORY
#define AW_SHELL_USE_HISTORY
#endif
#else
#define AW_SHELL_USE_HISTORY
#endif


/**< \brief Use history count */
//#define AW_SHELL_HISTORY_COUNT         1000u

/** @} grp_aw_if_shell_cfg */

#ifdef __cplusplus
}
#endif

#endif  /* __AW_SHELL_CFG_H */

/* end of file */

