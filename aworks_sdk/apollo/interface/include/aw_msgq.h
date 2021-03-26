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
 * \brief  AWorks ��Ϣ���б�׼�ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_msgq.h
 *
 * \par ����: �����÷�
 * \code
 *  AW_TASK_DECL_STATIC(task_a, 512);
 *  AW_TASK_DECL_STATIC(task_b, 512);
 *
 *  AW_MSGQ_DECL_STATIC(msgq, 10, 16);  // ������Ϣ���У�10����Ϣ, ÿ��16�ֽ�
 *
 *  void func_a (void *arg)
 *  {
 *      while (1) {
 *          // ������Ϣ (������)
 *          AW_MSGQ_SEND(msgq,                  // ��Ϣ����ʵ��
 *                       "I'm task_a",          // ��Ϣ����
 *                       11,                    // ��Ϣ�ĳ���
 *                       AW_MSGQ_WAIT_FOREVER); // ���õĵȴ�
 *
 *          aw_mdelay(50);
 *      }
 *  }
 *
 *  void isr (void)
 *  {
 *      aw_err_t ret;
 *
 *      // ������Ϣ (ISR��)
 *      ret = AW_MSGQ_SEND(msgq,                // ��Ϣ����ʵ��
 *                         "I'm ISR",           // ��Ϣ����
 *                         8,                   // ��Ϣ�ĳ���
 *                         AW_MSGQ_NO_WAIT);    // �޵ȴ�(ISR�б���Ϊ��ֵ)
 *
 *      if (ret == -AW_EAGAIN) {
 *          // ��Ϣ��������δ�ܳɹ�������Ϣ
 *      }
 *  }
 *
 *  void func_b (void *arg)
 *  {
 *      char buf[16];
 *      while (1) {
 *          // ������Ϣ
 *          AW_MSGQ_RECEIVE(msgq,                   // ��Ϣ����ʵ��
 *                          &buf[0],                // ��Ϣ������
 *                          16,                     // ����ܽ���16���ֽ�
 *                          AW_MSGQ_WAIT_FOREVER);  // ���õȴ�
 *
 *          aw_kprintf("%s", buf); 		            // ��ӡ���յ�����Ϣ
 *      }
 *  }
 *
 *  int main()
 *  {
 *      // ��ʼ����Ϣ����
 *      AW_MSGQ_INIT(msgq, AW_MSGQ_Q_PRIORITY);
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
 * \par ����������Ϣ����ʵ��Ƕ����һ�����ݽṹ
 * \code
 *  #include "aw_msgq.h"
 *  #include "aw_vdebug.h"
 *
 *  struct my_struct {
 *      int my_data1;
 *      AW_TASK_DECL(task_a, 512);          // ����ʵ��task_a, ��ջ512
 *      AW_TASK_DECL(task_b, 512);          // ����ʵ��task_b, ��ջ512
 *      AW_MSGQ_DECL(msgq, 10, 16);         // ��Ϣ����ʵ��:10����Ϣ��ÿ��16�ֽ�
 *      aw_msgq_id_t msgq_id;               // ��Ϣ����ID
 *  };
 *  struct my_struct   my_object;           // ���������ݶ���
 *
 *  void func_a (void *arg)
 *  {
 *      struct my_struct *p_myobj = arg;
 *      while (1) {
 *          // ������Ϣ(ʹ����Ϣ����ʵ��)
 *          AW_MSGQ_SEND(p_myobj->msgq,     // ��Ϣ����ʵ��
 *                       "I'm task_a",      // ��Ϣ
 *                       11,                // ��Ϣ��Ϣ����
 *                       AW_MSGQ_WAIT_FOREVER);
 *          aw_mdelay(50);
 *      }
 *  }
 *
 *  void func_b (void *arg)
 *  {
 *      struct my_struct *p_myobj = arg;
 *      while (1) {
 *          // ������Ϣ(ʹ����Ϣ����ID)
 *          aw_msgq_receive(p_myobj->msgq_id,       // ��Ϣ����ʵ��
 *                          &buf[0],                // ��Ϣ������
 *                          16,                     // ����ܽ���16���ֽ�
 *                          AW_MSGQ_WAIT_FOREVER);  // ���õȴ�
 *
 *          aw_kprintf("%s", buf); 		            // ��ӡ���յ�����Ϣ
 *      }
 *  }
 *
 *  int main() {
 *
 *      // ��ʼ����Ϣ����
 *      my_object.msgq_id = AW_MSGQ_INIT(my_object.msgq, AW_MSGQ_Q_PRIORITY);
 *      if (my_object.msgq_id == NULL) {
 *          return -1;  // ��Ϣ���г�ʼ��ʧ��
 *      }
 *
 *      // ��ʼ������
 *      AW_TASK_INIT(my_object.task_a,      // ����ʵ��
 *                   "task_a",              // ��������
 *                   5,                     // �������ȼ�
 *                   512,                   // �����ջ��С
 *                   func_a,                // ������ں���
 *                   (void *)&my_object);   // ������ڲ���
 *
 *      AW_TASK_INIT(my_object.task_b,      // ����ʵ��
 *                   "task_b",              // ��������
 *                   6,                     // �������ȼ�
 *                   512,                   // �����ջ��С
 *                   func_b,                // ������ں���
 *                   (void *)&my_object);   // ������ڲ���
 *
 *      // ��������
 *      AW_TASK_STARTUP(my_object.task_a);
 *      AW_TASK_STARTUP(my_object.task_b);
 *
 *      aw_mdelay(20 * 1000);   //��ʱ20��
 *
 *      // AW_MSGQ_TERMINATE(my_object.msgq);   // ��ֹ��Ϣ����
 *      // aw_msgq_terminate(my_object.msgq_id);// ��ֹ��Ϣ����(ʹ����Ϣ����ID)
 *
 *      return 0;
 *  }
 * \endcode
 *
 * // �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.11 17-09-06  anu, refine the description
 * - 1.10 13-02-28  zen, refine the description
 * - 1.01 13-01-24  orz, fix code style
 * - 1.00 12-10-23  liangyaozhan, first implementation
 * \endinternal
 */

#ifndef __AW_MSGQ_H
#define __AW_MSGQ_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_aw_if_msgq
 * \copydoc aw_msgq.h
 * @{
 */

#include "aw_psp_msgq.h"

/**
 * \name ��ʱ��������
 * @{
 */
/**
 * \brief ���õȴ�
 * \hideinitializer
 */
#define AW_MSGQ_WAIT_FOREVER     AW_PSP_MSGQ_WAIT_FOREVER

/**
 * \brief �޵ȴ�
 * \hideinitializer
 */
#define AW_MSGQ_NO_WAIT         AW_PSP_MSGQ_NO_WAIT
/** @} ��ʱ�������� */


/**
 * \name ��Ϣ���г�ʼ��ѡ��
 * @{
 */
/**
 * \brief �Ŷӷ�ʽΪ FIFO (�Ƚ��ȳ�)
 * \hideinitializer
 */
#define AW_MSGQ_Q_FIFO            AW_PSP_MSGQ_Q_FIFO

/**
 * \brief �Ŷӷ�ʽΪ���ȼ�
 * \hideinitializer
 */
#define AW_MSGQ_Q_PRIORITY        AW_PSP_MSGQ_Q_PRIORITY
/** @} ��Ϣ���г�ʼ��ѡ�� */

/**
 * \name ��Ϣ�������ȼ�ѡ��
 * @{
 */
/**
 * \brief ��ͨ���ȼ�
 * \hideinitializer
 */
#define AW_MSGQ_PRI_NORMAL        AW_PSP_MSGQ_RPI_NORMAL

/**
 * \brief �������ȼ�
 * \hideinitializer
 */
#define AW_MSGQ_PRI_URGENT        AW_PSP_MSGQ_PRI_URGENT
/** @} ��Ϣ�������ȼ�ѡ�� */

/**
 *  \name ��Ϣ�������Ͷ���
 *  @{
 */
typedef aw_psp_msgq_id_t   aw_msgq_id_t;   /**< ��Ϣ����ID         */
/** @} ��Ϣ�������Ͷ��� */


/**
 * \brief ������Ϣ����
 *
 * ���궨��һ����Ϣ���У��ڱ���ʱ�����Ϣ���������ڴ�ķ���(������Ϣ������);
 * ������ʱ����Ҫ���� AW_MSGQ_INIT() �����Ϣ���еĳ�ʼ��������
 * AW_MSGQ_TERMINATE() ��ֹ��Ϣ���С�
 *
 * ���� \a msgq ֻ����Ϊ AW_MSGQ_INIT(), AW_MSGQ_SEND(), AW_MSGQ_RECEIVE() ��
 * AW_MSGQ_TERMINATE() �Ĳ�����
 *
 * AW_MSGQ_INIT() ������Ϣ���е� \a msgq_id, \a msgq_id ��Ϊ���� aw_msgq_send(),
 * aw_msgq_receive() �� aw_msgq_terminate()�Ĳ���; ��Щ�����Ĺ������Ӧ�ĺ��
 * ������ͬ, ��������, ����ʹ�� \a msgq_id ��Ϊ����, \a msgq_id ������Ϊ��Ϣ����
 * �ľ���ں�������֮�䴫�ݡ�
 *
 * ����ʹ�ñ��꽫��Ϣ����ʵ��Ƕ�뵽��һ�����ݽṹ��, �����Ļ�, �����Ǹ��������
 * �����ʱ����Զ��������Ϣ����ʵ����ڴ���䣬������ķ�����ʾ��
 *
 * \attention ����Ϣ����û�п�ģ��(�ļ�)ʹ�õ������Ƽ�ʹ��
 *            AW_MSGQ_DECL_STATIC() ���汾�꣬�����ɱ���ģ��֮����Ϣ��������
 *            ��Ⱦ��
 *
 * \attention �������÷��ⲻ, ��Ī�ں�����ʹ�ô˺�������������Ϊ�ں����ڴ˺��
 *            ����ڴ�������ջ�ϣ�һ���������أ�������ڴ汻�Զ����ա��ں����ڲ�
 *            ʹ�� AW_TASK_DECL_STATIC()����������
 *
 * \param[in] msgq  ��Ϣ����ʵ��, �� AW_MSGQ_INIT(), AW_MSGQ_SEND(),
 *                  AW_MSGQ_RECEIVE() �� AW_MSGQ_TERMINATE() һ��ʹ�á�
 *
 * \param[in] msg_num  ��Ϣ����, ��Ϣ��������С = \a msg_num * \a msg_size
 * \param[in] msg_size ��Ϣ��С, ��Ϣ��������С = \a msg_num * \a msg_size
 *
 * \par �����������÷�
 * \code
 *  AW_TASK_DECL_STATIC(task_a, 512);
 *  AW_TASK_DECL_STATIC(task_b, 512);
 *
 *  AW_MSGQ_DECL_STATIC(msgq, 10, 16);  // ������Ϣ���У�10����Ϣ, ÿ��16�ֽ�
 *
 *  void func_a (void *arg)
 *  {
 *      while (1) {
 *          // ������Ϣ (������)
 *          AW_MSGQ_SEND(msgq,                  // ��Ϣ����ʵ��
 *                       "I'm task_a",          // ��Ϣ����
 *                       11,                    // ��Ϣ�ĳ���
 *                       AW_MSGQ_WAIT_FOREVER); // ���õĵȴ�
 *
 *          aw_mdelay(50);
 *      }
 *  }
 *
 *  void isr (void)
 *  {
 *      aw_err_t ret;
 *
 *      // ������Ϣ (ISR��)
 *      ret = AW_MSGQ_SEND(msgq,                // ��Ϣ����ʵ��
 *                         "I'm ISR",           // ��Ϣ����
 *                         8,                   // ��Ϣ�ĳ���
 *                         AW_MSGQ_NO_WAIT);    // �޵ȴ�(ISR�б���Ϊ��ֵ)
 *
 *      if (ret == -AW_EAGAIN) {
 *          // ��Ϣ��������δ�ܳɹ�������Ϣ
 *      }
 *  }
 *
 *  void func_b (void *arg)
 *  {
 *      char buf[16];
 *      while (1) {
 *          // ������Ϣ
 *          AW_MSGQ_RECEIVE(msgq,                   // ��Ϣ����ʵ��
 *                          &buf[0],                // ��Ϣ������
 *                          16,                     // ����ܽ���16���ֽ�
 *                          AW_MSGQ_WAIT_FOREVER);  // ���õȴ�
 *
 *          aw_kprintf("%s", buf); 		            // ��ӡ���յ�����Ϣ
 *      }
 *  }
 *
 *  int main()
 *  {
 *      // ��ʼ����Ϣ����
 *      AW_MSGQ_INIT(msgq, AW_MSGQ_Q_PRIORITY);
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
 * \par ����������Ϣ����ʵ��Ƕ����һ�����ݽṹ
 * \code
 *  struct my_struct {
 *      int my_data1;
 *      AW_TASK_DECL(task_a, 512);          // ����ʵ��task_a, ��ջ512
 *      AW_TASK_DECL(task_b, 512);          // ����ʵ��task_b, ��ջ512
 *      AW_MSGQ_DECL(msgq, 10, 16);         // ��Ϣ����ʵ��:10����Ϣ��ÿ��16�ֽ�
 *      aw_msgq_id_t msgq_id;               // ��Ϣ����ID
 *  };
 *  struct my_struct   my_object;           // ���������ݶ���
 *
 *  void func_a (void *arg)
 *  {
 *      struct my_struct *p_myobj = arg;
 *      while (1) {
 *          // ������Ϣ(ʹ����Ϣ����ʵ��)
 *          AW_MSGQ_SEND(p_myobj->msgq,     // ��Ϣ����ʵ��
 *                       "I'm task_a",      // ��Ϣ
 *                       11,                // ��Ϣ��Ϣ����
 *                       AW_MSGQ_WAIT_FOREVER);
 *          aw_mdelay(50);
 *      }
 *  }
 *
 *  void func_b (void *arg)
 *  {
 *      struct my_struct *p_myobj = arg;
 *      while (1) {
 *          // ������Ϣ(ʹ����Ϣ����ID)
 *          aw_msgq_receive(p_myobj->msgq_id,       // ��Ϣ����ʵ��
 *                          &buf[0],                // ��Ϣ������
 *                          16,                     // ����ܽ���16���ֽ�
 *                          AW_MSGQ_WAIT_FOREVER);  // ���õȴ�
 *
 *          aw_kprintf("%s", buf); 		            // ��ӡ���յ�����Ϣ
 *      }
 *  }
 *
 *  int main()
 *  {
 *      // ��ʼ����Ϣ����
 *      my_object.msgq_id = AW_MSGQ_INIT(my_object.msgq, AW_MSGQ_Q_PRIORITY);
 *      if (my_object.msgq_id == NULL) {
 *          return -1;  // ��Ϣ���г�ʼ��ʧ��
 *      }
 *
 *      // ��ʼ������
 *      AW_TASK_INIT(my_object.task_a,      // ����ʵ��
 *                   "task_a",              // ��������
 *                   5,                     // �������ȼ�
 *                   512,                   // �����ջ��С
 *                   func_a,                // ������ں���
 *                   (void *)&my_object);   // ������ڲ���
 *
 *      AW_TASK_INIT(my_object.task_b,      // ����ʵ��
 *                   "task_b",              // ��������
 *                   6,                     // �������ȼ�
 *                   512,                   // �����ջ��С
 *                   func_b,                // ������ں���
 *                   (void *)&my_object);   // ������ڲ���
 *
 *      // ��������
 *      AW_TASK_STARTUP(my_object.task_a);
 *      AW_TASK_STARTUP(my_object.task_b);
 *
 *      aw_mdelay(20 * 1000);   //��ʱ20��
 *
 *      // AW_MSGQ_TERMINATE(my_object.msgq);    // ��ֹ��Ϣ����
 *      // aw_msgq_terminate(my_object.msgq_id); // ��ֹ��Ϣ����(ʹ����Ϣ����ID)
 *
 *      return 0;
 *  }
 * \endcode
 *
 * \sa AW_MSGQ_DECL_STATIC(), AW_MSGQ_INIT(), AW_MSGQ_RECEIVE(), AW_MSGQ_SEND,
 *     AW_MSGQ_TERMINATE()
 * \sa aw_msgq_receive(), aw_msgq_send(), aw_msgq_terminate()
 * \hideinitializer
 */
#define AW_MSGQ_DECL(msgq, msg_num, msg_size)  \
            AW_PSP_MSGQ_DECL(msgq, msg_num, msg_size)

/**
 * \brief ������Ϣ����(��̬)
 *
 * ������ AW_MSGQ_DECL() �Ĺ�����ͬ���������ڣ� AW_MSGQ_DECL_STATIC() �ڶ�����
 * Ϣ���������ڴ�ʱ��ʹ�ùؼ��� \b static �����һ��������Խ���Ϣ����ʵ�������
 * ��������ģ����(�ļ���), �Ӷ�����ģ��֮�����Ϣ����������ͻ��
 * �������ں�����ʹ�ñ��궨����Ϣ���С�
 *
 * \param[in] msgq  ��Ϣ����ʵ��, �� AW_MSGQ_INIT(), AW_MSGQ_SEND(),
 *                  AW_MSGQ_RECEIVE() �� AW_MSGQ_TERMINATE() һ��ʹ�á�
 *
 * \param[in] msg_num  ��Ϣ����, ��Ϣ��������С = \a msg_num * \a msg_size
 * \param[in] msg_size ��Ϣ��С, ��Ϣ��������С = \a msg_num * \a msg_size
 *
 * \par ����
 * �� AW_MSGQ_DECL()
 *
 * \sa AW_MSGQ_DECL(), AW_MSGQ_INIT(), AW_MSGQ_RECEIVE(), AW_MSGQ_SEND,
 *     AW_MSGQ_TERMINATE()
 * \sa aw_msgq_receive(), aw_msgq_send(), aw_msgq_terminate()
 * \hideinitializer
 */
#define AW_MSGQ_DECL_STATIC(msgq, msg_num, msg_size)   \
            AW_PSP_MSGQ_DECL_STATIC(msgq, msg_num, msg_size)

/**
 * \brief ������Ϣ����
 *
 * ��������һ����Ϣ���У��൱��ʹ�ùؼ� \b extern ����һ���ⲿ���š�
 * ���Ҫʹ����һ��ģ��(�ļ�)�ж������Ϣ���У�����ʹ�ñ����������Ϣ���У�Ȼ��
 * �Ϳ��Բ�������Ϣ���С����������Ϣ����Ӧ����ʹ�� AW_MSGQ_DECL()����ġ�
 *
 * \param[in] msgq  ��Ϣ����
 *
 * \par ����
 * \code
 *  #include "aw_msgq.h"
 *
 *  AW_MSGQ_IMPORT(msgq_x);         // ������Ϣ���� msgq_x
 *
 *  int func () {
 *      AW_MSGQ_SEND(msgq_x);       // ����Ϣ���� msgq_x ������Ϣ
 *  }
 * \endcode
 *
 * \sa AW_MSGQ_DECL(), AW_MSGQ_DECL_STATIC(), AW_MSGQ_INIT(), AW_MSGQ_RECEIVE(),
 *     AW_MSGQ_SEND(), AW_MSGQ_TERMINATE()
 * \sa aw_msgq_receive(), aw_msgq_send(), aw_msgq_terminate()
 * \hideinitializer
 */
#define AW_MSGQ_IMPORT(msgq)     AW_PSP_MSGQ_IMPORT(msgq)

/**
 * \brief ��Ϣ���г�ʼ��
 *
 * ��ʼ���� AW_MSGQ_DECL() �� AW_MSGQ_DECL_STATIC() ��̬�������Ϣ���У�
 * ��Ƕ�뵽��һ�������ݶ����е���Ϣ���С�
 *
 * ��Ϣ���е�ѡ������������ڴ���Ϣ���е�������Ŷӷ�ʽ�����԰����������ȼ�
 * ���Ƚ��ȳ��ķ�ʽ�Ŷӣ����Ƿֱ��Ӧ #AW_MSGQ_Q_PRIORITY �� #AW_MSGQ_Q_FIFO ��
 *
 * \param[in]  msgq     ��Ϣ����ʵ��,�� AW_MSGQ_DECL()��
 *                      AW_MSGQ_DECL_STATIC() ����
 * \param[in]  msg_num  ��Ϣ����, ��Ϣ��������С = \a msg_num * \a msg_size
 * \param[in]  msg_size ��Ϣ��С, ��Ϣ��������С = \a msg_num * \a msg_size
 * \param[in]  options  ��ʼ��ѡ���־, #AW_MSGQ_Q_PRIORITY �� #AW_MSGQ_Q_FIFO
 *
 * \return �ɹ��Ļ�������Ϣ���е�ID��ʧ���򷵻�NULL
 *
 * \sa AW_SEM_Q_FIFO, AW_SEM_Q_PRIORITY
 * \sa AW_MSGQ_DECL(), AW_MSGQ_DECL_STATIC(), AW_MSGQ_RECEIVE(), AW_MSGQ_SEND(),
 *     AW_MSGQ_TERMINATE()
 * \sa aw_msgq_receive(), aw_msgq_send(), aw_msgq_terminate()
 * \hideinitializer
 */
#define AW_MSGQ_INIT(msgq, msg_num, msg_size, options) \
            AW_PSP_MSGQ_INIT(msgq, msg_num, msg_size, options)

/**
 * \brief ����Ϣ�����л�ȡ��Ϣ
 *
 * �� \a msgq ��ָ������Ϣ���л�ȡ��Ϣ����ȡ����Ϣ�������� \a p_buf ��ָ���Ļ���
 * ����, �û������ĳ���Ϊ \a nbytes �������Ϣ�ĳ��ȴ��� \a nbytes, ����Ϣʣ��
 * �Ĳ��ֽ��ᱻ����(���᷵�ش���ָʾ)�����\a nbytes Ϊ0 ��/�� \a p_buf ΪNULL,
 * ������Ϣ���ᱻ������
 *
 * ���� \a timeout ָ���˵���Ϣ������û����Ϣʱ���ȴ����ٸ�ϵͳ��������ȡ��Ϣ��
 * \a timeout Ҳ����Ϊ���������ֵ:
 *
 *   - #AW_MSGQ_WAIT_FOREVER \n
 *     ��Զ���ᳬʱ�����ý��ᱻ������ֱ������Ϣ�����гɹ���ȡ��Ϣ��ͨ������ֵ��
 *     �жϵ��ý��: AW_OK -�ɹ���ȡ��Ϣ��-AW_ENXIO -�ڵȴ���ʱ����Ϣ���б���
 *     ֹ
 *
 *   - #AW_MSGQ_NO_WAIT \n
 *     �������أ������Ƿ�ɹ���ȡ����Ϣ��ͨ������ֵ���жϵ��ý��:
 *     AW_OK -�ɹ���ȡ��Ϣ; -AW_EAGAIN -δ��ȡ����Ϣ, �Ժ�����;
 *     -AW_ENXIO -�ڵȴ���ʱ����Ϣ���б���ֹ
 *
 * ���� #AW_MSGQ_WAIT_FOREVER �⣬�����ĸ�ֵ���ǷǷ��ġ���������Ҫ��ָ֤���ĳ�ʱ
 * ֵΪ������
 *
 * \param[in]   msgq     ��Ϣ����ʵ��,�� AW_MSGQ_DECL()��
 *                       AW_MSGQ_DECL_STATIC() ����
 * \param[in]   p_buf    ������Ϣ�Ļ�����
 * \param[in]   nbytes   ������Ϣ�Ļ������ĳ���
 * \param[in]   timeout  ���ȴ�ʱ��(��ʱʱ��)����λ��ϵͳ���ģ�ϵͳʱ�ӽ���Ƶ
 *                       ���ɺ��� aw_sys_clkrate_get()���, ��ʹ�� 
 *                       aw_ms_to_ticks()������ת��Ϊ������; #AW_MSGQ_NO_WAIT 
 *                       Ϊ�޵ȴ�;#AW_MSGQ_WAIT_FOREVER Ϊһֱ�ȴ�
 *
 * \retval   AW_OK      �ɹ�������Ϣ
 * \retval  -AW_EINVAL  ������Ч
 * \retval  -AW_EPERM   ������Ĳ���,���ж��е����˱�����
 * \retval  -AW_ETIME   ��ȡ��Ϣ��ʱ
 * \retval  -AW_ENXIO   �ڵȴ���ʱ��, ��Ϣ���б���ֹ
 * \retval  -AW_EAGAIN  ��ȡ��Ϣʧ��, �Ժ����� (\a timeout = #AW_MSGQ_NO_WAIT ʱ)
 *
 * \par ����
 * ���෶����ο� AW_MSGQ_DECL() �� AW_MSGQ_DECL_STATIC()
 * \code
 *  char buf[16];   // ��Ϣ������
 *  aw_err_t ret;   // ����ֵ
 *
 *  AW_MSGQ_RECEIVE(msgq, buf, 16, AW_MSGQ_WAIT_FOREVER);   // ���õȴ�
 *
 *  //��ʱʱ�� 500ms
 *  ret = AW_MSGQ_RECEIVE(msgq, buf, 16, aw_ms_to_ticks(500));
 *  if (ret == -AW_ETIME) {  // ��ȡ��Ϣ��ʱ
 *      //...
 *  }
 *
 *  ret = AW_MSGQ_RECEIVE(msgq, buf, 16, AW_MSGQ_NO_WAIT);  // �޵ȴ�
 *  if (ret == -AW_EAGAIN) { // δ��ȡ����Ϣ
 *      //...
 *  }
 * \endcode
 *
 * \sa AW_MSGQ_DECL(), AW_MSGQ_DECL_STATIC(), AW_MSGQ_INIT(), AW_MSGQ_SEND(),
 *     AW_MSGQ_TERMINATE()
 * \sa aw_msgq_receive(), aw_msgq_send(), aw_msgq_terminate()
 * \hideinitializer
 */
#define AW_MSGQ_RECEIVE(msgq, p_buf, nbytes, timeout)   \
            AW_PSP_MSGQ_RECEIVE(msgq, p_buf, nbytes, timeout)

/**
 * \brief ������Ϣ����Ϣ����
 *
 * ���� \a p_buf �г���Ϊ \a nbytes ����Ϣ����Ϣ���� \a msgq ������������Ѿ���
 * �ȴ�������Ϣ������Ϣ����������Ͷ�ݵ���һ���ȴ���������û�������ڵȴ�������
 * Ϣ������Ϣ����������Ϣ�����С�
 *
 * ���� \a timeout ָ���˵���Ϣ������ʱ���ȴ����ٸ�ϵͳ������������Ϣ������Ϣ
 * ���С�\a timeout Ҳ����Ϊ���������ֵ:
 *
 *   - #AW_MSGQ_WAIT_FOREVER \n
 *     ��Զ���ᳬʱ�����ý��ᱻ������ֱ���ɹ�����Ϣ������Ϣ���С�ͨ������ֵ��
 *     �жϵ��ý��: AW_OK -�ɹ�������Ϣ��-AW_ENXIO -�ڵȴ���ʱ��
 *     ��Ϣ���б���ֹ
 *
 *   - #AW_MSGQ_NO_WAIT \n
 *     �������أ������Ƿ�ɹ����͵���Ϣ��ͨ������ֵ���жϵ��ý��:
 *     AW_OK -�ɹ�������Ϣ; -TIME -���ͳ�ʱ; -AW_ENXIO -�ڵȴ���ʱ��,
 *     ��Ϣ���б���ֹ
 *
 * ���� #AW_MSGQ_WAIT_FOREVER �⣬�����ĸ�ֵ���ǷǷ��ġ���������Ҫ��ָ֤���ĳ�ʱ
 * ֵΪ������
 *
 * ���� \a priority ָ������Ϣ�����͵����ȼ������ܵ�ֵ�У�
 *
 *  - #AW_MSGQ_PRI_NORMAL \n
 *    ��ͨ���ȼ�����Ϣ�����뵽��Ϣ���е�ĩβ
 *
 *  - #AW_MSGQ_PRI_URGENT \n
 *    �������ȼ�����Ϣ�����뵽��Ϣ���еĶ���
 *
 * \attention �������������жϷ�����(ISR)�е��ã�����ISR������֮��ͨ�ŵ���Ҫ
 * ��ʽ֮һ����ISR�е��õ�ʱ��\a timeout ��������Ϊ #AW_MSGQ_NO_WAIT ��
 *
 * \param[in] msgq     ��Ϣ����ʵ��, ��AW_MSGQ_DECL() ��
 *                     AW_MSGQ_DECL_STATIC() ����
 * \param[in] p_buf    ��Ϣ������
 * \param[in] nbytes   ��Ϣ����
 * \param[in] timeout  ���ȴ�ʱ��(��ʱʱ��)����λ��ϵͳ���ģ�ϵͳʱ�ӽ���Ƶ��
 *                     �ɺ��� aw_sys_clkrate_get()���, ��ʹ�� aw_ms_to_ticks()
 *                     ������ת��Ϊ������; #AW_MSGQ_NO_WAIT Ϊ�޵�
 *                     ��; #AW_MSGQ_WAIT_FOREVER Ϊһֱ�ȴ�
 * \param[in] priority ��Ϣ���͵����ȼ�:
 *                         - #AW_MSGQ_PRI_NORMAL -��ͨ���ȼ�
 *                         - #AW_MSGQ_PRI_URGENT -�������ȼ�
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 * \retval  -AW_ETIME  ������Ϣ��ʱ
 * \retval  -AW_ENXIO  �ڵȴ���ʱ��, ��Ϣ���б���ֹ
 * \retval  -AW_EAGAIN ������Ϣʧ��, �Ժ����� (\a timeout = #AW_MSGQ_NO_WAIT ʱ)
 *
 * \par ����
 * ��ο� AW_MSGQ_DECL() �� AW_MSGQ_RECEIVE()
 *
 * \sa AW_MSGQ_DECL(), AW_MSGQ_DECL_STATIC(), AW_MSGQ_INIT(), AW_MSGQ_RECEIVE(),
 *     AW_MSGQ_TERMINATE()
 * \sa aw_msgq_receive(), aw_msgq_receive(), aw_msgq_terminate()
 * \hideinitializer
 */
#define AW_MSGQ_SEND(msgq, p_buf, nbytes, timeout, priority) \
            AW_PSP_MSGQ_SEND(msgq, p_buf, nbytes, timeout, priority)

/**
 * \brief ��ֹ��Ϣ����
 *
 * ��ֹ��Ϣ���С��κεȴ�����Ϣ���е����񽫻�������������� -AW_ENXIO ���˺�����
 * ���ͷ���Ϣ������ص��κ��ڴ档
 *
 * \param[in] msgq  ��Ϣ����ʵ��,�� AW_MSGQ_DECL()�� AW_MSGQ_DECL_STATIC() ����
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 *
 * \par ����
 * ��ο� AW_MSGQ_DECL()
 *
 * \sa AW_MSGQ_DECL(), AW_MSGQ_DECL_STATIC(), AW_MSGQ_INIT(), AW_MSGQ_RECEIVE(),
 *     AW_MSGQ_SEND()
 * \sa aw_msgq_receive(), aw_msgq_send(), aw_msgq_terminate()
 * \hideinitializer
 */
#define AW_MSGQ_TERMINATE(msgq)  AW_PSP_MSGQ_TERMINATE(msgq)

/**
 * \brief ����Ƿ�Ϊ��Ч��Ϣ����
 *
 * \param[in] msgq  ��Ϣ����ʵ��,�� AW_MSGQ_DECL()�� AW_MSGQ_DECL_STATIC() ����
 *
 * \retval  AW_TRUE    ��Ч����
 * \retval  AW_FALSE   ��Ч����
 */
#define AW_MSGQ_VALID(msgq)         AW_PSP_MSGQ_VALID(msgq)

/**
 * \brief ��Ϣ��������Ϊ��Ч
 *
 * \param[in] msgq  ��Ϣ����ʵ��,�� AW_MSGQ_DECL()�� AW_MSGQ_DECL_STATIC() ����
 *
 * \return ��
 */
#define AW_MSGQ_SET_INVALID(msgq)   AW_PSP_MSGQ_SET_INVALID(msgq)

/**
 * \brief ����Ϣ�����л�ȡ��Ϣ
 *
 * �������Ĺ����� AW_MSGQ_RECEIVE()��ͬ,�������,������ʹ��\a msgq_id ��Ϊ������
 * \a msgq_id �� AW_MSGQ_INIT() ���ء�
 *
 * \param[in]   msgq_id  ��Ϣ����ID,�� AW_MSGQ_INIT() ����
 * \param[in]   p_buf    ������Ϣ�Ļ�����
 * \param[in]   nbytes   ������Ϣ�Ļ������ĳ���
 * \param[in]   timeout  ���ȴ�ʱ��(��ʱʱ��)����λ��ϵͳ���ģ�ϵͳʱ�ӽ���Ƶ
 *                       ���ɺ��� aw_sys_clkrate_get()���, ��ʹ�� 
 *                       aw_ms_to_ticks()������ת��Ϊ������; #AW_MSGQ_NO_WAIT Ϊ
 *                       �޵ȴ�;#AW_MSGQ_WAIT_FOREVER Ϊһֱ�ȴ�
 *
 * \retval   AW_OK      �ɹ�
 * \retval  -AW_EINVAL  ������Ч
 * \retval  -AW_EPERM   ������Ĳ���,���ж��е����˱�����
 * \retval  -AW_ETIME   ��ȡ��Ϣ��ʱ
 * \retval  -AW_ENXIO   �ڵȴ���ʱ��, ��Ϣ���б���ֹ
 * \retval  -AW_EAGAIN  ��ȡ��Ϣʧ��, �Ժ����� (\a timeout = #AW_MSGQ_NO_WAIT ʱ)
 *
 * \par ����
 * ��ο� AW_MSGQ_DECL() �� AW_MSGQ_DECL_STATIC()
 *
 * \sa AW_MSGQ_DECL(), AW_MSGQ_DECL_STATIC(), AW_MSGQ_INIT(), AW_MSGQ_RECEIVE(),
 *     AW_MSGQ_SEND(), AW_MSGQ_TERMINATE()
 * \sa aw_msgq_send(), aw_msgq_terminate()
 * \hideinitializer
 */
aw_err_t aw_msgq_receive (aw_msgq_id_t  msgq_id,
                          void         *p_buf,
                          size_t        nbytes,
                          aw_tick_t     timeout);

/**
 * \brief ������Ϣ����Ϣ����
 *
 * �������Ĺ����� AW_MSGQ_SEND() ��ͬ, �������, ������ʹ��\a msgq_id ��Ϊ������
 * \a msgq_id �� AW_MSGQ_INIT() ���ء�
 *
 * \param[in]   msgq_id   ��Ϣ����ID,�� #AW_MSGQ_INIT() ����
 * \param[in]   p_buf     ��Ϣ������
 * \param[in]   nbytes    ��Ϣ����
 * \param[in]   timeout   ���ȴ�ʱ��(��ʱʱ��)����λ��ϵͳ���ģ�ϵͳʱ�ӽ���Ƶ
 *                        ���ɺ��� aw_sys_clkrate_get()���, ��ʹ�� 
 *                        aw_ms_to_ticks()������ת��Ϊ������; #AW_MSGQ_NO_WAIT 
 *                        Ϊ�޵ȴ�;#AW_MSGQ_WAIT_FOREVER Ϊһֱ�ȴ�
 * \param[in]   priority  ��Ϣ���͵����ȼ�:
 *                            - #AW_MSGQ_PRI_NORMAL -��ͨ���ȼ�
 *                            - #AW_MSGQ_PRI_URGENT -�������ȼ�
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 * \retval  -AW_ETIME  ������Ϣ��ʱ
 * \retval  -AW_ENXIO  �ڵȴ���ʱ��, ��Ϣ���б���ֹ
 * \retval  -AW_EAGAIN ������Ϣʧ��, �Ժ����� (\a timeout = #AW_MSGQ_NO_WAIT ʱ)
 *
 * \par ����
 * ��ο� AW_MSGQ_DECL()
 *
 * \sa AW_MSGQ_DECL(), AW_MSGQ_DECL_STATIC(), AW_MSGQ_INIT(), AW_MSGQ_RECEIVE(),
 *     AW_MSGQ_SEND(), AW_MSGQ_TERMINATE()
 * \sa aw_msgq_receive(), aw_msgq_terminate()
 * \hideinitializer
 */
aw_err_t aw_msgq_send (aw_msgq_id_t  msgq_id,
                       aw_const void *p_buf,
                       size_t        nbytes,
                       int           timeout,
                       int           priority);

/**
 * \brief ��ֹ��Ϣ����
 *
 * ��ֹ��Ϣ���С��κεȴ�����Ϣ���е����񽫻�������������� -AW_ENXIO ���˺�����
 * ���ͷ���Ϣ������ص��κ��ڴ档
 *
 * \param[in] msgq_id ��Ϣ����ID,�� AW_MSGQ_INIT() ����
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 *
 * \par ����
 * ��ο� AW_MSGQ_DECL()
 *
 * \sa AW_MSGQ_DECL(), AW_MSGQ_DECL_STATIC(), AW_MSGQ_INIT(), AW_MSGQ_RECEIVE(),
 *     AW_MSGQ_SEND(), AW_MSGQ_TERMINATE()
 * \sa aw_msgq_receive(), aw_msgq_send()
 * \hideinitializer
 */
aw_err_t aw_msgq_terminate (aw_msgq_id_t msgq_id);

/**
 * \brief ����Ƿ�Ϊ��Ч��Ϣ����
 *
 * \param[in] msgq_id ��Ϣ����ID����AW_MSGQ_INIT()����
 *
 * \retval  AW_TRUE    ��Ч����
 * \retval  AW_FALSE   ��Ч����
 */
aw_bool_t aw_msgq_valid (aw_msgq_id_t msgq_id);

/** @} grp_aw_if_msgq */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_MSGQ_H */

/* end of file */
