
#include <stdio.h>
#include <errno.h>
#include "py/runtime.h"
#include "py/mphal.h"
#include "machine_led.h"
#include "aw_led.h"
#include "awbus_lite.h"
#include "driver/led/awbl_gpio_led.h"

#if MICROPY_PY_MACHINE_LED

typedef struct _machine_led_obj_t {
    mp_obj_base_t base;
    mp_uint_t led_id;
} machine_led_obj_t;

const mp_obj_base_t machine_led_obj_template = {&machine_led_type};

void machine_led_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_led_obj_t *self = self_in;
    mp_printf(print, "<LED %lu>", self->led_id);
}

STATIC mp_obj_t machine_led_obj_make_new(const mp_obj_type_t *type, uint n_args, uint n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 2, false);

    machine_led_obj_t *led = m_new_obj(machine_led_obj_t);

    if (!led) {
        mp_raise_OSError(ENOMEM);
    }

    // get led number
    mp_int_t led_id = mp_obj_get_int(args[0]);

    led->base = machine_led_obj_template;
    led->led_id = led_id;

    // check led number
    if (!(0 <= led_id && led_id <= (MP_MACHINE_LED_NUM_MAX - 1))) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "LED %d does not exist", led_id));
    }


    // return static led object
    return (mp_obj_t)led;
}

mp_obj_t machine_led_obj_set(mp_obj_t self_in,mp_obj_t on) {
    machine_led_obj_t *self = self_in;
    int bool_on= mp_obj_get_int(on);
    aw_led_set (self->led_id,bool_on);
    return mp_const_none;
}

mp_obj_t machine_led_obj_on(mp_obj_t self_in) {
    machine_led_obj_t *self = self_in;
    aw_led_on (self->led_id);
    return mp_const_none;
}

mp_obj_t machine_led_obj_off(mp_obj_t self_in) {
    machine_led_obj_t *self = self_in;
    aw_led_off (self->led_id);
    return mp_const_none;
}

mp_obj_t machine_led_obj_toggle(mp_obj_t self_in) {
    machine_led_obj_t *self = self_in;
    aw_led_toggle (self->led_id);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_led_obj_on_obj, machine_led_obj_on);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_led_obj_off_obj, machine_led_obj_off);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_led_obj_toggle_obj, machine_led_obj_toggle);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_led_obj_set_obj, machine_led_obj_set);

STATIC const mp_rom_map_elem_t machine_led_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_led_obj_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_led_obj_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&machine_led_obj_toggle_obj) },
    { MP_ROM_QSTR(MP_QSTR_set), MP_ROM_PTR(&machine_led_obj_set_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_led_locals_dict, machine_led_locals_dict_table);

const mp_obj_type_t machine_led_type = {
    { &mp_type_type },
    .name = MP_QSTR_LED,
    .print = machine_led_obj_print,
    .make_new = machine_led_obj_make_new,
    .locals_dict = (mp_obj_t)&machine_led_locals_dict,
};
#endif
