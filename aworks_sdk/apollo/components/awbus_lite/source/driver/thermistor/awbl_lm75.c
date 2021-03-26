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
 * \brief lm75温度传感器驱动程序.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-07-30  nwt, first implementation.
 * \endinternal
 */
#include "driver/thermistor/awbl_lm75.h"
#include "driver/thermistor/awbl_temp.h"

#include "awbl_driver_if.h"
#include "aw_compiler.h"
#include "aw_i2c.h"

/******************************************************************************/

/******************************************************************************/
 int __temp_get_from_lm75 (void *p_cookie)
{
    struct awbl_lm75_dev   *p_lm75_dev = NULL;


    int    temp = 0;

    int err = 0;

    /* 用于存放读取的值 */
    uint8_t temp_value[2];
    
    p_lm75_dev = SERV_GET_LM75_DEV((struct awbl_temp_service *)p_cookie);


    /* 从0地址读取两个字节，即温度 */
    err = aw_i2c_read(&p_lm75_dev->lm75,
                      0x00,
                      &temp_value[0],
                      2);

    if (err != AW_OK) {

        /* 读取温度出错 */
        return err;
    }

    /* 0xE0 = 1110 0000，小数部分仅高3位有效，低5位清零 */
    temp_value[1] &= 0xE0;

    temp = temp_value[0] << 8 | temp_value[1];

    return temp;
}

/******************************************************************************/

/* adc service functions (must defined as aw_const) */
aw_local struct awbl_temp_servfuncs __g_temp_servfuncs = {
    __temp_get_from_lm75
};

/** \brief instance initializing stage 2 */
aw_local void __lm75_inst_init2 (struct awbl_dev *p_dev)
{
    struct awbl_lm75_dev   *p_temp_dev  = DEV_TO_LM75_DEV(p_dev);
    struct awbl_lm75_par   *p_temp_par  = DEV_GET_LM75_PAR(p_dev);

    p_temp_dev->temp_serv.id = p_temp_par->id;
    p_temp_dev->temp_serv.p_servfuncs = &__g_temp_servfuncs;

    /* 添加服务至列表 */
    awbl_temp_serv_add(&p_temp_dev->temp_serv);
}

/** \brief instance initializing stage 3 */
aw_local void __lm75_inst_init3 (struct awbl_dev *p_dev)
{
    struct awbl_lm75_dev   *p_temp_dev  = DEV_TO_LM75_DEV(p_dev);
    struct awbl_lm75_par   *p_temp_par  = DEV_GET_LM75_PAR(p_dev);

    /* 生成从机设备LM75描述结构 */
    aw_i2c_mkdev(&(p_temp_dev->lm75),
                 p_dev->p_devhcf->bus_index,
                 p_temp_par->slave_addr,
                 (AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE));
}



/******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_lm75 = {
    NULL,                         /* devInstanceInit */
    __lm75_inst_init2,            /* devInstanceInit2 */
    __lm75_inst_init3             /* devConnect */
};

/** \brief driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_lm75 = {
    AWBL_VER_1,                         /* awbl_ver */
    AWBL_BUSID_I2C,                     /* bus_id */
    LM75_TEMP_NAME,                     /* p_drvname */
    &__g_awbl_drvfuncs_lm75,            /* p_busfuncs */
    NULL,                               /* p_methods */
    NULL                                /* pfunc_drv_probe */
};

/** \brief register ntc-thermistor driver */
void awbl_lm75_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_lm75);
}

/* end of file */
