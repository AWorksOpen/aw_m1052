/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_sensor.h"
#include "aw_shell.h"
#include "math.h"
#include "aw_task.h"
#include "app_config.h"
#include "aw_list.h"
#include "aw_mem.h"
#include "string.h"

struct sensor_cfginfo sensor_list[] = {
        /* ���ƣ� ��ʼid�� ͨ������ */
        {"HTS221",    0,    2},
        {"SHTC1",     2,    2},
        {"LIS3MDL",   4,    4},       /* ���豸��M1052-WIFI���İ��Ʒ���ų�ͻ��easyARM�������ʹ�� */
        {"BME280",    8,    3},
        {"BMA253",    11,   4},
        {"BMP280",    15,   2},
        {"LPS22HB",   17,   2},
        {"LSM6DSL",   19,   7},
        {"BH1730",    26,   1},
        {"BMG160",    27,   3},
        {"MMC5883MA", 30,   4},       /* ���豸��M1052-WIFI���İ��Ʒ���ų�ͻ��easyARM�������ʹ�� */
//        {"LTR553",    34,   2},       /* ���豸ʹ����Ч */
};


aw_err_t app_sensor_create (struct app_sys_config *p_cfg)
{
    uint32_t    sensor_nums = 0;
    uint32_t    i = 0;

    struct sensor_devinfo *p_buf = NULL;
    sensor_nums = sizeof(sensor_list) / sizeof(struct sensor_cfginfo);  /* ���㴫�����豸������ */
    for (i = 0; i < sensor_nums; i++) {
        /* Ϊÿ�������������豸���� */
        p_buf = (struct sensor_devinfo *)aw_mem_alloc(sizeof(struct sensor_devinfo));
        if (p_buf == NULL) {
            aw_kprintf("%s sensor devinfo memory alloc error!\r\n", sensor_list[i].name);
            goto __failed;
        }
        memset((void *)p_buf, 0x00, sizeof(struct sensor_devinfo));             /* ��ջ��� */

        /* Ϊ��ǰ�������豸����������Ϣ�ڴ� */
        p_buf->cfg = (struct sensor_cfginfo *)aw_mem_alloc(sizeof(struct sensor_cfginfo));
        if (p_buf->cfg == NULL) {
            aw_kprintf("%s sensor cfginfo memory alloc error!\r\n", sensor_list[i].name);
            goto __failed;
        }
        memset((void *)p_buf->cfg, 0x00, sizeof(struct sensor_cfginfo));             /* ��ջ��� */

        /* Ϊ��ǰ�������豸����������Ϣ�ڴ� */
        p_buf->data = (struct sensor_datinfo *)aw_mem_alloc(sizeof(struct sensor_datinfo));
        if (p_buf->data == NULL) {
            aw_kprintf("%s sensor datainfo memory alloc error!\r\n", sensor_list[i].name);
            goto __failed;
        }
        memset((void *)p_buf->data, 0x00, sizeof(struct sensor_datinfo));             /* ��ջ��� */

        p_buf->cfg->name = sensor_list[i].name;
        p_buf->cfg->start_chn = sensor_list[i].start_chn;
        p_buf->cfg->chn_nums = sensor_list[i].chn_nums;

        /* Ϊ����ͨ������type�ڴ� */
        p_buf->data->type = (uint32_t *)aw_mem_alloc(sizeof(*p_buf->data->type) * p_buf->cfg->chn_nums);
        if(p_buf->data->type == NULL) {
            aw_kprintf("sensor type memory alloc failed!,line:%d\r\n", __LINE__);
            goto __failed;
        }
        memset(p_buf->data->type, 0x00, sizeof(*p_buf->data->type) * p_buf->cfg->chn_nums);

        /* Ϊ����ͨ������unit�ڴ� */
        p_buf->data->unit = (uint32_t *)aw_mem_alloc(sizeof(*p_buf->data->unit) * p_buf->cfg->chn_nums);
        if(p_buf->data->unit == NULL) {
            aw_kprintf("sensor unit memory alloc failed!,line:%d\r\n", __LINE__);
            goto __failed;
        }
        memset(p_buf->data->unit, 0x00, sizeof(*p_buf->data->unit) * p_buf->cfg->chn_nums);

        /* Ϊ����ͨ������value�ڴ� */
        p_buf->data->value = (double *)aw_mem_alloc(sizeof(*p_buf->data->value) * p_buf->cfg->chn_nums);
        if(p_buf->data->value == NULL) {
            aw_kprintf("sensor value memory alloc failed!,line:%d\r\n", __LINE__);
            goto __failed;
        }
        memset(p_buf->data->value, 0x00, sizeof(*p_buf->data->value) * p_buf->cfg->chn_nums);

        /* ���������豸��ӵ������� */
        aw_list_add(&p_buf->node, &p_cfg->sensor_head);

        continue;

__failed:
        if (p_buf->cfg != NULL) {
            aw_mem_free((void *)p_buf->cfg);
        }
        if (p_buf->data != NULL) {
            aw_mem_free((void *)p_buf->data);
        }
        if (p_buf != NULL) {
            aw_mem_free((void *)p_buf);
        }
        return AW_ERROR;
    }

    return AW_OK;
}


aw_err_t app_sensor_enable (struct app_sys_config *p_cfg)
{
    uint32_t          i         = 0;
    aw_err_t          ret       = 0;
    const char *      t_type    = NULL;
    const char *      t_unit    = NULL;
    struct sensor_devinfo *sensor_dev = NULL;
    /* �����豸�����е�ÿһ���ڵ㣬ʹ��ÿһ���ڵ��еĴ�����ͨ�� */
    aw_list_for_each_entry(sensor_dev, &p_cfg->sensor_head, struct sensor_devinfo, node) {

        for (i = 0; i < sensor_dev->cfg->chn_nums; i++) {

            ret  = aw_sensor_enable(sensor_dev->cfg->start_chn + i);
            if (ret != AW_OK) {
                aw_kprintf("sensor enable failed:%d \r\n", sensor_dev->cfg->start_chn + i);
            }

            ret = aw_sensor_type_name_get (sensor_dev->cfg->start_chn + i,
                                           &t_type,
                                           &t_unit );
            if (ret != AW_OK) {
                aw_kprintf("get sensor type and uint failed:%d \r\n", sensor_dev->cfg->start_chn + i);
            }
            *(sensor_dev->data->type + i) = (uint32_t)t_type;
            *(sensor_dev->data->unit + i) = (uint32_t)t_unit;
        }
    }

    return AW_OK;
}


aw_err_t __aw_sensor_read(uint32_t chn, double *sample)
{
    aw_err_t err = 0;
    aw_sensor_val_t sen_data;
    double data = 0.0;

    err = aw_sensor_data_get(chn, &sen_data);
    if (err != AW_OK) {
        aw_kprintf("sensor id:%d get data error!\r\n", chn);
        return err;
    }

    data = sen_data.val;
    if (sen_data.unit > 0) {
        *sample = data * pow(10, sen_data.unit);
    } else if (sen_data.unit == 0) {
        *sample = data ;
    } else if (sen_data.unit < 0) {
        *sample = data / pow(10, ((~sen_data.unit) + 1));
    } else {
        aw_kprintf("sensor id:%d unit error!\r\n", chn);
        return err;
    }

    return AW_OK;
}


aw_task_func_t __sensor_read_handler (void *p_arg)
{
    uint32_t    i = 0;
    aw_err_t    ret = 0;
    struct sensor_devinfo *sensor_dev = NULL;
    struct aw_list_head *head = (struct aw_list_head *)p_arg;

    AW_FOREVER {

        /* �����豸�����е�ÿһ���ڵ㣬ʹ��ÿһ���ڵ��еĴ�����ͨ�� */
        aw_list_for_each_entry(sensor_dev, head, struct sensor_devinfo, node) {

            for (i = 0; i < sensor_dev->cfg->chn_nums; i++) {

                ret  = __aw_sensor_read(sensor_dev->cfg->start_chn + i, &sensor_dev->data->value[i]);
                if (ret != AW_OK) {
                    aw_kprintf("read sensor data error,id:%d\r\n", sensor_dev->cfg->start_chn + i);
                }
            }
        }
        aw_mdelay(500);

//        /* �����豸�����е�ÿһ���ڵ㣬��ӡÿһ���ڵ��еĴ�����������Ϣ */
//        aw_list_for_each_entry(sensor_dev, head, struct sensor_devinfo, node) {
//
//            aw_kprintf("name:%s s-chn:%d chns:%d\r\n", sensor_dev->cfg->name,
//                                                       sensor_dev->cfg->start_chn,
//                                                       sensor_dev->cfg->chn_nums);
//            for (i = 0; i < sensor_dev->cfg->chn_nums; i++) {
//
//                aw_kprintf("id:%d type:%s data:%.3f uint:%s\r\n", sensor_dev->cfg->start_chn + i,
//                                                                  (const char *)sensor_dev->data->type[i],
//                                                                  sensor_dev->data->value[i],
//                                                                  (const char *)sensor_dev->data->unit[i]);
//            }
//        }
    }

    return NULL;
}

aw_err_t app_sensor_read_task_create(struct app_sys_config *p_cfg)
{
    aw_task_id_t    tsk;
    tsk = aw_task_create("sensor_read_task",
                          10,
                          4096,
                          __sensor_read_handler,
                          &p_cfg->sensor_head);
    if (tsk == NULL) {
        aw_kprintf("sensor read task create error!\r\n");
        return AW_ERROR;
    }
    aw_task_startup(tsk);
    return AW_OK;
}


aw_err_t app_sensor(struct app *p_app)
{
    aw_err_t    ret = 0;
    ret = app_sensor_create (p_app->p_cfg);
    if (ret != AW_OK) {
        aw_kprintf("app sensor create error!\r\n");
        goto __failed;
    }
    ret = app_sensor_enable (p_app->p_cfg);
    if (ret != AW_OK) {
        aw_kprintf("app sensor enable error!\r\n");
        goto __failed;
    }
    ret = app_sensor_read_task_create(p_app->p_cfg);
    if (ret != AW_OK) {
        aw_kprintf("app sensor read task create error!\r\n");
        goto __failed;
    }

__failed:
    return ret;
}


/* end of file*/

