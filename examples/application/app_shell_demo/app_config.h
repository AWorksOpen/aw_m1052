/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_demo_config.h"

#define __BOARD_BASE   0
#define __BOARD_MIFARE 1
#define __BOARD_ZIGBEE 2
#define __BOARD_WIFI   3
#define __BOARD_LORA   4


#define APP_BOARD   __BOARD_WIFI

#define APP_VER     0x0100

/** \brief sdk°üµÄ°æ±¾  */
#define APP_VERSION

#define APP_LED
#define APP_SYS_RST
#define APP_BUZZER
#define APP_NVRAM
#define APP_RTC
#define APP_DISK
#define APP_NET
#define APP_LCD
#define APP_NAND
//#define APP_FTPD

#if (APP_BOARD == __BOARD_MIFARE)
#define APP_MIFARE
#endif

#if (APP_BOARD == __BOARD_WIFI)
#define APP_WIFI
#endif

#ifdef APP_FTPD
#ifndef AW_COM_FTPD
#error "app_ftpd.c file is dependent on AW_COM_FTPD,please open AW_COM_FTPD in aw_prj_params.h file."
#endif
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* __APP_CONFIG_H */

/* end of file */

