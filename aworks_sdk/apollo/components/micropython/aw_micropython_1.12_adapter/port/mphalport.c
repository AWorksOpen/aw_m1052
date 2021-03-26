
#include "mphalport.h"
#include "py/mpconfig.h"
#include "aw_serial.h"
#include "aw_mpy_cfg.h"
#include "aw_getchar.h"
#include "lib/utils/interrupt_char.h"
#include "py/runtime.h"

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    int ch;
    for (;;) {
        ch = aw_getchar();
        if (ch != -1) {
            break;
        }
        MICROPY_EVENT_POLL_HOOK;
        aw_mdelay(1);
    }
    return ch;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    aw_serial_write(MPY_CONSOLE_COMID, str, len);
}

/*上电后运行时间*/
mp_uint_t mp_hal_ticks_us(void) {

    aw_tick_t   ticks;

    ticks = aw_sys_tick_get();
    int ms = aw_ticks_to_ms(ticks);
    return ms * 1000;
}

/*上电后运行时间(ms)*/
mp_uint_t mp_hal_ticks_ms(void) {

    aw_tick_t   ticks;

    ticks = aw_sys_tick_get();

    return aw_ticks_to_ms(ticks);
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return aw_sys_tick_get();
}

void mp_hal_delay_us(mp_uint_t us) {
    aw_tick_t t0 = aw_sys_tick_get(), t1, dt;
    uint64_t dtick =aw_ms_to_ticks(us / 1000);
    while (1) {
        t1 = aw_sys_tick_get();
        dt = t1 - t0;
        if (dt >= dtick) {
            break;
        }
        // we have enough time to service pending events
        // (don't use MICROPY_EVENT_POLL_HOOK because it also yields)
        mp_handle_pending(true);
    }
}

void mp_hal_delay_ms(mp_uint_t ms) {
    aw_tick_t t0 = aw_sys_tick_get(), t1, dt;
    uint64_t dtick =aw_ms_to_ticks(ms);
    while (1) {
        t1 = aw_sys_tick_get();
        dt = t1 - t0;
        if (dt >= dtick) {
            break;
        }
        MICROPY_EVENT_POLL_HOOK;
        aw_mdelay(1);
    }
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags){
    return poll_flags;
}
