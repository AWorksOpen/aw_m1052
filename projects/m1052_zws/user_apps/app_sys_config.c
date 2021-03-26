/*
 * app_config_mode.c
 *
 *  Created on: 2019年10月28日
 *      Author: zengqingyuhang
 */

#include "aworks.h"
#include "aw_gpio.h"
#include "imx1050_pin.h"
#include "app_config.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "string.h"
#include "aw_mem.h"


extern aw_err_t app_udisk_workgroup (struct app_sys_config *p_cfg);
extern aw_err_t app_get_cfginfo(struct app_sys_config *p_cfg);
extern aw_err_t app_shell_config (void);


aw_err_t app_sys_configration (void *p_arg)
{
    aw_err_t ret = AW_ERROR;
    struct app *p_devinfo = (struct app *)p_arg;
    struct app_sys_config *p_cfg = NULL;

    /* 创建云设备配置对象 */
    p_cfg = (struct app_sys_config *)aw_mem_alloc(sizeof(*p_cfg));
    if (p_cfg == NULL) {
        aw_kprintf("struct app_sys_config memory alloc failed!\r\n");
        goto __failed;
    }
    memset(p_cfg, 0x00, sizeof(*p_cfg));

    /* 初始化传感器设备链表头节点 */
    aw_list_head_init(&p_cfg->sensor_head);

    /* 创建云设备net配置对象 */
    p_cfg->net = (struct net_info *)aw_mem_alloc(sizeof(*p_cfg->net));
    if (p_cfg->net == NULL) {
        aw_kprintf("struct net_info memory alloc failed!\r\n");
        goto __failed;
    }
    memset(p_cfg->net, 0x00, sizeof(*p_cfg->net));

    /* 创建云设备信息配置对象 */
    p_cfg->zws_devinfo = (struct zws_dev_info *)aw_mem_alloc(sizeof(*p_cfg->zws_devinfo));
    if (p_cfg->zws_devinfo == NULL) {
        aw_kprintf("struct zws_dev_info memory alloc failed!\r\n");
        goto __failed;
    }
    memset(p_cfg->zws_devinfo, 0x00, sizeof(*p_cfg->zws_devinfo));

    /* 读取Flash中的配置信息启动云应用 */
    ret = app_get_cfginfo(p_cfg);
    if (ret != AW_OK) {
        aw_kprintf("\r\nConfig information is invalied.\r\n");
    } else {
        aw_kprintf("\r\nConfig information is valied.\r\n");
    }

    /* 启动U盘更新配置信息的后台任务 */
    ret = app_udisk_workgroup (p_cfg);
    if (ret != AW_OK) {
        aw_kprintf("U-Disk update config information is disabled.\r\n");
    } else {
        aw_kprintf("U-Disk workgroup startup success.\r\n");
    }

    /* 启动Shell更新配置信息后台任务 */
    ret = app_shell_config ();
    if (ret != AW_OK) {
        aw_kprintf("Shell update config information is disabled.\r\n");
    } else {
        aw_kprintf("Shell workgroup is success.\r\n");
    }

    /* 配置对象传递给云设备实例 */
    p_devinfo->p_cfg = p_cfg;

    if (p_cfg->cfg_sta == VALID) {

        return AW_OK;
    }

    return AW_ERROR;

__failed:
    if (p_cfg->net) {
        aw_mem_free(p_cfg->net);
    }
    if (p_cfg->zws_devinfo) {
        aw_mem_free(p_cfg->zws_devinfo);
    }
    if (p_cfg) {
        aw_mem_free(p_cfg);
    }

    return AW_ERROR;
}











