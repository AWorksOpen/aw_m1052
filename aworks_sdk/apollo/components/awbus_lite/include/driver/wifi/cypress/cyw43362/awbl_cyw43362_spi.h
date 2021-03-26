/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus cyw43362 spi driver
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_cyw43362_spi.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 17-07-11  phd, first implementation
 * \endinternal
 */

#ifndef __AWBL_CYW43362_SPI_H
#define __AWBL_CYW43362_SPI_H

#ifdef AXIO_AWORKS
#include "aw_common_drivers_autoconf.h"
#ifdef CONFIG_DRV_CYW43362_SPI
#define AW_DRV_AWBL_CYW43362_WIFI_SPI
#endif
#endif

#ifdef AW_DRV_AWBL_CYW43362_WIFI_SPI

#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "awbl_spibus.h"

#include "awbl_cyw43362_common.h"

/** \brief CYW43362 deiver name */
#define AWBL_CYW43362_SPI_NAME          "cyw43362_spi"

struct awbl_cyw43362_spi_dev_info {
    uint16_t mode;
    uint32_t speed_hz;
    int      cs_pin;
    void   (*pfn_plfm_init) (void);
};

/**
 * \brief cyw43362_spi设备信息
 */
typedef struct awbl_cyw43362_spi_cfg {
    struct awbl_cyw43362_spi_dev_info super;
    awbl_cyw43362_cfg_info_t          base;
} awbl_cyw43362_spi_cfg_t;

/**
 * \brief cyw43362_spi设备结构体定义
 */
typedef struct awbl_cyw43362_spi_dev {

    /** \brief 继承自AWBus SPI device */
    struct awbl_spi_device super;

    cyw43362_dev_common_t base;
} awbl_cyw43362_spi_dev_t;

#define __DEV_CONVERT(p_this, p_arg) \
        awbl_cyw43362_spi_dev_t *p_this = (awbl_cyw43362_spi_dev_t*) (p_arg)

/** \brief declare cyw43362 device instance */
#define __DEV_DECL(p_this, p_dev) \
    awbl_cyw43362_spi_dev_t *p_this = \
        (awbl_cyw43362_spi_dev_t *) (p_dev)

/** \brief declare cyw43362 device infomation */
#define __INF_DECL(p_devinfo, p_dev) \
    awbl_cyw43362_spi_cfg_t *p_devinfo = \
        (awbl_cyw43362_spi_cfg_t *) AWBL_DEVINFO_GET(p_dev)

/**
 * \brief cyw43362 spi driver register
 */
void awbl_cyw43362_spi_drv_register (void);

#endif

#endif /* __AWBL_CYW43362_SPI_H */


/* end of file */

