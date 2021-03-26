/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief ZSN603 UART 模式驱动
 *
 * \internal
 * \par modification history:
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "aw_common.h"
#include "aw_int.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_gpio.h"
#include "aw_serial.h"
#include "awbus_lite.h"
#include "aw_sio_common.h"
#include "driver/rfid/awbl_zsn603_uart.h"
#include "driver/rfid/zsn603.h"
#include "driver/rfid/zsn603_platform.h"
/******************************************************************************
 forward declarations
 ******************************************************************************/
aw_local void __zsn603_inst_init1 (awbl_dev_t *p_dev);
aw_local void __zsn603_inst_init2 (awbl_dev_t *p_dev);
aw_local void __zsn603_inst_connect (awbl_dev_t *p_dev);

/******************************************************************************
 locals
 ******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_zsn603_drvfuncs = {
    __zsn603_inst_init1,
    __zsn603_inst_init2,
    __zsn603_inst_connect
};

/* driver methods (must defined as aw_const) */
aw_local aw_const struct awbl_dev_method __g_zsn603_dev_methods[] = {
    AWBL_METHOD_END
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_zsn603_drv_registration = {
    {
        AWBL_VER_1,                 /* awb_ver */
        AWBL_BUSID_PLB,             /* bus_id */
        AWBL_ZSN603_UART_NAME,           /* p_drvname */
       &__g_zsn603_drvfuncs,       /* p_busfuncs */
       &__g_zsn603_dev_methods[0], /* p_methods */
        NULL                        /* pfunc_drv_probe */
    }
};


/**
 * \brief register zsn603 driver
 *
 * This routine registers the zsn603 driver and device recognition
 * data with the AWBus subsystem.
 *
 * NOTE: This routine is called early during system initialization, and
 * *MUST NOT* make calls to OS facilities such as memory allocation
 * and I/O.
 */
void awbl_zsn603_uart_drv_register (void)
{
   awbl_drv_register((struct awbl_drvinfo *)&__g_zsn603_drv_registration);
}

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __zsn603_inst_init1 (awbl_dev_t *p_dev)
{
    return;
}
/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */

aw_local void __zsn603_inst_init2 (awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __zsn603_inst_connect (awbl_dev_t *p_dev)
{
    awbl_zsn603_uart_dev_t     *p_this = (awbl_zsn603_uart_dev_t *)p_dev;
    awbl_zsn603_uart_devinfo_t *p_info = awbldev_get_zsn603_uart_info(p_dev);

    zsn603_uart_dev_t              *p_drv       = &p_this->dev;
    zsn603_uart_devinfo_t          *p_dev_info  = &p_this->info;
    zsn603_platform_uart_devinfo_t *p_plat_info = &p_dev_info->platform_info;

    p_plat_info->baud_rate  = p_info->baud_rate;
    p_plat_info->uart_com   = p_info->uart_com;
    p_plat_info->int_pin    = p_info->int_pin;
    p_this->info.local_addr = p_info->local_address;

    zsn603_uart_init(p_drv, (const zsn603_uart_devinfo_t *)p_dev_info);

    return;
}

/**
 * \brief get zsn603 device handle
 *
 * \param[in] unit   :  device index
 */
zsn603_handle_t  awbl_zsn603_uart_handle_get(int unit)
{
    awbl_dev_t *p_dev = awbl_dev_find_by_name(AWBL_ZSN603_UART_NAME, unit);
    awbl_zsn603_uart_dev_t *p_zsn603_dev= (awbl_zsn603_uart_dev_t *)p_dev;
    if(p_zsn603_dev != NULL){
        return  &(p_zsn603_dev->dev.zsn603_dev);
    }else{
        return NULL;
    }
}


/* end of file */
