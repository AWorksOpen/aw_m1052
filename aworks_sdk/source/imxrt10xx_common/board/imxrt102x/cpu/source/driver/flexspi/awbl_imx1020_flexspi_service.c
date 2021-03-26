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
 * - 1.00 19-05-21  ral, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_int.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include "aw_vdebug.h"
#include "driver/flexspi/awbl_imx1020_flexspi.h"
#include "driver/flexspi/awbl_imx1020_flexspi_service.h"

aw_local awbl_imx1020_flexspi_service_t __gp_flexspi_serv ;

AWBL_METHOD_DEF(awbl_imx1020_flexspi_serv_func_get,
                "awbl_imx1020_flexspi_serv_func_get");

/**
 * \brief allocate imx1020 flexspi service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __imx1020_flexspi_serv_helper (struct awbl_dev *p_dev,
                                                 void            *p_arg)
{
    if( NULL != __gp_flexspi_serv.p_func ) {
        return AW_OK;
    }

    awbl_method_handler_t   pfn_imx1020_flexspi_get_serv = NULL;
    pfn_imx1020_flexspi_get_serv = awbl_dev_method_get(p_dev,
            AWBL_METHOD_CALL(awbl_imx1020_flexspi_serv_func_get));

    __gp_flexspi_serv.p_dev = (void *)p_dev;

    if(NULL != pfn_imx1020_flexspi_get_serv) {
        pfn_imx1020_flexspi_get_serv(p_dev, &(__gp_flexspi_serv.p_func));
    }
    return AW_OK;
}


/**
 * \brief Flexspi initial
 */
void awbl_imx1020_flexspi_init (void)
{

    awbl_dev_iterate(__imx1020_flexspi_serv_helper,
                     NULL,
                     AWBL_ITERATE_INSTANCES);
}


/* flexspi nor flash读数据 */
aw_err_t  imx1020_nor_flash_read (uint8_t *p_buf, uint32_t offset, uint32_t nbytes)
{
    RTK_INT_STATUS_DECL(old);
    aw_err_t ret;

    if (__gp_flexspi_serv.p_func == NULL) {
        return -AW_ENXIO;
    }

    if ((p_buf == NULL) || (nbytes == 0)) {
        return -AW_EINVAL;
    }

    RTK_INTERRUPT_DISABLE(old);

    ret = __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_read(
                                                        __gp_flexspi_serv.p_dev,
                                                        offset,
                                                        p_buf,
                                                        nbytes);

    RTK_INTERRUPT_ENABLE(old);

    return ret;
}


/* flexspi nor flash擦除扇区 */
aw_err_t imx1020_nor_flash_sector_erase (uint32_t offset)
{
    RTK_INT_STATUS_DECL(old);
    aw_err_t ret;

    if (__gp_flexspi_serv.p_func == NULL) {
        return -AW_ENXIO;
    }

    RTK_INTERRUPT_DISABLE(old);

    ret = __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_erase(
                                                        __gp_flexspi_serv.p_dev,
                                                        offset);
    RTK_INTERRUPT_ENABLE(old);

    return ret;
}


/* flexspi nor flash页编程*/
aw_err_t imx1020_nor_flash_page_program (uint8_t *p_buf,
                                         uint32_t offset,
                                         uint16_t nbytes)
{
    RTK_INT_STATUS_DECL(old);
    aw_err_t ret;

    if (__gp_flexspi_serv.p_func == NULL) {
        return -AW_ENXIO;
    }

    if ((p_buf == NULL) || (nbytes == 0)) {
        return -AW_EINVAL;
    }

    RTK_INTERRUPT_DISABLE(old);

    ret = __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_program(
                                                        __gp_flexspi_serv.p_dev,
                                                        offset,
                                                        p_buf,
                                                        nbytes);

    RTK_INTERRUPT_ENABLE(old);

    return ret;
}


/* flexspi nor flash read function register */
aw_err_t imx1020_nor_flash_read_fr(uint8_t *p_buf)
{
    RTK_INT_STATUS_DECL(old);
    aw_err_t ret;

    if (__gp_flexspi_serv.p_func == NULL) {
        return -AW_ENXIO;
    }

    if (p_buf == NULL) {
        return -AW_EINVAL;
    }

    RTK_INTERRUPT_DISABLE(old);

    ret =  __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_rdfr(
                                                        __gp_flexspi_serv.p_dev,
                                                        p_buf);

    RTK_INTERRUPT_ENABLE(old);

    return ret;
}


/* flexspi nor flash write function register */
aw_err_t imx1020_nor_flash_write_fr(uint8_t *p_buf)
{
    RTK_INT_STATUS_DECL(old);
    aw_err_t ret;

    if (__gp_flexspi_serv.p_func == NULL) {
        return -AW_ENXIO;
    }

    if (p_buf == NULL) {
        return -AW_EINVAL;
    }

    RTK_INTERRUPT_DISABLE(old);

    ret = __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_wrfr(
                                                      __gp_flexspi_serv.p_dev,
                                                                        p_buf);
    RTK_INTERRUPT_ENABLE(old);

    return ret;
}


/* flexspi nor flash read status register */
aw_err_t imx1020_nor_flash_read_sr(uint8_t *p_buf)
{
    RTK_INT_STATUS_DECL(old);
    aw_err_t ret;

    if (__gp_flexspi_serv.p_func == NULL) {
        return -AW_ENXIO;
    }

    if (p_buf == NULL) {
        return -AW_EINVAL;
    }

    RTK_INTERRUPT_DISABLE(old);

    ret = __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_rdsr(
                                                        __gp_flexspi_serv.p_dev,
                                                        p_buf);

    RTK_INTERRUPT_ENABLE(old);

    return ret;
}


/* flexspi nor flash write status register */
aw_err_t imx1020_nor_flash_write_sr(uint8_t *p_buf)
{
    RTK_INT_STATUS_DECL(old);
    aw_err_t ret;

    if (__gp_flexspi_serv.p_func == NULL) {
        return -AW_ENXIO;
    }

    if (p_buf == NULL) {
        return -AW_EINVAL;
    }

    RTK_INTERRUPT_DISABLE(old);

    ret = __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_wrsr(
                                                      __gp_flexspi_serv.p_dev,
                                                                        p_buf);
    RTK_INTERRUPT_ENABLE(old);

    return ret;
}


/* flexspi nor flash read information row */
aw_err_t imx1020_nor_flash_ir_read(uint8_t *p_buf,
                                   uint8_t  row,
                                   uint32_t offset,
                                   uint32_t nbytes)
{
    RTK_INT_STATUS_DECL(old);
    aw_err_t ret;
    uint32_t addr;

    if (__gp_flexspi_serv.p_func == NULL) {
        return -AW_ENXIO;
    }

    if ((p_buf == NULL) || (nbytes == 0) || (row > 3)) {
        return -AW_EINVAL;
    }

    addr = (row * 0x1000) + offset;

    RTK_INTERRUPT_DISABLE(old);

    ret = __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_irrd(
                                                        __gp_flexspi_serv.p_dev,
                                                        addr,
                                                        p_buf,
                                                        nbytes);

    RTK_INTERRUPT_ENABLE(old);

    return ret;
}


/* flexspi nor flash erase information row*/
aw_err_t imx1020_nor_flash_ir_erase(uint8_t row)
{
    RTK_INT_STATUS_DECL(old);
    aw_err_t ret;

    if (__gp_flexspi_serv.p_func == NULL) {
        return -AW_ENXIO;
    }

    if (row > 3) {
        return -AW_EINVAL;
    }

    RTK_INTERRUPT_DISABLE(old);

    ret = __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_irer(
                                                        __gp_flexspi_serv.p_dev,
                                                        row * 0x1000);

    RTK_INTERRUPT_ENABLE(old);

    return ret;
}


/* flexspi nor flash program information row*/
aw_err_t imx1020_nor_flash_ir_program (uint8_t *p_buf,
                                       uint8_t  row,
                                       uint32_t offset,
                                       uint16_t nbytes)
{
    RTK_INT_STATUS_DECL(old);
    uint32_t addr;
    aw_err_t ret;

    if (__gp_flexspi_serv.p_func == NULL) {
        return -AW_ENXIO;
    }

    if ((p_buf == NULL) || (nbytes == 0) || (row > 3)) {
        return -AW_EINVAL;
    }

    addr = (row * 0x1000) + offset;

    RTK_INTERRUPT_DISABLE(old);

    ret = __gp_flexspi_serv.p_func->pfunc_flexspi_nor_flash_irp(
                                                        __gp_flexspi_serv.p_dev,
                                                        addr,
                                                        p_buf,
                                                        nbytes);
    RTK_INTERRUPT_ENABLE(old);

    return ret;
}

/* end of file */
