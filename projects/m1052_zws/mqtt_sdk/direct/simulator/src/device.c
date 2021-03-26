
#include "device.h"
#include "client.h"
#include "commands.h"
#include "settings.h"


BOOL myinvert_init(client_t* c, const char* owner, const char* id) { 
    client_init(c, owner, STR_DEVICE_TYPE_MYINVERT, id);

    client_set_cmds(c, myinvert_get_commands());
    client_set_settings(c, settings_get_instance());

    return TRUE;
}

BOOL myzggw_init(client_t* c, const char* owner, const char* id) { 
    client_init(c, owner, STR_DEVICE_TYPE_MYZGGW, id);

    client_set_cmds(c, myzggw_get_commands());
    client_set_settings(c, settings_get_instance());

    return TRUE;
}

BOOL myinvert_get_all_data(params_t* params) {
    params->size = 1; /*保留版本号*/

    params_append_int(params, STR_KEY_TIME, time(0));
    /*TODO: 请在这里设置真实数据*/
    params_append_float(params, "total_energy", 100);
    params_append_float(params, "today_energy", 120);
    params_append_float(params, "temperature", 0);
    params_append_float(params, "gfci", 0);
    params_append_float(params, "bus_volt", 0);
    params_append_float(params, "power", 0);
    params_append_float(params, "q_power", 0);
    params_append_float(params, "pf", 0);
    params_append_float(params, "pv1_volt", 0);
    params_append_float(params, "pv1_curr", 0);
    params_append_float(params, "pv2_volt", 0);
    params_append_float(params, "pv2_curr", 0);
    params_append_float(params, "pv3_volt", 0);
    params_append_float(params, "pv3_curr", 0);
    params_append_float(params, "l1_volt", 0);
    params_append_float(params, "l1_curr", 0);
    params_append_float(params, "l1_freq", 0);
    params_append_float(params, "l1_dci", 0);
    params_append_float(params, "l1_power", 0);
    params_append_float(params, "l1_pf", 0);

   return TRUE;
}
