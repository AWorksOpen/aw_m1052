/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief sx127x 单次接收模式使用例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_DEV_SX127X (具体模块使用的SPI、GPIO引脚，请查看awbus_hwconf_sx127x.h)
 *      - AW_COM_CONSOLE
 *      - AW_DEV_GPIO_LED
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象
 *   sx127x 会以100ms为周期开启一个100ms的接收窗口接收数据
 *
 * \par 源代码
 * \snippet demo_lora_rx_timeout.c src_lora_rx_timeout
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lora_rx_timeout lora(单次接收)
 * \copydoc demo_lora_rx_timeout.c
 */

/** [src_lora_rx_timeout] */

#include "am_lora_type.h"
#include "am_sx127x.h"
#include "am_lora_utilities.h"
#include "driver/lora/awbl_sx127x.h"
#include "aw_led.h"

/**
 * 参数配置(部分参数保证发送端与接收端一致)
 */
#define RF_FREQUENCY                                490000000 // Hz
#define LORA_BANDWIDTH                              1         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       8         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_FIX_LENGTH_PAYLOAD_ON                  AM_LORA_FALSE
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_IQ_INVERSION_ON                        AM_LORA_FALSE

#define LORA_TEST_LED_ID       0
#define LORA_TEST_DEV_ID       0

/**
 * 无线事件
 */
static am_lora_radio_events_t __g_radio_events;

/**
 * 接收完成回调函数
 */
static void __on_rx_done (uint8_t *p_payload, uint16_t size, int16_t rssi, int8_t snr);

/**
 * 接收超时回调函数
 */
static void __on_rx_timeout (void);

/**
 * 接收错误回调函数
 */
static void __on_rx_error (void);

/**
 * SX127x操作句柄定义
 */
static am_sx127x_handle_t __g_sx127x_handle;

/**
 * sx127x 单次接收测试程序入口
 */
void demo_lora_rx_timeout_entry (void)
{
    /* 获取指定 ID 的设备 */
    __g_sx127x_handle = awbl_sx127x_get_handle_by_id(LORA_TEST_DEV_ID);
    if (!__g_sx127x_handle) {
        am_lora_kprintf("awbl_sx127x_get_handle_by_id err!\r\n");
        while(1);
    }

    /* 无线事件初始化 */
    __g_radio_events.pfn_rx_done_cb    = __on_rx_done;
    __g_radio_events.pfn_rx_timeout_cb = __on_rx_timeout;
    __g_radio_events.pfn_rx_error_cb   = __on_rx_error;
    am_sx127x_events_set(__g_sx127x_handle, &__g_radio_events);

    /* 接收信道及参数配置 */
    am_sx127x_channel_set(__g_sx127x_handle, RF_FREQUENCY);
    am_sx127x_rx_config_set(__g_sx127x_handle,
                            MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                            LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                            LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                            0, AM_LORA_TRUE, 0, 0, LORA_IQ_INVERSION_ON, AM_LORA_FALSE);

    /* 设置单次接收模式 */
    am_sx127x_rx_continuous_set(__g_sx127x_handle, AM_LORA_FALSE);
    /* 设置单次接收模式超时时间 */
    am_sx127x_symb_timeout_set(__g_sx127x_handle, 100);

    while (1) {
        /* 启动超时接收（非阻塞）*/
        am_sx127x_rx(__g_sx127x_handle, 0);
        am_lora_mdelay(100);
    }
}

/**
 * 接收完成回调函数
 */
static void __on_rx_done (uint8_t *p_payload, uint16_t size, int16_t rssi, int8_t snr)
{
    char temp_buffer[256] = {0};

    /* 单次接收完成切换操作模式到睡眠模式 */
    am_sx127x_sleep(__g_sx127x_handle);

    memset(temp_buffer, 0, sizeof(temp_buffer));
    memcpy(temp_buffer, p_payload, size);
    am_lora_kprintf("rssi is %d, snr is %d.\r\n", rssi, snr);
    am_lora_kprintf("payload is %s\r\n", temp_buffer);

    aw_led_toggle(LORA_TEST_LED_ID);
}

/**
 * 接收超时回调函数
 */
static void __on_rx_timeout (void)
{
    /* 接收超时切换操作模式到睡眠模式 */
    am_sx127x_sleep(__g_sx127x_handle);
    am_lora_kprintf("RX timeout!\r\n");
}

/**
 * 接收错误回调函数
 */
static void __on_rx_error (void)
{
    /* 接收错误切换操作模式到睡眠模式 */
    am_sx127x_sleep(__g_sx127x_handle);
    am_lora_kprintf("RX error!\r\n");
}

/** [src_lora_rx_timeout] */

/* end of file */
