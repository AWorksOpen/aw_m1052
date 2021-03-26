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
 * \file  rtk.h
 * \brief rtk is light weigh and powerful real time scheduler since 2012.
 *
 * \internal
 * \par modification history:
 * - 2.02.1110 17-11-10  imp, add OS_TRACE instrumentation;remove RTK_TRACE().
 * - 2.01.1209 15-12-09  dcf, add RTK_TRACE(); fix bug: msgq_send() always return 0.
 * - 2.00.1117 15-11-17  dcf, add semc_broadcast,
 *                              add members to rtk_task,
 *                              support task event callback on exit.
 *                              support thread local storage.
 * - 1.02.0    15-08-26  dcf, rewrite msgq, add mbox; fix software timer bug; 
 *                              Support newlib reent.
 * - 1.00.14433 14-11-19 dcf, support task_errno_set/get.
 * - 0.9        12-08-01 dcf first version.
 * \endinternal
 */

#ifndef __RTK_H
/* rtk header always define this macro */
#define __RTK_H 1

#define __RTK_VERSION__ ( (2<<24)  | (0<<16) | (1117<<0))
#define __RTK_VERSION_STR__ "3.00.1117"

#ifdef __cplusplus
extern "C" {
#endif       /* __cplusplus  */

#include "rtk_config.h"
#include <stdint.h>
#include "rtk_list.h"
#include "rtk_timestamp.h"
#include "rtk_wait_queue_type_def.h"
#include "rtk_task_type_def.h"
#include "rtk_arch.h"
#include "rtk_base_mutex.h"
#include "rtk_semb.h"
#include "rtk_semc.h"
#include "rtk_msgq.h"
#include "rtk_tick_down_counter.h"
#include "rtk_priority_q.h"
#include "rtk_task.h"
#include "rtk_mutex.h"
#include "rtk_int.h"
#include "rtk_priority_count.h"


/**
 * \brief kernel initialize
 *
 * kernel_init() must be called first of all.
 */
void rtk_init( void );



/**
 *  \brief starting up operating system.
 *
 *  calling this function will start the scheduler, and switch to
 *  the highest priority task created. So, task must be created and started up
 *  before calling this function.
 */
void rtk_startup( void );



typedef struct {

    void (*pfRecordEnterISR)      (void);
    void (*pfRecordExitISR)       (void);
    void (*pfRecordExitISRToSched)(void);

    void (*pfRecordTaskInfo)      (struct rtk_task* pTask);
    void (*pfRecordTaskCreate)    (struct rtk_task* pTask);
    void (*pfRecordTaskTerminate) (struct rtk_task* pTask);

    void (*pfRecordTaskStartExec) (struct rtk_task* pTask);
    void (*pfRecordTaskStopExec)  (void);
    void (*pfRecordTaskStartReady)(struct rtk_task* pTask);
    void (*pfRecordTaskStopReady) (struct rtk_task* pTask, unsigned int Reason);

    void (*pfRecordStart)         (void);
    void (*pfRecordStop)          (void);

    void (*pfRecordVoid)          (unsigned int EventID);
    void (*pfRecordU32)           (unsigned int EventID, unsigned long para0);
    void (*pfRecordU32x2)         (unsigned int EventID, 
                                   unsigned long para0, 
                                   unsigned long para1);
    void (*pfRecordU32x3)         (unsigned int EventID, 
                                   unsigned long para0, 
                                   unsigned long para1, 
                                   unsigned long para2);
    void (*pfRecordU32x4)         (unsigned int EventID, 
                                   unsigned long para0, 
                                   unsigned long para1, 
                                   unsigned long para2, 
                                   unsigned long para3);
    void (*pfRecordEndCall)       (unsigned int EventID);
    void (*pfRecordEndCallU32)    (unsigned int EventID, unsigned long retval);

    unsigned long (*pfShrinkId) (unsigned long ID);

    void (*pfRecordEnterTimer)    (unsigned long TimerId);
    void (*pfRecordExitTimer)     (void);

} OS_TRACE_API;

void OS_SetTraceAPI(const OS_TRACE_API *p_trace);


#ifdef __cplusplus
}
#endif /*  __cplusplus */


#endif

