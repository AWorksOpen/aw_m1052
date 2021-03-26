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
 * \brief Apollo标准定义头文件
 *
 * 在所有需要使用Apollo的任何服务的模块(*.<b>c</b>文件)中，该文件必须被直接
 * 或间接地包含。
 *
 * \internal
 * \par modification history:
 * - 1.00 12-07-16  zen, first implementation
 * \endinternal
 */

#ifndef __APOLLO_H
#define __APOLLO_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_common.h"  /* 标准定义头文件 */

/**
 * \brief 声明用户入口
 *
 * \note 固定为 C 函数，允许用户将此函数定义在C++文件中
 *
 */
int aw_main( void );

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __APOLLO_H */

/* end of file */
