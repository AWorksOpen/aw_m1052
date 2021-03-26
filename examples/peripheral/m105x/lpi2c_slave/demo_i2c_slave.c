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
 * - ��������:
 *   1. ����������aw_prj_params.hͷ�ļ���
 *      - �ر�AW_DEV_IMX1050_LPI2C1
 *      - �򿪴��ڵ��Ժ�
 *   2. ���Ӵ��ڣ��򿪴��ڵ�����������ò�����115200-8-N-1
 *   3. i2c�Ի����̽����յ������ݻط�������
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
 * \brief ��������I2C�ӻ�ģ��eeprom����
 *
 *        ������ͨ��һ��ȫ��g_data����䵱�洢�ռ䣬i2c��������
 *        �����ݱ���ȫ�������У���ȡ�����ݴ�ȫ����������ȡ��
 */
void demo_i2c_slave (void)
{
    aw_i2c_slave_init(I2C_MASTER_SLAVE_ADDR, i2c_callback);

    while (1) {
        ;
    }

}

