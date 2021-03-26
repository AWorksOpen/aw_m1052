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
 * \brief ������(spin lock)��׼�ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_spinlock.h
 *
 * \par ��ʾ��һ
 *
 * \code
 *  #include "aworks.h"
 *  #include "aw_spinlock.h"
 *
 *  aw_spinlock_isr_t lock;      // ����һ��������
 *
 *  aw_spinlock_isr_init(&lock); // ��ʼ����
 *  aw_spinlock_isr_take(&lock); // ��ȡ��
 *  // ��Ҫ�������ٽ����...
 *  aw_spinlock_isr_give(&lock); // �ͷ���
 * \endcode
 *
 * \par ��ʾ����
 *
 * \code
 *  #include "aworks.h"
 *  #include "aw_spinlock.h"
 *
 *  aw_spinlock_isr_t lock = AW_SPINLOCK_ISR_UNLOCK;  // ���岢��ʼ��һ��������
 *
 *  aw_spinlock_isr_take(&lock); // ��ȡ��
 *  // ��Ҫ�������ٽ����...
 *  aw_spinlock_isr_give(&lock); // �ͷ���
 * \endcode
 *
 * // �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.00 12-08-27  zen, first implementation
 * \endinternal
 */

#ifndef __AW_SPINLOCK_H
#define __AW_SPINLOCK_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_aw_if_spinlock
 * \copydoc aw_spinlock.h
 * @{
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "aw_psp_spinlock.h"

/*******************************************************************************
  defines
*******************************************************************************/

/**
 * \brief ISR�ɵ�����������δ������ʼֵ
 * \hideinitializer
 */
#define AW_SPINLOCK_ISR_UNLOCK     AW_PSP_SPINLOCK_ISR_UNLOCK

/**
 * \brief ���岢��ʼ��һ��ISR�ɵ���������
 *
 * \param[in] x      ��Ҫ�����������
 * \param[in] flags  ��־
 *
 * \hideinitializer
 */
#define AW_SPINLOCK_ISR_DECL(x, flags) AW_PSP_SPINLOCK_ISR_DECL((x), (flags))

/*******************************************************************************
  typedefs
*******************************************************************************/

/**
 * \brief ISR�ɵ�����������������
 */
typedef aw_psp_spinlock_isr_t  aw_spinlock_isr_t;

/*******************************************************************************
  interfaces
*******************************************************************************/

/**
 * \brief ��ʼ��һ��ISR�ɵ���������
 *
 * ��������ʼ��һ����\a lock ָ����ISR�ɵ�������������ǰ\a flags û�б����壬
 * ��������������֮�á�һ���������ڵ�һ�α�ʹ��ǰ�����뱻��ʼ����
 *
 * \attention ���������ж��е��ô˺�����
 *
 * \param[in,out]   p_lock ����ʼ������
 * \param[in]       flags ��־
 */
void aw_spinlock_isr_init(aw_spinlock_isr_t *p_lock, int flags);

/**
 * \brief �ͷ�һ��ISR�ɵ���������
 *
 * �������ͷ�һ����\a lock ָ����ISR�ɵ�����������
 *
 * \param[in,out]   p_lock ����ʼ������
 */
void aw_spinlock_isr_give(aw_spinlock_isr_t *p_lock);

/**
 * \brief ����һ��ISR�ɵ���������
 *
 * ����������һ����\a lock ָ����ISR�ɵ�����������
 *
 * \param[in,out]   p_lock ����ʼ������
 */
void aw_spinlock_isr_take(aw_spinlock_isr_t *p_lock);

/** @}  grp_aw_if_spinlcok */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_SPINLOCK_H */

/* end of file */
