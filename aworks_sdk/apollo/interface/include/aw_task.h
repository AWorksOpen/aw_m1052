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
 * \brief  多任务接口
 *
 * 使用本服务需要包含头文件 aw_task.h
 *
 * \note 当前平台优先级个数为8, 最低优先级为7，最高优先级为0
 *
 * \par 简单示例
 * \code
 *  #include "aworks.h"
 *  #include "aw_task.h"
 *
 *  AW_TASK_DECL(my_task, 256);                     // 定义任务实体 my_task
 *
 *  // 任务入口函数
 *  void taska (void *p_arg)
 *  {
 *      volatile int *p_exit_task = (volatile int *)p_arg;
 *      while (!(*p_exit_task)) {
 *          // ...
 *          aw_task_delay(aw_sys_clkrate_get());    // 延时 1S
 *      }
 *  }
 *
 *  int main()
 *  {
 *      volatile int    exit_task = 0;              // 控制目标任务退出
 *      // 初始化任务：
 *      AW_TASK_INIT(my_task,                       // 任务实体
 *                   "taskname",                    // 任务名字
 *                   5,                             // 任务优先级
 *                   taska,                         // 任务入口函数
 *                   (void*)&exit_task);            // 任务入口参数
 *
 *      // 启动任务
 *      AW_TASK_STARTUP(my_task);
 *
 *      // 延时 20S.
 *      aw_task_delay(aw_sys_clkrate_get() * 20);
 *
 *      // 终止任务
 *      exit_task = 1;
 *      // 等待目标任务退出
 *      AW_TASK_JOIN(my_task,NULL);
 *
 *      return 0;
 *  }
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.11 17-09-05  anu, refine the description
 * - 1.10 13-02-28  zen, refine the description
 * - 1.00 12-10-23  liangyaozhan, first implementation
 * \endinternal
 */

#ifndef __AW_TASK_H
#define __AW_TASK_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_aw_if_task
 * \copydoc aw_task.h
 * @{
 */
 
/**
 * \brief 定义任务入口函数类型
 *
 * 任务函数类型是初始化或创建任务时，需求的入口函数指针类型
 * p_arg用于创建或初始化任务是传入的参数，
 * 返回类型为void *,任务入口函数可以通过返回某个值来表明自己的为什么退出
 * 这个退出码可以被join函数得到
 *
 */
typedef void * (*aw_task_func_t)(void *p_arg);

#include "aw_psp_task.h"

typedef aw_psp_task_id_t    aw_task_id_t;           /**< 任务ID     */
#define AW_TASK_ID_INVALID  AW_PSP_TASK_ID_INVALID  /**< 无效任务ID  */

/**
 * \brief 定义任务
 *
 * 本宏定义一个任务，在编译时完成任务所需内存的分配; 在运行时，需要分别调用
 * AW_TASK_INIT()和 AW_TASK_STARTUP()完成任务的初始化和启动；任务的退出可以
 * 从任务函数中返回来退出
 *
 * 参数 \a task 只能作为宏 AW_TASK_INIT()、 AW_TASK_STARTUP()、AW_TASK_JOIN()和
 * AW_TASK_TERMINATE()的参数。
 *
 * AW_TASK_INIT()返回任务的\a task_id,\a task_id 作为函数 aw_task_startup() 、
 * aw_task_terminate()的参数; 这些函数的功能与对应的宏的功能相同，区别在于，函数
 * 使用\a task_id 作为参数，\a task_id 可以作为任务的句柄在函数调用之间传递。
 *
 * 可以使用本宏将任务实体嵌入到另一个数据结构中，这样的话，分配大的数据对象的时
 * 候就自动完成了任务实体的内存分配，如下面的范例所示。
 *
 * \attention 若任务没有跨模块(文件)使用的需求，推荐使用 AW_TASK_DECL_STATIC()
 *            代替本宏，这样可避免模块之间任务命名污染。
 *
 * \attention 除特殊用法外不, 切莫在函数内使用此宏来定义任务，因为在函数内此宏分
 *            配的内存来自于栈上，一旦函数返回，分配的内存被自动回收。在函数内部
 *            使用 AW_TASK_DECL_STATIC()来定义任务。
 *
 * \param[in] task          任务实体，与 AW_TASK_INIT() 、 AW_TASK_STARTUP()和
 *                      AW_TASK_TERMINATE() 一起使用。
 *
 * \param[in] stack_size    任务堆栈大小，此参数必须和 AW_TASK_INIT() 一致!
 *
 * \par 范例：常规用法
 * \code
 *  #include "aw_task.h"
 *
 *  AW_TASK_DECL(my_task, 512);         // 定义任务实体 my_task
 *
 *  // 任务入口函数
 *  void taska (void *p_arg)
 *  {
 *      while (1) {
 *          // ...
 *          aw_task_delay(aw_sys_clkrate_get());    // 延时 1S
 *      }
 *  }
 *
 *  int main ()
 *  {
 *
 *      // 初始化任务：
 *      AW_TASK_INIT(my_task,       // 任务实体
 *                   "taskname",    // 任务名字
 *                   5,             // 任务优先级
 *                   512,           // 任务堆栈大小(和 AW_TASK_DECL()一致)
 *                   taska,         // 任务入口函数
 *                   (void*)1);     // 任务入口参数
 *
 *      // 启动任务
 *      AW_TASK_STARTUP(my_task);
 *
 *      // 延时 20S.
 *      aw_task_delay(aw_sys_clkrate_get() * 20);
 *
 *      // 终止任务
 *      AW_TASK_TERMINATE(my_task);
 *
 *      return 0;
 *  }
 * \endcode
 *
 * \par 范例：将任务实体嵌入另一个数据结构
 * \code
 *  struct my_struct {
 *      int my_data1;
 *      int my_data2;
 *      AW_TASK_DECL(my_task, 512);     // 任务实体my_task, 堆栈512
 *  };
 *  struct my_struct   my_object;       // 定义大的数据对象
 *
 *  int main ()
 *  {
 *
 *      //...
 *
 *      // 初始化任务：
 *      AW_TASK_INIT(my_object.my_task, // 任务实体
 *                   "taskname",        // 任务名字
 *                   5,                 // 任务优先级
 *                   512,               // 任务堆栈大小
 *                   taska,             // 任务入口函数
 *                   (void*)1);         // 任务入口参数
 *
 *      // 启动任务
 *      AW_TASK_STARTUP(my_object.my_task);
 *
 *      // 延时 20S.
 *      aw_task_delay(aw_sys_clkrate_get() * 20);
 *
 *      // 终止任务
 *      AW_TASK_TERMINATE(my_object.my_task);
 *
 *      return 0;
 *  }
 * \endcode
 *
 * \sa AW_TASK_DECL_STATIC(), AW_TASK_INIT(), AW_TASK_STARTUP(),
 *     AW_TASK_TERMINATE()
 * \hideinitializer
 */
#define AW_TASK_DECL(task, stack_size)  \
            AW_PSP_TASK_DECL(task, stack_size)

/**
 * \brief 定义任务(静态)
 *
 * 本宏与 AW_TASK_DECL() 的功能相同，区别在于，AW_TASK_DECL_STATIC() 在定义任务
 * 所需内存时，使用关键字 \b static ，如此一来，便可以将任务实体的作用域限制在
 * 模块内(文件内)从而避免模块之间的任务命名冲突;还可以在函数内使用本宏定义任务。
 *
 * \param[in] task          任务实体，与 AW_TASK_INIT() 、 AW_TASK_STARTUP()和
 *                          AW_TASK_TERMINATE() 一起使用。
 *
 * \param[in] stack_size    任务栈大小, 此参数必须和 AW_TASK_INIT() 一致!
 *
 * \par 范例
 * \code
 *  #include "aw_task.h"
 *
 *  // 任务入口函数
 *  void taska (void *p_arg)
 *  {
 *      while (1) {
 *          // ...
 *          aw_task_delay(aw_sys_clkrate_get());    // 延时 1S
 *      }
 *  }
 *
 *  int main ()
 *  {
 *
 *      // 定义任务实体 my_task
 *      AW_TASK_DECL_STATIC(my_task, 512);
 *
 *      // 初始化任务：
 *      AW_TASK_INIT(my_task,       // 任务实体
 *                   "taskname",    // 任务名字
 *                   5,             // 任务优先级
 *                   512,           // 任务堆栈大小
 *                   taska,         // 任务入口函数
 *                   (void*)1);     // 任务入口参数
 *
 *      // 启动任务
 *      AW_TASK_STARTUP(my_task);
 *
 *      // 延时 20S.
 *      aw_task_delay(aw_sys_clkrate_get() * 20);
 *
 *      // 终止任务
 *      AW_TASK_TERMINATE(my_task);
 *
 *      return 0;
 *  }
 *  \endcode
 *
 * \sa AW_TASK_DECL(), AW_TASK_INIT(), AW_TASK_STARTUP(), AW_TASK_TERMINATE()
 * \hideinitializer
 */
#define AW_TASK_DECL_STATIC(task, stack_size) \
            AW_PSP_TASK_DECL_STATIC(task, stack_size)

/**
 * \brief 引用任务
 *
 * 本宏引入一个任务，相当于使用关键 \b extern 声明一个外部符号。如果要使用另一个
 * 模块(文件)中定义的任务，首先使用本宏引入该任务，然后就可以操作该任务。被引入
 * 任务应当是使用 AW_TASK_DECL() 定义的。
 *
 * \param[in] task  任务实体
 *
 * \par 范例
 * \code
 *  #include "aw_task.h"
 *
 *  AW_TASK_IMPORT(his_task);    // 引入任务 his_task
 *
 *  int func ()
 *  {
 *      AW_TASK_TERMINATE(his_task); // 终止任务 his_task
 *  }
 *  \endcode
 *
 * \sa AW_TASK_DECL(), AW_TASK_INIT(), AW_TASK_STARTUP(), AW_TASK_TERMINATE()
 * \hideinitializer
 */
#define AW_TASK_IMPORT(task)    AW_PSP_TASK_IMPORT(task)

/**
 * \brief 初始化任务
 *
 * \param[in] task      任务实体，由 AW_TASK_DECL() 或 AW_TASK_DECL_STATIC() 定义
 * \param[in] name      任务名称
 * \param[in] priority  任务优先级, 0 优先级最高, 最低优先级可通过
 *                      aw_task_lowest_priority() 获取。
 *                      当前平台优先级个数为8, 最低优先级为7, 最高优先级为0。
 *
 * \param[in] stack_size    任务堆栈大小，此参数必须和 AW_TASK_DECL() 一致!
 * \param[in] func      入口函数
 * \param[in] arg       入口参数
 *
 * \return          成功返回任务的ID，失败则返回NULL
 *
 * \note 参数 \a priority 是用户任务的优先级。系统保留了若干优先级供内部系统服务
 * 和设备驱动使用。对于这些系统服务和设备驱动，使用宏 AW_TASK_SYS_PRIORITY(pri)
 * 表示系统任务优先级。
 *
 * \par 范例
 * 见 AW_TASK_DECL() 和 AW_TASK_DECL_STATIC()
 *
 * \sa AW_TASK_DECL(), AW_TASK_DECL_STATIC(), AW_TASK_INIT(), AW_TASK_STARTUP(),
 *  AW_TASK_TERMINATE()
 * \hideinitializer
 */
#define AW_TASK_INIT(task, name, priority, stack_size, func, arg)    \
            AW_PSP_TASK_INIT(task, name, priority, stack_size, func, arg)

/**
 * \brief 系统任务优先级
 *
 * 系统保留了若干优先级供内部系统服务和设备驱动使用,对于这些系统服务和设备驱动,
 * 凡是涉及优先级的API，都必须使用宏 AW_TASK_SYS_PRIORITY(pri) 来表示系统任务的
 * 优先级,\a pri 为 0 时优先级最高,\a pri 值越大优先级越低。
 * 所有系统任务优先级都高于用户任务优先级。
 *
 * \param[in] priority  任务优先级, 0 优先级最高, 最低优先级可通过
 *                      aw_task_lowest_priority() 获取。
 *                      当前平台优先级个数为8, 最低优先级为7, 最高优先级为0。
 *
 * \par 示例
 * \code
 *  #include "aw_task.h"
 *
 *  AW_TASK_DECL_STATIC(sys_task, 512);
 *
 *  AW_TASK_INIT(sys_task,
 *               "sys_task",
 *               AW_TASK_SYS_PRIORITY(0),   // 最高系统优先级
 *               512,               // 任务堆栈大小
 *               sys_task_entry,
 *               NULL;
 * \endcode
 * \hideinitializer
 */
#define AW_TASK_SYS_PRIORITY(priority)  \
            AW_PSP_TASK_SYS_PRIORITY(priority)

/**
 * \brief 启动任务
 *
 * \param[in] task  任务实体，由 AW_TASK_DECL() 或 AW_TASK_DECL_STATIC() 定义
 *
 * \par 范例
 * \code
 *  #include "aw_task.h"
 *
 *  // 任务入口函数
 *  void taska (void *p_arg)
 *  {
 *      while (1) {
 *          // ...
 *          aw_task_delay(aw_sys_clkrate_get());    // 延时 1S
 *      }
 *  }
 *
 *  int main ()
 *  {
 *
 *      aw_task_id_t task_id;   // 任务ID
 *
 *      // 定义任务实体 my_task
 *      AW_TASK_DECL_STATIC(my_task, 512);
 *
 *      // 初始化任务：
 *      task_id = AW_TASK_INIT(my_task,       // 任务实体
 *                             "taskname",    // 任务名字
 *                             5,             // 任务优先级
 *                             512,           // 任务堆栈大小
 *                             taska,         // 任务入口函数
 *                             (void*)1);     // 任务入口参数
 *
 *      if (task_id != NULL) {
 *
 *          // 启动任务
 *          aw_task_startup(task_id);
 *
 *          // 延时 20S.
 *          aw_task_delay(aw_sys_clkrate_get() * 20);
 *
 *          // 终止任务
 *          aw_task_terminate(task_id);
 *      }
 *
 *      return 0;
 *  }
 * \endcode
 *
 * \sa AW_TASK_DECL(), AW_TASK_DECL_STATIC(), AW_TASK_INIT(), AW_TASK_STARTUP(),
 *  AW_TASK_TERMINATE()
 * \hideinitializer
 */
#define AW_TASK_STARTUP(task)           AW_PSP_TASK_STARTUP(task)

/**
 * \brief 终止任务
 *
 * 目标任务将停止执行，并且不再被调度
 *
 * 如果有任务正在join目标任务，则join函数会返回成功，但是
 * exit_code 的值为-AW_EPIPE
 *
 * 此函数不被推荐使用，因为直接terminate会导致很多问题
 * 大多数情形下，没办法确定目标任务的会在哪里停止，
 * 所以很多共享资源往往处于不确定的状态
 * 例如任务中C++对象的析构函数不能正常执行
 * 对于任务中持有的mutex会直接终止，以保护被mutex保护起来的临界资源
 * 其它任务将再也不能访问这些资源了
 * 退出任务最推荐的做法只有从任务函数中使用return退出
 * 另外，如果这个任务是create创建的，则此函数会负责回收任务的内存
 *
 * 如果任务被INIT后，没有startup，则需要用AW_TASK_TERMINATE来释放
 * 其资源
 *
 * 目标task被成功终止后，可以重新INIT和startup
 *
 * \param[in]   task_id     任务ID，由 AW_TASK_INIT() 返回
 *
 * \retval   AW_OK      成功终止
 * \retval  -AW_EPERM   终止操作被拒绝
 */
#define AW_TASK_TERMINATE(task)         AW_PSP_TASK_TERMINATE(task)

/**
 * \brief 任务是否有效的判断
 *
 * \param[in] task  任务实体，由 AW_TASK_DECL() 或 AW_TASK_DECL_STATIC() 定义
 *
 * \retval  非AW_FALSE  任务有效
 * \retval  AW_FALSE    任务无效
 * \hideinitializer
 */
#define AW_TASK_VALID(task)             AW_PSP_TASK_VALID(task)


/**
 * \brief 任务延迟的tick数
 *
 * \param[in] tick  滴答数
 *
 * \return 成功返回AW_OK，否则返回负的错误码
 *         -AW_EPERM    不允许的操作，主要发生在操作系统尚未启动或是在中断中调用
 * \hideinitializer
 */
#define AW_TASK_DELAY(tick)             AW_PSP_TASK_DELAY(tick)

/**
 * \brief 挂起任务
 *
 * \param[in] task  任务实体，由 AW_TASK_DECL() 或 AW_TASK_DECL_STATIC() 定义
 *
 * \retval   AW_OK     成功挂起
 * \retval  -AW_EPERM  挂起操作被拒绝
 * \hideinitializer
 */
#define AW_TASK_SUSPEND(task)           AW_PSP_TASK_SUSPEND(task)

/**
 * \brief 恢复任务
 *
 * \param[in] task  任务实体，由 AW_TASK_DECL() 或 AW_TASK_DECL_STATIC() 定义
 *
 * \retval   AW_OK     成功恢复
 * \retval  -AW_EPERM  恢复操作被拒绝
 * \hideinitializer
 */
#define AW_TASK_RESUME(task)            AW_PSP_TASK_RESUME(task)

/**
 * \brief 恢复任务
 *
 * \param[in] task  任务实体，由 AW_TASK_DECL() 或 AW_TASK_DECL_STATIC() 定义
 *
 * \retval   AW_OK     成功恢复
 * \retval  -AW_EPERM  恢复操作被拒绝
 * \hideinitializer
 */

#define AW_TASK_JOIN(task,p_status)     AW_PSP_TASK_JOIN(task,p_status)

/**
 * \brief 使调用者任务进入删除安全模式
 *
 * 尝试删除调用者任务的其它任务将会阻塞，直到调用者任务调用 AW_TASK_UNSAFE()
 * 退出删除安全模式。
 *
 * \return  AW_OK   总是成功
 * \hideinitializer
 */
#define AW_TASK_SAFE()                  AW_PSP_TASK_SAFE()

/**
 * \brief 使调用者任务退出删除安全模式
 *
 * 尝试删除调用者任务的其它任务将会阻塞，直到调用者任务调用 AW_TASK_UNSAFE()
 * 退出删除安全模式。
 *
 * \return  AW_OK   总是成功
 * \hideinitializer
 */
#define AW_TASK_UNSAFE()            AW_PSP_TASK_UNSAFE()

/**
 * \brief 获取指定任务的栈空间的使用情况
 *
 * \param[in]  task    任务实体，由 AW_TASK_DECL() 或 AW_TASK_DECL_STATIC() 定义
 * \param[out] p_total 总共的栈大小
 * \param[out] p_free  空闲的栈大小
 *
 * \par 范例
 * \code
 *  #include "aw_task_stkchk.h"
 *  #include "aw_vdebug.h"
 *
 *  AW_TASK_DECL_STATIC(task1, 512);
 *  AW_TASK_DECL_STATIC(task2, 512);
 *
 *  task1_entry ()
 *  {
 *      unsigned int total;
 *      unsigned int free;
 *
 *      AW_FOREVER {
 *          //... do something
 *          AW_TASK_STACK_CHECK(task2, &total, &free);
 *          aw_kprintf("task2 stack total: %d bytes, free：%d bytes", total,
 *                     free);
 *      }
 *  }
 *
 *  task2_entry ()
 *  {
 *      AW_FOREVER {
 *
 *          //... do something
 *
 *      }
 *  }
 *
 *  void aw_main ()
 *  {
 *
 *      AW_TASK_INIT(task1,   "task1",   5, 512, task1_entry,   0);
 *      AW_TASK_INIT(task1,   "task1",   6, 512, task2_entry,   0);
 *      AW_TASK_STARTUP(task1);
 *      AW_TASK_STARTUP(task2);
 *
 *      AW_FOREVER {
 *          //... do something
 *          aw_mdelay(10);
 *      }
 *  }
 * \endcode
 *
 * \retval  0:  栈检查成功
 *
 * \note  结果仅供堆栈空间大小分配时的参考，时间越长，检测结果越准确。
 */
#define AW_TASK_STACK_CHECK(task, p_total, p_free)  \
            AW_PSP_TASK_STACK_CHECK(task, p_total, p_free)

/**
 * \brief 获取当前任务的栈空间的使用情况
 *
 * \param[out] p_total 总共的栈大小
 * \param[out] p_free  空闲的栈大小
 *
 * \par 范例
 * \code
 *  #include "aw_task_stkchk.h"
 *  #include "aw_vdebug.h"
 *
 *  AW_TASK_DECL_STATIC(task1, 512);
 *
 *
 *  task1_entry ()
 *  {
 *      unsigned int total;
 *      unsigned int free;
 *
 *      AW_FOREVER {
 *          //... do something
 *          AW_TASK_STACK_CHECK_SELF(&total, &free);
 *          aw_kprintf("task1 stack total: %d bytes, free：%d bytes",
 *                     total,
 *                     free);
 *          aw_mdelay(10);
 *      }
 *  }
 *
 *  void aw_main ()
 *  {
 *
 *      AW_TASK_INIT(task1,   "task1",   5, 512, task1_entry,   0);
 *
 *      AW_TASK_STARTUP(task1);
 *
 *      AW_FOREVER {
 *          //... do something
 *          aw_mdelay(10);
 *      }
 *  }
 * \endcode
 *
 * \return  0:  栈检查成功
 *
 * \note  结果仅供堆栈空间大小分配时的参考，时间越长，检测结果越准确。
 */
#define AW_TASK_STACK_CHECK_SELF(p_total, p_free)  \
            AW_PSP_TASK_STACK_CHECK_SELF(p_total, p_free)

/**
 * \brief 创建任务
 *
 * \param[in] name      任务名称
 * \param[in] priority  任务优先级, 0 优先级最高, 最低优先级可通过
 *                      aw_task_lowest_priority() 获取。
 *
 * \param[in] stack_size  任务堆栈大小
 * \param[in] func        入口函数
 * \param[in] arg         入口参数
 *
 * \return          成功返回任务的ID，失败则返回NULL
 *
 * \note 参数 \a priority 是用户任务的优先级。系统保留了若干优先级供内部系统服务
 * 和设备驱动使用。对于这些系统服务和设备驱动，使用宏 AW_TASK_SYS_PRIORITY(pri)
 * 表示系统任务优先级。
 */
aw_task_id_t aw_task_create (const char     *name,
                             int             priority,
                             size_t          stack_size,
                             aw_task_func_t  func,
                             void           *arg);

/**
 * \brief 设置任务用户数据
 *
 * tls: 该值任务内部存储、记录
 *
 * \param[in]   task_id 任务ID
 * \param[in]   data    用户数据
 *
 * \return  旧的的tls
 */
void *aw_task_set_tls (aw_task_id_t task_id, void *data);

/**
 * \brief 获取任务用户数据
 *
 * tls: 该值任务内部存储、记录
 *
 * \param[in]   task_id 任务ID
 *
 * \return  当前的tls
 */
void *aw_task_get_tls (aw_task_id_t task_id);

/**
 * \brief 当前任务放弃CPU，在剩下的任务中寻找一个优先级最高的任务来执行
 *
 * 不可在中断中调用
 *
 * \param[in]
 * \retval  0               成功返回0，否则返回负的错误码
 * \retval  -AW_EPERM       在中断中调用，会返回这个值
 */
int aw_task_yield (void);

/**
 * \brief 启动任务
 *
 * \param[in]   task_id     任务ID，由 AW_TASK_INIT() 返回
 *
 * \retval   AW_OK      成功启动
 * \retval  -AW_EPERM   启动操作被拒绝
 *
 * \par 范例
 *  见 AW_TASK_DECL()
 *
 * \sa AW_TASK_DECL(), AW_TASK_DECL_STATIC(), AW_TASK_INIT(), AW_TASK_STARTUP(),
 *  AW_TASK_TERMINATE()
 */
aw_err_t aw_task_startup (aw_task_id_t task_id);

/**
 * \brief 终止任务
 *
 * 目标任务将停止执行，并且不再被调度
 *
 * 如果有任务正在join目标任务，则join函数会返回成功，但是
 * exit_code 的值为-AW_EPIPE
 *
 * 此函数不被推荐使用，因为直接terminate会导致很多问题
 * 大多数情形下，没办法确定目标任务的会在哪里停止，
 * 所以很多共享资源往往处于不确定的状态
 * 例如任务中C++对象的析构函数不能正常执行
 * 对于任务中持有的mutex会直接终止，以保护被mutex保护起来的临界资源
 * 其它任务将再也不能访问这些资源了
 * 退出任务最推荐的做法只有从任务函数中使用return退出
 * 另外，如果这个任务是create创建的，则此函数会负责回收任务的内存
 *
 * 如果任务被INIT后，没有startup，则需要用aw_task_terminate来释放
 * 其资源
 *
 *
 * \param[in]   task_id     任务ID，由 AW_TASK_INIT() 返回
 *
 * \retval   AW_OK      成功终止
 * \retval  -AW_EPERM   终止操作被拒绝
 */
aw_err_t aw_task_terminate (aw_task_id_t task_id);

/**
 * \brief 任务廷时
 *
 * 任务廷时，单位为时间节拍数，使用 aw_sys_clkrate_get() 获取系统时钟节拍频率，
 * 即1秒钟的系统时钟节拍数。
 *
 * - 秒级延时表示方式为       (aw_sys_clkrate_get() * delay_x_s);
 * - 毫秒级延时的表示方式为   (aw_sys_clkrate_get() * delay_x_ms / 1000);
 * - 可使用 aw_ms_to_ticks() 将毫秒数转换为节拍数
 *
 * \param[in] ticks 滴答数
 *
 * \return 成功返回AW_OK，否则返回负的错误码
 *         -AW_EPERM    不允许的操作，主要发生在操作系统尚未启动或是在中断中调用
 *
 * \par 范例
 * \code
 *  #include "aw_task.h"
 *  void taska ( int a, int b )
 *  {
 *      while (1){
 *          // ...
 *          aw_task_delay(aw_sys_clkrate_get() * 1);            // 延时1S
 *          aw_task_delay(aw_sys_clkrate_get() * 100 / 1000);   // 延时100ms
 *          aw_task_delay(aw_ms_to_ticks(100));                 // 延时100ms
 *      }
 *  }
 *
 *  \endcode
 *  \sa aw_sys_clkrate_get()
 *  \hideinitializer
 */
aw_err_t aw_task_delay (unsigned int ticks);

/**
 * \brief 动态地设置任务的优先级
 *
 * \param[in] task_id      任务ID，由 AW_TASK_INIT() 返回
 * \param[in] new_priority 新的优先级
 *                         当前平台优先级个数为8, 最低优先级为7, 最高优先级为0。
 *
 * \retval    AW_OK      成功设置
 * \retval   -AW_EPERM   操作被拒绝
 */
aw_err_t aw_task_priority_set (aw_task_id_t task_id, unsigned int new_priority);

/**
 * \brief 获取用户可用的优先级数目
 *
 * \note 当前平台优先级个数为8, 最低优先级为7, 最高优先级为0
 * 优先级的范围是：0 (最高优先级) ~ aw_task_priority_count() - 1 (最低优先级)
 *
 * \return  优先级数目
 * \hideinitializer
 */
unsigned int aw_task_priority_count (void);

/**
 * \brief 获取最低的优先级
 *
 * \note 当前平台优先级个数为8, 最低优先级为7, 最高优先级为0。
 * 最低优先级 = aw_task_priority_count() - 1
 *
 * \return  最低优先级
 * \hideinitializer
 */
unsigned int aw_task_lowest_priority (void);

/**
 * \brief 获取当前任务的ID
 *
 * \attention   不可在中断中调用本函数
 *
 * \return  当前任务的ID
 */
aw_task_id_t aw_task_id_self (void);

/**
 * \brief 获取当前任务的名字
 *
 * \attention   不可在中断中调用本函数
 *
 * \param[in] task_id 任务ID，由 AW_TASK_INIT() 返回
 *
 * \return  当前任务的名字
 * \hideinitializer
 */
const char *aw_task_name_get (aw_task_id_t task_id);

/**
 * \brief 获取当前任务的优先级
 *
 * \attention   不可在中断中调用本函数
 *
 * \param[in] task_id 任务ID，由 AW_TASK_INIT() 返回
 *
 * \return  当前任务的优先级
 * \hideinitializer
 */
unsigned int aw_task_priority_get (aw_task_id_t task_id);

/**
 * \brief 判断任务ID是否有效
 *
 * \param[in] task_id 任务ID，由 AW_TASK_INIT() 返回

 * \retval AW_FALSE 任务无效,否则有效
 * \hideinitializer
 */
aw_bool_t aw_task_valid (aw_task_id_t task_id);

/**
 * \brief 等待另一个任务退出
 *
 * \param[in] task_id 任务ID（由 AW_TASK_INIT() 返回）
 * \param[out] p_status p_status指向一个地址，用于存放等待任务的退出码

 * \retval AW_OK 等待的任务成功退出
 * \retval 负的错误码
 * \hideinitializer
 */
aw_err_t aw_task_join(aw_task_id_t task_id,void **p_status);

/**
 * \brief 退出当前任务
 *
 * 在某个任务中可以直接调用aw_task_exit来退出此任务
 * 但是此方法却不被推荐，正确的退出方法是从任务函数中使用return返回的方式退出任务
 * 这样做可以保证函数中C++类对象的析构函数被正确的执行，使用aw_task_exit，则任务
 * 函数中类对象的析构函数不会执行
 * \param[in] status 任务的退出码

 * \retval 发生错误，则返回负的错误码,否则不返回
 * \hideinitializer
 */
aw_err_t aw_task_exit(void *status);

/**
 * \brief 挂起任务
 *
 * 这是一个不推荐的操作，因为目标任务会停在哪里是不确定的
 * 比如说目标任务获得了锁，正在访问资源，然而它被挂起了
 * 其他任务就不能获得锁，访问共享资源了
 *
 * 所以如果目标任务不想执行，可以调用aw_task_delay或者等待信号量
 * 主动停止执行，被挂起往往会导致意料之外的结果
 *
 * \param[in] task_id  任务ID（由 AW_TASK_INIT()或aw_task_create 返回）
 *
 * \retval   AW_OK     成功挂起
 * \retval  -AW_EPERM  挂起操作被拒绝
 * \hideinitializer
 */
aw_err_t aw_task_suspend(aw_task_id_t task_id);

/**
 * \brief 唤醒已经被aw_task_suspend挂起的任务
 *
 *
 * \param[in] task_id  任务ID（由 AW_TASK_INIT()或aw_task_create 返回）
 *
 * \retval   AW_OK     成功唤醒
 * \retval  -AW_EPERM  唤醒操作被拒绝
 * \hideinitializer
 */
aw_err_t aw_task_resume(aw_task_id_t task_id);



/**
 * \brief 保护当前任务不被其它任务强制结束
 *
 * \param     无
 * \retval    AW_OK      成功
 * \retval   -AW_EPERM   操作被拒绝，可能是在中断中调用了此函数
 * \retval   -AW_EFAULT  错误的调用了aw_task_unsafe或遇到了致命错误
 */
aw_err_t aw_task_safe (void);

/**
 * \brief 去掉对当前任务的保护，其它任务可以结束掉当前任务了
 *
 * \param     无
 * \retval    AW_OK      成功
 * \retval   -AW_EPERM   操作被拒绝，可能是在中断中调用了此函数
 * \retval   -AW_EFAULT  错误的调用了aw_task_unsafe或遇到了致命错误
 */
aw_err_t aw_task_unsafe (void);

/** @} grp_aw_if_task */

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#endif    /* __AW_TASK_H */

/* end of file */
