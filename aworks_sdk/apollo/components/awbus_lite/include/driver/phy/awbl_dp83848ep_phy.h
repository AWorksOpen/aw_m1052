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
 * \brief driver for generic 10/100/1000 ethernet PHY chips
 *
 * \internal
 * \par modification history
 * - 1.00 15-04-01  axn, first implementation
 * \endinternal
 */
#ifndef __AWBL_DP83848EP_PHY_H
#define __AWBL_DP83848EP_PHY_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus    */

#include "apollo.h"
#include "aw_assert.h"

#include "awbl_miibus.h"

#define AWBL_DP83848EP_PHY_NAME   "awbl_dp83848ep_phy"

#define __PHY_MONITOR_STACK_SIZE   (1024)

struct awbl_dp83848ep_int_info {
    int phy_int_pin;
};


/**
 * \brief 通用PHY 设备信息
 */
struct awbl_dp83848ep_phy_devinfo {

    /** \brief 与phy匹配的网卡名，如: eth0 */
    const     char *p_ethif_name;

    uint32_t  phy_addr; /* 0 - 31 */

    /**
     * AWBL_PHY_LINK_AUTONEG       : Auto-Negotiation
     * AWBL_PHY_LINK_1000M_FULL    : 1000M/Full
     * AWBL_PHY_LINK_1000M_HALF    : 1000M/Half
     * AWBL_PHY_LINK_100M_FULL     : 100M/Full
     * AWBL_PHY_LINK_100M_HALF     : 100M/Half
     * AWBL_PHY_LINK_10M_FULL      : 10M/Full
     * AWBL_PHY_LINK_10M_HALF      : 10M/Half
     */
    uint32_t  phy_mode_set;

    /** \brief 如果为空则PHY的状态采用查询方式, 否则使用中断方式 */
    const struct awbl_dp83848ep_int_info *p_phy_int_info;

    /** \brief 平台相关初始化 */
    void (*pfunc_plfm_init)(void);
};

/**
 * \brief 通用PHY 设备实例，继承自awbl_phy_device
 */
struct awbl_dp83848ep_phy_dev {
    struct awbl_phy_device phy_dev;

    AW_TASK_DECL(phy_monitor_task, __PHY_MONITOR_STACK_SIZE);

    /** \brief 检测phy是否产生中断 */
    AW_SEMB_DECL(phy_smb);

};

void awbl_dp83848ep_phy_drv_register(void);

#ifdef __cplusplus
}
#endif /* __cplusplus     */

#endif /* __AWBL_DP83848EP_PHY_H */

/* end of file */

