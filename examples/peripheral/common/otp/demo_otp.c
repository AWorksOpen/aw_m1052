/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief OTP存储器操作例程（从OTP区域读数据，注意OTP只能写入一次）
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能AW_DEV_XXX_OCOTP
 *   2. 具体otp硬件配置需查看awbl_hwconf_xxx_ocotp.h
 *
 * - 实验现象：
 *   1. 串口打印调试信息。
 *
 * \par 源代码
 * \snippet demo_otp.c src_otp
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-16, mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_otp OTP操作
 * \copydoc demo_otp.c
 */

/** [src_otp] */
#include "aworks.h"
#include "aw_nvram.h"
#include "aw_vdebug.h"
#include "aw_psp_errno.h"
#include "stdio.h"
#include "aw_demo_config.h"

#define OTP_MEM             DE_OTP_MEM
#define OTP_MEM_UNIT        0

/**
 * \brief OTP操作入口
 * \return 无
 */
void demo_otp_entry (void)
{
    aw_err_t    ret = AW_OK;
    uint8_t     read_mac[6] = {0};
    int         i = 0;

    /* 读取数据 */
    ret = aw_nvram_get(OTP_MEM,
                       OTP_MEM_UNIT,
                       (char *)read_mac,
                       0,
                       sizeof(read_mac));
    if (AW_OK != ret) {
        aw_kprintf("aw_nvram_get error.\r\n");
        return ;
    }

    aw_kprintf("read_mac: ");
    for (i = 0; i < 5; i++) {
        aw_kprintf("%02x:", read_mac[i]);
    }
    aw_kprintf("%02x\r\n", read_mac[i]);

    return ;
}

/** [src_otp] */

/* end of file */
