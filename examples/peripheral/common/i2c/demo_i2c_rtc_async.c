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
 * \brief 使用I2C1(异步传输方式)例程
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
 * \snippet demo_i2c_rtc_async.c src_i2c_rtc_async
 *
 * \internal
 * \par History
 * - 1.00 17-09-01  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_i2c_rtc_async I2C(异步传输)
 * \copydoc demo_i2c_rtc_async.c
 */

/** [src_i2c_rtc_async] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_i2c.h"
#include "aw_vdebug.h"
#include "string.h"
#include "aw_sem.h"
#include "aw_demo_config.h"

#define I2C_SLAVE_ADDR        0X51           /* 从机地址(用户可查看具体I2C从机的配置文件，可根据配置文件修改从机地址) */
#define I2C_MASTER_BUSID      DE_I2C_ID      /* 从机使用I2C1(用户可查看具体I2C从机的配置文件，根据配置文件修改I2C总线) */

#define SLAVE_REG_SEC         0x02           /* 器件内子地址，从此地址开始写入数据(用户可根据具体的I2C从机设备修改) */

AW_SEMB_DECL_STATIC(sem_async);


static void __rtc_callback (void *p_arg)
{
    AW_SEMB_GIVE(sem_async);          /* 释放信号量 */
}

/**
 * \brief I2C异步写数据
 */
static aw_bool_t __aw_i2c_async_write (aw_i2c_device_t *i2c_slave,
                                       uint8_t         *data,
                                       size_t           size)
{
    aw_err_t   ret = AW_OK;

    aw_i2c_message_t  msg;
    aw_i2c_transfer_t trans[2];
    uint8_t           subaddr = SLAVE_REG_SEC;

    aw_i2c_mktrans(&trans[0],
                   I2C_SLAVE_ADDR,                  /* 目标器件地址 */
                   AW_I2C_M_7BIT | AW_I2C_M_WR,     /* 7-bit器件地址, 写操作 */
                   &subaddr,                        /* 子地址 */
                   1);                              /* 子地址长度，1byte */

    aw_i2c_mktrans(&trans[1],
                   I2C_SLAVE_ADDR,                  /* 目标器件地址 (将被忽略) */
                   AW_I2C_M_WR | AW_I2C_M_NOSTART,  /* 写操作, 不发送起始信号及从机地址 */
                   &data[0],                        /* 数据缓冲区 */
                   size);                           /* 数据个数 */

    aw_i2c_mkmsg(&msg,
                 &trans[0],
                 2,
                 __rtc_callback,
                 (void *)0);

    /* 处理此消息，异步 */
    ret = aw_i2c_async(I2C_MASTER_BUSID, &msg);
    if (ret != AW_OK) {
        return ret;
    }

    /* 等待消息处理完成 */
    ret = aw_semb_take(&sem_async, AW_SEM_WAIT_FOREVER);
    if (ret != AW_OK) {
        return ret;
    }

    return AW_TRUE;
}

/**
 * \brief I2C异步读数据
 */
static aw_bool_t __aw_i2c_async_read (aw_i2c_device_t  *i2c_slave,
                                      uint8_t          *data,
                                      size_t            size)
{
    aw_err_t    ret = AW_OK;

    aw_i2c_message_t  msg;
    aw_i2c_transfer_t trans[2];
    uint8_t           subaddr = SLAVE_REG_SEC;

    aw_i2c_mktrans(&trans[0],
                   I2C_SLAVE_ADDR,                  /* 目标器件地址 */
                   AW_I2C_M_7BIT | AW_I2C_M_WR,     /* 7-bit器件地址, 写操作 */
                   &subaddr,                        /* 子地址 */
                   1);                              /* 子地址长度，1byte */

    aw_i2c_mktrans(&trans[1],
                   I2C_SLAVE_ADDR,                  /* 目标器件地址 (将被忽略) */
                   AW_I2C_M_RD | AW_I2C_M_NOSTART,  /* 读操作, 不发送起始信号及从机地址 */
                   &data[0],                        /* 数据缓冲区 */
                   size);                           /* 数据个数 */

    aw_i2c_mkmsg(&msg,
                 &trans[0],
                 2,
                 __rtc_callback,
                 (void *)0);

    /* 处理此消息，异步 */
    ret = aw_i2c_async(I2C_MASTER_BUSID, &msg);
    if (ret != AW_OK) {
        return AW_FALSE;
    }

    /* 等待消息处理完成 */
    ret = aw_semb_take(&sem_async, AW_SEM_WAIT_FOREVER);
    if (ret != AW_OK) {
        return AW_FALSE;
    }

    return AW_TRUE;
}

/**
 * \brief I2C 异步读写 demo
 * \return 无
 */
void demo_i2c_rtc_async_entry (void)
{
    aw_i2c_device_t     i2c_slave;
    uint8_t             wr_data[10];
    uint8_t             rd_data[10];
    int                 i = 0;
    int                 ret;

    AW_SEMB_INIT(sem_async, 0, AW_SEM_Q_PRIORITY);

    aw_kprintf("I2C_ASYNC_Test:\r\n");

    aw_i2c_mkdev(&i2c_slave,
                 I2C_MASTER_BUSID,
                 I2C_SLAVE_ADDR,
                 AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);

    /* 清除缓存区 */
    memset(wr_data, 1, 10);
    memset(rd_data, 1, 10);

    /* 填充数据 */
    for (i = 0; i < 10; i++) {
        wr_data[i] = i;
    }

    /* 异步写数据 */
    ret = __aw_i2c_async_write(&i2c_slave, &wr_data[0], 10);
    if (ret == AW_FALSE) {
        aw_kprintf("aw_i2c_write fail:%d\r\n", ret);
        return ;
    } else {
        aw_kprintf("aw_i2c_write OK!\r\n");
    }

    /* 异步读数据 */
    ret = __aw_i2c_async_read(&i2c_slave, &rd_data[0], 10);
    if (ret == AW_FALSE) {
        aw_kprintf("aw_i2c_read fail:%d\r\n", ret);
        return ;
    } else {
        aw_kprintf("aw_i2c_read OK!\r\n");
    }

    /* 数据校验 */
    ret = strcmp((const char * )wr_data, (const char *)rd_data);
    if (ret != 0) {
        aw_kprintf("verify fail:%d\r\n", ret);
    } else {
        aw_kprintf("verify OK!\r\n");
    }

    return ;
}

/** [src_i2c_rtc_async] */

/* end of file */
