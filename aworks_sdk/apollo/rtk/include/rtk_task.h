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
 * \file  rtk_task.h
 * \brief 定义rtk task 相关结构和接口函数
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_TASK_H__
#define         __RTK_TASK_H__

/**
 *  @addtogroup kernel_structure
 *  @{
 */

/**
 *  @brief task control block definition
 *
 *  Task Control Block have many fields.
 *  Task's name is just a pointer.
 *  option and safe_count is not used currently.
 *  @sa TASK_API
 */
struct rtk_task
{
    void                       *sp;                 /**< \brief stack pointer, */
    const char                 *name;               /**< \brief task's name */
    char                       *stack_low;          /**< \brief stack low pointer */
    char                       *stack_high;         /**< \brief stack high pointer */
    char                       *stack_deep;         /**< \brief the maximal stack deep */
    rtk_task_prio_q_node_t      task_prio_q_node;   /**< \brief node in priority Q */
    unsigned int                readq_prio;
    /** \brief node in softtimer Q */
    struct rtk_tick             tick_node;
    unsigned long               last_run_tick;
    int                         last_run_tick_valid;
    /** \brief node in pending Q */
    rtk_list_node_t             wait_queue_node;
    /** \brief pend node if any */
    rtk_wait_queue_t           *pending_resource;

    /** \brief remember all base mutex */
    rtk_list_node_t             base_mutex_holded_head;
    unsigned int                base_mutex_need_prio;
    /** \brief running priority */
    unsigned int                waitq_prio;

#if CONFIG_RTK_MUTEX_EN
    /** \brief remember all mutex    */
    rtk_list_node_t             mutex_holded_head;
    /** \brief 持有互斥锁之前的优先级 */
    unsigned int                hold_mutex_org_priority;
#endif

    rtk_list_node_t             task_list_node;     /**< \brief node in task list */
    rtk_task_option_t           option;             /**< \brief 任务的选项 */
    uint32_t                    wake_reason;        /**< \brief 任务被唤醒的原因 */
#if CONFIG_TASK_TERMINATE_EN
    int                         safe_count;         /**< \brief prevent task deletion */
#endif
    rtk_task_status_t           status;             /**< \brief task status*/
#if CONFIG_RTK_PERFORMANCE_STATISTIC_EN
    uint64_t                    total_running_time;
#endif
    int                         error_number;       /**< \brief errno of current task */
#if CONFIG_TASK_JOIN_EN
    struct rtk_task            *p_who_join_me;
    struct rtk_task            *p_I_join_who;
    void                       *p_join_ret;
#endif
    void                       *thread_local_storage;
    void                       *env;

#if CONFIG_NEWLIB_REENT_EN
       /**
        * \brief Allocate a Newlib reent structure that is specific to this task.
        * Note Newlib support has been included by popular demand, but is not
        * used by the rtk maintainers themselves.  rtk is not
        * responsible for resulting newlib operation.  User must be familiar with
        * newlib and must provide system-wide implementations of the necessary
        * stubs. Be warned that (at the time of writing) the current newlib design
        * implements a system-wide malloc() that must be provided with locks.
        */
        struct _reent                   newlib_reent;
#endif

#if defined(__CC_ARM) || defined(__ARMCOMPILER_VERSION)
        char                            libc_perthread_libspace[96];
#endif
};

/**
 *  @}
 */


/**
 *  @defgroup TASK_API task API
 *  @{
 */

/**
 * \brief task control block init.
 *
 * \param task          pointer of task control block. You provide it.
 * \param name          name of the task. This string is copied to the top of the stack.
 * \param priority      task normal running priority.
 * \param stack_low     low address of the stack.
 * \param stack_high    high address of the stack.
 * \param pfunc         task entry function.
 * \param arg1          argument 1 to the task.
 * \param arg2          argument 2 to the task.
 *
 *  A task is initialized by this function but not be added to running queue.
 *  A task_starup(task) will add a initialized task to running queue.
 *  So you must use function task_startup() to start a initialized task.
 * @sa task_starup();
 */
struct rtk_task *rtk_task_init(struct rtk_task     *task,
                               const char          *name,
                               int                  priority,
                               rtk_task_option_t    option,
                               char *               stack_low,
                               char *               stack_high,
                               void                *pfunc,
                               void                *arg1,
                               void                *arg2);

struct rtk_task *rtk_task_create(const char          *name,
                                 int                  priority,
                                 rtk_task_option_t    option,
                                 unsigned int         stack_size,
                                 void                *pfunc,
                                 void                *arg1,
                                 void                *arg2);
int rtk_task_startup(struct rtk_task *task);

/**
 * \brief task unfreeze.
 *
 *  add a task to running queue.
 */
int rtk_task_unfreeze( struct rtk_task *task );

int rtk_task_freeze( struct rtk_task *task,int freeze_running_task);

void *rtk_task_set_tls(struct rtk_task *task, void *data );
void *rtk_task_get_tls(struct rtk_task *task );

/**
 * \brief set task priority
 * \fn rtk_task_priority_set
 *
 * \param[in]  task            task control block pointer. If NULL, current task's
 *                             priority will be change.
 * \param[in]  new_priority    new priority.
 *
 * \return     0               successfully.
 * \return     -AW_EINVAL      Invalid argument.
 * \return     -AW_EPERM       Permission denied. The task is not startup yet.
 *
 * new_priority:         P0       P1   P3
 *                        |       |     |
 *  0(high) ==============================================>> 256(low priority)
 *                                ^
 *                                |
 *                         current priority
 */
int rtk_task_priority_set( struct rtk_task *task, unsigned int priority );


/**
 * \brief stop a task and deinitialize.
 *
 *  the task may be running or pending.
 */
int rtk_task_terminate( struct rtk_task *task );

/**
 * \brief task delay
 *
 *  delay tick.
 */
int rtk_task_delay(unsigned int tick );


/**
 * \brief call this function to prevent other task terminate current task
 *
 *  You can all rtk_task_safe multi times,
 *  but how many times you call rtk_task_safe,how many times you must call
 *  rtk_task_unsafe,then other task can terminate this task
 *
 *  \param      none
 *
 *  \return     0               task is not exist.
 *  \return     negative        an nagative err code
 */
int rtk_task_safe( void );
/**
 * \brief call this function to unprevent other task terminate current task
 *
 * \param      none
 *
 * \return     0               task is not exist.
 * \return     negative        an nagative err code
 */
int rtk_task_unsafe( void );

/**
 * \brief check if a task is exist in the system
 *
 * \param[in]  task            task control block pointer.
 *
 * \return     0               task is not exist.
 * \return     negative        an nagative err code
 * \return     positive        the task is exist
 */
int rtk_task_check_exist(struct rtk_task *task);

/**
 * \brief Wait for a task exit,and get its exit code
 * \fn rtk_task_join
 *
 * \param[in]  task            task control block pointer.
 * \param[out] p_status        out the exit code
 *
 * \return     0               successfully.
 * \return     0 - errno
 *
 */
int rtk_task_join(struct rtk_task *task,void **p_status);

/**
 * \brief pontor of current task control bock.
 *
 * \ATTENTION  you should NEVER modify it.
 */
struct rtk_task *rtk_task_self(void);

/**
 * \brief 当前任务放弃CPU，在剩下的任务中寻找一个优先级最高的任务来执行
 *
 * 不可在中断中调用
 *
 * \retval  0               成功返回0，否则返回负的错误码
 * \retval  -AW_EPERM       在中断中调用，会返回这个值
 */
int rtk_task_yield( void );

/**
 * \brief     exit the thread in the thread function
 *
 * \note: 不要在中断中调用此函数，这个函数虽然可以成功退出任务，
 *        但是任务中的C++类对象的析构函数可能不会被正确执行
 *
 * \param  status    the task's exit code
 *
 * \return -AW_EPERM 在中断中调用了此函数，则返回此值
 */
int rtk_task_exit(void * status);

/**
 * \brief get current task_errno addr
 *
 * \return errno's address
 */
int * rtk_task_errno_addr( void );

int rtk_task_stack_check (
        struct rtk_task *p_task,
        unsigned int *p_total,
        unsigned int *p_free);

struct rtk_task_info {
    uint32_t        task_id;
    char            name[32];           /**< \brief task's name */
    char           *sp;                 /**< \brief stack pointer */
    char           *stack_low;          /**< \brief stack low pointer */
    char           *stack_high;         /**< \brief stack high pointer */
    char           *stack_deep;         /**< \brief the maximal stack deep */
    int             prio;               /**< \brief 线程的优先级 */
    uint64_t        total_running_time; /**< \brief 线程已经运行的时间 */
};


/**
 * \brief 枚举当前时刻RTK中的所有任务信息
 *
 * \attention 不能在中断中调用
 *
 * \param[out]      p_task_infos    用于输出任务信息的数组
 * \param[in]       task_info_count p_task_infos数组的元素个数
 *
 * \return  返回值<0为负的错误码，表明了发生了错误
 *
 *          否则会返回实际输出到p_task_infos数组中的任务个数
 *
 *          但是如果当前有效任务个数超过了task_info_count，则p_task_infos只会输出
 *          count个任务信息，而剩下的不会输出，所以p_task_infos数组元素个数
 *          最好大一点，并且此时返回值等于task_info_count
 *
 *          p_total_task_count用于输出当前时刻系统中任务总数，
 *          如果不关心这个，可以传入NULL
 *
 *          当返回值<p_total_task_count输出的值时，表明p_task_infos数组元素个数不够
 */
int rtk_enum_all_task_info(
        struct rtk_task_info *p_task_infos,
        int task_info_count,
        int *p_total_task_count);

/**
 *  @}
 */

#endif          /*__RTK_TASK_H__*/
