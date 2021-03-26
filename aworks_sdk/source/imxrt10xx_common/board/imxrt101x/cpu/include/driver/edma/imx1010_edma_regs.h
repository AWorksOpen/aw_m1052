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
 * \brief stm32f412 eDMA regs
 *
 * \internal
 * \par modification history:
 * - 1.00 2017-11-07  hsg, first implementation
 * \endinternal
 */

#ifndef __IMX1010_EDMA_REGS_H__
#define __IMX1010_EDMA_REGS_H__

#define __IO  volatile
#define __O   volatile
#define __I   volatile const

/**
 * \brief iMX RT1010 eDMA 寄存器块定义
 */
typedef struct {
    __IO uint32_t CR;   /**< Control Register, offset: 0x0 */
    __I  uint32_t ES;   /**< Error Status Register, offset: 0x4 */
         uint8_t RESERVED_0[4];
    __IO uint32_t ERQ;  /**< Enable Request Register, offset: 0xC */
         uint8_t RESERVED_1[4];
    __IO uint32_t EEI;  /**< Enable Error Interrupt Register, offset: 0x14 */
    __O  uint8_t CEEI;  /**< Clear Enable Error Interrupt Register, offset: 0x18 */
    __O  uint8_t SEEI;  /**< Set Enable Error Interrupt Register, offset: 0x19 */
    __O  uint8_t CERQ;  /**< Clear Enable Request Register, offset: 0x1A */
    __O  uint8_t SERQ;  /**< Set Enable Request Register, offset: 0x1B */
    __O  uint8_t CDNE;  /**< Clear DONE Status Bit Register, offset: 0x1C */
    __O  uint8_t SSRT;  /**< Set START Bit Register, offset: 0x1D */
    __O  uint8_t CERR;  /**< Clear Error Register, offset: 0x1E */
    __O  uint8_t CINT;  /**< Clear Interrupt Request Register, offset: 0x1F */
         uint8_t RESERVED_2[4];
    __IO uint32_t INT;  /**< Interrupt Request Register, offset: 0x24 */
         uint8_t RESERVED_3[4];
    __IO uint32_t ERR;  /**< Error Register, offset: 0x2C */
         uint8_t RESERVED_4[4];
    __I  uint32_t HRS;  /**< Hardware Request Status Register, offset: 0x34 */
         uint8_t RESERVED_5[12];
    __IO uint32_t EARS; /**< Enable Asynchronous Request in Stop Register, offset: 0x44 */
         uint8_t RESERVED_6[184];
    __IO uint8_t DCHPRI3;   /**< Channel n Priority Register, offset: 0x100 */
    __IO uint8_t DCHPRI2;   /**< Channel n Priority Register, offset: 0x101 */
    __IO uint8_t DCHPRI1;   /**< Channel n Priority Register, offset: 0x102 */
    __IO uint8_t DCHPRI0;   /**< Channel n Priority Register, offset: 0x103 */
    __IO uint8_t DCHPRI7;   /**< Channel n Priority Register, offset: 0x104 */
    __IO uint8_t DCHPRI6;   /**< Channel n Priority Register, offset: 0x105 */
    __IO uint8_t DCHPRI5;   /**< Channel n Priority Register, offset: 0x106 */
    __IO uint8_t DCHPRI4;   /**< Channel n Priority Register, offset: 0x107 */
    __IO uint8_t DCHPRI11;  /**< Channel n Priority Register, offset: 0x108 */
    __IO uint8_t DCHPRI10;  /**< Channel n Priority Register, offset: 0x109 */
    __IO uint8_t DCHPRI9;   /**< Channel n Priority Register, offset: 0x10A */
    __IO uint8_t DCHPRI8;   /**< Channel n Priority Register, offset: 0x10B */
    __IO uint8_t DCHPRI15;  /**< Channel n Priority Register, offset: 0x10C */
    __IO uint8_t DCHPRI14;  /**< Channel n Priority Register, offset: 0x10D */
    __IO uint8_t DCHPRI13;  /**< Channel n Priority Register, offset: 0x10E */
    __IO uint8_t DCHPRI12;  /**< Channel n Priority Register, offset: 0x10F */
         uint8_t RESERVED_7[3824];
    struct {                /* offset: 0x1000, array step: 0x20 */
      __IO uint32_t SADDR;  /**< TCD Source Address, array offset: 0x1000, array step: 0x20 */
      __IO uint16_t SOFF;   /**< TCD Signed Source Address Offset, array offset: 0x1004, array step: 0x20 */
      __IO uint16_t ATTR;   /**< TCD Transfer Attributes, array offset: 0x1006, array step: 0x20 */
      union {               /* offset: 0x1008, array step: 0x20 */
          __IO uint32_t NBYTES_MLNO;                       /**< TCD Minor Byte Count (Minor Loop Mapping Disabled), array offset: 0x1008, array step: 0x20 */
          __IO uint32_t NBYTES_MLOFFNO;                    /**< TCD Signed Minor Loop Offset (Minor Loop Mapping Enabled and Offset Disabled), array offset: 0x1008, array step: 0x20 */
          __IO uint32_t NBYTES_MLOFFYES;                   /**< TCD Signed Minor Loop Offset (Minor Loop Mapping and Offset Enabled), array offset: 0x1008, array step: 0x20 */
      };
      __IO uint32_t SLAST;  /**< TCD Last Source Address Adjustment, array offset: 0x100C, array step: 0x20 */
      __IO uint32_t DADDR;  /**< TCD Destination Address, array offset: 0x, array step: 0x20 */
      __IO uint16_t DOFF;   /**< TCD Signed Destination Address Offset, array offset: 0x1014, array step: 0x20 */
      union {               /* offset: 0x1016, array step: 0x20 */
          __IO uint16_t CITER_ELINKNO;  /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x1016, array step: 0x20 */
          __IO uint16_t CITER_ELINKYES; /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x1016, array step: 0x20 */
      };
      __IO uint32_t DLAST_SGA;  /**< TCD Last Destination Address Adjustment/Scatter Gather Address, array offset: 0x1018, array step: 0x20 */
      __IO uint16_t CSR;        /**< TCD Control and Status, array offset: 0x101C, array step: 0x20 */
      union {                   /* offset: 0x101E, array step: 0x20 */
          __IO uint16_t BITER_ELINKNO;  /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x101E, array step: 0x20 */
          __IO uint16_t BITER_ELINKYES; /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x101E, array step: 0x20 */
      };
    } TCD[16];
} rt1010_edma_regs_t;


/**
 * \brief iMX RT1010 DMAMUX 寄存器块定义
 */
typedef struct {
  __IO uint32_t CHCFG[16];                         /**<
                Channel 0 Configuration Register
              ..
                Channel 31 Configuration Register
              , array offset: 0x0, array step: 0x4 */
} rt1010_dmamux_regs_t;


/* 和TCD寄存器相似，用于使用Scatter/Gather 时保存传输描述符的配置  */
typedef struct __edma_tcd {
    __IO uint32_t SADDR;     /*!< SADDR register, used to save source address */
    __IO uint16_t SOFF;      /*!< SOFF register, save offset bytes every transfer */
    __IO uint16_t ATTR;      /*!< ATTR register, source/destination transfer size and modulo */
    __IO uint32_t NBYTES;    /*!< Nbytes register, minor loop length in bytes */
    __IO uint32_t SLAST;     /*!< SLAST register */
    __IO uint32_t DADDR;     /*!< DADDR register, used for destination address */
    __IO uint16_t DOFF;      /*!< DOFF register, used for destination offset */
    __IO uint16_t CITER;     /*!< CITER register, current minor loop numbers, for unfinished minor loop.*/
    __IO uint32_t DLAST_SGA; /*!< DLASTSGA register, next stcd address used in scatter-gather mode */
    __IO uint16_t CSR;       /*!< CSR register, for TCD control status */
    __IO uint16_t BITER;     /*!< BITER register, begin minor loop count. */

} __edma_tcd_t;


#endif   /* __IMX1010_EDMA_REGS_H__ */

/* enf of file */
