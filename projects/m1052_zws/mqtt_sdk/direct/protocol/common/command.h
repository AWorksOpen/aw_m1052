/** 
 * 功能说明：
 *     1.设备端命令的接口定义。
 *
 * 修改历史：
 *     1.2017-8-5 李先静<lixianjing@zlg.cn> 创建。
 */

#include "params.h"

#ifndef DEVICE_COMMAND_H
#define DEVICE_COMMAND_H

struct _command_t;
typedef struct _command_t command_t;

struct _client_t;

typedef BOOL (*command_exec_t)(command_t* cmd, struct _client_t* client, params_t* params); 

struct _command_t {
    const char* name;
    command_exec_t exec;
};

static inline BOOL command_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    return_value_if_fail(cmd != NULL && client != NULL && params != NULL, FALSE);

    return cmd->exec(cmd, client, params);
}

#endif/*DEVICE_COMMAND_H*/

