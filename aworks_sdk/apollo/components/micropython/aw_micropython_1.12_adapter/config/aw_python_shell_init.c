#include "aw_task.h"
#include "aworks.h"
#include "aw_mem.h"
#include "aw_shell.h"
#include "aw_delay.h"
#include "aw_slab.h"
#include "aw_vdebug.h"

extern void mpy_main(const char *filename);

/**
 * \brief shell打印调用python源码解析
 */
static int __shell_python (int                   argc,
                     char                 *argv[],
                     struct aw_shell_user *p_shell_user)
{
    if (argc > 0) {
        mpy_main(argv[0]);
    } else {
        mpy_main(NULL);
    }
    return AW_OK;
}

/**
 * \brief shell python命令
 */
static aw_shell_cmd_item_t __g_my_micropython_cmd[1] = {
    {__shell_python,    "python",     " MicroPython: `python [file.py]` execute python script"}
};

void aw_python_shell_init(void){

    /* 注册pyton命令  */
    static aw_shell_cmd_list_t micropython_cmd_list;
    static const char *help_str = "python";
    AW_SHELL_REGISTER_CMDS(&micropython_cmd_list, __g_my_micropython_cmd);
    AW_SHELL_COMP_ADD("python", &help_str);

    return ;
}

/**
 * \brief 任务入口
 * \param[in] p_arg : 任务参数
 * \return 无
 */
static void __python_task_entry (void *p_arg)
{
    aw_python_shell_init();

    AW_FOREVER{
        aw_mdelay(500);
    }
}

void aw_python_task_init(void){
    /* 定义任务实体 task0 */
    AW_TASK_DECL_STATIC(task, 8192);

    AW_TASK_INIT(task,               /* 任务实体 */
                "python",            /* 任务名字 */
                 2,                  /* 任务优先级  */
                 1024 * 10,               /* 任务堆栈大小 */
                 __python_task_entry,/* 任务入口函数 */
                (void*)0);           /* 任务入口参数 */

    AW_TASK_STARTUP(task);           /* 启动任务  */
}
