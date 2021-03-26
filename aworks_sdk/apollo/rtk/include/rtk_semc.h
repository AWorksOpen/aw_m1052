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
 * \file  rtk_semc.h
 * \brief ����rtk�����ź�����ؽṹ�ͽӿں���
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_SEMC_H__
#define         __RTK_SEMC_H__

/**
 *  @defgroup SEMAPHORE_COUNTER_API semaphore counter API
 *  @{
 */


struct rtk_semc
{
    rtk_wait_queue_t        wait_queue;
    unsigned int            count;
    unsigned long           magic_num;
};


/**
 * \brief semaphore counter declaration macro.
 */
#define RTK_SEMC_DECL(name) struct rtk_semc name

/**
 * \brief ��ʼ��һ�������ź���
 *
 * �������ж��е���
 *
 * \param[in] p_semc        struct rtk_semaphore_counter�ṹ
 * \param[in] InitCount     �����ź�����ʼ����ֵ
 * \retval    ʧ�ܷ���NULL���ɹ��򷵻�ִ��rtk_semaphore_counter�ṹ��ָ��
 */
struct rtk_semc * rtk_semc_init(
        struct rtk_semc *p_semc,
        unsigned int InitCount );

/**
 * \brief ����ʼ��һ�������ź���
 *
 * �������ж��е��ã��κ�һ���ɹ���ʼ�����ź��������Ե���rtk_semc_terminate
 * ������ʼ��������ʼ���ỽ�����Եȴ��ڴ��ź����ϵ�������Щ�����take������
 * �õ�����ֵ-AW_ENXIO
 *
 * \param[in] p_semc        struct rtk_semaphore_counter�ṹ
 * \retval    0             �ɹ�����0�����򷵻ظ��Ĵ�����
 */
int rtk_semc_terminate( struct rtk_semc *p_semc );

/**
 * \brief ��������һ�������ź�����ֵ
 *
 * �������ж��е��ã�Ҫ��ɹ����ã����p_semc�ϲ������κ������ŵ�take����
 *
 * \param[in] p_semc        struct rtk_semaphore_counter�ṹ
 * \param[in] reset_value   �µļ����ź�����ֵ
 * \retval    0             �ɹ�����0�����򷵻ظ��Ĵ�����
 */
int rtk_semc_reset( struct rtk_semc *p_semc, unsigned int reset_value );

/**
 * \brief ��ȡһ�������ź���
 *
 * �����ж��е��ã���Ϊ�൱��tick == 0
 *
 * \param[in] p_semc        struct rtk_semaphore_counter�ṹ
 * \param[in] tick          ��system tick���������ȴ�ʱ��
 *                          0��ʾ�����Ƿ��ȡ�ɹ���������������
 *                          AW_WAIT_FOREVER����ʾ�����õȴ�
 * \retval  0               �ɹ�����0�����򷵻ظ��Ĵ�����
 * \retval  -AW_EINVAL      ������Ч
 * \retval  -AW_ETIME       ��ȡ��ʱ
 * \retval  -AW_ENXIO       �ź���������������ֹ��
 * \retval  -AW_EAGAIN      ��Ҫ���ԣ�ֻ��tick==0���Ҽ����ź��������òŻ᷵��
 */
int rtk_semc_take( struct rtk_semc *p_semc, unsigned int tick );

/**
 * \brief �ͷ�һ�������ź���
 *
 * �����ж��е���
 *
 * \param[in] p_semc        struct rtk_semaphore_counter�ṹ
 * \retval  0               �ɹ�����0�����򷵻ظ��Ĵ�����
 * \retval  -AW_EINVAL      ������Ч
 * \retval  -AW_ENOSPC      semcû�пռ��ˣ�����ζ��semc�ļ���ֵ�Ѿ��ﵽ���
 */
int rtk_semc_give( struct rtk_semc *p_semc );

/**
 *  @}
 */

#endif          //__RTK_SEMC_H__
