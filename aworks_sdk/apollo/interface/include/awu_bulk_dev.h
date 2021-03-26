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
 * \brief USB Bulk Device 接口
 *
 * \internal
 * \par modification history:
 * - 1.00 13-07-09  zen, first implementation
 * \endinternal
 */

#ifndef __AWU_BULK_DEV_H
#define __AWU_BULK_DEV_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/** \brief 错误返回码 (负值) */
#define AWU_OK              0
#define AWU_NOTSTARTED      1
#define AWU_INPROGRESS      2
#define AWU_PERM            3
#define AWU_NOENT           4
#define AWU_IO              5
#define AWU_NXIO            6
#define AWU_NOMEM           7
#define AWU_BUSY            8
#define AWU_NODEV           9
#define AWU_INVAL           10
#define AWU_NOTSUP          11
#define AWU_TIMEDOUT        12
#define AWU_SUSPENDED       13
#define AWU_UNKNOWN         14
#define AWU_TEST_FAILED     15
#define AWU_STATE           16
#define AWU_STALLED         17
#define AWU_PARAM           18
#define AWU_ABORTED         19
#define AWU_SHORT           20

/** \brief 永久等待 */
#define AWU_WAIT_FOREVER    (-1)

/** \brief 设备信息 */
typedef struct {
    uint16_t  vid;                    /**< \brief 设备的 Vendor ID */
    uint16_t  pid;                    /**< \brief 设备的 Product ID */
    uint16_t  release;                /**< \brief 设备的版本号 */
    char     *p_product_name;         /**< \brief 设备描述 */
    char     *p_product_manufacturer; /**< \brief 设备制造商描述 */
    char     *p_product_serial;       /**< \brief 设备序列号 */
    int       self_powered;           /**< \brief 是否自供电设备 1-是 0-否 */
    uint32_t  max_power;              /**< \brief 设备需要的最大电力 (mA) */
} awu_bulk_devinfo_t;


/** \brief 这些回调函数由 Bulk 设备在对应的阶段调用, 每个回调都是可选的 */
typedef struct {
    int  (*init)(void *context);    /**< \brief 设备初始化时调用 */
    int  (*enable)(void *context);  /**< \brief 设备就绪(与主机连接成功)时调用 */
    int  (*disable)(void *context); /**< \brief 设备断开(与主机断开)时调用 */
    int  (*suspend)(void *context); /**< \brief 设备挂起时调用 */
    int  (*resume)(void *context);  /**< \brief 设备恢复时调用 */
    int  (*uninit)(void *context);  /**< \brief 设备卸载时调用 */
} awu_bulk_app_cb_t;


/**
 * \brief 初始化 Bulk 设备
 *
 * \param p_devinfo     Bulk 设备的初始化信息，请参考 awu_bulk_devinfo_t
 * \param p_app_cb      应用程序提供的回调函数，在Bulk 设备的不同状态下被回调,
 *                      请参考 awu_bulk_app_cb_t
 * \param p_app_ctx     Bulk 应用程序回调函数上下文，p_app_cb 中回调函数的参数
 *
 * \retval AWB_OK       成功
 * \retval 负值         错误，具体含义请参考“错误返回码”
 */
int awu_bulk_dev_init (awu_bulk_devinfo_t *p_devinfo,
                       awu_bulk_app_cb_t  *p_app_cb,
                       void               *p_app_ctx);

/**
 * \brief Bulk 设备是否已初始化并就绪(已成功连接主机)
 *
 * \retval  1    已初始化并就绪
 * \retval  0   尚未初始化未就绪
 */
int awu_bulk_dev_is_ready(void);

/**
 * \brief 启动 Bulk 设备
 *
 * \attention 本函数应当在设备初始化以及配置(awu_bulk_dev_init())完成之后调用。
 *            它将会初始化硬件连接并更新端点配置。当USB线缆连接到设备时，
 *            主机将会开始此设备的枚举。
 *
 * \retval AWB_OK       成功
 * \retval 负值         错误，具体含义请参考“错误返回码”
 */
int awu_bulk_dev_run(void);

/**
 * \brief 读 Bulk 管道
 *
 * \param pipe    Bulk 管道号 (1, 2... ), 管道个数请参考平台说明
 * \param p_buf   读缓冲区
 * \param nbytes  读取数据个数
 * \param timeout 超时 (ms) AWU_WAIT_FOREVER(永久等待)
 *
 * \retval >=0    实际读取到的数据个数
 * \retval 负值   错误，具体含义请参考“错误返回码”
 */
ssize_t awu_bulk_read(unsigned  pipe,
                      void     *p_buf,
                      size_t    nbytes,
                      int       timeout);

/**
 * \brief 写 Bulk 管道
 *
 * \param pipe    Bulk 管道号 (1, 2... ), 管道个数请参考平台说明
 * \param p_buf   写缓冲区
 * \param nbytes  写数据个数
 * \param timeout 超时 (ms) AWU_WAIT_FOREVER(永久等待)
 *
 * \retval >=0    实际写入的数据个数
 * \retval 负值   错误，具体含义请参考“错误返回码”
 */
ssize_t awu_bulk_write(unsigned    pipe,
                       const void *p_buf,
                       size_t      nbytes,
                       int         timeout);

/**
 * \brief 取消 Bulk 管道上的读操作
 *
 * \param pipe    Bulk 管道号 (1, 2... ), 管道个数请参考平台说明
 */
void awu_bulk_read_abort(unsigned pipe);

/**
 * \brief 取消 Bulk 管道上的写操作
 *
 * \param pipe    Bulk 管道号 (1, 2... ), 管道个数请参考平台说明
 */
void awu_bulk_write_abort(unsigned pipe);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWU_BULK_DEV_H */

/* end of file */
