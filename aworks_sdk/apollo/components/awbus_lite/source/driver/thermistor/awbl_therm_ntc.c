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
 * \brief NTC 热敏电阻驱动程序.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-07-30  hbt, first implementation.
 * \endinternal
 */
#include "driver/thermistor/awbl_therm_ntc.h"
#include "driver/thermistor/awbl_temp.h"

#include "awbl_driver_if.h"
#include "aw_compiler.h"
#include "aw_adc.h"

/******************************************************************************/
aw_local int __volt_get_from_therm (struct awbl_ntc_therm_dev *p_therm_dev)
{
    struct   awbl_ntc_therm_par  *par = THERM_DEV_GET_PAR(p_therm_dev);
    uint8_t  channel = par->channel;
    int           i, max, min, tmp = 0;
    aw_adc_val_t  adc_buf[12];
    int           adc_bits = aw_adc_bits_get(channel);
    
    for (i = 0; i < 12; i++) {
        aw_adc_sync_read(channel, (aw_adc_val_t *)&adc_buf[i], 1,AW_FALSE);
    }
    
    max = adc_buf[0];
    min = adc_buf[0];
    
    for (i = 0; i < 12; i++) {
        tmp += adc_buf[i];
        if (adc_buf[i] > max) {
            max = adc_buf[i];
            continue;
        }
        if (adc_buf[i] < min) {
            min = adc_buf[i];
        }
    }
    
    tmp = (tmp - max - min) / 10;
    return (tmp * 3300 / (1 << adc_bits));
}

/******************************************************************************/
aw_local int __ohm_get_from_volt (struct awbl_ntc_therm_dev *p_therm_dev)
{
    struct awbl_ntc_therm_par        *par = THERM_DEV_GET_PAR(p_therm_dev);  
    const struct awbl_temp_ohm_match *p_lst = par->p_tempohm_lst;
    int32_t up_ohm   = par->pullup_ohm;
    int32_t down_ohm = par->pulldown_ohm;
    int32_t tmp_mv;
    int16_t mv       = par->full_mv;
    int     connect  = par->connect;
    int     ohm      = 0;
    
    /* get voltage */
    tmp_mv = __volt_get_from_therm(p_therm_dev);
    
    /* 不同连接方式的电阻值计算 */
    if ((connect == CONNECT_UP) && (down_ohm != 0)) {
        ohm = (mv - tmp_mv) * down_ohm / tmp_mv;
    } else if ((connect == CONNECT_DOWN) && (up_ohm != 0)) {
        ohm = (tmp_mv * up_ohm  / (mv - tmp_mv));
    } else if ((connect == CONNECT_PARA_SER) && 
               (down_ohm != 0) && 
               (up_ohm != 0)) {
        ohm = (tmp_mv * up_ohm * down_ohm) / (((mv - tmp_mv) * 
               down_ohm) - tmp_mv * up_ohm);
    } else {
        return AW_ERROR;
    }
    
    if (ohm > p_lst[0].ohm) {
        ohm = p_lst[0].ohm;
    }
    return ohm;
}

/******************************************************************************/
 int __temp_get_from_ohm (void *p_cookie)
{
    struct awbl_ntc_therm_dev        *p_therm_dev = NULL;
    struct awbl_ntc_therm_par        *par         = NULL;
    const struct awbl_temp_ohm_match *p_lst       = NULL;
    uint8_t pos_h = 0;                   /* 数组高位下标 */
    uint8_t pos_l = 0;                   /* 数组低位下标 */
    uint8_t pos_m = 0;                   /* 运算中间下标 */
    int     ohm   = 0;
    int     temp  = 0;

    p_therm_dev = SERV_GET_THERM_DEV((struct awbl_temp_service *)p_cookie);
    par = SERV_GET_NTC_THERM_PAR((struct awbl_temp_service *)p_cookie);
    p_lst = par->p_tempohm_lst;
    pos_h = par->lst_count - 1; 
    
    ohm = __ohm_get_from_volt(p_therm_dev);

    /* 二分查表法 */
    while ((pos_h - pos_l) != 1) {
        pos_m = pos_l + ((pos_h - pos_l) >> 1);        
         
        if (p_lst[pos_m].ohm < ohm) {
            pos_h = pos_m;
        } else if (p_lst[pos_m].ohm > ohm) {
            pos_l = pos_m;
        } else {
            temp = p_lst[pos_m].temp_c;
            break;
        }
        
        if ((pos_h - pos_l) == 1) {
            temp = ((p_lst[pos_l].ohm - ohm) * 1000) /    /* 计算小数位 */
                   (p_lst[pos_l].ohm - p_lst[pos_h].ohm);
            temp += p_lst[pos_l].temp_c * 1000;           /* 放大1000倍 */
            break;
        }
    }

    return temp;
}

/******************************************************************************/

/* adc service functions (must defined as aw_const) */
aw_local struct awbl_temp_servfuncs __g_temp_servfuncs = {
    __temp_get_from_ohm
};

/** \brief instance initializing stage 1 */
aw_local void __ntc_therm_inst_init (struct awbl_dev *p_dev)
{
    struct awbl_ntc_therm_dev   *P_temp_dev  = DEV_TO_THERM_DEV(p_dev);
    struct awbl_ntc_therm_par   *p_temp_par  = DEV_GET_NTC_THERM_PAR(p_dev);

    /* 平台相关初始化 */
    p_temp_par->pfunc_plfm_init();

    P_temp_dev->temp_serv.id = p_temp_par->id;
    P_temp_dev->temp_serv.p_servfuncs = &__g_temp_servfuncs;

    /* 添加服务至列表 */
    awbl_temp_serv_add(&P_temp_dev->temp_serv);
}

/******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_ntc_therm = {
    __ntc_therm_inst_init,           /* devInstanceInit */
    NULL,                            /* devInstanceInit2 */
    NULL                             /* devConnect */
};

/** \brief driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_ntc_therm = {
    AWBL_VER_1,                         /* awbl_ver */
    AWBL_BUSID_PLB,                     /* bus_id */
    NTC_THERMISTOR_NAME,                /* p_drvname */
    &__g_awbl_drvfuncs_ntc_therm,       /* p_busfuncs */
    NULL,                               /* p_methods */
    NULL                                /* pfunc_drv_probe */
};

/** \brief register ntc-thermistor driver */
void awbl_ntc_therm_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_ntc_therm);
}

/* end of file */
