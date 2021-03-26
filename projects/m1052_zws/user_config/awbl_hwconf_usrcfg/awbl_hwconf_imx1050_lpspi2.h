/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef __AWBL_HWCONF_IMX1050_LPSPI2_H
#define __AWBL_HWCONF_IMX1050_LPSPI2_H


#ifdef AW_DEV_IMX1050_LPSPI2

#include "aw_gpio.h"
#include "aw_clk.h"
#include "driver/lpspi/awbl_imx10xx_lpspi.h"


/* ƽ̨��س�ʼ�� */
aw_local void __imx1050_lpspi2_plfm_init (void)
{

    int lpspi2_gpios[] = {
                          GPIO3_7,    //CLK
                          GPIO3_8,    //MOSI
                          GPIO3_9,    //MISO
                          };

    if (aw_gpio_pin_request("lpspi2_gpios",
                             lpspi2_gpios,
                             AW_NELEMENTS(lpspi2_gpios)) == AW_OK) {

        /* ���ù������� */
        aw_gpio_pin_cfg(GPIO3_7, GPIO3_7_LPSPI2_SCK);
        aw_gpio_pin_cfg(GPIO3_8, GPIO3_8_LPSPI2_SD0);
        aw_gpio_pin_cfg(GPIO3_9, GPIO3_9_LPSPI2_SDI);
    }

    aw_clk_enable(IMX1050_CLK_CG_LPSPI2);
}


/* �豸��Ϣ */
aw_local aw_const struct awbl_imx10xx_lpspi_devinfo __g_imx1050_lpspi2_devinfo = {
    {
        IMX1050_LPSPI2_BUSID,       /**< \brief ���߱�� */
    },
    IMX1050_LPSPI2_BASE,            /**< \brief �Ĵ�������ַ */
    INUM_LPSPI2,                    /**< \brief �жϱ�� */
    IMX1050_CLK_CG_LPSPI2,          /**< \brief ʱ�Ӻ� */
    AW_FALSE,                       /**< \brief Ӳ��CSģʽ */
    AW_FALSE,                       /**< \brief �ߵ�ƽ��Ч */
    200,                            /**< \brief cs��Ч����һ��sck edge��delay cycle */
    100,                            /**< \brief ���һ��sck edge��cs��Ч��delay cycle */
    __imx1050_lpspi2_plfm_init      /**< \brief ƽ̨��س�ʼ�� */
};

/* �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx10xx_lpspi_dev __g_imx1050_lpspi2_dev;

#define AWBL_HWCONF_IMX1050_LPSPI2                   \
    {                                                \
        AWBL_IMX10XX_LPSPI_NAME,                     \
        0,                                           \
        AWBL_BUSID_PLB,                              \
        0,                                           \
        (struct awbl_dev *)&__g_imx1050_lpspi2_dev,  \
        &__g_imx1050_lpspi2_devinfo                  \
    },

#else
#define AWBL_HWCONF_IMX1050_LPSPI2

#endif /* AW_DEV_IMX1050_LPSPI2 */

#endif /* __AWBL_HWCONF_IMX1050_LPSPI2_H */

/* end of file */
