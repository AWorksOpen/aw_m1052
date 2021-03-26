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

/**
 * \file
 * \brief Sensor ��ʾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - �����������꣨ע���Ƿ�������������ų�ͻ���Ͷ�Ӧ��I2C��
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. �ڿ������Ͻ������������.
 *
 * - ʵ������
 *   1. ���ڰ��մ�����ID˳���ӡ���յ��Ĵ��������ݡ�
 *
 */

#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_sensor.h"
#include "aw_shell.h"
#include "math.h"
#include "aw_task.h"


#define SENSOR_START_ID          0
#define SENSOR_END_ID            35


aw_local aw_err_t aw_sensor_read (uint32_t chn, double *sample, uint8_t *type)
{
    aw_err_t        err = 0;
    aw_sensor_val_t sen_data;
    double          data = 0.0;

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

    err = aw_sensor_type_get(chn);
    if (err == AW_ENODEV) {
        aw_kprintf("sensor id:%d get type error!\r\n", chn);
        return AW_ERROR;
    }
    *type = err;
    return AW_OK;
}

aw_local void* __sensor_handler (void *parg)
{
    aw_err_t        err         = 0;
    uint8_t         i           = 0;
    const char     *name        = NULL;
    const char     *unit        = NULL;
    double          sensor_data = 0;
    uint8_t         sensor_type = 0;

    for (i = SENSOR_START_ID; i < SENSOR_END_ID; i++) {
        err = aw_sensor_enable(i);
            if (err != AW_OK) {
                aw_kprintf("sensor id:%d enable failed! err:%d\r\n", i);
        }
    }

    AW_FOREVER {
        aw_mdelay(1000);
        aw_kprintf("\r\n");
        for (i = SENSOR_START_ID; i <= SENSOR_END_ID; i++) {
            err = aw_sensor_type_name_get(i, &name, &unit);
            if (err != AW_OK) {
                aw_kprintf("sensor id:%d get name error!\r\n", i);
            } else {
                err = aw_sensor_read(i, &sensor_data, &sensor_type);
                if (err != AW_OK) {
                    aw_kprintf("sensor id:%d %s get data error!\r\n", i, name);
                }else{
                    aw_kprintf("sensor id:%d %s:%.3f-%s\r\n", i, name, sensor_data, unit);
                }
            }
        }
    }

    return 0;
}

/**
 * \brief ������ demo
 * \return ��
 */
void demo_sensor_entry (void)
{
    aw_task_id_t    tsk;
    tsk = aw_task_create("sensor_task",
                         10,
                         2048,
                         __sensor_handler,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("sensor task create error!\r\n");
        return ;
    }
    aw_task_startup(tsk);
}

/* end of file*/
