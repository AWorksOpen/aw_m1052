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
 * \brief FLASH translation layer。
 *        该层的主要功能是对flash设备提供磨损均衡、地址映射、坏块管理等功能，
 *        使用该层的读写接口则不再需要关心flash的擦除、坏块等情况。
 *
 * \internal
 * \par modification history:
 * - 1.00 2017-4-15   vih, first implementation.
 * \endinternal
 */

#ifndef __AW_FTL_H
#define __AW_FTL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_ftl
 * \copydoc aw_ftl.h
 * @{
 */

#include "aworks.h"

/**
 * \brief IO控制的选项
 */
typedef enum aw_ftl_ioctl_req {

    /** \brief 设置系统信息的更新时间，单位ms, 默认为0（关闭自动更新） */
    AW_FTL_SYS_INFO_UPDATE_TIME,

    /** \brief 设置在写入了新数据时，会重置系统信息更新的延时时间 。默认为关闭 */
    AW_FTL_SYS_INFO_TARENEW,

    /** \brief 获取设备的名称 */
    AW_FTL_GET_DEV_NAME,

    /** \brief 获取设备扇区数量 */
    AW_FTL_GET_SECTOR_COUNT,

    /** \brief 获取设备扇区大小  */
    AW_FTL_GET_SECTOR_SIZE,

    /** \brief 获取设备存储器大小  */
    AW_FTL_GET_STORAGE_SIZE,

    /**
     * \brief 设置逻辑块清理的阀值。
     * 当逻辑块的脏扇区数量达到该阀值时， 会合并该逻辑块数据。
     * 取值是 0~100 代表百分值，为0 则一有脏扇区就启动清理，为
     * 100则不会自动清理； 默认为100
     */
    AW_FTL_LOGIC_THRESHOLD,

    /**
     * \brief 这只日志块清理的阀值。
     * 当所有日志块的扇区使用数量达到该阀值时， 会合并所有日志块数据。
     * 取值是 0~100 代表百分值，为0 则一有脏扇区就启动清理，  为100则
     * 不会自动清理； 默认为85
     */
    AW_FTL_LOG_THRESHOLD,

    /** \brief 删除逻辑扇区数据   */
    AW_FTL_SECTOR_DAT_DEL,

    /** \brief 格式化FTL设备   */
    AW_FTL_DEV_FORMAT,

} aw_ftl_ioctl_req_t;


/**
 * \brief 创建一个FTL设备，会对该设备进行初始化
 *
 * \param[in] p_ftl_name : 要创建的FTL设备名称
 * \param[in] p_mtd_name : 要使用的MTD设备名称
 *
 * \return 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 */
aw_err_t aw_ftl_dev_create (const char *p_ftl_name,
                            const char *p_mtd_name);

/**
 * \brief 销毁一个FTL设备
 *
 * \param[in] p_ftl_name : FTL设备名
 *
 * \return 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 */
aw_err_t aw_ftl_dev_destroy (const char *p_ftl_name);

/**
 * \brief 获取一个FTL设备
 *
 * \param[in] p_ftl_name : 要创建的FTL设备名称
 *
 * \return 初始化成功:返回 ftl设备指针，否则返回NULL
 */
void * aw_ftl_dev_get (const char *p_ftl_name);

/**
 * \brief 释放一个FTL设备
 *
 * \param[in] p_hd : FTL设备结构体指针
 *
 * \return 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 */
aw_err_t aw_ftl_dev_put (void *p_hd);

/**
 * \brief 读取指定位置的数据
 *
 * \param[in]  p_hd         : FTL设备结构体指针
 * \param[in]  start_addr   : 读取数据的起始地址
 * \param[out] data         : 存放数据的buf
 * \param[in]  dsize        : 存放数据的buf的空间大小
 *
 * \retval > 0              : 读出的数据长度
 * \retval -AW_EINVAL       : 参数无效
 * \retval -AW_ENODATA      : 没有数据
 * \retval -AW_ENOMEM       : 操作的扇区超出有效范围
 * \retval -AW_EIO          : IO错误
 */
aw_err_t aw_ftl_data_read  (void                *p_hd,
                            uint32_t             start_addr,
                            uint8_t             *data,
                            size_t               dsize);

/**
 * \brief 在指定位置中写入数据
 *
 * \param[in]  p_hd         : FTL设备结构体指针
 * \param[in]  start_addr   : 读取数据的起始地址
 * \param[in]  data         : 数据buf
 * \param[in]  dsize        : 数据buf的空间大小
 *
 * \retval > 0              : 写入的数据长度
 * \retval -AW_EINVAL       : 参数无效
 * \retval -AW_ENODATA      : 没有数据
 * \retval -AW_ENOMEM       : 操作的扇区超出有效范围
 * \retval -AW_EIO          : IO错误
 */
aw_err_t aw_ftl_data_write (void                *p_hd,
                            uint32_t             start_addr,
                            uint8_t             *data,
                            size_t               dsize);

/**
 * \brief 逻辑扇区读取
 *
 * \param[in]  p_hd   : FTL设备结构体指针
 * \param[in]  lsn    : 逻辑扇区编号
 * \param[out] data   : 存放数据的buf，注意该buf不能小于一个扇区的空间
 *
 * \retval AW_OK        :  成功
 * \retval -AW_EINVAL   :  参数无效
 * \retval -AW_ENODATA  :  没有数据
 * \retval -AW_ENOMEM   :  操作的扇区超出有效范围
 * \retval -AW_EIO      :  IO错误
 */
aw_err_t aw_ftl_sector_read (void              *p_hd,
                             uint32_t           lsn,
                             uint8_t           *data);

/**
 * \brief 逻辑扇区写入
 *
 * \param[in] p_hd   : FTL设备结构体指针
 * \param[in] lsn    : 逻辑扇区编号
 * \param[in] data   : 要写入的数据， 注意该buf不能小于一个扇区的空间
 *
 * \return 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 */
aw_err_t aw_ftl_sector_write (void              *p_hd,
                              uint32_t           lsn,
                              uint8_t           *data);

/**
 * \brief 将ftl设备注册成块设备
 *
 * \param[in] p_ftl_path  : ftl设备名，注册完后块设备也使用该名
 *
 * \return 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 */
aw_err_t aw_ftl_blk_register (const char *p_ftl_path);

/**
 * \brief 反注册ftl注册成的块设备
 *
 * \param[in] p_ftl_path  : ftl设备名，注册完后块设备也使用该名
 *
 * \return 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 */
aw_err_t aw_ftl_blk_unregister (const char *p_ftl_path);

/**
 * \brief io控制
 *
 * \param[in]     p_hd    : FTL设备结构体
 * \param[in]     req     : io请求
 * \param[in,out] p_arg   : 数据
 *
 * \return 操作成功返回AW_OK, 否则返回aw_err_t错误类型
 *
 * \par 范例
 * \code
 *
 *  uint32_t arg = 0;
 *  char     name[AW_BD_NAME_MAX];
 *
 *  //设置系统信息的保存间隔为60000ms, 时间为0则不自动保存
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_SYS_INFO_UPDATE_TIME, (void *)60000);
 *
 *  //设置系统信息的保存间隔为60000ms, 时间为0则不自动保存
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_SYS_INFO_TARENEW, (void *)TURE);
 *
 *  //获取设备的名称
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_GET_DEV_NAME, (void *)name);
 *
 *  //获取设备的扇区大小
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_GET_SECTOR_SIZE, (void *)&arg);
 *
 *  //获取设备的扇区数量
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_GET_SECTOR_COUNT, (void *)&arg);
 *
 *  //设置清理逻辑块的阀值，当逻辑块的dirty扇区达到该逻辑块扇区数量的79%时，
 *  //会启动清理
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_LOGIC_THRESHOLD, (void *)79);
 *
 *  //设置清理日志块的阀值，当所有日志块的使用扇区达到总日志块扇区数量的87%时，
 *  //会启动清理
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_LOG_THRESHOLD, (void *)87);
 *
 *  //删除逻辑扇区 200 的数据
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_SECTOR_DAT_DEL, (void *)200);
 *
 *  // 异步格式化 ftl
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_DEV_FORMAT, NULL);
 *  // 同步格式化
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_DEV_FORMAT, (void *)1);
 *
 * \endcode
 */
aw_err_t aw_ftl_dev_ioctl (void                *p_hd,
                           aw_ftl_ioctl_req_t   req,
                           void                *p_arg);

/**
 * \brief FTL对象的垃圾回收接口
 */
void aw_ftl_garbage_collect (void);


/** @}  grp_aw_if_ftl */

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __AW_FTL_H */

/* end of file */
