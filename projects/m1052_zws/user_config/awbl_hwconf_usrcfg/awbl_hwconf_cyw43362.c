/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief CYW43362驱动通用配置
 *
 * \internal
 * \par Modification History
 * - 1.00 18-03-13  xdn, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_nvram.h"

#include "aw_prj_params.h"

#if defined(AW_DEV_CYW43362_WIFI_SPI_0) || defined(AW_DEV_CYW43362_WIFI_SDIO_0)

//#include "cyw43362/wifi_nvram_image.h"
#include "cyw43362/wifi_nvram_ap6181.h"

#include "driver/wifi/cypress/cyw43362/awbl_cyw43362_sdio.h"
#include "driver/wifi/cypress/cyw43362/awbl_cyw43362_spi.h"

/*******************************************************************************
  RESOURCES
*******************************************************************************/

#define IMPORT_LD_SYMBOL(x)             extern unsigned long x
#define DEFINE_LD_SYMBOL(t, n, x)       t n = (t) &x
#define ACCESS_LD_SYMBOL(x)             ((unsigned long) &x)

#ifndef MIN
    #define MIN( x, y ) ((x) < (y) ? (x) : (y))
#endif

#define MAKE_TYPE(a, b, c, d) \
    ((uint32_t)((d) & 0xff) << 24) | \
     ((uint32_t)((c) & 0xff) << 16) | \
     ((uint32_t)((b) & 0xff) << 8)  | \
      (uint32_t)((a) & 0xff)

#define CYW43362_RES_TEXT                   0
#define CYW43362_RES_NVRAM                  1

/**
 * 使用 NVRAM 内的固件时，参考 img_stm32f412_wifi_firmware 工程
 */
#define CYW43362_WIFI_FIRMWARE_LOCATION     CYW43362_RES_TEXT
#define CYW43362_WIFI_CFG_NVRAM_LOCATION    CYW43362_RES_TEXT


#if (CYW43362_WIFI_FIRMWARE_LOCATION == CYW43362_RES_TEXT)
    IMPORT_LD_SYMBOL(__firmware_cyw43362_apsta_start);
    IMPORT_LD_SYMBOL(__firmware_cyw43362_apsta_end);
#elif (CYW43362_WIFI_FIRMWARE_LOCATION == CYW43362_RES_NVRAM)
    #include "aw_nvram.h"
    #include <stdio.h>
#endif

#define  MAC1_NVRAM_NAME            "otp_zy_mac"
#define  MAC1_NVRAM_UNIT            1

aw_local aw_err_t cyw43362_wifi_get_macaddr (uint8_t *p_mac_buf, int buf_len)
{
#define __ZY_MAC_HEAD_01 0x00
#define __ZY_MAC_HEAD_02 0x14

    uint8_t buf[4];
    aw_local aw_const uint8_t comp0[4] = {0, 0, 0, 0};
    aw_local aw_const uint8_t comp1[4] = {0xff, 0xff, 0xff, 0xff};

    if (buf_len < 6) {
        return AW_ERROR;
    }
    memset(buf, 0, sizeof(buf));
    aw_nvram_get(MAC1_NVRAM_NAME, MAC1_NVRAM_UNIT, (char *) buf, 0, 4);

    if (0 == memcmp(buf, comp0, 4) ||
        0 == memcmp(buf, comp1, 4)) {
        memset(p_mac_buf, 0, buf_len);
        return AW_ERROR;
    }

    p_mac_buf[0] = __ZY_MAC_HEAD_01;
    p_mac_buf[1] = __ZY_MAC_HEAD_02;
    buf[3]++;
    memcpy(&p_mac_buf[2], buf, 4);
    return AW_OK;
}

aw_local aw_bool_t __get_dhcp_en_sta (void)
{
    return AW_TRUE;
}

aw_const aw_netif_info_get_entry_t g_cyw43362_netif_info_ap = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

aw_const aw_netif_info_get_entry_t g_cyw43362_netif_info_sta = {
    NULL,
    NULL,
    NULL,
    cyw43362_wifi_get_macaddr,
    __get_dhcp_en_sta,
};

#if (CYW43362_WIFI_FIRMWARE_LOCATION == CYW43362_RES_NVRAM) \
    || (CYW43362_WIFI_CFG_NVRAM_LOCATION == CYW43362_RES_NVRAM)
/**
 * SPI Flash 的 NVRAM 内容使用 img_stm32f412_wifi_firmware 写入
 */
struct cyw43362_wifi_img_hdr {
    uint32_t type;          // "WMFW" or "WMNV"
    uint32_t crc;           // CRC32
    uint32_t size;          // Image size in bytes
    uint32_t offset;        // Offset of image
    uint32_t version;       // Version 5.90.230.31
    uint8_t  desc[12];      // Description
    uint8_t  reserved[32];  // Reserved
};

aw_local
aw_err_t resource_load_header (const char *name,
                              int         unit,
                              int         offset,
                              void       *p_buf,
                              size_t      size)
{
    return aw_nvram_get((char *) name,
            unit,
            (char *) p_buf,
            offset,
            size);
}

aw_local
aw_err_t resource_body_size (const char *name,
                            int         unit,
                            int         hdr_index,
                            uint32_t    type,
                            uint32_t   *size_out)
{
    struct cyw43362_wifi_img_hdr hdr;

    *size_out = 0;
    if (AW_OK != resource_load_header(name, unit, sizeof(hdr) * hdr_index, &hdr, sizeof(hdr))) {
        return AW_FALSE;
    }

    if (hdr.type != type ||
            hdr.crc == -1 || hdr.crc == 0 ||
            hdr.size == -1 || hdr.size == 0) {
        return AW_FALSE;
    }
    *size_out = hdr.size;
    return AW_TRUE;
}

aw_local
aw_bool_t resource_load_body (const char *name,
                           int         unit,
                           int         hdr_index,
                           uint32_t    offset,
                           void       *buffer,
                           uint32_t    size_in,
                           uint32_t   *size_out)
{
    struct cyw43362_wifi_img_hdr hdr;

    if (AW_OK != resource_load_header(name, unit, sizeof(hdr) * hdr_index, &hdr, sizeof(hdr))) {
        *size_out = 0;
        return AW_ERROR;
    }

    *size_out = MIN(size_in, (hdr.size - offset));
    if (AW_OK != aw_nvram_get((char *) name,
                              unit,
                              buffer,
                              hdr.offset + offset,
                              *size_out)) {
        *size_out = 0;
        return AW_ERROR;
    }
    return AW_OK;
}
#endif

#if (CYW43362_WIFI_FIRMWARE_LOCATION == CYW43362_RES_TEXT)

aw_err_t cyw43362_wifi_firmware_size (uint32_t *size_out)
{
    *size_out = (uint32_t) (ACCESS_LD_SYMBOL(__firmware_cyw43362_apsta_end) - \
            ACCESS_LD_SYMBOL(__firmware_cyw43362_apsta_start));
    return AW_OK;
}

aw_err_t cyw43362_wifi_firmware_read (uint32_t  offset,
                                      void     *buffer,
                                      uint32_t  size_in,
                                      uint32_t *size_out)
{
    DEFINE_LD_SYMBOL(const uint8_t *, fw_start, __firmware_cyw43362_apsta_start);
    uint32_t size_bin = (uint32_t) (ACCESS_LD_SYMBOL(__firmware_cyw43362_apsta_end) - \
            ACCESS_LD_SYMBOL(__firmware_cyw43362_apsta_start));

    *size_out = MIN(size_in, (size_bin - offset));
    memcpy(buffer, &fw_start[offset], *size_out);
    return AW_OK;
}

#endif

#if (CYW43362_WIFI_FIRMWARE_LOCATION == CYW43362_RES_NVRAM)
aw_err_t cyw43362_wifi_firmware_size (uint32_t *size_out)
{
    return resource_body_size("wifi_firmware",
            0,
            0, // firmware
            MAKE_TYPE('W', 'M', 'F', 'W'),
            size_out);
}

aw_err_t cyw43362_wifi_firmware_read (uint32_t  offset,
                                      void     *buffer,
                                      uint32_t  size_in,
                                      uint32_t *size_out)
{
    return resource_load_body("wifi_firmware",
            0,
            0, // firmware
            offset,
            buffer,
            size_in,
            size_out);
}

#endif

#if (CYW43362_WIFI_CFG_NVRAM_LOCATION == CYW43362_RES_TEXT)

//#define CYW43362_WIFI_CFG_NVRAM_IMAGE   cyw43362_wifi_nvram_image
#define CYW43362_WIFI_CFG_NVRAM_IMAGE   cyw43362_wifi_nvram_ap6181

aw_err_t cyw43362_wifi_cfg_nvram_size (uint32_t *size_out)
{
    *size_out = sizeof(CYW43362_WIFI_CFG_NVRAM_IMAGE);
    return AW_OK;
}

aw_err_t cyw43362_wifi_cfg_nvram_read (uint32_t  offset,
                                       void     *buffer,
                                       uint32_t  size_in,
                                       uint32_t *size_out)
{
    *size_out = MIN(size_in, (sizeof(CYW43362_WIFI_CFG_NVRAM_IMAGE) - offset));
    memcpy(buffer, &CYW43362_WIFI_CFG_NVRAM_IMAGE[offset], *size_out);
    return AW_OK;
}

#endif

#if (CYW43362_WIFI_CFG_NVRAM_LOCATION == CYW43362_RES_NVRAM)
aw_err_t cyw43362_wifi_cfg_nvram_size (uint32_t *size_out)
{
    return resource_body_size("wifi_firmware",
            0,
            1, // 6181
            MAKE_TYPE('W', 'M', 'N', 'V'),
            size_out);
}

aw_err_t cyw43362_wifi_cfg_nvram_read (uint32_t  offset,
                                       void     *buffer,
                                       uint32_t  size_in,
                                       uint32_t *size_out)
{
    return resource_load_body("wifi_firmware",
            0,
            1, // 6181
            offset,
            buffer,
            size_in,
            size_out);
}

#endif

#undef CYW43362_RES_TEXT
#undef CYW43362_RES_NVRAM
#undef CYW43362_RES_FS

#undef CYW43362_WIFI_FIRMWARE_LOCATION
#undef CYW43362_WIFI_CFG_NVRAM_LOCATION

aw_bool_t cyw43362_wifi_roam_config (uint32_t *roam_param)
{
#if 0
    if (NULL != roam_param) {
        if (AW_OK == aw_nvram_get((char *) DEV_NV_ROAMCFG_0, 0, (char *) roam_param, 0, 12)) {
            int i;
            for (i = 0; i < 3; i++) {
                if (0xFFFFFFFF == roam_param[i]) {
                    return AW_FALSE;
                }
            }
            return AW_TRUE;
        }
    }
#endif
    return AW_FALSE;
}
#endif
