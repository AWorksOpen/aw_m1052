#include "aworks.h"
#include "aw_led.h"
#include "aw_buzzer.h"

#include "client.h"


#define __BAD_PARAMETERS "bad parameters"
#define __goto_done_if_fail(p, msg) if(!(p)) {printf("%s:%d: %s\n", __func__, __LINE__, msg); message = msg; goto done;}


aw_local BOOL __led_exec (command_t        *cmd,
                          struct _client_t *client,
                          params_t         *params)
{
    int32_t     cmdid = 0;
    BOOL        ret = TRUE;
    const char* message = "ok";
    const char* raw = NULL;

    /* ���������� */
    __goto_done_if_fail(cmd != NULL &&
                        client != NULL &&
                        params != NULL,
                        __BAD_PARAMETERS);
    __goto_done_if_fail(params_get_string(params, "power", &raw),
                        __BAD_PARAMETERS);
    (void)cmd;

    params_dump(params);

    /* ִ���������LED */
    if (strcmp("on", raw) == 0) {
        aw_led_on(0);
    } else if (strcmp("off", raw) == 0) {
        aw_led_off(0);
    }

done:
    /* Ӧ���ƶ� */
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}



aw_local BOOL __buzzer_exec (command_t        *cmd,
                             struct _client_t *client,
                             params_t         *params)
{
    int32_t     cmdid = 0;
    BOOL        ret = TRUE;
    const char* message = "ok";
    int32_t     time, volume;

    /* ������ */
    __goto_done_if_fail(cmd != NULL &&
                        client != NULL
                        && params != NULL,
                        __BAD_PARAMETERS);
    __goto_done_if_fail(params_get_int(params, "time", &time),
                        __BAD_PARAMETERS);
    __goto_done_if_fail(params_get_int(params, "volume", &volume),
                        __BAD_PARAMETERS);

    (void)cmd;

    params_dump(params);

    /* ���Ʒ����� */
    aw_buzzer_loud_set(volume);
    aw_buzzer_beep(time);

done:
    /* Ӧ�� */
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}


/* ���豸���������б� */
aw_local command_t __ctrl_cmds[] = {
    {"led",     __led_exec},
    {"buzzer",  __buzzer_exec},
    {NULL, NULL}
};




command_t* sdk_demo_devtype_get_commands (void)
{
    return __ctrl_cmds;
}

