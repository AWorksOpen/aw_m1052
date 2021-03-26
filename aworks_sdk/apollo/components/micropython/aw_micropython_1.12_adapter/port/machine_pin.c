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

#include "machine_pin.h"
#include <errno.h>
#include "extmod/virtpin.h"
#if MICROPY_PY_MACHINE_PIN

/**
 *
 * from machine import Pin
 *
 * # create an output pin on pin #0
 * p0 = Pin(0, Pin.OUT)
 * p0 = Pin(('led',Pin.board.GPIO1_19))
 * p0.init(Pin.OUT, Pin.PULL_UP, value = 0)
 *
 * # set the value low then high
 * p0.value(1)
 * p0.value()
 *
 * p0.on()
 * p0.off()
 *
 * p0.mode()
 * p0.mode(Pin.OUT)
 *
 * p0.pull()
 * p0.pull(Pin.PULL_DOEN)
 *
 * # create an input pin on pin #2, with a pull up resistor
 * p2 = Pin(2, Pin.IN, Pin.PULL_UP)
 *
 * # read and print the pin value
 * print(p2.value())
 *
 * # reconfigure pin #0 in input mode
 * p0.mode(Pin.IN)
 *
 * # configure an irq callback
 * p0.irq(lambda p:print(p))
 *
 *
 */
void mp_pin_od_write(void *machine_pin, int stat) {
    if (stat == GPIO_LOW) {
        aw_gpio_set(((machine_pin_obj_t *)machine_pin)->pin, AW_GPIO_OUTPUT);
        aw_gpio_set(((machine_pin_obj_t *)machine_pin)->pin, stat);
    } else {
        aw_gpio_set(((machine_pin_obj_t *)machine_pin)->pin, GPIO_MODE_IN || GPIO_PULLUP);
    }
}
void mp_hal_pin_open_set(void *machine_pin, int mode) {
    aw_gpio_set(((machine_pin_obj_t *)machine_pin)->pin, mode);
}

char* mp_hal_pin_get_name(void *machine_pin) {
    return ((machine_pin_obj_t *)machine_pin)->name;
}

const mp_obj_base_t machine_pin_obj_template = {&machine_pin_type};

STATIC void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {

    machine_pin_obj_t *self = self_in;

    if(strlen(self->name) > 0){
        mp_printf(print, "Pin:(%s,%d)", self->name,self->pin);
        return ;
    }
    mp_printf(print, "Pin(%d)", self->pin);
    return ;
}

/**
 * \brief Constructors class
 *
 * machine.Pin(id, mode=-1, pull=-1, value)
 *
 * - `id` is mandatory and can be an arbitrary object.
 *   Among possible value types are: int (an internal Pin identifier),
 *   and tuple (pair of [port, pin]).
 *
 * - `mode` specifies the pin mode, which can be one of:Pin.IN ,
 *   Pin.OUT and Pin.OPEN_DRAIN
 *
 * - `pull` specifies if the pin has a (weak) pull resistor attached, and can be one of:
 *   None - No pull up or down resistor.
 *   Pin.PULL_UP - Pull up resistor enabled.
 *   Pin.PULL_DOWN - Pull down resistor enabled.
 *
 * - `value` is valid only for Pin.OUT and Pin.OPEN_DRAIN modes and specifies
 *   initial output pin value if given, otherwise the state of the pin
 *   peripheral remains unchanged.
 *
 */
typedef struct _mp_pins_request{
    int pinid;
    struct _mp_pins_request *next;
}mp_pins_request;

mp_pins_request *mp_pins_request_head = NULL;

mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    int wanted_pin;
    machine_pin_obj_t *self = m_new_obj(machine_pin_obj_t);

    if (!self) {
        mp_raise_OSError(ENOMEM);
    }

    memset(self->name,0, sizeof(self->name));
    if (MP_OBJ_IS_TYPE(args[0], &mp_type_tuple)) {
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[0], 2, &items);
        const char *pin_name = mp_obj_str_get_str(items[0]);
        strncpy(self->name, pin_name, sizeof(self->name));
        wanted_pin = mp_obj_get_int(items[1]);
    }else if (MP_OBJ_IS_INT(args[0])) {
        wanted_pin = mp_obj_get_int(args[0]);
    }

    if(aw_gpio_pin_request("mppin",(const int *)&wanted_pin,1) != AW_OK){
        mp_raise_msg_varg(&mp_type_OSError,"Request Pin fail.");
    }
    mp_pins_request *node = m_new_obj(mp_pins_request);
    node->pinid = self->pin;
    node->next = NULL;
    if(mp_pins_request_head ==NULL){
        mp_pins_request_head = node;
    }else{
        mp_pins_request * temp = mp_pins_request_head;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = node;
    }

    self->base = machine_pin_obj_template;
    self->pin = wanted_pin;

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)self;
}

STATIC mp_obj_t machine_pin_obj_init_helper(machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT,{.u_int = -1} },
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get io mode
    if(args[ARG_mode].u_int != AW_GPIO_INPUT && \
       args[ARG_mode].u_int != AW_GPIO_OUTPUT && \
       args[ARG_mode].u_int != AW_GPIO_OPEN_DRAIN ){
        mp_raise_ValueError("Not supports mode.");
    }else{
        self->mode = args[ARG_mode].u_int;
    }

    // get pull mode
    if(args[ARG_pull].u_obj == mp_const_none){
        self->pull = AW_GPIO_FLOAT;
    }else if(mp_obj_get_int(args[ARG_pull].u_obj) == AW_GPIO_PULL_DOWN || \
        mp_obj_get_int(args[ARG_pull].u_obj) == AW_GPIO_PULL_UP){
        self->pull = mp_obj_get_int(args[ARG_pull].u_obj);
    }else{
        mp_raise_ValueError("Not supports pull.");
    }

    //set mode and pull
    aw_err_t ret;

    ret = aw_gpio_pin_cfg(self->pin, self->mode | self->pull);
    if(ret != AW_OK){
        mp_raise_OSError(ret);
    }

    //set initial value
    if(args[ARG_value].u_int != -1 && \
       args[ARG_mode].u_int == AW_GPIO_INPUT){
        mp_raise_TypeError("value is valid only for Pin.OUT and Pin.OPEN_DRAIN modes");
    }else{
        ret = aw_gpio_set(self->pin,args[ARG_value].u_int);
        if(ret != AW_OK){
            mp_raise_OSError(ret);
        }
    }

    return mp_const_none;
}

// fast method for getting/setting pin value
STATIC mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = self_in;
    if (n_args == 0) {
        return mp_obj_new_int(aw_gpio_get(self->pin));
    } else {
        aw_gpio_set(self->pin, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

/**
 * \brief Methods
 *
 * Pin.init(mode=-1, pull=-1, value)
 *
 */
STATIC mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_obj_init_obj, 1, machine_pin_obj_init);

// Pin.value([x])
STATIC mp_obj_t machine_pin_obj_value(size_t n_args, const mp_obj_t *args) {
    machine_pin_obj_t *self = args[0];
    if(n_args == 1){
        return mp_obj_new_int(aw_gpio_get(self->pin));
    }else{
        aw_err_t ret = aw_gpio_set(self->pin,mp_obj_get_int(args[1]));
        if(ret != AW_OK){
            mp_raise_OSError(ret);
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_obj_value_obj, 1, 2, machine_pin_obj_value);

/**
 * \brief Pin.on()
 *
 * Set pin to “1” output level.
 *
 * Return none
 *
 */
STATIC mp_obj_t machine_pin_obj_on(mp_obj_t self_in) {

    machine_pin_obj_t *self = self_in;

    aw_err_t ret = aw_gpio_set(self->pin,1);
    if(ret != AW_OK){
        mp_raise_OSError(ret);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_obj_on_obj, machine_pin_obj_on);

/**
 * \brief Pin.off()
 *
 * Set pin to “0” output level.
 *
 * Return none
 *
 */
STATIC mp_obj_t machine_pin_obj_off(mp_obj_t self_in) {

    machine_pin_obj_t *self = self_in;

    aw_err_t ret = aw_gpio_set(self->pin,0);
    if(ret != AW_OK){
        mp_raise_OSError(ret);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_obj_off_obj, machine_pin_obj_off);

/**
 * \brief Pin.mode([mode])
 *
 * Get or set the pin mode.
 *
 * The integer returned will match one of the allowed constants for the mode
 * argument to the init function.
 */
STATIC mp_obj_t machine_pin_obj_mode(size_t n_args, const mp_obj_t *args) {

    machine_pin_obj_t *self = (machine_pin_obj_t *)args[0];

    if(n_args > 1){
        self->mode = mp_obj_get_int(args[1]);
        int flag = self->mode | self->pull;
        aw_err_t ret = aw_gpio_pin_cfg(self->pin,flag);
        if(ret != AW_OK){
            mp_raise_OSError(ret);
        }
        return mp_const_none;
    }else{
        return mp_obj_new_int(self->mode);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_obj_mode_obj, 1, 2, machine_pin_obj_mode);

/**
 * \brief Pin.pull([mode])
 *
 * Get or set the pin pull state.
 *
 * The integer returned will match one of the allowed constants for the pull
 * argument to the init function.
 */
STATIC mp_obj_t machine_pin_obj_pull(size_t n_args, const mp_obj_t *args) {

    machine_pin_obj_t *self = (machine_pin_obj_t *)args[0];

    if(n_args > 1){
        // get pull mode
        if(args[1] == mp_const_none){
            self->pull = AW_GPIO_FLOAT;
        }else if(mp_obj_get_int(args[1]) == AW_GPIO_PULL_DOWN || \
            mp_obj_get_int(args[1]) == AW_GPIO_PULL_UP){
            self->pull = mp_obj_get_int(args[1]);
        }else{
            mp_raise_ValueError("Not supports pull.");
        }

        aw_err_t ret = aw_gpio_pin_cfg(self->pin,self->pull | self->mode);
        if(ret != AW_OK){
            mp_raise_OSError(ret);
        }

        return mp_const_none;
    }else{
        return mp_obj_new_int(self->pull);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_obj_pull_obj, 1, 2, machine_pin_obj_pull);

STATIC void machine_pin_irq_handler(void * arg){
    machine_pin_obj_t *self = arg;
    mp_sched_schedule(self->pin_isr_cb, MP_OBJ_FROM_PTR(self));
}

// pin.irq(handler=None, trigger=IRQ_FALLING|IRQ_RISING)
STATIC mp_obj_t machine_pin_obj_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args){

    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = AW_GPIO_TRIGGER_FALL} },
        //TODO 软件中断待实现
//        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    aw_err_t ret;

    self->pin_isr_cb = args[ARG_handler].u_obj;

    // 连接回调函数到引脚
    if((ret = aw_gpio_trigger_connect(self->pin, machine_pin_irq_handler,\
            (void *)self)) != AW_OK){
        mp_raise_OSError(ret);
    }

    // 设置引脚触发类型，下降沿触发
    if((ret = aw_gpio_trigger_cfg(self->pin, args[ARG_trigger].u_int)) != AW_OK){
        mp_raise_OSError(ret);
    }

    // 开启引脚上的触发器
    if((ret = aw_gpio_trigger_on(self->pin)) != AW_OK){
        mp_raise_OSError(ret);
    }

    // TODO should return an IRQ object
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_obj_irq_obj,2,machine_pin_obj_irq);

void mp_pins_deinit(void){

    aw_err_t ret;

    mp_pins_request * temp = mp_pins_request_head;
    while(temp != NULL){
        ret = aw_gpio_pin_release(&temp->pinid,1);
        assert(!ret);
        mp_pins_request_head = mp_pins_request_head->next;
        m_del_obj(mp_pins_request,temp);
        temp = mp_pins_request_head;
    }

    return ;
}

STATIC const mp_rom_map_elem_t pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&machine_pin_obj_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),    MP_ROM_PTR(&machine_pin_obj_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),    MP_ROM_PTR(&machine_pin_obj_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),    MP_ROM_PTR(&machine_pin_obj_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_mode),    MP_ROM_PTR(&machine_pin_obj_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_pull),    MP_ROM_PTR(&machine_pin_obj_pull_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),    MP_ROM_PTR(&machine_pin_obj_irq_obj) },

    // class attributes
    { MP_ROM_QSTR(MP_QSTR_board),   MP_ROM_PTR(&machine_pin_board_pins_obj_type) },

    // class constants
    //mode
    { MP_ROM_QSTR(MP_QSTR_IN),    MP_ROM_INT(AW_GPIO_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT),    MP_ROM_INT(AW_GPIO_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN),    MP_ROM_INT(AW_GPIO_OPEN_DRAIN) },
    //pull
    { MP_ROM_QSTR(MP_QSTR_PULL_UP),    MP_ROM_INT(AW_GPIO_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN),    MP_ROM_INT(AW_GPIO_PULL_DOWN) },

    //IRQ
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(AW_GPIO_TRIGGER_FALL)},
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING), MP_ROM_INT(AW_GPIO_TRIGGER_RISE)},
    { MP_ROM_QSTR(MP_QSTR_IRQ_HIGH_LEVEL), MP_ROM_INT(AW_GPIO_TRIGGER_HIGH) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_LOW_LEVEL), MP_ROM_INT(AW_GPIO_TRIGGER_LOW) },

};
STATIC MP_DEFINE_CONST_DICT(pin_locals_dict, pin_locals_dict_table);

STATIC mp_uint_t machine_pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    switch (request) {
        case MP_PIN_READ: {
            return aw_gpio_get(self->pin);
        }
        case MP_PIN_WRITE: {
            aw_gpio_set(self->pin, arg);
            return 0;
        }
    }
    return -1;
}

STATIC const mp_pin_p_t machine_pin_pin_p = {
    .ioctl = machine_pin_ioctl,
};

const mp_obj_type_t machine_pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = machine_pin_print,
    .make_new = mp_pin_make_new,
    .call = machine_pin_call,
    .protocol = &machine_pin_pin_p,
    .locals_dict = (mp_obj_t)&pin_locals_dict,
};
#endif

