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
 * \brief EimBus source file
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-17  deo, first implementation
 * \endinternal
 */

/*******************************************************************************
    include
*******************************************************************************/
#include "awbl_eim_bus.h"
#include "apollo.h"
#include "aw_assert.h"
#include "aw_vdebug.h"
#include "awbus_lite.h"
#include "aw_sem.h"
#include "aw_list.h"
#include "string.h"

/*******************************************************************************
  defines
*******************************************************************************/
#define __EIM_MST_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_eim_master_devinfo *p_devinfo = \
        (struct awbl_eim_master_devinfo *)AWBL_DEVINFO_GET(p_dev)

/*******************************************************************************
  function
*******************************************************************************/
aw_local aw_bool_t awbl_eimbus_devmatch (const struct awbl_drvinfo *p_drv,
                                         struct awbl_dev           *p_dev);

/*******************************************************************************
  global variable
*******************************************************************************/
aw_local aw_const struct awbl_bustype_info __g_eim_bustype = {
    AWBL_BUSID_EIM,         /* bus_id */
    NULL,                   /* pfunc_bustype_init1 */
    NULL,                   /* pfunc_bustype_init2 */
    NULL,                   /* pfunc_bustype_connect */
    NULL,                   /* pfunc_bustype_newdev_present */
    awbl_eimbus_devmatch    /* pfunc_bustype_devmatch */
};



/******************************************************************************/
aw_local aw_bool_t awbl_eimbus_devmatch (const struct awbl_drvinfo *p_drv,
                                         struct awbl_dev           *p_dev)
{
    /* check bus type. */
    if (p_dev->p_devhcf->bus_type != AWBL_BUSID_EIM) {
        return AW_FALSE;
    }

    /* check driver name. */
    if (strcmp(p_dev->p_devhcf->p_name, p_drv->p_drvname) != 0) {
        return AW_FALSE;
    }

    return AW_TRUE;
}

/******************************************************************************/
void awbl_eim_bus_init(void)
{
    aw_err_t err;
    err = awbl_bustype_register(&__g_eim_bustype);
    aw_assert(err == AW_OK);
}

/******************************************************************************/
aw_err_t awbl_eimbus_create (struct awbl_eim_master *p_master)
{
    const struct awbl_devhcf    *p_devhcf_list;
    struct awbl_dev             *p_dev;
    size_t                       devhcf_list_count;
    int                          i;

    __EIM_MST_DEVINFO_DECL(p_mdinfo, p_master);

    if (p_master == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_INIT(p_master->dev_mux, AW_SEM_Q_PRIORITY);

    /* announce bus controller. */
    awbl_bus_announce(&(p_master->super), AWBL_BUSID_EIM);

    p_devhcf_list        = awbl_devhcf_list_get();
    devhcf_list_count    = awbl_devhcf_list_count_get();
    p_dev                = NULL;

    /* loop up all device that on this controller and announce it. */
    for (i = 0; i < devhcf_list_count; i++) {

        /* match bus type and bus ID. */
        if ((p_devhcf_list[i].bus_type  == AWBL_BUSID_EIM) &&
            (p_devhcf_list[i].bus_index == p_mdinfo->bus_index)) {

            p_dev = p_devhcf_list[i].p_dev;
            if (p_dev == NULL) {
                return -AW_ENOMEM;
            }
            p_dev->p_parentbus = ((struct awbl_dev *)p_master)->p_subbus;
            p_dev->p_devhcf = &p_devhcf_list[i];

            /* announce it. */
            awbl_dev_announce(p_dev);
        }
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_eimbus_timing (struct awbl_eim_device *p_dev,
                             uint8_t                 cs_select,
                             void                   *p_info)
{
    struct awbl_eim_master *p_master = NULL;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    p_master = AWBL_EIM_PARENT_GET(p_dev);
    if ((p_master->p_devinfo2->pfn_eim_timing == NULL) || (p_master == NULL)) {
        return -AW_ENODEV;
    }

    p_master->p_devinfo2->pfn_eim_timing(p_master, cs_select, p_info);

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_eimbus_config (struct awbl_eim_device *p_dev,
                             uint8_t                 cs_select,
                             void                   *p_info)
{
    struct awbl_eim_master *p_master = NULL;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    p_master = AWBL_EIM_PARENT_GET(p_dev);
    if ((p_master->p_devinfo2->pfn_eim_config == NULL) || (p_master == NULL)) {
        return -AW_ENODEV;
    }

    p_master->p_devinfo2->pfn_eim_config(p_master, cs_select, p_info);

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_eim_write (struct awbl_eim_device *p_dev,
                         uint8_t                 cs_select,
                         uint32_t                addr,
                         uint16_t                data)
{
    struct awbl_eim_master *p_master = NULL;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    p_master = AWBL_EIM_PARENT_GET(p_dev);
    if ((p_master->p_devinfo2->pfn_eim_write == NULL) || (p_master == NULL)) {
        return -AW_ENODEV;
    }

    p_master->p_devinfo2->pfn_eim_write(p_master, cs_select, addr, data);

    return AW_OK;
}

/******************************************************************************/
uint8_t awbl_eim_read (struct awbl_eim_device *p_dev,
                       uint8_t                 cs_select,
                       uint32_t                addr)
{
    struct awbl_eim_master *p_master = NULL;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    p_master = AWBL_EIM_PARENT_GET(p_dev);
    if ((p_master->p_devinfo2->pfn_eim_read == NULL) || (p_master == NULL)) {
        return -AW_ENODEV;
    }

    p_master->p_devinfo2->pfn_eim_read(p_master, cs_select, addr);

    return AW_OK;
}

/* end of file */
