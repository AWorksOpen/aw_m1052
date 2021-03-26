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
 * \brief 非易失性存储器接口例程（使用NVRAM 接口读写 SPI Flash 演示程序）
 *
 * - 操作步骤:
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - FLASH 设备宏
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象:
 *   1. 串口打印操作结果信息。
 *
 * \par 源代码
 * \snippet demo_nvram.c src_nvram
 *
 * \internal
 * \par History
 * - 1.00 17-09-05  deo, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_nvram NVRAM
 * \copydoc demo_nvram.c
 */

/** [src_nvram] */
#include "aworks.h"             /* 此头文件必须被首先包含 */

/* 本程序用到了以下服务 */
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_nvram.h"
#include "string.h"

#define TEST_MEM_NAME       "user_data"     /* 存储段名称，根据需求修改 */

#define BUF_SIZE            255             /* 读写缓冲区大小 */

static uint8_t __g_buf[BUF_SIZE];           /* 读写缓冲区 */

/**
 * \brief 使用NVRAM 接口读写 SPI Flash 演示程序
 * \return 无
 */
void demo_nvram_entry (void)
{
    uint32_t i;

    for (i = 0; i < BUF_SIZE; i++) {
        __g_buf[i] = i & 0xFF;
    }

    /* 设置 nvram 段 flexspi_flash 数据 */
    if (aw_nvram_set(TEST_MEM_NAME, 0, __g_buf, 0, BUF_SIZE) != AW_OK) {
        AW_INFOF(("SPIFLASH NVRAM: Write Failed!\r\n"));
        return ;
    }

    memset(__g_buf, 0, BUF_SIZE);

    /* 读取 nvram 段 flexspi_flash 数据 */
    if (aw_nvram_get(TEST_MEM_NAME, 0, __g_buf, 0, BUF_SIZE) != AW_OK) {
        AW_INFOF(("SPIFLASH NVRAM: Read Failed!\r\n"));
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

/** [src_nvram] */

/* end of file */
