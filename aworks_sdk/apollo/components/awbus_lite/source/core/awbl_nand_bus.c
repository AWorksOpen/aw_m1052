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
 * \brief NandBus source file
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-16  deo, first implementation
 * - 2.00 17-3-23   vih, modify
 * \endinternal
 */

/*******************************************************************************
    include
*******************************************************************************/
#include "aworks.h"
#include "awbl_nand_bus.h"
#include "aw_assert.h"
#include "aw_vdebug.h"
#include "awbus_lite.h"
#include "aw_sem.h"
#include "aw_list.h"
#include "string.h"

/*******************************************************************************
    function
*******************************************************************************/
aw_local aw_bool_t awbl_nandbus_devmatch (const struct awbl_drvinfo *p_drv,
                                          struct awbl_dev           *p_dev);

/*******************************************************************************
    global variable
*******************************************************************************/
aw_local aw_const struct awbl_bustype_info __g_nand_bustype = {
    AWBL_BUSID_NAND,        /**< \brief bus_id */
    NULL,                   /**< \brief pfunc_bustype_init1 */
    NULL,                   /**< \brief pfunc_bustype_init2 */
    NULL,                   /**< \brief pfunc_bustype_connect */
    NULL,                   /**< \brief pfunc_bustype_newdev_present */
    awbl_nandbus_devmatch   /**< \brief pfunc_bustype_devmatch */
};



/******************************************************************************/
aw_local aw_bool_t awbl_nandbus_devmatch (const struct awbl_drvinfo *p_drv,
                                          struct awbl_dev           *p_dev)
{
    /* check bus type. */
    if (p_dev->p_devhcf->bus_type != AWBL_BUSID_NAND) {
        return AW_FALSE;
    }

    /* check driver name. */
    if (strcmp(p_dev->p_devhcf->p_name, p_drv->p_drvname) != 0) {
        return AW_FALSE;
    }

    return AW_TRUE;
}

/******************************************************************************/
void awbl_nand_bus_init (void)
{
    aw_err_t err;
    err = awbl_bustype_register(&__g_nand_bustype);
    aw_assert(err == AW_OK);
}

/******************************************************************************/
aw_err_t awbl_nandbus_create (struct awbl_nandbus *p_bus,
                              uint8_t              bus_index)
{
    const struct awbl_devhcf    *p_devhcf_list;
    struct awbl_dev             *p_dev;
    size_t                       devhcf_list_count;
    int                          i;
/*    AWBL_NAND_CONTROLLER_INFO_DECL(p_info, &(p_controller->awbus.super)); */

    if (p_bus == NULL) {
        return -AW_EINVAL;
    }

    aw_spinlock_isr_init(&(p_bus->dev_lock), 0);
    AW_MUTEX_INIT(p_bus->dev_mux, AW_SEM_Q_PRIORITY);
/*    p_controller->ecc_support = ecc_support;*/
    p_bus->bus_index = bus_index;

    /* announce bus controller. */
    awbl_bus_announce(&(p_bus->awbus), AWBL_BUSID_NAND);

    p_devhcf_list        = awbl_devhcf_list_get();
    devhcf_list_count    = awbl_devhcf_list_count_get();
    p_dev                = NULL;

    /* loop up all device that on this controller and announce it. */
    for (i = 0; i < devhcf_list_count; i++) {

        /* match bus type and bus ID. */
        if ((p_devhcf_list[i].bus_type  == AWBL_BUSID_NAND) &&
            (p_devhcf_list[i].bus_index == p_bus->bus_index)) {

            p_dev = p_devhcf_list[i].p_dev;
            if (p_dev == NULL) {
                return -AW_ENOMEM;
            }
            p_dev->p_parentbus = ((struct awbl_dev *)p_bus)->p_subbus;
            p_dev->p_devhcf = &p_devhcf_list[i];

            /* announce it. */
            awbl_dev_announce(p_dev);
        }
    }

    return AW_OK;
}


/******************************************************************************/
aw_err_t awbl_nandbus_lock (struct awbl_nandbus *p_bus)
{
    if (p_bus == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_bus->dev_mux, AW_SEM_WAIT_FOREVER);

    aw_spinlock_isr_take(&(p_bus->dev_lock));
    if (p_bus->lock ) {
        aw_spinlock_isr_give(&(p_bus->dev_lock));
        return -AW_EBUSY;
    }
    p_bus->lock = AW_TRUE;
    aw_spinlock_isr_give(&(p_bus->dev_lock));

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_nandbus_unlock (struct awbl_nandbus *p_bus)
{
    if (p_bus == NULL) {
        return -AW_EINVAL;
    }

    aw_spinlock_isr_take(&(p_bus->dev_lock));
    p_bus->lock = AW_FALSE;
    aw_spinlock_isr_give(&(p_bus->dev_lock));

    AW_MUTEX_UNLOCK(p_bus->dev_mux);

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_nandbus_reset (struct awbl_nandbus *p_bus)
{
    if ((p_bus == NULL) || (p_bus->serv.pfn_reset == NULL)) {
        return -AW_EINVAL;
    }

    p_bus->serv.pfn_reset(p_bus);

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_nandbus_select (struct awbl_nandbus   *p_bus,
                              void                  *p_arg)
{
    if ((p_bus->serv.pfn_select == NULL) || (p_bus == NULL)) {
        return -AW_ENODEV;
    }
    p_bus->serv.pfn_select(p_bus, p_arg);
    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_nandbus_opt (struct awbl_nandbus      *p_bus,
                           struct awbl_nandbus_opt  *p_opt)
{
    if ((p_bus->serv.pfn_opt == NULL) || (p_bus == NULL)) {
        return -AW_ENODEV;
    }

    return p_bus->serv.pfn_opt(p_bus, p_opt);
}

/******************************************************************************/
aw_bool_t awbl_nandbus_is_ready (struct awbl_nandbus *p_bus)
{
    if ((p_bus->serv.pfn_ready == NULL) || (p_bus == NULL)) {
        return AW_FALSE;
    }

    return p_bus->serv.pfn_ready(p_bus, NULL);
}

/* end of file */
