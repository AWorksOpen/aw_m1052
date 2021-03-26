/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Nicko van Someren
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


#include "machine_sdcard.h"
#include "aw_sdcard.h"

#if MICROPY_HW_ENABLE_SDCARD

void machine_sdcard_print (const mp_print_t *print, mp_obj_t obj, mp_print_kind_t kind){

    sdcard_card_obj_t *self = (sdcard_card_obj_t *)&obj;
    mp_printf(print,"SDCard(%s)",self->pdrv);
}

STATIC mp_obj_t machine_sdcard_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments

    sdcard_card_obj_t * self = m_new_obj(sdcard_card_obj_t);
    self->base.type = type;
    self->pdrv = mp_obj_str_get_str(args[0]);

    //¼ì²âSD¿¨²åÈë
    if (!aw_sdcard_is_insert(self->pdrv)) {
        mp_raise_TypeError("There is not any sdcard");
    }

    self->p_sdcard = aw_sdcard_open(self->pdrv);
    self->sector_count = self->p_sdcard->p_mem_info->csd.sector_cnt;
    self->sector_size = self->p_sdcard->p_mem_info->csd.sector_size;

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_sdcard_readblocks(mp_obj_t self_in, mp_obj_t sector_num, mp_obj_t buf) {
    sdcard_card_obj_t *self = self_in;
    mp_buffer_info_t bufinfo;
    aw_err_t err;

    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    uint32_t sect_no = mp_obj_get_int(sector_num);
    uint32_t sect_cnt = (bufinfo.len / self->sector_size) + ((bufinfo.len % self->sector_size) ? 1:0);

    err = aw_sdcard_rw_sector(self->p_sdcard, sect_no, bufinfo.buf, sect_cnt, AW_TRUE);

    return mp_obj_new_bool(err == AW_OK);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_sdcard_readblocks_obj, machine_sdcard_readblocks);

STATIC mp_obj_t machine_sdcard_writeblocks(mp_obj_t self_in, mp_obj_t sector_num, mp_obj_t buf) {
    sdcard_card_obj_t *self = self_in;
    mp_buffer_info_t bufinfo;
    aw_err_t err;

    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    uint32_t sect_no = mp_obj_get_int(sector_num);
    uint32_t sect_cnt = (bufinfo.len / self->sector_size) + ((bufinfo.len % self->sector_size) ? 1:0);
    err =  aw_sdcard_rw_sector(self->p_sdcard, sect_no, ( uint8_t *)bufinfo.buf, sect_cnt , AW_FALSE);

    return mp_obj_new_bool(err == AW_OK);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_sdcard_writeblocks_obj, machine_sdcard_writeblocks);

mp_obj_t machine_sdcard_deinit(mp_obj_t self_in){
    sdcard_card_obj_t *self = self_in;
    mp_raise_OSError(aw_sdcard_close(self->pdrv));
    return MP_OBJ_NULL;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_sdcard_deinit_obj, machine_sdcard_deinit);

STATIC const mp_rom_map_elem_t machine_sdcard_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_sdcard_deinit_obj) },
    //  device protocol
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&machine_sdcard_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&machine_sdcard_writeblocks_obj) },
//    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&machine_sdcard_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_sdcard_locals_dict, machine_sdcard_locals_dict_table);

const mp_obj_type_t machine_sdcard_type = {
    { &mp_type_type },
    .name = MP_QSTR_SDCard,
    .print = machine_sdcard_print,
    .make_new = machine_sdcard_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_sdcard_locals_dict,
};

#endif // MICROPY_HW_ENABLE_SDCARD
