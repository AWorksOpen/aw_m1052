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

#ifndef __AWBL_HWCONF_CYW43362_SDIO_H
#define __AWBL_HWCONF_CYW43362_SDIO_H

#ifdef AW_DEV_CYW43362_WIFI_SDIO_0

#include "aw_delay.h"
#include "aw_spinlock.h"
#include "awbl_gpio.h"
#include "imx1050_pin.h"

#include "driver/wifi/cypress/cyw43362/awbl_cyw43362_sdio.h"


/**
 * \addtogroup grp_aw_plfm_ext_cyw43362_spi_usrcfg
 * @{
 */

#define CYW43362_WIFI_HAVE_MODE_SEL         0
#define GPIO_PIN_REQUEST_NEEDED             1

#define CYW43362_WIFI_WAKE_PIN              GPIO1_30  // WL_HOST_WAKE
#define CYW43362_WIFI_RST_PIN               GPIO1_28  // WL_REG_ON(RST)

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
extern aw_bool_t cyw43362_wifi_unmask_sdio_int (void *dev);
extern aw_bool_t cyw43362_wifi_enable_sdio_int (void *dev);

aw_local void __cyw43362_plfm_init (void)
{
#if GPIO_PIN_REQUEST_NEEDED
    aw_local aw_const int pins[] = {
#if CYW43362_WIFI_HAVE_MODE_SEL
        CYW43362_WIFI_MODE_PIN,
#endif
        CYW43362_WIFI_WAKE_PIN,
        CYW43362_WIFI_RST_PIN,
    };

    if (AW_OK == aw_gpio_pin_request(AWBL_CYW43362_SDIO_NAME, pins, AW_NELEMENTS(pins))) {
#endif
#if CYW43362_WIFI_HAVE_MODE_SEL
        aw_gpio_pin_cfg(CYW43362_WIFI_MODE_PIN, AW_GPIO_OUTPUT_INIT_HIGH);
#endif

    aw_gpio_pin_cfg(CYW43362_WIFI_WAKE_PIN, AW_GPIO_INPUT);
    aw_gpio_pin_cfg(CYW43362_WIFI_RST_PIN, AW_GPIO_OUTPUT_INIT_LOW);

    /* WIFI¸´Î» */
    aw_gpio_set(CYW43362_WIFI_RST_PIN, 0);
    aw_udelay(50000);
    aw_gpio_set(CYW43362_WIFI_RST_PIN, 1);

#if GPIO_PIN_REQUEST_NEEDED
    }
#endif
}

aw_local struct awbl_sdio_func __g_cyw43362_sdio_wlan_func = {
    .num = 2,
    .name = "/dev/sdio/cyw43362",
};

aw_local awbl_sdio_cyw43362_wlan_dev_t __g_sdio_cyw43362_wlan_dev = {
    .p_func = &__g_cyw43362_sdio_wlan_func,
};


#define AW_CYW43362_SDIO_XXX \
    { \
        "wifi43362", \
    }, \
 \
    { \
        &g_cyw43362_netif_info_sta, \
        &g_cyw43362_netif_info_ap, \
    }, \
 \
    { \
        "st", \
        "ap", \
    }, \
 \
    { \
        { \
            cyw43362_wifi_firmware_size, \
            cyw43362_wifi_firmware_read \
        }, \
        { \
            cyw43362_wifi_cfg_nvram_size, \
            cyw43362_wifi_cfg_nvram_read \
        }, \
    }, \
 \
    AW_WIFI_COUNTRY_CHINA, \
 \
    CYW43362_WIFI_RST_PIN, \
    CYW43362_WIFI_WAKE_PIN, \
 \
    cyw43362_wifi_roam_config


struct awbl_sdio_cyw43362_wlan_dev_info __g_cyw43362_sdio_wifi_info = {
        {
            AWBL_SDIO_TYPE_IO,
            AWBL_SDIO_CLASS_WLAN,                /* class */
            AWBL_SDIO_CYW43362_WLAN_VID,         /* vendor */
            AWBL_SDIO_CYW43362_WLAN_PID,         /* device */
            &__g_sdio_cyw43362_wlan_dev,
            __cyw43362_plfm_init,
        },
        {
            AW_CYW43362_SDIO_XXX
        },
};

aw_const awbl_cyw43362_cfg_info_t g_cyw43362_cfg_info = {
    AW_CYW43362_SDIO_XXX
};

#define AWBL_HWCONF_CYW43362_WIFI_SDIO          \
    {                                           \
        AWBL_CYW43362_SDIO_NAME,                \
        0,                                      \
        AWBL_BUSID_SDIO,                        \
        IMX1050_USBH2_BUSID,                                      \
        &__g_cyw43362_sdio_wlan_func.super,     \
        &__g_cyw43362_sdio_wifi_info            \
    },

/** @} */


#else

#define AWBL_HWCONF_CYW43362_WIFI_SDIO

#endif

#endif

/* end of file */
