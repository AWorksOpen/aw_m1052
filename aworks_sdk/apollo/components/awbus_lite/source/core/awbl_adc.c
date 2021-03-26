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
 * \brief AWBus ADC interface implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 2.01 15-06-11  tee, add some interface for samples rate  
 * - 2.00 14-09-25  ehf, modify some interface and implemention to support
 *                  read mulitiple samples once
 * - 1.00 12-11-06  zyr, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_adc.h"
#include "awbl_adc.h"
#include "aw_assert.h"
#include "aw_delayed_work.h"
#include "aw_msgq.h"
#include "string.h"
#include "aw_vdebug.h"

/**
 * \brief �ͻ���״̬
 */
#define __CLIENT_STAT_INITIALIZED           1      /**< \brief �ͻ��˳�ʼ����� */
#define __CLIENT_STAT_INPROCESS             2      /**< \brief �ͻ������ڱ������� */
#define __CLIENT_STAT_CANCELED              3      /**< \brief �ͻ��˱�ȡ�� */

/** \brief pointer to the first adc service */
struct awbl_adc_service *__gp_adc_serv_head  = NULL;


#define __GET_CH_MAX(p_serv)          ((p_serv)->p_servinfo->ch_max)
#define __GET_CH_MIN(p_serv)          ((p_serv)->p_servinfo->ch_min)
#define __GET_DEV_CHANNEL(p_serv, ch) ((ch) - __GET_CH_MIN((p_serv)))

#define __ADC_SPIN_LOCK_INIT(lock) \
    aw_spinlock_isr_init(lock, 0)

#define __ADC_SPIN_LOCK_IRQSAVE(lock, flag) \
    (void)flag;aw_spinlock_isr_take(lock)

#define __ADC_SPIN_UNLOCK_IRQRESTORE(lock, flag) \
    (void)flag;aw_spinlock_isr_give(lock)


/**
 * \name call driver functions macros
 * @{
 */
#define __SERV_START(p_serv, p_client)                  \
    (p_serv)->p_servfuncs->pfn_start((p_serv)->p_cookie,\
                                     __GET_DEV_CHANNEL((p_serv),(p_client)->channel),\
                                     (p_client)->p_desc,\
                                     (p_client)->desc_num,\
                                     (p_client)->count,     \
                                      __adc_seq_complete_cb, \
                                     (p_serv)->p_cur)
                                     
#define __SERV_STOP(p_serv, p_client)                  \
    (p_serv)->p_servfuncs->pfn_stop((p_serv)->p_cookie,\
                                    __GET_DEV_CHANNEL((p_serv),(p_client)->channel))
                                     
                                     
#define __SERV_BITS_GET(p_serv, channel) \
    (p_serv)->p_servfuncs->pfn_bits_get((p_serv)->p_cookie, channel)

#define __SERV_VREF_GET(p_serv, channel) \
    (p_serv)->p_servfuncs->pfn_vref_get((p_serv)->p_cookie, channel)
    
#define __SERV_RATE_GET(p_serv, channel, p_rate) \
    (p_serv)->p_servfuncs->pfn_rate_get((p_serv)->p_cookie, channel, p_rate)

#define __SERV_RATE_SET(p_serv, channel,rate) \
    (p_serv)->p_servfuncs->pfn_rate_set((p_serv)->p_cookie, channel, rate)

#define __SERV_FUNC_SET(p_serv, pfn_seq_complete, p_arg) \
    (p_serv)->p_servfuncs->pfn_seq_complete_set((p_serv)->p_cookie, pfn_seq_complete, p_arg)

/** @} */

/******************************************************************************
 forward declarations
 ******************************************************************************/

aw_local void __adc_seq_complete_cb (void *p_arg, aw_err_t stat);

/******************************************************************************
 implementation
 ******************************************************************************/

/**
 * \brief allocate ADC service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __adc_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t     pfunc_adc_serv = NULL;
    struct awbl_adc_service  *p_adc_serv     = NULL;
    struct awbl_adc_service **pp_serv_cur    = &__gp_adc_serv_head;

    /* find handler */
    pfunc_adc_serv = awbl_dev_method_get(p_dev,
                                         AWBL_METHOD_CALL(awbl_adcserv_get));

    /* call method handler to allocate ADC serice */

    if (pfunc_adc_serv != NULL) {

        pfunc_adc_serv(p_dev, &p_adc_serv);

        if (p_adc_serv != NULL) {

             while (*pp_serv_cur != NULL) {
                 pp_serv_cur = &(*pp_serv_cur)->p_next;
             }
             *pp_serv_cur        = p_adc_serv;
             p_adc_serv->p_next  = NULL;

            /* initialize the new server */
            p_adc_serv->p_cur = NULL;
            __ADC_SPIN_LOCK_INIT(&p_adc_serv->lock);
            p_adc_serv->semb_id = AW_SEMB_INIT(p_adc_serv->semb_busy,
                                               1,
                                               AW_SEM_Q_FIFO);
            /* initialize two server list */
            AW_INIT_LIST_HEAD(&p_adc_serv->adc_urgent);
            AW_INIT_LIST_HEAD(&p_adc_serv->adc_normal);
        }
    }
    return AW_OK; /* iterating continue */
}

/**
 * \brief allocate adc services from instance tree
 */
int __adc_serv_alloc (void)
{
    /* todo: count the number of service */

    awbl_dev_iterate(__adc_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

/**
 * \brief adc initial
 */
void awbl_adc_init (void)
{
    /* allocate adc services from instance tree */
    __adc_serv_alloc();
}

/**
 * \brief find out a service who accept the channel
 */
struct awbl_adc_service * __adc_ch_to_serv (aw_adc_channel_t ch)
{
    struct awbl_adc_service *p_serv_cur = __gp_adc_serv_head;

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
 * \brief ��ȡһ��ADCͨ����λ��
 */
int aw_adc_bits_get (aw_adc_channel_t ch)
{
    struct awbl_adc_service *p_serv =  __adc_ch_to_serv(ch);
    if (NULL == p_serv) {
        return -AW_ENXIO;
    }

    return __SERV_BITS_GET(p_serv, __GET_DEV_CHANNEL(p_serv, ch));
}


/**
 * \brief ��ȡADC������
 */
aw_err_t aw_adc_rate_get (aw_adc_channel_t ch, uint32_t *p_rate)
{
    struct awbl_adc_service *p_serv =  __adc_ch_to_serv(ch);
    if (NULL == p_serv) {
        return -AW_ENXIO;
    }

    return __SERV_RATE_GET(p_serv, __GET_DEV_CHANNEL(p_serv, ch),p_rate);
}

/**
 * \brief ����һ��ADCͨ���Ĳ�����
 */
aw_err_t aw_adc_rate_set (aw_adc_channel_t ch, uint32_t rate)
{
    struct awbl_adc_service *p_serv =  __adc_ch_to_serv(ch);
    if (NULL == p_serv) {
        return -AW_ENXIO;
    }

    return __SERV_RATE_SET(p_serv, __GET_DEV_CHANNEL(p_serv, ch), rate);
}


/**
 * \brief ��ȡһ��ADCͨ���Ĳο���ѹ����λ��mV��
 */
int aw_adc_vref_get (aw_adc_channel_t ch)
{
    struct awbl_adc_service *p_serv =  __adc_ch_to_serv(ch);
    if (NULL == p_serv) {
        return -AW_ENXIO;
    }

    return __SERV_VREF_GET(p_serv, __GET_DEV_CHANNEL(p_serv, ch));
}

/**
 * \brief ��һ��ͨ���Ĳ���ֵֵת��Ϊ��ѹֵ����λ��mV��
 *
 * \param[in] ch     ADCͨ����
 * \param[in] p_val  ���ADC����ֵ
 * \param[in] cnt    ADC����ֵ�ĸ���
 * \param[in] p_mv   ���ת������ĵ�ѹֵ(mV)
 *
 * \retval   AW_OK   ת���ɹ�
 * \retval -AW_ENXIO ADCͨ���Ų�����
 *
 * \note �������Ҫ����ԭ����ֵ���ݣ�Ϊ�˽�ʡ�ռ䣬����ֱ�ӽ�p_mv��ֵ����Ϊp_val.
 */
aw_err_t aw_adc_val_to_mv(aw_adc_channel_t  ch,
                          void             *p_val,
                          uint32_t          cnt,
                          uint32_t         *p_mv)
{
    struct awbl_adc_service *p_serv =  __adc_ch_to_serv(ch);
    uint64_t   ref_mv, temp;
    uint32_t   adc_bits;
    int        i;

    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    ref_mv   = __SERV_VREF_GET(p_serv, __GET_DEV_CHANNEL(p_serv, ch));
    adc_bits = __SERV_BITS_GET(p_serv, __GET_DEV_CHANNEL(p_serv, ch));
    
    if (adc_bits <= 8) {
        uint8_t *ptr = (uint8_t *)p_val;
        /* convert sample values to mill volt value,p_mv can be p_val */
        for (i = 0; i < cnt; i++) {
            temp = ref_mv * ptr[i];
            p_mv[i] = temp /( (1 << adc_bits) -1);
        }        
    } else if (adc_bits <= 16) {
        uint16_t *ptr = (uint16_t *)p_val;
        /* convert sample values to mill volt value,p_mv can be p_val */
        for (i = 0; i < cnt; i++) {
            temp = ref_mv * ptr[i];
            p_mv[i] = temp /( (1 << adc_bits) -1);
        }    
    }  else {
        uint32_t *ptr = (uint32_t *)p_val;
        /* convert sample values to mill volt value,p_mv can be p_val */
        for (i = 0; i < cnt; i++) {
            temp = ref_mv * ptr[i];
            p_mv[i] = temp /( (1 << adc_bits) -1);
        }    
    }
    return AW_OK;
}

/**
 * \brief ������ɻص�������ͨ��ֱ���޸����һ������������ɻص�����
 */
aw_local void __adc_seq_complete_cb (void *p_arg, aw_err_t stat)
{
    struct aw_adc_client    *p_client = (struct aw_adc_client *)p_arg;
    struct awbl_adc_service *p_serv   = p_client->p_serv;

    uint32_t cpu_flag = 0;

    /**
     * \brief ת�����д�����Ϊ0��Ϊ0�����������ϵ�ת������
     *        ת������������Ҳ��ֱ��ɾ���ÿͻ��� 
     */
    if(p_client->count == 1 || stat != AW_OK) {

        __ADC_SPIN_LOCK_IRQSAVE(&p_serv->lock, cpu_flag);
        aw_list_del_init(&p_client->node);
        
        /* �ص���ʼ̬ */
        p_client->stat = __CLIENT_STAT_INITIALIZED;
 
        if( !aw_list_empty(&p_serv->adc_urgent) ) {
            p_serv->p_cur = aw_list_first_entry(&p_serv->adc_urgent,
                                             struct aw_adc_client,
                                             node);
        } else if ( !aw_list_empty(&p_serv->adc_normal) ) {
            p_serv->p_cur = aw_list_first_entry(&p_serv->adc_normal,
                                             struct aw_adc_client,
                                             node);
        } else {
            p_serv->p_cur = NULL;
        }
        
        __ADC_SPIN_UNLOCK_IRQRESTORE(&p_serv->lock, cpu_flag);
        
    } else {
        if (p_client->count) {
            p_client->count--;
        }
        
        /* ��ǰת�����ǽ������񣬲��Ҵ��ڽ�������,������ǰ�����л����������� */
        if( !p_client->urgent && !aw_list_empty(&p_serv->adc_urgent) ) {
            
            /* ֹͣ��ǰ���� */
            __SERV_STOP(p_client->p_serv,p_client);
            
            p_serv->p_cur = aw_list_first_entry(&p_serv->adc_urgent,
                                                 struct aw_adc_client,
                                                 node);

                                                 
        }
    }

    /* ����Ŀͻ����л��ˣ���Ҫ�������� */
    if (p_serv->p_cur != p_client && p_serv->p_cur != NULL) {
        /* start once convert */
        __SERV_START(p_serv,p_serv->p_cur);
    }
}

/**
 * \brief initialize an adc client
 */
aw_err_t aw_adc_client_init (struct aw_adc_client  *p_client,
                             aw_adc_channel_t       ch,
                             aw_bool_t              urgent)
{
    if (NULL == p_client) {
        return -AW_EINVAL;
    }

    AW_INIT_LIST_HEAD(&p_client->node);

    p_client->channel   = ch;
    p_client->urgent    = urgent;
    p_client->p_desc    = NULL;
    p_client->p_serv    =  __adc_ch_to_serv(ch);

    /* no service for current adc channel, init fail */
    if (NULL == p_client->p_serv) {
        return -AW_ENXIO;
    }
    
    p_client->data_bits          =  __SERV_BITS_GET(p_client->p_serv, ch);
    
    if (p_client->data_bits <= 8) {
        p_client->data_bits = 8;
    } else if (p_client->data_bits <= 16) {
        p_client->data_bits = 16;
    } else {
        p_client->data_bits = 32;
    }

    /* ��ʼ����ɣ��ͻ���׼������                */
    p_client->stat          = __CLIENT_STAT_INITIALIZED;

    return AW_OK;
}

/**
 * \brief �����ͻ���ת��
 */
aw_err_t aw_adc_client_start (struct aw_adc_client  *p_client,
                              aw_adc_buf_desc_t     *p_desc,
                              int                    desc_num,
                              uint32_t               count)
{ 
    struct awbl_adc_service *p_serv   = NULL;
    aw_err_t                 err      = AW_OK;
    uint32_t                 cpu_flag = 0;
    int  i;

    if (NULL == p_client || desc_num == 0) {
        return -AW_EINVAL;
    }

    if (p_client->p_serv == NULL) {
        return -AW_EINVAL;
    }

    for (i = 0; i < desc_num; i++) {
        if (p_desc[i].length == 0) {
            return -AW_EINVAL;
        }
    }
    
    /* ����������ֻ�ܵ���ת�� */
    if (count != 1 && desc_num == 1) {
        return -AW_EINVAL;
    }
    
    p_serv = p_client->p_serv;

    __ADC_SPIN_LOCK_IRQSAVE(&p_serv->lock, cpu_flag);

    /* �ͻ������ڴ�����,������������첽����ͬһ���ͻ���  */
    if (p_client->stat == __CLIENT_STAT_INPROCESS) {
        return -AW_EALREADY;
    } else if (p_client->stat == __CLIENT_STAT_CANCELED) {
        /* �ͻ���ȡ�������һ��ת����δ��ɣ���ʱ��ֻ��Ҫ�ٴ����¸�ֵ������������*/

    } else if (p_client->stat != __CLIENT_STAT_INITIALIZED) {
        return -AW_EPERM;                  /* �ͻ���δ����ʼ��������������  */
    }

    /* The p_client already in list or the p_client not been initialized */
    if ( !aw_list_empty(&(p_client->node)) ) {
        return -AW_EPERM;
    }
 
    /* ��ʶ�ͻ������ڴ����� */
    p_client->stat               = __CLIENT_STAT_INPROCESS;
    p_client->count              = count;
    p_client->p_desc             = p_desc;
    p_client->desc_num           = desc_num;
 
    __ADC_SPIN_UNLOCK_IRQRESTORE(&p_serv->lock, cpu_flag);

    /* ȷ��һ��ADC����ֻ��ͬʱ��һ���ͻ���ռ�� */
    err = aw_semb_take(p_serv->semb_id, AW_SEM_WAIT_FOREVER);
    
    if (AW_OK != err) {
        p_client->stat    = __CLIENT_STAT_INITIALIZED;
        goto exit;
    }

    __ADC_SPIN_LOCK_IRQSAVE(&p_serv->lock, cpu_flag);
    
    if (p_client->urgent) {
        /* add client to urgent list */
        /* ��ӵ�����ͷ����ȡ���Ǵ�����ͷȡ��   */
        aw_list_add(&p_client->node, &p_serv->adc_urgent);         
    } else {
        /* ��ӵ�����β��  */
        aw_list_add_tail(&p_client->node, &p_serv->adc_normal);    
    }
    __ADC_SPIN_UNLOCK_IRQRESTORE(&p_serv->lock, cpu_flag);
    
    /* ��ǰADC���������ڴ���Ŀͻ��ˣ���ȡ��һ���ͻ��� */
    if(p_serv->p_cur == NULL) {

        __ADC_SPIN_LOCK_IRQSAVE(&p_serv->lock, cpu_flag);
        
        if( !aw_list_empty(&p_serv->adc_urgent) ) {

            p_serv->p_cur = aw_list_first_entry(&p_serv->adc_urgent,
                                                struct aw_adc_client,
                                                node);

        } else if ( !aw_list_empty(&p_serv->adc_normal) ) {

            p_serv->p_cur = aw_list_first_entry(&p_serv->adc_normal,
                                                 struct aw_adc_client,
                                                 node);
        }
        
        __ADC_SPIN_UNLOCK_IRQRESTORE(&p_serv->lock, cpu_flag);

        /* һ������һ���ͻ��ˣ����p_serv->p_cur��ʱһ����Ϊ��  */
        err = __SERV_START(p_serv,p_serv->p_cur);
      
        if (err != AW_OK) {
            p_client->stat = __CLIENT_STAT_INITIALIZED;
        }
    }

    exit:
        aw_semb_give(p_serv->semb_id);
        return err;
}

/******************************************************************************/

/**
 * \brief read done callback function
 */
aw_local void __adc_sync_read_done (void *p_cookie, aw_err_t stat)
{
    aw_msgq_id_t rd_msgq = *((aw_msgq_id_t *)p_cookie);

    if (p_cookie) {
        aw_msgq_send(rd_msgq,
                     &stat,
                     sizeof(aw_err_t),
                     AW_MSGQ_NO_WAIT,
                     AW_MSGQ_PRI_URGENT);
    }
}

/**
 * \brief read adc sample values in synchronization mode
 *  async read can not be canceled,once start,only return when convert complete.
 */
aw_err_t aw_adc_sync_read (aw_adc_channel_t ch,
                           void            *p_val,
                           uint32_t         samples,
                           aw_bool_t        urgent)
{
    struct aw_adc_client     rd_client;
    aw_msgq_id_t             rd_msgq;
    aw_adc_buf_desc_t        buf_desc;
    
    aw_err_t                 err = AW_OK;

    AW_MSGQ_DECL(sem_sync,1,sizeof(aw_err_t));
    memset(&sem_sync,0,sizeof(sem_sync) );
    rd_msgq = AW_MSGQ_INIT(sem_sync, 1, sizeof(aw_err_t), AW_SEM_Q_PRIORITY);
    if (NULL == rd_msgq) {
        return -AW_ENOMEM;
    }

    err = aw_adc_client_init(&rd_client,ch,urgent);

    if (err != AW_OK) {
        return err;
    }
    
    buf_desc.length       = samples;
    buf_desc.pfn_complete = __adc_sync_read_done;
    buf_desc.p_arg        = &rd_msgq;
    buf_desc.p_buf        = p_val;

    err = aw_adc_client_start(&rd_client, &buf_desc, 1, 1);

    /* wait for adc client convert finish */
    if (AW_OK == err) {
        aw_msgq_receive(rd_msgq, &err, sizeof(aw_err_t),AW_MSGQ_WAIT_FOREVER);
    }
    return err;
}

/******************************************************************************/

/**
 * \brief cancel an adc client
 */
aw_err_t aw_adc_client_cancel (struct aw_adc_client *p_client)
{
    struct awbl_adc_service *p_serv = NULL;
    uint32_t                 cpu_flag = 0;

    if (NULL == p_client) {
        return -AW_EINVAL;
    }

    if (p_client->p_serv == NULL) {
        return -AW_EINVAL;
    }

    p_serv = p_client->p_serv;

    /* �ÿͻ��˸���û������ */
    if (p_client->stat != __CLIENT_STAT_INPROCESS) {
        return -AW_EPERM;
    }

    __ADC_SPIN_LOCK_IRQSAVE(&p_serv->lock, cpu_flag);

    /* �ÿͻ��˵�ǰ���ڴ�����  */
    if (p_serv->p_cur == p_client) {

        /* �ָ��ж� */
        __ADC_SPIN_UNLOCK_IRQRESTORE(&p_serv->lock, cpu_flag);
        
        __SERV_STOP(p_serv,p_client);
 
        /* ���¹ر��ж� */
        __ADC_SPIN_LOCK_IRQSAVE(&p_serv->lock, cpu_flag);
        
        /* �����������̱��жϣ�����ת����ɣ���ʱ p_client ���ܱ�ɾ���� */
        if (!aw_list_empty(&p_client->node)) {
            
            aw_list_del_init(&p_client->node);

            if( !aw_list_empty(&p_serv->adc_urgent) ) {
                p_serv->p_cur = aw_list_first_entry(&p_serv->adc_urgent,
                                                 struct aw_adc_client,
                                                 node);
            } else if ( !aw_list_empty(&p_serv->adc_normal) ) {
                p_serv->p_cur = aw_list_first_entry(&p_serv->adc_normal,
                                                 struct aw_adc_client,
                                                 node);
            } else {
                p_serv->p_cur = NULL;
            }
            
            if (p_serv->p_cur) {
                __SERV_START(p_serv,p_serv->p_cur);
            }
        }
        
        /* �ص���ʼ̬ */
        p_client->stat = __CLIENT_STAT_INITIALIZED;        

    } else if (!aw_list_empty(&p_client->node)) {     /* �ͻ��˻�δ��ʼ���� */

        aw_list_del_init(&p_client->node);            /* ֱ��ɾ�� */
        
        p_client->stat = __CLIENT_STAT_INITIALIZED;   /* �ص���ʼ��״̬ */
    }
    
    __ADC_SPIN_UNLOCK_IRQRESTORE(&p_serv->lock, cpu_flag);

    return AW_OK;
}

/* end of file */
