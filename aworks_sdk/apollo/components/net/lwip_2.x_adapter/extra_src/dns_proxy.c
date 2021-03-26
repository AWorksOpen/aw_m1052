/**
 * @file
 * A DNS redirector which responds to all DNS queries with the local address of the server.
 */

#include <string.h>
#include <stdint.h>
#include "lwip/ext/dns_proxy.h"
#include "aw_mem.h"
#include "aw_vdebug.h"

#ifdef VDEBUG_INFO
#undef VDEBUG_INFO
#define VDEBUG_INFO                     0
#endif


/******************************************************
 *                      Macros
 ******************************************************/

#define PFX "DNS-PROXY"

#if defined(__VIH_DEBUG)
#include "aw_shell.h"
extern aw_shell_user_t *aw_default_shell_get();
extern int aw_shell_printf_with_color (aw_shell_user_t *p_user,
                                       const char *color,
                                       const char *func_name,
                                       const char *fmt, ...);
#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask)) {\
        aw_shell_printf_with_color(aw_default_shell_get(), \
                                   color, \
                                   __func__, \
                                   fmt, \
                                   ##__VA_ARGS__);\
    }
#define __LOG_BASE_ERR   __LOG_BASE
#else
#define __LOG_BASE(mask, color, fmt, ...)
#define __LOG_BASE_ERR(mask, color, fmt, ...)  \
    if (mask && 0) {AW_INFOF((fmt, ##__VA_ARGS__)); }
#endif

#define __LOG(mask, fmt, ...)  __LOG_BASE(mask, NULL, fmt, ##__VA_ARGS__)
#define __LOG_PFX(mask, fmt, ...) \
    __LOG_BASE(mask, PFX, "[%s:%4d] " fmt "\n", \
               PFX, __LINE__, ##__VA_ARGS__)

#define __LOG_RES(is_true, mask, fmt, ...) \
    if (is_true) {\
        __LOG_BASE(mask, NULL, "\x1B[38;5;40m[%s:%4d] [ OK ]%s " fmt  "\n", \
                    PFX, __LINE__, ##__VA_ARGS__);\
    } else {\
        __LOG_BASE_ERR(__MASK_ERR, \
                       AW_SHELL_FCOLOR_RED,   \
                       "[%s:%4d] " "[ ER ] " fmt "\n", \
                       PFX, __LINE__ , ##__VA_ARGS__);\
    }

#define __ASSERT_DETAIL(cond, fmt, ...) \
    if (!(cond)) {\
        __LOG_BASE_ERR(1, AW_SHELL_FCOLOR_RED,\
                   "[ ASSERT Failed ][%s:%d] " #cond "\n", \
                   __func__, __LINE__); \
        if (strlen(fmt) != 0) {\
            __LOG_BASE_ERR(1, NULL,"    [ DETAIL ] " fmt "\n", \
                       ##__VA_ARGS__); \
        } \
        while(1); \
    }

#define __GOTO_EXIT_DETAIL_IF(cond, ret_val, fmt, ...) \
    if ((cond)) {\
        __LOG_BASE_ERR(1, AW_SHELL_FCOLOR_RED, \
                       "[%s:%4d] " "[ ERROR:%d ] " #cond "\n", \
                       PFX, __LINE__, (int)(ret_val)); \
        ret = (ret_val); \
        if (strlen(fmt) != 0) {\
            __LOG_BASE_ERR(1, NULL,"    [ DETAIL ] " fmt "\n", ##__VA_ARGS__);\
        }\
        goto exit; \
    }

#define __ASSERT(cond)                __ASSERT_DETAIL(cond, "")
#define __GOTO_EXIT_IF(cond, ret_val) __GOTO_EXIT_DETAIL_IF(cond, ret_val, "")

#define __MASK_INF  1
#define __MASK_ERR  1


#define APP_LIB_PRINT_IPADDR_FMT  "%u.%u.%u.%u"

#define APP_LIB_PRINT_IPADDR_VAL(addr) \
    addr & 0x000000ff, \
   (addr & 0x0000ff00) >> 8, \
   (addr & 0x00ff0000) >> 16, \
    addr >> 24

/******************************************************
 *                    Constants
 ******************************************************/

#if 0
/* DNS redirect thread constants*/
#define DNS_THREAD_PRIORITY                 (WICED_DEFAULT_LIBRARY_PRIORITY)
#define DNS_STACK_SIZE                      (1024)
#endif

#define DNS_FLAG_QR_MASK                    (0x01 << 15)

/* DNS opcodes */
#define DNS_FLAG_OPCODE_MASK         (0x0F << 11)

/* Generic DNS flag constants */
#define DNS_FLAG_YES            (1)
#define DNS_FLAG_NO             (0)

/* DNS flags */
#define DNS_FLAG_QR_QUERY                 (0 << 15)
#define DNS_FLAG_QR_RESPONSE              (1 << 15)
#define DNS_FLAG_OPCODE_SQUERY            (0 << 11) /* RFC 1035 */
#define DNS_FLAG_OPCODE_IQUERY            (1 << 11) /* RFC 1035, 3425 */
#define DNS_FLAG_OPCODE_STATUS            (2 << 11) /* RFC 1035 */
#define DNS_FLAG_OPCODE_NOTIFY            (4 << 11) /* RFC 1996 */
#define DNS_FLAG_OPCODE_UPDATE            (5 << 11) /* RFC 2136 */
#define DNS_FLAG_AUTHORITIVE_RECORD       (1 << 10)
#define DNS_FLAG_TRUNCATED                (1 << 9)
#define DNS_FLAG_DESIRE_RECURSION         (1 << 8)
#define DNS_FLAG_RECURSION_AVAILABLE      (1 << 7)
#define DNS_FLAG_AUTHENTICATED            (1 << 5)
#define DNS_FLAG_ACCEPT_NON_AUTHENTICATED (1 << 4)
#define DNS_FLAG_REPLY_CODE_NO_ERROR      (0 << 0)

/* DNS socket constants*/
#define DNS_UDP_PORT                        (53)

/*
 * DNS socket timeout value in milliseconds.
 * Modify this to make thread exiting more responsive
 */
#define DNS_SOCKET_TIMEOUT                  (500)

#define DNS_MAX_DOMAIN_LEN                  (255)
#define DNS_QUESTION_TYPE_CLASS_SIZE        (4)
#define DNS_IPV4_ADDRESS_SIZE               (4)

#define DNS_MAX_PACKET_SIZE      \
    (sizeof(dns_header_t) + DNS_MAX_DOMAIN_LEN + \
            DNS_QUESTION_TYPE_CLASS_SIZE + sizeof(response_answer_array) + \
            DNS_IPV4_ADDRESS_SIZE)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

#pragma pack(1)

typedef struct {
    uint16_t transaction_id;
    uint16_t flags;
    uint16_t num_questions;
    uint16_t num_answer_records;
    uint16_t num_authority_records;
    uint16_t num_additional_records;
} dns_header_t;

typedef struct {
    uint8_t  name_offset_indicator;
    uint8_t  name_offset_value;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t request_length;
    /* Note: Actual IPv4 address needs to be inserted after this */
} dns_a_record_t;

#pragma pack()

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static void dns_thread_main (void *p_arg);
aw_local void dns_forward_task_init (void);

extern const ip_addr_t* dns_getserver(u8_t numdns);

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

aw_err_t dns_proxy_start (dns_proxy_t *server)
{
    int ret;

    dns_forward_task_init();

    /* Prepare network */
    server->server.sin_family = AF_INET;
    server->server.sin_port = htons(53);
    server->server.sin_addr.s_addr = 0;

    server->proxy_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > server->proxy_socket) {
        return AW_ERROR;
    }

#if 1
    int timeout = 2 * 1000;
    setsockopt(server->proxy_socket,
               SOL_SOCKET,
               SO_RCVTIMEO,
               (const char*)&timeout,
               sizeof(timeout));
    __LOG_PFX(__MASK_INF, "setsockopt(SO_RCVTIMEO, 2S)");
#endif

    ret = bind(server->proxy_socket,
               (struct sockaddr *)&server->server,
               sizeof(server->server));
    if (0 > ret) {
        closesocket(server->proxy_socket);
        return AW_ERROR;
    }

    server->quit = AW_FALSE;

    __LOG_PFX(1, "startup dns proxy server.");
    /* Create the DNS responder thread */
    AW_TASK_INIT(server->dns_thread,
                 "dns proxy",
                 DNS_THREAD_PRIORITY,
                 DNS_STACK_SIZE,
                 dns_thread_main,
                 (void *)server);

    AW_TASK_STARTUP(server->dns_thread);

    return AW_OK;
}

aw_err_t dns_proxy_stop (dns_proxy_t *server)
{
    server->quit = AW_TRUE;
    return AW_OK;
}

/**
 * \brief 带通配符的匹配 ,'*'代表任意0个或多个字符，'？'代表任意一个字符
 */
aw_bool_t match_with_wildcard_char(char* str1, const char* pattern)
{
    if (str1 == NULL) return AW_FALSE;
    if (pattern == NULL) return AW_FALSE;
    int len1 = strlen(str1);
    int len2 = strlen(pattern);
    int mark = 0;//用于分段标记,'*'分隔的字符串
    int p1 = 0, p2 = 0;
    char str_c,pattern_c;

    while (p1<len1 && p2<len2) {
        str_c = str1[p1];
        pattern_c = pattern[p2];

        if (str_c < 32) {
            str_c = '.';
        }

        if (pattern_c == '?') {
            p1++;
            p2++;
            continue;
        }
        if (pattern_c == '*') {
            /**
             * 如果当前是*号，则mark前面一部分已经获得匹配，
             * 从当前点开始继续下一个块的匹配
             */
            p2++;
            mark = p2;
            continue;
        }
        if (str_c != pattern_c) {
            if (p1 == 0 && p2 == 0) {
                /**
                 * 如果是首字符，特殊处理，不相同即匹配失败
                 */
                return AW_FALSE;
            }
            /**
             * pattern: ...*bdef*...
             *             ^
             *             mark
             *                ^
             *                p2
             *             ^
             *             new p2
             * str1:.....bdcf...
             *             ^
             *             p1
             *              ^
             *            new p1
             * 如上示意图所示，在比到e和c处不想等
             * p2返回到mark处，
             * p1需要返回到下一个位置。
             * 因为*前已经获得匹配，所以mark打标之前的不需要再比较
             */
            p1 -= p2 - mark - 1;
            p2 = mark;
            continue;
        }
        /**
         * 此处处理相等的情况
         */
        p1++;
        p2++;
    }
    if (p2 == len2) {
        if (p1 == len1) {
            /**
             * 两个字符串都结束了，说明模式匹配成功
             */
            return AW_TRUE;
        }
        if (pattern[p2 - 1] == '*') {
            /**
             * str1还没有结束，但pattern的最后一个字符是*，所以匹配成功
             *
             */
            return AW_TRUE;
        }
    }
    while (p2<len2) {
        /**
         * pattern多出的字符只要有一个不是*,匹配失败
         *
         */
        if (pattern[p2] != '*')
            return AW_FALSE;
        p2++;
    }
    return AW_TRUE;
}

aw_local aw_err_t __dns_forward (dns_proxy_t    *server,
                                 uint8_t        *request,
                                 size_t          request_len)
{
    struct sockaddr_in   dns_server;
    struct sockaddr_in   rcv_sockaddr;
    socklen_t            rcv_sockaddr_len = 0;
    const ip_addr_t     *p_dns_ip;
    aw_err_t             ret;
    struct timeval       timeout;

    __ASSERT(server != NULL && request != NULL && request_len > 0);

    (void)p_dns_ip;

    /* Prepare network */
    p_dns_ip                   = dns_getserver(0);
    dns_server.sin_family      = AF_INET;
    dns_server.sin_port        = htons(53);
    dns_server.sin_addr.s_addr = p_dns_ip->u_addr.ip4.addr;

    if (p_dns_ip->u_addr.ip4.addr == 0) {
        __LOG_RES(AW_FALSE, 1, "not found dns server!");
        return AW_ERROR;
    }

    server->dns_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > server->dns_socket) {
        return AW_ERROR;
    }

    timeout.tv_usec = 0;
    timeout.tv_sec  = 3;
    setsockopt(server->dns_socket,
               SOL_SOCKET,
               SO_RCVTIMEO,
               (void *)&timeout,
               sizeof(timeout));

    ret = sendto(server->dns_socket,
                 request,
                 request_len,
                 0,
                 (struct sockaddr * )&dns_server,
                 sizeof(dns_server));
    if (ret < 0) {
        ret = -AW_EPERM;
        goto exit;
    }

    rcv_sockaddr_len = sizeof(struct sockaddr);
    ret = recvfrom(server->dns_socket,
                               server->rcv_buff,
                               sizeof(server->rcv_buff),
                               0,
                               (struct sockaddr * )&rcv_sockaddr,
                               &rcv_sockaddr_len);
    __GOTO_EXIT_IF(ret <= 0, -AW_ETIMEDOUT);

    server->rcv_len = ret;
    __LOG_PFX(__MASK_INF, "DNS server: " APP_LIB_PRINT_IPADDR_FMT ":%d(%d)",
                 APP_LIB_PRINT_IPADDR_VAL(rcv_sockaddr.sin_addr.s_addr),
                 rcv_sockaddr.sin_port,
                 server->rcv_len);
exit:
    closesocket(server->dns_socket);
    return ret;
}

/******************************************************************************/
typedef struct {
    struct sockaddr_in      client;
    dns_proxy_t            *dns_proxy_server;
    uint8_t                 request[128];
    int                     request_length;
    aw_bool_t               flag_idle;
    AW_SEMB_DECL(           req_semb);
} __dns_forward_req_t;

static __dns_forward_req_t __g_dns_req = { .flag_idle = AW_TRUE };
/******************************************************************************/


aw_local void dns_forward_task (void *p_arg)
{
    dns_proxy_t         *server;

    dns_header_t        *dns_header_ptr;
    uint16_t             transaction_id;
    uint16_t             num_questions;
    uint16_t             flags;
    uint8_t             *request;
    int                  request_length;
    struct sockaddr_in   client;

    AW_FOREVER {
        AW_SEMB_TAKE(__g_dns_req.req_semb, AW_SEM_WAIT_FOREVER);
        aw_assert(!__g_dns_req.flag_idle);

        server         = __g_dns_req.dns_proxy_server;
        client         = __g_dns_req.client;
        request        = __g_dns_req.request;
        request_length = __g_dns_req.request_length;

        dns_header_ptr = (dns_header_t *) request;
        __LOG_PFX(__MASK_INF, "DNS client: " APP_LIB_PRINT_IPADDR_FMT ":%d(%d)",
                     APP_LIB_PRINT_IPADDR_VAL(client.sin_addr.s_addr),
                     htons(client.sin_port),
                     request_length);

        transaction_id = dns_header_ptr->transaction_id;

        /* Swap endian-ness of header */
        flags         = ntohs(dns_header_ptr->flags);
        num_questions = ntohs(dns_header_ptr->num_questions);

#if 1
        /* Only respond to valid queries */
        if (((flags & DNS_FLAG_QR_MASK) != DNS_FLAG_QR_QUERY) ||
                ((flags & DNS_FLAG_OPCODE_MASK) != DNS_FLAG_OPCODE_SQUERY) ||
                (num_questions < 1)) {
            __LOG_RES(AW_FALSE, __MASK_INF, "DNS : invalid query");
            goto again;
        }
#endif

        if (__dns_forward(server, request, request_length) < 0) {
            goto again;
        }

        dns_header_ptr = (dns_header_t *) server->rcv_buff;
        if (dns_header_ptr->transaction_id != transaction_id) {
            __LOG_RES(AW_FALSE, __MASK_INF, "transaction_id is't equal.");
            goto again;
        }

        __ASSERT(server->rcv_len > 0 && server->rcv_len < 0xFFFF);
        __ASSERT(dns_header_ptr);

        /* Send packet */
        sendto(server->proxy_socket,
               dns_header_ptr,
               server->rcv_len,
               0,
               (struct sockaddr * )&client,
               sizeof(struct sockaddr));

again:
        /* 清除标志，等待接收下一个请求 */
        __g_dns_req.flag_idle = AW_TRUE;
    }
}

aw_local void dns_forward_task_init (void)
{
    aw_err_t            ret;
    static aw_bool_t    flag_inited = AW_FALSE;
    AW_TASK_DECL_STATIC(dns_forward, DNS_STACK_SIZE);

    if (flag_inited) {
        return;
    }

    AW_SEMB_INIT(__g_dns_req.req_semb, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    AW_TASK_INIT(dns_forward,
                 "dns forward",
                 DNS_THREAD_PRIORITY,
                 DNS_STACK_SIZE,
                 dns_forward_task,
                 NULL);

    ret = AW_TASK_STARTUP(dns_forward);
    aw_assert(ret == AW_OK);
    aw_assert(__g_dns_req.flag_idle == AW_TRUE);

    flag_inited = AW_TRUE;
}

/******************************************************************************/

#if 1
aw_local void dns_thread_main (void *p_arg)
{
    dns_proxy_t         *server = (dns_proxy_t *)p_arg;
    uint8_t              request[128];
    int                  request_length;

    /* Loop endlessly */
    while (server->quit == AW_FALSE) {
        struct sockaddr_in  client;

        /* Sleep until data is received from socket. */
        socklen_t len = sizeof(struct sockaddr);

        request_length = recvfrom(server->proxy_socket,
                                  request,
                                  sizeof(request),
                                  0,
                                  (struct sockaddr * )&client,
                                  &len);
        if (1 > request_length) {
            continue;
        }

        if (!__g_dns_req.flag_idle) {
            __LOG_PFX(1, "task is busy , drop this packet!");
            continue;
        }

        memcpy(__g_dns_req.request, request, request_length);
        __g_dns_req.request_length   = request_length;
        __g_dns_req.client           = client;
        __g_dns_req.dns_proxy_server = server;

        /* 最后设置 flag_idle 由 dns_forward_task 中清除  */
        __g_dns_req.flag_idle      = AW_FALSE;

        /* 信号量唤醒 forward 线程 */
        AW_SEMB_GIVE(__g_dns_req.req_semb);
    }

    /* Delete DNS socket */
    closesocket(server->proxy_socket);
    aw_task_terminate(aw_task_id_self());
}

#elif 1
aw_local void dns_thread_main (void *p_arg)
{
    dns_proxy_t         *server = (dns_proxy_t *)p_arg;
    dns_header_t        *dns_header_ptr;
    uint8_t              request[128];
    uint16_t             transaction_id;
    int                  request_length;
    uint16_t             num_questions;
    uint16_t             flags;


    /* Loop endlessly */
    while (server->quit == AW_FALSE) {
        struct sockaddr_in  client;

        /* Sleep until data is received from socket. */
        socklen_t len = sizeof(struct sockaddr);

        request_length = recvfrom(server->proxy_socket,
                                  request,
                                  sizeof(request),
                                  0,
                                  (struct sockaddr * )&client,
                                  &len);
        if (1 > request_length) {
            continue;
        }
        dns_header_ptr = (dns_header_t *) request;
        __LOG_PFX(__MASK_INF, "DNS client: " APP_LIB_PRINT_IPADDR_FMT ":%d(%d)",
                     APP_LIB_PRINT_IPADDR_VAL(client.sin_addr.s_addr),
                     htons(client.sin_port),
                     request_length);

        transaction_id = dns_header_ptr->transaction_id;

        /* Swap endian-ness of header */
        flags         = ntohs(dns_header_ptr->flags);
        num_questions = ntohs(dns_header_ptr->num_questions);

#if 1
        /* Only respond to valid queries */
        if (((flags & DNS_FLAG_QR_MASK) != DNS_FLAG_QR_QUERY) ||
                ((flags & DNS_FLAG_OPCODE_MASK) != DNS_FLAG_OPCODE_SQUERY) ||
                (num_questions < 1)) {
            __LOG_RES(AW_FALSE, __MASK_INF, "DNS : invalid query");
            continue;
        }
#endif

        if (__dns_forward(server, request, request_length) < 0) {
           continue;
        }

        dns_header_ptr = (dns_header_t *) server->rcv_buff;
        if (dns_header_ptr->transaction_id != transaction_id) {
            __LOG_RES(AW_FALSE, __MASK_INF, "transaction_id is't equal.");
            continue;
        }

        __ASSERT(server->rcv_len > 0 && server->rcv_len < 0xFFFF);
        __ASSERT(dns_header_ptr);

        /* Send packet */
        sendto(server->proxy_socket,
               dns_header_ptr,
               server->rcv_len,
               0,
               (struct sockaddr * )&client,
               sizeof(struct sockaddr));
    }

    /* Delete DNS socket */
    closesocket(server->proxy_socket);
    aw_task_terminate(aw_task_id_self());
}

#else
/* tag:thread */

/**
 *  Implements a very simple DNS redirection server
 */
aw_local void dns_thread_main(void *p_arg)
{
    dns_proxy_t *server = (dns_proxy_t *)p_arg;
    dns_header_t     *dns_header_ptr;
    uint8_t           request[128];
    uint8_t           respond[128];

    /* Loop endlessly */
    while (server->quit == AW_FALSE) {
        struct sockaddr_in  client;
        dns_a_record_t     *a_record;
        uint8_t            *loc_ptr;
        size_t              valid_query = 0;

        ip_addr_t addr;

        /* Sleep until data is received from socket. */
        socklen_t len = sizeof(struct sockaddr);
        int request_length = recvfrom(server->proxy_socket,
                                      request, sizeof(request),
                                      0,
                                      (struct sockaddr *)&client, &len);
        if (1 > request_length) {
            continue;
        }
        dns_header_ptr = (dns_header_t *) request;
        __LOG_PFX(__MASK_INF, "DNS client: " APP_LIB_PRINT_IPADDR_FMT ":%d(%d)",
                     APP_LIB_PRINT_IPADDR_VAL(client.sin_addr.s_addr),
                     client.sin_port,
                     request_length);

        /* Swap endian-ness of header */
        dns_header_ptr->flags         = ntohs(dns_header_ptr->flags);
        dns_header_ptr->num_questions = ntohs(dns_header_ptr->num_questions);

        /* Only respond to valid queries */
        if (((dns_header_ptr->flags & DNS_FLAG_QR_MASK) != DNS_FLAG_QR_QUERY) ||
                ((dns_header_ptr->flags & DNS_FLAG_OPCODE_MASK) !=
                        DNS_FLAG_OPCODE_SQUERY) ||
                (dns_header_ptr->num_questions < 1)) {
            __LOG_PFX(__MASK_INF, "DNS : invalid query");
            continue;
        }

        /* Create our reply using the same packet */
        /* Set the header values */
        dns_header_ptr->flags = DNS_FLAG_QR_RESPONSE        |
                                DNS_FLAG_OPCODE_SQUERY      |
                                DNS_FLAG_AUTHORITIVE_RECORD |
                                DNS_FLAG_AUTHENTICATED      |
                                DNS_FLAG_REPLY_CODE_NO_ERROR;

        dns_header_ptr->num_questions         = 1;
        dns_header_ptr->num_answer_records    = 1;
        dns_header_ptr->num_authority_records = 0;

        /* Find end of question */
        loc_ptr = (uint8_t *) &dns_header_ptr[1];

#ifdef DNS_REDIRECT_ALL_QUERIES
        /*
         * Skip over domain name - made up of sections with one
         * byte leading size values
         */
        while ( ( *loc_ptr != 0 ) &&
                ( *loc_ptr <= 64 ) )
        {
            loc_ptr += *loc_ptr + 1;
        }
        loc_ptr++; /* skip terminating null */
#else
        {
#if 0
            static char cache_url[DNS_CACHE_URL_SIZE_MAX];
            int i = 0;
            int size = strlen((const char *)loc_ptr);

            if (size >= DNS_CACHE_URL_SIZE_MAX) {
                __LOG_PFX(__MASK_INF, "DNS CACHE: cache not enough");
                continue;
            }

            memset(cache_url, 0, sizeof(cache_url));
            memcpy(cache_url, loc_ptr + 1, size);

            while (cache_url[i] != 0) {
                if (cache_url[i] < 32) {
                    cache_url[i] = '.';
                }
                i++;
            }

            aw_kprintf("DNS hostname: %s\n", cache_url);
#endif
            for (valid_query = 0; valid_query < server->table_size; valid_query++) {
                if (match_with_wildcard_char((char *)loc_ptr + 1,
                                             server->table[valid_query].pattern)) {
                    loc_ptr += strlen((const char *)loc_ptr) + 1;
                    /* Move pointer to end of question */
                    loc_ptr += DNS_QUESTION_TYPE_CLASS_SIZE;
                    __LOG_PFX(__MASK_INF, "DNS query: %d", valid_query);
                    break;
                }
            }


            if (valid_query >= server->table_size) {
#if DNS_CACHE_ENABLE
                static char cache_url[DNS_CACHE_URL_SIZE_MAX];
                int i = 0;
                int size = strlen((const char *)loc_ptr);

                if (size >= DNS_CACHE_URL_SIZE_MAX) {
                    __LOG_PFX(__MASK_INF, "DNS CACHE: cache not enough");
                    continue;
                }

                memset(cache_url, 0, sizeof(cache_url));
                memcpy(cache_url, loc_ptr + 1, size);

                while (cache_url[i] != 0) {
                    if (cache_url[i] < 32) {
                        cache_url[i] = '.';
                    }
                    i++;
                }

                __LOG_PFX(__MASK_INF, "DNS CACHE: get hostname by \"%s\"", cache_url);
                if (dns_gethostbyname(cache_url, &addr, NULL, NULL) == 0) {
                    loc_ptr += strlen((const char *)loc_ptr) + 1;
                    /* Move pointer to end of question */
                    loc_ptr += DNS_QUESTION_TYPE_CLASS_SIZE;

                } else {
                    __LOG_PFX(__MASK_INF, "DNS CACHE: get hostname fail");
                    continue;
                }
#else /* DNS_CACHE_ENABLE */
                if (server->func_redirect_ip_default) {
                    int i = 0;
                    for (i = 0; i < server->url_redirect_array_size; i++) {
                        if (match_with_wildcard_char((char *)loc_ptr + 1,
                                                     server->url_redirect_array[i])) {
                            loc_ptr += strlen((const char *)loc_ptr) + 1;
                            /* Move pointer to end of question */
                            loc_ptr += DNS_QUESTION_TYPE_CLASS_SIZE;

                            addr.addr = server->func_redirect_ip_default();
                            __LOG_PFX(__MASK_INF, "DNS : default redirect");
                            break;
                        }
                    }
                    if (i >= server->url_redirect_array_size) {
                        __LOG_PFX(__MASK_INF, "DNS : unavaliable");
                        continue;
                    }

                } else {
                    __LOG_PFX(__MASK_INF, "DNS : unavaliable");
                    continue;
                }

#endif /* DNS_CACHE_ENABLE */
            }
        }
#endif

        /*
         * Check that domain name was not too large
         * for packet - probably from an attack
         */
        if ((loc_ptr - (uint8_t *)dns_header_ptr) > request_length) {
            __LOG_PFX(__MASK_INF, "DNS : domain name was too large");
            continue;
        }

        aw_assert(sizeof(respond) >= loc_ptr - (uint8_t *)dns_header_ptr);
        memcpy(respond, dns_header_ptr, (loc_ptr - (uint8_t *)dns_header_ptr));
        loc_ptr = respond + (loc_ptr - (uint8_t *)dns_header_ptr);
        dns_header_ptr = (dns_header_t *)respond;

        /* Query for this devices domain - reply with only local IP */
        dns_header_ptr->num_additional_records = 0;

        /* Copy the A record in */
        a_record = (dns_a_record_t *)loc_ptr;
        memcpy(loc_ptr, &a_record_template, sizeof(a_record_template));
        loc_ptr += sizeof(a_record_template);
        a_record->name_offset_value = 0x0C;

        /* Add our IP address */
        aw_assert((uint8_t *)loc_ptr - (uint8_t *)dns_header_ptr + sizeof(uint32_t) < sizeof(respond));

        if (valid_query >= server->table_size) {
            memcpy(loc_ptr, &addr.addr, sizeof(addr.addr));
            loc_ptr += sizeof(addr.addr);
        } else {
            memcpy(loc_ptr, &server->table[valid_query].addr.addr, sizeof(server->table[valid_query].addr.addr));
            loc_ptr += sizeof(server->table[valid_query].addr.addr);
        }


        /* Swap the endian-ness of the header variables */
        dns_header_ptr->flags                  = htons(dns_header_ptr->flags);
        dns_header_ptr->num_questions          = htons(dns_header_ptr->num_questions);
        dns_header_ptr->num_answer_records     = htons(dns_header_ptr->num_answer_records);
        dns_header_ptr->num_authority_records  = htons(dns_header_ptr->num_authority_records);
        dns_header_ptr->num_additional_records = htons(dns_header_ptr->num_additional_records);

        /* Send packet */
        sendto(server->proxy_socket,
               respond,
               loc_ptr - respond,
               0,
               (struct sockaddr *)&client,
               sizeof(struct sockaddr));
        __LOG_PFX(__MASK_INF, "DNS respond(%d): " APP_LIB_PRINT_IPADDR_FMT,
                     loc_ptr - respond,
                     APP_LIB_PRINT_IPADDR_VAL(server->table[valid_query].addr.addr));
    }

    /* Delete DNS socket */
    closesocket(server->proxy_socket);
    aw_task_terminate(aw_task_id_self());
}
#endif


aw_local dns_proxy_t dns_server;

void dns_proxy_init (void)
{
    static char s_inited = AW_FALSE;
    if (!s_inited) {
        s_inited = AW_TRUE;
        dns_proxy_start(&dns_server);
    }
}
