/*
 * report.c
 *
 *  Created on: 2019年10月16日
 *      Author: zengqingyuhang
 */


#include "aw_time.h"
#include "aw_temp.h"
#include "client.h"
#include "settings.h"
#include "app_config.h"
#include "aw_list.h"
#include "string.h"
#include "aw_mem.h"
#include "aw_vdebug.h"

#define __TIME_ZONE 8


BOOL sdk_demo_devtype_get_all_data(params_t* params,
                                   struct aw_list_head *sensor_list,
                                   char *keybuff)
{

    int         i = 0;
    float       tmp1;
    char        buf[64];
    uint64_t    t;
    static int  heartbeats = 0;

    params->size = 1;
    struct sensor_devinfo *sensor_dev = NULL;

    /* 遍历设备链表中的每一个节点，使能每一个节点中的传感器通道 */
    aw_list_for_each_entry(sensor_dev, sensor_list, struct sensor_devinfo, node) {

        for (i = 0; i < sensor_dev->cfg->chn_nums; i++) {

            memset((void *)keybuff, 0x00, sizeof(*keybuff));            /* 清空缓存 */
            sprintf(keybuff, "%s_%s_%d", sensor_dev->cfg->name,         /* 生成关键字 */
                                         (char *)sensor_dev->data->type[i],
                                         i);

//            aw_kprintf("%s\r\n",keybuff);

            params_append_float(params, (const char *)keybuff, sensor_dev->data->value[i]);
        }
    }

    /* 打包CPU温度 */
    tmp1 = aw_temp_read(0) / 1000.0;
    params_append_float(params, "cpu_temperature", tmp1);

    /* 打包系统时间 */
    t = ((uint64_t)aw_time(NULL) - (__TIME_ZONE * 3600)) * 1000;
    snprintf(buf, sizeof(buf), "%lld", t);
    params_append_string(params, "time", buf);

    /* 上报自定义heartbeats */
    heartbeats++;
    params_append_int(params, "heartbeat", heartbeats);

    return TRUE;
}

