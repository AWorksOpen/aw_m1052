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
 * \brief flash-eeprom driver source file
 *
 * define flash-eeprom driver
 *
 * \internal
 * \par modification history:
 * - 1.00 16-01-25  bob, first implementation
 * \endinternal
 */


/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "aw_flash_eeprom.h"
#include "mtd/aw_mtd.h"

#define __EEPROM_ERROR_TRANSBOUNDY  -1      /**< \brief 地址越界 */

#define __FLASH_BLOCK_STAT_VALID     0x1234 /**< \brief 块未满有效 */
#define __FLASH_BLOCK_STAT_INVALID   0x5678 /**< \brief 块满 */
#define __FLASH_BLOCK_STAT_ERASED    0xFFFF /**< \brief 块空 */

/** \brief 块的空闲区大小，用来放一些eeprom信息 */
#define __FLASH_BLOCK_OOB_SIZE (AW_FLASH_EEPROM_DATA_SIZE + AW_FLASH_EEPROM_ADDR_SIZE)

/** \brief 获取上一个地址 */
static int32_t __cur_prev_addr_get (awbl_flash_eeprom_dev_t *p_dev,
                                    uint32_t                 addr);

/** \brief 获取下一个地址 */
static int32_t __cur_next_addr_get (awbl_flash_eeprom_dev_t *p_dev,
                                     uint32_t                  addr);

/** \brief 将一个数据放新的块中 */
static int32_t __input_new_block(awbl_flash_eeprom_dev_t *p_dev,
                                 int32_t                  new_addr,
                                 int32_t                  old_addr);

/** \brief 当前块数据满，重整整理数据将有效数据放到新块中 */
static int __block_trans(awbl_flash_eeprom_dev_t *p_dev);

/** \brief 读取指定地址的数据 */
static int __flash_eeprom_read (awbl_flash_eeprom_dev_t *p_dev,
                                uint32_t                 addr,
                                uint8_t                 *p_buf);

/** \brief 写入指定地址的数据 */
static int __flash_eeprom_write (awbl_flash_eeprom_dev_t *p_dev,
                                 uint32_t                 addr,
                                 uint8_t                 *p_buf);


static int32_t __cur_prev_addr_get (awbl_flash_eeprom_dev_t *p_dev,
                                    uint32_t                 addr)
{
    /* 获取上一个flash块存储地址 */
    uint8_t val_size = AW_FLASH_EEPROM_ADDR_SIZE + AW_FLASH_EEPROM_DATA_SIZE;

    uint16_t page_useless_size = p_dev->p_info->page_size  % val_size;

    if (addr < (2 * val_size)) {
        return __EEPROM_ERROR_TRANSBOUNDY;
    }

    if (addr % p_dev->p_info->page_size == 0) {
        addr = addr - val_size;
        return addr - page_useless_size;
    } else {
        addr = addr - val_size;
        return addr;
    }
}

static int32_t __cur_next_addr_get (awbl_flash_eeprom_dev_t *p_dev,
                                     uint32_t                  addr)
{
    /* 获取下一个flash块存储地址 */
    uint8_t val_size = AW_FLASH_EEPROM_ADDR_SIZE + AW_FLASH_EEPROM_DATA_SIZE;

    uint16_t page_useless_size = p_dev->p_info->page_size  % val_size;

    uint16_t page_addr;

    addr += val_size;

    if ((addr % p_dev->p_info->block_size) == 0) {
        return __EEPROM_ERROR_TRANSBOUNDY;
    }

    page_addr = addr % p_dev->p_info->page_size;


    if (p_dev->p_info->page_size - page_addr >= val_size) {
        return addr;
    } else {

        if (addr + page_useless_size >= p_dev->p_info->block_size) {
            return __EEPROM_ERROR_TRANSBOUNDY;
        } else {
            return addr + page_useless_size;
        }
    }
}

static int32_t __input_new_block(awbl_flash_eeprom_dev_t *p_dev,
                                 int32_t                  new_addr,
                                 int32_t                  old_addr)

{
    size_t   ret_len;
    uint8_t  i;

    uint8_t  data_buf[AW_FLASH_EEPROM_DATA_SIZE + AW_FLASH_EEPROM_ADDR_SIZE];

    uint8_t  new_addr_buf[AW_FLASH_EEPROM_ADDR_SIZE];
    uint8_t  old_addr_buf[AW_FLASH_EEPROM_ADDR_SIZE];

    uint16_t new_block_num = (p_dev->w_block_num + 1) % p_dev->p_info->nblocks;
    uint16_t old_block_num =  p_dev->w_block_num;

    int32_t new_block_cur_addr = AW_FLASH_EEPROM_ADDR_SIZE +
                                 AW_FLASH_EEPROM_DATA_SIZE;

    aw_mtd_read (p_dev->p_mtd_dev,

                 p_dev->p_info->offset                     +
                 old_block_num * p_dev->p_info->block_size +
                 old_addr                                  +
                 AW_FLASH_EEPROM_DATA_SIZE,

                 AW_FLASH_EEPROM_ADDR_SIZE,
                &ret_len,
                 old_addr_buf);
    /* 在新块中查找该地址的数据，如果没有将该数据写入新块，如果有丢弃过时的数据 */
    while((new_block_cur_addr < new_addr) && (new_block_cur_addr > 0)) {

        aw_mtd_read (p_dev->p_mtd_dev,

                     p_dev->p_info->offset                     +
                     new_block_num * p_dev->p_info->block_size +
                     new_block_cur_addr                        +
                     AW_FLASH_EEPROM_DATA_SIZE,

                     AW_FLASH_EEPROM_ADDR_SIZE,
                    &ret_len,
                     new_addr_buf);


        for (i = 0; i < AW_FLASH_EEPROM_ADDR_SIZE; i++) {
            if (old_addr_buf[i] != new_addr_buf[i]) {
                break;
            }
        }

        if (i == AW_FLASH_EEPROM_ADDR_SIZE) {
            return new_addr;
        }

        new_block_cur_addr = __cur_next_addr_get(p_dev, new_block_cur_addr);
    }

    aw_mtd_read (p_dev->p_mtd_dev,

                 p_dev->p_info->offset +
                 old_block_num * p_dev->p_info->block_size +
                 old_addr,

                 AW_FLASH_EEPROM_DATA_SIZE + AW_FLASH_EEPROM_ADDR_SIZE,
                &ret_len,
                 data_buf);

    aw_mtd_write (p_dev->p_mtd_dev,

                 p_dev->p_info->offset                     +
                 new_block_num * p_dev->p_info->block_size +
                 new_addr,

                 AW_FLASH_EEPROM_DATA_SIZE + AW_FLASH_EEPROM_ADDR_SIZE,
                &ret_len,
                 data_buf);

    new_addr = __cur_next_addr_get(p_dev, new_addr);

    return new_addr;

}

static int __block_trans(awbl_flash_eeprom_dev_t *p_dev)
{
    uint16_t new_block_num = (p_dev->w_block_num + 1) % p_dev->p_info->nblocks;
    uint16_t old_block_num = p_dev->w_block_num;

    uint8_t oob_data[__FLASH_BLOCK_OOB_SIZE];

    int32_t old_block_star_addr = AW_FLASH_EEPROM_ADDR_SIZE +
                                  AW_FLASH_EEPROM_DATA_SIZE;

    int32_t new_block_cur_addr  = AW_FLASH_EEPROM_ADDR_SIZE +
                                  AW_FLASH_EEPROM_DATA_SIZE;

    off_t    fail;
    size_t   oob_size;
    int      priv;
    enum aw_mtd_erase_status   state;
    struct   aw_mtd_erase_info erase_info = {
        p_dev->p_mtd_dev,
        0,
        4096,
        fail,
        2,
        NULL,
        &priv,
        state,
        NULL,
    };

    erase_info.addr = new_block_num * p_dev->p_info->block_size +
                                      p_dev->p_info->offset;
    erase_info.len  = p_dev->p_info->block_size;
    aw_mtd_erase (p_dev->p_mtd_dev, &erase_info);

    /* 在存放数据的块中从上往下将数据搬到一个新块中，重复的数据丢弃 */
    while(p_dev->cur_addr >= old_block_star_addr) {

        new_block_cur_addr = __input_new_block(p_dev,
                                               new_block_cur_addr,
                                               p_dev->cur_addr);

        p_dev->cur_addr = __cur_prev_addr_get (p_dev, p_dev->cur_addr);

        if (p_dev->cur_addr == __EEPROM_ERROR_TRANSBOUNDY) {
            break;
        }
    }

    p_dev->cur_addr    = new_block_cur_addr;
    p_dev->w_block_num = new_block_num;

    /* 在新块里写入标志 */
    oob_data[0] = (__FLASH_BLOCK_STAT_VALID & 0xFF);
    oob_data[1] = ((__FLASH_BLOCK_STAT_VALID >> 8) & 0xFF);
    aw_mtd_write (p_dev->p_mtd_dev,
                  p_dev->p_info->block_size * new_block_num + p_dev->p_info->offset,
                  __FLASH_BLOCK_OOB_SIZE,
                 &oob_size,
                 (void *)oob_data);

    /* 檫除旧块中的数据 */
    erase_info.addr = old_block_num * p_dev->p_info->block_size +
                                      p_dev->p_info->offset;
    erase_info.len  = p_dev->p_info->block_size;
    aw_mtd_erase (p_dev->p_mtd_dev, &erase_info);


    return AW_OK;
}

int __flash_eeprom_read (awbl_flash_eeprom_dev_t *p_dev,
                         uint32_t                 addr,
                         uint8_t                 *p_buf)
{
    int32_t cur_addr;
    size_t  ret_len;

    uint8_t oob_data[4];
    uint8_t i;

    /* 在存放数据的块中从上往下查找第一个匹配的数据 */
    cur_addr = p_dev->cur_addr;
    do {

        cur_addr = __cur_prev_addr_get(p_dev, cur_addr);

        aw_mtd_read (p_dev->p_mtd_dev,

                     p_dev->p_info->block_size * p_dev->w_block_num +
                     p_dev->p_info->offset                          +
                     cur_addr                                       +
                     AW_FLASH_EEPROM_DATA_SIZE,

                     AW_FLASH_EEPROM_ADDR_SIZE,
                    &ret_len,
                    (void *)oob_data);

        if (*((uint16_t *)oob_data) == addr) { /* 2位*/
            aw_mtd_read(p_dev->p_mtd_dev,

                        p_dev->p_info->block_size * p_dev->w_block_num +
                        p_dev->p_info->offset                          +
                        cur_addr,

                        AW_FLASH_EEPROM_DATA_SIZE,
                       &ret_len,
                       (void *)p_buf);
            return AW_OK;
        }

    } while(cur_addr != __EEPROM_ERROR_TRANSBOUNDY);

    /* 没有匹配到地址默认这个地址里面的数据为空 */
    for (i = 0; i < AW_FLASH_EEPROM_DATA_SIZE; i++) {
        p_buf[i] = 0;
    }

    return AW_OK;
}

int __flash_eeprom_write (awbl_flash_eeprom_dev_t *p_dev,
                          uint32_t                 addr,
                          uint8_t                 *p_buf)
{

    size_t   ret_len;
    uint8_t  write_buf[AW_FLASH_EEPROM_DATA_SIZE + AW_FLASH_EEPROM_ADDR_SIZE];
    uint16_t i;
    int32_t  addr_tmp;


    awbl_flash_eeprom_devinfo_t *p_info = p_dev->p_info;

    if (AW_FLASH_EEPROM_ADDR_SIZE > 4) {

    }

    for (i = 0; i < AW_FLASH_EEPROM_DATA_SIZE; i++) {
        write_buf[i] = p_buf[i];
    }

    for ( ; i < (AW_FLASH_EEPROM_DATA_SIZE + AW_FLASH_EEPROM_ADDR_SIZE); i++) {
        write_buf[i] = (addr & 0xFF);
        addr = (addr >> 8);
    }


    aw_mtd_write(p_dev->p_mtd_dev,

                 p_info->block_size * p_dev->w_block_num +
                 p_info->offset                          +
                 p_dev->cur_addr,

                 AW_FLASH_EEPROM_DATA_SIZE + AW_FLASH_EEPROM_ADDR_SIZE,
                &ret_len,
                 write_buf);

    addr_tmp = __cur_next_addr_get(p_dev, p_dev->cur_addr);

    if (addr_tmp < 0) {
        __block_trans(p_dev);
        if (p_dev->cur_addr < 0) {
            return -2;
        }
    } else {
        p_dev->cur_addr = addr_tmp;
    }


    return AW_OK;
}

aw_err_t aw_flash_eeprom_read (awbl_flash_eeprom_dev_t *p_dev,
                               uint32_t                 addr,
                               void                    *p_buf,
                               uint32_t                 len)
{
    uint32_t i, j;
    int      ret;
    uint8_t  data_buf[AW_FLASH_EEPROM_DATA_SIZE];

    if (NULL == p_dev && NULL == p_buf) {
        return -EINVAL;
    }

    aw_mutex_lock(p_dev->mutex_id, AW_SEM_WAIT_FOREVER);

    /* 读取数据 */
    for (i = 0; i < len ; i += AW_FLASH_EEPROM_DATA_SIZE) {

        if ((len - i) >= AW_FLASH_EEPROM_DATA_SIZE) {
            ret = __flash_eeprom_read(p_dev,
                                      addr,
                                     (uint8_t *)p_buf);
            p_buf += AW_FLASH_EEPROM_DATA_SIZE;
            addr++;
        } else {
            ret = __flash_eeprom_read(p_dev,
                                      addr,
                                      data_buf);
            for(j = 0; j < len - i; j++) {
                ((uint8_t *)p_buf)[j] = data_buf[j];
            }
        }

        if (ret != AW_OK) {

        }
        addr++;
    }
    aw_mutex_unlock(p_dev->mutex_id);

    return AW_OK;
}

aw_err_t aw_flash_eeprom_write (awbl_flash_eeprom_dev_t *p_dev,
                                uint32_t                 addr,
                                void                    *p_buf,
                                uint32_t                 len)
{
    uint32_t i, j;
    int      ret;
    uint8_t  data_buf[AW_FLASH_EEPROM_DATA_SIZE];

    if (NULL == p_dev && NULL == p_buf) {
        return -EINVAL;;
    }

    aw_mutex_lock(p_dev->mutex_id, AW_SEM_WAIT_FOREVER);

    /* 将数据写入 */
    for (i = 0; i < len ; i += AW_FLASH_EEPROM_DATA_SIZE) {

        if ((len - i) >= AW_FLASH_EEPROM_DATA_SIZE) {
            ret = __flash_eeprom_write(p_dev,
                                    addr,
                                   (uint8_t *)p_buf);
            p_buf += AW_FLASH_EEPROM_DATA_SIZE;
            addr++;
        } else {

            for(j = 0; j < len - i; j++) {
                data_buf[j] = ((uint8_t *)p_buf)[j];
            }
            ret = __flash_eeprom_write(p_dev,
                                    addr,
                                    data_buf);

        }

        if (ret != AW_OK) {
            return ret;
        }
        addr++;
    }
    aw_mutex_unlock(p_dev->mutex_id);

    return AW_OK;
}

void aw_flash_eeprom_allclean (awbl_flash_eeprom_dev_t   *p_dev)
{
    off_t    fail;
    int      priv;
    enum aw_mtd_erase_status   state;
    struct   aw_mtd_erase_info erase_info = {
        p_dev->p_mtd_dev,
        0,
        4096,
        fail,
        2,
        NULL,
        &priv,
        state,
        NULL,
    };

    if (NULL == p_dev) {
        return ;
    }
    /* 檫除所有的数据 */
    erase_info.addr = p_dev->p_info->offset;
    erase_info.len  = p_dev->p_info->block_size * p_dev->p_info->nblocks;

    aw_mutex_lock(p_dev->mutex_id, AW_SEM_WAIT_FOREVER);
    aw_mtd_erase (p_dev->p_mtd_dev, &erase_info);
    aw_mutex_unlock(p_dev->mutex_id);

    aw_flash_eeprom_init(p_dev, p_dev->p_info);

    return ;
}

aw_err_t aw_flash_eeprom_init (awbl_flash_eeprom_dev_t     *p_dev,
                               awbl_flash_eeprom_devinfo_t *p_info)
{
    uint16_t i;
    int32_t  addr;
    size_t   oob_size;
    uint8_t  oob_data[__FLASH_BLOCK_OOB_SIZE];
    off_t    fail;
    int      priv;
    enum aw_mtd_erase_status   state;
    struct   aw_mtd_erase_info erase_info = {
        p_dev,
        0,
        4096,
        fail,
        2,
        NULL,
        &priv,
        state,
        NULL,
    };

    if (NULL == p_dev || NULL == p_info) {
        return -EINVAL;
    }


    p_dev->p_mtd_dev = aw_mtd_dev_find(p_info->name);

    if (!p_dev->p_mtd_dev) {
        return -ENXIO;
    }

    erase_info.mtd = p_dev->p_mtd_dev;

    p_dev->mutex_id = AW_MUTEX_INIT(p_dev->mutex, AW_SEM_Q_PRIORITY);

    p_dev->p_info = p_info;

    p_dev->w_block_num = 0xFFFF;


    aw_mutex_lock(p_dev->mutex_id, AW_SEM_WAIT_FOREVER);

    /* 查找存放数据数据的块 */
    for (i = 0; i < p_info->nblocks; i++) {

        aw_mtd_read(p_dev->p_mtd_dev,
                    p_info->block_size * i + p_info->offset,
                    __FLASH_BLOCK_OOB_SIZE,
                   &oob_size,
                   (void *)oob_data);

        if (oob_size == __FLASH_BLOCK_OOB_SIZE) {

            if ( *((uint16_t *)oob_data) == __FLASH_BLOCK_STAT_VALID) {
                p_dev->w_block_num = i;
            } else if ( *((uint16_t *)oob_data) == __FLASH_BLOCK_STAT_INVALID) {
                if (p_dev->w_block_num > p_info->nblocks) {
                    p_dev->w_block_num = i;
                }
            } else if (*((uint16_t *)oob_data) != __FLASH_BLOCK_STAT_ERASED) {
                erase_info.addr = i * p_dev->p_info->block_size + p_info->offset;
                erase_info.len  = p_dev->p_info->block_size;
                aw_mtd_erase (p_dev->p_mtd_dev, &erase_info);
            } else {

            }
        }
    }

    /* 如果所有块中没有数据存放，檫除所有块 */
    if (p_dev->w_block_num > p_info->nblocks) {
        p_dev->w_block_num = 0;
        oob_data[0] = (__FLASH_BLOCK_STAT_VALID & 0xFF);
        oob_data[1] = ((__FLASH_BLOCK_STAT_VALID >> 8) & 0xFF);

        erase_info.addr = p_dev->w_block_num * p_dev->p_info->block_size +
                                                p_info->offset;
        erase_info.len  = p_dev->p_info->block_size;
        aw_mtd_erase(p_dev->p_mtd_dev, &erase_info);

        /* 在当前块写入数据未满标志 */
        aw_mtd_write(p_dev->p_mtd_dev,
                     p_info->block_size * p_dev->w_block_num + p_info->offset,
                     __FLASH_BLOCK_OOB_SIZE,
                    &oob_size,
                    (void *)oob_data);

        p_dev->cur_addr = __FLASH_BLOCK_OOB_SIZE;
    } else {
        addr = p_dev->p_info->block_size;

        /* 初始化查找下一个存放数据的地址 */
        while((addr = __cur_prev_addr_get(p_dev, addr)) >=
               __FLASH_BLOCK_OOB_SIZE) {

            aw_mtd_read(p_dev->p_mtd_dev,

                        p_info->block_size * p_dev->w_block_num +
                        p_info->offset                          +
                        addr,

                        __FLASH_BLOCK_OOB_SIZE,
                       &oob_size,
                       (void *)oob_data);

            for (i = AW_FLASH_EEPROM_DATA_SIZE; i < __FLASH_BLOCK_OOB_SIZE; i++) {
                if (oob_data[AW_FLASH_EEPROM_DATA_SIZE] != 0xFF) {
                    break;
                }
            }

            /* 如果该地址放有数据 */
            if (i != __FLASH_BLOCK_OOB_SIZE) {
                p_dev->cur_addr = __cur_next_addr_get(p_dev, addr);
                if (p_dev->cur_addr < 0) {
                    p_dev->cur_addr = addr;
                    __block_trans(p_dev);
                } else {
                    break;
                }
            }
        }

        if (addr < __FLASH_BLOCK_OOB_SIZE) {
            p_dev->cur_addr = __cur_next_addr_get(p_dev, 0);
        }
    }

    aw_mutex_unlock(p_dev->mutex_id);

    return AW_OK;
}

void aw_flash_eeprom_uninit (awbl_flash_eeprom_dev_t  *p_dev)
{

}
