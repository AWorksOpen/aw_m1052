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
 * \brief ��־��Ϣ��ͷ�ļ�
 * \note 
 *       
 * \par �򵥾���
 * \code
 * aw_log_dev_t dev_uart;
 *
 * aw_loglib_cfg_init();  
 *
 * // ��ʼ���豸�ṹ��dev_uart
 * aw_loglib_log_dev_init(&dev_uart, COM0, aw_serial_write);
 * aw_loglib_dev_register(&dev_uart); // ע���豸dev_uart
 *   
 * aw_log_msg("Hello, I'm aw_log_msg", 0, 0, 0, 0, 0, 0);
 *
 * aw_loglib_dev_unregister(&dev_uart); // �豸dev_uartȡ��ע�� 
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


/** \brief ����������־���� */
#define AW_LOG_EXPORT_ALL           (0xFFFFFFF)

/** \brief ����λ��ƫ�Ƶ���־ͷ�� */
#define AW_LOG_EXPORT_OFFSET_HEAD   (-0xFFFFFFF)

/**
 * \brief ��־�ȼ�(�Ӹߵ���)
 */
typedef enum aw_log_level {
    AW_LOG_LVL_ASSERT = 0,      /**< \brief ��־�ȼ������� */
    AW_LOG_LVL_ERROR,           /**< \brief ��־�ȼ������� */
    AW_LOG_LVL_WARN,            /**< \brief ��־�ȼ������� */
    AW_LOG_LVL_INFO,            /**< \brief ��־�ȼ�����Ϣ */
    AW_LOG_LVL_DEBUG,           /**< \brief ��־�ȼ������� */
    AW_LOG_LVL_VERBOSE,         /**< \brief ��־�ȼ�����ϸ */
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

/** \brief ��ǩ */
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

/** \brief ��ǰ�����ṹ�� */
struct aw_logger;

/** \brief ����һ������ָ������ */
typedef ssize_t (*aw_pfn_logger_print_t)(struct aw_logger *p_logger,
                                         const char       *p_buf,
                                         size_t            nbytes);

/**
 * \brief �豸�ṹ��.
 * �ýṹ��ʹ��˫�������豸�ڵ���������
 */
typedef struct aw_logger {

    /**\brief �ڵ�  */
    struct aw_list_head        node;

    /**\brief д��־�ӿ�  */
    aw_pfn_logger_print_t      print;

    /**\brief ��logger��id��id��ͬ��logger��ͬʱд��־  */
    int                        id;

    /**\brief ������д��洢�豸����ʱ��ʱ��δ��ʱ���ݶ�����ڻ����У�Ϊ0��ʹ�û���*/
    int                        tim;

    /**\brief ע���־ */
    aw_bool_t                  reg_flag;

    /**\brief ��־���ݴ�С */
    uint32_t                   log_size;

    int                        export_pos;

    /**\brief ��־�����ӿ� */
    aw_err_t (*pfn_export)    (struct aw_logger  *p_logger,
                               int                offset,
                               void              *buf,
                               uint32_t           buf_size);

    /**\brief ��ʼ���ӿ� */
    aw_err_t (*pfn_init)       (struct aw_logger *p_logger);

    /**\brief ����ʼ���ӿ� */
    aw_err_t (*pfn_deinit)     (struct aw_logger *p_logger);

    /**\brief ��� log ���� */
    aw_err_t (*pfn_data_clear) (struct aw_logger *p_logger);

    /**\brief ������ */
    AW_MUTEX_DECL(mutex_lock);
} aw_logger_t;
 


/**
 * \brief ����Ϣ��־
 * \attention �жϲ��ɵ���
 * \retval AW_Ok �ɹ�
 */
aw_err_t aw_log_on (void);

/**
 * \brief �ر���Ϣ��־
 * \attention �жϲ��ɵ���
 * \retval AW_OK �ɹ�
 */
aw_err_t aw_log_off (void);

/**
 * \brief ������Ϣ��ʽ
 * \attention �жϲ��ɵ���
 *
 * \param[in] level : ��Ҫ���ø�ʽ����Ϣ�ȼ���
 * \param[in] set   : ����ֵ��ΪAW_LOG_FMT_*ֵ���߶��AW_LOG_FMT_*�Ļ�ֵ
 *
 * \retval AW_OK �ɹ�
 * \note ������Ϣ��ʽ��LEVEL/TAG [TIME TASK](DIR FUNC:LINE):MSG
 *       LEVEL ��ӦAW_LOG_FMT_LVL����ʾ��Ϣ��������"A/"��
 *       TAG   ��ӦAW_LOG_FMT_TAG����ʾ��ǩ��Ϣ������"aw_log_tag"��
 *       TIME  ��ӦAW_LOG_FMT_TIME����ʾʱ����Ϣ���ⲿ�û�ʵ�ֺ���#aw_loglib_time_get()
 *             �����޸���ʾ��Ϣ�ĸ�ʽ
 *       TASK  ��ӦAW_LOG_FMT_TASK_INFO����ʾ������Ϣ���ⲿ�û�ʵ�ֺ���
 *             #aw_loglib_task_info_get()�����޸���ʾ������Ϣ�ĸ�ʽ
 *       DIR   ��ӦAW_LOG_FMT_DIR����ʾ�ļ�·����һ��ʹ�ú�#__FILE__
 *       FUNC  ��ӦAW_LOG_FMT_FUNC����ʾ�������ƣ�һ��ʹ�ú�#__FUNCTION__
 *       LINE  ��ӦAW_LOG_FMT_LINE����ʾ���õ��������һ��ʹ�ú�#__LINE__
 *       MSG   Ϊ�û���Ҫ����ĸ�ʽ����Ϣ�����û����룬֧�����6��������
 */
aw_err_t aw_log_fmt_set (aw_log_level_t level, size_t set);

/**
 * \brief ���ù��˷�ʽ
 * \attention �жϲ��ɵ���
 *
 * \param[in] level     : ��Ҫ���ø�ʽ����Ϣ�ȼ���
 * \param[in] p_tag     : ��ǩ
 * \param[in] p_keyword : �ؼ���
 *
 * \retval AW_OK �ɹ�
 * \note ���������ֹ��˷�ʽ���ȼ���level������ǩ��tag�����ؼ��֣�keyword��
 *       �ȼ���level��        �� ��Ϣ�ȼ����������õĹ���ֵ�����ȼ���ͬ����ߣ�����Ϣ�Ż�������������level
 *                       ����ΪAW_LOG_LVL_ERROR����ֻ����Ϣ����ΪAW_LOG_LVL_ERROR��
 *                       AW_LOG_LVL_ASSERT����Ϣ���������
 *       ��ǩ��tag��              ��ֻ����Ϣ��ǩ�а������õĹ��˱�ǩ����Ϣ�Ż������
 *       �ؼ��֣�keyword��������Ϣ��ʽ�ϳɵ���Ϣ��ֻ�а����˹ؼ��֣������������Ϣ��ʽ�����ü�#aw_log_fmt_set()
 */
aw_err_t aw_log_filter_set (aw_log_level_t level, const char *p_tag, const char *p_keyword);

/**
 * \brief ���ù��˵ȼ�
 * \attention �жϲ��ɵ��á�
 *
 * \param[in] level : �ȼ�
 *
 * \retval AW_OK �ɹ�
 */
aw_err_t aw_log_filter_lvl_set (aw_log_level_t level);

/**
 * \brief ���ù��˱�ǩ
 * \attention �жϲ��ɵ���
 *
 * \param[in] p_tag : ��ǩ
 *
 * \retval AW_OK �ɹ�
 */
aw_err_t aw_log_filter_tag_set (const char *p_tag);

/**
 * \brief ���ù��˹ؼ���
 * \attention �жϲ��ɵ���
 *
 * \param[in] p_keyword : �ؼ���
 *
 * \retval AW_OK �ɹ�
 */
aw_err_t aw_log_filter_keyword_set (const char *p_keyword);


/**
 * \brief ������Ϣ
 * \attention �жϿɵ��ã�������
 *
 * \param[in] id     ��aw_logger_register()ע����־��ʱָ����id
 * \param[in] p_fmt  ��ʽ���ַ���ָ��   
 * \param[in] arg1   ����1
 * \param[in] arg2   ����2
 * \param[in] arg3   ����3
 * \param[in] arg4   ����4
 * \param[in] arg5   ����5
 * \param[in] arg6   ����6
 *
 * \retval    AW_OK    �ɹ�
 * \retval    -EINVAL  ������Ч
 * \retval    -ETIME   ������Ϣ��ʱ
 * \retval    -ENXIO   �ٵȴ���ʱ����Ϣ���б���ֹ
 * \retval    -EAGAIN  ������Ϣʧ��,�Ժ�����(timeout = AW_MSGQ_NO_WAITʱ) 
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
 * \brief ������չ��Ϣ���첽��
 * \attention �жϿɵ��ã�������
 *
 * \param[in] id     ��aw_logger_register()ע����־��ʱָ����id
 * \param[in] level  ��Ϣ�ȼ���
 * \param[in] p_tag  ��ǩ
 * \param[in] p_file �ļ���
 * \param[in] p_func ���ú�����
 * \param[in] line   ���õ������
 * \param[in] p_fmt  ��ʽ���ַ���ָ��
 * \param[in] arg1   ����1
 * \param[in] arg2   ����2
 * \param[in] arg3   ����3
 * \param[in] arg4   ����4
 * \param[in] arg5   ����5
 * \param[in] arg6   ����6
 *
 * \retval    AW_OK    �ɹ�
 * \retval    -EINVAL  ������Ч
 * \retval    -ETIME   ������Ϣ��ʱ
 * \retval    -ENXIO   �ٵȴ���ʱ����Ϣ���б���ֹ
 * \retval    -EAGAIN  ������Ϣʧ��,�Ժ�����(timeout = AW_MSGQ_NO_WAITʱ)
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
 * \brief ������չ��Ϣ��ͬ����
 * \attention �жϲ��ɵ��ã�������
 *
 * \param[in] id     ��aw_logger_register()ע����־��ʱָ����id
 * \param[in] level  ��Ϣ�ȼ���
 * \param[in] p_tag  ��ǩ
 * \param[in] p_file �ļ���
 * \param[in] p_func ���ú�����
 * \param[in] line   ���õ������
 * \param[in] p_fmt  ��ʽ���ַ���ָ��
 * \param[in] arg1   ����1
 * \param[in] arg2   ����2
 * \param[in] arg3   ����3
 * \param[in] arg4   ����4
 * \param[in] arg5   ����5
 * \param[in] arg6   ����6
 *
 * \retval    AW_OK    �ɹ�
 * \retval    -EINVAL  ������Ч
 * \retval    -ETIME   ������Ϣ��ʱ
 * \retval    -ENXIO   �ٵȴ���ʱ����Ϣ���б���ֹ
 * \retval    -EAGAIN  ������Ϣʧ��,�Ժ�����(timeout = AW_MSGQ_NO_WAITʱ)
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
 * \brief �豸ע��
 * \attention �жϲ��ɵ���
 *
 * \param[in] p_logger      �豸�ṹ��ָ��
 * \param[in] id            ����־����id��ÿ����־����id��Ӧ����Ψһ��
 * \param[in] tim           log��ͬ��ʱ��(ms)��дlog����ʱ������д�뻺�棬
 *                          �ﵽʱ����д�뵽�洢������Ϊ0����ʹ�û��档
 *           
 * \retval    AW_OK       �ɹ�
 * \retval    -AW_EINVAL  ������Ч
 * \retval    -AW_EPERM   ������Ĳ��������ж��е����˱�����
 * \retval    -AW_ENXIO   �����ѹر�,�޷�ע���豸
 * \retval    -EAGAIN     ����æ,������
 * \retval    -ENOTSUP    ��־����printָ��δ��ֵ
 */                                
aw_err_t aw_logger_register(aw_logger_t          *p_logger,
                            int                   id,
                            int                   tim);
                                
/**
 * \brief �豸ע��
 * \attention �жϲ��ɵ���
 * \param[in] p_logger   �豸�ṹ��ָ��
 *           
 * \retval    AW_OK       �ɹ�
 * \retval    -AW_EINVAL  ������Ч
 * \retval    -AW_EPERM   ������Ĳ��������ж��е����˱�����
 * \retval    -AW_ENXIO   �����ѹر�,�޷�ע���豸
 * \retval    -EAGAIN     ����æ,������
 */
aw_err_t aw_logger_unregister(aw_logger_t *p_logger);


/**
 * \brief ������־����
 *
 * \param[in]       p_logger    ��־��ָ��
 * \param[in]       offset      ��ƫ��
 * \param[in]       buf         �������ݵ�buf
 * \param[in]       buf_size    buf�Ĵ�С
 *
 * \retval  > 0         ��ȡ����������
 * \retval  -EINVAL     ������Ч
 * \retval  -ENOTSUP    ��֧�ָù���
 * \retval  -EFAULT     ƫ�Ƴ�����Χ
 * \retval  ELSE        �������󣬾���ɲ鿴errno
 */
aw_err_t aw_log_export (aw_logger_t       *p_logger,
                        int                offset,
                        void              *buf,
                        uint32_t           buf_size);

/**
 * \brief ��ȡ��־���ݵĴ�С
 *
 * \param[in]       p_logger    ��־��ָ��
 *
 * \retval  ����־�����ݴ�С
 */
uint32_t aw_log_data_size_get (aw_logger_t  *p_logger);

/**
 * \brief �����־����
 *
 * \param[in]       p_logger    ��־��ָ��
 *
 * \retval  AW_OK        �ɹ�
 * \retval  ELSE        �������󣬾���ɲ鿴errno
 */
aw_err_t aw_log_data_clear (aw_logger_t       *p_logger);

/**
 * \brief ���loglib��ʼ��
 * \param[in] task_id       ����ID
 * \param[in] msgq_id       ����ID
 * \param[in] p_logbuf      bufָ��
 * \param[in] logbuf_size   buf��С
 *
 * \retval  AW_OK        �ɹ�
 * \retval  -AW_ENOMEM   �ռ�(�ڴ�)����
 * \retval  -AW_EINVAL   ��Ч����
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

