/*******************************************************************************
*                                 AnyWhere
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      anywhere.support@zlg.cn
*******************************************************************************/

/*******************************************************************************
  iMX RT1050 ����OTP  ������Ϣ
*******************************************************************************/

#ifndef  __AWBL_HWCONF_IMX1050_OCOTP_H
#define  __AWBL_HWCONF_IMX1050_OCOTP_H

#include "driver/otp/awbl_imx1050_ocotp.h"

#ifdef AW_DEV_IMX1050_OCOTP

/**
 * \brief iMX RT1050 �ڲ� OCOTP �洢�������б�
 */
aw_local aw_const struct awbl_nvram_segment __g_imx1050_ocotp_seglst[] = {
    /* �洢������, ��Ԫ��, ��ʼ��ַ������ */
    {"otp_lock",      0,   0x00,        4},        /* Lock controls */
    {"otp_osp",       0,   0x04,        8},        /* uids */
    {"otp_cfg_info",  1,  (0x02 * 4),  (4 * 7)},   /* Configuration and Manufacturing Info */
    {"otp_mem_info",  2,  (0x08 * 4),  (4 * 5)},   /* Memory Related Info */
    {"otp_ana_info",  3,  (0x0D * 4),  (4 * 3)},   /* Analog Info */
    {"otp_zy_data",   0,  (0x18 * 4),  (4 * 8)},
    {"otp_zy_mac",    0,  (0x22 * 4),  (4 * 2)},   /* MAC */
    {"otp_all",       8,     0x00,      0xBC  },
};


/* iMX RT1050 �ڲ� OCOTP �豸��Ϣ */
aw_local aw_const struct awbl_imx1050_ocotp_devinfo __g_imx1050_ocotp_devinfo = {

    &__g_imx1050_ocotp_seglst[0],            /* ����ʧ�Դ洢�������б� */
    AW_NELEMENTS(__g_imx1050_ocotp_seglst),  /* ����ʧ�Դ洢�������б���Ŀ�� */
    IMX1050_OCOTP_BASE,                      /* �Ĵ�����ַ */
    IMX1050_CLK_CG_OCOTP                       /* ����ʱ��ID */
};


/* iMX RT1050 �ڲ� OCOTP �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx1050_ocotp_dev __g_imx1050_ocotp_dev;

#define AWBL_HWCONF_IMX1050_OCOTP                    \
    {                                                \
        AWBL_IMX1050_OCOTP_NAME,                     \
        0,                                           \
        AWBL_BUSID_PLB,                              \
        0,                                           \
        (struct awbl_dev *)&__g_imx1050_ocotp_dev,   \
        &__g_imx1050_ocotp_devinfo                   \
    },
#else
#define AWBL_HWCONF_IMX1050_OCOTP
#endif  /* AW_DEV_IMX1050_OCOTP */

#endif /* __AWBL_HWCONF_IMX1050_OCOTP_H */

/* end of file */
