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
 * \brief AWBus DMA �ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_dma.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 13-09-30  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_DMA_H
#define __AWBL_DMA_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "apollo.h"
#include "aw_spinlock.h"
#include "aw_sem.h"
#include "aw_dma.h"

/** \brief DMA ͨ��״̬ */
typedef enum awbl_dma_chan_status {

    /** \brief δ��ʼ�� */
    AWBL_DMA_CHAN_STATUS_NOT_INITED = 0,

    /** \brief ͨ������ */
    AWBL_DMA_CHAN_STATUS_IDLE,

    /** \brief ͨ������ */
    AWBL_DMA_CHAN_STATUS_RUN,

    /** \brief ͨ����ͣ */
    AWBL_DMA_CHAN_STATUS_PAUSED,

    /** \brief ͨ������ */
    AWBL_DMA_CHAN_STATUS_ERROR

} awbl_dma_chan_status_t;

/** \brief DMA ������״̬ */
typedef enum awbl_dma_status {

    /** \brief δ��ʼ�� */
    AWBL_DMA_STATUS_NOT_INITED = 0,

    /** \brief DMA ���� */
    AWBL_DMA_STATUS_IDLE,

    /** \brief DMA ���� */
    AWBL_DMA_STATUS_RUN,

    /** \brief DMA ��ͣ */
    AWBL_DMA_STATUS_PAUSE,

    /** \brief DMA ���� */
    AWBL_DMA_STATUS_ERROR

} awbl_dma_status_t;

/**
 * \brief AWBus DMA ͨ��ʵ��
 */
typedef struct awbl_dma_chan_res {

    /** \brief ��Щ��Ա��DMA��������ʼ���͸��� */
    bool                      allocated;    /**< \brief �Ƿ��ѱ����� */
    void                     *p_dmac_spec;  /**< \brief DMA�������� */
    struct awbl_dma_service  *p_serv;       /**< \brief ��Ӧ��DMA����ʵ�� */
    awbl_dma_chan_status_t    status;       /**< \brief DMA ͨ��״̬ */
    struct aw_list_head       trans_list;   /**< \brief ���������б� */
    aw_spinlock_isr_t         isr_lock;     /**< \brief �����б�ķ���  */

    AW_MUTEX_DECL(dev_lock);

    /** \brief ��Щ��Ա��DMA������ʼ�� */
    unsigned    chan_no;        /**< \brief ͨ���� */
    unsigned    chan_pri;       /**< \brief ͨ�����ȼ� */
    bool        sup_any;        /**< \brief �Ƿ�֧�� AW_DMA_CHAN_ANY ����*/
    uint32_t    flags;          /**< \brief ���Ա�־ (��������0) */
    void       *p_dmac_ctrl;    /**< \brief ��DMA����ʹ�� */

} awbl_dma_chan_res_t;

/** \brief ����DMAͨ����Դ */
#define AWBL_DMA_CHAN_RES_LOCK(p_chan_res) \
    AW_MUTEX_LOCK((p_chan_res)->dev_lock, AW_SEM_WAIT_FOREVER)

/** \brief �ͷ�DMAͨ����Դ */
#define AWBL_DMA_CHAN_RES_UNLOCK(p_chan_res) \
    AW_MUTEX_UNLOCK((p_chan_res)->dev_lock)

/** \brief ����DMAͨ����Դ(��������ʽ) */
#define AWBL_DMA_CHAN_RES_SPIN_LOCK(p_chan_res) \
    aw_spinlock_isr_take(&(p_chan_res)->isr_lock)

/** \brief �ͷ�DMAͨ����Դ(��������ʽ) */
#define AWBL_DMA_CHAN_RES_SPIN_UNLOCK(p_chan_res) \
    aw_spinlock_isr_give(&(p_chan_res)->isr_lock)



/** \brief �ӿ������Ĵ������ȡ��DMA���� */
aw_static_inline aw_dma_trans_req_t *
    awbl_dma_trans_req_pop(awbl_dma_chan_res_t *p_chan_res)
{
    AWBL_DMA_CHAN_RES_SPIN_LOCK(p_chan_res);

    if (aw_list_empty_careful(&p_chan_res->trans_list)) {
        AWBL_DMA_CHAN_RES_SPIN_UNLOCK(p_chan_res);
        return NULL;
    } else {
        struct aw_list_head *p_node = p_chan_res->trans_list.next;
        aw_list_del(p_node);
        AWBL_DMA_CHAN_RES_SPIN_UNLOCK(p_chan_res);
        return aw_list_entry(p_node, aw_dma_trans_req_t, node);
    }
}

/**
 * \brief ��ʼ��DMAͨ��ʵ��
 *
 * \param p_chan_res    DMA ͨ��ʵ��
 * \param chan_no       ʵ���� DMA �������϶�Ӧ��ͨ����
 * \param chan_pri      Ĭ�����ȼ�
 * \param sup_any       �Ƿ�֧�� AW_DMA_CHAN_ANY ����
 * \param flags         ���Ա�־ (��������0)
 */
static aw_inline void awbl_dma_chan_res_init(awbl_dma_chan_res_t *p_chan_res,
                                             unsigned             chan_no,
                                             unsigned             chan_pri,
                                             bool                 sup_any,
                                             uint32_t             flags,
                                             void                *p_dmac_ctrl)
{
    p_chan_res->chan_no     = chan_no;
    p_chan_res->chan_pri    = chan_pri;
    p_chan_res->sup_any     = sup_any;
    p_chan_res->flags       = flags;
    p_chan_res->p_dmac_ctrl = p_dmac_ctrl;
}

/**
 * \brief AWBus DMA ������
 */
struct awbl_dma_servopts {

    /**
     * \brief DMA ͨ������̽��
     *
     * DMA �������ڷ����Ӧ�� DMA ͨ��֮ǰ������ô˺���������Ȩ���� DMA ������
     * DMA ������Ҫ�ж� p_chan_res ��������ͨ���ܷ񱻷���: �����ܱ����䣬��Ҫ��
     * AW_ERROR ; ���ܹ������䣬���ʼ����Ӧ�� DMA ͨ������󷵻� AW_OK��
     *
     * \param p_serv        DMA ����
     * \param p_chan_res    ͨ����Դ
     * \param chan_no       DMA ͨ����
     * \param chan_pri      DMA ͨ�����ȼ� (������к�)
     * \param flags         ���Ա�־ (����)
     * \param p_dmac_spec   ��DMA��������������
     *
     * \retval AW_OK        ���Է���
     * \retval AW_ERROR     �����Է���
     *
     * \attention ��������Ҫ�� awbl_dev_init2() ���ܹ�ʹ��
     */
   aw_err_t  (*pfunc_chan_alloc_probe)(struct awbl_dma_service  *p_serv,
                                       struct awbl_dma_chan_res *p_chan_res);

    /**
     * \brief �ͷ�DMAͨ��
     * \attention ��������Ҫ�� awbl_dev_init2() ���ܹ�ʹ��
     */
    aw_err_t (*pfunc_dma_chan_free)(struct awbl_dma_service  *p_serv,
                                    awbl_dma_chan_res_t      *chan_res);



    /**
     * \brief ��������
     * \attention ��������Ҫ�� awbl_dev_init2() ���ܹ�ʹ��
     */
    aw_err_t (*launch_transfer) (awbl_dma_chan_res_t *chan_res);

    /**
     * \brief ���ô���
     */
    aw_err_t (*transfer_setup) (struct awbl_dma_service  *p_serv,
                                awbl_dma_chan_res_t      *chan_res,
                                aw_dma_trans_req_t       *p_trans_req);

};

/**
 * \brief AWBus DMA ����ʵ��
 */
typedef struct awbl_dma_service {
    bool    initialized;        /**< \brief ��ʼ���Ƿ���� */

    size_t  queue_num;          /**< \brief �����������ÿ�������в�ͬ���ȼ�*/
    size_t  chan_num;           /**< \brief ͨ����*/

    struct awbl_dma_chan_res  *p_chan_res;      /**< \brief ͨ����Դ */

    const struct awbl_dma_servopts *p_servopts; /**< \brief DMA �������غ��� */

    awbl_dma_status_t status;           /**< \brief DMA ״̬ */
    awbl_dev_t *p_dmac;                 /**< \brief DMA ������ */

    AW_MUTEX_DECL(dev_lock);            /**< \brief ���񻥳� */

    unsigned last_chan_id;              /**< \brief �ϴη����ͨ���� */
    struct awbl_dma_service *p_next;    /**< \brief ָ����һ�� DMA ���� */
} awbl_dma_service_t;

/** \brief ����DMAͨ����Դ(��������ʽ) */
#define AWBL_DMA_SERV_SPIN_LOCK(p_serv) \
    aw_spinlock_isr_take(&(p_serv)->isr_lock)

/** \brief �ͷ�DMAͨ����Դ(��������ʽ) */
#define AWBL_DMA_SERV_SPIN_UNLOCK(p_serv) \
    aw_spinlock_isr_give(&(p_serv)->isr_lock)


/**
 * \brief ��ʼ��DMA����ʵ��(AWBus ��ʼ����1�׶�����������)
 *
 * \param p_chan_res    DMA ͨ��ʵ��
 * \param chan_no       ʵ���� DMA �������϶�Ӧ��ͨ����
 * \param chan_pri      Ĭ�����ȼ�
 * \param sup_any       �Ƿ�֧�� AW_DMA_CHAN_ANY ����
 * \param flags         ���Ա�־ (��������0)
 */
static aw_inline void
    awbl_dma_service_init(awbl_dma_service_t             *p_serv,
                          size_t                          chan_num,
                          size_t                          queue_num,
                          struct awbl_dma_chan_res       *p_chan_res,
                          const struct awbl_dma_servopts *p_servopts)
{
    int i;

    p_serv->chan_num   = chan_num;
    p_serv->queue_num  = queue_num;
    p_serv->p_chan_res = p_chan_res;
    p_serv->p_servopts = p_servopts;

    p_serv->last_chan_id = chan_num - 1;
    p_serv->p_next       = NULL;
    p_serv->initialized  = false;

    for (i = 0; i < p_serv->chan_num; i++) {

        p_chan_res[i].allocated   = false;
        p_chan_res[i].p_dmac_spec = NULL;
        p_chan_res[i].p_serv      = p_serv;
        p_chan_res[i].status      = AWBL_DMA_CHAN_STATUS_IDLE;

        aw_list_head_init(&p_chan_res[i].trans_list);
        aw_spinlock_isr_init(&p_chan_res[i].isr_lock, 0);
        AW_MUTEX_INIT(p_chan_res[i].dev_lock, AW_SEM_Q_PRIORITY);
    }

    AW_MUTEX_INIT(p_serv->dev_lock, AW_SEM_Q_PRIORITY);

    p_serv->status      = AWBL_DMA_STATUS_IDLE;
    p_serv->initialized = true;
}

/**
 * \brief AWBus DMA �ӿ�ģ���ʼ��
 *
 * \attention ������Ӧ���� awbl_dev_init1() ֮���� awbl_dev_init2() ֮ǰ����
 */
void awbl_dma_lib_init(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_DMA_H */

/* end of file */

