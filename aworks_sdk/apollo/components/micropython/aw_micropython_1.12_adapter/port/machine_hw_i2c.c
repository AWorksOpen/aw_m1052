/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 SummerGift <zhangyuan@rt-thread.com>
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

#include "machine_hw_i2c.h"

#if MICROPY_PY_MACHINE_I2C
/**
 * \brief moduleref machine
 * \class I2C - a two-wire serial protocol
 *
 * I2C is a two-wire protocol for communicating between devices.  At the physical
 * level it consists of 2 wires: SCL and SDA, the clock and data lines respectively.
 *
 * I2C objects are created attached to a specific bus.  They can be initialised
 * when created, or initialised later on:
 *
 * The following methods implement the standard I2C master read and write operations
 * that target a given slave device.
 *
 *     from machine import I2C
 *
 *     i2c = I2C(1)                             # create on bus 1
 *     i2c = I2C(1, freq=200000, timeout=1000)  # create and init as a master
 *     i2c.init(freq=2000000)                   # init freq
 *     i2c.deinit()                             # turn off the peripheral
 *
 * Basic methods for slave are write and read:
 *
 *     addr = i2c.scan()[0]
 *     buf = bytearray(3)
 *     i2c.writeto(addr, buf, stop=True)      # write 3 bytes
 *     i2c.readfrom(addr,len(buf), stop=True) # read 3 bytes
 *     data = bytearray(3)                    # create a buffer
 *     i2c.readfrom_into(addr, data)          # receive 3 bytes, writing them into data
 *
 *     reg = 0x02                             # register address
 *     i2c.writeto_mem(addr, reg, buf)
 *     i2c.readfrom_mem_into(addr, reg, data)
 *
 * Try some raw read and writes
 *     reg2 = bytearray(3)
 *     reg2[0] = 0x02                       # register address
 *     reg2[1] = 0x01                       # data
 *     reg2[2] = 0x03
 *     i2c.writeto(addr, reg2, stop=False)  # After writing the register address,
 *                                          # The data on the bus is started to be sent
 *     # now read
 *     print(i2c.readfrom(addr, 2, stop=False)[0])
 *     reg2_read = bytearray(2)
 *     i2c.readfrom_into(addr, reg2_read, stop=False)
 *     reg2_read.decode()
 *
 */
STATIC const mp_obj_type_t machine_hard_i2c_type;

typedef struct _machine_hard_i2c_obj_t {

    mp_obj_base_t base;

    int i2c_id;

    struct aw_i2c_config i2c_cfg;

} machine_hard_i2c_obj_t;

STATIC void machine_hard_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print,"I2C(id = %d,freq = %d,timeout = %d)",\
            self->i2c_id,self->i2c_cfg.speed,self->i2c_cfg.timeout);

    return;
}

/**
 * \brief method init(freq=200000, timeout=1000)
 *
 * Initialise the I2C bus with the given parameters:
 *
 *   - `freq` is the SCL clock rate (only sensible for a master)
 *   - `timeout` is the timeout in milliseconds to wait between characters.
 *
 */
STATIC mp_obj_t machine_hard_i2c_init_helper(machine_hard_i2c_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    //参数类型、默认值等合法性定义
    STATIC const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MP_HW_I2C_FREQ_DEFAULT} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MP_HW_I2C_TIMEOUT_DEFAULT} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    //设置i2c属性，设置前先获取配置
    aw_err_t ret;
    ret = aw_i2c_get_cfg(self->i2c_id,&self->i2c_cfg);
    if(ret != AW_OK){
        mp_raise_ValueError("Get i2c config fail.");
    }

    self->i2c_cfg.speed = args[0].u_int;
    self->i2c_cfg.timeout = args[1].u_int;

    ret = aw_i2c_set_cfg(self->i2c_id,&self->i2c_cfg);
    if(ret != AW_OK){
        mp_raise_ValueError("Set i2c config fail.");
    }

    return mp_const_none;
}

/**
 *
 * \brief classmethod \constructor(bus, ...)
 *
 * Construct an I2C object on the given bus.  `bus` can be 1 or 2.
 * With no additional parameters, the I2C object is created but not
 * initialised (it has the settings from the last initialisation of
 * the bus, if any).  If extra arguments are given, the bus is initialised.
 * See `init` for parameters of initialisation.
 *
 */
mp_obj_t machine_hard_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {

    int i2c_id = mp_obj_get_int(all_args[0]);

    if ( i2c_id < 0 ||  i2c_id > MP_MACHINE_I2C_NUM_MAX) {
        mp_raise_msg_varg(&mp_type_ValueError, "I2C(%s) doesn't exist", i2c_id);
    }

    // create new hard I2C object
    machine_hard_i2c_obj_t *self = m_new_obj(machine_hard_i2c_obj_t);
    self->base.type = &machine_hard_i2c_type;
    self->i2c_id = i2c_id;

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
        machine_hard_i2c_init_helper(self, n_args - 1, all_args + 1, &kw_args);
    }

    return (mp_obj_t) self;

}

STATIC mp_obj_t machine_hard_i2c_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_hard_i2c_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_hard_i2c_init_obj, 1, machine_hard_i2c_init);

/**
 * \method writeto(addr, buf, stop=True)
 * write data on the bus:
 *
 *   - `addr` is the address to send to (only required in master mode)
 *   - `buf` is the data to send (an integer to send, or a buffer object)
 *   - `stop` is the STOP signal. If stop is true then a STOP condition is generated at the end of
 *      the transfer, even if a NACK is received.
 *
 * Return value: the number of ACKs that were received.
 *
 */
STATIC mp_obj_t machine_hard_i2c_writeto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_buf,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_stop, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };

    // parse args
    machine_hard_i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    //丛机结构体配置
    aw_i2c_device_t p_dev;
    p_dev.busid = self->i2c_id;
    p_dev.addr = args[0].u_int;
    p_dev.flags = AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1].u_obj, &bufinfo, MP_BUFFER_WRITE);

    aw_err_t ret = AW_OK;
    if(args[2].u_bool){
        ret = aw_i2c_write(&p_dev, 0, bufinfo.buf, bufinfo.len);
        if(ret != AW_OK){
            mp_raise_OSError(ret);
        }
    }else{
        ret = aw_i2c_raw_write(&p_dev, bufinfo.buf, bufinfo.len);
        if(ret != AW_OK){
            mp_raise_OSError(ret);
        }
    }

    //TODO
    //须返回写成功数据个数，待实现
    //return MP_OBJ_NEW_SMALL_INT(ret);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_hard_i2c_writeto_obj, 3, machine_hard_i2c_writeto);

/**
 * \brief method scan()
 *
 * Scan all I2C addresses from 0x08 to 0x77 and return a list of those that respond.
 * Only valid when in master mode.
 *
 */
STATIC mp_obj_t machine_hard_i2c_scan(mp_obj_t self_in) {
    machine_hard_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t list = mp_obj_new_list(0, NULL);

    //丛机结构体配置
    aw_i2c_device_t p_dev;
    p_dev.busid = self->i2c_id;
    p_dev.flags = AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE;

    // 7-bit addresses 0b0000xxx and 0b1111xxx are reserved
    for (int addr = 0x08; addr < 0x78; ++addr) {
        p_dev.addr = addr;
        aw_err_t ret = aw_i2c_write(&p_dev, 0, "1", 1);
        if (ret == AW_OK) {
            mp_obj_list_append(list, MP_OBJ_NEW_SMALL_INT(addr));
        }
    }
    return list;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_hard_i2c_scan_obj, machine_hard_i2c_scan);

/**
 * \brief method readfrom(addr, nbytes, stop=True)
 *
 * Read nbytes from the slave specified by addr.
 *
 *   - `addr` is the address to receive from (only required in master mode)
 *   - `nbytes` can be an integer, which is the number of bytes to receive,
 *     or a mutable buffer, which will be filled with received bytes
 *   - `stop` is the STOP sign.If stop is true then a STOP condition is generated at the end of
 *      the transfer.
 *
 * Returns a bytes object with the data read
 *
 */
STATIC mp_obj_t machine_hard_i2c_readfrom(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_nbytes,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_stop, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };

    // parse args
    machine_hard_i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    aw_i2c_device_t p_dev;
    p_dev.busid = self->i2c_id;
    p_dev.addr = args[0].u_int;
    p_dev.flags = AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE;

    vstr_t vstr;
    vstr_init_len(&vstr, args[1].u_int);

    aw_err_t ret = AW_OK;
    if(args[2].u_bool){
        ret = aw_i2c_read(&p_dev, 0, (uint8_t *)vstr.buf, vstr.len);
        if(ret != AW_OK){
            mp_raise_OSError(ret);
        }
    }else{
        ret = aw_i2c_raw_read(&p_dev, (uint8_t *)vstr.buf, vstr.len);
        if(ret != AW_OK){
            mp_raise_OSError(ret);
        }
    }

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_hard_i2c_readfrom_obj, 3, machine_hard_i2c_readfrom);


/**
 * \brief method readfrom_into(addr, buf, stop=True, /)
 *
 * Read into buf from the slave specified by addr.
 *
 *   - `addr` is the address to receive from (only required in master mode)
 *   - `buf` is bytearray to store data.The number of bytes read will be the length of buf.
 *   - `stop` is the STOP sign.If stop is true then a STOP condition is generated at the end of
 *     the transfer.
 *
 * The method returns None.
 *
 */
STATIC mp_obj_t machine_hard_i2c_readfrom_into(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_buf,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_stop, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };

    // parse args
    machine_hard_i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);


    aw_i2c_device_t p_dev;
    p_dev.busid = self->i2c_id;
    p_dev.addr = args[0].u_int;
    p_dev.flags = AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1].u_obj, &bufinfo, MP_BUFFER_WRITE);

    aw_err_t ret = AW_OK;
    if(args[2].u_bool){
        ret = aw_i2c_read(&p_dev, 0, bufinfo.buf, bufinfo.len);
        if (ret != AW_OK) {
            mp_raise_OSError(ret);
        }
    }else{
        ret = aw_i2c_raw_read(&p_dev, bufinfo.buf, bufinfo.len);
        if (ret != AW_OK) {
            mp_raise_OSError(ret);
        }
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_hard_i2c_readfrom_into_obj, 3,machine_hard_i2c_readfrom_into);


STATIC const mp_arg_t machine_hard_i2c_mem_allowed_args[] = {
    { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_memaddr, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_arg,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 7} },
};

/**
 * \brief readfrom_mem(addr, memaddr, nbytes, addrsize=7)
 *
 * Read nbytes from the slave specified by addr starting from the memory address specified by memaddr.
 * The argument addrsize specifies the address size in bits.
 *
 * Returns a bytes object with the data read.
 */
STATIC mp_obj_t machine_hard_i2c_readfrom_mem(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_addr, ARG_memaddr, ARG_n, ARG_addrsize };
    mp_arg_val_t args[MP_ARRAY_SIZE(machine_hard_i2c_mem_allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(machine_hard_i2c_mem_allowed_args), machine_hard_i2c_mem_allowed_args, args);

    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)MP_OBJ_TO_PTR(pos_args[0]);

    aw_i2c_device_t p_dev;
    p_dev.busid = self->i2c_id;
    p_dev.addr = args[ARG_addr].u_int;

    if(args[ARG_addrsize].u_int == 7){
        p_dev.flags = AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE;
    }else if(args[ARG_addrsize].u_int == 10){
        p_dev.flags = AW_I2C_ADDR_10BIT | AW_I2C_SUBADDR_1BYTE;
    }else{
        mp_raise_ValueError("addrsize just be 7/10 bits.");
    }

    // create the buffer to store data into
    vstr_t vstr;
    vstr_init_len(&vstr, mp_obj_get_int(args[ARG_n].u_obj));

    // do the transfer
    aw_err_t ret = aw_i2c_read(&p_dev, args[ARG_memaddr].u_int, (uint8_t *)vstr.buf, vstr.len);
    if (ret != AW_OK) {
        mp_raise_OSError(ret);
    }

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_hard_i2c_readfrom_mem_obj, 3, machine_hard_i2c_readfrom_mem);

/**
 * \brief readfrom_mem_into(addr, memaddr, buf, addrsize=7)
 *
 * Read into buf from the slave specified by addr starting from the memory address specified by memaddr.
 * The number of bytes read is the length of buf. The argument addrsize specifies the address size in bits.
 *
 * The method returns None.
 */
STATIC mp_obj_t machine_hard_i2c_readfrom_mem_into(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_addr, ARG_memaddr, ARG_buf, ARG_addrsize };
    mp_arg_val_t args[MP_ARRAY_SIZE(machine_hard_i2c_mem_allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(machine_hard_i2c_mem_allowed_args), machine_hard_i2c_mem_allowed_args, args);

    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)MP_OBJ_TO_PTR(pos_args[0]);

    aw_i2c_device_t p_dev;
    p_dev.busid = self->i2c_id;
    p_dev.addr = args[ARG_addr].u_int;

    if(args[ARG_addrsize].u_int == 7){
        p_dev.flags = AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE;
    }else if(args[ARG_addrsize].u_int == 10){
        p_dev.flags = AW_I2C_ADDR_10BIT | AW_I2C_SUBADDR_1BYTE;
    }else{
        mp_raise_ValueError("addrsize just be 7/10 bits.");
    }
    // get the buffer to store data into
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_WRITE);

    // do the transfer
    aw_err_t ret = aw_i2c_read(&p_dev, args[ARG_memaddr].u_int, bufinfo.buf, bufinfo.len);
    if (ret != AW_OK) {
        mp_raise_OSError(ret);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_hard_i2c_readfrom_mem_into_obj, 1, machine_hard_i2c_readfrom_mem_into);

/**
 * \brief writeto_mem(addr, memaddr, buf, addrsize=8)
 *
 * Write buf to the slave specified by addr starting from the memory address specified by memaddr. The argument
 * addrsize specifies the address size in bits.
 *
* The method returns None.
 *
 */
STATIC mp_obj_t machine_hard_i2c_writeto_mem(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_addr, ARG_memaddr, ARG_buf, ARG_addrsize };
    mp_arg_val_t args[MP_ARRAY_SIZE(machine_hard_i2c_mem_allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(machine_hard_i2c_mem_allowed_args), machine_hard_i2c_mem_allowed_args, args);

    machine_hard_i2c_obj_t *self = (machine_hard_i2c_obj_t *)MP_OBJ_TO_PTR(pos_args[0]);

    aw_i2c_device_t p_dev;
    p_dev.busid = self->i2c_id;
    p_dev.addr = args[ARG_addr].u_int;
    p_dev.flags = AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE;

    // get the buffer to write the data from
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_READ);

    // do the transfer
    aw_err_t ret = aw_i2c_write(&p_dev, args[ARG_memaddr].u_int, bufinfo.buf, bufinfo.len);
    if(ret != AW_OK){
        mp_raise_OSError(ret);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_hard_i2c_writeto_mem_obj, 1, machine_hard_i2c_writeto_mem);

STATIC const mp_rom_map_elem_t machine_hard_i2c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_hard_i2c_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&machine_hard_i2c_scan_obj) },

    // standard bus operations
    { MP_ROM_QSTR(MP_QSTR_readfrom), MP_ROM_PTR(&machine_hard_i2c_readfrom_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom_into), MP_ROM_PTR(&machine_hard_i2c_readfrom_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeto), MP_ROM_PTR(&machine_hard_i2c_writeto_obj) },
//    { MP_ROM_QSTR(MP_QSTR_writevto), MP_ROM_PTR(&machine_hard_i2c_writevto_obj) },

    // memory operations
    { MP_ROM_QSTR(MP_QSTR_readfrom_mem), MP_ROM_PTR(&machine_hard_i2c_readfrom_mem_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom_mem_into), MP_ROM_PTR(&machine_hard_i2c_readfrom_mem_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeto_mem), MP_ROM_PTR(&machine_hard_i2c_writeto_mem_obj) },
};
MP_DEFINE_CONST_DICT(machine_hard_i2c_locals_dict, machine_hard_i2c_locals_dict_table);


STATIC const mp_obj_type_t machine_hard_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = machine_hard_i2c_print,
    .make_new = machine_hard_i2c_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_hard_i2c_locals_dict,
};

#endif // MICROPYTHON_USING_MACHINE_I2C
