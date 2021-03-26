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
 * \brief gt911 touch screen controller
 *
 * \internal
 * \par modification history:
 * - 1.00 14-07-17  ops, first implemetation
 * \endinternal
 */
#ifndef AWBL_GT911_TS_H_
#define AWBL_GT911_TS_H_

#include "awbl_input.h"
#include "awbl_ts.h"
#include "aw_task.h"
#include "aw_i2c.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"
#include "awbus_lite.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

//#define GT911_DEBUG

#define CFG_SAVE    1
#define CFG_NOTSAVE 0


#define GT911_CHIPTYPE_REG       0x8000        /**< \brief 读取芯片类型*/
#define GT911_COMMAND_REG        0x8040        /**< \brief 控制寄存器  */
#define GT911_CONFIGVERSION_REG  0x8047        /**< \brief 配置文件版本号*/
#define GT911_CHECKSUM_REG       0X80FF        /**< \brief 校验和寄存器 */
#define GT911_ID_VERSION_REG     0x8140        /**< \brief 固件版本.  */
#define GT911_VENDORID_REG       0x814A        /**< \brief 模组选项信息 */
#define GT911_GSTID_REG          0x814E        /**< \brief 触摸信息 */
#define GT911_TP1_REG            0x8150        /**< \brief 第一个触摸点坐标起始地址*/
#define GT911_TP2_REG            0x8158        /**< \brief 第二个触摸点坐标起始地址*/
#define GT911_TP3_REG            0x8160        /**< \brief 第三个触摸点坐标起始地址*/
#define GT911_TP4_REG            0x8168        /**< \brief 第四个触摸点坐标起始地址*/
#define GT911_TP5_REG            0x8170        /**< \brief 第五个触摸点坐标起始地址*/


/**
 * \name   GT911触摸设备I2C从机地址
 * \remark GT911 有两个从设备地址
 * \remark 其I2C设备地址由7位设备地址加1位读写控制位组成，高7位为地址，bit0为读写控制位
 * \remark 设备的地址的选择基于主控在上电初始化时控制Reset 和INT 口不同状态
 * @{
 */
#define GT911_ADDRESS1        (0x5D)
#define GT911_ADDRESS2        (0x14)

#define AWBL_GT911_TOUCHSCREEN_NAME            "gt911"

typedef struct awbl_gt911_ts_devinfo {
    /** \brief GT911设备地址               */
    int  dev_addr;

    /** \brief 连接GT911设备的复位引脚 */
    int   rst_pin;

    /** \brief 连接GT011设备的中断引脚 */
    int   int_pin;

    /** \brief 用户定义的x轴最大坐标 */
    uint16_t x_usr_output_max;
    /** \brief 用户定义的y轴最大坐标 */
    uint16_t y_usr_output_max;

    awbl_ts_service_info_t ts_serv_info;

}awbl_gt911_ts_devinfo_t;

typedef struct awbl_gt911_ts_dev {

    /** \brief 继承自 AWBus设备 */
    struct awbl_dev super;

    /** \bried 继承自ts服务.   */
    struct awbl_ts_service ts_serv;

    /** \brief gt911从设备结构.*/
    aw_i2c_device_t i2c_gt911;

    /** \brief 第一下按下标志 */
    uint8_t is_debounce;

}awbl_gt911_ts_dev_t;

/** \brief 触摸屏驱动注册*/
void awbl_gt911_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* AWBL_GT911_TS_H_ */



















