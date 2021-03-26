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
 * \brief OS Trace 的配置文件
 */

#ifndef   __OS_CFG_TRACE_H
#define   __OS_CFG_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  打开或者关闭跟 Trace 相关的全部/部分宏。
 *
 *  配置为 1 打开，为零关闭。
 *
 */
#ifndef OS_CFG_TRACE_EN
#define OS_CFG_TRACE_EN                 1
#endif

#if (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))
/**
 * \brief 是否启用 Trace_Start/Trace_Stop 控制。
 *
 *  配置为 1 打开，为零关闭。
 *
 *  如果想从 rtk_init() 开始记录事件，或者自定义开始记录的位置，
 *  可以打开这个宏，使得 Trace_Start/Trace_Stop 作用。
 */
#ifndef OS_CFG_TRACE_CONTROL_EN
#define OS_CFG_TRACE_CONTROL_EN         1
#endif

/**
 * \brief 是否监视部分 API 的进入。
 *
 *  配置为 1 打开，为零关闭。
 */
#ifndef OS_CFG_TRACE_API_ENTER_EN
#define OS_CFG_TRACE_API_ENTER_EN       1
#endif

/**
 * \brief 是否监视部分 API 的退出。
 *
 *  配置为 1 打开，为零关闭。
 */
#ifndef OS_CFG_TRACE_API_EXIT_EN
#define OS_CFG_TRACE_API_EXIT_EN        1
#endif

#endif  /* (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))*/


#ifdef __cplusplus
}
#endif

#endif /* __OS_CFG_TRACE_H*/

/* End of file  */
