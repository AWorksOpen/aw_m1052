/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file timer_wrapper.c
 * @brief AWorks implementation of the timer interface.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>

#include "timer_interface.h"

//#include "app_debug.h"

//static aw_inline aw_tick_t aw_sys_tick_diff(aw_tick_t __t0, aw_tick_t __t1)
//{
//    return __t1 - __t0;
//}

bool has_timer_expired(Timer *timer) {
    aw_tick_t diff = aw_sys_tick_diff(timer->begin, aw_sys_tick_get());
//    debug_printt_if(!(timer->ticks_to_wait > diff),
//                    "TimerIsExpired 0x%x,%u, %u, %u, %u",
//                    timer,
//                    timer->begin,
//                    timer->ticks_to_wait,
//                    aw_sys_tick_get(),
//                    diff);
    return (timer->ticks_to_wait > diff) ? false : true;
}

void countdown_ms(Timer *timer, uint32_t timeout) {
    timer->ticks_to_wait = aw_ms_to_ticks(timeout); /* convert milliseconds to ticks */
    timer->begin = aw_sys_tick_get(); /* Record the time at which this function was entered. */
}

uint32_t left_ms(Timer *timer) {
    aw_tick_t diff = aw_sys_tick_diff(timer->begin, aw_sys_tick_get());
    return (timer->ticks_to_wait > diff) ? aw_ticks_to_ms(timer->ticks_to_wait - diff) : 0;
}

void countdown_sec(Timer *timer, uint32_t timeout) {
    countdown_ms(timer, timeout * 1000);
}

void init_timer(Timer *timer) {
    timer->ticks_to_wait = 0;
}

void init_timer_delta(Timer *timer, size_t sec, size_t usec) {
    timer->ticks_to_wait = aw_ms_to_ticks(sec * 1000 + usec / 1000);
    timer->begin = aw_sys_tick_get();
}

#ifdef __cplusplus
}
#endif
