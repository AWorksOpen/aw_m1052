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
 * \brief Freescale i.MX10xx eFlexPWMÍâÉè¼Ä´æÆ÷
 *
 * \internal
 * \par modification history:
 * - 1.00 18-01-09  mex, first implemetation
 * \endinternal
 */

#ifndef __IMX10xx_EFLEX_PWM_REGS_H
#define __IMX10xx_EFLEX_PWM_REGS_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#define   __I     volatile const      /**< \brief Defines 'read only' permissions */
#define   __O     volatile            /**< \brief Defines 'write only' permissions */
#define   __IO    volatile            /**< \brief Defines 'read / write' permissions */

/**
 * \brief  eFlexPWM - Register Layout Typedef
 */
typedef struct {

    struct {                               /* offset: 0x0, array step: 0x60 */
        __I  uint16_t CNT;                             /**< Counter Register, array offset: 0x0, array step: 0x60 */
        __IO uint16_t INIT;                 /**< \brief Initial Count Register, array offset: 0x2, array step: 0x60 */
        __IO uint16_t CTRL2;                /**< \brief Control 2 Register, array offset: 0x4, array step: 0x60 */
        __IO uint16_t CTRL;                 /**< \brief Control Register, array offset: 0x6, array step: 0x60 */
             uint8_t RESERVED_0[2];
        __IO uint16_t VAL0;                 /**< \brief Value Register 0, array offset: 0xA, array step: 0x60 */
        __IO uint16_t FRACVAL1;             /**< \brief Fractional Value Register 1, array offset: 0xC, array step: 0x60 */
        __IO uint16_t VAL1;                 /**< \brief Value Register 1, array offset: 0xE, array step: 0x60 */
        __IO uint16_t FRACVAL2;             /**< \brief Fractional Value Register 2, array offset: 0x10, array step: 0x60 */
        __IO uint16_t VAL2;                 /**< \brief Value Register 2, array offset: 0x12, array step: 0x60 */
        __IO uint16_t FRACVAL3;             /**< \brief Fractional Value Register 3, array offset: 0x14, array step: 0x60 */
        __IO uint16_t VAL3;                 /**< \brief Value Register 3, array offset: 0x16, array step: 0x60 */
        __IO uint16_t FRACVAL4;             /**< \brief Fractional Value Register 4, array offset: 0x18, array step: 0x60 */
        __IO uint16_t VAL4;                 /**< \brief Value Register 4, array offset: 0x1A, array step: 0x60 */
        __IO uint16_t FRACVAL5;             /**< \brief Fractional Value Register 5, array offset: 0x1C, array step: 0x60 */
        __IO uint16_t VAL5;                 /**< \brief Value Register 5, array offset: 0x1E, array step: 0x60 */
        __IO uint16_t FRCTRL;               /**< \brief Fractional Control Register, array offset: 0x20, array step: 0x60 */
        __IO uint16_t OCTRL;                /**< \brief Output Control Register, array offset: 0x22, array step: 0x60 */
        __IO uint16_t STS;                  /**< \brief Status Register, array offset: 0x24, array step: 0x60 */
        __IO uint16_t INTEN;                /**< \brief Interrupt Enable Register, array offset: 0x26, array step: 0x60 */
        __IO uint16_t DMAEN;                /**< \brief DMA Enable Register, array offset: 0x28, array step: 0x60 */
        __IO uint16_t TCTRL;                /**< \brief Output Trigger Control Register, array offset: 0x2A, array step: 0x60 */
        __IO uint16_t DISMAP[2];            /**< \brief Fault Disable Mapping Register 0..Fault Disable Mapping Register 1, array offset: 0x2C, array step: index*0x60, index2*0x2 */
        __IO uint16_t DTCNT0;               /**< \brief Deadtime Count Register 0, array offset: 0x30, array step: 0x60 */
        __IO uint16_t DTCNT1;               /**< \brief Deadtime Count Register 1, array offset: 0x32, array step: 0x60 */
        __IO uint16_t CAPTCTRLA;            /**< \brief Capture Control A Register, array offset: 0x34, array step: 0x60 */
        __IO uint16_t CAPTCOMPA;            /**< \brief Capture Compare A Register, array offset: 0x36, array step: 0x60 */
        __IO uint16_t CAPTCTRLB;            /**< \brief Capture Control B Register, array offset: 0x38, array step: 0x60 */
        __IO uint16_t CAPTCOMPB;            /**< \brief Capture Compare B Register, array offset: 0x3A, array step: 0x60 */
        __IO uint16_t CAPTCTRLX;            /**< \brief Capture Control X Register, array offset: 0x3C, array step: 0x60 */
        __IO uint16_t CAPTCOMPX;            /**< \brief Capture Compare X Register, array offset: 0x3E, array step: 0x60 */
        __I  uint16_t CVAL0;                /**< \brief Capture Value 0 Register, array offset: 0x40, array step: 0x60 */
        __I  uint16_t CVAL0CYC;             /**< \brief Capture Value 0 Cycle Register, array offset: 0x42, array step: 0x60 */
        __I  uint16_t CVAL1;                /**< \brief Capture Value 1 Register, array offset: 0x44, array step: 0x60 */
        __I  uint16_t CVAL1CYC;             /**< \brief Capture Value 1 Cycle Register, array offset: 0x46, array step: 0x60 */
        __I  uint16_t CVAL2;                /**< \brief Capture Value 2 Register, array offset: 0x48, array step: 0x60 */
        __I  uint16_t CVAL2CYC;             /**< \brief Capture Value 2 Cycle Register, array offset: 0x4A, array step: 0x60 */
        __I  uint16_t CVAL3;                /**< \brief Capture Value 3 Register, array offset: 0x4C, array step: 0x60 */
        __I  uint16_t CVAL3CYC;             /**< \brief Capture Value 3 Cycle Register, array offset: 0x4E, array step: 0x60 */
        __I  uint16_t CVAL4;                /**< \brief Capture Value 4 Register, array offset: 0x50, array step: 0x60 */
        __I  uint16_t CVAL4CYC;             /**< \brief Capture Value 4 Cycle Register, array offset: 0x52, array step: 0x60 */
        __I  uint16_t CVAL5;                /**< \brief Capture Value 5 Register, array offset: 0x54, array step: 0x60 */
        __I  uint16_t CVAL5CYC;             /**< \brief Capture Value 5 Cycle Register, array offset: 0x56, array step: 0x60 */
             uint8_t RESERVED_1[8];
    } SM[4];

    __IO uint16_t OUTEN;                    /**< \brief Output Enable Register, offset: 0x180 */
    __IO uint16_t MASK;                     /**< \brief Mask Register, offset: 0x182 */
    __IO uint16_t SWCOUT;                   /**< \brief Software Controlled Output Register, offset: 0x184 */
    __IO uint16_t DTSRCSEL;                 /**< \brief PWM Source Select Register, offset: 0x186 */
    __IO uint16_t MCTRL;                    /**< \brief Master Control Register, offset: 0x188 */
    __IO uint16_t MCTRL2;                   /**< \brief Master Control 2 Register, offset: 0x18A */
    __IO uint16_t FCTRL;                    /**< \brief Fault Control Register, offset: 0x18C */
    __IO uint16_t FSTS;                     /**< \brief Fault Status Register, offset: 0x18E */
    __IO uint16_t FFILT;                    /**< \brief Fault Filter Register, offset: 0x190 */
    __IO uint16_t FTST;                     /**< \brief Fault Test Register, offset: 0x192 */
    __IO uint16_t FCTRL2;                   /**< \brief Fault Control 2 Register, offset: 0x194 */

} imx10xx_eflex_pwm_regs_t;



/*******************************************************************************
  SM0CTRL2 - SM3CTRL2 Register
*******************************************************************************/


/* Clock Source Select */
#define __PWM_CTRL2_CLK_SEL_SHIFT       0
#define __PWM_CTRL2_CLK_SEL(x)          (((x) & 0x03) << __PWM_CTRL2_CLK_SEL_SHIFT)

/* Reload Source Select */
#define __PWM_CTRL2_RELOAD_SEL_SHIFT    2
#define __PWM_CTRL2_RELOAD_SEL_MASK     (0x1UL << __PWM_CTRL2_RELOAD_SEL_SHIFT)

/* Force Source Select */
#define __PWM_CTRL2_FORCE_SEL_SHIFT     3
#define __PWM_CTRL2_FORCE_SEL(x)        (((x) & 0x07) << __PWM_CTRL2_FORCE_SEL_SHIFT)

/* Force Initialization */
#define __PWM_CTRL2_FORCE_SHIFT         6
#define __PWM_CTRL2_FORCE_MASK          (0x1UL << __PWM_CTRL2_FORCE_SHIFT)

/* Force enable */
#define __PWM_CTRL2_FRCEN_SHIFT         7
#define __PWM_CTRL2_FRCEN_MASK          (0x1UL << __PWM_CTRL2_FRCEN_SHIFT)

/* Initialization Control Select */
#define __PWM_CTRL2_INIT_SEL_SHIFT      8
#define __PWM_CTRL2_INIT_SEL(x)         (((x) & 0x03) << __PWM_CTRL2_INIT_SEL_SHIFT)

/* PWM_X Initial Value */
#define __PWM_CTRL2_PWMX_INIT_SHIFT     10
#define __PWM_CTRL2_PWMX_INIT_MASK      (0x1UL << __PWM_CTRL2_PWMX_INIT_SHIFT)

/* PWM45 Initial Value */
#define __PWM_CTRL2_PWM45_INIT_SHIFT    11
#define __PWM_CTRL2_PWM45_INIT_MASK     (0x1UL << __PWM_CTRL2_PWM45_INIT_SHIFT)

/* PWM23 Initial Value */
#define __PWM_CTRL2_PWM23_INIT_SHIFT    12
#define __PWM_CTRL2_PWM23_INIT_MASK     (0x1UL << __PWM_CTRL2_PWM23_INIT_SHIFT)

/* Independent or Complementary Pair Operation */
#define __PWM_CTRL2_INDEP_SHIFT         13
#define __PWM_CTRL2_INDEP_MASK          (0x1UL << __PWM_CTRL2_INDEP_SHIFT)

/* WAIT Enable */
#define __PWM_CTRL2_WAITEN_SHIFT        14
#define __PWM_CTRL2_WAITEN_MASK         (0x1UL << __PWM_CTRL2_WAITEN_SHIFT)

/* Debug Enable */
#define __PWM_CTRL2_DBGEN_SHIFT         15
#define __PWM_CTRL2_DBGEN_MASK          (0x1UL << __PWM_CTRL2_DBGEN_SHIFT)



/*******************************************************************************
  SM0CTRL - SM3CTRL Register
*******************************************************************************/

/* Double Switching Enable */
#define __PWM_CTRL_DBLEN_SHIFT          0
#define __PWM_CTRL_DBLEN_MASK           (0x1UL << __PWM_CTRL_DBLEN_SHIFT)

/* Double Switching Enable */
#define __PWM_CTRL_DBLX_SHIFT           1
#define __PWM_CTRL_DBLX_MASK           (0x1UL << __PWM_CTRL_DBLX_SHIFT)

/* Load Mode Select  */
#define __PWM_CTRL_LDMOD_SHIFT          2
#define __PWM_CTRL_LDMOD_MASK          (0x1UL << __PWM_CTRL_LDMOD_SHIFT)

/* Split the DBLPWM signal to PWMA and PWMB */
#define __PWM_CTRL_SPLIT_SHIFT          3
#define __PWM_CTRL_SPLIT_MASK          (0x1UL << __PWM_CTRL_SPLIT_SHIFT)

/* Prescaler */
#define __PWM_CTRL_PRSC_SHIFT           4
#define __PWM_CTRL_PRSC_MASK           (0x07 << __PWM_CTRL_PRSC_SHIFT)
#define __PWM_CTRL_PRSC(x)             (((x) & 0x07) << __PWM_CTRL_PRSC_SHIFT)

/* Compare Mode */
#define __PWM_CTRL_COMPMODE_SHIFT       7
#define __PWM_CTRL_COMPMODE_MASK       (0x1UL << __PWM_CTRL_COMPMODE_SHIFT)

/* Deadtime */
#define __PWM_CTRL_DT_SHIFT             8
#define __PWM_CTRL_DT_MASK             (0x03 << __PWM_CTRL_DT_SHIFT)
#define __PWM_CTRL_DT(x)               (((x) & 0x03) << __PWM_CTRL_DT_SHIFT)

/* Full Cycle Reload */
#define __PWM_CTRL_FULL_SHIFT           10
#define __PWM_CTRL_FULL_MASK           (0x1UL << __PWM_CTRL_FULL_SHIFT)

/* Half Cycle Reload */
#define __PWM_CTRL_HALF_SHIFT           11
#define __PWM_CTRL_HALF_MASK           (0x1UL << __PWM_CTRL_HALF_SHIFT)

/* Load Frequency */
#define __PWM_CTRL_LDFQ_SHIFT           12
#define __PWM_CTRL_LDFQ_MASK           (0xF << __PWM_CTRL_LDFQ_SHIFT)
#define __PWM_CTRL_LDFQ(x)             (((x) & 0xF) << __PWM_CTRL_LDFQ_SHIFT)



/*******************************************************************************
  Output Control Register Register
*******************************************************************************/

/* PWM_X Fault State */
#define __PWM_OCTRL_PWMXFS_SHIFT         0
#define __PWM_OCTRL_PWMXFS_MASK         (0x03 << __PWM_OCTRL_PWMXFS_SHIFT)
#define __PWM_OCTRL_PWMXFS(x)           (((x) & 0x03) << __PWM_OCTRL_PWMXFS_SHIFT)

/* PWM_B Fault State */
#define __PWM_OCTRL_PWMBFS_SHIFT         2
#define __PWM_OCTRL_PWMBFS_MASK         (0x03 << __PWM_OCTRL_PWMBFS_SHIFT)
#define __PWM_OCTRL_PWMBFS(x)           (((x) & 0x03) << __PWM_OCTRL_PWMBFS_SHIFT)

/* PWM_A Fault State */
#define __PWM_OCTRL_PWMAFS_SHIFT         4
#define __PWM_OCTRL_PWMAFS_MASK         (0x03 << __PWM_OCTRL_PWMAFS_SHIFT)
#define __PWM_OCTRL_PWMAFS(x)           (((x) & 0x03) << __PWM_OCTRL_PWMAFS_SHIFT)

/* PWM_X Output Polarity */
#define __PWM_OCTRL_POLX_SHIFT           8
#define __PWM_OCTRL_POLX_MASK           (0x1UL << __PWM_OCTRL_POLX_SHIFT)

/* PWM_B Output Polarity */
#define __PWM_OCTRL_POLB_SHIFT           9
#define __PWM_OCTRL_POLB_MASK           (0x1UL << __PWM_OCTRL_POLB_SHIFT)

/* PWM_A Output Polarity */
#define __PWM_OCTRL_POLA_SHIFT           10
#define __PWM_OCTRL_POLA_MASK           (0x1UL << __PWM_OCTRL_POLA_SHIFT)

/* PWM_X Input */
#define __PWM_OCTRL_PWMX_IN_SHIFT        13
#define __PWM_OCTRL_PWMX_IN_MASK        (0x1UL << __PWM_OCTRL_PWMX_IN_SHIFT)

/* PWM_B Input */
#define __PWM_OCTRL_PWMB_IN_SHIFT        14
#define __PWM_OCTRL_PWMB_IN_MASK        (0x1UL << __PWM_OCTRL_PWMB_IN_SHIFT)

/* PWM_A Input */
#define __PWM_OCTRL_PWMA_IN_SHIFT        15
#define __PWM_OCTRL_PWMA_IN_MASK        (0x1UL << __PWM_OCTRL_PWMA_IN_SHIFT)


/*******************************************************************************
  Output Enable Register
*******************************************************************************/

/* PWM_X Output Enables */
#define __PWM_OUTEN_PWMX_EN_SHIFT         0
#define __PWM_OUTEN_PWMX_EN(x)           (((x) & 0xF) << __PWM_OUTEN_PWMX_EN_SHIFT)


/* PWM_B Output Enables */
#define __PWM_OUTEN_PWMB_EN_SHIFT         4
#define __PWM_OUTEN_PWMB_EN(x)           (((x) & 0xF) << __PWM_OUTEN_PWMB_EN_SHIFT)


/* PWM_A Output Enables */
#define __PWM_OUTEN_PWMA_EN_SHIFT         8
#define __PWM_OUTEN_PWMA_EN(x)           ((x) & 0xF) << __PWM_OUTEN_PWMA_EN_SHIFT)


/*******************************************************************************
  Deadtime Count Register 0
*******************************************************************************/

/* DTCNT0 */
#define __PWM_DTCNT0(x)                  (((x) & 0xFF))

/*******************************************************************************
  Deadtime Count Register 1
*******************************************************************************/

/* DTCNT0 */
#define __PWM_DTCNT1(x)                  (((x) & 0xFF))

/*******************************************************************************
  Master Control Register
*******************************************************************************/

/* Load Okay */
#define __PWM_MCTRL_LDOK_SHIFT            0
#define __PWM_MCTRL_LDOK_MASK            (0xF << __PWM_MCTRL_LDOK_SHIFT)
#define __PWM_MCTRL_LDOK(x)              (((x) & 0xF) << __PWM_MCTRL_LDOK_SHIFT)

/* Clear Load Okay */
#define __PWM_MCTRL_CLDOK_SHIFT           4
#define __PWM_MCTRL_CLDOK_MASK           (0xF << __PWM_MCTRL_CLDOK_SHIFT)
#define __PWM_MCTRL_CLDOK(x)             (((x) & 0xF) << __PWM_MCTRL_CLDOK_SHIFT)

/* Run */
#define __PWM_MCTRL_RUN_SHIFT             8
#define __PWM_MCTRL_RUN_MASK             (0xF << __PWM_MCTRL_RUN_SHIFT)
#define __PWM_MCTRL_RUN(x)               (((x) & 0xF) << __PWM_MCTRL_RUN_SHIFT)

/* Current Polarity */
#define __PWM_MCTRL_IPOL_SHIFT            12
#define __PWM_MCTRL_IPOL_MASK            (0xF << __PWM_MCTRL_IPOL_SHIFT)
#define __PWM_MCTRL_IPOL(x)              (((x) & 0xF) << __PWM_MCTRL_IPOL_SHIFT)


/*******************************************************************************
  Fault Status Register
*******************************************************************************/

/* Fault Flags */
#define __PWM_FSTS_FFLAG_SHIFT            0
#define __PWM_FSTS_FFLAG_MASK            (0xF << __PWM_FSTS_FFLAG_SHIFT)


/*******************************************************************************
 Fault Filter Register
*******************************************************************************/

/* Fault Filter Period */
#define __PWM_FFILT_FILT_PER_SHIFT        0
#define __PWM_FFILT_FILT_PER_MASK        (0xF << __PWM_FFILT_FILT_PER_SHIFT)


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX10xx_EFLEX_PWM_REGS_H */

/* end of file */
