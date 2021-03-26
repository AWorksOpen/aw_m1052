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
 * \brief AWorksƽ̨����shell��ʾ����
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - ���Դ���ʹ�ܺ�
 *      - AW_COM_SHELL_SERIAL
 *      - AW_DEV_GPIO_LED
 *   2. ���������ط���LED�Ĳ���
 *   3. ��shell�����նˣ����ò�����Ϊ115200-8-N-1
 *
 * - ʵ������
 *   1. �ն�ִ��aworks�����ӡ��Welcome to AWorks Platform!��
 *   2. �û�ͨ��shell�նˣ����롰led_on LED_RUN������led_off LED_RUN����
 *      ��led_toggle LED_RUN�����������LED״̬������������ı�״̬
 *   3. �û�ͨ��shell�նˣ����롰.demo���������demo�������ʱ����tab�ῴ��������µ�
 *      ����ֱ���"func1"����func2������func3��
 *   4. �û����롰func1����func2�������������Ӧ������
 *   5. �û����롰func3 -w 123 abc������shell�ն˻��ӡ����demo write 123 abc����
 *      �û����롰func3 -w 123 abc -d 456 edf -r 789������ʱshell�ն˻�
 *      ���δ�ӡ����demo write 123 abc������demo delete 456 edf������demo read 789��
 *
 *
 * \par Դ����
 * \snippet demo_shell.c src_shell
 *
 * \internal
 * \par Modification history
 * - 1.00 16-03-26  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_shell Shell
 * \copydoc demo_shell.c
 */

/** [src_shell] */
#include "aworks.h"
#include "aw_assert.h"
#include "aw_delay.h"
#include "aw_task.h"
#include "aw_shell.h"
#include "aw_led.h"
#include "string.h"

/**
 * \brief shell��LED�������
 */
int __shell_led_on (int                   argc,
                    char                 *argv[],
                    struct aw_shell_user *p_shell_user)
{
    uint8_t led_num = 0;

    if (argc < 1) {
        return -AW_EINVAL;
    } else if (argc > 1) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        return -AW_E2BIG;
    }

    if (!strcmp(argv[0], "LED_ERR")) {
        led_num = 1;
    } else if (!strcmp(argv[0], "LED_RUN")) {
        led_num = 0;
    } else {
        aw_shell_printf(p_shell_user, "Unknow LED Name\r\n");
        return -AW_EINVAL;
    }

    aw_led_on(led_num);

    return AW_OK;
}

/**
 * \brief shell�ر�LED�������
 */
int __shell_led_off (int                   argc,
                     char                 *argv[],
                     struct aw_shell_user *p_shell_user)
{
    uint8_t led_num = 0;

    if (argc < 1) {
        return -AW_EINVAL;
    } else if (argc > 1) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        return -AW_E2BIG;
    }

    if (!strcmp(argv[0], "LED_ERR")) {
        led_num = 1;
    } else if (!strcmp(argv[0], "LED_RUN")) {
        led_num = 0;
    } else {
        aw_shell_printf(p_shell_user, "Unknow LED Name\r\n");
        return -AW_EINVAL;
    }

    aw_led_off(led_num);

    return AW_OK;
}

/**
 * \brief shell��תLED�������
 */
int __shell_led_toggle (int                   argc,
                        char                 *argv[],
                        struct aw_shell_user *p_shell_user)
{
    uint8_t led_num = 0;

    if (argc < 1) {
        return -AW_EINVAL;
    } else if (argc > 1) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        return -AW_E2BIG;
    }

    if (!strcmp(argv[0], "LED_ERR")) {
        led_num = 1;
    } else if (!strcmp(argv[0], "LED_RUN")) {
        led_num = 0;
    } else {
        aw_shell_printf(p_shell_user, "Unknow LED Name\r\n");
        return -AW_EINVAL;
    }

    aw_led_toggle(led_num);

    return AW_OK;
}

/**
 * \brief shell��ӡaworks��ӭ����
 */
int __shell_welcome (int                   argc,
                     char                 *argv[],
                     struct aw_shell_user *p_shell_user)
{
    if (argc == 0) {
        aw_shell_printf(p_shell_user, "Welcome to AWorks Platform!\r\n");
    } else {
        aw_shell_printf(p_shell_user, "Unknow Arg!\r\n");
    }

    return AW_OK;
}


/**
 * \brief shell led���������
 */
static const aw_shell_cmd_item_t __g_my_led_cmds[] = {
    {__shell_led_on,     "led_on",     "<LED_Name:LED_RUN/LED_ERR>"}, /* ��ָ��LED */
    {__shell_led_off,    "led_off",    "<LED_Name:LED_RUN/LED_ERR>"}, /* �ر�ָ��LED */
    {__shell_led_toggle, "led_toggle", "<LED_Name:LED_RUN/LED_ERR>"}  /* ��תָ��LED */
};

/**
 * \brief shell aworks����
 */
static const aw_shell_cmd_item_t __g_my_aworks_cmd[] = {
    {__shell_welcome,    "aworks",     "AWorks Platform"}
};

/******************************************************************************
    ���ע��
******************************************************************************/
/**
 * \brief shell demo�����func1�������
 */
aw_local int __demo_func1 (int                      argc,
                           char                   **argv,
                           struct aw_shell_user    *sh )
{
    /*
     * todo
     * ���ڴ˴�����û�����
     */
    aw_shell_printf(sh, "run _demo_func1.\n");

    return AW_OK;
}

/**
 * \brief shell demo�����func2�������
 */
aw_local int __demo_func2 (int                      argc,
                           char                   **argv,
                           struct aw_shell_user    *sh )
{
    /*
     * todo
     * ���ڴ˴�����û�����
     */
    aw_shell_printf(sh, "run _demo_func2.\n");

    return AW_OK;
}

/**
 * \brief shell demo�����func3�������
 */
aw_local int __demo_func3 (int                      argc,
                           char                   **argv,
                           struct aw_shell_user    *sh )
{
    while (1) {

        /* ���������ʽ�ǡ�-w [arg1] [arg2]������ʽ */
        if (aw_shell_args_get(sh, "-w", 2) == AW_OK) {

            aw_shell_printf(sh,
                            "demo write %d %s.\n",
                            aw_shell_long_arg(sh, 1),
                            aw_shell_str_arg(sh, 2));

            //todo

        /* ���������ʽ�ǡ�-r [arg1] ������ʽ */
        } else if (aw_shell_args_get(sh, "-r", 1) == AW_OK) {

            aw_shell_printf(sh, "demo read %d.\n", aw_shell_long_arg(sh, 1));

            //todo

        /* ���������ʽ�ǡ�-d [arg1] [arg2]������ʽ */
        } else if (aw_shell_args_get(sh, "-d", 2) == AW_OK) {

            aw_shell_printf(sh,
                            "demo delete %d %s.\n",
                            aw_shell_long_arg(sh, 1),
                            aw_shell_str_arg(sh, 2));

            //todo
        } else {
            break;
        }
    }

    return AW_OK;
}

/**
 * \brief shell demo�������
 */
static struct aw_shell_cmdset __g_demo_cmdset = {
    .p_enter_name   = ".demo",         /* ����������� */
    .p_exit_name    = ".exit",         /* ����˳�  */
    .p_man          = "enter demo demo commands",
    .pfn_always_run = NULL,
    .pfn_entry_init = NULL,
};

/**
 * \brief shell demo��������������
 */
static const struct aw_shell_cmd __g_demo_cmdset_cmds[] = {
    {__demo_func1, "func1",      "run func1 "},
    {__demo_func2, "func2",      "run func2"},
    {__demo_func3, "func3",      "run func3 "},
};

/**
 * \brief shell demo����Ĳ�ȫ�ַ���
 */
static const char *_g_demo_str[] = {
    "/dev/mtd0",
    "/sdemo",
};

/**
 * \brief shell�������
 * \param[in] p_arg : �������
 * \return ��
 */
static void* __task_entry (void *p_arg)
{
    static aw_shell_cmd_list_t cmd_list;
    static aw_shell_cmd_list_t aworks_cmd_list;
    uint8_t                    cnt = 0;

    /* ����Զ���shell���� */
    AW_SHELL_REGISTER_CMDS(&cmd_list, __g_my_led_cmds);
    AW_SHELL_REGISTER_CMDS(&aworks_cmd_list, __g_my_aworks_cmd);

    /* ����Զ����shell��� */
    AW_SHELL_REGISTER_CMDSET(&__g_demo_cmdset, __g_demo_cmdset_cmds);

    /* ��__g_demo_cmdset�����Ӳ�ȫ�ַ��� */
    AW_SHELL_COMP_ADD(".demo", _g_demo_str);

    aw_shell_system("aworks");  /* ִ��aworks���� */

    AW_FOREVER {
        aw_mdelay(5000);

        /* 30s�� �Ƴ�aworks����ն˲��ٴ�ӡ */
        if (cnt++ == 6) {
            aw_shell_unregister_cmds(&aworks_cmd_list);     /* �Ƴ����� */
            aw_shell_unregister_cmdset(&__g_demo_cmdset);   /* �Ƴ���� */
            aw_shell_printf(AW_DBG_SHELL_IO, "unregister aworks cmd\r\n");
            break;
        }
    }

    return 0;
}

/**
 * \brief ����
 * \return ��
 */
void demo_shell_entry (void)
{
    AW_TASK_DECL_STATIC(task_shell_demo, 2048);

    /* ��ʼ������ */
    AW_TASK_INIT(task_shell_demo,       /* ����ʵ�� */
                 "task_shell_demo",     /* �������� */
                  6,                    /* �������ȼ� */
                  2048,                 /* �����ջ��С */
                  __task_entry,         /* ������ں��� */
                  (void *)NULL);        /* ������ڲ��� */

    /* ��������task_shell_demo */
    AW_TASK_STARTUP(task_shell_demo);
}

/** [src_shell] */

/* end of file */
