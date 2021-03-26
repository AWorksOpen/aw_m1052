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
 * \brief RTK ʱ������
 *
 * RTK ʱ���������һ���������е�64λʱ���
 * ����Ƶ����4GHZ����Ҳ���Ա�֤145�겻�����
 * ���ڸ�Ϊͳ��RTK���������ʱ��
 *  *
 * //�������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.00 13-02-27  zen,   first implementation
 * \endinternal
 */

#ifndef __RTK_TIMESTAMP_H
#define __RTK_TIMESTAMP_H

#include "aw_timestamp.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/


/** \brief ����ʱ���Ƶ�ʵ����� */
typedef aw_timestamp_freq_t rtk_timestamp_freq_t;
typedef aw_timestamp_t      rtk_timestamp_t;

/**
 * \brief ��ȡ��ǰʱ��ʱ���
 *
 * \return ϵͳ��ǰʱ���ֵ(ʱ�����ʱ���ļ���ֵ)
 */
#define rtk_timestamp_get aw_timestamp_get


/**
 * \brief ��ȡʱ�����ʱ��Ƶ��
 *
 * \return ʱ�����ʱ��Ƶ��, Hz
 */
static inline aw_timestamp_freq_t rtk_timestamp_freq_get(void)
{
    return aw_timestamp_freq_get();
}



/** @} grp_aw_if_system */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif    /* __RTK_TIMESTAMP_H    */

/* end of file */
