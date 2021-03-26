/*******************************************************************************
*                                 AWorks
*
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief demo i2c slave
 * - 操作步骤:
 *   1. 本例程需在aw_prj_params.h头文件里
 *      - 关闭AW_DEV_IMX1050_LPI2C1
 *      - 打开串口调试宏
 *   2. 连接串口，打开串口调试软件，设置波特率115200-8-N-1
 *   3. i2c丛机例程将接收到的数据回发给主机
 *
 *
 * \internal
 * \par modification history:
 * - 1.00 20-04-14  zq, first implemetation
 * \endinternal
 */
#include "aworks.h"
#include "aw_imx10xx_lpi2c_slave.h"

#define I2C_MASTER_SLAVE_ADDR      0x7EU
#define DATA_LEN_MAX               30

uint8_t g_data[DATA_LEN_MAX] = {0};
static uint16_t __g_rx_index = 0, __g_tx_index = 1;

void i2c_callback (i2c_slave_event_t event, uint8_t *p_data)
{
    switch(event){

        case SlaveTransmitEvent:
            *p_data = g_data[__g_tx_index];
            __g_tx_index = (__g_tx_index + 1) % DATA_LEN_MAX;
            break;

        case SlaveReceiveEvent:
            g_data[__g_rx_index] = *p_data;
            __g_rx_index = (__g_rx_index + 1) % DATA_LEN_MAX;
            break;

        case SlaveCompletionEvent:
            __g_rx_index = 0;
            __g_tx_index = 1;
            break;

        default :
            break;
    }
}

/**
 * \brief 本例程是I2C从机模拟eeprom例程
 *
 *        该例程通过一个全局g_data数组充当存储空间，i2c主机发送
 *        的数据保存全局数组中，读取的数据从全局数组中提取。
 */
void demo_i2c_slave (void)
{
    aw_i2c_slave_init(I2C_MASTER_SLAVE_ADDR, i2c_callback);

    while (1) {
        ;
    }

}

