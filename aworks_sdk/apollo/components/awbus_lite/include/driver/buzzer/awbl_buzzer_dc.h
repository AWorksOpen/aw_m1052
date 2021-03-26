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
 * \brief DC-Buzzer driver.
 *
 * �������������κο�ͨ����׼GPIO�ӿڿ��Ƶ�ֱ����������
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "dc-buzzer"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct dc_buzzer_param
 *
 * \par 2.�����豸
 *
 *  - �κο���ʹ�ñ�׼GPIO�ӿڿ��Ƶ�ֱ��������
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_buzzer_dc_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_buzzer
 *
 * \internal
 * \par modification history
 * - 1.00 13-03-08  orz, first implementation.
 * \endinternal
 */

#ifndef __AWBL_BUZZER_DC_H
#define __AWBL_BUZZER_DC_H

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_buzzer.h"
#include "aw_common.h"
#include "aw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_buzzer_dc
 * \copydetails awbl_buzzer_dc.h
 * @{
 */

/**
 * \defgroup  grp_awbl_buzzer_dc_hwconf �豸����/����
 *
 * ֱ��������Ӳ���豸�Ķ���������Ĵ�����ʾ���û��ڶ���Ŀ��ϵͳ��Ӳ����Դʱ��
 * ����ֱ�ӽ���δ���Ƕ�뵽 awbus_lite_hwconf_usrcfg.c �ж�Ӧ��λ�ã�Ȼ����豸
 * ��Ϣ���б�Ҫ������(�����õ���Ŀ�á�x����ʶ)��
 *
 * \note һ��ϵͳ��ֻ����һ��������

 * \include  hwconf_usrcfg_buzzer_dc.c
 * \note �����������д����û����á���ʶ�ģ���ʾ�û��ڿ���Ӧ��ʱ�������ô˲�����
 *
 * - ��1�� ����ֱ����������GPIO \n
 * ��������д����ֱ����������GPIO����Լ�ʹ���������еĵ�ƽ��
 */

/** @} */

#define DC_BUZZER_NAME  "dc-buzzer"

/** \brief dc-buzzer device platform data */
struct dc_buzzer_param {
    uint16_t    gpio;              /**< \brief GPIO number the buzzer is driven */
    uint16_t    active_low;        /**< \brief driven polarity of the GPIO */
    void (*pfn_plfm_init) (void);  /**< \brief platform initializing */
};

/** \brief get dc-buzzer parameter from device */
#define dev_get_dc_buzzer_par(p_dev)    ((void *)AWBL_DEVINFO_GET(p_dev))

/** \brief dc-buzzer device */
struct dc_buzzer {
    struct awbl_buzzer_dev  bdev;       /**< \brief buzzer device, should go first */
    struct awbl_dev         dev;        /**< \brief the AWBus device */
    uint16_t                gpio;       /**< \brief GPIO number the buzzer is driven */
    uint16_t                active_low; /**< \brief driven polarity of the GPIO */
};

/** \brief convert a awbl_dev pointer to dc_buzzer pointer */
#define dev_to_dc_buzzer(p_dev)    AW_CONTAINER_OF(p_dev, struct dc_buzzer, dev)

/** \brief convert a buzzer_dev pointer to dc_buzzer pointer */
#define buzzer_to_dc_buzzer(dev)   ((struct dc_buzzer *)(dev))

/******************************************************************************/
void awbl_dc_buzzer_drv_register (void);


#ifdef __cplusplus
}
#endif

#endif /* __AWBL_BUZZER_DC_H */

/* end of file */
