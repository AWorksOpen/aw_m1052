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

#ifndef __AWBL_HWCONF_SX127X_H
#define __AWBL_HWCONF_SX127X_H

#ifdef  AW_DEV_SX127X

#include "aworks.h"
#include "aw_delay.h"
#include "aw_gpio.h"
#include "driver/lora/awbl_sx127x.h"

/*******************************************************************************
    ������Ϣ
*******************************************************************************/

/** \brief sx127xƽ̨��ʼ�� */
static void __plfm_sx127x_init (void *p_arg)
{
    /* ��ȡ�豸ָ�� */
    awbl_sx127x_dev_t *p_dev = (awbl_sx127x_dev_t *)p_arg;

    /*
     * ��ʼ��DIO��RST����
     *
     */
    if (aw_gpio_pin_request("lora_dio0",
                            &p_dev->p_devinfo->dio0,
                            1) == AW_OK) {
        aw_gpio_pin_cfg(p_dev->p_devinfo->dio0, AW_GPIO_INPUT  | AW_GPIO_PULL_DOWN);
    }
    if (aw_gpio_pin_request("lora_dio1",
                            &p_dev->p_devinfo->dio1,
                            1) == AW_OK) {
        aw_gpio_pin_cfg(p_dev->p_devinfo->dio1, AW_GPIO_INPUT  | AW_GPIO_PULL_DOWN);
    }
    if (aw_gpio_pin_request("lora_dio2",
                            &p_dev->p_devinfo->dio2,
                            1) == AW_OK) {
        aw_gpio_pin_cfg(p_dev->p_devinfo->dio2, AW_GPIO_INPUT  | AW_GPIO_PULL_DOWN);
    }
//    aw_gpio_pin_cfg(p_dev->p_devinfo->dio3, AW_GPIO_INPUT  | AW_GPIO_PULL_DOWN);
//    aw_gpio_pin_cfg(p_dev->p_devinfo->dio4, AM_GPIO_INPUT  | AW_GPIO_PULL_UP);
//    aw_gpio_pin_cfg(p_dev->p_devinfo->dio5, AM_GPIO_INPUT  | AW_GPIO_PULL_UP);
    if (aw_gpio_pin_request("lora_reset",
                            &p_dev->p_devinfo->reset,
                            1) == AW_OK) {
        aw_gpio_pin_cfg(p_dev->p_devinfo->reset,  AW_GPIO_OUTPUT | AW_GPIO_PULL_UP);
    }
    aw_gpio_set(p_dev->p_devinfo->reset, 0);
    aw_mdelay(1);
    aw_gpio_set(p_dev->p_devinfo->reset, 1);

    if (p_dev->p_devinfo->use_gpio_spi) {
        /* ��ʼ��SPI���� */
        if (aw_gpio_pin_request("lora_cs",
                                &p_dev->p_devinfo->cs,
                                1) == AW_OK) {
            aw_gpio_pin_cfg(p_dev->p_devinfo->cs,   AW_GPIO_OUTPUT);
        }
        aw_gpio_set(p_dev->p_devinfo->cs, 1);

        if (aw_gpio_pin_request("lora_sck",
                                &p_dev->p_devinfo->sck,
                                1) == AW_OK) {
            aw_gpio_pin_cfg(p_dev->p_devinfo->sck,  AW_GPIO_OUTPUT);
        }
        if (aw_gpio_pin_request("lora_miso",
                                &p_dev->p_devinfo->miso,
                                1) == AW_OK) {
            aw_gpio_pin_cfg(p_dev->p_devinfo->miso, AW_GPIO_INPUT);
        }
        if (aw_gpio_pin_request("lora_mosi",
                                &p_dev->p_devinfo->mosi,
                                1) == AW_OK) {
            aw_gpio_pin_cfg(p_dev->p_devinfo->mosi, AW_GPIO_OUTPUT);
        }

        aw_gpio_set(p_dev->p_devinfo->mosi, 0);
        aw_gpio_set(p_dev->p_devinfo->sck, 0);
    }

    /* �����жϷ����� */
    aw_gpio_trigger_connect(p_dev->p_devinfo->dio0, p_dev->p_dio_irq[0], p_dev);
    aw_gpio_trigger_connect(p_dev->p_devinfo->dio1, p_dev->p_dio_irq[1], p_dev);
    aw_gpio_trigger_connect(p_dev->p_devinfo->dio2, p_dev->p_dio_irq[2], p_dev);
//    aw_gpio_trigger_connect(p_dev->p_devinfo->dio3, p_dev->p_dio_irq[3], p_dev);
//    aw_gpio_trigger_connect(p_dev->p_devinfo->dio4, p_dev->p_dio_irq[4], p_dev);
//    aw_gpio_trigger_connect(p_dev->p_devinfo->dio5, p_dev->p_dio_irq[5], p_dev);

    /* ���ô������źʹ���ģʽ */
    //���óɸߵ�ƽ���������Ե͹���ģʽ
    aw_gpio_trigger_cfg(p_dev->p_devinfo->dio0, AW_GPIO_TRIGGER_HIGH);
    aw_gpio_trigger_cfg(p_dev->p_devinfo->dio1, AW_GPIO_TRIGGER_HIGH);
    aw_gpio_trigger_cfg(p_dev->p_devinfo->dio2, AW_GPIO_TRIGGER_HIGH);
//    aw_gpio_trigger_cfg(p_dev->p_devinfo->dio3, AW_GPIO_TRIGGER_HIGH);
//    aw_gpio_trigger_cfg(p_dev->p_devinfo->dio4, AW_GPIO_TRIGGER_HIGH);
//    aw_gpio_trigger_cfg(p_dev->p_devinfo->DIO5, AW_GPIO_TRIGGER_HIGH);

    /* ʹ�����Ŵ����ж� */
    aw_gpio_trigger_on(p_dev->p_devinfo->dio0);
    aw_gpio_trigger_on(p_dev->p_devinfo->dio1);
    aw_gpio_trigger_on(p_dev->p_devinfo->dio2);
//    aw_gpio_trigger_on(p_dev->p_devinfo->dio3);
//    aw_gpio_trigger_on(p_dev->p_devinfo->dio4);
//    aw_gpio_trigger_on(p_dev->p_devinfo->dio5);
}

/** \brief ���sx127xƽ̨��ʼ�� */
static void __plfm_sx127x_deinit (void *p_arg)
{
    /* ��ȡ�豸ָ�� */
    awbl_sx127x_dev_t *p_dev = (awbl_sx127x_dev_t *)p_arg;

//    /* ȥIO��ʼ�� */
//    am_gpio_pin_cfg(p_dev->p_devinfo->dio0, AM_GPIO_INPUT  | AM_GPIO_FLOAT);
//    am_gpio_pin_cfg(p_dev->p_devinfo->dio1, AM_GPIO_INPUT  | AM_GPIO_FLOAT);
//    am_gpio_pin_cfg(p_dev->p_devinfo->dio2, AM_GPIO_INPUT  | AM_GPIO_FLOAT);
//    am_gpio_pin_cfg(p_dev->p_devinfo->dio3, AM_GPIO_INPUT  | AM_GPIO_FLOAT);
////    am_gpio_pin_cfg(p_dev->p_devinfo->dio4, AM_GPIO_INPUT  | AM_GPIO_FLOAT);
//    am_gpio_pin_cfg(p_dev->p_devinfo->dio5, AM_GPIO_INPUT  | AM_GPIO_FLOAT);
//    am_gpio_pin_cfg(p_dev->p_devinfo->reset, AM_GPIO_INPUT | AM_GPIO_PULLUP);
//
//    am_gpio_pin_cfg(p_dev->p_devinfo->sck,  AM_GPIO_INPUT  | AM_GPIO_PULLUP);
//    am_gpio_pin_cfg(p_dev->p_devinfo->cs,   AM_GPIO_INPUT  | AM_GPIO_PULLUP);
//    am_gpio_pin_cfg(p_dev->p_devinfo->miso, AM_GPIO_INPUT  | AM_GPIO_FLOAT);
//    am_gpio_pin_cfg(p_dev->p_devinfo->mosi, AM_GPIO_INPUT  | AM_GPIO_PULLUP);
}

/**
 * \brief sx127x�豸��Ϣ
 */
static const awbl_sx127x_devinfo_t __g_sx127x_devinfo = {
    0,              /**< \brief �豸id */

    GPIO3_4,        /**< \brief RESET */
    GPIO3_5,        /**< \brief DIO0 */
    GPIO3_3,        /**< \brief DIO1 */
    GPIO3_2,        /**< \brief DIO2 */
    GPIO3_1,        /**< \brief DIO3 */
    0xFFFF,         /**< \brief DIO4 ��Ч��������δ���ӣ� */
    GPIO3_0,        /**< \brief DIO5 */

    AW_TRUE,           /**< \brief use gpio spi */

    GPIO1_30,       /**< \brief SPI MOSI */
    GPIO1_29,       /**< \brief SPI MISO */
    GPIO1_31,       /**< \brief SPI CLK */
    GPIO1_28,       /**< \brief SPI CS */

    0,              /**< \brief DIO task prio */
    2,              /**< \brief SPI device num */

    2000000,

    __plfm_sx127x_init,          /**< \brief sx127x��ƽ̨��ʼ�� */
    __plfm_sx127x_deinit,        /**< \brief sx127x��ƽ̨ȥ��ʼ�� */

};

/* �豸ʵ���ڴ澲̬���� */
aw_local awbl_sx127x_dev_t __g_sx127x_dev;

#define AWBL_HWCONF_SX127X                          \
    {                                               \
        SX127X_DEV_NAME,                            \
        0,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        &__g_sx127x_dev.super,                      \
        &__g_sx127x_devinfo                         \
    },

#else
#define AWBL_HWCONF_SX127X

#endif  /* AW_DEV_SX127X */

#endif  /* __AWBL_HWCONF_SX127X_H */

/* end of file */
