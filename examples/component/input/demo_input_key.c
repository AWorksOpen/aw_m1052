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
 * \brief AWorks������ϵͳʹ�����̣��������룩
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ���Դ��ڶ�Ӧ�ĺ�
 *      - ��Ӧƽ̨��PWM��
 *      - AW_COM_CONSOLE
 *      - AW_COM_INPUT_EV
 *      - AW_COM_INPUT_EV_KEY
 *      - AW_DEV_PWM_BUZZER
 *   2. ��������DE_KEY_IO�ܽ����ӵ��û�������·��
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ����״̬�仯Ϊ1ʱ����������200ms��
 *
 * \par Դ����
 * \snippet demo_input.c src_input
 *
 * \internal
 * \par Modification history
 * - 1.00 16-04-05  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_input_key ������ϵͳ
 * \copydoc demo_input_key.c
 */

/** [src_input_key] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_gpio.h"
#include "aw_input.h"
#include "aw_demo_config.h"
#include "aw_task.h"
#include "aw_defer.h"
#include "aw_delay.h"
#include "aw_buzzer.h"

/******************************************************************************/
#define GPIO_KEY   DE_KEY_IO    /**< \brief GPIO�����ܽź� */
#define DELAY_MS   25           /**< \brief ��ʱʱ��-ms */
/******************************************************************************/
static int KEY_NUM;             /**< \brief ����״̬flag */
/******************************************************************************/

static void* __task_key (void * p_arg);
static void* __task_input (void * p_arg);

/**
 * \brief GPIO ���������ص�����
 * \param[in] p_arg  �ص���������
 */
aw_local void __gpio_key_callback (void *p_arg)
{
    uint32_t key      = (uint32_t)p_arg;

    uint32_t cur_stat = aw_gpio_get(key);

    /* �ϱ������¼� */
    aw_input_report_key(AW_INPUT_EV_KEY, KEY_0, cur_stat);
}

/**
 * \brief ������ϵͳ�ص�����
 * \param[in] p_input_data  �����¼�����
 * \param[in] p_usr_data    �û�����
 */
aw_local void key_proc_cb (struct aw_input_event *p_input_data, void *p_usr_data)
{
    struct aw_input_key_data *p_key_data = NULL;

    if ((p_input_data != NULL) &&
        (p_input_data->ev_type != AW_INPUT_EV_KEY)) {
        return;
    }

    p_key_data = (struct aw_input_key_data *)p_input_data;
    if (p_key_data->key_code == KEY_0) {
        if (!p_key_data->key_state) {
            KEY_NUM = 1;
        }
    }
}

static void* __task_key (void * p_arg)
{
    /* ����GPIO�����ܽ�Ϊ���빦�� */
    int gpio0_gpios[] = {GPIO_KEY};

    if (aw_gpio_pin_request("gpios",
                             gpio0_gpios,
                             AW_NELEMENTS(gpio0_gpios)) == AW_OK) {
        aw_gpio_pin_cfg(GPIO_KEY, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
    }

    /* ���ӻص����������� */
    aw_gpio_trigger_connect(GPIO_KEY, __gpio_key_callback, (void *)GPIO_KEY);

    /* �������Ŵ������ͣ��½��ش��� */
    aw_gpio_trigger_cfg(GPIO_KEY, AW_GPIO_TRIGGER_FALL);

    /* ���������ϵĴ�����  */
    aw_gpio_trigger_on(GPIO_KEY);

    while (1) {
        aw_mdelay(500);
    }

    return 0;
}

static void* __task_input (void * p_arg)
{
    struct aw_input_handler key_input_handler;

    /* ע�ᰴ�������¼���������ϵͳ�� */
    aw_input_handler_register(&key_input_handler, key_proc_cb, NULL);

    while (1) {
        if (KEY_NUM == 1) {
            aw_buzzer_loud_set(80); /* ���÷���������ǿ�� */
            aw_buzzer_beep(200);    /* ��������ʱ200ms */
            KEY_NUM = 0;
        }
    }

    return 0;
}

/******************************************************************************/
/**
 * \brief demo���
 */
void demo_input_key_entry (void)
{
    aw_kprintf("\ninput testing...\n");

    aw_task_id_t task_id1 = aw_task_create ("key_task",
                                           8,
                                           1024,
                                           __task_key,
                                           (void *)NULL);
    if (task_id1 == NULL) {
        aw_kprintf("key task startup fail.\r\n");
        return ;
    } else {
        aw_task_startup(task_id1);
        aw_kprintf("key task startup successful.\r\n");
    }

    aw_task_id_t task_id2 = aw_task_create ("key_task",
                                            7,
                                            1024,
                                            __task_input,
                                            (void *)NULL);
    if (task_id2 == NULL) {
        aw_kprintf("input task startup fail.\r\n");
        return ;
    } else {
        aw_task_startup(task_id2);
        aw_kprintf("input task startup successful.\r\n");
    }

    return ;
}

/** [src_input_key] */

/* end of file */
