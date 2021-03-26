/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief modbus错误号头文件.
 *
 * \internal
 * \par modification history
 * - 1.03 2019-03-26  cml, use serial interface replace int serial.
 * - 1.02 2015-05-18  cod, fix code style and comments, a little improve.
 * - 1.01 2013-03-29  liuweiyun, fix code style and comments, a little improve.
 * - 1.00 2012-05-03  ygy, first implementation
 * \endinternal
 */

#ifndef __AW_MB_ERR_H /* { */
#define __AW_MB_ERR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_mb_err
 * \copydoc aw_mb_err.h
 * @{
 */

/** \brief 错误号 */
typedef enum aw_mb_err {
    AW_MB_ERR_NOERR,                   /**< \brief 没有错误 */
    AW_MB_ERR_ALLOC_FAIL,              /**< \brief 分配内存失败 */
    AW_MB_ERR_ENOREG,                  /**< \brief 非法注册地址 */
    AW_MB_ERR_EINVAL,                  /**< \brief 非法参数 */
    AW_MB_ERR_EPORTERR,                /**< \brief 端口错误 */
    AW_MB_ERR_ENORES,                  /**< \brief 资源不足 */
    AW_MB_ERR_EIO,                     /**< \brief I/O错误 */
    AW_MB_ERR_ENOTSUP,                 /**< \brief 不支持该功能 */
    AW_MB_ERR_EILLSTATE,               /**< \brief 协议栈非法状态 */
    AW_MB_ERR_ETIMEDOUT,               /**< \brief 超时错误 */
    AW_MB_ERR_ECRC,                    /**< \brief CRC错误  */
    AW_MB_ERR_EFRAME_TICK,             /**< \brief 帧时序错误 */
    AW_MB_ERR_EFRAME_LEN,              /**< \brief 帧长度错误 */
    AW_MB_ERR_EUNEXECPTION_FUNCTION,   /**< \brief 非期待功能错误 */
    AW_MB_ERR_ILLEGAL_FUNCTION,        /**< \brief 非法功能 */
    AW_MB_ERR_ILLEGAL_DATA_ADDRESS,    /**< \brief 非法数据地址 */
    AW_MB_ERR_ILLEGAL_DATA_VALUE,      /**< \brief 非法数据值 */
    AW_MB_ERR_SLAVE_DEVICE_FAILURE,    /**< \brief 从机设备故障 */
    AW_MB_ERR_ACKNOWLEDGE,             /**< \brief 确认 */
    AW_MB_ERR_SLAVE_BUSY,              /**< \brief 从机设备忙 */
    AW_MB_ERR_MEMORY_PARITY_ERROR,     /**< \brief 存储奇偶性差错*/
    AW_MB_ERR_GATEWAY_PATH_FAILED,     /**< \brief 网关路径不可用 */
    AW_MB_ERR_GATEWAY_TGT_FAILED,      /**< \brief 网关目标设备响应失败 */

    AW_MB_ERR_MODE_NO_SUPPORT,         /**< \brief 不支持的通讯模式 */
    AW_MB_ERR_THREAD_FAIL,             /**< \brief 线程创建失败 */
    AW_MB_ERR_OPEN_FILE,               /**< \brief 打开通信文件失败 */
    AW_MB_ERR_UNKOWN_EXP,              /**< \brief 未知异常 */

    AW_MB_ERR_MASTER_RCV_ERR,          /**< \brief 主站接收数据错误  */
} aw_mb_err_t;

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_err */

#endif /* } __AW_MB_ERR_H */

/* end of file */
