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
 * \brief OS Trace 的内部头文件
 *
 * 主要是处理 Trace API ，并提供 Trace 插桩用的宏。
 * 主要提供3种宏：
 *   - SystemView 提供的系统事件（包括 Timer 事件）。
 *   - OS 提供的 API 调用事件。
 *   - OS 提供的 API 进入/退出事件。
 */

#ifndef __OS_TRACE_EVENTS_H
#define __OS_TRACE_EVENTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os_cfg_trace.h"

/*******************************************************************************
  用于 SystemView Trace 的固定的定义
*******************************************************************************/

#if (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))
#define OS_TRACE_ID_OFFSET                         (32u)

#define OS_TRACE_ID_RTK_INIT                       (5u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_RTK_STARTUP                    (6u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_RTK_MAX_PRIORITY               (7u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_RTK_TICK_DOWN_COUNTER_INIT     (8u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_RTK_TICK_DOWN_COUNTER_START    (9u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_RTK_TICK_DOWN_COUNTER_STOP     (10u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_RTK_TICK_DOWN_COUNTER_SET_FUNC (11u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_RTK_TICK_DOWN_COUNTER_ANNOUNCE (12u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_TICK_GET                       (13u + OS_TRACE_ID_OFFSET)

#define OS_TRACE_ID_TASK_INIT                      (20u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_TASK_STARTUP                   (21u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_TASK_TERMINATE                 (22u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_TASK_YIELD                     (23u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_TASK_DELAY                     (24u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_TASK_SUSPEND                   (25u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_TASK_RESUME                    (26u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_TASK_SET_TLS                   (27u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_TASK_GET_TLS                   (28u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_TASK_SAFE                      (29u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_TASK_UNSAFE                    (30u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_TASK_PRIORITY_SET              (31u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_TASK_ERRNO_ADDR                (32u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_TASK_JOIN                      (33u + OS_TRACE_ID_OFFSET)

#define OS_TRACE_ID_MUTEX_INIT                     (40u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_MUTEX_TERMINATE                (41u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_MUTEX_UNLOCK                   (42u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_MUTEX_LOCK                     (43u + OS_TRACE_ID_OFFSET)

#define OS_TRACE_ID_SEMB_INIT                      (50u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_SEMB_TERMINATE                 (51u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_SEMB_GIVE                      (52u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_SEMB_TAKE                      (53u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_SEMB_RESET                     (54u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_SEMB_CLEAR                     (55u + OS_TRACE_ID_OFFSET)

#define OS_TRACE_ID_SEMC_INIT                      (60u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_SEMC_TERMINATE                 (61u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_SEMC_GIVE                      (62u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_SEMC_TAKE                      (63u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_SEMC_RESET                     (64u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_SEMC_CLEAR                     (65u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_SEMC_BROADCAST                 (66u + OS_TRACE_ID_OFFSET)

#define OS_TRACE_ID_MSGQ_INIT                      (70u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_MSGQ_TERMINATE                 (71u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_MSGQ_SEND                      (72u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_MSGQ_RECEIVE                   (73u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_MSGQ_CLEAR                     (74u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_MSGQ_IS_FULL                   (75u + OS_TRACE_ID_OFFSET)
#define OS_TRACE_ID_MSGQ_IS_EMPTY                  (76u + OS_TRACE_ID_OFFSET)

#endif /* OS_CFG_TRACE_EN && OS_CFG_TRACE_EN > 0u */


/*******************************************************************************
  提供 SystemView 的系统事件。
*******************************************************************************/

#if (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))

static  const   OS_TRACE_API    *__gp_os_trace_api=(void*)0;
#define TRACEAPI_SET(p_trace)   {__gp_os_trace_api=p_trace;}
#define __TRACE_API_VALID()     ((void*)0!=__gp_os_trace_api)

#if (defined(OS_CFG_TRACE_CONTROL_EN) && (OS_CFG_TRACE_CONTROL_EN > 0u))
#define TRACE_Start()       \
         {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordStart();}
#define TRACE_Stop()       \
         {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordStop();}
#else
#define TRACE_Start()
#define TRACE_Stop()
#endif /* OS_CFG_TRACE_CONTROL_EN && OS_CFG_TRACE_CONTROL_EN > 0u */

/*****************************************************************************
*                   Trace System Recorder Functions
******************************************************************************/
#define OS_TRACE_ENTER_ISR()                        \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordEnterISR();}
#define OS_TRACE_ISR_EXIT_TO_SCHEDULER()            \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordExitISRToSched();}
#define OS_TRACE_EXIT_ISR(is_int_context)           \
            {if(is_int_context) if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordExitISR();}

#define OS_TRACE_TASK_START_EXEC(p_task)            \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordTaskStartExec(p_task);}
#define OS_TRACE_TASK_STOP_EXEC()                   \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordTaskStopExec();}
#define OS_TRACE_TASK_CREATE(p_task)                \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordTaskCreate(p_task);}
#define OS_TRACE_TASK_TERMINATE2(p_task)            \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordTaskTerminate(p_task);}
#define OS_TRACE_TASK_START_READY(p_task)           \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordTaskStartReady(p_task);}
#define OS_TRACE_TASK_INFO(p_task)                  \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordTaskInfo(p_task);}
#define OS_TRACE_TASK_STOP_READY(p_task, Reason)    \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordTaskStopReady(p_task, Reason);}

#define OS_TRACE_TIMER_ENTER(TimerId)               \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordEnterTimer((unsigned long)TimerId);}
#define OS_TRACE_TIMER_EXIT()                       \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordExitTimer();}

#else
#define TRACEAPI_SET(p_trace)
#define __TRACE_API_VALID()
#define TRACE_Start()
#define TRACE_Stop()
#define OS_TRACE_ENTER_ISR()
#define OS_TRACE_ISR_EXIT_TO_SCHEDULER()
#define OS_TRACE_EXIT_ISR(is_int_context)
#define OS_TRACE_TASK_START_EXEC(p_task)
#define OS_TRACE_TASK_STOP_EXEC()
#define OS_TRACE_TASK_CREATE(p_task)
#define OS_TRACE_TASK_TERMINATE2(p_task)
#define OS_TRACE_TASK_START_READY(p_task)
#define OS_TRACE_TASK_INFO(p_task)
#define OS_TRACE_TASK_STOP_READY(p_task, Reason)

#define OS_TRACE_TIMER_ENTER(TimerId)
#define OS_TRACE_TIMER_EXIT()

#endif  /* OS_CFG_TRACE_EN && OS_CFG_TRACE_EN > 0u */


/*****************************************************************************
*            提供 SystemView 记录函数调用/退出的 API
******************************************************************************/

#if (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))

#define __RECORDVOID(EventID)                           \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordVoid(EventID);}
#define __RECORDU32(EventID,Para0)                      \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordU32(EventID, \
                                                                   (unsigned long)Para0);}
#define __RECORDU32X2(EventID,Para0,Para1)              \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordU32x2(\
                    EventID,(unsigned long)Para0,(unsigned long)Para1);}
#define __RECORDU32X3(EventID,Para0,Para1,Para2)        \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordU32x3(\
                    EventID,(unsigned long)Para0,(unsigned long)Para1,(unsigned long)Para2);}
#define __RECORDU32X4(EventID,Para0,Para1,Para2,Para3)  \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordU32x4(\
                    EventID,(unsigned long)Para0,(unsigned long)Para1,\
                            (unsigned long)Para2,(unsigned long)Para3);}
#define __EndCall(EventID)                              \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordEndCall(EventID);}
#define __EndCallU32(EventID, RetVal)                   \
            {if(__TRACE_API_VALID())__gp_os_trace_api->pfRecordEndCallU32(\
                    EventID,(unsigned long)RetVal);}
#define __SHRINKID(ID)                                  \
            (__gp_os_trace_api->pfShrinkId((unsigned long)ID))

#else

#define __RECORDVOID(EventID)
#define __RECORDU32(EventID,Para0)
#define __RECORDU32X2(EventID,Para0,Para1)
#define __RECORDU32X3(EventID,Para0,Para1,Para2)
#define __RECORDU32X4(EventID,Para0,Para1,Para2,Para3)
#define __EndCall(EventID)
#define __EndCallU32(EventID, RetVal)
#define __SHRINKID(ID)

#endif  //(defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))

/*****************************************************************************
*                   RTK 部分 API
******************************************************************************/
#if (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))

#define OS_TRACE_RTK_INIT()                         __RECORDVOID(OS_TRACE_ID_RTK_INIT)
#define OS_TRACE_RTK_STARTUP()                      __RECORDVOID(OS_TRACE_ID_RTK_STARTUP)
#define OS_TRACE_RTK_TICK_DOWN_COUNTER_INIT(p_tick)        \
                __RECORDU32(OS_TRACE_ID_RTK_TICK_DOWN_COUNTER_INIT, p_tick)
#define OS_TRACE_RTK_TICK_DOWN_COUNTER_START(p_tick,tick)   \
                __RECORDU32X2(OS_TRACE_ID_RTK_TICK_DOWN_COUNTER_START,   p_tick,tick)
#define OS_TRACE_RTK_TICK_DOWN_COUNTER_STOP(p_tick)          \
            __RECORDU32(OS_TRACE_ID_RTK_TICK_DOWN_COUNTER_STOP,      p_tick)
#define OS_TRACE_RTK_TICK_DOWN_COUNTER_SET_FUNC(p_tick,func,arg) \
            __RECORDU32X3(OS_TRACE_ID_RTK_TICK_DOWN_COUNTER_SET_FUNC,p_tick,func,arg)
#define OS_TRACE_RTK_TICK_DOWN_COUNTER_ANNOUNCE()
#define OS_TRACE_RTK_MAX_PRIORITY(priority)   \
            __RECORDU32(OS_TRACE_ID_RTK_MAX_PRIORITY, priority)
#define OS_TRACE_TICK_GET(systick)           \
                __RECORDU32(OS_TRACE_ID_TICK_GET, systick)

#define OS_TRACE_TASK_INIT(p_task)           \
                __RECORDU32(OS_TRACE_ID_TASK_INIT,   p_task)
#define OS_TRACE_TASK_STARTUP(p_task)         \
                __RECORDU32(OS_TRACE_ID_TASK_STARTUP,   __SHRINKID(p_task))
#define OS_TRACE_TASK_YIELD()                \
                __RECORDVOID(OS_TRACE_ID_TASK_YIELD)
#define OS_TRACE_TASK_TERMINATE(p_task)    \
                __RECORDU32(OS_TRACE_ID_TASK_TERMINATE, __SHRINKID(p_task))
#define OS_TRACE_TASK_DELAY(tick)           \
                __RECORDU32(OS_TRACE_ID_TASK_DELAY,  tick)
#define OS_TRACE_TASK_SUSPEND(p_task)       \
                __RECORDU32(OS_TRACE_ID_TASK_SUSPEND,   __SHRINKID(p_task))
#define OS_TRACE_TASK_RESUME(p_task)         \
            __RECORDU32(OS_TRACE_ID_TASK_RESUME,    __SHRINKID(p_task))
#define OS_TRACE_TASK_SAFE(count)           \
                __RECORDU32(OS_TRACE_ID_TASK_SAFE,   count)
#define OS_TRACE_TASK_UNSAFE(count)         \
                __RECORDU32(OS_TRACE_ID_TASK_UNSAFE, count)
#define OS_TRACE_TASK_ERRNO_ADDR(err_addr)    \
            __RECORDU32(OS_TRACE_ID_TASK_ERRNO_ADDR,  err_addr)

#define OS_TRACE_MUTEX_INIT(p_mutex)            \
            __RECORDU32(OS_TRACE_ID_MUTEX_INIT,  p_mutex)
#define OS_TRACE_SEMB_INIT(p_semb, cnt)         \
            __RECORDU32X2(OS_TRACE_ID_SEMB_INIT, p_semb, cnt)
#define OS_TRACE_SEMC_INIT(p_semc, cnt)         \
            __RECORDU32X2(OS_TRACE_ID_SEMC_INIT, p_semc, cnt)
#define OS_TRACE_MSGQ_INIT(p_msgq, p_buf, cnt, sz) \
            __RECORDU32X2(OS_TRACE_ID_MSGQ_INIT, p_msgq, sz)

#else

#define OS_TRACE_RTK_INIT()
#define OS_TRACE_RTK_STARTUP()
#define OS_TRACE_RTK_TICK_DOWN_COUNTER_INIT(p_tick)
#define OS_TRACE_RTK_TICK_DOWN_COUNTER_START(p_tick,tick)
#define OS_TRACE_RTK_TICK_DOWN_COUNTER_STOP(p_tick)
#define OS_TRACE_RTK_TICK_DOWN_COUNTER_SET_FUNC(p_tick,func,arg)
#define OS_TRACE_RTK_TICK_DOWN_COUNTER_ANNOUNCE()
#define OS_TRACE_RTK_MAX_PRIORITY(priority)
#define OS_TRACE_TICK_GET(systick)
#define OS_TRACE_TASK_INIT(p_task)
#define OS_TRACE_TASK_STARTUP(p_task)
#define OS_TRACE_TASK_YIELD()
#define OS_TRACE_TASK_TERMINATE(p_task)
#define OS_TRACE_TASK_DELAY(tick)
#define OS_TRACE_TASK_SUSPEND(p_task)
#define OS_TRACE_TASK_RESUME(p_task)
#define OS_TRACE_TASK_SAFE(count)
#define OS_TRACE_TASK_UNSAFE(count)
#define OS_TRACE_TASK_ERRNO_ADDR(err_addr)
#define OS_TRACE_MUTEX_INIT(p_mutex)
#define OS_TRACE_SEMB_INIT(p_semb, cnt)
#define OS_TRACE_SEMC_INIT(p_semc, cnt)
#define OS_TRACE_MSGQ_INIT(p_msgq, p_buf, cnt, sz)

#endif       //(defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))


/*****************************************************************************
*                   RTK 部分 API的进入
******************************************************************************/
#if (defined(OS_CFG_TRACE_API_ENTER_EN) && (OS_CFG_TRACE_API_ENTER_EN > 0u))

#define  OS_TRACE_TASK_SET_TLS_ENTER(p_task,data)     \
            __RECORDU32X2(OS_TRACE_ID_TASK_SET_TLS,      __SHRINKID(p_task), data)
#define  OS_TRACE_TASK_GET_TLS_ENTER(p_task)          \
            __RECORDU32(OS_TRACE_ID_TASK_GET_TLS,        __SHRINKID(p_task))
#define  OS_TRACE_TASK_PRIORITY_SET_ENTER(p_task, priority)\
            __RECORDU32X2(OS_TRACE_ID_TASK_PRIORITY_SET, __SHRINKID(p_task), priority)
#define  OS_TRACE_TASK_JOIN_ENTER(p_task,p_join_task)   \
            __RECORDU32X2(OS_TRACE_ID_TASK_JOIN, __SHRINKID(p_task), __SHRINKID(p_join_task))

#define  OS_TRACE_MUTEX_UNLOCK_ENTER(p_mutex)      \
            __RECORDU32(OS_TRACE_ID_MUTEX_UNLOCK,     p_mutex)
#define  OS_TRACE_MUTEX_LOCK_ENTER(p_mutex,tick)   \
            __RECORDU32X2(OS_TRACE_ID_MUTEX_LOCK,     p_mutex,tick)
#define  OS_TRACE_MUTEX_TERMINATE_ENTER(p_mutex)    \
            __RECORDU32(OS_TRACE_ID_MUTEX_TERMINATE,  p_mutex)

#define  OS_TRACE_SEMB_GIVE_ENTER(p_semb)          \
            __RECORDU32(OS_TRACE_ID_SEMB_GIVE,        p_semb)
#define  OS_TRACE_SEMB_TAKE_ENTER(p_semb, tick)   \
            __RECORDU32X2(OS_TRACE_ID_SEMB_TAKE,      p_semb, tick)
#define  OS_TRACE_SEMB_TERMINATE_ENTER(p_semb)     \
            __RECORDU32(OS_TRACE_ID_SEMB_TERMINATE,   p_semb)
#define  OS_TRACE_SEMB_CLEAR_ENTER(p_semb)         \
            __RECORDU32(OS_TRACE_ID_SEMB_CLEAR,       p_semb)
#define  OS_TRACE_SEMB_RESET_ENTER(p_semb,value)    \
            __RECORDU32X2(OS_TRACE_ID_SEMB_RESET,     p_semb, value)

#define  OS_TRACE_SEMC_GIVE_ENTER(p_semc)          \
            __RECORDU32(OS_TRACE_ID_SEMC_GIVE,        p_semc)
#define  OS_TRACE_SEMC_TAKE_ENTER(p_semc,tick)     \
            __RECORDU32X2(OS_TRACE_ID_SEMC_TAKE,      p_semc, tick)
#define  OS_TRACE_SEMC_BROADCAST_ENTER(p_semc)     \
            __RECORDU32(OS_TRACE_ID_SEMC_BROADCAST,   p_semc)
#define  OS_TRACE_SEMC_TERMINATE_ENTER(p_semc)      \
            __RECORDU32(OS_TRACE_ID_SEMC_TERMINATE,   p_semc)
#define  OS_TRACE_SEMC_CLEAR_ENTER(p_semc)          \
            __RECORDU32(OS_TRACE_ID_SEMC_CLEAR,       p_semc)
#define  OS_TRACE_SEMC_RESET_ENTER(p_semc,value)   \
            __RECORDU32X2(OS_TRACE_ID_SEMC_RESET,     p_semc, value)

#define  OS_TRACE_MSGQ_RECEIVE_ENTER(p_msgq, buff, size, tick) \
            __RECORDU32X4(OS_TRACE_ID_MSGQ_RECEIVE, p_msgq, buff, size, tick)
#define  OS_TRACE_MSGQ_SEND_ENTER(p_msgq, buff, size, tick) \
            __RECORDU32X4(OS_TRACE_ID_MSGQ_SEND,    p_msgq, buff, size, tick)
#define  OS_TRACE_MSGQ_CLEAR_ENTER(p_msgq)                 \
            __RECORDU32(OS_TRACE_ID_MSGQ_CLEAR,     p_msgq)
#define  OS_TRACE_MSGQ_TERMINATE_ENTER(p_msgq)         \
            __RECORDU32(OS_TRACE_ID_MSGQ_TERMINATE, p_msgq)
#define  OS_TRACE_MSGQ_IS_FULL_ENTER(p_msgq)             \
            __RECORDU32(OS_TRACE_ID_MSGQ_IS_FULL,   p_msgq)
#define  OS_TRACE_MSGQ_IS_EMPTY_ENTER(p_msgq)         \
            __RECORDU32(OS_TRACE_ID_MSGQ_IS_EMPTY,  p_msgq)

#else

#define  OS_TRACE_TASK_SET_TLS_ENTER(p_task,data)
#define  OS_TRACE_TASK_GET_TLS_ENTER(p_task)
#define  OS_TRACE_TASK_PRIORITY_SET_ENTER(p_task, priority)
#define  OS_TRACE_TASK_JOIN_ENTER(p_task,p_join_task)

#define  OS_TRACE_MUTEX_UNLOCK_ENTER(p_mutex)
#define  OS_TRACE_MUTEX_LOCK_ENTER(p_mutex,tick)
#define  OS_TRACE_MUTEX_TERMINATE_ENTER(p_mutex)

#define  OS_TRACE_SEMB_GIVE_ENTER(p_semb)
#define  OS_TRACE_SEMB_TAKE_ENTER(p_semb, tick)
#define  OS_TRACE_SEMB_TERMINATE_ENTER(p_semb)
#define  OS_TRACE_SEMB_CLEAR_ENTER(p_semb)
#define  OS_TRACE_SEMB_RESET_ENTER(p_semb,value)

#define  OS_TRACE_SEMC_GIVE_ENTER(p_semc)
#define  OS_TRACE_SEMC_TAKE_ENTER(p_semc,tick)
#define  OS_TRACE_SEMC_BROADCAST_ENTER(p_semc)
#define  OS_TRACE_SEMC_TERMINATE_ENTER(p_semc)
#define  OS_TRACE_SEMC_CLEAR_ENTER(p_semc)
#define  OS_TRACE_SEMC_RESET_ENTER(p_semc,value)

#define  OS_TRACE_MSGQ_RECEIVE_ENTER(p_msgq, buff, size, tick)
#define  OS_TRACE_MSGQ_SEND_ENTER(p_msgq, buff, size, tick)
#define  OS_TRACE_MSGQ_CLEAR_ENTER(p_msgq)
#define  OS_TRACE_MSGQ_TERMINATE_ENTER(p_msgq)
#define  OS_TRACE_MSGQ_IS_FULL_ENTER(p_msgq)
#define  OS_TRACE_MSGQ_IS_EMPTY_ENTER(p_msgq)

#endif /* OS_CFG_TRACE_API_ENTER_EN && OS_CFG_TRACE_API_ENTER_EN > 0u */

/*****************************************************************************
*                   RTK 部分 API的退出
******************************************************************************/
#if (defined(OS_CFG_TRACE_API_EXIT_EN) && (OS_CFG_TRACE_API_EXIT_EN > 0u))

#define OS_TRACE_TASK_SET_TLS_EXIT(RetVal)    \
            __EndCallU32(OS_TRACE_ID_TASK_SET_TLS,   RetVal)
#define OS_TRACE_TASK_GET_TLS_EXIT(RetVal)    \
            __EndCallU32(OS_TRACE_ID_TASK_GET_TLS,   RetVal)
#define OS_TRACE_TASK_PRIORITY_SET_EXIT(RetVal)\
            __EndCallU32(OS_TRACE_ID_TASK_PRIORITY_SET,RetVal)
#define OS_TRACE_TASK_JOIN_EXIT(RetVal)       \
            __EndCallU32(OS_TRACE_ID_TASK_JOIN,RetVal)

#define OS_TRACE_MUTEX_UNLOCK_EXIT(RetVal)   \
            __EndCallU32(OS_TRACE_ID_MUTEX_UNLOCK,   RetVal)
#define OS_TRACE_MUTEX_LOCK_EXIT(RetVal)      \
            __EndCallU32(OS_TRACE_ID_MUTEX_LOCK,     RetVal)
#define OS_TRACE_MUTEX_TERMINATE_EXIT(RetVal)  \
            __EndCallU32(OS_TRACE_ID_MUTEX_TERMINATE,RetVal)

#define OS_TRACE_SEMB_GIVE_EXIT(RetVal)    \
            __EndCallU32(OS_TRACE_ID_SEMB_GIVE,      RetVal)
#define OS_TRACE_SEMB_TAKE_EXIT(RetVal)      \
            __EndCallU32(OS_TRACE_ID_SEMB_TAKE,      RetVal)
#define OS_TRACE_SEMB_TERMINATE_EXIT(RetVal)  \
            __EndCallU32(OS_TRACE_ID_SEMB_TERMINATE, RetVal)
#define OS_TRACE_SEMB_CLEAR_EXIT(RetVal)      \
            __EndCallU32(OS_TRACE_ID_SEMB_CLEAR,     RetVal)
#define OS_TRACE_SEMB_RESET_EXIT(RetVal)      \
            __EndCallU32(OS_TRACE_ID_SEMB_RESET,     RetVal)

#define OS_TRACE_SEMC_GIVE_EXIT(RetVal)      \
            __EndCallU32(OS_TRACE_ID_SEMC_GIVE,      RetVal)
#define OS_TRACE_SEMC_TAKE_EXIT(RetVal)      \
            __EndCallU32(OS_TRACE_ID_SEMC_TAKE,      RetVal)
#define OS_TRACE_SEMC_BROADCAST_EXIT(RetVal)  \
            __EndCallU32(OS_TRACE_ID_SEMC_BROADCAST, RetVal)
#define OS_TRACE_SEMC_TERMINATE_EXIT(RetVal)   \
            __EndCallU32(OS_TRACE_ID_SEMC_TERMINATE, RetVal)
#define OS_TRACE_SEMC_CLEAR_EXIT(RetVal)    \
            __EndCallU32(OS_TRACE_ID_SEMC_CLEAR,     RetVal)
#define OS_TRACE_SEMC_RESET_EXIT(RetVal)    \
            __EndCallU32(OS_TRACE_ID_SEMC_RESET,     RetVal)

#define OS_TRACE_MSGQ_RECEIVE_EXIT(RetVal)   \
            __EndCallU32(OS_TRACE_ID_MSGQ_RECEIVE,  RetVal)
#define OS_TRACE_MSGQ_SEND_EXIT(RetVal)      \
            __EndCallU32(OS_TRACE_ID_MSGQ_SEND,     RetVal)
#define OS_TRACE_MSGQ_CLEAR_EXIT(RetVal)      \
            __EndCallU32(OS_TRACE_ID_MSGQ_CLEAR,    RetVal)
#define OS_TRACE_MSGQ_TERMINATE_EXIT(RetVal)  \
        __EndCallU32(OS_TRACE_ID_MSGQ_TERMINATE,RetVal)
#define OS_TRACE_MSGQ_IS_FULL_EXIT(RetVal)     \
            __EndCallU32(OS_TRACE_ID_MSGQ_IS_FULL,  RetVal)
#define OS_TRACE_MSGQ_IS_EMPTY_EXIT(RetVal)   \
            __EndCallU32(OS_TRACE_ID_MSGQ_IS_EMPTY, RetVal)

#else

#define OS_TRACE_TASK_SET_TLS_EXIT(RetVal)
#define OS_TRACE_TASK_GET_TLS_EXIT(RetVal)
#define OS_TRACE_TASK_PRIORITY_SET_EXIT(RetVal)
#define OS_TRACE_TASK_JOIN_EXIT(RetVal)

#define OS_TRACE_MUTEX_UNLOCK_EXIT(RetVal)
#define OS_TRACE_MUTEX_LOCK_EXIT(RetVal)
#define OS_TRACE_MUTEX_TERMINATE_EXIT(RetVal)

#define OS_TRACE_SEMB_GIVE_EXIT(RetVal)
#define OS_TRACE_SEMB_TAKE_EXIT(RetVal)
#define OS_TRACE_SEMB_TERMINATE_EXIT(RetVal)
#define OS_TRACE_SEMB_CLEAR_EXIT(RetVal)
#define OS_TRACE_SEMB_RESET_EXIT(RetVal)

#define OS_TRACE_SEMC_GIVE_EXIT(RetVal)
#define OS_TRACE_SEMC_TAKE_EXIT(RetVal)
#define OS_TRACE_SEMC_BROADCAST_EXIT(RetVal)
#define OS_TRACE_SEMC_TERMINATE_EXIT(RetVal)
#define OS_TRACE_SEMC_CLEAR_EXIT(RetVal)
#define OS_TRACE_SEMC_RESET_EXIT(RetVal)

#define OS_TRACE_MSGQ_RECEIVE_EXIT(RetVal)
#define OS_TRACE_MSGQ_SEND_EXIT(RetVal)
#define OS_TRACE_MSGQ_CLEAR_EXIT(RetVal)
#define OS_TRACE_MSGQ_TERMINATE_EXIT(RetVal)
#define OS_TRACE_MSGQ_IS_FULL_EXIT(RetVal)
#define OS_TRACE_MSGQ_IS_EMPTY_EXIT(RetVal)

#endif /* OS_CFG_TRACE_API_EXIT_EN && OS_CFG_TRACE_API_EXIT_EN > 0u */


#ifdef __cplusplus
}
#endif

#endif /* __OS_TRACE_EVENTS_H */

/* End of file */
