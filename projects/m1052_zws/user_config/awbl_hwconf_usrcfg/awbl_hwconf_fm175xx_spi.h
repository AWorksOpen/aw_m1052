/*******************************************************************************
*                                  AWorks
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

#ifndef AWBL_HWCONF_FM175XX_SPI_H
#define AWBL_HWCONF_FM175XX_SPI_H

#include "aw_gpio.h"
#include "aw_delay.h"
#include "driver/card_reader/awbl_fm175xx.h"
#include "imx1050_pin.h"

#ifdef AW_DEV_FM175XX_SPI3_0

/**
 * \addtogroup grp_aw_plfm_ext_fm175xx_usrcfg
 * @{
 */



/* ƽ̨��س�ʼ�� */
aw_local void __fm175xx_plfm_init (void)
{
    int fm175xx_gpios[] = {GPIO3_4, GPIO3_5, GPIO1_28};  /* NPD�ܽ�, IRQ�ܽţ�Ƭѡ�ܽ� */

    aw_gpio_pin_request("fm175xx_gpios",
                         fm175xx_gpios,
                         AW_NELEMENTS(fm175xx_gpios));
}

/* FM175XX �豸��Ϣ */
aw_local aw_const awbl_fm175xx_devinfo_t __g_fm175xx_dev0info = {
    AW_SPI_MODE_0,                     /* �ӿ�ʱ��ģʽ */
    GPIO1_28,                          /* NSS �ܽ�(CS) */
    GPIO3_4,                           /* NPD�ܽ� */
    GPIO3_5,                           /* IRQ �ܽ� */
    10000000,                          /* SPI����ʱ�� */
    AWBL_ISO_IEC_14443_A |             /* ISO/IEC 14443 AЭ�� */
    AWBL_ISO_IEC_14443_B,              /* ISO/IEC 14443 BЭ��, \note FM17510��֧�� */
    AWBL_FM175XX_CFG_TYPEA_1,          /* Ĭ������Ϊ֧��A�� */
    __fm175xx_plfm_init
};

/* FM175XX �豸ʵ���ڴ澲̬���� */
aw_local awbl_fm175xx_dev_t __g_fm175xx_dev0;

#define AWBL_HWCONF_FM175XX_0                 \
    {                                         \
        AWBL_FM175XX_NAME,                    \
        0,                                    \
        AWBL_BUSID_SPI,                       \
        IMX1050_LPSPI3_BUSID,                 \
        &(__g_fm175xx_dev0.spi_dev.super),    \
        &__g_fm175xx_dev0info                 \
    },

/** @} */

#else
#define AWBL_HWCONF_FM175XX_0

#endif

#endif
