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
 * \brief ft5x06 touch screen controller
 *
 * \internal
 * \par modification history:
 * - 1.00 14-07-17  ops, first implemetation
 * \endinternal
 */
#ifndef AWBL_FT5X06_TS_H_
#define AWBL_FT5X06_TS_H_

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

/**
 * \name FT5x06触摸设备I2C从机地址（驱动当中会左移一位，变成0x70)
 * @{
 */
#define FT5x06_ADDRESS        (0x70 >> 1)

/** @} */

#define AWBL_FT5X06_TOUCHSCREEN_NAME            "ft5x06_touchscreen"


typedef struct awbl_ft5x06_ts_devinfo {
    /** \brief FT5x06设备地址               */
    int  dev_addr;

    /** \brief 连接FT5X06设备的复位引脚 */
    int   rst_pin;

    /** \brief 连接FT5X06设备的中断引脚 */
    int   int_pin;

    awbl_ts_service_info_t ts_serv_info;

}awbl_ft5x06_ts_devinfo_t;


typedef struct awbl_ft5x06_ts_dev {

    /** \brief 继承自 AWBus设备 */
    struct awbl_dev super;

    /** \bried 继承自ts服务.   */
    struct awbl_ts_service ts_serv;

    /** \brief ft5x06从设备结构.*/
    aw_i2c_device_t i2c_ft5x06;

    /** \brief 第一下按下标志 */
    uint8_t is_debounce;

}awbl_ft5x06_ts_dev_t;

/**
 * \brief ft5x063 pwm driver register
 */
void awbl_ft5x06_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* AWBL_FT5X06_TS_H_ */
