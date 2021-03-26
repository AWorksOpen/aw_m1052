/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief shell操作接口
 *
 * \internal
 * \par modification history
 * - 17-06-19  vih, 增加命令集；增加tab补全;
 * - 16-01-07  deo, remove aw_fs_state_t
 * - 15-12-03  dcf, 将函数指针io_printf标志为删除状态, 增加aw_shell_printf(),
 *             aw_shell_putc/s, aw_shell_getc
 * - 15-04-28  rnk, support multiuser
 * - 14-01-15  orz, first implementation.
 * \endinternal
 */

#ifndef __AW_SHELL_H
#define __AW_SHELL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_shell
 * \copydoc aw_shell.h
 * @{
 */

#include "aworks.h"
#include "aw_sem.h"


/** \brief aw_shell_user结构体的类型定义 */
typedef struct aw_shell_user  aw_shell_user_t;

/** \brief shell命令的函数原型 */
typedef int (*aw_shellcmd_t)(int argc, char *argv[], struct aw_shell_user *);

/**
 * \brief shell字体颜色
 */
#define AW_SHELL_FCOLOR_DEFAULT  "\x1B[0m"        /**< \brief 默认颜色 */
#define AW_SHELL_FCOLOR_BLACK    "\x1B[30m"       /**< \brief 黑色 */
#define AW_SHELL_FCOLOR_RED      "\x1B[31m"       /**< \brief 红色 */
#define AW_SHELL_FCOLOR_GREEN    "\x1B[32m"       /**< \brief 绿色 */
#define AW_SHELL_FCOLOR_YELLOW   "\x1B[33m"       /**< \brief 黄色 */
#define AW_SHELL_FCOLOR_BLUE     "\x1B[34m"       /**< \brief 蓝色 */
#define AW_SHELL_FCOLOR_MAGENTA  "\x1B[35m"       /**< \brief 洋红色 */
#define AW_SHELL_FCOLOR_CYAN     "\x1B[36m"       /**< \brief 蓝绿色 */
#define AW_SHELL_FCOLOR_WHITE    "\x1B[37m"       /**< \brief 白色 */

#define AW_SHELL_FCOLOR_LGREEN   "\x1B[38;5;36m"  /**< \brief 淡绿色 */
#define AW_SHELL_FCOLOR_LBLUE    "\x1B[38;5;33m"  /**< \brief 淡蓝色 */
#define AW_SHELL_FCOLOR_LCYAN    "\x1B[38;5;45m"  /**< \brief 淡蓝绿色 */
#define AW_SHELL_FCOLOR_HCYAN    "\x1B[38;5;123m" /**< \brief 深蓝绿色 */
#define AW_SHELL_FCOLOR_LYELLOW  "\x1B[38;5;222m" /**< \brief 淡黄色 */
#define AW_SHELL_FCOLOR_HYELLOW  "\x1B[38;5;229m" /**< \brief 深黄色 */
#define AW_SHELL_FCOLOR_ORANGE   "\x1B[38;5;208m" /**< \brief 橙色 */
#define AW_SHELL_FCOLOR_GRAY     "\x1B[38;5;245m" /**< \brief 灰色 */
#define AW_SHELL_FCOLOR_PINK     "\x1B[38;5;211m" /**< \brief 粉红色 */

/**
 * \brief shell的选项
 */
typedef struct aw_shell_opt {

    aw_bool_t       use_escape_seq;        /**< \brief 使用转义序列 */
    aw_bool_t       use_history;           /**< \brief 使用历史功能 */
    aw_bool_t       use_color;             /**< \brief 使用颜色 */
    aw_bool_t       use_complete;          /**< \brief 使用补全 */
    aw_bool_t       use_cursor;            /**< \brief 使用光标 */
    aw_bool_t       use_cmdset;            /**< \brief 使用命令集 */
    aw_bool_t       use_chart;             /**< \brief 使用表格 */

    char        *p_endchar;       /**< \brief 命令结束符，默认为 "\r" 或 "\n" */
    uint8_t      endchar_len;     /**< \brief 命令结束符的长度 */

    char        *p_desc;          /**< \brief shell的指示符 */
    char        *p_desc_color;    /**< \brief shell指示符颜色 */

    char        *p_his_buf;       /**< \brief 保存历史的buf */
    uint32_t     his_buf_size;    /**< \brief 历史buf的大小 */

} aw_shell_opt_t;

/**
 * \brief shell的命令结构体
 */
typedef struct aw_shell_cmd {

    /** \brief 命令入口  */
    int       (*entry) (int argc, char *argv[], struct aw_shell_user *);
    const char *name;                   /**< \brief 命令名 */
    const char *desc;                   /**< \brief 命令描述 */

}aw_shell_cmd_item_t;

/**
 * \brief shell命令链表
 */
typedef struct aw_shell_cmd_list {
    struct aw_shell_cmd_list  *next;    /**< \brief 下一条命令链表 */
    const struct aw_shell_cmd *start;   /**< \brief 当前命令数组的起始地址 */
    const struct aw_shell_cmd *end;     /**< \brief 当前命令数组的结尾地址 */
}aw_shell_cmd_list_t;


/**
 * \brief 自定义的补全字符串
 */
struct aw_shell_comp {
    const char **start;                 /**< \brief 第一条字符串的首地址 */
    const char **end;                   /**< \brief 最后一条字符串的结束地址 */
};

/**
 * \brief shell 命令集
 */
struct aw_shell_cmdset {
    struct aw_shell_cmdset    *next;   /**< \brief 下一个命令集指针 */
    struct aw_shell_cmd_list   cmds;   /**< \brief 该命令集里的命令链 */
    struct aw_shell_comp       comp;   /**< \brief 该命令集的自定义补全字符串 */

    /** \brief 该命令集的入口名 */
    const char                *p_enter_name;

    /** \brief 该命令集的退出的字符串；如果为NULL，则默认ctrl+c退出 */
    const char                *p_exit_name;

    /** \brief 该命令集的描述 */
    const char                *p_man;

    /** \brief 在该命令集下会一直运行的函数指针 */
    void                     (*pfn_always_run) (struct aw_shell_user *user);

    /** \brief 进入该命令集时调用的初始化函数 */
    void                     (*pfn_entry_init) (struct aw_shell_user *user);

    /** \brief 加入初始化信号量，只在第一次进入当前命令集时才初始化一次  */
    AW_SEMB_DECL(              init_sem);
};


/**
 * \brief 注册shell命令
 *
 * \param[in]       list        链表地址
 * \param[in]       start       命令的首地址
 * \param[in]       end         命令的尾地址
 *
 * \return 成功返回AW_OK，否则返回值aw_err_t的错误类型
 */
aw_err_t aw_shell_register_cmds (struct aw_shell_cmd_list  *list,
                                 const struct aw_shell_cmd *start,
                                 const struct aw_shell_cmd *end);

/** \brief 命令的注册  */
#define AW_SHELL_REGISTER_CMDS(p_list, cmds) \
    aw_shell_register_cmds(p_list, &cmds[0], &cmds[AW_NELEMENTS(cmds)])

/**
 * \brief 删除注册了的shell命令
 *
 * \param[in]       list        链表地址
 *
 * \return 成功返回AW_OK，否则返回值aw_err_t的错误类型
 */
aw_err_t aw_shell_unregister_cmds (struct aw_shell_cmd_list *list);

/** \brief 命令集的注册  */
#define AW_SHELL_REGISTER_CMDSET(p_set, cmds) \
        aw_shell_register_cmdset(p_set, &cmds[0], &cmds[AW_NELEMENTS(cmds)])

/**
 * \brief 注册命令集
 *
 * \param[in]       list        命令集的地址
 * \param[in]       start       命令的首地址
 * \param[in]       end         命令的尾地址
 *
 * \return 成功返回AW_OK，否则返回值aw_err_t的错误类型
 */
aw_err_t aw_shell_register_cmdset (struct aw_shell_cmdset    *list,
                                   const struct aw_shell_cmd *start,
                                   const struct aw_shell_cmd *end);

/**
 * \brief 删除注册了的shell命令集
 *
 * \param[in]       list        链表地址
 *
 * \return 成功返回AW_OK，否则返回值aw_err_t的错误类型
 */
aw_err_t aw_shell_unregister_cmdset (struct aw_shell_cmdset *list);

/**
 * \brief 添加自定义补全字符串
 *
 * \param[in]       cmdset_name 命令集的名字，如果为NULL则为系统命令的字符串
 * \param[in]       start       字符串的首地址
 * \param[in]       nelements   字符串的数量
 *
 * \return 成功返回AW_OK，否则返回值aw_err_t的错误类型
 */
aw_err_t aw_shell_comp_add (const char  *cmdset_name,
                            const char **start,
                            uint8_t      nelements);

/** \brief 添加补全字符串  */
#define AW_SHELL_COMP_ADD(p_set_name, p_comp) \
        aw_shell_comp_add(p_set_name, p_comp, AW_NELEMENTS(p_comp))


/**
 * \brief 选项和参数的获取
 *
 * \param[in]       user        aw_shell_user结构体地址
 * \param[in]       opt         要获取的选项
 * \param[in]       args        参数的数量
 *
 * \return  成功返回AW_OK，否则返回值aw_err_t的错误类型
 *
 * \attention 参数的第一个字符如果为'-'则要加'\'，否则会被识别为是选项opt；
 *
 * \note      可以用aw_shell_dec_arg 和 aw_shell_str_arg 函数直接返回需要的参数
 */
aw_err_t aw_shell_args_get (struct aw_shell_user *user,
                            const char           *opt,
                            int                   args);

/**
 * \brief 获取指定的有符号数值参数
 *
 * \param[in]       user    aw_shell_user结构体地址
 * \param[in]       index   要获取的参数，有opt则从1开始(0是opt)
 *
 * \return  返回整型参数
 *
 * \note    调用该函数前应该先调用aw_shell_args_get
 * \note    支持8/10/16进制自动识别，例如：020/16/0x10/0X10
 */
long int aw_shell_long_arg (struct aw_shell_user *user, int index);

/**
 * \brief 获取指定的无符号数值参数
 *
 * \param[in]       user    aw_shell_user结构体地址
 * \param[in]       index   要获取的参数，有opt则从1开始(0是opt)
 *
 * \return  返回整型参数
 *
 * \note    调用该函数前应该先调用aw_shell_args_get
 * \note    支持8/10/16进制自动识别，例如：020/16/0x10/0X10
 */
unsigned long int aw_shell_ulong_arg (struct aw_shell_user *user, int index);

/**
 * \brief 获取指定的字符串参数
 *
 * \param[in]       user    aw_shell_user结构体地址
 * \param[in]       index   要获取的参数，有opt则从1开始(0是opt)
 *
 * \return  返回字符串参数
 *
 * \note    调用该函数前应该先调用aw_shell_args_get
 */
char *aw_shell_str_arg (struct aw_shell_user *user, int index);

/**
 * \brief   调用shell命令，只能调用1级命令
 *          目前aw_shell_system默认调用的shell实例为串口实例
 *
 * \param[in]   cmd_name_arg    系统命令字符串
 *
 * \return 成功返回AW_OK，否则返回aw_err_t的错误类型
 */
aw_err_t aw_shell_system (const char *cmd_name_arg);

/**
 * \brief   调用shell命令，可以调用1级命令和命令集命令
 *          目前aw_shell_system默认调用的shell实例为串口实例
 *
 * \param[in]   p_user          user指针
 * \param[in]   cmdset_name     命令集名字，为NULL则调用1级命令
 * \param[in]   cmd_name_arg    系统命令字符串
 *
 * \return 成功返回AW_OK，否则返回aw_err_t的错误类型
 */
aw_err_t aw_shell_system_ex (struct aw_shell_user *p_user,
                             const char           *cmdset_name,
                             const char           *cmd_name_arg);

/**
 * \brief   使能shell的颜色
 *
 * \param[in]  p_user    aw_shell_user结构体地址
 */
void aw_shell_color_enable (aw_shell_user_t *p_user);

/**
 * \brief   失能shell的颜色
 *
 * \param[in]  p_user    aw_shell_user结构体地址
 */
void aw_shell_color_disable (aw_shell_user_t *p_user);

/**
 * \brief   设置shell的字体颜色
 *
 * \param[in]   p_user     aw_shell_user结构体地址
 * \param[in]   p_color    要设置的颜色宏
 *
 * \return  成功返回AW_OK，否则返回aw_err_t的错误类型
 */
aw_err_t aw_shell_color_set (aw_shell_user_t *p_user, const char *p_color);

/**
 * \brief   shell打印
 *
 * \param[in]  sh    aw_shell_user结构体地址
 * \param[in]  fmt   格式化输入
 *
 * \return  返回输出的字符长度
 */
int aw_shell_printf (aw_shell_user_t *sh, const char *fmt, ...);

/**
 * \brief shell输出一个字符
 *
 * \param[in]  sh    aw_shell_user结构体地址
 * \param[in]  c     字符
 *
 * \return  成功返回输出的字符长度，出错为负值错误代码
 */
int aw_shell_putc (aw_shell_user_t *sh, int c);

/**
 * \brief shell输出一条字符串
 *
 * \param[in]  sh    aw_shell_user结构体地址
 * \param[in]  str   字符串
 *
 * \return  成功返回输出的字符长度，出错为负值错误代码
 */
int aw_shell_puts (aw_shell_user_t *sh, const char *str);

/**
 * \brief 从shell获取一个字符，如果没有用户输入，则会等待用户输入
 *
 * \param[in]  sh    aw_shell_user结构体地址
 *
 * \return  返回值大于0，则是有效字符，小于0则无效
 */
int aw_shell_getc (aw_shell_user_t *sh);

/**
 * \brief 从shell获取一个字符，如果没用用户输入，则立即返回
 *
 * \param[in]  sh    aw_shell_user结构体地址
 *
 * \return  返回值大于0，则是有效字符，小于0则无效
 */
int aw_shell_try_getc (aw_shell_user_t *sh);


/**
 * \brief 从shell中读取一行，未接收到回车符时会阻塞
 *
 * \param[in] sh        shell在用户任务中的上下文
 * \param[in] s         传入字符缓冲区
 * \param[in] count     传入字符缓冲区的长度
 *
 * \return          成功返回读取到的字节数
 */
int aw_shell_gets (struct aw_shell_user *sh, char *s, int count);


/**
 * \brief 获取调试shell的实例指针
 *
 * \return 返回shell的指针
 */
aw_shell_user_t *aw_default_shell_get(void);
#define aw_dbg_shell_user aw_default_shell_get

/**
 * \brief 设置调试shell
 *
 * \return 返回旧的shell
 */
aw_shell_user_t *aw_default_shell_set(aw_shell_user_t *sh);

/** \brief 调试shell的实例指针 */
#define AW_DBG_SHELL_IO aw_default_shell_get()

/** @} grp_aw_if_shell */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* __AW_SHELL_H */

/* end of file */
