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
 * \file  rtk_semb.h
 * \brief ����rtk�������ź�����ؽṹ�ͽӿں���
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_SEMB_H__
#define         __RTK_SEMB_H__

/**
 *  @defgroup SEMAPHORE_BINARY_API semaphore binary API
 *  @{
 */

struct rtk_semb
{
    rtk_wait_queue_t        wait_queue;
    unsigned int            count;
    unsigned long           magic_num;

};

/**
 * \brief ��ʼ��һ����ֵ�ź���
 *
 * �������ж��е���
 *
 * \param[in] p_semb        struct rtk_semaphore_binary�ṹ
 * \param[in] InitCount     ��ֵ�ź�����ʼ����ֵ��0��ʾ���źţ�����ֵ���ź�
 *
 * \retval    ʧ�ܷ���NULL���ɹ��򷵻�ִ��rtk_semaphore_binary�ṹ��ָ��
 */
struct rtk_semb * rtk_semb_init(
        struct rtk_semb *p_semb,
        int InitCount );

/**
 * \brief ����ʼ��һ���������ź���
 *
 * �������ж��е��ã��κ�һ���ɹ���ʼ�����ź��������Ե���rtk_semb_terminate
 * ������ʼ��������ʼ���ỽ�����Եȴ��ڴ��ź����ϵ�������Щ�����take������
 * �õ�����ֵ-AW_ENXIO
 *
 * \param[in] p_semb        struct rtk_semaphore_counter�ṹ
 * \retval    0             �ɹ�����0�����򷵻ظ��Ĵ�����
 * \retval    -AW_EPERM     û��Ȩ�޽��д˲������������ж��в���
 */
int rtk_semb_terminate( struct rtk_semb *p_semb );

/**
 * \brief ��������һ���������ź�����ֵ
 *
 * �������ж��е���
 *
 * \param[in] p_semc        struct rtk_semaphore_counter�ṹ
 * \param[in] reset_value   �µĶ������ź�����ֵ��0��ʾ���ź�
 *                              ��0��ʾ���ź�
 *                              ���ź�������Ϊ���źŻᵼ��һ��pending���񱻻���
 * \retval    0             �ɹ�����0�����򷵻ظ��Ĵ�����
 */
int rtk_semb_reset( struct rtk_semb*p_semb, uint32_t reset_value );


/**
 * \brief ��ȡһ���������ź���
 *
 * �����ж��е��ã���Ϊ�൱��tick == 0
 *
 * \param[in] p_semb        struct rtk_semaphore_binary�ṹ
 * \param[in] tick          ��system tick���������ȴ�ʱ��
 *                          0��ʾ�����Ƿ��ȡ�ɹ���������������
 *                          AW_WAIT_FOREVER����ʾ�����õȴ�
 * \retval  0               �ɹ�����0�����򷵻ظ��Ĵ�����
 * \retval  -AW_EINVAL      ������Ч
 * \retval  -AW_ETIME       ��ȡ��ʱ
 * \retval  -AW_ENXIO       �ź���������������ֹ��
 * \retval  -AW_EAGAIN      ��Ҫ���ԣ�ֻ��tick==0���Ҽ����ź��������òŻ᷵��
 */
int rtk_semb_take( struct rtk_semb *p_semb, unsigned int tick );

/**
 * \brief �ͷ�һ���������ź���
 *
 * �����ж��е���
 *
 * \param[in] p_semb        struct rtk_semaphore_binary�ṹ
 * \retval  0               �ɹ�����0�����򷵻ظ��Ĵ�����
 * \retval  -AW_EINVAL      ������Ч
 */
int rtk_semb_give( struct rtk_semb *p_semb );

/**
 *  @}
 */

#endif          /*__RTK_SEMC_H__*/
