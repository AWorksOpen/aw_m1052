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

/** \brief ftl��־�洢�������� */
typedef struct aw_log_ftl_ctx {

    void                       *p_ftl_dev;        /**< \brief ftl�豸ָ�� */

#define __LOG_FTL_DEV_NAME_MAX    32

    /** \brief �豸�� */
    char                        dev_name[__LOG_FTL_DEV_NAME_MAX];

    aw_logger_t                 logger;           /**< \brief ��־��ʵ�� */

    struct aw_delayed_work      delay_work;       /**< \brief ���ڶ�ʱ  */
    struct aw_defer_djob        djob;             /**< \brief ��ʱ����  */
    aw_bool_t                   defer_init;       /**< \brief ��ʱ�����ĳ�ʼ����־  */

    int                         tim;              /**< \brief ��ʱms  */

    int                         err;              /**< \brief ����ֵ  */

    uint16_t                   *head;             /**< \brief ����ͷ��ռ2�ֽ�  */
    char                       *buf;              /**< \brief ���ݻ���  */
    int                         offset;           /**< \brief �����ƫ��  */

    int                         sector_size;      /**< \brief ������С  */
    int                         sector_data_size; /**< \brief �������ݿռ��С  */
    int                         nsectors;         /**< \brief ��������  */

    int                         free_sector;      /**< \brief ��ȫ�ɾ�����������  */

    /** \brief ��������ż��������������ڸ�ֵ�������  */
    int                         sector_threshold;
    int                         newest_lsn;       /**< \brief �������ݵ�������  */
    int                         oldest_lsn;       /**< \brief ��ɵ����ݵĿ��  */

    aw_bool_t                   init_flag;        /**< \brief ��ʼ�����  */
    aw_bool_t                   wait_write;       /**< \brief �ȴ�д����  */
    aw_bool_t                   format_req;       /**< \brief ��ʽ����������  */
} aw_log_ftl_ctx_t;


/**
 * \brief ��ʼ��һ�� log_ftl ʵ��
 *
 * \param[in]       p_ctx       ʵ��ָ��
 * \param[in]       p_dev_name  mtd�豸��
 * \param[in]       format_req  ��ʽ������, AW_TRUE:�����ʽ��  AW_FALSE:����ʽ��
 *
 * \return �ɹ�����aw_logger_t *���͵�ָ�룬ʧ�ܷ���NULL
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
