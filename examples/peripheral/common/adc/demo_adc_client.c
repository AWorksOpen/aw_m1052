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
 * \brief ADC利用客户端获取采样电压例程
 *
 * 一般使用客户端是为了满足较为复杂的电压采样要求，用户可根据实际需求进行灵活的
 * 配置。
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 对应开发平台的ADC1宏
 *      - 对应平台的串口宏
 *      - AW_COM_CONSOLE
 *
 * - 实验现象：
 *   1. 串口每隔1s打印出当前采样序列的电压值。
 *
 * - 备注：
 *   1. 用户可在对应平台的 awbl_hwconf_xxxxxx_adc1.h 文件中修改adc参数配置
 *
 * \par 源代码
 * \snippet demo_adc_client.c src_adc_client
 *
 * \internal
 * \par Modification History
 * - 1.00 15-03-24  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_adc_client ADC客户端采样
 * \copydoc demo_adc_client.c
 */

/** [src_adc_client] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_adc.h"
#include "aw_vdebug.h"
#include "aw_msgq.h"
#include "aw_demo_config.h"

#define ADC_CHANNEL    DE_ADC_CH    /* ADC通道号 */

#define SAMPLES_NUM    5            /* 采样次数 */
#define ADC_READ_CNT   20           /* 读取ADC次数 */

/******************************************************************************/

/* ADC完成转换结构体 */
struct adc_complete {
    int stat;    /**< \brief 转换结果 */
    int seq;     /**< \brief 转换序列 */
};

/******************************************************************************/

/* 声明一个消息队列 (20条struct adc_complete类型的消息) */
AW_MSGQ_DECL_STATIC(adc_msgq, 20, sizeof(struct adc_complete));

aw_local uint16_t __adc_val0[ADC_READ_CNT];  /* adc接收缓冲区0 */
aw_local uint16_t __adc_val1[ADC_READ_CNT];  /* adc接收缓冲区1 */
aw_local uint32_t __vol_val[ADC_READ_CNT];   /* adc转换缓冲区 */

/******************************************************************************/

/**
 * \brief ADC转换完成回调
 * \param[in] p_arg : 任务参数
 * \param[in] stat  : 转换结果标志
 * \return 无
 */
aw_local void __adc_complete_cb (void *p_arg, int stat)
{
    struct adc_complete msg = {stat, (int)p_arg};

    /* 发送一个消息 */
    AW_MSGQ_SEND(adc_msgq,
                 &msg,
                 sizeof(struct adc_complete),
                 AW_MSGQ_NO_WAIT,
                 AW_MSGQ_PRI_NORMAL);
}

/**
 * \brief ADC 客户端 demo
 * \return 无
 */
aw_local void* __task_handle (void *parg)
{
    aw_adc_client_t     client;
    aw_adc_buf_desc_t   desc[2];
    struct adc_complete info;
    uint32_t            i;

    /* 初始化一个消息队列 */
    AW_MSGQ_INIT(adc_msgq, 20, sizeof(struct adc_complete), AW_SEM_Q_PRIORITY);

    /* 初始化ADC缓冲区描述符0 */
    aw_adc_mkbufdesc(&desc[0], __adc_val0, ADC_READ_CNT, __adc_complete_cb, (void *)0);

    /* 初始化ADC缓冲区描述符1 */
    aw_adc_mkbufdesc(&desc[1], __adc_val1, ADC_READ_CNT, __adc_complete_cb, (void *)1);

    /* 初始化ADC客户端 */
    aw_adc_client_init(&client, ADC_CHANNEL, AW_FALSE);

    /* 持续转换 */
    aw_adc_client_start(&client, desc, 2, 0);

    while (1) {

        /* 等待获取一条消息 */
        AW_MSGQ_RECEIVE(adc_msgq,
                        &info,
                        sizeof(struct adc_complete),
                        AW_MSGQ_WAIT_FOREVER);

        if (info.stat != AW_OK) {
            aw_kprintf("缓冲区 %d 转换出错！！！\r\n",info.seq);
        } else {

            if (info.seq == 0) {
                aw_kprintf("seq 0 complete!\r\n");

                /* 转换成电压值 */
                aw_adc_val_to_mv(ADC_CHANNEL, __adc_val0, ADC_READ_CNT, __vol_val);
            }
            if (info.seq == 1) {
                aw_kprintf("seq 1 complete!\r\n");

                /* 转换成电压值 */
                aw_adc_val_to_mv(ADC_CHANNEL, __adc_val1, ADC_READ_CNT, __vol_val);
            }

            /* 打印20次的电压值 */
            for (i = 0; i < ADC_READ_CNT; i++) {
                aw_kprintf("%d conversion result : %dmV\n", i + 1, __vol_val[i]);
            }
        }

        aw_mdelay(1000);
    }

    return 0;
}

/******************************************************************************/
/* ADC demo入口 */
void demo_adc_client_entry (void)
{
    aw_task_id_t    tsk;

    /* 创建一个ADC采集任务(优先级12，棧4096) */
    tsk = aw_task_create("Adc Client demo",
                         12,
                         4096,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("Adc Client demo task create failed\r\n");
        return;
    }

    /* 启动任务 */
    if (aw_task_startup(tsk) != AW_OK) {
        aw_task_delete(tsk);
    }
}

/** [src_adc_client] */

/* end of file */
