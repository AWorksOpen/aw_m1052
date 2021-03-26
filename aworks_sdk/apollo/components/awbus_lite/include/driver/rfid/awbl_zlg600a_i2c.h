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
 * \brief AWBus ZLG600A UART�ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_zlg600a.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-05-03  win, first implementation
 * \endinternal
 */

#ifndef __AWBL_ZLG600A_I2C_H
#define __AWBL_ZLG600A_I2C_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_drv_zlg600a_i2c
 * \copydetails awbl_zlg600a_i2c.h
 * @{
 */

/**
 * \defgroup  grp_awbl_zlg600a_i2c_impl ʵ�����
 * \todo
 *
 */

/**
 * \defgroup  grp_awbl_zlg600a_i2c_hwconf �豸����/����
 * \todo
 */

/** @} grp_awbl_drv_zlg600a_i2c */

#include "awbl_zlg600a.h"
#include "awbl_i2cbus.h"

/** \brief PCF8563 ������ */
#define AWBL_ZLG600A_I2C_NAME   "zlg600a_i2c"

/**
 * \brief ZLG600A I2C�豸��Ϣ
 */
typedef struct awbl_zlg600a_i2c_devinfo {

    /** \brief ZLG600A ����������Ϣ  */
    struct awbl_zlg600a_servinfo zlg600a_servinfo;

    int pin;

    uint8_t   frame_fmt;    /**< \brief Ĭ��ʹ�õ�֡��ʽ */
    uint8_t   addr;         /**< \brief Ĭ�ϴ���ĵ�ַ */
    uint8_t   mode;         /**< \brief ͨ��ģʽ */

    /** \brief ��ʱ����λms�� */
    int       timeout;

    /** ƽ̨��س�ʼ��������ʱ�ӡ���ʼ�����ŵȲ��� */
    void (*pfunc_plfm_init)(void);

} awbl_zlg600a_i2c_devinfo_t;

/**
 * \brief ZLG600A I2C�豸�ṹ�嶨��
 */
typedef struct awbl_zlg600a_i2c_dev {

    /** \brief �̳��� AWBus I2C device �豸 */
    struct awbl_i2c_device super;

    /** \brief ��Ƭ����ṹ�� */
    awbl_zlg600a_service_t serv;

    uint8_t  addr;         /**< \brief Ĭ�ϴ���ĵ�ַ      */

    AW_MUTEX_DECL(devlock);  /**< \brief �豸�� */

    /** \brief �ź��� */
    AW_SEMB_DECL(semb);

} awbl_zlg600a_i2c_dev_t;

/**
 * \brief ZLG600A I2C driver register
 */
void awbl_zlg600a_i2c_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_ZLG600A_I2C_H */

/* end of file */
