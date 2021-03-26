/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx lpspi 从机例程引脚配置
 *
 * \internal
 * \par modification history:
 * - 1.00 20-04-16  licl, first implementation
 * \endinternal
 */

#include "aw_clk.h"
#include "aw_gpio.h"
#include "aw_rt10xx_lpspi_slv.h"
#include "imx1050_pin.h"

/* 平台相关初始化 */
aw_err_t imx1050_lpspi3_slv_plfm_init (void)
{
    aw_err_t ret = AW_OK;

    int lpspi3_gpios[] = {GPIO1_31,  //SCK
                          GPIO1_30,  //MOSI
                          GPIO1_29,  //MISO
                          GPIO1_28,  //CS
                          };

    ret = aw_gpio_pin_request("lpspi3_gpios",
                              lpspi3_gpios,
                              AW_NELEMENTS(lpspi3_gpios));
    if (ret != AW_OK) {
        return ret;
    }

    /* 配置功能引脚 */
    aw_gpio_pin_cfg(GPIO1_31, GPIO1_31_LPSPI3_SCK);
    aw_gpio_pin_cfg(GPIO1_30, GPIO1_30_LPSPI3_SDO);
    aw_gpio_pin_cfg(GPIO1_29, GPIO1_29_LPSPI3_SDI);
    aw_gpio_pin_cfg(GPIO1_28, GPIO1_28_LPSPI3_PCS0);

    /* 使能lpspi时钟 */
    aw_clk_enable(IMX1050_CLK_CG_LPSPI3);

    return AW_OK;
}

/*end of file */
