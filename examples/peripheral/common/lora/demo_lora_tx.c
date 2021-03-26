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
 * \brief sx127x 发送使用例程
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
 *   sx127x 会以500ms为周期定时发送数据
 *
 * \par 源代码
 * \snippet demo_lora_tx.c src_lora_tx
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lora_tx lora(发送)
 * \copydoc demo_lora_tx.c
 */

/** [src_lora_tx] */

#include "am_lora_type.h"
#include "am_sx127x.h"
#include "am_lora_utilities.h"
#include "driver/lora/awbl_sx127x.h"
#include "aw_led.h"

/**
 * 参数配置(部分参数保证发送端与接收端一致)
 */
#define RF_FREQUENCY                                490000000 // Hz
#define TX_OUTPUT_POWER                             17        // 17 dBm
#define TX_TIMEOUT                                  65535     // seconds (MAX value
#define LORA_BANDWIDTH                              1         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       8         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx  8
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
 * 发送完成回调函数
 */
static void __on_tx_done (void);

/**
 * 发送超时回调函数
 */
static void __on_tx_timeout (void);

/**
 * SX127x操作句柄定义
 */
static am_sx127x_handle_t __g_sx127x_handle;

static volatile aw_bool_t  __g_tx_done = AW_FALSE;

/**
 * sx127x 定时发送测试程序入口
 */
void demo_lora_tx_entry (void)
{
    char  str_buf[] = {"sx127x send"};

    /* 获取指定 ID 的设备 */
    __g_sx127x_handle = awbl_sx127x_get_handle_by_id(LORA_TEST_DEV_ID);
    if (!__g_sx127x_handle) {
        am_lora_kprintf("awbl_sx127x_get_handle_by_id err!\r\n");
        while(1);
    }

    /* 无线事件初始化 */
    __g_radio_events.pfn_tx_done_cb    = __on_tx_done;
    __g_radio_events.pfn_tx_timeout_cb = __on_tx_timeout;
    am_sx127x_events_set(__g_sx127x_handle, &__g_radio_events);

    /* 发送信道及参数配置 */
    am_sx127x_channel_set(__g_sx127x_handle, RF_FREQUENCY);
    am_sx127x_tx_config_set(__g_sx127x_handle,
                            MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                            LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                            LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                            AM_LORA_TRUE, 0, 0, LORA_IQ_INVERSION_ON, 3000);

    __g_tx_done = AW_TRUE;

    while (1) {
        if (__g_tx_done) {
            /* 发送数据 */
            am_sx127x_send(__g_sx127x_handle, (uint8_t *)str_buf, strlen(str_buf));
            __g_tx_done = AW_FALSE;
        }
        am_lora_mdelay(500);
    }
}

/**
 * 发送完成回调函数
 */
static void __on_tx_done (void)
{
    am_lora_kprintf("TX Done!\r\n");
    aw_led_toggle(LORA_TEST_LED_ID);
    __g_tx_done = AW_TRUE;
}

/**
 * 发送超时回调函数
 */
static void __on_tx_timeout (void)
{
    am_lora_kprintf("TX Timeout!\r\n");
}

/** [src_lora_tx] */

/* end of file */
