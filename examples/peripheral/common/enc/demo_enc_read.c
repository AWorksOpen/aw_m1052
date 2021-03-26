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
 * \brief 编码器例程
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能AW_DEV_XXX_ENC
 *
 * - 实验现象：
 *   1. 编码器每转一圈，回到零位时，会产生一个高电平脉冲，此时会触发Index中断。
 *   2. 串口打印调试信息。
 *
 * \par 源代码
 * \snippet demo_enc_read.c src_enc_read
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-16, mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_enc_read 编码器(读)
 * \copydoc demo_enc_read.c
 */

/** [src_enc_read] */
#include "aworks.h"                     /* 此头文件必须被首先包含 */
#include "aw_delay.h"                   /* 延时服务 */
#include "aw_vdebug.h"
#include "aw_prj_params.h"
#include "driver/enc/awbl_enc.h"



/* Index中断次数计数 */
aw_local uint32_t __g_index_conter = 0;

/* 使用的编码器的输入通道 */
#define ENC_CHAN   1

/* 编码器的线数 */
#define ENC_LINE   1000


/* Index中断回调函数 */
aw_local void __index_int_func (void *p_arg)
{
    /*
     * 编码器每转一圈，回到零位时，
     * 会产生一个高电平脉冲，此时会触发Index中断。
     */
    __g_index_conter++;
}

/*
 * /brief 例程入口
 */
void demo_enc_read_entry (void)
{

    aw_err_t ret  = 0;
    int read_cont = 20;
    int position  = 0;
    int16_t hold_position_difference = 0;
    int16_t hold_revolution = 0;

    /* enc初始化并注册中断回调函数 */
    ret = awbl_enc_init(ENC_CHAN, __index_int_func, NULL);
    if (AW_OK != ret) {
        aw_kprintf("awbl_enc_init fail!\r\n");
        return ;
    }

    while (read_cont--) {

        ret = awbl_enc_get_position(ENC_CHAN, &position);
        if (AW_OK != ret) {
            aw_kprintf("awbl_enc_get_position error!\r\n");
            continue ;
        }
        aw_kprintf("enc position is: %d\r\n", position);

        ret = awbl_enc_get_position_difference(ENC_CHAN, &hold_position_difference);
        if (AW_OK != ret) {
            aw_kprintf("awbl_enc_get_position_difference error!\r\n");
            continue ;
        }
        aw_kprintf("enc position difference is: %d\r\n", hold_position_difference);

        ret = awbl_enc_get_revolution(ENC_CHAN, &hold_revolution);
        if (AW_OK != ret) {
            aw_kprintf("awbl_enc_get_revolution error!\r\n");
            continue ;
        }
        aw_kprintf("enc revolution is: %d\r\n", hold_revolution);

        aw_mdelay(200);
    }
}


/** [src_enc_read] */

/* end of file */

