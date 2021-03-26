
/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief  Web Server演示例程(goahead方式实现)
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 对应平台的wifi外设宏
 *      - 蜂鸣器
 *      - LED
 *   2. 屏蔽其他地方对LED灯的使用
 *   3. 手机加入开发板开启的AP，账号：AWWIFI， 密码：meiyoumima
 *   4. 在shell界面输入 ip addr指令，查看ap ip地址
 *   5. 加入开发板热点后，手机打开浏览器，并输开发板wifi ap的IP地址
 *
 * - 实验现象：
 *   1. 浏览器弹出网站页面，显示出demo网页。
 *
 * \par 源代码
 * \snippet demo_webserver_goahead.c src_webserver_goahead
 *
 * \internal
 * \par modification history:
 * - 1.00 17-09-07, sdy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_webserver_goahead
 * \copydoc demo_webserver_goahead.c
 */

/** [src_std_webserver] */
#include "aworks.h"

extern void app_wifi_ap_entry(void);
extern void app_webserver_goahead_entry(void);

void app_webserver_main(void){

    app_wifi_ap_entry();

    app_webserver_goahead_entry();

    return ;
}
