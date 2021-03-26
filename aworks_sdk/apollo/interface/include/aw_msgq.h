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
 * \brief  AWorks 消息队列标准接口
 *
 * 使用本服务需要包含头文件 aw_msgq.h
 *
 * \par 范例: 常规用法
 * \code
 *  AW_TASK_DECL_STATIC(task_a, 512);
 *  AW_TASK_DECL_STATIC(task_b, 512);
 *
 *  AW_MSGQ_DECL_STATIC(msgq, 10, 16);  // 定义消息队列：10条消息, 每条16字节
 *
 *  void func_a (void *arg)
 *  {
 *      while (1) {
 *          // 发送消息 (任务中)
 *          AW_MSGQ_SEND(msgq,                  // 消息队列实体
 *                       "I'm task_a",          // 消息内容
 *                       11,                    // 消息的长度
 *                       AW_MSGQ_WAIT_FOREVER); // 永久的等待
 *
 *          aw_mdelay(50);
 *      }
 *  }
 *
 *  void isr (void)
 *  {
 *      aw_err_t ret;
 *
 *      // 发送消息 (ISR中)
 *      ret = AW_MSGQ_SEND(msgq,                // 消息队列实体
 *                         "I'm ISR",           // 消息内容
 *                         8,                   // 消息的长度
 *                         AW_MSGQ_NO_WAIT);    // 无等待(ISR中必须为此值)
 *
 *      if (ret == -AW_EAGAIN) {
 *          // 消息队列满，未能成功发送消息
 *      }
 *  }
 *
 *  void func_b (void *arg)
 *  {
 *      char buf[16];
 *      while (1) {
 *          // 接收消息
 *          AW_MSGQ_RECEIVE(msgq,                   // 消息队列实体
 *                          &buf[0],                // 消息缓冲区
 *                          16,                     // 最多能接收16个字节
 *                          AW_MSGQ_WAIT_FOREVER);  // 永久等待
 *
 *          aw_kprintf("%s", buf); 		            // 打印接收到的消息
 *      }
 *  }
 *
 *  int main()
 *  {
 *      // 初始化消息队列
 *      AW_MSGQ_INIT(msgq, AW_MSGQ_Q_PRIORITY);
 *
 *      // 初始化任务：
 *      AW_TASK_INIT(task_a,            // 任务实体
 *                   "task_a",          // 任务名字
 *                   5,                 // 任务优先级
 *                   512,               // 任务堆栈大小
 *                   func_a,            // 任务入口函数
 *                   NULL);             // 任务入口参数
 *
 *      AW_TASK_INIT(task_b,            // 任务实体
 *                   "task_b",          // 任务名字
 *                   6,                 // 任务优先级
 *                   512,               // 任务堆栈大小
 *                   func_b,            // 任务入口函数
 *                   NULL);             // 任务入口参数
 *
 *      // 启动任务
 *      AW_TASK_STARTUP(task_a);
 *      AW_TASK_STARTUP(task_b);
 *
 *      return 0;
 *  }
 * \endcode
 *
 * \par 范例：将消息队列实体嵌入另一个数据结构
 * \code
 *  #include "aw_msgq.h"
 *  #include "aw_vdebug.h"
 *
 *  struct my_struct {
 *      int my_data1;
 *      AW_TASK_DECL(task_a, 512);          // 任务实体task_a, 堆栈512
 *      AW_TASK_DECL(task_b, 512);          // 任务实体task_b, 堆栈512
 *      AW_MSGQ_DECL(msgq, 10, 16);         // 消息队列实体:10条消息，每条16字节
 *      aw_msgq_id_t msgq_id;               // 消息队列ID
 *  };
 *  struct my_struct   my_object;           // 定义大的数据对象
 *
 *  void func_a (void *arg)
 *  {
 *      struct my_struct *p_myobj = arg;
 *      while (1) {
 *          // 发送消息(使用消息队列实体)
 *          AW_MSGQ_SEND(p_myobj->msgq,     // 消息队列实体
 *                       "I'm task_a",      // 消息
 *                       11,                // 消息消息长度
 *                       AW_MSGQ_WAIT_FOREVER);
 *          aw_mdelay(50);
 *      }
 *  }
 *
 *  void func_b (void *arg)
 *  {
 *      struct my_struct *p_myobj = arg;
 *      while (1) {
 *          // 接收消息(使用消息队列ID)
 *          aw_msgq_receive(p_myobj->msgq_id,       // 消息队列实体
 *                          &buf[0],                // 消息缓冲区
 *                          16,                     // 最多能接收16个字节
 *                          AW_MSGQ_WAIT_FOREVER);  // 永久等待
 *
 *          aw_kprintf("%s", buf); 		            // 打印接收到的消息
 *      }
 *  }
 *
 *  int main() {
 *
 *      // 初始化消息队列
 *      my_object.msgq_id = AW_MSGQ_INIT(my_object.msgq, AW_MSGQ_Q_PRIORITY);
 *      if (my_object.msgq_id == NULL) {
 *          return -1;  // 消息队列初始化失败
 *      }
 *
 *      // 初始化任务：
 *      AW_TASK_INIT(my_object.task_a,      // 任务实体
 *                   "task_a",              // 任务名字
 *                   5,                     // 任务优先级
 *                   512,                   // 任务堆栈大小
 *                   func_a,                // 任务入口函数
 *                   (void *)&my_object);   // 任务入口参数
 *
 *      AW_TASK_INIT(my_object.task_b,      // 任务实体
 *                   "task_b",              // 任务名字
 *                   6,                     // 任务优先级
 *                   512,                   // 任务堆栈大小
 *                   func_b,                // 任务入口函数
 *                   (void *)&my_object);   // 任务入口参数
 *
 *      // 启动任务
 *      AW_TASK_STARTUP(my_object.task_a);
 *      AW_TASK_STARTUP(my_object.task_b);
 *
 *      aw_mdelay(20 * 1000);   //延时20秒
 *
 *      // AW_MSGQ_TERMINATE(my_object.msgq);   // 终止消息队列
 *      // aw_msgq_terminate(my_object.msgq_id);// 终止消息队列(使用消息队列ID)
 *
 *      return 0;
 *  }
 * \endcode
 *
 * // 更多内容待添加。。。
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
 * \name 超时参数定义
 * @{
 */
/**
 * \brief 永久等待
 * \hideinitializer
 */
#define AW_MSGQ_WAIT_FOREVER     AW_PSP_MSGQ_WAIT_FOREVER

/**
 * \brief 无等待
 * \hideinitializer
 */
#define AW_MSGQ_NO_WAIT         AW_PSP_MSGQ_NO_WAIT
/** @} 超时参数定义 */


/**
 * \name 消息队列初始化选项
 * @{
 */
/**
 * \brief 排队方式为 FIFO (先进先出)
 * \hideinitializer
 */
#define AW_MSGQ_Q_FIFO            AW_PSP_MSGQ_Q_FIFO

/**
 * \brief 排队方式为优先级
 * \hideinitializer
 */
#define AW_MSGQ_Q_PRIORITY        AW_PSP_MSGQ_Q_PRIORITY
/** @} 消息队列初始化选项 */

/**
 * \name 消息发送优先级选项
 * @{
 */
/**
 * \brief 普通优先级
 * \hideinitializer
 */
#define AW_MSGQ_PRI_NORMAL        AW_PSP_MSGQ_RPI_NORMAL

/**
 * \brief 紧急优先级
 * \hideinitializer
 */
#define AW_MSGQ_PRI_URGENT        AW_PSP_MSGQ_PRI_URGENT
/** @} 消息发送优先级选项 */

/**
 *  \name 消息队列类型定义
 *  @{
 */
typedef aw_psp_msgq_id_t   aw_msgq_id_t;   /**< 消息队列ID         */
/** @} 消息队列类型定义 */


/**
 * \brief 定义消息队列
 *
 * 本宏定义一个消息队列，在编译时完成消息队列所需内存的分配(包括消息缓冲区);
 * 在运行时，需要调用 AW_MSGQ_INIT() 完成消息队列的初始化；调用
 * AW_MSGQ_TERMINATE() 终止消息队列。
 *
 * 参数 \a msgq 只能作为 AW_MSGQ_INIT(), AW_MSGQ_SEND(), AW_MSGQ_RECEIVE() 和
 * AW_MSGQ_TERMINATE() 的参数。
 *
 * AW_MSGQ_INIT() 返回消息队列的 \a msgq_id, \a msgq_id 作为函数 aw_msgq_send(),
 * aw_msgq_receive() 和 aw_msgq_terminate()的参数; 这些函数的功能与对应的宏的
 * 功能相同, 区别在于, 函数使用 \a msgq_id 作为参数, \a msgq_id 可以作为消息队列
 * 的句柄在函数调用之间传递。
 *
 * 可以使用本宏将消息队列实体嵌入到另一个数据结构中, 这样的话, 分配那个大的数据
 * 对象的时候就自动完成了消息队列实体的内存分配，如下面的范例所示。
 *
 * \attention 若消息队列没有跨模块(文件)使用的需求，推荐使用
 *            AW_MSGQ_DECL_STATIC() 代替本宏，这样可避免模块之间消息队列命名
 *            污染。
 *
 * \attention 除特殊用法外不, 切莫在函数内使用此宏来定义任务，因为在函数内此宏分
 *            配的内存来自于栈上，一旦函数返回，分配的内存被自动回收。在函数内部
 *            使用 AW_TASK_DECL_STATIC()来定义任务。
 *
 * \param[in] msgq  消息队列实体, 与 AW_MSGQ_INIT(), AW_MSGQ_SEND(),
 *                  AW_MSGQ_RECEIVE() 和 AW_MSGQ_TERMINATE() 一起使用。
 *
 * \param[in] msg_num  消息条数, 消息缓冲区大小 = \a msg_num * \a msg_size
 * \param[in] msg_size 消息大小, 消息缓冲区大小 = \a msg_num * \a msg_size
 *
 * \par 范例：常规用法
 * \code
 *  AW_TASK_DECL_STATIC(task_a, 512);
 *  AW_TASK_DECL_STATIC(task_b, 512);
 *
 *  AW_MSGQ_DECL_STATIC(msgq, 10, 16);  // 定义消息队列：10条消息, 每条16字节
 *
 *  void func_a (void *arg)
 *  {
 *      while (1) {
 *          // 发送消息 (任务中)
 *          AW_MSGQ_SEND(msgq,                  // 消息队列实体
 *                       "I'm task_a",          // 消息内容
 *                       11,                    // 消息的长度
 *                       AW_MSGQ_WAIT_FOREVER); // 永久的等待
 *
 *          aw_mdelay(50);
 *      }
 *  }
 *
 *  void isr (void)
 *  {
 *      aw_err_t ret;
 *
 *      // 发送消息 (ISR中)
 *      ret = AW_MSGQ_SEND(msgq,                // 消息队列实体
 *                         "I'm ISR",           // 消息内容
 *                         8,                   // 消息的长度
 *                         AW_MSGQ_NO_WAIT);    // 无等待(ISR中必须为此值)
 *
 *      if (ret == -AW_EAGAIN) {
 *          // 消息队列满，未能成功发送消息
 *      }
 *  }
 *
 *  void func_b (void *arg)
 *  {
 *      char buf[16];
 *      while (1) {
 *          // 接收消息
 *          AW_MSGQ_RECEIVE(msgq,                   // 消息队列实体
 *                          &buf[0],                // 消息缓冲区
 *                          16,                     // 最多能接收16个字节
 *                          AW_MSGQ_WAIT_FOREVER);  // 永久等待
 *
 *          aw_kprintf("%s", buf); 		            // 打印接收到的消息
 *      }
 *  }
 *
 *  int main()
 *  {
 *      // 初始化消息队列
 *      AW_MSGQ_INIT(msgq, AW_MSGQ_Q_PRIORITY);
 *
 *      // 初始化任务：
 *      AW_TASK_INIT(task_a,            // 任务实体
 *                   "task_a",          // 任务名字
 *                   5,                 // 任务优先级
 *                   512,               // 任务堆栈大小
 *                   func_a,            // 任务入口函数
 *                   NULL);             // 任务入口参数
 *
 *      AW_TASK_INIT(task_b,            // 任务实体
 *                   "task_b",          // 任务名字
 *                   6,                 // 任务优先级
 *                   512,               // 任务堆栈大小
 *                   func_b,            // 任务入口函数
 *                   NULL);             // 任务入口参数
 *
 *      // 启动任务
 *      AW_TASK_STARTUP(task_a);
 *      AW_TASK_STARTUP(task_b);
 *
 *      return 0;
 *  }
 * \endcode
 *
 * \par 范例：将消息队列实体嵌入另一个数据结构
 * \code
 *  struct my_struct {
 *      int my_data1;
 *      AW_TASK_DECL(task_a, 512);          // 任务实体task_a, 堆栈512
 *      AW_TASK_DECL(task_b, 512);          // 任务实体task_b, 堆栈512
 *      AW_MSGQ_DECL(msgq, 10, 16);         // 消息队列实体:10条消息，每条16字节
 *      aw_msgq_id_t msgq_id;               // 消息队列ID
 *  };
 *  struct my_struct   my_object;           // 定义大的数据对象
 *
 *  void func_a (void *arg)
 *  {
 *      struct my_struct *p_myobj = arg;
 *      while (1) {
 *          // 发送消息(使用消息队列实体)
 *          AW_MSGQ_SEND(p_myobj->msgq,     // 消息队列实体
 *                       "I'm task_a",      // 消息
 *                       11,                // 消息消息长度
 *                       AW_MSGQ_WAIT_FOREVER);
 *          aw_mdelay(50);
 *      }
 *  }
 *
 *  void func_b (void *arg)
 *  {
 *      struct my_struct *p_myobj = arg;
 *      while (1) {
 *          // 接收消息(使用消息队列ID)
 *          aw_msgq_receive(p_myobj->msgq_id,       // 消息队列实体
 *                          &buf[0],                // 消息缓冲区
 *                          16,                     // 最多能接收16个字节
 *                          AW_MSGQ_WAIT_FOREVER);  // 永久等待
 *
 *          aw_kprintf("%s", buf); 		            // 打印接收到的消息
 *      }
 *  }
 *
 *  int main()
 *  {
 *      // 初始化消息队列
 *      my_object.msgq_id = AW_MSGQ_INIT(my_object.msgq, AW_MSGQ_Q_PRIORITY);
 *      if (my_object.msgq_id == NULL) {
 *          return -1;  // 消息队列初始化失败
 *      }
 *
 *      // 初始化任务：
 *      AW_TASK_INIT(my_object.task_a,      // 任务实体
 *                   "task_a",              // 任务名字
 *                   5,                     // 任务优先级
 *                   512,                   // 任务堆栈大小
 *                   func_a,                // 任务入口函数
 *                   (void *)&my_object);   // 任务入口参数
 *
 *      AW_TASK_INIT(my_object.task_b,      // 任务实体
 *                   "task_b",              // 任务名字
 *                   6,                     // 任务优先级
 *                   512,                   // 任务堆栈大小
 *                   func_b,                // 任务入口函数
 *                   (void *)&my_object);   // 任务入口参数
 *
 *      // 启动任务
 *      AW_TASK_STARTUP(my_object.task_a);
 *      AW_TASK_STARTUP(my_object.task_b);
 *
 *      aw_mdelay(20 * 1000);   //延时20秒
 *
 *      // AW_MSGQ_TERMINATE(my_object.msgq);    // 终止消息队列
 *      // aw_msgq_terminate(my_object.msgq_id); // 终止消息队列(使用消息队列ID)
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
 * \brief 定义消息队列(静态)
 *
 * 本宏与 AW_MSGQ_DECL() 的功能相同，区别在于， AW_MSGQ_DECL_STATIC() 在定义消
 * 息队列所需内存时，使用关键字 \b static ，如此一来，便可以将消息队列实体的作用
 * 域限制在模块内(文件内), 从而避免模块之间的消息队列命名冲突；
 * 还可以在函数内使用本宏定义消息队列。
 *
 * \param[in] msgq  消息队列实体, 与 AW_MSGQ_INIT(), AW_MSGQ_SEND(),
 *                  AW_MSGQ_RECEIVE() 和 AW_MSGQ_TERMINATE() 一起使用。
 *
 * \param[in] msg_num  消息条数, 消息缓冲区大小 = \a msg_num * \a msg_size
 * \param[in] msg_size 消息大小, 消息缓冲区大小 = \a msg_num * \a msg_size
 *
 * \par 范例
 * 见 AW_MSGQ_DECL()
 *
 * \sa AW_MSGQ_DECL(), AW_MSGQ_INIT(), AW_MSGQ_RECEIVE(), AW_MSGQ_SEND,
 *     AW_MSGQ_TERMINATE()
 * \sa aw_msgq_receive(), aw_msgq_send(), aw_msgq_terminate()
 * \hideinitializer
 */
#define AW_MSGQ_DECL_STATIC(msgq, msg_num, msg_size)   \
            AW_PSP_MSGQ_DECL_STATIC(msgq, msg_num, msg_size)

/**
 * \brief 引用消息队列
 *
 * 本宏引入一个消息队列，相当于使用关键 \b extern 声明一个外部符号。
 * 如果要使用另一个模块(文件)中定义的消息队列，首先使用本宏引入该消息队列，然后
 * 就可以操作该消息队列。被引入的消息队列应当是使用 AW_MSGQ_DECL()定义的。
 *
 * \param[in] msgq  消息队列
 *
 * \par 范例
 * \code
 *  #include "aw_msgq.h"
 *
 *  AW_MSGQ_IMPORT(msgq_x);         // 引入消息队列 msgq_x
 *
 *  int func () {
 *      AW_MSGQ_SEND(msgq_x);       // 向消息队列 msgq_x 发送消息
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
 * \brief 消息队列初始化
 *
 * 初始化由 AW_MSGQ_DECL() 或 AW_MSGQ_DECL_STATIC() 静态定义的消息队列，
 * 或嵌入到另一个大数据对象中的消息队列。
 *
 * 消息队列的选项决定了阻塞于此消息队列的任务的排队方式，可以按任务本身优先级
 * 或先进先出的方式排队，它们分别对应 #AW_MSGQ_Q_PRIORITY 和 #AW_MSGQ_Q_FIFO 。
 *
 * \param[in]  msgq     消息队列实体,由 AW_MSGQ_DECL()或
 *                      AW_MSGQ_DECL_STATIC() 定义
 * \param[in]  msg_num  消息条数, 消息缓冲区大小 = \a msg_num * \a msg_size
 * \param[in]  msg_size 消息大小, 消息缓冲区大小 = \a msg_num * \a msg_size
 * \param[in]  options  初始化选项标志, #AW_MSGQ_Q_PRIORITY 或 #AW_MSGQ_Q_FIFO
 *
 * \return 成功的话返回消息队列的ID，失败则返回NULL
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
 * \brief 从消息队列中获取消息
 *
 * 从 \a msgq 所指定的消息队中获取消息。获取的消息被拷贝到 \a p_buf 所指定的缓冲
 * 区中, 该缓冲区的长度为 \a nbytes 。如果消息的长度大于 \a nbytes, 则消息剩余
 * 的部分将会被丢弃(不会返回错误指示)。如果\a nbytes 为0 和/或 \a p_buf 为NULL,
 * 整个消息将会被丢弃。
 *
 * 参数 \a timeout 指定了当消息队列中没有消息时，等待多少个系统节拍来获取消息。
 * \a timeout 也可以为下面的特殊值:
 *
 *   - #AW_MSGQ_WAIT_FOREVER \n
 *     永远不会超时，调用将会被阻塞，直到从消息队列中成功获取消息。通过返回值来
 *     判断调用结果: AW_OK -成功获取消息；-AW_ENXIO -在等待的时候，消息队列被终
 *     止
 *
 *   - #AW_MSGQ_NO_WAIT \n
 *     立即返回，不管是否成功获取到消息。通过返回值来判断调用结果:
 *     AW_OK -成功获取消息; -AW_EAGAIN -未获取到消息, 稍后再试;
 *     -AW_ENXIO -在等待的时候，消息队列被终止
 *
 * 除了 #AW_MSGQ_WAIT_FOREVER 外，其它的负值都是非法的。调用者需要保证指定的超时
 * 值为正数。
 *
 * \param[in]   msgq     消息队列实体,由 AW_MSGQ_DECL()或
 *                       AW_MSGQ_DECL_STATIC() 定义
 * \param[in]   p_buf    保存消息的缓冲区
 * \param[in]   nbytes   保存消息的缓冲区的长度
 * \param[in]   timeout  最大等待时间(超时时间)，单位是系统节拍，系统时钟节拍频
 *                       率由函数 aw_sys_clkrate_get()获得, 可使用 
 *                       aw_ms_to_ticks()将毫秒转换为节拍数; #AW_MSGQ_NO_WAIT 
 *                       为无等待;#AW_MSGQ_WAIT_FOREVER 为一直等待
 *
 * \retval   AW_OK      成功接收消息
 * \retval  -AW_EINVAL  参数无效
 * \retval  -AW_EPERM   不允许的操作,在中断中调用了本函数
 * \retval  -AW_ETIME   获取消息超时
 * \retval  -AW_ENXIO   在等待的时候, 消息队列被终止
 * \retval  -AW_EAGAIN  获取消息失败, 稍后再试 (\a timeout = #AW_MSGQ_NO_WAIT 时)
 *
 * \par 范例
 * 更多范例请参考 AW_MSGQ_DECL() 和 AW_MSGQ_DECL_STATIC()
 * \code
 *  char buf[16];   // 消息缓冲区
 *  aw_err_t ret;   // 返回值
 *
 *  AW_MSGQ_RECEIVE(msgq, buf, 16, AW_MSGQ_WAIT_FOREVER);   // 永久等待
 *
 *  //超时时间 500ms
 *  ret = AW_MSGQ_RECEIVE(msgq, buf, 16, aw_ms_to_ticks(500));
 *  if (ret == -AW_ETIME) {  // 获取消息超时
 *      //...
 *  }
 *
 *  ret = AW_MSGQ_RECEIVE(msgq, buf, 16, AW_MSGQ_NO_WAIT);  // 无等待
 *  if (ret == -AW_EAGAIN) { // 未获取到消息
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
 * \brief 发送消息到消息队列
 *
 * 发送 \a p_buf 中长度为 \a nbytes 的消息到消息队列 \a msgq 。如果有任务已经在
 * 等待接收消息，则消息将会立即被投递到第一个等待的任务。如没有任务在等待接收消
 * 息，则消息被保存在消息队列中。
 *
 * 参数 \a timeout 指定了当消息队列满时，等待多少个系统节拍数来将消息放入消息
 * 队列。\a timeout 也可以为下面的特殊值:
 *
 *   - #AW_MSGQ_WAIT_FOREVER \n
 *     永远不会超时，调用将会被阻塞，直到成功将消息放入消息队列。通过返回值来
 *     判断调用结果: AW_OK -成功发送消息；-AW_ENXIO -在等待的时候，
 *     消息队列被终止
 *
 *   - #AW_MSGQ_NO_WAIT \n
 *     立即返回，不管是否成功发送到消息。通过返回值来判断调用结果:
 *     AW_OK -成功发送消息; -TIME -发送超时; -AW_ENXIO -在等待的时候,
 *     消息队列被终止
 *
 * 除了 #AW_MSGQ_WAIT_FOREVER 外，其它的负值都是非法的。调用者需要保证指定的超时
 * 值为正数。
 *
 * 参数 \a priority 指定了消息被发送的优先级，可能的值有：
 *
 *  - #AW_MSGQ_PRI_NORMAL \n
 *    普通优先级，消息被放入到消息队列的末尾
 *
 *  - #AW_MSGQ_PRI_URGENT \n
 *    紧急优先级，消息被放入到消息队列的队首
 *
 * \attention 本函数可以在中断服务函数(ISR)中调用，这是ISR和任务之间通信的主要
 * 方式之一。在ISR中调用的时候，\a timeout 参数必须为 #AW_MSGQ_NO_WAIT 。
 *
 * \param[in] msgq     消息队列实体, 由AW_MSGQ_DECL() 或
 *                     AW_MSGQ_DECL_STATIC() 定义
 * \param[in] p_buf    消息缓冲区
 * \param[in] nbytes   消息长度
 * \param[in] timeout  最大等待时间(超时时间)，单位是系统节拍，系统时钟节拍频率
 *                     由函数 aw_sys_clkrate_get()获得, 可使用 aw_ms_to_ticks()
 *                     将毫秒转换为节拍数; #AW_MSGQ_NO_WAIT 为无等
 *                     待; #AW_MSGQ_WAIT_FOREVER 为一直等待
 * \param[in] priority 消息发送的优先级:
 *                         - #AW_MSGQ_PRI_NORMAL -普通优先级
 *                         - #AW_MSGQ_PRI_URGENT -紧急优先级
 *
 * \retval   AW_OK     成功
 * \retval  -AW_EINVAL 参数无效
 * \retval  -AW_ETIME  发送消息超时
 * \retval  -AW_ENXIO  在等待的时候, 消息队列被终止
 * \retval  -AW_EAGAIN 发送消息失败, 稍后再试 (\a timeout = #AW_MSGQ_NO_WAIT 时)
 *
 * \par 范例
 * 请参考 AW_MSGQ_DECL() 和 AW_MSGQ_RECEIVE()
 *
 * \sa AW_MSGQ_DECL(), AW_MSGQ_DECL_STATIC(), AW_MSGQ_INIT(), AW_MSGQ_RECEIVE(),
 *     AW_MSGQ_TERMINATE()
 * \sa aw_msgq_receive(), aw_msgq_receive(), aw_msgq_terminate()
 * \hideinitializer
 */
#define AW_MSGQ_SEND(msgq, p_buf, nbytes, timeout, priority) \
            AW_PSP_MSGQ_SEND(msgq, p_buf, nbytes, timeout, priority)

/**
 * \brief 终止消息队列
 *
 * 终止消息队列。任何等待此消息队列的任务将会解阻塞，并返回 -AW_ENXIO 。此函数不
 * 会释放消息队列相关的任何内存。
 *
 * \param[in] msgq  消息队列实体,由 AW_MSGQ_DECL()或 AW_MSGQ_DECL_STATIC() 定义
 *
 * \retval   AW_OK     成功
 * \retval  -AW_EINVAL 参数无效
 *
 * \par 范例
 * 请参考 AW_MSGQ_DECL()
 *
 * \sa AW_MSGQ_DECL(), AW_MSGQ_DECL_STATIC(), AW_MSGQ_INIT(), AW_MSGQ_RECEIVE(),
 *     AW_MSGQ_SEND()
 * \sa aw_msgq_receive(), aw_msgq_send(), aw_msgq_terminate()
 * \hideinitializer
 */
#define AW_MSGQ_TERMINATE(msgq)  AW_PSP_MSGQ_TERMINATE(msgq)

/**
 * \brief 检查是否为有效消息队列
 *
 * \param[in] msgq  消息队列实体,由 AW_MSGQ_DECL()或 AW_MSGQ_DECL_STATIC() 定义
 *
 * \retval  AW_TRUE    有效队列
 * \retval  AW_FALSE   无效队列
 */
#define AW_MSGQ_VALID(msgq)         AW_PSP_MSGQ_VALID(msgq)

/**
 * \brief 消息队列设置为无效
 *
 * \param[in] msgq  消息队列实体,由 AW_MSGQ_DECL()或 AW_MSGQ_DECL_STATIC() 定义
 *
 * \return 无
 */
#define AW_MSGQ_SET_INVALID(msgq)   AW_PSP_MSGQ_SET_INVALID(msgq)

/**
 * \brief 从消息队列中获取消息
 *
 * 本函数的功能与 AW_MSGQ_RECEIVE()相同,差别在于,本函数使用\a msgq_id 作为参数。
 * \a msgq_id 由 AW_MSGQ_INIT() 返回。
 *
 * \param[in]   msgq_id  消息队列ID,由 AW_MSGQ_INIT() 返回
 * \param[in]   p_buf    保存消息的缓冲区
 * \param[in]   nbytes   保存消息的缓冲区的长度
 * \param[in]   timeout  最大等待时间(超时时间)，单位是系统节拍，系统时钟节拍频
 *                       率由函数 aw_sys_clkrate_get()获得, 可使用 
 *                       aw_ms_to_ticks()将毫秒转换为节拍数; #AW_MSGQ_NO_WAIT 为
 *                       无等待;#AW_MSGQ_WAIT_FOREVER 为一直等待
 *
 * \retval   AW_OK      成功
 * \retval  -AW_EINVAL  参数无效
 * \retval  -AW_EPERM   不允许的操作,在中断中调用了本函数
 * \retval  -AW_ETIME   获取消息超时
 * \retval  -AW_ENXIO   在等待的时候, 消息队列被终止
 * \retval  -AW_EAGAIN  获取消息失败, 稍后再试 (\a timeout = #AW_MSGQ_NO_WAIT 时)
 *
 * \par 范例
 * 请参考 AW_MSGQ_DECL() 和 AW_MSGQ_DECL_STATIC()
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
 * \brief 发送消息到消息队列
 *
 * 本函数的功能与 AW_MSGQ_SEND() 相同, 差别在于, 本函数使用\a msgq_id 作为参数。
 * \a msgq_id 由 AW_MSGQ_INIT() 返回。
 *
 * \param[in]   msgq_id   消息队列ID,由 #AW_MSGQ_INIT() 返回
 * \param[in]   p_buf     消息缓冲区
 * \param[in]   nbytes    消息长度
 * \param[in]   timeout   最大等待时间(超时时间)，单位是系统节拍，系统时钟节拍频
 *                        率由函数 aw_sys_clkrate_get()获得, 可使用 
 *                        aw_ms_to_ticks()将毫秒转换为节拍数; #AW_MSGQ_NO_WAIT 
 *                        为无等待;#AW_MSGQ_WAIT_FOREVER 为一直等待
 * \param[in]   priority  消息发送的优先级:
 *                            - #AW_MSGQ_PRI_NORMAL -普通优先级
 *                            - #AW_MSGQ_PRI_URGENT -紧急优先级
 *
 * \retval   AW_OK     成功
 * \retval  -AW_EINVAL 参数无效
 * \retval  -AW_ETIME  发送消息超时
 * \retval  -AW_ENXIO  在等待的时候, 消息队列被终止
 * \retval  -AW_EAGAIN 发送消息失败, 稍后再试 (\a timeout = #AW_MSGQ_NO_WAIT 时)
 *
 * \par 范例
 * 请参考 AW_MSGQ_DECL()
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
 * \brief 终止消息队列
 *
 * 终止消息队列。任何等待此消息队列的任务将会解阻塞，并返回 -AW_ENXIO 。此函数不
 * 会释放消息队列相关的任何内存。
 *
 * \param[in] msgq_id 消息队列ID,由 AW_MSGQ_INIT() 返回
 *
 * \retval   AW_OK     成功
 * \retval  -AW_EINVAL 参数无效
 *
 * \par 范例
 * 请参考 AW_MSGQ_DECL()
 *
 * \sa AW_MSGQ_DECL(), AW_MSGQ_DECL_STATIC(), AW_MSGQ_INIT(), AW_MSGQ_RECEIVE(),
 *     AW_MSGQ_SEND(), AW_MSGQ_TERMINATE()
 * \sa aw_msgq_receive(), aw_msgq_send()
 * \hideinitializer
 */
aw_err_t aw_msgq_terminate (aw_msgq_id_t msgq_id);

/**
 * \brief 检查是否为有效消息队列
 *
 * \param[in] msgq_id 消息队列ID，由AW_MSGQ_INIT()返回
 *
 * \retval  AW_TRUE    有效队列
 * \retval  AW_FALSE   无效队列
 */
aw_bool_t aw_msgq_valid (aw_msgq_id_t msgq_id);

/** @} grp_aw_if_msgq */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_MSGQ_H */

/* end of file */
