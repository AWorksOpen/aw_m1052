/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief SPI主机演示例程（测试SPI的基本读写功能）
 *
 * - 操作步骤:
 *   1. 将主机 SPI 的 MOSI 引脚和 MISO 引脚用杜邦线和从机设备的MOSI、MISO相连；
 *   2. 需在aw_prj_params.h中使能：
 *      - 相应平台的SPI宏
 *      - AW_COM_CONSOLE
 *   3. 禁能其他使用该SPI宏的外设；
 *
 * - 实验现象:
 *   1. 主机通过MISO发送数据，发出的数据从MOSI读回；
 *   2. 主机持续打印发送、接收到的数据信息。
 *
 * \par 源代码
 * \snippet demo_spi_master.c src_spi_master
 *
 * \internal
 * \par History
 * - 1.00 18-06-28  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_spi_master SPI(主机)
 * \copydoc demo_spi_master.c
 */

/** [src_spi_master] */
#include "aworks.h"         /* 此头文件必须被首先包含 */
#include "aw_delay.h"       /* 延时服务 */
#include "aw_spi.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"
#include "aw_demo_config.h"

#define HARD_CS_CFG          1
//#define SOFT_CS_CFG          1 /* 使用软件CS请打开spi配置文件，关闭硬件CS模式*/

/** \brief SPI1的配置 */
#define SPI_DEV_BUSID        DE_SPI_DEV_BUSID       /**< \brief SPI总线 */
#define SPI_DEV_MODE         AW_SPI_CPHA            /**< \brief SPI模式 */
#define SPI_DEV_SPEED        2000000                /**< \brief SPI速度 */
#define SPI_DEV_BPW          8                     /**< \brief SPI数据位数，取值只能是8、16、32 */
#define SPI_TEST_LEN         16                     /**< \brief SPI收发数据量 */
#define SPI_CS_PIN           DE_SPI_CS_PIN          /**< \brief SPI片选引脚，根据具体平台修改 */

struct aw_spi_message       spi_msg;
struct aw_spi_transfer      spi_trans;

#if HARD_CS_CFG
void pfunc_cs (int state){

    return ;
}
#endif

/*
 * \brief 测试SPI主机模式的基本读写功能
 */
void demo_spi_master_entry (void)
{
    aw_kprintf("demo_spi_master test...\n");

    aw_spi_device_t     spi_dev;
    int                 ret;
    int                 i;
#if SPI_DEV_BPW == 8
    uint8_t             read_buf[SPI_TEST_LEN]  = {0};
    uint8_t             write_buf[SPI_TEST_LEN] = {0};
#elif SPI_DEV_BPW == 16
    uint16_t             read_buf[SPI_TEST_LEN]  = {0};
    uint16_t             write_buf[SPI_TEST_LEN] = {0};
#elif SPI_DEV_BPW == 32
    uint32_t             read_buf[SPI_TEST_LEN]  = {0};
    uint32_t             write_buf[SPI_TEST_LEN] = {0};
#endif

#if SOFT_CS_CFG
    int                 cs_pin = SPI_CS_PIN;
    /* 配置SPI之前需先申请CS引脚 */
    aw_gpio_pin_request("spi_demo_cs", &cs_pin, 1);
#endif

    /* 生成 SPI FLASH 设备 */
    aw_spi_mkdev(&spi_dev,
                  SPI_DEV_BUSID,   /* 位于哪条SPI总线上 */
                  SPI_DEV_BPW,     /* 字大小 */
                  SPI_DEV_MODE,    /* SPI 模式 */
                  SPI_DEV_SPEED,   /* 支持的最大速度 */
#if HARD_CS_CFG
                  NULL,            /* 片选引脚 */
                  pfunc_cs);       /* 无自定义的片选控制函数 */
#else
                  cs_pin,          /* 片选引脚 */
                  NULL);           /* 无自定义的片选控制函数 */
#endif
    if (aw_spi_setup(&spi_dev) != AW_OK) {
        aw_kprintf("aw_spi_setup fail!\r\n");
        goto _exit ;
    }


    /* 设置发送数据 */
    for (i = 0; i < SPI_TEST_LEN; i++) {
        write_buf[i] = i;
    }

    memset(read_buf,0,SPI_TEST_LEN);

    while(1){
        /* 传输数据 */
        ret = aw_spi_write_and_read (&spi_dev,
                                     write_buf,
                                     read_buf,
                                     sizeof(write_buf));

        if (ret != AW_OK) {
            aw_kprintf("__spi_trans fail!\r\n");
            goto _exit ;
        }
        aw_kprintf("write_buf:");
        for(i = 0; i < SPI_TEST_LEN;i++){
            aw_kprintf("%d ",write_buf[i]);
        }
        aw_kprintf("\r\n");

        aw_kprintf("read_buf:");
        for(i = 0; i < SPI_TEST_LEN;i++){
            aw_kprintf("%d ",read_buf[i]);
        }
        aw_kprintf("\r\n");

        aw_mdelay(500);
    }

_exit:
#if SOFT_CS_CFG
    /* 测试完成后释放CS引脚 */
    aw_gpio_pin_release(&cs_pin, 1);
#endif
    return ;
}

/** [src_spi_master] */

/*end of file */
