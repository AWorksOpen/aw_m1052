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
 * \brief 将nor flash作为nvram设备
 *        未实现mtd设备
 *
 * \internal
 * \par modification history:
 * - 1.00 15-5-20, ehf, first implementation.
 * \endinternal
 */

#include "apollo.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "driver/norflash/awbl_nor_flash.h"
#include "awbl_nvram.h"
#include <string.h>

/*****************************************************************************/
#define __NOR_FLASH_DEVINFO_DECL(p_info, p_nor_dev) \
        awbl_nor_flash_devinfo_t *p_info = \
        (awbl_nor_flash_devinfo_t *)AWBL_DEVINFO_GET(p_nor_dev)

#define __NOR_FLASH_DEV_DECL(p_dev, p_nor_dev) \
        awbl_nor_flash_dev_t *p_nordev = \
        AW_CONTAINER_OF(p_nor_dev,  awbl_nor_flash_dev_t, nor_dev)

/* nor flash 在系统中基地址 */
aw_local uint32_t nor_base_addr = 0;

/*****************************************************************************/
void __nor_flash_inst_init1 (struct awbl_dev *p_dev)
{
    return ;
}

void __nor_flash_inst_init2 (struct awbl_dev *p_dev)
{
    __NOR_FLASH_DEVINFO_DECL(p_info, p_dev);

    /* init base addr */
    nor_base_addr = p_info->base_addr;

    /* platform initialization */
    if (p_info->pfunc_plfm_init != NULL) {
        p_info->pfunc_plfm_init();
    }

    return;
}

void __nor_flash_inst_connect (struct awbl_dev *p_dev)
{
    return ;
}

/*****************************************************************************/
/*
 * byte模式下: 系统地址总线ADDR0 -> NOR FLASH ADDR0
 * word模式下: 系统地址总线ADDR1 -> NOR FLASH ADDR0
 *
 * 对于byte/word模式下：系统地址总线ADDR0 均连接到 NOR FLASH ADDR0
 * 上的NOR FLASH 暂不支持
 * */
#define __nor_read8(addr)       *(uint8_t *)(nor_base_addr + addr)
#define    __nor_read16(addr)   *(uint16_t *)(nor_base_addr + (addr << 1))
#define    __nor_write8(addr, dat)      *(uint8_t *)(nor_base_addr + addr) = dat
#define    __nor_write16(addr, dat) \
                *(uint16_t *)(nor_base_addr + (addr << 1)) = dat

aw_local int __nor_toggle_bit_check(uint32_t addr, aw_bool_t byte_mode)
{
    uint16_t   val1 = 0;
    uint16_t   val2 = 0;
    uint32_t time_out = 0;

    while (1) {
        if (byte_mode) {
            val1 = __nor_read8(addr);
            val2 = __nor_read8(addr);
        } else {
            val1 = __nor_read16(addr);
            val2 = __nor_read16(addr);
        }
        val1 ^= val2;                      /* 读得的两次结果异或           */
        if (!(val1 & 0x40)) {              /* 如果操作完成，DQ6位停止轮转  */
               return 0;                   /* Toggle，位与结果为0          */
        }
        if (time_out ++ > 10000000) {
            return 1;
        }
    }
}

uint32_t __nor_flash_erase_sector(uint32_t start_addr, aw_bool_t byte_mode)
{
    if (byte_mode) {
        __nor_write8(0x555, 0xaa);
        __nor_write8(0x2aa, 0x55);
        __nor_write8(0x555, 0x80);
        __nor_write8(0x555, 0xaa);
        __nor_write8(0x2aa, 0x55);
        __nor_write8(start_addr, 0x30);
        return __nor_toggle_bit_check(start_addr, byte_mode);
    } else {
        start_addr /= 2;
        __nor_write16(0x555, 0xaa);
        __nor_write16(0x2aa, 0x55);
        __nor_write16(0x555, 0x80);
        __nor_write16(0x555, 0xaa);
        __nor_write16(0x2aa, 0x55);
        __nor_write16(start_addr, 0x30);
        return __nor_toggle_bit_check(start_addr, byte_mode);
    }
}


uint32_t __nor_flash_write8(uint32_t addr, uint8_t dat)
{
    __nor_write8(0x555, 0xaa);
    __nor_write8(0x2aa, 0x55);
    __nor_write8(0x555, 0xa0);
    __nor_write8(addr, dat);

    return __nor_toggle_bit_check(addr, AW_TRUE);
}

uint32_t __nor_flash_write16(uint32_t addr, uint16_t dat)
{
    __nor_write16(0x555, 0xaa);
    __nor_write16(0x2aa, 0x55);
    __nor_write16(0x555, 0xa0);
    __nor_write16(addr, dat);

    return __nor_toggle_bit_check(addr, AW_FALSE);
}

/*
 *  读NOR Flash内size个字节
 *  addr地址单位为字节
 *  调用时不考虑地址在Byte/Word模式下的区别
 */
static aw_err_t __nor_flash_read_nbytes(awbl_nor_flash_dev_t *p_dev,
                                        uint32_t              addr,
                                        uint8_t              *p_dstbuf,
                                        size_t                size)
{
    uint16_t   u16data;
    uint16_t  *p16data;
    __NOR_FLASH_DEVINFO_DECL(p_info, p_dev);

    /* nor flash byte mode */
    if (p_info->byte_mode)
    {
        while(size--)
        {
            *p_dstbuf++ = __nor_read8(addr++);
        }
    /* nor flash word mode */
    } else {
        /* 源地址字对齐 */
        if ((addr % 2) != 0) {
            u16data = __nor_read16(addr / 2);
            *p_dstbuf++ = u16data & 0xFF;
            addr++;
            size--;
        }
        /* word模式下内部地址除2 */
        addr /= 2;
        /* 判断目的地址是否字对齐分别拷贝 */
        if ( ((uint32_t)p_dstbuf %2 ) == 0 ) {
            p16data = (uint16_t *)p_dstbuf;
            while(size > 1)
            {
                *p16data++ = __nor_read16(addr);
                addr++;
                size -= 2;
            }
            p_dstbuf = (uint8_t *)p16data;
        } else {
            while(size > 1)
            {
                u16data = __nor_read16(addr);
                *p_dstbuf++ = u16data >> 8;
                *p_dstbuf++ = u16data;
                size -= 2;
            }
        }
        if (size == 1) {
            u16data = __nor_read16(addr);
            *p_dstbuf = u16data >> 8;
        }
    }
    return AW_OK;
}

/*
 *    读NOR Flash内size个字节
 *    addr地址单位为字节
 *    调用时不考虑内部地址在Byte/Word模式下的区别
 */
static aw_err_t __nor_flash_write_nbytes(awbl_nor_flash_dev_t *p_dev,
                                                               uint32_t addr,
                                                               uint8_t *p_srcbuf,
                                                               uint32_t size)
{
    uint16_t   u16data;
    uint16_t  *p16data;
    __NOR_FLASH_DEVINFO_DECL(p_info, p_dev);

    /* nor flash byte mode */
    if (p_info->byte_mode)
    {
        while(size--)
        {
            __nor_flash_write8(addr, *p_srcbuf);
            addr++;
            p_srcbuf++;
        }
    /* nor flash word mode */
    } else {
        /* nor flash 地址16位对齐 */
        if ((addr % 2) != 0) {
            /* 改写低8位 */
            u16data = *p_srcbuf + 0xFF00;
            __nor_flash_write16(addr / 2,  u16data);
            p_srcbuf++;
            size--;
        }
        addr /= 2;
        /* 判断源地址是否字对齐分别拷贝 */
        if ( ((uint32_t)p_srcbuf % 2 ) == 0 ) {
            p16data = (uint16_t *)p_srcbuf;
            while(size > 1)
            {
                __nor_flash_write16(addr, *p16data);
                p16data++;
                addr++;
                size -= 2;
            }
            p_srcbuf = (uint8_t *)p16data;
        } else {
            while(size > 1)
            {
                u16data = *p_srcbuf++;
                u16data = (u16data << 8) + *p_srcbuf++;
                __nor_flash_write16(addr, u16data);
                addr++;
                size -= 2;
            }
        }
        if (size == 1) {
            u16data =( *p_srcbuf << 8) + 0xff;
            __nor_write16(addr, u16data);
        }
    }
    return AW_OK;
}

/**
 * \brief awbl_nvram_get method handler
 */
aw_local aw_err_t __norflash_nvram_get (struct awbl_dev *p_dev,
                                                               char            *p_name,
                                                               int              unit,
                                                               void            *p_buf,
                                                               int              offset,
                                                               int              len)
{
    __NOR_FLASH_DEVINFO_DECL(p_devinfo, p_dev);
    const struct awbl_nvram_segment *p_seg = p_devinfo->p_seg_list;

    int              i;
    int              copylen;
    aw_err_t     ret;

    for(i = 0; i < p_devinfo->seg_count; i++) {
        if ( ((p_seg->unit) == unit) && (strcmp(p_seg->p_name, p_name) == 0) ) {
            copylen = len;
            if (offset + len > p_seg->seg_size) {
                copylen = p_seg->seg_size - offset;
                if (copylen < 0) {
                    return -AW_EIO;
                }
            }
            if (p_seg->seg_addr + offset + copylen >
                (p_devinfo->sector_size * p_devinfo->sector_cnt)) {
                return -AW_ENXIO;
            }
            ret = __nor_flash_read_nbytes((awbl_nor_flash_dev_t *)p_dev,
                                                           p_seg->seg_addr + offset,
                                                           (uint8_t *)p_buf,
                                                           copylen);
            return ret != AW_OK ? -AW_EIO : AW_OK;
        }
        p_seg++;
    }
    return -AW_ENXIO;
}

/**
 * \brief awbl_nvram_set method handler
 */
aw_local aw_err_t __norflash_nvram_set (struct awbl_dev *p_dev,
                                        char            *p_name,
                                        int              unit,
                                        char            *p_buf,
                                        int              offset,
                                        int              len)
{
    __NOR_FLASH_DEVINFO_DECL(p_devinfo, p_dev);
    const struct awbl_nvram_segment *p_seg = p_devinfo->p_seg_list;

    int      i;
    int      copylen;
    aw_err_t ret;

    uint32_t  sector_addr_start = 0;
    uint32_t  sector_addr_end = 0;
    uint32_t  current_addr = 0;
    uint32_t  copy_size = 0;
    uint32_t  j = 0;

    for (i = 0; i < p_devinfo->seg_count; i++) {
        if ((p_seg->unit == unit) && (strcmp(p_seg->p_name, p_name) == 0)) {
            copylen = len;
            if (offset + len > p_seg->seg_size) {
                copylen = p_seg->seg_size - offset;
                if (copylen < 0) {
                    return -AW_EIO;
                }
            }
            if (p_seg->seg_addr + offset + copylen  >
                (p_devinfo-> sector_size * p_devinfo->sector_cnt)) {
                return  -AW_ENXIO;
            }

            for (j = 0; j < copylen;) {
                /* 计算扇区地址范围 */
                current_addr = j + p_seg->seg_addr + offset;
                sector_addr_start = current_addr / p_devinfo->sector_size * 
                                    p_devinfo->sector_size;
                sector_addr_end = sector_addr_start+p_devinfo->sector_size;
                /* 读取一个扇区数据  */
                ret = __nor_flash_read_nbytes ((awbl_nor_flash_dev_t *)p_dev,
                                                sector_addr_start,
                                                p_devinfo->p_bulk_buff,
                                                p_devinfo->sector_size);
                if (ret != AW_OK) {
                    return  ret;
                }
                /* 合并数据 */
                copy_size = (copylen - j) < (sector_addr_end - current_addr)? \
                                   (copylen - j): \
                                   (sector_addr_end - current_addr);
                memcpy(&p_devinfo->
                           p_bulk_buff[current_addr % p_devinfo->sector_size],
                       &p_buf[j], copy_size);
                j += copy_size;
                /* 擦除扇区 */
                __nor_flash_erase_sector(sector_addr_start, p_devinfo->byte_mode);
                if (ret != 0) {
                    return AW_ERROR;
                }
                /* 将数据重新写入 */
                ret = __nor_flash_write_nbytes((awbl_nor_flash_dev_t *)p_dev,
                                                sector_addr_start,
                                                p_devinfo->p_bulk_buff,
                                                p_devinfo->sector_size);
            }
            return  AW_OK;
        }
        p_seg++;
    }
    return -AW_ENXIO;
}
/*****************************************************************************/
/** \brief driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_nvram_get);
AWBL_METHOD_IMPORT(awbl_nvram_set);

aw_local aw_const struct awbl_dev_method __g_norflash_methods[] = {
    AWBL_METHOD(awbl_nvram_get, __norflash_nvram_get),
    AWBL_METHOD(awbl_nvram_set, __norflash_nvram_set),
    AWBL_METHOD_END
};

/** \brief driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_nor_flash_drvfuncs = {
        __nor_flash_inst_init1,
        __nor_flash_inst_init2,
        __nor_flash_inst_connect
};

/** \brief driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_nor_flash_drv_registration = {
    {
        AWBL_VER_1,                    /**< \brief awb_ver */
        AWBL_BUSID_PLB,                /**< \brief bus_id */
        AWBL_NOR_FLASH_NAME,           /**< \brief p_drvname */
        &__g_nor_flash_drvfuncs,       /**< \brief p_busfuncs */
        &__g_norflash_methods[0],      /**< \brief p_methods */
        NULL                           /**< \brief pfunc_drv_probe */
    }
};

void awbl_nor_flash_drv_register (void)
{
    aw_err_t err;
    err = awbl_drv_register((struct awbl_drvinfo *)
                            &__g_nor_flash_drv_registration);
    while (err != 0) {
    }
    return;
}

/* end of file */

