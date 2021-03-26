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

#include "machine_hw_spi_.h"


#if MICROPY_PY_MACHINE_SPI
/**
 * /brief class machine.SPI - a master-driven serial protocol
 *
 * SPI is a serial protocol that is driven by a master.  At the physical level
 * there are 3 lines: SCK, MOSI, MISO.
 *
 * See usage model of I2C; SPI is very similar.  Main difference is
 * parameters to init the SPI bus:
 *
 *     from machine import SPI
 *     from machine import Pin
 *     spi = SPI(1)
 *     spi = SPI(1, baudrate=600000,  polarity=0, phase=0, bits=8, firstbit=SPI.MSB)
 *
 * Only required parameter is mode,Polarity can be 0 or 1,
 * and is the level the idle clock line sits at.  Phase can be 0 or 1
 * to sample data on the first or second clock edge respectively.
 *
 * Additional method for SPI:
 *
 *     spi.init(1, baudrate=600000)
 *
 *     buf = bytearray(4)
 *
 *     SPI.read(nbytes, write=0x00)                 # Read a number of bytes specified by nbytes while
 *                                                  # continuously writing the single byte given by write.
 *
 *     SPI.readinto(buf, write=0x00)                # Read into the buffer specified by buf while
 *                                                  # continuously writing the single byte given by write.
 *
 *     SPI.write(buf )                              # Write the bytes contained in buf. Returns None
 *
 *     SPI.write_readinto(write_buf, read_buf )     # Write the bytes from write_buf while reading into read_buf.
 *                                                  # The buffers can be the same or different, but
 *                                                  # both buffers must have the same length. Returns None
 *
 *     SPI.deinit()                                 #Turn off the SPI bus
 *
 */

STATIC const mp_obj_type_t machine_hard_spi_type;

typedef struct _machine_hard_spi_obj_t {

    mp_obj_base_t base;

    /**< /brief 当前配置信息  */
    aw_spi_device_t spi_device;

} machine_hard_spi_obj_t;

STATIC void machine_hard_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t*)self_in;
    mp_printf(print,"SPI(device port : spi%d)",self->spi_device.busid);
}
void mp_pfunc_cs(int state){
    //None
    return ;
}


/**
 * \brief SPI.init(baudrate=328125, polarity=0, phase=0, bits=8, firstbit=SPI.MSB,cs_pin = )
 *
 * Initialise the SPI bus with the given parameters:
 *   - `baudrate` is the SCK clock rate (only sensible for a master).
 *
 */
STATIC mp_obj_t machine_hard_spi_init_helper(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_cs_pin };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 1000000} },
        { MP_QSTR_polarity, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_firstbit, MP_ARG_INT, {.u_int = MICROPY_PY_MACHINE_SPI_MSB} },
//        { MP_QSTR_cs_pin, MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args),
                     allowed_args, args);

    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t*)self_in;

    //波特率
    self->spi_device.max_speed_hz = args[ARG_baudrate].u_int;
    //字大小
    self->spi_device.bits_per_word = args[ARG_bits].u_int;
    //模式
    if(args[ARG_polarity].u_int == 0 && args[ARG_phase].u_int == 0){
        self->spi_device.mode = AW_SPI_MODE_0;
    }else if(args[ARG_polarity].u_int == 0 && args[ARG_phase].u_int == 1){
        self->spi_device.mode = AW_SPI_MODE_1;
    }else if(args[ARG_polarity].u_int == 1 && args[ARG_phase].u_int == 0){
        self->spi_device.mode = AW_SPI_MODE_2;
    }else if(args[ARG_polarity].u_int == 1 && args[ARG_phase].u_int == 1){
        self->spi_device.mode = AW_SPI_MODE_3;
    }else{
        mp_raise_TypeError("Not supports the mode.");
    }

    //选择高位/地位传输
    if(args[ARG_firstbit].u_int == MICROPY_PY_MACHINE_SPI_LSB){
        self->spi_device.mode |= AW_SPI_LSB_FIRST;
    }else if(args[ARG_firstbit].u_int != MICROPY_PY_MACHINE_SPI_MSB){
        mp_raise_ValueError("Not supports.firstbit just be 0/1.");
    }

    //由用户程序控制
//    self->spi_device.cs_pin = args[ARG_cs_pin].u_int;
    self->spi_device.pfunc_cs = mp_pfunc_cs;

    //TODO
    aw_spi_mkdev(&self->spi_device,
                 self->spi_device.busid,
                 self->spi_device.bits_per_word,                /* 字大小 */
                 self->spi_device.mode,                         /* SPI 模式 */
                 self->spi_device.max_speed_hz,                 /* 支持的最大速度 */
                 self->spi_device.cs_pin,                       /* 片选引脚*/
                 self->spi_device.pfunc_cs);

    if (aw_spi_setup(&self->spi_device) != AW_OK) {
        mp_raise_ValueError("SPI config fail! ");
    }

    return mp_const_none;
}

STATIC mp_obj_t machine_hard_spi_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_hard_spi_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_hard_spi_init_obj, 1, machine_hard_spi_init);

/**
 * \brief constructor(bus, ...)
 *
 * Construct an SPI object on the given bus.  `bus` can be 1 or 2.
 * With no additional parameters, the SPI object is created but not
 * initialised (it has the settings from the last initialisation of
 * the bus, if any).  If extra arguments are given, the bus is initialised.
 * See `init` for parameters of initialisation.
 *
 * At the moment, the NSS pin is not used by the SPI driver and is free
 * for other use.
 */
mp_obj_t machine_hard_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_int_t spi_id = mp_obj_get_int(all_args[0]);

    if (spi_id < 0 || spi_id > MP_MACHINE_SPI_NUM_MAX ) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, " SPI device %d not found!\n", spi_id));
    }

    //创建窗口对象并初始化
    machine_hard_spi_obj_t *self = m_new_obj(machine_hard_spi_obj_t);
    self->base.type = &machine_hard_spi_type;
    self->spi_device.busid = spi_id;

    if (n_args > 1 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
        machine_hard_spi_init_helper((mp_obj_base_t*)self, n_args - 1, all_args + 1, &kw_args);
    }

    return (mp_obj_t) self;
}

/**
 * \brief method deinit()
 *
 * Turn off the spi bus.
 *
 */
STATIC mp_obj_t machine_hard_spi_deinit(mp_obj_t self_in) {

    //TODO
    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)&self_in;
    m_del_obj(self->base.type, self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_hard_spi_deinit_obj, machine_hard_spi_deinit);

/**
 * \brief SPI.read(nbytes, write=0x00)
 *
 * Read a number of bytes specified by `nbytes` while continuously writing the
 * single byte given by `write`.
 *
 * Returns a bytes object with the data that was read
 *
 */
STATIC mp_obj_t machine_hard_spi_read(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_nbytes,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_write, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    // parse args
    machine_hard_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    vstr_t vstr;
    vstr_init_len(&vstr, args[0].u_int);
    memset(vstr.buf, n_args == 3 ? args[1].u_int : 0, vstr.len);

    aw_err_t ret = aw_spi_write_and_read(&self->spi_device,vstr.buf,vstr.buf,vstr.len);
    if(ret != AW_OK){
        mp_raise_OSError(ret);
    }

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_hard_spi_read_obj, 2, machine_hard_spi_read);

/**
 * \brief SPI.readinto(buf, write=0x00)
 *
 * Read into the buffer specified by `buf` while continuously writing the single
 * byte given by `write`.
 *
 * Returns None
 *
 */
STATIC mp_obj_t machine_hard_spi_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buf,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_write, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    // parse args
    machine_hard_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0].u_obj, &bufinfo, MP_BUFFER_WRITE);
    memset(bufinfo.buf, n_args == 3 ? args[1].u_int : 0, bufinfo.len);

    aw_err_t ret = aw_spi_write_and_read(&self->spi_device,bufinfo.buf,bufinfo.buf,bufinfo.len);
    if(ret != AW_OK){
        mp_raise_OSError(ret);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_hard_spi_readinto_obj, 2, machine_hard_spi_readinto);

/**
 * \brief SPI.write(buf )
 *
 * Write the bytes contained in `buf`.
 *
 * Returns None.
 */
STATIC mp_obj_t machine_hard_spi_write(mp_obj_t self_in, mp_obj_t buf){

    // parse args
    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)self_in;

    // get the buffer to write the data from
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);

    aw_err_t ret = aw_spi_write_and_read(&self->spi_device,NULL,bufinfo.buf,bufinfo.len);
    if(ret != AW_OK){
        mp_raise_OSError(ret);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(machine_hard_spi_write_obj,machine_hard_spi_write);

/**
 * \brief SPI.write_readinto(write_buf, read_buf )
 *
 * Write the bytes from `write_buf` while reading into `read_buf`.
 * The buffers can be the same or different, but both buffers must have the same length.
 *
 * Returns None.
 */
STATIC mp_obj_t machine_hard_spi_write_readinto(mp_obj_t self_in, mp_obj_t \
        wr_buf, mp_obj_t rd_buf){

    // parse args
    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)self_in;

    mp_buffer_info_t src;
    mp_get_buffer_raise(wr_buf, &src, MP_BUFFER_READ);
    mp_buffer_info_t dest;
    mp_get_buffer_raise(rd_buf, &dest, MP_BUFFER_WRITE);
    if (src.len != dest.len) {
        mp_raise_ValueError("buffers must be the same length");
    }

    aw_err_t ret = aw_spi_write_and_read(&self->spi_device,src.buf,dest.buf,src.len);
    if(ret != AW_OK){
        mp_raise_OSError(ret);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(machine_hard_spi_write_readinto_obj,machine_hard_spi_write_readinto);
STATIC const mp_rom_map_elem_t machine_hard_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_hard_spi_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_hard_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&machine_hard_spi_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&machine_hard_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_hard_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&machine_hard_spi_write_readinto_obj) },

    { MP_ROM_QSTR(MP_QSTR_MSB), MP_ROM_INT(MICROPY_PY_MACHINE_SPI_MSB) },
    { MP_ROM_QSTR(MP_QSTR_LSB), MP_ROM_INT(MICROPY_PY_MACHINE_SPI_LSB) },
};
MP_DEFINE_CONST_DICT(mp_machine_hard_spi_locals_dict, machine_hard_spi_locals_dict_table);


STATIC const mp_obj_type_t machine_hard_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = machine_hard_spi_print,
    .make_new = machine_hard_spi_make_new,
    .locals_dict = (mp_obj_t)&mp_machine_hard_spi_locals_dict,
};

#endif // MICROPYTHON_USING_MACHINE_SPI

