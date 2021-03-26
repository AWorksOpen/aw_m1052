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
 * \brief AWBus LPC11XX SSP ����ͷ�ļ�
 *
 * LPC11XX SSP Ϊ LPC11XX ϵ��MCU��Ƭ��SSP��������ֻʵ����SPI�������ܡ�
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "lpc11xx_ssp"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_lpc11xx_ssp_devinfo
 *
 * \par 2.�����豸
 *
 *  - NXP LPC11XX ϵ��MCU
 *  - NXP LPC13XX ϵ��MCU
 *  - ������LPC11XX SSP ���ݵ��豸
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_lpc11xx_ssp_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_spi
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-20  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_LPC11XX_SPI_H
#define __AWBL_LPC11XX_SPI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_drv_lpc11xx_ssp
 * \copydetails awbl_lpc11xx_ssp.h
 * @{
 */

/**
 * \defgroup  grp_awbl_lpc11xx_ssp_hwconf �豸����/����
 *
 * LPC11XX I2C Ӳ���豸�Ķ���������Ĵ�����ʾ���û��ڶ���Ŀ��ϵͳ��Ӳ����Դʱ��
 * ����ֱ�ӽ���δ���Ƕ�뵽 awbus_lite_hwconf_usrcfg.c �ж�Ӧ��λ�ã�Ȼ����豸
 * ��Ϣ���б�Ҫ������(�����õ���Ŀ�á�x����ʶ)��
 *
 * \note �ж��ٸ�I2C���ͽ�����Ĵ���Ƕ����ٴΣ�ע�⽫�����޸�Ϊ��ͬ�����֡�

 * \include  hwconf_usrcfg_lpc11xx_ssp.c
 * \note �����������д����û����á���ʶ�ģ���ʾ�û��ڿ���Ӧ��ʱ�������ô˲�����
 *
 * - ��1�� ƽ̨��س�ʼ�� \n
 * ��ƽ̨��ʼ�����������ʹ��SSP�豸ʱ�ӵ���ƽ̨��صĳ�ʼ��������
 *
 * - ��2�� SPI���߱�� \n
 * �����SSP�����������߱�š�����ϵͳ�е�SPI���߱�ŷ��䲻�����ص���
 * ʹ�� Apollo��׼SPI�ӿ�ʱ��ʹ�ô����߱�ţ�����Զ�λ�����ﶨ���
 * LPC11XX SSP�豸��
 *
 * - ��3�� �Ĵ�������ַ \n
 * ��ο�MCU���û��ֲ���д�˲���
 *
 * - ��4�� �жϺ� \n
 * ��ο�ϵͳ���жϺŷ�����д�˲������� \ref grp_aw_plfm_inums

 * - ��5�� ����ʱ��Ƶ�� \n
 * �����ϵͳ��������д��ֵ��
 *
 * - ��6�� LPC11XX SSP ��Ԫ�� \n
 * Ϊÿ��LPC11XX SSP ���䲻ͬ�ĵ�Ԫ��
 */

/** @} grp_awbl_drv_lpc11xx_ssp */

#include "aw_spinlock.h"
#include "awbl_spibus.h"

#define AWBL_LPC11XX_SSP_NAME   "lpc11xx_ssp"

/**
 * \brief LPC11XX SSP �豸��Ϣ
 */
struct awbl_lpc11xx_ssp_devinfo {

    /** \brief �̳��� AWBus SPI �������豸��Ϣ */
    struct awbl_spi_master_devinfo spi_master_devinfo;

    uint32_t regbase;               /**< \brief �Ĵ�������ַ */
    int      inum;                  /**< \brief �жϺ� */
    uint32_t clkfreq;               /**< \brief ����ʱ��Ƶ�� */

    /** \brief ƽ̨��س�ʼ��������ʱ�ӡ���ʼ�����ŵȲ��� */
    void (*pfunc_plfm_init)(void);
};

/**
 * \brief LPC11XX SSP �豸ʵ��
 */
struct awbl_lpc11xx_ssp_dev {

    /** \brief �̳��� AWBus SPI ���������� */
    struct awbl_spi_master  super;

    /** \brief ��ǰ���ڴ���� Message */
    struct aw_spi_message *p_cur_msg;

    /** \brief ��ǰ���ڴ���� Transfer */
    struct aw_spi_transfer *p_cur_trans;

    /** \brief �������ݼ��� */
    size_t      data_ptr;

    /** \brief ������æ��־ */
    aw_bool_t        busy;

    /** \brief ������״̬��״̬  */
    int         state;

    uint32_t    cur_speed;      /**< /brief ��������ǰ�ٶ� */
    uint8_t     cur_bpw;        /**< /brief ��������ǰ֡��С */
    uint8_t     cur_mode;       /**< /brief ��������ǰģʽ */

    uint8_t     nbytes_to_recv; /**< /brief �����յ��ֽ��� */
    uint8_t     cs_toggle;      /**< /brief Ƭѡ������־ */
    aw_spi_device_t *p_tgl_dev; /**< /brief ��ǰ������SPI�豸 */
};

/**
 * \brief LPC11XX SSP master driver register
 */
void awbl_lpc11xx_ssp_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_LPC11XX_SPI_H */

/* end of file */
