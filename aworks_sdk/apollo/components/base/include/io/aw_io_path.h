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
 * \brief Â·¾¶¹ÜÀí
 *
 * \internal
 * \par modification history:
 * - 1.00 17-03-24  deo, first implementation
 * \endinternal
 */


#define AW_IO_PATH_LEN_MAX   255


char *aw_io_pwd (void);
void aw_io_pwd_set (char *path);
aw_err_t aw_io_full_path (char *full, const char *path);

