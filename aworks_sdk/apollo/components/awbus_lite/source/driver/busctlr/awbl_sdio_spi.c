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
 * \brief SDIO SPI mode driver
 *
 * \internal
 * \par modification history:
 * - 1.00 14-10-29  deo, first implementation
 * - 1.01 17-01-06  xdn, adaptive new sdio interface
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_spi.h"
#include "aw_gpio.h"
#include "aw_delay.h"
#include "aw_assert.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_sdiobus.h"
#include "driver/busctlr/awbl_sdio_spi.h"

/*******************************************************************************
  global
*******************************************************************************/
/** \brief Table for CRC-7 (polynomial x^7 + x^3 + 1) */
const uint8_t crc7_syndrome_table[256] = {
    0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f,
    0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,
    0x19, 0x10, 0x0b, 0x02, 0x3d, 0x34, 0x2f, 0x26,
    0x51, 0x58, 0x43, 0x4a, 0x75, 0x7c, 0x67, 0x6e,
    0x32, 0x3b, 0x20, 0x29, 0x16, 0x1f, 0x04, 0x0d,
    0x7a, 0x73, 0x68, 0x61, 0x5e, 0x57, 0x4c, 0x45,
    0x2b, 0x22, 0x39, 0x30, 0x0f, 0x06, 0x1d, 0x14,
    0x63, 0x6a, 0x71, 0x78, 0x47, 0x4e, 0x55, 0x5c,
    0x64, 0x6d, 0x76, 0x7f, 0x40, 0x49, 0x52, 0x5b,
    0x2c, 0x25, 0x3e, 0x37, 0x08, 0x01, 0x1a, 0x13,
    0x7d, 0x74, 0x6f, 0x66, 0x59, 0x50, 0x4b, 0x42,
    0x35, 0x3c, 0x27, 0x2e, 0x11, 0x18, 0x03, 0x0a,
    0x56, 0x5f, 0x44, 0x4d, 0x72, 0x7b, 0x60, 0x69,
    0x1e, 0x17, 0x0c, 0x05, 0x3a, 0x33, 0x28, 0x21,
    0x4f, 0x46, 0x5d, 0x54, 0x6b, 0x62, 0x79, 0x70,
    0x07, 0x0e, 0x15, 0x1c, 0x23, 0x2a, 0x31, 0x38,
    0x41, 0x48, 0x53, 0x5a, 0x65, 0x6c, 0x77, 0x7e,
    0x09, 0x00, 0x1b, 0x12, 0x2d, 0x24, 0x3f, 0x36,
    0x58, 0x51, 0x4a, 0x43, 0x7c, 0x75, 0x6e, 0x67,
    0x10, 0x19, 0x02, 0x0b, 0x34, 0x3d, 0x26, 0x2f,
    0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
    0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04,
    0x6a, 0x63, 0x78, 0x71, 0x4e, 0x47, 0x5c, 0x55,
    0x22, 0x2b, 0x30, 0x39, 0x06, 0x0f, 0x14, 0x1d,
    0x25, 0x2c, 0x37, 0x3e, 0x01, 0x08, 0x13, 0x1a,
    0x6d, 0x64, 0x7f, 0x76, 0x49, 0x40, 0x5b, 0x52,
    0x3c, 0x35, 0x2e, 0x27, 0x18, 0x11, 0x0a, 0x03,
    0x74, 0x7d, 0x66, 0x6f, 0x50, 0x59, 0x42, 0x4b,
    0x17, 0x1e, 0x05, 0x0c, 0x33, 0x3a, 0x21, 0x28,
    0x5f, 0x56, 0x4d, 0x44, 0x7b, 0x72, 0x69, 0x60,
    0x0e, 0x07, 0x1c, 0x15, 0x2a, 0x23, 0x38, 0x31,
    0x46, 0x4f, 0x54, 0x5d, 0x62, 0x6b, 0x70, 0x79
};
const uint16_t crc_itu_t_table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

/*******************************************************************************
  macro define
*******************************************************************************/
#define __SDIO_SPI_HOST_TO_INFO_DECL(p_info, p_host) \
            awbl_spi_sdio_dev_info_t *p_info = \
                (awbl_spi_sdio_dev_info_t *)AWBL_DEVINFO_GET( \
                    &(((struct awbl_sdio_host *)p_host)->super.super))

#define __SDIO_SPI_HOST_TO_DEV_DECL(p_dev, p_host) \
            awbl_spi_sdio_dev_t *p_dev = \
                AW_CONTAINER_OF(p_host, awbl_spi_sdio_dev_t, host)

#define __SDIO_SPI_AWDEV_TO_INFO_DECL(p_info, p_awdev) \
            awbl_spi_sdio_dev_info_t *p_info = \
                (awbl_spi_sdio_dev_info_t *)AWBL_DEVINFO_GET(p_awdev)

#define __SDIO_SPI_AWDEV_TO_DEV_DECL(p_dev, p_awdev) \
            awbl_spi_sdio_dev_t *p_dev = \
                AW_CONTAINER_OF( \
                    AW_CONTAINER_OF(p_awdev, struct awbl_sdio_host, super), \
                    awbl_spi_sdio_dev_t, \
                    host)


/******************************************************************************/
aw_local uint16_t __sdio_spi_crc16(
                        uint16_t       crc,
                        const uint8_t  *buffer,
                        uint16_t       len)
{
    while (len--) {
        crc = (crc << 8) ^
                crc_itu_t_table[((crc >> 8) ^ (*buffer)) & 0xff];
        buffer++;
    }
    return crc;
}

/******************************************************************************/
aw_local uint8_t __sdio_spi_crc7(
                        uint8_t        crc,
                        const uint8_t  *buffer,
                        uint8_t        len)
{
    while (len--) {
        crc = crc7_syndrome_table[(crc << 1) ^ (*buffer)];
        buffer++;
    }
    return crc;
}

/******************************************************************************/
aw_local aw_err_t __sdio_spi_rec_data (
                        awbl_spi_sdio_dev_t *p_dev,
                        uint8_t             *p_buf,
                        uint32_t            len)
{
    struct aw_spi_message  spi_msg;
    struct aw_spi_transfer trans;
    uint8_t                temp = 0;
    uint32_t               timeout;
    uint16_t               crc;
    aw_err_t               err;
    __SDIO_SPI_HOST_TO_INFO_DECL(p_info, &(p_dev->host));

    aw_assert(p_dev != NULL);
    aw_assert(p_buf != NULL);
    aw_assert(len != 0);

    aw_gpio_set(p_info->cs_pin, 0);
    aw_spi_msg_init(&spi_msg, NULL, NULL);
    aw_spi_trans_add_tail(&spi_msg, &trans);

    /* 接收数据起始令牌 */
    timeout = 100000;
    do {
        aw_spi_mktrans(
                &trans,
                NULL,
                &temp,
                1,
                0,
                0,
                0,
                p_dev->speed,
                AW_SPI_READ_MOSI_HIGH);

        aw_spi_sync(&(p_dev->sdio_spi), &spi_msg);
        if (temp == 0xFE) {
            break;
        }
    } while (timeout--);

    if(temp != 0xFE) {
        err = -AW_ETIME;
        goto _exit;
    }

    /* 接收数据段 */
    aw_spi_mktrans(
            &trans,
            NULL,
            p_buf,
            len,
            0,
            0,
            0,
            p_dev->speed,
            AW_SPI_READ_MOSI_HIGH);

    err = aw_spi_sync(&(p_dev->sdio_spi), &spi_msg);
    if (AW_OK != err) {
        goto _exit;
    }
    /* 接收CRC */
    aw_spi_mktrans(
            &trans,
            NULL,
            (uint8_t*)&crc,
            2,
            0,
            0,
            0,
            p_dev->speed,
            AW_SPI_READ_MOSI_HIGH);

    err = aw_spi_sync(&(p_dev->sdio_spi), &spi_msg);
    if (AW_OK != err) {
        goto _exit;
    }

_exit:
    aw_gpio_set(p_info->cs_pin, 1);
    /* 发送8个周期 */
    aw_spi_mktrans(
            &trans,
            NULL,
            &temp,
            1,
            0,
            0,
            0,
            p_dev->speed,
            AW_SPI_READ_MOSI_HIGH);

    aw_spi_sync(&(p_dev->sdio_spi), &spi_msg);
    return err;
}

/******************************************************************************/
aw_local aw_err_t __sdio_spi_send_data (
                        awbl_spi_sdio_dev_t *p_dev,
                        uint8_t             tkn,
                        uint8_t             *p_buf,
                        uint32_t            len)
{
    struct aw_spi_message  spi_msg;
    struct aw_spi_transfer trans;
    uint8_t                temp = 0;
    uint32_t               timeout;
    uint16_t               crc;
    aw_err_t               err;
    __SDIO_SPI_HOST_TO_INFO_DECL(p_info, &(p_dev->host));

    aw_assert(p_dev != NULL);

    aw_spi_msg_init(&spi_msg, NULL, NULL);
    aw_spi_trans_add_tail(&spi_msg, &trans);

    if (len) {
        /* 发送24个空闲周期 */
        aw_gpio_set(p_info->cs_pin, 1);
        aw_spi_mktrans(
                &trans,
                NULL,
                &timeout,
                3,
                0,
                0,
                0,
                p_dev->speed,
                AW_SPI_READ_MOSI_HIGH);

        aw_spi_sync(&(p_dev->sdio_spi), &spi_msg);

        if (p_info->host_info.crc_en) {
            crc = __sdio_spi_crc16(0, p_buf, len);
            crc = (crc << 8) | (crc >> 8);
        }
    }

    aw_gpio_set(p_info->cs_pin, 0);

    /* 发送令牌 */
    aw_spi_mktrans(
            &trans,
            &tkn,
            NULL,
            1,
            0,
            0,
            0,
            p_dev->speed,
            AW_SPI_READ_MOSI_HIGH);

    err = aw_spi_sync(&(p_dev->sdio_spi), &spi_msg);
    if (err != AW_OK) {
        goto _exit;
    }

    if ((p_buf != NULL) && (len != 0)) {
        /* 发送数据段 */
        aw_spi_mktrans(
                &trans,
                p_buf,
                NULL,
                len,
                0,
                0,
                0,
                p_dev->speed,
                AW_SPI_READ_MOSI_HIGH);

        err = aw_spi_sync(&(p_dev->sdio_spi), &spi_msg);
        if (err != AW_OK) {
            goto _exit;
        }
        /* 发送CRC */
        aw_spi_mktrans(
                &trans,
                &crc,
                NULL,
                2,
                0,
                0,
                0,
                p_dev->speed,
                AW_SPI_READ_MOSI_HIGH);

        err = aw_spi_sync(&(p_dev->sdio_spi), &spi_msg);
        if (err != AW_OK) {
            goto _exit;
        }

        /* 接收数据响应令牌 */
        aw_spi_mktrans(
                &trans,
                NULL,
                &temp,
                1,
                0,
                0,
                0,
                p_dev->speed,
                AW_SPI_READ_MOSI_HIGH);

        err = aw_spi_sync(&(p_dev->sdio_spi), &spi_msg);

        if (err != AW_OK) {
            goto _exit;
        }
        if ((temp & 0x0F) != 0x05) {
            err = -AW_EIO;
            goto _exit;
        }
    }

    timeout = 100000;
    do {
        aw_spi_mktrans(
                &trans,
                NULL,
                &temp,
                1,
                0,
                0,
                0,
                p_dev->speed,
                AW_SPI_READ_MOSI_HIGH);

        aw_spi_sync(&(p_dev->sdio_spi), &spi_msg);
        if (temp == 0xFF) {
            break;
        }
    } while (timeout--);

    if(temp == 0) {
        err = -AW_ETIME;
        goto _exit;
    }

_exit:
    aw_gpio_set(p_info->cs_pin, 1);
    /* 发送8个周期 */
    aw_spi_mktrans(
            &trans,
            NULL,
            &temp,
            1,
            0,
            0,
            0,
            p_dev->speed,
            AW_SPI_READ_MOSI_HIGH);

    aw_spi_sync(&(p_dev->sdio_spi), &spi_msg);
    return err;
}

/******************************************************************************/
aw_local aw_err_t __sdio_spi_send_cmd (
                        struct awbl_sdio_host *p_host,
                        struct awbl_sdio_cmd  *p_cmd)
{
    aw_err_t               err;
    struct aw_spi_message  msg;
    struct aw_spi_transfer trans;
    uint8_t                p_buf[6], *rsp, temp, *ptr;
    uint32_t               len;
    uint32_t               timeout;
    __SDIO_SPI_HOST_TO_DEV_DECL(p_dev, p_host);
    __SDIO_SPI_HOST_TO_INFO_DECL(p_info, &(p_dev->host));

    aw_assert(p_dev != NULL);
    aw_assert(p_cmd != NULL);

    /* 初始化SPI消息 */
    aw_spi_msg_init(&msg, NULL, NULL);
    aw_spi_trans_add_tail(&msg, &trans);

    /* 禁止片选并发送八个时钟 */
    aw_gpio_set(p_info->cs_pin, 1);
    aw_spi_mktrans(
            &trans,
            NULL,
            &temp,
            1,
            0,
            0,
            0,
            p_dev->speed,
            AW_SPI_READ_MOSI_HIGH);

    aw_spi_sync(&(p_dev->sdio_spi), &msg);

    /* 发送命令 */
    aw_gpio_set(p_info->cs_pin, 0);
    p_buf[0] = p_cmd->cmd | 0x40;
    p_buf[1] = p_cmd->arg >> 24;
    p_buf[2] = p_cmd->arg >> 16;
    p_buf[3] = p_cmd->arg >> 8;
    p_buf[4] = p_cmd->arg >> 0;
    p_buf[5] = (__sdio_spi_crc7(0, p_buf, 5) << 1) | 0x01;

    aw_spi_mktrans(
            &trans,
            p_buf,
            NULL,
            6,
            0,
            0,
            0,
            p_dev->speed,
            AW_SPI_READ_MOSI_HIGH);

    err = aw_spi_sync(&(p_dev->sdio_spi), &msg);
    if(AW_OK != err) {
        goto _exit;
    }

    /* 接收响应R1 */
    rsp = (uint8_t *)(p_cmd->p_rsp);
    timeout = 500;
    do {
        aw_spi_mktrans(
                &trans,
                NULL,
                rsp,
                1,
                0,
                0,
                0,
                p_dev->speed,
                AW_SPI_READ_MOSI_HIGH);

        aw_spi_sync(&(p_dev->sdio_spi), &msg);
    } while ((*rsp & 0x80) &&  (--timeout));

    if (timeout == 0) {
        err = -AW_ETIME;
        goto _exit;
    }

    /* 接收剩余响应部分 */
    switch (p_cmd->rsp_type) {
        case AWBL_SDIO_RSP_NONE:  len = 0;break;
        case AWBL_SDIO_RSP_R1:    len = 0;break;
        case AWBL_SDIO_RSP_R1B:   len = 0;break;
        case AWBL_SDIO_RSP_R2:    len = 1;break;
        case AWBL_SDIO_RSP_R3:    len = 4;break;
        case AWBL_SDIO_RSP_R4:    len = 4;break;
        case AWBL_SDIO_RSP_R5:    len = 1;break;
        case AWBL_SDIO_RSP_R7:    len = 4;break;
        default:len = 0;break;
    }
    rsp++;
    if (len) {
        aw_spi_mktrans(
                &trans,
                NULL,
                rsp,
                len,
                0,
                0,
                0,
                p_dev->speed,
                AW_SPI_READ_MOSI_HIGH);

        aw_spi_sync(&(p_dev->sdio_spi), &msg);
    }
    aw_gpio_set(p_info->cs_pin, 1);

    /* 禁止片选后发送八个时钟周期 */
    aw_spi_mktrans(
            &trans,
            NULL,
            &temp,
            1,
            0,
            0,
            0,
            p_dev->speed,
            AW_SPI_READ_MOSI_HIGH);

    aw_spi_sync(&(p_dev->sdio_spi), &msg);

    if (p_cmd->opt != AWBL_SDIO_NO_DATA) {
        /* 执行数据传输 */
        len = p_cmd->nblock;
        ptr = p_cmd->p_data;

        /* 接收数据 */
        if (p_cmd->opt == AWBL_SDIO_RD_DATA) {
            while (len--) {
                err = __sdio_spi_rec_data(
                                    p_dev,
                                    ptr,
                                    p_cmd->blk_size);
                if (err) {
                    break;
                }
                ptr += p_cmd->blk_size;
            };
            return err;
        }
        /* 发送数据 */
        if (p_cmd->opt == AWBL_SDIO_WR_DATA) {
            uint8_t tkn;
            tkn = (p_cmd->cmd == AWBL_SDIO_CMD25) ? 0xFC : 0xFE;
            while (len--) {
                err = __sdio_spi_send_data(
                                    p_dev,
                                    tkn,
                                    p_cmd->p_data,
                                    p_cmd->blk_size);
                if (err) {
                    break;
                }
                ptr += p_cmd->blk_size;
            };

            if (tkn == 0xFC) {
                __sdio_spi_send_data(p_dev, 0xFD, NULL, 0);
            }
            return err;
        }

        return -AW_EINVAL;
    }

    err = AW_OK;
_exit:
    aw_gpio_set(p_info->cs_pin, 1);
    return err;
}

/******************************************************************************/
aw_local void __sdio_spi_trans_task_entry (void *p_arg)
{
    awbl_spi_sdio_dev_t *p_dev  = (awbl_spi_sdio_dev_t *)p_arg;
    struct awbl_sdio_host    *p_host = &(p_dev->host);
    struct awbl_sdio_msg     *p_msg;
    struct awbl_sdio_cmd     *p_cmd;
    aw_err_t            err;

    AW_FOREVER {
        AW_SEMB_TAKE(p_dev->sem_sync, AW_SEM_WAIT_FOREVER);

        while ((p_msg = awbl_sdio_msg_out(p_host))) {
            /* update status */
            p_msg->status = -AW_EINPROGRESS;
            p_cmd         = NULL;

            /* handle all command */
            aw_list_for_each_entry(p_cmd,
                                   &p_msg->cmd_list,
                                   struct awbl_sdio_cmd,
                                   cmd_node) {
                /* do a command */
                do {
                    err = __sdio_spi_send_cmd(p_host, p_cmd);
                } while ((err != AW_OK) && (p_cmd->retries--));

                /* 命令发送不成功 */
                if (err != AW_OK) {
                    /* error */
                    p_msg->status = err;
                    break;


                }

                /* 命令发送成功，检测R1状态 */
                if (p_cmd->p_rsp[0] != 0) {
                    if ((AWBL_SD_SPI_R1_PARA_ERROR |
                         AWBL_SD_SPI_R1_ADDRESS_ERROR)
                         & p_cmd->p_rsp[0]) {
                        /* CMD12在地址不对齐时会引起ADDRESS_ERROR 可忽略 */
                        if (p_cmd->cmd != AWBL_SDIO_CMD12) {
                            p_msg->status = -AW_EFAULT;    /* 地址错误 */
                        }
                    } else if (AWBL_SD_SPI_R1_ILLEGAL_CMD & p_cmd->p_rsp[0]) {
                        p_msg->status = -AW_ENOSYS;    /* 不支持的命令 */
                    } else if (AWBL_SD_SPI_R1_COM_CRC_ERROR & p_cmd->p_rsp[0]) {
                        p_msg->status = -AW_EILSEQ;    /* CRC校验失败 */
                    } else if ((AWBL_SD_SPI_R1_ERASE_SEQ_ERROR |
                                AWBL_SD_SPI_R1_ERASE_RESET)
                                & p_cmd->p_rsp[0]) {
                        p_msg->status = -AW_EIO;       /* IO错误 */
                    }
                }

            }

            /* update status */
            if (p_msg->status == -AW_EINPROGRESS) {
                p_msg->status = AW_OK;
            }
            /* complete  */
            if (p_msg->pfn_complete) {
                p_msg->pfn_complete(p_msg->p_arg);
            }
        }
    }
}

/******************************************************************************/
aw_local aw_err_t __sdio_spi_xfer (struct awbl_sdio_host *p_host)
{
    __SDIO_SPI_HOST_TO_DEV_DECL(p_dev, p_host);

    if (p_host == NULL) {
        return -AW_EINVAL;
    }

    AW_SEMB_GIVE(p_dev->sem_sync);
    return AW_OK;
}

/******************************************************************************/
aw_local void __sdio_spi_cancel (struct awbl_sdio_host *p_host,
                                 struct awbl_sdio_msg  *p_msg)
{

}

/******************************************************************************/
aw_local uint32_t __sdio_spi_set_speed (
                        struct awbl_sdio_host *p_host,
                        uint32_t         speed)
{
    __SDIO_SPI_HOST_TO_DEV_DECL(p_dev, p_host);

    p_dev->speed = speed;
    return speed;
}

/******************************************************************************/
aw_local aw_err_t __sdio_spi_dummy_clks (
                            struct awbl_sdio_host *p_host,
                            uint8_t          nclks)
{
    struct aw_spi_message  msg;
    struct aw_spi_transfer trans;
    uint8_t                i, temp, n = nclks/8;
    __SDIO_SPI_HOST_TO_DEV_DECL(p_dev, p_host);

    aw_spi_msg_init(&msg, NULL, NULL);
    aw_spi_trans_add_tail(&msg, &trans);

    for (i=0; i<n; i++) {
        aw_spi_mktrans(
                &trans,
                NULL,
                &temp,
                1,
                0,
                0,
                0,
                p_dev->speed,
                AW_SPI_READ_MOSI_HIGH);

        aw_spi_sync(&(p_dev->sdio_spi), &msg);
    }
    return AW_OK;
}

aw_local void __sdio_port_irq(void *p_arg)
{
    uint32_t trig_flag;
    uint8_t  port_state = 0;
    aw_err_t ret = AW_OK;

    awbl_spi_sdio_dev_t *p_dev = (awbl_spi_sdio_dev_t*)p_arg;

    __SDIO_SPI_HOST_TO_INFO_DECL(p_info, p_dev);

    ret = awbl_sdio_host_get_port_status(&(p_dev->host), &port_state);

    trig_flag = (AWBL_SDIO_CD_STUBBS == port_state) ?
            AW_GPIO_TRIGGER_FALL: AW_GPIO_TRIGGER_RISE;

    ret |= aw_gpio_trigger_cfg(p_info->pin_cd, trig_flag);
    ret |= aw_gpio_trigger_on(p_info->pin_cd);

    awbl_sdio_port_change(&(p_dev->host), port_state);
    return;
}


aw_local void __spi_port_set_trigger (awbl_spi_sdio_dev_t      *p_dev,
                                      awbl_spi_sdio_dev_info_t *p_info)
{
    uint8_t  port_state = 0;
    uint32_t trig_flag;
    aw_err_t ret = AW_OK;

    ret = awbl_sdio_host_get_port_status(&(p_dev->host), &port_state);


    trig_flag = (AWBL_SDIO_CD_STUBBS == port_state) ?
            AW_GPIO_TRIGGER_FALL: AW_GPIO_TRIGGER_RISE;

    ret |= aw_gpio_pin_cfg(p_info->pin_cd, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
    ret |= aw_gpio_trigger_connect(p_info->pin_cd, __sdio_port_irq, (void *)p_dev);
    ret |= aw_gpio_trigger_cfg(p_info->pin_cd, trig_flag);
    ret |= aw_gpio_trigger_on(p_info->pin_cd);
}


/******************************************************************************/
aw_local void __sdio_spi_hw_init (awbl_spi_sdio_dev_t *p_dev)
{
    __SDIO_SPI_HOST_TO_INFO_DECL(p_info, p_dev);
    /* 配置 SPI FLASH 设备 */
    aw_spi_mkdev(&(p_dev->sdio_spi),
                 p_dev->host.super.super.p_devhcf->bus_index,
                 8,
                 p_info->mode ,
                 p_info->max_speed,
                 0,
                 NULL);

    /* 设置 SPI FLASH 设备 */
    aw_spi_setup(&(p_dev->sdio_spi));

    aw_gpio_pin_cfg(p_info->cs_pin, AW_GPIO_OUTPUT | AW_GPIO_PULL_UP);
}

/******************************************************************************/
aw_local aw_err_t __sdio_spi_detect (struct awbl_sdio_host *p_host,
                                     uint8_t               *p_status)
{
    __SDIO_SPI_HOST_TO_DEV_DECL(p_dev, p_host);
    __SDIO_SPI_HOST_TO_INFO_DECL(p_info, &(p_dev->host));

    if (p_info->pfn_cd) {
        if (p_info->pfn_cd(p_dev)) {
            *p_status = AWBL_SDIO_CD_PLUG;
        } else {
            *p_status = AWBL_SDIO_CD_STUBBS;
        }
    }
    return AW_OK;
}

/******************************************************************************/
aw_local void __sdio_spi_inst_init1 (struct awbl_dev *p_awdev)
{
    return ;
}

/******************************************************************************/
aw_local void __sdio_spi_inst_init2 (struct awbl_dev *p_awdev)
{
    __SDIO_SPI_AWDEV_TO_INFO_DECL(p_info, p_awdev);
    __SDIO_SPI_AWDEV_TO_DEV_DECL(p_dev, p_awdev);
    struct awbl_sdio_host *p_host = (struct awbl_sdio_host *)p_awdev;

    /* platform initialization */
    if (p_info->pfunc_plfm_init != NULL) {
        p_info->pfunc_plfm_init();
    }

    /* init controller hardware */
    __sdio_spi_hw_init(p_dev);

    __spi_port_set_trigger(p_dev, p_info);

    /* create sdio bus */
    awbl_sdio_host_create(p_host);

    /* 初始化驱动信号量 */
    AW_SEMB_INIT(p_dev->sem_sync, 0, AW_SEM_Q_FIFO);

    /* 初始化任务ssp_trans_task */
    AW_TASK_INIT(p_dev->spi_sdio_trans_task,        /* 任务实体 */
                 "sdio_spi_trans_task",             /* 任务名字 */
                 p_info->task_trans_prio,           /* 任务优先级 */
                 _TRANS_TASK_STACK_SIZE,            /* 任务堆栈大小 */
                 __sdio_spi_trans_task_entry,       /* 任务入口函数 */
                 (void *)p_dev);                    /* 任务入口参数 */

    /* 启动任务task0 */
    AW_TASK_STARTUP(p_dev->spi_sdio_trans_task);

    return;
}

/******************************************************************************/
aw_local void __sdio_spi_inst_connect (struct awbl_dev *p_awdev)
{
    return ;
}




/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_sdio_spi_drvfuncs = {
    __sdio_spi_inst_init1,
    __sdio_spi_inst_init2,
    __sdio_spi_inst_connect
};



/* driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_sdio_host_drvinfo __g_sdio_spi_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_SPI | AWBL_DEVID_BUSCTRL,    /* bus_id */
        AWBL_SDIO_SPI_NAME,                     /* p_drvname */
        &__g_sdio_spi_drvfuncs,                 /* p_busfuncs */
        NULL,                                   /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    },
    __sdio_spi_xfer,
    __sdio_spi_set_speed,
    NULL,
    __sdio_spi_dummy_clks,
    NULL,
    __sdio_spi_detect
};

/******************************************************************************/
void awbl_sdio_spi_drv_register (void)
{
    aw_err_t err;
    err = awbl_drv_register((struct awbl_drvinfo *)&__g_sdio_spi_drv_registration);
    err = (aw_err_t)err;
    return;
}

/* end of file*/

