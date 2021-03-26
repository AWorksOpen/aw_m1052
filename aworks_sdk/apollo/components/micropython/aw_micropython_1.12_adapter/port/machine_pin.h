#ifndef __MACHINE_PIN_H__
#define __MACHINE_PIN_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mpconfigport.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "py/obj.h"
#include "aw_gpio.h"
#include "aw_list.h"

#if MICROPY_PY_MACHINE_PIN

#define MP_NAME_MAX 8
typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    char name[MP_NAME_MAX];
    mp_uint_t pin;
    uint32_t mode;
    uint32_t pull;
    mp_obj_t pin_isr_cb;
} machine_pin_obj_t;


extern const mp_obj_type_t machine_pin_board_pins_obj_type;
extern const mp_obj_dict_t machine_pin_board_pins_locals_dict;
extern const mp_obj_type_t machine_pin_type;
static mp_obj_t machine_pin_obj_init_helper(machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
void mp_hal_pin_open_set(void *machine_pin, int mode);
char* mp_hal_pin_get_name(void *machine_pin);

extern const mp_obj_type_t machine_pin_type;
#endif /*MICROPY_PY_MACHINE_PIN*/

#endif/*#if MICROPY_PY_MACHINE_PIN == 1*/
