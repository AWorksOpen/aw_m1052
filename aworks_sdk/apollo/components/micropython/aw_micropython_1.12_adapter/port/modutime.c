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

#include "py/mpconfig.h"
#if MICROPY_PY_UTIME

#include "lib/timeutils/timeutils.h"

#include "py/runtime.h"
#include "py/smallint.h"
#include "py/mphal.h"
#include "utime_mphal.h"
#include "time.h"
#include "math.h"
#include "py/obj.h"
#include "aw_types.h"
#include <sys/time.h>
#include "aw_time.h"
#include <assert.h>

aw_time_t seconds_since_1900_to_2000(void){
    aw_tm_t       tm;            // 细分时间
    aw_timespec_t timespec;      // 日历时间

    /* 设置当前时间 */
    tm.tm_sec    = 0;           //设置秒
    tm.tm_min    = 0;           //设置分
    tm.tm_hour   = 0;           //设置小时
    tm.tm_mday   = 1;            //设置日
    tm.tm_mon    = 1 - 1;        //设置月，实际月份要减一
    tm.tm_year   = 2000 - 1900;  //设置年，距1900的年数

    aw_err_t ret = aw_tm_to_time(&tm, &timespec.tv_sec);   // 转换为日历时间
    assert(!ret);

    return timespec.tv_sec;
}
STATIC mp_obj_t mod_utime_time(void) {
    aw_err_t ret;
    aw_timespec_t timespec;      // 日历时间

    ret = aw_timespec_get(&timespec);
    assert(!ret);

    mp_int_t seconds = timespec.tv_sec - seconds_since_1900_to_2000();
    return mp_obj_new_int(seconds);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_utime_time_obj, mod_utime_time);


STATIC mp_obj_t time_localtime(size_t n_args, const mp_obj_t *args) {
    timeutils_struct_time_t tm;
    mp_int_t seconds;
    if (n_args == 0 || args[0] == mp_const_none) {
        aw_err_t ret;
        aw_timespec_t timespec;      // 日历时间
        ret = aw_timespec_get(&timespec);
        assert(!ret);
        seconds = timespec.tv_sec - seconds_since_1900_to_2000();
    } else {
        seconds = mp_obj_get_int(args[0]);
    }
    timeutils_seconds_since_2000_to_struct_time(seconds, &tm);
    mp_obj_t tuple[8] = {
        tuple[0] = mp_obj_new_int(tm.tm_year),
        tuple[1] = mp_obj_new_int(tm.tm_mon),
        tuple[2] = mp_obj_new_int(tm.tm_mday),
        tuple[3] = mp_obj_new_int(tm.tm_hour),
        tuple[4] = mp_obj_new_int(tm.tm_min),
        tuple[5] = mp_obj_new_int(tm.tm_sec),
        tuple[6] = mp_obj_new_int(tm.tm_wday),
        tuple[7] = mp_obj_new_int(tm.tm_yday),
    };
    return mp_obj_new_tuple(8, tuple);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(time_localtime_obj, 0, 1, time_localtime);

STATIC mp_obj_t time_mktime(mp_obj_t tuple) {

    size_t len;
    mp_obj_t *elem;
    mp_obj_get_array(tuple, &len, &elem);

    // localtime generates a tuple of len 8. CPython uses 9, so we accept both.
    if (len < 8 || len > 9) {
        mp_raise_msg_varg(&mp_type_TypeError, "mktime needs a tuple of length 8 or 9 (%d given)", len);
    }

    return mp_obj_new_int_from_uint(timeutils_mktime(mp_obj_get_int(elem[0]),
        mp_obj_get_int(elem[1]), mp_obj_get_int(elem[2]), mp_obj_get_int(elem[3]),
        mp_obj_get_int(elem[4]), mp_obj_get_int(elem[5])));
}
MP_DEFINE_CONST_FUN_OBJ_1(time_mktime_obj, time_mktime);

STATIC const mp_rom_map_elem_t mp_module_utime_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_utime) },
    { MP_ROM_QSTR(MP_QSTR_localtime), MP_ROM_PTR(&time_localtime_obj) },
    { MP_ROM_QSTR(MP_QSTR_mktime), MP_ROM_PTR(&time_mktime_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&mp_utime_sleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep_ms), MP_ROM_PTR(&mp_utime_sleep_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep_us), MP_ROM_PTR(&mp_utime_sleep_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&mod_utime_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_ms), MP_ROM_PTR(&mp_utime_ticks_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_us), MP_ROM_PTR(&mp_utime_ticks_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_cpu), MP_ROM_PTR(&mp_utime_ticks_cpu_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_add), MP_ROM_PTR(&mp_utime_ticks_add_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_diff), MP_ROM_PTR(&mp_utime_ticks_diff_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_utime_globals, mp_module_utime_globals_table);

const mp_obj_module_t mp_module_utime = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_utime_globals,
};

#endif // MICROPY_PY_UTIME
