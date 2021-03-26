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
 * \brief 事件标志组，可以通过它实现一对一、一对多、多对一、多对多线程间的同步操作。
 *
 * 使用本服务需要包含头文件
 * \code
 * #include aw_flag_grp.h
 * \endcode
 *
 * \par 简单示例
 * \code
 *
    #include "aw_flag_grp.h"
    #include "aw_vdebug.h"
    #include "aw_delay.h"
    #include "aw_task.h"

    // 定义事件组及节点
    aw_local aw_flag_grp_t      __g_flg_grp;
    aw_local aw_flag_node_t     __g_flg_node0;
    aw_local aw_flag_node_t     __g_flg_node1;

    #define  TEST_FLAG_TASK_STACK_SIZE    512
    #define  TEST_FLAG_TASK_PRIO          3
    AW_TASK_DECL_STATIC(test_flg_task, TEST_FLAG_TASK_STACK_SIZE);

    #define  TEST_NODE0_FLAG   0x0C
    #define  TEST_NODE1_FLAG   0x83

    aw_local void __task_flag_entry (void *p_arg)
    {
        aw_err_t  ret = AW_OK;

        for (;;) {
            aw_mdelay(500);

            ret = aw_flag_pend(&__g_flg_grp,
                               &__g_flg_node0,
                               TEST_NODE0_FLAG,
                               AW_FLAG_NODE_WAIT_AND|AW_FLAG_NODE_WAIT_CLEAR,
                               AW_SEM_WAIT_FOREVER,
                               NULL);
            if (ret != AW_OK) {
                AW_ERRF(("aw_flag_pend err: %d\n", ret));
                continue;
            }

            ret = aw_flag_post(&__g_flg_grp,
                               TEST_NODE1_FLAG,
                               AW_FLAG_OPT_SET);
            if (ret != AW_OK) {
                AW_ERRF(("aw_flag_post err: %d\n", ret));
                continue;
            }
        }
    }

    void  test_flag_grp(void)
    {
        aw_err_t  ret   = AW_OK;
        uint32_t  flag  = 0;

        // 初始化事件组
        ret = aw_flag_init(&__g_flg_grp,
                           0,
                           AW_FLAG_WAIT_Q_PRIORITY);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_init err: %d\n", ret));
            return;
        }

        // 初始化事件组节点0
        ret = aw_flag_node_init(&__g_flg_node0);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_node_init err: %d\n", ret));
            return;
        }

        // 初始化事件组节点1
        ret = aw_flag_node_init(&__g_flg_node1);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_node_init err: %d\n", ret));
            return;
        }

        // 初始化任务
        AW_TASK_INIT(test_flg_task,
                     "test_flg_task",
                     TEST_FLAG_TASK_PRIO,
                     TEST_FLAG_TASK_STACK_SIZE,
                     __task_flag_entry,
                     NULL);
        // 启动任务
        AW_TASK_STARTUP(test_flg_task);

        // 设置事件组 TEST_NODE0_FLAG
        ret = aw_flag_post(&__g_flg_grp,
                           TEST_NODE0_FLAG,
                           AW_FLAG_OPT_SET);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_post err: %d\n", ret));
            return;
        }

        // 等待事件组标志满足条件 TEST_NODE1_FLAG
        ret = aw_flag_pend(&__g_flg_grp,
                           &__g_flg_node1,
                           TEST_NODE1_FLAG,
                           AW_FLAG_NODE_WAIT_OR|AW_FLAG_NODE_WAIT_CLEAR,
                           AW_SEM_WAIT_FOREVER,
                           &flag);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_pend err: %d\n", ret));
            return;
        }
        AW_INFOF(("return flag is: %d\n", flag));

        // 销毁节点
        ret = aw_flag_node_terminate(&__g_flg_node0);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_node_terminate err: %d\n", ret));
            return;
        }
        ret = aw_flag_node_terminate(&__g_flg_node1);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_node_terminate err: %d\n", ret));
            return;
        }

        // 销毁事件组对象
        ret = aw_flag_terminate(&__g_flg_grp);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_node_terminate err: %d\n", ret));
            return;
        }

        AW_INFOF(("test aw_flag_grp successful.\n"));
    }
 *
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history
 * - 1.00 15-06-02  dav, create file
 * - 1.01 17-04-25  may, add cpu lock to protect, change interface, fix bug.
 * @endinternal
 */

#ifndef __AW_FLAG_GRP_H
#define __AW_FLAG_GRP_H

#ifdef __cplusplus
extern "C" {
#endif

 /**
  * \addtogroup grp_aw_if_flag_grp
  * \copydoc aw_flag_grp.h
  * @{
  */

#include "aworks.h"
#include "aw_sem.h"
#include "aw_int.h"
#include "aw_list.h"

/**
 * \brief 定义事件组结构体
 */
#define AW_FLAG_DECL(flag_grp) \
            struct aw_flag_grp flag_grp;

/**
 * \brief 定义静态事件组结构体
 */
#define AW_FLAG_DECL_STATIC(flag_grp) \
            static struct aw_flag_grp flag_grp;

/**
 * \brief 定义事件组节点
 */
#define AW_FLAG_NODE_DECL(flag_grp_node) \
            struct aw_flag_grp_node flag_grp_node;

/**
 * \brief 定义静态事件组节点
 */
#define AW_FLAG_NODE_DECL_STATIC(flag_grp_node) \
            static struct aw_flag_grp_node flag_grp_node;

/**
 * \brief 事件组节点排队等待方式
 */
typedef enum {
    AW_FLAG_WAIT_Q_FIFO     = 0,    /**< \brief 按照节点先进先激活*/
    AW_FLAG_WAIT_Q_PRIORITY = 1     /**< \brief 按照节点所在线程优先级越高越先激活 */
} aw_flag_wait_q_t;

/**
 * \brief 事件组节点等待条件
 */
#define AW_FLAG_NODE_WAIT_AND       0x01    /**< \brief 必须所有标志位都满足 */
#define AW_FLAG_NODE_WAIT_OR        0x02    /**< \brief 当有一个标志位条件满足即可 */
#define AW_FLAG_NODE_WAIT_CLEAR     0x04    /**< \brief 等待标志位满足后清除当前标志位 */

/**
 * \brief 事件组操作，包括置位、清除及赋值
 */
typedef enum {
    AW_FLAG_OPT_SET = 0,        /**< \brief 置位对应标记位 */
    AW_FLAG_OPT_CLR = 1,        /**< \brief 清空对应标记位 */
    AW_FLAG_OPT_CPY = 2         /**< \brief 赋值标志 */
} aw_flag_opt_t;

/**
 * \brief 事件组类型定义
 */
typedef struct aw_flag_grp {
    uint32_t flags;                     /**< \brief 统一标志组标记 */
    struct aw_list_head wait_list_head; /**< \brief 任务链表首地址 */
    aw_flag_wait_q_t  wait_q;           /**< \brief 事件组节点等待排队方式 */
    int               state;            /**< \brief 事件组当前状态 */
} aw_flag_grp_t;

/**
 * \brief 事件组节点类型定义
 */
typedef struct aw_flag_grp_node {
    struct aw_list_head list_node;      /**< \brief 任务链表中的节点 */
    uint32_t            flags;          /**< \brief 当前正等待的标志位 */
    uint32_t            flag_buf;       /**< \brief 标志缓冲 */
    uint32_t            wait_type;      /**< \brief 等待方式 */
    int                 prio;           /**< \brief 所在任务的优先级 */
    int                 use_sta;        /**< \brief 当前事件节点是否被占用 */
    AW_SEMB_DECL( sem );                /**< \brief 等待信号量 */
} aw_flag_node_t;

/**
 * \brief 初始化事件组对象
 *
 * \param[in] p_flag_grp    事件组对象
 * \param[in] flags         初始的标记
 * \param[in] wait_q        事件组节点排队等待方式，可以为 AW_FLAG_WAIT_Q_FIFO 或
 *                          AW_FLAG_WAIT_Q_PRIORITY
 *
 * \return 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 *
 * \attention 对象只能初始化一次
 */
aw_err_t aw_flag_init(aw_flag_grp_t    *p_flag_grp,
                      uint32_t          flags,
                      aw_flag_wait_q_t  wait_q);

/**
 * \brief 初始化事件组节点
 *
 * \param[in] p_node        事件组节点对象
 *
 * \return 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 *
 * \attention 对象只能初始化一次
 */
aw_err_t aw_flag_node_init(aw_flag_node_t *p_node);

/**
 * \brief 等待事件组节点条件满足后(对应标志位符合条件)触发
 *
 * \param[in] p_flag_grp    事件组对象
 * \param[in] p_node        需要等待的事件组节点对象
 * \param[in] flags         需要等待的事件标志值
 * \param[in] wait_type     事件组节点等待方式，AW_FLAG_NODE_WAIT_AND 或
 *                          AW_FLAG_NODE_WAIT_OR，若等待条件后需要清除该
 *                          标志位，可以|AW_FLAG_NODE_WAIT_CLEAR，如：
 *                          (AW_FLAG_NODE_WAIT_AND|AW_FLAG_NODE_WAIT_CLEAR)，
 *                          (AW_FLAG_NODE_WAIT_OR|AW_FLAG_NODE_WAIT_CLEAR)，
 * \param[in] timeout       等待超时时间(ms)，超时后返回错误值，
 *                          若想探测条件是否满足，可以置为 0,
 *                          若想永久等待，可以置为 AW_SEM_WAIT_FOREVER
 * \param[in] p_rev         返回当前的满足的标志位，若不想获取，可以传入NULL
 *
 * \retval  AW_OK       已接收到指定事件标志位
 * \retval  -AW_EINVAL  参数有错
 * \retval  -AW_ETIME   等待超时
 * \retval  -AW_EBUSY   节点被占用
 * \retval  -AW_ENODEV  节点或事件组已销毁
 *
 * \attention 不可在中断服务中调用，除非参数 timeout 为 0
 */
aw_err_t aw_flag_pend(aw_flag_grp_t  *p_flag_grp,
                      aw_flag_node_t *p_node,
                      uint32_t        flags,
                      uint32_t        wait_type,
                      uint32_t        timeout,
                      uint32_t       *p_rev);

/**
 * \brief 设置事件组中标记
 *
 * \param[in] p_flag_grp    事件组对象
 * \param[in] flags         设置的标记
 * \param[in] option        设置方式： AW_FLAG_OPT_SET、AW_FLAG_OPT_CLR、
 *                          AW_FLAG_OPT_CPY，如：
 *                          若 flags 为 0x11，事件组当前标志为grp_flag，则：
 *                             AW_FLAG_OPT_SET： grp_flag | flags
 *                             AW_FLAG_OPT_CLR： grp_flag & (~flags)
 *                             AW_FLAG_OPT_CPY: grp_flag = flags
 *
 * \retval AW_OK        成功
 * \retval -AW_EINVAL   参数有错
 * \retval -AW_ENODEV   事件组对象已销毁
 *
 * \attention 可以在中断服务或线程中调用
 */
aw_err_t aw_flag_post(aw_flag_grp_t    *p_flag_grp,
                      uint32_t          flags,
                      aw_flag_opt_t     option);

/**
 * \brief 销毁事件组对象
 *
 * \param[in] p_flag_grp  事件组对象
 *
 * \return 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 */
aw_err_t aw_flag_terminate(aw_flag_grp_t *p_flag_grp);

/**
 * \brief 销毁事件组节点对象
 *
 * \param[in] p_node  事件组节点对象
 *
 * \return 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 */
aw_err_t aw_flag_node_terminate(aw_flag_node_t *p_node);

/** @} grp_aw_if_flag_grp */

#ifdef __cplusplus
}
#endif

#endif /* __AW_FLAG_GRP_H */

 /* end of file */
