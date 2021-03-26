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
 * \brief AWBus Ti C6000 系列DSP 通用CIC控制器驱动
 *
 * C6XXX SOC 中断系统结构:
 *__________________________________________________________________________
 *|                   Peripheral                     |     CorePac          |
 *|__________________________________________________|______________________|
 *|Peripheral |         CIC                          | EventCombiner| Core  |
 *|  Rise ----|->SysInt--(map)->HostInt -(fixed-map)-|-> Event -----|-> HWI |
 *|___________|______________________________________|______________|_______|
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "tic6000_cic"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_tic6000_cic_devinfo
 *
 * \par 2.兼容设备
 *
 *  - Ti C66xx 系列DSP
 *  - 其它兼容设备
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_tic6000_cic_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_int
 *
 * \par 5.实现相关
 *
 *  - \ref grp_awbl_tic6000_cic_impl
 *
 * \internal
 * \par modification history:
 * - 1.00 14-03-06  zen, first implemetation
 * \endinternal
 */

#ifndef __AWBL_TIC6000_CIC_H
#define __AWBL_TIC6000_CIC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_intctlr.h"
#include "aw_spinlock.h"

/** \brief 驱动名 */
#define AWBL_TIC6000_CIC_NAME   "tic6000_cic"

/** \breif HostInt 映射信息 */
typedef struct awbl_tic6000_cic_hostint_map {

    /** \brief HostInt */
    int hostint;

    /** \brief Event ID */
    int event_id;
} awbl_tic6000_cic_hostint_map_t;

/** \brief 设备信息 */
typedef struct awbl_tic6000_cic_devinfo {

    /** \brief IntCtlr 服务相关信息 */
    struct awbl_intctlr_servinfo intctlr_servinfo;

    /** \brief HostInt-EventId 映射表 */
    const struct awbl_tic6000_cic_hostint_map *p_hostint_event_map;

    /** \brief HostInt-EventId 映射表成员数 */
    size_t hostint_event_map_nelements;

    /** \brief SysInt-HostInt 映射表 (成员0：SysInt0 映射的 HostInt...) */
    const int *p_sysint_hostint_map;

    /** \brief SysInt-HostInt 映射表成员数 */
    size_t sysint_hostint_map_nelements;

    /**
     *  \brief 能否将(中断)事件路由到CPU，
     * 只有为true才能连接ISR (aw_int_connect())
     */
    bool can_route_to_cpu;

} awbl_tic6000_cic_devinfo_t;

/** \brief 设备实例 */
typedef struct awbl_tic6000_cic_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev super;

    /** \brief IntCtlr 服务 */
    struct awbl_intctlr_service intctlr_serv;

    /** \brief 设备锁 */
    aw_spinlock_isr_t devlock;

} awbl_tic6000_cic_dev_t;

/** \brief tic6000 intc driver register */
void awbl_tic6000_cic_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_TIC6000_CIC_H */

/* end of file */
