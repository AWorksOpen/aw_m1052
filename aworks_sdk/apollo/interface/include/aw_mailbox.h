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
 * \brief ����ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_mailbox.h
 *
 * \par ����: �����÷�
 * \code
 *  #include "aworks.h"
 *  #include "aw_mailbox.h"
 *  #include "aw_delay.h"
 *  #include "aw_task.h"
 *  #include "aw_vdebug.h"
 *
 *  AW_TASK_DECL_STATIC(mb_task0, 512);
 *  AW_TASK_DECL_STATIC(mb_task1, 512);
 *
 *  AW_MAILBOX_DECL_STATIC(__g_mailbox, 10);       // �������䣺10���ʼ�
 *
 *  aw_local char __g_mb_str1[] = "I'm a mail!";
 *  aw_local char __g_mb_str2[] = "This is another mail!";
 *
 *  // �����ʼ�������ں���
 *  aw_local void __mb_task0_entry (void *p_arg)
 *  {
 *      unsigned char *p_str;
 *
 *      while (1) {
 *          if (AW_MAILBOX_RECV(__g_mailbox, 
 *                              &p_str, 
 *                              AW_MAILBOX_WAIT_FOREVER) == AW_OK) {
 *                   
 *
 *              aw_kprintf("mb_task0 recv a mail,the content is : %s\n", p_str);
 *          }
 *          aw_mdelay(10);
 *      }
 *  }
 *
 *  // �����ʼ�������ں���
 *  aw_local void __mb_task1_entry (void *p_arg)
 *  {
 *      uint32_t  count = 0;
 *      aw_err_t  ret   = AW_OK;
 *
 *      while (1) {
 *          count++;
 *
 *          if (count & 0x01) {
 *              ret = AW_MAILBOX_SEND(__g_mailbox,
 *                                    (uint32_t)__g_mb_str1,
 *                                     AW_MAILBOX_WAIT_FOREVER,
 *                                     AW_MAILBOX_PRI_NORMAL);
 *          } else {
 *              ret = AW_MAILBOX_SEND(__g_mailbox,
 *                                    (uint32_t)__g_mb_str2,
 *                                     AW_MAILBOX_WAIT_FOREVER,
 *                                     AW_MAILBOX_PRI_NORMAL);
 *          }
 *
 *          if (AW_OK == ret) {
 *               aw_kprintf("mb_task1 send a mail.\n");
 *          }
 *          aw_mdelay(1000); 
 *      }
 *  }
 *
 *  int aw_main (void)
 *  {
 *      // ��ʼ������
 *      AW_MAILBOX_INIT(__g_mailbox, 10, AW_MAILBOX_Q_PRIORITY);
 *
 *      // ��ʼ������mb_task0
 *      AW_TASK_INIT(mb_task0,                  // ����ʵ��
 *                   "mb_task0",                // ��������
 *                   2,                         // �������ȼ�
 *                   512,                       // �����ջ��С
 *                   __mb_task0_entry,          // ������ں���
 *                   NULL);                     // ������ڲ���
 *      // ��������mb_task0
 *      AW_TASK_STARTUP(mb_task0);
 *
 *      // ��ʼ������mb_task1
 *      AW_TASK_INIT(mb_task1,                  // ����ʵ��
 *                   "mb_task1",                // ��������
 *                   1,                         // �������ȼ�
 *                   512,                       // �����ջ��С
 *                   __mb_task1_entry,          // ������ں���
 *                   NULL);                     // ������ڲ���
 *      // ��������task0
 *      AW_TASK_STARTUP(mb_task1);
 *
 *      return 0;
 *  }
 *
 * \endcode
 *
 * \par ������������ʵ��Ƕ����һ�����ݽṹ
 * \code
 *  #include "aworks.h"
 *  #include "aw_mailbox.h"
 *  #include "aw_delay.h"
 *  #include "aw_task.h"
 *  #include "aw_vdebug.h"
 * 
 *  struct my_struct {
 *      int my_data1;
 *      AW_TASK_DECL(mb_task0, 512);           // ����ʵ��task_a, ��ջ512
 *      AW_TASK_DECL(mb_task1, 512);           // ����ʵ��task_b, ��ջ512
 *      AW_MAILBOX_DECL(mailbox, 10);          // �������䣺10���ʼ�
 *      aw_mailbox_id_t mailbox_id;            // ����ID
 *  };
 *  aw_local struct my_struct   __g_my_object; // ���������ݶ���
 *
 *  aw_local char __g_mb_str1[] = "I'm a mail!";
 *  aw_local char __g_mb_str2[] = "This is another mail!";
 *
 *  // �����ʼ�������ں���
 *  aw_local void __mb_task0_entry (void *p_arg)
 *  {
 *      unsigned char    *p_str;
 *      struct my_struct *p_myobj = (struct my_struct *)p_arg;
 *
 *      while (1) {
 *          if (aw_mailbox_recv(p_myobj->mailbox_id, 
 *                              &p_str, 
 *                              AW_MAILBOX_WAIT_FOREVER) == AW_OK) {
 *                   
 *
 *              aw_kprintf("mb_task0 recv a mail, the content is :%s\n", p_str);
 *          }
 *          aw_mdelay(10);
 *      }
 *  }
 *
 *  // �����ʼ�������ں���
 *  aw_local void __mb_task1_entry (void *p_arg)
 *  {
 *      uint32_t          count   = 0;
 *      aw_err_t          ret     = AW_OK;
 *      struct my_struct *p_myobj = (struct my_struct *)p_arg;
 *
 *      while (1) {
 *          count++;
 *
 *          if (count & 0x01) {
 *              ret = aw_mailbox_send(p_myobj->mailbox_id,
 *                                    (uint32_t)__g_mb_str1,
 *                                     AW_MAILBOX_WAIT_FOREVER,
 *                                     AW_MAILBOX_PRI_NORMAL);
 *          } else {
 *              ret = aw_mailbox_send(p_myobj->mailbox_id,
 *                                    (uint32_t)__g_mb_str2,
 *                                     AW_MAILBOX_WAIT_FOREVER,
 *                                     AW_MAILBOX_PRI_NORMAL);
 *          }
 *
 *          if (AW_OK == ret) {
 *               aw_kprintf("mb_task1 send a mail.\n");
 *          }
 *          aw_mdelay(1000);
 *      }
 *  }
 *
 *  int aw_main (void)
 *  {
 *      // ��ʼ������
 *      __g_my_object.mailbox_id = AW_MAILBOX_INIT(__g_my_object.mailbox, 
 *                                             10, 
 *                                             AW_PSP_MAILBOX_Q_PRIORITY);
 *
 *      if (NULL == __g_my_object.mailbox_id) {
 *            AW_ERRF(("AW_MAILBOX_INIT err!\n"));
 *            return 0;
 *      }
 *
 *      // ��ʼ������mb_task0
 *      AW_TASK_INIT(__g_my_object.mb_task0,    // ����ʵ��
 *                   "mb_task0",                // ��������
 *                   2,                         // �������ȼ�
 *                   512,                       // �����ջ��С
 *                   __mb_task0_entry,          // ������ں���
 *                   (void *)&__g_my_object);   // ������ڲ���
 *      // ��������mb_task0
 *      AW_TASK_STARTUP(__g_my_object.mb_task0);
 *
 *      // ��ʼ������mb_task1
 *      AW_TASK_INIT(__g_my_object.mb_task1,    // ����ʵ��
 *                   "mb_task1",                // ��������
 *                   1,                         // �������ȼ�
 *                   512,                       // �����ջ��С
 *                   __mb_task1_entry,          // ������ں���
 *                   (void *)&__g_my_object);   // ������ڲ���
 *      // ��������task0
 *      AW_TASK_STARTUP(__g_my_object.mb_task1);
 *
 *      return 0;
 *  }
 *
 * \endcode
 *
 * // �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history
 * - 1.10 17-09-12  tee, modified the AW_MAILBOX_INIT() 
 * - 1.01 17-09-06  anu, refine the description
 * - 1.00 17-04-01  may, create file
 * @endinternal
 */
#ifndef __AW_MAILBOX_H
#define __AW_MAILBOX_H

/**
 * \addtogroup grp_aw_if_mailbox
 * \copydoc aw_mailbox.h
 * @{
 */

#include "aw_common.h"
#include "aw_psp_mailbox.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name ��ʱ��������
 * @{
 */
/**
 * \brief ���õȴ�
 * \hideinitializer
 */
#define AW_MAILBOX_WAIT_FOREVER     AW_PSP_MAILBOX_WAIT_FOREVER

/**
 * \brief �޵ȴ�
 * \hideinitializer
 */
#define AW_MAILBOX_NO_WAIT          AW_PSP_MAILBOX_NO_WAIT
/** @} ��ʱ�������� */
    
    
/**
 * \name �����ʼ��ѡ��
 * @{
 */
/**
 * \brief �Ŷӷ�ʽΪ FIFO (�Ƚ��ȳ�)
 * \hideinitializer
 */
#define AW_MAILBOX_Q_FIFO            AW_PSP_MAILBOX_Q_FIFO

/**
 * \brief �Ŷӷ�ʽΪ���ȼ�
 * \hideinitializer
 */
#define AW_MAILBOX_Q_PRIORITY        AW_PSP_MAILBOX_Q_PRIORITY
/** @} �����ʼ��ѡ�� */
    
/**
 * \name �ʼ��������ȼ�ѡ��
 * @{
 */
/**
 * \brief ��ͨ���ȼ�
 * \hideinitializer
 */
#define AW_MAILBOX_PRI_NORMAL        AW_PSP_MAILBOX_RPI_NORMAL

/**
 * \brief �������ȼ�
 * \hideinitializer
 */
#define AW_MAILBOX_PRI_URGENT        AW_PSP_MAILBOX_PRI_URGENT
/** @} �ʼ��������ȼ�ѡ�� */
    
/**
 * \brief �������Ͷ���
 */
typedef aw_psp_mailbox_id_t aw_mailbox_id_t;
 
/**
 * \brief ��������
 *
 * �������ڶ���һ������ʵ�壬�ڱ���ʱ������������ڴ�ķ���(�����ʼ�������);
 * ������ʱ����Ҫ���� AW_MAILBOX_INIT() �������ĳ�ʼ��������
 * AW_MAILBOX_TERMINATE() ��ֹ���䡣
 *
 * ���� \a mailbox ֻ����Ϊ AW_MAILBOX_INIT(), AW_MAILBOX_SEND(), 
 * AW_MAILBOX_RECV() �� AW_MAILBOX_TERMINATE() �Ĳ�����
 * 
 * AW_MAILBOX_INIT() ��������� \a mailbox_id, \a mailbox_id ��Ϊ���� 
 * aw_mailbox_send(), aw_mailbox_recv() �� aw_mailbox_terminate()�Ĳ���; 
 * ��Щ�����Ĺ������Ӧ�ĺ�Ĺ�����ͬ, ��������, ����ʹ�� \a mailbox_id ��Ϊ
 * ����, \a mailbox_id ������Ϊ����ľ���ں�������֮�䴫�ݡ�
 *
 * ����ʹ�ñ��꽫����ʵ��Ƕ�뵽��һ�����ݽṹ��, �����Ļ�, �����Ǹ��������
 * �����ʱ����Զ����������ʵ����ڴ���䣬������ķ�����ʾ��
 *
 * \attention ������û�п�ģ��(�ļ�)ʹ�õ������Ƽ�ʹ��
 *            AW_MAILBOX_DECL_STATIC() ���汾�꣬�����ɱ���ģ��֮����������
 *            ��Ⱦ��
 *
 * \attention �������÷���, ��Ī�ں�����ʹ�ô˺�����������ʵ�壬��Ϊ�ں�����
 *            �˺������ڴ�������ջ�ϣ�һ���������أ�������ڴ汻�Զ����ա�
 *            �ں����ڲ�ʹ�� AW_MAILBOX_DECL_STATIC()����������
 *
 * \param[in] mailbox   ����ʵ��, �� AW_MAILBOX_INIT(), AW_MAILBOX_SEND(),
 *                      AW_MAILBOX_RECV() �� AW_MAILBOX_TERMINATE() һ��ʹ�á�
 *
 * \param[in] mail_num  �����������洢�ʼ��������Ŀ, ����ÿ���ʼ��Ĵ�С�̶�Ϊ
 *                      4�ֽڣ���ˣ��ʼ���������С = \a mail_num * \a 4
 *
 * \par ����: �����÷�
 * \code
 *  #include "aworks.h"
 *  #include "aw_mailbox.h"
 *  #include "aw_delay.h"
 *  #include "aw_task.h"
 *  #include "aw_vdebug.h"
 *
 *  AW_TASK_DECL_STATIC(mb_task0, 512);
 *  AW_TASK_DECL_STATIC(mb_task1, 512);
 *
 *  AW_MAILBOX_DECL_STATIC(__g_mailbox, 10);       // �������䣺10���ʼ�
 *
 *  aw_local char __g_mb_str1[] = "I'm a mail!";
 *  aw_local char __g_mb_str2[] = "This is another mail!";
 *
 *  // �����ʼ�������ں���
 *  aw_local void __mb_task0_entry (void *p_arg)
 *  {
 *      unsigned char *p_str;
 *
 *      while (1) {
 *          if (AW_MAILBOX_RECV(__g_mailbox, 
 *                              &p_str, 
 *                              AW_MAILBOX_WAIT_FOREVER) == AW_OK) {
 *                   
 *
 *              aw_kprintf("mb_task0 recv a mail,the content is : %s\n", p_str);
 *          }
 *          aw_mdelay(10);
 *      }
 *  }
 *
 *  // �����ʼ�������ں���
 *  aw_local void __mb_task1_entry (void *p_arg)
 *  {
 *      uint32_t  count = 0;
 *      aw_err_t  ret   = AW_OK;
 *
 *      while (1) {
 *          count++;
 *
 *          if (count & 0x01) {
 *              ret = AW_MAILBOX_SEND(__g_mailbox,
 *                                    (uint32_t)__g_mb_str1,
 *                                     AW_MAILBOX_WAIT_FOREVER,
 *                                     AW_MAILBOX_PRI_NORMAL);
 *          } else {
 *              ret = AW_MAILBOX_SEND(__g_mailbox,
 *                                    (uint32_t)__g_mb_str2,
 *                                     AW_MAILBOX_WAIT_FOREVER,
 *                                     AW_MAILBOX_PRI_NORMAL);
 *          }
 *
 *          if (AW_OK == ret) {
 *               aw_kprintf("mb_task1 send a mail.\n");
 *          }
 *          aw_mdelay(1000); 
 *      }
 *  }
 *
 *  int aw_main (void)
 *  {
 *      // ��ʼ������
 *      AW_MAILBOX_INIT(__g_mailbox, 10, AW_MAILBOX_Q_PRIORITY);
 *
 *      // ��ʼ������mb_task0
 *      AW_TASK_INIT(mb_task0,                  // ����ʵ��
 *                   "mb_task0",                // ��������
 *                   2,                         // �������ȼ�
 *                   512,                       // �����ջ��С
 *                   __mb_task0_entry,          // ������ں���
 *                   NULL);                     // ������ڲ���
 *      // ��������mb_task0
 *      AW_TASK_STARTUP(mb_task0);
 *
 *      // ��ʼ������mb_task1
 *      AW_TASK_INIT(mb_task1,                  // ����ʵ��
 *                   "mb_task1",                // ��������
 *                   1,                         // �������ȼ�
 *                   512,                       // �����ջ��С
 *                   __mb_task1_entry,          // ������ں���
 *                   NULL);                     // ������ڲ���
 *      // ��������task0
 *      AW_TASK_STARTUP(mb_task1);
 *
 *      return 0;
 *  }
 *
 * \endcode
 *
 * \par ������������ʵ��Ƕ����һ�����ݽṹ
 * \code
 *  #include "aworks.h"
 *  #include "aw_mailbox.h"
 *  #include "aw_delay.h"
 *  #include "aw_task.h"
 *  #include "aw_vdebug.h"
 * 
 *  struct my_struct {
 *      int my_data1;
 *      AW_TASK_DECL(mb_task0, 512);           // ����ʵ��task_a, ��ջ512
 *      AW_TASK_DECL(mb_task1, 512);           // ����ʵ��task_b, ��ջ512
 *      AW_MAILBOX_DECL(mailbox, 10);          // �������䣺10���ʼ�
 *      aw_mailbox_id_t mailbox_id;            // ����ID
 *  };
 *  aw_local struct my_struct   __g_my_object; // ���������ݶ���
 *
 *  aw_local char __g_mb_str1[] = "I'm a mail!";
 *  aw_local char __g_mb_str2[] = "This is another mail!";
 *
 *  // �����ʼ�������ں���
 *  aw_local void __mb_task0_entry (void *p_arg)
 *  {
 *      unsigned char    *p_str;
 *      struct my_struct *p_myobj = (struct my_struct *)p_arg;
 *
 *      while (1) {
 *          if (aw_mailbox_recv(p_myobj->mailbox_id, 
 *                              &p_str, 
 *                              AW_MAILBOX_WAIT_FOREVER) == AW_OK) {
 *                   
 *
 *              aw_kprintf("mb_task0 recv a mail, the content is :%s\n", p_str);
 *          }
 *          aw_mdelay(10);
 *      }
 *  }
 *
 *  // �����ʼ�������ں���
 *  aw_local void __mb_task1_entry (void *p_arg)
 *  {
 *      uint32_t          count   = 0;
 *      aw_err_t          ret     = AW_OK;
 *      struct my_struct *p_myobj = (struct my_struct *)p_arg;
 *
 *      while (1) {
 *          count++;
 *
 *          if (count & 0x01) {
 *              ret = aw_mailbox_send(p_myobj->mailbox_id,
 *                                    (uint32_t)__g_mb_str1,
 *                                     AW_MAILBOX_WAIT_FOREVER,
 *                                     AW_MAILBOX_PRI_NORMAL);
 *          } else {
 *              ret = aw_mailbox_send(p_myobj->mailbox_id,
 *                                    (uint32_t)__g_mb_str2,
 *                                     AW_MAILBOX_WAIT_FOREVER,
 *                                     AW_MAILBOX_PRI_NORMAL);
 *          }
 *
 *          if (AW_OK == ret) {
 *               aw_kprintf("mb_task1 send a mail.\n");
 *          }
 *          aw_mdelay(1000);
 *      }
 *  }
 *
 *  int aw_main (void)
 *  {
 *      // ��ʼ������
 *      __g_my_object.mailbox_id = AW_MAILBOX_INIT(__g_my_object.mailbox, 
 *                                             10, 
 *                                             AW_PSP_MAILBOX_Q_PRIORITY);
 *      if (NULL == __g_my_object.mailbox_id) {
 *            AW_ERRF(("AW_MAILBOX_INIT err!\n"));
 *            return 0;
 *      }
 *
 *      // ��ʼ������mb_task0
 *      AW_TASK_INIT(__g_my_object.mb_task0,    // ����ʵ��
 *                   "mb_task0",                // ��������
 *                   2,                         // �������ȼ�
 *                   512,                       // �����ջ��С
 *                   __mb_task0_entry,          // ������ں���
 *                   (void *)&__g_my_object);       // ������ڲ���
 *      // ��������mb_task0
 *      AW_TASK_STARTUP(__g_my_object.mb_task0);
 *
 *      // ��ʼ������mb_task1
 *      AW_TASK_INIT(__g_my_object.mb_task1,    // ����ʵ��
 *                   "mb_task1",                // ��������
 *                   1,                         // �������ȼ�
 *                   512,                       // �����ջ��С
 *                   __mb_task1_entry,          // ������ں���
 *                   (void *)&__g_my_object);   // ������ڲ���
 *      // ��������task0
 *      AW_TASK_STARTUP(__g_my_object.mb_task1);
 *
 *      return 0;
 *  }
 *
 * \endcode
 *
 * \sa AW_MAILBOX_DECL_STATIC(), AW_MAILBOX_INIT(), AW_MAILBOX_RECV(), 
 *     AW_MAILBOX_SEND(), AW_MAILBOX_TERMINATE()
 * \sa aw_mailbox_recv(), aw_mailobox_send(), aw_mailbox_terminate()
 * \hideinitializer
 */
#define AW_MAILBOX_DECL(mailbox, mail_num) \
           AW_PSP_MAILBOX_DECL(mailbox, mail_num)

/**
 * \brief ��������ʵ��(��̬)
 *
 * ������ AW_MAILBOX_DECL() �Ĺ�����ͬ���������ڣ� AW_MAILBOX_DECL_STATIC() ��
 * �������������ڴ�ʱ��ʹ�ùؼ��� \b static �����һ��������Խ�����ʵ�������
 * ��������ģ����(�ļ���), �Ӷ�����ģ��֮�������������ͻ��
 * �������ں�����ʹ�ñ��궨�����䡣
 *
 * \param[in] mailbox  ����ʵ��, �� AW_MAILBOX_INIT(), AW_MAILBOX_SEND(),
 *                     AW_MAILBOX_RECV() �� AW_MAILBOX_TERMINATE() һ��ʹ�á�
 *
 * \param[in] mail_num  �����������洢�ʼ��������Ŀ, ����ÿ���ʼ��Ĵ�С�̶�Ϊ
 *                      4�ֽڣ���ˣ��ʼ���������С = \a mail_num * \a 4
 *
 * \par ����
 * �� AW_MAILBOX_DECL()
 *
 * \sa AW_MAILBOX_DECL(), AW_MAILBOX_INIT(), AW_MAILBOX_RECV(), 
 *     AW_MAILBOX_SEND(), AW_MAILBOX_TERMINATE()
 * \sa aw_mailbox_recv(), aw_mailbox_send(), aw_mailbox_terminate()
 * \hideinitializer
 */
#define AW_MAILBOX_DECL_STATIC(mailbox, mail_num) \
            AW_PSP_MAILBOX_DECL_STATIC(mailbox, mail_num)

/**
 * \brief ��ʼ������
 *
 * ��ʼ���� AW_MAILBOX_DECL() �� AW_MAILBOX_DECL_STATIC() ��̬��������䣬
 * ��Ƕ�뵽��һ�������ݶ����е����䡣
 *
 * �����ѡ������������ڴ������������Ŷӷ�ʽ�����԰����������ȼ�
 * ���Ƚ��ȳ��ķ�ʽ�Ŷӣ����Ƿֱ��Ӧ #AW_MAILBOX_Q_PRIORITY �� 
 * #AW_MAILBOX_Q_FIFO ��
 *
 * \param[in] mailbox   ����ʵ��
 *                      �� AW_MAILBOX_DECL() �� AW_MAILBOX_DECL_STATIC()����
 *
 * \param[in] mail_num  �����������洢�ʼ��������Ŀ, ��ֵ�����붨������ʵ��
 *                      ʱָ������������һ��
 *
 * \param[in] options  ��ʼ��ѡ���־
 *                      - #AW_MAILBOX_Q_PRIORITY �ȴ���������������ȼ��Ŷ�
 *                      - #AW_MAILBOX_Q_FIFO     �ȴ�������������Ƚ��ȳ��Ŷ�
 *
 * \return �ɹ��Ļ����������ID��ʧ���򷵻�NULL
 *
 * \sa AW_MAILBOX_Q_FIFO, AW_MAILBOX_Q_PRIORITY
 * \sa AW_MAILBOX_DECL(), AW_MAILBOX_DECL_STATIC(), AW_MAILBOX_RECV(),
 *     AW_MAILBOX_SEND(), AW_MAILBOX_TERMINATE()
 * \sa aw_mailbox_recv(), aw_mailbox_send(), aw_mailbox_terminate()
 * \hideinitializer
 */
#define AW_MAILBOX_INIT(mailbox, mail_num, options) \
           AW_PSP_MAILBOX_INIT(mailbox, mail_num, options)

/**
 * \brief ������Ϣ��4�ֽ����ݣ�������
 *
 * �������� data �ĳ��ȹ̶�Ϊ4�ֽڡ�����������Ѿ��ڵȴ�������Ϣ�������Ϣ
 * ����������Ͷ�ݵ���һ���ȴ���������û�������ڵȴ�������Ϣ������Ϣ������
 * �������С�
 *
 * ���� \a timeout ָ���˵�������ʱ���ȴ����ٸ�ϵͳ������������Ϣ��������
 * \a timeout Ҳ����Ϊ���������ֵ:
 *
 *   - #AW_MAILBOX_WAIT_FOREVER \n
 *     ��Զ���ᳬʱ�����ý��ᱻ������ֱ���ɹ�����Ϣ�������䡣ͨ������ֵ���ж�
 *     ���ý��: AW_OK -�ɹ�������Ϣ��-AW_ENXIO -�ڵȴ���ʱ�����䱻��ֹ��
 *     
 *   - #AW_MAILBOX_NO_WAIT \n
 *     �������أ������Ƿ�ɹ����͵���Ϣ��ͨ������ֵ���жϵ��ý��:
 *     AW_OK -�ɹ�������Ϣ; -AW_EAGAIN -����ʧ�ܣ������������Ժ�����; 
 *
 * ���� #AW_MAILBOX_WAIT_FOREVER �⣬�����ĸ�ֵ���ǷǷ��ġ���������Ҫ��ָ֤��
 * �ĳ�ʱֵΪ������
 *
 * ���� \a priority ָ������Ϣ�����͵����ȼ������ܵ�ֵ�У�
 *
 *  - #AW_MAILBOX_PRI_NORMAL \n
 *    ��ͨ���ȼ�����Ϣ��˳�򱻷��뵽�����ĩβ
 *
 *  - #AW_MAILBOX_PRI_URGENT \n
 *    �������ȼ�����Ϣ�����뵽����Ŀ�ͷ����һ�λ�ȡ��Ϣʱ�����õ�����Ϣ
 *
 * \attention �������������жϷ�����(ISR)�е��ã�����ISR������֮��ͨ�ŵ���Ҫ
 * ��ʽ֮һ����ISR�е��õ�ʱ��\a timeout ��������Ϊ #AW_MAILBOX_NO_WAIT ��
 *
 * \param[in] mailbox   ����ʵ��
 *                      �� AW_MAILBOX_DECL() �� AW_MAILBOX_DECL_STATIC()����
 *
 * \param[in] data     ��Ϣ��32λ����
 *
 * \param[in] timeout  ���ȴ�ʱ��(��ʱʱ��)����λ��ϵͳ���ģ�ϵͳʱ�ӽ���Ƶ��
 *                     �ɺ��� aw_sys_clkrate_get()���, ��ʹ�� aw_ms_to_ticks()
 *                     ������ת��Ϊ������; #AW_MAILBOX_NO_WAIT Ϊ�޵�
 *                     ��; #AW_MAILBOX_WAIT_FOREVER Ϊһֱ�ȴ�
 *
 * \param[in] priority ��Ϣ���͵����ȼ�:
 *                      - #AW_MAILBOX_PRI_NORMAL -��ͨ���ȼ�
 *                      - #AW_MAILBOX_PRI_URGENT -�������ȼ�
 *
 * \retval  AW_OK      �ɹ�
 * \retval  -AW_EINVAL ������Ч
 * \retval  -AW_ETIME  ������Ϣ��ʱ
 * \retval  -AW_ENXIO  �ڵȴ���ʱ��, ���䱻��ֹ
 * \retval  -AW_EAGAIN ������Ϣʧ��, �Ժ����� 
 *                  (\a timeout= #AW_MAILBOX_NO_WAIT ʱ)
 *
 * \par ����
 * ��ο� AW_MAILBOX_DECL()
 *
 * \sa AW_MAILBOX_DECL(), AW_MAILBOX_DECL_STATIC(), AW_MAILBOX_INIT(), 
 *     AW_MAILBOX_RECV(), AW_MAILBOX_TERMINATE()
 * \sa aw_mailbox_recv(), aw_mailbox_send(), aw_mailbox_terminate()
 * \hideinitializer
 */
#define AW_MAILBOX_SEND(mailbox, data, timeout, priority) \
            AW_PSP_MAILBOX_SEND(mailbox, data, timeout, priority)

/**
 * \brief �������л�ȡ��Ϣ
 *
 * �� \a mailbox ��ָ���������л�ȡ��Ϣ����ȡ����Ϣ�������� \a p_data ��ָ���
 * ��������, �û������ĳ���Ϊ 4�ֽڣ����Ա����ȡ����32λ������ ����� p_data 
 * ΪNULL, ������Ϣ���ᱻ������
 *
 * ���� \a timeout ָ���˵�������û����Ϣʱ���ȴ����ٸ�ϵͳ��������ȡ��Ϣ��
 * \a timeout Ҳ����Ϊ���������ֵ:
 *
 *   - #AW_MAILBOX_WAIT_FOREVER \n
 *     ��Զ���ᳬʱ�����ý��ᱻ������ֱ���������гɹ���ȡ��Ϣ��ͨ������ֵ���ж�
 *     ���ý��: AW_OK -�ɹ���ȡ��Ϣ��-AW_ENXIO -�ڵȴ���ʱ�����䱻��ֹ
 *
 *   - #AW_MAILBOX_NO_WAIT \n
 *     �������أ������Ƿ�ɹ���ȡ����Ϣ��ͨ������ֵ���жϵ��ý��:
 *     AW_OK -�ɹ���ȡ��Ϣ; -AW_EAGAIN -δ��ȡ����Ϣ, �Ժ�����; 
 *
 * ���� #AW_MAILBOX_WAIT_FOREVER �⣬�����ĸ�ֵ���ǷǷ��ġ���������Ҫ��ָ֤��
 * �ĳ�ʱֵΪ������
 *
 * \param[in] mailbox   ����ʵ��
 *                      �� AW_MAILBOX_DECL() �� AW_MAILBOX_DECL_STATIC()����
 *
 * \param[in] p_data    ����32λ���ݣ�������һ����ַ����Ϣ�Ļ�����
 *
 * \param[in] timeout  ���ȴ�ʱ��(��ʱʱ��)����λ��ϵͳ���ģ�ϵͳʱ�ӽ���Ƶ��
 *                     �ɺ��� aw_sys_clkrate_get()���, ��ʹ�� aw_ms_to_ticks()
 *                     ������ת��Ϊ������; #AW_MAILBOX_NO_WAIT Ϊ�޵ȴ�;
 *                     #AW_MAILBOX_WAIT_FOREVER Ϊһֱ�ȴ�
 *
 * \retval  AW_OK       �ɹ�������Ϣ
 * \retval  -AW_EINVAL  ������Ч
 * \retval  -AW_EPERM   ������Ĳ���,���ж��е����˱�����
 * \retval  -AW_ETIME   ��ȡ��Ϣ��ʱ
 * \retval  -AW_ENXIO   �ڵȴ���ʱ��,���䱻��ֹ
 * \retval  -AW_EAGAIN  ��ȡ��Ϣʧ��,�Ժ�����(\a timeout=#AW_MAILBOX_NO_WAIT ʱ)
 *
 * \par ����
 * ���෶����ο� AW_MAILBOX_DECL() 
 * \code
 *  aw_err_t ret;   // ����ֵ
 *  uint32_t data;
 *
 *  AW_MAILBOX_RECV(mailbox, &data, AW_MAILBOX_WAIT_FOREVER);    // ���õȴ�
 *
 *  //��ʱʱ�� 500ms
 *  ret = AW_MAILBOX_RECV(mailbox, &data, aw_ms_to_ticks(500));
 *  if (ret == -ETIME) {      // ��ȡ��Ϣ��ʱ
 *      //...
 *  }
 *
 *  ret = AW_MAILBOX_RECV(mailbox, &data, AW_MAILBOX_NO_WAIT);   // �޵ȴ�
 *  if (ret == -AW_EAGAIN) {  // δ��ȡ����Ϣ
 *      //...
 *  }
 * \endcode
 *
 * \sa AW_MAILBOX_DECL(), AW_MAILBOX_DECL_STATIC(), AW_MAILBOX_INIT(), 
 *     AW_MAILBOX_SEND(), AW_MAILBOX_TERMINATE()
 * \sa aw_mailbox_recv(), aw_mailbox_send(), aw_mailbox_terminate()
 * \hideinitializer
 */
#define AW_MAILBOX_RECV(mailbox, p_data, timeout) \
            AW_PSP_MAILBOX_RECV(mailbox, p_data, timeout)

/**
 * \brief ��ֹ����
 *
 * ��ֹ���䡣�κεȴ�����������񽫻�������������� -AW_ENXIO ���˺�������
 * �ͷ�������ص��κ��ڴ档
 *
 * \param[in] mailbox  ����ʵ��,�� AW_MAILBOX_DECL()�� AW_MAILBOX_DECL_STATIC() ����
 *
 * \retval  AW_OK      �ɹ�
 * \retval  -AW_EINVAL ������Ч
 *
 * \par ����
 * ��ο� AW_MAILBOX_DECL()
 *
 * \sa AW_MAILBOX_DECL(), AW_MAILBOX_DECL_STATIC(), AW_MAILBOX_INIT(), 
 *     AW_MAILBOX_RECV(), AW_MAILBOX_SEND()
 * \sa aw_mailbox_recv(), aw_mailbox_send(), aw_mailbox_terminate()
 * \hideinitializer
 */
#define AW_MAILBOX_TERMINATE(mailbox) \
            AW_PSP_MAILBOX_TERMINATE(mailbox)

/**
 * \brief ������д��32λ���������ַ
 *
 * �������Ĺ����� AW_MAILBOX_SEND() ��ͬ, �������, ������ʹ��\a mailbox_id ��Ϊ
 * ������\a mailbox_id �� AW_MAILBOX_INIT() ���ء�
 *
 * \param[in] mailbox_id  ����ID,�� #AW_MAILBOX_INIT() ����
 * \param[in] data        �����͵���Ϣ��32λ����
 *
 * \param[in] timeout     ���ȴ�ʱ��(��ʱʱ��)����λ��ϵͳ����
 *                        ϵͳʱ�ӽ���Ƶ���ɺ��� aw_sys_clkrate_get()���,
 *                        ��ʹ�� aw_ms_to_ticks()������ת��Ϊ������;
 *                        - #AW_MAILBOX_NO_WAIT Ϊ�޵ȴ�;
 *                        - #AW_MAILBOX_WAIT_FOREVER Ϊһֱ�ȴ�
 *
 * \param[in] priority   �ʼ����͵����ȼ�:
 *                      - #AW_MAILBOX_PRI_NORMAL -��ͨ���ȼ�
 *                      - #AW_MAILBOX_PRI_URGENT -�������ȼ�
 *                 
 * \retval  AW_OK      �ɹ�
 * \retval  -AW_EINVAL ������Ч
 * \retval  -AW_ETIME  ������Ϣ��ʱ
 * \retval  -AW_ENXIO  �ڵȴ���ʱ��, ���䱻��ֹ
 * \retval  -AW_EAGAIN ������Ϣʧ��, �Ժ����� (\a timeout = #AW_MAILBOX_NO_WAIT ʱ)
 *
 * \par ����
 * ��ο� AW_MAILBOX_DECL()
 *
 * \sa AW_MAILBOX_DECL(), AW_MAILBOX_DECL_STATIC(), AW_MAILBOX_INIT(), 
 *     AW_MAILBOX_RECV(), AW_MAILBOX_SEND(), AW_MAILBOX_TERMINATE()
 * \sa aw_mailbox_recv(), aw_mailbox_terminate()
 * \hideinitializer
 */
aw_err_t aw_mailbox_send(aw_mailbox_id_t mailbox_id,
                         uint32_t        data,
                         int             timeout,
                         int             priority);

/**
 * \brief �������л�ȡ��Ϣ
 *
 * �������Ĺ����� AW_MAILBOX_RECV()��ͬ,�������,������ʹ��\a mailbox_id ��Ϊ
 * ������\a mailbox_id �� AW_MAILBOX_INIT() ���ء�
 *
 * \param[in] mailbox_id  ����ID,�� #AW_MAILBOX_INIT() ����
 * \param[in] p_data      �����ʼ��Ļ�������4�ֽ�����
 *
 * \param[in] timeout     ���ȴ�ʱ��(��ʱʱ��)����λ��ϵͳ����
 *                        ϵͳʱ�ӽ���Ƶ���ɺ��� aw_sys_clkrate_get()���,
 *                        ��ʹ�� aw_ms_to_ticks()������ת��Ϊ������;
 *                        - #AW_MAILBOX_NO_WAIT Ϊ�޵ȴ�;
 *                        - #AW_MAILBOX_WAIT_FOREVER Ϊһֱ�ȴ�
 *
 * \retval  AW_OK       �ɹ�
 * \retval  -AW_EINVAL  ������Ч
 * \retval  -AW_EPERM   ������Ĳ���,���ж��е����˱�����
 * \retval  -AW_ETIME   ��ȡ��Ϣ��ʱ
 * \retval  -AW_ENXIO   �ڵȴ���ʱ��, ���䱻��ֹ
 * \retval  -AW_EAGAIN  ��ȡ��Ϣʧ��, �Ժ�����(\a timeout = #AW_MAILBOX_NO_WAIT ʱ)
 *
 * \par ����
 * ��ο� AW_MAILBOX_DECL()
 *
 * \sa AW_MAILBOX_DECL(), AW_MAILBOX_DECL_STATIC(), AW_MAILBOX_INIT(), 
 *     AW_MAILBOX_RECV(), AW_MAILBOX_SEND(), AW_MAILBOX_TERMINATE()
 * \sa aw_mailbox_send(), aw_mailbox_terminate()
 * \hideinitializer
 */
aw_err_t aw_mailbox_recv(aw_mailbox_id_t mailbox_id, void *p_data, int timeout);
 
/**
 * \brief ��ֹ����
 *
 * ��ֹ���䡣�κεȴ�����������񽫻�������������� -AW_ENXIO ���˺�������
 * �ͷ�������ص��κ��ڴ档
 *
 * \param[in] mailbox_id  ����ID,�� #AW_MAILBOX_INIT() ����
 *
 * \retval  AW_OK       �ɹ�
 * \retval  -AW_EINVAL  ������Ч
 *
 * \par ����
 * ��ο� AW_MAILBOX_DECL()
 *
 * \sa AW_MAILBOX_DECL(), AW_MAILBOX_DECL_STATIC(), AW_MAILBOX_INIT(), 
 *     AW_MAILBOX_RECV(), AW_MAILBOX_SEND(), AW_MAILBOX_TERMINATE()
 * \sa aw_mailbox_send(), aw_mailbox_recv()
 * \hideinitializer
 */
aw_err_t aw_mailbox_terminate(aw_mailbox_id_t mailbox_id);

/** @} grp_aw_if_mailbox */

#ifdef __cplusplus
}
#endif

#endif /* __AW_MAILBOX_H */

 /* end of file */
