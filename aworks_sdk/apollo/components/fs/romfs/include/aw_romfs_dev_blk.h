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
 * \brief AWorks RomFs dev blk lib
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-30  sdy, first implementation
 * \endinternal
 */

#ifndef __AW_ROMFS_DEV_BLK_H
#define __AW_ROMFS_DEV_BLK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fs/aw_blk_dev.h"

struct aw_rd_blk_info {
    const char  dev_name[AW_BD_NAME_MAX];
    uint32_t    addr;
};

struct aw_rd_blk_private_info {
    struct aw_blk_dev  *p_bd;
    uint32_t            bsize;
};

/**
 * \brief ��̬����romfs dev, �����뵽����
 *
 * \param[in]    name       �豸��
 * \param[in]    dev_info   �豸��Ϣ
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
aw_err_t aw_romfs_dev_blk_create (const char                    *name,
                                  struct aw_rd_blk_info         *dev_info);

#endif /* __AW_ROMFS_DEV_BLK_H */

/* end of file */
