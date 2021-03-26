/*
 * pins.c
 *
 *  Created on: 2018Äê3ÔÂ8ÈÕ
 *      Author: caowenjing
 */
#include "mpconfigport.h"
#include "py/obj.h"
#include "aw_mpy_cfg.h"

#if AW_MPY_IMXRT105X
#include "boards/imxrt105x/mpconfigboard_pins.h"
#elif AW_MPY_IMX28X
#include "boards/imxrt105x/mpconfigboard_pins.h"
#elif AW_MPY_IMXRT106X
#include "boards/imxrt106x/mpconfigboard_pins.h"
#endif

#if MICROPY_PY_MACHINE_PIN
//extern const mp_rom_map_elem_t machine_pin_board_pins_locals_dict_table[];

MP_DEFINE_CONST_DICT(machine_pin_board_pins_locals_dict, machine_pin_board_pins_locals_dict_table);


const mp_obj_type_t machine_pin_board_pins_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_board,
    .locals_dict = (mp_obj_t)&machine_pin_board_pins_locals_dict,
};#endif
