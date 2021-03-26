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
 * \brief LORA ��������ӿ�,����Ϣд��flash
 *
 * \internal
 * \par Modification history
 * - 1.00 17-08-21  sky, first implementation
 * \endinternal
 */

#ifndef __AM_LORA_STORE_H
#define __AM_LORA_STORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @addtogroup am_if_lora_store
 * @copydoc am_at_lora_store.h
 * @{
 */

/**
 * \brief ��ȡоƬUID
 *
 * \param[out] p_uid_out : ��������UID�Ļ�����
 * \param[in]  length    : ��Ҫ���յĳ���
 *
 * \note KL26 UIDΪ10Byte�����յĳ��Ȳ���С��10�����16��10~16֮���Byte��0���
 *
 * \retval AM_OK      : ��ȡ�ɹ�
 * \retval -AM_EINVAL : ��������
 */
int am_uid_get (uint8_t *p_uid_out, uint8_t length);

/**
 * \brief ������Ϣ
 *
 * \param[in] p_info_in  : ��Ҫ�������Ϣ
 * \param[in] length     : ��Ϣ�ĳ���
 * \param[in] sector_num : Ŀ������
 *
 * \note ���ȱ���Ϊ4����������������
 *
 * \retval AM_OK      : ����ɹ�
 * \retval -AM_EINVAL : ��������
 */
int am_lora_save (uint8_t *p_info_in, uint16_t length, uint16_t sector_num);

/**
 * \brief ������Ϣ
 *
 * \param[out] p_info_out : ����������Ϣ�Ļ�����
 * \param[in]  length     : ��Ϣ�ĳ���
 * \param[in]  sector_num : Ŀ������
 *
 * \retval AM_OK      : ����ɹ�
 * \retval -AM_EINVAL : ��������
 */
int am_lora_load (uint8_t *p_info_out, uint16_t length, uint16_t sector_num);

/**
 * @} am_if_lora_store
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LORA_STORE_H */

/* end of file */
