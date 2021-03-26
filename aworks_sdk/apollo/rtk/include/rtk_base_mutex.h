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
 * \file  rtk_base_mutex.c
 * \brief ����rtk_base_mutex���ڲ���������
 *
 * rtk_base_mutex��һ��rtk�ڲ�ʹ�õ�����������ĳЩ������滻��ֱ�ӽ����жϵ�
 * ȫ����������Ҫ���������ں��е�һЩ���ݽṹ���������߼����ǽ����жϵ�ȫ����
 * rtk_base_mutex���õ����Ƚ��ȳ��Ĳ��ԣ����ֲ���ʹ����lock��unlock������ʱ�䶼��
 * O(1)��
 *
 * ֮���Բ���FIFO���ԣ���Ҫ����Ϊ�����ʱ��ΪO(1)
 * ֻ�����ڲ�������Ӱ�������������ں˽ṹ�ϵ��������ȼ��ߵ������ȱ�����
 *
 * ������rtk_base_mutex������rtk_mutex�������ȼ�������������ȡrtk_base_mutex
 * �����rtk_mutex�ڼ䣬�����ȼ������������rtk_base_mutex��(ԭ�ȵķ�����ֱ��
 * �����жϣ���ʱ�������ᷢ�����ȣ�ʹ��rtk_base_mutex�����������ж��ӳ�ʱ��)
 *
 * �����ȼ���������󣬽��Լ���ӵ�rtk_mutex�ĵȴ������У�Ȼ������ȼ�����
 * �����ѣ�����rtk_mutex���Լ�Ҳ���뵽rtk_mutex�ȴ������У�
 * ���ǿ϶����ڵ����ȼ�����֮ǰ���Ӷ��Ǹ����ȼ������ȱ�����
 *
 * �����������У�����ڸ����ȼ������ȡrtk_base_mutex֮�󣬽��Լ���ӵ�rtk_mutex
 * �ȴ�����֮ǰ����Ϊ�����жϣ��������ȼ����������ִ�ж���rtk_mutex��ÿ��ã�
 * ��ʱ�������ȼ�������Ҫ�Ȼ�ȡrtk_base_mutex������ȴ�ֱ�������ȼ�����������,
 * ��Ϊ��rtk_mutex�ȴ������и����ȼ��������и���ǰ��������Ȼ���ȱ�����
 * ��Ҳ��Ӱ������ȼ�������ִ�еĻ���
 *
 * \internal
 * \par modification history:
 * - 2018-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_BASE_MUTEX_H__
#define         __RTK_BASE_MUTEX_H__
/**
 *  @brief rtk_base_mutex
 */
typedef struct
{
    struct rtk_task    *owner;
    /** \brief ���������б�����л���������Ľڵ� */
    rtk_list_node_t     base_mutex_hold_node;   
    /** \brief ���������ݹ���еĴ��� */
    int                 mutex_recurse_count;    
}rtk_base_mutex_t;

/**
 * \brief ��ʼ��һ��rtk_base_mutex
 *        �˺��������û��ӿں�����������ע�ⲻҪ�ظ���ʼ��
 *
 * \param[in]   p_base_mutex        rtk_base_mutex����
 *
 * \return   ��
 */
void rtk_base_mutex_init(rtk_base_mutex_t *p_base_mutex);

/**
 * \brief ����һ��rtk_base_mutex
 *        ���rtk_base_mutex���������ռ�ݣ���ǰ��������ȴ�
 * \param[in]   p_base_mutex        rtk_base_mutex����
 * \return   ��
 */
void rtk_base_mutex_lock(rtk_base_mutex_t *p_base_mutex);

/**
 * \brief ����һ��rtk_base_mutex
 *
 * \param[in]   p_base_mutex        rtk_base_mutex����
 * \return   ��
 */
void rtk_base_mutex_unlock(rtk_base_mutex_t *p_base_mutex);

/**
 * \brief ����һ��rtk_base_mutex,���ӵĹ����ǲ������������
 *
 * \param[in]   p_base_mutex        rtk_base_mutex����
 * \retval      0                   �Ժ���Ҫִ���������
 * \retval      1                   �Ժ���Ҫִ���������
 */
int rtk_base_mutex_unlock_no_sche(rtk_base_mutex_t *p_base_mutex);

/**
 * \brief ����һ��rtk_base_mutex,���ӵĹ����ǲ������������
 * ���Ҵ˺������ܹ���ӵ���������ɱ�Ϊ�������Ժ����
 * �����rtk_base_mutex_unlock_no_sche����Ϊӵ���������ٴ�����
 * ���������У����Դ˺���ʡȥ�˵��������ھ���������λ�õĲ���
 * �ҽ��ܹ����Ѿ������ٽ����Ļ����²���
 *
 * \param[in]   p_base_mutex        rtk_base_mutex����
 * \retval      0                   �Ժ���Ҫִ���������
 * \retval      1                   �Ժ���Ҫִ���������
 */
void rtk_base_mutex_unlock_not_ready_ec(rtk_base_mutex_t *p_base_mutex);

#endif          /*__RTK_BASE_MUTEX_H__*/
