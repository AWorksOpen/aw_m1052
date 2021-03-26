/*******************************************************************************
*                                 AWorks
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
 * \brief AWorks �����������̣���̬���񴴽���ɾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_DEV_GPIO_LED
 *      - AW_COM_CONSOLE
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ��������ʹ�õ�LED�Ƶĵط�
 *
 * - ʵ������
 *   1.LED_RUN��500ms��ʱ��������.
 *   2.���ڴ�ӡ��ǰִ��������
 *   3.����2���ڴ�ӡ20�κ�����ɾ�������ٴ�ӡ��
 *
 * \par Դ����
 * \snippet demo_task_static.c src_task_static
 *
 * \internal
 * \par Modification history
 * - 1.01 17-09-05  anu, modify.
 * - 1.00 16-03-26  cod, first implementation.
 * \endinternal
 */

 /**
 * \addtogroup demo_if_task_static ��̬����
 * \copydoc demo_task_static.c
 */

/** [src_task_static] */
#include "aworks.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_demo_config.h"

#define LED_RUN DM_LED

/*
 * ��������ʵ�壬����ô���������񲻹��ⲿ������C�ļ���ʹ�ã�Ϊ�˱���ģ�������������ͻ��
 * �Լ���������ģ������������������ʹ��AW_TASK_DECL_STATIC�����������������������
 * ��C�ļ���
 *
 * ע���������ģ����Ҫʹ�ã�����벻��STATIC��ʹ��AW_TASK_DECL������������Ҫʹ�õ�
 *     �ط���ʹ�� AW_TASK_IMPORT() ���������ʵ�塣�磺AW_TASK_IMPORT(task2)
 *     ���������ʾ���ֶ��巽����ʵ���У�����ʹ��AW_TASK_DECL_STATIC��������
 */
AW_TASK_DECL_STATIC(task2, 2048);

/**
 * \brief ����2��ں���
 *
 * \param[in] p_arg ע��ʱ�û�������Զ������
 *
 * \return ��
 */
static void task2_entry (void *p_arg)
{
    aw_task_id_t task2_id = aw_task_id_self();  /* ��ȡ��ǰ�����ID  */
    uint8_t      cnt      = 0;

    /* ����ͨ������ʽ����һ����ѭ��������ѭ�� */
    AW_FOREVER {
        aw_kprintf("Current Task's name is %s, Current Count is %d\r\n",
                    aw_task_name_get(task2_id), ++cnt);

        aw_mdelay(500);

        /* cnt������20�� ��ֹ��ǰ����  */
        if (cnt == 20) {
            aw_kprintf("current task will be terminated\r\n");
            aw_task_exit(task2_id);
        }
    }
}

/**
 * \brief ����1��ں���
 *
 * \param[in] p_arg ע��ʱ�û�������Զ������
 *
 * \return ��
 */
static void task1_entry(void *p_arg)
{
    /* ���弴������������2��ID������ID������񴴽�*/
    aw_task_id_t task2_id;

    aw_kprintf("Get number: %d\n", (int)p_arg);

    /*
     * �����������д�������һ������������ʾ����������2
     */
    task2_id = AW_TASK_INIT(task2,        /* ����ʵ�� */
                            "demo_task2", /* ������ */
                            4,            /* �������ȼ� */
                            2048,         /* �����ջ��С */
                            task2_entry,  /* ������ڲ��� */
                            (void*)2);    /* ���ݸ�����Ĳ��� */

    aw_task_startup (task2_id);   /* ����ʹ������ID����һ������ */
                                  /* ��AW_TASK_STARTUP(task2)Ч����ͬ */

    /*
     * ��������ʱ�������һ��������ã���������2���ȼ�������1���ȼ��ߣ����ȼ���ֵԽС�����ȼ�Խ�ߣ�
     * ��ˣ������󣬻�����ȥִ������2����������2����ʱ����ʱ���Ż��ͷ�CPU���ص�����1����ִ��
     *
     */

    /* ����ͨ������ʽ����һ����ѭ��������ѭ�� */
    AW_FOREVER {

        aw_kprintf("Current Task's name is %s\r\n",
                    aw_task_name_get(aw_task_id_self()));

        aw_led_toggle(LED_RUN);

        /* ������ʱ 500ms����aw_mdelay(500)��ͬ */
        aw_task_delay(aw_sys_clkrate_get() * 500 / 1000);
    }
}

/******************************************************************************/
/**
 * \brief ��̬�����������ں���
 *
 * \param[in] ��
 *
 * \return ��
 */
void demo_task_static_entry (void)
{
    /*
     * ���õĴ�������ķ�ʽ�����ú괴����
     * ʹ�ú��������������������
     *      1����������ʵ��  AW_TASK_DECL_STATIC  ���� AW_TASK_DECL
     *      2����ʼ������    AW_TASK_INIT
     *      3����������      AW_TASK_STARTUP
     */

    /* ��������ʵ�壬�����ڣ����������ڣ����䣬����ʹ��STATIC */
    AW_TASK_DECL_STATIC(task1, 2048);   /* ������������������У���ʹ�ø������� */

    AW_TASK_INIT(task1,        /* ����ʵ��:AW_TASK_DECL_STATIC ���� AW_TASK_DECL ����� */
                "demo_task1",  /* �������� */
                 5,            /* �������ȼ� */
                 2048,         /* �����ջ��С */
                 task1_entry,  /* ������ں��� */
                 (void*)1);    /* ������ڲ��� */

    AW_TASK_STARTUP(task1);    /* ����task1,���ڵ�ǰ�����ȼ���ߣ��������к���task1_entry */
}

/** [src_task_static] */

/* end of file */
