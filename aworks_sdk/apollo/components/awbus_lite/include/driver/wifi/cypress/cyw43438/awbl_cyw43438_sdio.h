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
 * \brief AWBus cyw43438 sdio driver
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_cyw43438_sdio.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 20-04-16  hsg, first implementation
 * \endinternal
 */

#ifndef __AWBL_CYW43438_SDIO_H
#define __AWBL_CYW43438_SDIO_H

#include "aw_spinlock.h"
#include "awbl_sdiobus.h"

#include "awbl_cyw43438_common.h"

/** \brief CYW43438 deiver name */
#define AWBL_CYW43438_SDIO_NAME         "cyw43438_sdio"

#define AWBL_SDIO_CYW43438_WLAN_NUM     0x2
#define AWBL_SDIO_CYW43438_WLAN_VID     0x02d0
#define AWBL_SDIO_CYW43438_WLAN_PID     0xa962


typedef struct awbl_sdio_cyw43438_wlan_dev_info {
    struct awbl_sdio_dev_info super;
    awbl_cyw43438_cfg_info_t  base;
}awbl_sdio_cyw43438_wlan_dev_info_t;

typedef struct awbl_sdio_cyw43438_wlan_dev {

    struct awbl_sdio_func *p_func;
    int                    mode;
    unsigned long          ioport;

    cyw43438_dev_common_t base;
} awbl_sdio_cyw43438_wlan_dev_t;

#define __DEV_CONVERT(p_this, p_arg) \
        awbl_sdio_cyw43438_wlan_dev_t *p_this = \
            (awbl_sdio_cyw43438_wlan_dev_t*) p_arg

#define __DEV_DECL(p_this, p_dev) \
    awbl_sdio_cyw43438_wlan_dev_info_t *__info = \
        (awbl_sdio_cyw43438_wlan_dev_info_t *) \
            ((struct awbl_dev *) p_dev)->p_devhcf->p_devinfo; \
    awbl_sdio_cyw43438_wlan_dev_t *p_this = \
        (awbl_sdio_cyw43438_wlan_dev_t *) __info->super.p_drv;

/* declare cyw43438 device infomation */
#define __INF_DECL(p_info, p_dev) \
    extern awbl_sdio_cyw43438_wlan_dev_info_t *__gp_cyw43438_sdio_info; \
    awbl_sdio_cyw43438_wlan_dev_info_t *p_info = NULL; \
    if (__gp_cyw43438_sdio_info != NULL) \
        p_info = \
            (awbl_sdio_cyw43438_wlan_dev_info_t *) __gp_cyw43438_sdio_info; \
    else \
        p_info = (awbl_sdio_cyw43438_wlan_dev_info_t *) AWBL_DEVINFO_GET(p_dev)

/**
 * \brief cyw43438 sdio driver register
 */
void awbl_cyw43438_sdio_drv_register (void);


#endif /* __AWBL_CYW43438_SDIO_H */

/* end of file */
