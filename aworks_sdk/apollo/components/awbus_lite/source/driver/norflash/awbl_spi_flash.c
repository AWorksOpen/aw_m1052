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
 * \brief SPI-Flash Çý¶¯
 *
 * \internal
 * \par modification history:
 * - 1.00 14-10-31  deo, first implementation
 * \endinternal
 */


/*******************************************************************************
  includes
*******************************************************************************/
#include "aworks.h"
#include "awbus_lite.h"
#include "aw_list.h"
#include "aw_sem.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_spi_flash.h"
#include "driver/norflash/awbl_spi_flash.h"
#include "string.h"



#define __AWDEV_TO_SPIFLASH(p_awdev) \
            AW_CONTAINER_OF(AW_CONTAINER_OF( \
                                p_awdev, \
                                struct awbl_spi_device, \
                                super), \
                            struct awbl_spi_flash_dev, \
                            spi)

#define __AWDEV_TO_SPIFLASH_INFO(p_awdev) \
            (struct awbl_spi_flash_devinfo *)AWBL_DEVINFO_GET(p_awdev)




#define __SFLASH_TRACE(info)  AW_WARNF(info)

extern aw_const struct awbl_dev_method g_spi_flash_methods[];

aw_local struct aw_list_head  __g_spi_flash_list;
AW_MUTEX_DECL(                __g_spi_flash_list_lock);



aw_local void __drv_init1 (struct awbl_dev *p_awdev);
aw_local void __drv_init2 (struct awbl_dev *p_awdev);
aw_local void __drv_connect (struct awbl_dev *p_awdev);




/******************************************************************************/
aw_local aw_const struct awbl_drvfuncs __g_drvfuncs = {
        __drv_init1,
        __drv_init2,
        __drv_connect
};




/******************************************************************************/
aw_local aw_const struct awbl_drvinfo __g_spi_flash_drv_registration = {
    AWBL_VER_1,                             /**< \brief awb_ver */
    AWBL_BUSID_SPI,                         /**< \brief bus_id */
    AWBL_SPI_FLASH_NAME,                    /**< \brief p_drvname */
    &__g_drvfuncs,                          /**< \brief p_busfuncs */
    &g_spi_flash_methods[0],                /**< \brief p_methods */
    NULL                                    /**< \brief pfunc_drv_probe */
};




/******************************************************************************/
struct awbl_spi_flash_dev *__flash_find (const char *name)
{
    struct awbl_spi_flash_dev *p_flash;
    int                        len;

    len = strlen(name);
    if (len == 0) {
        return NULL;
    }

    AW_MUTEX_LOCK(__g_spi_flash_list_lock, AW_SEM_WAIT_FOREVER);
    aw_list_for_each_entry(p_flash,
                           &__g_spi_flash_list,
                           struct awbl_spi_flash_dev,
                           node) {
        if (strlen(p_flash->p_info->name) == len) {
            if (strncmp(p_flash->p_info->name, name, len) == 0) {
                AW_MUTEX_UNLOCK(__g_spi_flash_list_lock);
                return p_flash;
            }
        }
    }
    AW_MUTEX_UNLOCK(__g_spi_flash_list_lock);
    return NULL;
}




/******************************************************************************/
aw_local void __drv_init1 (struct awbl_dev *p_awdev)
{
    return ;
}




/******************************************************************************/
aw_local void __drv_init2 (struct awbl_dev *p_awdev)
{
    struct awbl_spi_flash_dev     *p_flash;
    struct awbl_spi_flash_devinfo *p_info;
    aw_err_t                       ret;
    int                            i;

    p_flash = __AWDEV_TO_SPIFLASH(p_awdev);
    p_info  = __AWDEV_TO_SPIFLASH_INFO(p_awdev);
    p_flash->p_info = p_info;
    p_flash->size   = p_info->block_size * p_info->nblocks;

    /* platform initialization */
    if (p_info->pfunc_plfm_init != NULL) {
        p_info->pfunc_plfm_init();
    }

    if (AW_MUTEX_INIT(p_flash->devlock, AW_SEM_Q_PRIORITY) == NULL) {
        return;
    }

    /* SPI device */
    aw_spi_mkdev(&(p_flash->spi.spi_dev),
                 p_flash->spi.super.p_devhcf->bus_index,
                 8,
                 p_info->spi_mode,
                 p_info->spi_speed,
                 p_info->spi_cs_pin,
                 NULL);

    aw_spi_setup(&(p_flash->spi.spi_dev));
    if (p_info->spi_speed != p_flash->spi.spi_dev.max_speed_hz) {
        __SFLASH_TRACE(("ax speed %d not be supported, change to %d.\n",
                        p_info->spi_speed,
                        p_flash->spi.spi_dev.max_speed_hz));
    }

    if ((p_info->p_mtd_ex) && (p_info->p_mtd_ex->pfn_init)) {
        p_info->p_mtd_ex->pfn_init(p_flash,
                                   (void *)p_info->p_mtd_ex->p_mtd,
                                   (void *)p_info->p_mtd_ex->p_info);
    }

    return;
}




/******************************************************************************/
aw_local void __drv_connect (struct awbl_dev *p_awdev)
{
    struct awbl_spi_flash_dev  *p_flash;

    p_flash = __AWDEV_TO_SPIFLASH(p_awdev);

    AW_MUTEX_LOCK(__g_spi_flash_list_lock, AW_SEM_WAIT_FOREVER);
    aw_list_add_tail(&p_flash->node, &__g_spi_flash_list);
    AW_MUTEX_UNLOCK(__g_spi_flash_list_lock);

    return ;
}




/******************************************************************************/
void awbl_spi_flash_lib_init (void)
{
    aw_list_head_init(&__g_spi_flash_list);
    AW_MUTEX_INIT(__g_spi_flash_list_lock, AW_SEM_Q_PRIORITY);
}




/******************************************************************************/
void awbl_spi_flash_drv_register (void)
{
    aw_err_t err;
    err = awbl_drv_register((struct awbl_drvinfo *)
                            &__g_spi_flash_drv_registration);
    while (err != 0) {
    }
    return;
}




/******************************************************************************/
aw_local aw_err_t __status_reg_get (struct awbl_spi_flash_dev *p_dev,
                                    uint8_t                   *status)
{
    struct aw_spi_message  msg;
    struct aw_spi_transfer trans1;
    struct aw_spi_transfer trans2;
    uint8_t                reg;

    aw_spi_msg_init(&msg, NULL, NULL);

    reg = 0x05;

    aw_spi_mktrans(&trans1,
                   &reg,
                   NULL,
                   1,
                   0,
                   0,
                   0,
                   p_dev->spi.spi_dev.max_speed_hz,
                   0);

    aw_spi_mktrans(&trans2,
                   NULL,
                   status,
                   1,
                   0,
                   0,
                   0,
                   p_dev->spi.spi_dev.max_speed_hz,
                   0);

    aw_spi_trans_add_tail(&msg, &trans1);
    aw_spi_trans_add_tail(&msg, &trans2);

    return aw_spi_sync(&(p_dev->spi.spi_dev), &msg);
}




/******************************************************************************/
aw_local aw_err_t __busy_wait (struct awbl_spi_flash_dev *p_dev)
{
    uint8_t     status;
    uint32_t    timeout = 0;
    aw_err_t    ret;
#define __TIMEOUT_US    (2*1000*1000)

    while(1) {
        __status_reg_get(p_dev, &status);
        if ((status & 0x03) == 0x03) {
            aw_udelay(1);
            timeout++;
            if (timeout == __TIMEOUT_US) {
                ret = -AW_ETIME;
                break;
            }
        } else {
            ret = AW_OK;
            break;
        }
    }

    return ret;
}




/******************************************************************************/
aw_local aw_err_t __wr_enable (struct awbl_spi_flash_dev *p_dev)
{
    struct aw_spi_message   msg;
    struct aw_spi_transfer  trans;
    uint8_t                 reg;

    aw_spi_msg_init(&msg, NULL, NULL);

    reg = 0x06;

    aw_spi_mktrans(&trans,
                   &reg,
                   NULL,
                   1,
                   0,
                   0,
                   0,
                   p_dev->spi.spi_dev.max_speed_hz,
                   0);

    aw_spi_trans_add_tail(&msg, &trans);

    return aw_spi_sync(&(p_dev->spi.spi_dev), &msg);
}




/******************************************************************************/
aw_local aw_err_t __page_write (struct awbl_spi_flash_dev *p_dev,
                                uint32_t                   addr,
                                uint8_t                   *p_data,
                                uint32_t                   len)
{
    struct aw_spi_message   msg;
    struct aw_spi_transfer  trans1;
    struct aw_spi_transfer  trans2;
    uint8_t                 cmd[4];

    if (AW_OK != __busy_wait(p_dev)) {
        return -AW_EBUSY;
    }

    __wr_enable(p_dev);

    if (AW_OK != __busy_wait(p_dev)) {
        return -AW_EBUSY;
    }
    aw_spi_msg_init(&msg, NULL, NULL);

    cmd[0] = 0x02;
    cmd[1] = (addr & 0xFFFFFF) >> 16;
    cmd[2] = (addr & 0xFFFF) >> 8;
    cmd[3] = addr & 0xFF;

    aw_spi_mktrans(&trans1,
                   cmd,
                   NULL,
                   4,
                   0,
                   0,
                   0,
                   p_dev->spi.spi_dev.max_speed_hz,
                   0);

    aw_spi_mktrans(&trans2,
                   p_data,
                   NULL,
                   len,
                   0,
                   0,
                   0,
                   p_dev->spi.spi_dev.max_speed_hz,
                   0);

    aw_spi_trans_add_tail(&msg, &trans1);
    aw_spi_trans_add_tail(&msg, &trans2);

    if (AW_OK != aw_spi_sync(&(p_dev->spi.spi_dev), &msg)) {
        return -AW_EIO;
    }

    return AW_OK;
}




/******************************************************************************/
aw_local aw_err_t __page_read (struct awbl_spi_flash_dev *p_dev,
                               uint32_t                   addr,
                               uint8_t                   *p_buf,
                               uint32_t                   len)
{
    struct aw_spi_message   msg;
    struct aw_spi_transfer  trans1;
    struct aw_spi_transfer  trans2;
    uint8_t                 cmd[5];

    if (AW_OK != __busy_wait(p_dev)) {
        return -AW_EBUSY;
    }

    /* initialize message */
    aw_spi_msg_init(&msg, NULL, NULL);

    cmd[0] = 0x0b;
    cmd[1] = (addr & 0xFFFFFF) >> 16;
    cmd[2] = (addr & 0xFFFF) >> 8;
    cmd[3] = addr & 0xFF;
    cmd[4] = 0xFF;

    aw_spi_mktrans(&trans1,
                   cmd,
                   NULL,
                   5,
                   0,
                   0,
                   0,
                   p_dev->spi.spi_dev.max_speed_hz,
                   0);

    aw_spi_mktrans(&trans2,
                   NULL,
                   p_buf,
                   len,
                   0,
                   0,
                   0,
                   p_dev->spi.spi_dev.max_speed_hz,
                   0);

    aw_spi_trans_add_tail(&msg, &trans1);
    aw_spi_trans_add_tail(&msg, &trans2);

    if (AW_OK != aw_spi_sync(&(p_dev->spi.spi_dev), &msg)) {
        return -AW_EIO;
    }

    return AW_OK;
}




/******************************************************************************/
aw_err_t awbl_spi_flash_read (struct awbl_spi_flash_dev *p_dev,
                              uint32_t                   addr,
                              uint8_t                   *p_buf,
                              uint32_t                   len,
                              uint32_t                  *rlen)
{
    aw_err_t  ret;

    if ((p_dev == NULL) ||
        (p_buf == NULL)) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dev->devlock, AW_SEM_WAIT_FOREVER);

    len = min(len, (p_dev->size - addr));
    ret = __page_read(p_dev, addr, p_buf, len);

    AW_MUTEX_UNLOCK(p_dev->devlock);

    if ((ret == AW_OK) && (rlen)) {
        *rlen = len;
    }

    return ret;
}




/******************************************************************************/
aw_err_t awbl_spi_flash_write (struct awbl_spi_flash_dev *p_dev,
                               uint32_t                   addr,
                               uint8_t                   *p_buf,
                               uint32_t                   len,
                               uint32_t                  *rlen)
{
    uint8_t    *ptr;
    uint32_t    tmp, align;
    aw_err_t    ret;

    if ((p_dev == NULL) ||
        (p_buf == NULL) ||
        (addr >= p_dev->size)) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dev->devlock, AW_SEM_WAIT_FOREVER);

    ptr   = p_buf;
    align = p_dev->p_info->page_size - (addr & (p_dev->p_info->page_size - 1));
    while (len) {
        tmp = min(len, p_dev->p_info->page_size);
        tmp = min(tmp, align);

        ret = __page_write(p_dev, addr, ptr, tmp);
        if (ret != AW_OK) {
            break;
        }

        len   -= tmp;
        ptr   += tmp;
        addr  += tmp;
        align  = p_dev->p_info->page_size;

        if (addr >= p_dev->size) {
            break;
        }
    }

    AW_MUTEX_UNLOCK(p_dev->devlock);

    if ((ret == AW_OK) && (rlen)) {
        *rlen = (uint32_t)(ptr - p_buf);
    }

    return ret;
}




/******************************************************************************/
aw_err_t awbl_spi_flash_erase (struct awbl_spi_flash_dev *p_dev,
                               uint32_t                   addr)
{
    struct aw_spi_message  msg;
    struct aw_spi_transfer trans;
    uint8_t                cmd[4];
    aw_err_t               ret;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_dev->devlock, AW_SEM_WAIT_FOREVER);

    if (AW_OK != __busy_wait(p_dev)) {
        AW_MUTEX_UNLOCK(p_dev->devlock);
        return -AW_EBUSY;
    }

    __wr_enable(p_dev);

    if (AW_OK != __busy_wait(p_dev)) {
        AW_MUTEX_UNLOCK(p_dev->devlock);
        return -AW_EBUSY;
    }

    aw_spi_msg_init(&msg, NULL, NULL);

    cmd[0] = 0x20;
    cmd[1] = (addr & 0xFFFFFF) >> 16;
    cmd[2] = (addr & 0xFFFF) >> 8;
    cmd[3] = addr & 0xFF;

    aw_spi_mktrans(&trans,
                   cmd,
                   NULL,
                   4,
                   0,
                   0,
                   0,
                   p_dev->spi.spi_dev.max_speed_hz,
                   0);

    aw_spi_trans_add_tail(&msg, &trans);

    ret = aw_spi_sync(&(p_dev->spi.spi_dev), &msg);

    AW_MUTEX_UNLOCK(p_dev->devlock);

    if (ret != AW_OK) {
        return -AW_EIO;
    }

    return AW_OK;
}




/******************************************************************************/
struct awbl_spi_flash_dev *awbl_spi_flash_open (const char *name)
{
    return __flash_find(name);
}




/******************************************************************************/
aw_err_t awbl_spi_flash_close (struct awbl_spi_flash_dev *p_flash)
{
    if (p_flash == NULL) {
        return -AW_EINVAL;
    }

    /* TODO */


    return AW_OK;
}




/******************************************************************************/
aw_inline aw_spi_flash_t aw_spi_flash_open (const char *name)
{
    return (aw_spi_flash_t)awbl_spi_flash_open(name);
}




/******************************************************************************/
aw_inline aw_err_t aw_spi_flash_close (aw_spi_flash_t p_flash)
{
    return awbl_spi_flash_close((struct awbl_spi_flash_dev *)p_flash);
}




/******************************************************************************/
aw_inline aw_err_t aw_spi_flash_read (aw_spi_flash_t   p_flash,
                                      uint32_t         addr,
                                      void            *p_buf,
                                      uint32_t         len,
                                      uint32_t        *rlen)
{
    return awbl_spi_flash_read((struct awbl_spi_flash_dev *)p_flash,
                                addr,
                                p_buf,
                                len,
                                rlen);
}




/******************************************************************************/
aw_inline aw_err_t aw_spi_flash_write (aw_spi_flash_t   p_flash,
                                       uint32_t         addr,
                                       void            *p_data,
                                       uint32_t         len,
                                       uint32_t        *rlen)
{
    return awbl_spi_flash_write((struct awbl_spi_flash_dev *)p_flash,
                                addr,
                                p_data,
                                len,
                                rlen);
}




/******************************************************************************/
aw_inline aw_err_t aw_spi_flash_erase (aw_spi_flash_t  p_flash,
                                       uint32_t        addr)
{
    return awbl_spi_flash_erase((struct awbl_spi_flash_dev *)p_flash, addr);
}




/******************************************************************************/
aw_err_t aw_spi_flash_info_get (aw_spi_flash_t       p_flash,
                                aw_spi_flash_info_t *p_info)
{
    struct awbl_spi_flash_dev *p_this;


    if ((p_flash == NULL) ||
        (p_info == NULL)) {
        return -AW_EINVAL;
    }

    p_this = (struct awbl_spi_flash_dev *)p_flash;

    p_info->block_size = p_this->p_info->block_size;
    p_info->nblocks    = p_this->p_info->nblocks;
    p_info->page_size  = p_this->p_info->page_size;

    return AW_OK;
}

/* end of file */
