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
 * \brief sx127x CAD模式使用例程
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
 *   该 demo 周期性的开启CAD来检测当前信道是否有数据发送
 *
 * \par 源代码
 * \snippet demo_lora_channel_activity_detect.c src_lora_cad
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-19  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lora_cad lora(CAD模式)
 * \copydoc demo_lora_channel_activity_detect.c
 */

/** [src_lora_cad] */

#include "am_lora_type.h"
#include "am_sx127x.h"
#include "am_lora_utilities.h"
#include "driver/lora/awbl_sx127x.h"
#include "aw_led.h"

/**
 * 参数配置
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
static am_lora_radio_events_t __g_lora_radio_events;

/**
 * CAD完成回调函数
 */
static void __on_cad_done (am_lora_bool_t channelActivityDetected);

/**
 * SX1276操作句柄定义
 */
static am_sx127x_handle_t __g_sx127x_handle;

/**
 * CAD完成标志
 */
static volatile aw_bool_t  __g_sx127x_cad_done = AW_TRUE;

/**
 * LoRa信道活跃测试程序入口
 */
void demo_lora_channel_activity_detect_entry (void)
{
    /* 获取指定 ID 的设备 */
    __g_sx127x_handle = awbl_sx127x_get_handle_by_id(LORA_TEST_DEV_ID);
    if (!__g_sx127x_handle) {
        am_lora_kprintf("awbl_sx127x_get_handle_by_id err!\r\n");
        while(1);
    }

    /* 无线事件初始化 */
    __g_lora_radio_events.pfn_cad_done_cb = __on_cad_done;
    am_sx127x_events_set(__g_sx127x_handle, &__g_lora_radio_events);

    /* 接收信道及参数配置 */
    am_sx127x_channel_set(__g_sx127x_handle, RF_FREQUENCY);
    am_sx127x_rx_config_set(__g_sx127x_handle,
                            MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                            LORA_CODINGRATE, 0, 90,
                            LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                            0, AM_LORA_TRUE, 0, 0, LORA_IQ_INVERSION_ON, AM_LORA_FALSE);

    while (1) {
        if (__g_sx127x_cad_done) {
            /* 启动CAD检测
             *
             * 检测时间：(2^SF + 32)/BW (单位秒)
             *
             */
            __g_sx127x_cad_done = AW_FALSE;
            am_sx127x_cad_start(__g_sx127x_handle);
        }

        am_lora_mdelay(10);
    }
}

/**
 * CAD完成回调函数
 */
static void __on_cad_done (am_lora_bool_t channel_activity_detected)
{
    if (channel_activity_detected == AM_LORA_TRUE) {
        am_lora_kprintf("Channel is active.\r\n");
        aw_led_toggle(LORA_TEST_LED_ID);

    } else {
        /* 检测超时切换到睡眠模式 */
        am_sx127x_sleep(__g_sx127x_handle);
        am_lora_kprintf("Channel is negative.\r\n");
    }

    __g_sx127x_cad_done = AW_TRUE;
}

/** [src_lora_cad] */

/* end of file */
