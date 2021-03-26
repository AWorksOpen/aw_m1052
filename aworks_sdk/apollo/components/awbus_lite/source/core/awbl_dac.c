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
 * \brief AWBus dac interface implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-06  tee, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_dac.h"
#include "awbl_dac.h"
#include "aw_assert.h"

/** \brief pointer to the first dac service */
struct awbl_dac_service *__gp_dac_serv_head  = NULL;
 
/******************************************************************************
  locals
 ******************************************************************************/
#define __GET_CH_MAX(p_serv)          ((p_serv)->p_servinfo->ch_max)
#define __GET_CH_MIN(p_serv)          ((p_serv)->p_servinfo->ch_min)
#define __GET_DEV_CHANNEL(p_serv, ch) ((ch) - __GET_CH_MIN((p_serv)))

#define __dac_SPIN_LOCK_INIT(lock) \
    aw_spinlock_isr_init(lock, 0)

#define __dac_SPIN_LOCK_IRQSAVE(lock, flag) \
    (void)flag;aw_spinlock_isr_take(lock)

#define __dac_SPIN_UNLOCK_IRQRESTORE(lock, flag) \
    (void)flag;aw_spinlock_isr_give(lock)

#define __DAC_MUTEX_LOCK(p_serv)       AW_MUTEX_LOCK(p_serv->mutex_busy, \
                                                     AW_SEM_WAIT_FOREVER)

#define __DAC_MUTEX_UNLOCK(p_serv)     AW_MUTEX_UNLOCK(p_serv->mutex_busy)

/******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief allocate dac service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval          AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __dac_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t     pfunc_dac_serv = NULL;
    struct awbl_dac_service  *p_dac_serv     = NULL;

    /* find handler */
    pfunc_dac_serv = awbl_dev_method_get(p_dev,
                                         AWBL_METHOD_CALL(awbl_dacserv_get));

    /* call method handler to allocate dac serice */

    if (pfunc_dac_serv != NULL) {

        pfunc_dac_serv(p_dev, &p_dac_serv);

        if (p_dac_serv != NULL) {

            struct awbl_dac_service **pp_serv_cur   = &__gp_dac_serv_head;
            
            while (*pp_serv_cur != NULL) {
                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }
            
            *pp_serv_cur       = p_dac_serv;
            p_dac_serv->p_next = NULL;
            
            /* initialize the new server */
            __dac_SPIN_LOCK_INIT(&p_dac_serv->lock);
            p_dac_serv->mutex_id = AW_MUTEX_INIT(p_dac_serv->mutex_busy, 
                                                 AW_SEM_Q_FIFO);
        }
    }
    return AW_OK; /* iterating continue */
}

/**
 * \brief allocate dac services from instance tree
 */
int __dac_serv_alloc (void)
{
    /* todo: count the number of service */

    __gp_dac_serv_head = NULL;
    
    awbl_dev_iterate(__dac_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

/**
 * \brief dac initial
 */
void awbl_dac_init (void)
{
    /* allocate dac services from instance tree */
    __dac_serv_alloc();
}

/**
 * \brief find out a service who accept the channel
 */
struct awbl_dac_service * __dac_ch_to_serv (aw_dac_channel_t ch)
{
    struct awbl_dac_service *p_serv_cur = __gp_dac_serv_head;

    while ((p_serv_cur != NULL)) {

        if ((ch >= __GET_CH_MIN(p_serv_cur)) &&
            (ch <= __GET_CH_MAX(p_serv_cur))) {
            return p_serv_cur;
        }

        p_serv_cur = p_serv_cur->p_next;
    }
    return NULL;
}

/**
 * \brief get dac bits
 */
int aw_dac_bits_get (aw_dac_channel_t ch)
{
    struct awbl_dac_service *p_serv =  __dac_ch_to_serv(ch);
    
    if (NULL == p_serv) {
        return -AW_ENXIO;                         /* ��Ӧͨ�����豸������ */
    }
    
    if(p_serv->p_servfuncs->pfn_bits_get) {
        return p_serv->p_servfuncs->pfn_bits_get(p_serv->p_cookie, 
                                                 __GET_DEV_CHANNEL(p_serv, ch));
    }
    
    return -AW_EFAULT;
}


int aw_dac_vref_get (aw_dac_channel_t ch)
{
    struct awbl_dac_service *p_serv =  __dac_ch_to_serv(ch);
    
    if (NULL == p_serv) {
        return -AW_ENXIO;                           /* ��Ӧͨ�����豸������  */
    }
    
    if(p_serv->p_servfuncs->pfn_vref_get) {
        return p_serv->p_servfuncs->pfn_vref_get(p_serv->p_cookie, 
                                                 __GET_DEV_CHANNEL(p_serv, ch));
    }
    
    return -AW_EFAULT;
}

/**
 * \brief ����һ��DACͨ����������ֵ
 *
 * \param[in] ch  DACͨ����
 * \param[in] val �����õ�������ֵ
 *
 * \retval   AW_OK      �����ɹ�
 * \retval  -AW_ENXIO   DACͨ���Ų�����
 * \retval  -AW_EFAULT  ��������
 * \retval  -AW_EINVAL  ��Ч�����������õ�������ֵ����
 */
aw_err_t aw_dac_val_set (aw_dac_channel_t ch, aw_dac_val_t val)
{
    struct awbl_dac_service *p_serv =  __dac_ch_to_serv(ch);
   
    aw_err_t ret;
    
    if (NULL == p_serv) {
        return -AW_ENXIO;                           /* ��Ӧͨ�����豸������  */
    }
    
    if(p_serv->p_servfuncs->pfn_val_set) {
        
        /* Mutex ��Ϊ�����е�һ��Ԫ�أ�Ӧ�����м����ɶ�Mutex�Ĳ��� */
        __DAC_MUTEX_LOCK(p_serv);
        ret = p_serv->p_servfuncs->pfn_val_set(p_serv->p_cookie, 
                                               __GET_DEV_CHANNEL(p_serv, ch),val);
        __DAC_MUTEX_UNLOCK(p_serv);
        
        return ret;
    }
    
    
    return -AW_EFAULT;
}
                         
/**
 * \brief ʹ��һ��DACͨ�����ģ����
 *
 * \param[in] ch  DACͨ����
 * \param[in] val �����õ�������ֵ
 *
 * \retval   AW_OK      �����ɹ�
 * \retval  -AW_EFAULT  ��������
 * \retval  -AW_ENXIO   DACͨ���Ų�����
 *
 * \note ���ڸ�ʹ�ܺ�����aw_adc_val_set()�����ĵ����Ⱥ��ϵ��˵�����£�
 *       ����ȵ��ø�ʹ�ܺ�������DACʹ�ܺ������ѹΪ0������ͨ����������
 *       aw_dac_val_set()����Ϊ��Ҫ��ֵ������ȵ�������aw_dac_val_set()
 *       ���������˵�ѹֵ����ʹ�ܺ�Ϊ��Ӧ�ĵ�ѹ���ֵ��
 */
aw_err_t aw_dac_enable(aw_dac_channel_t ch)
{
    struct awbl_dac_service *p_serv =  __dac_ch_to_serv(ch);
    
    aw_err_t ret;
    
    if (NULL == p_serv) {
        return -AW_ENXIO;                            /* ��Ӧͨ�����豸������ */
    }
    
    if(p_serv->p_servfuncs->pfn_enable) {
        
        __DAC_MUTEX_LOCK(p_serv);
        ret = p_serv->p_servfuncs->pfn_enable(p_serv->p_cookie, 
                                              __GET_DEV_CHANNEL(p_serv, ch));
        __DAC_MUTEX_UNLOCK(p_serv);
        
        return ret;
    }
    
    return -AW_EFAULT;
}

/**
 * \brief ����һ��DACͨ�����ģ����
 *
 * \param[in] ch  DACͨ����
 * \param[in] val �����õ�������ֵ
 *
 * \retval   AW_OK      �����ɹ�
 * \retval  -AW_EFAULT  ��������
 * \retval  -AW_ENXIO   DACͨ���Ų�����
 */
aw_err_t aw_dac_disable(aw_dac_channel_t ch)
{
    struct awbl_dac_service *p_serv =  __dac_ch_to_serv(ch);
    
    aw_err_t ret;
    
    if (NULL == p_serv) {
        return -AW_ENXIO;                             /* ��Ӧͨ�����豸������ */
    }
    
    if(p_serv->p_servfuncs->pfn_disable) {
        
        __DAC_MUTEX_LOCK(p_serv);
        ret = p_serv->p_servfuncs->pfn_disable(p_serv->p_cookie, 
                                               __GET_DEV_CHANNEL(p_serv, ch));
        __DAC_MUTEX_UNLOCK(p_serv);
        
        return ret;
    }
    
    return -AW_EFAULT;
}
                        
                        
/**
 * \brief ����һ��DACͨ����������ֵ
 *
 * \param[in] ch  DACͨ����
 * \param[in] val �����õ�������ֵ
 *
 * \retval   AW_OK      �����ɹ�
 * \retval  -AW_ENXIO   DACͨ���Ų�����
 * \retval  -AW_EFAULT  ��������
 * \retval  -AW_EINVAL  ��Ч�����������õ�������ֵ����
 */
aw_err_t aw_dac_mv_set(aw_dac_channel_t ch,
                       int              mv)
{
    int          dac_bits;
    aw_dac_val_t dac_val;
    int          dac_vref;
        
    dac_vref = aw_dac_vref_get(ch);
    dac_bits = aw_dac_bits_get(ch);
    
    if (dac_bits < 0) {
        return dac_bits;
    }
    
    if (dac_vref < 0) {
        return dac_vref;
    }
    
    dac_val = mv * (1 << dac_bits) / dac_vref;
    
    return aw_dac_val_set(ch, dac_val);
}
 
/* end of file */
