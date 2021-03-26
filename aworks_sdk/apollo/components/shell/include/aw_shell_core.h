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
 * \file aw_shell_core.h
 * \brief
 *
 * \internal
 * \par History
 * - 1.00 17-06-19, vih, first implementation.
 * \endinternal
 */

#ifndef __AW_SHELL_CORE_H
#define __AW_SHELL_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_shell_core
 * \copydoc aw_shell_core.h
 * @{
 */

#include "aw_shell_cfg.h"
#include "aw_shell.h"
#include "aw_common.h"
#include "aw_list.h"
#include "io/aw_dirent.h"

#include <stdarg.h>

/** \brief ǰ������ */
struct aw_shell_context;
struct aw_shell_user;


/**
 * \brief shell�û�״̬
 */
typedef enum aw_shell_user_state {
    AW_SHELL_SUS_UNINIT = 0,
    AW_SHELL_SUS_RUNING,
    AW_SHELL_SUS_EXITING,
    AW_SHELL_SUS_DEAD,
} aw_shell_user_state_t;


/** \brief shell user  */
struct aw_shell_user {

    /** \brief ��ʽ����ӡ��Ĭ�ϸ�ֵaw_shell_printf */
    int (*io_printf)(struct aw_shell_user *sh, const char *fmt, ...);

    /** \brief ���һ���ַ��� */
    int (*io_puts)  (struct aw_shell_user *sh, const char *s);

    /** \brief ���һ���ַ� */
    int (*io_putc)  (struct aw_shell_user *sh, int ch);

    /**
     * \brief ��ȡһ���ַ�,����������
     * �������ֻ֧�ֲ�ѯ����֧��������ȡ
     * �ҵ�ǰ�����ݿɶ��������ʹ��aw_task_delayһ��ʱ�������
     * �ɹ����ػ�ȡ�����ַ������򷵻�-ENODATA��
     * �˺�����Ҫ���ڵȴ��û����루����������������Ե��û�û������
     * ��ʱ��������������紮�ڣ��������õȴ������û�û�������ʱ��
     * shell�߳̿�����Զ��������
     */
    int (*io_getc)  (struct aw_shell_user *sh);

    /**
     * \brief ��ȡһ���ַ�������������;
     *  �ɹ����ػ�ȡ�����ַ������򷵻�-ENODATA��
     *  
     *  �˺�������������ȡ�û����룬���������ж��û��Ƿ������еĹ�����
     *  �����ctrl+C
     */
    int (*io_try_getc)  (struct aw_shell_user *sh);

    struct aw_shell_context *sc;
    void                    *user_arg;
    aw_shell_user_state_t    state;

    AW_MUTEX_DECL(mutex);

};

/******************************************************************************/

#ifdef AW_SHELL_USE_HISTORY

/** \brief shell history */
struct aw_shell_history {

    /** \brief ������ʷ��buf */
    /* uint8_t  *buf[AW_SHELL_HISTORY_COUNT][AW_SHELL_LINE_SIZE + 1];*/
    uint8_t  *buf;
    uint32_t  buf_num;

    uint32_t oldest;                             /**< \brief ��ɵ���ʷid */
    uint32_t newest;                             /**< \brief ���µ���ʷid */
    uint32_t count;                              /**< \brief ��ʷ������ */
};
#endif
/******************************************************************************/
/** \brief Simple shell context struct. */
struct aw_shell_context {

    aw_shell_opt_t          *p_opt;               /**< \brief shell��ѡ�� */

    uint8_t                  stat;

    /** \brief �������ݵ���buf */
    uint8_t                  line_buf[AW_SHELL_LINE_SIZE + 1];
    int                      line_len;          /**< \brief ��buf�н��յ����ݳ��� */

    int                      cursor_pos;        /**< \brief ����λ�� */

    uint8_t                  rec[15];           /**< \brief �������ݵ���ʱ���� */

    /** \brief ������buf */
    char                     des[AW_SHELL_LINE_SIZE >> 1];
    char                     des_store[AW_SHELL_LINE_SIZE >> 1];

    /**< \brief ��������ɫ */
    char                     des_color[AW_SHELL_LINE_SIZE >> 1];
    aw_bool_t                   des_exist;      /**< \brief ���������ڵı�־ */

    struct aw_list_head      alias_list;        /**< \brief �������� */

#ifdef AW_SHELL_USE_HISTORY
    uint32_t                 his_index;         /**< \brief ��ǰ��ʷ������ */
    struct aw_shell_history  his;               /**< \brief ������ʷ�Ľṹ�� */
#endif

#ifdef AW_SHELL_USE_CMDSET
    struct aw_shell_cmdset * cur_cmdset;        /**< \brief ��ǰ���ڵ���� */
    aw_bool_t                   is_cmdset;      /**< \brief �Ƿ�������� */
#endif /* AW_SHELL_USE_CMDSET */

    int                      opt_start;         /**< \brief opt���±� */
    int                      opt_end;           /**< \brief ��һ��opt���±� */
    aw_bool_t                   opt_is_valid;   /**< \brief ��ǰѡ���Ƿ���Ч */

    aw_bool_t                   inited;         /**< \brief ��ʼ����־ */
    aw_bool_t                   color_enable;   /**< \brief ��ɫʹ�� */

    int                      argc;              /**< \brief �������� */
    char                   **argv;              /**< \brief ������ַ */

    AW_MUTEX_DECL(           lock);
};


/******************************************************************************/
#ifdef AW_SHELL_USE_CHART

/** \brief chart structure */
struct aw_shell_chart {
    struct aw_shell_user *user;             /**< \brief user��ָ��  */

    uint32_t              h_frame;          /**< \brief ˮƽ�߿�ķ���  */
    uint32_t              v_frame;          /**< \brief ��ֱ�߿�ķ���  */

#define __CHART_ITEM_MAX   15
    uint32_t              item_sep;         /**< \brief item�ķָ���  */
    const char           *item_sep_context; /**< \brief item�ķָ��ߵ�����  */
    uint32_t              line_sep;         /**< \brief line�ķָ���  */

    /** \brief ����ÿ��item����󳤶�  */
    uint8_t               item_max_len[__CHART_ITEM_MAX];
    uint8_t               nitems;           /**< \brief item������  */
    uint8_t               cur_inline;       /**< \brief ��ǰ����������  */
    int                   cur_line;         /**< \brief ��ǰ������  */
    uint32_t              line_max_len;     /**< \brief һ��������ʾ����  */
    aw_bool_t             is_default_hframe;/**< \brief �Ƿ�ʹ��Ĭ�ϱ߿�  */
    aw_bool_t             wait_use_hframe;  /**< \brief �ȴ�ʹ��Ĭ�ϱ߿�  */

    char                  align_mode;       /**< \brief ����ģʽ */
#define AW_SHELL_CHART_ALIGN_RIGHT      1   /**< \brief �Ҷ��� */
#define AW_SHELL_CHART_ALIGN_LEFT       2   /**< \brief ����� */
#define AW_SHELL_CHART_ALIGN_MID        3   /**< \brief ���� */
};

#else

struct aw_shell_chart {
    #define AW_SHELL_CHART_ALIGN_RIGHT      1   /**< \brief �Ҷ��� */
    #define AW_SHELL_CHART_ALIGN_LEFT       2   /**< \brief ����� */
    #define AW_SHELL_CHART_ALIGN_MID        3   /**< \brief ���� */
    #define __CHART_ITEM_MAX   15
};

#endif /* AW_SHELL_USE_CHART */
/******************************************************************************/


/******************************************************************************/
/**
 * \brief ���ĳ�ʼ��
 *
 * \param[in]       p_this      aw_shell_chart�ṹ���ַ
 * \param[in]       user        aw_shell_user�ṹ���ַ
 * \param[in]       mode        ���Ķ���ģʽ
 * \param[in]       h_frame     ˮƽ�߿���ַ���Ϊ0����ˮƽ�߿�
 * \param[in]       v_frame     ��ֱ�߿���ַ���Ϊ0���޴�ֱ�߿�
 *
 * \retval  �ɹ�����AW_OK�����򷵻�ֵaw_err_t�Ĵ�������
 */
int aw_shell_chart_init (struct aw_shell_chart *p_this,
                         struct aw_shell_user  *user,
                         char                   mode,
                         uint32_t               h_frame,
                         uint32_t               v_frame);

/**
 * \brief ���һ���̶���󳤶ȵ���������µ����ݵĳ��ȳ�
 *        ���������󳤶�ʱ���Զ�����
 *
 * \param[in]       p_this      aw_shell_chart�ṹ���ַ
 * \param[in]       item        ��������ƣ�ΪNULL����ʾ��������
 * \param[in]       maxlen      �������󳤶�
 *
 * \retval  �ɹ����ظ����id�����򷵻ظ���
 *
 * \note    ���id��ϵͳ�Զ����䣬�����ʾ�����ʱ���Ⱥ�˳�������ʾ
 */
int  aw_shell_chart_item (struct aw_shell_chart  *p_this,
                          const char             *item,
                          uint8_t                 maxlen);

/**
 * \brief ��ָ�����к�����������ַ�������
 *
 * \param[in]       p_this      aw_shell_chart�ṹ���ַ
 * \param[in]       p_str       �ַ���
 * \param[in]       item_id     ���id����id����ָ���ַ���������һ�������
 * \param[in]       line        ָ������һ����ʾ�ַ���
 *
 * \retval  �ɹ�����AW_OK�����򷵻�ֵaw_err_t�Ĵ�������
 *
 * \note    ͬһ�еĲ�ͬ���������ӿ�������˳�򣬵���������line��ֻ��ͬ�л����
 */
int  aw_shell_chart_line (struct aw_shell_chart  *p_this,
                          const  char            *p_str,
                          uint8_t                 item_id,
                          int                     line);

/**
 * \brief �������ķָ��ߺͷָ�������
 *
 * \param[in]       p_this      aw_shell_chart�ṹ���ַ
 * \param[in]       sep         �ָ����ķ���
 * \param[in]       p_context   �ָ��ߵ����ݣ���NULL
 *
 * \retval  ��
 */
void aw_shell_chart_item_separator ( struct aw_shell_chart    *p_this,
                                     uint32_t                  sep,
                                     const char               *p_context);

/**
 * \brief ��������ݵķָ��ߺͷָ�������
 *
 * \param[in]       p_this      aw_shell_chart�ṹ���ַ
 * \param[in]       sep         �ָ����ķ���
 * \param[in]       p_context   �ָ��ߵ����ݣ���NULL
 *
 * \retval  ��
 */
void aw_shell_chart_line_separator ( struct aw_shell_chart  *p_this,
                                     uint32_t                sep,
                                     const char             *p_context);

/**
 * \brief �Ա�����������ɣ�����ӡ���ķָ���
 *
 * \param[in]       p_this      aw_shell_chart�ṹ���ַ
 * \param[in]       sep         �ָ����ķ���
 * \param[in]       p_context   �ָ��ߵ����ݣ���NULL
 *
 * \retval  ��
 */
void aw_shell_chart_finish (   struct aw_shell_chart      *p_this,
                               uint32_t                    sep,
                               const char                 *p_context);

/******************************************************************************/

/******************************************************************************/
/**
 * \brief shellʵ���ĳ�ʼ��
 * \param sc        shell����ʱ�����Ľṹ��
 * \param sh        shell���û������е�������
 * \param opt       shell��ѡ��
 *
 * \retval AW_OK    ���гɹ�
 */
aw_err_t aw_shell_init(struct aw_shell_context *sc, 
                       struct aw_shell_user    *sh, 
                       aw_shell_opt_t          *p_opt);

/**
 * \brief ����һ��shellʵ��
 * \param sc        shell����ʱ�����Ľṹ��
 * \param sh        shell���û������е�������
 *
 * \retval AW_OK    ���гɹ�
 */
aw_err_t aw_shell_server(struct aw_shell_context *sc, struct aw_shell_user *sh);


/** @} grp_aw_if_shell_core */

#ifdef __cplusplus
}
#endif


#endif /* __AW_SHELL_CORE_H */

/* end of file */
