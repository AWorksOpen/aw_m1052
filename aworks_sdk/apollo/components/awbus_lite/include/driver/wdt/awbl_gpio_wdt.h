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
 * \brief AWBus 使用GPIO喂狗 接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_gpio_wdt.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-08-27  may, first implementation
 * \endinternal
 */

#ifndef __AWBL_GPIO_WDT_H
#define __AWBL_GPIO_WDT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_wdt.h"


/*******************************************************************************
  define
*******************************************************************************/

/** \brief  定义用GPIO喂狗的名称 */
#define  GPIO_WDT_NAME  "gpio-wdt"


/******************************************************************************/

/** \brief 定义GPIO喂狗的参数 */
struct  awbl_gpio_wdt_param {
    uint32_t    gpio_num;           /**< \brief GPIO number the feed watchdog */
    uint32_t    wdt_time;           /**< \brief watchdog time (ms)*/
    void (*pfunc_plfm_init)(void);  /**< \brief device init */
};

/** \brief 定义GPIO喂狗的看门狗设备 */
struct  awbl_gpio_wdt_dev {
    struct awbl_dev          dev;    /**< \brief the AWBus device */
    struct awbl_hwwdt_info   info;   /**< \brief hwwdt info */
    uint16_t                 gpio_num;
};

/**
 * \brief gpio wdt driver register
 */
void  awbl_gpio_wdt_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_GPIO_WDT_H */

/* end of file */

