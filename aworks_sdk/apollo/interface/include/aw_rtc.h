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
 * \brief RTC (ʵʱʱ��) ��׼����
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_rtc.h
 *
 * \par ʹ��ʾ��
 * \code
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 13-09-05  zen, first implementation
 * \endinternal
 */

#ifndef __AW_RTC_H
#define __AW_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_time.h"

/**
 * \addtogroup grp_aw_if_rtc
 * \copydoc aw_rtc.h
 * @{
 */


/**
 * \brief ����RTCʱ��
 * \param[in] rtc_id   RTC �豸���, ��0��ʼ����
 * \param[in] p_tm     Ҫ���õ�ʱ��
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_ENODEV  RTC�豸������
 * \retval -AW_EIO     ����ʧ��
 */
aw_err_t aw_rtc_time_set (int rtc_id, aw_tm_t *p_tm);


/**
 * \brief ��ȡRTCʱ��
 * \param[in]  rtc_id  RTC �豸���, ��0��ʼ����
 * \param[out] p_tm    ��ȡ��ʱ��
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_ENODEV  RTC�豸������
 * \retval -AW_EIO     ��ȡʧ��
 */
aw_err_t aw_rtc_time_get (int rtc_id, aw_tm_t *p_tm);


/**
 * \brief ��ȡRTC����ʱ�� (�롢�����ʾ)
 * \param[in]  rtc_id  RTC �豸���, ��0��ʼ����
 * \param[out] p_tv    ��ȡ������ʱ��
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_ENODEV  RTC�豸������
 * \retval -AW_EIO     ��ȡʧ��
 */
aw_err_t aw_rtc_timespec_get(int rtc_id, aw_timespec_t *p_tv);


/**
 * \brief ����RTC����ʱ�� (�롢�����ʾ)
 * \param[in] rtc_id   RTC �豸���, ��0��ʼ����
 * \param[in] p_tv     Ҫ���õ�����ʱ��
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_ENODEV  RTC�豸������
 * \retval -AW_EIO     ����ʧ��
 */
aw_err_t aw_rtc_timespec_set(int rtc_id, aw_timespec_t *p_tv);


/** @} grp_aw_if_rtc */

#ifdef __cplusplus
}
#endif

#endif /* __AW_RTC_H */

/* end of file */

