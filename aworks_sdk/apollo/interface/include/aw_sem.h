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
 * \brief RTOS�ź�����������ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_sem.h
 *
 * \par ��ʾ��
 * \code
 *  #include "aw_sem.h"
 *
 *  AW_TASK_DECL_STATIC(task_a, 512);   // �������� task_a
 *  AW_TASK_DECL_STATIC(task_b, 512);   // �������� task_b
 *
 *  AW_SEMB_DECL_STATIC(sem);           // ����������ź��� sem
 *
 *  void func_a (void *arg)
 *  {
 *      while (1) {
 *          //...
 *          AW_SEMB_GIVE(sem);          //�ͷ��ź���
 *      }
 *  }
 *
 *  void func_b (void *arg)
 *  {
 *      while (1) {
 *          AW_SEMB_TAKE(sem, AW_SEM_WAIT_FOREVER); //�ȴ��ź���
 *          //...
 *      }
 *  }
 *
 *  int main ()
 *  {
 *
 *      // ��ʼ���ź���
 *      AW_SEMB_INI(sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
 *
 *      // ��ʼ������
 *      AW_TASK_INIT(task_a,            // ����ʵ��
 *                   "task_a",          // ��������
 *                   5,                 // �������ȼ�
 *                   512,               // �����ջ��С
 *                   func_a,            // ������ں���
 *                   NULL);             // ������ڲ���
 *
 *      AW_TASK_INIT(task_b,            // ����ʵ��
 *                   "task_b",          // ��������
 *                   6,                 // �������ȼ�
 *                   512,               // �����ջ��С
 *                   func_b,            // ������ں���
 *                   NULL);             // ������ڲ���
 *
 *      // ��������
 *      AW_TASK_STARTUP(task_a);
 *      AW_TASK_STARTUP(task_b);
 *
 *      return 0;
 *  }
 * \endcode
 *
 * //�������ݴ���ӡ�����
 *
 * \internal
 * \par modification history
 * - 1.12 17-09-05  anu, refine the description
 * - 1.11 17-09-06  zen, refine the description
 * - 1.10 13-03-04  zen, refine the description
 * - 1.02 12-11-20  lyzh, rename some MACRO name. Merge aw_semb.h, aw_semc.h,
 *                  and aw_mutex.h
 * - 1.01 12-11-05  orz, implement with aw_task.h
 * - 1.00 12-10-23  orz, created
 * \endinternal
 */

#ifndef __AW_SEM_H
#define __AW_SEM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_sem
 * \copydoc aw_sem.h
 * @{
 */

#include "aw_psp_sem.h"

/**
 * \name ��ʱ��������
 * @{
 */
/**
 * \brief ���õȴ�
 * \hideinitializer
 */
#define AW_SEM_WAIT_FOREVER     AW_PSP_SEM_WAIT_FOREVER

/**
 * \brief �޵ȴ�
 * \hideinitializer
 */
#define AW_SEM_NO_WAIT          AW_PSP_SEM_NO_WAIT
/** @} ��ʱ�������� */


/**
 * \name �ź�����ʼ��ѡ��
 * @{
 */
/**
 * \brief �Ŷӷ�ʽΪ FIFO (�Ƚ��ȳ�)
 * \hideinitializer
 */
#define AW_SEM_Q_FIFO         AW_PSP_SEM_Q_FIFO

/**
 * \brief �Ŷӷ�ʽΪ���ȼ�
 * \hideinitializer
 */
#define AW_SEM_Q_PRIORITY     AW_PSP_SEM_Q_PRIORITY

/**
 * \brief �����ź���ɾ����ȫ (���ڻ����ź���)
 * \hideinitializer
 */
#define AW_SEM_DELETE_SAFE    AW_PSP_SEM_DELETE_SAFE

/**
 * \brief ��ֹ�����ź������ȼ���ת (���ڻ����ź���)
 * \hideinitializer
 */
#define AW_SEM_INVERSION_SAFE   AW_PSP_SEM_INVERSION_SAFE
/** @} �ź�����ʼ��ѡ�� */



#include "_aw_semb.h"
#include "_aw_semc.h"
#include "_aw_mutex.h"




/** @} grp_aw_if_sem */

#ifdef __cplusplus
}
#endif

#endif /* __AW_SEMAPHORE */

/* end of file */
