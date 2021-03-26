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
 * \brief Apollo CLK 接口头文件
 *
 * \internal
 * \par modification history:
 * - 1.01 14-10-22  ehf, add some interface
 * - 1.00 14-05-14  zen, first implementation
 * \endinternal
 */

#ifndef __AW_CLK_H
#define __AW_CLK_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "apollo.h"
#include "aw_cpu_clk.h"


/**
 * \brief 使能时钟
 *
 * \param[in]   clk_id 时钟 ID (由平台定义)
 *
 * \retval      AW_OK       成功
 * \retval      -EINVAL     无效参数
 * \retval      -EIO        使能失败
 */
aw_err_t aw_clk_enable(aw_clk_id_t clk_id);

/**
 * \brief 禁能时钟
 *
 * \param[in]   clk_id 时钟 ID (由平台定义)
 *
 * \retval      AW_OK       成功
 * \retval      -EINVAL     无效参数（时钟或频率不存在）
 * \retval      -EIO        禁能失败
 */
aw_err_t aw_clk_disable(aw_clk_id_t clk_id);

/**
 * \brief 获取时钟频率
 *
 * \param[in]   clk_id 时钟 ID (由平台定义)
 *
 * \retval    0       时钟频率为0，或获取时钟频率失败
 * \retval    正值    时钟频率
 */
aw_clk_rate_t aw_clk_rate_get(aw_clk_id_t clk_id);

/**
 * \brief 设置时钟频率
 *
 * \param[in]   clk_id  时钟 ID (由平台定义)
 * \param[in]   rate    时钟频率
 *
 * \retval    AW_OK       成功
 * \retval    -AW_EINVAL  无效参数（时钟或频率不存在）
 */
aw_err_t aw_clk_rate_set(aw_clk_id_t clk_id, aw_clk_rate_t rate);

/**
 * \brief 获取时钟旁路
 *
 * \param[in]   clk_id 时钟 ID (由平台定义)
 *
 * \retval    正值   源时钟ID（不在时钟范围内则说明不存在）
 */
aw_clk_id_t aw_clk_parent_get(aw_clk_id_t clk_id);

/**
 * \brief 设置时钟旁路
 *
 * \param[in]   clk_id      子时钟 ID      (由平台定义)
 * \param[in]   clk_parent  时钟源ID       (由平台定义)
 *
 * \retval   AW_OK       成功
 * \retval   -AW_EINVAL  无效参数 （时钟不存在或该时钟间不存在可设置的旁路关系）
 */
aw_err_t aw_clk_parent_set(aw_clk_id_t clk_id, aw_clk_id_t clk_parent);

/**
 * \brief 获取时钟引用次数
 *
 * \param[in]   clk_id  时钟 ID (由平台定义)
 *
 * \retval     非负值        引用次数
 * \retval     -AW_EINVAL    无效参数
 */
int aw_clk_usecount_get(aw_clk_id_t clk_id);

#ifdef __cplusplus
}
#endif  /* __cplusplus     */

#endif  /* __AW_CLK_H */

/* end of file */

