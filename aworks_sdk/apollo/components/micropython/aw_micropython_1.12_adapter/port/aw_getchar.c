/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Armink (armink.ztl@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <aw_getchar.h>
#include <stdio.h>
#include "lib/utils/interrupt_char.h"
#include "aworks.h"
#include "aw_vdebug.h"
#include "py/runtime.h"
#include "aw_serial.h"
#include "aw_common.h"
#include "aw_errno.h"
#include "aw_task.h"
#include "aw_rngbuf.h"
#include "aw_mpy_cfg.h"


/* 定义并初始化环形缓冲区 */
AW_RNGBUF_DEFINE(stdin_ringbuf, char, 256);

static void * __uart_read_func(void * p_arg)
{
    char data;

    aw_err_t                  ret;
    struct aw_serial_dcb     dcb;
    struct aw_serial_timeout  timeout;

    // 配置串口波特了为115200，8个数据位，1位停止位，无奇偶校验
    // 首先获取dcb的默认配置
    ret = aw_serial_dcb_get(MPY_CONSOLE_COMID,&dcb);
    assert(!ret);

    // 修改
    dcb.baud_rate = 115200;
    dcb.byte_size = 8;
    dcb.stop_bits = AW_SERIAL_ONESTOPBIT;
    // 无校验
    dcb.f_parity = AW_FALSE;
    ret = aw_serial_dcb_set(MPY_CONSOLE_COMID,&dcb);
    assert(!ret);

    // 配置串口超时
    ret = aw_serial_timeout_get(MPY_CONSOLE_COMID,&timeout);
    assert(!ret);

    // 读总超时为1s
    timeout.rd_timeout = 1000;
    // 码间超时为50ms
    timeout.rd_interval_timeout = 50;
    ret = aw_serial_timeout_set(MPY_CONSOLE_COMID,&timeout);
    assert(!ret);

    while(1){
        if(aw_serial_read(MPY_CONSOLE_COMID, &data, 1) > 0){
            if (data == mp_interrupt_char) {
                mp_keyboard_interrupt();
            } else {
                aw_rngbuf_putchar(&stdin_ringbuf, data);
            }
        }
    }

    return NULL;

}
static aw_task_id_t read_id = NULL;
void aw_getchar_init(void) {

    read_id = aw_task_create("uart read",2,1024,__uart_read_func,NULL);
    if(read_id != NULL){
        aw_task_startup(read_id);
    }

}

void aw_getchar_deinit(void) {
    aw_rngbuf_flush(&stdin_ringbuf);
    aw_task_terminate(read_id);
}

int aw_getchar(void) {

    uint8_t    data = 0;
    if (aw_rngbuf_getchar(&stdin_ringbuf, (char *)&data)) {
        return data;
    }else{
        return -1;
    }
}
