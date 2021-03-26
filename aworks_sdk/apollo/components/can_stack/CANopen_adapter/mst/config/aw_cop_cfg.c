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
 * \brief CANopen 配置
 *
 * \internal
 * \par modification history:
 * - 1.00 18-10-08  anu, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "mst/inc/aw_cop_cfg.h"
#include "aw_prj_params.h"

#include "objdicts_mst/obdcfg.h"

extern tCopKernel PUBLIC AppCbRxPdo (
    CCM_DECL_INSTANCE_HDL_ uint16_t wPdoCommuIndex_p);
extern tCopKernel PUBLIC AppCbTxPdo (
    CCM_DECL_INSTANCE_HDL_ uint16_t wPdoCommuIndex_p);

#define __COP_MAX_INST    COP_MAX_INSTANCES        /**< \brief 实例个数 */

#ifndef AW_COP_CFG_TIMER_NAME
#define AW_COP_CFG_TIMER_NAME        "imx1050_gpt" /**< \brief 定时器名称 */
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

#if __COP_MAX_INST >= 1
#include "aw_cop_mst_inst0.c"
#endif

#if __COP_MAX_INST >= 2
#include "aw_cop_mst_inst1.c"
#endif

#if __COP_MAX_INST >= 3
#error "need add inst file"
#endif

static aw_cop_data_t *__gp_cop_inst_data[__COP_MAX_INST] = {
#if __COP_MAX_INST >= 1
    &g_cop_data_inst0,
#endif

#if __COP_MAX_INST >= 2
    &g_cop_data_inst1,
#endif
};

static aw_cop_brt_list_t  __g_can_btr_list[__COP_MAX_INST] = {
    {
        0,
        {
            {AW_COP_BAUD_1M  , AW_CFG_CAN_BTR_1000K},
            {AW_COP_BAUD_800K, AW_CFG_CAN_BTR_800K},
            {AW_COP_BAUD_500K, AW_CFG_CAN_BTR_500K},
            {AW_COP_BAUD_250K, AW_CFG_CAN_BTR_250K},
            {AW_COP_BAUD_125K, AW_CFG_CAN_BTR_125K},
            {AW_COP_BAUD_100K, AW_CFG_CAN_BTR_100K},
            {AW_COP_BAUD_50K , AW_CFG_CAN_BTR_50K},
            {AW_COP_BAUD_20K , AW_CFG_CAN_BTR_20K},
            {AW_COP_BAUD_10K , AW_CFG_CAN_BTR_10K},
        }
    },

#if __COP_MAX_INST >= 2
    {
        1,
        {
            {AW_COP_BAUD_1M  , AW_CFG_CAN_BTR_1000K},
            {AW_COP_BAUD_800K, AW_CFG_CAN_BTR_800K},
            {AW_COP_BAUD_500K, AW_CFG_CAN_BTR_500K},
            {AW_COP_BAUD_250K, AW_CFG_CAN_BTR_250K},
            {AW_COP_BAUD_125K, AW_CFG_CAN_BTR_125K},
            {AW_COP_BAUD_100K, AW_CFG_CAN_BTR_100K},
            {AW_COP_BAUD_50K , AW_CFG_CAN_BTR_50K},
            {AW_COP_BAUD_20K , AW_CFG_CAN_BTR_20K},
            {AW_COP_BAUD_10K , AW_CFG_CAN_BTR_10K},
        }
    },
#endif
};

void aw_cop_lib_init (void)
{
    aw_hwtimer_handle_t p_hwtimer;

    p_hwtimer = aw_hwtimer_alloc_byname(AW_COP_CFG_TIMER_NAME,
                                        AW_COP_CFG_TIMER_UNIT,
                                        AW_COP_CFG_TIMER_ID,
                                        &aw_cop_timer_irq,
                                        NULL);

    aw_cop_init(__COP_MAX_INST,
                (const aw_hwtimer_handle_t)p_hwtimer,
                (aw_cop_data_t          **)__gp_cop_inst_data,
                (const aw_cop_brt_list_t *)&__g_can_btr_list);

}

/* end of file */
