/******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief IMX1050 eDMA hardware configure
 *
 * \internal
 * \par Modification History
 * - 1.00 17-11-15  hsg, first implementation.
 * \endinternal
 */

#ifndef     __AWBL_HWCONF_IMX1050_EDMA_H
#define     __AWBL_HWCONF_IMX1050_EDMA_H

#include "aw_cpu_clk.h"
#include "imx1050_reg_base.h"
#include "imx1050_inum.h"
#include "driver/edma/awbl_imx10xx_edma.h"

/* 平台相关初始化 */
aw_local void __imx1050_edma_plfm_init (void)
{
    aw_clk_enable(IMX1050_CLK_CG_DMA);
}

aw_local const uint32_t inum[] = {
    IMX_EDMA_CHAN0_16,
    IMX_EDMA_CHAN1_17,
    IMX_EDMA_CHAN2_18,
    IMX_EDMA_CHAN3_19,
    IMX_EDMA_CHAN4_20,
    IMX_EDMA_CHAN5_21,
    IMX_EDMA_CHAN6_22,
    IMX_EDMA_CHAN7_23,
    IMX_EDMA_CHAN8_24,
    IMX_EDMA_CHAN9_25,
    IMX_EDMA_CHAN10_26,
    IMX_EDMA_CHAN11_27,
    IMX_EDMA_CHAN12_28,
    IMX_EDMA_CHAN13_29,
    IMX_EDMA_CHAN14_30,
    IMX_EDMA_CHAN15_31,
};

/* 设备信息 */
aw_local aw_const struct awbl_imx10xx_edma_devinfo __g_imx1050_edma_devinfo = {
    IMX1050_EDMA_BASE,
    IMX1050_DMAMUX_BASE,
    inum,
    sizeof(inum) / sizeof(inum[0]),
    IMX_EDMA_ERR,
    __imx1050_edma_plfm_init
};

/* 设备实例内存静态分配 */
aw_local awbl_imx10xx_edma_dev_t __g_imx1050_edma_dev = {{0}};

#define AWBL_HWCONF_IMX1050_EDMA     \
    {                               \
        AWBL_IMX10XX_EDMA_NAME,      \
        0,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        (struct awbl_dev *)&__g_imx1050_edma_dev,   \
        &__g_imx1050_edma_devinfo                   \
    },

#endif
/* end of file */

