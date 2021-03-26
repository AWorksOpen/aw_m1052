/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
**** ***************************************************************************/

/**
 * \file
 * \brief SPIFI标准接口头文件
 *
 * \internal
 * \par modification history:
 * - 1.00 18-01-31  mex, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_wdt.h"
#include "driver/flexspi/awbl_imx1050_flexspi.h"
#include "driver/flexspi/awbl_imx1050_flexspi_service.h"


static awbl_imx1050_flexspi_service_t __gp_flexspi_serv ;


AWBL_METHOD_IMPORT(awbl_imx1050_flexspi_serv_func_get);
AWBL_METHOD_DEF(awbl_imx1050_flexspi_serv_func_get,
                "awbl_imx1050_flexspi_serv_func_get");

#define __FLEXSPI_DEVINFO_DECL(p_devinfo) \
    struct awbl_imx1050_flexspi_devinfo *p_devinfo = \
        (struct awbl_imx1050_flexspi_devinfo *)AWBL_DEVINFO_GET(__gp_flexspi_serv.p_dev)


/**
 * \brief allocate imx1050 flexspi service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __imx1050_flexspi_serv_helper (
        struct awbl_dev *p_dev,
        void *p_arg) {
    if( NULL != __gp_flexspi_serv.p_func ) {
        return AW_OK;
    }

    awbl_method_handler_t   pfn_imx1050_flexspi_get_serv = NULL;
    pfn_imx1050_flexspi_get_serv = awbl_dev_method_get(p_dev,
            AWBL_METHOD_CALL(awbl_imx1050_flexspi_serv_func_get));

    __gp_flexspi_serv.p_dev = (void *)p_dev;

    if(NULL != pfn_imx1050_flexspi_get_serv) {
        pfn_imx1050_flexspi_get_serv(p_dev, &(__gp_flexspi_serv.p_func));
    }
    return AW_OK;
}

/**
 * \brief Flexspi initial
 */
void awbl_imx1050_flexspi_init (void)
{

    awbl_dev_iterate(__imx1050_flexspi_serv_helper,
                     NULL,
                     AWBL_ITERATE_INSTANCES);

}


/* flexspi nor flash读数据 */
aw_err_t  awbl_flexspi_nor_flash_read (uint32_t   addr,
                                       uint8_t   *p_buf,
                                       uint32_t   bytes)
{
    aw_err_t ret = __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_read(
                                                        __gp_flexspi_serv.p_dev,
                                                        addr,
                                                        p_buf,
                                                        bytes);
    /* 将cache中的数据无效化， 重新从内存读取 */
    aw_cache_invalidate((const void *)(FLEXSPI_AMBA_BASE + addr), bytes);

    return ret;
}


/* flexspi nor flash读状态 */
aw_err_t  awbl_flexspi_nor_flash_read_status(uint8_t   *p_buf,
                                             uint32_t   bytes)
{
    awbl_hwwdt_feed();
    return __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_read_status(
                                                         __gp_flexspi_serv.p_dev,
                                                         p_buf,
                                                         bytes);
}


/* flexspi nor flash写状态 */
aw_err_t  awbl_flexspi_nor_flash_write_status(uint8_t *p_buf)
{
    awbl_hwwdt_feed();
    return __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_write_status(
                                                         __gp_flexspi_serv.p_dev,
                                                         *p_buf);
}


/* flexspi nor flash read function */
aw_err_t  awbl_flexspi_nor_flash_read_function(uint8_t   *p_buf,
                                               uint32_t   bytes)
{

    return __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_read_function(
                                                         __gp_flexspi_serv.p_dev,
                                                         p_buf,
                                                         bytes);
}


/* flexspi nor flash write function */
aw_err_t  awbl_flexspi_nor_flash_write_function(uint8_t *p_buf)
{
    awbl_hwwdt_feed();
    return __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_write_function(
                                                         __gp_flexspi_serv.p_dev,
                                                         *p_buf);
}


/* flexspi nor flash写使能 */
aw_err_t  awbl_flexspi_nor_flash_write_enable(uint32_t  addr)
{

    return __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_write_enable(
                                                         __gp_flexspi_serv.p_dev,
                                                         addr);

}


/* flexspi nor flash擦除扇区 */
aw_err_t awbl_flexspi_nor_flash_erase (uint32_t  addr)
{
    __FLEXSPI_DEVINFO_DECL(p_devinfo);
    aw_err_t ret;

    awbl_hwwdt_feed();
    ret = __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_erase(
                                                        __gp_flexspi_serv.p_dev,
                                                        addr);

    /* 将cache中的数据无效化， 重新从内存读取 */
    aw_cache_invalidate((const void *)(FLEXSPI_AMBA_BASE + addr), p_devinfo->dev_cfg.sector_size);

    return ret;
}


/* flexspi nor flash编程*/
aw_err_t awbl_flexspi_nor_flash_program (uint32_t   addr,
                                         uint8_t   *p_buf,
                                         uint32_t   bytes)
{
    aw_err_t ret;
    
    awbl_hwwdt_feed();
    
    ret = __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_program(
                                                        __gp_flexspi_serv.p_dev,
                                                        addr,
                                                        p_buf,
                                                        bytes);
    /* 将cache中的数据无效化， 重新从内存读取 */
    aw_cache_invalidate((const void *)(FLEXSPI_AMBA_BASE + addr), bytes);

    return ret;
}

/* flexspi nor flash read information row */
aw_err_t awbl_flexspi_nor_flash_info_read(uint8_t  index,
                                          uint8_t  addr,
                                          uint8_t *p_buf,
                                          uint32_t bytes)
{
    uint32_t offset;

    if (__gp_flexspi_serv.p_func == NULL) {
        return -AW_ENXIO;
    }

    if ((p_buf == NULL) || (bytes == 0) || (index > 3)) {
        return -AW_EINVAL;
    }
    
    awbl_hwwdt_feed();

    offset = (index * 0x1000) + addr;

    return __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_read_ir(
                                                        __gp_flexspi_serv.p_dev,
                                                        offset,
                                                        p_buf,
                                                        bytes);
}


/* flexspi nor flash erase information row*/
aw_err_t awbl_flexspi_nor_flash_info_erase (uint8_t index)
{
    if (__gp_flexspi_serv.p_func == NULL) {
        return -AW_ENXIO;
    }

    if (index > 3) {
        return -AW_EINVAL;
    }
    
    awbl_hwwdt_feed();

    return __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_erase_ir(
                                                        __gp_flexspi_serv.p_dev,
                                                        index * 0x1000);
}


/* flexspi nor flash program information row*/
aw_err_t awbl_flexspi_nor_flash_info_program (uint8_t  index,
                                              uint8_t  addr,
                                              uint8_t *p_buf,
                                              uint32_t bytes)
{
    uint32_t offset;

    if (__gp_flexspi_serv.p_func == NULL) {
        return -AW_ENXIO;
    }

    if ((p_buf == NULL) || (bytes == 0) || (index > 3)) {
        return -AW_EINVAL;
    }
    
    awbl_hwwdt_feed();

    offset = (index * 0x1000) + addr;

    return __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_write_ir(
                                                        __gp_flexspi_serv.p_dev,
                                                        offset,
                                                        p_buf,
                                                        bytes);
}

/* end of file */
