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
 * \brief AWBus 精简版接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbus_lite.h"
 * \endcode
 * 本模块为 AWBus 的精简版实现，用于在小资源平台上对驱动、硬件设备进行管理，
 * 实现硬件设备的自动发现和实例化。
 * 平台上所有的(硬件)设备实例被组织到一个树形结构上，上层设备使用“method”
 * 接口来获取这些设备实例提供的接口。
 *
 * 更多内容待添加。。。
 *
 * \attention 只能在系统初始化的时候，先注册所有驱动，再实例化；不支持动态注册
 *            驱动。
 *
 * \internal
 * \par modification history:
 * - 1.01 12-11-02  zen, add function awbl_dev_parent()
 * - 1.00 12-09-21  zen, first implementation
 * \endinternal
 */

#ifndef __AWBUS_LITE_H
#define __AWBUS_LITE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/*******************************************************************************
  defines
*******************************************************************************/

/**
 * \name AWBus-lite 版本号
 * @{
 * 精简版实现中，版本号用8-bit表示：bit[7:0] 主版本号，无副版本号和微调版本号。
 */
#define AWBL_VER_1              1       /**< \current version 1.0.0  */

/** @} */

#define AWBL_MAX_DRV_NAME_LEN   31      /* 31-byte device names */

/**
 * \name 总线类型ID
 * @{
 * 共8-bit，
 * bit[6:0] 总线类型，为下面定义的值，范围(0~127)；
 * bit[7] 设备类型，1-此设备为总线控制器，0-此设备为普通设备
 */
#define AWBL_DEVID_GET(a)         ((a) & 0x80)
#define AWBL_BUSID_GET(a)         ((a) & 0x7F)

#define AWBL_DEVID_DEVICE         0x00 /**< \brief 普通设备 */
#define AWBL_DEVID_BUSCTRL        0x80 /**< \brief 总线控制器 */

#define AWBL_BUSID_PLB            1  /**< \brief 处理器本地总线 */

#define AWBL_BUSID_VME            2  /**< \brief VME */
#define AWBL_BUSID_PCI            3  /**< \brief PCI */
#define AWBL_BUSID_PCIX           4  /**< \brief PCI w/ PCI-X ext */
#define AWBL_BUSID_PCIEXPRESS     5  /**< \brief PCI w/ PCI-Express ext */
#define AWBL_BUSID_HYPERTRANSPORT 6  /**< \brief PCI w/ HyperTransport ext */
#define AWBL_BUSID_RAPIDIO        7  /**< \brief RapidIO */
#define AWBL_BUSID_MII            8  /**< \brief MII 总线 */
#define AWBL_BUSID_VIRTUAL        9  /**< \brief 虚拟总线 */

#define AWBL_BUSID_MF             10  /**< \brief 多功能设备总线 */
#define AWBL_BUSID_USB_HOST_EHCI  11  /**< \brief EHCI 总线类型 */
#define AWBL_BUSID_USB_HOST_OHCI  12  /**< \brief OHCI 总线类型 */
#define AWBL_BUSID_USB_HOST_UHCI  13  /**< \brief UHCI 总线类型 */
#define AWBL_BUSID_USB_HUB        14  /**< \brief hub 总线类型 */

#define AWBL_BUSID_I2C            20  /**< \brief I2C 总线类型 */
#define AWBL_BUSID_SPI            21  /**< \brief SPI 总线类型 */
#define AWBL_BUSID_SDIO           22  /**< \brief SDIO 总线类型 */
#define AWBL_BUSID_NAND           23  /**< \brief NAND 总线类型 */
#define AWBL_BUSID_USBH           24  /**< \brief USB 总线类型 */
#define AWBL_BUSID_SATA           25  /**< \brief SATA 总线类型 */
#define AWBL_BUSID_EIM            26  /**< \brief EIM 总线类型 */
/** @} */

/**
 * \name 方法(method)相关的宏定义
 * @{
 */

/** \brief 方法对象 */
#define AWBL_METHOD(name, handler)  \
    {(awbl_method_id_t)(&awbl_##name##_desc[0]), (awbl_method_handler_t)handler}

/** \brief 方法列表结束标志 */
#define AWBL_METHOD_END     {0, NULL}

/** \brief 声明一个方法 */
#define AWBL_METHOD_IMPORT(name)    aw_import char aw_const awbl_##name##_desc[]

/** \brief 定义一个方法 */
#define AWBL_METHOD_DEF(method, string) char aw_const awbl_##method##_desc[]=""

/** \brief 得到方法的ID */
#define AWBL_METHOD_CALL(method)    (awbl_method_id_t)(&awbl_##method##_desc[0])

/** @} */

/**
 * \name 标准方法
 * @{
 */

/* 移除设备设备时将调用此方法 , 返回AW_OK表示成功，返回其它值表示移除失败 */
AWBL_METHOD_IMPORT(awbl_drv_unlink);

/** @} */

/** \brief 得到设备实例的设备信息(由驱动定义) */
#define AWBL_DEVINFO_GET(p_dev) \
    (((struct awbl_dev *)(p_dev))->p_devhcf->p_devinfo)

/** \brief 得到设备实例 HCF 信息(AWBUS所定义, 包括 name, unit, bus_type 等信息) */
#define AWBL_DEVHCF_GET(p_dev) \
    (((struct awbl_dev *)(p_dev))->p_devhcf)

/** \brief 得到设备实例的名字 */
#define AWBL_DEV_NAME_GET(p_dev) \
    (((struct awbl_dev *)(p_dev))->p_devhcf->p_name)

/** \brief 得到设备实例的单元号 */
#define AWBL_DEV_UNIT_GET(p_dev) \
    (((struct awbl_dev *)(p_dev))->p_devhcf->unit)

/** \brief 得到设备实例的总线类型 */
#define AWBL_DEV_BUS_TYPE_GET(p_dev) \
    (((struct awbl_dev *)(p_dev))->p_devhcf->bus_type)

/** \brief 得到设备实例的总线编号 */
#define AWBL_DEV_BUS_INDEX_GET(p_dev) \
    (((struct awbl_dev *)(p_dev))->p_devhcf->bus_index)

/*******************************************************************************
 type defines
*******************************************************************************/
typedef struct awbl_dev         awbl_dev_t;
typedef struct awbl_busctlr     awbl_busctlr_t;
typedef struct awbl_drvfuncs    awbl_drvfuncs_t;
typedef struct awbl_drvinfo     awbl_drvinfo_t;
typedef struct awbl_dev_method  awbl_dev_method_t;
typedef struct awbl_buspresent  awbl_buspresent_t;

/** \biref 传递给 awbl_dev_iterate() 的处理函数类型 */
typedef aw_err_t (*awbl_iterate_func_t)(struct awbl_dev *p_dev, void *p_arg);

/** \biref 方法ID类型 */
typedef int *awbl_method_id_t;

/** \biref 方法入口函数类型 */
typedef aw_err_t (*awbl_method_handler_t)(struct awbl_dev *p_dev, void *p_arg);

/**
 * \biref 设备方法
 *
 * 该结构体被所有设备用于给上层模块提供服务，例如，电源管理。
 */
struct awbl_dev_method {
    awbl_method_id_t       dev_method_id;   /**< \brief 方法的ID */
    awbl_method_handler_t  pfunc_handler;   /**< \brief 方法的入口点 */
};

/**
 * \brief 总线类型信息
 *
 * 本结构体提供了在总线子系统初始化或操作时会用到的入口点。
 */
struct awbl_bustype_info {

    /**
     * \brief 总线类型
     *
     * 总线类型，例如， AWBL_BUSID_PLB
     */
    uint8_t  bus_id;

    /** \brief 初始化阶段1初始化函数 */
    int (*pfunc_bustype_init1)(void);

    /** \brief 初始化阶段2初始化函数 */
    int (*pfunc_bustype_init2)(void);

    /** \brief 初始化阶段3连接函数 */
    int (*pfunc_bustype_connect)(void);

    /** \brief 检查总线上的新设备 */
    int (*pfunc_bustype_newdev_present)(void);

    /** \brief 检查设备和驱动是否匹配 */
    aw_bool_t (*pfunc_bustype_devmatch)(const struct awbl_drvinfo *p_drv,
                                        struct awbl_dev           *p_dev);
};

/**
 * \brief AWBus 设备实例结构体
 *
 * 本结构体在总线和设备被配置时使用。驱动必须已经注册到总线子系统中。这些信息在
 * 总线探测和枚举阶段被被使用。
 */
struct awbl_dev {

    /** \brief 指向下一个实例 */
    struct awbl_dev  *p_next;

    /** \brief 指向父总线 */
    struct awbl_buspresent *p_parentbus;

    /**
     * \brief 指向子总线
     *
     * 当设备为总线控制器时，指向其所控制的下级总线；
     * 为普通设备时，由设备自己定义用途。
     */
    struct awbl_buspresent *p_subbus;

    /** \brief 设备实例所对应的驱动 */
    const struct awbl_drvinfo *p_driver;

    /** \brief 设备实例所对应的硬件配置信息 */
    const struct awbl_devhcf *p_devhcf;

    /** \brief AWBus 内部使用 */
    uint32_t    flags;
};

/*
 * The following flags are defined for use with the flags field in
 * the AWBus device instance structure above.
 */
#define AWBL_INST_INIT1_DONE    0x00000010
#define AWBL_INST_INIT2_DONE    0x00000020
#define AWBL_INST_CONNECT_DONE  0x00000040

#define AWBL_REMOVAL_MASK       0x0000f000
#define AWBL_REMOVAL_INSTANCE   0x00001000
#define AWBL_REMOVAL_ORPHAN     0x00002000

/**
 * \biref bus instance
 *
 * Keep track of busses which are known to be present on the system.
 * This is only used internally.
 */
struct awbl_buspresent {
    struct awbl_buspresent         *p_next;    /**< \brief next bus instance */
    const struct awbl_bustype_info *p_bustype; /**< \brief bus type */
    struct awbl_dev     *p_ctlr;         /**< \brief pointer to bus controller*/
    struct awbl_dev     *p_inst_list;    /**< \brief pointer to instance list */
    struct awbl_dev     *p_orphan_list;  /**< \brief pointer to orphan list */
};

/**
 * \brief AWBus 总线控制器设备实例结构体
 */
struct awbl_busctlr {
    struct awbl_dev        super;
    struct awbl_buspresent buspresent;  /**< \brief 总线控制器对应的总线实例 */
};

/**
 * \biref 驱动入口点结构体
 *
 * 本结构体提供了驱动的入口点，总线子系统在初始化和操作的时候会使用这些入口点。
 * 本数据结构向总线子系统标识了驱动，用于驱动注册到子系统中。
 * 每个驱动或总线控制器调用 aw_drv_register() 并传递一个由驱动声明的本结构体。
 */
struct awbl_drvfuncs {

    /*
     * \brief 初始化驱动和设备(第1阶段)
     *
     * 本函数在系统启动时总线枚举和配置时被调用。对于普通驱动此成员可以为空函数
     * [nulDrv()]
     * \param[in,out] p_dev   设备实例
     */
    void (*pfunc_dev_init1) (struct awbl_dev *p_dev);

    /*
     * \brief 初始化驱动和设备(第2阶段)
     *
     * 本函数在系统启动时总线枚举和配置时被调用。
     * 本函数在 awbl_dev_init2() 中被直接调用。对于总线控制器驱动此成员可以为
     * 空函数[nulDrv()]
     *
     *  \param[in,out] p_dev   设备实例
     */
    void (*pfunc_dev_init2) (struct awbl_dev *p_dev);

    /*
     * \brief 初始化驱动和设备(第3阶段)
     *
     * 本函数在系统启动时总线枚举和配置时被调用。
     * 本函数在 awbl_dev_connect() 中被直接调用。对于总线控制器驱动此成员可以为
     * 空函数[nulDrv()]。
     *
     *  \param[in,out] p_dev   设备信息，尚未形成实例
     */
    void (*pfunc_dev_connect) (struct awbl_dev *p_dev);
};

/**
 * \biref 驱动注册信息结构体
 *
 * 本结构体向总线子系统标识了驱动，用于将驱动注册到子系统中。
 * 每个驱动或总线控制器调用 awbl_drv_register()
 * 并传递一个由驱动声明的本结构体。
 *
 * 本结构体不应该被直接使用。取而代之的是，每种总线类型定义了一个它自己的结构体，
 * 并把本结构体作为其第一个成员(是被包含，而不是作为一个指针)。总线定义的结构体
 * 可以包含额外的信息，例如，PCI bus 所定义的 DEV_ID 和 VEND_ID。为了得到这些结
 * 构体的定义，请包含 awbl_{bus type}.h，例如，“awbl_plb.h”或“awbl_i2c.h”。
 *
 * \attention lite 版驱动定义此结构体时，必须使用 aw_const 关键字
 *
 * \par 示例
 *
 *  // GPIO 驱动入口点(设备实例初始化函数)
 *  aw_local aw_const struct awbl_drvfuncs lpc11xx_gpio_drvfuncs = {
 *      lpc11xx_gpio_inst_init1,
 *      lpc11xx_gpio_inst_init2,
 *      lpc11xx_gpio_inst_connect
 *  };
 *
 *  // 驱动方法
 *  awb_local aw_const struct awb_dev_method lpc11xx_gpio_drv_methods = {
 *      AWB_DEVMETHOD(awbGpioFunc, lpc11xx_gpio_func_get),
 *      AWB_DEVMETHOD_END
 *  }
 *
 *  // 驱动注册信息
 *  aw_local aw_const awbl_drvinfo_t lpc11xx_gpio_drv_registration = {
 *      AWBL_VER_1,
 *      AWBL_BUSID_PLB,
 *      "lpc11xx_gpio",
 *      &lpc11xx_gpio_drv_funcs,
 *      &lpc11xx_gpio_drv_methods
 *  }
 */
struct awbl_drvinfo {

    /**
     * \brief 支持的 AWBus 版本号
     *
     * 例如，AWBL_VER_1
     */
    uint8_t     awb_ver;

    /**
     * \brief 总线ID
     *
     * 例如，I2C总线设备:AWBL_BUSID_I2C 或
     *                   AWBL_BUSID_I2C | AWBL_DEVID_DEVICE;
     *       I2C总线控制器:AWBL_BUSID_I2C | AWBL_DEVID_DEVICE;
     */
    uint8_t     bus_id;

    /**
     * \brief 驱动名/设备名
     *
     * 驱动的名字，驱动与设备结合变成实例后，可作为设备的名字
     */
    char       *p_drvname;

    /**
     * \brief 驱动入口点
     *
     * 系统启动的时候调用这些入口点完成实例的初始化
     */
    const struct awbl_drvfuncs *p_busfuncs;

    /**
     * \breif 驱动提供的方法
     *
     * 驱动提供的方法
     */
    const struct awbl_dev_method *p_methods;

    /**
     * \brief 驱动探测函数
     *
     * \retval true     探测成功
     * \retval false    探测失败
     */
    aw_bool_t (* pfunc_drv_probe) (awbl_dev_t *p_dev);

};

/**
 * \brief 硬件设备信息
 *
 * 在硬件配置文件 awbus_lite_hwconf_usrcfg.c 中使用本结构体来描述目标系统上的硬件
 * 设备
 */
struct awbl_devhcf {
    const char          *p_name;    /**< \brief 设备名 */
    uint8_t              unit;      /**< \brief 设备单元号 */
    uint8_t              bus_type;  /**< \brief 设备所处总线的类型 */
    uint8_t              bus_index; /**< \brief 设备所处总线的编号 */
    struct awbl_dev     *p_dev;     /**< \brief 指向设备实例内存 */
    const void          *p_devinfo; /**< \brief 指向设备信息(常量) */
};


/**
 * \brief 获取硬件设备列表
 *
 * \attention 本函数由应用提供,调用awbl_devhcf_list_count_get() 获取列表成员个数
 *
 * \return 硬件设备列表
 *
 * \sa awbl_devhcf_list_count_get()
 */
aw_import aw_const struct awbl_devhcf *awbl_devhcf_list_get(void);

/**
 * \brief 获取硬件设备列表成员个数
 *
 * \attention 本函数由应用提供,调用awbl_devhcf_list_get() 获取列表
 *
 * \return 硬件设备列表成员个数
 *
 * \sa awbl_devhcf_list_get();
 */
aw_import size_t awbl_devhcf_list_count_get(void);


/**
 * \brief 获取硬件设备列表成员数
 */
void awbus_lite_init (void);

/**
 * \brief AWBus 设备初始化第1阶段
 *
 * 本函数执行设备初始化的第1阶段。本函数执行前，应当已建立最基本的 C 运行环境，
 * 操作系统服务尚未提供。不可以在本阶段执行连接中断、申请信号量等操作。
 *
 * \retval  AW_OK   成功
 */
aw_err_t awbl_dev_init1 (void);

/**
 * \brief AWBus 设备初始化第2阶段
 *
 * 本函数执行设备初始化的第2阶段。本函数执行前，操作系统服务应当已经能够使用，
 * 本阶段执行设备的主要初始化。
 *
 * \retval  AW_OK   成功
 */
aw_err_t awbl_dev_init2 (void);

/**
 * \brief AWBus 设备初始化第3阶段
 *
 * 本函数执行设备初始化的第3阶段。本函数执行前，系统的主要初始化已经完成，可以
 * 使用各种资源。如果平台支持任务(线程)，则此函数将在一个单独的任务中被执行。
 * 设备耗时长的初始化操作应当放在此阶段中。
 *
 * \retval  AW_OK   成功
 */
aw_err_t awbl_dev_connect (void);

/**
 * \brief 注册一个设备驱动
 *
 * 本函数注册一个设备驱动到 AWBus 系统
 *
 * \attention   只能在 awbl_dev_init1() 函数被调用之前注册驱动
 *
 * \param[in]   p_drv       驱动注册信息
 *
 * \retval      AW_OK       注册成功
 * \retval      -EPERM      不允许的操作 (在 awbl_dev_init1() 之后注册驱动)
 * \retval      -ENOTSUP    当前 AWBus 不支持此版本的驱动
 * \retval      -ENOSPC     空间不足
 */
aw_err_t awbl_drv_register(const struct awbl_drvinfo *p_drvinfo);

/**
 * \brief 注册一个总线类型
 *
 * 本函数注册一个总线类型到 AWBus 系统
 *
 * \param[in]   p_bustype   总线信息
 *
 * \retval      AW_OK       注册成功
 * \retval      -ENOSPC     空间不足
 */
aw_err_t awbl_bustype_register(const struct awbl_bustype_info *p_bustype);

/**
 * \brief 向总线子系统声明一个设备
 *
 * 本函数遍历已注册的总线类型和设备驱动，尝试给已发现的设备匹配一个合适的驱动。
 * 如果找到了驱动，然后就可以创建设备实例，该实例被添加到设备所处的总线上。
 * 如果没有找到匹配的驱动，则设备被添加到所处总线的孤儿列表中。
 *
 * 在调用本函数之前，p_dev->p_parentbus 必须被正确赋值。否则，该设备将被丢弃。
 *
 * \param[in,out]   p_dev   未完成的设备信息
 *
 * \retval          AW_OK   成功
 * \retval         -ENOENT  设备无父总线
 */
aw_err_t awbl_dev_announce (struct awbl_dev *p_dev);


/**
 * \brief 从总线子系统中移除一个设备
 *
 * 本函数将设备从其父总线上删除，并调用驱动提供的unlink方法，若设备为总线控制
 * 器，将递归删除其子总线上的所有设备。
 *
 * \param[in,out]   p_dev   未完成的设备信息
 *
 * \retval          AW_OK    成功
 * \retval          AW_ERROR 失败
 */
aw_err_t awbl_dev_remove_announce (struct awbl_dev *p_dev);

/**
 * \brief 向总线子系统声明一个总线实例
 *
 * 当总线控制器发现一个新总线的时候，调用本来将总线实例化。
 *
 * 在调用本函数之前，p_dev->p_parentbus 必须被正确赋值。否则，该总线将被丢弃。
 *
 * \param[in,out]   p_busdev        总线控制器
 * \param[in]       bus_id          要实例化的总线类型
 *
 * \retval          AW_OK       成功
 * \retval         -EINVA       参数错误，p_busctlr 为 NULL
 * \retval         -ENODEV      总线控制器尚未被声明
 * \retval         -ENOENT      未找到总线类型 \a bus_id 或未找到父总线
 */
aw_err_t awbl_bus_announce (struct awbl_busctlr *p_busctlr, uint8_t bus_id);

/**
 * \brief 在每个设备上执行指定的动作
 *
 * \a pfunc_action 的返回值决定遍历是否继续：
 *    - AW_OK                   遍历继续
 *    - -EINTR或其它非AW_OK的值 遍历终止
 *
 * \attention   若\a flags 被设置了 AWBL_ITERATE_INTRABLE 标志，
 *              则\a pfunc_action 的返回值可终止遍历；若没有设置，
 *              则 awbl_dev_iterate() 总是返回 AW_OK。
 *
 * \param[in]   pfunc_action    对每个设备要执行的动作
 * \param[in]   p_arg           传递给 \a pfunc_action 的参数
 * \param[in]   flags           决定遍历的行为，为以下值的或操作：
 *                              - AWBL_ITERATE_INSTANCES
 *                              - AWBL_ITERATE_ORPHANS
 *                              - AWBL_ITERATE_INTRABLE
 * \retval      AW_OK           成功
 * \retval      -EFAULT         \a pfunc_action 为 NULL
 * \retval      -EINTR          遍历被中断，未能成功遍历所有设备
 */
aw_err_t awbl_dev_iterate(awbl_iterate_func_t  pfunc_action,
                          void                *p_arg,
                          int                  flags);

/* awb_dev_iterate() flags 参数定义 */

#define AWBL_ITERATE_INSTANCES   1 /* 遍历所有实例 */
#define AWBL_ITERATE_ORPHANS     2 /* 遍历所有孤儿设备 */
#define AWBL_ITERATE_INTRABLE    4 /* pfunc_action 的返回值可终止遍历 */

/**
 * \brief 查找设备的方法
 *
 * 本函数在设备中查找一个指定方法的入口点
 *
 * \param[in]   p_dev   设备
 * \param[in]   method  指定的方法
 *
 * \retval      NULL    未找到方法或参数错误
 * \retval      非NULL  方法的入口点(函数指针)
 */
awbl_method_handler_t awbl_dev_method_get(struct awbl_dev  *p_dev,
                                          awbl_method_id_t  method);

/**
 * \brief 在所有设备上运行指定的方法
 *
 * 本函数在所有设备(实例)上运行指定的方法，并传递参数\a p_arg 。
 *
 * \param[in]       method  要运行的方法
 * \param[in,out]   p_arg   运行方法时传入的参数
 *
 * \retval          AW_OK   成功
 */
aw_err_t awbl_alldevs_method_run(awbl_method_id_t method, void *p_arg);

/**
 * \brief 本函数无任何操作，总是返回 -ENOTSUP
 *
 * \retval  -ENOTSUP    不支持的操作
 */
aw_err_t awbl_rtn_notsup(void);

/**
 * \brief 本函数查找一个指定设备的父亲
 *
 * \param p_dev 指定的设备
 *
 * \return 指向父亲设备的指针，或NULL
 */
struct awbl_dev *awbl_dev_parent(struct awbl_dev *p_dev);

/**
 * \brief 查找指定名称和单元号的设备
 *
 * \param name 设备名字
 * \param unit 单元号
 *
 * \return 指向父亲设备的指针，或NULL
 */
awbl_dev_t *awbl_dev_find_by_name(const char *name, int unit);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBUS_LITE_H */

/* end of file */
