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
 * \brief AWBus ZLG600A 接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_zlg600a.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-05-03  win, first implementation
 * \endinternal
 */

#ifndef __AWBL_ZLG600A_UART_H
#define __AWBL_ZLG600A_UART_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_drv_zlg600a_uart
 * \copydetails awbl_zlg600a.h
 * @{
 */

/**
 * \defgroup  grp_awbl_zlg600a_uart_impl 实现相关
 * \todo
 *
 */

/**
 * \defgroup  grp_awbl_zlg600a_uart_hwconf 设备定义/配置
 * \todo
 */

/** @} grp_awbl_drv_zlg600a_uart */

#include "aw_task.h"
#include "aw_sem.h"
#include "awbl_zlg600a.h"

/** \brief PCF8563 驱动名 */
#define AWBL_ZLG600A_UART_NAME          "zlg600a_uart"

#define __ZLG600A_UART_TASK_STACK_SIZE  512

/**
 * \brief ZLG600A 串口设备信息
 */
typedef struct awbl_zlg600a_uart_devinfo {

    /** \brief ZLG600A 服务配置信息  */
    struct awbl_zlg600a_servinfo zlg600a_servinfo;

    uint8_t   frame_fmt;    /**< \brief 默认使用的帧格式 */
    int       baudrate;     /**< \brief 波特率 */
    uint8_t   addr;         /**< \brief 默认传输的地址      */
    uint8_t   mode;         /**< \brief 通信模式 */

    /** \brief 超时（单位ms） */
    int       timeout;

    uint8_t *p_txbuf;      /**< \brief 发送缓冲区 */
    uint32_t txbuf_size;   /**< \brief 发送缓冲区大小 */

    /** 平台相关初始化：开启时钟、初始化引脚等操作 */
    void (*pfunc_plfm_init)(void);
} awbl_zlg600a_uart_devinfo_t;

/**
 * \brief ZLG600A串口设备结构体定义
 */
typedef struct awbl_zlg600a_uart_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev dev;

    /** \brief 卡片服务结构体 */
    awbl_zlg600a_service_t serv;

    uint8_t  addr;         /**< \brief 默认传输的地址      */

} awbl_zlg600a_uart_dev_t;

/**
 * \brief ZLG600A UART driver register
 *        awbl_serail_init()需要在awbl_dev_connect()函数之前初始化
 */
void awbl_zlg600a_uart_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_ZLG600A_UART_H */

/* end of file */
