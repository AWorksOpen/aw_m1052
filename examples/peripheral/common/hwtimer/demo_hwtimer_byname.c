/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief hwtimer ���Գ���
 *
 * hwtimer ͨ�ö�ʱ�����ṩ���¹���:
 *
 * - �������裺
 *   1. ��aw_prj_params.h��ʹ��
 *      - ��Ӧƽ̨��Ӳ����ʱ����
 *      - AW_DEV_GPIO_LED
 *   2. ���������ط���LED�Ĳ���
 *
 * - ʵ������
 *   1. LED�Զ�ʱ��һ���Ƶ����˸��
 *   2. ���ڴ�ӡ��Ӧ��Ϣ��
 *
 * - ��ע��
 *   1. ʹ�ø�����ʱ��Ҫ�ڶ�ʱ���������ļ����޸� ���豸��Ϣ�� Ϊ ��ͨ�����Ʒ��䶨ʱ������
 *
 * \par Դ����
 * \snippet demo_hwtimer_byname.c src_hwtimer_byname
 *
 * \internal
 * \par modification history:
 * - 1.00 2018-07-03  sdq, first implementation
 * \endinternal
 */
/**
 * \addtogroup demo_if_hwtimer_byname Ӳ����ʱ��(byname)
 * \copydoc demo_hwtimer_byname.c
 */

/** [src_hwtimer_byname] */
#include "aworks.h"
#include "aw_int.h"
#include "aw_hwtimer.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_delay.h"
#include "aw_demo_config.h"

#define  __TEST_HWTIMER_DRV_NAME  DE_HWTIMER_TIMER_NAME     /* ��ʱ������ */
#define  __TEST_HWTIMER_BUSID     DE_HWTIMER_TIMER_ID       /* ��ʱ�����ڵ�����ID */

#define  LED  DM_LED

/**
 * \brief Ӳ����ʱ���жϷ�������
 * \param[in] p_arg : �������
 */
static void mytimer_isr (void *p_arg)
{
    aw_led_toggle(LED);
}

/**
 * \brief hwtimer ���Ժ���
 */
aw_local void* __task_handle (void *arg)
{
    aw_hwtimer_handle_t mytimer;

    /* ������Ϊ __TEST_HWTIMER_DRV_NAME �Ķ�ʱ�� */
    mytimer = aw_hwtimer_alloc_byname(__TEST_HWTIMER_DRV_NAME,  /* ��ʱ������ */
                                      __TEST_HWTIMER_BUSID,     /* ��ʱ����������ID */
                                      0,                        /* ��ʱ����� */
                                      mytimer_isr,              /* �жϻص����� */
                                      NULL);                    /* �жϻص������Ĳ��� */

    if (mytimer) {
        aw_hwtimer_enable(mytimer, 20);     /* ʹ�ܶ�ʱ�ж� 20Hz */
    } else {
        aw_kprintf("Timer allocation fail!\n");
    }

    for (;;) {
        aw_mdelay(1000);
    }

    return 0;
}

/******************************************************************************/
void demo_hwtimer_byname_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Hwtimer byname demo",
                         12,
                         1024,
                         __task_handle,
                         NULL);
    if (tsk == NULL) {
        aw_kprintf("Hwtimer byname demo task create failed\r\n");
        return;
    }

    aw_task_startup(tsk);

}

/** [src_hwtimer_byname] */

/*end of file */
