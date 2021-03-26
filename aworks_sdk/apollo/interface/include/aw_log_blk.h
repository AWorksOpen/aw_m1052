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
 * @brief log adapt fs.
 *
 * @internal
 * @par History
 * - 170707, vih, First implementation.
 * @endinternal
 */


#ifndef __AW_LOG_BLK_H__
#define __AW_LOG_BLK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_log_blk
 * \copydoc aw_log_blk.h
 * @{
 */

#include "aworks.h"
#include "aw_delayed_work.h"
#include "aw_sys_defer.h"
#include "aw_loglib.h"
#include "fs/aw_blk_dev.h"


/** \brief �����Ľṹ�� */
typedef struct aw_log_blk_ctx {
    struct aw_blk_dev          *p_bdev;         /**< \brief BLK�豸ָ�� */

#define __LOG_BLK_DEV_NAME_MAX    32
    
    /** \brief �豸�� */
    char                        dev_name[__LOG_BLK_DEV_NAME_MAX];

    aw_logger_t                 logger;         /**< \brief ��־��ʵ�� */

    struct aw_delayed_work      delay_work;     /**< \brief ���ڶ�ʱ  */
    struct aw_defer_djob        djob;           /**< \brief ��ʱ����  */
    aw_bool_t                   defer_init;     /**< \brief ��ʱ�����ĳ�ʼ����־  */

    int                         tim;            /**< \brief ��ʱms  */

    int                         err;            /**< \brief ����ֵ  */

    uint16_t                   *head;           /**< \brief ����ͷ��ռ2�ֽ�  */
    char                       *buf;            /**< \brief ���ݻ���  */
    int                         offset;         /**< \brief �����ƫ��  */
    int                         blk_size;       /**< \brief ��Ĵ�С  */
    int                         blk_data_size;  /**< \brief �����ݴ�С  */
    int                         nblks;          /**< \brief �������  */

    int                         free_blk;       /**< \brief ��ȫ�ɾ��Ŀ�����  */
    
    /** \brief ��������ż������п���ڸ�ֵ�������  */
    int                         blk_threshold;  
    int                         newest_blk;     /**< \brief ���µ����ݵĿ��  */
    int                         oldest_blk;     /**< \brief ��ɵ����ݵĿ��  */

    aw_bool_t                   init_flag;      /**< \brief ��ʼ�����  */
    aw_bool_t                   wait_write;     /**< \brief �ȴ�д����  */
    aw_bool_t                   format_req;     /**< \brief ��ʽ����������  */
} aw_log_blk_ctx_t;

/**
 * \brief ��ʼ��һ�� log_blk ʵ��
 *
 * \param[in]       p_ctx       ʵ��ָ��
 * \param[in]       p_dev_name  ���豸��
 * \param[in]       format_req  ��ʽ������, AW_TRUE:�����ʽ��  AW_FALSE:����ʽ��
 *
 * \return �ɹ�����aw_logger_t *���͵�ָ�룬ʧ�ܷ���NULL
 *
 */
aw_logger_t * aw_log_blk_ctor (aw_log_blk_ctx_t *p_ctx,
                               const char       *p_dev_name,
                               aw_bool_t         format_req);

/** @} grp_aw_log_blk */

#ifdef __cplusplus
}
#endif

#endif /* __AW_LOG_BLK_H__ */

/* end of file */
