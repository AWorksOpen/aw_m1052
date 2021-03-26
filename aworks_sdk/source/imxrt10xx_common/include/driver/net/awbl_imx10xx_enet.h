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
 * \brief AWBus-lite iMX RT10xx ENET ����
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


/** \brief iMX RT10xx ENET �������� */
#define AWBL_IMX10XX_ENET_NAME       "awbl_imx10xx_enet"
#define AWBL_IMX10XX_ENET_RX_THREAD_STK_SIZE    2048

/** \brief PHY�ӿ����� */
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

/** \brief iMX RT10xx ENET �豸��Ϣ */
struct awbl_imx10xx_enet_devinfo {
    const char                 *p_nic_name;
    const aw_netif_info_get_entry_t *get_info_entry;
    int                         rx_task_prio;           /**< \brief �����������ȼ� */

    uint32_t                    emac_regbase;           /**< \brief emac �Ĵ�������ַ */
    uint32_t                    inum;                   /**< \brief �ж������� */
    uint32_t                    clk_id;                 /**< \brief ʱ��ID */
    uint32_t                    fec_rbd_num;            /**< \brief ���ջ������������ĸ���(��СΪ2)  */
    uint32_t                    fec_tbd_num;            /**< \brief ���ͻ������������ĸ���(��СΪ2)  */
    int                         create_mii_bus_id;      /**< \brief ��EMAC������MII bus��� */
    phy_interface_t             interface_mode;         /**< \brief AWBL_PHY_RMII_MODE or AWBL_PHY_MII_MODE */
    void                      (*pfn_plfm_init) (void);  /**< \brief ƽ̨��س�ʼ�� */
};

/** \brief ʹ�õĽ��ջ������������ĸ��� */
#define FEC_RBD_NUM             64
/** \brief ʹ�õķ��ͻ������������ĸ��� */
#define FEC_TBD_NUM             64

/** \brief iMX RT10xx enet �豸 */
typedef struct awbl_imx10xx_enet_dev {
    struct awbl_dev         dev;                /**< \brief ƽ̨��س�ʼ������ */
    aw_netif_t              ethif;              /**< \brief ��ӦLWIP ��̫���ӿ� */
    struct awbl_mii_master  mii_master;         /**< \brief mii���� */

    AW_SEMB_DECL(mii_sem);                      /**< \brief ��־MII������ɵ��ź��� */

    struct ethernet_regs   *eth;                /**< \brief enet�Ĵ�������ַ */
    phy_interface_t         phy_interface;      /**< \brief PHY�Ľӿ����ͣ�RMII��MII */
    uint32_t                phy_mode;           /**< \brief PHY��ģʽ����Ҫָ�ٶȺ�˫��ģʽ */
    uint32_t                phy_stat;           /**< \brief PHY��״̬ */

    struct fec_bd          *rbd_base;           /**< \brief �����������Ļ���ַ */
    int                     rbd_index;          /**< \brief ��һ��Ҫ���Ľ��������� */
    AW_MUTEX_DECL(rx_lock);                     /**< \brief ͬ�����ղ����Ļ����� */

    struct fec_bd          *tbd_base;           /**< \brief �����������Ļ���ַ */
    int                     tbd_head;           /**< \brief ��һ�����Ա�ʹ�õķ��������� */
    int                     tbd_tail;           /**< \brief ��һ�����ڱ�ʹ�õ���������ʵ�������ѭ������ */
    AW_MUTEX_DECL(tx_lock);                     /**< \brief ͬ�����Ͳ����Ļ����� */

    volatile int            emac_init_flg;      /**< \brief ��ʼ����ɱ�־ */
    int                     inum;               /**< \brief ���豸��Ӧ���жϱ�� */

    AW_TASK_DECL(rx_task, AWBL_IMX10XX_ENET_RX_THREAD_STK_SIZE);
    AW_SEMB_DECL(rx_semb);
} awbl_imx10xx_enet_dev_t;

void awbl_imx10xx_enet_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_IMX10xx_ENET_H */


