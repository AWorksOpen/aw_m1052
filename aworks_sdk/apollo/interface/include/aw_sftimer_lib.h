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
 * \brief  �����ʱ����
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_sftimer_lib.h
 *
 * ʹ�������ʱ���⣬��Ҫ�ⲿ�ṩ�����Եķ������̣������� aw_sftimer_group_tick()
 * ���䵱�����ʱ����ġ�ʱ�ӡ���
 * 
 * ��ʱ����ʹ�á�Ͱ������Ŷ�ʱ��������ʱ������Ҫ��������Ķ�ʱ��ʱ��
 * ��������Ͱ���Ĵ�С�����ڽ�����ĳ�����ڷ�������ִ�д��������Ŀ����ԡ�
 *
 * ����ʹ��Ӳ����ʱ���ṩ�����ʱ����ġ�ʱ�ӡ���
 *
 * \par ��ʼ����ʱ����
 * \code
 * #define  BUCKET_SIZE 6      // Ͱ�ĸ���
 *
 * // ��ʱ����ġ�ʱ�ӡ�Ƶ�ʣ����������Ӳ����ʱ���жϷ������̵�Ƶ��
 * #define  CLKRATE     1000   
 *
 * static struct aw_sftimer_group  my_sftimer_group;
 * static struct aw_sftimer_bucket my_bucket[BUCKET_SIZE];
 * 
 * static void sftimer_group_init(void)
 * {
 *     aw_sftimer_group_init(&my_sftimer_group,
 *                           &my_bucket[0],
 *                            BUCKET_SIZE,
 *                            CLKRATE);
 * }
 * \endcode 
 *
 * \par ����Ӳ����ʱ���жϷ�������
 * \code
 * static aw_hwtimer_handle_t my_hwtimer;
 * 
 * static void hwtimer_isr (void *p_arg)
 * {
 *     aw_sftimer_group_tick(&my_sftimer_group);
 * }
 *
 * void hwtimer_init(void)
 * {
 *     mytimer = aw_hwtimer_alloc(10000,                   
 *                                1,                      
 *                                1000000,                
 *                                AW_HWTIMER_AUTO_RELOAD, 
 *                                hwtimer_isr,            
 *                               (void *)0);          
 *
 *
 *     // Ӳ����ʱ���жϷ������̵�ִ��Ƶ��
 *     aw_hwtimer_enable(my_hwtimer, CLKRATE);
 * }
 * \endcode
 *
 * \par ���������ʱ��
 * \code
 * #define SFTIMER_MS  1000  // ��ʱ���Ķ�ʱʱ�䣬����
 *
 * static struct aw_sftimer  my_sftimer;
 *
 * static void sftimer_handler(void *arg)
 * {
 *     // ִ�ж�ʱ����
 *
 *     // ������ʱ��
 *     aw_sftimer_restart(&my_sftimer);
 * }
 * 
 * static void sftimer_init(void)
 * {
 *     // ��ʼ����ʱ��
 *     aw_sftimer_init(&my_sftimer,
 *                     &my_sftimer_group,
 *                      sftimer_handler��
 *                     (void *)0);
 * 
 *     // ������ʱ��
 *     aw_sftimer_start_ms(&my_sftimer, 
 *                          SFTIMER_MS);
 * }
 * \endcode 
 *
 * aw_sftimer_start() ʹ�õĵδ���������ʼ����ʱ����ʱ����� \e clkrate �йأ�
 * ���Բ��ܵ��� aw_ms_to_ticks() ����ȡ�δ�����Ҫ����ת����
 * �Ƽ�ʹ�� aw_sftimer_start_ms() ���ڲ������н�ʱ��ת���ɵδ�����
 *
 * \internal
 * \par modification history:
 * - 1.10 13-03-21  zen, refine the description
 * - 1.01 13-02-22  orz, fix the defines and code style.
 * - 1.00 12-10-23  liangyaozhan, first implementation.
 * \endinternal
 */

#ifndef __AW_SFTIMER_LIB_H
#define __AW_SFTIMER_LIB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_list.h"

/**
 * \addtogroup grp_aw_if_sftimer_lib 
 * \copydoc aw_sftimer_lib.h
 * @{
 */

/** \brief ��ʱ��Ͱ�Ķ��� */
struct aw_sftimer_bucket {

    /** \brief �ṩ�����ʱ��������� */
    struct aw_list_head list_head;
};

/** \brief ��ʱ����Ķ��� */
struct aw_sftimer_group {

    /** \brief ��ʱ�����Ͱ */
    struct aw_sftimer_bucket *p_buckets;

    /** \brief Ͱ����������ɢ����ʱ�����ͳ䵱��ʱ���� */
    size_t                    bucket_cnt;

    /** \brief ��ǰ��Ͱ */
    unsigned int              cur_bucket;

    /** \brief ��ʱ�ӡ�Ƶ�ʣ����ⲿ�ṩ��ʱ�����Ƶ�� */
    unsigned int              clkrate;
};


/**
 * \brief ��ʱ���Ķ���
 *
 * �û���Ӧ��ֱ�ӷ�����Щ��Ա��
 */
struct aw_sftimer {

    /** \brief ����ڵ� */
    struct aw_list_head listnode; 

    /** \brief Ͱ���� */  
    uint32_t            bucket;     

    /** \brief ʣ�������������ʱ���鶨ʱ */
    uint32_t            round;      

    /** \brief ����ʱ�䣬����������ʱ�� */
    uint32_t            expires;    

    /** 
     * \brief ��ʱ��ִ�еĺ��� 
     * 
     * \param[in] arg �û�����
     */
    void (*func)(void *arg);        

    /** \brief ��ʱ��ִ�к����Ĳ���  */
    void  *arg;                    

    /** \brief ������ʱ���� */
    struct aw_sftimer_group *p_timer_grp;
};

/** 
 * \brief ��ʱ�������ڴ�������
 *
 * һ������ⲿ�ṩ�����ڷ�����ִ�С�
 * ִ�е�Ƶ�ʵ�ͬ����ʱ�����ʱ��Ƶ�ʡ�
 * 
 * \param[in] p_timer_grp      ��Ҫ�������ʱ����
 *
 * \sa aw_sftimer_group_init()
 */
void aw_sftimer_group_tick (struct aw_sftimer_group *p_timer_grp);


/** 
 * \brief ��ʼ����ʱ����
 *
 * \e clkrate �� aw_sftimer_group_tick() ��ִ�е�Ƶ�ʡ�
 * 
 * \param[in] p_timer_grp      ��ʱ����
 * \param[in] p_buckets        Ͱ����ʼ��ַ
 * \param[in] bucket_cnt       Ͱ�ĸ���
 * \param[in] clkrate          ��ʱ����Ķ�ʱƵ��
 *
 * \retval AW_OK              ��ʼ���ɹ�
 * \retval -AW_EINVAL         ������Ч
 */
aw_err_t aw_sftimer_group_init (struct aw_sftimer_group  *p_timer_grp,
                                struct aw_sftimer_bucket *p_buckets,
                                size_t                    bucket_cnt,
                                unsigned int              clkrate);

/** 
 * \brief ��ʼ����ʱ��  
 * 
 * \param[in] p_timer      ��ʱ��
 * \param[in] p_timer_grp  ��ʱ����
 * \param[in] func         ��ʱִ�еĺ���
 * \param[in] arg          ��ʱִ�еĺ�������
 *
 * \retval AW_OK          ��ʼ���ɹ�
 * \retval -AW_EINVAL     ������Ч
 */
aw_err_t aw_sftimer_init (struct aw_sftimer *p_timer,
                          struct aw_sftimer_group *p_timer_grp,
                          void     (*func) (void *arg),
                          void      *arg);

/** 
 * \brief ������ʱ�� 
 * 
 * \param[in] p_timer ��ʱ��
 * \param[in] ticks   ��ʱʱ��(�δ���)
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_EINVAL  ������Ч
 */
aw_err_t aw_sftimer_start (struct aw_sftimer *p_timer,
                           unsigned int       ticks);


/** 
 * \brief ��ʱִ�еĺ��� 
 * 
 * \param[in] p_timer   ��ʱ��
 * \param[in] ms        ��ʱʱ��(����)
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_EINVAL  ������Ч
 */
aw_err_t aw_sftimer_start_ms (struct aw_sftimer *p_timer,
                              unsigned int       ms);


/** 
 * \brief ������ʱ��
 * 
 * \param[in] p_timer   ��ʱ��
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_EINVAL  ������Ч
 */
aw_err_t aw_sftimer_restart (struct aw_sftimer *p_timer);


/** 
 * \brief ֹͣ��ʱ��
 * 
 * \param[in] p_timer   ��ʱ��
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_EINVAL  ������Ч
 */
aw_err_t aw_sftimer_stop (struct aw_sftimer *p_timer);


/** @} grp_aw_if_sftimer_lib */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_SFTIMER_LIB_H */

/* end of file */
