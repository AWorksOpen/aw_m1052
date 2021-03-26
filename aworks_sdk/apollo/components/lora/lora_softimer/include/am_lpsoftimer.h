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
 * \brief Software Timer �����ʱ��
 *
 * \details
 *        ��һ��Ӳ����ʱ����ֳɶ�������ʱ����������softimer������ÿtickִ��һ�Σ�
 *        ��������Ӳ����ʱ����ƥ�䣨match������ʵ�ּ�ϵش����������������ڵ͹��ĳ��ϡ�
 *        �����Ӳ����ʱ����֧���ڵ͹���ģʽ�й������ܲ����ɻ��ѵ��жϣ�
 *        ��ýӿ�ʵ�ֵ������ʱ���߱��͹��Ļ��ѹ��ܡ�
 *
 * \internal
 * \par Modification history
 * - 1.0.0 17-06-13  sky, ebi, �ӿڶ���
 * \endinternal
 */

#ifndef __SOFTIMER_H
#define __SOFTIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup if_softimer
 * @copydoc softimer.h
 * @{
 */

#include <stdio.h>
#include <stdint.h>

/**
 * \brief softimer ʱ������
 */
typedef uint64_t am_lpsoftimer_time_t;

/**
 * \brief softimer����
 */
typedef struct am_lpsoftimer {

    am_lpsoftimer_time_t  dest_time;              /**< \brief Ŀ��ʱ��(ms) */
    am_lpsoftimer_time_t  reload_time;            /**< \brief ���ص�ʱ��(ms) */
    uint8_t               is_running;             /**< \brief �Ƿ����������� */
    void                 (*pfn_cb) (void *p_arg); /**< \brief ��ʱ���ص����� */
    void                  *p_arg;                 /**< \brief �ص������������ */

    struct am_lpsoftimer *p_next;                 /**< \brief ָ����һ����ʱ�� */

} am_lpsoftimer_t;

/**
 * \brief softimer ��ʱ��ʵ����ʼ��
 *
 * \param[in] p_obj        : ��Ҫ��ʼ����softimerʵ��
 * \param[in] pfn_callback : ָ��ö�ʱ���ص�����
 * \param[in] p_cb_arg     : �ص�������p_arg����
 *
 * \return ��
 */
void am_lpsoftimer_init (am_lpsoftimer_t *p_obj,
                         void           (*pfn_callback)(void *p_arg),
                         void            *p_arg);

/**
 * \brief softimer �趨��ʱֵ
 *
 * \param[in] p_obj : ָ��softimerʵ��
 * \param[in] time  : ָ��ö�ʱ������ʱֵ��ms��
 *
 * \note ���Ϊ0ʱ��Ч���ڲ�������Ϊ1.
 * \return ��
 */
void am_lpsoftimer_value_set (am_lpsoftimer_t *p_obj, am_lpsoftimer_time_t time);

/**
 * \brief softimer ��ʱ������
 *
 * \param[in] p_obj : softimerʵ��
 *
 * \return ��
 */
void am_lpsoftimer_start (am_lpsoftimer_t *p_obj);

/**
 * \brief softimer ��ʱ��ֹͣ
 *
 * \param[in] p_obj : softimerʵ��
 *
 * \return ��
 */
void am_lpsoftimer_stop (am_lpsoftimer_t *p_obj);

/**
 * \brief softimer ��ʱ����������
 *
 * \param[in] p_obj : softimerʵ��
 *
 * \return ��
 */
void am_lpsoftimer_restart (am_lpsoftimer_t *p_obj);

/**
 * \brief softimer ��ȡ��ȥʱ��
 *
 * \param[in] saved_time : �ο�ʱ���(��Ҫ����δ����ʱ�䣬����ᵼ�¼���ֵ���)
 *
 * \return ��ȥʱ��
 */
am_lpsoftimer_time_t am_lpsoftimer_elapsed_get (am_lpsoftimer_time_t saved_time);

/**
 * \brief softimer ��ȡ��ǰ�����ʱֵ(��ϵͳ��������)
 *
 * \param[in] ��
 *
 * \return ��ʱֵ
 */
am_lpsoftimer_time_t am_lpsoftimer_current_get (void);

/**
 * \brief softimer ������һ�γ�ʱ
 *
 * \param[in] timeout : �����ڿ�ʼ���֮����붨ʱ���ж�
 *
 * \return ��
 */
void am_lpsoftimer_timeout_set (am_lpsoftimer_time_t timeout);

/**
 * \brief softimer �¶Ȳ���
 *
 * \param[in] period      : ʱ������
 * \param[in] temperature : �¶�
 *
 * \return �������ʱ������
 */
uint32_t am_lpsoftimer_temp_compensation (uint32_t period, float temperature);

/**
 * \brief softimer �жϷ�����
 *
 * \param[in] ��
 *
 * \return ��
 *
 * \note ��ƽ̨�Ķ�ʱ���жϷ������лص��ú���
 */
void am_lpsoftimer_isr (void);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __SOFTIMER_H */

/* end of file */
