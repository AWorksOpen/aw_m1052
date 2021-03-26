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
 * \brief LM75�¶ȴ���������
 *

 *
 * \par 1.������Ϣ
 *
 *  - ������:   "lm75_temperature"
 *  - ��������: AWBL_BUSID_I2C
 *  - �豸��Ϣ: struct awbl_lm75_par
 *
 * \par 2.�����豸
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_lm75_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_lm75
 *
 * \internal
 * \par modification history
 * - 1.00 14-07-30  hbt, first implementation.
 * \endinternal
 */

#ifndef __AWBL_LM75_H
#define __AWBL_LM75_H

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_common.h"
#include "aw_types.h"
#include "aw_i2c.h"

#include "awbl_temp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_lm75
 * @{
 */

/**
 * \defgroup  grp_awbl_lm75_hwconf �豸����/����
 *
 *
 * \include  awbl_lm75.c
 *
 */

/** @} */


/** \brief lm75�¶ȴ������豸��Ϣ */
struct awbl_lm75_par {

    uint32_t   id;                   /**< \brief �豸id�� */

    uint32_t   slave_addr;           /**< \brief �ӻ��豸��ַ */
};

/** \brief lm75�¶ȴ������豸ʵ�� */
struct awbl_lm75_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev dev;

    /** \brief lm75�ӻ��豸�ṹ��*/
    aw_i2c_device_t lm75;

    /** \brief  �¶ȴ��������� */
    struct awbl_temp_service  temp_serv;
};

#define LM75_TEMP_NAME    "lm75_temperature"

#define SERV_GET_LM75_DEV(p_serv)   \
        (AW_CONTAINER_OF(p_serv, struct awbl_lm75_dev, temp_serv))

#define __SERV_TO_DEV(p_serv)   \
        ((AW_CONTAINER_OF(p_serv, struct awbl_lm75_dev, temp_serv))->dev)

#define SERV_GET_LM75_PAR(p_serv) \
        ((struct awbl_lm75_par *)AWBL_DEVINFO_GET(&__SERV_TO_DEV(p_serv)))

#define DEV_TO_LM75_DEV(p_dev)  \
        (AW_CONTAINER_OF(p_dev, struct awbl_lm75_dev, dev))

#define LM75_DEV_GET_PAR(p_therm_dev)  \
        ((struct awbl_lm75_par *)((p_therm_dev)->dev.p_devhcf->p_devinfo))

#define DEV_GET_LM75_PAR(p_dev)   ((void *)AWBL_DEVINFO_GET(p_dev))

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_LM75_H */

/* end of file */
