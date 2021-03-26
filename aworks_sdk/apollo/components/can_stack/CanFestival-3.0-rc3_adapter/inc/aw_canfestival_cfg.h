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

#ifndef __AW_CANFESTIVAL_CFG_H
#define __AW_CANFESTIVAL_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_canfestival_cfg
 * \copydoc aw_canfestival_cfg.h
 * @{
 */

#include "aw_types.h"
#include "aw_can.h"
#include "aw_hwtimer.h"
#include "canfestival.h"

#define AW_CANFESTIVAL_BRT_LIST_CNT  9 /** \brief 波特率列表个数 */

/** \brief CAN波特率列表 */
typedef struct aw_canfestival_brt_list {

    int chn;                            /**< \brief CAN BUS通道号 */

    struct brt_list {
        int                 val;        /**< \brief CAN波特率值 单位K */
        aw_can_baud_param_t param;      /**< \brief CAN波特率 */
    } brt[AW_CANFESTIVAL_BRT_LIST_CNT];

} aw_canfestival_brt_list_t;

/**
 * \brief CANfestival 初始化
 *
 * \param[in] inst_cnt       实例Canfestival的个数
 * \param[in] recv_task_prio 接收任务的优先级
 * \param[in] p_hwtimer      需要提供一个定时器句柄
 * \param[in] p_chn_brt_list 实例用到的CAN bus通道对应的波特率表
 *
 * \return AW_OK操作成功，其他错误原因参考错误码
 */
aw_err_t aw_canfestival_init (
    int                              inst_cnt,
    int                              recv_task_prio,
    const aw_hwtimer_handle_t        p_hwtimer,
    const aw_canfestival_brt_list_t *p_chn_brt_list);

/**
 * \brief CANfestival 注册定时器
 *
 * \param[in] p_hwtimer 中断参数
 */
void aw_canfestival_timer_register (const aw_hwtimer_handle_t *p_hwtimer);

/**
 * \brief CANfestival 定时器中断服务函数
 *
 * \param[in] p_arg 中断参数
 */
void aw_canfestival_timer_irq (void *p_arg);

/** @} grp_aw_if_canfestival_cfg */

#ifdef __cplusplus
}
#endif

#endif /* __AW_CANFESTIVAL_CFG_H */

/* end of file */
