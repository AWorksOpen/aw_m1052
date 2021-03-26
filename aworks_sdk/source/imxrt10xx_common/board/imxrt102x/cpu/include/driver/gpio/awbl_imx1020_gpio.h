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

/**
 * \file
 * \brief AWBus-lite iMX RT1020 GPIO 驱动
 *
 * iMX RT102X系列MCU的片上GPIO，最多可以有97个GPIO引脚可用，有如下
 * 特性:
 *  - 引脚可以由软件配置为输入/输出
 *  - 每个单独引脚可被用作外部中断输入引脚
 *  - 每个中断可配置为下降沿、上升沿或高低电平产生中断
 *  - 所有GPIO 引脚默认为输入
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "imx1020_gpio"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_imx1020_gpio_devinfo
 *
 * \internal
 * \par modification history:
 * - 1.00 18-09-25, mex, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX1020_GPIO_H
#define __AWBL_IMX1020_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "awbl_gpio.h"
#include "awbl_intctlr.h"
#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "imx1020_pin.h"


/** \brief IMX1020 GPIO 驱动名 */
#define AWBL_IMX1020_GPIO_NAME   "imx1020_gpio"

/** \brief IMX1020 GPIO 的端口(bank)数 */
#define  IMX1020_GPIO_PORTS   4

/** \brief IMX1020 GPIO 每端口含的GPIO个数 */
#define  IMX1020_PORT_PINS    32


/** \brief IMX1020 GPIO平台初始化函数指针 */
typedef void (*pfn_gpio_plat_init_t)(void);


/**
 * \brief IMX1020 GPIO 设备信息
 */
struct awbl_imx1020_gpio_devinfo{

    /** \brief 平台相关的初始化函数 */
    pfn_gpio_plat_init_t         plat_init;

    /** \brief GPIO端口寄存器基地址 */
    const uint32_t               *gpio_regs_base;

    /** \brief IOMUXC 寄存器基地址 */
    uint32_t                     iomuxc_base;

    /** \brief IOMUXC SNVS寄存器基地址 */
    uint32_t                     iomuxc_snvs_base;

    /** \brief GPIO 引脚配置状态 */
    uint8_t                      *p_pin_cfg;

    /** \brief GPIO 服务配置信息 */
    struct awbl_gpio_servinfo    gpio_info;

    /** \brief IntCtlr 服务配置信息 */
    struct awbl_intctlr_servinfo int_info;
};


/**
 * \brief GPIO引脚中断触发信息结构
 */
struct awbl_imx1020_gpio_trigger_info {

    aw_pfuncvoid_t  pfunc_callback;     /**< \brief 中断回调函数 */
    void           *p_arg;              /**< \brief 传递给回调函数的参数 */
};


/**
 * \brief IMX1020 GPIO 设备实例
 */
struct awbl_imx1020_gpio_dev {

    struct awbl_dev             awdev;           /**< \brief 继承自 AWBus 设备 */
    struct awbl_gpio_service    gpio_serv;       /**< \brief GPIO 服务 */
    struct awbl_intctlr_service intctlr_serv;    /**< \brief IntCtlr 服务 */
    aw_spinlock_isr_t           dev_lock;        /**< \brief 设备锁 */

    /** \brief GPIO 引脚ISR信息 */
    struct awbl_imx1020_gpio_trigger_info trig_info[IMX1020_GPIO_PINS];

};


/**
 * \brief 用户调用此函数注册GPIO驱动
 */
void awbl_imx1020_gpio_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX1020_GPIO_H */


/* end of file */
