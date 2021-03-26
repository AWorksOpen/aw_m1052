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
 * \brief AWorks 延时标准接口
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "aw_delay.h"
 * \endcode
 *
 * \par 简单示例
 * \code
 *  #include "aworks.h"
 *  #include "aw_delay.h"
 *
 *  aw_mdelay(100);     // 延时100 ms
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 12-07-09  zyr, first implementation
 * \endinternal
 */

#ifndef __AW_DELAY_H
#define __AW_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_delay
 * \copydoc aw_delay.h
 * @{
 */

/*lint ++flb */
#include "stdint.h"
#include "aw_task.h"
#include "aw_psp_delay.h"

/**
 * \brief 延时毫秒
 *
 * \param[in] ms   延时毫秒数
 *
 * \return  none
 */
extern void aw_mdelay(uint32_t ms);

/**
 * \brief 延时微秒
 *
 * \attention 本延时使用CPU周期实现
 *
 * \param[in] us   延时微秒数
 *
 * \return  none
 */
extern void aw_udelay(uint32_t us);

/** @}  grp_aw_if_delay */

#ifdef __cplusplus
}
#endif

#endif /* __AW_DELAY_H */

/* end of file */
