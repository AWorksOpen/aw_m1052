/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief imx1050 nandflash driver head file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-3-22  smc, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_NAND_H
#define __AWBL_IMX6UL_NAND_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "aworks.h"
#include "awbus_lite.h"
#include "mtd/aw_mtd.h"

#include "awbl_nand.h"
#include "awbl_nand_bus.h"

struct awbl_imx1050_nand_dev;

struct awbl_imx1050_nand_info {
    /** \brief 控制器操作Nand芯片的时序 */
    struct awbl_nand_timing     tim;
};

void awbl_imx1050_nand_platform_register (void);

#endif    /* __AWBL_IMX1050_NAND_H */

/* end of file */
