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
 * \brief AWorks RomFs dev array lib
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-30  sdy, first implementation
 * \endinternal
 */

#ifndef __AW_ROMFS_DEV_ARRAY_H
#define __AW_ROMFS_DEV_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

struct aw_rd_array_info {
    const unsigned char *img;
};

/**
 * \brief ��̬����romfs dev, �����뵽����
 *
 * \param[in]    name       �豸��
 * \param[in]    dev_info   �豸��Ϣ
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
aw_err_t aw_romfs_dev_array_create (const char                    *name,
                                    struct aw_rd_array_info       *dev_info);

#endif /* __AW_ROMFS_DEV_ARRAY_H */

/* end of file */
