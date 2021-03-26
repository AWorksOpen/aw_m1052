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
 * \brief AWBus DAC 接口头文件
 *
 * 使用本模块需要包含以下头文件：
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
     * \brief 获取一个DAC通道的位数
     *
     * \param[in] ch  DAC通道号
     *
     * \return >0         该DAC通道的有效位数
     */
    int (*pfn_bits_get) (void *p_cookie, aw_dac_channel_t ch);

    /**
     * \brief 获取一个DAC通道的参考电压
     *
     * \param[in] ch  DAC通道号
     *
     * \return >0         该DAC通道对应的参考电压
     */
    int (*pfn_vref_get) (void *p_cookie, aw_dac_channel_t ch);

    /**
     * \brief 设置一个DAC通道的数字量值
     *
     * \param[in] ch  DAC通道号
     * \param[in] val 待设置的数字量值
     *
     * \retval   AW_OK      操作成功
     * \retval  -AW_EINVAL  无效参数，待设置的数字量值过大
     */
    aw_err_t (*pfn_val_set) (void            *p_cookie,
                             aw_dac_channel_t ch,
                             aw_dac_val_t     val);
    
    
    /**
     * \brief 使能一个DAC通道输出模拟量
     *
     * \param[in] ch  DAC通道号
     * \param[in] val 待设置的数字量值
     *
     * \retval   AW_OK      操作成功
     *
     * \note 关于该使能函数和aw_adc_val_set()函数的调用先后关系，说明如下：
     *       如果先调用该使能函数，则DAC使能后输出电压为0，可以通过后续调用
     *       aw_dac_val_set()设置为需要的值。如果先调用了用aw_dac_val_set()
     *       函数设置了电压值，则使能后即为相应的电压输出值。
     */
    aw_err_t (*pfn_enable) (void *p_cookie, aw_dac_channel_t ch);

    /**
     * \brief 禁能一个DAC通道输出模拟量
     *
     * \param[in] ch  DAC通道号
     * \param[in] val 待设置的数字量值
     *
     * \retval   AW_OK      操作成功
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
