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
 * \file aw_loglib.h
 * \brief 日志消息库头文件
 * \note 
 *       
 * \par 简单举例
 * \code
 * aw_log_dev_t dev_uart;
 *
 * aw_loglib_cfg_init();  
 *
 * // 初始化设备结构体dev_uart
 * aw_loglib_log_dev_init(&dev_uart, COM0, aw_serial_write);
 * aw_loglib_dev_register(&dev_uart); // 注册设备dev_uart
 *   
 * aw_log_msg("Hello, I'm aw_log_msg", 0, 0, 0, 0, 0, 0);
 *
 * aw_loglib_dev_unregister(&dev_uart); // 设备dev_uart取消注册 
 * \endcode
 * \internal
 * \par modification history:
 * - 1.00 14-11-2  win&bob, first implementation
 * - 1.10 17-3-15  win, add new features.
 * \endinternal
 */
#ifndef __AW_LOGLIB_H
#define __AW_LOGLIB_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_log_lib
 * \copydoc aw_loglib.h
 * @{
 */
#include "aw_list.h"
#include "aw_errno.h" 
#include "aw_task.h"
#include "aw_msgq.h"
#include "aw_sem.h"


/** \brief 导出所有日志数据 */
#define AW_LOG_EXPORT_ALL           (0xFFFFFFF)

/** \brief 导出位置偏移到日志头部 */
#define AW_LOG_EXPORT_OFFSET_HEAD   (-0xFFFFFFF)

/**
 * \brief 日志等级(从高到低)
 */
typedef enum aw_log_level {
    AW_LOG_LVL_ASSERT = 0,      /**< \brief 日志等级：断言 */
    AW_LOG_LVL_ERROR,           /**< \brief 日志等级：错误 */
    AW_LOG_LVL_WARN,            /**< \brief 日志等级：警告 */
    AW_LOG_LVL_INFO,            /**< \brief 日志等级：信息 */
    AW_LOG_LVL_DEBUG,           /**< \brief 日志等级：调试 */
    AW_LOG_LVL_VERBOSE,         /**< \brief 日志等级：详细 */
} aw_log_level_t;

/** \brief output log's level total number */
#define AW_LOG_LVL_TOTAL_NUM    6

/** \brief all formats index */
typedef enum aw_log_fmt {
    AW_LOG_FMT_LVL       = 1 << 0, /**< \brief level */
    AW_LOG_FMT_TAG       = 1 << 1, /**< \brief tag */
    AW_LOG_FMT_TIME      = 1 << 2, /**< \brief current time */
    AW_LOG_FMT_TASK_INFO = 1 << 3, /**< \brief task info */
    AW_LOG_FMT_DIR       = 1 << 5, /**< \brief file directory and name */
    AW_LOG_FMT_FUNC      = 1 << 6, /**< \brief function name */
    AW_LOG_FMT_LINE      = 1 << 7, /**< \brief line number */
} aw_log_fmt;

/** \brief macro definition for all formats */
#define AW_LOG_FMT_ALL    (AW_LOG_FMT_LVL       | \
                           AW_LOG_FMT_TAG       | \
                           AW_LOG_FMT_TIME      | \
                           AW_LOG_FMT_TASK_INFO | \
                           AW_LOG_FMT_DIR       | \
                           AW_LOG_FMT_FUNC      | \
                           AW_LOG_FMT_LINE)

/** \brief 标签 */
#define AW_LOG_TAG "aw_log_tag"

/** \brief log level assert */
#define AW_LOG_A(id, tag, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
        aw_log_msg_ex(id, AW_LOG_LVL_ASSERT, tag, __FILE__, __FUNCTION__, \
                      __LINE__, fmt, arg1, arg2, arg3, arg4, arg5, arg6)

/** \brief log level error */
#define AW_LOG_E(id, tag, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
        aw_log_msg_ex(id, AW_LOG_LVL_ERROR, tag, __FILE__, __FUNCTION__, \
                      __LINE__, fmt, arg1, arg2, arg3, arg4, arg5, arg6)

/** \brief log level warning */
#define AW_LOG_W(id, tag, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
        aw_log_msg_ex(id, AW_LOG_LVL_WARN, tag, __FILE__, __FUNCTION__, \
                      __LINE__, fmt, arg1, arg2, arg3, arg4, arg5, arg6)

/** \brief log level info */
#define AW_LOG_I(id, tag, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
        aw_log_msg_ex(id, AW_LOG_LVL_INFO, tag, __FILE__, __FUNCTION__, \
                      __LINE__, fmt, arg1, arg2, arg3, arg4, arg5, arg6)

/** \brief log level debug */
#define AW_LOG_D(id, tag, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
        aw_log_msg_ex(id, AW_LOG_LVL_DEBUG, tag, __FILE__, __FUNCTION__, \
                      __LINE__, fmt, arg1, arg2, arg3, arg4, arg5, arg6)

/** \brief log level verbose */
#define AW_LOG_V(id, tag, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
        aw_log_msg_ex(id, AW_LOG_LVL_VERBOSE, tag, __FILE__, __FUNCTION__, \
                      __LINE__, fmt, arg1, arg2, arg3, arg4, arg5, arg6)

/** \brief raw log message */
#define AW_LOG(id, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
        aw_log_msg(id, fmt, arg1, arg2, arg3, arg4, arg5, arg6)

/** \brief 提前申明结构体 */
struct aw_logger;

/** \brief 定义一个函数指针类型 */
typedef ssize_t (*aw_pfn_logger_print_t)(struct aw_logger *p_logger,
                                         const char       *p_buf,
                                         size_t            nbytes);

/**
 * \brief 设备结构体.
 * 该结构体使用双向链表将设备节点链接起来
 */
typedef struct aw_logger {

    /**\brief 节点  */
    struct aw_list_head        node;

    /**\brief 写日志接口  */
    aw_pfn_logger_print_t      print;

    /**\brief 该logger的id，id相同的logger会同时写日志  */
    int                        id;

    /**\brief 将数据写入存储设备的延时，时间未到时数据都存放在缓存中，为0则不使用缓存*/
    int                        tim;

    /**\brief 注册标志 */
    aw_bool_t                  reg_flag;

    /**\brief 日志数据大小 */
    uint32_t                   log_size;

    int                        export_pos;

    /**\brief 日志导出接口 */
    aw_err_t (*pfn_export)    (struct aw_logger  *p_logger,
                               int                offset,
                               void              *buf,
                               uint32_t           buf_size);

    /**\brief 初始化接口 */
    aw_err_t (*pfn_init)       (struct aw_logger *p_logger);

    /**\brief 反初始化接口 */
    aw_err_t (*pfn_deinit)     (struct aw_logger *p_logger);

    /**\brief 清空 log 数据 */
    aw_err_t (*pfn_data_clear) (struct aw_logger *p_logger);

    /**\brief 互斥锁 */
    AW_MUTEX_DECL(mutex_lock);
} aw_logger_t;
 


/**
 * \brief 打开消息日志
 * \attention 中断不可调用
 * \retval AW_Ok 成功
 */
aw_err_t aw_log_on (void);

/**
 * \brief 关闭消息日志
 * \attention 中断不可调用
 * \retval AW_OK 成功
 */
aw_err_t aw_log_off (void);

/**
 * \brief 设置消息格式
 * \attention 中断不可调用
 *
 * \param[in] level : 需要设置格式的消息等级。
 * \param[in] set   : 设置值，为AW_LOG_FMT_*值或者多个AW_LOG_FMT_*的或值
 *
 * \retval AW_OK 成功
 * \note 完整消息格式：LEVEL/TAG [TIME TASK](DIR FUNC:LINE):MSG
 *       LEVEL 对应AW_LOG_FMT_LVL，表示消息级别（例如"A/"）
 *       TAG   对应AW_LOG_FMT_TAG，表示标签信息（例如"aw_log_tag"）
 *       TIME  对应AW_LOG_FMT_TIME，表示时间信息，外部用户实现函数#aw_loglib_time_get()
 *             可以修改显示消息的格式
 *       TASK  对应AW_LOG_FMT_TASK_INFO，表示任务信息，外部用户实现函数
 *             #aw_loglib_task_info_get()可以修改显示任务信息的格式
 *       DIR   对应AW_LOG_FMT_DIR，表示文件路径，一般使用宏#__FILE__
 *       FUNC  对应AW_LOG_FMT_FUNC，表示函数名称，一般使用宏#__FUNCTION__
 *       LINE  对应AW_LOG_FMT_LINE，表示调用点的行数，一般使用宏#__LINE__
 *       MSG   为用户需要输出的格式化消息，由用户输入，支持最多6个参数。
 */
aw_err_t aw_log_fmt_set (aw_log_level_t level, size_t set);

/**
 * \brief 设置过滤方式
 * \attention 中断不可调用
 *
 * \param[in] level     : 需要设置格式的消息等级。
 * \param[in] p_tag     : 标签
 * \param[in] p_keyword : 关键字
 *
 * \retval AW_OK 成功
 * \note 设置了三种过滤方式：等级（level）、标签（tag）、关键字（keyword）
 *       等级（level）        ： 消息等级不大于设置的过滤值（优先级相同或更高）的消息才会输出，例如过滤level
 *                       设置为AW_LOG_LVL_ERROR，则只有消息级别为AW_LOG_LVL_ERROR和
 *                       AW_LOG_LVL_ASSERT的消息才能输出。
 *       标签（tag）              ：只有消息标签中包含设置的过滤标签的消息才会输出。
 *       关键字（keyword）：按消息格式合成的消息，只有包含了关键字，才能输出。消息格式的设置见#aw_log_fmt_set()
 */
aw_err_t aw_log_filter_set (aw_log_level_t level, const char *p_tag, const char *p_keyword);

/**
 * \brief 设置过滤等级
 * \attention 中断不可调用、
 *
 * \param[in] level : 等级
 *
 * \retval AW_OK 成功
 */
aw_err_t aw_log_filter_lvl_set (aw_log_level_t level);

/**
 * \brief 设置过滤标签
 * \attention 中断不可调用
 *
 * \param[in] p_tag : 标签
 *
 * \retval AW_OK 成功
 */
aw_err_t aw_log_filter_tag_set (const char *p_tag);

/**
 * \brief 设置过滤关键字
 * \attention 中断不可调用
 *
 * \param[in] p_keyword : 关键字
 *
 * \retval AW_OK 成功
 */
aw_err_t aw_log_filter_keyword_set (const char *p_keyword);


/**
 * \brief 发送消息
 * \attention 中断可调用，无阻塞
 *
 * \param[in] id     用aw_logger_register()注册日志器时指定的id
 * \param[in] p_fmt  格式化字符串指针   
 * \param[in] arg1   参数1
 * \param[in] arg2   参数2
 * \param[in] arg3   参数3
 * \param[in] arg4   参数4
 * \param[in] arg5   参数5
 * \param[in] arg6   参数6
 *
 * \retval    AW_OK    成功
 * \retval    -EINVAL  参数无效
 * \retval    -ETIME   发送消息超时
 * \retval    -ENXIO   再等待的时候消息队列被终止
 * \retval    -EAGAIN  发送消息失败,稍后再试(timeout = AW_MSGQ_NO_WAIT时) 
 */
aw_err_t aw_log_msg (int   id,
                     char *p_fmt,
                     int   arg1,
                     int   arg2,
                     int   arg3,
                     int   arg4,
                     int   arg5,
                     int   arg6);

/**
 * \brief 发送扩展消息（异步）
 * \attention 中断可调用，无阻塞
 *
 * \param[in] id     用aw_logger_register()注册日志器时指定的id
 * \param[in] level  消息等级。
 * \param[in] p_tag  标签
 * \param[in] p_file 文件名
 * \param[in] p_func 调用函数名
 * \param[in] line   调用点的行数
 * \param[in] p_fmt  格式化字符串指针
 * \param[in] arg1   参数1
 * \param[in] arg2   参数2
 * \param[in] arg3   参数3
 * \param[in] arg4   参数4
 * \param[in] arg5   参数5
 * \param[in] arg6   参数6
 *
 * \retval    AW_OK    成功
 * \retval    -EINVAL  参数无效
 * \retval    -ETIME   发送消息超时
 * \retval    -ENXIO   再等待的时候消息队列被终止
 * \retval    -EAGAIN  发送消息失败,稍后再试(timeout = AW_MSGQ_NO_WAIT时)
 */
aw_err_t aw_log_msg_ex (int                  id,
                        aw_log_level_t       level,
                        const char          *p_tag,
                        const char          *p_file,
                        const char          *p_func,
                        const long           line,
                        char                *p_fmt,
                        int                  arg1,
                        int                  arg2,
                        int                  arg3,
                        int                  arg4,
                        int                  arg5,
                        int                  arg6);

/**
 * \brief 发送扩展消息（同步）
 * \attention 中断不可调用，有阻塞
 *
 * \param[in] id     用aw_logger_register()注册日志器时指定的id
 * \param[in] level  消息等级。
 * \param[in] p_tag  标签
 * \param[in] p_file 文件名
 * \param[in] p_func 调用函数名
 * \param[in] line   调用点的行数
 * \param[in] p_fmt  格式化字符串指针
 * \param[in] arg1   参数1
 * \param[in] arg2   参数2
 * \param[in] arg3   参数3
 * \param[in] arg4   参数4
 * \param[in] arg5   参数5
 * \param[in] arg6   参数6
 *
 * \retval    AW_OK    成功
 * \retval    -EINVAL  参数无效
 * \retval    -ETIME   发送消息超时
 * \retval    -ENXIO   再等待的时候消息队列被终止
 * \retval    -EAGAIN  发送消息失败,稍后再试(timeout = AW_MSGQ_NO_WAIT时)
 */
aw_err_t aw_log_msg_ex_sync (int             id,
                             aw_log_level_t  level,
                             const char     *p_tag,
                             const char     *p_file,
                             const char     *p_func,
                             const long      line,
                             char           *p_fmt,
                             int             arg1,
                             int             arg2,
                             int             arg3,
                             int             arg4,
                             int             arg5,
                             int             arg6);

/**
 * \brief 设备注册
 * \attention 中断不可调用
 *
 * \param[in] p_logger      设备结构体指针
 * \param[in] id            该日志器的id，每个日志器的id都应该是唯一的
 * \param[in] tim           log的同步时间(ms)：写log数据时，会先写入缓存，
 *                          达到时间后会写入到存储器。可为0，则不使用缓存。
 *           
 * \retval    AW_OK       成功
 * \retval    -AW_EINVAL  参数无效
 * \retval    -AW_EPERM   不允许的操作，在中断中调用了本函数
 * \retval    -AW_ENXIO   任务已关闭,无法注册设备
 * \retval    -EAGAIN     任务忙,请重试
 * \retval    -ENOTSUP    日志器的print指针未赋值
 */                                
aw_err_t aw_logger_register(aw_logger_t          *p_logger,
                            int                   id,
                            int                   tim);
                                
/**
 * \brief 设备注销
 * \attention 中断不可调用
 * \param[in] p_logger   设备结构体指针
 *           
 * \retval    AW_OK       成功
 * \retval    -AW_EINVAL  参数无效
 * \retval    -AW_EPERM   不允许的操作，在中断中调用了本函数
 * \retval    -AW_ENXIO   任务已关闭,无法注销设备
 * \retval    -EAGAIN     任务忙,请重试
 */
aw_err_t aw_logger_unregister(aw_logger_t *p_logger);


/**
 * \brief 导出日志数据
 *
 * \param[in]       p_logger    日志器指针
 * \param[in]       offset      读偏移
 * \param[in]       buf         保存数据的buf
 * \param[in]       buf_size    buf的大小
 *
 * \retval  > 0         读取到的数据量
 * \retval  -EINVAL     参数无效
 * \retval  -ENOTSUP    不支持该功能
 * \retval  -EFAULT     偏移超出范围
 * \retval  ELSE        其他错误，具体可查看errno
 */
aw_err_t aw_log_export (aw_logger_t       *p_logger,
                        int                offset,
                        void              *buf,
                        uint32_t           buf_size);

/**
 * \brief 获取日志数据的大小
 *
 * \param[in]       p_logger    日志器指针
 *
 * \retval  该日志的数据大小
 */
uint32_t aw_log_data_size_get (aw_logger_t  *p_logger);

/**
 * \brief 清空日志数据
 *
 * \param[in]       p_logger    日志器指针
 *
 * \retval  AW_OK        成功
 * \retval  ELSE        其他错误，具体可查看errno
 */
aw_err_t aw_log_data_clear (aw_logger_t       *p_logger);

/**
 * \brief 完成loglib初始化
 * \param[in] task_id       任务ID
 * \param[in] msgq_id       队列ID
 * \param[in] p_logbuf      buf指针
 * \param[in] logbuf_size   buf大小
 *
 * \retval  AW_OK        成功
 * \retval  -AW_ENOMEM   空间(内存)不足
 * \retval  -AW_EINVAL   无效参数
 */
aw_err_t aw_loglib_init (aw_task_id_t  task_id,
                         aw_msgq_id_t  msgq_id,
                         char         *p_logbuf,
                         size_t        logbuf_size);



/** @} grp_aw_log_lib */

#ifdef __cplusplus
}
#endif

#endif /* __AW_LOGLIB_H */

/* end of file */

