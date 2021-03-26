
#include "client.h"
#include "errors.h"
#include "warnings.h"
#include "device_consts.h"

#ifndef DEVICE_DEVICE_H
#define DEVICE_DEVICE_H

BEGIN_C_DECLS

BOOL myinvert_init(client_t* c, const char* owner, const char* id);
BOOL myinvert_get_all_data(params_t* params);
BOOL myzggw_init(client_t* c, const char* owner, const char* id);
BOOL myzggw_get_all_data(params_t* params);


END_C_DECLS

#endif/*DEVICE_DEVICE_H*/

