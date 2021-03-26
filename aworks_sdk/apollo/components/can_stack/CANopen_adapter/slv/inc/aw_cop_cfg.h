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
 * - 1.00 19-12-17  anu, first implementation
 * \endinternal
 */

#ifndef __AW_COP_CFG_H
#define __AW_COP_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_cop_cfg
 * \copydoc aw_cop_cfg.h
 * @{
 */

#include "aw_types.h"
#include "aw_can.h"
#include "aw_hwtimer.h"
#include "aw_cop_slv.h"

#include "copinc.h"

#define AW_COP_BRT_LIST_CNT  9          /**< \brief 波特率列表个数 */

/** \brief CAN波特率列表 */
typedef struct aw_cop_brt_list {

    int chn;                            /**< \brief CAN BUS通道号 */

    struct brt_list {
        aw_cop_baud_t       val;        /**< \brief CAN波特率值 单位K */
        aw_can_baud_param_t param;      /**< \brief CAN波特率 */
    } brt[AW_COP_BRT_LIST_CNT];

} aw_cop_brt_list_t;

/**
 * \brief CANopen 初始化
 *
 * \param[in] inst_cnt       实例CANopen的个数
 * \param[in] p_hwtimer      需要提供一个定时器句柄
 * \param[in] p_chn_brt_list 实例用到的CAN bus通道对应的波特率表
 *
 * \return AW_OK操作成功，其他错误原因参考错误码
 */
aw_err_t aw_cop_init (
    int                         inst_cnt,
    const aw_hwtimer_handle_t   p_hwtimer,
    const aw_cop_brt_list_t    *p_chn_brt_list);

/**
 * \brief CANopen 定时器中断服务函数
 *
 * \param[in] p_arg 中断参数
 */
void aw_cop_timer_irq (void *p_arg);

/** @} grp_aw_if_cop_cfg */

#ifdef __cplusplus
}
#endif

#endif /* __AW_COP_CFG_H */

/* end of file */
