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

#ifndef __AW_ATOMIC_H
#define __AW_ATOMIC_H

#include "aw_psp_atomic.h"


/** \brief ԭ�����ͱ���v����ԭ�Ӷ����� */
#define aw_atomic_get(v) aw_psp_atomic_get(v)

/** \brief ����ԭ�����ͱ���v��ֵΪi */
#define aw_atomic_set(v,i) aw_psp_atomic_set(v,i) 

/** \brief ԭ�����ͱ���v��1,�����ؽ�� */
#define aw_atomic_inc_return(v) aw_psp_atomic_inc_return(v) 
 
/** \brief ԭ�����ͱ���v��1�������ؽ�� */
#define aw_atomic_dec_return(v) aw_psp_atomic_dec_return(v) 

/** \brief ԭ�����ͱ���v��1,�����ؽ�� */
#define aw_atomic_add_return(i, v) aw_psp_atomic_add_return(i, v)

/** \brief ԭ�����ͱ���v��1�������ؽ�� */
#define aw_atomic_sub_return(i, v) aw_psp_atomic_sub_return(i, v)

/** \brief ԭ�����ͱ���v��1������������0���򷵻��棬���򷵻ؼ� */
#define aw_atomic_inc_and_test(v) aw_psp_atomic_inc_and_test(v) 

/** \brief ԭ�����ͱ���v��1������������0���򷵻��棬���򷵻ؼ� */
#define aw_atomic_dec_and_test(v) aw_psp_atomic_dec_and_test(v) 

/** \brief ԭ�����ͱ���v��i������������0���򷵻��棬���򷵻ؼ� */
#define aw_atomic_sub_and_test(i, v) aw_psp_atomic_sub_and_test(i, v) 

/** \brief ԭ�����ͱ���v��i��������Ϊ�������򷵻��棬���򷵻ؼ� */
#define aw_atomic_add_negative(i,v) aw_psp_atomic_add_negative(i,v) 

/** \brief ԭ�ӱ���v��x�Ƚ��бȽϣ���x�����ʱ�� ��y��ֵ��ԭ�ӱ��� */
#define aw_atomic_cmpxchg(x, y, v) return aw_psp_atomic_cmpxchg(x, y, v);

#endif