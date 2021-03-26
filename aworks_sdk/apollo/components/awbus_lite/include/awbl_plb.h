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
 * \brief AWBus PLB总线接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_plb.h"
 * \endcode
 * 本模块为 PLB 总线的精简版实现
 *
 * 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-21  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_PLB_H
#define __AWBL_PLB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \biref PLB bus 驱动注册信息结构体
 *
 * \see struct awbl_drvinfo
 */
typedef struct awbl_plb_drvinfo{
    struct awbl_drvinfo super;
} awbl_plb_drvinfo_t;

/**
 * \brief AWBus PLB 总线注册函数
 */
int awbl_plb_register(void);

/**
 * \brief AWBus PLB 总线初始化第一阶段
 *
 * 第一阶段，AWBus 在实例树中查找 PLB 总线设备，并执行实例的第一阶段初始化函数。
 * 驱动在此阶段中只做静默设备的操作。
 */
int awbl_plb_init1(struct awbl_dev *p_plbdev);

#ifdef __cplusplus
}
#endif  /* __cplusplus 	*/

#endif  /* __AWBL_PLB_H */

/* end of file */
