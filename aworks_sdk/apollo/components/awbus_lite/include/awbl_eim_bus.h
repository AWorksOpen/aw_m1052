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
 * \brief EimBus head file
 *
 * define EimBus data structure and functions
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-17  deo, first implementation
 * \endinternal
 */
#ifndef __AWBL_EIM_BUS_H
#define __AWBL_EIM_BUS_H


#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus */

#include "aw_list.h"
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "string.h"

/** \biref 得到EIM从机设备的父控制器 */
#define AWBL_EIM_PARENT_GET(p_dev) \
    ((struct awbl_eim_master *) \
        (((struct awbl_dev *)(p_dev))->p_parentbus->p_ctlr))

struct awbl_eim_master;
struct awbl_eimbus_opt;

/** \biref AWBus EIM 总线控制器 (EIM 主机) 设备信息 (平台配置部分) */
struct awbl_eim_master_devinfo {

    /** \brief 控制器所对应的总线编号 */
    uint8_t     bus_index;
};

/** \biref AWBus EIM 总线控制器 (EIM 主机) 设备信息 (驱动配置部分) */
struct awbl_eim_master_devinfo2 {
    /** \brief 设置配置 */
    void (*pfn_eim_config) (struct awbl_eim_master *p_dev,
                            uint8_t                 cs_select,
                            void                   *p_info);

    /** \brief 设置时序 */
    void (*pfn_eim_timing)  (struct awbl_eim_master *p_master,
                             uint8_t                 cs_select,
                             void                   *p_info);

    /** \brief 写操作 */
    void  (*pfn_eim_write)  (struct awbl_eim_master   *p_master,
                             uint8_t                   cs_select,
                             uint32_t                  addr,
                             uint16_t                  data);

     /** \brief 读操作 */
    uint16_t  (*pfn_eim_read)  (struct awbl_eim_master   *p_master,
                                uint8_t                   cs_select,
                                uint32_t                  addr);
};

/** \brief AWBus EIM 总线控制器 (EIM主机) 实例 */
struct awbl_eim_master {

    /** \brief 总线控制器 */
    struct awbl_busctlr          super;     /**< \brief 继承自 AWBus 总线控制器 */

    /** \brief EIM 控制器相关信息 (驱动配置部分) */
    const struct awbl_eim_master_devinfo2 *p_devinfo2;

    /** \brief 设备互斥锁 */
    AW_MUTEX_DECL(               dev_mux);

    /** \brief 当前控制器是否上锁 */
    aw_bool_t                    lock;

    void                        *priv;
};

/* \biref EIM bus 从机设备驱动注册信息结构体 */
typedef struct awbl_eim_drvinfo {
    struct awbl_drvinfo super;      /**< \brief 继承自 AWBus 设备驱动信息 */
} awbl_eim_drvinfo_t;

/** \brief AWBus EIM 总线设备 (EIM从机) 实例 */
struct awbl_eim_device {
    struct awbl_dev super;          /**< \brief 继承自 AWBus 设备 */
};

struct awbl_eimbus_opt {

    /** \brief 操作类型 */
    uint8_t      type;

    /** \brief 指向操作所需数据 */
    const void  *ptr;
};
/*******************************************************************************
  Eim控制器通用接口
*******************************************************************************/

/**
 * \brief EimBus初始化
 */
void awbl_eim_bus_init(void);


/**
 * \brief 创建控制器
 * 
 * param[in] p_master : 总线控制器
 *
 * \return AW_OK表示创建成功，其他表示失败。
 */
aw_err_t awbl_eimbus_create (struct awbl_eim_master *p_master);


/**
 * \brief 总线控制器加锁
 *
 * \param[in] p_dev : eim设备
 *
 * \return AW_OK表示操作成功，其他表示失败。
 */
aw_err_t awbl_eimbus_lock (struct awbl_eim_device *p_dev);


/**
 * \brief 总线控制器去锁
 *
 * \param[in] p_dev : eim设备
 *
 * \return AW_OK表示操作成功，其他表示失败。
 */
aw_err_t awbl_eimbus_unlock (struct awbl_eim_device *p_dev);

/**
 * \brief 设置配置
 * \param[in] p_dev     :  eim设备
 * \param[in] cs_select :  CS 选择
 * \param[in] p_info    :  配置信息
 *
 * \return AW_OK表示操作成功，其他表示失败。
 */
aw_err_t awbl_eimbus_config (struct awbl_eim_device *p_dev,
                             uint8_t                 cs_select,
                             void                   *p_info);

/**
 * \brief 设置时序
 * \param[in] p_dev     :  eim设备
 * \param[in] cs_select :  CS 选择
 * \param[in] p_info    :  时序信息
 *
 * \return AW_OK表示操作成功，其他表示失败。
 */
aw_err_t awbl_eimbus_timing (struct awbl_eim_device *p_dev,
                             uint8_t                 cs_select,
                             void                   *p_info);

/**
 * \brief 写操作
 *
 * \param[in] p_dev     :  eim设备
 * \param[in] cs_select :  CS 选择
 * \param[in] addr      :  相对地址
 * \param[in] data      :  写信息
 *
 * \return AW_OK表示操作成功，其他表示失败。
 */
aw_err_t awbl_eim_write (struct awbl_eim_device *p_dev,
                         uint8_t                 cs_select,
                         uint32_t                addr,
                         uint16_t                data);

/**
 * \brief 读操作
 *
 * \param[in] p_dev     :  eim设备
 * \param[in] cs_select :  CS 选择
 * \param[in] addr      :  相对地址
 *
 * \return AW_OK表示操作成功，其他表示失败。
 */
uint8_t awbl_eim_read (struct awbl_eim_device *p_dev,
                       uint8_t                 cs_select,
                       uint32_t                addr);

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif /* __AWBL_EIM_BUS_H */
