/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#include "aworks.h"
#if CONFIG_RTK_PERFORMANCE_STATISTIC_EN
#include "string.h"
#include "aw_vdebug.h"
#include "aw_system.h"
#include "aw_delay.h"
#include "aw_shell.h"
#include "rtk.h"
#include "aw_timestamp.h"
#include "aw_shell_core.h"
#include "aw_msgq.h"


typedef struct {
    uint32_t timeout;
    struct aw_shell_user *sh;
} timeout_msg_t;
static struct rtk_task_info start_task_infos[64],
                            cur_task_infos[64];
AW_MSGQ_DECL_STATIC(timeout_msgq, 10, sizeof(timeout_msg_t));

static struct rtk_task_info *find_task_info(struct rtk_task_info *p_task_infos,
                                            int                   task_count,
                                            uint32_t              task_id)
{
    int                     i;
    struct rtk_task_info   *p_task_info = NULL;

    for (i = 0; i< task_count;i ++) {
        if (p_task_infos->task_id == task_id) {
            p_task_info = p_task_infos;
            break;
        }
        p_task_infos ++;
    }

    return p_task_info;
}

void*  __cpuload_task(void *p_arg)
{
    int                     i;
    uint64_t                run_time, run_rate;
    uint64_t                start_stamp,stamp;
    int                     old_task_count,new_task_count;
    struct rtk_task_info   *p_cur_task_info, *p_start_task_info;

    struct aw_shell_chart   chart;
    int                     cpu_usage_id, name_id, prio_id, time_id;
    char                    cpu_time_buf[16];
    char                    cpu_usage_buf[16];
    char                    chart_str_buf[16];
    aw_timestamp_freq_t     freq;
    timeout_msg_t           msg;

    while(1) {

        AW_MSGQ_RECEIVE(timeout_msgq,
                        &msg,
                        sizeof(timeout_msg_t),
                        AW_MSGQ_WAIT_FOREVER);

        old_task_count = rtk_enum_all_task_info(start_task_infos,
                                                AW_NELEMENTS(start_task_infos),
                                                NULL);

        start_stamp = rtk_timestamp_get();

        aw_task_delay(msg.timeout);

        stamp = rtk_timestamp_get() - start_stamp;

        new_task_count = rtk_enum_all_task_info(cur_task_infos,
                                                AW_NELEMENTS(cur_task_infos),
                                                NULL);


        aw_shell_chart_init(&chart, msg.sh, AW_SHELL_CHART_ALIGN_MID, 0, '|');

        /* ÉèÖÃitemµÄ±ß¿ò */
        aw_shell_chart_item_separator(&chart, '=', NULL);

        prio_id = aw_shell_chart_item(&chart, "Priority", 8);
        name_id = aw_shell_chart_item(&chart, "Name", 24);
        time_id = aw_shell_chart_item(&chart, "Run time", 14);
        cpu_usage_id = aw_shell_chart_item(&chart, "Usage rate", 12);

        aw_shell_chart_line_separator(&chart, '=', NULL);

        freq = aw_timestamp_freq_get();


        for (i = 0;i < new_task_count;i++ ) {

            p_cur_task_info  = &cur_task_infos[i];

            p_start_task_info = find_task_info(start_task_infos,
                                               old_task_count,
                                               p_cur_task_info->task_id);

            if (NULL == p_start_task_info) {
                continue;
            }

            run_time = p_cur_task_info->total_running_time - p_start_task_info->total_running_time;
            run_rate = run_time * 100000000;
            run_rate /= stamp;

            aw_shell_chart_line(&chart, p_cur_task_info->name, name_id, i);

            aw_snprintf(chart_str_buf, sizeof(chart_str_buf), "%d", p_cur_task_info->prio);
            aw_shell_chart_line(&chart, chart_str_buf, prio_id, i);

            aw_snprintf(cpu_time_buf, sizeof(cpu_time_buf), "%1.3f ms", run_time * 1000.0 / freq);
            aw_shell_chart_line(&chart, cpu_time_buf, time_id, i);

            aw_snprintf(cpu_usage_buf, sizeof(cpu_usage_buf), "%f%%", run_rate / 1000000.0);
            aw_shell_chart_line(&chart, cpu_usage_buf, cpu_usage_id, i);
        }

        aw_shell_chart_line_separator(&chart, '=', NULL);
        aw_shell_printf(msg.sh, "test time: %1.3fms \n\r", stamp * 1000.0 / freq);
    }
}



aw_local aw_err_t __cpuload_cmd (int argc, char *argv[], struct aw_shell_user *sh)
{
    timeout_msg_t msg;
    msg.timeout = 2000;
    msg.sh = sh;

    if (argc == 1) {
        msg.timeout = strtol(argv[0], 0, 0);
    }

    AW_MSGQ_SEND(timeout_msgq,
                 &msg,
                 sizeof(timeout_msg_t),
                 AW_MSGQ_NO_WAIT, AW_MSGQ_PRI_NORMAL);

    return 0;
}


static const struct aw_shell_cmd __g_shell_cmds[] = {
    {__cpuload_cmd,  "cpuload",  "[ms timeout]"},
};

void cpuload_shell_reg (void)
{
    aw_task_id_t task;
    AW_MSGQ_INIT(timeout_msgq, 10, sizeof(timeout_msg_t), AW_MSGQ_Q_FIFO);
    static struct aw_shell_cmd_list _shell;

    (void)AW_SHELL_REGISTER_CMDS(&_shell, __g_shell_cmds);

    task = aw_task_create("cpuload task", 0, 2*1024, __cpuload_task, NULL);
    aw_task_startup(task);
}
/* end of file*/

#endif
