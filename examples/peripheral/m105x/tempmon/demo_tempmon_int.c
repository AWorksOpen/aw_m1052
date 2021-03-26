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
 * \brief mcu温度监控例程，mcu温度超出设定温度范围将进入中断
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能AW_DEV_IMX1050_TEMPMON
 *
 * - 实验现象：
 *   1. mcu温度超出设定温度范围将进入中断；
 *   2. 串口打印调试信息。
 *
 * \par 源代码
 * \snippet demo_tempmon_int.c src_tempmon_int
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-16, mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_tempmon_int mcu温度监控例程
 * \copydoc demo_tempmon_int.c
 */

/** [src_tempmon_int] */
#include "aworks.h"         /* this file must be included first */
#include "aw_serial.h"      /* serial device service */
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "driver/tempmon/awbl_imx10xx_tempmon.h"
#include "aw_temp.h"

AW_SEMB_DECL(temp_sem_sync);

/**
 * \brief test tempmon callback
 *
 * \param[in] p_arg     : point to user's input
 * \param[in] int_state : depend on driver
 *
 * \return none
 */
aw_local void __tempmon_callback (void *p_arg, uint8_t int_state)
{
    if (AWBL_IMX10xx_TEMPMON_STATE_TEMP_LOW & int_state) {
        aw_kprintf("entry low temp int,temp limit 70 ~ 100 C\r\n");
    } else if (AWBL_IMX10xx_TEMPMON_STATE_TEMP_HIGH & int_state) {
        aw_kprintf("entry high temp int,temp limit 70 ~ 100 C\r\n");
    }

    AW_SEMB_GIVE(temp_sem_sync);

    /* disable tempmon int */
    awbl_imx10xx_tempmon_int_disable();

    return ;
}

/**
 * \brief tempmon low temp int entry
 */
void demo_tempmon_int_entry (void)
{
    aw_err_t ret = AW_OK;

    aw_kprintf("demo_tempmon_test...\r\n");

    AW_SEMB_INIT(temp_sem_sync, AW_SEM_EMPTY, AW_SEM_Q_FIFO);

    /* set high temperature limit is 100 ℃ */
    ret = awbl_imx10xx_tempmon_limit_temp_set(AWBL_IMX10xx_TEMPMON_LIMIT_TEMP_HIGH, 100000);
    if (AW_OK == ret) {
        aw_kprintf("set high temperature limit is 100 C\r\n");
    }

    /* set low temperature limit is 70 ℃ */
    ret = awbl_imx10xx_tempmon_limit_temp_set(AWBL_IMX10xx_TEMPMON_LIMIT_TEMP_LOW, 70000);
    if (AW_OK == ret) {
        aw_kprintf("set low temperature limit is 70 C\r\n");
    }

    /* connect callback func */
    ret = awbl_imx10xx_tempmon_int_connect(__tempmon_callback, (void *)0);
    if (AW_OK == ret) {
        aw_kprintf("connect callback func OK.\r\n");
    }

    /* enable tempmon int */
    awbl_imx10xx_tempmon_int_enable();

    AW_SEMB_TAKE(temp_sem_sync, AW_SEM_WAIT_FOREVER);

    awbl_imx10xx_tempmon_int_disconnect();

    aw_kprintf("demo_tempmon_low_test OK.\r\n");

    return ;
}

/** [src_tempmon_int] */

/* end of file */
