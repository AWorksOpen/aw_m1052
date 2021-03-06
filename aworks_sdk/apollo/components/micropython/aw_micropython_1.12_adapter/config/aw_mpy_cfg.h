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
 * - 200226, first implementation.
 * @endinternal
 */

#ifndef __AW_MPY_CFG_H
#define __AW_MPY_CFG_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \addtogroup grp_aw_if_mpy_cfg
 * \copydoc aw_mpy_cfg.h
 * @{
 */

#define CONFIG_MPY_CONSOLE_COMID    0
#define AWORKS
#define CONFIG_IMX1052_BOARD 1


/** \brief config the uart comid of python */
#if defined(AWORKS) && defined(CONFIG_MPY_CONSOLE_COMID)
#define MPY_CONSOLE_COMID    CONFIG_MPY_CONSOLE_COMID
#endif

/** \brief config micropython running board */
#if CONFIG_IMX28X_BOARD
#define AW_MPY_IMX28X       1
#elif CONFIG_IMX1052_BOARD
#define AW_MPY_IMXRT105X    1
#elif CONFIG_IMX1062_BOARD
#define AW_MPY_IMXRT106X    1
#endif

/** @} grp_aw_if_mpy_cfg */

#ifdef __cplusplus
}
#endif

#endif  /* __AW_MPY_CFG_H */

/* end of file */