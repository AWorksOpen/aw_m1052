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
 * \brief ��ҵ���п�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_job_queue.h
 *
 * �û�����ѡ���ڴ�����ҵ����ʱ���Ƿ�����׼��ҵ�أ�
 * ���������½���ҵʱ������ѡ��������ҵ�����ڲ��Ŀ�����ҵ��
 * ����ѡ���û��ṩ��ҵ�Ŀռ䣬����������Ϣ��
 *
 * \par ʹ�ñ�׼��ҵ��
 * \code
 *
 * #define STD_JOB_NUM     10
 * #define MY_JOB_PRIO     20
 *
 * static aw_job_queue_id_t     my_jobq_id;
 * static aw_qjob_std_pool_t    my_pool;
 * static aw_qjob_std_pool_id_t my_pool_id = &my_pool;
 *
 * static void std_jobq_init(void)
 * {  
 *     aw_job_queue_std_pool_init(my_pool_id);  
 *
 *     aw_job_queue_std_jobs_alloc(my_pool_id, STD_JOB_NUM); 
 *
 *     my_jobq_id = aw_job_queue_create(my_pool_id);   
 * }
 *
 * static void add_std_job(void)
 * {
 *     aw_job_queue_std_post(my_jobq_id,
 *                           MY_JOB_PRIO,
 *                           callback_func,
 *                    (void*)0,
 *                    (void*)0,
 *                    (void*)0,
 *                    (void*)0);
 * } 
 * \endcode 
 *
 * \par ʹ���û��ṩ����ҵ�ռ�
 * \code
 *
 * #define MY_JOB_PRIO     20
 *
 * static aw_qjob_t       user_job;
 * static aw_job_queue_t  my_jobq;
 *
 * static void user_jobq_init(void)
 * {  
 *     aw_job_queue_init(&my_jobq, NULL);   
 * }
 *
 * static void add_user_job(void)
 * {
 *     aw_job_queue_fill_job(&user_job, 
 *                           callback_func, 
 *                    (void*)0, 
 *                           MY_JOB_PRIO, 
 *                           0);
 *
 *     aw_job_queue_post(&my_jobq, &user_job);
 * } 
 * \endcode 
 *
 * \internal
 * \par modification history:
 * - 1.01 15-06-02  deo, add aw_job_queue_fill_job
 * - 1.00 14-03-19  zen, first implementation
 * \endinternal
 */

#ifndef __AW_JOB_QUEUE_H
#define __AW_JOB_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_task.h"
#include "aw_sem.h"
#include "aw_list.h"
#include "aw_spinlock.h"

/**
 * \addtogroup grp_aw_if_job_queue
 * \copydoc aw_job_queue.h
 * @{
 */

#ifndef AW_SRCFG_QJOB_NUM_PRI
#define AW_SRCFG_QJOB_NUM_PRI   32  /**< \brief ��ҵ�������ȼ����� */
#endif

#if AW_SRCFG_QJOB_NUM_PRI > 32
#error "AW_SRCFG_QJOB_NUM_PRI can not bigger than 32"
#endif

/** \brief ��ǰ��ҵ�Ѿ�����ҵ������ */
#define AW_QJOB_ENQUEUED    0x100

/** \brief ��ǰ��ҵ�Ǵ���ҵ���г��з�������� */
#define AW_QJOB_POOL_ALLOC  0x200

/** \brief ��ҵ����æ */
#define AW_QJOB_BUSY        0x20 

/** \brief ���Ե�ǰ��ҵ�Ƿ�æ */
#define AW_QJOB_IS_BUSY(p_job)  ((p_job)->flags & AW_QJOB_BUSY)

/** \brief ���õ�ǰ��ҵæ */
#define AW_QJOB_SET_BUSY(p_job) ((p_job)->flags |= AW_QJOB_BUSY)

/** \brief ���õ�ǰ��ҵ���� */
#define AW_QJOB_CLR_BUSY(p_job) ((p_job)->flags &= ~AW_QJOB_BUSY)


/** \brief ��ҵ����������ȼ� */
#define AW_QJOB_PRI_LOWEST   0   

/** \brief ��ҵ����������ȼ� */
#define AW_QJOB_PRI_HIGHEST  31    

/** \brief ��ҵ�������ȼ����� */
#define AW_QJOB_NUM_PRI      32    

/** \brief ��ҵ�������ȼ����� */
#define AW_QJOB_PRI_MASK     0x1f

/** 
 * \brief ������ҵ�Ķ���
 *
 * ���ǻ�����ҵ�Ķ��壬�ɷŵ������ṹ���������չ��ҵ�Ķ��塣
 * һ���ִ�к����������Ըýṹ���ָ����Ϊ���������
 */
struct aw_qjob {

    /** \brief ��ҵִ�еĺ���  */
    aw_pfuncvoid_t      func;

    /** \brief ��ҵִ�еĺ����Ĳ��� */
    void               *p_arg;

    /** \brief ��ҵ�����ȼ� */
    uint16_t            pri;

    /** \brief ��ҵ�ı�� */
    uint16_t            flags;

    /** \brief ��ҵ���еĽڵ� */
    struct aw_list_head node;
};

/** \brief ������ҵ */
typedef struct aw_qjob aw_qjob_t;

/**
 * \brief ��׼��ҵ�Ķ���
 * \extends aw_qjob
 */
struct aw_qjob_std {
    
    /** \brief ������ҵ */
    aw_qjob_t base;
    
    /** \brief ��׼����1 */
    void     *arg1;
    
    /** \brief ��׼����2 */
    void     *arg2;
    
    /** \brief ��׼����3 */
    void     *arg3;
    
    /** \brief ��׼����4 */
    void     *arg4;

};

/** \brief ��׼��ҵ */
typedef struct aw_qjob_std aw_qjob_std_t;


/** \brief ��׼��ҵ�صĶ��� */
struct aw_qjob_std_pool {
    
    /** \brief ��׼��ҵ�Ŀ������� */
    struct aw_list_head  free;
    
    /** \brief ��׼��ҵ���� */
    struct aw_list_head  blocks;    
    
    /** \brief �ṩ����ҵ�������� */
    size_t               ref_count; 
    
    /** \brief ������ָ�� */
    aw_spinlock_isr_t    *p_lock;

};

/** \brief ��׼��ҵ�� */
typedef struct aw_qjob_std_pool aw_qjob_std_pool_t;

/** \brief ��׼��ҵ�� ID */
typedef aw_qjob_std_pool_t  *aw_qjob_std_pool_id_t;


/** \brief ��ҵ���еĶ��� */
struct aw_job_queue {

    /** \brief �ȴ����� */
    struct aw_list_head  jobs[AW_SRCFG_QJOB_NUM_PRI];

    /** \brief λͼ����¼��ǰ������ȼ� */
    uint32_t             active_pri;

    /** \brief ��׼��ҵ�� */
    aw_qjob_std_pool_t  *p_std_pool;
    
    /** \brief ��ǰ���е����� ID */
    aw_task_id_t         task_id;   

    /**
     * \name ��Ա����
     * @{
     */
    AW_SEMB_DECL(semb);  /**< \brief �����ƻ����� */
    /** @} */

    /** \brief ��ҵ���б�� */
    uint32_t             flags;

/** \brief ��ҵ������������ */
#define AW_JOBQ_FLG_RUNNING  0x01

/** \brief ��ҵ���иմ��� */
#define AW_JOBQ_FLG_CREATED  0x02

    /** \brief �������û��ռ� */
    void           *user_val;

    /** \brief ������ */
    aw_spinlock_isr_t   lock;

};

/** \brief ��ҵ���� */
typedef struct aw_job_queue  aw_job_queue_t;

/**
 * \brief ��ҵ���� ID
 */
typedef aw_job_queue_t *aw_job_queue_id_t;

/**
 * \brief �����ҵ
 *
 * ��ͬ�ڳ�ʼ������ӵ���ҵ���к󣬲Ż�ִ����ز�����
 *
 * \param[in]  p_job   ��ҵ����
 * \param[in]  func    Ҫִ�еĺ���
 * \param[in]  p_arg   Ҫִ�еĺ�������
 * \param[in]  pri     ��ҵ�����ȼ�
 * \param[in]  flags   ��ҵ�ı��
 *
 * \return ��
 */
void aw_job_queue_fill_job (aw_qjob_t      *p_job,
                            aw_pfuncvoid_t  func,
                            void           *p_arg,
                            uint16_t        pri,
                            uint16_t        flags);

/**
 * \brief ɾ����ҵ����
 *
 * ע����ɾ������ҵ���к󣬲�Ҫ�ٴ����ø� ID�� 
 *
 * \param[in]  job_queue_id   ��ҵ���� ID
 *
 * \return ��
 */
void aw_job_queue_delete (aw_job_queue_id_t job_queue_id);

/**
 * \brief ����ҵ���뵽��ҵ����
 *
 * ������ɹ������޸���ҵ�ı�ǣ�������Ѽ���ȴ����У�
 * ��������ҵ���еĻ���ȼ�λͼ��
 *
 * \param[in] job_queue_id ��ҵ���� ID
 * \param[in] p_job        Ҫ�������ҵ
 *
 * \retval  AW_OK      �����ɹ�
 * \retval -AW_EINVAL  ������Ч
 * \retval -AW_EBUSY   ��ҵ����æ
 *
 * \sa aw_job_queue_std_post()
 */
aw_err_t aw_job_queue_post (aw_job_queue_id_t job_queue_id, aw_qjob_t *p_job);


/**
 * \brief �����׼��ҵ���뵽��ҵ����
 *
 * ������ɹ����Ὣ�����ҵ�ӿ�������ŵ��ȴ����У�
 * ��������ҵ���е����ȼ�λͼ���û��ȿ���ͨ�� aw_job_queue_post()
 * �����û��ṩ����ҵ�ռ䣬Ҳ���Դ� aw_job_queue_std_post() �����׼��ҵ��
 * ����ҵ�ռ䣬ִ����Ӧ������
 *
 * \param[in] job_queue_id  ��ҵ���� ID
 * \param[in] priority      ���ȼ�
 * \param[in] func          ִ�еĺ���
 * \param[in] arg1          ����1
 * \param[in] arg2          ����2
 * \param[in] arg3          ����3
 * \param[in] arg4          ����4
 *
 * \retval  AW_OK      ����ɹ�
 * \retval -AW_EINVAL  ������Ч
 * \retval -AW_EBUSY   ��ҵ����æ
 * \retval -AW_ENOENT  û�п�����ҵ�������
 */
aw_err_t aw_job_queue_std_post (aw_job_queue_id_t job_queue_id,
                                int               priority,
                                aw_pfuncvoid_t    func,
                                void             *arg1,
                                void             *arg2,
                                void             *arg3,
                                void             *arg4);

/**
 * \brief ��ҵ���д�������
 *
 * ������ҵ���������ҵ��һ�����һ���������߳�ִ��������̡�
 *
 * \param[in]  job_queue_id   ��ҵ���� ID
 *
 * \return ������ǰ��ҵ���������У��ͷ��� -AW_EINVAL��
 *         ��һ�о������򲻻�Ӹú������أ�һֱִ�С�
 */
aw_err_t aw_job_queue_process (aw_job_queue_id_t job_queue_id);


/**
 * \brief �ӱ�׼��ҵ�ش���һ����ҵ����
 *
 * ���ɹ�����ʱ���᷵����ҵ���� ID��
 *
 * \param[in]  std_pool_id  ��ҵ���еı�׼��ҵ�� 
 *
 * \return ���ɹ���������ҵ����ID����ʧ�ܣ�����NULL��
 */
aw_job_queue_id_t aw_job_queue_create (aw_qjob_std_pool_id_t std_pool_id);


/**
 * \brief ��ʼ����ҵ����
 *
 * ��ʼ���ɹ����Ὣ \e std_pool_id �ĳ�Ա \e ref_count ��1��
 * ������� \e std_pool_id ΪNULL������ҵ����û�б�׼��ҵ�أ�
 * ������ҵ�Ŀռ����û��ṩ��
 *
 * \param[in,out]  p_jobq       Ҫ��ʼ������ҵ���� 
 * \param[in]      std_pool_id  ��׼��ҵ���г� ID
 *
 * \retval  AW_OK      ��ʼ���ɹ�
 * \retval -AW_EINVAL  ������Ч
 */
aw_err_t aw_job_queue_init (aw_job_queue_t        *p_jobq,
                            aw_qjob_std_pool_id_t  std_pool_id);


/**
 * \brief �����µ���ҵ����׼��ҵ�صĿ�������
 *
 * \param[in,out] std_pool_id  ��׼��ҵ�� ID 
 * \param[in]     njobs        ��ҵ����
 *
 * \retval  AW_OK       ����ɹ�
 * \retval -AW_EINVAL   ������Ч
 * \retval -AW_ENOMEM   �ڴ治��
 */
aw_err_t aw_job_queue_std_jobs_alloc (aw_qjob_std_pool_id_t std_pool_id,
                                      size_t                njobs);

/**
 * \brief ��ʼ����׼��ҵ��
 *
 * \param[in,out] std_pool_id  ��׼��ҵ�� ID 
 *
 * \retval  AW_OK  ��ʼ���ɹ�
 */
aw_err_t aw_job_queue_std_pool_init (aw_qjob_std_pool_id_t std_pool_id);


/** @} grp_aw_if_job_queue */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_JOB_QUEUE_H */

/* end of file */
