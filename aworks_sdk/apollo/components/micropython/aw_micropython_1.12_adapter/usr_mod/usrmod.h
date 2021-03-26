#ifndef __USRMOD_H
#define __USRMOD_H

#include <errno.h>
#include "py/nlr.h"
#include "py/runtime.h"
#include <string.h>
#include <stdio.h>
#include "usrclass1.h"
#include "py/obj.h"

#if MICROPY_PY_USRMOD
extern const mp_obj_module_t mp_module_usrmod;
#endif

#endif//__USRMOD_H
