/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief ADC读取采样电压例程(同步读取)
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 对应平台的 ADC1宏
 *      - 对应平台的串口宏
 *      - AW_COM_CONSOLE
 *
 * - 实验现象：
 *   1. 串口每隔500ms，打印出连续采样到的电压值
 *   2. 将开发板上的DE_ADC_CH引脚接3.3v电源和GND观察电压值变化
 *
 * - 备注：
 *   1. 用户可在对应平台的  awbl_hwconf_xxxxxx_adc1.h 文件中修改adc参数配置
 *
 * \par 源代码
 * \snippet demo_adc.c src_adc
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-23  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_adc ADC采样
 * \copydoc demo_adc.c
 */

/** [src_adc] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_adc.h"
#include "aw_vdebug.h"
#include "string.h"
#include "aw_demo_config.h"

/* ADC通道号 */
#define ADC_CHANNEL    DE_ADC_CH

/* 采样次数 */
#define SAMPLES_NUM    5

/**
 * \brief ADC 同步读取电压值 demo
 * \return 无
 */
aw_local void* __task_handle (void *parg)
{
    uint16_t adc_val[SAMPLES_NUM];  /* ADC值 */
    uint32_t vol_val[SAMPLES_NUM];  /* 电压值 */
    int      i;

    /* ADC值清0 */
    memset(adc_val, 0, sizeof(adc_val));

    while (1) {

        /* 读取通道的AD值 */
        aw_adc_sync_read(ADC_CHANNEL, adc_val, SAMPLES_NUM, AW_FALSE);

        /* 转换为实际电压值 */
        aw_adc_val_to_mv(ADC_CHANNEL, adc_val, SAMPLES_NUM, vol_val);

        /* 打印5次采样电压值 */
        for (i = 0; i < SAMPLES_NUM; i++) {
            aw_kprintf(" %d conversion result : %dmV\n", i + 1, vol_val[i]);
        }
        aw_mdelay(500);
        aw_kprintf("\r\n");
    }

    return 0;
}

/******************************************************************************/
/* ADC demo入口函数 */
void demo_adc_entry (void)
{
    aw_task_id_t    tsk;

    /* 创建一个ADC采集任务(优先级12，棧2048) */
    tsk = aw_task_create("Adc demo",
                         12,
                         2048,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("Adc demo task create failed\r\n");
        return;
    }

    /* 启动任务 */
    if (aw_task_startup(tsk) != AW_OK) {
        aw_task_delete(tsk);
    }
}

/** [src_adc] */

/* end of file */
