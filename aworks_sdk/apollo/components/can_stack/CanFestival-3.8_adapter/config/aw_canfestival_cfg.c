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
 * \brief CanFestival 配置
 *
 * \internal
 * \par modification history:
 * - 1.00 18-10-08  anu, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_canfestival_cfg.h"
#include "aw_prj_params.h"

#define __COP_MAX_INST         1             /**< \brief 实例个数 */
#define __COP_RECV_TASK_PRIO   4             /**< \brief 接收任务优先级 */
#define __COP_RPDO_FIFO_CNT    64            /**< \brief 每个RPDO拥有FIFO个数 */

#ifndef AW_COP_CFG_TIMER_NAME
#define AW_COP_CFG_TIMER_NAME        "imx10xx_gpt" /**< \brief 定时器名称 */
#endif

#ifndef AW_COP_CFG_TIMER_UNIT
#define AW_COP_CFG_TIMER_UNIT        1             /**< \brief 定时器UNIT */
#endif

#ifndef AW_COP_CFG_TIMER_ID
#define AW_COP_CFG_TIMER_ID          0             /**< \brief 定时器ID */
#endif

#if __COP_MAX_INST == 0
#error "__COP_MAX_INST  cannot defined 0!"
#endif

#include "aw_canfestival_inst0.c"

#if __COP_MAX_INST >= 2
#include "aw_canfestival_inst1.c"
#endif

static aw_canfestival_data_t *__gp_canfestival_inst_data[__COP_MAX_INST] = {
    &g_canfestival_inst0_data,

#if __COP_MAX_INST >= 2
    &g_canfestival_inst1_data,
#endif

};

static aw_canfestival_brt_list_t  __g_can_btr_list[__COP_MAX_INST] = {
    {
        0,
        {
            {1000, AW_CFG_CAN_BTR_1000K},
            {800 , AW_CFG_CAN_BTR_800K},
            {500 , AW_CFG_CAN_BTR_500K},
            {250 , AW_CFG_CAN_BTR_250K},
            {125 , AW_CFG_CAN_BTR_125K},
            {100 , AW_CFG_CAN_BTR_100K},
            {50  , AW_CFG_CAN_BTR_50K},
            {20  , AW_CFG_CAN_BTR_20K},
            {10  , AW_CFG_CAN_BTR_10K},
        }
    },

#if __COP_MAX_INST >= 2
    {
        1,
        {
            {1000, AW_CFG_CAN_BTR_1000K},
            {800 , AW_CFG_CAN_BTR_800K},
            {500 , AW_CFG_CAN_BTR_500K},
            {250 , AW_CFG_CAN_BTR_250K},
            {125 , AW_CFG_CAN_BTR_125K},
            {100 , AW_CFG_CAN_BTR_100K},
            {50  , AW_CFG_CAN_BTR_50K},
            {20  , AW_CFG_CAN_BTR_20K},
            {10  , AW_CFG_CAN_BTR_10K},
        }
    },
#endif

};

void aw_canfestival_lib_init (void)
{
    aw_hwtimer_handle_t p_hwtimer;

    p_hwtimer = aw_hwtimer_alloc_byname(AW_COP_CFG_TIMER_NAME,
                                        AW_COP_CFG_TIMER_UNIT,
                                        AW_COP_CFG_TIMER_ID,
                                        &aw_canfestival_timer_irq,
                                        NULL);
#if 0 
    __g_timer.p_hwtimer = aw_hwtimer_alloc(1000000,
        0,
        12000000,
        0,
        &__timer_notify,
        &__g_timer);
#endif 

    aw_canfestival_init(__COP_MAX_INST,
                        __COP_RECV_TASK_PRIO,
                        __COP_RPDO_FIFO_CNT,
                        (const aw_hwtimer_handle_t)p_hwtimer,
                        (void **)__gp_canfestival_inst_data,
                        (const aw_canfestival_brt_list_t *)&__g_can_btr_list);

}

/* end of file */
