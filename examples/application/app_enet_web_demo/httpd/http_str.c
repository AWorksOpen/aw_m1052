/*
 * http_str.c
 *
 *  Created on: 2017Äê12ÔÂ14ÈÕ
 *      Author: chenminghao
 */

#include "httpd/http_str.h"

static int __httpsrv_htoi (char *s)
{
    int value;
    int c;

    c = ((unsigned char *) s)[0];
    if (isupper(c)) {
        c = tolower(c);
    }
    value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

    c = ((unsigned char *) s)[1];
    if (isupper(c)) {
        c = tolower(c);
    }
    value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

    return (value);
}

size_t httpsrv_raw_url_decode (char *p_encoded, size_t len)
{
    char *dest = p_encoded;
    char *data = p_encoded;

    while (len--) {
        if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) && isxdigit((int) *(data + 2))) {
            *dest = (char) __httpsrv_htoi(data + 1);
            data += 2;
            len -= 2;
        } else {
            *dest = *data;
        }
        data++;
        dest++;
    }
    *dest = '\0';
    return dest - p_encoded;
}

/* rfc1738:

     ...The characters ";",
     "/", "?", ":", "@", "=" and "&" are the characters which may be
     reserved for special meaning within a scheme...

     ...Thus, only alphanumerics, the special characters "$-_.+!*'(),", and
     reserved characters used for their reserved purposes may be used
     unencoded within a URL...

     For added safety, we only leave -_. unencoded.
 */

static const unsigned char hexchars[] = "0123456789ABCDEF";

size_t httpsrv_raw_url_encode (
    char const *p_decoded,
    size_t      buf_size,
    char const *p_encoded,
    size_t      len
)
{
    register size_t x, y;
    char *str;

    str = (char *) p_encoded;
    for (x = 0, y = 0; buf_size >= y && len--; x++, y++) {
        str[y] = (unsigned char) p_decoded[x];
#ifndef CHARSET_EBCDIC
        if ((str[y] < '0' && str[y] != '-' && str[y] != '.') || (str[y] < 'A' && str[y] > '9')
                || (str[y] > 'Z' && str[y] < 'a' && str[y] != '_') || (str[y] > 'z' && str[y] != '~')) {
            str[y++] = '%';
            str[y++] = hexchars[(unsigned char) p_decoded[x] >> 4];
            str[y] = hexchars[(unsigned char) p_decoded[x] & 15];
#else /*CHARSET_EBCDIC*/
            if (!isalnum(str[y]) && strchr("_-.~", str[y]) != NULL) {
                str[y++] = '%';
                str[y++] = hexchars[os_toascii[(unsigned char) p_decoded[x]] >> 4];
                str[y] = hexchars[os_toascii[(unsigned char) p_decoded[x]] & 15];
#endif /*CHARSET_EBCDIC*/
        }
    }
    str[y] = '\0';

    return y;
}
