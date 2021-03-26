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
 * \brief AWBus DAC �ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_dac.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-05  tee, first implementation
 * \endinternal
 */

#ifndef __AWBL_DAC_H
#define __AWBL_DAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_dac.h"
#include "aw_spinlock.h"
#include "aw_sem.h"
#include "awbus_lite.h"

struct awbl_adc_service;

/**
 * \brief channel range of DAC controller instance in AWBus DAC frame
 *
 * channel IDs of DAC controller instance must be continuous.
 */
struct awbl_dac_servinfo {

    /** \brief the minimum ch */
    aw_dac_channel_t ch_min;

    /** \brief the maximum ch */
    aw_dac_channel_t ch_max;
};

/** \brief AWBus ADC service functions */
struct awbl_dac_servfuncs {

    /**
     * \brief ��ȡһ��DACͨ����λ��
     *
     * \param[in] ch  DACͨ����
     *
     * \return >0         ��DACͨ������Чλ��
     */
    int (*pfn_bits_get) (void *p_cookie, aw_dac_channel_t ch);

    /**
     * \brief ��ȡһ��DACͨ���Ĳο���ѹ
     *
     * \param[in] ch  DACͨ����
     *
     * \return >0         ��DACͨ����Ӧ�Ĳο���ѹ
     */
    int (*pfn_vref_get) (void *p_cookie, aw_dac_channel_t ch);

    /**
     * \brief ����һ��DACͨ����������ֵ
     *
     * \param[in] ch  DACͨ����
     * \param[in] val �����õ�������ֵ
     *
     * \retval   AW_OK      �����ɹ�
     * \retval  -AW_EINVAL  ��Ч�����������õ�������ֵ����
     */
    aw_err_t (*pfn_val_set) (void            *p_cookie,
                             aw_dac_channel_t ch,
                             aw_dac_val_t     val);
    
    
    /**
     * \brief ʹ��һ��DACͨ�����ģ����
     *
     * \param[in] ch  DACͨ����
     * \param[in] val �����õ�������ֵ
     *
     * \retval   AW_OK      �����ɹ�
     *
     * \note ���ڸ�ʹ�ܺ�����aw_adc_val_set()�����ĵ����Ⱥ��ϵ��˵�����£�
     *       ����ȵ��ø�ʹ�ܺ�������DACʹ�ܺ������ѹΪ0������ͨ����������
     *       aw_dac_val_set()����Ϊ��Ҫ��ֵ������ȵ�������aw_dac_val_set()
     *       ���������˵�ѹֵ����ʹ�ܺ�Ϊ��Ӧ�ĵ�ѹ���ֵ��
     */
    aw_err_t (*pfn_enable) (void *p_cookie, aw_dac_channel_t ch);

    /**
     * \brief ����һ��DACͨ�����ģ����
     *
     * \param[in] ch  DACͨ����
     * \param[in] val �����õ�������ֵ
     *
     * \retval   AW_OK      �����ɹ�
     */
    aw_err_t (*pfn_disable) (void *p_cookie, aw_dac_channel_t ch);
};

/** \brief AWBus DAC service instance */
struct awbl_dac_service {

    /** \brief point to next service, internal used only */
    struct awbl_dac_service   *p_next;

    /** \brief spin lock, internal used only */
    aw_spinlock_isr_t          lock;

    /** \brief allocate a mutex */
    AW_MUTEX_DECL(mutex_busy);

    /** \brief semb_id to indicate the semb_busy, internal used only */
    aw_mutex_id_t mutex_id;

    /** \brief service information */
    struct awbl_dac_servinfo        *p_servinfo;

    /** \brief service function */
    const struct awbl_dac_servfuncs *p_servfuncs;

    /** 
     * \brief a context argument
     *  will be passed to every service function as their first argument.
     */
    void                            *p_cookie;
};

/** \brief AWBus method, used to get a AWBus ADC service
 *
 * a driver can implement the method 'awbl_adcserv_get'
 * to provide a AWBus ADC service. \see awbl_adc_methods.c
 */
AWBL_METHOD_IMPORT(awbl_dacserv_get);

/**
 * \brief initialize AWBus ADC service
 *
 * \attention the function should be called
 * after awbl_dev_init1() and before awbl_dev_init2()
 */
void awbl_dac_init (void);

#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_ADC_H */

/* end of file */
