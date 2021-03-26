/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#include "apollo.h"
#include "host/awbl_usbh.h"
#include "aw_prj_params.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define DEV_SPEC_END  {                      \
                            .vid = 0,        \
                            .pid = 0,        \
                            .config_num = 0, \
                       }

/* 设置设备初始化配置*/
const struct awbl_usbh_dev_spec_config_info dev_spec_config_tab[] = {
#ifdef AW_DRV_USBH_CDC_ECM
/* 华为4G模块，设置4G模块模式(PPP或ECM)*/
{
        .vid = 0x12D1,
        .pid = 0x15c1,
        .config_num = 2,
},
#endif
    DEV_SPEC_END,
};


