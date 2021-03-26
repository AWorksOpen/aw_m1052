#ifndef __USRCLASS1_H
#define __USRCLASS1_H

#include "py/obj.h"
#include "errno.h"
#include "py/nlr.h"
#include "py/runtime.h"
#include <string.h>
#include <stdio.h>
#include "py/objarray.h"

#if MICROPY_PY_USRMOD
#define USRCLASS_CONST_AGE 30

extern const mp_obj_type_t usrmod_usrclass1_type;
#endif

#endif//__USRCLASS1_H
