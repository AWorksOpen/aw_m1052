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

#define __COP_MAX_INST         2             /**< \brief 实例个数 */
#define __COP_RECV_TASK_PRIO   4             /**< \brief 接收任务优先级 */

#define __COP_TIME_NAME        "imx1050_gpt" /**< \brief 定时器名称 */
#define __COP_TIME_UNIT        1             /**< \brief 定时器UNIT */
#define __COP_TIME_ID          0             /**< \brief 定时器ID */

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
};

void aw_canfestival_lib_init (void)
{
    aw_hwtimer_handle_t p_hwtimer;

    p_hwtimer = aw_hwtimer_alloc_byname(__COP_TIME_NAME,
                                        __COP_TIME_UNIT,
                                        __COP_TIME_ID,
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
                        (const aw_hwtimer_handle_t)p_hwtimer,
                        (const aw_canfestival_brt_list_t *)&__g_can_btr_list);

}

/* end of file */
