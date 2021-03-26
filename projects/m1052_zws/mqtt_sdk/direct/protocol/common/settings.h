/** 
 * 功能说明：
 *     1.设置接口的抽象。包括设置系统时间和存储一些状态。
 *
 * 修改历史：
 *     1.2017-8-5 李先静<lixianjing@zlg.cn> 创建。
 */

#include "types_def.h"

#ifndef DEVICE_SETTINGS_H
#define DEVICE_SETTINGS_H

BEGIN_C_DECLS

struct _settings_t;
typedef struct _settings_t settings_t;

typedef BOOL        (*settings_set_t)(settings_t* settings, const char* key, const char* value);
typedef const char* (*settings_get_t)(settings_t* settings, const char* key);

struct _settings_t {
    settings_set_t set;
    settings_get_t get;
};

static inline BOOL settings_set(settings_t* settings, const char* key, const char* value) {
    return_value_if_fail(settings != NULL && settings->set != NULL && key != NULL && value != NULL, FALSE);

    return settings->set(settings, key, value);
}

static inline const char* settings_get(settings_t* settings, const char* key) {
    return_value_if_fail(settings != NULL && settings->get != NULL && key != NULL, FALSE);

    return settings->get(settings, key);
}

/*已经上线的标志*/
#define STR_KEY_ALREADY_ONLIEN   "already_online"

settings_t* settings_get_instance();

END_C_DECLS

#endif/*DEVICE_SETTINGS_H*/

