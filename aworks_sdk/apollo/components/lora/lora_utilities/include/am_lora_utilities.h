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
 * \brief ���ߺ���ʵ��
 *
 * \internal
 * \par Modification history
 * - 1.0.0 17-09-21  sky, first implementation.
 * \endinternal
 */

#ifndef __AM_LORA_UTILITIES_H
#define __AM_LORA_UTILITIES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <time.h>

/**
 * @addtogroup am_if_lora_utilities
 * @copydoc am_lora_utilities.h
 * @{
 */

/**
 * \brief utilities get rtc_time
 * \param [in] p_time : point to the time struct
 */
void am_lora_utilities_rtc_time_get (struct tm *p_time);

/**
 * \brief utilities set rtc_time
 * \param [in] p_time : point to the time struct
 */
void am_lora_utilities_rtc_time_set (struct tm *p_time);

/**
 * \brief utilities get the utc second (from 1979 0 0)
 * \param [in] none
 * \retval utc second
 */
uint32_t am_lora_utilities_rtc_sec_get (void);

/**
 * \brief utilities set the utc second (from 1979 0 0)
 * \param [in] value : second which is wait for set
 */
void am_lora_utilities_rtc_sec_set (uint32_t value);

/**
 * \brief enter sleep mode (for lowpower)
 * \param [in] none
 */
void am_lora_utilities_sys_sleep (void);

/**
 * \brief �ر�CPU�ж�
 * \attention �ر�ʱ��һ��Ҫ���ұ����� am_lora_cpu_unlock() ��������ʹ��
 * \return �жϹر���Ϣ
 */
uint32_t am_lora_cpu_lock (void);

/**
 * \brief ��CPU�ж�
 * \attention ������ am_lora_cpu_lock() ��������ʹ��
 * \param[in] key : �жϹر���Ϣ
 * \return    ��
 */
void am_lora_cpu_unlock (uint32_t cpu_key);

/**
 * \brief ���뼶����ʱ
 * \param[in] nms : ��ʱ�ĺ�����
 * \return ��
 */
void am_lora_mdelay (uint32_t nms);

/**
 * \brief ΢�뼶����ʱ
 * \param[in] nus : ��ʱ��΢����
 * \return ��
 */
void am_lora_udelay (uint32_t nus);

/**
 * \brief ������׼�������
 */
extern int (*am_lora_kprintf)(const char *fmt, ...);

/**
 * \brief ��ʼ����׼�������
 * \param[in] pfunc_printf : ƽ̨�ı�׼�������
 * \return ��
 */
void am_lora_kprintf_init (int (*pfunc_printf)(const char *fmt, ...));

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LORA_UTILITIES_H */

/* end of file */
