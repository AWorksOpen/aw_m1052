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
 * \brief GNSS �ӿ�ͷ�ļ�
 *
 * \par ʹ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-12-28  vih, first implementation.
 * \endinternal
 */

#ifndef __AWBL_GNSS_H
#define __AWBL_GNSS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_gnss.h"
#include "aw_time.h"
#include "aw_sem.h"
#include "aw_delayed_work.h"
#include "awbl_nmea_0183_decode.h"

typedef enum awbl_gnss_init_stat{
    AWBL_GNSS_INIT_WAIT = 0,
    AWBL_GNSS_INIT_OK,
    AWBL_GNSS_INIT_FAILED

} awbl_gnss_init_stat_t;

/**
 * \brief AWBus GNSS ����ʵ���������Ϣ
 */
struct awbl_gnss_servinfo {

    /** \brief GNSS �豸��� */
    int gnss_id;
};

struct awbl_gnss_com {
    int      id;
    int      baud;
    uint32_t timout;

    uint8_t *buf;
    uint32_t size;
};

/**
 * \brief AWBus GNSS ������
 */
struct awbl_gnss_servfunc {

    /** \brief GNSS �豸���� */
    aw_err_t (*pfn_start)(void *p_cookie);

    /** \brief GNSS �豸�ر� */
    aw_err_t (*pfn_end)(void *p_cookie);

    /** \brief GNSS �豸io���� */
    aw_err_t (*dev_ioctl)(void *p_cookie, aw_gnss_ioctl_req_t req, void *arg);

    /** \brief ��ȡ���ݽӿڣ� ��ȱʡ��ȱʡʱʹ�� serial ������ʱ��Ҫ���ú� awbl_gnss_com ���� */
    aw_err_t (*dat_get)(void                  *p_cookie,
                        aw_gnss_data_type_t    type,
                        void                  *ptr,
                        uint8_t                len);
};

/**
 * \brief AWBus GNSS ����ʵ��
 */
struct awbl_gnss_service {

    struct awbl_gnss_service        *p_next;     /**< \brief ��һ��GNSS�豸��ַ */

    const struct awbl_gnss_servinfo *p_servinfo; /**< \brief GNSS �豸������Ϣ */

    const struct awbl_gnss_servfunc *p_servfunc; /**< \brief GNSS �豸������ */

    struct awbl_nmea_0183_msg        data;       /** \brief nmea���������ݻ��� */

    struct awbl_gnss_com            *p_com;      /**< \brief GNSS �豸��com�ṹ�� */

    uint32_t                         func_flag;  /**< \brief ��Ž������͵ı�� */

    uint32_t                         sys_freq;   /**< \brief ϵͳƵ�� */

    uint32_t                         begin_tim;  /**< \brief ���ݵ���Ч����ʼʱ��(ms) */

    uint32_t                         valid_tim;  /**< \brief ���ݵ���Чʱ��(ms) */

    aw_bool_t                        dev_is_enable;

    awbl_gnss_init_stat_t            init_stat;

    /** \brief ���ڽ������ݳ�ʱʱ�����ź���  */
    struct aw_delayed_work           timout_work;

    AW_SEMB_DECL(                    timout_sem);

    AW_MUTEX_DECL(op_mutex);

    void *p_cookie;
};


/**
 * \brief AWBus GNSS �ӿ�ģ���ʼ��
 *
 * \attention ������Ӧ���� awbl_dev_init2() ֮�����
 */
void awbl_gnss_init (void);

aw_err_t awbl_gnss_send_wait_res (
    struct awbl_gnss_service         *p_this,
    uint8_t                          *p_send,
    uint8_t                          *p_res,
    uint32_t                          timout);

#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_GNSS_H */

/* end of file */

