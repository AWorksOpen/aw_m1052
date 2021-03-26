#include "warnings.h"

#define BUFF_SIZE 256

BOOL myinvert_report_temperature_high(client_t* client, float value){
    params_t params;
    char buff[BUFF_SIZE];
    return_value_if_fail(client != NULL, FALSE);

    request_init(&params, buff, sizeof(buff));
    params_append_float(&params, "value", value);
    params_append_string(&params, STR_KEY_NAME, "temperature_high");

    return client_report_warning(client, &params);
}

