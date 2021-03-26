/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief imx1050 nandflash driver source file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-12-18  smc, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "awbl_nand.h"
#include "awbl_nand_bus.h"
#include "awbl_nand_pf.h"

#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_cache.h"
#include "driver/busctlr/awbl_imx1050_semc.h"
#include "driver/nand/awbl_imx1050_nand.h"

#include "awbl_plb.h"
#include "aw_bitops.h"

#include "aw_mem.h"
#include "string.h"
#include "stdio.h"

#include "aw_assert.h"
/******************************************************************************/
#ifdef  __VIH_IDENTITY_DEBUG

#define _ERR_DMSG(msg)  \
        aw_kprintf( "\nNAND MSG: ");\
        aw_kprintf msg; \
        aw_kprintf(".\n-----MSG: at file %s.\n"\
                      "-----MSG: at line %d in %s()\n\n", \
                  __FILE__, \
                  __LINE__, \
                  __FUNCTION__) \

#else
#define _ERR_DMSG(msg)
#endif

#define BUG_HERE                            while(1){}

/******************************************************************************/
/* The default for the length of Galois Field. */
#define __GALOIS_FIELD 13
/******************************************************************************/

extern aw_err_t awbl_nand_read_id (struct awbl_nand_chip *p_nand,
                                   uint8_t               *p_ids,
                                   uint8_t                id_len);

/**
 * \brief OPT结构体配置
 */
aw_local void __nandflash_config_opt (struct awbl_nandbus_opt *p_opt,
                                      uint8_t               type,
                                      aw_bool_t             lock_ce,
                                      const void           *ptr,
                                      uint32_t              len)
{
    p_opt->type     = type;
    p_opt->lock_ce  = lock_ce;
    p_opt->ptr      = ptr;
    p_opt->len      = len;
}

//
///******************************************************************************/
/**
 * \brief NandFlash复位
 */
aw_local aw_err_t __imx1050_nand_reset (awbl_nand_platform_serv_t *p_this)
{
    struct awbl_nandbus     *p_bus = p_this->p_bus;
   awbl_nand_info_t         *p_info = p_this->p_info;
    struct awbl_nandbus_opt  ctrl;
    aw_err_t                 err;
    uint8_t                  temp;

    awbl_nandbus_lock(p_bus);

    /* select chip. */
    err = awbl_nandbus_select(p_bus, (void *)p_info);
    if (err != AW_OK) {
        goto _exit;
    }

    /* command phase. */
    temp = AWBL_NANDFLASH_CMD_RESET;

    __nandflash_config_opt(&ctrl, NAND_CMD, AW_FALSE, &temp, 1);

    err = awbl_nandbus_opt(p_bus, &ctrl);
    if (err != AW_OK) {
        goto _reset_host;
    }

    /* status phase, wait for complete. */
    temp = 0xFF;
    while (!awbl_nandbus_is_ready(p_bus)) {
        temp--;
        if (temp == 0) {
            err = -AW_ETIME;
            goto _reset_host;
        }
    }

    err = AW_OK;
    goto _exit;

_reset_host:
    awbl_nandbus_reset(p_bus);

_exit:
    awbl_nandbus_unlock(p_bus);
    return err;
}


aw_local aw_bool_t __imx1050_nand_drv_probe (awbl_nand_platform_serv_t *p_this)
{
    struct awbl_nandbus     *p_bus  = p_this->p_bus;
    awbl_nand_info_t        *p_info = p_this->p_info;
    int err;
    uint8_t     ids[10];
    int i;

    /* reset the flash */
    err = __imx1050_nand_reset(p_this);
    if (err != AW_OK) {
        AW_ERRF(("nand: reset failed\n"));
        return AW_FALSE;
    }

    err = p_this->read_id(p_this, ids, p_info->id_len);
    if (err != AW_OK) {
        AW_ERRF(("nand: read ID failed\n"));
        return AW_FALSE;
    }

    AW_INFOF(("nand: chip find, id is \"%02xH(maker ID), %02xH(device ID)",
            ids[0], ids[1]));

    for (i = 2; i < p_info->id_len; i++) {
        AW_INFOF((", %02xH", ids[i]));
    }
    AW_INFOF(("\" \n"));

    /* check IDs */
    if ((p_info->attr.maker_id != ids[0]) || (p_info->attr.device_id != ids[1])) {
        AW_ERRF(("nand error: probe failed, expect maker ID %02xH, device ID %02xH\n",
                 p_info->attr.maker_id, p_info->attr.device_id));
        return AW_FALSE;
    }

//    AW_INFOF(("nandflash:find a device. \"%02xH(maker ID), %02xH(device ID)\" \n",
//               p_info->maker_id, p_info->device_id));

    return AW_TRUE;
}

/******************************************************************************/
/* tag init */
awbl_nand_ecc_t *awbl_nand_platform_hw_ecc_create (awbl_nand_ecc_init_t *p_init)
{
    return NULL;
}

awbl_nand_platform_serv_t *awbl_imx1050_nand_platform_serv_create (
        awbl_nand_info_t    *p_info,
        struct awbl_nandbus *p_bus)
{
    awbl_nand_platform_serv_t *p_pf_serv;
    aw_assert(p_bus);

    p_pf_serv = malloc(sizeof(*p_pf_serv));
    aw_assert(p_pf_serv);
    memset(p_pf_serv, 0, sizeof(*p_pf_serv));

    awbl_nand_serv_default_set(p_pf_serv);
    p_pf_serv->reset  = __imx1050_nand_reset;
    p_pf_serv->probe  = __imx1050_nand_drv_probe;
    p_pf_serv->p_info = p_info;
    p_pf_serv->p_bus  = p_bus;
    return p_pf_serv;
}

void awbl_imx1050_nand_platform_serv_destroy (awbl_nand_platform_serv_t *p_this)
{
    aw_assert(p_this);
    free(p_this);
}

void awbl_imx1050_nand_platform_register (void)
{
    (void)awbl_nand_platform_serv_register(awbl_imx1050_nand_platform_serv_create,\
                                           awbl_imx1050_nand_platform_serv_destroy);
}
