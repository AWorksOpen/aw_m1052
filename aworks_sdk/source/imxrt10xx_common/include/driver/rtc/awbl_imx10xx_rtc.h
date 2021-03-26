/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief imx10xx SNVS RTC驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 17-08-30  mex, first implementation
 * \endinternal
 */

#ifndef AWBL_IMX10xx_RTC_H
#define AWBL_IMX10xx_RTC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_rtc.h"
#include "aw_sem.h"

/** \brief 驱动名 */
#define AWBL_IMX10XX_RTC_NAME    "imx10xx_rtc"

/**
 * \brief imx10xx RTC 设备信息
 */
struct awbl_imx10xx_rtc_devinfo {

    /** \brief RTC 服务配置信息 */
    struct awbl_rtc_servinfo  rtc_servinfo;

    uint32_t                  regbase;  /**< 寄存器基地址 */
    void (*pfunc_plfm_init)(void);      /**< 平台相关初始化：开启时钟 */
};


/**
 * \brief imx10xx RTC 设备实例
 */
struct awbl_imx10xx_rtc_dev {

    /** \brief 驱动设备 */
    struct awbl_dev super;

    /** \brief 提供 RTC 服务 */
    struct awbl_rtc_service rtc_serv;

    /** \brief 设备锁 */
    AW_MUTEX_DECL(dev_lock);
};

/**
 * \brief imx10xx RTC driver register
 */
void awbl_imx10xx_rtc_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* AWBL_IMX10xx_RTC_H */

/* end of file */
