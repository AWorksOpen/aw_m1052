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
 * \brief MII BUS
 *
 * \internal
 * \par modification history
 * - 1.00 15-04-01  axn, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_assert.h"
#include "aw_vdebug.h"
#include "aw_sem.h"
#include "aw_list.h"
#include "aw_delay.h"
#include "aw_task.h"
#include "aw_int.h"
#include <string.h> 
#include "aw_vdebug.h"
#include "awbl_miibus.h"

/*******************************************************************************
  import
*******************************************************************************/

AWBL_METHOD_IMPORT(awbl_mii_phy_mode_set);
AWBL_METHOD_IMPORT(awbl_mii_phy_mode_get);
AWBL_METHOD_IMPORT(awbl_mii_phy_notice);
AWBL_METHOD_IMPORT(awbl_mii_phy_up);
AWBL_METHOD_IMPORT(awbl_mii_phy_down);


/*******************************************************************************
  defines
*******************************************************************************/
#define __MII_MONITOR_TASK_STACK_SIZE       4096
#define __MII_MONITOR_TASK_PRO              (aw_task_lowest_priority() - 1)
#define __MII_MONITOR_TASK_DELAY_MS         1000

#define __MII_MST_DEVINFO_GET(p_dev) \
        ((struct awbl_mii_master_devinfo *)AWBL_DEVINFO_GET(p_dev))

#define __MII_MST_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_mii_master_devinfo *p_devinfo =  \
        (struct awbl_mii_master_devinfo *)AWBL_DEVINFO_GET(p_dev)

/*******************************************************************************
  locals
*******************************************************************************/
/** \brief head of MII masters list */
aw_local struct awbl_mii_master *__gp_mii_master_head = NULL;

/** \brief phy monitor task */
AW_TASK_DECL_STATIC(__g_mii_monitor_task, __MII_MONITOR_TASK_STACK_SIZE);


aw_local void __mii_master_insert (struct awbl_mii_master **pp_head,
                                       struct awbl_mii_master  *p_master);

aw_local void __miibus_monitor_start (void);

aw_local aw_err_t __miibus_devnum (struct awbl_mii_master *p_master);

aw_bool_t awbl_miibus_devmatch (const struct awbl_drvinfo *p_drv,
                                struct awbl_dev           *p_dev);


/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __miibus_inst_init1(awbl_dev_t *p_dev)
{
    return;
}


/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __miibus_inst_init2(awbl_dev_t *p_dev)
{
    /* announce as bus */
    awbl_bus_announce((struct awbl_busctlr *)p_dev, AWBL_BUSID_MII);

    /* add this master to list */
    __mii_master_insert(&__gp_mii_master_head, (struct awbl_mii_master *)p_dev);

    /* enumerate the mii device */
    __miibus_devnum((struct awbl_mii_master *)p_dev);
}


/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __miibus_inst_connect(awbl_dev_t *p_dev)
{

}


/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_mii_bus_drvfuncs = {
    __miibus_inst_init1,
    __miibus_inst_init2,
    __miibus_inst_connect
};


/* driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_mii_bus_drv_registration = {

    AWBL_VER_1,                              /* awb_ver */
    AWBL_BUSID_PLB,                          /* bus_id */
    AWBL_MII_BUS_NAME,                       /* p_drvname */
    &__g_mii_bus_drvfuncs,                   /* p_busfuncs */
    NULL,                                    /* p_methods */
    NULL                                     /* pfunc_drv_probe */
};


/* MII总线注册信息 */
aw_local aw_const struct awbl_bustype_info __g_mii_bustype = {
    AWBL_BUSID_MII,         /* bus_id */
    NULL,                   /* pfunc_bustype_init1 */
    NULL,                   /* pfunc_bustype_init2 */
    NULL,                   /* pfunc_bustype_connect */
    NULL,                   /* pfunc_bustype_newdev_present */
    awbl_miibus_devmatch    /* pfunc_bustype_devmatch */
};


/**
 * \brief add an mii master to master list
 */
aw_local void __mii_master_insert (struct awbl_mii_master **pp_head,
                                   struct awbl_mii_master  *p_master)
{
    while (*pp_head != NULL) {
        pp_head = &(*pp_head)->p_next;
    }

    *pp_head = p_master;

     p_master->p_next = NULL;
}


aw_bool_t awbl_miibus_devmatch (const struct awbl_drvinfo *p_drv,
                             struct awbl_dev           *p_dev)
{
    /* 检查总线类型 */
    if (p_dev->p_devhcf->bus_type != AWBL_BUSID_MII) {
        return AW_FALSE;
    }

    /* 检查驱动名 */
    if (strcmp(p_dev->p_devhcf->p_name, p_drv->p_drvname) != 0) {
        return AW_FALSE;
    }

    return AW_TRUE;
}


/**
 * \brief 以太网速度和双工模式配置函数
 */
aw_err_t aw_eth_mode_set (char *p_name, uint32_t mode)
{
    uint32_t awbl_mode;
    struct awbl_mii_master *p_mii_master = __gp_mii_master_head;
    struct awbl_phy_device *p_phy_dev;

    if (p_name == NULL) {
        return -AW_EINVAL;          /* 无效参数 */
    }

    switch (mode)
    {
    case AW_ETH_MODE_AUTONEG:
        awbl_mode = AWBL_PHY_LINK_AUTONEG;
        break;
    case AW_ETH_MODE_1000M_FULL:
        awbl_mode = AWBL_PHY_LINK_1000M_FULL;
        break;
    case AW_ETH_MODE_1000M_HALF:
        awbl_mode = AWBL_PHY_LINK_1000M_HALF;
        break;
    case AW_ETH_MODE_100M_FULL:
        awbl_mode = AWBL_PHY_LINK_100M_FULL;
        break;
    case AW_ETH_MODE_100M_HALF:
        awbl_mode = AWBL_PHY_LINK_100M_HALF;
        break;
    case AW_ETH_MODE_10M_FULL:
        awbl_mode = AWBL_PHY_LINK_10M_FULL;
        break;
    case AW_ETH_MODE_10M_HALF:
        awbl_mode = AWBL_PHY_LINK_10M_HALF;
        break;
    default :
        return -AW_EINVAL;          /* 无效参数 */
    }


    if (p_mii_master) {
        p_phy_dev = p_mii_master->p_phy_dev_list_head;
        while (p_phy_dev) {
            if (0 == strcmp(p_phy_dev->p_ethif_name, p_name)) {
                break;
            }
            p_phy_dev = p_phy_dev->p_next;
        }
    }

    if (p_phy_dev) {
        awbl_miibus_mode_set(p_phy_dev, awbl_mode);
    } else {
        return -AW_ENODEV;        /* 未找到设备 */
    }

    return AW_OK;

}


/******************************************************************************/
void awbl_miibus_init (void)
{
    aw_err_t err;

    /* register MII BUS as a valid bus type */
    err = awbl_bustype_register(&__g_mii_bustype);
    aw_assert(err == AW_OK);

    err = awbl_drv_register((struct awbl_drvinfo *)&__g_mii_bus_drv_registration);
    aw_assert(err == AW_OK);
}


/******************************************************************************/
aw_local aw_err_t __miibus_devnum (struct awbl_mii_master *p_master)
{
    int                 i;
    struct awbl_dev    *p_dev               = NULL;
    const struct awbl_devhcf *p_devhcf_list = awbl_devhcf_list_get();
    size_t              devhcf_list_count   = awbl_devhcf_list_count_get();

    __MII_MST_DEVINFO_DECL(p_this_info, p_master);

    /* search for the devices on MII bus from HCF list */

    for (i = 0; i < devhcf_list_count; i++) {

        /* the device is on this MII bus */
        if ((p_devhcf_list[i].bus_type  == AWBL_BUSID_MII) &&
            (p_devhcf_list[i].bus_index == p_this_info->bus_index)) {

            p_dev = p_devhcf_list[i].p_dev;
            if (p_dev == NULL) {
                return -AW_ENOMEM;     /* the device has no instance memeory */
            }

            /* dev bus handle is controller's subordinate bus */
            p_dev->p_parentbus = ((struct awbl_dev *)p_master)->p_subbus;

            /* save device's HCF information */
            p_dev->p_devhcf = &p_devhcf_list[i];

            /* we now have the device, so let the bus subsystem know */
            awbl_dev_announce(p_dev);
        }
    }

    return AW_OK;
}


/**
 * \brief  创建MII总线控制器
 */
aw_err_t awbl_miibus_create(struct awbl_dev        *p_dev,
                            struct awbl_mii_master *p_master,
                            uint8_t                 bus_index,
                            aw_const awbl_mii_master_ops_t  *p_mii_ops)
{
    aw_err_t err;

    /* check parameters */
    if (p_master == NULL) {
        return -AW_EFAULT;
    }

    p_master->p_mii_parent = p_dev;

    /* dev bus handle is controller's subordinate bus */
    p_master->super.super.p_parentbus = p_dev->p_parentbus;

    p_master->mii_dev_info.bus_index = bus_index;

    p_master->mii_devhcf.p_name    = AWBL_MII_BUS_NAME;
    p_master->mii_devhcf.unit      = p_dev->p_devhcf->unit;     //bus_index;
    p_master->mii_devhcf.bus_type  = p_dev->p_devhcf->bus_type; //AWBL_BUSID_PLB;
    p_master->mii_devhcf.bus_index = bus_index;
    p_master->mii_devhcf.p_dev     = (awbl_dev_t *)p_master;
    p_master->mii_devhcf.p_devinfo = &p_master->mii_dev_info;

    p_master->p_mii_master_ops = p_mii_ops;

    /* save device's HCF information */
    /* 保存设备信息什么情况下有可能用呢？ */
    p_master->super.super.p_devhcf = &p_master->mii_devhcf;

    err = awbl_dev_announce ((struct awbl_dev *)(p_master));
    if (err != AW_OK) {
        return err;
    }

    AW_MUTEX_INIT(p_master->dev_miibus_mutex, AW_SEM_Q_PRIORITY);

    return AW_OK;
}


/**
 * \brief  读MII bus上PHY的寄存器
 */
aw_err_t awbl_miibus_read (struct awbl_phy_device  *p_phy_dev,
                           int                      phy_reg,
                           uint16_t                *p_reg_val)
{
    struct awbl_mii_master  *p_master       = NULL;
    int                      ret            = AW_OK;

    if (NULL == p_phy_dev) {
        return -AW_EINVAL;
    }

    /* 找到phy所挂载的master */
    p_master = (struct awbl_mii_master *)awbl_dev_parent(&p_phy_dev->phy_dev);

    if ((p_master == NULL) ||
        (p_master->p_mii_master_ops == NULL) ||
        (p_master->p_mii_master_ops->pfn_mii_phy_read == NULL)) {
        return -AW_ENODEV;
    }

    AW_MUTEX_LOCK(p_master->dev_miibus_mutex, AW_SEM_WAIT_FOREVER);

    ret = p_master->p_mii_master_ops->pfn_mii_phy_read (p_master,
                                                        p_phy_dev->phy_addr,
                                                        phy_reg,
                                                        p_reg_val);
    AW_MUTEX_UNLOCK(p_master->dev_miibus_mutex);
    return ret;
}


/**
 * \brief  写MII bus上PHY的寄存器
 */
aw_err_t awbl_miibus_write(struct awbl_phy_device  *p_phy_dev,
                           int                      phy_reg,
                           uint16_t                 reg_val)
{
    struct awbl_mii_master  *p_master        = NULL;
    int                      ret             = AW_OK;

    if (NULL == p_phy_dev ) {
        return -AW_EINVAL;
    }

    /* 找到phy所挂载的master */
    p_master = (struct awbl_mii_master *)awbl_dev_parent(&p_phy_dev->phy_dev);

    if ((p_master == NULL) ||
        (p_master->p_mii_master_ops == NULL) ||
        (p_master->p_mii_master_ops->pfn_mii_phy_write == NULL)) {
        return -AW_ENODEV;
    }

    AW_MUTEX_LOCK(p_master->dev_miibus_mutex, AW_SEM_WAIT_FOREVER);
    ret = p_master->p_mii_master_ops->pfn_mii_phy_write(p_master,
                                                        p_phy_dev->phy_addr,
                                                        phy_reg,
                                                        reg_val);
    AW_MUTEX_UNLOCK(p_master->dev_miibus_mutex);

    return ret;
}

/**
 * \brief get MII bus master by MII bus ID
 */
struct awbl_mii_master *awbl_miibus_get_master_by_id (uint8_t mii_bus_id)
{
    struct awbl_mii_master *p_master_cur = __gp_mii_master_head;

    if (NULL == __gp_mii_master_head) {
        return NULL;
    }

    AW_MUTEX_LOCK(p_master_cur->dev_miibus_mutex, AW_SEM_WAIT_FOREVER);
    while (p_master_cur != NULL) {

        if (mii_bus_id == p_master_cur->mii_dev_info.bus_index) {

            AW_MUTEX_UNLOCK(p_master_cur->dev_miibus_mutex);
            return p_master_cur;
        }

        p_master_cur = p_master_cur->p_next;
    }

    AW_MUTEX_UNLOCK(p_master_cur->dev_miibus_mutex);


    return NULL;
}


/**
 * \brief add a phy to MII bus
 */
aw_err_t awbl_miibus_add_phy(struct awbl_mii_master  *p_master,
                             struct awbl_phy_device  *p_phy_dev)
{
    struct awbl_phy_device **pp_head  = NULL;

    if (NULL == p_master || NULL == p_phy_dev) {
        return -AW_ENODEV;
    }

    AW_MUTEX_LOCK(p_master->dev_miibus_mutex, AW_SEM_WAIT_FOREVER);

    pp_head = &p_master->p_phy_dev_list_head;

    while (*pp_head != NULL) {
        pp_head = &(*pp_head)->p_next;
    }

    *pp_head = p_phy_dev;

    p_phy_dev->p_next = NULL;

    /* 启动查询PHY任务  */
    __miibus_monitor_start();

    AW_MUTEX_UNLOCK(p_master->dev_miibus_mutex);

    return AW_OK;
}


/**
 * \brief get PHY info by MII bus ID
 */
aw_err_t awbl_miibus_info_get (struct awbl_phy_device *p_phy_dev,
                               uint32_t               *p_mode,
                               uint32_t               *p_stat)
{
    struct awbl_mii_master *p_master  = NULL;

    if (p_phy_dev == NULL) {
        return -AW_EINVAL;
    }

    p_master = (struct awbl_mii_master *)awbl_dev_parent(&p_phy_dev->phy_dev);

    if (NULL == p_master) {
        return -AW_ENODEV;
    }

    AW_MUTEX_LOCK(p_master->dev_miibus_mutex, AW_SEM_WAIT_FOREVER);

    *p_mode = p_phy_dev->phy_mode;
    *p_stat = p_phy_dev->phy_state;

    AW_MUTEX_UNLOCK(p_master->dev_miibus_mutex);

    return AW_OK;
}


/**
 * \brief get PHY mode
 */
aw_err_t awbl_miibus_mode_get (struct awbl_phy_device *p_phy_dev,
                               uint32_t               *p_mode,
                               uint32_t               *p_sts)
{
    awbl_miibus_mode_get_t  mii_phy_mode_get = NULL;
    struct awbl_mii_master *p_master         = NULL;
    int                     ret              = AW_OK;

    if (p_phy_dev == NULL) {
        return -AW_EINVAL;
    }

    /*
     * 获取出phy所在的master
     */
    p_master = (struct awbl_mii_master *)awbl_dev_parent(&p_phy_dev->phy_dev);

    if (NULL == p_master) {
        return -AW_ENODEV;
    }

    /* 从PHY驱动设备中查找awbl_mii_phy_mode_get的方法 */
    mii_phy_mode_get = (awbl_miibus_mode_get_t)awbl_dev_method_get(
                                       (struct awbl_dev *)p_phy_dev,
                                       AWBL_METHOD_CALL(awbl_mii_phy_mode_get));

    if (mii_phy_mode_get == NULL) {
        return -AW_ENODEV;
    }

    AW_MUTEX_LOCK(p_master->dev_miibus_mutex, AW_SEM_WAIT_FOREVER);
    ret = mii_phy_mode_get(p_phy_dev, p_mode, p_sts);
    AW_MUTEX_UNLOCK(p_master->dev_miibus_mutex);

    return ret;
}


/**
 * \brief set PHY mode
 */
aw_err_t awbl_miibus_mode_set (struct awbl_phy_device  *p_phy_dev,
                               uint32_t                 mode)
{
    awbl_miibus_mode_set_t  mii_phy_mode_set = NULL;
    struct awbl_mii_master *p_master         = NULL;
    int                     ret              = AW_OK;

    if (NULL == p_phy_dev) {
        return -AW_EINVAL;
    }

    p_master = (struct awbl_mii_master *)awbl_dev_parent(&p_phy_dev->phy_dev);

    if (NULL == p_master) {
        return -AW_ENODEV;
    }

    mii_phy_mode_set = (awbl_miibus_mode_set_t)awbl_dev_method_get(
                                      (struct awbl_dev *)p_phy_dev,
                                      AWBL_METHOD_CALL(awbl_mii_phy_mode_set));

    if (mii_phy_mode_set == NULL) {
        return -AW_ENODEV;
    }

    AW_MUTEX_LOCK(p_master->dev_miibus_mutex, AW_SEM_WAIT_FOREVER);
    ret = mii_phy_mode_set(p_phy_dev, mode);
    AW_MUTEX_UNLOCK(p_master->dev_miibus_mutex);

    return ret;
}


/**
 * \brief  PHY UP
 */
aw_err_t awbl_miibus_phy_up (struct awbl_phy_device *p_phy_dev)
{
    awbl_miibus_phy_up_t  awbl_miibus_phy_up = NULL;
    struct awbl_mii_master *p_master         = NULL;
    int                     ret              = AW_OK;

    if (p_phy_dev == NULL) {
        return -AW_EINVAL;
    }

    /*
     * 获取出phy所在的master
     */
    p_master = (struct awbl_mii_master *)awbl_dev_parent(&p_phy_dev->phy_dev);

    if (NULL == p_master) {
        return -AW_ENODEV;
    }

    /* 从PHY驱动设备中查找awbl_mii_phy_up的方法 */
    awbl_miibus_phy_up = (awbl_miibus_phy_up_t)awbl_dev_method_get(
                                       (struct awbl_dev *)p_phy_dev,
                                       AWBL_METHOD_CALL(awbl_mii_phy_up));

    if (awbl_miibus_phy_up == NULL) {
        return -AW_ENODEV;
    }

    AW_MUTEX_LOCK(p_master->dev_miibus_mutex, AW_SEM_WAIT_FOREVER);
    ret = awbl_miibus_phy_up(p_phy_dev);
    AW_MUTEX_UNLOCK(p_master->dev_miibus_mutex);

    return ret;
}


/**
 * \brief  PHY DOWN
 */
aw_err_t awbl_miibus_phy_down (struct awbl_phy_device *p_phy_dev)
{
    awbl_miibus_phy_down_t  awbl_miibus_phy_down = NULL;
    struct awbl_mii_master *p_master         = NULL;
    int                     ret              = AW_OK;

    if (p_phy_dev == NULL) {
        return -AW_EINVAL;
    }

    /*
     * 获取出phy所在的master
     */
    p_master = (struct awbl_mii_master *)awbl_dev_parent(&p_phy_dev->phy_dev);

    if (NULL == p_master) {
        return -AW_ENODEV;
    }

    /* 从PHY驱动设备中查找awbl_mii_phy_down的方法 */
    awbl_miibus_phy_down = (awbl_miibus_phy_down_t)awbl_dev_method_get(
                                       (struct awbl_dev *)p_phy_dev,
                                       AWBL_METHOD_CALL(awbl_mii_phy_down));

    if (awbl_miibus_phy_down == NULL) {
        return -AW_ENODEV;
    }

    AW_MUTEX_LOCK(p_master->dev_miibus_mutex, AW_SEM_WAIT_FOREVER);
    ret = awbl_miibus_phy_down(p_phy_dev);
    AW_MUTEX_UNLOCK(p_master->dev_miibus_mutex);

    return ret;
}


/**
 * \brief notice PHY
 */
aw_err_t awbl_miibus_notice (struct awbl_phy_device *p_phy_dev,
                             uint32_t                mode,
                             uint32_t                stat)
{
    awbl_miibus_notice_t    mii_notice       = NULL;
    struct awbl_mii_master *p_master         = NULL;
    int                     ret              = AW_OK;

    if (NULL == p_phy_dev) {
        return -AW_EINVAL;
    }

    p_master = (struct awbl_mii_master *)awbl_dev_parent(&p_phy_dev->phy_dev);

    if (NULL == p_master) {
        return -AW_ENODEV;
    }

    /* 从phy驱动中发现一个方法 */
    mii_notice = (awbl_miibus_notice_t)awbl_dev_method_get(
                                    (struct awbl_dev *)p_phy_dev,
                                    AWBL_METHOD_CALL(awbl_mii_phy_notice));

    if (mii_notice == NULL) {
        return -AW_ENODEV;
    }

    AW_MUTEX_LOCK(p_master->dev_miibus_mutex, AW_SEM_WAIT_FOREVER);
    ret = mii_notice(p_phy_dev, mode, stat);
    AW_MUTEX_UNLOCK(p_master->dev_miibus_mutex);
    return ret;
}


aw_err_t awbl_miibus_update (struct awbl_phy_device *p_phy_dev)
{
    struct awbl_mii_master  *p_master    = NULL;
    uint32_t                 mode        = 0, stat = 0;
    int                      ret         = AW_OK;

    if ((NULL == p_phy_dev) ||
        (NULL == p_phy_dev->phy_attach_dev.pfn_update_link)) {
        return -AW_EINVAL;
    }

    p_master = (struct awbl_mii_master *)awbl_dev_parent(&p_phy_dev->phy_dev);

    if (p_master == NULL) {
        return -AW_ENODEV;
    }

    AW_MUTEX_LOCK(p_master->dev_miibus_mutex, AW_SEM_WAIT_FOREVER);
    awbl_miibus_info_get(p_phy_dev, &mode, &stat);

    p_phy_dev->phy_attach_dev.pfn_update_link(
            p_phy_dev->phy_attach_dev.p_enet_dev, mode, stat);

    AW_MUTEX_UNLOCK(p_master->dev_miibus_mutex);

    return ret;
}


/**
 * Convenience function to print out the current phy status
 * @pdev: the phy_device struct
 */
aw_local void __phy_print_status(struct awbl_phy_device *p_phy_dev)
{
    uint16_t                speed    = 0;

    AW_INFOF(("PHY %d:%d - Link is %s",
              AWBL_DEV_UNIT_GET(p_phy_dev),
              p_phy_dev->phy_addr,
             (p_phy_dev->phy_state & AWBL_IFM_ACTIVE) ? "Up" : "Down"));

    if (p_phy_dev->phy_state & AWBL_IFM_ACTIVE) {

        switch (AWBL_IFM_SUBTYPE(p_phy_dev->phy_mode)) {

        case AWBL_IFM_1000_T: speed = 1000; break;
        case AWBL_IFM_100_TX: speed = 100;  break;
        case AWBL_IFM_10_T:   speed = 10;   break;
        default : break;
        }

        AW_INFOF((" - %d/%s", speed,
                ((p_phy_dev->phy_mode & AWBL_IFM_GMASK) == AWBL_IFM_FDX) ?
                  "Full" : "Half"));
    }
    AW_INFOF(("\n"));
}


/**
 * \brief PHY监视任务，将会遍历所有的MII bus和总线上的所有PHY，根据状态上报信息
 *
 * \note 以查询方式,查询当前系统中所有的PHY的状态
 */
aw_local void __miibus_monitor_task (void *parg)
{
    struct awbl_mii_master *p_master_cur = __gp_mii_master_head;
    struct awbl_phy_device *p_phy_cur    = NULL;

    uint32_t phy_mode, phy_stat;
    uint32_t old_phy_mode, old_phy_stat;

    AW_FOREVER {

        p_master_cur = __gp_mii_master_head;

        /**
         * iterate the MII bus list
         */
        while (p_master_cur != NULL) {
            p_phy_cur = p_master_cur->p_phy_dev_list_head;
            while( NULL != p_phy_cur) {

                /* 
                 * 这个锁的目的是防止获取出模式后,
                 * 立即又有进程将PHY的模式给修改了，
                 * 最终导致网卡的模式和PHY的模式不匹配
                 */
                AW_MUTEX_LOCK(p_master_cur->dev_miibus_mutex, AW_SEM_WAIT_FOREVER);
                if (AW_OK == awbl_miibus_mode_get(p_phy_cur,
                                                  &phy_mode,
                                                  &phy_stat)) {
                    uint8_t update = 0;

                    /* Link state change */
                    if (phy_stat != p_phy_cur->phy_state) {

                        if ((!(phy_stat & AWBL_IFM_ACTIVE)) || /* Link is down */
                            ((phy_stat & AWBL_IFM_ACTIVE) &&
                            (AWBL_IFM_SUBTYPE(phy_mode) != AWBL_IFM_NONE))) {
                            /* Link is up */
                            update = 1;
                        }
                    } else if (phy_mode != p_phy_cur->phy_mode) {

                        /* Link mode change */
                        if (AWBL_IFM_SUBTYPE(phy_mode) != AWBL_IFM_NONE) {

                            update = 1;
                        }
                    }

                    if (update) {
                        old_phy_mode = phy_mode;
                        old_phy_stat = phy_stat;

                        /* p_phy_cur->phy_mode和phy_state是随时都可能变化的  */
                        p_phy_cur->phy_mode  = phy_mode;
                        p_phy_cur->phy_state = phy_stat;

                        if (AW_OK == awbl_miibus_update(p_phy_cur)) {
                            awbl_miibus_notice (p_phy_cur, phy_mode, phy_stat);
                            __phy_print_status(p_phy_cur);
                        } else {
                            p_phy_cur->phy_mode = old_phy_mode;
                            p_phy_cur->phy_state = old_phy_stat;

                        }
                    }
                }

                AW_MUTEX_UNLOCK(p_master_cur->dev_miibus_mutex);
                p_phy_cur = p_phy_cur->p_next;
            }
            p_master_cur = p_master_cur->p_next;
        }
        aw_mdelay(__MII_MONITOR_TASK_DELAY_MS);
    }
}


/**
 * \brief MII bus monitor task start, 
 *        this will iterate all the MII bus on the system.
 */
aw_local void __miibus_monitor_start (void)
{
    /*
     * ensure this task will create only once
     */
    if (!AW_TASK_VALID(__g_mii_monitor_task)) {
        /* create mii bus monitor task */
        AW_TASK_INIT(__g_mii_monitor_task,
                    "mii bus monitor",
                    __MII_MONITOR_TASK_PRO,
                    __MII_MONITOR_TASK_STACK_SIZE,
                    __miibus_monitor_task,
                    (void*)0);

        AW_TASK_STARTUP(__g_mii_monitor_task);
    }
}

/* end of file */

