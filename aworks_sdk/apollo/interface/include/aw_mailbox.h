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
 * \brief 邮箱接口
 *
 * 使用本服务需要包含头文件 aw_mailbox.h
 *
 * \par 范例: 常规用法
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
 *  AW_MAILBOX_DECL_STATIC(__g_mailbox, 10);       // 定义邮箱：10封邮件
 *
 *  aw_local char __g_mb_str1[] = "I'm a mail!";
 *  aw_local char __g_mb_str2[] = "This is another mail!";
 *
 *  // 接收邮件任务入口函数
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
 *  // 发送邮件任务入口函数
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
 *      // 初始化邮箱
 *      AW_MAILBOX_INIT(__g_mailbox, 10, AW_MAILBOX_Q_PRIORITY);
 *
 *      // 初始化任务mb_task0
 *      AW_TASK_INIT(mb_task0,                  // 任务实体
 *                   "mb_task0",                // 任务名字
 *                   2,                         // 任务优先级
 *                   512,                       // 任务堆栈大小
 *                   __mb_task0_entry,          // 任务入口函数
 *                   NULL);                     // 任务入口参数
 *      // 启动任务mb_task0
 *      AW_TASK_STARTUP(mb_task0);
 *
 *      // 初始化任务mb_task1
 *      AW_TASK_INIT(mb_task1,                  // 任务实体
 *                   "mb_task1",                // 任务名字
 *                   1,                         // 任务优先级
 *                   512,                       // 任务堆栈大小
 *                   __mb_task1_entry,          // 任务入口函数
 *                   NULL);                     // 任务入口参数
 *      // 启动任务task0
 *      AW_TASK_STARTUP(mb_task1);
 *
 *      return 0;
 *  }
 *
 * \endcode
 *
 * \par 范例：将邮箱实体嵌入另一个数据结构
 * \code
 *  #include "aworks.h"
 *  #include "aw_mailbox.h"
 *  #include "aw_delay.h"
 *  #include "aw_task.h"
 *  #include "aw_vdebug.h"
 * 
 *  struct my_struct {
 *      int my_data1;
 *      AW_TASK_DECL(mb_task0, 512);           // 任务实体task_a, 堆栈512
 *      AW_TASK_DECL(mb_task1, 512);           // 任务实体task_b, 堆栈512
 *      AW_MAILBOX_DECL(mailbox, 10);          // 定义邮箱：10封邮件
 *      aw_mailbox_id_t mailbox_id;            // 邮箱ID
 *  };
 *  aw_local struct my_struct   __g_my_object; // 定义大的数据对象
 *
 *  aw_local char __g_mb_str1[] = "I'm a mail!";
 *  aw_local char __g_mb_str2[] = "This is another mail!";
 *
 *  // 接收邮件任务入口函数
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
 *  // 发送邮件任务入口函数
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
 *      // 初始化邮箱
 *      __g_my_object.mailbox_id = AW_MAILBOX_INIT(__g_my_object.mailbox, 
 *                                             10, 
 *                                             AW_PSP_MAILBOX_Q_PRIORITY);
 *
 *      if (NULL == __g_my_object.mailbox_id) {
 *            AW_ERRF(("AW_MAILBOX_INIT err!\n"));
 *            return 0;
 *      }
 *
 *      // 初始化任务mb_task0
 *      AW_TASK_INIT(__g_my_object.mb_task0,    // 任务实体
 *                   "mb_task0",                // 任务名字
 *                   2,                         // 任务优先级
 *                   512,                       // 任务堆栈大小
 *                   __mb_task0_entry,          // 任务入口函数
 *                   (void *)&__g_my_object);   // 任务入口参数
 *      // 启动任务mb_task0
 *      AW_TASK_STARTUP(__g_my_object.mb_task0);
 *
 *      // 初始化任务mb_task1
 *      AW_TASK_INIT(__g_my_object.mb_task1,    // 任务实体
 *                   "mb_task1",                // 任务名字
 *                   1,                         // 任务优先级
 *                   512,                       // 任务堆栈大小
 *                   __mb_task1_entry,          // 任务入口函数
 *                   (void *)&__g_my_object);   // 任务入口参数
 *      // 启动任务task0
 *      AW_TASK_STARTUP(__g_my_object.mb_task1);
 *
 *      return 0;
 *  }
 *
 * \endcode
 *
 * // 更多内容待添加。。。
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
 * \name 超时参数定义
 * @{
 */
/**
 * \brief 永久等待
 * \hideinitializer
 */
#define AW_MAILBOX_WAIT_FOREVER     AW_PSP_MAILBOX_WAIT_FOREVER

/**
 * \brief 无等待
 * \hideinitializer
 */
#define AW_MAILBOX_NO_WAIT          AW_PSP_MAILBOX_NO_WAIT
/** @} 超时参数定义 */
    
    
/**
 * \name 邮箱初始化选项
 * @{
 */
/**
 * \brief 排队方式为 FIFO (先进先出)
 * \hideinitializer
 */
#define AW_MAILBOX_Q_FIFO            AW_PSP_MAILBOX_Q_FIFO

/**
 * \brief 排队方式为优先级
 * \hideinitializer
 */
#define AW_MAILBOX_Q_PRIORITY        AW_PSP_MAILBOX_Q_PRIORITY
/** @} 邮箱初始化选项 */
    
/**
 * \name 邮件发送优先级选项
 * @{
 */
/**
 * \brief 普通优先级
 * \hideinitializer
 */
#define AW_MAILBOX_PRI_NORMAL        AW_PSP_MAILBOX_RPI_NORMAL

/**
 * \brief 紧急优先级
 * \hideinitializer
 */
#define AW_MAILBOX_PRI_URGENT        AW_PSP_MAILBOX_PRI_URGENT
/** @} 邮件发送优先级选项 */
    
/**
 * \brief 邮箱类型定义
 */
typedef aw_psp_mailbox_id_t aw_mailbox_id_t;
 
/**
 * \brief 定义邮箱
 *
 * 本宏用于定义一个邮箱实体，在编译时完成邮箱所需内存的分配(包括邮件缓冲区);
 * 在运行时，需要调用 AW_MAILBOX_INIT() 完成邮箱的初始化；调用
 * AW_MAILBOX_TERMINATE() 终止邮箱。
 *
 * 参数 \a mailbox 只能作为 AW_MAILBOX_INIT(), AW_MAILBOX_SEND(), 
 * AW_MAILBOX_RECV() 和 AW_MAILBOX_TERMINATE() 的参数。
 * 
 * AW_MAILBOX_INIT() 返回邮箱的 \a mailbox_id, \a mailbox_id 作为函数 
 * aw_mailbox_send(), aw_mailbox_recv() 和 aw_mailbox_terminate()的参数; 
 * 这些函数的功能与对应的宏的功能相同, 区别在于, 函数使用 \a mailbox_id 作为
 * 参数, \a mailbox_id 可以作为邮箱的句柄在函数调用之间传递。
 *
 * 可以使用本宏将邮箱实体嵌入到另一个数据结构中, 这样的话, 分配那个大的数据
 * 对象的时候就自动完成了邮箱实体的内存分配，如下面的范例所示。
 *
 * \attention 若邮箱没有跨模块(文件)使用的需求，推荐使用
 *            AW_MAILBOX_DECL_STATIC() 代替本宏，这样可避免模块之间邮箱命名
 *            污染。
 *
 * \attention 除特殊用法外, 切莫在函数内使用此宏来定义邮箱实体，因为在函数内
 *            此宏分配的内存来自于栈上，一旦函数返回，分配的内存被自动回收。
 *            在函数内部使用 AW_MAILBOX_DECL_STATIC()来定义任务。
 *
 * \param[in] mailbox   邮箱实体, 与 AW_MAILBOX_INIT(), AW_MAILBOX_SEND(),
 *                      AW_MAILBOX_RECV() 和 AW_MAILBOX_TERMINATE() 一起使用。
 *
 * \param[in] mail_num  邮箱容量，存储邮件的最大数目, 由于每个邮件的大小固定为
 *                      4字节，因此，邮件缓冲区大小 = \a mail_num * \a 4
 *
 * \par 范例: 常规用法
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
 *  AW_MAILBOX_DECL_STATIC(__g_mailbox, 10);       // 定义邮箱：10封邮件
 *
 *  aw_local char __g_mb_str1[] = "I'm a mail!";
 *  aw_local char __g_mb_str2[] = "This is another mail!";
 *
 *  // 接收邮件任务入口函数
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
 *  // 发送邮件任务入口函数
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
 *      // 初始化邮箱
 *      AW_MAILBOX_INIT(__g_mailbox, 10, AW_MAILBOX_Q_PRIORITY);
 *
 *      // 初始化任务mb_task0
 *      AW_TASK_INIT(mb_task0,                  // 任务实体
 *                   "mb_task0",                // 任务名字
 *                   2,                         // 任务优先级
 *                   512,                       // 任务堆栈大小
 *                   __mb_task0_entry,          // 任务入口函数
 *                   NULL);                     // 任务入口参数
 *      // 启动任务mb_task0
 *      AW_TASK_STARTUP(mb_task0);
 *
 *      // 初始化任务mb_task1
 *      AW_TASK_INIT(mb_task1,                  // 任务实体
 *                   "mb_task1",                // 任务名字
 *                   1,                         // 任务优先级
 *                   512,                       // 任务堆栈大小
 *                   __mb_task1_entry,          // 任务入口函数
 *                   NULL);                     // 任务入口参数
 *      // 启动任务task0
 *      AW_TASK_STARTUP(mb_task1);
 *
 *      return 0;
 *  }
 *
 * \endcode
 *
 * \par 范例：将邮箱实体嵌入另一个数据结构
 * \code
 *  #include "aworks.h"
 *  #include "aw_mailbox.h"
 *  #include "aw_delay.h"
 *  #include "aw_task.h"
 *  #include "aw_vdebug.h"
 * 
 *  struct my_struct {
 *      int my_data1;
 *      AW_TASK_DECL(mb_task0, 512);           // 任务实体task_a, 堆栈512
 *      AW_TASK_DECL(mb_task1, 512);           // 任务实体task_b, 堆栈512
 *      AW_MAILBOX_DECL(mailbox, 10);          // 定义邮箱：10封邮件
 *      aw_mailbox_id_t mailbox_id;            // 邮箱ID
 *  };
 *  aw_local struct my_struct   __g_my_object; // 定义大的数据对象
 *
 *  aw_local char __g_mb_str1[] = "I'm a mail!";
 *  aw_local char __g_mb_str2[] = "This is another mail!";
 *
 *  // 接收邮件任务入口函数
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
 *  // 发送邮件任务入口函数
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
 *      // 初始化邮箱
 *      __g_my_object.mailbox_id = AW_MAILBOX_INIT(__g_my_object.mailbox, 
 *                                             10, 
 *                                             AW_PSP_MAILBOX_Q_PRIORITY);
 *      if (NULL == __g_my_object.mailbox_id) {
 *            AW_ERRF(("AW_MAILBOX_INIT err!\n"));
 *            return 0;
 *      }
 *
 *      // 初始化任务mb_task0
 *      AW_TASK_INIT(__g_my_object.mb_task0,    // 任务实体
 *                   "mb_task0",                // 任务名字
 *                   2,                         // 任务优先级
 *                   512,                       // 任务堆栈大小
 *                   __mb_task0_entry,          // 任务入口函数
 *                   (void *)&__g_my_object);       // 任务入口参数
 *      // 启动任务mb_task0
 *      AW_TASK_STARTUP(__g_my_object.mb_task0);
 *
 *      // 初始化任务mb_task1
 *      AW_TASK_INIT(__g_my_object.mb_task1,    // 任务实体
 *                   "mb_task1",                // 任务名字
 *                   1,                         // 任务优先级
 *                   512,                       // 任务堆栈大小
 *                   __mb_task1_entry,          // 任务入口函数
 *                   (void *)&__g_my_object);   // 任务入口参数
 *      // 启动任务task0
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
 * \brief 定义邮箱实体(静态)
 *
 * 本宏与 AW_MAILBOX_DECL() 的功能相同，区别在于， AW_MAILBOX_DECL_STATIC() 在
 * 定义邮箱所需内存时，使用关键字 \b static ，如此一来，便可以将邮箱实体的作用
 * 域限制在模块内(文件内), 从而避免模块之间的邮箱命名冲突；
 * 还可以在函数内使用本宏定义邮箱。
 *
 * \param[in] mailbox  邮箱实体, 与 AW_MAILBOX_INIT(), AW_MAILBOX_SEND(),
 *                     AW_MAILBOX_RECV() 和 AW_MAILBOX_TERMINATE() 一起使用。
 *
 * \param[in] mail_num  邮箱容量，存储邮件的最大数目, 由于每个邮件的大小固定为
 *                      4字节，因此，邮件缓冲区大小 = \a mail_num * \a 4
 *
 * \par 范例
 * 见 AW_MAILBOX_DECL()
 *
 * \sa AW_MAILBOX_DECL(), AW_MAILBOX_INIT(), AW_MAILBOX_RECV(), 
 *     AW_MAILBOX_SEND(), AW_MAILBOX_TERMINATE()
 * \sa aw_mailbox_recv(), aw_mailbox_send(), aw_mailbox_terminate()
 * \hideinitializer
 */
#define AW_MAILBOX_DECL_STATIC(mailbox, mail_num) \
            AW_PSP_MAILBOX_DECL_STATIC(mailbox, mail_num)

/**
 * \brief 初始化邮箱
 *
 * 初始化由 AW_MAILBOX_DECL() 或 AW_MAILBOX_DECL_STATIC() 静态定义的邮箱，
 * 或嵌入到另一个大数据对象中的邮箱。
 *
 * 邮箱的选项决定了阻塞于此邮箱的任务的排队方式，可以按任务本身优先级
 * 或先进先出的方式排队，它们分别对应 #AW_MAILBOX_Q_PRIORITY 和 
 * #AW_MAILBOX_Q_FIFO 。
 *
 * \param[in] mailbox   邮箱实体
 *                      由 AW_MAILBOX_DECL() 或 AW_MAILBOX_DECL_STATIC()定义
 *
 * \param[in] mail_num  邮箱容量，存储邮件的最大数目, 该值必须与定义邮箱实体
 *                      时指定的邮箱容量一致
 *
 * \param[in] options  初始化选项标志
 *                      - #AW_MAILBOX_Q_PRIORITY 等待邮箱的任务按照优先级排队
 *                      - #AW_MAILBOX_Q_FIFO     等待邮箱的任务按照先进先出排队
 *
 * \return 成功的话返回邮箱的ID，失败则返回NULL
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
 * \brief 发送消息（4字节数据）到邮箱
 *
 * 发送数据 data 的长度固定为4字节。如果有任务已经在等待接收消息，则该消息
 * 将会立即被投递到第一个等待的任务。如没有任务在等待接收消息，则消息被保存
 * 在邮箱中。
 *
 * 参数 \a timeout 指定了当邮箱满时，等待多少个系统节拍数来将消息放入邮箱
 * \a timeout 也可以为下面的特殊值:
 *
 *   - #AW_MAILBOX_WAIT_FOREVER \n
 *     永远不会超时，调用将会被阻塞，直到成功将消息放入邮箱。通过返回值来判断
 *     调用结果: AW_OK -成功发送消息；-AW_ENXIO -在等待的时候，邮箱被终止。
 *     
 *   - #AW_MAILBOX_NO_WAIT \n
 *     立即返回，不管是否成功发送到消息。通过返回值来判断调用结果:
 *     AW_OK -成功发送消息; -AW_EAGAIN -发送失败，邮箱已满，稍后重试; 
 *
 * 除了 #AW_MAILBOX_WAIT_FOREVER 外，其它的负值都是非法的。调用者需要保证指定
 * 的超时值为正数。
 *
 * 参数 \a priority 指定了消息被发送的优先级，可能的值有：
 *
 *  - #AW_MAILBOX_PRI_NORMAL \n
 *    普通优先级，消息按顺序被放入到邮箱的末尾
 *
 *  - #AW_MAILBOX_PRI_URGENT \n
 *    紧急优先级，消息被放入到邮箱的开头，下一次获取消息时，将得到该消息
 *
 * \attention 本函数可以在中断服务函数(ISR)中调用，这是ISR和任务之间通信的主要
 * 方式之一。在ISR中调用的时候，\a timeout 参数必须为 #AW_MAILBOX_NO_WAIT 。
 *
 * \param[in] mailbox   邮箱实体
 *                      由 AW_MAILBOX_DECL() 或 AW_MAILBOX_DECL_STATIC()定义
 *
 * \param[in] data     消息，32位数据
 *
 * \param[in] timeout  最大等待时间(超时时间)，单位是系统节拍，系统时钟节拍频率
 *                     由函数 aw_sys_clkrate_get()获得, 可使用 aw_ms_to_ticks()
 *                     将毫秒转换为节拍数; #AW_MAILBOX_NO_WAIT 为无等
 *                     待; #AW_MAILBOX_WAIT_FOREVER 为一直等待
 *
 * \param[in] priority 消息发送的优先级:
 *                      - #AW_MAILBOX_PRI_NORMAL -普通优先级
 *                      - #AW_MAILBOX_PRI_URGENT -紧急优先级
 *
 * \retval  AW_OK      成功
 * \retval  -AW_EINVAL 参数无效
 * \retval  -AW_ETIME  发送消息超时
 * \retval  -AW_ENXIO  在等待的时候, 邮箱被终止
 * \retval  -AW_EAGAIN 发送消息失败, 稍后再试 
 *                  (\a timeout= #AW_MAILBOX_NO_WAIT 时)
 *
 * \par 范例
 * 请参考 AW_MAILBOX_DECL()
 *
 * \sa AW_MAILBOX_DECL(), AW_MAILBOX_DECL_STATIC(), AW_MAILBOX_INIT(), 
 *     AW_MAILBOX_RECV(), AW_MAILBOX_TERMINATE()
 * \sa aw_mailbox_recv(), aw_mailbox_send(), aw_mailbox_terminate()
 * \hideinitializer
 */
#define AW_MAILBOX_SEND(mailbox, data, timeout, priority) \
            AW_PSP_MAILBOX_SEND(mailbox, data, timeout, priority)

/**
 * \brief 从邮箱中获取消息
 *
 * 从 \a mailbox 所指定的邮箱中获取消息。获取的消息被拷贝到 \a p_data 所指向的
 * 缓冲区中, 该缓冲区的长度为 4字节，用以保存获取到的32位的数据 。如果 p_data 
 * 为NULL, 整个消息将会被丢弃。
 *
 * 参数 \a timeout 指定了当邮箱中没有消息时，等待多少个系统节拍来获取消息。
 * \a timeout 也可以为下面的特殊值:
 *
 *   - #AW_MAILBOX_WAIT_FOREVER \n
 *     永远不会超时，调用将会被阻塞，直到从邮箱中成功获取消息。通过返回值来判断
 *     调用结果: AW_OK -成功获取消息；-AW_ENXIO -在等待的时候，邮箱被终止
 *
 *   - #AW_MAILBOX_NO_WAIT \n
 *     立即返回，不管是否成功获取到消息。通过返回值来判断调用结果:
 *     AW_OK -成功获取消息; -AW_EAGAIN -未获取到消息, 稍后再试; 
 *
 * 除了 #AW_MAILBOX_WAIT_FOREVER 外，其它的负值都是非法的。调用者需要保证指定
 * 的超时值为正数。
 *
 * \param[in] mailbox   邮箱实体
 *                      由 AW_MAILBOX_DECL() 或 AW_MAILBOX_DECL_STATIC()定义
 *
 * \param[in] p_data    保存32位数据（整数或一个地址）消息的缓冲区
 *
 * \param[in] timeout  最大等待时间(超时时间)，单位是系统节拍，系统时钟节拍频率
 *                     由函数 aw_sys_clkrate_get()获得, 可使用 aw_ms_to_ticks()
 *                     将毫秒转换为节拍数; #AW_MAILBOX_NO_WAIT 为无等待;
 *                     #AW_MAILBOX_WAIT_FOREVER 为一直等待
 *
 * \retval  AW_OK       成功接收消息
 * \retval  -AW_EINVAL  参数无效
 * \retval  -AW_EPERM   不允许的操作,在中断中调用了本函数
 * \retval  -AW_ETIME   获取消息超时
 * \retval  -AW_ENXIO   在等待的时候,邮箱被终止
 * \retval  -AW_EAGAIN  获取消息失败,稍后再试(\a timeout=#AW_MAILBOX_NO_WAIT 时)
 *
 * \par 范例
 * 更多范例请参考 AW_MAILBOX_DECL() 
 * \code
 *  aw_err_t ret;   // 返回值
 *  uint32_t data;
 *
 *  AW_MAILBOX_RECV(mailbox, &data, AW_MAILBOX_WAIT_FOREVER);    // 永久等待
 *
 *  //超时时间 500ms
 *  ret = AW_MAILBOX_RECV(mailbox, &data, aw_ms_to_ticks(500));
 *  if (ret == -ETIME) {      // 获取消息超时
 *      //...
 *  }
 *
 *  ret = AW_MAILBOX_RECV(mailbox, &data, AW_MAILBOX_NO_WAIT);   // 无等待
 *  if (ret == -AW_EAGAIN) {  // 未获取到消息
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
 * \brief 终止邮箱
 *
 * 终止邮箱。任何等待此邮箱的任务将会解阻塞，并返回 -AW_ENXIO 。此函数不会
 * 释放邮箱相关的任何内存。
 *
 * \param[in] mailbox  邮箱实体,由 AW_MAILBOX_DECL()或 AW_MAILBOX_DECL_STATIC() 定义
 *
 * \retval  AW_OK      成功
 * \retval  -AW_EINVAL 参数无效
 *
 * \par 范例
 * 请参考 AW_MAILBOX_DECL()
 *
 * \sa AW_MAILBOX_DECL(), AW_MAILBOX_DECL_STATIC(), AW_MAILBOX_INIT(), 
 *     AW_MAILBOX_RECV(), AW_MAILBOX_SEND()
 * \sa aw_mailbox_recv(), aw_mailbox_send(), aw_mailbox_terminate()
 * \hideinitializer
 */
#define AW_MAILBOX_TERMINATE(mailbox) \
            AW_PSP_MAILBOX_TERMINATE(mailbox)

/**
 * \brief 向邮箱写入32位的整数或地址
 *
 * 本函数的功能与 AW_MAILBOX_SEND() 相同, 差别在于, 本函数使用\a mailbox_id 作为
 * 参数。\a mailbox_id 由 AW_MAILBOX_INIT() 返回。
 *
 * \param[in] mailbox_id  邮箱ID,由 #AW_MAILBOX_INIT() 返回
 * \param[in] data        待发送的消息：32位数据
 *
 * \param[in] timeout     最大等待时间(超时时间)，单位是系统节拍
 *                        系统时钟节拍频率由函数 aw_sys_clkrate_get()获得,
 *                        可使用 aw_ms_to_ticks()将毫秒转换为节拍数;
 *                        - #AW_MAILBOX_NO_WAIT 为无等待;
 *                        - #AW_MAILBOX_WAIT_FOREVER 为一直等待
 *
 * \param[in] priority   邮件发送的优先级:
 *                      - #AW_MAILBOX_PRI_NORMAL -普通优先级
 *                      - #AW_MAILBOX_PRI_URGENT -紧急优先级
 *                 
 * \retval  AW_OK      成功
 * \retval  -AW_EINVAL 参数无效
 * \retval  -AW_ETIME  发送消息超时
 * \retval  -AW_ENXIO  在等待的时候, 邮箱被终止
 * \retval  -AW_EAGAIN 发送消息失败, 稍后再试 (\a timeout = #AW_MAILBOX_NO_WAIT 时)
 *
 * \par 范例
 * 请参考 AW_MAILBOX_DECL()
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
 * \brief 从邮箱中获取消息
 *
 * 本函数的功能与 AW_MAILBOX_RECV()相同,差别在于,本函数使用\a mailbox_id 作为
 * 参数。\a mailbox_id 由 AW_MAILBOX_INIT() 返回。
 *
 * \param[in] mailbox_id  邮箱ID,由 #AW_MAILBOX_INIT() 返回
 * \param[in] p_data      保存邮件的缓冲区，4字节数据
 *
 * \param[in] timeout     最大等待时间(超时时间)，单位是系统节拍
 *                        系统时钟节拍频率由函数 aw_sys_clkrate_get()获得,
 *                        可使用 aw_ms_to_ticks()将毫秒转换为节拍数;
 *                        - #AW_MAILBOX_NO_WAIT 为无等待;
 *                        - #AW_MAILBOX_WAIT_FOREVER 为一直等待
 *
 * \retval  AW_OK       成功
 * \retval  -AW_EINVAL  参数无效
 * \retval  -AW_EPERM   不允许的操作,在中断中调用了本函数
 * \retval  -AW_ETIME   获取消息超时
 * \retval  -AW_ENXIO   在等待的时候, 邮箱被终止
 * \retval  -AW_EAGAIN  获取消息失败, 稍后再试(\a timeout = #AW_MAILBOX_NO_WAIT 时)
 *
 * \par 范例
 * 请参考 AW_MAILBOX_DECL()
 *
 * \sa AW_MAILBOX_DECL(), AW_MAILBOX_DECL_STATIC(), AW_MAILBOX_INIT(), 
 *     AW_MAILBOX_RECV(), AW_MAILBOX_SEND(), AW_MAILBOX_TERMINATE()
 * \sa aw_mailbox_send(), aw_mailbox_terminate()
 * \hideinitializer
 */
aw_err_t aw_mailbox_recv(aw_mailbox_id_t mailbox_id, void *p_data, int timeout);
 
/**
 * \brief 终止邮箱
 *
 * 终止邮箱。任何等待此邮箱的任务将会解阻塞，并返回 -AW_ENXIO 。此函数不会
 * 释放邮箱相关的任何内存。
 *
 * \param[in] mailbox_id  邮箱ID,由 #AW_MAILBOX_INIT() 返回
 *
 * \retval  AW_OK       成功
 * \retval  -AW_EINVAL  参数无效
 *
 * \par 范例
 * 请参考 AW_MAILBOX_DECL()
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
