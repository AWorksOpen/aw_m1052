/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief (任务调用 + 浮点计算) 性能测试
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能 对应平台UART宏。
 *
 * - 实验现象：
 *   1. 串口打印计算信息。
 *
 * \par 源代码
 * \snippet demo_vfp_task.c src_vfp_task
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-09  mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_vfp_task 浮点计算性能测试
 * \copydoc demo_vfp_task.c
 */

/** [src_vfp_task] */

#include "aworks.h"
#include "aw_task.h"
#include "aw_system.h"
#include "aw_vdebug.h"
#include "aw_sem.h"
#include "aw_timer.h"

static double cal_ln2 (int cal_count)
{
    register double t, r, N;
    register int    i;

    r = 1;
    t = 0;

    //计算方法 ln2 = 1 - 1/2 + 1/3 - 1/4 + .... + 1/n
    for (i = 1; i < cal_count; i++) {
        N = (double)i + 1;
        t = 1 / (double)N;
        if (i % 2) {
            r -= t;
        } else {
            r += t;
        }
    }
    return r;
}

static double cal_pi (int cal_count)
{
    register double t, r, N, c;
    register int    i;

    r = 0;
    c = 4;
    N = 1;

    //圆周率计算方法 π = 4(1-1/3+1/5-1/7+....1/n)
    for (i = 0; i < cal_count; i++) {
        N = 2 * i + 1;
        t = c / (double)N;
        if (i % 2) {
            r -= t;
        } else {
            r += t;
        }
    }
    return r;
}

AW_SEMB_DECL_STATIC(__g_pi_finish);
AW_SEMB_DECL_STATIC(__g_ln2_finish);

AW_TASK_DECL_STATIC(task_test_pi, 1024);
AW_TASK_DECL_STATIC(task_test_ln2, 1024);


static void task_test_pi_fn (void *param)
{
    double *p = (double *)param;
    double  t;

    t = cal_pi(100000000);
    if (p != NULL) {
        *p = t;
    } else {
        aw_kprintf("%s@%d::%f\n", __func__, __LINE__, t);
    }
    AW_SEMB_GIVE(__g_pi_finish);
}

static void task_test_ln2_fn (void *param)
{
    double *p = (double *)param;

    *p = cal_ln2(100000000);
    AW_SEMB_GIVE(__g_ln2_finish);
}

void demo_vfp_task_entry (void) {

    double                  pi1, pi2, ln1, ln2;
    volatile aw_tick_t      tick;

    //首先计算出pi和ln2正确的值
    tick = aw_sys_tick_get();
    //级数越大计算结果越精确
    pi1  = cal_pi(100000000);
    tick = aw_sys_tick_get() - tick;
    aw_kprintf("pi = %.9f, time = %dms \n", pi1, tick);

    tick = aw_sys_tick_get();
    //级数越大计算结果越精确
    ln1  = cal_ln2(100000000);
    tick = aw_sys_tick_get() - tick;
    aw_kprintf("ln2 = %.9f, time = %dms \n", ln1, tick);

    AW_SEMB_INIT(__g_pi_finish, AW_SEM_EMPTY, AW_SEM_Q_FIFO);
    AW_SEMB_INIT(__g_ln2_finish, AW_SEM_EMPTY, AW_SEM_Q_FIFO);

    //开启两个任务同时计算pi和ln2
    AW_TASK_INIT(task_test_pi,
                "task_test_pi",
                 10,
                 1024,
                 task_test_pi_fn,
                 &pi2);

    AW_TASK_INIT(task_test_ln2,
                "task_test_ln2",
                 10,
                 1024,
                 task_test_ln2_fn,
                 &ln2);

    AW_TASK_STARTUP(task_test_pi);

    AW_TASK_STARTUP(task_test_ln2);

    AW_SEMB_TAKE(__g_ln2_finish, AW_SEM_WAIT_FOREVER);
    AW_SEMB_TAKE(__g_pi_finish, AW_SEM_WAIT_FOREVER);

    if (pi2 == pi1) {
        aw_kprintf("pi1 and pi2 is equal.\r\n");
    }
    if (ln2 == ln1) {
        aw_kprintf("ln1 and ln2 is equal.\r\n");
    }

    return ;
}


/** [src_vfp_task] */

/* end of file */
