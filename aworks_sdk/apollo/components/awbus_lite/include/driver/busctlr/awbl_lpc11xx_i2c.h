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
 * \brief AWBus LPC11XX I2C ����ͷ�ļ�
 *
 * LPC11XX I2C Ϊ LPC11XX ϵ��MCU��Ƭ��I2C��������ֻʵ����I2C�������ܡ�
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "lpc11xx_i2c"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_lpc11xx_i2c_devinfo
 *
 * \par 2.�����豸
 *
 *  - NXP LPC11XX ϵ��MCU
 *  - NXP LPC13XX ϵ��MCU
 *  - ������LPC11XX I2C ���ݵ��豸
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_lpc11xx_i2c_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_i2c
 *
 * \internal
 * \par modification history:
 * - 1.00 12-10-11  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_LPC11XX_I2C_H
#define __AWBL_LPC11XX_I2C_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_drv_lpc11xx_i2c
 * \copydetails awbl_lpc11xx_i2c.h
 * @{
 */

/**
 * \defgroup  grp_awbl_lpc11xx_i2c_hwconf �豸����/����
 *
 * LPC11XX I2C Ӳ���豸�Ķ���������Ĵ�����ʾ���û��ڶ���Ŀ��ϵͳ��Ӳ����Դʱ��
 * ����ֱ�ӽ���δ���Ƕ�뵽 awbus_lite_hwconf_usrcfg.c �ж�Ӧ��λ�ã�Ȼ����豸
 * ��Ϣ���б�Ҫ������(�����õ���Ŀ�á�x����ʶ)��
 *
 * \note �ж��ٸ�I2C���ͽ�����Ĵ���Ƕ����ٴΣ�ע�⽫�����޸�Ϊ��ͬ�����֡�

 * \include  hwconf_usrcfg_i2c_lpc11xx.c
 * \note �����������д����û����á���ʶ�ģ���ʾ�û��ڿ���Ӧ��ʱ�������ô˲�����
 *
 * - ��1�� ƽ̨��س�ʼ�� \n
 * ��ƽ̨��ʼ�����������ʹ��I2C�豸ʱ�ӵ���ƽ̨��صĳ�ʼ��������
 *
 * - ��2�� I2C���߱�� \n
 * �����I2C�����������߱�š�����ϵͳ�е�I2C���߱�ŷ��䲻�����ص���
 * ʹ�� Apollo��׼I2C�ӿ�ʱ��ʹ�ô����߱�ţ�����Զ�λ�����ﶨ���
 * LPC11XX I2C�豸��
 *
 * - ��3�����û����á� I2C�����ٶ� \n
 * �����I2C�������������ٶȣ�
 *
 * - ��4�� I2C��ʱʱ�� \n
 * �����I2C�������ĳ�ʱʱ��,��λ:ϵͳ������, #AWBL_I2C_WAITFOREVER ��ʾ�޳�ʱ��
 * ����ͬ���ӿ� (aw_i2c_sync(), aw_i2c_read(), aw_i2c_write()), ������δ�ڴ˴�
 * ���õ�ʱ������ɣ��������� -TIME ��ʱ����
 *
 * - ��5�� �Ĵ�������ַ \n
 * ��ο�MCU���û��ֲ���д�˲���
 *
 * - ��6�� �жϺ� \n
 * ��ο�ϵͳ���жϺŷ�����д�˲������� \ref grp_aw_plfm_inums

 * - ��7�� ����ʱ��Ƶ�� \n
 * �����ϵͳ��������д��ֵ��
 *
 * - ��8�� LPC11XX I2C ��Ԫ�� \n
 * Ϊÿ��LPC11XX I2C���䲻ͬ�ĵ�Ԫ��
 */

/** @} grp_awbl_drv_lpc11xx_i2c */

#include "aw_spinlock.h"
#include "awbl_i2cbus.h"

#define AWBL_LPC11XX_I2C_NAME   "lpc11xx_i2c"

/**
 * \brief LPC11XX I2C �豸��Ϣ
 */
struct awbl_lpc11xx_i2c_devinfo {

    /** \brief �̳��� AWBus I2C �������豸��Ϣ */
    struct awbl_i2c_master_devinfo i2c_master_devinfo;

    uint32_t regbase;               /**< \brief �Ĵ�������ַ */
    int      inum;                  /**< \brief �жϺ� */
    uint32_t clkfreq;               /**< \brief ����ʱ��Ƶ�� */
    void (*pfunc_plfm_init)(void);  /**< \brief ƽ̨��س�ʼ��������ʱ�ӡ���ʼ������ */
};

/**
 * \brief LPC11XX I2C �豸ʵ��
 */
struct awbl_lpc11xx_i2c_dev {

    /** \brief �̳��� AWBus I2C ���������� */
    struct awbl_i2c_master  super;

    /** \brief ��ǰ���ڴ���� Message */
    struct aw_i2c_message *p_cur_msg;

    /** \brief ��ǰ���ڴ���� Transfer */
    struct aw_i2c_transfer *p_cur_trans;

    /** \brief �������ݼ��� */
    size_t      data_ptr;

    /** \brief ������æ��־ */
    aw_bool_t        busy;

    /** \brief ������״̬��״̬  */
    int         state;
};

/**
 * \brief LPC11XX I2C master driver register
 */
void awbl_lpc11xx_i2c_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_LPC11XX_I2C_H */

/* end of file */
