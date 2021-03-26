/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 SummerGift <zhangyuan@rt-thread.com>
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

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/stream.h"

#include <stdarg.h>
#include "machine_uart.h"
#include "aw_sio_common.h"
#include "aw_serial.h"
#include "aw_ioctl.h"

#if MICROPY_PY_MACHINE_UART

/**
 * \moduleref machine
 * \class UART - duplex serial communication bus
 *
 * UART implements the standard UART/USART duplex serial communications protocol.  At
 * the physical level it consists of 2 lines: RX and TX.  The unit of communication
 * is a character (not to be confused with a string character) which can be 8 or 9
 * bits wide.
 *
 * UART objects can be created and initialised using:
 *
 *     from machine import UART
 *
 *     uart = UART(1, 9600)                         # init with given baudrate
 *     uart.init(9600, bits=8, parity=None, stop=1) # init with given parameters
 *
 * Bits can be 5/6/7/8.  Parity can be None, 0 (even) or 1 (odd).  Stop can be 1 or 2.
 *
 * A UART object acts like a stream object and reading and writing is done
 * using the standard stream methods:
 *
 *     uart.read(10)       # read 10 characters, returns a bytes object
 *     uart.read()         # read all available characters
 *     uart.readline()     # read a line
 *     uart.readinto(buf)  # read and store into the given buffer
 *     uart.write('abc')   # write the 3 characters
 *
 * Individual characters can be read/written using:
 *
 *     uart.readchar()     # read 1 character and returns it as an integer
 *     uart.writechar(42)  # write 1 character
 *
 * To check if there is anything to be read, use:
 *
 *     uart.any()               # returns True if any characters waiting
 */

/**
 * \brief uart类属性
 */
typedef struct _machine_uart_obj_t {

    //基类
    mp_obj_base_t base;

    //串口id号
    int uart_id;

    //串口可配置属性
    struct aw_serial_dcb serial_dcb;

    //串口超时设置
    struct aw_serial_timeout serial_timeout;

}machine_uart_obj_t;


STATIC void machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = (machine_uart_obj_t*) self_in;

    mp_printf(print, "uart device(id = %d,baudrate = %d, bits = %d,",\
            self->uart_id,
            self->serial_dcb.baud_rate,
            self->serial_dcb.byte_size);

    if(self->serial_dcb.f_parity == 0){
        mp_printf(print,"parity = None,");
    }else if(self->serial_dcb.f_parity == 1){
        mp_printf(print,"parity = %d,",self->serial_dcb.parity);
    }

    if(self->serial_dcb.stop_bits == AW_SERIAL_ONESTOPBIT){
        mp_printf(print,"stop = 1,");
    }else if(self->serial_dcb.stop_bits  == AW_SERIAL_ONE5STOPTS){
        mp_printf(print,"stop = 1.5,");
    }else if(self->serial_dcb.stop_bits == AW_SERIAL_TWOSTOPBITS){
        mp_printf(print,"stop = 2,");
    }

    mp_printf(print,"timeout = %d,timeout_char = %d)",
            self->serial_timeout.rd_timeout,
            self->serial_timeout.rd_interval_timeout);

    return ;
}

/**
 * \brief method init(baudrate, bits=8, parity=None, stop=1, timeout=1000, timeout_char=0)
 *
 * Initialise the UART bus with the given parameters:
 *
 *   - `baudrate` is the clock rate.
 *   - `bits` is the number of bits per byte, 5, 6, 7 or 8.
 *   - `parity` is the parity, `None`, 0 (even) or 1 (odd).
 *   - `stop` is the number of stop bits, 1, 1.5 or 2.
 *   - `timeout` is the timeout in milliseconds to wait for the first character.
 *   - `timeout_char` is the timeout in milliseconds to wait between characters.
 *
 */
STATIC mp_obj_t machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    //参数类型、默认值等合法性定义
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 115200} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_stop, MP_ARG_OBJ, {.u_obj = (mp_obj_t)1} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 50} },
    };

    struct {
        mp_arg_val_t baudrate, bits, parity, stop, timeout, timeout_char;
    } args;

    // parse args
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, (mp_arg_val_t*)&args);

    // 设置串口属性,设置之前首先获取配置
    aw_err_t ret;
    ret = aw_serial_dcb_get(self->uart_id,&self->serial_dcb);
    if(ret != AW_OK){
        mp_raise_ValueError("Get serial dcb fail.");
    }

    // 波特率
    self->serial_dcb.baud_rate = args.baudrate.u_int;

    // 数据位
    if(args.bits.u_int == 5 || args.bits.u_int == 6 || \
            args.bits.u_int == 7 || args.bits.u_int == 8){
        self->serial_dcb.byte_size = args.bits.u_int;
    }else{
        mp_raise_ValueError("bits parameter must be:5/6/7/8 ");
    }

    // 奇偶校验
    if(args.parity.u_obj == mp_const_none){
        self->serial_dcb.f_parity = 0;
    }else if (mp_obj_get_int(args.parity.u_obj) == 1){
        self->serial_dcb.f_parity = 1;
        self->serial_dcb.parity = AW_SERIAL_ODDPARITY;
    }else if(mp_obj_get_int(args.parity.u_obj) == 0) {
        self->serial_dcb.f_parity = 1;
        self->serial_dcb.parity = AW_SERIAL_EVENPARITY;
    }else{
        mp_raise_ValueError("parity parameter must be 0/1,0 is even and 1 is odd.");
    }

    // stop bits
    if(mp_obj_is_integer(args.stop.u_obj)){
        if((mp_int_t)args.stop.u_obj == 1 || mp_obj_get_int(args.stop.u_obj) == 1){
            self->serial_dcb.stop_bits = AW_SERIAL_ONESTOPBIT;
        }else if(mp_obj_get_int(args.stop.u_obj) == 2){
            self->serial_dcb.stop_bits = AW_SERIAL_TWOSTOPBITS;
        }else{
            mp_raise_ValueError("stop_bits parameter must be 1/1.5/2.");
        }
    }else if(mp_obj_is_float(args.stop.u_obj)){
        if(mp_obj_get_float(args.stop.u_obj) == 1.5){
            self->serial_dcb.stop_bits = AW_SERIAL_ONE5STOPTS;
        }else{
            mp_raise_ValueError("stop_bits parameter must be 1/1.5/2.");
        }
    }else{
        mp_raise_ValueError("stop_bits parameter must be 1/1.5/2.");
    }

    ret = aw_serial_dcb_set(self->uart_id, &self->serial_dcb);
    if(ret != AW_OK){
        mp_raise_ValueError("Set serial dcb fail.");
    }

    //设置超时
    self->serial_timeout.rd_timeout = args.timeout.u_int;
    self->serial_timeout.rd_interval_timeout =args.timeout_char.u_int;

    ret = aw_serial_timeout_set(self->uart_id,&self->serial_timeout);
    if(ret != AW_OK){
        mp_raise_ValueError("Set serial timeout fail. ");
    }

    return mp_const_none;
}

/**
 * \brief classmethod
 *
 * Construct a UART object on the given bus.
 *
 * With no additional parameters, the UART object is created but not
 * initialised (it has the settings from the last initialisation of
 * the bus, if any).  If extra arguments are given, the bus is initialised.
 * See `init` for parameters of initialisation.
 *
 */
STATIC mp_obj_t machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    mp_int_t uart_num = mp_obj_get_int(args[0]);

    //串口id范围检查
    if (uart_num < 0 || uart_num > MP_MACHINE_UART_NUM_MAX) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "UART(%d) does not exist", uart_num));
    }

    // 创建新的串口对象
    machine_uart_obj_t *self = m_new_obj(machine_uart_obj_t);
    self->base.type = &machine_uart_type;
    self->uart_id = uart_num;

    // 初始化uart
    if (n_args > 1 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t) self;
}


STATIC mp_obj_t machine_uart_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_uart_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_uart_init_obj, 1, machine_uart_init);

/**
 * \brief method deinit()
 *
 * Turn off the UART bus.
 *
 */
STATIC mp_obj_t machine_uart_deinit(mp_obj_t self_in) {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_deinit_obj, machine_uart_deinit);

/**
 * \brief method writechar(char)
 *
 * Write a single character on the bus.  `char` is an integer to write.
 * Return value: `None`.
 *
 */
STATIC mp_obj_t machine_uart_writechar(mp_obj_t self_in, mp_obj_t char_in) {
    machine_uart_obj_t *self = self_in;
    const char data = (char)mp_obj_get_int(char_in);
    uint32_t len = 0;
    uint32_t timeout = 0;
    do
    {
        len = aw_serial_write(self->uart_id, &data, 1);
        timeout++;
    }
    while (len != 1 && timeout < self->serial_timeout.rd_timeout);

    if(timeout >= self->serial_timeout.rd_timeout){
        mp_raise_OSError(MP_ETIMEDOUT);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_uart_writechar_obj, machine_uart_writechar);

/**
 * \brief method readchar()
 *
 * Receive a single character on the bus.
 * Return value: The character read, as an integer.  Returns -1 on timeout.
 *
 */
STATIC mp_obj_t machine_uart_readchar(mp_obj_t self_in) {
    machine_uart_obj_t *self = self_in;
    char data;

    aw_serial_read(self->uart_id,&data,1);

    return MP_OBJ_NEW_SMALL_INT(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_readchar_obj, machine_uart_readchar);

/**
 * \brief method any()
 *
 * Return `True` if any characters waiting, else `False`.
 *
 */
STATIC mp_obj_t machine_uart_any(mp_obj_t self_in) {
    machine_uart_obj_t *self = self_in;
    int len = 0 ;
    aw_serial_ioctl(self->uart_id,AW_FIONREAD,&len);
    return MP_OBJ_NEW_SMALL_INT(len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_any_obj, machine_uart_any);

STATIC const mp_rom_map_elem_t machine_uart_locals_dict_table[] = {
    // instance methods

    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_uart_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_uart_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_uart_any_obj) },

//    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&mp_stream_ioctl_obj) },
    /// \method read([nbytes])
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    /// \method readline()
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj)},
    /// \method readinto(buf[, nbytes])
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    /// \method write(buf)
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },

    { MP_ROM_QSTR(MP_QSTR_writechar), MP_ROM_PTR(&machine_uart_writechar_obj) },
    { MP_ROM_QSTR(MP_QSTR_readchar), MP_ROM_PTR(&machine_uart_readchar_obj) },

};
MP_DEFINE_CONST_DICT(machine_uart_locals_dict, machine_uart_locals_dict_table);

STATIC mp_uint_t machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = self_in;
    byte *buf = buf_in;
    return aw_serial_read(self->uart_id, buf, size);
}

STATIC mp_uint_t machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = self_in;
    const byte *buf = buf_in;
    return aw_serial_write(self->uart_id, buf, size);
}

//STATIC mp_uint_t machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
//    machine_uart_obj_t *self = self_in;
//
//    return aw_serial_ioctl(self->uart_id,SIO_MODE_SET,(void *)request);
//}

STATIC const mp_stream_p_t uart_stream_p = {
    .read = machine_uart_read,
    .write = machine_uart_write,
    .ioctl = NULL,
    .is_text = false,
};

const mp_obj_type_t machine_uart_type = {
    { &mp_type_type },
    .name = MP_QSTR_UART,
    .print = machine_uart_print,
    .make_new = machine_uart_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &uart_stream_p,
    .locals_dict = (mp_obj_dict_t*)&machine_uart_locals_dict,
};

#endif // MICROPYTHON_USING_MACHINE_UART
