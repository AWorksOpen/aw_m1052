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
 * \brief 无源蜂鸣器例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_DEV_PWM_BUZZER
 *      - 对应平台的PWM宏
 *      - 对应平台的串口宏
 *      - AW_COM_CONSOLE
 * - 实验现象：
 *   1. 蜂鸣器每隔500ms鸣叫一次。
 *
 * - 备注：
 *   1. 如需修改无源蜂鸣器鸣叫频率，请在awbl_hwconf_buzzer_pwm.h中修改配置信息。
 *
 * \par 源代码
 * \snippet demo_pwm_buzzer.c src_pwm_buzzer
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-04  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_pwm_buzzer 无源蜂鸣器
 * \copydoc demo_pwm_buzzer.c
 */

/** [src_pwm_buzzer] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_buzzer.h"

/**
 * \brief 建立蜂鸣器例程入口
 * \return 无
 */
aw_local void* __task_handle (void *arg)
{
    while(1) {

        aw_buzzer_loud_set(80); /* 设置蜂鸣器鸣叫强度 */
        aw_buzzer_beep(500);    /* 启动蜂鸣器延时500ms */

        aw_buzzer_loud_set(50); /* 设置蜂鸣器鸣叫强度 */
        aw_buzzer_on();         /* 启动蜂鸣器 */
        aw_mdelay(500);         /* 延时500ms */
        aw_buzzer_off();        /* 关闭蜂鸣器 */
        aw_mdelay(500);         /* 延时500ms */
    }

    return 0;
}

/******************************************************************************/
void demo_buzzer_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Buzzer demo",
                         12,
                         1024,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("Buzzer demo task create failed\r\n");
        return;
    }

    if (aw_task_startup(tsk) != AW_OK) {
        aw_task_delete(tsk);
    }
}

/** [src_pwm_buzzer] */

/* end of file */
