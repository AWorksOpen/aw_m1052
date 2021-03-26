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
 * \brief 继电器操作接口
 *
 * 使用本服务需要包含以下头文件
 * \code
 * #include "aw_relay.h"
 * \endcode
 * 
 * \par 使用示例
 * \code
 * #include "aw_relay.h"
 *
 * aw_relay_on(RELAY1);      // 继电器1闭合
 * aw_relay_off(RELAY1);     // 继电器1断开
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 14-07-30  hbt, first implementation.
 * \endinternal
 */

#ifndef __AW_RELAY_H
#define __AW_RELAY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_relay
 * \copydoc aw_relay.h
 * @{
 */

#include "aw_types.h"


/**
 * \brief 继电器闭合
 * \param id   继电器编号，可用编号由具体平台决定
 */
void aw_relay_on (unsigned int id);

/**
 * \brief 继电器断开
 * \param id   继电器编号，可用编号由具体平台决定
 */
void aw_relay_off (unsigned int id);

/** @} grp_aw_if_relay */

#ifdef __cplusplus
}
#endif

#endif /* __AW_RELAY_H */

/* end of file */
