/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWorks MD5 算法使用例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口使能宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 串口输出计算所得的MD5值；
 *   2. 计算字符串"abcdefghijklmnopqrstuvwxyz" 的md5值，如果无误，该值应该为：
 *      c3fcd3d76192e4007dfb496cca67e13b，一个128bit的数据
 *
 * \par 源代码
 * \snippet demo_md5.c src_md5
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-05  mkr, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_md5 MD5算法
 * \copydoc demo_md5.c
 */

/** [src_md5] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_md5.h"
#include <string.h>

/**
 * \brief md5 算法入口函数
 * \return 无
 */
void demo_md5_entry (void)
{
    char str[] = "abcdefghijklmnopqrstuvwxyz";
    uint8_t std_result[] = {
            0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00,
            0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b,
    };
    uint8_t cal_result[16] = {0};
    uint8_t i;

    aw_md5_t md5;

    aw_kprintf("\nMD5 testing...\r\n");

    /* 初始化md5算法 */
    aw_md5_init(&md5);

    /* 更新填充输入值 */
    aw_md5_update(&md5, (unsigned char *)str, strlen(str));

    /* 计算结果 */
    aw_md5_final(cal_result, &md5);

    if (0 == memcmp(std_result, cal_result, 16)) {
        aw_kprintf(
                "\nThe calculate is right!\nThe md5 value of \"%s\" is ：\r\n",
                str);
        for (i = 0; i < 16; i++) {
            aw_kprintf("%0.2x", cal_result[i]);
        }
    } else {
        aw_kprintf("\nThe calculate is error!\r\n");
    }
}

/** [src_md5] */

/* end of file */
