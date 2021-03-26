/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      AWorks.support@zlg.cn
*******************************************************************************/
/**
 * \file  rtk_atomic.h
 * \brief ԭ�Ӳ����ӿ�ͷ�ļ�
 *
 * \internal
 * \par modification history:
 * - 2019-10-23  wk, first implementation.
 * \endinternal
 */
#ifndef __RTK_ATOMIC_H
#define __RTK_ATOMIC_H


#ifdef __cplusplus
extern "C" {
#endif


/** \brief ԭ�����ͱ���v����ԭ�Ӷ����� */
#define rtk_atomic_get(v)  (*(volatile int *)&(v)->counter)

/** \brief ����ԭ�����ͱ���v��ֵΪi */
#define rtk_atomic_set(v,i) (((v)->counter) = (i))

/** \brief ԭ�����ͱ���v��1,�����ؽ�� */
#define rtk_atomic_inc_return(v)    (rtk_atomic_add_return(1, v))

/** \brief ԭ�����ͱ���v��1�������ؽ�� */
#define rtk_atomic_dec_return(v)    (rtk_atomic_sub_return(1, v))

/** \brief ԭ�����ͱ���v��1������������0���򷵻��棬���򷵻ؼ� */
#define rtk_atomic_inc_and_test(v)  (rtk_atomic_add_return(1, v) == 0)

/** \brief ԭ�����ͱ���v��1������������0���򷵻��棬���򷵻ؼ� */
#define rtk_atomic_dec_and_test(v)  (rtk_atomic_sub_return(1, v) == 0)

/** \brief ԭ�����ͱ���v��i������������0���򷵻��棬���򷵻ؼ� */
#define rtk_atomic_sub_and_test(i, v) (rtk_atomic_sub_return(i, v) == 0)

/** \brief ԭ�����ͱ���v��i��������Ϊ�������򷵻��棬���򷵻ؼ� */
#define rtk_atomic_add_negative(i,v) (rtk_atomic_add_return(i, v) < 0)


/**
 * \brief ����ԭ�ӱ�������
 */
typedef struct {

    volatile int counter;

} atomic_t ;

/**
 * \brief ԭ�����ͱ������мӷ�����,�����ؽ��
 *
 * \param[in] add_val ����
 * \param[in] p_val   ָ��ԭ�ӱ�����ָ��
 *
 * \return    ԭ�ӱ�����add_val֮��ļ�����
 *
 */
int rtk_atomic_add_return(int add_val, atomic_t *p_val);

/**
 * \brief ԭ�����ͱ������м������㣬�����ؽ��
 *
 * \param[in] sub_val ����
 * \param[in] p_val   ָ��ԭ�ӱ�����ָ��
 *
 * \return    ԭ�ӱ�����add_val֮��ļ�����
 *
 */
int rtk_atomic_sub_return(int sub_val, atomic_t *p_val);


/**
 * \brief ԭ�ӱ�����ɵ���ֵ�Ƚ��бȽϣ�����ɵ���ֵ�����ʱ��
 *        ���µ���ֵ��ֵ��ԭ�ӱ���
 *
 * \param[in] old_val �ɵ���ֵ
 * \param[in] new_val �µ���ֵ
 * \param[in] p_val   ָ��ԭ�ӱ�����ָ��
 *
 * \return    �ɵ�ԭ�ӱ�����ֵ
 *
 */
int rtk_atomic_cmpxchg(int old_val, int new_val, atomic_t *p_val);


#ifdef __cplusplus
}
#endif

#endif /* __ATOMIC_H */
