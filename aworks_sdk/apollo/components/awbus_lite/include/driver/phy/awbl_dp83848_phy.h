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
#ifndef __AWBL_DP83848_PHY_H
#define __AWBL_DP83848_PHY_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "apollo.h"
#include "aw_assert.h"

#include "awbl_miibus.h"

#define AWBL_DP83848_PHY_NAME   "awbl_dp83848_phy"


/**
 * \brief 通用PHY 设备信息
 */
struct awbl_dp83848_phy_devinfo {

    /** \brief 与phy匹配的网卡名，如: eth0 */
    const     char *p_ethif_name;

    uint32_t  phy_addr; /**< \brief 0 - 31 */

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

    /** 平台相关初始化 */
    void (*pfunc_plfm_init)(void);
};

/**
 * \brief 通用PHY 设备实例，继承自awbl_phy_device
 */
struct awbl_dp83848_phy_dev {
    struct awbl_phy_device phy_dev;

};

void awbl_dp83848_phy_drv_register(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_DP83848_PHY_H */

/* end of file */
