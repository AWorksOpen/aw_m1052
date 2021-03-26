/*
 * http_str.h
 *
 *  Created on: 2017Äê12ÔÂ14ÈÕ
 *      Author: chenminghao
 */

#ifndef USER_CODE_HTTPD_HTTP_STR_H_
#define USER_CODE_HTTPD_HTTP_STR_H_

#include <stddef.h>
#include <stdint.h>

size_t httpsrv_raw_url_decode (char *p_encoded, size_t len);
size_t httpsrv_raw_url_encode (
    char const *p_decoded,
    size_t      buf_size,
    char const *p_encoded,
    size_t      len
);

#endif /* USER_CODE_HTTPD_HTTP_STR_H_ */
