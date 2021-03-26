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
 * \brief MII BUS
 *
 * \internal
 * \par modification history
 * - 1.00 15-04-01  axn, first implementation
 * \endinternal
 */

#ifndef AWBL_MIIBUS_H_
#define AWBL_MIIBUS_H_

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus */

#include "aw_list.h"
#include "aw_sem.h"
#include "aw_mii.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_miilib.h"

/**
 * PHY Link parameters
 */
#define AWBL_PHY_LINK_AUTONEG        AWBL_IFM_AUTO
/** \brief 1000M/Full */
#define AWBL_PHY_LINK_1000M_FULL     AWBL_IFM_1000_T | AWBL_IFM_FDX
/** \brief 1000M/Half */  
#define AWBL_PHY_LINK_1000M_HALF     AWBL_IFM_1000_T | AWBL_IFM_HDX  
/** \brief 100M/Full  */
#define AWBL_PHY_LINK_100M_FULL      AWBL_IFM_100_TX | AWBL_IFM_FDX  
/** \brief 100M/Half  */
#define AWBL_PHY_LINK_100M_HALF      AWBL_IFM_100_TX | AWBL_IFM_HDX  
/** \brief 10M/Full */
#define AWBL_PHY_LINK_10M_FULL       AWBL_IFM_10_T   | AWBL_IFM_FDX  
/** \brief 10M/Half */
#define AWBL_PHY_LINK_10M_HALF       AWBL_IFM_10_T   | AWBL_IFM_HDX  

#define AWBL_MII_BUS_NAME            "awbl_mii_bus"

/** \biref AWBus MII 总线控制器 设备信息 */
struct awbl_mii_master_devinfo {

    /** \brief 控制器所对应的总线编号 */
    uint8_t     bus_index;
};


struct awbl_mii_master;

/** \brief PHY驱动提供的基本操作 */
typedef struct awbl_mii_master_ops {

    aw_err_t (*pfn_mii_phy_write) (struct awbl_mii_master *p_master,
                                   int                     phy_addr,
                                   int                     phy_reg,
                                   uint16_t                reg_val);

    aw_err_t (*pfn_mii_phy_read) (struct awbl_mii_master *p_master,
                                  int                     phy_addr,
                                  int                     phy_reg,
                                  uint16_t               *p_reg_val);
} awbl_mii_master_ops_t;


struct awbl_mii_master {
    struct awbl_busctlr     super;        /**< \brief 继承自 AWBus 总线控制器*/
    struct awbl_mii_master *p_next;       /**< \brief 指向下一个MII总线控制器*/

    struct awbl_dev        *p_mii_parent; /**< \brief 指向父亲设备  */

    struct awbl_mii_master_devinfo mii_dev_info;

    struct awbl_devhcf      mii_devhcf;   /**< \brief 硬件设备信息  */

    AW_MUTEX_DECL(dev_miibus_mutex);      /**< \brief 控制器互斥量 */

    struct awbl_phy_device *p_phy_dev_list_head;

    const struct awbl_mii_master_ops *p_mii_master_ops;
};

typedef aw_err_t (*awbl_miibus_mode_get_t)(struct awbl_phy_device *p_phy_dev,
                                           uint32_t               *p_mode,
                                           uint32_t               *p_sts
                                           );

typedef aw_err_t (*awbl_miibus_mode_set_t)(struct awbl_phy_device *p_phy_dev,
                                           uint32_t                mode);

typedef aw_err_t (*awbl_miibus_notice_t)(struct awbl_phy_device  *p_phy_dev,
                                         uint32_t                 p_mode,
                                         uint32_t                 p_sts);

typedef aw_err_t (*awbl_miibus_phy_up_t)(struct awbl_phy_device  *p_phy_dev);

typedef aw_err_t (*awbl_miibus_phy_down_t)(struct awbl_phy_device  *p_phy_dev);


struct   attach_dev {

    void *p_enet_dev;

    aw_err_t (*pfn_update_link) (void                    *p_dev,
                                 uint32_t                 p_mode,
                                 uint32_t                 p_sts);
};


/**
 * \brief PHY 设备实例
 */
struct awbl_phy_device {
    struct awbl_dev         phy_dev;

    struct awbl_phy_device *p_next;

    struct attach_dev       phy_attach_dev; /**< \brief phy和网卡匹配后保存网卡的必要信息 */

    const char             *p_ethif_name;   /** \brief 网卡名，如eth0 */

    uint32_t                phy_addr;       /**< \brief phy的地址(在mii bus层会用到) */

    uint32_t                phy_mode;       /**< \brief phy的模式 保存 */
    uint32_t                phy_state;      /**< \brief phy的状态保存  */
};


/**
 * \brief MII bus 初始化
 */
void     awbl_miibus_init(void);


/**
 * \brief 创建一个MII bus
 *
 * \param p_dev     指定的设备
 * \param p_master  总线设备空间
 * \param bus_index 总线的编号
 * \param p_mii_ops 提供MDIO接口操作函数
 *
 * \return AW_OK 成功    负值  失败(查看aw_errno.h所对应的错误码)
 */
aw_err_t awbl_miibus_create(struct awbl_dev                 *p_dev,
                            struct awbl_mii_master          *p_master,
                            uint8_t                          bus_index,
                            aw_const awbl_mii_master_ops_t  *p_mii_ops);


/**
 * \brief 读取PHY寄存器
 *
 * \param p_phy_dev MII总线控制器
 * \param phy_reg  PHY寄存器
 * \param p_reg_val  存储读取的寄存器值
 *
 * \return AW_OK 成功    负值  失败(查看aw_errno.h所对应的错误码)
 */
aw_err_t awbl_miibus_read (struct awbl_phy_device  *p_phy_dev,
                           int                      phy_reg,
                           uint16_t                *p_reg_val);


/**
 * \brief 写PHY寄存器
 *
 * \param p_phy_dev PHY设备
 * \param phy_reg   PHY寄存器
 * \param reg_val   要写入的寄存器值
 *
 * \return AW_OK 成功    负值  失败(查看aw_errno.h所对应的错误码)
 */
aw_err_t awbl_miibus_write (struct awbl_phy_device  *p_phy_dev,
                            int                      phy_reg,
                            uint16_t                 p_reg_val);


/**
 * \brief 获取PHY的模式和状态，此操作将会使用硬件读取PHY寄存器
 *
 * \param p_phy_dev PHY设备
 * \param p_mode    存放模式信息
 * \param p_sts     存放连接状态信息
 *
 * \return AW_OK 成功    负值  失败(查看aw_errno.h所对应的错误码)
 */
aw_err_t awbl_miibus_mode_get (struct awbl_phy_device *p_phy_dev,
                               uint32_t               *p_mode,
                               uint32_t               *p_sts);


/**
 * \brief 设置PHY的工作模式
 *
 * \param p_phy_dev PHY设备
 * \param mode  模式信息
 *
 * \return AW_OK 成功    负值  失败(查看aw_errno.h所对应的错误码)
 */
aw_err_t awbl_miibus_mode_set (struct awbl_phy_device  *p_phy_dev,
                               uint32_t                 mode);


/**
 * \brief 通知PHY设备状态改变
 *
 * \param p_phy_dev PHY设备
 * \param mode      模式信息
 * \param stat      连接状态信息
 *
 * \return AW_OK 成功    负值  失败(查看aw_errno.h所对应的错误码)
 */
aw_err_t awbl_miibus_notice (struct awbl_phy_device *p_phy_dev,
                             uint32_t                mode,
                             uint32_t                stat);


/**
 * \brief 通知eth设备状态更新
 *
 * \param p_phy_dev PHY设备
 * \param mode  模式信息
 * \param stat  连接状态信息
 *
 * \return AW_OK 成功    负值  失败(查看aw_errno.h所对应的错误码)
 */
aw_err_t awbl_miibus_update (struct awbl_phy_device *p_phy_dev);


/**
 * \brief 获取PHY的工作模式和连接状态，此操作仅仅是获得最新保存的状态信息，
 *        并不会立即读取PHY的寄存器
 *
 * \param p_phy_dev PHY设备
 * \param p_mode    模式信息
 * \param p_stat    状态信息
 * \return AW_OK 成功    负值  失败(查看aw_errno.h所对应的错误码)
 */
aw_err_t awbl_miibus_info_get(struct awbl_phy_device *p_phy_dev,
                              uint32_t               *p_mode,
                              uint32_t               *p_stat);


/**
 * \brief 通过MII bus ID 获取MII总线控制器
 *
 * \param mii_bus_id MII bus编号（ID）
 *
 * \return MII总线控制器，NULL 没有找到
 */
struct awbl_mii_master *awbl_miibus_get_master_by_id(uint8_t mii_bus_id);


/**
 * \brief 向MII bus中增加一个查询PHY设备
 *
 * \param p_master MII总线控制器
 * \param p_phy_dev PHY设备
 *
 * \return AW_OK 成功    负值  失败(查看aw_errno.h所对应的错误码)
 */
aw_err_t awbl_miibus_add_phy(struct awbl_mii_master *p_master,
                             struct awbl_phy_device *p_phy_dev);


/**
 * \brief PHY 正常模式
 *
 * \param p_phy_dev PHY设备
 *
 * \return AW_OK 成功    负值  失败(查看aw_errno.h所对应的错误码)
 */
aw_err_t awbl_miibus_phy_up (struct awbl_phy_device *p_phy_dev);


/**
 * \brief PHY power down模式
 *
 * \param p_phy_dev PHY设备
 *
 * \return AW_OK 成功    负值  失败(查看aw_errno.h所对应的错误码)
 */
aw_err_t awbl_miibus_phy_down (struct awbl_phy_device *p_phy_dev);


#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif /* AWBL_MIIBUS_H_ */

/* end of file */

