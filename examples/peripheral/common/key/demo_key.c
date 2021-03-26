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
 * \brief ������ʾ����
 *
 * - �������裺
 *   1. ��aw_prj_params.hͷ�ļ���ʹ��
 *      - ���Դ��ڶ�Ӧ�ĺ�
 *      - AW_COM_CONSOLE
 *      - AW_DEV_GPIO_KEY
 *      - AW_COM_INPUT_EV_ABS
 *
 *   2. ʹ�ô����߷ֱ�Ϳ�����ĵ��Դ���DURX��DUTX�������򿪴��ڵ������֣�
 *      ���ò�����Ϊ115200��1��ֹͣλ����У�飬�����ء�
 *
 *   3.��  KEY1 ��Ӧ�Ĺܽ����ӵ��û�������·�������̰��� KEY1 ��ʹ��GPIO���ӿ����û������ļ�
 *    awbl_hwconf_gpio_key.h �����á�
 *
 *   4. ����awbl_hwconf_gpio_key.h�ļ������ð�������Ϊ�͵�ƽ��
 *
 * - ʵ������
 *   1. ���ڴ�ӡ����������/�ɿ���Ϣ���Լ�ģ�ⰴ����ģ��������Ϣ��
 *
 * \par Դ����
 * \snippet demo_key.c src_key
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-25  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_key ������ʾ
 * \copydoc demo_key.c
 */

/** [src_key] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_gpio.h"
#include "aw_input.h"
#include "aw_input_code.h"

/******************************************************************************/
#define SIMULATE_KEY_EVENT       (0xF0) /**< \brief ģ�ⰴ���¼� */
#define SIMULATE_KEY_DOWN        (1)    /**< \brief ģ�ⰴ������ */
#define SIMULATE_KEY_UP          (0)    /**< \brief ģ�ⰴ���ɿ� */

#define SIMULATE_ABS_COORD_EVENT (0xF1) /**< \brief ģ����������¼� */
#define SIMULATE_ABS_COORD_X     (3)    /**< \brief ģ�����X����ֵ */
#define SIMULATE_ABS_COORD_Y     (4)    /**< \brief ģ�����Y����ֵ */
#define SIMULATE_ABS_COORD_Z     (5)    /**< \brief ģ�����Z����ֵ */

/******************************************************************************/
/**
 * \brief �����ص�����
 * \param[in] p_input_data  ����/ָ�������¼�����
 * \param[in] p_usr_data    �û��Զ�������
 */
aw_local void __key_proc_cb (aw_input_event_t *p_input_data, void *p_usr_data)
{
    struct aw_input_key_data *p_key_data =
                             (struct aw_input_key_data *)p_input_data;
    struct aw_input_ptr_data *p_ptr_data =
                             (struct aw_input_ptr_data *)p_input_data;

    if ((p_input_data != NULL) &&
        (AW_INPUT_EV_KEY != p_input_data->ev_type) &&
        (AW_INPUT_EV_ABS != p_input_data->ev_type)) {
        return;
    }

    switch (p_key_data->key_code) {

    case KEY_1 :
        if (p_key_data->key_state) {
            AW_INFOF(("key_proc_cb : KEY1 is down : %d. \n",
                      p_key_data->key_state));
        } else {
            AW_INFOF(("key_proc_cb : KEY1 is release : %d. \n",
                      p_key_data->key_state));
        }
        break;

    case SIMULATE_KEY_EVENT:
        if (p_key_data->key_state) {
            AW_INFOF(("key_proc_cb : SIMULATE_KEY is down : %d. \n",
                      p_key_data->key_state));
        } else {
            AW_INFOF(("key_proc_cb : SIMULATE_KEY is release : %d. \n",
                      p_key_data->key_state));
        }
        break;

    case SIMULATE_ABS_COORD_EVENT:
        AW_INFOF(("abs coordinate: x = %d, y = %d, z = %d\n",
                  p_ptr_data->pos.x,
                  p_ptr_data->pos.y,
                  p_ptr_data->pos.z));
        break;

    default:
        AW_INFOF(("Unknow Key Code!\r\n"));
        break;
    }
}

/******************************************************************************/
/**
 * \brief ���� demo
 * \return ��
 */
aw_local void* __task_handle (void *arg)
{
    int         times   = 0;

    struct aw_input_handler key_hdlr;

    aw_kprintf("\nKey demo testing...\n");

    /* ע��һ��������������������ϵͳ */
    aw_input_handler_register(&key_hdlr, __key_proc_cb, NULL);

    AW_FOREVER {
        aw_kprintf("\nYou can press the key1 ~ key4 for watching the phenomenon!\n");
        aw_mdelay(2000); /* ��ʱ2s */

        /* ģ���ϱ�һ�������¼�-���� */
        aw_input_report_key(AW_INPUT_EV_KEY,
                            SIMULATE_KEY_EVENT,
                            SIMULATE_KEY_DOWN);

        aw_mdelay(2000); /* ��ʱ2s */

        /* ģ���ϱ�һ�������¼�-�ɿ� */
        aw_input_report_key(AW_INPUT_EV_KEY,
                            SIMULATE_KEY_EVENT,
                            SIMULATE_KEY_UP);

        aw_mdelay(2000); /* ��ʱ2s */

        /* ģ���ϱ�һ�����������¼� */
        aw_input_report_coord(AW_INPUT_EV_ABS,
                              SIMULATE_ABS_COORD_EVENT,
                              SIMULATE_ABS_COORD_X,
                              SIMULATE_ABS_COORD_Y,
                              SIMULATE_ABS_COORD_Z);

        times++;

        if (times == 10) {
            break;
        }
    }
    /* ��������ϵͳ��ע������������ */
    aw_input_handler_unregister(&key_hdlr);

    return NULL;
}

/******************************************************************************/
void demo_key_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Key demo",
                         12,
                         1024,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("Key demo task create failed\r\n");
        return;
    }

    aw_task_startup(tsk);
}

/** [src_key] */

/* end of file */
