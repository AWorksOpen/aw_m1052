/*
 * awbl_adc_key.h
 *
 *  Created on: 2020年9月28日
 *      Author: zengqingyuhang
 */

#ifndef _AWBL_ADC_KEY_H_
#define _CODE_AWBL_ADC_KEY_H_


#include "aw_common.h"
#include "awbus_lite.h"
#include "awbl_input.h"
#include "aw_types.h"

#define ADC_KEY_DRV_NAME            "adc_key_drv"


struct  adc_key_cfg {

    uint32_t        voltage_mv;                 /* 按键按下去时的电压值 */
    int             key_code;                   /* 按键编码 */
    char           *key_name;                   /* 按键名称 */
    aw_bool_t       continuous;                 /* 是否支持连按 1：支持；0：不支持 */
    aw_bool_t       pressed;                    /* 按键初始状态 */
    uint32_t        cnt;                        /* 按键初始计数值，用于判断连按模式进入条件 */
};


struct awbl_adc_key_devinfo {

    uint32_t            adc_chan;               /* ad按键使用的转换通道号 */
    struct adc_key_cfg *key_cfg_tab;            /* 电压值与按键的对照表 */
    uint32_t            key_nums;               /* 按键个数 */
    uint32_t            scan_period_ms;         /* 按键扫描周期 */
    uint32_t            continue_cnt;           /* 进入连按模式阈值 */
    float               offset;                 /* 电压值上下浮动范围，例如设置为0.1，
                                                                                                            则正负0.1范围的电压值都是有效的按键触发事件 */
    aw_bool_t           printf_org_val;         /* 是否打印ad转换的原始值 */
};


struct awbl_adc_key_dev {

    struct awbl_dev     pdev;
    uint32_t            key_status;
};


void awbl_adc_key_drv_register (void);



#endif /* _AWBL_ADC_KEY_H_ */
