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
 * \brief AWBus LPC11XX SSP 驱动头文件
 *
 * LPC11XX SSP 为 LPC11XX 系列MCU的片上SSP，本驱动只实现了SPI主机功能。
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "lpc11xx_ssp"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_lpc11xx_ssp_devinfo
 *
 * \par 2.兼容设备
 *
 *  - NXP LPC11XX 系列MCU
 *  - NXP LPC13XX 系列MCU
 *  - 其它与LPC11XX SSP 兼容的设备
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_lpc11xx_ssp_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_spi
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-20  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_LPC11XX_SPI_H
#define __AWBL_LPC11XX_SPI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_drv_lpc11xx_ssp
 * \copydetails awbl_lpc11xx_ssp.h
 * @{
 */

/**
 * \defgroup  grp_awbl_lpc11xx_ssp_hwconf 设备定义/配置
 *
 * LPC11XX I2C 硬件设备的定义如下面的代码所示，用户在定义目标系统的硬件资源时，
 * 可以直接将这段代码嵌入到 awbus_lite_hwconf_usrcfg.c 中对应的位置，然后对设备
 * 信息进行必要的配置(可配置的项目用【x】标识)。
 *
 * \note 有多少个I2C，就将下面的代码嵌入多少次，注意将变量修改为不同的名字。

 * \include  hwconf_usrcfg_lpc11xx_ssp.c
 * \note 以下配置项中带【用户配置】标识的，表示用户在开发应用时可以配置此参数。
 *
 * - 【1】 平台相关初始化 \n
 * 在平台初始化函数中完成使能SSP设备时钟等与平台相关的初始化操作。
 *
 * - 【2】 SPI总线编号 \n
 * 定义此SSP控制器的总线编号。整个系统中的SPI总线编号分配不能有重叠。
 * 使用 Apollo标准SPI接口时，使用此总线编号，便可以定位到这里定义的
 * LPC11XX SSP设备。
 *
 * - 【3】 寄存器基地址 \n
 * 请参考MCU的用户手册填写此参数
 *
 * - 【4】 中断号 \n
 * 请参考系统的中断号分配填写此参数，见 \ref grp_aw_plfm_inums

 * - 【5】 输入时钟频率 \n
 * 请根据系统的设置填写此值。
 *
 * - 【6】 LPC11XX SSP 单元号 \n
 * 为每个LPC11XX SSP 分配不同的单元号
 */

/** @} grp_awbl_drv_lpc11xx_ssp */

#include "aw_spinlock.h"
#include "awbl_spibus.h"

#define AWBL_LPC11XX_SSP_NAME   "lpc11xx_ssp"

/**
 * \brief LPC11XX SSP 设备信息
 */
struct awbl_lpc11xx_ssp_devinfo {

    /** \brief 继承自 AWBus SPI 控制器设备信息 */
    struct awbl_spi_master_devinfo spi_master_devinfo;

    uint32_t regbase;               /**< \brief 寄存器基地址 */
    int      inum;                  /**< \brief 中断号 */
    uint32_t clkfreq;               /**< \brief 输入时钟频率 */

    /** \brief 平台相关初始化：开启时钟、初始化引脚等操作 */
    void (*pfunc_plfm_init)(void);
};

/**
 * \brief LPC11XX SSP 设备实例
 */
struct awbl_lpc11xx_ssp_dev {

    /** \brief 继承自 AWBus SPI 主机控制器 */
    struct awbl_spi_master  super;

    /** \brief 当前正在处理的 Message */
    struct aw_spi_message *p_cur_msg;

    /** \brief 当前正在处理的 Transfer */
    struct aw_spi_transfer *p_cur_trans;

    /** \brief 传输数据计数 */
    size_t      data_ptr;

    /** \brief 控制器忙标志 */
    aw_bool_t        busy;

    /** \brief 控制器状态机状态  */
    int         state;

    uint32_t    cur_speed;      /**< /brief 控制器当前速度 */
    uint8_t     cur_bpw;        /**< /brief 控制器当前帧大小 */
    uint8_t     cur_mode;       /**< /brief 控制器当前模式 */

    uint8_t     nbytes_to_recv; /**< /brief 待接收的字节数 */
    uint8_t     cs_toggle;      /**< /brief 片选自锁标志 */
    aw_spi_device_t *p_tgl_dev; /**< /brief 当前自锁的SPI设备 */
};

/**
 * \brief LPC11XX SSP master driver register
 */
void awbl_lpc11xx_ssp_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_LPC11XX_SPI_H */

/* end of file */
