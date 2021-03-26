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
 * \brief iMX RT1050 OCOTP¼Ä´æÆ÷¶¨Òå
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-15  mex, first implementation
 * \endinternal
 */

#ifndef __IMX6UL_OCOTP_REGS_H
#define __IMX6UL_OCOTP_REGS_H

#include "aworks.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#define   __I     volatile const      /**< \brief Defines 'read only' permissions */
#define   __O     volatile            /**< \brief Defines 'write only' permissions */
#define   __IO    volatile            /**< \brief Defines 'read / write' permissions */

/**
 * \brief  OCOTP Register Layout Typedef
 */
typedef struct
{
    __IO uint32_t CTRL;                     /**< OTP Controller Control Register, offset: 0x0 */
    __IO uint32_t CTRL_SET;                 /**< OTP Controller Control Register, offset: 0x4 */
    __IO uint32_t CTRL_CLR;                 /**< OTP Controller Control Register, offset: 0x8 */
    __IO uint32_t CTRL_TOG;                 /**< OTP Controller Control Register, offset: 0xC */
    __IO uint32_t TIMING;                   /**< OTP Controller Timing Register, offset: 0x10 */
         uint8_t RESERVED_0[12];
    __IO uint32_t DATA;                     /**< OTP Controller Write Data Register, offset: 0x20 */
         uint8_t RESERVED_1[12];
    __IO uint32_t READ_CTRL;                /**< OTP Controller Write Data Register, offset: 0x30 */
         uint8_t RESERVED_2[12];
    __IO uint32_t READ_FUSE_DATA;           /**< OTP Controller Read Data Register, offset: 0x40 */
         uint8_t RESERVED_3[12];
    __IO uint32_t SW_STICKY;                /**< Sticky bit Register, offset: 0x50 */
         uint8_t RESERVED_4[12];
    __IO uint32_t SCS;                      /**< Software Controllable Signals Register, offset: 0x60 */
    __IO uint32_t SCS_SET;                  /**< Software Controllable Signals Register, offset: 0x64 */
    __IO uint32_t SCS_CLR;                  /**< Software Controllable Signals Register, offset: 0x68 */
    __IO uint32_t SCS_TOG;                  /**< Software Controllable Signals Register, offset: 0x6C */
         uint8_t RESERVED_5[32];
    __I  uint32_t VERSION;                  /**< OTP Controller Version Register, offset: 0x90 */
         uint8_t RESERVED_6[108];
    __IO uint32_t TIMING2;                  /**< OTP Controller Timing Register 2, offset: 0x100 */
         uint8_t RESERVED_7[764];
    __IO uint32_t LOCK;                     /**< Value of OTP Bank0 Word0 (Lock controls), offset: 0x400 */
         uint8_t RESERVED_8[12];
    __IO uint32_t CFG0;                     /**< Value of OTP Bank0 Word1 (Configuration and Manufacturing Info.), offset: 0x410 */
         uint8_t RESERVED_9[12];
    __IO uint32_t CFG1;                     /**< Value of OTP Bank0 Word2 (Configuration and Manufacturing Info.), offset: 0x420 */
         uint8_t RESERVED_10[12];
    __IO uint32_t CFG2;                     /**< Value of OTP Bank0 Word3 (Configuration and Manufacturing Info.), offset: 0x430 */
         uint8_t RESERVED_11[12];
    __IO uint32_t CFG3;                     /**< Value of OTP Bank0 Word4 (Configuration and Manufacturing Info.), offset: 0x440 */
         uint8_t RESERVED_12[12];
    __IO uint32_t CFG4;                     /**< Value of OTP Bank0 Word5 (Configuration and Manufacturing Info.), offset: 0x450 */
         uint8_t RESERVED_13[12];
    __IO uint32_t CFG5;                     /**< Value of OTP Bank0 Word6 (Configuration and Manufacturing Info.), offset: 0x460 */
         uint8_t RESERVED_14[12];
    __IO uint32_t CFG6;                     /**< Value of OTP Bank0 Word7 (Configuration and Manufacturing Info.), offset: 0x470 */
         uint8_t RESERVED_15[12];
    __IO uint32_t MEM0;                     /**< Value of OTP Bank1 Word0 (Memory Related Info.), offset: 0x480 */
         uint8_t RESERVED_16[12];
    __IO uint32_t MEM1;                     /**< Value of OTP Bank1 Word1 (Memory Related Info.), offset: 0x490 */
         uint8_t RESERVED_17[12];
    __IO uint32_t MEM2;                     /**< Value of OTP Bank1 Word2 (Memory Related Info.), offset: 0x4A0 */
         uint8_t RESERVED_18[12];
    __IO uint32_t MEM3;                     /**< Value of OTP Bank1 Word3 (Memory Related Info.), offset: 0x4B0 */
         uint8_t RESERVED_19[12];
    __IO uint32_t MEM4;                     /**< Value of OTP Bank1 Word4 (Memory Related Info.), offset: 0x4C0 */
         uint8_t RESERVED_20[12];
    __IO uint32_t ANA0;                     /**< Value of OTP Bank1 Word5 (Analog Info.), offset: 0x4D0 */
         uint8_t RESERVED_21[12];
    __IO uint32_t ANA1;                     /**< Value of OTP Bank1 Word6 (Analog Info.), offset: 0x4E0 */
         uint8_t RESERVED_22[12];
    __IO uint32_t ANA2;                     /**< Value of OTP Bank1 Word7 (Analog Info.), offset: 0x4F0 */
         uint8_t RESERVED_23[140];
    __IO uint32_t SRK0;                     /**< Shadow Register for OTP Bank3 Word0 (SRK Hash), offset: 0x580 */
         uint8_t RESERVED_24[12];
    __IO uint32_t SRK1;                     /**< Shadow Register for OTP Bank3 Word1 (SRK Hash), offset: 0x590 */
         uint8_t RESERVED_25[12];
    __IO uint32_t SRK2;                     /**< Shadow Register for OTP Bank3 Word2 (SRK Hash), offset: 0x5A0 */
         uint8_t RESERVED_26[12];
    __IO uint32_t SRK3;                     /**< Shadow Register for OTP Bank3 Word3 (SRK Hash), offset: 0x5B0 */
         uint8_t RESERVED_27[12];
    __IO uint32_t SRK4;                     /**< Shadow Register for OTP Bank3 Word4 (SRK Hash), offset: 0x5C0 */
         uint8_t RESERVED_28[12];
    __IO uint32_t SRK5;                     /**< Shadow Register for OTP Bank3 Word5 (SRK Hash), offset: 0x5D0 */
         uint8_t RESERVED_29[12];
    __IO uint32_t SRK6;                     /**< Shadow Register for OTP Bank3 Word6 (SRK Hash), offset: 0x5E0 */
         uint8_t RESERVED_30[12];
    __IO uint32_t SRK7;                     /**< Shadow Register for OTP Bank3 Word7 (SRK Hash), offset: 0x5F0 */
         uint8_t RESERVED_31[12];
    __IO uint32_t SJC_RESP0;                /**< Value of OTP Bank4 Word0 (Secure JTAG Response Field), offset: 0x600 */
         uint8_t RESERVED_32[12];
    __IO uint32_t SJC_RESP1;                /**< Value of OTP Bank4 Word1 (Secure JTAG Response Field), offset: 0x610 */
         uint8_t RESERVED_33[12];
    __IO uint32_t MAC0;                     /**< Value of OTP Bank4 Word2 (MAC Address), offset: 0x620 */
         uint8_t RESERVED_34[12];
    __IO uint32_t MAC1;                     /**< Value of OTP Bank4 Word3 (MAC Address), offset: 0x630 */
         uint8_t RESERVED_35[12];
    __IO uint32_t GP3;                      /**< Value of OTP Bank4 Word4 (MAC Address), offset: 0x640 */
         uint8_t RESERVED_36[28];
    __IO uint32_t GP1;                      /**< Value of OTP Bank4 Word6 (General Purpose Customer Defined Info), offset: 0x660 */
         uint8_t RESERVED_37[12];
    __IO uint32_t GP2;                      /**< Value of OTP Bank4 Word7 (General Purpose Customer Defined Info), offset: 0x670 */
         uint8_t RESERVED_38[12];
    __IO uint32_t SW_GP1;                   /**< Value of OTP Bank5 Word0 (SW GP1), offset: 0x680 */
         uint8_t RESERVED_39[12];
    __IO uint32_t SW_GP20;                  /**< Value of OTP Bank5 Word1 (SW GP2), offset: 0x690 */
         uint8_t RESERVED_40[12];
    __IO uint32_t SW_GP21;                  /**< Value of OTP Bank5 Word2 (SW GP2), offset: 0x6A0 */
         uint8_t RESERVED_41[12];
    __IO uint32_t SW_GP22;                  /**< Value of OTP Bank5 Word3 (SW GP2), offset: 0x6B0 */
         uint8_t RESERVED_42[12];
    __IO uint32_t SW_GP23;                  /**< Value of OTP Bank5 Word4 (SW GP2), offset: 0x6C0 */
         uint8_t RESERVED_43[12];
    __IO uint32_t MISC_CONF0;               /**< Value of OTP Bank5 Word5 (Misc Conf), offset: 0x6D0 */
         uint8_t RESERVED_44[12];
    __IO uint32_t MISC_CONF1;               /**< Value of OTP Bank5 Word6 (Misc Conf), offset: 0x6E0 */
         uint8_t RESERVED_45[12];
    __IO uint32_t SRK_REVOKE;               /**< Value of OTP Bank5 Word7 (SRK Revoke), offset: 0x6F0 */

} imx1050_ocotp_regs_t;

#define BV_OCOTP_CTRL_WR_UNLOCK__KEY    (0x3e77)

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX6UL_IOMUXC_REGS_H */

/* end of file */
