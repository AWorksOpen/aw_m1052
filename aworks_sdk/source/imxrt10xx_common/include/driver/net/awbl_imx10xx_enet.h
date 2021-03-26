/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus-lite iMX RT10xx ENET 驱动
 *
 * \internal
 * \par modification history:
 * - 1.10 18-04-17, phd, migrate to aw_netif
 * - 1.00 2017-11-07, mex, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_ENET_H
#define __AWBL_IMX10xx_ENET_H

#include "aw_assert.h"
#include "aw_isr_defer.h"
#include "aw_msgq.h"
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "aw_task.h"
#include "awbl_miibus.h"

#include "aw_netif.h"

#include "driver/net/imx10xx_enet_regs.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/** \brief iMX RT10xx ENET 驱动名称 */
#define AWBL_IMX10XX_ENET_NAME       "awbl_imx10xx_enet"
#define AWBL_IMX10XX_ENET_RX_THREAD_STK_SIZE    2048

/** \brief PHY接口类型 */
typedef enum {
    PHY_INTERFACE_MODE_NA,
    PHY_INTERFACE_MODE_MII,
    PHY_INTERFACE_MODE_GMII,
    PHY_INTERFACE_MODE_SGMII,
    PHY_INTERFACE_MODE_TBI,
    PHY_INTERFACE_MODE_REVMII,
    PHY_INTERFACE_MODE_RMII,
    PHY_INTERFACE_MODE_RGMII,
    PHY_INTERFACE_MODE_RGMII_ID,
    PHY_INTERFACE_MODE_RGMII_RXID,
    PHY_INTERFACE_MODE_RGMII_TXID,
    PHY_INTERFACE_MODE_RTBI,
    PHY_INTERFACE_MODE_SMII,
    PHY_INTERFACE_MODE_XGMII,
} phy_interface_t;

/** \brief iMX RT10xx ENET 设备信息 */
struct awbl_imx10xx_enet_devinfo {
    const char                 *p_nic_name;
    const aw_netif_info_get_entry_t *get_info_entry;
    int                         rx_task_prio;           /**< \brief 接收任务优先级 */

    uint32_t                    emac_regbase;           /**< \brief emac 寄存器基地址 */
    uint32_t                    inum;                   /**< \brief 中断向量号 */
    uint32_t                    clk_id;                 /**< \brief 时钟ID */
    uint32_t                    fec_rbd_num;            /**< \brief 接收缓冲区描述符的个数(最小为2)  */
    uint32_t                    fec_tbd_num;            /**< \brief 发送缓冲区描述符的个数(最小为2)  */
    int                         create_mii_bus_id;      /**< \brief 此EMAC创建的MII bus编号 */
    phy_interface_t             interface_mode;         /**< \brief AWBL_PHY_RMII_MODE or AWBL_PHY_MII_MODE */
    void                      (*pfn_plfm_init) (void);  /**< \brief 平台相关初始化 */
};

/** \brief 使用的接收缓冲区描述符的个数 */
#define FEC_RBD_NUM             64
/** \brief 使用的发送缓冲区描述符的个数 */
#define FEC_TBD_NUM             64

/** \brief iMX RT10xx enet 设备 */
typedef struct awbl_imx10xx_enet_dev {
    struct awbl_dev         dev;                /**< \brief 平台相关初始化函数 */
    aw_netif_t              ethif;              /**< \brief 对应LWIP 以太网接口 */
    struct awbl_mii_master  mii_master;         /**< \brief mii总线 */

    AW_SEMB_DECL(mii_sem);                      /**< \brief 标志MII操作完成的信号量 */

    struct ethernet_regs   *eth;                /**< \brief enet寄存器基地址 */
    phy_interface_t         phy_interface;      /**< \brief PHY的接口类型，RMII或MII */
    uint32_t                phy_mode;           /**< \brief PHY的模式，主要指速度和双工模式 */
    uint32_t                phy_stat;           /**< \brief PHY的状态 */

    struct fec_bd          *rbd_base;           /**< \brief 接收描述符的基地址 */
    int                     rbd_index;          /**< \brief 下一个要读的接收描述符 */
    AW_MUTEX_DECL(rx_lock);                     /**< \brief 同步接收操作的互斥锁 */

    struct fec_bd          *tbd_base;           /**< \brief 发送描述符的基地址 */
    int                     tbd_head;           /**< \brief 下一个可以被使用的发送描述符 */
    int                     tbd_tail;           /**< \brief 第一个正在被使用的描述符，实际组成了循环队列 */
    AW_MUTEX_DECL(tx_lock);                     /**< \brief 同步发送操作的互斥锁 */

    volatile int            emac_init_flg;      /**< \brief 初始化完成标志 */
    int                     inum;               /**< \brief 本设备对应的中断编号 */

    AW_TASK_DECL(rx_task, AWBL_IMX10XX_ENET_RX_THREAD_STK_SIZE);
    AW_SEMB_DECL(rx_semb);
} awbl_imx10xx_enet_dev_t;

void awbl_imx10xx_enet_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_IMX10xx_ENET_H */


