/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief httpsrv_cgi_helper.c
 *
 * \internal
 * \par History
 * - 17-10-18, chenminghao, First implementation.
 * \endinternal
 */


#include <ctype.h>
#include <string.h>
#include "lwip/sockets.h"
#include "httpsrv_prv.h"

#include "httpsrv_cgi_helper.h"

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

static char *__strnstr(const char *buffer, const char *token, size_t n)
{
    const char *p;
    int tokenlen = (int)strlen(token);
    if (tokenlen == 0)
    {
        return (char *)buffer;
    }
    for (p = buffer; p + tokenlen <= buffer + n; p++)
    {
        if ((*p == *token) && (strncmp(p, token, tokenlen) == 0))
        {
            return (char *)p;
        }
    }
    return NULL;
}

size_t httpsrv_raw_url_decode (char *p_encoded, size_t len)
{
    char *dest = p_encoded;
    char *data = p_encoded;

    while (len--) {
        if (*data == '%' && 
            len >= 2     && 
            isxdigit((int) *(data + 1)) && isxdigit((int) *(data + 2))) {
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

size_t httpsrv_raw_url_encode (char const *p_decoded, 
                               char const *p_encoded, 
                               size_t      len)
{
    register size_t x, y;
    char *str;

    str = (char *) p_encoded;
    for (x = 0, y = 0; len--; x++, y++) {
        str[y] = (unsigned char) p_decoded[x];
#ifndef CHARSET_EBCDIC
        if ((str[y] < '0' && str[y] != '-' && str[y] != '.') || 
            (str[y] < 'A' && str[y] > '9')
                || (str[y] > 'Z' && str[y] < 'a' && str[y] != '_') 
                || (str[y] > 'z' && str[y] != '~')) {
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

/**
 * Extract URI parameters from the parameter-part of an URI in the form
 * "test.cgi?x=y" @todo: better explanation!
 * Pointers to the parameters are stored in hs->param_vals.
 *
 * @p_param_names Save the name of the parameter
 * @p_param_vals Save the parameter value string
 * @p_param params pointer to the NULL-terminated parameter string from the URI
 * @max_param_cnt Parse up to max_param_cnt from the passed string and ignore 
 *                the remainder (if any)
 * @return number of parameters extracted
 */
size_t httpsrv_extract_uri_parameters (char *p_param_names[],
                                       char *p_param_vals[],
                                       char *p_param,
                                       int   max_param_cnt)
{
    char *pair;
    char *equals;
    size_t loop;

    /* If we have no parameters at all, return immediately. */
    if (NULL == p_param || (p_param[0] == '\0')) {
        return (0);
    }

    /* Get a pointer to our first parameter */
    pair = p_param;

    /* Parse up to max_param_cnt from the passed string and ignore the
     * remainder (if any) */
    for (loop = 0; (loop < max_param_cnt) && pair; loop++) {

        /* Save the name of the parameter */
        p_param_names[loop] = pair;

        /* Remember the start of this name=value pair */
        equals = pair;

        /* Find the start of the next name=value pair and replace the delimiter
         * with a 0 to terminate the previous pair string. */
        pair = strchr(pair, '&');
        if (pair) {
            *pair = '\0';
            pair++;
        } else {
            /* We didn't find a new parameter so find the end of the URI and
             * replace the space with a '\0' */
            pair = strchr(equals, ' ');
            if (pair) {
                *pair = '\0';
            }

            /* Revert to NULL so that we exit the loop as expected. */
            pair = NULL;
        }

        /* Now find the '=' in the previous pair, replace it with '\0' and save
         * the parameter value string. */
        equals = strchr(equals, '=');
        if (equals) {
            *equals = '\0';
            p_param_vals[loop] = equals + 1;
        } else {
            p_param_vals[loop] = NULL;
        }
    }

    return loop;
}

/******************************************************************************
*
* Searches the list of parameters passed to a CGI handler and returns the
* index of a given parameter within that list.
*
* \param pcToFind is a pointer to a string containing the name of the
* parameter that is to be found.
* \param pcParam is an array of character pointers, each containing the name
* of a single parameter as encoded in the URI requesting the CGI.
* \param iNumParams is the number of elements in the pcParam array.
*
* This function searches an array of parameters to find the string passed in
* \e p_target.  If the string is found, the index of that string within the
* \e p_param array is returned, otherwise -1 is returned.
*
* \return Returns the index of string \e pcToFind within array \e pcParam
* or -1 if the string does not exist in the array.
*
*******************************************************************************/
int httpsrv_find_cgi_parameter (const char *p_target, char *p_param[], int max_param_cnt)
{
    int i;

    /*Scan through all the parameters in the array.*/
    for (i = 0; i < max_param_cnt; i++) {
        /* Does the parameter name match the provided string?*/
        if (strcmp(p_target, p_param[i]) == 0) {
           /* We found a match - return the index.*/
            return i;
        }
    }

    /* If we drop out, the parameter was not found. */
    return -1;
}




/**
 * \brief
 *
 * \details
 *
 * \return  0 没有检测到“分割线”
 *          1 检测到“分割线”
 *
 */
static int __cgi_file_boundary_end_find(uint32_t            ses_handle,
                                        httpsrv_cgi_file_t *p_file)
{
    static char file_buf[HTTPSRV_CGI_FILE_BUFFER_SIZE];
    static int  area_size = HTTPSRV_CGI_FILE_BUFFER_SIZE / 2;
    static int  tmp_size = 0;
    static int  state = 0;

    switch (state) {
    case 0:          /* file_buf 中没有任何数据不需要进行任何操作 */
        tmp_size = 0;
        state = 2;
        return 0;
    case 1:          /* file_buf 中有数据需要直接外发 */
        if (tmp_size) {
            memcpy(p_file->data, file_buf, tmp_size);
            p_file->data_length = tmp_size;
        }
        state = 2;
        return 0;
    case 2:     /* file_buf 中没有数据，需要校验传进来的数据是否含有“分割线” */
    {
        int size = min(p_file->data_length, area_size);
        int read_lenth = 0;
        char *str = file_buf;

        memcpy(str, p_file->data + p_file->data_length - size, size);
        str += size;
        read_lenth = HTTPSRV_cgi_read(ses_handle, str, area_size * 2 - size);
        str = __strnstr(file_buf, p_file->boundary, area_size * 2);
        if (str) {  /* 已经查询到“分割线” */
            int boundary_index = str - file_buf - 4;
            if (boundary_index >= size) { /* 分割线属于后半区域 */
                int i = 0;
                for (i = 0;i < read_lenth; i++) {
                    file_buf[i] = file_buf[size + i];
                }
                tmp_size = read_lenth;
                state = 1;
                return 0;
            } else {   /* 分割线属于前半区域 */
                p_file->data_length -=  (size - boundary_index);
                state = 0;
                return 1;
            }
        } else {   /* 没有检测到分割线 */
            int i = 0;
            for (i = 0;i < read_lenth; i++) {
                file_buf[i] = file_buf[size + i];
            }
            tmp_size = read_lenth;
            state = 1;
            return 0;
        }

    }
        break;
    }
    return -1;
}


int httpsev_cgi_file_data_init(uint32_t ses_handle, httpsrv_cgi_file_t *p_file)
{
    HTTPSRV_SESSION_STRUCT *session = (HTTPSRV_SESSION_STRUCT *)ses_handle;
    p_file->state = 0;
    int recv_time = 500;

    /* 必须设置接收超时，以避免一直阻塞 */
    lwip_setsockopt(session->sock,
                    SOL_SOCKET, 
                    SO_RCVTIMEO, 
                    (const void *)&recv_time, 
                    sizeof(int));

    return 0;
}


int httpsev_cgi_file_data_get(uint32_t            ses_handle, 
                              httpsrv_cgi_file_t *p_file, 
                              size_t              len)
{
    size_t data_length = 0;           /* 未处理的数据大小 */
    size_t length = len;              /* 期望接收的数据大小 */
    char *data_str = p_file->data;
    int ret = 1;
    int count = 0;

    p_file->data_length = 0;

    __cgi_file_boundary_end_find(ses_handle, p_file);
    data_str += p_file->data_length;
    length   -= p_file->data_length;

    while (1) {
        count++;
        if (count > 10) {
            ret = -1;
            break;
        }
        data_length = HTTPSRV_cgi_read(ses_handle, data_str, length);
        length = 0;
        data_str = p_file->data;
        if (p_file->data_length) {
            data_length += p_file->data_length;
        }

        if (p_file->state == 3) {
            p_file->data_length = data_length;


            if (__cgi_file_boundary_end_find(ses_handle, p_file)) {
                ret = 0;
            } else {
                ret = 1;
            }

            break;
        }

        if (p_file->state == 0) {
            if (strncmp(data_str + 2, p_file->boundary, strlen(p_file->boundary)) == 0) {
                if (data_length > (strlen(p_file->boundary) + 2 + 2)) { 
                    /* 分割线 + "--" + "\r\n" */
                    data_length -= (strlen(p_file->boundary) + 2 + 2);
                } else {
                    goto read_again;
                }

                length += strlen(p_file->boundary) + 2 + 2;
                data_str += strlen(p_file->boundary) + 2 + 2;
                p_file->state = 1; /* 已经验证分割线 */

            }
        }
        if (p_file->state == 1) {
            char *str = NULL;

            str = __strnstr(data_str, "Content-Type:", data_length);
            if (str == NULL) {
                goto read_again;
            }
            /* 处理表单信息 */
            str = __strnstr(data_str, "form-data;", data_length);
            if (str) {
                if (!__strnstr(data_str, "\r\n", data_length)) {
                    goto read_again;
                }
                
                /* "*" + "form-data;" + " " */
                data_length -= ((str - data_str) + 10 + 1); 

                length += ((str - data_str) + 10 + 1);
                data_str = str + 10 + 1;

                while (strncmp(data_str, "Content-Type:", 13) != 0) {
                    uint8_t flag = 1;

                    if (strncmp(data_str, "name=", 5) == 0) {
                        int i = 0;
                        data_str += 5 + 1;
                        while (data_str[i] != '"' && i < data_length) {

                            if (p_file->name) {
                                p_file->name[i] = data_str[i];
                            }

                            i++;
                        }

                        data_length -= (5 + 1 + i + 3);
                        
                        /* "name=" + "\"" + name + "\" " */
                        length += 5 + 1 + i + 3;  
                        data_str += i + 3;

                        flag = 0;
                    }
                    if (strncmp(data_str, "filename=", 9) == 0) {
                        int i = 0;
                        data_str += 9 + 1;
                        while (data_str[i] != '"' && i < data_length) {

                            if (p_file->filename) {
                                p_file->filename[i] = data_str[i];
                            }

                            i++;
                        }

                        data_length -= (9 + 1 + i + 3);
                        
                        /* "name=" + "\"" + name + "\" " */
                        length += 9 + 1 + i + 3;  
                        data_str += i + 3;

                        flag = 0;
                    }

                    if (flag) {  /* 避免错误的数据死循环  */
                        data_str++;
                        length++;
                        data_length--;
                        if (data_length == 0) {
                            ret = -1;
                            break;
                        }
                    }
                }
                p_file->state = 2;
            }
        }

        if (p_file->state == 2) {
            char *str = NULL;

            str = __strnstr(data_str, "\r\n\r\n", data_length);
            if (!str) {
                goto read_again;
            }

            data_length -= ((str - data_str) + 4); /* "*" + "\r\n" */
            length += ((str - data_str) + 4);
            data_str = str + 4;
            p_file->state = 3;
        }

read_again:
        {
            size_t i = 0;

            /* 将剩余有效的数据往前移 */
            while (i < data_length) {
                p_file->data[i] = data_str[i];
                i++;
            }
            data_str = p_file->data + data_length;
            p_file->data_length = data_length;
        }
    }
    return ret;
}

/* end of file */