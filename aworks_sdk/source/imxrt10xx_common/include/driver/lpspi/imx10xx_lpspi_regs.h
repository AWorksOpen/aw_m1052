/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief iMX RT10XX spi regs
 *
 * \internal
 * \par modification history:
 * - 1.00 17-10-17  mex, first implementation
 * \endinternal
 */

#ifndef __IMX10XX_LPSPI_REGS_H
#define __IMX10XX_LPSPI_REGS_H


/**
 * \brief iMX RT10XX LPSPI ¼Ä´æÆ÷¿é¶¨Òå
 */
typedef struct {

    uint32_t VERID;                     /**< \brief Version ID Register, offset: 0x0 */
    uint32_t PARAM;                     /**< \brief Parameter Register, offset: 0x4 */
    uint8_t RESERVED_0[8];
    uint32_t CR;                        /**< \brief Control Register, offset: 0x10 */
    uint32_t SR;                        /**< \brief Status Register, offset: 0x14 */
    uint32_t IER;                       /**< \brief Interrupt Enable Register, offset: 0x18 */
    uint32_t DER;                       /**< \brief DMA Enable Register, offset: 0x1C */
    uint32_t CFGR0;                     /**< \brief Configuration Register 0, offset: 0x20 */
    uint32_t CFGR1;                     /**< \brief Configuration Register 1, offset: 0x24 */
    uint8_t RESERVED_1[8];
    uint32_t DMR0;                      /**< \brief Data Match Register 0, offset: 0x30 */
    uint32_t DMR1;                      /**< \brief Data Match Register 1, offset: 0x34 */
    uint8_t RESERVED_2[8];
    uint32_t CCR;                       /**< \brief Clock Configuration Register, offset: 0x40 */
    uint8_t RESERVED_3[20];
    uint32_t FCR;                       /**< \brief FIFO Control Register, offset: 0x58 */
    uint32_t FSR;                       /**< \brief FIFO Status Register, offset: 0x5C */
    uint32_t TCR;                       /**< \brief Transmit Command Register, offset: 0x60 */
    uint32_t TDR;                       /**< \brief Transmit Data Register, offset: 0x64 */
    uint8_t RESERVED_4[8];
    uint32_t RSR;                       /**< \brief Receive Status Register, offset: 0x70 */
    uint32_t RDR;                       /**< \brief Receive Data Register, offset: 0x74 */

} imx10xx_lpspi_regs_t;



/**
 * \brief LPSPI status flags in SPIx_SR register
 */
enum __lpspi_flags
{
    kLPSPI_TxDataRequestFlag = 0x1UL,       /**< \brief Transmit data flag */
    kLPSPI_RxDataReadyFlag = 0x2UL,         /**< \brief Receive data flag */
    kLPSPI_WordCompleteFlag = 0x100UL,      /**< \brief Word Complete flag */
    kLPSPI_FrameCompleteFlag = 0x200UL,     /**< \brief Frame Complete flag */
    kLPSPI_TransferCompleteFlag = 0x400UL,  /**< \brief Transfer Complete flag */
    kLPSPI_TransmitErrorFlag = 0x800UL,     /**< \brief Transmit Error flag (FIFO underrun) */
    kLPSPI_ReceiveErrorFlag = 0x1000UL,     /**< \brief Receive Error flag (FIFO overrun) */
    kLPSPI_DataMatchFlag = 0x2000UL,        /**< \brief Data Match flag */
    kLPSPI_ModuleBusyFlag = 0x1000000UL,    /**< \brief Module Busy flag */
    kLPSPI_AllStatusFlag = 0x1003f03UL      /**< \brief Used for clearing all w1c status flags */
};

/**
 * \brief LPSPI interrupt source
 */
enum __lpspi_interrupt_source
{
    kLPSPI_TxInterrupt = 0x1UL,                 /**< \brief Transmit data interrupt */
    kLPSPI_RxInterrupt = 0x2UL,                 /**< \brief Receive data interrupt */
    kLPSPI_WordCompleteInterrupt = 0x100UL,      /**< \brief Word complete interrupt */
    kLPSPI_FrameCompleteInterrupt = 0x200UL,     /**< \brief Frame complete interrupt */
    kLPSPI_TransferCompleteInterrupt = 0x400UL,  /**< \brief Transfer complete interrupt */
    kLPSPI_TransmitErrorInterrupt = 0x800UL,     /**< \brief Transmit error interrupt(FIFO underrun)*/
    kLPSPI_ReceiveErrorInterrupt = 0x1000UL,     /**< \brief Receive Error interrupt (FIFO overrun) */
    kLPSPI_DataMatchInterrupt = 0x2000UL,        /**< \brief Data Match interrupt */
    kLPSPI_AllInterrupt = 0x3F03UL              /**< \brief All above interrupts */
};

/**
 * \brief LPSPI delay type
 */
typedef enum __lpspi_delay_type
{
    kLPSPI_PcsToSck = 1,              /**< \brief PCS-to-SCK delay. */
    kLPSPI_LastSckToPcs = 2,          /**< \brief Last SCK edge to PCS delay. */
    kLPSPI_BetweenTransfer = 3        /**< \brief  Delay between transfers. */
} lpspi_delay_type_t;


/*******************************************************************************
  LPSPI Control Register
*******************************************************************************/

/** \brief Module Enable */
#define __LPSPI_CR_MEN_MASK              (0x1UL << 0)

/** \brief  Reset all internal logic and registers, except the Control Register */
#define __LPSPI_CR_RST_MASK              (0x1UL << 1)

/** \brief  Reset Receive FIFO */
#define __LPSPI_CR_RRF_MASK              (0x1UL << 9)

/** \brief  Reset Transmit FIFO */
#define __LPSPI_CR_RTF_MASK              (0x1UL << 8)

/*******************************************************************************
  LPSPI Configuration Register 1
*******************************************************************************/

/** \brief  Configures the LPSPI in master or slave mode */
#define __LPSPI_CFGR1_MASTER_MASK        (0x1UL)

/** \brief  PCS[3:2] are disabled */
#define __LPSPI_CFGR1_PCSCFG_MASK        (0x1UL << 27)

/** \brief  Pin Configuration */
#define __LPSPI_CFGR1_CFGR1_PINCFG(x)    ((((uint32_t)(x)) << 24) & 0x3000000UL)

/** \brief  Configures if the output data is tristated between accesses (LPSPI_PCS is negated) */
#define __LPSPI_CFGR1_OUTCFG(x)          ((((uint32_t)(x)) << 26) & 0x4000000UL)

/** \brief  No Stall Configuration */
#define __LPSPI_CFGR1_NOSTALL_CFG(x)     ((((uint32_t)(x)) << 3) & 0x8UL)

/** \brief  Peripheral Chip Select Polarity */
#define __LPSPI_CFGR1_PCSPOL(x)          ((((uint32_t)(x)) << 8) & 0xF00UL)


/** \brief  Configures the divide ratio of the SCK pin in master mode */
#define __LPSPI_CCR_SCKDIV(x)             ((((uint32_t)(x)) << 0) & 0xFFUL)

/** \brief  Configures the delay in master mode from the PCS negation to the next PCS assertion */
#define __LPSPI_CCR_DBT(x)                ((((uint32_t)(x)) << 8) & 0xFF00UL)

/** \brief  Configures the delay in master mode from the PCS assertion to the first SCK edge */
#define __LPSPI_CCR_PCSSCK(x)             ((((uint32_t)(x)) << 16) & 0xFF0000UL)

/** \brief  Configures the delay in master mode from the last SCK edge to the PCS negation */
#define __LPSPI_CCR_SCKPCS(x)             ((((uint32_t)(x)) << 24) & 0xFF000000UL)


/*******************************************************************************
  LPSPI Transmit Command Register
*******************************************************************************/

/** \brief  Frame Size */
#define __LPSPI_TCR_FRAMESZ(x)            ((((uint32_t)(x)) << 0) & 0xFFFUL)

/** \brief  Transfer Width */
#define __LPSPI_TCR_WIDTH(x)              ((((uint32_t)(x)) << 16) & 0x30000UL)

/** \brief  Transmit Data Mask */
#define __LPSPI_TCR_TXMSK(x)              ((((uint32_t)(x)) << 18) & 0x40000UL)

/** \brief  Receive Data Mask */
#define __LPSPI_TCR_RXMSK(x)              ((((uint32_t)(x)) << 19) & 0x80000UL)

/** \brief  Continuing Command */
#define __LPSPI_TCR_CONTC(x)              ((((uint32_t)(x)) << 20) & 0x100000UL)

/** \brief  Continuous Transfer */
#define __LPSPI_TCR_CONT(x)               ((((uint32_t)(x)) << 21) & 0x200000UL)

/** \brief  Byte Swap */
#define __LPSPI_TCR_BYSW(x)               ((((uint32_t)(x)) << 22) & 0x400000UL)

/** \brief  Peripheral Chip Select */
#define __LPSPI_TCR_PCS(x)                ((((uint32_t)(x)) << 24) & 0x3000000UL)

/** \brief  Prescaler Value */
#define __LPSPI_TCR_PRESCALE(x)           ((((uint32_t)(x)) << 27) & 0x38000000UL)

/** \brief  Data is changed on the leading edge of SCK and captured on the following edge */
#define __LPSPI_TCR_CPHA                  (0x1UL << 30)

/** \brief  The inactive state value of SCK is high */
#define __LPSPI_TCR_CPOL                  (0x1UL << 31)

/** \brief  Data is transferred LSB first */
#define __LPSPI_TCR_LSBF                  (0x1UL << 23)

#endif /* __IMX10XX_LPSPI_REGS_H */

/* end of file  */
