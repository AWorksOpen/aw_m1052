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
 * \brief CRC校验例程（软件CRC）
 *
 * - 操作步骤:
 *   1. 需要在aw_prj_params.h头文件里使能：
 *      - 对应平台的串口宏
 *      - AW_COM_CONSOLE
 *   2. 使用串口线分别和开发板的调试串口DURX、DUTX相连，打开串口调试助手，
 *      设置波特率为115200，1个停止位，无校验，无流控。
 *
 * - 实验现象：
 *   1. 校验数据为字符'1' ~ '9'，分别使用平台接口、软件CRC，
 *      按照CRC-16（CCITT标准）计算校验数据， 对比校验结果，打印对比结果。
 *
 * \par 源代码
 * \snippet demo_crc.c src_crc
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-24  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_crc CRC校验
 * \copydoc demo_crc.c
 */

/** [src_crc] */
#include "aworks.h"
#include "aw_crc.h"
#include "aw_vdebug.h"

/******************************************************************************/

/**
 * \brief 软件CRC计算
 * \param[in] p_frame  指向数据缓存区的指针
 * \param[in] len      数据个数
 * \param[in] crc      CRC初值
 * \return crc16 值
 */
aw_local uint16_t __software_crc16 (uint8_t *p_frame, uint16_t len, uint16_t crc)
{
    int i;

    for (; len > 0; len--) {
        crc = crc ^ (*p_frame++ << 8);
        for (i = 0; i < 8; i++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }

        crc &= 0xFFFF;
    }

    return crc;
}

/******************************************************************************/

/**
 * \brief crc校验例程
 * \return 无
 */
void demo_crc_entry (void)
{
    aw_err_t  ret;
    uint8_t   ndata[9]   = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    uint16_t  crc16_sft  = 0; /* 软件计算的CRC值 */
    uint32_t  crc16_plat = 0; /* 平台接口计算的CRC值 */

    aw_kprintf("\ncrc demo testing...\n");

    /* 定义了一个模型，该模型就是常用的CRC-16模型 */
    AW_CRC_DECL(crc16,          /* crc模型 crc16 */
                16,             /* crc宽度 16 */
                0x1021,         /* 生成多项式 */
                0xFFFF,         /* 初始值， CCITT标准要求初值必须为0 */
                AW_FALSE,       /* 输入字节bit不反序 */
                AW_FALSE,       /* 输出CRC按bit不反序 */
                0x0000);        /* 输出异或值 */


    ret =  AW_CRC_INIT(crc16,             /* crc模型，由AW_CRC_DECL()定义的 */
                       crctable16_1021,   /* crc表，系统的，不需要计算 */
                       AW_CRC_FLAG_AUTO); /* 自动模式，自动选择硬件或软件实现 */
    if (ret != AW_OK) {
        aw_kprintf("crc error\n");
    } else {
        AW_CRC_CAL(crc16,     /* crc模型，由AW_CRC_DECL()定义的 */
                   &ndata[0], /* 待进行CRC计算的数据 */
                   5);        /* 此次计算的数据 */

        AW_CRC_CAL(crc16,     /* crc模型，由AW_CRC_DECL()定义的 */
                   &ndata[5], /* 待进行CRC计算的数据 */
                   4);        /* 此次计算的数据 */

        /* 中间可以继续调用AW_CRC_CAL()计算
         * ...
         */

        /* 最后可以提取出最终结果 */
        crc16_plat = AW_CRC_FINAL(crc16);  /* crc模型，由AW_CRC_DECL()定义的 */

        aw_kprintf("platform crc16's value is = 0x%x\n", crc16_plat);

        /* 如果后续还需计算，则需要重新调用AW_CRC_INIT()初始化后再计算。 */
    }

    /* 使用软件CRC计算得到的数值 */
    crc16_sft = __software_crc16(ndata, 9, 0xFFFF);

    aw_kprintf("software crc16's value is = 0x%x\n", crc16_sft);

    if (crc16_plat == crc16_sft) {
        aw_kprintf("\n---Equal---\n");
    } else {
        aw_kprintf("\n---No Equal---\n");
    }
}

/** [src_crc] */

/* end of file */
