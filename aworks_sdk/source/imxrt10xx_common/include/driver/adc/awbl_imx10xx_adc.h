/******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx adc driver
 *
 * \internal
 * \par Modification History
 * - 1.00 2017-11-02  mex, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_ADC_H
#define __AWBL_IMX10xx_ADC_H

#include "aworks.h"
#include "aw_gpio.h"
#include "awbl_adc.h"
#include "aw_spinlock.h"
#include "aw_isr_defer.h"
#include "awbus_lite.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/** \brief iMX RT10xx ADC ������ */
#define AWBL_IMX10XX_ADC_NAME            "imx10xx_adc"

/** \brief iMX RT10xx ADC ģ������ͨ����Χ */
#define IMX10xx_ADC_MIN_CH           0
#define IMX10xx_ADC_MAX_CH           15


/**
 * \brief iMX RT10xx adc �豸��Ϣ
 */
typedef struct awbl_imx10xx_adc_devinfo {
    struct awbl_adc_servinfo adc_servinfo;    /**< \brief ADC ���������Ϣ */
    uint32_t                 regbase;         /**< \brief ADC �Ĵ�������ַ  */
    uint8_t                  inum;            /**< \brief ADC �жϺ� */
    uint8_t                  bits;            /**< \brief ADC ����λ�� */
    uint32_t                 refmv;           /**< \brief ADC �ο���ѹ mV */
    uint32_t                 clk_id;          /**< \brief ADC ʱ��ID */
    void (*pfn_plfm_init) (void);             /**< \brief ƽ̨��ʼ������ */
} awbl_imx10xx_adc_devinfo_t;

/**
 * \brief iMX RT10xx adc �豸ʵ��
 */
typedef struct awbl_imx10xx_adc_dev {

    struct awbl_dev         super;             /**< \brief �̳��� AWBus �豸 */
    struct awbl_adc_service adc_serv;          /**< \brief ADC ���� */

    aw_adc_channel_t        phy_chan;          /**< \brief �豸��ǰת����ͨ�� */

    aw_adc_buf_desc_t      *p_bufdesc;         /**< \brief ָ��������������� */
    uint32_t                bufdesc_num;       /**< \brief ���������� */
    uint32_t                cur_bufdesc_id;    /**< \brief ���ڴ���Ļ�����ID */

    uint32_t                samples;           /**< \brief ����������ɵĴ��� */
    uint32_t                remain_count;      /**< \brief ʣ�������ת������ */
    aw_bool_t               is_stopped;        /**< \brief ��־�Ƿ�ǿ��ֹͣ */

    pfn_adc_complete_t      pfn_seq_complete;  /**< \brief ������ɻص����� */
    void                   *p_arg;             /**< \brief �ص��������� */
    volatile uint8_t        temp_conver_end;   /** \brief  �¶�ADCת����ɱ�־ */

    AW_MUTEX_DECL(dev_lock);                   /**< \brief �豸�� */

} awbl_imx10xx_adc_dev_t;

/**
 * \brief iMX RT10xx ADC ����ע��
 */
void awbl_imx10xx_adc_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10xx_ADC_H */

/* end of file */
