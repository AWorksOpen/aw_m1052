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
 * \brief AWorks RomFs data analysis
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-30  sdy, first implementation
 * \endinternal
 */

#ifndef __AW_ROMFS_FD_H
#define __AW_ROMFS_FD_H

#ifdef __cplusplus
extern "C" {
#endif

void aw_uint16_to_2bytes(uint8_t *dBuf, uint16_t val);
void aw_uint32_to_4bytes(uint8_t *dBuf, uint32_t val);
uint16_t aw_bytes2R_to_uint16(uint8_t *dBuf);
uint32_t aw_bytes4R_to_uint32(uint8_t *dBuf);

aw_err_t aw_romfs_head_checksum(uint8_t *head_addr);
aw_err_t aw_romfs_open(struct aw_romfs_file *p_fil, char *path);
ssize_t aw_romfs_read (struct aw_romfs_file *p_fil, void *buf, size_t nbyte);
aw_err_t aw_romfs_close(struct aw_romfs_file *p_fil);

#endif /* __AW_ROMFS_FD_H */

/* end of file */
