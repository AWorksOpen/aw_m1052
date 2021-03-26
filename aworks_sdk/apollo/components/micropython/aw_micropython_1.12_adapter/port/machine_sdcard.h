
#ifndef __MACHINE_SDCARD_H__
#define __MACHINE_SDCARD_H__

#include <string.h>
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "aw_sdcard.h"

#if MICROPY_HW_ENABLE_SDCARD
typedef struct _sdcard_obj_t {
    mp_obj_base_t base;
    const char *pdrv;
    int sector_size;
    int sector_count;
    aw_sdcard_dev_t *p_sdcard;
} sdcard_card_obj_t;

extern const mp_obj_type_t machine_sdcard_type;

#endif

#endif
