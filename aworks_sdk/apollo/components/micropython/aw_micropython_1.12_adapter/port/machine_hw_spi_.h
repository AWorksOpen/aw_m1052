#include <stdio.h>
#include <string.h>
#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/machine_spi.h"
#include "aw_spi.h"
#include "awbl_spibus.h"
#include "awbus_lite.h"
#include "mpconfigport.h"

#if MICROPY_PY_MACHINE_SPI

// if a port didn't define MSB/LSB constants then provide them
#ifndef MICROPY_PY_MACHINE_SPI_MSB
#define MICROPY_PY_MACHINE_SPI_MSB (0)
#define MICROPY_PY_MACHINE_SPI_LSB (1)
#endif

#endif
