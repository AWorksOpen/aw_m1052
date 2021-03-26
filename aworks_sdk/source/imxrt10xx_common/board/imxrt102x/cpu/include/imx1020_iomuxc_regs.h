/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief 定义iMX RT1020 IOMUXC相关的寄存器结构体
 *
 * \internal
 * \par modification history:
 * - 1.00 18-9-25  mex, first implementation
 * \endinternal
 */

#ifndef __IMX1020_IOMUXC_REGS_H
#define __IMX1020_IOMUXC_REGS_H

#include "aworks.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#pragma pack(push)
#pragma pack(1)
typedef struct {

    volatile uint32_t IOMUXC_GPR_GPR0;
    volatile uint32_t IOMUXC_GPR_GPR1;
    volatile uint32_t IOMUXC_GPR_GPR2;
    volatile uint32_t IOMUXC_GPR_GPR3;
    volatile uint32_t IOMUXC_GPR_GPR4;
    volatile uint32_t IOMUXC_GPR_GPR5;
    volatile uint32_t IOMUXC_GPR_GPR6;
    volatile uint32_t IOMUXC_GPR_GPR7;
    volatile uint32_t IOMUXC_GPR_GPR8;
    volatile uint32_t IOMUXC_GPR_GPR9;
    volatile uint32_t IOMUXC_GPR_GPR10;
    volatile uint32_t IOMUXC_GPR_GPR11;
    volatile uint32_t IOMUXC_GPR_GPR12;
    volatile uint32_t IOMUXC_GPR_GPR13;
    volatile uint32_t IOMUXC_GPR_GPR14;
    volatile uint32_t IOMUXC_GPR_GPR15;
    volatile uint32_t IOMUXC_GPR_GPR16;
    volatile uint32_t IOMUXC_GPR_GPR17;
    volatile uint32_t IOMUXC_GPR_GPR18;
    volatile uint32_t IOMUXC_GPR_GPR19;
    volatile uint32_t IOMUXC_GPR_GPR20;
    volatile uint32_t IOMUXC_GPR_GPR21;
    volatile uint32_t IOMUXC_GPR_GPR22;
    volatile uint32_t IOMUXC_GPR_GPR23;
    volatile uint32_t IOMUXC_GPR_GPR24;
    volatile uint32_t IOMUXC_GPR_GPR25;
    volatile uint32_t IOMUXC_GPR_GPR26;
} hw_iomuxc_gpr_regs_t;

typedef struct {

    volatile uint32_t reserved[5];
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_00;      /* offset 0x0014 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_01;      /* offset 0x0018 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_02;      /* offset 0x001C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_03;      /* offset 0x0020 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_04;      /* offset 0x0024 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_05;      /* offset 0x0028 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_06;      /* offset 0x002C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_07;      /* offset 0x0030 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_08;      /* offset 0x0034 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_09;      /* offset 0x0038 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_10;      /* offset 0x003C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_11;      /* offset 0x0040 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_12;      /* offset 0x0044 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_13;      /* offset 0x0048 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_14;      /* offset 0x004C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_15;      /* offset 0x0050 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_16;      /* offset 0x0054 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_17;      /* offset 0x0058 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_18;      /* offset 0x005C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_19;      /* offset 0x0060 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_20;      /* offset 0x0064 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_21;      /* offset 0x0068 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_22;      /* offset 0x006C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_23;      /* offset 0x0070 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_24;      /* offset 0x0074 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_25;      /* offset 0x0078 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_26;      /* offset 0x007C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_27;      /* offset 0x0080 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_28;      /* offset 0x0084 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_29;      /* offset 0x0088 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_30;      /* offset 0x008C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_31;      /* offset 0x0090 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_32;      /* offset 0x0094 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_33;      /* offset 0x0098 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_34;      /* offset 0x009C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_35;      /* offset 0x00A0 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_36;      /* offset 0x00A4 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_37;      /* offset 0x00A8 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_38;      /* offset 0x00AC */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_39;      /* offset 0x00B0 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_40;      /* offset 0x00B4 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_EMC_41;      /* offset 0x00B8 */

    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_00;    /* offset 0x00BC */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_01;    /* offset 0x00C0 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_02;    /* offset 0x00C4 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_03;    /* offset 0x00C8 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_04;    /* offset 0x00CC */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_05;    /* offset 0x00D0 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_06;    /* offset 0x00D4 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_07;    /* offset 0x00D8 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_08;    /* offset 0x00DC */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_09;    /* offset 0x00E0 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_10;    /* offset 0x00E4 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_11;    /* offset 0x00E8 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_12;    /* offset 0x00EC */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_13;    /* offset 0x00F0 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_14;    /* offset 0x00F4 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B0_15;    /* offset 0x00F8 */

    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_00;    /* offset 0x00FC */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_01;    /* offset 0x0100 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_02;    /* offset 0x0104 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_03;    /* offset 0x0108 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_04;    /* offset 0x010C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_05;    /* offset 0x0110 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_06;    /* offset 0x0114 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_07;    /* offset 0x0118 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_08;    /* offset 0x011C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_09;    /* offset 0x0120 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_10;    /* offset 0x0124 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_11;    /* offset 0x0128 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_12;    /* offset 0x012C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_13;    /* offset 0x0130 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_14;    /* offset 0x0134 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_B1_15;    /* offset 0x0138 */

    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B0_00;   /* offset 0x013C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B0_01;   /* offset 0x0140 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B0_02;   /* offset 0x0144 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B0_03;   /* offset 0x0148 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B0_04;   /* offset 0x014C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B0_05;   /* offset 0x0150 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B0_06;   /* offset 0x0154 */

    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B1_00;   /* offset 0x0158 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B1_01;   /* offset 0x015C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B1_02;   /* offset 0x0160 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B1_03;   /* offset 0x0164 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B1_04;   /* offset 0x0168 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B1_05;   /* offset 0x016C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B1_06;   /* offset 0x0170 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B1_07;   /* offset 0x0174 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B1_08;   /* offset 0x0178 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B1_09;   /* offset 0x017C */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B1_10;   /* offset 0x0180 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_B1_11;   /* offset 0x0184 */

    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_00;     /* offset 0x0188 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_01;     /* offset 0x018C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_02;     /* offset 0x0190 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_03;     /* offset 0x0194 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_04;     /* offset 0x0198 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_05;     /* offset 0x019C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_06;     /* offset 0x01A0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_07;     /* offset 0x01A4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_08;     /* offset 0x01A8 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_09;     /* offset 0x01AC */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_10;     /* offset 0x01B0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_11;     /* offset 0x01B4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_12;     /* offset 0x01B8 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_13;     /* offset 0x01BC */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_14;     /* offset 0x01C0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_15;     /* offset 0x01C4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_16;     /* offset 0x01C8 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_17;     /* offset 0x01CC */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_18;     /* offset 0x01D0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_19;     /* offset 0x01D4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_20;     /* offset 0x01D8 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_21;     /* offset 0x01DC */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_22;     /* offset 0x01E0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_23;     /* offset 0x01E4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_24;     /* offset 0x01E8 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_25;     /* offset 0x01EC */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_26;     /* offset 0x01F0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_27;     /* offset 0x01F4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_28;     /* offset 0x01F8 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_29;     /* offset 0x01FC */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_30;     /* offset 0x0200 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_31;     /* offset 0x0204 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_32;     /* offset 0x0208 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_33;     /* offset 0x020C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_34;     /* offset 0x0210 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_35;     /* offset 0x0214 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_36;     /* offset 0x0218 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_37;     /* offset 0x021C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_38;     /* offset 0x0220 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_39;     /* offset 0x0224 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_40;     /* offset 0x0228 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_EMC_41;     /* offset 0x022C */

    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_00;   /* offset 0x0230 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_01;   /* offset 0x0234 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_02;   /* offset 0x0238 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_03;   /* offset 0x023C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_04;   /* offset 0x0240 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_05;   /* offset 0x0244 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_06;   /* offset 0x0248 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_07;   /* offset 0x024C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_08;   /* offset 0x0250 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_09;   /* offset 0x0254 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_10;   /* offset 0x0258 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_11;   /* offset 0x025C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_12;   /* offset 0x0260 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_13;   /* offset 0x0264 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_14;   /* offset 0x0268 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B0_15;   /* offset 0x026C */

    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_00;   /* offset 0x0270 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_01;   /* offset 0x0274 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_02;   /* offset 0x0278 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_03;   /* offset 0x027C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_04;   /* offset 0x0280 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_05;   /* offset 0x0284 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_06;   /* offset 0x0288 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_07;   /* offset 0x028C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_08;   /* offset 0x0290 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_09;   /* offset 0x0294 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_10;   /* offset 0x0298 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_11;   /* offset 0x029C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_12;   /* offset 0x02A0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_13;   /* offset 0x02A4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_14;   /* offset 0x02A8 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_B1_15;   /* offset 0x02AC */

    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B0_00;   /* offset 0x02B0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B0_01;   /* offset 0x02B4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B0_02;   /* offset 0x02B8 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B0_03;   /* offset 0x02BC */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B0_04;   /* offset 0x02C0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B0_05;   /* offset 0x02C4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B0_06;   /* offset 0x02C8 */

    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B1_00;   /* offset 0x02CC */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B1_01;   /* offset 0x02D0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B1_02;   /* offset 0x02D4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B1_03;   /* offset 0x02D8 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B1_04;   /* offset 0x02DC */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B1_05;   /* offset 0x02E0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B1_06;   /* offset 0x02E4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B1_07;   /* offset 0x02E8 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B1_08;   /* offset 0x02EC */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B1_09;   /* offset 0x02F0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B1_10;   /* offset 0x02F4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_B1_11;   /* offset 0x02F8 */

    volatile uint32_t ANATOP_USB_OTG_ID_SELECT_INPUT; /* offset 0x02FC */
    volatile uint32_t CCM_PMIC_READY_SELECT_INPUT;    /* offset 0x0300 */
    volatile uint32_t ENET_RMII_SELECT_INPUT;         /* offset 0x0304 */
    volatile uint32_t ENET_MDIO_SELECT_INPUT;         /* offset 0x0308 */
    volatile uint32_t ENET_RX_DATA0_SELECT_INPUT;     /* offset 0x030C */
    volatile uint32_t ENET_RX_DATA1_SELECT_INPUT;     /* offset 0x0310 */
    volatile uint32_t ENET_RX_EN_SELECT_INPUT;        /* offset 0x0314 */
    volatile uint32_t ENET_RX_ERR_SELECT_INPUT;       /* offset 0x0318 */
    volatile uint32_t ENET_TX_CLK_SELECT_INPUT;       /* offset 0x031C */
    volatile uint32_t FLEXCAN1_RX_SELECT_INPUT;       /* offset 0x0320 */
    volatile uint32_t FLEXCAN2_RX_SELECT_INPUT;       /* offset 0x0324 */

    volatile uint32_t FLEXPWM1_PWMA0_SELECT_INPUT;    /* offset 0x0328 */
    volatile uint32_t FLEXPWM1_PWMA1_SELECT_INPUT;    /* offset 0x032C */
    volatile uint32_t FLEXPWM1_PWMA2_SELECT_INPUT;    /* offset 0x0330 */
    volatile uint32_t FLEXPWM1_PWMA3_SELECT_INPUT;    /* offset 0x0334 */
    volatile uint32_t FLEXPWM1_PWMB0_SELECT_INPUT;    /* offset 0x0338 */
    volatile uint32_t FLEXPWM1_PWMB1_SELECT_INPUT;    /* offset 0x033C */
    volatile uint32_t FLEXPWM1_PWMB2_SELECT_INPUT;    /* offset 0x0340 */
    volatile uint32_t FLEXPWM1_PWMB3_SELECT_INPUT;    /* offset 0x0344 */
    volatile uint32_t FLEXPWM2_PWMA0_SELECT_INPUT;    /* offset 0x0348 */
    volatile uint32_t FLEXPWM2_PWMA1_SELECT_INPUT;    /* offset 0x034C */
    volatile uint32_t FLEXPWM2_PWMA2_SELECT_INPUT;    /* offset 0x0350 */
    volatile uint32_t FLEXPWM2_PWMA3_SELECT_INPUT;    /* offset 0x0354 */
    volatile uint32_t FLEXPWM2_PWMB0_SELECT_INPUT;    /* offset 0x0358 */
    volatile uint32_t FLEXPWM2_PWMB1_SELECT_INPUT;    /* offset 0x035C */
    volatile uint32_t FLEXPWM2_PWMB2_SELECT_INPUT;    /* offset 0x0360 */
    volatile uint32_t FLEXPWM2_PWMB3_SELECT_INPUT;    /* offset 0x0364 */
    volatile uint32_t FLEXSPI_A_DATA0_SELECT_INPUT;   /* offset 0x0368 */
    volatile uint32_t FLEXSPI_A_DATA1_SELECT_INPUT;   /* offset 0x036C */
    volatile uint32_t FLEXSPI_A_DATA2_SELECT_INPUT;   /* offset 0x0370 */
    volatile uint32_t FLEXSPI_A_DATA3_SELECT_INPUT;   /* offset 0x0374 */
    volatile uint32_t FLEXSPI_A_SCLK_SELECT_INPUT;    /* offset 0x0378 */

    volatile uint32_t LPI2C1_SCL_SELECT_INPUT;        /* offset 0x037C */
    volatile uint32_t LPI2C1_SDA_SELECT_INPUT;        /* offset 0x0380 */
    volatile uint32_t LPI2C2_SCL_SELECT_INPUT;        /* offset 0x0384 */
    volatile uint32_t LPI2C2_SDA_SELECT_INPUT;        /* offset 0x0388 */
    volatile uint32_t LPI2C3_SCL_SELECT_INPUT;        /* offset 0x038C */
    volatile uint32_t LPI2C3_SDA_SELECT_INPUT;        /* offset 0x0390 */
    volatile uint32_t LPI2C4_SCL_SELECT_INPUT;        /* offset 0x0394 */
    volatile uint32_t LPI2C4_SDA_SELECT_INPUT;        /* offset 0x0398 */

    volatile uint32_t LPSPI1_PCS0_SELECT_INPUT;       /* offset 0x039C */
    volatile uint32_t LPSPI1_SCK_SELECT_INPUT;        /* offset 0x03A0 */
    volatile uint32_t LPSPI1_SDI_SELECT_INPUT;        /* offset 0x03A4 */
    volatile uint32_t LPSPI1_SDO_SELECT_INPUT;        /* offset 0x03A8 */
    volatile uint32_t LPSPI2_PCS0_SELECT_INPUT;       /* offset 0x03AC */
    volatile uint32_t LPSPI2_SCK_SELECT_INPUT;        /* offset 0x03B0 */
    volatile uint32_t LPSPI2_SDI_SELECT_INPUT;        /* offset 0x03B4 */
    volatile uint32_t LPSPI2_SDO_SELECT_INPUT;        /* offset 0x03B8 */
    volatile uint32_t LPSPI4_PCS0_SELECT_INPUT;       /* offset 0x03BC */
    volatile uint32_t LPSPI4_SCK_SELECT_INPUT;        /* offset 0x03C0 */
    volatile uint32_t LPSPI4_SDI_SELECT_INPUT;        /* offset 0x03C4 */
    volatile uint32_t LPSPI4_SDO_SELECT_INPUT;        /* offset 0x03C8 */

    volatile uint32_t LPUART2_CTS_B_SELECT_INPUT;     /* offset 0x03CC */
    volatile uint32_t LPUART2_RX_SELECT_INPUT;        /* offset 0x03D0 */
    volatile uint32_t LPUART2_TX_SELECT_INPUT;        /* offset 0x03D4 */
    volatile uint32_t LPUART3_RX_SELECT_INPUT;        /* offset 0x03D8 */
    volatile uint32_t LPUART3_TX_SELECT_INPUT;        /* offset 0x03DC */
    volatile uint32_t LPUART4_CTS_B_SELECT_INPUT;     /* offset 0x03E0 */
    volatile uint32_t LPUART4_RX_SELECT_INPUT;        /* offset 0x03E4 */
    volatile uint32_t LPUART4_TX_SELECT_INPUT;        /* offset 0x03E8 */
    volatile uint32_t LPUART5_RX_SELECT_INPUT;        /* offset 0x03EC */
    volatile uint32_t LPUART5_TX_SELECT_INPUT;        /* offset 0x03F0 */
    volatile uint32_t LPUART6_RX_SELECT_INPUT;        /* offset 0x03F4 */
    volatile uint32_t LPUART6_TX_SELECT_INPUT;        /* offset 0x03F8 */
    volatile uint32_t LPUART7_RX_SELECT_INPUT;        /* offset 0x03FC */
    volatile uint32_t LPUART7_TX_SELECT_INPUT;        /* offset 0x0400 */
    volatile uint32_t LPUART8_RX_SELECT_INPUT;        /* offset 0x0404 */
    volatile uint32_t LPUART8_TX_SELECT_INPUT;        /* offset 0x0408 */

    volatile uint32_t NMI_SELECT_INPUT;                   /* offset 0x040C */
    volatile uint32_t QTIMER1_TIMER0_INPUT_SELECT_INPUT;  /* offset 0x0410 */
    volatile uint32_t QTIMER1_TIMER1_INPUT_SELECT_INPUT;  /* offset 0x0414 */
    volatile uint32_t QTIMER1_TIMER2_INPUT_SELECT_INPUT;  /* offset 0x0418 */
    volatile uint32_t QTIMER1_TIMER3_INPUT_SELECT_INPUT;  /* offset 0x041C */
    volatile uint32_t QTIMER2_TIMER0_INPUT_SELECT_INPUT;  /* offset 0x0420 */
    volatile uint32_t QTIMER2_TIMER1_INPUT_SELECT_INPUT;  /* offset 0x0424 */
    volatile uint32_t QTIMER2_TIMER2_INPUT_SELECT_INPUT;  /* offset 0x0428 */
    volatile uint32_t QTIMER2_TIMER3_INPUT_SELECT_INPUT;  /* offset 0x042C */

    volatile uint32_t SAI1_MCLK_SELECT_INPUT;         /* offset 0x0430 */
    volatile uint32_t SAI1_RX_BCLK_SELECT_INPUT;      /* offset 0x0434 */
    volatile uint32_t SAI1_RX_DATA0_SELECT_INPUT;     /* offset 0x0438 */
    volatile uint32_t SAI1_RX_DATA1_SELECT_INPUT;     /* offset 0x043C */
    volatile uint32_t SAI1_RX_DATA2_SELECT_INPUT;     /* offset 0x0440 */
    volatile uint32_t SAI1_RX_DATA3_SELECT_INPUT;     /* offset 0x0444 */
    volatile uint32_t SAI1_RX_SYNC_SELECT_INPUT;      /* offset 0x0448 */
    volatile uint32_t SAI1_TX_BCLK_SELECT_INPUT;      /* offset 0x044C */
    volatile uint32_t SAI1_TX_SYNC_SELECT_INPUT;      /* offset 0x0450 */
    volatile uint32_t SAI2_MCLK_SELECT_INPUT;         /* offset 0x0454 */
    volatile uint32_t SAI2_RX_BCLK_SELECT_INPUT;      /* offset 0x0458 */
    volatile uint32_t SAI2_RX_DATA0_SELECT_INPUT;     /* offset 0x045C */
    volatile uint32_t SAI2_RX_SYNC_SELECT_INPUT;      /* offset 0x0460 */
    volatile uint32_t SAI2_TX_BCLK_SELECT_INPUT;      /* offset 0x0464 */
    volatile uint32_t SAI2_TX_SYNC_SELECT_INPUT;      /* offset 0x0468 */
    volatile uint32_t SAI3_MCLK_SELECT_INPUT;         /* offset 0x046C */
    volatile uint32_t SAI3_RX_BCLK_SELECT_INPUT;      /* offset 0x0470 */
    volatile uint32_t SAI3_RX_DATA0_SELECT_INPUT;     /* offset 0x0474 */
    volatile uint32_t SAI3_RX_SYNC_SELECT_INPUT;      /* offset 0x0478 */
    volatile uint32_t SAI3_TX_BCLK_SELECT_INPUT;      /* offset 0x047C */
    volatile uint32_t SAI3_TX_SYNC_SELECT_INPUT;      /* offset 0x0480 */

    volatile uint32_t SEMC_READY_SELECT_INPUT;        /* offset 0x0484 */
    volatile uint32_t SPDIF_IN_SELECT_INPUT;          /* offset 0x0488 */
    volatile uint32_t USB_OTG_OC_SELECT_INPUT;        /* offset 0x048C */
    volatile uint32_t USDHC1_CD_B_SELECT_INPUT;       /* offset 0x0490 */
    volatile uint32_t USDHC1_WP_SELECT_INPUT;         /* offset 0x0494 */
    volatile uint32_t USDHC2_CD_B_SELECT_INPUT;       /* offset 0x0498 */
    volatile uint32_t USDHC2_WP_SELECT_INPUT;         /* offset 0x049C */
    volatile uint32_t XBAR1_IN14_SELECT_INPUT;        /* offset 0x04A0 */
    volatile uint32_t XBAR1_IN15_SELECT_INPUT;        /* offset 0x04A4 */
    volatile uint32_t XBAR1_IN16_SELECT_INPUT;        /* offset 0x04A8 */
    volatile uint32_t XBAR1_IN17_SELECT_INPUT;        /* offset 0x04AC */
    volatile uint32_t XBAR1_IN10_SELECT_INPUT;        /* offset 0x04B0 */
    volatile uint32_t XBAR1_IN12_SELECT_INPUT;        /* offset 0x04B4 */
    volatile uint32_t XBAR1_IN13_SELECT_INPUT;        /* offset 0x04B8 */
    volatile uint32_t XBAR1_IN18_SELECT_INPUT;        /* offset 0x04BC */
    volatile uint32_t XBAR1_IN19_SELECT_INPUT;        /* offset 0x04C0 */

} hw_iomuxc_regs_t;

typedef struct {

    volatile uint32_t SNVS_SW_MUX_CTL_PAD_WAKEUP;          /* offset 0x00 */
    volatile uint32_t SNVS_SW_MUX_CTL_PAD_PMIC_ON_REQ;     /* offset 0x04 */
    volatile uint32_t SNVS_SW_MUX_CTL_PAD_PMIC_STBY_REQ;   /* offset 0x08 */
    volatile uint32_t SNVS_SW_PAD_CTL_PAD_TEST_MODE;       /* offset 0x0C */
    volatile uint32_t SNVS_SW_PAD_CTL_PAD_POR_B;           /* offset 0x10 */
    volatile uint32_t SNVS_SW_PAD_CTL_PAD_ONOFF;           /* offset 0x14 */
    volatile uint32_t SNVS_SW_PAD_CTL_PAD_WAKEUP;          /* offset 0x18 */
    volatile uint32_t SNVS_SW_PAD_CTL_PAD_PMIC_ON_REQ;     /* offset 0x1C */
    volatile uint32_t SNVS_SW_PAD_CTL_PAD_PMIC_STBY_REQ;   /* offset 0x20 */

} hw_iomuxc_snvs_regs_t;

#pragma pack(pop)



#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX1020_IOMUXC_REGS_H */

/* end of file */
