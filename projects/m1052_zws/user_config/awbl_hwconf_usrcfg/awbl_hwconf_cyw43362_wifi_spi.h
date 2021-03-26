/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_CYW43362_SPI_H
#define __AWBL_HWCONF_CYW43362_SPI_H

#ifdef AW_DEV_CYW43362_WIFI_SPI_0

#include "aw_delay.h"
#include "aw_spinlock.h"
#include "awbl_gpio.h"

#include "driver/wifi/cypress/cyw43362/awbl_cyw43362_spi.h"

/**
 * \addtogroup grp_aw_plfm_ext_cyw43362_spi_usrcfg
 * @{
 */

#define CYW43362_WIFI_HAVE_MODE_SEL         0
#define GPIO_PIN_REQUEST_NEEDED             1

#define CYW43362_WIFI_WAKE_PIN              GPIO3_5  // WL_HOST_WAKE
#define CYW43362_WIFI_RST_PIN               GPIO3_4  // WL_REG_ON(RST)
#define CYW43362_WIFI_CS_PIN                GPIO1_28  // SPI1_NSS(CS)
#define CYW43362_WIFI_IRQ_PIN               GPIO3_3 // SPI1_IRQ
#if CYW43362_WIFI_HAVE_MODE_SEL
    #define CYW43362_WIFI_MODE_PIN          PIOB_10 // MODE_SEL
#endif

extern aw_const aw_netif_info_get_entry_t g_cyw43362_netif_info_sta;
extern aw_const aw_netif_info_get_entry_t g_cyw43362_netif_info_ap;
extern aw_bool_t cyw43362_wifi_roam_config (uint32_t *roam_param);
extern aw_err_t cyw43362_wifi_firmware_size (uint32_t *size_out);
extern aw_err_t cyw43362_wifi_firmware_read (uint32_t  offset,
                                             void     *buffer,
                                             uint32_t  size_in,
                                             uint32_t *size_out);
extern aw_err_t cyw43362_wifi_cfg_nvram_size (uint32_t *size_out);
extern aw_err_t cyw43362_wifi_cfg_nvram_read (uint32_t  offset,
                                              void     *buffer,
                                              uint32_t  size_in,
                                              uint32_t *size_out);

aw_local void __cyw43362_plfm_init (void)
{
#if GPIO_PIN_REQUEST_NEEDED
    aw_local aw_const int pins[] = {
#if CYW43362_WIFI_HAVE_MODE_SEL
        CYW43362_WIFI_MODE_PIN,
#endif
        CYW43362_WIFI_WAKE_PIN,
        CYW43362_WIFI_RST_PIN,
//        CYW43362_WIFI_CS_PIN,
        CYW43362_WIFI_IRQ_PIN,
    };

    if (AW_OK == aw_gpio_pin_request(AWBL_CYW43362_SPI_NAME, pins, AW_NELEMENTS(pins))) {
#endif
#if CYW43362_WIFI_HAVE_MODE_SEL
        aw_gpio_pin_cfg(CYW43362_WIFI_MODE_PIN, AW_GPIO_OUTPUT_INIT_HIGH);
#endif
        aw_gpio_pin_cfg(CYW43362_WIFI_WAKE_PIN, AW_GPIO_OUTPUT_INIT_HIGH);
        aw_gpio_pin_cfg(CYW43362_WIFI_RST_PIN, AW_GPIO_OUTPUT_INIT_LOW);
#if GPIO_PIN_REQUEST_NEEDED
    }
#endif
}

aw_local awbl_cyw43362_spi_dev_t __g_cyw43362_spi_dev;

aw_local aw_const awbl_cyw43362_spi_cfg_t __g_cyw43362_spi_cfg = {
    {
        AW_SPI_MODE_0,
        24000000,
        CYW43362_WIFI_CS_PIN,
        __cyw43362_plfm_init,
    },
    {
        {
            "wifi43362",
        },
        {
            &g_cyw43362_netif_info_sta,
            &g_cyw43362_netif_info_ap,
        },
        {
            "st",
            "ap",
        },
        {
            {
                cyw43362_wifi_firmware_size,
                cyw43362_wifi_firmware_read,
            },
            {
                cyw43362_wifi_cfg_nvram_size,
                cyw43362_wifi_cfg_nvram_read,
            },
        },
        AW_WIFI_COUNTRY_CHINA,
        CYW43362_WIFI_RST_PIN,
        CYW43362_WIFI_IRQ_PIN,
        cyw43362_wifi_roam_config
    }
};

#define AWBL_HWCONF_CYW43362_WIFI_SPI           \
    {                                           \
        AWBL_CYW43362_SPI_NAME,                 \
        0,                                      \
        AWBL_BUSID_SPI,                         \
        IMX1050_LPSPI3_BUSID,                   \
        &__g_cyw43362_spi_dev.super.super,      \
        &__g_cyw43362_spi_cfg                   \
    },

/** @} */


#else

#define AWBL_HWCONF_CYW43362_WIFI_SPI

#endif

#endif

/* end of file */
