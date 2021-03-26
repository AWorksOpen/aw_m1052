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
 * \brief 异常标准接口
 *
 * 使用本服务需要包含以头文件 aw_exc.h
 *
 * 本模块提供了操作CPU异常的接口。通常，只应当在系统初始化的时候调用这些接口来
 * 进行异常的连接等操作。异常号由平台定义 。
 *
 * \par 简单示例
 * \code
 * #include "apollo.h"
 * #include "aw_exc.h"
 *
 *  void my_esr(void)
 *  {
 *      //异常处理。。。
 *  }
 *  aw_exc_int_connect(EXCNUM_HARDFALT,
 *                     (aw_pfuncvoid_t)my_esr); //连接HardFalt异常服务函数
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-04  zen, first implementation
 * \endinternal
 */

#ifndef __AW_EXC_H
#define __AW_EXC_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_aw_if_exc
 * \copydoc aw_exc.h
 * @{
 */

#include "aw_common.h"
#include "aw_psp_exc.h"

/**
 * \brief 连接一个C函数到一个CPU异常
 *
 * 本函数连接一个指定的C函数到一个指定的异常上。当异常号为 \a excnum 的异常发生
 * 时，将会在特权模式下调用该C函数，若没有C函数连接，则CPU的执行将停留在
 * 一个 \c while(1) 永久循环中。一个异常只能连接一个C函数，最后连接的C函数将覆盖
 * 前一次连接的C函数。
 *
 * \attention   aw_exc_int_connect() 只应当在系统初始化的时候被调用
 *
 * \param[in]   excnum      异常号，由平台定义
 *
 * \param[in]   pfunc_esr   要调用的C函数(ESR, 异常服务函数)，可以为任何普通的
 *                          C代码，但是，它不能调用某些会阻塞的函数，并且应当
 *                          保证函数的执行尽可能地短
 *
 * \retval      AW_OK       成功
 * \retval     -AW_ENXIO    \a excnum 为不存在的异常号
 * \retval     -AW_EFAULT   \a pfunc_esr 为 NULL
 *
 * \par 示例
 * \code
 *  #include "apollo.h"
 *  #include "aw_exc.h"
 *
 *  void my_esr(void)
 *  {
 *      //异常处理。。。
 *  }
 *  aw_exc_int_connect(EXCNUM_HARDFALT,
 *                     (aw_pfuncvoid_t)my_esr); //连接HardFalt异常服务函数
 * \endcode
 */
aw_err_t aw_exc_int_connect(int excnum, aw_pfuncvoid_t pfunc_esr);

/** @} grp_aw_if_exc */

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#endif	/* __AW_EXC_H */

/* end of file */

