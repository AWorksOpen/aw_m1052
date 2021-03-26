#include "client.h"

#include "commands.h"

#define BAD_PARAMETERS "bad parameters"
#define goto_done_if_fail(p, msg) if(!(p)) {printf("%s:%d: %s\n", __func__, __LINE__, msg); message = msg; goto done;}

#define STR_NAME_MYINVERT_SET "set"
#define STR_NAME_MYINVERT_SET_CONFIG "set_config"
#define STR_NAME_MYINVERT_SYNC_TIME "sync_time"
#define STR_NAME_MYINVERT_NOTIFY_UPGRADE "notify_upgrade"
#define STR_NAME_MYINVERT_EXEC_UPGRADE "exec_upgrade"
#define STR_NAME_MYINVERT_REQ_REPORT "req_report"
#define STR_NAME_MYINVERT_PASS_THROUGH "pass_through"

static BOOL myinvert_set_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    int cmdid = 0;
    BOOL ret = TRUE;
    const char* message = "ok";
    const char* name = NULL;
    const char* value = NULL;
    goto_done_if_fail(cmd != NULL && client != NULL && params != NULL, BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "name", &name), BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "value", &value), BAD_PARAMETERS);
    (void)cmd;
    printf("%s\n", __func__);
    params_dump(params);
    /*TODO: implement the command*/
done:
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}

static BOOL myinvert_set_config_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    int cmdid = 0;
    BOOL ret = TRUE;
    const char* message = "ok";
    goto_done_if_fail(cmd != NULL && client != NULL && params != NULL, BAD_PARAMETERS);
    (void)cmd;
    printf("%s\n", __func__);
    params_dump(params);
    /*TODO: implement the command*/
done:
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}

static BOOL myinvert_sync_time_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    int cmdid = 0;
    BOOL ret = TRUE;
    const char* message = "ok";
    int time = 0;
    goto_done_if_fail(cmd != NULL && client != NULL && params != NULL, BAD_PARAMETERS);
    goto_done_if_fail(params_get_int(params, "time", &time), BAD_PARAMETERS);
    (void)cmd;
    printf("%s\n", __func__);
    params_dump(params);
    /*TODO: implement the command*/
done:
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}

static BOOL myinvert_notify_upgrade_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    int cmdid = 0;
    BOOL ret = TRUE;
    const char* message = "ok";
    const char* model = NULL;
    const char* version = NULL;
    const char* url = NULL;
    goto_done_if_fail(cmd != NULL && client != NULL && params != NULL, BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "model", &model), BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "version", &version), BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "url", &url), BAD_PARAMETERS);
    (void)cmd;
    printf("%s\n", __func__);
    params_dump(params);
    /*TODO: implement the command*/
done:
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}

static BOOL myinvert_exec_upgrade_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    int cmdid = 0;
    BOOL ret = TRUE;
    const char* message = "ok";
    const char* model = NULL;
    const char* version = NULL;
    const char* url = NULL;
    goto_done_if_fail(cmd != NULL && client != NULL && params != NULL, BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "model", &model), BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "version", &version), BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "url", &url), BAD_PARAMETERS);
    (void)cmd;
    printf("%s\n", __func__);
    params_dump(params);
    /*TODO: implement the command*/
done:
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}

static BOOL myinvert_req_report_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    int cmdid = 0;
    BOOL ret = TRUE;
    const char* message = "ok";
    goto_done_if_fail(cmd != NULL && client != NULL && params != NULL, BAD_PARAMETERS);
    (void)cmd;
    printf("%s\n", __func__);
    params_dump(params);
    /*TODO: implement the command*/
done:
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}

static BOOL myinvert_pass_through_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    int cmdid = 0;
    BOOL ret = TRUE;
    const char* message = "ok";
    const char* raw = NULL;
    goto_done_if_fail(cmd != NULL && client != NULL && params != NULL, BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "raw", &raw), BAD_PARAMETERS);
    (void)cmd;
    printf("%s\n", __func__);
    params_dump(params);
    /*TODO: implement the command*/
done:
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}

static command_t myinvert_cmds[] = {
    {STR_NAME_MYINVERT_SET, myinvert_set_exec},
    {STR_NAME_MYINVERT_SET_CONFIG, myinvert_set_config_exec},
    {STR_NAME_MYINVERT_SYNC_TIME, myinvert_sync_time_exec},
    {STR_NAME_MYINVERT_NOTIFY_UPGRADE, myinvert_notify_upgrade_exec},
    {STR_NAME_MYINVERT_EXEC_UPGRADE, myinvert_exec_upgrade_exec},
    {STR_NAME_MYINVERT_REQ_REPORT, myinvert_req_report_exec},
    {STR_NAME_MYINVERT_PASS_THROUGH, myinvert_pass_through_exec},
    {NULL, NULL}
};

command_t* myinvert_get_commands() {
    return myinvert_cmds;
}
#define STR_NAME_MYZGGW_SET_CONFIG "set_config"
#define STR_NAME_MYZGGW_SYNC_TIME "sync_time"
#define STR_NAME_MYZGGW_NOTIFY_UPGRADE "notify_upgrade"
#define STR_NAME_MYZGGW_EXEC_UPGRADE "exec_upgrade"
#define STR_NAME_MYZGGW_REQ_REPORT "req_report"
#define STR_NAME_MYZGGW_PASS_THROUGH "pass_through"

static BOOL myzggw_set_config_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    int cmdid = 0;
    BOOL ret = TRUE;
    const char* message = "ok";
    goto_done_if_fail(cmd != NULL && client != NULL && params != NULL, BAD_PARAMETERS);
    (void)cmd;
    printf("%s\n", __func__);
    params_dump(params);
    /*TODO: implement the command*/
done:
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}

static BOOL myzggw_sync_time_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    int cmdid = 0;
    BOOL ret = TRUE;
    const char* message = "ok";
    int time = 0;
    goto_done_if_fail(cmd != NULL && client != NULL && params != NULL, BAD_PARAMETERS);
    goto_done_if_fail(params_get_int(params, "time", &time), BAD_PARAMETERS);
    (void)cmd;
    printf("%s\n", __func__);
    params_dump(params);
    /*TODO: implement the command*/
done:
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}

static BOOL myzggw_notify_upgrade_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    int cmdid = 0;
    BOOL ret = TRUE;
    const char* message = "ok";
    const char* model = NULL;
    const char* version = NULL;
    const char* url = NULL;
    goto_done_if_fail(cmd != NULL && client != NULL && params != NULL, BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "model", &model), BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "version", &version), BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "url", &url), BAD_PARAMETERS);
    (void)cmd;
    printf("%s\n", __func__);
    params_dump(params);
    /*TODO: implement the command*/
done:
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}

static BOOL myzggw_exec_upgrade_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    int cmdid = 0;
    BOOL ret = TRUE;
    const char* message = "ok";
    const char* model = NULL;
    const char* version = NULL;
    const char* url = NULL;
    goto_done_if_fail(cmd != NULL && client != NULL && params != NULL, BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "model", &model), BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "version", &version), BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "url", &url), BAD_PARAMETERS);
    (void)cmd;
    printf("%s\n", __func__);
    params_dump(params);
    /*TODO: implement the command*/
done:
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}

static BOOL myzggw_req_report_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    int cmdid = 0;
    BOOL ret = TRUE;
    const char* message = "ok";
    goto_done_if_fail(cmd != NULL && client != NULL && params != NULL, BAD_PARAMETERS);
    (void)cmd;
    printf("%s\n", __func__);
    params_dump(params);
    /*TODO: implement the command*/
done:
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}

static BOOL myzggw_pass_through_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    int cmdid = 0;
    BOOL ret = TRUE;
    const char* message = "ok";
    const char* raw = NULL;
    goto_done_if_fail(cmd != NULL && client != NULL && params != NULL, BAD_PARAMETERS);
    goto_done_if_fail(params_get_string(params, "raw", &raw), BAD_PARAMETERS);
    (void)cmd;
    printf("%s\n", __func__);
    params_dump(params);
    /*TODO: implement the command*/
done:
    if(params_get_int(params, STR_KEY_CMDID, &cmdid)) {
        client_reply_cmd(client, cmdid, ret, message);
    }
    return ret;
}

static command_t myzggw_cmds[] = {
    {STR_NAME_MYZGGW_SET_CONFIG, myzggw_set_config_exec},
    {STR_NAME_MYZGGW_SYNC_TIME, myzggw_sync_time_exec},
    {STR_NAME_MYZGGW_NOTIFY_UPGRADE, myzggw_notify_upgrade_exec},
    {STR_NAME_MYZGGW_EXEC_UPGRADE, myzggw_exec_upgrade_exec},
    {STR_NAME_MYZGGW_REQ_REPORT, myzggw_req_report_exec},
    {STR_NAME_MYZGGW_PASS_THROUGH, myzggw_pass_through_exec},
    {NULL, NULL}
};

command_t* myzggw_get_commands() {
    return myzggw_cmds;
}
