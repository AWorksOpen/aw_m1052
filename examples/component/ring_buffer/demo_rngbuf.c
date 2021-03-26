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
 * \brief 环形缓冲区演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口使能宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象:
 *      串口打印 ringbuf 操作结果信息。
 *
 * \par 源代码
 * \snippet demo_rngbuf.c src_rngbuf
 *
 * \internal
 * \par History
 * - 1.00 17-09-06  sup, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_rngbuf 环形缓冲区
 * \copydoc demo_rngbuf.c
 */

/** [src_rngbuf] */
#include "aworks.h"             /* 此头文件必须被首先包含 */
#include "aw_vdebug.h"
#include "aw_rngbuf.h"


/**
 * \brief 环形缓冲区组件演示例程入口
 * \return 无
 */
void demo_rngbuf_entry (void)
{
    uint8_t     i = 0;
    char        dat;
    char        buf[] = "hello aworks";

    /* 定义并初始化环形缓冲区 */
    AW_RNGBUF_DEFINE(rb, char, 16);

    /* 获取环形缓冲区的大小 */
    AW_INFOF(("\nRingbuf size is:%d\n", AW_RNGBUF_SPACE_SIZE(rb)));

    /* 每次往缓冲区存入一个字节直到存满 */
    AW_FOREVER {
        if (!aw_rngbuf_isfull(&rb)) { /* 缓冲区未满 */

            /* 向环形缓冲区传入一个字节数据 */
            aw_rngbuf_putchar(&rb, 'a' + i);
            AW_INFOF(("Ringbuf put a char:%c\n", 'a' + i));
            i++;
        } else { /* 缓冲区已满  */
            AW_INFOF(("Ringbuf is full\n"));
            break;
        }
    }

    AW_INFOF(("---------------------------------------------------\n"));

    /* 每次从缓冲区取出一个字节直到缓冲区为空 */
    AW_FOREVER {
        if (!aw_rngbuf_isempty(&rb)) {

            /* 从环形缓冲区获取一个字节数据 */
            aw_rngbuf_getchar(&rb, &dat);
            AW_INFOF(("Ringbuf get a char:%c\n", dat));
        } else { /* 缓冲区为空 */
            AW_INFOF(("Ringbuf is empty\n"));
            break;
        }
    }

    /* 一次存放多个字节到缓冲区，然后查询缓冲区存储状态 */
    AW_INFOF(("\nRingbuf put:%s\n", buf));
    aw_rngbuf_put(&rb, buf, sizeof(buf));
    AW_INFOF(("Ringbuf saved bytes:%d\n", aw_rngbuf_nbytes(&rb)));
    AW_INFOF(("Ringbuf freebytes:%d\n", aw_rngbuf_freebytes(&rb)));

    /* 清空环形缓冲区, 然后查询缓冲区是否为空 */
    AW_INFOF(("\nRingbuf flush\n"));
    aw_rngbuf_flush(&rb);
    AW_INFOF(("Ringbuf %s empty\n",
                aw_rngbuf_isempty(&rb) ? "is" : "is not"));

    aw_rngbuf_putchar(&rb, 'a');        /* 写入数据'a'，指针移动一个字节 */
    aw_rngbuf_put_ahead(&rb, 'b', 0);   /* 写入数据'b'，但不移动写入指针 */
    aw_rngbuf_put_ahead(&rb, 'c', 1);   /* 写入数据'c'，但不移动写入指针 */
    aw_rngbuf_move_ahead(&rb, 2);       /* 往前移动数据指针2个字节 */

    aw_rngbuf_get(&rb, buf, 3);         /* 此时可以把"abc"都读出来 */
    AW_INFOF(("\nRingbuf get data:"));
    for (i = 0; i < 3; i++) {
        AW_INFOF(("%c ", buf[i]));
    }
    AW_INFOF(("\n"));
}

/** [src_rngbuf] */

/* end of file */
