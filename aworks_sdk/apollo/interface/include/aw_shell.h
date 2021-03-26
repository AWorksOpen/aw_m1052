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
 * \brief shell�����ӿ�
 *
 * \internal
 * \par modification history
 * - 17-06-19  vih, �������������tab��ȫ;
 * - 16-01-07  deo, remove aw_fs_state_t
 * - 15-12-03  dcf, ������ָ��io_printf��־Ϊɾ��״̬, ����aw_shell_printf(),
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


/** \brief aw_shell_user�ṹ������Ͷ��� */
typedef struct aw_shell_user  aw_shell_user_t;

/** \brief shell����ĺ���ԭ�� */
typedef int (*aw_shellcmd_t)(int argc, char *argv[], struct aw_shell_user *);

/**
 * \brief shell������ɫ
 */
#define AW_SHELL_FCOLOR_DEFAULT  "\x1B[0m"        /**< \brief Ĭ����ɫ */
#define AW_SHELL_FCOLOR_BLACK    "\x1B[30m"       /**< \brief ��ɫ */
#define AW_SHELL_FCOLOR_RED      "\x1B[31m"       /**< \brief ��ɫ */
#define AW_SHELL_FCOLOR_GREEN    "\x1B[32m"       /**< \brief ��ɫ */
#define AW_SHELL_FCOLOR_YELLOW   "\x1B[33m"       /**< \brief ��ɫ */
#define AW_SHELL_FCOLOR_BLUE     "\x1B[34m"       /**< \brief ��ɫ */
#define AW_SHELL_FCOLOR_MAGENTA  "\x1B[35m"       /**< \brief ���ɫ */
#define AW_SHELL_FCOLOR_CYAN     "\x1B[36m"       /**< \brief ����ɫ */
#define AW_SHELL_FCOLOR_WHITE    "\x1B[37m"       /**< \brief ��ɫ */

#define AW_SHELL_FCOLOR_LGREEN   "\x1B[38;5;36m"  /**< \brief ����ɫ */
#define AW_SHELL_FCOLOR_LBLUE    "\x1B[38;5;33m"  /**< \brief ����ɫ */
#define AW_SHELL_FCOLOR_LCYAN    "\x1B[38;5;45m"  /**< \brief ������ɫ */
#define AW_SHELL_FCOLOR_HCYAN    "\x1B[38;5;123m" /**< \brief ������ɫ */
#define AW_SHELL_FCOLOR_LYELLOW  "\x1B[38;5;222m" /**< \brief ����ɫ */
#define AW_SHELL_FCOLOR_HYELLOW  "\x1B[38;5;229m" /**< \brief ���ɫ */
#define AW_SHELL_FCOLOR_ORANGE   "\x1B[38;5;208m" /**< \brief ��ɫ */
#define AW_SHELL_FCOLOR_GRAY     "\x1B[38;5;245m" /**< \brief ��ɫ */
#define AW_SHELL_FCOLOR_PINK     "\x1B[38;5;211m" /**< \brief �ۺ�ɫ */

/**
 * \brief shell��ѡ��
 */
typedef struct aw_shell_opt {

    aw_bool_t       use_escape_seq;        /**< \brief ʹ��ת������ */
    aw_bool_t       use_history;           /**< \brief ʹ����ʷ���� */
    aw_bool_t       use_color;             /**< \brief ʹ����ɫ */
    aw_bool_t       use_complete;          /**< \brief ʹ�ò�ȫ */
    aw_bool_t       use_cursor;            /**< \brief ʹ�ù�� */
    aw_bool_t       use_cmdset;            /**< \brief ʹ����� */
    aw_bool_t       use_chart;             /**< \brief ʹ�ñ�� */

    char        *p_endchar;       /**< \brief �����������Ĭ��Ϊ "\r" �� "\n" */
    uint8_t      endchar_len;     /**< \brief ����������ĳ��� */

    char        *p_desc;          /**< \brief shell��ָʾ�� */
    char        *p_desc_color;    /**< \brief shellָʾ����ɫ */

    char        *p_his_buf;       /**< \brief ������ʷ��buf */
    uint32_t     his_buf_size;    /**< \brief ��ʷbuf�Ĵ�С */

} aw_shell_opt_t;

/**
 * \brief shell������ṹ��
 */
typedef struct aw_shell_cmd {

    /** \brief �������  */
    int       (*entry) (int argc, char *argv[], struct aw_shell_user *);
    const char *name;                   /**< \brief ������ */
    const char *desc;                   /**< \brief �������� */

}aw_shell_cmd_item_t;

/**
 * \brief shell��������
 */
typedef struct aw_shell_cmd_list {
    struct aw_shell_cmd_list  *next;    /**< \brief ��һ���������� */
    const struct aw_shell_cmd *start;   /**< \brief ��ǰ�����������ʼ��ַ */
    const struct aw_shell_cmd *end;     /**< \brief ��ǰ��������Ľ�β��ַ */
}aw_shell_cmd_list_t;


/**
 * \brief �Զ���Ĳ�ȫ�ַ���
 */
struct aw_shell_comp {
    const char **start;                 /**< \brief ��һ���ַ������׵�ַ */
    const char **end;                   /**< \brief ���һ���ַ����Ľ�����ַ */
};

/**
 * \brief shell ���
 */
struct aw_shell_cmdset {
    struct aw_shell_cmdset    *next;   /**< \brief ��һ�����ָ�� */
    struct aw_shell_cmd_list   cmds;   /**< \brief �������������� */
    struct aw_shell_comp       comp;   /**< \brief ��������Զ��岹ȫ�ַ��� */

    /** \brief ������������ */
    const char                *p_enter_name;

    /** \brief ��������˳����ַ��������ΪNULL����Ĭ��ctrl+c�˳� */
    const char                *p_exit_name;

    /** \brief ����������� */
    const char                *p_man;

    /** \brief �ڸ�����»�һֱ���еĺ���ָ�� */
    void                     (*pfn_always_run) (struct aw_shell_user *user);

    /** \brief ��������ʱ���õĳ�ʼ������ */
    void                     (*pfn_entry_init) (struct aw_shell_user *user);

    /** \brief �����ʼ���ź�����ֻ�ڵ�һ�ν��뵱ǰ���ʱ�ų�ʼ��һ��  */
    AW_SEMB_DECL(              init_sem);
};


/**
 * \brief ע��shell����
 *
 * \param[in]       list        �����ַ
 * \param[in]       start       ������׵�ַ
 * \param[in]       end         �����β��ַ
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵaw_err_t�Ĵ�������
 */
aw_err_t aw_shell_register_cmds (struct aw_shell_cmd_list  *list,
                                 const struct aw_shell_cmd *start,
                                 const struct aw_shell_cmd *end);

/** \brief �����ע��  */
#define AW_SHELL_REGISTER_CMDS(p_list, cmds) \
    aw_shell_register_cmds(p_list, &cmds[0], &cmds[AW_NELEMENTS(cmds)])

/**
 * \brief ɾ��ע���˵�shell����
 *
 * \param[in]       list        �����ַ
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵaw_err_t�Ĵ�������
 */
aw_err_t aw_shell_unregister_cmds (struct aw_shell_cmd_list *list);

/** \brief �����ע��  */
#define AW_SHELL_REGISTER_CMDSET(p_set, cmds) \
        aw_shell_register_cmdset(p_set, &cmds[0], &cmds[AW_NELEMENTS(cmds)])

/**
 * \brief ע�����
 *
 * \param[in]       list        ����ĵ�ַ
 * \param[in]       start       ������׵�ַ
 * \param[in]       end         �����β��ַ
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵaw_err_t�Ĵ�������
 */
aw_err_t aw_shell_register_cmdset (struct aw_shell_cmdset    *list,
                                   const struct aw_shell_cmd *start,
                                   const struct aw_shell_cmd *end);

/**
 * \brief ɾ��ע���˵�shell���
 *
 * \param[in]       list        �����ַ
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵaw_err_t�Ĵ�������
 */
aw_err_t aw_shell_unregister_cmdset (struct aw_shell_cmdset *list);

/**
 * \brief ����Զ��岹ȫ�ַ���
 *
 * \param[in]       cmdset_name ��������֣����ΪNULL��Ϊϵͳ������ַ���
 * \param[in]       start       �ַ������׵�ַ
 * \param[in]       nelements   �ַ���������
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵaw_err_t�Ĵ�������
 */
aw_err_t aw_shell_comp_add (const char  *cmdset_name,
                            const char **start,
                            uint8_t      nelements);

/** \brief ��Ӳ�ȫ�ַ���  */
#define AW_SHELL_COMP_ADD(p_set_name, p_comp) \
        aw_shell_comp_add(p_set_name, p_comp, AW_NELEMENTS(p_comp))


/**
 * \brief ѡ��Ͳ����Ļ�ȡ
 *
 * \param[in]       user        aw_shell_user�ṹ���ַ
 * \param[in]       opt         Ҫ��ȡ��ѡ��
 * \param[in]       args        ����������
 *
 * \return  �ɹ�����AW_OK�����򷵻�ֵaw_err_t�Ĵ�������
 *
 * \attention �����ĵ�һ���ַ����Ϊ'-'��Ҫ��'\'������ᱻʶ��Ϊ��ѡ��opt��
 *
 * \note      ������aw_shell_dec_arg �� aw_shell_str_arg ����ֱ�ӷ�����Ҫ�Ĳ���
 */
aw_err_t aw_shell_args_get (struct aw_shell_user *user,
                            const char           *opt,
                            int                   args);

/**
 * \brief ��ȡָ�����з�����ֵ����
 *
 * \param[in]       user    aw_shell_user�ṹ���ַ
 * \param[in]       index   Ҫ��ȡ�Ĳ�������opt���1��ʼ(0��opt)
 *
 * \return  �������Ͳ���
 *
 * \note    ���øú���ǰӦ���ȵ���aw_shell_args_get
 * \note    ֧��8/10/16�����Զ�ʶ�����磺020/16/0x10/0X10
 */
long int aw_shell_long_arg (struct aw_shell_user *user, int index);

/**
 * \brief ��ȡָ�����޷�����ֵ����
 *
 * \param[in]       user    aw_shell_user�ṹ���ַ
 * \param[in]       index   Ҫ��ȡ�Ĳ�������opt���1��ʼ(0��opt)
 *
 * \return  �������Ͳ���
 *
 * \note    ���øú���ǰӦ���ȵ���aw_shell_args_get
 * \note    ֧��8/10/16�����Զ�ʶ�����磺020/16/0x10/0X10
 */
unsigned long int aw_shell_ulong_arg (struct aw_shell_user *user, int index);

/**
 * \brief ��ȡָ�����ַ�������
 *
 * \param[in]       user    aw_shell_user�ṹ���ַ
 * \param[in]       index   Ҫ��ȡ�Ĳ�������opt���1��ʼ(0��opt)
 *
 * \return  �����ַ�������
 *
 * \note    ���øú���ǰӦ���ȵ���aw_shell_args_get
 */
char *aw_shell_str_arg (struct aw_shell_user *user, int index);

/**
 * \brief   ����shell���ֻ�ܵ���1������
 *          Ŀǰaw_shell_systemĬ�ϵ��õ�shellʵ��Ϊ����ʵ��
 *
 * \param[in]   cmd_name_arg    ϵͳ�����ַ���
 *
 * \return �ɹ�����AW_OK�����򷵻�aw_err_t�Ĵ�������
 */
aw_err_t aw_shell_system (const char *cmd_name_arg);

/**
 * \brief   ����shell������Ե���1��������������
 *          Ŀǰaw_shell_systemĬ�ϵ��õ�shellʵ��Ϊ����ʵ��
 *
 * \param[in]   p_user          userָ��
 * \param[in]   cmdset_name     ������֣�ΪNULL�����1������
 * \param[in]   cmd_name_arg    ϵͳ�����ַ���
 *
 * \return �ɹ�����AW_OK�����򷵻�aw_err_t�Ĵ�������
 */
aw_err_t aw_shell_system_ex (struct aw_shell_user *p_user,
                             const char           *cmdset_name,
                             const char           *cmd_name_arg);

/**
 * \brief   ʹ��shell����ɫ
 *
 * \param[in]  p_user    aw_shell_user�ṹ���ַ
 */
void aw_shell_color_enable (aw_shell_user_t *p_user);

/**
 * \brief   ʧ��shell����ɫ
 *
 * \param[in]  p_user    aw_shell_user�ṹ���ַ
 */
void aw_shell_color_disable (aw_shell_user_t *p_user);

/**
 * \brief   ����shell��������ɫ
 *
 * \param[in]   p_user     aw_shell_user�ṹ���ַ
 * \param[in]   p_color    Ҫ���õ���ɫ��
 *
 * \return  �ɹ�����AW_OK�����򷵻�aw_err_t�Ĵ�������
 */
aw_err_t aw_shell_color_set (aw_shell_user_t *p_user, const char *p_color);

/**
 * \brief   shell��ӡ
 *
 * \param[in]  sh    aw_shell_user�ṹ���ַ
 * \param[in]  fmt   ��ʽ������
 *
 * \return  ����������ַ�����
 */
int aw_shell_printf (aw_shell_user_t *sh, const char *fmt, ...);

/**
 * \brief shell���һ���ַ�
 *
 * \param[in]  sh    aw_shell_user�ṹ���ַ
 * \param[in]  c     �ַ�
 *
 * \return  �ɹ�����������ַ����ȣ�����Ϊ��ֵ�������
 */
int aw_shell_putc (aw_shell_user_t *sh, int c);

/**
 * \brief shell���һ���ַ���
 *
 * \param[in]  sh    aw_shell_user�ṹ���ַ
 * \param[in]  str   �ַ���
 *
 * \return  �ɹ�����������ַ����ȣ�����Ϊ��ֵ�������
 */
int aw_shell_puts (aw_shell_user_t *sh, const char *str);

/**
 * \brief ��shell��ȡһ���ַ������û���û����룬���ȴ��û�����
 *
 * \param[in]  sh    aw_shell_user�ṹ���ַ
 *
 * \return  ����ֵ����0��������Ч�ַ���С��0����Ч
 */
int aw_shell_getc (aw_shell_user_t *sh);

/**
 * \brief ��shell��ȡһ���ַ������û���û����룬����������
 *
 * \param[in]  sh    aw_shell_user�ṹ���ַ
 *
 * \return  ����ֵ����0��������Ч�ַ���С��0����Ч
 */
int aw_shell_try_getc (aw_shell_user_t *sh);


/**
 * \brief ��shell�ж�ȡһ�У�δ���յ��س���ʱ������
 *
 * \param[in] sh        shell���û������е�������
 * \param[in] s         �����ַ�������
 * \param[in] count     �����ַ��������ĳ���
 *
 * \return          �ɹ����ض�ȡ�����ֽ���
 */
int aw_shell_gets (struct aw_shell_user *sh, char *s, int count);


/**
 * \brief ��ȡ����shell��ʵ��ָ��
 *
 * \return ����shell��ָ��
 */
aw_shell_user_t *aw_default_shell_get(void);
#define aw_dbg_shell_user aw_default_shell_get

/**
 * \brief ���õ���shell
 *
 * \return ���ؾɵ�shell
 */
aw_shell_user_t *aw_default_shell_set(aw_shell_user_t *sh);

/** \brief ����shell��ʵ��ָ�� */
#define AW_DBG_SHELL_IO aw_default_shell_get()

/** @} grp_aw_if_shell */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* __AW_SHELL_H */

/* end of file */
