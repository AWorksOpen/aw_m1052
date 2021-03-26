/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief imx10xx lpi2c driver  header file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-10-18  like, first implementation
 * \endinternal
 */

#ifndef AW_IMX10XX_LPI2C_SLAVE_H
#define AW_IMX10XX_LPI2C_SLAVE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "driver/lpi2c/imx10xx_lpi2c_regs.h"
#include "imx1050_reg_base.h"
#include "imx1050_pin.h"
#include "aw_gpio.h"
#include "imx1050_inum.h"


#define I2C_CLK_ID    IMX1050_CLK_CG_LPI2C1
#define I2C_INT_NUM   INUM_LPI2C1
#define I2C_BASE      IMX1050_LPI2C1_BASE

typedef struct i2c_slave_handle i2c_slave_handle_t;
typedef enum i2c_slave_event i2c_slave_event_t;
typedef void (*i2c_slave_callback_t)(i2c_slave_event_t event, uint8_t *data);


enum i2c_slave_event
{
    SlaveTransmitEvent = 0x02U,
    SlaveReceiveEvent = 0x04U,
    SlaveRepeatedStartEvent = 0x10U,
    SlaveCompletionEvent = 0x20U,

    SlaveAllEvents = SlaveTransmitEvent | SlaveReceiveEvent |
                     SlaveRepeatedStartEvent | SlaveCompletionEvent,
};

struct i2c_slave_handle
{
    uint16_t             addr;
    i2c_slave_callback_t callback;
};

void aw_i2c_slave_init(uint16_t addr, i2c_slave_callback_t cb);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* AWBL_IMX10XX_LPI2C_H */
