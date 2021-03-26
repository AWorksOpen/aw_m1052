#include "aw_task.h"
#include "aworks.h"
#include "aw_mem.h"
#include "aw_shell.h"
#include "aw_delay.h"
#include "aw_slab.h"
#include "aw_vdebug.h"

extern void mpy_main(const char *filename);

/**
 * \brief shell��ӡ����pythonԴ�����
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
 * \brief shell python����
 */
static aw_shell_cmd_item_t __g_my_micropython_cmd[1] = {
    {__shell_python,    "python",     " MicroPython: `python [file.py]` execute python script"}
};

void aw_python_shell_init(void){

    /* ע��pyton����  */
    static aw_shell_cmd_list_t micropython_cmd_list;
    static const char *help_str = "python";
    AW_SHELL_REGISTER_CMDS(&micropython_cmd_list, __g_my_micropython_cmd);
    AW_SHELL_COMP_ADD("python", &help_str);

    return ;
}

/**
 * \brief �������
 * \param[in] p_arg : �������
 * \return ��
 */
static void __python_task_entry (void *p_arg)
{
    aw_python_shell_init();

    AW_FOREVER{
        aw_mdelay(500);
    }
}

void aw_python_task_init(void){
    /* ��������ʵ�� task0 */
    AW_TASK_DECL_STATIC(task, 8192);

    AW_TASK_INIT(task,               /* ����ʵ�� */
                "python",            /* �������� */
                 2,                  /* �������ȼ�  */
                 1024 * 10,               /* �����ջ��С */
                 __python_task_entry,/* ������ں��� */
                (void*)0);           /* ������ڲ��� */

    AW_TASK_STARTUP(task);           /* ��������  */
}
