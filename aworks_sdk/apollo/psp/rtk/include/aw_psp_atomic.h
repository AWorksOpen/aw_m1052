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
 * \brief task ƽ̨��ر�׼����ͷ�ļ�
 *
 * \internal
 * \par modification history:
 * - 1.00 19-10-23  zen, refine the description
 * \endinternal
 */

#ifndef __AW_PSP_ATOMIC_H
#define __AW_PSP_ATOMIC_H

#include "rtk.h"                                /* Ŀǰʹ��RTK�ں� */
#include "rtk_atomic.h"
#include "aw_compiler.h"
#include "aw_types.h"


/** \brief ԭ�����ͱ���v����ԭ�Ӷ����� */
#define aw_psp_atomic_get(v) rtk_atomic_get(v)

/** \brief ����ԭ�����ͱ���v��ֵΪi */
#define aw_psp_atomic_set(v,i) rtk_atomic_set(v,i) 

/** \brief ԭ�����ͱ���v��1,�����ؽ�� */
#define aw_psp_atomic_inc_return(v) rtk_atomic_inc_return(v) 
 
/** \brief ԭ�����ͱ���v��1�������ؽ�� */
#define aw_psp_atomic_dec_return(v) rtk_atomic_dec_return(v) 

/** \brief ԭ�����ͱ���v��1������������0���򷵻��棬���򷵻ؼ� */
#define aw_psp_atomic_inc_and_test(v) rtk_atomic_inc_and_test(v) 

/** \brief ԭ�����ͱ���v��1������������0���򷵻��棬���򷵻ؼ� */
#define aw_psp_atomic_dec_and_test(v) rtk_atomic_dec_and_test(v) 

/** \brief ԭ�����ͱ���v��i������������0���򷵻��棬���򷵻ؼ� */
#define aw_psp_atomic_sub_and_test(i, v) rtk_atomic_sub_and_test(i, v) 

/** \brief ԭ�����ͱ���v��i��������Ϊ�������򷵻��棬���򷵻ؼ� */
#define aw_psp_atomic_add_negative(i,v) rtk_atomic_add_negative(i,v) 

/**
 * \brief ԭ�����ͱ������мӷ�����,�����ؽ��
 *
 * \param[in] add_val ����
 * \param[in] p_val   ָ��ԭ�ӱ�����ָ��
 *
 * \return    ԭ�ӱ�����add_val֮��ļ�����
 *
 */
static aw_inline int aw_psp_atomic_add_return(int add_val, atomic_t *p_val)
{
    return rtk_atomic_add_return(add_val, &p_val->counter);
}

/**
 * \brief ԭ�����ͱ������м������㣬�����ؽ��
 *
 * \param[in] sub_val ����
 * \param[in] p_val   ָ��ԭ�ӱ�����ָ��
 *
 * \return    ԭ�ӱ�����add_val֮��ļ�����
 *
 */
static aw_inline int aw_psp_atomic_sub_return(int sub_val, atomic_t *p_val)
{
    return rtk_atomic_sub_return(sub_val, &p_val->counter);
}


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
static aw_inline int aw_psp_atomic_cmpxchg(int old_val, int new_val, atomic_t *p_val)
{
    return rtk_atomic_cmpxchg(old_val, new_val, &p_val->counter);
}

#endif