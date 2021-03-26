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
 * \brief system information
 *
 * \internal
 * \par modification history:
 * - 1.00 17-04-15   vih, first implementation.
 * \endinternal
 */

#ifndef __AW_FTL_SYS_H
#define __AW_FTL_SYS_H

/**
 * \addtogroup grp_aw_if_ftl_sys
 * \copydoc aw_ftl_sys.h
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


typedef struct aw_ftl_sys_info {
    /** \brief 系统信息块数组 */
    uint16_t *array;

    /** \brief 每个系统信息块的有效性 */
    aw_bool_t  *blk_valid;

    /** \brief 系统信息更新的间隔时间(ms),为0则不会自动更新 */
    uint32_t update_time;

    /** \brief 存放系统信息的物理块的扇区的使用计数 */
    uint16_t sector_use_cnt;

    /** \brief 存放一次系统信息的所需要的扇区数量 */
    uint16_t need_sectors;

    /** \brief 需要的系统信息块的数量，是从数据块里获取 */
    uint8_t  nsys_blk;

    /** \brief 存放系统信息的物理块号的有效性 */
    aw_bool_t   pbn_valid;

    /** \brief 系统信息需要更新 */
    aw_bool_t   need_update;

    /** \brief 系统信息正在等待更新 */
    aw_bool_t   waiting_update;

    /** \brief 在写入新数据时会重置系统信息的等待时间 */
    aw_bool_t   time_auto_renew;

}aw_ftl_sys_info_t;

struct aw_ftl_dev;

/**
 * \brief 初始化
 *
 * \param[in] p_ftl           : FTL设备结构体
 *
 * \retval  AW_OK  : 操作成功
 * \retval  -ENOMEM: 内存不足
 */
aw_err_t aw_ftl_sys_info_init (struct aw_ftl_dev *p_ftl);
aw_err_t aw_ftl_sys_info_deinit (struct aw_ftl_dev *p_ftl);

/** \brief 将系统配置信息存储到flash中 */
int aw_ftl_sys_info_update (struct aw_ftl_dev *p_ftl);

/**
 * \brief 从flahs中下载系统配置信息
 *
 * \param[in] p_ftl : FTL设备结构体
 *
 * \retval  AW_OK  : 操作成功
 */
int aw_ftl_sys_info_download (struct aw_ftl_dev *p_ftl);

int aw_ftl_sys_info_clear (struct aw_ftl_dev *p_ftl);




/** @}  grp_aw_if_ftl_sys */

#ifdef __cplusplus
}
#endif

#endif /* __AW_FTL_SYS_H */



/* end of file */
