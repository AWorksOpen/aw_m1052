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
 * \brief AWorks平台串口shell演示例程
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - 调试串口使能宏
 *      - AW_COM_SHELL_SERIAL
 *      - AW_DEV_GPIO_LED
 *   2. 禁能其它地方对LED的操作
 *   3. 打开shell调试终端，设置波特率为115200-8-N-1
 *
 * - 实验现象：
 *   1. 终端执行aworks命令，打印“Welcome to AWorks Platform!”
 *   2. 用户通过shell终端，键入“led_on LED_RUN”、“led_off LED_RUN”、
 *      “led_toggle LED_RUN”命令，开发板LED状态将按键入命令改变状态
 *   3. 用户通过shell终端，键入“.demo”，会进入demo命令集，此时按下tab会看到该命令集下的
 *      命令分别有"func1"、“func2”、“func3”
 *   4. 用户键入“func1”或“func2”，则会运行相应的命令
 *   5. 用户键入“func3 -w 123 abc”，则shell终端会打印出“demo write 123 abc”；
 *      用户键入“func3 -w 123 abc -d 456 edf -r 789”，此时shell终端会
 *      依次打印出“demo write 123 abc”，“demo delete 456 edf”，“demo read 789”
 *
 *
 * \par 源代码
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
 * \brief shell打开LED命令入口
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
 * \brief shell关闭LED命令入口
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
 * \brief shell翻转LED命令入口
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
 * \brief shell打印aworks欢迎命令
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
 * \brief shell led控制命令表
 */
static const aw_shell_cmd_item_t __g_my_led_cmds[] = {
    {__shell_led_on,     "led_on",     "<LED_Name:LED_RUN/LED_ERR>"}, /* 打开指定LED */
    {__shell_led_off,    "led_off",    "<LED_Name:LED_RUN/LED_ERR>"}, /* 关闭指定LED */
    {__shell_led_toggle, "led_toggle", "<LED_Name:LED_RUN/LED_ERR>"}  /* 翻转指定LED */
};

/**
 * \brief shell aworks命令
 */
static const aw_shell_cmd_item_t __g_my_aworks_cmd[] = {
    {__shell_welcome,    "aworks",     "AWorks Platform"}
};

/******************************************************************************
    命令集注册
******************************************************************************/
/**
 * \brief shell demo命令集的func1命令入口
 */
aw_local int __demo_func1 (int                      argc,
                           char                   **argv,
                           struct aw_shell_user    *sh )
{
    /*
     * todo
     * 可在此处添加用户代码
     */
    aw_shell_printf(sh, "run _demo_func1.\n");

    return AW_OK;
}

/**
 * \brief shell demo命令集的func2命令入口
 */
aw_local int __demo_func2 (int                      argc,
                           char                   **argv,
                           struct aw_shell_user    *sh )
{
    /*
     * todo
     * 可在此处添加用户代码
     */
    aw_shell_printf(sh, "run _demo_func2.\n");

    return AW_OK;
}

/**
 * \brief shell demo命令集的func3命令入口
 */
aw_local int __demo_func3 (int                      argc,
                           char                   **argv,
                           struct aw_shell_user    *sh )
{
    while (1) {

        /* 如果参数形式是“-w [arg1] [arg2]”的形式 */
        if (aw_shell_args_get(sh, "-w", 2) == AW_OK) {

            aw_shell_printf(sh,
                            "demo write %d %s.\n",
                            aw_shell_long_arg(sh, 1),
                            aw_shell_str_arg(sh, 2));

            //todo

        /* 如果参数形式是“-r [arg1] ”的形式 */
        } else if (aw_shell_args_get(sh, "-r", 1) == AW_OK) {

            aw_shell_printf(sh, "demo read %d.\n", aw_shell_long_arg(sh, 1));

            //todo

        /* 如果参数形式是“-d [arg1] [arg2]”的形式 */
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
 * \brief shell demo命令集对象
 */
static struct aw_shell_cmdset __g_demo_cmdset = {
    .p_enter_name   = ".demo",         /* 命令集进入名字 */
    .p_exit_name    = ".exit",         /* 命令集退出  */
    .p_man          = "enter demo demo commands",
    .pfn_always_run = NULL,
    .pfn_entry_init = NULL,
};

/**
 * \brief shell demo命令集包含的命令表
 */
static const struct aw_shell_cmd __g_demo_cmdset_cmds[] = {
    {__demo_func1, "func1",      "run func1 "},
    {__demo_func2, "func2",      "run func2"},
    {__demo_func3, "func3",      "run func3 "},
};

/**
 * \brief shell demo命令集的补全字符串
 */
static const char *_g_demo_str[] = {
    "/dev/mtd0",
    "/sdemo",
};

/**
 * \brief shell任务入口
 * \param[in] p_arg : 任务参数
 * \return 无
 */
static void* __task_entry (void *p_arg)
{
    static aw_shell_cmd_list_t cmd_list;
    static aw_shell_cmd_list_t aworks_cmd_list;
    uint8_t                    cnt = 0;

    /* 添加自定义shell命令 */
    AW_SHELL_REGISTER_CMDS(&cmd_list, __g_my_led_cmds);
    AW_SHELL_REGISTER_CMDS(&aworks_cmd_list, __g_my_aworks_cmd);

    /* 添加自定义的shell命令集 */
    AW_SHELL_REGISTER_CMDSET(&__g_demo_cmdset, __g_demo_cmdset_cmds);

    /* 给__g_demo_cmdset命令集添加补全字符串 */
    AW_SHELL_COMP_ADD(".demo", _g_demo_str);

    aw_shell_system("aworks");  /* 执行aworks命令 */

    AW_FOREVER {
        aw_mdelay(5000);

        /* 30s后 移除aworks命令，终端不再打印 */
        if (cnt++ == 6) {
            aw_shell_unregister_cmds(&aworks_cmd_list);     /* 移除命令 */
            aw_shell_unregister_cmdset(&__g_demo_cmdset);   /* 移除命令集 */
            aw_shell_printf(AW_DBG_SHELL_IO, "unregister aworks cmd\r\n");
            break;
        }
    }

    return 0;
}

/**
 * \brief 任务
 * \return 无
 */
void demo_shell_entry (void)
{
    AW_TASK_DECL_STATIC(task_shell_demo, 2048);

    /* 初始化任务 */
    AW_TASK_INIT(task_shell_demo,       /* 任务实体 */
                 "task_shell_demo",     /* 任务名字 */
                  6,                    /* 任务优先级 */
                  2048,                 /* 任务堆栈大小 */
                  __task_entry,         /* 任务入口函数 */
                  (void *)NULL);        /* 任务入口参数 */

    /* 启动任务task_shell_demo */
    AW_TASK_STARTUP(task_shell_demo);
}

/** [src_shell] */

/* end of file */
