/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief 定义iMX RT1010 IOMUXC相关的寄存器结构体
 *
 * \internal
 * \par modification history:
 * - 1.00 18-9-25  mex, first implementation
 * \endinternal
 */

#ifndef __IMX1010_IOMUXC_REGS_H
#define __IMX1010_IOMUXC_REGS_H

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

} hw_iomuxc_gpr_regs_t;


typedef struct {

    volatile uint32_t SNVS_SW_MUX_CTL_PAD_PMIC_ON_REQ;      /* offset 0x00 */
    volatile uint32_t SNVS_SW_PAD_CTL_PAD_TEST_MODE;        /* offset 0x04 */
    volatile uint32_t SNVS_SW_PAD_CTL_PAD_POR_B;            /* offset 0x08 */
    volatile uint32_t SNVS_SW_PAD_CTL_PAD_ONOFF;            /* offset 0x0C */
    volatile uint32_t SNVS_SW_PAD_CTL_PAD_PMIC_ON_REQ;      /* offset 0x10 */
} hw_iomuxc_snvs_regs_t;



typedef struct {

    volatile uint32_t reserved[4];
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_14;        /* offset 0x0010 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_13;        /* offset 0x0014 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_12;        /* offset 0x0018 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_11;        /* offset 0x001c */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_10;        /* offset 0x0020 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_09;        /* offset 0x0024 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_08;        /* offset 0x0028 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_07;        /* offset 0x002c */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_06;        /* offset 0x0030 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_05;        /* offset 0x0034 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_04;        /* offset 0x0038 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_03;        /* offset 0x003c */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_02;        /* offset 0x0040 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_01;        /* offset 0x0044 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_AD_00;        /* offset 0x0048 */

    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_14;        /* offset 0x004c */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_13;        /* offset 0x0050 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_12;        /* offset 0x0054 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_11;        /* offset 0x0058 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_10;        /* offset 0x005c */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_09;        /* offset 0x0060 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_08;        /* offset 0x0064 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_07;        /* offset 0x0068 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_06;        /* offset 0x006c */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_05;        /* offset 0x0070 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_04;        /* offset 0x0074 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_03;        /* offset 0x0078 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_02;        /* offset 0x007c */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_01;        /* offset 0x0080 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_SD_00;        /* offset 0x0084 */

    volatile uint32_t SW_MUX_CTL_PAD_GPIO_13;           /* offset 0x0088 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_12;           /* offset 0x008c */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_11;           /* offset 0x0090 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_10;           /* offset 0x0094 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_09;           /* offset 0x0098 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_08;           /* offset 0x009c */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_07;           /* offset 0x00A0 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_06;           /* offset 0x00A4 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_05;           /* offset 0x00A8 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_04;           /* offset 0x00AC */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_03;           /* offset 0x00B0 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_02;           /* offset 0x00B4 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_01;           /* offset 0x00B8 */
    volatile uint32_t SW_MUX_CTL_PAD_GPIO_00;           /* offset 0x00BC */

    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_14;        /* offset 0x00C0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_13;        /* offset 0x00C4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_12;        /* offset 0x00C8 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_11;        /* offset 0x00CC */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_10;        /* offset 0x00D0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_09;        /* offset 0x00D4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_08;        /* offset 0x00D8 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_07;        /* offset 0x00DC */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_06;        /* offset 0x00E0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_05;        /* offset 0x00E4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_04;        /* offset 0x00E8 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_03;        /* offset 0x00EC */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_02;        /* offset 0x00F0 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_01;        /* offset 0x00F4 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_AD_00;        /* offset 0x00F8 */

    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_14;        /* offset 0x00FC */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_13;        /* offset 0x0100 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_12;        /* offset 0x0104 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_11;        /* offset 0x0108 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_10;        /* offset 0x010C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_09;        /* offset 0x0110 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_08;        /* offset 0x0114 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_07;        /* offset 0x0118 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_06;        /* offset 0x011C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_05;        /* offset 0x0120 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_04;        /* offset 0x0124 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_03;        /* offset 0x0128 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_02;        /* offset 0x012C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_01;        /* offset 0x0130 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_SD_00;        /* offset 0x0134 */

    volatile uint32_t SW_PAD_CTL_PAD_GPIO_13;           /* offset 0x0138 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_12;           /* offset 0x013C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_11;           /* offset 0x0140 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_10;           /* offset 0x0144 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_09;           /* offset 0x0148 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_08;           /* offset 0x014C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_07;           /* offset 0x0150 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_06;           /* offset 0x0154 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_05;           /* offset 0x0158 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_04;           /* offset 0x015C */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_03;           /* offset 0x0160 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_02;           /* offset 0x0164 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_01;           /* offset 0x0168 */
    volatile uint32_t SW_PAD_CTL_PAD_GPIO_00;           /* offset 0x016C */

    volatile uint32_t USB_OTG_ID_SELECT_INPUT;          /* offset 0x0170 */

    volatile uint32_t FLEXPWM1_PWMA_SELECT_INPUT_0;     /* offset 0x0174 */
    volatile uint32_t FLEXPWM1_PWMA_SELECT_INPUT_1;     /* offset 0x0178 */
    volatile uint32_t FLEXPWM1_PWMA_SELECT_INPUT_2;     /* offset 0x017C */
    volatile uint32_t FLEXPWM1_PWMA_SELECT_INPUT_3;     /* offset 0x0180 */
    volatile uint32_t FLEXPWM1_PWMB_SELECT_INPUT_0;     /* offset 0x0184 */
    volatile uint32_t FLEXPWM1_PWMB_SELECT_INPUT_1;     /* offset 0x0188 */
    volatile uint32_t FLEXPWM1_PWMB_SELECT_INPUT_2;     /* offset 0x018C */
    volatile uint32_t FLEXPWM1_PWMB_SELECT_INPUT_3;     /* offset 0x0190 */

    volatile uint32_t FLEXSPI_DQS_FA_SELECT_INPUT;      /* offset 0x0194 */
    volatile uint32_t FLEXSPI_DQS_FB_SELECT_INPUT;      /* offset 0x0198 */

    volatile uint32_t KPP_COL_SELECT_INPUT_0;           /* offset 0x019C */
    volatile uint32_t KPP_COL_SELECT_INPUT_1;           /* offset 0x01A0 */
    volatile uint32_t KPP_COL_SELECT_INPUT_2;           /* offset 0x01A4 */
    volatile uint32_t KPP_COL_SELECT_INPUT_3;           /* offset 0x01A8 */
    volatile uint32_t KPP_ROW_SELECT_INPUT_0;           /* offset 0x01AC */
    volatile uint32_t KPP_ROW_SELECT_INPUT_1;           /* offset 0x01B0 */
    volatile uint32_t KPP_ROW_SELECT_INPUT_2;           /* offset 0x01B4 */
    volatile uint32_t KPP_ROW_SELECT_INPUT_3;           /* offset 0x01B8 */

    volatile uint32_t LPI2C1_HREQ_SELECT_INPUT;         /* offset 0x01BC */
    volatile uint32_t LPI2C1_SCL_SELECT_INPUT;          /* offset 0x01C0 */
    volatile uint32_t LPI2C1_SDA_SELECT_INPUT;          /* offset 0x01C4 */
    volatile uint32_t LPI2C2_SCL_SELECT_INPUT;          /* offset 0x01C8 */
    volatile uint32_t LPI2C2_SDA_SELECT_INPUT;          /* offset 0x01CC */

    volatile uint32_t LPSPI1_PCS_SELECT_INPUT_0;        /* offset 0x01D0 */
    volatile uint32_t LPSPI1_SCK_SELECT_INPUT;          /* offset 0x01D4 */
    volatile uint32_t LPSPI1_SDI_SELECT_INPUT;          /* offset 0x01D8 */
    volatile uint32_t LPSPI1_SDO_SELECT_INPUT;          /* offset 0x01DC */
    volatile uint32_t LPSPI2_PCS_SELECT_INPUT_0;        /* offset 0x01E0 */
    volatile uint32_t LPSPI2_SCK_SELECT_INPUT;          /* offset 0x01E4 */
    volatile uint32_t LPSPI2_SDI_SELECT_INPUT;          /* offset 0x01E8 */
    volatile uint32_t LPSPI2_SDO_SELECT_INPUT;          /* offset 0x01EC */

    volatile uint32_t LPUART1_RXD_SELECT_INPUT;         /* offset 0x01F0 */
    volatile uint32_t LPUART1_TXD_SELECT_INPUT;         /* offset 0x01F4 */
    volatile uint32_t LPUART2_RXD_SELECT_INPUT;         /* offset 0x01F8 */
    volatile uint32_t LPUART2_TXD_SELECT_INPUT;         /* offset 0x01FC */
    volatile uint32_t LPUART3_RXD_SELECT_INPUT;         /* offset 0x0200 */
    volatile uint32_t LPUART3_TXD_SELECT_INPUT;         /* offset 0x0204 */
    volatile uint32_t LPUART4_RXD_SELECT_INPUT;         /* offset 0x0208 */
    volatile uint32_t LPUART4_TXD_SELECT_INPUT;         /* offset 0x020C */

    volatile uint32_t NMI_GLUE_NMI_SELECT_INPUT;        /* offset 0x0210 */
    volatile uint32_t SPDIF_IN1_SELECT_INPUT;           /* offset 0x0214 */
    volatile uint32_t SPDIF_TX_CLK2_SELECT_INPUT;       /* offset 0x0218 */
    volatile uint32_t USB_OTG_OC_SELECT_INPUT;          /* offset 0x021C */
    volatile uint32_t XEV_GLUE_RXEV_SELECT_INPUT;       /* offset 0x0220 */

} hw_iomuxc_regs_t;

#pragma pack(pop)



#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX1010_IOMUXC_REGS_H */

/* end of file */
