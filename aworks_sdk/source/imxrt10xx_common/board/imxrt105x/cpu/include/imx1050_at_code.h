/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief 提供将函数定义到指定内存区域中运行的方法
 
 * \par 示例
 * \code
 * #include “imx1050_at_code.h”
 *
 * __IMX1050_RAM_USED__(int test_at_code(int *p_var));
 *
 * void test_at_code(int *p_var)
 * {
 *     *p_var = 0;
 *
 *     return AW_OK;
 * }
 *
 * int main()
 * {
 *    int var;
 *
 *    test_at_code(&var);    //跳转到内部ram中执行
 * }
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 19-07-19  ral, first implementation
 * \endinternal
 */

#ifndef __IMX1050_AT_CODE_H
#define __IMX1050_AT_CODE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus*/

/**
 * \brief 声明一个函数并将其定义到".ram.func"段
 *  通知编译器该函数是有用的，防止编译器在优化时将其去除
 *
 * \param[in] pfunc 函数完整声明
 */
#define __IMX1050_RAM_USED__(pfunc) __attribute__((used, section(".ram.func"))) pfunc

/**
 * \brief 声明一个函数并将其定义到".ram.func"段
 *  通知编译器该函数不进行内联扩展，防止编译器将其展开
 *
 * \param[in] pfunc 函数完整声明
 */
#define __IMX1050_RAM_NOINLINE__(pfunc)  __attribute__((noinline, section(".ram.func"))) pfunc

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX1050_AT_CODE_H */

/* end of file */
