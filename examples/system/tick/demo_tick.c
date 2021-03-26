/*******************************************************************************
*                                 AWorks
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
 * \brief 利用操作系统节拍服务测量程序耗时的例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   
 * - 实验现象：
 *   1. 打印程序耗时等相关信息。
 
 * - 说明：
 *   测量程序耗时是在操作系统的节拍下测量的，因此时间精度只能为ms级。
 *
 * \par 源代码
 * \snippet demo_tick.c src_tick
 * 
 * \internal
 * \par Modification history
 * - 1.00 16-03-26  cod, first implementation.
 * \endinternal
 */
 
/**
 * \addtogroup demo_if_tick 系统节拍服务
 * \copydoc demo_tick.c
 */

/** [src_tick] */

/* 使用节拍服务只需要包含 "aworks.h"头文件 */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"


/** 
 * \brief tick demo 入口函数
 *
 * \return 无
 */
void demo_tick_entry (void)
{
    unsigned int tick;
    unsigned int mtime;
    unsigned int clkrate;
 
    /*
     * 系统的节拍是操作系统的"心脏".
     * 系统的节拍与时间的关系，可由系统节拍的频率得到。系统的节拍频率在：
     * aw_prj_param.h文件中的系统配置中设置。默认设置为：
     *        #define AW_CFG_TICKS_PER_SECOND          1000
     * 如设置为1000,即系统节拍频率为1000Hz，一个节拍就为 1 ms.
     * 系统提供了接口函数获取节拍频率。
     */

    clkrate = aw_sys_clkrate_get();  /* 获取系统的节拍频率 */

    tick = aw_sys_tick_get();        /* 获取当前系统的节拍 */

    aw_mdelay(500);                  /* 可以放入需要测量时间的代码 */

    tick = aw_sys_tick_get() - tick; /* 当前节拍减去上次的节拍 */

    mtime = aw_ticks_to_ms(tick);    /* 将节拍转换为时间 */

    aw_kprintf("system tick: %4d, use ms: %4d", clkrate, mtime);
}

/** [src_tick] */

/* end of file */
