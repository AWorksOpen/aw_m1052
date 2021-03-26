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
 * \brief ʱ�����ط���
 *
 * ��Ҫ������ aw_timestamp.h �Ϳ���ʹ�ñ�����
 *
 * \par ��ʾ��:�������ִ��ʱ��,΢�뼶���������뼶���ʱ�����
 * \code
 *
 * #include "aw_timestamp.h"
 * 
 * int aw_main()
 * {
 *     aw_timestamp_t          stamps;
 *     aw_timestamp_freq_t     freq;
 *     uint32_t                ns,us;
 * 
 *     stamps = aw_timestamp_get();
 *     //  do something
 *     stamps = aw_timestamp_get() - stamps;
 * 
 *     us = aw_timestamps_to_us(stamps);
 *     ns = aw_timestamps_to_ns(stamps);
 * 
 *     aw_kprintf("stamps used=%d, time=%d us,%d ns",
 *                 (uint32_t)stamps,
 *                 us,
 *                 ns);
 *     return 0;
 * }
 * \endcode
 *
 * //�������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.02 19-05-16  sni,   ����ӿڣ�ʹ�ø�����
 * - 1.01 14-08-18  tee,   modified and tested.
 * - 1.00 13-02-27  zen,   first implementation
 * \endinternal
 */

#ifndef __AW_TIMESTAMP_H
#define __AW_TIMESTAMP_H

#include "aw_types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_timestamp
 * \copydoc aw_timestamp.h
 * @{
 */

/** \brief ����ʱ���Ƶ�ʵ����� */
typedef uint32_t    aw_timestamp_freq_t;

/** \brief ����ʱ������� */
typedef uint64_t    aw_timestamp_t;

/**
 * \brief ��ȡ��ǰʱ��ʱ���
 *
 * �˺����������ж��е��ã������ǰϵͳ��Ϊȱ����ʱ������֧��
 * ��ȡʱ�������᷵��0
 *
 * \return ϵͳ��ǰʱ���ֵ(ʱ�����ʱ���ļ���ֵ)
 */
aw_timestamp_t aw_timestamp_get(void);


/**
 * \brief ��ȡϵͳʱ�����ʱ���ļ���Ƶ��
 *
 * ��֧��Ƶ��Ϊ1Hz������������ϵͳ�в�����ʱ�����ʱ����
 * ��᷵��1
 * 
 * ��isr�е��õ���Ϊ�ǲ�ȷ����
 *
 * \return ʱ�����ʱ��Ƶ��, Hz
 */
aw_timestamp_freq_t aw_timestamp_freq_get(void);

/**
 * \brief  ��ʱ���ת��Ϊʱ��(us)
 *
 * �˺������������룬����ֵ��ע��ĵط�
 * �������ʵ�ʶ�Ӧ����1.499999us,��ת�����Ϊ1 us
 * ��ʵ�ʶ�Ӧ1.5000000us�Ļ�����ת�����Ϊ2us
 * ����������벻������������Ϊ������Ը���Ƶ�����м���
 *
 * ��ISR�е���Ҳ����ӳ��жϴ���ʱ��
 *
 * \param  stamps ʱ�������
 * \return ����ʱ���������ת���õ���ʱ��us��
 */
uint32_t aw_timestamps_to_us(aw_timestamp_t stamps);

/**
 * \brief  ��ʱ���ת��Ϊʱ��(ns)
 *
 * �˺������������룬����ֵ��ע��ĵط�
 * �������ʵ�ʶ�Ӧ����1.499999ns,��ת�����Ϊ1 ns
 * ��ʵ�ʶ�Ӧ1.5000000ns�Ļ�����ת�����Ϊ2ns
 * ����������벻������������Ϊ������Ը���Ƶ�����м���
 *
 * ��ISR�е���Ҳ����ӳ��жϴ���ʱ��
 *
 * \param  stamps ʱ�������
 * \return ����ʱ���������ת���õ���ʱ��ns��
 */
uint32_t aw_timestamps_to_ns(aw_timestamp_t stamps);

/** @} grp_aw_if_system */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif    /* __AW_TIMESTAMP_H    */

/* end of file */
