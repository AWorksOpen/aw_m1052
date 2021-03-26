/*
 * Copyright 2017, Cypress Semiconductor Corporation or a subsidiary of 
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

#ifndef __DNS_PROXY_H__
#define __DNS_PROXY_H__

#include "apollo.h"
#include "aw_task.h"

#include "lwip/sockets.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define DNS_CACHE_ENABLE         0      /* DNS缓存使能 */
#define DNS_CACHE_URL_SIZE_MAX   30     /* DNS缓存时URL最大长度 */


#define DNS_THREAD_PRIORITY                 (8)
#define DNS_STACK_SIZE                      (4096)

#define MAKE_IPV4_ADDRESS(a, b, c, d) \
         (((u32_t)((d) & 0xff) << 24) | \
          ((u32_t)((c) & 0xff) << 16) | \
          ((u32_t)((b) & 0xff) << 8)  | \
           (u32_t)((a) & 0xff))

#define DNS_QUERY_TABLE_ENTRY(s)            {s, sizeof(s)}

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct {
    const char       *pattern;
    const ip_addr_t   addr;
} dns_query_table_entry_t;

typedef struct {
    const char       *pattern;
    size_t            pattern_size;
    uint32_t (*func_redirect_ip)(void);
} dns_redirect_default_t;

typedef struct {
    dns_query_table_entry_t const *table;
    size_t                         table_size;
} dns_query_table_info_t;

typedef struct {
    int                              dns_socket;
    int                              proxy_socket;
    struct sockaddr_in               server;
    dns_query_table_entry_t const   *table;
    size_t                           table_size;
    AW_TASK_DECL(                    dns_thread, DNS_STACK_SIZE);
    volatile aw_bool_t               quit;

    dns_redirect_default_t           redirect_default;
    /*
     * 当没有匹配到url时，默认重定向的ip获  取函数指针。
     * 注意：当  DNS_CACHE_ENABLE > 0   时  该函数无效。
     */
    uint32_t                       (*func_redirect_ip_default)(void);
    /* 选择默认重定向的url数组指针(url可以  使用通配符.和*) */
    const char                     **url_redirect_array;
    /* 选择默认重定向的url数组元素个数 */
    size_t                           url_redirect_array_size;

    uint8_t                          rcv_buff[1024];
    size_t                           rcv_len;
} dns_proxy_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/**
 *  Start a DNS proxy server.
 *
 *  DNS queries from other devices will be replied with
 *  an answer that indicates this WICED device handles the requested domain.
 *
 *  The proxy can be set to reply to all DNS queries, or to a specific list of domains.
 *
 * @param[in] server  Structure workspace that will be used for this DNS proxy instance - allocated by caller.
 *
 * @return @ref aw_err_t
 */
aw_err_t dns_proxy_start (dns_proxy_t *server);


/**
 *  Stop a daemon which redirects DNS queries
 *
 * @param[in] server  Structure workspace that was previously used with @ref wiced_dns_proxy_start
 *
 * @return @ref aw_err_t
 */
aw_err_t dns_proxy_stop (dns_proxy_t *server);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __DNS_PROXY_H__ */
