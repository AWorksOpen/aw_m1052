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

/** \brief 前置声明 */
struct aw_shell_context;
struct aw_shell_user;


/**
 * \brief shell用户状态
 */
typedef enum aw_shell_user_state {
    AW_SHELL_SUS_UNINIT = 0,
    AW_SHELL_SUS_RUNING,
    AW_SHELL_SUS_EXITING,
    AW_SHELL_SUS_DEAD,
} aw_shell_user_state_t;


/** \brief shell user  */
struct aw_shell_user {

    /** \brief 格式化打印，默认赋值aw_shell_printf */
    int (*io_printf)(struct aw_shell_user *sh, const char *fmt, ...);

    /** \brief 输出一个字符串 */
    int (*io_puts)  (struct aw_shell_user *sh, const char *s);

    /** \brief 输出一个字符 */
    int (*io_putc)  (struct aw_shell_user *sh, int ch);

    /**
     * \brief 获取一个字符,可以阻塞，
     * 如果外设只支持查询，不支持阻塞获取
     * 且当前无数据可读，则可以使用aw_task_delay一段时间后重试
     * 成功返回获取到的字符，否则返回-ENODATA；
     * 此函数主要用于等待用户输入（命令或其它），所以当用户没有输入
     * 的时候可以阻塞，比如串口，可以永久等待，当用户没有输入的时候
     * shell线程可以永远不被唤醒
     */
    int (*io_getc)  (struct aw_shell_user *sh);

    /**
     * \brief 获取一个字符，不允许阻塞;
     *  成功返回获取到的字符，否则返回-ENODATA；
     *  
     *  此函数用于立即获取用户输入，可以用来判断用户是否在运行的过程中
     *  输入过ctrl+C
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

    /** \brief 保存历史的buf */
    /* uint8_t  *buf[AW_SHELL_HISTORY_COUNT][AW_SHELL_LINE_SIZE + 1];*/
    uint8_t  *buf;
    uint32_t  buf_num;

    uint32_t oldest;                             /**< \brief 最旧的历史id */
    uint32_t newest;                             /**< \brief 最新的历史id */
    uint32_t count;                              /**< \brief 历史的数量 */
};
#endif
/******************************************************************************/
/** \brief Simple shell context struct. */
struct aw_shell_context {

    aw_shell_opt_t          *p_opt;               /**< \brief shell的选项 */

    uint8_t                  stat;

    /** \brief 接收数据的行buf */
    uint8_t                  line_buf[AW_SHELL_LINE_SIZE + 1];
    int                      line_len;          /**< \brief 行buf中接收的数据长度 */

    int                      cursor_pos;        /**< \brief 光标的位置 */

    uint8_t                  rec[15];           /**< \brief 接收数据的临时缓存 */

    /** \brief 描述符buf */
    char                     des[AW_SHELL_LINE_SIZE >> 1];
    char                     des_store[AW_SHELL_LINE_SIZE >> 1];

    /**< \brief 描述符颜色 */
    char                     des_color[AW_SHELL_LINE_SIZE >> 1];
    aw_bool_t                   des_exist;      /**< \brief 描述符存在的标志 */

    struct aw_list_head      alias_list;        /**< \brief 别名链表 */

#ifdef AW_SHELL_USE_HISTORY
    uint32_t                 his_index;         /**< \brief 当前历史的索引 */
    struct aw_shell_history  his;               /**< \brief 保存历史的结构体 */
#endif

#ifdef AW_SHELL_USE_CMDSET
    struct aw_shell_cmdset * cur_cmdset;        /**< \brief 当前所在的命令集 */
    aw_bool_t                   is_cmdset;      /**< \brief 是否是在命令集 */
#endif /* AW_SHELL_USE_CMDSET */

    int                      opt_start;         /**< \brief opt的下标 */
    int                      opt_end;           /**< \brief 下一个opt的下标 */
    aw_bool_t                   opt_is_valid;   /**< \brief 当前选项是否有效 */

    aw_bool_t                   inited;         /**< \brief 初始化标志 */
    aw_bool_t                   color_enable;   /**< \brief 颜色使能 */

    int                      argc;              /**< \brief 参数计数 */
    char                   **argv;              /**< \brief 参数地址 */

    AW_MUTEX_DECL(           lock);
};


/******************************************************************************/
#ifdef AW_SHELL_USE_CHART

/** \brief chart structure */
struct aw_shell_chart {
    struct aw_shell_user *user;             /**< \brief user的指针  */

    uint32_t              h_frame;          /**< \brief 水平边框的符号  */
    uint32_t              v_frame;          /**< \brief 垂直边框的符号  */

#define __CHART_ITEM_MAX   15
    uint32_t              item_sep;         /**< \brief item的分隔符  */
    const char           *item_sep_context; /**< \brief item的分隔线的内容  */
    uint32_t              line_sep;         /**< \brief line的分隔符  */

    /** \brief 保存每个item的最大长度  */
    uint8_t               item_max_len[__CHART_ITEM_MAX];
    uint8_t               nitems;           /**< \brief item的数量  */
    uint8_t               cur_inline;       /**< \brief 当前的内行数量  */
    int                   cur_line;         /**< \brief 当前的行数  */
    uint32_t              line_max_len;     /**< \brief 一行最大的显示长度  */
    aw_bool_t             is_default_hframe;/**< \brief 是否使用默认边框  */
    aw_bool_t             wait_use_hframe;  /**< \brief 等待使用默认边框  */

    char                  align_mode;       /**< \brief 对齐模式 */
#define AW_SHELL_CHART_ALIGN_RIGHT      1   /**< \brief 右对齐 */
#define AW_SHELL_CHART_ALIGN_LEFT       2   /**< \brief 左对齐 */
#define AW_SHELL_CHART_ALIGN_MID        3   /**< \brief 居中 */
};

#else

struct aw_shell_chart {
    #define AW_SHELL_CHART_ALIGN_RIGHT      1   /**< \brief 右对齐 */
    #define AW_SHELL_CHART_ALIGN_LEFT       2   /**< \brief 左对齐 */
    #define AW_SHELL_CHART_ALIGN_MID        3   /**< \brief 居中 */
    #define __CHART_ITEM_MAX   15
};

#endif /* AW_SHELL_USE_CHART */
/******************************************************************************/


/******************************************************************************/
/**
 * \brief 表格的初始化
 *
 * \param[in]       p_this      aw_shell_chart结构体地址
 * \param[in]       user        aw_shell_user结构体地址
 * \param[in]       mode        表格的对齐模式
 * \param[in]       h_frame     水平边框的字符，为0则无水平边框
 * \param[in]       v_frame     垂直边框的字符，为0则无垂直边框
 *
 * \retval  成功返回AW_OK，否则返回值aw_err_t的错误类型
 */
int aw_shell_chart_init (struct aw_shell_chart *p_this,
                         struct aw_shell_user  *user,
                         char                   mode,
                         uint32_t               h_frame,
                         uint32_t               v_frame);

/**
 * \brief 添加一个固定最大长度的列项，该项下的内容的长度超
 *        过该项的最大长度时会自动换行
 *
 * \param[in]       p_this      aw_shell_chart结构体地址
 * \param[in]       item        该项的名称，为NULL则不显示该项名称
 * \param[in]       maxlen      该项的最大长度
 *
 * \retval  成功返回该项的id，否则返回负数
 *
 * \note    项的id由系统自动分配，项的显示则按添加时的先后顺序进行显示
 */
int  aw_shell_chart_item (struct aw_shell_chart  *p_this,
                          const char             *item,
                          uint8_t                 maxlen);

/**
 * \brief 往指定的行和列项里添加字符串内容
 *
 * \param[in]       p_this      aw_shell_chart结构体地址
 * \param[in]       p_str       字符串
 * \param[in]       item_id     项的id，该id用于指定字符串属于那一项的内容
 * \param[in]       line        指定在哪一行显示字符串
 *
 * \retval  成功返回AW_OK，否则返回值aw_err_t的错误类型
 *
 * \note    同一行的不同项的内容添加可以任意顺序，但是行数（line）只能同行或递增
 */
int  aw_shell_chart_line (struct aw_shell_chart  *p_this,
                          const  char            *p_str,
                          uint8_t                 item_id,
                          int                     line);

/**
 * \brief 添加列项的分隔线和分隔线内容
 *
 * \param[in]       p_this      aw_shell_chart结构体地址
 * \param[in]       sep         分隔符的符号
 * \param[in]       p_context   分隔线的内容，可NULL
 *
 * \retval  无
 */
void aw_shell_chart_item_separator ( struct aw_shell_chart    *p_this,
                                     uint32_t                  sep,
                                     const char               *p_context);

/**
 * \brief 添加行内容的分隔线和分隔线内容
 *
 * \param[in]       p_this      aw_shell_chart结构体地址
 * \param[in]       sep         分隔符的符号
 * \param[in]       p_context   分隔线的内容，可NULL
 *
 * \retval  无
 */
void aw_shell_chart_line_separator ( struct aw_shell_chart  *p_this,
                                     uint32_t                sep,
                                     const char             *p_context);

/**
 * \brief 对表格进行最后的完成，并打印最后的分隔线
 *
 * \param[in]       p_this      aw_shell_chart结构体地址
 * \param[in]       sep         分隔符的符号
 * \param[in]       p_context   分隔线的内容，可NULL
 *
 * \retval  无
 */
void aw_shell_chart_finish (   struct aw_shell_chart      *p_this,
                               uint32_t                    sep,
                               const char                 *p_context);

/******************************************************************************/

/******************************************************************************/
/**
 * \brief shell实例的初始化
 * \param sc        shell运行时上下文结构体
 * \param sh        shell在用户任务中的上下文
 * \param opt       shell的选项
 *
 * \retval AW_OK    运行成功
 */
aw_err_t aw_shell_init(struct aw_shell_context *sc, 
                       struct aw_shell_user    *sh, 
                       aw_shell_opt_t          *p_opt);

/**
 * \brief 运行一个shell实例
 * \param sc        shell运行时上下文结构体
 * \param sh        shell在用户任务中的上下文
 *
 * \retval AW_OK    运行成功
 */
aw_err_t aw_shell_server(struct aw_shell_context *sc, struct aw_shell_user *sh);


/** @} grp_aw_if_shell_core */

#ifdef __cplusplus
}
#endif


#endif /* __AW_SHELL_CORE_H */

/* end of file */
