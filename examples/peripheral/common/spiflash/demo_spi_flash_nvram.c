/*******************************************************************************
 *                                 AWorks
 *                       ---------------------------
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
 * \brief 例程（使用NVRAM 接口读写）
 *
 * - 操作步骤:
 *   1. aw_prj_params.h 中使能
 *      - AW_DEV_xxx_FLEXSPI
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3.  aw_nvram_set 和 aw_nvram_get 接口中非易失性存储信息的名字必须与
 *      awbl_hwconf_xxx_flexspi.h 中的存储段名称一致。
 *   4. 若有写保护，需要在对应的配置文件打开写保护
 *
 * - 实验现象:
 *   1. 串口打印操作结果信息。
 *
 * \par 源代码
 * \snippet demo_spi_flash_nvram.c src_spi_flash_nvram
 *
 * \internal
 * \par History
 * - 1.00 17-09-05  deo, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_spi_flash_nvram SPI Flash(NVRAM)
 * \copydoc demo_spi_flash_nvram.c
 */

/** [src_spi_flash_nvram] */
#include "aworks.h"             /* 此头文件必须被首先包含 */

/* 本程序用到了以下服务 */
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_nvram.h"
#include "string.h"
#include "aw_demo_config.h"

#define BUF_SIZE    255                 /* 读写缓冲区大小 */

static uint8_t      __g_buf[BUF_SIZE];  /* 读写缓冲区 */

#define TEST_MEM_NAME       DE_FLEXSPI_NVRAM_NAME         /*存储段名称，根据需求修改*/

/**
 * \brief 使用NVRAM 接口读写 SPI Flash 演示程序
 * \return 无
 */
void demo_spi_flash_nvram_entry (void)
{
    uint32_t i;

    for (i = 0; i < BUF_SIZE; i++) {
        __g_buf[i] = i & 0xFF;
    }

    /* 通过  nvram 接口向 flash 中写入数据 */
    if (aw_nvram_set(TEST_MEM_NAME, 0, (char *)__g_buf, 0, BUF_SIZE) != AW_OK) {
        AW_INFOF(("SPIFLASH NVRAM: Write Failed!\r\n"));
        return ;
    }

    memset(__g_buf, 0, BUF_SIZE);

    /* 通过 nvram 接口从flash中读取数据 */
    if (aw_nvram_get(TEST_MEM_NAME, 0, (char *)__g_buf, 0, BUF_SIZE) != AW_OK) {
        AW_INFOF(("SPIFLASH NVRAM: Write Failed!\r\n"));
        return ;
    }

    for (i = 0; i < BUF_SIZE; i++) {
        if (__g_buf[i] != (i & 0xFF)) {
            AW_INFOF(("SPIFLASH NVRAM: Verify Failed!\r\n"));
            return;
        }
    }

    AW_INFOF(("SPIFLASH NVRAM: Verify Successful!\r\n"));
}

/** [src_spi_flash_nvram] */

/* end of file */
