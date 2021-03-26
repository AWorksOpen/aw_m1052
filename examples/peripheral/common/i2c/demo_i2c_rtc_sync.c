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
 * \brief 使用I2C1(同步传输方式)例程
 *
 * - 操作步骤:
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *      - 对应平台的I2C宏（如：AW_DEV_xxx_LPI2Cx）
 *      - 对应平台的串口宏
 *   2. 连接串口，打开串口调试软件，设置波特率115200-8-N-1。
 *
 * - 实验现象:
 *   1.串口打印读写数据信息。
 *
 * \par 源代码
 * \snippet demo_i2c_rtc_sync.c src_i2c_rtc_sync
 *
 * \internal
 * \par History
 * - 1.00 18-06-27  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_i2c_rtc_sync I2C(同步传输)
 * \copydoc demo_i2c_rtc_sync.c
 */

/** [src_i2c_rtc_sync] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_i2c.h"
#include "aw_vdebug.h"
#include "string.h"
#include "aw_demo_config.h"

#define I2C_SLAVE_ADDR        0x7EU           /* 从机地址(用户可查看具体I2C从机的配置文件，可根据配置文件修改从机地址) */
#define I2C_MASTER_BUSID      DE_I2C_ID      /* 从机使用I2C1(用户可查看具体I2C从机的配置文件，根据配置文件修改I2C总线) */

#define SLAVE_REG_SEC         0x02           /* 器件内子地址，从此地址开始写入数据(用户可根据具体的I2C从机设备修改) */

/**
 * \brief I2C 同步读写 demo
 * \return 无
 */
void demo_i2c_rtc_sync_entry (void)
{
    aw_i2c_device_t     i2c_slave;
    uint8_t             wr_data[10];
    uint8_t             rd_data[10];
    int                 i = 0;
    int                 ret;

    aw_kprintf("I2C_SYNC_Test:\r\n");

    aw_i2c_mkdev(&i2c_slave,
                 I2C_MASTER_BUSID,
                 I2C_SLAVE_ADDR,
                 AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);

    /* 清除缓存区 */
    memset(wr_data, 1, 10);
    memset(rd_data, 1, 10);

    for(i = 0; i < 10; i++) {
        wr_data[i] = i;
    }

    /* 写数据到从机设备 */
    ret = aw_i2c_write(&i2c_slave, SLAVE_REG_SEC, &wr_data[0], 10);
    if (ret != AW_OK) {
        aw_kprintf("aw_i2c_write fail:%d\r\n", ret);
        return ;
    } else {
        aw_kprintf("aw_i2c_write OK!\r\n");
    }

    /* 从从机设备中读取数据 */
    ret = aw_i2c_read(&i2c_slave, SLAVE_REG_SEC, &rd_data[0], 10);
    if (ret != AW_OK) {
        aw_kprintf("aw_i2c_read fail:%d\r\n", ret);
        return ;
    } else {
        aw_kprintf("aw_i2c_read OK!\r\n");
    }

    /* 数据校验 */
    ret = strcmp((const char * )wr_data, (const char *)rd_data);
    if (ret != AW_OK) {
        aw_kprintf("verify fail:%d\r\n", ret);
    } else {
        aw_kprintf("verify OK!\r\n");
    }

    return ;
}

/** [src_i2c_rtc_sync] */

/* end of file */
