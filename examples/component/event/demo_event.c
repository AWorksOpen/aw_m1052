/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief �¼�����ʾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ��ڶ�Ӧ�ĺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ���ڴ�ӡ��ص�����Ϣ��
 *
 * \par Դ����
 * \snippet demo_event.c src_event
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-05  anu, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_event �¼�����ʾ����
 * \copydoc demo_event.c
 */

/** [src_event] */

#include "aworks.h"
#include "aw_event.h"
#include "aw_vdebug.h"
#include "aw_delay.h"

static struct event_category   __g_category;        /* �¼����� */
static struct event_type       __g_event_1;         /* �¼�����1 */
static struct event_type       __g_event_2;         /* �¼�����2 */
static struct event_handler    __g_handler_cat;     /* �¼������� */
static struct event_handler    __g_handler_evt_1_1; /* �¼�����1-1��� */
static struct event_handler    __g_handler_evt_2_1; /* �¼�����2-1��� */
static struct event_handler    __g_handler_evt_2_2; /* �¼�����2-2��� */
static struct event_handler    __g_handler_evt_2_3; /* �¼�����2-3��� */
static struct event_handler    __g_handler_evt_2_4; /* �¼�����2-4��� */

/**
 * \brief �¼�������Ļص�����
 *
 * \note �ú������¼���������лص����ú������ӡ�¼����ݣ�����ʱ���ݣ��Լ��û����ݣ�ע��ʱ��¼��
 *
 * \param[in] p_evt_type �¼�����
 * \param[in] p_evt_data �¼�������
 * \param[in] p_usr_data �û�����
 *
 * \return ��
 */
static void __event_function_test (struct event_type *p_evt_type,
                                   void              *p_evt_data,
                                   void              *p_usr_data)
{
    static int count = 0;

    aw_kprintf("#%d: %s raised, handler_%s called\n",
               count++, (const char *)p_evt_data, (const char *)p_usr_data);
}

/**
 * \brief �����¼������ؽӿ�
 *
 * \return ��
 */
void demo_event_entry (void)
{
    aw_event_category_init(&__g_category); /* ��ʼ���¼����� */

    aw_event_init(&__g_event_1);           /* ��ʼ���¼�����1 */
    aw_event_init(&__g_event_2);           /* ��ʼ���¼�����2 */

    /* ��ʼ���¼���� */
    aw_event_handler_init(&__g_handler_cat, __event_function_test,
                          "category", EVENT_HANDLER_FLAG_NONE);
    aw_event_handler_init(&__g_handler_evt_1_1, __event_function_test,
                          "event_1_1", EVENT_HANDLER_FLAG_NONE);
    aw_event_handler_init(&__g_handler_evt_2_1, __event_function_test,
                          "event_2_1", EVENT_HANDLER_FLAG_NONE);
    aw_event_handler_init(&__g_handler_evt_2_2, __event_function_test,
                          "event_2_2", EVENT_HANDLER_FLAG_NONE);
    aw_event_handler_init(&__g_handler_evt_2_3, __event_function_test,
                          "event_2_3", EVENT_HANDLER_FLAG_NONE);
    /* ���¼���������ᱻ�Ƴ� */
    aw_event_handler_init(&__g_handler_evt_2_4, __event_function_test,
                          "event_2_4", EVENT_HANDLER_FLAG_AUTO_UNREG);

    /* ���¼�������ע�ᵽ�¼������� */
    aw_event_category_handler_register(&__g_category, &__g_handler_cat);

    /* ���¼����ע�ᵽ��Ӧ���¼������� */
    aw_event_handler_register(&__g_event_1, &__g_handler_evt_1_1);

    /* ���¼����ע�ᵽ��Ӧ���¼������� */
    aw_event_handler_register(&__g_event_2, &__g_handler_evt_2_1);
    aw_event_handler_register(&__g_event_2, &__g_handler_evt_2_2);
    aw_event_handler_register(&__g_event_2, &__g_handler_evt_2_3);
    aw_event_handler_register(&__g_event_2, &__g_handler_evt_2_4);

    /* ���¼�����ע�ᵽ�¼������� */
    aw_event_category_event_register(&__g_category, &__g_event_1);
    aw_event_category_event_register(&__g_category, &__g_event_2);

    aw_kprintf("event SYNC test start...\n");
    aw_event_raise(&__g_event_1, "event_1_sync", EVENT_PROC_FLAG_SYNC);         /* ͬ�������¼� */
    aw_event_raise(&__g_event_1, "event_1_async", EVENT_PROC_FLAG_ASYNC);       /* �첽�����¼� */
    aw_event_raise(&__g_event_1, "event_1_cat_only", EVENT_PROC_FLAG_CAT_ONLY); /* ֻ�����¼����ࣨ�첽�� */

    aw_mdelay(100);

    aw_kprintf("\n---------------\n");
    aw_event_raise(&__g_event_2, "event_2", EVENT_PROC_FLAG_SYNC);

    aw_kprintf("\n--------------- now handler_event_2_4 should be deleted\n");
    aw_event_raise(&__g_event_2, "event_2", EVENT_PROC_FLAG_SYNC);

    aw_kprintf("\n--------------- now handler_event_2_2 should be deleted\n");
    aw_event_handler_unregister(&__g_event_2, &__g_handler_evt_2_2);
    aw_event_raise(&__g_event_2, "event_2", EVENT_PROC_FLAG_SYNC);
}

/** [src_event] */

/* end of file */
