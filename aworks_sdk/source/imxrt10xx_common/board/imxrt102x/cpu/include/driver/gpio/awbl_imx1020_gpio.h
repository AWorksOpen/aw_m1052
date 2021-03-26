/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief AWBus-lite iMX RT1020 GPIO ����
 *
 * iMX RT102Xϵ��MCU��Ƭ��GPIO����������97��GPIO���ſ��ã�������
 * ����:
 *  - ���ſ������������Ϊ����/���
 *  - ÿ���������ſɱ������ⲿ�ж���������
 *  - ÿ���жϿ�����Ϊ�½��ء������ػ�ߵ͵�ƽ�����ж�
 *  - ����GPIO ����Ĭ��Ϊ����
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "imx1020_gpio"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_imx1020_gpio_devinfo
 *
 * \internal
 * \par modification history:
 * - 1.00 18-09-25, mex, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX1020_GPIO_H
#define __AWBL_IMX1020_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "awbl_gpio.h"
#include "awbl_intctlr.h"
#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "imx1020_pin.h"


/** \brief IMX1020 GPIO ������ */
#define AWBL_IMX1020_GPIO_NAME   "imx1020_gpio"

/** \brief IMX1020 GPIO �Ķ˿�(bank)�� */
#define  IMX1020_GPIO_PORTS   4

/** \brief IMX1020 GPIO ÿ�˿ں���GPIO���� */
#define  IMX1020_PORT_PINS    32


/** \brief IMX1020 GPIOƽ̨��ʼ������ָ�� */
typedef void (*pfn_gpio_plat_init_t)(void);


/**
 * \brief IMX1020 GPIO �豸��Ϣ
 */
struct awbl_imx1020_gpio_devinfo{

    /** \brief ƽ̨��صĳ�ʼ������ */
    pfn_gpio_plat_init_t         plat_init;

    /** \brief GPIO�˿ڼĴ�������ַ */
    const uint32_t               *gpio_regs_base;

    /** \brief IOMUXC �Ĵ�������ַ */
    uint32_t                     iomuxc_base;

    /** \brief IOMUXC SNVS�Ĵ�������ַ */
    uint32_t                     iomuxc_snvs_base;

    /** \brief GPIO ��������״̬ */
    uint8_t                      *p_pin_cfg;

    /** \brief GPIO ����������Ϣ */
    struct awbl_gpio_servinfo    gpio_info;

    /** \brief IntCtlr ����������Ϣ */
    struct awbl_intctlr_servinfo int_info;
};


/**
 * \brief GPIO�����жϴ�����Ϣ�ṹ
 */
struct awbl_imx1020_gpio_trigger_info {

    aw_pfuncvoid_t  pfunc_callback;     /**< \brief �жϻص����� */
    void           *p_arg;              /**< \brief ���ݸ��ص������Ĳ��� */
};


/**
 * \brief IMX1020 GPIO �豸ʵ��
 */
struct awbl_imx1020_gpio_dev {

    struct awbl_dev             awdev;           /**< \brief �̳��� AWBus �豸 */
    struct awbl_gpio_service    gpio_serv;       /**< \brief GPIO ���� */
    struct awbl_intctlr_service intctlr_serv;    /**< \brief IntCtlr ���� */
    aw_spinlock_isr_t           dev_lock;        /**< \brief �豸�� */

    /** \brief GPIO ����ISR��Ϣ */
    struct awbl_imx1020_gpio_trigger_info trig_info[IMX1020_GPIO_PINS];

};


/**
 * \brief �û����ô˺���ע��GPIO����
 */
void awbl_imx1020_gpio_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX1020_GPIO_H */


/* end of file */
