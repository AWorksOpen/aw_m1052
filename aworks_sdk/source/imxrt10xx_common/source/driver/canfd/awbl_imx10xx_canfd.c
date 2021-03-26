/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
*
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx FLEXCANFD
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-29  hsg, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "awbus_lite.h"
#include "aw_common.h"
#include "aw_int.h"
#include "aw_task.h"
#include "awbl_can.h"
#include "aw_can.h"
#include "aw_clk.h"
#include "driver/canfd/awbl_imx10xx_canfd.h"

/*******************************************************************************
  local defines
*******************************************************************************/

/**
 * \brief imx10xx CANFD 寄存器块定义
 */
typedef struct imx10xx_canfd_regs {
    volatile uint32_t MCR;              /**< \brief Module Configuration Register, offset: 0x0 */
    volatile uint32_t CTRL1;            /**< \brief Control 1 Register..Control 1 register, offset: 0x4 */
    volatile uint32_t TIMER;            /**< \brief Free Running Timer Register..Free Running Timer, offset: 0x8 */
             uint8_t  RESERVED_0[4];
    volatile uint32_t RXMGMASK;         /**< \brief Rx Mailboxes Global Mask Register, offset: 0x10 */
    volatile uint32_t RX14MASK;         /**< \brief Rx Buffer 14 Mask Register..Rx 14 Mask register, offset: 0x14 */
    volatile uint32_t RX15MASK;         /**< \brief Rx Buffer 15 Mask Register..Rx 15 Mask register, offset: 0x18 */
    volatile uint32_t ECR;              /**< \brief Error Counter Register..Error Counter, offset: 0x1C */
    volatile uint32_t ESR1;             /**< \brief Error and Status 1 Register..Error and Status 1 register, offset: 0x20 */
    volatile uint32_t IMASK2;           /**< \brief Interrupt Masks 2 Register..Interrupt Masks 2 register, offset: 0x24 */
    volatile uint32_t IMASK1;           /**< \brief Interrupt Masks 1 Register..Interrupt Masks 1 register, offset: 0x28 */
    volatile uint32_t IFLAG2;           /**< \brief Interrupt Flags 2 Register..Interrupt Flags 2 register, offset: 0x2C */
    volatile uint32_t IFLAG1;           /**< \brief Interrupt Flags 1 Register..Interrupt Flags 1 register, offset: 0x30 */
    volatile uint32_t CTRL2;            /**< \brief Control 2 Register..Control 2 register, offset: 0x34 */
    volatile uint32_t ESR2;             /**< \brief Error and Status 2 Register..Error and Status 2 register, offset: 0x38 */
             uint8_t  RESERVED_1[8];
    volatile uint32_t CRCR;             /**< \brief CRC Register, offset: 0x44 */
    volatile uint32_t RXFGMASK;         /**< \brief Rx FIFO Global Mask Register..Legacy Rx FIFO Global Mask register, offset: 0x48 */
    volatile uint32_t RXFIR;            /**< \brief Rx FIFO Information Register..Legacy Rx FIFO Information Register, offset: 0x4C */
    volatile uint32_t CBT;              /**< \brief CAN Bit Timing Register, offset: 0x50 */
             uint8_t  RESERVED_2[44];
    union {                             /* offset: 0x80 */
      struct {                          /* offset: 0x80, array step: 0x18 */
        volatile uint32_t CS;           /**< \brief Message Buffer 0 CS Register..Message Buffer 41 CS Register, array offset: 0x80, array step: 0x18 */
        volatile uint32_t ID;           /**< \brief Message Buffer 0 ID Register..Message Buffer 41 ID Register, array offset: 0x84, array step: 0x18 */
        volatile uint32_t WORD[4];      /**< \brief Message Buffer 0 WORD_16B Register..Message Buffer 41 WORD_16B Register, array offset: 0x88, array step: index*0x18, index2*0x4 */
      } MB_16B[42];
      struct {                          /* offset: 0x80, array step: 0x28 */
        volatile uint32_t CS;           /**< \brief Message Buffer 0 CS Register..Message Buffer 23 CS Register, array offset: 0x80, array step: 0x28 */
        volatile uint32_t ID;           /**< \brief Message Buffer 0 ID Register..Message Buffer 23 ID Register, array offset: 0x84, array step: 0x28 */
        volatile uint32_t WORD[8];      /**< \brief Message Buffer 0 WORD_32B Register..Message Buffer 23 WORD_32B Register, array offset: 0x88, array step: index*0x28, index2*0x4 */
      } MB_32B[24];
      struct {
          struct {                          /* offset: 0x80, array step: 0x48 */
            volatile uint32_t CS;           /**< \brief Message Buffer 0 CS Register..Message Buffer 13 CS Register, array offset: 0x80, array step: 0x48 */
            volatile uint32_t ID;           /**< \brief Message Buffer 0 ID Register..Message Buffer 13 ID Register, array offset: 0x84, array step: 0x48 */
            volatile uint32_t WORD[16];     /**< \brief Message Buffer 0 WORD_64B Register..Message Buffer 13 WORD_64B Register, array offset: 0x88, array step: index*0x48, index2*0x4 */
          } MB0[7];
          uint8_t  RESERVED_MB[8];
          struct {                          /* offset: 0x80, array step: 0x48 */
            volatile uint32_t CS;           /**< \brief Message Buffer 0 CS Register..Message Buffer 13 CS Register, array offset: 0x80, array step: 0x48 */
            volatile uint32_t ID;           /**< \brief Message Buffer 0 ID Register..Message Buffer 13 ID Register, array offset: 0x84, array step: 0x48 */
            volatile uint32_t WORD[16];     /**< \brief Message Buffer 0 WORD_64B Register..Message Buffer 13 WORD_64B Register, array offset: 0x88, array step: index*0x48, index2*0x4 */
          } MB7[7];
      } MB_64B;
      struct {                          /* offset: 0x80, array step: 0x10 */
        volatile uint32_t CS;           /**< \brief Message Buffer 0 CS Register..Message Buffer 63 CS Register, array offset: 0x80, array step: 0x10 */
        volatile uint32_t ID;           /**< \brief Message Buffer 0 ID Register..Message Buffer 63 ID Register, array offset: 0x84, array step: 0x10 */
        volatile uint32_t WORD[2];      /**< \brief Message Buffer 0 WORD_8B Register..Message Buffer 63 WORD_8B Register, array offset: 0x88, array step: index*0x10, index2*0x4 */
      } MB_8B[64];
      struct {                          /* offset: 0x80, array step: 0x10 */
        volatile uint32_t CS;           /**< \brief Message Buffer 0 CS Register..Message Buffer 63 CS Register, array offset: 0x80, array step: 0x10 */
        volatile uint32_t ID;           /**< \brief Message Buffer 0 ID Register..Message Buffer 63 ID Register, array offset: 0x84, array step: 0x10 */
        volatile uint32_t WORD0;        /**< \brief Message Buffer 0 WORD0 Register..Message Buffer 63 WORD0 Register, array offset: 0x88, array step: 0x10 */
        volatile uint32_t WORD1;        /**< \brief Message Buffer 0 WORD1 Register..Message Buffer 63 WORD1 Register, array offset: 0x8C, array step: 0x10 */
      } MB[64];
    };
             uint8_t  RESERVED_3[1024];
    volatile uint32_t RXIMR[64];        /**< \brief Rx Individual Mask Registers, array offset: 0x880, array step: 0x4 */
             uint8_t  RESERVED_4[96];
    volatile uint32_t GFWR;             /**< \brief Glitch Filter Width Registers, offset: 0x9E0 */
             uint8_t  RESERVED_5[524];
    volatile uint32_t EPRS;             /**< \brief Enhanced CAN Bit Timing Prescalers, offset: 0xBF0 */
    volatile uint32_t ENCBT;            /**< \brief Enhanced Nominal CAN Bit Timing, offset: 0xBF4 */
    volatile uint32_t EDCBT;            /**< \brief Enhanced Data Phase CAN bit Timing, offset: 0xBF8 */
    volatile uint32_t ETDC;             /**< \brief Enhanced Transceiver Delay Compensation, offset: 0xBFC */
    volatile uint32_t FDCTRL;           /**< \brief CAN FD Control Register, offset: 0xC00 */
    volatile uint32_t FDCBT;            /**< \brief CAN FD Bit Timing Register, offset: 0xC04 */
    volatile uint32_t FDCRC;            /**< \brief CAN FD CRC Register, offset: 0xC08 */
    volatile uint32_t ERFCR;            /**< \brief Enhanced Rx FIFO Control Register, offset: 0xC0C */
    volatile uint32_t ERFIER;           /**< \brief Enhanced Rx FIFO Interrupt Enable register, offset: 0xC10 */
    volatile uint32_t ERFSR;            /**< \brief Enhanced Rx FIFO Status Register, offset: 0xC14 */
             uint8_t  RESERVED_6[24];
    volatile uint32_t HR_TIME_STAMP[64];    /**< \brief High Resolution Time Stamp, array offset: 0xC30, array step: 0x4 */
             uint8_t  RESERVED_7[8912];
    volatile uint32_t ERFFEL[128];      /**< \brief Enhanced Rx FIFO Filter Element, array offset: 0x3000, array step: 0x4 */

} imx10xx_canfd_regs_t;

//todo
/* Module Configuration Register (MCR) */
#define     __MCR_MDIS_SHIFT                    (31U)
#define     __MCR_FRZ_SHIFT                     (30U)
#define     __MCR_RFEN_SHIFT                    (29U)
#define     __MCR_HALT_SHIFT                    (28U)
#define     __MCR_NOTRDY_SHIFT                  (27U)
#define     __MCR_WAKMSK_SHIFT                  (26U)
#define     __MCR_SOFTRST_SHIFT                 (25U)
#define     __MCR_FRZACK_SHIFT                  (24U)
#define     __MCR_SUPV_SHIFT                    (23U)
#define     __MCR_SLFWAK_SHIFT                  (22U)
#define     __MCR_WRNEN_SHIFT                   (21U)
#define     __MCR_LPMACK_SHIFT                  (20U)
#define     __MCR_WAKSRC_SHIFT                  (19U)
#define     __MCR_DOZE_SHIFT                    (18U)
#define     __MCR_SRXDIS_SHIFT                  (17U)
#define     __MCR_IRMQ_SHIFT                    (16U)
#define     __MCR_DMA_SHIFT                     (15U)
#define     __MCR_LPRIOEN_SHIFT                 (13U)
#define     __MCR_AEN_SHIFT                     (12U)
#define     __MCR_FDEN_SHIFT                    (11U)
#define     __MCR_IDAM_SHIFT                    (8U)
#define     __MCR_IDAM_LEN                      (2U)
#define     __MCR_MAXMB_SHIFT                   (0U)
#define     __MCR_MAXMB_LEN                     (7U)

/* Control 1 register (CTRL1) */
#define     __CTRL1_PERSDIV_SHIFT               (24U)
#define     __CTRL1_PERSDIV_LEN                 (8U)
#define     __CTRL1_RJW_SHIFT                   (22U)
#define     __CTRL1_RJW_LEN                     (2U)
#define     __CTRL1_PSEG1_SHIFT                 (19U)
#define     __CTRL1_PSEG1_LEN                   (3U)
#define     __CTRL1_PSEG2_SHIFT                 (16U)
#define     __CTRL1_PSEG2_LEN                   (3U)
#define     __CTRL1_BOFFMSK_SHIFT               (15U)
#define     __CTRL1_ERRMSK_SHIFT                (14U)
#define     __CTRL1_CLKSRC_SHIFT                (13U)
#define     __CTRL1_LPB_SHIFT                   (12U)
#define     __CTRL1_TWRNMSK_SHIFT               (11U)
#define     __CTRL1_RWRNMSK_SHIFT               (10U)
#define     __CTRL1_SMP_SHIFT                   (7U)
#define     __CTRL1_BOFFREC_SHIFT               (6U)
#define     __CTRL1_TSYN_SHIFT                  (5U)
#define     __CTRL1_LBUF_SHIFT                  (4U)
#define     __CTRL1_LOM_SHIFT                   (3U)
#define     __CTRL1_PROPSEG_SHIFT               (0U)
#define     __CTRL1_PROPSEG_LEN                 (3U)

/* Free Running Timer (TIMER) */
#define     __TIMER_TIMER_SHIFT                 (0U)
#define     __TIMER_TIMER_LEN                   (16U)

/* Rx Mailboxes Global Mask Register (RXMGMASK) */
#define     __RXMGMASK_MG_SHIFT                 (0U)
#define     __RXMGMASK_MG_LEN                   (32U)

/* Rx 14 Mask register (RX14MASK) */
#define     __RX14MASK_RX14M_SHIFT              (0U)
#define     __RX14MASK_RX14M_LEN                (32U)

/* Rx 15 Mask register (RX15MASK) */
#define     __RX15MASK_RX15M_SHIFT              (0U)
#define     __RX15MASK_RX15M_LEN                (32U)

/* Error Counter (ECR) */
#define     __ECR_RXERRCNT_FAST_SHIFT           (24U)
#define     __ECR_RXERRCNT_FAST_LEN             (8U)
#define     __ECR_TXERRCNT_FAST_SHIFT           (16U)
#define     __ECR_TXERRCNT_FAST_LEN             (8U)
#define     __ECR_RXERRCNT_SHIFT                (8U)
#define     __ECR_RXERRCNT_LEN                  (8U)
#define     __ECR_TXERRCNT_SHIFT                (0U)
#define     __ECR_TXERRCNT_LEN                  (8U)

/* Error and Status 1 register (ESR1) */
#define     __ESR1_BIT1ERR_FAST_SHIFT           (31U)
#define     __ESR1_BIT0ERR_FAST_SHIFT           (30U)
#define     __ESR1_CRCERR_FAST_SHIFT            (28U)
#define     __ESR1_FRMERR_FAST_SHIFT            (27U)
#define     __ESR1_STFERR_FAST_SHIFT            (26U)
#define     __ESR1_ERROVR_SHIFT                 (21U)
#define     __ESR1_ERRINT_FAST_SHIFT            (20U)
#define     __ESR1_BOFFDONEINT_SHIFT            (19U)
#define     __ESR1_SYNCH_SHIFT                  (18U)
#define     __ESR1_TWRNINT_SHIFT                (17U)
#define     __ESR1_RWRNINT_SHIFT                (16U)
#define     __ESR1_BIT1ERR_SHIFT                (15U)
#define     __ESR1_BIT0ERR_SHIFT                (14U)
#define     __ESR1_ACKERR_SHIFT                 (13U)
#define     __ESR1_CRCERR_SHIFT                 (12U)
#define     __ESR1_FRMERR_SHIFT                 (11U)
#define     __ESR1_STFERR_SHIFT                 (10U)
#define     __ESR1_TXWRN_SHIFT                  (9U)
#define     __ESR1_RXWRN_SHIFT                  (8U)
#define     __ESR1_IDLE_SHIFT                   (7U)
#define     __ESR1_TX_SHIFT                     (6U)
#define     __ESR1_FLTCONF_SHIFT                (4U)
#define     __ESR1_FLTCONF_LEN                  (2U)
#define     __ESR1_RX_SHIFT                     (3U)
#define     __ESR1_BOFFINT_SHIFT                (2U)
#define     __ESR1_ERRINT_SHIFT                 (1U)
#define     __ESR1_WAKINT_SHIFT                 (0U)

/* Interrupt Masks 2 register (IMASK2) */
#define     __IMASK2_BUF63TO32M_SHIFT           (0U)
#define     __IMASK2_BUF63TO32M_LEN             (32U)

/* Interrupt Masks 1 register (IMASK1) */
#define     __IMASK1_BUF31TO0M_SHIFT            (0U)
#define     __IMASK1_BUF31TO0M_LEN              (32U)

/* Interrupt Flags 2 register (IFLAG2) */
#define     __IFLAG2_BUF63TO32I_SHIFT           (0U)
#define     __IFLAG2_BUF63TO32I_LEN             (32U)

/* Interrupt Flags 1 register (IFLAG1) */
#define     __IFLAG1_BUF31TO8I_SHIFT            (16U)
#define     __IFLAG1_BUF31TO8I_LEN              (16U)
#define     __IFLAG1_BUF7I_SHIFT                (7U)
#define     __IFLAG1_BUF6I_SHIFT                (6U)
#define     __IFLAG1_BUF5I_SHIFT                (5U)
#define     __IFLAG1_BUF4TO1I_SHIFT             (1U)
#define     __IFLAG1_BUF4TO1I_LEN               (4U)
#define     __IFLAG1_BUF0I_SHIFT                (0U)

/* Control 2 register (CTRL2) */
#define     __CTRL2_ERRMSK_FAST_SHIFT           (31U)
#define     __CTRL2_BOFFDONEMSK_SHIFT           (30U)
#define     __CTRL2_RFFN_SHIFT                  (24U)
#define     __CTRL2_RFFN_LEN                    (4U)
#define     __CTRL2_TASD_SHIFT                  (19U)
#define     __CTRL2_TASD_LEN                    (5U)
#define     __CTRL2_MRP_SHIFT                   (18U)
#define     __CTRL2_RRS_SHIFT                   (17U)
#define     __CTRL2_EACEN_SHIFT                 (16U)
#define     __CTRL2_TIMER_SRC_SHIFT             (15U)
#define     __CTRL2_PREXCEN_SHIFT               (14U)
#define     __CTRL2_BTE_SHIFT                   (13U)
#define     __CTRL2_ISOCANFDEN_SHIFT            (12U)
#define     __CTRL2_EDFLTDIS_SHIFT              (11U)
#define     __CTRL2_MBTSBASE_SHIFT              (8U)
#define     __CTRL2_MBTSBASE_LEN                (2U)
#define     __CTRL2_TSTAMPCAP_SHIFT             (6U)
#define     __CTRL2_TSTAMPCAP_LEN               (2U)

/* Error and Status 2 register (ESR2) */
#define     __ESR2_LPTM_SHIFT                   (16U)
#define     __ESR2_LPTM_LEN                     (7U)
#define     __ESR2_VPS_SHIFT                    (14U)
#define     __ESR2_IMB_SHIFT                    (13U)

/* CRC Register (CRCR) */
#define     __CRCR_MBCRC_SHIFT                  (16U)
#define     __CRCR_MBCRC_LEN                    (7U)
#define     __CRCR_TXCRC_SHIFT                  (0U)
#define     __CRCR_TXCRC_LEN                    (15U)

/* Legacy Rx FIFO Global Mask register (RXFGMASK) */
#define     __RXFGMASK_FGM_SHIFT                (0U)
#define     __RXFGMASK_FGM_LEN                  (32U)

/* Legacy Rx FIFO Information Register (RXFIR) */
#define     __RXFIR_IDHIT_SHIFT                 (0U)
#define     __RXFIR_IDHIT_LEN                   (8U)

/* CAN Bit Timing Register (CBT) */
#define     __CBT_BTF_SHIFT                     (31U)
#define     __CBT_EPRESDIV_SHIFT                (21U)
#define     __CBT_EPRESDIV_LEN                  (10U)
#define     __CBT_ERJW_SHIFT                    (16U)
#define     __CBT_ERJW_LEN                      (5U)
#define     __CBT_EPROPSEG_SHIFT                (10U)
#define     __CBT_EPROPSEG_LEN                  (6U)
#define     __CBT_EPSEG1_SHIFT                  (5U)
#define     __CBT_EPSEG1_LEN                    (5U)
#define     __CBT_EPSEG2_SHIFT                  (0U)
#define     __CBT_EPSEG2_LEN                    (5U)

/* Enhanced CAN Bit Timing Prescalers (EPRS) */
#define     __EPRS_EDPRESDIV_SHIFT              (16U)
#define     __EPRS_EDPRESDIV_LEN                (10U)
#define     __EPRS_ENPRESDIV_SHIFT              (0U)
#define     __EPRS_ENPRESDIV_LEN                (10U)

/* Enhanced Nominal CAN Bit Timing (ENCBT) */
#define     __ENCBT_NRJW_SHIFT                  (22U)
#define     __ENCBT_NRJW_LEN                    (9U)
#define     __ENCBT_NTSEG2_SHIFT                (12U)
#define     __ENCBT_NTSEG2_LEN                  (7U)
#define     __ENCBT_NTSEG1_SHIFT                (0U)
#define     __ENCBT_NTSEG1_LEN                  (8U)

/* Enhanced Data Phase CAN bit Timing (EDCBT) */
#define     __EDCBT_DRJW_SHIFT                  (22U)
#define     __EDCBT_DRJW_LEN                    (4U)
#define     __EDCBT_DTSEG2_SHIFT                (12U)
#define     __EDCBT_DTSEG2_LEN                  (4U)
#define     __EDCBT_DTSEG1_SHIFT                (0U)
#define     __EDCBT_DTSEG1_LEN                  (5U)

/* Enhanced Transceiver Delay Compensation (ETDC)  */
#define     __ETDC_TDMDIS_SHIFT                 (31U)
#define     __ETDC_ETDCOFF_SHIFT                (16U)
#define     __ETDC_ETDCOFF_LEN                  (7U)
#define     __ETDC_ETDCVAL_SHIFT                (0U)
#define     __ETDC_ETDCVAL_LEN                  (8U)

/* CAN FD Control Register (FDCTRL) */
#define     __FDCTRL_FDRATE_SHIFT               (31U)
#define     __FDCTRL_MBDSR1_SHIFT               (19U)
#define     __FDCTRL_MBDSR1_LEN                 (2U)
#define     __FDCTRL_MBDSR0_SHIFT               (16U)
#define     __FDCTRL_MBDSR0_LEN                 (2U)
#define     __FDCTRL_TDCEN_SHIFT                (15U)
#define     __FDCTRL_TDCFAIL_SHIFT              (14U)
#define     __FDCTRL_TDCOFF_SHIFT               (8U)
#define     __FDCTRL_TDCOFF_LEN                 (5U)
#define     __FDCTRL_TDCVAL_SHIFT               (0U)
#define     __FDCTRL_TDCVAL_LEN                 (5U)

/* CAN FD Bit Timing Register (FDCBT) */
#define     __FDCBT_FPRESDIV_SHIFT              (20U)
#define     __FDCBT_FPRESDIV_LEN                (10U)
#define     __FDCBT_FRJW_SHIFT                  (16U)
#define     __FDCBT_FRJW_LEN                    (3U)
#define     __FDCBT_FPROPSEG_SHIFT              (10U)
#define     __FDCBT_FPROPSEG_LEN                (5U)
#define     __FDCBT_FPSEG1_SHIFT                (5U)
#define     __FDCBT_FPSEG1_LEN                  (3U)
#define     __FDCBT_FPSEG2_SHIFT                (0U)
#define     __FDCBT_FPSEG2_LEN                  (3U)


/* CAN FD CRC Register (FDCRC) */
#define     __FDCRC_FD_MBCRC_SHIFT              (24U)
#define     __FDCRC_FD_MBCRC_LEN                (7U)
#define     __FDCRC_FD_TXCRC_SHIFT              (0U)
#define     __FDCRC_FD_TXCRC_LEN                (21U)

/* Enhanced Rx FIFO Control Register (ERFCR) */
#define     __ERFCR_ERFEN_SHIFT                 (31U)
#define     __ERFCR_DMALW_SHIFT                 (26U)
#define     __ERFCR_DMALW_LEN                   (5U)
#define     __ERFCR_NEXIF_SHIFT                 (16U)
#define     __ERFCR_NEXIF_LEN                   (7U)
#define     __ERFCR_NFE_SHIFT                   (8U)
#define     __ERFCR_NFE_LEN                     (6U)
#define     __ERFCR_ERFWM_SHIFT                 (0U)
#define     __ERFCR_ERFWM_LEN                   (5U)

/* Enhanced Rx FIFO Interrupt Enable register (ERFIER) */
#define     __ERFIER_ERFUFWIE_SHIFT             (31U)
#define     __ERFIER_ERFOVFIE_SHIFT             (30U)
#define     __ERFIER_ERFWMIIE_SHIFT             (29U)
#define     __ERFIER_ERFDAIE_SHIFT              (28U)

/* Enhanced Rx FIFO Status Register (ERFSR) */
#define     __ERFSR_ERFUFW_SHIFT                (31U)
#define     __ERFSR_ERFOVF_SHIFT                (30U)
#define     __ERFSR_ERFWMI_SHIFT                (29U)
#define     __ERFSR_ERFDA_SHIFT                 (28U)
#define     __ERFSR_ERFCLR_SHIFT                (27U)
#define     __ERFSR_ERFE_SHIFT                  (17U)
#define     __ERFSR_ERFF_SHIFT                  (16U)
#define     __ERFSR_ERFEL_SHIFT                 (0U)
#define     __ERFSR_ERFEL_LEN                   (6U)



//todo
#define __CAN_MAX_LEN                   (0x8UL)
#define __CANFD_MAX_LEN                 (0x64UL)

#define __CAN_SFF_MASK                  (0x7FFUL)       /* standard frame format */
#define __CAN_EFF_MASK                  (0x1FFFFFFFUL)  /* extended frame format */

/* MBN BLOCK0 bit, MB offset 0x0 */
#define __CAN_MBN_BLK0_EDL_SHIFT        (31UL)
#define __CAN_MBN_BLK0_BRS_SHIFT        (30UL)
#define __CAN_MBN_BLK0_ESI_SHIFT        (29UL)
#define __CAN_MBN_BLK0_CODE_SHIFT       (24UL)
#define __CAN_MBN_BLK0_CODE_LEN         (4UL)
#define __CAN_MBN_BLK0_SRR_SHIFT        (22UL)
#define __CAN_MBN_BLK0_IDE_SHIFT        (21UL)
#define __CAN_MBN_BLK0_RTR_SHIFT        (20UL)
#define __CAN_MBN_BLK0_DLC_SHIFT        (16UL)
#define __CAN_MBN_BLK0_DLC_LEN          (4UL)
#define __CAN_MBN_BLK0_TS_SHIFT         (0UL)
#define __CAN_MBN_BLK0_TS_LEN           (16UL)

/* MBN BLOCK1 bit, MB offset 0x4 */
#define __CAN_MBN_BLK1_PRIO_SHIFT       (29UL)
#define __CAN_MBN_BLK1_PRIO_LEN         (3UL)
#define __CAN_MBN_BLK1_SID_SHIFT        (18UL)
#define __CAN_MBN_BLK1_SID_LEN          (11UL)
#define __CAN_MBN_BLK1_EID_SHIFT        (0UL)
#define __CAN_MBN_BLK1_EID_LEN          (29UL)

/* CANFD 数据长度超过8时, DLC编码定义 */
#define __CANFD_DLC_12                  (9UL)
#define __CANFD_DLC_16                  (10UL)
#define __CANFD_DLC_20                  (11UL)
#define __CANFD_DLC_24                  (12UL)
#define __CANFD_DLC_32                  (13UL)
#define __CANFD_DLC_48                  (14UL)
#define __CANFD_DLC_64                  (15UL)

/* MBN CODE description */
#define __CAN_MBN_CODE_RX_INACTIVE          (0UL)
#define __CAN_MBN_CODE_RX_EMPTY             (4UL)
#define __CAN_MBN_CODE_RX_FULL              (2UL)
#define __CAN_MBN_CODE_RX_OVERRUN           (6UL)
#define __CAN_MBN_CODE_RX_RANSWER           (0xAUL)
#define __CAN_MBN_CODE_RX_BUSY              (1UL)
#define __CAN_MBN_CODE_TX_INACTIVE          (8UL)
#define __CAN_MBN_CODE_TX_ABORT             (9UL)
#define __CAN_MBN_CODE_TX_DATA_OR_REMOTE    (0xCUL)     /* MB is TX Data Frame (when MB RTR = 0) */
                                                        /* or MB is a TX Remote Request Frame (when MB RTR = 1) */
#define __CAN_MBN_CODE_TX_TANSWER           (0xEUL)



/* MBn count */
#define __CAN_MAX_MBN           (11UL)

#define __CAN_MB_64B_BLK_MAX    (18UL)
#define __CAN_MB_32B_BLK_MAX    (10UL)
#define __CAN_MB_16B_BLK_MAX    (6UL)
#define __CAN_MB_8B_BLK_MAX     (4UL)

#define __CAN_MBN_RECV_CHN_START    (0UL)    /* 接收通道的MB 起始号 */
#define __CAN_MBN_RECV_CHN_NUM      (10UL)   /* 接收通道的MB 数量，必须为偶数，只有使用其中一半，另一半备用防止相同id帧连续来时第二帧丢失 */
#define __CAN_MBN_SEND_CHN          (10UL)    /* 发送通道的MB */
//#define __CAN_MBN_SEND_CHN_NUM      (1UL)   /* 发送通道的MB 数量 */


/** \brief 从p_serv获得canfd_dev宏操作 */
#define __IMX10xx_CANFD_GET_THIS(p_serv) \
        AW_CONTAINER_OF(p_serv, struct awbl_imx10xx_canfd_dev, can_serv)

/** \brief 从p_dev获得canfd_dev宏操作 */
#define __IMX10xx_CANFD_GET_DEV(p_dev) \
        AW_CONTAINER_OF(p_dev, struct awbl_imx10xx_canfd_dev, dev)

/** \brief 从p_dev获得can_devinfo宏操作 */
#define __IMX10xx_CANFD_GET_INFO(p_dev) \
        ((struct awbl_imx10xx_canfd_dev_info *)AWBL_DEVINFO_GET(p_dev))


/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __imx10xx_can_inst_init (awbl_dev_t *p_dev);
aw_local void __imx10xx_can_inst_init2 (awbl_dev_t *p_dev);
aw_local void __imx10xx_can_inst_connect (awbl_dev_t *p_dev);

aw_local aw_can_err_t __imx10xx_can_init (
    struct awbl_can_service *p_serv, aw_can_work_mode_type_t work_mode);

aw_local aw_can_err_t __imx10xx_can_start (struct awbl_can_service *p_serv);
aw_local aw_can_err_t __imx10xx_can_set_haltfrz (
    struct awbl_can_service *p_serv);
aw_local aw_can_err_t __imx10xx_can_reset_haltfrz (
    struct awbl_can_service *p_serv);
aw_local aw_can_err_t __imx10xx_can_reset (struct awbl_can_service *p_serv);
aw_local aw_can_err_t __imx10xx_can_sleep (struct awbl_can_service *p_serv);
aw_local aw_can_err_t __imx10xx_can_wakeup (struct awbl_can_service *p_serv);

aw_local aw_can_err_t __imx10xx_can_int_enable (
    struct awbl_can_service *p_serv, awbl_can_int_type_t int_mask);
aw_local aw_can_err_t __imx10xx_can_int_disable (
    struct awbl_can_service *p_serv, awbl_can_int_type_t int_mask);
aw_local aw_can_err_t __imx10xx_can_baudrate_set (
    struct awbl_can_service *p_serv, const aw_can_baud_param_t *p_can_btr0);
aw_local aw_can_err_t __imx10xx_can_baudrate_get (
    struct awbl_can_service *p_serv, aw_can_baud_param_t *p_can_btr0);
aw_local aw_can_err_t __imx10xx_can_errreg_cnt_get (
    struct awbl_can_service *p_serv, aw_can_err_cnt_t *p_can_err_reg);
aw_local aw_can_err_t __imx10xx_can_errreg_cnt_clr (
    struct awbl_can_service *p_serv);
aw_local aw_can_err_t __imx10xx_can_reg_msg_read (
    struct awbl_can_service *p_serv, struct aw_can_msg *p_canmsg);
aw_local aw_can_err_t __imx10xx_can_reg_msg_write (
    struct awbl_can_service *p_serv, struct aw_can_msg *p_canmsg);
aw_local aw_can_err_t __imx10xx_can_msg_snd_stop (
    struct awbl_can_service *p_serv);

aw_local aw_can_err_t __imx10xx_can_filter_table_set (
    struct awbl_can_service *p_serv, uint8_t *p_filterbuff, size_t length);
aw_local aw_can_err_t __imx10xx_can_filter_table_get (
    struct awbl_can_service *p_serv,
    uint8_t                 *p_filterbuff,
    size_t                  *p_length);
aw_local aw_can_err_t __imx10xx_can_intstatus_get (
    struct awbl_can_service *p_serv,
    awbl_can_int_type_t     *p_int_type,
    aw_can_bus_err_t        *p_bus_err);


/*******************************************************************************
    globals
*******************************************************************************/
//todo
/** \brief sio driver functions */
static awbl_can_drv_funcs_t __g_imx10xx_flexcanfd_drv_funcs = {
    __imx10xx_can_init,
    __imx10xx_can_start,
    __imx10xx_can_set_haltfrz,
    __imx10xx_can_sleep,
    __imx10xx_can_wakeup,
    __imx10xx_can_int_enable,
    __imx10xx_can_int_disable,
    __imx10xx_can_baudrate_set,
    __imx10xx_can_baudrate_get,
    __imx10xx_can_errreg_cnt_get,
    __imx10xx_can_errreg_cnt_clr,
    __imx10xx_can_reg_msg_read,
    __imx10xx_can_reg_msg_write,
    __imx10xx_can_msg_snd_stop,
    __imx10xx_can_filter_table_set,
    __imx10xx_can_filter_table_get,
    __imx10xx_can_intstatus_get,
    NULL,
    NULL
};


aw_const struct awbl_drvfuncs g_imx10xx_canfd_awbl_drvfuncs = {
    __imx10xx_can_inst_init,        /* devInstanceInit */
    __imx10xx_can_inst_init2,       /* devInstanceInit2 */
    __imx10xx_can_inst_connect      /* devConnect */
};



/*******************************************************************************
    static
*******************************************************************************/

/**
 * driver registration (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvinfo __g_imx10xx_canfd_drv_registration = {
    AWBL_VER_1,                         /* awb_ver */
    AWBL_BUSID_PLB,                     /* bus_id */
    AWBL_IMX10XX_CANFD_NAME,            /* p_drvname*/
    &g_imx10xx_canfd_awbl_drvfuncs,     /* p_busfuncs */
    NULL,                               /* p_methods */
    NULL                                /* pfunc_drv_probe */
};


/**
 * \brief initialize imx10xx can device
 *
 * This is the imx10xx can initialization routine.
 *
 * \note
 * This routine is called early during system initialization, and
 * *MUST NOT* make calls to OS facilities such as memory allocation and I/O.
 */
aw_local void __imx10xx_can_inst_init (awbl_dev_t *p_dev)
{
    return;
}


/**
 * \brief initialize imx10xx can device
 *
 * This routine initializes the imx10xx can device.
 */
aw_local void __imx10xx_can_inst_init2 (awbl_dev_t *p_dev)
{

    awbl_imx10xx_canfd_dev_t       *p_this;
    awbl_imx10xx_canfd_dev_info_t  *p_info;

    /* get the device */
    p_this = __IMX10xx_CANFD_GET_DEV(p_dev);

    /* get the device info */
    p_info = __IMX10xx_CANFD_GET_INFO(p_dev);

    /* Call platform initialize function first */
    if (NULL != p_info->pfn_plfm_init) {
        p_info->pfn_plfm_init();
    }

    awbl_can_service_init(&(p_this->can_serv),
                          &p_info->servinfo,
                          &__g_imx10xx_flexcanfd_drv_funcs);

    aw_int_connect(p_info->int_num,\
                   (aw_pfuncvoid_t)awbl_can_isr_handler,\
                   (void*)&p_this->can_serv);

    aw_int_enable(p_info->int_num);

    awbl_can_service_register(&p_this->can_serv);

}

/**
 * \brief connect imx10xx can device to system
 *
 * Nothing to do here.  We want serial channels available for output reasonably
 * early during the boot process.  Because this is a serial channel, we connect
 * to the I/O system in __imx10xx_can_inst_init2() instead of here.
 */
aw_local void __imx10xx_can_inst_connect (awbl_dev_t *p_dev)
{
    return;
}


/* 复位所有的状态 */
static void __imx10xx_reset_status (struct awbl_can_service *p_serv)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    uint32_t i;

    AW_REG_BIT_CLR32(&p_reg->MCR, __MCR_DOZE_SHIFT);
    while (AW_REG_BIT_ISSET32(&p_reg->MCR, __MCR_LPMACK_SHIFT));

    __imx10xx_can_reset(p_serv);

    AW_REG_BIT_SET32(&p_reg->MCR, __MCR_WRNEN_SHIFT);
    AW_REG_BIT_SET32(&p_reg->MCR, __MCR_WAKSRC_SHIFT);
    AW_REG_BITS_SET32(&p_reg->MCR,
                      __MCR_MAXMB_SHIFT,
                      __MCR_MAXMB_LEN,
                      63);

    AW_REG_WRITE32(&p_reg->CTRL1, 0);

    AW_REG_BITS_SET32(&p_reg->CTRL2, __CTRL2_TASD_SHIFT, __CTRL2_TASD_LEN, 0x16);
//    AW_REG_BIT_SET32(&p_reg->CTRL2, __CTRL2_RRS_SHIFT);
//    AW_REG_BIT_SET32(&p_reg->CTRL2, __CTRL2_EACEN_SHIFT);

    for (i = 0; i < 64; i++) {
        AW_REG_WRITE32(&p_reg->MB[i].CS, 0);
    }
    for (i = 0; i < 7; i++) {
        AW_REG_WRITE32(&(p_reg->MB_64B.MB0[i].CS), 0);
        AW_REG_WRITE32(&(p_reg->MB_64B.MB7[i].CS), 0);
    }

}


/** \brief CAN 底层部分的初始化 */
aw_local aw_can_err_t __imx10xx_can_init (
    struct awbl_can_service *p_serv, aw_can_work_mode_type_t work_mode)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    aw_can_fd_baud_param_t canfd_btr = {{15,  4,   4,   0,   2  },
                                        {15,  4,   4,   0,   1  }};

    uint32_t i;

    /* check param */
    if ((p_this == NULL) ||
        (p_info == NULL) ||
        (p_reg  == NULL)) {
        return -AW_CAN_ERR_INVALID_PARAMETER;
    }


    /* choose CAN Engine clock source */
    AW_REG_BIT_SET32(&p_reg->MCR, __MCR_MDIS_SHIFT);
    while (!AW_REG_BIT_ISSET32(&p_reg->MCR, __MCR_LPMACK_SHIFT));
    AW_REG_BIT_SET32(&p_reg->CTRL1, __CTRL1_CLKSRC_SHIFT);
    AW_REG_BIT_CLR32(&p_reg->MCR, __MCR_MDIS_SHIFT);
    while (AW_REG_BIT_ISSET32(&p_reg->MCR, __MCR_LPMACK_SHIFT));

    __imx10xx_reset_status(p_serv);

    AW_REG_BIT_SET32(&p_reg->CBT, __CBT_BTF_SHIFT);

    /* 配置一次波特率 */
    __imx10xx_can_baudrate_set(p_serv, (const aw_can_baud_param_t *)&canfd_btr);

    /* 进入冻结模式 */
    __imx10xx_can_set_haltfrz(p_serv);

    /* 复位过滤寄存器，使之不进行过滤 */
    for (i = 0; i < 64; i++) {
        AW_REG_WRITE32(&p_reg->RXIMR[i], 0x0);
    }
    AW_REG_WRITE32(&p_reg->RXMGMASK, 0x0);          /* 0代码不进行ID过滤检查 */
    AW_REG_WRITE32(&p_reg->RX14MASK, 0x0);
    AW_REG_WRITE32(&p_reg->RX15MASK, 0x0);
    AW_REG_WRITE32(&p_reg->RXFGMASK, 0x0);


    AW_REG_BITS_SET32(&p_reg->MCR,
                      __MCR_MAXMB_SHIFT,
                      __MCR_MAXMB_LEN,
                      __CAN_MAX_MBN - 1);


    AW_REG_BIT_SET32(&p_reg->MCR, __MCR_WRNEN_SHIFT);

    /* loop back mode disable, this mode is used to test */
    AW_REG_BIT_CLR32(&p_reg->CTRL1, __CTRL1_LPB_SHIFT);

    /* Timer Sync enable */
    AW_REG_BIT_SET32(&p_reg->CTRL1, __CTRL1_TSYN_SHIFT);

    /* listen only */
    if (work_mode & AW_CAN_WORK_MODE_LISTEN_ONLY) {
        AW_REG_BIT_SET32(&p_reg->CTRL1, __CTRL1_LOM_SHIFT);
    } else {
        AW_REG_BIT_CLR32(&p_reg->CTRL1, __CTRL1_LOM_SHIFT);
    }

    /* non-ISO CANFD? */
    if (work_mode & AW_CAN_WORK_MODE_NON_ISO) {
        AW_REG_BIT_CLR32(&p_reg->CTRL2, __CTRL2_ISOCANFDEN_SHIFT);
    } else {
        AW_REG_BIT_SET32(&p_reg->CTRL2, __CTRL2_ISOCANFDEN_SHIFT);
    }

    AW_REG_BIT_SET32(&p_reg->MCR, __MCR_LPRIOEN_SHIFT);

    /* Remote Request Storing */
    AW_REG_BIT_SET32(&p_reg->CTRL2, __CTRL2_RRS_SHIFT);

    /* Entire Frame Arbitration Field Comparison Enable For Rx Mailboxes */
    AW_REG_BIT_SET32(&p_reg->CTRL2, __CTRL2_EACEN_SHIFT);   /* 设置为1时通过RXMGASK[bit30]来控制是否与MB[RCV].CS[bit21]进行对比；设置为0时将直接与MB[RCV].CS[bit21]进行对比，只接收与MB[RCV].CS[bit21]相同的帧*/

    /* Transceiver Delay Compensation Enable */
    AW_REG_BIT_SET32(&p_reg->FDCTRL, __FDCTRL_TDCEN_SHIFT);

    AW_REG_BIT_SET32(&p_reg->MCR, __MCR_SRXDIS_SHIFT);  /* self reception disable */
    AW_REG_BIT_SET32(&p_reg->MCR, __MCR_AEN_SHIFT);     /* 使能中止传输 */
    AW_REG_BIT_CLR32(&p_reg->MCR, __MCR_RFEN_SHIFT);    /* 禁能Legacy Rx FIFO,在CANFD模式下不能使能此位 */
    AW_REG_BIT_SET32(&p_reg->MCR, __MCR_IRMQ_SHIFT);    /* 使能独立滤波掩码 */

    if (p_info->servinfo.ctr_type == AW_CAN_CTLR_HS) {
        AW_REG_BIT_CLR32(&p_reg->MCR, __MCR_FDEN_SHIFT);

        /* 初始化接收MB */
        for (i = __CAN_MBN_RECV_CHN_START; i < __CAN_MBN_RECV_CHN_START + __CAN_MBN_RECV_CHN_NUM; i++) {
            AW_REG_BITS_SET32(&(p_reg->MB_8B[i].CS),
                              __CAN_MBN_BLK0_CODE_SHIFT,
                              __CAN_MBN_BLK0_CODE_LEN,
                              __CAN_MBN_CODE_RX_EMPTY);
        }

        /* 初始化发送MB */
        AW_REG_BITS_SET32(&(p_reg->MB_8B[__CAN_MBN_SEND_CHN].CS),
                          __CAN_MBN_BLK0_CODE_SHIFT,
                          __CAN_MBN_BLK0_CODE_LEN,
                          __CAN_MBN_CODE_TX_INACTIVE);

    } else if (p_info->servinfo.ctr_type == AW_CAN_CTLR_FD) {
        AW_REG_BIT_SET32(&p_reg->MCR, __MCR_FDEN_SHIFT);
        AW_REG_BIT_SET32(&p_reg->FDCTRL, __FDCTRL_FDRATE_SHIFT);

        /* 设置MB的最大的有效字节数据为最大的64字节 */
        AW_REG_BITS_SET32(&p_reg->FDCTRL,
                          __FDCTRL_MBDSR0_SHIFT,
                          __FDCTRL_MBDSR0_LEN,
                          3);
        AW_REG_BITS_SET32(&p_reg->FDCTRL,
                          __FDCTRL_MBDSR1_SHIFT,
                          __FDCTRL_MBDSR1_LEN,
                          3);

        /* 初始化接收MB */
        for (i = __CAN_MBN_RECV_CHN_START; i < __CAN_MBN_RECV_CHN_START + __CAN_MBN_RECV_CHN_NUM; i++) {
            if (i < 7) {
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB0[i].CS),
                                  __CAN_MBN_BLK0_CODE_SHIFT,
                                  __CAN_MBN_BLK0_CODE_LEN,
                                  __CAN_MBN_CODE_RX_EMPTY);
            } else {
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB7[i - 7].CS),
                                  __CAN_MBN_BLK0_CODE_SHIFT,
                                  __CAN_MBN_BLK0_CODE_LEN,
                                  __CAN_MBN_CODE_RX_EMPTY);
            }
        }

        /* 初始化发送MB */
        if (__CAN_MBN_SEND_CHN < 7) {
            AW_REG_BITS_SET32(&(p_reg->MB_64B.MB0[__CAN_MBN_SEND_CHN].CS),
                              __CAN_MBN_BLK0_CODE_SHIFT,
                              __CAN_MBN_BLK0_CODE_LEN,
                              __CAN_MBN_CODE_TX_INACTIVE);
        } else {
            AW_REG_BITS_SET32(&(p_reg->MB_64B.MB7[__CAN_MBN_SEND_CHN - 7].CS),
                              __CAN_MBN_BLK0_CODE_SHIFT,
                              __CAN_MBN_BLK0_CODE_LEN,
                              __CAN_MBN_CODE_TX_INACTIVE);
        }
    }

#ifdef CANFD_AND_CAN_SWITCH_DYNAMICALLY
    p_this->ctrl_type = p_info->servinfo.ctr_type;  /* 记录当前控制器类型 */
#endif

    /* 退出冻结模式  */
    __imx10xx_can_reset_haltfrz(p_serv);

    return AW_CAN_ERR_NONE;
}

/** \brief CAN启动 */
aw_local aw_can_err_t __imx10xx_can_start (struct awbl_can_service *p_serv)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    AW_REG_BIT_CLR32(&p_reg->MCR, __MCR_MDIS_SHIFT);
    AW_REG_BIT_CLR32(&p_reg->MCR, __MCR_HALT_SHIFT);

    while (AW_REG_BIT_ISSET32(&p_reg->MCR, __MCR_LPMACK_SHIFT));

    return AW_CAN_ERR_NONE;
}

/** \brief CAN 进入冻结模式  */
aw_local aw_can_err_t __imx10xx_can_set_haltfrz (
    struct awbl_can_service *p_serv)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    do {
        AW_REG_BIT_SET32(&p_reg->MCR, __MCR_FRZ_SHIFT);
        AW_REG_BIT_SET32(&p_reg->MCR, __MCR_HALT_SHIFT);
    } while (!AW_REG_BIT_ISSET32(&p_reg->MCR, __MCR_FRZACK_SHIFT));

    return AW_CAN_ERR_NONE;
}

/** \brief CAN 退出冻结模式  */
aw_local aw_can_err_t __imx10xx_can_reset_haltfrz (
    struct awbl_can_service *p_serv)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    do {
        AW_REG_BIT_CLR32(&p_reg->MCR, __MCR_HALT_SHIFT);
        AW_REG_BIT_CLR32(&p_reg->MCR, __MCR_FRZ_SHIFT);
    } while (AW_REG_BIT_ISSET32(&p_reg->MCR, __MCR_FRZACK_SHIFT));


    return AW_CAN_ERR_NONE;
}

/** \brief CAN复位（停止） */
aw_local aw_can_err_t __imx10xx_can_reset (struct awbl_can_service *p_serv)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    AW_REG_BIT_SET32(&p_reg->MCR, __MCR_SOFTRST_SHIFT);

    while (AW_REG_BIT_ISSET32(&p_reg->MCR, __MCR_SOFTRST_SHIFT));

    return AW_CAN_ERR_NONE;
}

/** \brief CAN休眠  */
aw_local aw_can_err_t __imx10xx_can_sleep (struct awbl_can_service *p_serv)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    if (AW_REG_BIT_ISSET32(&p_reg->MCR, __MCR_SOFTRST_SHIFT)) {
        return -AW_CAN_ERR_SLEEP_IN_CFG_MODE;
    }

    AW_REG_BIT_SET32(&p_reg->MCR, __MCR_DOZE_SHIFT);

    while (!AW_REG_BIT_ISSET32(&p_reg->MCR, __MCR_LPMACK_SHIFT));

    return AW_CAN_ERR_NONE;
}

/** \brief CAN唤醒 */
aw_local aw_can_err_t __imx10xx_can_wakeup (struct awbl_can_service *p_serv)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    AW_REG_BIT_CLR32(&p_reg->MCR, __MCR_DOZE_SHIFT);

    while (AW_REG_BIT_ISSET32(&p_reg->MCR, __MCR_LPMACK_SHIFT));

    return AW_CAN_ERR_NONE;
}

/** \brief 中断使能  */
aw_local aw_can_err_t __imx10xx_can_int_enable (
    struct awbl_can_service *p_serv, awbl_can_int_type_t int_mask)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);
    uint32_t                     i;

    /* 在冻结模式下配置 */
    __imx10xx_can_set_haltfrz(p_serv);

    if (int_mask & AWBL_CAN_INT_TX) {
//        for (i = 0; i < __CAN_MBN_SEND_CHN_NUM; i++) {
//            AW_REG_BIT_SET32(&p_reg->IMASK1, __CAN_MBN_SEND_CHN_START + i);
//        }
        AW_REG_BIT_SET32(&p_reg->IMASK1, __CAN_MBN_SEND_CHN);
    }

    if (int_mask & AWBL_CAN_INT_RX) {
        for (i = 0; i < __CAN_MBN_RECV_CHN_NUM; i++) {
            AW_REG_BIT_SET32(&p_reg->IMASK1, __CAN_MBN_RECV_CHN_START + i);
        }
    }

    if (int_mask & AWBL_CAN_INT_WAKE_UP) {
        AW_REG_BIT_SET32(&p_reg->MCR, __MCR_WAKMSK_SHIFT);
    }

    if (int_mask & AWBL_CAN_INT_ERROR) {
        AW_REG_BIT_SET32(&p_reg->CTRL1, __CTRL1_ERRMSK_SHIFT);
    }

    if (int_mask & AWBL_CAN_INT_DATAOVER) {
        //此MCU无数据超载中断
    }

    if (int_mask & AWBL_CAN_INT_WARN) {
        AW_REG_BIT_SET32(&p_reg->MCR, __MCR_WRNEN_SHIFT);
        AW_REG_BIT_SET32(&p_reg->CTRL1, __CTRL1_TWRNMSK_SHIFT);
        AW_REG_BIT_SET32(&p_reg->CTRL1, __CTRL1_RWRNMSK_SHIFT);
    }

    if (int_mask & AWBL_CAN_INT_ERROR_PASSIVE) {
        /* 此MCU无总线消极中断 */
    }

    if (int_mask & AWBL_CAN_INT_BUS_OFF) {
        AW_REG_BIT_SET32(&p_reg->CTRL1, __CTRL1_BOFFMSK_SHIFT);
    }

    __imx10xx_can_reset_haltfrz(p_serv);

    return AW_CAN_ERR_NONE;
}

/** \brief 中断禁能  */
aw_local aw_can_err_t __imx10xx_can_int_disable (
    struct awbl_can_service *p_serv, awbl_can_int_type_t int_mask)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    uint32_t                     i;

    /* 在冻结模式下配置 */
    __imx10xx_can_set_haltfrz(p_serv);

    if (int_mask & AWBL_CAN_INT_TX) {
//        for (i = 0; i < __CAN_MBN_SEND_CHN_NUM; i++) {
//            AW_REG_BIT_CLR32(&p_reg->IMASK1, __CAN_MBN_SEND_CHN_START + i);
//        }
        AW_REG_BIT_CLR32(&p_reg->IMASK1, __CAN_MBN_SEND_CHN);
    }

    if (int_mask & AWBL_CAN_INT_RX) {
        for (i = 0; i < __CAN_MBN_RECV_CHN_NUM; i++) {
            AW_REG_BIT_CLR32(&p_reg->IMASK1, __CAN_MBN_RECV_CHN_START + i);
        }
    }

    if (int_mask & AWBL_CAN_INT_WAKE_UP) {
        AW_REG_BIT_CLR32(&p_reg->MCR, __MCR_WAKMSK_SHIFT);
    }

    if (int_mask & AWBL_CAN_INT_ERROR) {
        AW_REG_BIT_CLR32(&p_reg->CTRL1, __CTRL1_ERRMSK_SHIFT);
    }

    if (int_mask & AWBL_CAN_INT_DATAOVER) {
        //此MCU无数据超载中断
    }

    if (int_mask & AWBL_CAN_INT_WARN) {
        AW_REG_BIT_CLR32(&p_reg->MCR, __MCR_WRNEN_SHIFT);
        AW_REG_BIT_CLR32(&p_reg->CTRL1, __CTRL1_TWRNMSK_SHIFT);
        AW_REG_BIT_CLR32(&p_reg->CTRL1, __CTRL1_RWRNMSK_SHIFT);
    }

    if (int_mask & AWBL_CAN_INT_ERROR_PASSIVE) {
        /* 此MCU无总线消极中断 */
    }

    if (int_mask & AWBL_CAN_INT_BUS_OFF) {
        AW_REG_BIT_CLR32(&p_reg->CTRL1, __CTRL1_BOFFMSK_SHIFT);
    }

    __imx10xx_can_reset_haltfrz(p_serv);

    return AW_CAN_ERR_NONE;
}

/** \brief 波特率设置 */
aw_local aw_can_err_t __imx10xx_can_baudrate_set (
    struct awbl_can_service *p_serv, const aw_can_baud_param_t *p_can_btr0)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);
    aw_can_fd_baud_param_t      *p_canfd_btr = (aw_can_fd_baud_param_t *)p_can_btr0;
    uint8_t                      pseg1, prop;

    __imx10xx_can_set_haltfrz(p_serv);

#ifdef CANFD_AND_CAN_SWITCH_DYNAMICALLY
    if (p_this->ctrl_type == AW_CAN_CTLR_HS) {
#else
    /* 高速CAN控制器 */
    if (p_info->servinfo.ctr_type == AW_CAN_CTLR_HS) {
#endif
        /* | SYNC_SEG |     Time Segment 1     | Time Segment 2 | */
        /* |          | (PROP_SEG + PSEG1 + 2) |   (PSEG2 + 1)  | */

        if ((p_can_btr0->tseg1 - 2) > (AW_BITS_MASK(__CTRL1_PSEG1_LEN))) {
            pseg1 = AW_BITS_MASK(__CTRL1_PSEG1_LEN);
            prop = (p_can_btr0->tseg1 - 2) - pseg1;
        } else {
            pseg1 = p_can_btr0->tseg1 - 2;
            prop = 0;
        }

        AW_REG_BITS_SET32(&p_reg->CBT,
                          __CBT_EPRESDIV_SHIFT,
                          __CBT_EPRESDIV_LEN,
                          p_can_btr0->brp - 1);

        AW_REG_BITS_SET32(&p_reg->CBT,
                          __CBT_ERJW_SHIFT,
                          __CBT_ERJW_LEN,
                          p_can_btr0->sjw - 1);

        AW_REG_BITS_SET32(&p_reg->CBT,
                          __CBT_EPSEG1_SHIFT,
                          __CBT_EPSEG1_LEN,
                          pseg1);

        AW_REG_BITS_SET32(&p_reg->CBT,
                          __CBT_EPROPSEG_SHIFT,
                          __CBT_EPROPSEG_LEN,
                          prop);

        AW_REG_BITS_SET32(&p_reg->CBT,
                          __CBT_EPSEG2_SHIFT,
                          __CBT_EPSEG2_LEN,
                          p_can_btr0->tseg2 - 1);

        if (p_can_btr0->smp) {
            AW_REG_BIT_SET32(&p_reg->CTRL1, __CTRL1_SMP_SHIFT);
        } else {
            AW_REG_BIT_CLR32(&p_reg->CTRL1, __CTRL1_SMP_SHIFT);
        }

    } else {

        /* | SYNC_SEG |     Time Segment 1     | Time Segment 2 | */
        /* |          | (PROP_SEG + PSEG1 + 2) |   (PSEG2 + 1)  | */

        if ((p_canfd_btr->nominal_baud.tseg1 - 2) > (AW_BITS_MASK(__CBT_EPSEG1_LEN))) {
            pseg1 = AW_BITS_MASK(__CBT_EPSEG1_LEN);
            prop = (p_canfd_btr->nominal_baud.tseg1 - 2) - pseg1;
        } else {
            pseg1 = p_canfd_btr->nominal_baud.tseg1 - 2;
            prop = 0;
        }

        AW_REG_BITS_SET32(&p_reg->CBT,
                          __CBT_EPRESDIV_SHIFT,
                          __CBT_EPRESDIV_LEN,
                          p_canfd_btr->nominal_baud.brp - 1);

        AW_REG_BITS_SET32(&p_reg->CBT,
                          __CBT_ERJW_SHIFT,
                          __CBT_ERJW_LEN,
                          p_canfd_btr->nominal_baud.sjw - 1);

        AW_REG_BITS_SET32(&p_reg->CBT,
                          __CBT_EPSEG1_SHIFT,
                          __CBT_EPSEG1_LEN,
                          pseg1);

        AW_REG_BITS_SET32(&p_reg->CBT,
                          __CBT_EPROPSEG_SHIFT,
                          __CBT_EPROPSEG_LEN,
                          prop);

        AW_REG_BITS_SET32(&p_reg->CBT,
                          __CBT_EPSEG2_SHIFT,
                          __CBT_EPSEG2_LEN,
                          p_canfd_btr->nominal_baud.tseg2 - 1);

        /* 在CANFD模式下只用使用单次采样 */
        AW_REG_BIT_CLR32(&p_reg->CTRL1, __CTRL1_SMP_SHIFT);


        /* | SYNC_SEG |     Time Segment 1     | Time Segment 2 | */
        /* |          | (PROP_SEG + PSEG1 + 2) |   (PSEG2 + 1)  | */

        if ((p_canfd_btr->data_baud.tseg1 - 1) > (AW_BITS_MASK(__FDCBT_FPSEG1_LEN))) {
            pseg1 = AW_BITS_MASK(__FDCBT_FPSEG1_LEN);
            prop = (p_canfd_btr->data_baud.tseg1 - 1) - pseg1;
        } else {
            pseg1 = p_canfd_btr->data_baud.tseg1 - 1;
            prop = 0;
        }

        AW_REG_BITS_SET32(&p_reg->FDCBT,
                          __FDCBT_FPRESDIV_SHIFT,
                          __FDCBT_FPRESDIV_LEN,
                          p_canfd_btr->data_baud.brp - 1);

        AW_REG_BITS_SET32(&p_reg->FDCBT,
                          __FDCBT_FRJW_SHIFT,
                          __FDCBT_FRJW_LEN,
                          p_canfd_btr->data_baud.sjw - 1);

        AW_REG_BITS_SET32(&p_reg->FDCBT,
                          __FDCBT_FPSEG1_SHIFT,
                          __FDCBT_FPSEG1_LEN,
                          pseg1);

        AW_REG_BITS_SET32(&p_reg->FDCBT,
                          __FDCBT_FPROPSEG_SHIFT,
                          __FDCBT_FPROPSEG_LEN,
                          prop);

        AW_REG_BITS_SET32(&p_reg->FDCBT,
                          __FDCBT_FPSEG2_SHIFT,
                          __FDCBT_FPSEG2_LEN,
                          p_canfd_btr->data_baud.tseg2 - 1);


        AW_REG_BITS_SET32(&p_reg->FDCTRL,
                          __FDCTRL_TDCOFF_SHIFT,
                          __FDCTRL_TDCOFF_LEN,
                          pseg1 + prop + 2);
    }

    __imx10xx_can_reset_haltfrz(p_serv);

    return AW_CAN_ERR_NONE;
}

/** \brief 波特率获取 */
aw_local aw_can_err_t __imx10xx_can_baudrate_get (
    struct awbl_can_service *p_serv, aw_can_baud_param_t *p_can_btr0)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);
    aw_can_fd_baud_param_t        *p_canfd_btr = (aw_can_fd_baud_param_t *)p_can_btr0;
    volatile uint32_t              reg;

#ifdef CANFD_AND_CAN_SWITCH_DYNAMICALLY
    if (p_this->ctrl_type == AW_CAN_CTLR_HS) {
#else
    /* 高速CAN控制器 */
    if (p_info->servinfo.ctr_type == AW_CAN_CTLR_HS) {
#endif

        reg = AW_REG_READ32(&p_reg->CBT);
        p_can_btr0->brp   = AW_BITS_GET(reg, __CBT_EPRESDIV_SHIFT, __CBT_EPRESDIV_LEN) + 1;
        p_can_btr0->sjw   = AW_BITS_GET(reg, __CBT_ERJW_SHIFT, __CBT_ERJW_LEN) + 1;
        p_can_btr0->tseg1 = AW_BITS_GET(reg, __CBT_EPSEG1_SHIFT, __CBT_EPSEG1_LEN) + \
                            AW_BITS_GET(reg, __CBT_EPROPSEG_SHIFT, __CBT_EPROPSEG_LEN) + 2;
        p_can_btr0->tseg2 = AW_BITS_GET(reg, __CBT_EPSEG2_SHIFT, __CBT_EPSEG2_LEN) + 1;
        reg = AW_REG_READ32(&p_reg->CTRL1);
        p_can_btr0->smp   = AW_BIT_GET(reg, __CTRL1_SMP_SHIFT);
    } else {

        reg = AW_REG_READ32(&p_reg->CBT);
        p_canfd_btr->nominal_baud.brp   = AW_BITS_GET(reg, __CBT_EPRESDIV_SHIFT, __CBT_EPRESDIV_LEN) + 1;
        p_canfd_btr->nominal_baud.sjw   = AW_BITS_GET(reg, __CBT_ERJW_SHIFT, __CBT_ERJW_LEN) + 1;
        p_canfd_btr->nominal_baud.tseg1 = AW_BITS_GET(reg, __CBT_EPSEG1_SHIFT, __CBT_EPSEG1_LEN) + \
                                          AW_BITS_GET(reg, __CBT_EPROPSEG_SHIFT, __CBT_EPROPSEG_LEN) + 2;
        p_canfd_btr->nominal_baud.tseg2 = AW_BITS_GET(reg, __CBT_EPSEG2_SHIFT, __CBT_EPSEG2_LEN) + 1;
        p_canfd_btr->nominal_baud.smp   = 0;      /* 单次采样 */

        reg = AW_REG_READ32(&p_reg->FDCBT);
        p_canfd_btr->data_baud.brp   = AW_BITS_GET(reg, __FDCBT_FPRESDIV_SHIFT, __FDCBT_FPRESDIV_LEN) + 1;
        p_canfd_btr->data_baud.sjw   = AW_BITS_GET(reg, __FDCBT_FRJW_SHIFT, __FDCBT_FRJW_LEN) + 1;
        p_canfd_btr->data_baud.tseg1 = AW_BITS_GET(reg, __FDCBT_FPSEG1_SHIFT, __FDCBT_FPSEG1_LEN) + \
                                       AW_BITS_GET(reg, __FDCBT_FPROPSEG_SHIFT, __FDCBT_FPROPSEG_LEN) + 1;
        p_canfd_btr->data_baud.tseg2 = AW_BITS_GET(reg, __FDCBT_FPSEG2_SHIFT, __FDCBT_FPSEG2_LEN) + 1;
        p_canfd_btr->data_baud.smp   = 0;      /* 单次采样 */
    }

    return AW_CAN_ERR_NONE;
}

/** \brief 获取错误计数 */
aw_local aw_can_err_t __imx10xx_can_errreg_cnt_get (
    struct awbl_can_service *p_serv, aw_can_err_cnt_t *p_can_err_reg)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    uint32_t err_cnt = AW_REG_READ32(&p_reg->ECR);

    p_can_err_reg->rx_err_cnt = (uint8_t)AW_BITS_GET(err_cnt, __ECR_RXERRCNT_SHIFT,__ECR_RXERRCNT_LEN);
    p_can_err_reg->tx_err_cnt = (uint8_t)AW_BITS_GET(err_cnt, __ECR_TXERRCNT_SHIFT,__ECR_TXERRCNT_LEN);

    return AW_CAN_ERR_NONE;
}

/** \brief 清除错误计数 */
aw_local aw_can_err_t __imx10xx_can_errreg_cnt_clr (
    struct awbl_can_service *p_serv)
{
    /* 暂时不支持 */
    return -AW_CAN_ERR_NOT_SUPPORT;
}

/** \brief read massage buffer reg */
aw_local aw_can_err_t __imx10xx_can_reg_msg_read (
    struct awbl_can_service *p_serv, struct aw_can_msg *p_canmsg)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    uint32_t                     i, index, len;
    volatile uint32_t            iflag1, reg;

    uint8_t  externflag, remoteflag;

    aw_can_std_msg_t *p_can_msg   = (aw_can_std_msg_t *)p_canmsg;
    aw_can_fd_msg_t  *p_canfd_msg = (aw_can_fd_msg_t *)p_canmsg;

    iflag1 = AW_REG_READ32(&p_reg->IFLAG1);

    for (i = __CAN_MBN_RECV_CHN_START; i < __CAN_MBN_RECV_CHN_START + __CAN_MBN_RECV_CHN_NUM; i++) {
        if (AW_BIT_ISSET(iflag1, i)) {
            index = i;
            break;
        }
    }

    if (index >= (__CAN_MBN_RECV_CHN_START + __CAN_MBN_RECV_CHN_NUM)) {
        return -AW_CAN_ERR_NO_OP;   /* 未接收到数据，一般不会执行到此处 */
    }

#ifdef CANFD_AND_CAN_SWITCH_DYNAMICALLY
    if (p_this->ctrl_type == AW_CAN_CTLR_HS) {
#else
    /* 高速CAN控制器 */
    if (p_info->servinfo.ctr_type == AW_CAN_CTLR_HS) {
#endif
        reg = AW_REG_READ32(&(p_reg->MB_8B[index].CS));

        p_can_msg->can_msg.timestamp = AW_BITS_GET(reg,
                                                   __CAN_MBN_BLK0_TS_SHIFT,
                                                   __CAN_MBN_BLK0_TS_LEN);
        p_can_msg->can_msg.length = AW_BITS_GET(reg,
                                                __CAN_MBN_BLK0_DLC_SHIFT,
                                                __CAN_MBN_BLK0_DLC_LEN);
        p_can_msg->can_msg.flags = 0;

        remoteflag = AW_BIT_ISSET(reg, __CAN_MBN_BLK0_RTR_SHIFT) ? 1 : 0;
        externflag = AW_BIT_ISSET(reg, __CAN_MBN_BLK0_IDE_SHIFT) ? 1 : 0;

        /* 扩展帧/标准帧? 以及帧id */
        if (externflag) {
            p_can_msg->can_msg.flags |= AW_CAN_MSG_FLAG_EXTERND;
            p_can_msg->can_msg.id = AW_REG_BITS_GET32(&(p_reg->MB_8B[index].ID),
                                                      __CAN_MBN_BLK1_EID_SHIFT,
                                                      __CAN_MBN_BLK1_EID_LEN);
        } else {
            p_can_msg->can_msg.id = AW_REG_BITS_GET32(&(p_reg->MB_8B[index].ID),
                                                      __CAN_MBN_BLK1_SID_SHIFT,
                                                      __CAN_MBN_BLK1_SID_LEN);
        }

        /* 远程帧/数据帧? 以及数据 */
        if (remoteflag) {
            p_can_msg->can_msg.flags |= AW_CAN_MSG_FLAG_REMOTE;
//            p_can_msg->can_msg.length = 0;  /* 远程帧的数据长度可以不为0 */
        } else {
            for (i = 0; i < p_can_msg->can_msg.length; i++) {
                p_can_msg->msgbuff[i] = AW_REG_BITS_GET32(&(p_reg->MB_8B[index].WORD[i >> 2]),
                                                          ((3 - (i & 0x3)) << 3),
                                                          8);
            }
        }

        /* 通道号 */
        p_can_msg->can_msg.chn = p_info->servinfo.chn;

    } else {

        if (index < 7) {
            reg = AW_REG_READ32(&(p_reg->MB_64B.MB0[index].CS));
        } else {
            reg = AW_REG_READ32(&(p_reg->MB_64B.MB7[index - 7].CS));
        }

        p_canfd_msg->can_msg.timestamp = AW_BITS_GET(reg,
                                                   __CAN_MBN_BLK0_TS_SHIFT,
                                                   __CAN_MBN_BLK0_TS_LEN);
        len = AW_BITS_GET(reg, __CAN_MBN_BLK0_DLC_SHIFT, __CAN_MBN_BLK0_DLC_LEN);
        if (len <= 8) {
            p_canfd_msg->can_msg.length = len;
        } else {
            switch (len) {
            case __CANFD_DLC_12:
                p_canfd_msg->can_msg.length = 12;
                break;
            case __CANFD_DLC_16:
                p_canfd_msg->can_msg.length = 16;
                break;
            case __CANFD_DLC_20:
                p_canfd_msg->can_msg.length = 20;
                break;
            case __CANFD_DLC_24:
                p_canfd_msg->can_msg.length = 24;
                break;
            case __CANFD_DLC_32:
                p_canfd_msg->can_msg.length = 32;
                break;
            case __CANFD_DLC_48:
                p_canfd_msg->can_msg.length = 48;
                break;
            case __CANFD_DLC_64:
                p_canfd_msg->can_msg.length = 64;
                break;
            }
        }

        p_canfd_msg->can_msg.flags = 0;
        externflag = AW_BIT_ISSET(reg, __CAN_MBN_BLK0_IDE_SHIFT) ? 1 : 0;
        remoteflag = AW_BIT_ISSET(reg, __CAN_MBN_BLK0_RTR_SHIFT) ? 1 : 0;

        if (AW_BIT_ISSET(reg, __CAN_MBN_BLK0_EDL_SHIFT)) {
            p_canfd_msg->can_msg.flags |= AW_CAN_MSG_FLAG_FD_CTRL;
        }
        if (AW_BIT_ISSET(reg, __CAN_MBN_BLK0_BRS_SHIFT)) {
            p_canfd_msg->can_msg.flags |= AW_CAN_MSG_FLAG_BRS;
        }
        if (AW_BIT_ISSET(reg, __CAN_MBN_BLK0_ESI_SHIFT)) {
            p_canfd_msg->can_msg.flags |= AW_CAN_MSG_FLAG_ESI;
        }

        if (externflag) {
            p_canfd_msg->can_msg.flags |= AW_CAN_MSG_FLAG_EXTERND;

            if (index < 7) {
                p_canfd_msg->can_msg.id = AW_REG_BITS_GET32(&(p_reg->MB_64B.MB0[index].ID),
                                                            __CAN_MBN_BLK1_EID_SHIFT,
                                                            __CAN_MBN_BLK1_EID_LEN);
            } else {
                p_canfd_msg->can_msg.id = AW_REG_BITS_GET32(&(p_reg->MB_64B.MB7[index - 7].ID),
                                                            __CAN_MBN_BLK1_EID_SHIFT,
                                                            __CAN_MBN_BLK1_EID_LEN);
            }
        } else {
            if (index < 7) {
                p_canfd_msg->can_msg.id = AW_REG_BITS_GET32(&(p_reg->MB_64B.MB0[index].ID),
                                                            __CAN_MBN_BLK1_SID_SHIFT,
                                                            __CAN_MBN_BLK1_SID_LEN);
            } else {
                p_canfd_msg->can_msg.id = AW_REG_BITS_GET32(&(p_reg->MB_64B.MB7[index - 7].ID),
                                                            __CAN_MBN_BLK1_SID_SHIFT,
                                                            __CAN_MBN_BLK1_SID_LEN);
            }
        }

        if (remoteflag) {
            p_canfd_msg->can_msg.flags |= AW_CAN_MSG_FLAG_REMOTE;
//            p_canfd_msg->can_msg.length = 0;    /* 远程帧的数据长度可以不为0 */
        } else {
            for (i = 0; i < p_canfd_msg->can_msg.length; i++) {
                if (index < 7) {
                    p_canfd_msg->msgbuff[i] = AW_REG_BITS_GET32(&(p_reg->MB_64B.MB0[index].WORD[i >> 2]),
                                                                ((3 - (i & 0x3)) << 3),
                                                                8);
                } else {
                    p_canfd_msg->msgbuff[i] = AW_REG_BITS_GET32(&(p_reg->MB_64B.MB7[index - 7].WORD[i >> 2]),
                                                                ((3 - (i & 0x3)) << 3),
                                                                8);
                }
            }
        }
        p_canfd_msg->can_msg.chn = p_info->servinfo.chn;
    }

    /* 清除中断中断标志位 */
    writel((1 << index), &p_reg->IFLAG1);

    /* 读取定时器寄存器来解锁mb */
    AW_REG_READ32(&p_reg->TIMER);

    return AW_CAN_ERR_NONE;
}

/** \brief write massage buffer reg */
aw_local aw_can_err_t __imx10xx_can_reg_msg_write (
    struct awbl_can_service *p_serv, struct aw_can_msg *p_canmsg)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    uint32_t                     i;

    uint32_t reg;
    uint8_t  externflag;
    uint8_t  remoteflag;
    uint8_t  sendtype;
    uint8_t  len;
//    uint8_t  fd_ctrl;

    aw_can_std_msg_t *p_can_msg   = (aw_can_std_msg_t *)p_canmsg;
    aw_can_fd_msg_t  *p_canfd_msg = (aw_can_fd_msg_t *)p_canmsg;

    externflag = (p_can_msg->can_msg.flags & AW_CAN_MSG_FLAG_EXTERND) ? 1 : 0;
    remoteflag = (p_can_msg->can_msg.flags & AW_CAN_MSG_FLAG_REMOTE) ? 1 : 0;
    sendtype   = (p_can_msg->can_msg.flags & 0xF); /* 发送类型 bit0-3 */

#ifdef CANFD_AND_CAN_SWITCH_DYNAMICALLY
    if (p_this->ctrl_type == AW_CAN_CTLR_HS) {
#else
    /* 高速CAN控制器 */
    if (p_info->servinfo.ctr_type == AW_CAN_CTLR_HS) {
#endif

        /* Check if Message Buffer is available. */
        if ((AW_REG_BITS_GET32(&(p_reg->MB_8B[__CAN_MBN_SEND_CHN].CS),
                               __CAN_MBN_BLK0_CODE_SHIFT,
                               __CAN_MBN_BLK0_CODE_LEN)) == __CAN_MBN_CODE_TX_DATA_OR_REMOTE) {
            return -AW_CAN_ERR_NO_OP;   /* 发送MB正在进行中，一般不会执行到此处  */
        }

        len = p_can_msg->can_msg.length;

        /* 参数检查  */
        if (len > __CAN_MAX_LEN) {
            return -AW_CAN_ERR_ILLEGAL_DATA_LENGTH;
        }
        if((sendtype == AW_CAN_MSG_FLAG_SEND_ONCE) ||
           (sendtype == AW_CAN_MSG_FLAG_SEND_SELFONCE)) {
            return -AW_CAN_ERR_ILLEGAL_MASK_VALUE;  /* 该CAN控制器不支持单次传输 */
        }

        if (sendtype == AW_CAN_MSG_FLAG_SEND_SELF) {
            if (p_this->sendtype != AW_CAN_MSG_FLAG_SEND_SELF) {
                __imx10xx_can_set_haltfrz(p_serv);
                AW_REG_BIT_SET32(&p_reg->CTRL1, __CTRL1_LPB_SHIFT);
                AW_REG_BIT_CLR32(&p_reg->MCR, __MCR_SRXDIS_SHIFT);
                __imx10xx_can_reset_haltfrz(p_serv);
                p_this->sendtype = AW_CAN_MSG_FLAG_SEND_SELF;
            }
        } else {
            if (p_this->sendtype != AW_CAN_MSG_FLAG_SEND_NORMAL) {
                __imx10xx_can_set_haltfrz(p_serv);
                AW_REG_BIT_CLR32(&p_reg->CTRL1, __CTRL1_LPB_SHIFT);
                AW_REG_BIT_SET32(&p_reg->MCR, __MCR_SRXDIS_SHIFT);
                __imx10xx_can_reset_haltfrz(p_serv);
                p_this->sendtype = AW_CAN_MSG_FLAG_SEND_NORMAL;
            }
        }

        if (externflag) {
            AW_REG_BITS_SET32(&(p_reg->MB_8B[__CAN_MBN_SEND_CHN].ID),
                                __CAN_MBN_BLK1_EID_SHIFT,
                                __CAN_MBN_BLK1_EID_LEN,
                                (p_can_msg->can_msg.id) & __CAN_EFF_MASK);
        } else {
            AW_REG_BITS_SET32(&(p_reg->MB_8B[__CAN_MBN_SEND_CHN].ID),
                              __CAN_MBN_BLK1_SID_SHIFT,
                              __CAN_MBN_BLK1_SID_LEN,
                              (p_can_msg->can_msg.id) & __CAN_SFF_MASK);
        }

        reg = 0;
        if (!remoteflag)
        {
            for (i = 0; i < len; i++) {
                reg |= p_can_msg->msgbuff[i] << ((3 - (i & 0x3)) << 3);
                if ((i & 0x3) == 0x3) {
                    AW_REG_WRITE32(&(p_reg->MB_8B[__CAN_MBN_SEND_CHN].WORD[i >> 2]),
                                   reg);
                    reg = 0;
                }
            }
            if ((i & 0x3)) {
                AW_REG_WRITE32(&(p_reg->MB_8B[__CAN_MBN_SEND_CHN].WORD[i >> 2]),
                               reg);
            }
        }

        reg = 0;
        AW_BITS_SET(reg,
                    __CAN_MBN_BLK0_CODE_SHIFT,
                    __CAN_MBN_BLK0_CODE_LEN,
                    __CAN_MBN_CODE_TX_DATA_OR_REMOTE);
        if (externflag) {
            AW_BIT_SET(reg, __CAN_MBN_BLK0_SRR_SHIFT);
        }
        reg |= externflag << __CAN_MBN_BLK0_IDE_SHIFT;
        reg |= remoteflag << __CAN_MBN_BLK0_RTR_SHIFT;
        AW_BITS_SET(reg,
                    __CAN_MBN_BLK0_DLC_SHIFT,
                    __CAN_MBN_BLK0_DLC_LEN,
                    len);
        AW_BITS_SET(reg,
                    __CAN_MBN_BLK0_TS_SHIFT,
                    __CAN_MBN_BLK0_TS_LEN,
                    p_can_msg->can_msg.timestamp);
        AW_REG_WRITE32(&(p_reg->MB_8B[__CAN_MBN_SEND_CHN].CS), reg);
    } else {

        /* Check if Message Buffer is available. */
        if (__CAN_MBN_SEND_CHN < 7) {
            if ((AW_REG_BITS_GET32(&(p_reg->MB_64B.MB0[__CAN_MBN_SEND_CHN].CS),
                                   __CAN_MBN_BLK0_CODE_SHIFT,
                                   __CAN_MBN_BLK0_CODE_LEN)) == __CAN_MBN_CODE_TX_DATA_OR_REMOTE) {
                return -AW_CAN_ERR_NO_OP;   /* 发送MB正在进行中，一般不会执行到此处  */
            }
        } else {
            if ((AW_REG_BITS_GET32(&(p_reg->MB_64B.MB7[__CAN_MBN_SEND_CHN - 7].CS),
                                   __CAN_MBN_BLK0_CODE_SHIFT,
                                   __CAN_MBN_BLK0_CODE_LEN)) == __CAN_MBN_CODE_TX_DATA_OR_REMOTE) {
                return -AW_CAN_ERR_NO_OP;   /* 发送MB正在进行中，一般不会执行到此处  */
            }
        }

        len = p_canfd_msg->can_msg.length;

        /* 参数检查 */
        if (len > __CANFD_MAX_LEN) {
            return -AW_CAN_ERR_ILLEGAL_DATA_LENGTH;
        }
        if ((sendtype == AW_CAN_MSG_FLAG_SEND_ONCE)    ||   /* 该CAN控制器不支持单次传输 */
           (sendtype == AW_CAN_MSG_FLAG_SEND_SELFONCE) ||
           (sendtype == AW_CAN_MSG_FLAG_SEND_SELF)) {       /* 在CANFD状态下不支持自发自收 */
            return -AW_CAN_ERR_ILLEGAL_MASK_VALUE;
        }
        if ((p_canfd_msg->can_msg.flags & AW_CAN_MSG_FLAG_FD_CTRL) && (remoteflag)) {
            return -AW_CAN_ERR_ILLEGAL_MASK_VALUE;  /* CANFD协议不允许发送远程帧 */
        }

        if (p_this->sendtype != AW_CAN_MSG_FLAG_SEND_NORMAL) {
            __imx10xx_can_set_haltfrz(p_serv);
            AW_REG_BIT_CLR32(&p_reg->CTRL1, __CTRL1_LPB_SHIFT);
            AW_REG_BIT_SET32(&p_reg->MCR, __MCR_SRXDIS_SHIFT);
            __imx10xx_can_reset_haltfrz(p_serv);
            p_this->sendtype = AW_CAN_MSG_FLAG_SEND_NORMAL;
        }

        if (externflag) {
            if (__CAN_MBN_SEND_CHN < 7) {
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB0[__CAN_MBN_SEND_CHN].ID),
                                  __CAN_MBN_BLK1_EID_SHIFT,
                                  __CAN_MBN_BLK1_EID_LEN,
                                  (p_canfd_msg->can_msg.id) & __CAN_EFF_MASK);\
            } else {
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB7[__CAN_MBN_SEND_CHN - 7].ID),
                                  __CAN_MBN_BLK1_EID_SHIFT,
                                  __CAN_MBN_BLK1_EID_LEN,
                                  (p_canfd_msg->can_msg.id) & __CAN_EFF_MASK);\
            }
        } else {
            if (__CAN_MBN_SEND_CHN < 7) {
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB0[__CAN_MBN_SEND_CHN].ID),
                                    __CAN_MBN_BLK1_SID_SHIFT,
                                    __CAN_MBN_BLK1_SID_LEN,
                                    (p_canfd_msg->can_msg.id) & __CAN_SFF_MASK);
            } else {
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB7[__CAN_MBN_SEND_CHN - 7].ID),
                                    __CAN_MBN_BLK1_SID_SHIFT,
                                    __CAN_MBN_BLK1_SID_LEN,
                                    (p_canfd_msg->can_msg.id) & __CAN_SFF_MASK);
            }
        }

        reg = 0;
        if (!remoteflag)
        {
            if (__CAN_MBN_SEND_CHN < 7) {
                for (i = 0; i < len; i++) {
                    reg |= p_canfd_msg->msgbuff[i] << ((3 - (i & 0x3)) << 3);
                    if ((i & 0x3) == 0x3) {
                        AW_REG_WRITE32(&(p_reg->MB_64B.MB0[__CAN_MBN_SEND_CHN].WORD[i >> 2]),
                                       reg);
                        reg = 0;
                    }
                }
                if ((i & 0x3)) {
                    AW_REG_WRITE32(&(p_reg->MB_64B.MB0[__CAN_MBN_SEND_CHN].WORD[i >> 2]),
                                   reg);
                }
            } else {
                for (i = 0; i < len; i++) {
                    reg |= p_canfd_msg->msgbuff[i] << ((3 - (i & 0x3)) << 3);
                    if ((i & 0x3) == 0x3) {
                        AW_REG_WRITE32(&(p_reg->MB_64B.MB7[__CAN_MBN_SEND_CHN - 7].WORD[i >> 2]),
                                       reg);
                        reg = 0;
                    }
                }
                if ((i & 0x3)) {
                    AW_REG_WRITE32(&(p_reg->MB_64B.MB7[__CAN_MBN_SEND_CHN - 7].WORD[i >> 2]),
                                   reg);
                }
            }
        }

        reg = 0;
        AW_BITS_SET(reg,
                    __CAN_MBN_BLK0_CODE_SHIFT,
                    __CAN_MBN_BLK0_CODE_LEN,
                    __CAN_MBN_CODE_TX_DATA_OR_REMOTE);

        AW_BIT_SET(reg, __CAN_MBN_BLK0_SRR_SHIFT);  /* 进行发送时此位必须设置为1 */
        reg |= externflag << __CAN_MBN_BLK0_IDE_SHIFT;
        reg |= remoteflag << __CAN_MBN_BLK0_RTR_SHIFT;

        if (len > 8) {
            switch (len) {
            case 12:
                len = __CANFD_DLC_12;
                break;
            case 16:
                len = __CANFD_DLC_16;
                break;
            case 20:
                len = __CANFD_DLC_20;
                break;
            case 24:
                len = __CANFD_DLC_24;
                break;
            case 32:
                len = __CANFD_DLC_32;
                break;
            case 48:
                len = __CANFD_DLC_48;
                break;
            case 64:
                len = __CANFD_DLC_64;
                break;
            default :
                return AW_CAN_ERR_ILLEGAL_DATA_LENGTH;
            }
        }

        AW_BITS_SET(reg,
                    __CAN_MBN_BLK0_DLC_SHIFT,
                    __CAN_MBN_BLK0_DLC_LEN,
                    len);
        AW_BITS_SET(reg,
                    __CAN_MBN_BLK0_TS_SHIFT,
                    __CAN_MBN_BLK0_TS_LEN,
                    p_can_msg->can_msg.timestamp);

        if (p_canfd_msg->can_msg.flags & AW_CAN_MSG_FLAG_FD_CTRL) {
            AW_BIT_SET(reg, __CAN_MBN_BLK0_EDL_SHIFT);
        }
        if (p_canfd_msg->can_msg.flags & AW_CAN_MSG_FLAG_BRS) {
            AW_BIT_SET(reg, __CAN_MBN_BLK0_BRS_SHIFT);
        }

        if (__CAN_MBN_SEND_CHN < 7) {
            AW_REG_WRITE32(&(p_reg->MB_64B.MB0[__CAN_MBN_SEND_CHN].CS), reg);
        } else {
            AW_REG_WRITE32(&(p_reg->MB_64B.MB7[__CAN_MBN_SEND_CHN - 7].CS), reg);
        }
    }

    return AW_CAN_ERR_NONE;
}

/** \brief stop massage buffer reg send */
aw_local aw_can_err_t __imx10xx_can_msg_snd_stop (
    struct awbl_can_service *p_serv)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    uint32_t reg, timeout;

    /* 首先禁止发送MB中断 */
    AW_REG_BIT_CLR32(&p_reg->IMASK1, __CAN_MBN_SEND_CHN);

#ifdef CANFD_AND_CAN_SWITCH_DYNAMICALLY
    if (p_this->ctrl_type == AW_CAN_CTLR_HS) {
#else
    /* 高速CAN控制器 */
    if (p_info->servinfo.ctr_type == AW_CAN_CTLR_HS) {
#endif
        /* 判断当前发送MB是否发送完成 */
        reg = AW_REG_BITS_GET32(&(p_reg->MB_8B[__CAN_MBN_SEND_CHN].CS),
                                __CAN_MBN_BLK0_CODE_SHIFT,
                                __CAN_MBN_BLK0_CODE_LEN);

        if ((reg == __CAN_MBN_CODE_TX_INACTIVE) ||  /* 数据帧发送完成后MB的CODE域将变为0x8 */
            (reg == __CAN_MBN_CODE_RX_EMPTY)) {     /* 远程帧发送完成后MB的CODE域将变为0x4 */
            if (AW_REG_BIT_ISSET32(&p_reg->IFLAG1, __CAN_MBN_SEND_CHN)) {
                AW_REG_BIT_SET32(&p_reg->IFLAG1, __CAN_MBN_SEND_CHN);
                AW_REG_BIT_SET32(&p_reg->IMASK1, __CAN_MBN_SEND_CHN);
                return -AW_CAN_ERR_ABORT_TRANSMITTED;   /* 终止失败 */
            } else {
                AW_REG_BIT_SET32(&p_reg->IMASK1, __CAN_MBN_SEND_CHN);
                return AW_CAN_ERR_NONE;     /* 发送MB在禁用中断前已经被处理了, 无MB可中止 */
            }
        }

        /* 运行至此处, 说明发送MB正在被挂起（等待发送） */

        /* 执行发送中止操作 */
        __imx10xx_can_set_haltfrz(p_serv);
        AW_REG_BIT_SET32(&p_reg->MCR, __MCR_AEN_SHIFT);
        __imx10xx_can_reset_haltfrz(p_serv);

        AW_REG_BITS_SET32(&(p_reg->MB_8B[__CAN_MBN_SEND_CHN].CS),
                          __CAN_MBN_BLK0_CODE_SHIFT,
                          __CAN_MBN_BLK0_CODE_LEN,
                          __CAN_MBN_CODE_TX_ABORT);


        timeout = 10000;
        while (!AW_REG_BIT_ISSET32(&p_reg->IFLAG1, __CAN_MBN_SEND_CHN) && (--timeout));

        reg = AW_REG_BITS_GET32(&(p_reg->MB_8B[__CAN_MBN_SEND_CHN].CS),
                                __CAN_MBN_BLK0_CODE_SHIFT,
                                __CAN_MBN_BLK0_CODE_LEN);

    } else {

        /* 判断当前发送MB是否发送完成 */
        if (__CAN_MBN_SEND_CHN < 7) {
            reg = AW_REG_BITS_GET32(&(p_reg->MB_64B.MB0[__CAN_MBN_SEND_CHN].CS),
                                    __CAN_MBN_BLK0_CODE_SHIFT,
                                    __CAN_MBN_BLK0_CODE_LEN);
        } else {
            reg = AW_REG_BITS_GET32(&(p_reg->MB_64B.MB7[__CAN_MBN_SEND_CHN - 7].CS),
                                    __CAN_MBN_BLK0_CODE_SHIFT,
                                    __CAN_MBN_BLK0_CODE_LEN);
        }

        if ((reg == __CAN_MBN_CODE_TX_INACTIVE) ||  /* 数据帧发送完成后MB的CODE域将变为0x8 */
            (reg == __CAN_MBN_CODE_RX_EMPTY)) {     /* 远程帧发送完成后MB的CODE域将变为0x4 */
            if (AW_REG_BIT_ISSET32(&p_reg->IFLAG1, __CAN_MBN_SEND_CHN)) {
                AW_REG_BIT_SET32(&p_reg->IFLAG1, __CAN_MBN_SEND_CHN);
                AW_REG_BIT_SET32(&p_reg->IMASK1, __CAN_MBN_SEND_CHN);
                return -AW_CAN_ERR_ABORT_TRANSMITTED;   /* 终止失败 */
            } else {
                AW_REG_BIT_SET32(&p_reg->IMASK1, __CAN_MBN_SEND_CHN);
                return AW_CAN_ERR_NONE;     /* 发送MB在禁用中断前已经被处理了, 无MB可中止 */
            }
        }

        /* 运行至此处, 说明发送MB正在被挂起（等待发送） */

        /* 执行发送中止操作 */
        __imx10xx_can_set_haltfrz(p_serv);
        AW_REG_BIT_SET32(&p_reg->MCR, __MCR_AEN_SHIFT);
        __imx10xx_can_reset_haltfrz(p_serv);

        if (__CAN_MBN_SEND_CHN < 7) {
            AW_REG_BITS_SET32(&(p_reg->MB_64B.MB0[__CAN_MBN_SEND_CHN].CS),
                              __CAN_MBN_BLK0_CODE_SHIFT,
                              __CAN_MBN_BLK0_CODE_LEN,
                              __CAN_MBN_CODE_TX_ABORT);
        } else {
            AW_REG_BITS_SET32(&(p_reg->MB_64B.MB7[__CAN_MBN_SEND_CHN - 7].CS),
                              __CAN_MBN_BLK0_CODE_SHIFT,
                              __CAN_MBN_BLK0_CODE_LEN,
                              __CAN_MBN_CODE_TX_ABORT);
        }

        timeout = 10000;
        while (!AW_REG_BIT_ISSET32(&p_reg->IFLAG1, __CAN_MBN_SEND_CHN) && (--timeout));

        if (__CAN_MBN_SEND_CHN < 7) {
            reg = AW_REG_BITS_GET32(&(p_reg->MB_64B.MB0[__CAN_MBN_SEND_CHN].CS),
                                    __CAN_MBN_BLK0_CODE_SHIFT,
                                    __CAN_MBN_BLK0_CODE_LEN);
        } else {
            reg = AW_REG_BITS_GET32(&(p_reg->MB_64B.MB7[__CAN_MBN_SEND_CHN - 7].CS),
                                    __CAN_MBN_BLK0_CODE_SHIFT,
                                    __CAN_MBN_BLK0_CODE_LEN);
        }
    }

    __imx10xx_can_set_haltfrz(p_serv);
    AW_REG_BIT_CLR32(&p_reg->MCR, __MCR_AEN_SHIFT);
    __imx10xx_can_reset_haltfrz(p_serv);
    AW_REG_BIT_SET32(&p_reg->IFLAG1, __CAN_MBN_SEND_CHN);
    AW_REG_BIT_SET32(&p_reg->IMASK1, __CAN_MBN_SEND_CHN);

    if ((reg == __CAN_MBN_CODE_TX_INACTIVE) ||  /* 数据帧发送完成后MB的CODE域将变为0x8 */
        (reg == __CAN_MBN_CODE_RX_EMPTY)) {     /* 远程帧发送完成后MB的CODE域将变为0x4 */
        return -AW_CAN_ERR_ABORT_TRANSMITTED;
    }

    return AW_CAN_ERR_NONE;
}


/**
 * \brief 验收滤波表设置函数, 滤波表是以小端模式传输
 *
 * 滤波表格式: [ID] ... [ID] [MSK] ... [MSK] 4字节对齐
 * [ID]  : bit31: 为远程帧标志, 远程帧为1 数据帧为0; bit30: 为扩展帧标志, 扩展帧为1 标准帧为0; bit28-bit0: ID(标准帧为bit28-bit18)
 * [MSK] : bit31: 为远程帧掩码, 1对该位进行对比 0不考虑该位; bit30: 为扩展帧掩码, 1对该位进行对比 0不考虑该位; bit28-bit0:ID掩码, 1对该位进行对比，0不考虑该位
 *
 * \note  对于CANFD模式远程标志过滤无作用也就是说bit31位无作用，因为CANFD不支持远程帧
 *
 * \note 使用范例
 * 滤波方式是全ID+标志滤波
 *
 * 第1段是全ID滤波的格式,如下
 * ----------------------------------------------------------
 * | rtr(31) | ide(30) | id(28~0) | reserved(29) |
 * ----------------------------------------------------------
 * rtr 远程帧标志.若此位为1,则接收对应ID的远程帧,否则接收数据帧
 * ide 拓展帧标志.若此位为1,则接收对应ID的拓展帧,否则接收标准帧
 * id  仲裁ID.其中(28~18)为标准帧ID段,(28~0)为拓展帧ID段
 * rsv 保留位.
 *
 * 在对应仲裁ID结束之后是第2段,掩码ID.它与第二段一一对应,表明当
 * 对应位为1时报文的仲裁位必须符合第二段才接收,对应位为0时不作要求.
 *
 * 例1：uint32_t filter_table[] {
 *         __FLITER(0x0, 0x1, 0xff),
 *
 *         __FLITER(0x1, 0x1, 0x1fffffff)
 * }
 * 表明只接收ID为0xff的扩展数据帧
 *
 * 例2：uint32_t filter_table[] {
 *         __FLITER(0x0, 0x1, 0xff),
 *
 *         __FLITER(0x1, 0x0, 0x0)
 * }
 * 表明只接收数据帧,对ID 和 是否为扩展帧不作要求。
 *
 * 例3：uint32_t filter_table[] {
 *         __FLITER(0x0, 0x1, 0xff),
 *
 *         __FLITER(0x1, 0x1, 0x1ffffffc)
 * }
 * 表明只接收ID为0b111111xx的扩展数据帧（ID中“x”表示可为0可为1，也就是可接收ID为0b11111100、0b11111101、0b11111110、0b11111111的帧）
 *
 * 注：因CANFD不支持远程帧所以bit31位实际无效果
 *
 * #define __FLITER(rtr, ide, id)  (((rtr & 0x1) << 31) | ((ide & 0x1) << 30) | (id))
 * #define __MASK(rtr, ide, id)    (((rtr & 0x1) << 31) | ((ide & 0x1) << 30) | (id))
 *
 * static uint32_t filter_table[] = {
 *    __FLITER(0x0, 0x1, 0xf),
 *
 *    __MASK(0x1, 0x1, 0x1ffffff0)
 * };
 *
 */
aw_local aw_can_err_t __imx10xx_can_filter_table_set (
    struct awbl_can_service *p_serv, uint8_t *p_filterbuff, size_t length)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    uint32_t *p_filbuf = NULL;
    uint32_t  i, j, index;
    volatile uint8_t   rcv_offset = __CAN_MBN_RECV_CHN_NUM / 2;

    /* 参数检查 */
    if ((p_filterbuff == NULL) || (length == 0)) {
        return -AW_CAN_ERR_INVALID_PARAMETER;
    }
    if ((length % (4 * 2) != 0) || ((length / 4) / 2 > rcv_offset)) {
        return -AW_CAN_ERR_ILLEGAL_DATA_LENGTH;
    }

    /* 将CTRL2[EACEN]位置1通过RXIMR[bit30]来控制过滤标准帧和扩展帧，MCR[IRMQ]位置1以使能独立滤波掩码 */
    __imx10xx_can_set_haltfrz(p_serv);
//    AW_REG_BIT_CLR32(&p_reg->CTRL2, __CTRL2_EACEN_SHIFT);
    AW_REG_BIT_SET32(&p_reg->CTRL2, __CTRL2_EACEN_SHIFT);
    AW_REG_BIT_SET32(&p_reg->MCR, __MCR_IRMQ_SHIFT); /* 使能独立滤波掩码 */


#ifdef CANFD_AND_CAN_SWITCH_DYNAMICALLY
    if (p_this->ctrl_type == AW_CAN_CTLR_HS) {
#else
    /* 高速CAN控制器 */
    if (p_info->servinfo.ctr_type == AW_CAN_CTLR_HS) {
#endif

        /* 先设置 [ID], 设置2组以防止相同id的帧来时由于处理不及时导致第二帧丢失 */
        p_filbuf = (uint32_t *)p_filterbuff;
        for (i = 0; i < (length / 4 / 2); i++) {
            if (AW_BIT_ISSET(p_filbuf[i], 31)) {
                index = i + __CAN_MBN_RECV_CHN_START;
                AW_REG_BIT_SET32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                index += rcv_offset;
                AW_REG_BIT_SET32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
            } else {
                index = i + __CAN_MBN_RECV_CHN_START;
                AW_REG_BIT_CLR32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                index += rcv_offset;
                AW_REG_BIT_CLR32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
            }
            if (AW_BIT_ISSET(p_filbuf[i], 30)) {
                index = i + __CAN_MBN_RECV_CHN_START;
                AW_REG_BIT_SET32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                index += rcv_offset;
                AW_REG_BIT_SET32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
            } else {
                index = i + __CAN_MBN_RECV_CHN_START;
                AW_REG_BIT_CLR32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                index += rcv_offset;
                AW_REG_BIT_CLR32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
            }
            index = i + __CAN_MBN_RECV_CHN_START;
            AW_REG_BITS_SET32(&(p_reg->MB_8B[index].ID),
                              __CAN_MBN_BLK1_EID_SHIFT,
                              __CAN_MBN_BLK1_EID_LEN,
                              p_filbuf[i] & __CAN_EFF_MASK);
            index += rcv_offset;
            AW_REG_BITS_SET32(&(p_reg->MB_8B[index].ID),
                              __CAN_MBN_BLK1_EID_SHIFT,
                              __CAN_MBN_BLK1_EID_LEN,
                              p_filbuf[i] & __CAN_EFF_MASK);
        }

        /* 填充剩下的滤波ID */
        for (j = 0; i < (rcv_offset); i++, j++) {
            j %= (length / 4 / 2);

            if (AW_BIT_ISSET(p_filbuf[j], 31)) {
                index = i + __CAN_MBN_RECV_CHN_START;
                AW_REG_BIT_SET32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                index += rcv_offset;
                AW_REG_BIT_SET32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
            } else {
                index = i + __CAN_MBN_RECV_CHN_START;
                AW_REG_BIT_CLR32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                index += rcv_offset;
                AW_REG_BIT_CLR32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
            }
            if (AW_BIT_ISSET(p_filbuf[j], 30)) {
                index = i + __CAN_MBN_RECV_CHN_START;
                AW_REG_BIT_SET32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                index += rcv_offset;
                AW_REG_BIT_SET32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
            } else {
                index = i + __CAN_MBN_RECV_CHN_START;
                AW_REG_BIT_CLR32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                index += rcv_offset;
                AW_REG_BIT_CLR32(&(p_reg->MB_8B[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
            }

            index = i + __CAN_MBN_RECV_CHN_START;
            AW_REG_BITS_SET32(&(p_reg->MB_8B[index].ID),
                              __CAN_MBN_BLK1_EID_SHIFT,
                              __CAN_MBN_BLK1_EID_LEN,
                              p_filbuf[j] & __CAN_EFF_MASK);
            index += rcv_offset;
            AW_REG_BITS_SET32(&(p_reg->MB_8B[index].ID),
                              __CAN_MBN_BLK1_EID_SHIFT,
                              __CAN_MBN_BLK1_EID_LEN,
                              p_filbuf[j] & __CAN_EFF_MASK);
        }

    } else {

        /* 先设置 [ID], 设置2组以防止相同id的帧来时由于处理不及时导致第二帧丢失 */
        p_filbuf = (uint32_t *)p_filterbuff;
        for (i = 0; i < (length / 4 / 2); i++) {
            if (AW_BIT_ISSET(p_filbuf[i], 31)) {
                index = i + __CAN_MBN_RECV_CHN_START;
                if (index < 7) {
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                }
                index = i + __CAN_MBN_RECV_CHN_START + rcv_offset;
                if (index < 7) {
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                }
            } else {
                index = i + __CAN_MBN_RECV_CHN_START;
                if (index < 7) {
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                }
                index = i + __CAN_MBN_RECV_CHN_START + rcv_offset;
                if (index < 7) {
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                }
            }
            if (AW_BIT_ISSET(p_filbuf[i], 30)) {
                index = i + __CAN_MBN_RECV_CHN_START;
                if (index < 7) {
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                }
                index = i + __CAN_MBN_RECV_CHN_START + rcv_offset;
                if (index < 7) {
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                }
            } else {
                index = i + __CAN_MBN_RECV_CHN_START;
                if (index < 7) {
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                }
                index = i + __CAN_MBN_RECV_CHN_START + rcv_offset;
                if (index < 7) {
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                }
            }
            index = i + __CAN_MBN_RECV_CHN_START;
            if (index < 7) {
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB0[index].ID),
                                  __CAN_MBN_BLK1_EID_SHIFT,
                                  __CAN_MBN_BLK1_EID_LEN,
                                  p_filbuf[i] & __CAN_EFF_MASK);
            } else {
                index -= 7;
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB7[index].ID),
                                  __CAN_MBN_BLK1_EID_SHIFT,
                                  __CAN_MBN_BLK1_EID_LEN,
                                  p_filbuf[i] & __CAN_EFF_MASK);
            }
            index = i + __CAN_MBN_RECV_CHN_START + rcv_offset;
            if (index < 7) {
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB0[index].ID),
                                  __CAN_MBN_BLK1_EID_SHIFT,
                                  __CAN_MBN_BLK1_EID_LEN,
                                  p_filbuf[i] & __CAN_EFF_MASK);
            } else {
                index -= 7;
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB7[index].ID),
                                  __CAN_MBN_BLK1_EID_SHIFT,
                                  __CAN_MBN_BLK1_EID_LEN,
                                  p_filbuf[i] & __CAN_EFF_MASK);
            }
        }

        /* 填充剩下的滤波ID */
        for (j = 0; i < (rcv_offset); i++, j++) {
            j %= (length / 4 / 2);

            if (AW_BIT_ISSET(p_filbuf[j], 31)) {
                index = i + __CAN_MBN_RECV_CHN_START;
                if ((index) < 7) {
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                }
                index = i + __CAN_MBN_RECV_CHN_START + rcv_offset;
                if (index < 7) {
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                }
            } else {
                index = i + __CAN_MBN_RECV_CHN_START;
                if (index < 7) {
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                }
                index = i + __CAN_MBN_RECV_CHN_START + rcv_offset;
                if ((index) < 7) {
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_RTR_SHIFT);
                }
            }
            if (AW_BIT_ISSET(p_filbuf[j], 30)) {
                index = i + __CAN_MBN_RECV_CHN_START;
                if (index < 7) {
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                }
                index = i + __CAN_MBN_RECV_CHN_START + rcv_offset;
                if (index < 7) {
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_SET32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                }
            } else {
                index = i + __CAN_MBN_RECV_CHN_START;
                if (index < 7) {
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                }
                index = i + __CAN_MBN_RECV_CHN_START + rcv_offset;
                if (index < 7) {
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB0[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                } else {
                    index -= 7;
                    AW_REG_BIT_CLR32(&(p_reg->MB_64B.MB7[index].CS), __CAN_MBN_BLK0_IDE_SHIFT);
                }
            }

            index = i + __CAN_MBN_RECV_CHN_START;
            if (index < 7) {
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB0[index].ID),
                                  __CAN_MBN_BLK1_EID_SHIFT,
                                  __CAN_MBN_BLK1_EID_LEN,
                                  p_filbuf[j] & __CAN_EFF_MASK);
            } else {
                index -= 7;
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB7[index].ID),
                                  __CAN_MBN_BLK1_EID_SHIFT,
                                  __CAN_MBN_BLK1_EID_LEN,
                                  p_filbuf[j] & __CAN_EFF_MASK);
            }
            index = i + __CAN_MBN_RECV_CHN_START + rcv_offset;
            if (index < 7) {
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB0[index].ID),
                                  __CAN_MBN_BLK1_EID_SHIFT,
                                  __CAN_MBN_BLK1_EID_LEN,
                                  p_filbuf[j] & __CAN_EFF_MASK);
            } else {
                index -= 7;
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB7[index].ID),
                                  __CAN_MBN_BLK1_EID_SHIFT,
                                  __CAN_MBN_BLK1_EID_LEN,
                                  p_filbuf[j] & __CAN_EFF_MASK);
            }
        }
    }

    /* 再设置 [MSK] */
    p_filbuf = (uint32_t *) (p_filterbuff + (length / 2));
    for (i = 0; i < (length / 4 / 2); i++) {
        AW_REG_WRITE32(&p_reg->RXIMR[i + __CAN_MBN_RECV_CHN_START], p_filbuf[i]);
        AW_REG_WRITE32(&p_reg->RXIMR[i + __CAN_MBN_RECV_CHN_START + (rcv_offset)], p_filbuf[i]);
    }

    /* 填充剩下的滤波掩码 */
    for (j = 0; i < rcv_offset; i++, j++) {
        j %= (length / 4 / 2);
        AW_REG_WRITE32(&p_reg->RXIMR[i + __CAN_MBN_RECV_CHN_START], p_filbuf[j]);
        AW_REG_WRITE32(&p_reg->RXIMR[i + __CAN_MBN_RECV_CHN_START + (rcv_offset)], p_filbuf[j]);
    }

    __imx10xx_can_reset_haltfrz(p_serv);

    /* 保存当前滤波表的长度，用于滤波表获取函数使用 */
    p_this->filter_table_len = length;

    return AW_CAN_ERR_NONE;
}

/** \brief 验收滤波表获取函数, 滤波表是以小端模式传输 */
aw_local aw_can_err_t __imx10xx_can_filter_table_get (
    struct awbl_can_service *p_serv,
    uint8_t                 *p_filterbuff,
    size_t                  *p_length)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    uint32_t  i;
    uint32_t *p_filbuf;
    uint32_t  len = p_this->filter_table_len;

    p_filbuf = (uint32_t *)p_filterbuff;

#ifdef CANFD_AND_CAN_SWITCH_DYNAMICALLY
    if (p_this->ctrl_type == AW_CAN_CTLR_HS) {
#else
    /* 高速CAN控制器 */
    if (p_info->servinfo.ctr_type == AW_CAN_CTLR_HS) {
#endif
        for (i = 0; i < (len / 4 / 2); i++) {
            p_filbuf[i] = 0;
            if (AW_REG_BIT_ISSET32(&(p_reg->MB_8B[i].CS), __CAN_MBN_BLK0_RTR_SHIFT)) {
                AW_BIT_SET(p_filbuf[i], 31);
            }
            if (AW_REG_BIT_ISSET32(&(p_reg->MB_8B[i].CS), __CAN_MBN_BLK0_IDE_SHIFT)) {
                AW_BIT_SET(p_filbuf[i], 30);
            }
            AW_BITS_SET(p_filbuf[i], 0, 28, (AW_REG_READ32(&p_reg->MB_8B[i].ID) & __CAN_EFF_MASK));
        }

    } else {

        for (i = 0; i < (len / 4 / 2); i++) {
            p_filbuf[i] = 0;
            if (i < 7) {
                if (AW_REG_BIT_ISSET32(&(p_reg->MB_64B.MB0[i].CS), __CAN_MBN_BLK0_RTR_SHIFT)) {
                    AW_BIT_SET(p_filbuf[i], 31);
                }
                if (AW_REG_BIT_ISSET32(&(p_reg->MB_64B.MB0[i].CS), __CAN_MBN_BLK0_IDE_SHIFT)) {
                    AW_BIT_SET(p_filbuf[i], 30);
                }
                AW_BITS_SET(p_filbuf[i], 0, 28, (AW_REG_READ32(&p_reg->MB_64B.MB0[i].ID) & __CAN_EFF_MASK));
            } else {
                if (AW_REG_BIT_ISSET32(&(p_reg->MB_64B.MB7[i - 7].CS), __CAN_MBN_BLK0_RTR_SHIFT)) {
                    AW_BIT_SET(p_filbuf[i], 31);
                }
                if (AW_REG_BIT_ISSET32(&(p_reg->MB_64B.MB7[i - 7].CS), __CAN_MBN_BLK0_IDE_SHIFT)) {
                    AW_BIT_SET(p_filbuf[i], 30);
                }
                AW_BITS_SET(p_filbuf[i], 0, 28, (AW_REG_READ32(&p_reg->MB_64B.MB7[i - 7].ID) & __CAN_EFF_MASK));
            }
        }
    }

    p_filbuf = (uint32_t *) (p_filterbuff + (len / 2));
    for (i = 0; i < (len / 4 / 2); i++) {
        __imx10xx_can_set_haltfrz(p_serv);
        p_filbuf[i] = AW_REG_READ32(&p_reg->RXIMR[i]);
        __imx10xx_can_reset_haltfrz(p_serv);
    }

    *p_length = len;

    return AW_CAN_ERR_NONE;
}

/**
 * \brief CAN bus 中断标志位获取
 * 读完中断位之后 接下来进行判断中断类型，并清除对应中断标志位
 **/
aw_local aw_can_err_t __imx10xx_can_intstatus_get (
    struct awbl_can_service *p_serv,
    awbl_can_int_type_t     *p_int_type,
    aw_can_bus_err_t        *p_bus_err)
{
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);

    awbl_can_int_type_t status;
    uint32_t iflag1;
    uint32_t rcv_msk;

    *p_int_type = AWBL_CAN_INT_NONE;
    *p_bus_err  = AW_CAN_BUS_ERR_NONE;

    status = AW_REG_READ32(&p_reg->ESR1);
    iflag1 = AW_REG_READ32(&p_reg->IFLAG1);

    /* 发送成功中断 */
    if (AW_REG_BIT_ISSET32(&p_reg->IMASK1, __CAN_MBN_SEND_CHN)) {
        if (AW_BIT_ISSET(iflag1, __CAN_MBN_SEND_CHN)) {
            *p_int_type |= AWBL_CAN_INT_TX;
            writel((1 << __CAN_MBN_SEND_CHN), &p_reg->IFLAG1);

#ifdef CANFD_AND_CAN_SWITCH_DYNAMICALLY
            if (p_this->ctrl_type == AW_CAN_CTLR_HS) {
#else
            /* 高速CAN控制器 */
            if (p_info->servinfo.ctr_type == AW_CAN_CTLR_HS) {
#endif
                AW_REG_BITS_SET32(&(p_reg->MB_8B[__CAN_MBN_SEND_CHN].CS),
                                  __CAN_MBN_BLK0_CODE_SHIFT,
                                  __CAN_MBN_BLK0_CODE_LEN,
                                  __CAN_MBN_CODE_TX_INACTIVE);
            } else {

                if (__CAN_MBN_SEND_CHN < 7) {
                    AW_REG_BITS_SET32(&(p_reg->MB_64B.MB0[__CAN_MBN_SEND_CHN].CS),
                                      __CAN_MBN_BLK0_CODE_SHIFT,
                                      __CAN_MBN_BLK0_CODE_LEN,
                                      __CAN_MBN_CODE_TX_INACTIVE);
                } else {
                    AW_REG_BITS_SET32(&(p_reg->MB_64B.MB7[__CAN_MBN_SEND_CHN - 7].CS),
                                      __CAN_MBN_BLK0_CODE_SHIFT,
                                      __CAN_MBN_BLK0_CODE_LEN,
                                      __CAN_MBN_CODE_TX_INACTIVE);
                }
            }
        }
    }

    /* 接收中断 */
    rcv_msk = AW_BITS_MASK(__CAN_MBN_RECV_CHN_NUM) << __CAN_MBN_RECV_CHN_START;
    if (rcv_msk & iflag1) {
        *p_int_type |= AWBL_CAN_INT_RX;
    }

    /* 唤醒中断  */
    if (AW_BIT_ISSET(status, __ESR1_WAKINT_SHIFT)) {
        *p_int_type |= AWBL_CAN_INT_WAKE_UP;
        writel((1 << __ESR1_WAKINT_SHIFT), &p_reg->ESR1);
    }

    /* 错误中断 */
    if (AW_BIT_ISSET(status, __ESR1_ERRINT_SHIFT)) {
        *p_int_type |= AWBL_CAN_INT_ERROR;
        writel((1 << __ESR1_ERRINT_SHIFT), &p_reg->ESR1);

        /* 判断错误类型 */
        if ((AW_BIT_ISSET(status, __ESR1_BIT0ERR_SHIFT))      ||
            (AW_BIT_ISSET(status, __ESR1_BIT1ERR_SHIFT))      ||
            (AW_BIT_ISSET(status, __ESR1_BIT0ERR_FAST_SHIFT)) ||
            (AW_BIT_ISSET(status, __ESR1_BIT1ERR_FAST_SHIFT))) {
            *p_bus_err |= AW_CAN_BUS_ERR_BIT;
        }
        if (AW_BIT_ISSET(status, __ESR1_ACKERR_SHIFT)) {
            *p_bus_err |= AW_CAN_BUS_ERR_ACK;
        }
        if ((AW_BIT_ISSET(status, __ESR1_CRCERR_SHIFT)) ||
            (AW_BIT_ISSET(status, __ESR1_CRCERR_FAST_SHIFT))) {
            *p_bus_err |= AW_CAN_BUS_ERR_CRC;
        }
        if ((AW_BIT_ISSET(status, __ESR1_FRMERR_SHIFT)) ||
            (AW_BIT_ISSET(status, __ESR1_FRMERR_FAST_SHIFT))) {
            *p_bus_err |= AW_CAN_BUS_ERR_FORM;
        }
        if ((AW_BIT_ISSET(status, __ESR1_STFERR_SHIFT)) ||
            (AW_BIT_ISSET(status, __ESR1_STFERR_FAST_SHIFT))) {
            *p_bus_err |= AW_CAN_BUS_ERR_STUFF;
        }
    }

    /* 总线超载中断, 该控制器不能检测这个中断 */

    /* 错误报警中断 */
    if ((AW_BIT_ISSET(status, __ESR1_RWRNINT_SHIFT)) ||
        (AW_BIT_ISSET(status, __ESR1_TWRNINT_SHIFT))) {
        *p_int_type |= AWBL_CAN_INT_WARN;
        writel(((1 << __ESR1_RWRNINT_SHIFT) | (1 << __ESR1_TWRNINT_SHIFT)), &p_reg->ESR1);
    }

    /* 错误被动中断, 该控制器不能检测这个中断  */

    /* 错误关闭中断 */
    if (AW_BIT_ISSET(status, __ESR1_BOFFINT_SHIFT)) {
        *p_int_type |= AWBL_CAN_INT_BUS_OFF;
        writel((1 << __ESR1_BOFFINT_SHIFT), &p_reg->ESR1);
    }

    return AW_CAN_ERR_NONE;
}

#ifdef CANFD_AND_CAN_SWITCH_DYNAMICALLY
extern awbl_can_service_t *__can_chn_to_serv (int chn);

/**
 * \brief canfd控制器类型动态切换函数
 *
 * \note 使用此接口时需要注意，只有在配置文件(如：awbl_hwconf_imx1064_flexcan3.h)中将CANFD控制器
 *       默认配置为CANFD控制器时，才可以调用此接口将此控制器在CANFD与CAN之间动态切换。
 *       如果在配置文件中默认将CANFD控制器配置为CAN控制器时，调用此函数做动态切换是将返回失败。
 *
 * \param[in] chn   CAN通道号
 *
 * \param[in] type  要切换的控制器类型
 *
 * \return  AW_CAN_ERR_NONE                 切换成功
 *         -AW_CAN_ERR_NO_DEV               设备不存在 或 此控制器默认是CAN控制器，无法做切换
 *         -AW_CAN_ERR_ILLEGAL_CTRL_TYPE    type参数错误
 */
aw_can_err_t imxrt10xx_canfd_ctrl_type_switch (int chn, uint8_t type)
{
    struct awbl_can_service       *p_serv = __can_chn_to_serv(chn);
    awbl_imx10xx_canfd_dev_t      *p_this = __IMX10xx_CANFD_GET_THIS(p_serv);
    awbl_imx10xx_canfd_dev_info_t *p_info = __IMX10xx_CANFD_GET_INFO(&(p_this->dev));
    imx10xx_canfd_regs_t          *p_reg  = (imx10xx_canfd_regs_t *)(p_info->reg_base);
    uint32_t                       i;

    if ((p_serv == NULL) || (p_serv->p_info->ctr_type != AW_CAN_CTLR_FD)) {
        return -AW_CAN_ERR_NO_DEV;
    }

    /* 进入冻结模式 */
    __imx10xx_can_set_haltfrz(p_serv);

    if (type == AW_CAN_CTLR_HS) {
        AW_REG_BIT_CLR32(&p_reg->MCR, __MCR_FDEN_SHIFT);

        /* 初始化接收MB */
        for (i = __CAN_MBN_RECV_CHN_START; i < __CAN_MBN_RECV_CHN_START + __CAN_MBN_RECV_CHN_NUM; i++) {
            AW_REG_BITS_SET32(&(p_reg->MB_8B[i].CS),
                              __CAN_MBN_BLK0_CODE_SHIFT,
                              __CAN_MBN_BLK0_CODE_LEN,
                              __CAN_MBN_CODE_RX_EMPTY);
        }

        /* 初始化发送MB */
        AW_REG_BITS_SET32(&(p_reg->MB_8B[__CAN_MBN_SEND_CHN].CS),
                          __CAN_MBN_BLK0_CODE_SHIFT,
                          __CAN_MBN_BLK0_CODE_LEN,
                          __CAN_MBN_CODE_TX_INACTIVE);

    } else if (type == AW_CAN_CTLR_FD) {
        AW_REG_BIT_SET32(&p_reg->MCR, __MCR_FDEN_SHIFT);
        AW_REG_BIT_SET32(&p_reg->FDCTRL, __FDCTRL_FDRATE_SHIFT);

        /* 设置MB的最大的有效字节数据为最大的64字节 */
        AW_REG_BITS_SET32(&p_reg->FDCTRL,
                          __FDCTRL_MBDSR0_SHIFT,
                          __FDCTRL_MBDSR0_LEN,
                          3);
        AW_REG_BITS_SET32(&p_reg->FDCTRL,
                          __FDCTRL_MBDSR1_SHIFT,
                          __FDCTRL_MBDSR1_LEN,
                          3);

        /* 初始化接收MB */
        for (i = __CAN_MBN_RECV_CHN_START; i < __CAN_MBN_RECV_CHN_START + __CAN_MBN_RECV_CHN_NUM; i++) {
            if (i < 7) {
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB0[i].CS),
                                  __CAN_MBN_BLK0_CODE_SHIFT,
                                  __CAN_MBN_BLK0_CODE_LEN,
                                  __CAN_MBN_CODE_RX_EMPTY);
            } else {
                AW_REG_BITS_SET32(&(p_reg->MB_64B.MB7[i - 7].CS),
                                  __CAN_MBN_BLK0_CODE_SHIFT,
                                  __CAN_MBN_BLK0_CODE_LEN,
                                  __CAN_MBN_CODE_RX_EMPTY);
            }
        }

        /* 初始化发送MB */
        if (__CAN_MBN_SEND_CHN < 7) {
            AW_REG_BITS_SET32(&(p_reg->MB_64B.MB0[__CAN_MBN_SEND_CHN].CS),
                              __CAN_MBN_BLK0_CODE_SHIFT,
                              __CAN_MBN_BLK0_CODE_LEN,
                              __CAN_MBN_CODE_TX_INACTIVE);
        } else {
            AW_REG_BITS_SET32(&(p_reg->MB_64B.MB7[__CAN_MBN_SEND_CHN - 7].CS),
                              __CAN_MBN_BLK0_CODE_SHIFT,
                              __CAN_MBN_BLK0_CODE_LEN,
                              __CAN_MBN_CODE_TX_INACTIVE);
        }
    } else {
        return -AW_CAN_ERR_ILLEGAL_CTRL_TYPE;
    }

    p_this->ctrl_type = type;  /* 记录当前控制器类型 */

    /* 退出冻结模式  */
    __imx10xx_can_reset_haltfrz(p_serv);

    return AW_CAN_ERR_NONE;
}
#endif




/**
 * \brief register imx10xx can driver
 *
 * This routine registers the imx10xx can driver and device recognition
 * data with the AWBus subsystem.
 *
 * NOTE: This routine is called early during system initialization, and
 * *MUST NOT* make calls to OS facilities such as memory allocation
 * and I/O.
 */
void awbl_imx10xx_flexcanfd_drv_register (void)
{
    awbl_drv_register(&__g_imx10xx_canfd_drv_registration);
}

/* end of file */

