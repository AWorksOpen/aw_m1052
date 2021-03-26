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
 * \brief GNSS 接口头文件
 *
 * \par 使用示例
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
 * \brief AWBus GNSS 服务实例的相关信息
 */
struct awbl_gnss_servinfo {

    /** \brief GNSS 设备编号 */
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
 * \brief AWBus GNSS 服务函数
 */
struct awbl_gnss_servfunc {

    /** \brief GNSS 设备开启 */
    aw_err_t (*pfn_start)(void *p_cookie);

    /** \brief GNSS 设备关闭 */
    aw_err_t (*pfn_end)(void *p_cookie);

    /** \brief GNSS 设备io控制 */
    aw_err_t (*dev_ioctl)(void *p_cookie, aw_gnss_ioctl_req_t req, void *arg);

    /** \brief 获取数据接口； 可缺省，缺省时使用 serial 读，此时需要设置好 awbl_gnss_com 参数 */
    aw_err_t (*dat_get)(void                  *p_cookie,
                        aw_gnss_data_type_t    type,
                        void                  *ptr,
                        uint8_t                len);
};

/**
 * \brief AWBus GNSS 服务实例
 */
struct awbl_gnss_service {

    struct awbl_gnss_service        *p_next;     /**< \brief 下一个GNSS设备地址 */

    const struct awbl_gnss_servinfo *p_servinfo; /**< \brief GNSS 设备服务信息 */

    const struct awbl_gnss_servfunc *p_servfunc; /**< \brief GNSS 设备服务函数 */

    struct awbl_nmea_0183_msg        data;       /** \brief nmea解码后的数据缓存 */

    struct awbl_gnss_com            *p_com;      /**< \brief GNSS 设备的com结构体 */

    uint32_t                         func_flag;  /**< \brief 存放解码类型的标记 */

    uint32_t                         sys_freq;   /**< \brief 系统频率 */

    uint32_t                         begin_tim;  /**< \brief 数据的有效的起始时间(ms) */

    uint32_t                         valid_tim;  /**< \brief 数据的有效时间(ms) */

    aw_bool_t                        dev_is_enable;

    awbl_gnss_init_stat_t            init_stat;

    /** \brief 用于接收数据超时时发送信号量  */
    struct aw_delayed_work           timout_work;

    AW_SEMB_DECL(                    timout_sem);

    AW_MUTEX_DECL(op_mutex);

    void *p_cookie;
};


/**
 * \brief AWBus GNSS 接口模块初始化
 *
 * \attention 本函数应当在 awbl_dev_init2() 之后调用
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

