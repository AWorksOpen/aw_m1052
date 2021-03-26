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
 * @file
 * @brief log adapt ftl.
 *
 * @internal
 * @par History
 * - 170707, vih, First implementation.
 * @endinternal
 */


#ifndef __AW_LOG_FTL_H__
#define __AW_LOG_FTL_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_log_ftl
 * \copydoc aw_log_ftl.h
 * @{
 */

#include "aworks.h"
#include "aw_delayed_work.h"
#include "aw_sys_defer.h"
#include "aw_ftl.h"
#include "aw_loglib.h"

/** \brief ftl日志存储的上下文 */
typedef struct aw_log_ftl_ctx {

    void                       *p_ftl_dev;        /**< \brief ftl设备指针 */

#define __LOG_FTL_DEV_NAME_MAX    32

    /** \brief 设备名 */
    char                        dev_name[__LOG_FTL_DEV_NAME_MAX];

    aw_logger_t                 logger;           /**< \brief 日志器实例 */

    struct aw_delayed_work      delay_work;       /**< \brief 用于定时  */
    struct aw_defer_djob        djob;             /**< \brief 延时工作  */
    aw_bool_t                   defer_init;       /**< \brief 延时工作的初始化标志  */

    int                         tim;              /**< \brief 定时ms  */

    int                         err;              /**< \brief 错误值  */

    uint16_t                   *head;             /**< \brief 数据头，占2字节  */
    char                       *buf;              /**< \brief 数据缓存  */
    int                         offset;           /**< \brief 缓存的偏移  */

    int                         sector_size;      /**< \brief 扇区大小  */
    int                         sector_data_size; /**< \brief 扇区数据空间大小  */
    int                         nsectors;         /**< \brief 扇区数量  */

    int                         free_sector;      /**< \brief 完全干净的扇区数量  */

    /** \brief 数据清除门槛，空闲扇区低于该值启动清除  */
    int                         sector_threshold;
    int                         newest_lsn;       /**< \brief 最新数据的扇区号  */
    int                         oldest_lsn;       /**< \brief 最旧的数据的块号  */

    aw_bool_t                   init_flag;        /**< \brief 初始化标记  */
    aw_bool_t                   wait_write;       /**< \brief 等待写入标记  */
    aw_bool_t                   format_req;       /**< \brief 格式化磁盘请求  */
} aw_log_ftl_ctx_t;


/**
 * \brief 初始化一个 log_ftl 实例
 *
 * \param[in]       p_ctx       实例指针
 * \param[in]       p_dev_name  mtd设备名
 * \param[in]       format_req  格式化请求, AW_TRUE:请求格式化  AW_FALSE:不格式化
 *
 * \return 成功返回aw_logger_t *类型的指针，失败返回NULL
 *
 */
aw_logger_t * aw_log_ftl_ctor (aw_log_ftl_ctx_t *p_ctx,
                               const char       *p_dev_name,
                               aw_bool_t         format_req);


/** @} grp_aw_log_ftl */

#ifdef __cplusplus
}
#endif

#endif /* __AW_LOG_FTL_H__ */

/* end of file */
