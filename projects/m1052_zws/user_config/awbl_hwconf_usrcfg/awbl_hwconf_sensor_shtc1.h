/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_SENSOR_SHTC1_H
#define __AWBL_HWCONF_SENSOR_SHTC1_H


#ifdef AW_DEV_SENSOR_SHTC1

#include "driver/sensor/awbl_shtc1.h"

/** \brief SHTC1 设备信息 */
aw_local aw_const struct awbl_shtc1_devinfo __g_shtc1_devinfo = {

        SENSOR_SHTC1_START_ID,  /*< \brief 通道起始ID     */
        0x70,                   /*< \brief IIC设备地址    */

};

/** \brief SHTC1 设备实例内存静态分配 */
aw_local struct awbl_shtc1_dev __g_shtc1_dev;

#define AWBL_HWCONF_SHTC1                         \
    {                                             \
        AWBL_SHTC1_NAME,                          \
        0,                                        \
        AWBL_BUSID_I2C,                           \
        IMX1050_LPI2C1_BUSID,                     \
        (struct awbl_dev *)&__g_shtc1_dev,        \
        &__g_shtc1_devinfo                        \
    },

#else
#define AWBL_HWCONF_SHTC1

#endif  /* AW_DEV_SENSOR_SHTC1 */

#endif  /* __AWBL_HWCONF_SENSOR_SHTC1_H */

/* end of file */
