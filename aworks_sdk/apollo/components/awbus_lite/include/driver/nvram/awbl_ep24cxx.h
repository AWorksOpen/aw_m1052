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
 * \brief ep24cxx 通用EEPROM驱动
 *
 * EP24CXX 是指与常见的CAT24C02、CAT24C08这一系列EEPROM兼容芯片。芯片容量从
 * 1kbit 到 1Mbit。
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "ep24cxx"
 *  - 总线类型: AWBL_BUSID_I2C
 *  - 设备信息: struct awbl_ep24cxx_devinfo
 *
 * \par 2.兼容设备
 *
 *  - CAT24C02、CAT24C04、CAT24C256等全系列EEPROM；
 *  - 其它与CAT24CXX兼容的EEPROM
 *
 * \par 3.访问接口
 *
 *  - \ref grp_aw_serv_nvram
 *
 * \par 4.设备定义/配置
 *
 *  - \ref grp_awbl_ep24cxx_hwconf
 *
 * \par 5.组件依赖
 *
 * 能够使用本驱动之前需要添加以下依赖组件:
 *
 *  - AW_COM_AWBL_NVRAM (NVRAM 通用库)
 *  - AW_COM_AWBL_I2CBUS (I2C-bus 通用库)
 *
 * \internal
 * \par modification history:
 * - 1.00 12-10-22  zen, first implementation
 * \endinternal
 */

/**
 * \addtogroup grp_awbl_drv_ep24cxx
 * \copydetails awbl_ep24cxx.h
 * @{
 */

/**
 * \defgroup grp_awbl_ep24cxx_hwconf    设备定义/配置
 *
 * EP24CXX 硬件设备的定义如下面的代码所示，用户在定义目标系统的硬件资源时，
 * 可以直接将这段代码嵌入到 awbus_lite_hwconf_usrcfg.c 中对应的位置，然后对设备
 * 信息进行必要的配置(可配置的项目用【x】标识)。
 * \note 需要扩展多少片EEPROM，就将下面的代码嵌入多少次，注意将变量修改为不同的
 *       名字。
 *
 * \include  hwconf_usrcfg_nvram_ep24cxx.c
 * \note 以下配置项中带【用户配置】标识的，表示用户在开发应用时可以配置此参数。
 *
 * - 【1】存储段配置列表【用户配置】 \n
 * 在此列表中将EEPROM设备的存储空间划分为若干存储段，然后就可以使用标准 NVRAM
 * 接口 aw_nvram_set() 或 aw_nvram_get() 来读写这些存储段，更多信息请参考
 * \ref grp_aw_serv_nvram 。
 *
 * - 【2】I2C 从机地址 \n
 * 根据EEPROM的用户手册以及目标系统的硬件设计填写此参数。
 *
 * - 【3】芯片型号 \n
 * 可填写的型号请参考 \ref grp_awbl_ep24cxx_model 。
 *
 * - 【4】是否有分页 \n
 * 请根据EEPROM的用户手册填写此参数， true -有分页， false -无分页。
 * \note 铁电公司的FRAM无分页，填写 false 。
 *
 * - 【5】写周期 \n
 * 请根据EEPROM的用户手册填写此参数，单位：ms。
 * \note 铁电公司的FRAM无写周期，填写 0 。
 *
 * - 【6】EP24CXX 单元号 \n
 * 为每个 EP24CXX 设备分配不同的单元号
 *
 * - 【7】I2C总线编号 \n
 * EEPROM所在I2C总线编号，系统的I2C总线编号请参考 \ref grp_aw_plfm_i2c_busid
 */

/** \defgroup grp_awbl_ep24cxx_model */

/** @} grp_awbl_drv_ep24cxx */

#ifndef __AWBL_EP24CXX_H
#define __AWBL_EP24CXX_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_task.h"
#include "awbl_i2cbus.h"
#include "awbl_nvram.h"
#include "aw_sem.h"

#define AWBL_EP24CXX_NAME   "ep24cxx"

/**
 * \addtogroup grp_awbl_ep24cxx_model EEPROM 芯片型号定义
 * @{
 * 只要和CAT EEPROM 系列芯片兼容，便属于下面某种型号。
 *
 * 例如，ON 公司的芯片 CAT24C04 CAT24L04 和 RAMTRON 公司的 FM24C04 FM24L04
 * 都属于型号 AWBL_EP24CXX_EP24C04
 */
//#define AWBL_EP24CXX_EP24C01    (0u)        /**< \brief 128 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C02    (1u)        /**< \brief 256 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C04    (2u)        /**< \brief 512 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C08    (3u)        /**< \brief 1024 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C16    (4u)        /**< \brief 2048 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C32    (5u)        /**< \brief 4096 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C64    (6u)        /**< \brief 8192 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C128   (7u)        /**< \brief 16384 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C256   (8u)        /**< \brief 32768 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C512   (9u)        /**< \brief 65536 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C1024  (10u)       /**< \brief 131072 Byte EEPROM */
//
//#define AWBL_DA24XX_DA2460      (11u)       /**< \brief 256 Byte EEPROM */


/*
 * bit[14:0]：bytes of page size
 * bit[15]：are there pages，1:has pages  0:has no pages
 * bit[18:16]：data address bits number in slave address
 * bit[19]：bytes of the register address length，0:1byte  1:2bytes
 * bit[23:20]：max size, (2^n)*128 bytes in the eeprom
 * bit[31:24]：write time 0~255 ms
 */

#define AWBL_EP24CXX_EP24C01    \
    ( (16u) | (1u << 15) | (0u << 16) | (0u << 19) | (0u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C02    \
    ( (16u) | (1u << 15) | (0u << 16) | (0u << 19) | (1u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C04    \
    ( (16u) | (1u << 15) | (1u << 16) | (0u << 19) | (2u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C08    \
    ( (16u) | (1u << 15) | (2u << 16) | (0u << 19) | (3u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C16    \
    ( (16u) | (1u << 15) | (3u << 16) | (0u << 19) | (4u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C32    \
    ( (32u) | (1u << 15) | (0u << 16) | (1u << 19) | (5u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C64    \
    ( (32u) | (1u << 15) | (0u << 16) | (1u << 19) | (6u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C128   \
    ( (64u) | (1u << 15) | (0u << 16) | (1u << 19) | (7u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C256   \
    ( (64u) | (1u << 15) | (0u << 16) | (1u << 19) | (8u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C512   \
    ( (128u) | (1u << 15) | (0u << 16) | (1u << 19) | (9u << 20) | (10u << 24) )
#define AWBL_EP24CXX_EP24C1024  \
   ( (256u) | (1u << 15) | (1u << 16) | (1u << 19) | (10u << 20) | (10u << 24) )


#define AWBL_EP24CXX_DS2460      ( \
/* page size */                        (8u)       | \
/* has page  */                        (1u << 15) | \
/* data bits in slave address */       (0u << 16) | \
/* register address length  */         (0u << 19) | \
/* max size  */                        (1u << 20) | \
/* write time  */                      (10u << 24) \
                                )

/** @} */

#define AWBL_EP24CXX_TPMAX      (11u)
#define AWBL_EP24CXX_TPCNT      (AWBL_EP24CXX_TPMAX + 1)

/**
 * \brief EP24CXX 设备信息
 */
struct awbl_ep24cxx_devinfo {

    /** \brief 设备从机地址 */
    uint8_t addr;

    /** \brief 芯片型号 */
    uint32_t type;

    /** \brief 非易失性存储段配置列表 */
    const struct awbl_nvram_segment *p_seglst;

    /** \brief 非易失性存储段配置列表条目数 */
    size_t seglst_count;
};

/**
 * \brief EP24CXX 设备实例
 */
struct awbl_ep24cxx_dev {
    struct awbl_i2c_device  super;      /**< \brief 继承自AWBus I2C device */
    AW_MUTEX_DECL(devlock);             /**< \brief 设备锁 */
};

/**
 * \brief lpc11xx driver register
 */
void awbl_ep24cxx_drv_register(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_EP24CXX_H */

/* end of file */
