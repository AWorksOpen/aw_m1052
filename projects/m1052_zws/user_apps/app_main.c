/*
 * app_main.c
 *
 *  Created on: 2019年8月10日
 *      Author: wengyedong
 */

#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_buzzer.h"
#include "app_config.h"
#include "imx1050_pin.h"
#include "aw_delay.h"

extern void app_led (void);
extern aw_err_t app_sensor(struct app *p_app);
extern aw_err_t app_net_init (struct app *p_this, const char *wifi_device);
extern void *demo_zws (struct app *p_dev);
//extern aw_err_t app_sys_config (void *p_arg);

extern aw_err_t app_sys_configration (void *p_arg);

/* 定义应用实例对象 */
struct app __g_app;

void app_main (void)
{
    aw_err_t ret;
    /* led */
    app_led();

    /* 系统配置 */
    ret = app_sys_configration (&__g_app);
    if (ret != AW_OK) {
        aw_kprintf("system config error.\r\n");
        ret = AW_ERROR;
        goto __exit;
    }
    aw_buzzer_beep(200);

    /* 创建传感器线程 */
    ret = app_sensor(&__g_app);
    if (ret != AW_OK) {
        aw_kprintf("sensor init error.\r\n");
        ret = AW_ERROR;
        goto __exit;
    }

    /* 初始化网络功能 */
    ret = app_net_init(&__g_app, "wifi43362");
    if (ret != AW_OK) {
        aw_kprintf("Wi-Fi init error %d.\r\n", ret);
        goto __exit;
    }

    /* 运行云功能 */
    __g_app.p_zws = demo_zws(&__g_app);
    if (__g_app.p_zws == NULL) {
        aw_kprintf("ZWS init error.\r\n");
        ret = AW_ERROR;
    }

    aw_buzzer_beep(100);
    aw_mdelay(100);
    aw_buzzer_beep(100);

__exit:
    if (ret != AW_OK) {
        aw_buzzer_beep(1000);
    }
}
