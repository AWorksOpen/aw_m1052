/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
*
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx FLEXCANFD
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-29  hsg, first implementation
 * \endinternal
 */

#include "awbl_can.h"
#include "awbus_lite.h"

#ifndef __AWBL_IMX10xx_CANFD_H
#define __AWBL_IMX10xx_CANFD_H

#define AWBL_IMX10XX_CANFD_NAME    "imx10xx_canfd"


#define CANFD_AND_CAN_SWITCH_DYNAMICALLY

/** \brief imx10xx canfd initialization structure for plb based AWBus devices */
typedef struct awbl_imx10xx_canfd_dev_info {

    awbl_can_servinfo_t  servinfo;
    uint32_t             reg_base;
    uint8_t              int_num;

    void   (*pfn_plfm_init)(void);
} awbl_imx10xx_canfd_dev_info_t;

/** \brief imx10xx canfd device channel structure for awbus lite */
typedef struct awbl_imx10xx_canfd_dev {

    /**< \brief device */
    struct awbl_dev  dev;

     /** \brief driver functions, always goes first */
    struct awbl_can_service can_serv;

    /**< \brief ���ڼ�¼�������� */
    uint8_t sendtype;

    /** \brief ���ڼ�¼��ǰ�˲���ĳ��� */
    uint32_t filter_table_len;

#ifdef CANFD_AND_CAN_SWITCH_DYNAMICALLY
    /** \brief ���ڼ�¼��ǰ������������ */
    uint8_t ctrl_type;
#endif
} awbl_imx10xx_canfd_dev_t;

/**
 * \brief iMX RT10xx CANFD ����ע��
 */
void awbl_imx10xx_flexcanfd_drv_register (void);


#ifdef CANFD_AND_CAN_SWITCH_DYNAMICALLY
/**
 * \brief canfd���������Ͷ�̬�л�����
 *
 * \note ʹ�ô˽ӿ�ʱ��Ҫע�⣬ֻ���������ļ�(�磺awbl_hwconf_imx1064_flexcan3.h)�н�CANFD������
 *       Ĭ������ΪCANFD������ʱ���ſ��Ե��ô˽ӿڽ��˿�������CANFD��CAN֮�䶯̬�л���
 *       ����������ļ���Ĭ�Ͻ�CANFD����������ΪCAN������ʱ�����ô˺�������̬�л��ǽ�����ʧ�ܡ�
 *
 * \param[in] chn   CANͨ����
 *
 * \param[in] type  Ҫ�л��Ŀ���������
 *
 * \return  AW_CAN_ERR_NONE                 �л��ɹ�
 *         -AW_CAN_ERR_NO_DEV               �豸������ �� �˿�����Ĭ����CAN���������޷����л�
 *         -AW_CAN_ERR_ILLEGAL_CTRL_TYPE    type��������
 */
aw_can_err_t imxrt10xx_canfd_ctrl_type_switch (int chn, uint8_t type);
#endif

#endif /* __AWBL_IMX10xx_CANFD_H */





