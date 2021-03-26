/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 编码器测速例程
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能AW_DEV_XXXX_ENC
 *
 * - 实验现象：
 *   1. 编码器每转一圈，回到零位时，会产生一个高电平脉冲，此时会触发Index中断。
 *   2. 串口打印调试信息。
 *
 * \par 源代码
 * \snippet demo_enc_speed.c src_enc_speed
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-16, mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_enc_speed 编码器(测速)
 * \copydoc demo_enc_speed.c
 */

/** [src_enc_speed] */


#include "aworks.h"                     /* 此头文件必须被首先包含 */
#include "aw_delay.h"                   /* 延时服务 */
#include "aw_vdebug.h"
#include "aw_prj_params.h"
#include "aw_sem.h"
#include "aw_hwtimer.h"
#include "driver/enc/awbl_enc.h"
#include "aw_demo_config.h"


#define  __TEST_HWTIMER_DRV_NAME   DE_HWTIMER_NAME        /* 定时器名字 */
#define  __TEST_HWTIMER_BUSID      DE_GPTID               /* 定时器所在的总线ID */

/* Index中断次数计数 */
aw_local uint32_t __g_index_conter = 0;

/* 使用的编码器的输入通道 */
#define ENC_CHAN   1

/* 编码器的线数 */
#define ENC_LINE   1000


/** \brief 定义信号量 */
AW_SEMB_DECL_STATIC(__g_semb_intr);

/** \brief 中断服务函数 */
aw_local void gpt_isr (void *p_arg)
{
    AW_SEMB_GIVE(__g_semb_intr);
}

/* Index中断回调函数 */
aw_local void __index_int_func (void *p_arg)
{
    /*
     * 编码器每转一圈，回到零位时，
     * 会产生一个高电平脉冲，此时会触发Index中断。
     */
    __g_index_conter++;
}

void demo_enc_speed_entry (void)
{
    int  position = 0;
    float speed   = 0.0;

    aw_hwtimer_handle_t timer;

    /* 分配名为 __TEST_HWTIMER_DRV_NAME 的定时器 */
    timer = aw_hwtimer_alloc_byname(__TEST_HWTIMER_DRV_NAME,
                                    __TEST_HWTIMER_BUSID,
                                    0,
                                    gpt_isr,
                                    NULL);

    AW_SEMB_INIT(__g_semb_intr, AW_SEM_EMPTY,AW_SEM_FULL);

    /* enc初始化并注册中断回调函数 */
    awbl_enc_init(ENC_CHAN, __index_int_func, NULL);

    /* 使能定时器,设置周期为1s */
    aw_hwtimer_enable(timer, 1);

    while(1) {

        /* 等待定时时间到 */
        AW_SEMB_TAKE(__g_semb_intr, AW_SEM_WAIT_FOREVER);

        /* 获取位置 */
        awbl_enc_get_position(ENC_CHAN, &position);

        /* 计算每分钟转的圈数 */
        speed = __g_index_conter + position / (float)(4 * ENC_LINE);
        speed *= 60;
        __g_index_conter = 0;

        aw_kprintf("Motor speed is %f rpm\r\n", speed);
    }
}


/** [src_enc_speed] */

/* end of file */
