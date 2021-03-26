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
 * \brief AWBus ADC �ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_adc.h"
 * \endcode
 *
 * �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.03 15-06-12  tee, add and modify some interface
 * - 1.02 15-04-07  tee, modify some interface and implemention
 *                       cancel the driver interface of not support callback,
 *                       data space is pass from standard interface,not define
 *                       by drivers
 * - 1.01 14-09-25  ehf, modify some interface and implemention to
 *                  support read mulitiple samples once
 * - 1.00 12-11-05  zyr, first implementation
 * \endinternal
 */

#ifndef __AWBL_ADC_H
#define __AWBL_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_adc.h"
#include "aw_spinlock.h"
#include "aw_sem.h"
#include "awbus_lite.h"

typedef void (*pfn_convert_callback_t) (void *p_cookie, aw_err_t state);
    
/**
 * \brief channel range of AD controller instance in AWBus ADC frame
 *
 * channel IDs of AD controller instance must be continuous.
 */
struct awbl_adc_servinfo {

    /** \brief the minimum ch */
    aw_adc_channel_t ch_min;

    /** \brief the maximum ch */
    aw_adc_channel_t ch_max;
};

/** \brief AWBus ADC service functions */
struct awbl_adc_servfuncs {

    /**
     * \brief ����ת��
     *
     * \param[in] p_cookie     the p_cookie of driver
     * \param[in] channel      the channel to covert
     * \param[in] p_desc       ����������,�����ǵ��������������������ĵ�ַ����
     *                         �������������������ĵ�ַ��
     * \param[in] desc_num     ָ�������������ĸ���
     * \param[in] count        ���е�ת��������һ��ת�����б�ʾdesc_numָ������
     *                         �ĵĻ���������������������ָ��Ϊ0������������
     *                         ��ת������һ�����н������Զ�����������һ������
     * \param[in] pfn_seq_complete  ������ɻص�����,��һ������ת�����ʱ��
     *                               - ������������������ת�����
     *                               - ������������ӵ�һ�������������һ������
     *                                 �����������Ӧ�õ��øú������õĻص�����
     *                                 ��һ�������쳣����ʱ��ҲӦ���øú�������
     *                                 �Ļص���������ָ��������Ϣ
     * \param[in] p_arg         ������ɻص������Ĳ���
     *
     * \retval   AW_OK     success
     * \retval -AW_ENXIO   channel is nonexistent
     */
    aw_err_t (*pfn_start) (void                   *p_cookie,
                           aw_adc_channel_t        channel,
                           aw_adc_buf_desc_t      *p_desc,
                           int                     desc_num,
                           uint32_t                count,
                           pfn_adc_complete_t      pfn_seq_complete,
                           void                   *p_arg);
    /**
     * \brief ֹͣת��
     *
     * \param[in] p_cookie     the p_cookie of driver
     * \param[in] channel      the channel to covert
     *
     * \retval   AW_OK     success
     * \retval -AW_ENXIO   channel is nonexistent
     */
    aw_err_t (*pfn_stop) (void                   *p_cookie,
                          aw_adc_channel_t        channel);
    
    /**
     * \brief get channel's AD bits
     *
     * \param p_cookie[in] indicate an adc controller instance
     * \param channel[in]  channel of adc controller
     *
     * \retval >0     the AD bits of the corresponding channel
     * \retval -ENXIO channel is nonexistent
     */
    aw_err_t (*pfn_bits_get) (void             *p_cookie, 
                              aw_adc_channel_t  channel);

    /**
     * \brief get channel's AD refernce Voltage
     */
    aw_err_t (*pfn_vref_get) (void              *p_cookie, 
                              aw_adc_channel_t   channel);
    
    /**
     * \brief ��ȡADC�Ĳ�����
     */
    aw_err_t (*pfn_rate_get) (void             *p_cookie, 
                              aw_adc_channel_t  channel,
                              uint32_t         *p_rate);

    /**
     * \brief ����ADC�Ĳ����ʣ�ʵ�ʲ����ʿ��ܴ��ڲ���
     */
    aw_err_t (*pfn_rate_set) (void             *p_cookie, 
                              aw_adc_channel_t  channel, 
                              uint32_t          rate);
};

/** \brief AWBus ADC service instance */
struct awbl_adc_service {

    /** \brief current adc client node, internal used only */
    struct aw_adc_client      *p_cur;

    /** \brief point to next service, internal used only */
    struct awbl_adc_service   *p_next;

    /** \brief spin lock, internal used only */
    aw_spinlock_isr_t          lock;

    /** \brief allocate a semaphore, internal used only */
    AW_SEMB_DECL(semb_busy);

    /** \brief semb_id to indicate the semb_busy, internal used only */
    aw_semb_id_t               semb_id;

    /** \brief adc urgent client pending list head node */
    struct aw_list_head        adc_urgent;

    /** \brief adc normal pending list head node */
    struct aw_list_head        adc_normal;

    /** \brief service information */
    struct awbl_adc_servinfo   *p_servinfo;

    /** \brief service function */
    const struct awbl_adc_servfuncs *p_servfuncs;

    /** 
     * \brief a context argument
     * will be passed to every service function as their first argument.
     */
    void                      *p_cookie;
};

/** \brief AWBus method, used to get a AWBus ADC service
 *
 * a driver can implement the method 'awbl_adcserv_get'
 * to provide a AWBus ADC service. \see awbl_adc_methods.c
 */
AWBL_METHOD_IMPORT(awbl_adcserv_get);

/**
 * \brief initialize AWBus ADC service
 *
 * \attention the function should be called
 * after awbl_dev_init1() and before awbl_dev_init2()
 */
void awbl_adc_init (void);

#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_ADC_H */

/* end of file */
