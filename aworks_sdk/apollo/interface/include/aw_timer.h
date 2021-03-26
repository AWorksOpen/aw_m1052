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
 * \brief  �����ʱ���ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_timer.h
 *
 * \par ��ʱ���ṩ�����Է���
 * \code
 *
 * #define TIMER_MS    500
 * static  aw_timer_t  my_timer;
 *
 *  static void my_callback (void *p_arg)
 *  {
 *      // ִ����ز���
 *
 *      // ������ʱ��
 *      aw_timer_start(&my_timer, aw_ms_to_tick(TIMER_MS));
 *  }
 *
 *  static void timer_init(void) {
 *   
 *      // ��ʼ����ʱ��
 *      aw_timer_init(&my_timer, my_callback, 0);
 *
 *      // ������ʱ��
 *      aw_timer_start(&my_timer, aw_ms_to_tick(TIMER_MS));
 *
 *      // �ȴ���ʱ��������ִ�лص�����
 *  }
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.10 13-03-21  zen, refine the description
 * - 1.01 13-02-22  orz, fix the defines and code style.
 * - 1.00 12-10-23  liangyaozhan, first implementation.
 * \endinternal
 */

#ifndef __AW_TIMER_H
#define __AW_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

#include "aworks.h"
#include "aw_psp_timer.h"

/**
 * \brief �����ʱ������
 */
typedef aw_psp_timer_t   aw_timer_t;

/**
 * \addtogroup grp_aw_if_timer
 * \copydoc aw_timer.h
 * @{
 */

/**
 * \brief ��ʼ�������ʱ��
 *
 * ��ͼ��ʼ��һ�����ڹ����Ķ�ʱ�����ƻ��ں����ݽṹ��
 * �û�������ʹ�� aw_timer_stop() ����ֹͣ��ʱ�����ٳ�ʼ����
 *
 * \param[in] p_timer   �����ʱ��
 * \param[in] p_func    ��ʱ���ص���������ʱ����ʱ�����ô˻ص����������������
 * \param[in] p_arg     �ص������Ĳ���
 *
 * \return  �ɹ����������ʱ����ID��ʧ���򷵻�NULL
 *
 * \sa aw_timer_start(), aw_timer_destroy()
 */
void aw_timer_init (aw_timer_t *p_timer, aw_pfuncvoid_t p_func, void *p_arg);

/**
 * \brief ���������ʱ��
 *
 * ����ʱ����ʱ�������ʱ�������Զ�ֹͣ������Ҫ�����ԵĶ�ʱ������Ҫ�ڻص�������
 * �ٴε��� aw_timer_start() ��
 *
 * \note ���Ա���������������һ������Ϊ׼��
 *
 * \param[in] p_timer   �����ʱ��
 * \param[in] ticks     ��ʱ���ȣ���λ��ϵͳ����������ʹ�� aw_ms_to_ticks()
 *                      ��������ת��Ϊ������
 * \sa aw_timer_init()
 */
void aw_timer_start (aw_timer_t *p_timer, unsigned int ticks);

/**
 * \brief ֹͣ�����ʱ��
 *
 * \param[in] p_timer  �����ʱ��
 *
 * \sa aw_timer_init(), aw_timer_start()
 */
void aw_timer_stop (aw_timer_t *p_timer);


/**
 * \brief ���������ʱ��
 *
 * �ͷ������ʱ��ռ�õ���Դ
 *
 * \param[in] p_timer   �����ʱ��
 */
aw_inline static void aw_timer_destroy (aw_timer_t *p_timer)
{
    aw_timer_stop(p_timer);     /* FIXME: ��PSP��ʵ��������� */
}

/** @} grp_aw_if_timer */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_TIMER_H */

/* end of file */
