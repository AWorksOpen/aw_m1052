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
 * \brief AWBus WATCHDOG 接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_wdt.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.01 16-01-12  may, add awbl_hwwdt_feed()
 * - 1.00 14-08-27  may, first implementation
 * \endinternal
 */

#ifndef __AWBL_WDT_H
#define __AWBL_WDT_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "aw_wdt.h"


/**
 * \brief AWBus watchdog信息
 */
struct awbl_hwwdt_info {

    /** \brief 喂狗操作*/
    void  (*feed)(struct awbl_hwwdt_info *pinfo);

    /** \brief 硬件看门狗时间 */
    uint32_t  wdt_time;
};

/**
 * \brief AWBus watchdog 接口模块初始化
 *
 * \attention 本函数应当在 awbl_dev_init2() 之后调用
 */
void   awbl_wdt_init (void);

/**
 * \brief AWBus watchdog 喂硬件看门狗接口
 *
 * \attention 本函数应当在 awbl_wdt_init() 之后调用
 */
aw_err_t awbl_hwwdt_feed (void);

/**
 * \brief AWBus 注册硬件看门狗，用于获取操作底层硬件看门狗的信息
 */
void  awbl_wdt_device_register (struct awbl_hwwdt_info  *pwdt_info);


#ifdef __cplusplus
}
#endif    /* __cplusplus     */



#endif /* __AWBL_WDT_H */

/* end of file */

