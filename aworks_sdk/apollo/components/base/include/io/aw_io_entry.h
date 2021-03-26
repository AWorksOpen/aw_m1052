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
 * \brief I/O entry manage
 *
 * \internal
 * \par modification history:
 * - 1.00 16-12-08  deo, first implementation
 * \endinternal
 */


#ifndef __AW_IO_ENTRY_H
#define __AW_IO_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_iosys_entry
 * @{
 */


#include "aw_io_device.h"



/**
 * \brief I/O 设备入口结构体
 */
struct aw_io_entry {
    struct aw_iodev  *p_dev;
    void             *p_ent;
};


/**
 * \brief 初始化IO入口管理器
 *
 * \param[in]        p_ents     入口结构体指针
 * \param[in]        count      入口数量
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_io_entry_init (struct aw_io_entry *p_ents, int count);


/**
 * \brief 获取入口的索引
 *
 * \param[in]        p_ents     入口结构体指针
 *
 * \return 返回 >=0 为入口索引，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_io_entry_to_index (struct aw_io_entry *p_entry);


/**
 * \brief 通过索引获取入口指针
 *
 * \param[in]        index     入口的索引
 *
 * \return 成功返回入口指针，否则返回NULL
 */
struct aw_io_entry *aw_io_index_to_entry (int index);


/**
 * \brief 分配一个入口
 *
 * \return 成功返回入口指针，否则返回NULL
 */
struct aw_io_entry *aw_io_entry_alloc (void);


/**
 * \brief 释放一个alloc出来的入口
 *
 * \param[in]        p_ents     入口结构体指针
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_io_entry_free (struct aw_io_entry *p_entry);


/**
 * \brief 检查入口索引的有效性
 *
 * \param[in]        index     入口的索引
 *
 * \return 有效返回 TRUE，无效返回 AW_FALSE
 */
aw_bool_t aw_io_entry_valid (int index);

/** @} grp_iosys_entry */

#ifdef __cplusplus
}
#endif

#endif /* __AW_IO_ENTRY_H */

/* end of file */
