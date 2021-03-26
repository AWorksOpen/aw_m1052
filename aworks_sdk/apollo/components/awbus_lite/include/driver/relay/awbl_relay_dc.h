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
 * \brief DC-Relay driver.
 *
 * �������������κο�ͨ����׼GPIO�ӿڿ��Ƶ�ֱ���̵�����
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "dc-relay"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct dc_relay_param
 *
 * \par 2.�����豸
 *
 *  - �κο���ʹ�ñ�׼GPIO�ӿڿ��Ƶ�ֱ���̵���
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_relay_dc_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_relay
 *
 * \internal
 * \par modification history
 * - 1.00 14-07-30  hbt, first implementation.
 * \endinternal
 */

#ifndef __AWBL_RELAY_DC_H
#define __AWBL_RELAY_DC_H

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_common.h"
#include "aw_types.h"
#include "awbl_relay.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_relay_dc
 * \copydetails awbl_relay_dc.h
 * @{
 */

/**
 * \defgroup  grp_awbl_relay_dc_hwconf �豸����/����
 *
 * ֱ���̵���Ӳ���豸�Ķ���������Ĵ�����ʾ���û��ڶ���Ŀ��ϵͳ��Ӳ����Դʱ��
 * ����ֱ�ӽ���δ���Ƕ�뵽 awbus_lite_hwconf_usrcfg.c �ж�Ӧ��λ�ã�Ȼ����豸
 * ��Ϣ���б�Ҫ������(�����õ���Ŀ�á�x����ʶ)��
 *
 * \note Ŀǰ��ϵͳ��ֻ��һ���̵������Ժ����Ӳ����������Ӷ���̵���

 * \include  hwconf_usrcfg_relay_dc.c
 * \note �����������д����û����á���ʶ�ģ���ʾ�û��ڿ���Ӧ��ʱ�������ô˲�����
 *
 * - ��1�� ֱ���̵����豸��Ϣ \n
 *
 *         (1) GPIO �б� \n
 *
 *         (2) �̵����豸��Ŀ \n
 *
 *         (3) �̵�����ʼ���� \n
 *
 *         (4) �̵�������״̬ \n
 *
 */

/** @} */

#define DC_RELAY_NAME  "dc-relay"

/** \brief dc-relay device platform data */
struct dc_relay_param {
    const uint16_t *relay_gpios;   /**< \brief GPIO number table */
    uint8_t         relay_num;     /**< \brief numbers of relays */
    uint8_t         base_num;      /**< \brief base number of relay */
    uint8_t         active_low;    /**< \brief driven polarity of the GPIO */
};

/** \brief dc-relay device */
struct dc_relay {
    struct  awbl_dev     dev;        /**< \brief the AWBus device */
    struct  dc_relay    *next;        /**< \brief the next relay device */
};

/** \brief get dc-relay parameter from device */
#define dev_get_dc_relay_par(p_dev)    ((void *)AWBL_DEVINFO_GET(p_dev))

/** \brief convert a awbl_dev pointer to dc_relay pointer */
#define dev_to_dc_relay(p_dev)    AW_CONTAINER_OF(p_dev, struct dc_relay, dev)

/** \brief convert a relay_dev pointer to dc_relay pointer */
#define relay_to_dc_relay(dev)   ((struct dc_relay *)(dev))

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_RELAY_DC_H */

/* end of file */
