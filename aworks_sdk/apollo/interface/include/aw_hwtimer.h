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
 * \brief Ӳ����ʱ��������׼�ӿ�
 *
 * ��ģ���ṩӲ����ʱ������
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_hwtimer.h
 *
 * \note ϵͳ��Ӳ����ʱ����Դ���ޣ� ���Զ�ʱ�ľ���Ҫ�����ر�ߣ��Ƽ�ʹ��
 * \ref grp_aw_if_timer ���档
 *
 * \par ʹ��ʾ��
 * \code
 *
 * static aw_hwtimer_handle_t  mytmr;  // ��ʱ�����
 *
 * // ��ʱ���жϻص�
 * void my_timer_isr (void *p_arg)
 * {
 *     ; // �ж�ִ������
 * }
 * // ����һ������Ƶ��100000Hz����СƵ��1Hz�����Ƶ��8000Hz���жϣ�
 * // ���Զ���װ����ֵ�Ķ�ʱ��
 * mytmr = aw_hwtimer_alloc(100000,
 *                          1,
 *                          8000,
 *                          AW_HWTIMER_CAN_INTERRUPT | AW_HWTIMER_AUTO_RELOAD,
 *                          &my_timer_isr,
 *                          &count);
 * if (AW_HWTIMER_NULL != mytmr) {
 *     ; // ����ɹ�����һЩ��������
 *
 *     // ����ʱ������ʹ��ʱ���ͷŸö�ʱ��
 *     aw_hwtimer_release(mytmr);
 * }
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 12-11-14  orz, first implementation
 * \endinternal
 */

#ifndef __AW_HWTIMER_H
#define __AW_HWTIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_hwtimer
 * \copydoc aw_hwtimer.h
 * @{
 */

#include "aw_common.h"

/**
 * \name ��ʱ������
 * \brief ����ֽڴ���ʱ��λ�����綨ʱ��λ��Ϊ32��������ֽھ�Ϊ32.
          ������ֽڴ����˶�ʱ����ͨ����Ŀ����ͨ��Ϊ4��������ֽڼ�Ϊ4
          �������ָ���λ������һ�ֶ�ʱ������,��ˣ����֧��16�ֶ�ʱ������
          AW_HWTIMER_FEATURE(0) ~ AW_HWTIMER_FEATURE(15)
 * @{
 */

/** \brief  ����ֽڴ���ʱ����λ�� */
#define AW_HWTIMER_SIZE(size)              ((uint32_t)(size & 0x00fful))

/** \brief  �������Եõ���ʱ����λ�� */
#define AW_HWTIMER_SIZE_GET(features)      (features & 0x00fful)

/** \brief  �������Եõ���ʱ����������ֵ */
#define AW_HWTIMER_MAXCOUNT_GET(features)  ((uint32_t)((1ul << AW_HWTIMER_SIZE_GET(features)) - 1))

/** \brief  ��ʱ����ͨ����Ŀ */
#define AW_HWTIMER_CHAN_NUM(num)           (((uint32_t)num << 8) & 0xff00ul)

/** \brief  �������Եõ���ʱ����λ�� */
#define AW_HWTIMER_CHAN_NUM_GET(features)  ((features & 0x00ff00ul) >> 8)


/** \brief  ��ʱ�������Դӵ�8λ��ʼ */
#define  AW_HWTIMER_FEATURE(bit)           ((uint32_t)(1ul<< (bit + 16)))

/** \brief ��ʱ���ɲ����ж� */
#define AW_HWTIMER_CAN_INTERRUPT             AW_HWTIMER_FEATURE(0)

/** \brief ��ʱ������������ǰ����ֵ */
#define AW_HWTIMER_INTERMEDIATE_COUNT        AW_HWTIMER_FEATURE(1)

/** \brief ��ʱ�����ܱ���ֹ */
#define AW_HWTIMER_CANNOT_DISABLE            AW_HWTIMER_FEATURE(2)

/** \brief ����ȡ��ʱ����ǰ����ֵʱ��ʱ����ֹͣ���� */
#define AW_HWTIMER_STOP_WHILE_READ           AW_HWTIMER_FEATURE(3)

/** \brief ��ʱ���жϺ����Զ���װ������ֵ */
#define AW_HWTIMER_AUTO_RELOAD               AW_HWTIMER_FEATURE(4)

/** \brief ��ʱ���ļ��������������еģ������޸����ļ�����תֵ */
#define AW_HWTIMER_CANNOT_MODIFY_ROLLOVER    AW_HWTIMER_FEATURE(5)

/** \brief ��ʱ������֧����С�����Ƶ��֮�������Ƶ�ʣ�ֻ֧�ָ�����Ĳ���Ƶ�� */
#define AW_HWTIMER_CANNOT_SUPPORT_ALL_FREQS  AW_HWTIMER_FEATURE(6)

/** @} */

/**
 * \name  ��ʱ��ͨ���Ƿ������֣���ָ�������֡���Ԫ�š�ͨ���ţ�����
 * \brief ������ĳ��ͨ���Ƿ������ַ���
 *
 * @{
 */

/** \brief  ָ����ĳ��ͨ���������ַ��� */
#define AW_HWTIMER_CHAN_ALLOC_BY_NAME(ch)            ((uint32_t)(1ul << ch))

/** \brief ָ���˸ö�ʱ���������ַ��䣬����ֻ��һ��ͨ���Ķ�ʱ��ʹ�ô����� */
#define AW_HWTIMER_ALLOC_BY_NAME                     ((uint32_t)(0xFFFFFFFF))

/** \brief ָ���˸ö�ʱ���������ַ��䣬����ֻ��һ��ͨ���Ķ�ʱ��ʹ�ô����� */
#define AW_HWTIMER_NOT_ALLOC_BY_NAME                  ((uint32_t)(0))

/** @} */



/** \brief �ն�ʱ������������ж϶�ʱ������Ƿ���Ч */
#define AW_HWTIMER_NULL     ((void *)0)

/** \brief ��ʱ��������� */
typedef void *aw_hwtimer_handle_t;

/**
 * \brief ��ȡ��ʱ��������
 *
 * ����ӿ����ڻ�ȡ��ʱ������СƵ�ʡ����Ƶ�ʺ����Ա�־���������Ҫĳ����ȡĳ��
 * ����ֵ����Ѹ����Բ�����ΪNULL
 *
 * \param[in] timer        Ҫ��ѯ�Ķ�ʱ�����
 * \param[in] p_min_freq   ���涨ʱ����СƵ�ʵ�ָ��
 * \param[in] p_max_freq   ���涨ʱ�����Ƶ�ʵ�ָ��
 * \param[in] p_features   ���涨ʱ�����Ե�ָ��
 *
 * \retval AW_OK       û�д���
 * \retval -AW_EINVAL  ��Ч�Ķ�ʱ�����
 */
extern aw_err_t aw_hwtimer_features_get (aw_hwtimer_handle_t timer,
                                         uint32_t    *p_min_freq,
                                         uint32_t    *p_max_freq,
                                         uint32_t    *p_features);

/**
 * \brief ��ϵͳ����һ������ָ��Ҫ��Ķ�ʱ��
 *
 * \param[in] freq      �����Ķ�ʱ������Ƶ�ʣ�Ƶ��Խ�߾���Խ�ߣ�
 * \param[in] min_freq  ���������ܴﵽ����СƵ��
 * \param[in] max_freq  ���������ܴﵽ�����Ƶ�ʣ�Ӳ���ޱ�Ƶ�������max_freq �Q freq��
 * \param[in] features  ��ʱ������
 * \param[in] pfunc_isr ��ʱ���жϷ�����
 * \param[in] p_arg     ��ʱ���жϷ���������
 *
 * \note �������ɹ�����ʵ�ʵĶ�ʱ��������Ƶ�ʲ�һ������freq��
 *      ��СƵ�� �Q min_freq�����Ƶ�� �R max_freq��
 *      ʵ�ʲ�����ͨ�� aw_hwtimer_features_get() ��ȡ
 *
 * \return ��ʱ�������AW_HWTIMER_NULLΪ��Ч�Ķ�ʱ�����
 */
extern aw_hwtimer_handle_t aw_hwtimer_alloc (uint32_t  freq,
                                             uint32_t  min_freq,
                                             uint32_t  max_freq,
                                             uint32_t  features,
                                             void    (*pfunc_isr) (void *p_arg),
                                             void     *p_arg);
/**
 * \brief ��ϵͳ����һ������ָ�����ֺ��豸��Ԫ�ŵĶ�ʱ��
 *
 * \param[in] pname     ��ʱ��������
 * \param[in] uint      ��ʱ�����豸��Ԫ��
 * \param[in] time_no   ��ʱ������
 * \param[in] pfunc_isr ��ʱ���жϷ�����
 * \param[in] p_arg     ��ʱ���жϷ���������
 *
 * \return ��ʱ�������AW_HWTIMER_NULLΪ��Ч�Ķ�ʱ�����
 */
extern aw_hwtimer_handle_t aw_hwtimer_alloc_byname (const char *pname,
                                                    uint8_t     uint,
                                                    uint8_t     time_no,
                                                    void      (*pfunc_isr) (void *p_arg),
                                                    void       *p_arg);

/**
 * \brief �ͷŶ�ʱ��
 *
 * \param[in] timer Ҫ�ͷŵĶ�ʱ�����
 *
 * \return AW_OK Ϊ�ɹ�������������Ϊʧ��
 */
extern aw_err_t aw_hwtimer_release (aw_hwtimer_handle_t timer);

/**
 * \brief ʹ�ܶ�ʱ��
 *
 * \param[in] timer        Ҫʹ�ܵĶ�ʱ�����
 * \param[in] frequency_hz ��ʱ�ж�Ƶ��
 *
 * \return AW_OK Ϊ�ɹ�������������Ϊʧ��
 */
extern aw_err_t aw_hwtimer_enable (aw_hwtimer_handle_t timer, uint32_t frequency_hz);



/**
 * \brief ���ܶ�ʱ��
 *
 * \param[in] timer Ҫ��ֹ�Ķ�ʱ�����
 *
 * \return AW_OK Ϊ�ɹ�������������Ϊʧ��
 */
extern aw_err_t aw_hwtimer_disable (aw_hwtimer_handle_t timer);

/**
 * \brief ��ȡ��ʱ����ǰ����ֵ
 *
 * \param[in]     timer    ��ʱ�����
 * \param[in,out] p_count  ���涨ʱ������ֵ��ָ��
 *
 * \return AW_OK Ϊ�ɹ�������������Ϊʧ��
 */
extern aw_err_t aw_hwtimer_count_get (aw_hwtimer_handle_t timer, uint32_t *p_count);



/**
 * \brief ��ȡ��ʱ����תֵ
 *
 * \param[in]     timer    ��ʱ�����
 * \param[in,out] p_count  ���涨ʱ����תֵ��ָ��
 *
 * \return AW_OK Ϊ�ɹ�������������Ϊʧ��
 */
extern aw_err_t aw_hwtimer_rollover_get (aw_hwtimer_handle_t timer, uint32_t *p_count);


/**
 * \brief ��ȡ��ʱ����תֵ
 *
 * \param[in]     timer       ��ʱ�����
 * \param[in,out] p_clk_freq  ���ʱ��Ƶ��
 *
 * \return AW_OK Ϊ�ɹ�������������Ϊʧ��
 */
extern aw_err_t aw_hwtimer_clk_freq_get (aw_hwtimer_handle_t timer, uint32_t *p_clk_freq);


/**
 * \brief ��ȡ��ʱ����תֵ
 *
 * \param[in]     timer    ��ʱ�����
 * \param[in,out] p_count  ���涨ʱ����תֵ��ָ��
 *
 * \return AW_OK Ϊ�ɹ�������������Ϊʧ��
 */
extern aw_err_t aw_hwtimer_rollover_get64 (aw_hwtimer_handle_t timer, uint64_t *p_count);

/**
 * \brief ʹ��һ��64λ��ʱ��
 *
 * \param[in] timer        Ҫʹ�ܵĶ�ʱ�����
 * \param[in] frequency_hz ��ʱ�ж�Ƶ��
 *
 * \return AW_OK Ϊ�ɹ�������������Ϊʧ��
 */
extern aw_err_t aw_hwtimer_enable64 (aw_hwtimer_handle_t timer, uint64_t frequency_hz);

/**
 * \brief ��ȡ��ʱ����ǰ����ֵ
 *
 * \param[in]     timer    ��ʱ�����
 * \param[in,out] p_count  ���涨ʱ������ֵ��ָ��
 *
 * \return AW_OK Ϊ�ɹ�������������Ϊʧ��
 */
extern aw_err_t aw_hwtimer_count_get64 (aw_hwtimer_handle_t timer, uint64_t *p_count);

/**
 * \brief ����ʱ���Ķ�ʱʱ��(ms)ת��ΪƵ��
 *
 * \param[in] ms ��ʱ�����
 *
 * \return ����ʱ��ת���õ���Ƶ��
 */
extern uint32_t aw_hwtimer_ms_to_freq (uint32_t ms);

/**
 * \brief ����ʱ���Ķ�ʱʱ��(us)ת��ΪƵ��
 *
 * \param[in] us ��ʱ�����
 *
 * \return ����ʱ��ת���õ���Ƶ��
 */
extern uint32_t aw_hwtimer_us_to_freq (uint32_t us);

/** @} grp_aw_if_hwtimer */

#ifdef __cplusplus
}
#endif

#endif /* __AW_HWTIMER_H */

/* end of file */
