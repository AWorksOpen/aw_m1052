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
 * \brief NS16550 compatible UART plb interface driver support
 *
 * 本驱动适用于所有 16550 标准兼容的UART器件
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:    "ns16550"
 *  - 总线类型:  AWBL_BUSID_PLB
 *  - 设备信息:  struct awbl_ns16550_plb_chan_param
 *
 * \par 2.兼容设备
 *
 *  - 所有 16550 标准兼容的UART器件
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_ns16550_plb_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_serial
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-28  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_NS16550_PLB_H
#define __AWBL_NS16550_PLB_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_ns16550
 * \copydetails awbl_ns16550_plb.h
 * @{
 */

/**
 * \defgroup  grp_awbl_ns16550_plb_hwconf 设备定义/配置
 *
 * ns16550 兼容设备的定义如下面的代码所示，用户在定义目标系统的硬件资源时，
 * 可以直接将这段代码嵌入到 awbus_lite_hwconf_usrcfg.c 中对应的位置，然后对设备
 * 信息进行必要的配置(可配置的项目用【x】标识)。
 *
 * \note 有多少个16550兼容设备，就将下面的代码嵌入多少次，注意将变量修改为不同的
 *       名字。
 *
 * \include  hwconf_usrcfg_ns16550_plb.c
 * \note 以下配置项中带【用户配置】标识的，表示用户在开发应用时可以配置此参数。
 *
 * - 【1】平台相关初始化函数 \n
 * 在平台初始化函数中完成使能串口时钟、初始化硬件引脚等与平台相关的操作。
 *
 * - 【2】串口的中断号 \n
 * 请参考系统的中断号分配填写此参数，见 \ref grp_aw_plfm_inums
 *
 * - 【3】设备寄存器的起始地址 \n
 * 请查阅芯片数据手册确认设备的寄存器起始地址
 *
 * - 【4】设备寄存器间隔移位值 \n
 * 16C550兼容UART设备的寄存器大小为8位，在不同的实现上有不同的间隔，比如大部分
 * 32位SoC上的16C550兼容UART的寄存器地址为4字节对齐，即间隔为4 \n
 * 这里为了效率起见使用的是间隔的移位值，比如间隔4则移位值为2（4 == (1 << 2))，
 * 间隔为2则移位值为1，间隔为1则移位值为0
 *
 * - 【5】UART设备的FIFO大小 \n
 * 请查阅数据手册确定FIFO的大小，单位为字节
 *
 * - 【6】UART设备的输入时钟频率 \n
 * 这个输入时钟可能由外部的晶体振荡器提供，或者由系统分频后提供，需参考具体的
 * 电路或SoC手册以及系统时钟配置等来确定
 *
 * - 【7】UART波特率分频值计算函数 \n
 *  - 一般使用驱动自带的波特率计算函数而不需使用这个计算函数
 *  - 但如果该设备有特殊的波特率计算方式（如带小数波特率设置）则必须使用该函数，
 *    函数原型为： \n
 *    uint32_t uart_divisor(uint32_t xtal, uint32_t baudrate);
 *      - \a xtal     为UART设备的输入时钟频率 \n
 *      - \a baudrate 为期望的波特率 \n
 *      - 返回值为分频值
 *
 * - 【8】设备单元号 \n
 * 这个单元号即串口设备的“串口号”
 *
 */

/** @} */

#include "aw_common.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "awbl_access.h"

#include "driver/serial/awbl_ns16550.h"

/*******************************************************************************
    types
*******************************************************************************/

/** \brief ns16550 plb channel */
struct awbl_ns16550_plb_chan {
    struct awbl_dev          dev;       /**< \brief spi device data */
    struct awbl_ns16550_chan nschan;    /**< \brief NS16550 channel data */
};

/** \brief ns16550 plb channel parameter */
struct awbl_ns16550_plb_chan_param {

    /** \brief NS16550 parameter, always go first */
    struct awbl_ns16550_chan_param  nsparam;
};


#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_NS16550_PLB_H */

/* end of file */
