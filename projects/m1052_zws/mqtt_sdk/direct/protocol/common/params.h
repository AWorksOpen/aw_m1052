/** 
 * 功能说明：
 *     1.参数解析和打包
 *
 * 修改历史：
 *     1.2017-8-5 李先静<lixianjing@zlg.cn> 创建。
 */

#include "types_def.h"

#ifndef DEVICE_PARAMS_H
#define DEVICE_PARAMS_H

BEGIN_C_DECLS

/**
 * 参数集合。key和value都存为字符串，并自带空字符。如：key1\0value1\0vkey2\0value2\0。
 * XXX: 第一个字节保留(设置为0x00)，方便以后支持其它类型的打包方式。
 */
typedef struct _params_t{
    char*  buff;
    size_t size;
    size_t capacity;
}params_t;

/**
 * 初始化一个用于请求的params，可以向里面追加参数。
 */
params_t* request_init(params_t* params, char* buff, size_t capacity);

/**
 * 初始化一个回应的params，可以从里面获取参数。
 */
params_t* response_init(params_t* params, char* buff, size_t size);

/**
 * 向params里追加一个字符串参数。
 */
BOOL params_append_string(params_t* params, const char* key, const char* value);

/**
 * 向params里追加一个int参数。
 */
BOOL params_append_int(params_t* params, const char* key, int32_t value);

/**
 * 向params里追加一个uint参数。
 */
BOOL params_append_uint(params_t* params, const char* key, uint64_t value);

/**
 * 向params里追加一个float参数。
 */
BOOL params_append_float(params_t* params, const char* key, float value);

/**
 * 向params里追加一个double参数。
 */
BOOL params_append_double(params_t* params, const char* key, double value);

/**
 * 从params里获取指定的字符串参数。
 */
const char* params_get(params_t* params, const char* key);

/**
 * 判断params里是否有指定的参数。
 */
BOOL params_has(params_t* params, const char* key);

/**
 * 从params里获取指定的int参数。
 */
BOOL params_get_int(params_t* params, const char* key, int32_t* value);

/**
 * 从params里获取指定的uint参数。
 */
BOOL params_get_uint(params_t *params, const char* key, uint64_t* value);

/**
 * 从params里获取指定的float参数。
 */
BOOL params_get_float(params_t* params, const char* key, float* value);

/**
 * 从params里获取指定的double参数。
 */
BOOL params_get_double(params_t* params, const char* key, double* value);

/**
 * 从params里获取指定的字符串参数。
 */
BOOL params_get_string(params_t* params, const char* key, const char** value);

BOOL params_dump(params_t* params);

END_C_DECLS

#endif/*DEVICE_PARAMS_H*/
