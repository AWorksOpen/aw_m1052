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
 * \brief AWBus Ti C6000 系列DSP 通用中断控制器驱动
 *
 * 本驱动基于BIOS6.3，需要将本驱动提供的4个ISR
 * (g_awbl_tic6000_intc_isr0 ~ g_awbl_tic6000_intc_isr3) 分别连接到BIOS TCONF 的
 * HWI_INT4 ~ HWI_INT7。
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "tic6000_intc"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_tic6000_intc_devinfo
 *
 * \par 2.兼容设备
 *
 *  - Ti C674x 系列DSP
 *  - 其它兼容设备
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_tic6000_intc_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_int
 *
 * \par 5.实现相关
 *
 *  - \ref grp_awbl_tic6000_intc_impl
 *
 * \internal
 * \par modification history:
 * - 1.00 14-03-06  zen, first implemetation
 * \endinternal
 */

#ifndef __AWBL_TIC6000_INTC_H
#define __AWBL_TIC6000_INTC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/**
\addtogroup grp_awbl_drv_tic6000_intc
\copydetails awbl_tic6000_intc.h
@{
*/

/**
 * \defgroup grp_awbl_tic6000_intc_impl  实现相关
 *
 * \par 1. 特性
 *
 *  - 本实现只提供对外设中断的控制
 *  - 每个中断只能连接1个中断服务函数
 *  - 不提供中断优先级的设置
 */

/**
 * \addtogroup grp_awbl_tic6000_intc_hwconf  设备定义/配置
 *
 * ARM NVIC 硬件设备的定义如下面的代码所示，用户在定义目标系统的硬件资源时，
 * 可以直接将这段代码嵌入到 awbus_lite_hwconf_usrcfg.c 中对应的位置，然后对设备
 * 信息进行必要的配置(可配置的项目用【x】标识)。
 *
 * \include  hwconf_usrcfg_nvic_armcm.c
 *
 * \note 以下配置项中带【用户配置】标识的，表示用户在开发应用时可以配置此参数。
 *
 * - 【1】 中断个数 \n
 *      请参考MCU的用户手册，填写NVIC实际支持的中断个数。
 *
 * - 【2】 ISR信息个数【用户配置】 \n
 *      此参数控制了调用 aw_int_connect() 连接ISR(中断服务函数)到此NVIC的最大次
 *      数(个数)。如果连接的ISR个数超出了这里的定义，则将返回错误，直到有ISR被断
 *      开(调用函数 aw_int_disconnect())。MCU内存资源有限时，用户可以将此参数设
 *      置为实际需要连接的ISR个数，以节约内存(1条ISR信息占用8个字节)。
 *      \note 推荐将此参数纳为"用户配置"，在 aw_prj_params.h 中配置，例如:
 *      \code
 *      #define AW_CFG_ARMCM_NVIC_ISRINFO_COUNT     20
 *      \endcode
 *
 * - 【3】【4】 中断号范围 \n
 *     【3】【4】分别定义起始中断号和结束中断号,整个系统的中断号分配不能有重叠。
 *      使用Apollo标准中断接口时，传入此范围的中断号，便可以定位到这里定义的
 *      NVIC设备。系统的中断号分配请参考 \ref grp_aw_plfm_inums 。
 */

/** @} grp_awbl_drv_tic6000_intc */

#include "awbl_intctlr.h"
#include "aw_spinlock.h"

/** \brief 驱动名 */
#define AWBL_TIC6000_INTC_NAME   "tic6000_intc"

/**
 * \brief INTC 设备信息
 */
typedef struct awbl_tic6000_intc_devinfo {

    /** \brief IntCtlr 服务相关信息 */
    struct awbl_intctlr_servinfo intctlr_servinfo;

    /** \brief Event Combiner 0-3 中断号 (4~15) */
    int  ecm_intnum[4];

} awbl_tic6000_intc_devinfo_t;

/**
 * \brief NVIC 设备实例
 */
typedef struct awbl_tic6000_intc_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev super;

    /** \brief IntCtlr 服务 */
    struct awbl_intctlr_service intctlr_serv;

    /** \brief 设备锁 */
    aw_spinlock_isr_t devlock;

} awbl_tic6000_intc_dev_t;

/**
 * \brief tic6000 intc driver register
 */
void awbl_tic6000_intc_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_TIC6000_INTC_H */

/* end of file */
