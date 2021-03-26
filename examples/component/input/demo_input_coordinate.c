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
 * \brief AWorks������ϵͳʹ�����̣�ģ���������룩
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ���Դ��ڶ�Ӧ�ĺ�
 *      - AW_COM_CONSOLE
 *      - AW_COM_INPUT_EV
 *      - AW_COM_INPUT_EV_ABS
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ����ѭ����ӡ����ֵ��
 *
 * \par Դ����
 * \snippet demo_input_coordinate.c src_input_coordinate
 *
 * \internal
 * \par Modification history
 * - 1.00 16-04-05  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_input_coordinate ������ϵͳ
 * \copydoc demo_input_coordinate.c
 */

/** [src_input_coordinate] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_input.h"
#include "aw_demo_config.h"
#include "aw_task.h"
#include "aw_delay.h"


static void* __input_task (void *p_arg);
static void __input_callback (aw_input_event_t *p_input_data, void *p_usr_data);


static void __input_callback (aw_input_event_t *p_input_data, void *p_usr_data)
{
    struct aw_input_ptr_data *p_data;

    p_data = (struct aw_input_ptr_data *)p_input_data;

    if (p_data->input_ev.ev_type != AW_INPUT_EV_ABS) {
        aw_kprintf("this is not coordinate input event.\r\n");
    } else {
        aw_kprintf("coordinate(x = %d, y = %d, z = %d)\r\n",
                   p_data->pos.x,
                   p_data->pos.y,
                   p_data->pos.z);
    }

    return ;
}

static void* __input_task (void * p_arg)
{
    aw_input_handler_t input_handler;
    int x, y, z;
    int ret;

    /* ע�ᰴ�������¼���������ϵͳ�� */
    aw_input_handler_register(&input_handler, __input_callback, NULL);

    while (1) {

        /*ģ������������*/
        x = rand() % 100;
        y = rand() % 100;
        z = rand() % 100;

        /*ÿ��һ���ϱ�һ�����������¼�*/
        ret = aw_input_report_coord(AW_INPUT_EV_ABS, ABS_RESERVED, x, y, z);
        if (ret != AW_OK) {
            aw_kprintf("aw_input_report_coord error:%d\r\n", ret);
        }

        aw_mdelay(1000);
    }

    return 0;
}

/******************************************************************************/
/**
 * \brief ������ں���
 * \param[in] p_arg  ������ڲ���
 */
void demo_input_coordinate_entry (void)
{
    aw_kprintf("\ninput testing...\n");

    aw_task_id_t task_id = aw_task_create("input task",
                                          8,
                                          1024,
                                          __input_task,
                                          NULL);
    if (task_id == NULL) {
        aw_kprintf("input task startup fail.\r\n");
    } else {
        aw_kprintf("input task startup successful.\r\n");
        aw_task_startup(task_id);
    }

    return ;
}

/** [src_input_coordinate] */

/* end of file */
