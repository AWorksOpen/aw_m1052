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
#ifndef __MPHALPORT_H__
#define __MPHALPORT_H__

#include "aw_mpy_cfg.h"
#include "aw_system.h"
#include "aw_delay.h"
#include "aw_gpio.h"

/** \brief 板级资源配置*/
#if AW_MPY_IMXRT105X
#include "boards/imxrt105x/mpconfigboard.h"
#elif AW_MPY_IMX28X
#include "boards/imx28x/mpconfigboard.h"
#elif AW_MPY_IMXRT106X
#include "boards/imxrt106x/mpconfigboard.h"
#endif

#define UINT_FMT                    "%u"        /** \brief 无符号整形*/
#define INT_FMT                     "%d"        /** \brief 整型*/
typedef int                         mp_int_t;   /** \brief 整型*/
typedef unsigned                    mp_uint_t;  /** \brief 无符号整形*/
typedef long                        mp_off_t;   /** \brief 长整型*/
#define bool                        aw_bool_t   /** \brief bool类型*/
#define true                        AW_TRUE     /** \brief true值*/
#define false                       AW_FALSE    /** \brief false值*/
#define MP_HAL_PIN_FMT              "%s"


/**< \brief 引脚功能配置*/
#define GPIO_LOW                        0x00
#define GPIO_HIGH                       0x01
#define GPIO_MODE_IN                    AW_GPIO_INPUT
#define GPIO_PULLUP                     AW_GPIO_PULL_UP
#define GPIO_PULLDOWN                   AW_GPIO_PULL_DOWN
#define GPIO_NOPULL                     AW_GPIO_FLOAT
#define GPIO_MODE_OUT_PP                AW_GPIO_OUTPUT | AW_GPIO_PUSH_PULL
#define GPIO_MODE_OUT_OD                AW_GPIO_OUTPUT | AW_GPIO_OPEN_DRAIN
#define GPIO_MODE_IT_RISING             AW_GPIO_TRIGGER_RISE
#define GPIO_MODE_IT_FALLING            AW_GPIO_TRIGGER_FALL
#define GPIO_MODE_IT_RISING_FALLING     AW_GPIO_TRIGGER_BOTH_EDGES
#define GPIO_MODE_IT_HIGH               AW_GPIO_TRIGGER_HIGH
#define GPIO_MODE_IT_LOW                AW_GPIO_TRIGGER_LOW
#define GPIO_MODE_IT_OFF                AW_GPIO_TRIGGER_OFF



mp_uint_t mp_hal_ticks_us(void);
mp_uint_t mp_hal_ticks_ms(void);
mp_uint_t mp_hal_ticks_cpu(void);
void mp_hal_delay_us(mp_uint_t delay);
void mp_hal_delay_ms(mp_uint_t delay);
uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags);


extern void mp_hal_set_interrupt_char (int c);
extern void mp_pin_od_write(void *machine_pin, int stat);

// needed for machine.I2C
#define mp_hal_delay_us_fast(us) mp_hal_delay_us(us)
#define mp_hal_pin_od_low(pin)   mp_pin_od_write(pin, GPIO_LOW)
#define mp_hal_pin_od_high(pin)  mp_pin_od_write(pin, GPIO_HIGH)
#define mp_hal_pin_open_drain(p) mp_hal_pin_open_set(p, GPIO_MODE_OUT_OD)

// needed for soft machine.SPI
#define mp_hal_pin_output(p)     mp_hal_pin_open_set(p, AW_GPIO_OUTPUT)
#define mp_hal_pin_input(p)      mp_hal_pin_open_set(p, AW_GPIO_INPUT)
#define mp_hal_pin_name(p)       mp_hal_pin_get_name(p)
#define mp_hal_pin_high(p)       mp_hal_pin_write(p, 1)


#endif
