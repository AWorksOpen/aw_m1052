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
 * \brief AWBus GPIO I2C ����ͷ�ļ�
 *
 * ������ʹ�� Apollo ��׼GPIO�ӿ���ģ��I2C��������
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "gpio_i2c"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_gpio_i2c_devinfo
 *
 * \par 2.�����豸
 *
 *  - �κ�ʵ��Apollo��׼GPIO�ӿڵ��豸
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_gpio_i2c_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_i2c
 *
 * \internal
 * \par modification history:
 * - 1.00 13-08-26  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_GPIO_I2C_H
#define __AWBL_GPIO_I2C_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_drv_gpio_i2c
 * \copydetails awbl_gpio_i2c.h
 * @{
 */

/**
 * \defgroup  grp_awbl_gpio_i2c_hwconf �豸����/����
 *  //TODO
 */

/** @} grp_awbl_drv_gpio_i2c */

#include "aw_task.h"
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "awbl_i2cbus.h"

#define AWBL_GPIO_I2C_NAME   "gpio_i2c"

/**
 * \brief GPIO I2C �豸��Ϣ
 */
struct awbl_gpio_i2c_devinfo {

    /** \brief �̳��� AWBus I2C �������豸��Ϣ */
    struct awbl_i2c_master_devinfo i2c_master_devinfo;

    int      pin_scl;               /**< \brief SCL ���ű�� */
    int      pin_sda;               /**< \brief SDA ���ű�� */

    /**  
     * \brief �����ٶ�,ͨ������߼���������ȷ��ʵ�ʵ������ٶ� 
     */
    unsigned speed_exp;             

    void (*pfunc_sda_set_input)  (struct awbl_gpio_i2c_devinfo *);
    void (*pfunc_sda_set_output) (struct awbl_gpio_i2c_devinfo *);

    /**
     * \brief ƽ̨��س�ʼ��������ʱ�ӡ���ʼ������
     */
    void (*pfunc_plfm_init)(void); 
};

/**
 * \brief GPIO I2C �豸ʵ��
 */
struct awbl_gpio_i2c_dev {

    /** \brief �̳��� AWBus I2C ���������� */
    struct awbl_i2c_master  super;

    /** \brief ��䳬ʱʱ�� */
    uint32_t    timeout;

    /** \brief ���߿���ʱ�� us */
    uint32_t    bus_idle_time;

    /** \brief ʹ��aw_udelay���е���ʱ */
    uint32_t    speed;
};

/**
 * \brief GPIO I2C master driver register
 */
void awbl_gpio_i2c_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_GPIO_I2C_H */

/* end of file */

