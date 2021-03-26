/*
 * webui.c
 *
 *  Created on: 2017È™ûÔøΩ12ÈèàÔøΩ14ÈèÉÔøΩ
 *      Author: chenminghao
 */

#include "apollo.h"
#include "aw_nvram.h"
#include "aw_timer.h"

#include "httpd/httpd.h"
#include "httpd/fs.h"
#include "httpd/http_str.h"
#include "httpd/cgifuncs.h"

#include "webui.h"
#include "utils/jsmn.h"

#define VDEBUG_INFO 5

#include "aw_time.h"
#include "aw_rtc.h"
#include "aw_led.h"
#include "aw_buzzer.h"
#include <stdio.h>
/******************************************************************************/

#define WEBUI_REBOOT_DELAY_MS       1000
#define WEBUI_SAVE_PARAM_CNT        20

#define JSON_ENDL                   "\\n"

/******************************************************************************/

#define CHECK_RET_AND_PLUS_VAR(ret, size, buf)  \
    if (-1 != (ret) && (ret) < (size)) {        \
        (buf) += (ret);                         \
        (size) -= (ret);                        \
    } else {                                    \
        (ret) = -1;                             \
        break;                                  \
    }

#define CONV_LAST_DOT_TO_CH(buf, ch) buf[-1] = ch

#define PREPARE_URL_DECODE_STR(str, len, t)     \
    jsmntok_t *g = t;                           \
    str = JSON_STRING + g->start;               \
    len = g->end - g->start;                    \
    len = httpsrv_raw_url_decode(str, len);     \
    str[len] = '\0';

/******************************************************************************/
static char *cgi_hdl_index (int32_t iIndex, int32_t iNumParams, char *pcParams[], char *pcValue[]);

/******************************************************************************/

typedef struct {
    uint16_t port;
    struct tcp_pcb *pcb;
} webui_t;

static http_auth g_auth[] = {
    {"/", "PASSWORD PLS", "admin", "123456", LWIP_HTTP_AUTH_SCHEME_BASIC},
    {0, 0, 0, 0, LWIP_HTTP_AUTH_SCHEME_NONE}
};

static const tCGI g_cgis[] = {
    {"/index.php", (tCGIHandler) cgi_hdl_index},
};

/******************************************************************************/

static webui_t g_webui;
static aw_timer_t g_reboot_tmr;

/******************************************************************************/

static void webui_reboot_tmr_cb (void *p_arg)
{
//    app_reboot("webui");
}

static void webui_delay_reboot (int ms)
{
    if (0 > ms) {
        aw_timer_stop(&g_reboot_tmr);
    } else if (0 == ms) {
//        aw_timer_start(&g_reboot_tmr, aw_ms_to_ticks(ms));
    } else {
        aw_timer_start(&g_reboot_tmr, aw_ms_to_ticks(ms));
    }
}

/******************************************************************************/

static void webui_init_internal (webui_t *p_httpd)
{
    aw_timer_init(&g_reboot_tmr, webui_reboot_tmr_cb, &g_reboot_tmr);

    p_httpd->pcb = httpd_init_addr(IP_ADDR_ANY, p_httpd->port);
    if (p_httpd->pcb) {
        http_set_cgi_handlers(g_cgis, AW_NELEMENTS(g_cgis));
        http_set_auth_table(g_auth);
    }
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

static char *cgi_hdl_index (int32_t iIndex, int32_t iNumParams, char *pcParams[], char *pcValue[])
{
    struct fs_file file;
    int32_t idx;
    char *buf;
    int size;
    int ret;

    fs_open(&file, CUSTOM_FILE_NAME_INDEX);
    buf = (char *) file.data;
    size = file.maxlen;

    idx = FindCGIParameter("led", pcParams, iNumParams);
    if (-1 != idx) {
        aw_led_toggle(0);
        snprintf(buf, size, "done");
    } else if (-1 != (idx = FindCGIParameter("buzzer", pcParams, iNumParams))) {
        aw_buzzer_loud_set(50);
        aw_buzzer_beep(500);
        snprintf(buf, size, "done");
    } else if (-1 != (idx = FindCGIParameter("rtcget", pcParams, iNumParams))) {
        aw_tm_t  tm;
        ret = aw_rtc_time_get(0, &tm);

        do {
            ret = snprintf(buf, size, "{\"year\":\"");
            CHECK_RET_AND_PLUS_VAR(ret, size, buf);
            ret = snprintf(buf, size, "%d", tm.tm_year+1900);
            CHECK_RET_AND_PLUS_VAR(ret, size, buf);

            ret = snprintf(buf, size, "\",\"mon\":\"");
            CHECK_RET_AND_PLUS_VAR(ret, size, buf);
            ret = snprintf(buf, size, "%d", tm.tm_mon+1);
            CHECK_RET_AND_PLUS_VAR(ret, size, buf);

            ret = snprintf(buf, size, "\",\"day\":\"");
            CHECK_RET_AND_PLUS_VAR(ret, size, buf);
            ret = snprintf(buf, size, "%d", tm.tm_mday);
            CHECK_RET_AND_PLUS_VAR(ret, size, buf);

            ret = snprintf(buf, size, "\",\"hour\":\"");
            CHECK_RET_AND_PLUS_VAR(ret, size, buf);
            ret = snprintf(buf, size, "%d", tm.tm_hour);
            CHECK_RET_AND_PLUS_VAR(ret, size, buf);

            ret = snprintf(buf, size, "\",\"min\":\"");
            CHECK_RET_AND_PLUS_VAR(ret, size, buf);
            ret = snprintf(buf, size, "%d", tm.tm_min);
            CHECK_RET_AND_PLUS_VAR(ret, size, buf);

            ret = snprintf(buf, size, "\",\"sec\":\"");
            CHECK_RET_AND_PLUS_VAR(ret, size, buf);
            ret = snprintf(buf, size, "%d", tm.tm_sec);
            CHECK_RET_AND_PLUS_VAR(ret, size, buf);

            ret = snprintf(buf, size, "\"}");
            CHECK_RET_AND_PLUS_VAR(ret, size, buf);
        } while(0);
    } else if (-1 != (idx = FindCGIParameter("rtcset", pcParams, iNumParams))) {
        do {
            aw_tm_t  tm;
            jsmn_parser json;
            jsmntok_t t[48];
            char* JSON_STRING = pcValue[idx];
            char* str;
            int i = 0;
            int len = 0;

            jsmn_init(&json);
            idx = jsmn_parse(&json, JSON_STRING, strlen(JSON_STRING), t, AW_NELEMENTS(t));
            if (idx < 0) {
                aw_kprintf("set error!\r\n");
            }
            for (i = 1; i < idx; i++) {
                if (jsoneq(JSON_STRING, &t[i], "year") == 0) {
                    PREPARE_URL_DECODE_STR(str, len, &t[i + 1]);
                    tm.tm_year = atoi(str)-1900;
                } else if (jsoneq(JSON_STRING, &t[i], "mon") == 0) {
                    PREPARE_URL_DECODE_STR(str, len, &t[i + 1]);
                    tm.tm_mon = atoi(str)-1;
                } else if (jsoneq(JSON_STRING, &t[i], "day") == 0) {
                    PREPARE_URL_DECODE_STR(str, len, &t[i + 1]);
                    tm.tm_mday = atoi(str);
                } else if (jsoneq(JSON_STRING, &t[i], "hour") == 0) {
                    PREPARE_URL_DECODE_STR(str, len, &t[i + 1]);
                    tm.tm_hour = atoi(str);
                } else if (jsoneq(JSON_STRING, &t[i], "min") == 0) {
                    PREPARE_URL_DECODE_STR(str, len, &t[i + 1]);
                    tm.tm_min = atoi(str);
                } else if (jsoneq(JSON_STRING, &t[i], "sec") == 0) {
                    PREPARE_URL_DECODE_STR(str, len, &t[i + 1]);
                    tm.tm_sec = atoi(str);
                }
            }

            /* …Ë÷√ ±º‰ */
            ret = aw_rtc_time_set(0, &tm);
            if (ret == AW_OK) {
                snprintf(buf, size, "done");
            }
        } while (0);
    } else {
        aw_kprintf("this function unrealized\r\n");
        snprintf((char*) file.data, file.maxlen, "this function unrealized");
    }

    fs_setlen_custom(&file, strlen(file.data));
    fs_close(&file);

    return CUSTOM_FILE_NAME_INDEX;
}

/******************************************************************************/

void webui_init (uint16_t port)
{
    g_webui.port = port;
    webui_init_internal(&g_webui);
}
