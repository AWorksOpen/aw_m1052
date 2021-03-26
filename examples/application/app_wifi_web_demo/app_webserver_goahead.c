
#include "aworks.h"
#include "awbus_lite.h"
#include "aw_adc.h"
#include "aw_assert.h"
#include "aw_buzzer.h"
#include "aw_delay.h"
#include "aw_delayed_work.h"
#include "aw_led.h"
#include "aw_task.h"
#include "aw_wdt.h"
#include "aw_netif.h"
#include "aw_wifi_types.h"
#include "uemf.h"
#include "wsIntrn.h"
#include "um.h"
#include "misc/cJSON-2.0.0/cJSON.h"
#include "misc/cJSON-2.0.0/cJSON_Utils.h"
#include "aw_vdebug.h"
#include "cpu_usage.h"
#include "aw_sockets.h"
#include "aw_sdk_version.h"
#include "aw_psp_task.h"

#define FMT_VER     "%d.%d.%d"

#define aw_kprintl(fmt, args...) \
    aw_kprintf(fmt ENDL, ##args)

#define ADC_CH_NUM  3
#define APP_PRINT_HWADDR_VAL(addr) \
    (addr)[0], (addr)[1], (addr)[2], (addr)[3], (addr)[4], (addr)[5]
/*
 *  Change configuration here
 */
static int      finished;                   /* Finished flag */

//static char     web_buf[1024 * 100];

#define WEBS_LOGIN T("/index.html")

#define TASK_WEB_STACK_SIZE    10240
#define TASK_WEB_PRIO          2

#define ADC_SAMPLE_PRIO       6
#define ADC_SAMPLE_STACK_SIZE 512

AW_TASK_DECL_STATIC(task_web_server, TASK_WEB_STACK_SIZE);
AW_TASK_DECL_STATIC(adc_sample, ADC_SAMPLE_STACK_SIZE);

static int g_beep_state = 0;
static uint32_t g_led_state = 0;

static const uint32_t g_adc_chan[] = {
    10
};
static uint32_t g_adc_val[ADC_CH_NUM];
static const uint32_t g_sample_rate = 1500000;

static struct aw_delayed_work g_delayed_work_sys_reboot;

CPU_USAGE_DEF(g_cpu_usage, 40);

/****************************** Forward Declarations **************************/

static int  initWebs ();
static int  websHomePageHandler (webs_t wp, char_t *urlPrefix, char_t *webDir,
                int arg, char_t *url, char_t *path, char_t* query);

#ifdef B_STATS
#error WARNING:  B_STATS directive is not supported in this OS!
#endif


/*********************************** Code *************************************/

static void sys_reboot (void *p_arg)
{
    static struct awbl_wdt wdt_dev;
    aw_wdt_add(&wdt_dev, 100);
}

static uint32_t adc_val_get (int ch)
{
    if (ch > ADC_CH_NUM - 1) {
        return 0;
    }
    return g_adc_val[ch];
}

static void adc_test_init(void)
{

    int i;

    for (i = 0; i < AW_NELEMENTS(g_adc_chan); i++) {
        aw_adc_rate_set(g_adc_chan[i], g_sample_rate);
    }
}

static void adc_test_read_once(void)
{
    int                 i;
    uint16_t            adc_val = 0;

    for (i = 0; i < AW_NELEMENTS(g_adc_chan); i++) {
        aw_adc_sync_read(g_adc_chan[i], &adc_val, 1, AW_TRUE);
        aw_adc_val_to_mv(g_adc_chan[i], &adc_val, 1, &g_adc_val[i]);
//        aw_kprintf("adc-chan:%d,val:%dmv\n", g_adc_chan[i], g_adc_val[i]);
    }
}

static void adc_sample_task(void *p_arg)
{
    adc_test_init();
    while (1) {
        adc_test_read_once();
        aw_mdelay(1000);
    }
}


/*
 *  Write a webs header. This is a convenience routine to write a common
 *  header for a form back to the browser.
 */

void websHeaderOnly (webs_t wp)
{
    a_assert(websValid(wp));

    websWrite(wp, T("HTTP/1.0 200 OK\r\n"));

    /*
     *  The Server HTTP header below must not be modified unless
     *  explicitly allowed by licensing terms.
     */
#ifdef WEBS_SSL_SUPPORT
    websWrite(wp, T("Server: %s/%s %s/%s\r\n"),
            T("AWorks", T("0.1.0"), SSL_NAME, SSL_VERSION);
#else
    websWrite(wp, T("Server: %s/%s\r\n"), T("AWorks"), T("0.1.0"));
#endif

    websWrite(wp, T("Pragma: no-cache\r\n"));
    websWrite(wp, T("Cache-control: no-cache\r\n"));
    websWrite(wp, T("Content-Type: text/plain\r\n"));
    websWrite(wp, T("\r\n"));
}

int websHandleDevInfo (webs_t wp, char_t *urlPrefix, char_t *webDir, int arg,
        char_t *url, char_t *path, char_t *query)
{
    struct in_addr ip;
    struct in_addr nm;
    struct in_addr gw;
#ifdef IN_ETH0
    aw_netif_ipv4_addr_get_by_name(IN_ETH0, &ip, &nm, &gw);
#else
    aw_netif_ipv4_addr_get_by_name("ap", &ip, &nm, &gw);
#endif

    websHeaderOnly(wp);
    websWrite(wp,
                "{\"v_aworks\":\"V" "%s" "\","
                "\"v_wifi\":\"V" FMT_VER "\","
                "\"v_web\":\"V" FMT_VER "\",",
                aw_sdk_version_get(),
                1, 0, 1,
                1, 2, 0
                );
    websWrite(wp,
                "\"net_ipaddr\":\"%s\",",
                inet_ntoa(ip)
                );
    websWrite(wp,
                "\"net_netmskr\":\"%s\",",
                inet_ntoa(nm)
                );
    websWrite(wp,
                "\"net_gateway\":\"%s\"}",
                inet_ntoa(gw)
                );
    websDone(wp, 200);
    return 1;
}

int websHandleIoCtrl (webs_t wp, char_t *urlPrefix, char_t *webDir, int arg,
        char_t *url, char_t *path, char_t *query)
{
    websHeaderOnly(wp);
    if (WEBS_POST_REQUEST & websGetRequestFlags(wp)) {
        cJSON *object = NULL;
        cJSON *json = NULL;

        object = cJSON_Parse(wp->postData);

        json = cJSON_GetObjectItem(object, "sw1");
        if (json) {
            if (json->valueint == 1) {
                aw_buzzer_on();
                g_beep_state = AW_TRUE;
            } else if (json->valueint == 0) {
                aw_buzzer_off();
                g_beep_state = AW_FALSE;
            }
        }

        json = cJSON_GetObjectItem(object, "sw2");
        if (json) {
            if (json->valueint == 1) {
                aw_led_on(0);
                g_led_state |= (1ul << 0);
            } else if (json->valueint == 0) {
                aw_led_off(0);
                g_led_state &= ~(1ul << 0);
            }
        }
        cJSON_Delete(object);
    } else {
        websWrite(wp,
                "{\"val1\":%d,\"val2\":%d}",
                g_beep_state,
                ((g_led_state & (1ul << 0)) ? 1 : 0)
                );
    }
    websDone(wp, 200);
    return 1;
}

int websHandleDataRefresh (webs_t wp, char_t *urlPrefix, char_t *webDir, int arg,
        char_t *url, char_t *path, char_t *query)
{
    websHeaderOnly(wp);
    websWrite(wp,
            "{\"val1\":%d,\"val2\":%d,\"val3\":%d}",
            adc_val_get(0),
            adc_val_get(1),
            adc_val_get(2)
            );
    websDone(wp, 200);
    return 1;
}

int websHandleSysTime (webs_t wp, char_t *urlPrefix, char_t *webDir, int arg,
        char_t *url, char_t *path, char_t *query)
{
    aw_tm_t tm;

    aw_tm_get(&tm);

    websHeaderOnly(wp);
    if (WEBS_POST_REQUEST & websGetRequestFlags(wp)) {
        cJSON *object = NULL;
        cJSON *json = NULL;

        object = cJSON_Parse(wp->postData);

        memset(&tm, 0, sizeof(tm));
        json = cJSON_GetObjectItem(object, "y");
        if (json) {
            tm.tm_year = json->valueint;
        }
        json = cJSON_GetObjectItem(object, "m");
        if (json) {
            tm.tm_mon = json->valueint;
        }
        json = cJSON_GetObjectItem(object, "d");
        if (json) {
            tm.tm_mday = json->valueint;
        }
        json = cJSON_GetObjectItem(object, "h");
        if (json) {
            tm.tm_hour = json->valueint;
        }
        json = cJSON_GetObjectItem(object, "i");
        if (json) {
            tm.tm_min = json->valueint;
        }
        json = cJSON_GetObjectItem(object, "s");
        if (json) {
            tm.tm_sec = json->valueint;
        }
        tm.tm_isdst = -1;
        cJSON_Delete(object);
        if (aw_tm_set(&tm) == AW_OK) {
            websWrite(wp, "ok");
        } else {
            websWrite(wp, "fail");
        }
    } else {
        websWrite(wp,
                "{\"data\":\"%04d-%02d-%02d\",\"time\":\"%02d:%02d:%02d\"}",
                1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec
                );
    }
    websDone(wp, 200);
    return 1;
}

void task_delay(unsigned int ms ){
    aw_tick_t tick = aw_ms_to_ticks (ms);
    rtk_task_delay(tick);
}


int websHandleSysState (webs_t wp, char_t *urlPrefix, char_t *webDir, int arg,
        char_t *url, char_t *path, char_t *query)
{
    struct rtk_task_info info[128];
    int i = 0;
    int err;

    websHeaderOnly(wp);
    err = rtk_enum_all_task_info(info, 64, NULL);

    /*计算cpu负载,待实现*/
    websWrite(wp, "{\"cpu\":[");
    for(i = 0;i < err;i++){
        websWrite(wp, "{\"r\":%d},", 20);

    }

    /*计算堆栈信息*/
    websWrite(wp, "],\"stack\":[");
    for(i = 0;i < err;i++){
        websWrite(wp, "{\"n\":\"%s\",\"p\":%d,\"u\":%d,\"f\":%d,},",
                info[i].name, info[i].prio, info[i].stack_high - info[i].sp, info[i].sp - info[i].stack_low);
    }

    websWrite(wp, "]}");
    websDone(wp, 200);
    return 1;
}

int websHandleSysReboot (webs_t wp, char_t *urlPrefix, char_t *webDir, int arg,
        char_t *url, char_t *path, char_t *query)
{
    aw_tm_t tm;
    aw_tm_get(&tm);

    websHeaderOnly(wp);
    websWrite(wp, "ok");
    websDone(wp, 200);
    aw_delayed_work_start(&g_delayed_work_sys_reboot, 3000);
    return 1;
}

/**
 * \brief   Main -- entry point from eCos
 */
static void web_server_entry (void *pdata)
{
    /*
     *  Initialize the memory allocator. Allow use of malloc and start
     *  with a 60K heap.  For each page request approx 8KB is allocated.
     *  60KB allows for several concurrent page requests.  If more space
     *  is required, malloc will be used for the overflow.
     */
//    bopen(web_buf, sizeof(web_buf), B_USER_BUF);
    bopen(NULL, 1024 * 500, B_USE_MALLOC);

    /*
     *  Initialize the web server
     */
    if (initWebs() < 0) {
        return;
    }

    umOpen();
    umAddGroup("goahead", PRIV_ADMIN, AM_DIGEST, AW_TRUE, AW_FALSE);
    umAddUser("aworks", "aworks", "goahead", AW_TRUE, AW_FALSE);
//    umAddAccessLimit("/", AM_DIGEST, 0, "goahead");

    /*
     *  Basic event loop. SocketReady returns true when a socket is ready for
     *  service. SocketSelect will block until an event occurs. SocketProcess
     *  will actually do the servicing.
     */
    while (!finished) {
        if (socketReady(-1) || socketSelect(-1, 2000)) {
            socketProcess(-1);
        }
        emfSchedProcess();
    }

    /*
     *  Close the socket module, report memory leaks and close the memory allocator
     */
    websCloseServer();
    socketClose();
    bclose();

}

/**
 * \brief   Initialize the web server.
 */
static int initWebs()
{
    char        host[128];
    char_t      wbuf[128];

    memset(host, 0, sizeof(host));
    memset(wbuf, 0, sizeof(wbuf));

    /*
     *  Initialize the socket subsystem
     */
    socketOpen();

    /*
     *  Configure the web server options before opening the web server
     */
    websSetDefaultDir("/");

//    websSetIpaddr("0.0.0.0");
//    ascToUni(wbuf, host, min(strlen(host) + 1, sizeof(wbuf)));
//    websSetHost(wbuf);

    /*
     *  Configure the web server options before opening the web server
     */
    websSetDefaultPage(WEBS_LOGIN);
//    websSetPassword(password);

    /*
     *  Open the web server on the given port. If that port is taken, try
     *  the next sequential port for up to "retries" attempts.
     */
    websOpenServer(80, 5);

    /*
     *  First create the URL handlers. Note: handlers are called in sorted order
     *  with the longest path handler examined first. Here we define the security
     *  handler, forms handler and the default web page handler.
     */
    websUrlHandlerDefine(T(""), NULL, 0, websSecurityHandler,
        WEBS_HANDLER_FIRST);
    websUrlHandlerDefine(T("/goform"), NULL, 0, websFormHandler, 0);
    websUrlHandlerDefine(T(""), NULL, 0, websDefaultHandler,
        WEBS_HANDLER_LAST);

    /*
     *  Create a handler for the default home page
     */
    websUrlHandlerDefine(T("/"), NULL, 0, websHomePageHandler, 0);

    /**
     * Create a handler for dyn page
     */
    websUrlHandlerDefine(T("/dev_info.php"), NULL, 0, websHandleDevInfo, 0);
    websUrlHandlerDefine(T("/io_ctrl.php"), NULL, 0, websHandleIoCtrl, 0);
    websUrlHandlerDefine(T("/data_refresh.php"), NULL, 0, websHandleDataRefresh, 0);
    websUrlHandlerDefine(T("/sys_time.php"), NULL, 0, websHandleSysTime, 0);
    websUrlHandlerDefine(T("/sys_state.php"), NULL, 0, websHandleSysState, 0);
    websUrlHandlerDefine(T("/sys_reboot.php"), NULL, 0, websHandleSysReboot, 0);
    return 0;
}

/**
 * \brief	Home page handler
 */
static int websHomePageHandler (webs_t wp, char_t *urlPrefix, char_t *webDir,
    int arg, char_t* url, char_t* path, char_t* query)
{
    /*
     *  If the empty or "/" URL is invoked, redirect default URLs to the home page
     */
    if (*url == '\0' || gstrcmp(url, T("/")) == 0) {
        websRedirect(wp, WEBS_LOGIN);
        return 1;
    }
    return 0;
}

/**
 * \brief Web Server例程入口函数
 */
void app_webserver_goahead_entry(void)
{
    aw_delayed_work_init(&g_delayed_work_sys_reboot, sys_reboot, NULL);

    AW_TASK_INIT(adc_sample,
                 "adc_sample",
                 ADC_SAMPLE_PRIO,
                 ADC_SAMPLE_STACK_SIZE,
                 adc_sample_task,
                 NULL);

    AW_TASK_INIT(task_web_server,       /* 任务实体 */
                 "goahead",             /* 任务名字 */
                 TASK_WEB_PRIO,         /* 任务优先级 */
                 TASK_WEB_STACK_SIZE,   /* 任务堆栈大小 */
                 web_server_entry,      /* 任务入口函数 */
                 NULL);                 /* 任务入口参数 */

    AW_TASK_STARTUP(adc_sample);
    AW_TASK_STARTUP(task_web_server);   /* 启动任务 */
}

/** [src_webserver_goahead] */

/* end of file */
