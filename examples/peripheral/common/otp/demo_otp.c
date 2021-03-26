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
 * \brief OTP�洢���������̣���OTP��������ݣ�ע��OTPֻ��д��һ�Σ�
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��AW_DEV_XXX_OCOTP
 *   2. ����otpӲ��������鿴awbl_hwconf_xxx_ocotp.h
 *
 * - ʵ������
 *   1. ���ڴ�ӡ������Ϣ��
 *
 * \par Դ����
 * \snippet demo_otp.c src_otp
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-16, mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_otp OTP����
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
 * \brief OTP�������
 * \return ��
 */
void demo_otp_entry (void)
{
    aw_err_t    ret = AW_OK;
    uint8_t     read_mac[6] = {0};
    int         i = 0;

    /* ��ȡ���� */
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
