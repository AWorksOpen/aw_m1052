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

/** \biref AWBus MII ���߿����� �豸��Ϣ */
struct awbl_mii_master_devinfo {

    /** \brief ����������Ӧ�����߱�� */
    uint8_t     bus_index;
};


struct awbl_mii_master;

/** \brief PHY�����ṩ�Ļ������� */
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
    struct awbl_busctlr     super;        /**< \brief �̳��� AWBus ���߿�����*/
    struct awbl_mii_master *p_next;       /**< \brief ָ����һ��MII���߿�����*/

    struct awbl_dev        *p_mii_parent; /**< \brief ָ�����豸  */

    struct awbl_mii_master_devinfo mii_dev_info;

    struct awbl_devhcf      mii_devhcf;   /**< \brief Ӳ���豸��Ϣ  */

    AW_MUTEX_DECL(dev_miibus_mutex);      /**< \brief ������������ */

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
 * \brief PHY �豸ʵ��
 */
struct awbl_phy_device {
    struct awbl_dev         phy_dev;

    struct awbl_phy_device *p_next;

    struct attach_dev       phy_attach_dev; /**< \brief phy������ƥ��󱣴������ı�Ҫ��Ϣ */

    const char             *p_ethif_name;   /** \brief ����������eth0 */

    uint32_t                phy_addr;       /**< \brief phy�ĵ�ַ(��mii bus����õ�) */

    uint32_t                phy_mode;       /**< \brief phy��ģʽ ���� */
    uint32_t                phy_state;      /**< \brief phy��״̬����  */
};


/**
 * \brief MII bus ��ʼ��
 */
void     awbl_miibus_init(void);


/**
 * \brief ����һ��MII bus
 *
 * \param p_dev     ָ�����豸
 * \param p_master  �����豸�ռ�
 * \param bus_index ���ߵı��
 * \param p_mii_ops �ṩMDIO�ӿڲ�������
 *
 * \return AW_OK �ɹ�    ��ֵ  ʧ��(�鿴aw_errno.h����Ӧ�Ĵ�����)
 */
aw_err_t awbl_miibus_create(struct awbl_dev                 *p_dev,
                            struct awbl_mii_master          *p_master,
                            uint8_t                          bus_index,
                            aw_const awbl_mii_master_ops_t  *p_mii_ops);


/**
 * \brief ��ȡPHY�Ĵ���
 *
 * \param p_phy_dev MII���߿�����
 * \param phy_reg  PHY�Ĵ���
 * \param p_reg_val  �洢��ȡ�ļĴ���ֵ
 *
 * \return AW_OK �ɹ�    ��ֵ  ʧ��(�鿴aw_errno.h����Ӧ�Ĵ�����)
 */
aw_err_t awbl_miibus_read (struct awbl_phy_device  *p_phy_dev,
                           int                      phy_reg,
                           uint16_t                *p_reg_val);


/**
 * \brief дPHY�Ĵ���
 *
 * \param p_phy_dev PHY�豸
 * \param phy_reg   PHY�Ĵ���
 * \param reg_val   Ҫд��ļĴ���ֵ
 *
 * \return AW_OK �ɹ�    ��ֵ  ʧ��(�鿴aw_errno.h����Ӧ�Ĵ�����)
 */
aw_err_t awbl_miibus_write (struct awbl_phy_device  *p_phy_dev,
                            int                      phy_reg,
                            uint16_t                 p_reg_val);


/**
 * \brief ��ȡPHY��ģʽ��״̬���˲�������ʹ��Ӳ����ȡPHY�Ĵ���
 *
 * \param p_phy_dev PHY�豸
 * \param p_mode    ���ģʽ��Ϣ
 * \param p_sts     �������״̬��Ϣ
 *
 * \return AW_OK �ɹ�    ��ֵ  ʧ��(�鿴aw_errno.h����Ӧ�Ĵ�����)
 */
aw_err_t awbl_miibus_mode_get (struct awbl_phy_device *p_phy_dev,
                               uint32_t               *p_mode,
                               uint32_t               *p_sts);


/**
 * \brief ����PHY�Ĺ���ģʽ
 *
 * \param p_phy_dev PHY�豸
 * \param mode  ģʽ��Ϣ
 *
 * \return AW_OK �ɹ�    ��ֵ  ʧ��(�鿴aw_errno.h����Ӧ�Ĵ�����)
 */
aw_err_t awbl_miibus_mode_set (struct awbl_phy_device  *p_phy_dev,
                               uint32_t                 mode);


/**
 * \brief ֪ͨPHY�豸״̬�ı�
 *
 * \param p_phy_dev PHY�豸
 * \param mode      ģʽ��Ϣ
 * \param stat      ����״̬��Ϣ
 *
 * \return AW_OK �ɹ�    ��ֵ  ʧ��(�鿴aw_errno.h����Ӧ�Ĵ�����)
 */
aw_err_t awbl_miibus_notice (struct awbl_phy_device *p_phy_dev,
                             uint32_t                mode,
                             uint32_t                stat);


/**
 * \brief ֪ͨeth�豸״̬����
 *
 * \param p_phy_dev PHY�豸
 * \param mode  ģʽ��Ϣ
 * \param stat  ����״̬��Ϣ
 *
 * \return AW_OK �ɹ�    ��ֵ  ʧ��(�鿴aw_errno.h����Ӧ�Ĵ�����)
 */
aw_err_t awbl_miibus_update (struct awbl_phy_device *p_phy_dev);


/**
 * \brief ��ȡPHY�Ĺ���ģʽ������״̬���˲��������ǻ�����±����״̬��Ϣ��
 *        ������������ȡPHY�ļĴ���
 *
 * \param p_phy_dev PHY�豸
 * \param p_mode    ģʽ��Ϣ
 * \param p_stat    ״̬��Ϣ
 * \return AW_OK �ɹ�    ��ֵ  ʧ��(�鿴aw_errno.h����Ӧ�Ĵ�����)
 */
aw_err_t awbl_miibus_info_get(struct awbl_phy_device *p_phy_dev,
                              uint32_t               *p_mode,
                              uint32_t               *p_stat);


/**
 * \brief ͨ��MII bus ID ��ȡMII���߿�����
 *
 * \param mii_bus_id MII bus��ţ�ID��
 *
 * \return MII���߿�������NULL û���ҵ�
 */
struct awbl_mii_master *awbl_miibus_get_master_by_id(uint8_t mii_bus_id);


/**
 * \brief ��MII bus������һ����ѯPHY�豸
 *
 * \param p_master MII���߿�����
 * \param p_phy_dev PHY�豸
 *
 * \return AW_OK �ɹ�    ��ֵ  ʧ��(�鿴aw_errno.h����Ӧ�Ĵ�����)
 */
aw_err_t awbl_miibus_add_phy(struct awbl_mii_master *p_master,
                             struct awbl_phy_device *p_phy_dev);


/**
 * \brief PHY ����ģʽ
 *
 * \param p_phy_dev PHY�豸
 *
 * \return AW_OK �ɹ�    ��ֵ  ʧ��(�鿴aw_errno.h����Ӧ�Ĵ�����)
 */
aw_err_t awbl_miibus_phy_up (struct awbl_phy_device *p_phy_dev);


/**
 * \brief PHY power downģʽ
 *
 * \param p_phy_dev PHY�豸
 *
 * \return AW_OK �ɹ�    ��ֵ  ʧ��(�鿴aw_errno.h����Ӧ�Ĵ�����)
 */
aw_err_t awbl_miibus_phy_down (struct awbl_phy_device *p_phy_dev);


#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif /* AWBL_MIIBUS_H_ */

/* end of file */

