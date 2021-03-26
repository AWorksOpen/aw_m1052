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
 * \brief ADC��ȡ������ѹ����(ͬ����ȡ)
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ��Ӧƽ̨�� ADC1��
 *      - ��Ӧƽ̨�Ĵ��ں�
 *      - AW_COM_CONSOLE
 *
 * - ʵ������
 *   1. ����ÿ��500ms����ӡ�������������ĵ�ѹֵ
 *   2. ���������ϵ�DE_ADC_CH���Ž�3.3v��Դ��GND�۲��ѹֵ�仯
 *
 * - ��ע��
 *   1. �û����ڶ�Ӧƽ̨��  awbl_hwconf_xxxxxx_adc1.h �ļ����޸�adc��������
 *
 * \par Դ����
 * \snippet demo_adc.c src_adc
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-23  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_adc ADC����
 * \copydoc demo_adc.c
 */

/** [src_adc] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_adc.h"
#include "aw_vdebug.h"
#include "string.h"
#include "aw_demo_config.h"

/* ADCͨ���� */
#define ADC_CHANNEL    DE_ADC_CH

/* �������� */
#define SAMPLES_NUM    5

/**
 * \brief ADC ͬ����ȡ��ѹֵ demo
 * \return ��
 */
aw_local void* __task_handle (void *parg)
{
    uint16_t adc_val[SAMPLES_NUM];  /* ADCֵ */
    uint32_t vol_val[SAMPLES_NUM];  /* ��ѹֵ */
    int      i;

    /* ADCֵ��0 */
    memset(adc_val, 0, sizeof(adc_val));

    while (1) {

        /* ��ȡͨ����ADֵ */
        aw_adc_sync_read(ADC_CHANNEL, adc_val, SAMPLES_NUM, AW_FALSE);

        /* ת��Ϊʵ�ʵ�ѹֵ */
        aw_adc_val_to_mv(ADC_CHANNEL, adc_val, SAMPLES_NUM, vol_val);

        /* ��ӡ5�β�����ѹֵ */
        for (i = 0; i < SAMPLES_NUM; i++) {
            aw_kprintf(" %d conversion result : %dmV\n", i + 1, vol_val[i]);
        }
        aw_mdelay(500);
        aw_kprintf("\r\n");
    }

    return 0;
}

/******************************************************************************/
/* ADC demo��ں��� */
void demo_adc_entry (void)
{
    aw_task_id_t    tsk;

    /* ����һ��ADC�ɼ�����(���ȼ�12����2048) */
    tsk = aw_task_create("Adc demo",
                         12,
                         2048,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("Adc demo task create failed\r\n");
        return;
    }

    /* �������� */
    if (aw_task_startup(tsk) != AW_OK) {
        aw_task_delete(tsk);
    }
}

/** [src_adc] */

/* end of file */
