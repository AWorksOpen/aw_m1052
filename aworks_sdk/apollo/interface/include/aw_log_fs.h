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


#ifndef __AW_LOG_FS_H__
#define __AW_LOG_FS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_log_fs
 * \copydoc aw_log_fs.h
 * @{
 */

#include "aworks.h"
#include "aw_delayed_work.h"
#include "aw_sys_defer.h"
#include "aw_loglib.h"

#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/aw_dirent.h"
#include "io/sys/aw_stat.h"

/** \brief fs��־�洢�������� */
typedef struct aw_log_fs_ctx {

#define _FILE_PATH_MAX_LEN   128
    
    /** \brief �ļ��� */
    char                        file_name[_FILE_PATH_MAX_LEN];  
    char                        file_name_old[_FILE_PATH_MAX_LEN];

    aw_logger_t                 logger;       /**< \brief ��־��ʵ�� */

    struct aw_delayed_work      delay_work;   /**< \brief ���ڶ�ʱ  */
    struct aw_defer_djob        defer_job;    /**< \brief ��ʱ����  */
    aw_bool_t                   defer_init;   /**< \brief ��ʱ�����ĳ�ʼ����־  */
    uint32_t                    tim;          /**< \brief ��ʱms  */
    
    int                         handle;       /**< \brief �ļ����  */
    int                         handle_old;   /**< \brief �ļ����  */
    int                         file_size;    /**< \brief handle �ļ���С  */

    int                         err;          /**< \brief ����ֵ  */
    int                         offset;       /**< \brief ����ƫ��  */
    int16_t                     resource_cnt; /**< \brief ʹ����Դ����  */
    char                       *buf;          /**< \brief ����  */
    aw_bool_t                   init_flag;    /**< \brief ��ʼ����־  */
    aw_bool_t                   format_req;   /**< \brief ��ʽ����������  */

    /** \brief log �ļ���д������������  */
    uint32_t                    storage_size; 

} aw_log_fs_ctx_t;

/**
 * \brief ��ʼ��һ��log_fsʵ��
 *
 * \param[in]       p_ctx       ʵ��ָ��
 * \param[in]       p_file_path �ļ���
 * \param[in]       storage_size �ļ����������
 * \param[in]       format_req  ��ʽ������, TRUE:�����ʽ��  FALSE:����ʽ��
 *
 * \return �ɹ�����aw_logger_t *���͵�ָ�룬ʧ�ܷ���NULL
 *
 */
aw_logger_t * aw_log_fs_ctor (aw_log_fs_ctx_t *p_ctx,
                              const char      *p_file_path,
                              uint32_t         storage_size,
                              aw_bool_t        format_req);

/** @} grp_aw_log_fs */

#ifdef __cplusplus
}
#endif

#endif /* __AW_LOG_FS_H__ */

/* end of file */
