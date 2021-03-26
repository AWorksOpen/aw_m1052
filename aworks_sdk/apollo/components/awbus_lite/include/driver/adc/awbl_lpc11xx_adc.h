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
 * \brief AWBus-lite lpc11xx ADC
 *
 * LPC13XX ADC0 Ϊ LPC13XX ϵ��MCU��Ƭ��ADC0������������:
 *
 * - 10-bit ��αƽ�ADת����
 * - 8������ͨ��
 * - ������Χ (0~3.6V)�����ܳ���VDD�ĵ�ѹ
 * - 10-bit ת��ʱ�� >= 2.44us
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "lpc11xx_adc"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_lpc11xx_adc_devinfo
 *
 * \par 2.�����豸
 *
 *  - NXP LPC11XX ϵ��MCU
 *  - NXP LPC13XX ϵ��MCU
 *  - ������LPC11XX GPIO���ݵ��豸
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_lpc11xx_adc_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_adc
 *
 * \internal
 * \par modification history:
 * - 1.01 13-05-13  zyr, add jobq support, 
 *                   modify the way to get APB CLK
 * - 1.00 12-11-05 zyr, first implementation
 * \endinternal
 */

#ifndef __AWBL_LPC11XX_ADC_H
#define __AWBL_LPC11XX_ADC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/**
 * \addtogroup grp_awbl_drv_lpc11xx_adc
 * \copydetails awbl_lpc11xx_adc.h
 * @{
 */

/**
 * \defgroup  grp_awbl_lpc11xx_adc_hwconf �豸����/����
 *
 * LPC11XX ADC Ӳ���豸�Ķ���������Ĵ�����ʾ���û��ڶ���Ŀ��ϵͳ��Ӳ����Դʱ��
 * ����ֱ�ӽ���δ���Ƕ�뵽 awbus_lite_hwconf_usrcfg.c �ж�Ӧ��λ�ã�Ȼ����豸
 * ��Ϣ���б�Ҫ������(�����õ���Ŀ�á�x����ʶ)��
 *
 * \note �ж��ٸ�ADC���ͽ�����Ĵ���Ƕ����ٴΣ�ע�⽫�����޸�Ϊ��ͬ�����֡�
 *
 * \include  hwconf_usrcfg_adc_lpc11xx.c
 * \note �����������д����û����á���ʶ�ģ���ʾ�û��ڿ���Ӧ��ʱ�������ô˲�����
 *
 * - ��1��ƽ̨��س�ʼ������ \n
 * ��ƽ̨��ʼ�����������ʹ��ADCʱ�ӵ���ƽ̨��صĲ�����
 *
 * - ��2��ADC����ʱ��Ƶ�ʻ�ȡ���� \n
 * ƽ̨��أ���������������ã����ڻ�ȡ����ʱ�ӡ�
 *
 * - ��3����4��ADͨ����� \n
 * ADC����ʼͨ����źͽ���ͨ����ţ�����ϵͳ��ADͨ����ŷ��䲻�����ص���ϵͳ��
 * ADͨ����ŷ�����ο� \ref grp_aw_plfm_adc_chans ��
 *
 * - ��5���Ĵ�������ַ \n
 * ��ο�MCU���û��ֲ���д�˲�����
 *
 * - ��6���жϺ� \n
 * ��ο�MCU���û��ֲ���д�˲�����
 *
 * - ��7��ADCλ�� \n
 * �����õ�ֵΪ 3~10������ʱ�������޸�ADC��ת��λ����ֻ�����������á�
 *
 * - ��8��ADת������ \n
 * ��ο�MCU���û��ֲ���д�˲��������ܴ���оƬ�����������ʡ�
 *
 * - ��9�� LPC11XX ADC ��Ԫ��
 * Ϊÿ��LPC11XX ADC ���䲻ͬ�ĵ�Ԫ��
 */

/** @} grp_awbl_drv_lpc11xx_gpio */

#include "awbl_adc.h"
#include "aw_spinlock.h"
#include "aw_isr_defer.h"

#define AWBL_LPC11XX_ADC_NAME   "lpc11xx_adc"

/*******************************************************************************
  defines
*******************************************************************************/
#if 0
#define AWBL_CONFIG_LPC11XX_ADC_JOBQ   /**< \brief Use job queue for this driver */
#endif


struct awbl_lpc11xx_adc_drv_ext {

    /** \brief current callback function */
    pfn_convert_callback_t  pfn_convert_cb;
    void                   *p_arg;
    aw_adc_val_t           *p_buf;
    aw_adc_channel_t        phy_ch;            /* ��ǰת����ͨ��  */

#ifdef AWBL_CONFIG_LPC11XX_ADC_JOBQ
    /** \brief adc result */
    uint32_t digital_val;
    /** \brief Job queue for ISR */
    struct aw_isr_defer_job  isr_jobq;
#endif
};

/**
 * \brief lpc11xx ADC �豸��Ϣ
 */
typedef struct awbl_lpc11xx_adc_devinfo {

    /** \brief ADC ���������Ϣ */
    struct awbl_adc_servinfo adc_servinfo;

    /** \brief ADC �Ĵ�������ַ */
    uint32_t regbase;

    /** \brief ADC �жϺ�      */
    uint32_t intnum;

    /** \brief ADC λ��        */
    uint8_t bits;
    
    /** \brief �ο���ѹ,��λmV */
    uint32_t vref;

    /** \brief ADC ʱ�� */
    uint32_t clk;

    /** \brief BSP function to get apb clock */
    uint32_t (*pfunc_plfm_apbclk_get) (void);

    /** \brief BSP function to pre initialize the device */
    void (*pfunc_plfm_init) (void);
} awbl_lpc11xx_adc_devinfo_t;

#define awbldev_get_lpc11xx_adc_info(p_dev) \
          ((struct awbl_lpc11xx_adc_devinfo *)AWBL_DEVINFO_GET(((awbl_dev_t *)(p_dev))))

/**
 * \brief lpc11xx ADC �豸ʵ��
 */
typedef struct awbl_lpc11xx_adc_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev super;

    /** \brief ADC ���� */
    struct awbl_adc_service adc_serv;

    /** \brief �豸��չ */
    struct awbl_lpc11xx_adc_drv_ext ext;
} awbl_lpc11xx_adc_dev_t;

/**
 * \brief lpc11xx driver register
 */
void awbl_lpc11xx_adc_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_LPC11XX_ADC_H */

/* end of file */
