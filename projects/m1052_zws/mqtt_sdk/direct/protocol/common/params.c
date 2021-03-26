/** 
 * 功能说明：
 *     1.参数解析和打包
 *
 * 修改历史：
 *     1.2017-8-5 李先静<lixianjing@zlg.cn> 创建。
 */

#include "params.h"

#define VERSION 0x00

params_t* request_init(params_t* params, char* buff, size_t capacity) {

    return_value_if_fail(params != NULL && buff != NULL, NULL);

    params->buff = buff;
    params->size = 1; /*保留一个字节*/
    params->capacity = capacity;
    memset(buff, 0x00, capacity);
    params->buff[0] = VERSION;

    return params;
}

params_t* response_init(params_t* params, char* buff, size_t size) {
    return_value_if_fail(params != NULL && buff != NULL, NULL);

    params->buff = buff;
    params->size = size;
    params->capacity = size;
    
    assert(*buff == VERSION);

    return params;
}

BOOL params_append_string(params_t* params, const char* key, const char* value) {
    char* p = NULL;
    size_t klen = 0;
    size_t vlen = 0;
    return_value_if_fail(params != NULL && params->buff != NULL && key != NULL && value != NULL, FALSE);

    klen = strlen(key);
    vlen = strlen(value);
    return_value_if_fail((params->size + klen + vlen + 2) < params->capacity, FALSE);

    p = params->buff + params->size;
    memcpy(p, key, klen+1);
    memcpy(p+klen+1, value, vlen+1);
    params->size += klen + vlen + 2;

    return TRUE;
}

BOOL params_append_int(params_t* params, const char* key, int32_t value) {
    char buff[32];
    snprintf(buff, sizeof(buff), "%d", value);

    return params_append_string(params, key, buff);
}

BOOL params_append_uint(params_t* params, const char* key, uint64_t value) {
    char buff[32];
    snprintf(buff, sizeof(buff), "%llu", value);

    return params_append_string(params, key, buff);
}

BOOL params_append_float(params_t* params, const char* key, float value) {
    char buff[64];
    snprintf(buff, sizeof(buff), "%f", value);

    return params_append_string(params, key, buff);
}

BOOL params_append_double(params_t* params, const char* key, double value) {
    char buff[64];
    snprintf(buff, sizeof(buff), "%lf", value);

    return params_append_string(params, key, buff);
}

BOOL params_has(params_t* params, const char* key) {
    return params_get(params, key) != NULL;
}

const char* params_get(params_t* params, const char* key) {
    const char* p = NULL;
    return_value_if_fail(params != NULL && params->buff != NULL && key != NULL, NULL);

    p = params->buff + 1;
    assert(params->buff[0] == VERSION);
    for(; (p-params->buff) < (int)params->size; p += strlen(p)+1) {
        if(strcmp(key, p) == 0) {
            p += strlen(p) + 1;

            return p;
        }

        p += strlen(p) + 1;
    }

    return NULL;
}

BOOL params_get_int(params_t* params, const char* key, int32_t* value) {
    const char* str = params_get(params, key);

    if(str != NULL && value != NULL) {
        *value = atoi(str);
    }

    return str != NULL;
}

BOOL params_get_uint(params_t *params, const char* key, uint64_t* value) {
    const char* str = params_get(params, key);

    if(str != NULL && value != NULL) {
        *value = atoll(str);
    }

    return str != NULL;
}

BOOL params_get_float(params_t* params, const char* key, float* value) {
    const char* str = params_get(params, key);

    if(str != NULL && value != NULL) {
        *value = (float)atof(str);
    }

    return str != NULL;
}

BOOL params_get_double(params_t* params, const char* key, double* value) {
    const char* str = params_get(params, key);

    if(str != NULL && value != NULL) {
        *value = atof(str);
    }

    return str != NULL;
}

BOOL params_get_string(params_t* params, const char* key, const char** value) {
    const char* str = params_get(params, key);

    if(str != NULL && value != NULL) {
        *value = str;
    }

    return str != NULL;
}

BOOL params_dump(params_t* params) {
    const char* p = NULL;
    return_value_if_fail(params != NULL, FALSE);
    
    printf("params: ");
    for(p = params->buff; p < (params->buff + params->size); p++) {
        printf("%s:", p);
        p += strlen(p);
    }
    printf("\n");

    return TRUE;
}

