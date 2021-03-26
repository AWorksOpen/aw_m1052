/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx FlexIO UART
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-02  pea, first implementation
 * \endinternal
 */

#ifndef __IMX10xx_FLEXIO_UART_H
#define __IMX10xx_FLEXIO_UART_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "aw_serial.h"
#include "aw_clk.h"
#include "awbl_sio.h"

#define AWBL_IMX10XX_FLEXIO_UART_DRV_NAME       "imx10xx_flexio_uart"

/** \brief iMX10xx FlexIO UART 设备通道 */
typedef struct awbl_imx10xx_flexio_uart_chan {

    struct aw_sio_chan sio_chan;           /**< \brief 标准串行通道 */
    uint8_t            channel_no;         /**< \brief 通道号 */
    uint8_t            channel_mode;       /**< \brief 中断模式或者轮询模式 */
    uint16_t           options;            /**< \brief 硬件配置信息 */

    aw_bool_t          is_tx_int_en;       /**< \brief 是否使能发送中断 */
    uint32_t           baudrate;           /**< \brief 波特率 */

    void *txget_arg;                       /**< \brief 发送数据获取回调函数参数 */
    void *rxput_arg;                       /**< \brief 接收数据设置回调函数参数 */
    int (*pfn_txchar_get)(void *, char *); /**< \brief 发送数据获取回调函数 */
    int (*pfn_rxchar_put)(void *, char );  /**< \brief 接收数据设置回调函数 */

    aw_spinlock_isr_t dev_lock;            /**< \brief 自旋锁 */

    struct awbl_imx10xx_flexio_uart_chan *p_next; /**< \brief 下一个通道 */

} awbl_imx10xx_flexio_uart_chan_t;

/** \brief 设备配置信息 */
typedef struct awbl_imx10xx_flexio_uart_devinfo {

    int             inum;                /**< \brief Actual architectural vector */
    uint32_t        regbase;             /**< \brief Virt address of register set */
    aw_clk_id_t     async_parent_clk_id; /**< \brief clk id */
    aw_bool_t       is_doze_en;          /**< \brief 是否使能doze */
    aw_bool_t       is_debug_en;         /**< \brief 是否使能debug */
    aw_bool_t       is_fast_access_en;   /**< \brief 是否使能fast_access */
    uint8_t         tx_pin_index;        /**< \brief TX 引脚号 */
    uint8_t         rx_pin_index;        /**< \brief RX 引脚号 */
    uint8_t         shifter_index[2];    /**< \brief 使用的Shifter号 */
    uint8_t         timer_index[2];      /**< \brief 使用的Timer号 */

    /** \brief 平台初始化函数 */
    void            (*pfn_plfm_init)(void);

} awbl_imx10xx_flexio_uart_dev_info_t;

/** \brief 设备实例 */
typedef struct awbl_imx10xx_flexio_uart_dev {

    /** \brief AWBus 设备实例结构体 */
    struct awbl_dev                      dev;

    /** \brief FlexIO UART 通道 */
    struct awbl_imx10xx_flexio_uart_chan chan;

} awbl_imx10xx_flexio_uart_dev_t;

/** \brief Flex UART驱动注册 */
void awbl_imx10xx_flexio_uart_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __IMX10xx_FLEXIO_UART_H */

/* end of file */
