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
 * \brief SPI-Flash NVRAM �ӿ�
 *
 * \internal
 * \par modification history:
 * - 1.00 17-08-31  may, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "aworks.h"
#include "driver/norflash/awbl_spi_flash.h"
#include "driver/norflash/awbl_spi_flash_nvram.h"


#define __AWDEV_TO_SPIFLASH(p_awdev) \
            AW_CONTAINER_OF(AW_CONTAINER_OF( \
                                p_awdev, \
                                struct awbl_spi_device, \
                                super), \
                            struct awbl_spi_flash_dev, \
                            spi)

#define __AWDEV_TO_SPIFLASH_INFO(p_awdev) \
            (struct awbl_spi_flash_devinfo *)AWBL_DEVINFO_GET(p_awdev)




/******************************************************************************/
aw_err_t awbl_spi_flash_nvram_get (struct awbl_dev *p_awdev,
                                   char            *p_name,
                                   int              unit,
                                   void            *p_buf,
                                   int              offset,
                                   int              len)
{
    struct awbl_spi_flash_dev           *p_flash;
    struct awbl_spi_flash_devinfo       *p_info;
    struct awbl_spi_flash_nvram_info    *p_ninfo;
    const struct awbl_nvram_segment     *p_seg;
    aw_err_t                             ret;
    int                                  i;
    int                                  copylen;

    p_flash = __AWDEV_TO_SPIFLASH(p_awdev);
    p_info  = __AWDEV_TO_SPIFLASH_INFO(p_awdev);
    p_ninfo = (struct awbl_spi_flash_nvram_info *)p_info->p_nvram_info;
    p_seg   = p_ninfo->p_seglst;

    for (i = 0; i < p_ninfo->seglst_count; i++) {
        if ((p_seg->unit == unit) &&
            (strcmp(p_seg->p_name, p_name) == 0)) {
            copylen = len;
            if (offset + len > p_seg->seg_size) {
                copylen = p_seg->seg_size - offset;
                if (copylen < 0) {
                    return -AW_EIO;
                }
            }
            if (p_seg->seg_addr + offset + copylen  >
                (p_info->block_size * p_info->nblocks)) {
                return  -AW_ENXIO;
            }

            ret = awbl_spi_flash_read(p_flash,
                                      p_seg->seg_addr + offset,
                                      (uint8_t *)p_buf,
                                      copylen,
                                      NULL);

            return ret != AW_OK ? -AW_EIO : AW_OK;
        }
        p_seg++;
    }

    return -AW_ENXIO;
}




/******************************************************************************/
aw_err_t awbl_spi_flash_nvram_set (struct awbl_dev *p_awdev,
                                   char            *p_name,
                                   int              unit,
                                   char            *p_buf,
                                   int              offset,
                                   int              len)
{
    struct awbl_spi_flash_dev           *p_flash;
    struct awbl_spi_flash_devinfo       *p_info;
    struct awbl_spi_flash_nvram_info    *p_ninfo;
    const struct awbl_nvram_segment     *p_seg;
    aw_err_t                             ret;
    int                                  i;
    int                                  copylen;

    p_flash = __AWDEV_TO_SPIFLASH(p_awdev);
    p_info  = __AWDEV_TO_SPIFLASH_INFO(p_awdev);
    p_ninfo = (struct awbl_spi_flash_nvram_info *)p_info->p_nvram_info;
    p_seg   = p_ninfo->p_seglst;

    for (i = 0; i < p_ninfo->seglst_count; i++) {
        if ((p_seg->unit == unit) && (strcmp(p_seg->p_name, p_name) == 0)) {

            copylen = len;
            if (offset + len > p_seg->seg_size) {
                copylen = p_seg->seg_size - offset;
                if (copylen < 0) {
                    return -AW_EIO;
                }
            }
            if (p_seg->seg_addr + offset + copylen  >
                (p_info->block_size * p_info->nblocks)) {
                return  -AW_ENXIO;
            }
            if (NULL == p_ninfo->p_blk_buf) {
                return  -AW_EINVAL;
            }

            uint32_t  block_addr_pre    = 0;
            uint32_t  block_addr_next   = 0;
            uint32_t  current_addr      = 0;
            uint32_t  copy_size         = 0;
            uint32_t  j                 = 0;
            uint32_t  k                 = 0;
            uint8_t  *p_write_buf       = NULL;

            /* ���㵱ǰ�Ĵ洢��ַ */
            current_addr    = j + p_seg->seg_addr + offset;
            block_addr_pre  = current_addr / p_info->block_size * 
                              p_info->block_size;
            block_addr_next = block_addr_pre + p_info->block_size;

            /*
             *  ������д��spiflash��
             *  1.  ��д�����ʼ��ַ���Կ��С�������д���������С�ڿ��С��
             *      ����Ҫ�Ƚ���ǰ�����ݶ��������͵�ǰ���ݺϲ���������ǰ�飬��
             *      д��
             *  2.  ����ֱ�Ӳ�����ǰ�飬д������
             */
            for (j = 0; j < copylen;) {

                copy_size = (copylen - j) < (block_addr_next - current_addr)? \
                            (copylen - j): \
                            (block_addr_next - current_addr);

                /* ��һ����� */
                if (copy_size < p_info->block_size) {

                    /* ��ȡ���� */
                    ret = awbl_spi_flash_read(p_flash,
                                              block_addr_pre,
                                              p_ninfo->p_blk_buf,
                                              p_info->block_size,
                                              NULL);
                    if (ret != AW_OK) {
                        return  ret;
                    }

                    /* �ϲ����� */
                    memcpy(&p_ninfo->p_blk_buf[current_addr % 
                        p_info->block_size], &p_buf[j], copy_size);

                    p_write_buf = p_ninfo->p_blk_buf;


                /* �ڶ������ */
                } else {
                    p_write_buf = (uint8_t *)(p_buf+j);
                }

                /* ��������  */
                ret = awbl_spi_flash_erase(p_flash, block_addr_pre);
                if (ret != AW_OK) {
                    return  ret;
                }

                /* ������д�� */
                for (k = 0; k < p_info->block_size; k += p_info->page_size) {

                    ret = awbl_spi_flash_write(p_flash,
                                               block_addr_pre + k,
                                               p_write_buf + k,
                                               p_info->page_size,
                                               NULL);
                    if (ret != AW_OK) {
                        return  ret;
                    }
                }

                j += copy_size;
                current_addr += copy_size;
                block_addr_pre += p_info->block_size;
                block_addr_next = block_addr_pre + p_info->block_size;
            }

            return  AW_OK;
        }
        p_seg++;
    }

    return -AW_ENXIO;
}

/* end of file */
