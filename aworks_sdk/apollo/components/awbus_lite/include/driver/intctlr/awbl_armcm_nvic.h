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
 * \brief AWBus ARM Cortex-M 系列CPU 通用NVIC驱动
 *
 * NVIC(嵌套向量中断控制器)是 ARM Cortex-M 系列处理器的一部分。与CPU的紧密耦合
 * 允许更低的中断延迟以及对迟到中断的有效处理。
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "armcm_nvic"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_armcm_nvic_devinfo
 *
 * \par 2.兼容设备
 *
 *  - Cortex-M0 CPU
 *  - Cortex-M1 CPU
 *  - Cortex-M3 CPU
 *  - Cortex-M4 CPU
 *  - 其它兼容设备
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_armcm_nvic_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_gpio
 *
 * \par 5.实现相关
 *
 *  - \ref grp_awbl_armcm_nvic_impl
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-06  zen, first implemetation
 * \endinternal
 */

#ifndef __AWBL_ARMCM_NVIC_H
#define __AWBL_ARMCM_NVIC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/**
\addtogroup grp_awbl_drv_armcm_nvic
\copydetails awbl_armcm_nvic.h
@{
*/

/**
 * \defgroup grp_awbl_armcm_nvic_impl  实现相关
 *
 * \par 1. 特性
 *
 *  - 本实现只提供对外设中断的控制，NVIC 的异常由 \ref grp_aw_serv_exc 处理
 *  - 每个中断只能连接1个中断服务函数
 *  - 不提供中断优先级的设置
 */

/**
 * \addtogroup grp_awbl_armcm_nvic_hwconf  设备定义/配置
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

/** @} grp_awbl_drv_armcm_nvic */

#include "awbl_intctlr.h"
#include "aw_spinlock.h"

#define AWBL_ARMCM_NVIC_NAME   "armcm_nvic"

/**
 * \brief NVIC 设备信息
 */
typedef struct awbl_armcm_nvic_devinfo {

    /** \brief IntCtlr 服务相关信息 */
    struct awbl_intctlr_servinfo    intctlr_servinfo;

    /** \brief ISR 信息条数 */
    int                             isrinfo_cnt;
    /** \brief ISR 信息槽内存 (大小等于 isrinfo_cnt) */
    struct awbl_armcm_nvic_isrinfo *p_isrinfo;

    /** \brief 内存异常向量表基地址，其个数应该等于isrinfo_cnt + 保留异常数量 */
    /** \brief 如果不支持直接修改异常向量表，此地址可以为NULL*/
    uint32_t                       *p_ram_vector_base;
    /** \brief 内存异常向量表最后一个元素的地址 */
    uint32_t                       *p_ram_vector_end;
} awbl_armcm_nvic_devinfo_t;

/**
 * \brief ISR information
 */
struct awbl_armcm_nvic_isrinfo {

    /** \brief trigger callback function */
    aw_pfuncvoid_t  pfunc_isr;

    /** \brief argument for\a  pfunc_callback */
    void *p_arg;
};

/**
 * \brief NVIC 设备实例
 */
typedef struct awbl_armcm_nvic_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev super;

    /** \brief IntCtlr 服务 */
    struct awbl_intctlr_service intctlr_serv;

} awbl_armcm_nvic_dev_t;

/**
 * \brief NVIC driver register
 */
void awbl_armcm_nvic_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_ARMCM_NVIC_H */

/* end of file */
