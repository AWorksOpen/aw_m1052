/** 
 * 功能说明：
 *     1.设置接口的参考实现。
 *
 * 修改历史：
 *     1.2017-8-5 李先静<lixianjing@zlg.cn> 创建。
 */

#include "aw_time.h"
#include "client.h"
#include "settings.h"

static char s_online[32];
static settings_t settings;

#define __TIME_ZONE  8

static BOOL dummy_settings_set(settings_t* settings, const char* key, const char* value) {
    if(strcmp(STR_KEY_TIME, key) == 0) {
        aw_time_t time;
        aw_tm_t   tm;

        strncpy(s_online, value, sizeof(s_online)-1);

        /*设置系统时间。*/
        time = atoi(value);
        time += (__TIME_ZONE * 3600);

        aw_time_to_tm(&time, &tm);
        aw_tm_set(&tm);

        return TRUE; 
    } else if(strcmp(STR_KEY_ALREADY_ONLIEN, key) == 0) {
        /*设置上线状态*/
        return TRUE;
    }

    (void)settings;
    return FALSE;
}

static const char* dummy_settings_get(settings_t* settings, const char* key) {
    if(strcmp(STR_KEY_ALREADY_ONLIEN, key) == 0) {
        /*获取上线状态*/
        return s_online; 
    }else{
        return NULL;
    }
    
    (void)settings;
}

settings_t* settings_get_instance() {
    settings.get = dummy_settings_get;
    settings.set = dummy_settings_set;

    return &settings;
}

