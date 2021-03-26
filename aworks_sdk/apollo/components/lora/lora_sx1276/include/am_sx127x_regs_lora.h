/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https:/*www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief SX1276 LoRa modem registers and bits definitions
 *
 * \internal
 * \par Modification history
 * - 1.0.0 17-09-25  sky, first implementation.
 * \endinternal
 */

#ifndef __AM_SX127X_REGS_LORA_H
#define __AM_SX127X_REGS_LORA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup am_if_sx1276_regs_lora
 * @copydoc am_sx1276_regs_lora.h
 * @{
 */

/**
 * \brief SX1276 Internal registers Address
 */
#define AM_SX127X_LORA_REG_FIFO                             0x00
/* Common settings */
#define AM_SX127X_LORA_REG_OPMODE                           0x01
#define AM_SX127X_LORA_REG_FRFMSB                           0x06
#define AM_SX127X_LORA_REG_FRFMID                           0x07
#define AM_SX127X_LORA_REG_FRFLSB                           0x08
/* Tx settings */
#define AM_SX127X_LORA_REG_PACONFIG                         0x09
#define AM_SX127X_LORA_REG_PARAMP                           0x0A
#define AM_SX127X_LORA_REG_OCP                              0x0B
/* Rx settings */
#define AM_SX127X_LORA_REG_LNA                              0x0C
/* LoRa registers */
#define AM_SX127X_LORA_REG_FIFOADDRPTR                      0x0D
#define AM_SX127X_LORA_REG_FIFOTXBASEADDR                   0x0E
#define AM_SX127X_LORA_REG_FIFORXBASEADDR                   0x0F
#define AM_SX127X_LORA_REG_FIFORXCURRENTADDR                0x10
#define AM_SX127X_LORA_REG_IRQFLAGSMASK                     0x11
#define AM_SX127X_LORA_REG_IRQFLAGS                         0x12
#define AM_SX127X_LORA_REG_RXNBBYTES                        0x13
#define AM_SX127X_LORA_REG_RXHEADERCNTVALUEMSB              0x14
#define AM_SX127X_LORA_REG_RXHEADERCNTVALUELSB              0x15
#define AM_SX127X_LORA_REG_RXPACKETCNTVALUEMSB              0x16
#define AM_SX127X_LORA_REG_RXPACKETCNTVALUELSB              0x17
#define AM_SX127X_LORA_REG_MODEMSTAT                        0x18
#define AM_SX127X_LORA_REG_PKTSNRVALUE                      0x19
#define AM_SX127X_LORA_REG_PKTRSSIVALUE                     0x1A
#define AM_SX127X_LORA_REG_RSSIVALUE                        0x1B
#define AM_SX127X_LORA_REG_HOPCHANNEL                       0x1C
#define AM_SX127X_LORA_REG_MODEMCONFIG1                     0x1D
#define AM_SX127X_LORA_REG_MODEMCONFIG2                     0x1E
#define AM_SX127X_LORA_REG_SYMBTIMEOUTLSB                   0x1F
#define AM_SX127X_LORA_REG_PREAMBLEMSB                      0x20
#define AM_SX127X_LORA_REG_PREAMBLELSB                      0x21
#define AM_SX127X_LORA_REG_PAYLOADLENGTH                    0x22
#define AM_SX127X_LORA_REG_PAYLOADMAXLENGTH                 0x23
#define AM_SX127X_LORA_REG_HOPPERIOD                        0x24
#define AM_SX127X_LORA_REG_FIFORXBYTEADDR                   0x25
#define AM_SX127X_LORA_REG_MODEMCONFIG3                     0x26
#define AM_SX127X_LORA_REG_FEIMSB                           0x28
#define AM_SX127X_LORA_REG_FEIMID                           0x29
#define AM_SX127X_LORA_REG_FEILSB                           0x2A
#define AM_SX127X_LORA_REG_RSSIWIDEBAND                     0x2C
#define AM_SX127X_LORA_REG_TEST2F                           0x2F
#define AM_SX127X_LORA_REG_TEST30                           0x30
#define AM_SX127X_LORA_REG_DETECTOPTIMIZE                   0x31
#define AM_SX127X_LORA_REG_INVERTIQ                         0x33
#define AM_SX127X_LORA_REG_TEST36                           0x36
#define AM_SX127X_LORA_REG_DETECTIONTHRESHOLD               0x37
#define AM_SX127X_LORA_REG_SYNCWORD                         0x39
#define AM_SX127X_LORA_REG_TEST3A                           0x3A
#define AM_SX127X_LORA_REG_INVERTIQ2                        0x3B

/* end of documented register in datasheet */
/* I/O settings */
#define AM_SX127X_LORA_REG_DIOMAPPING1                      0x40
#define AM_SX127X_LORA_REG_DIOMAPPING2                      0x41
/* Version */
#define AM_SX127X_LORA_REG_VERSION                          0x42
/* Additional settings */
#define AM_SX127X_LORA_REG_PLLHOP                           0x44
#define AM_SX127X_LORA_REG_TCXO                             0x4B
#define AM_SX127X_LORA_REG_PADAC                            0x4D
#define AM_SX127X_LORA_REG_FORMERTEMP                       0x5B
#define AM_SX127X_LORA_REG_BITRATEFRAC                      0x5D
#define AM_SX127X_LORA_REG_AGCREF                           0x61
#define AM_SX127X_LORA_REG_AGCTHRESH1                       0x62
#define AM_SX127X_LORA_REG_AGCTHRESH2                       0x63
#define AM_SX127X_LORA_REG_AGCTHRESH3                       0x64
#define AM_SX127X_LORA_REG_PLL                              0x70

/**
 * \brief SX1276 LoRa bits control definition
 * 
 */

/**
 * RegFifo
 */

/**
 * \brief RegOpMode
 */
#define AM_SX127X_LORA_RF_OPMODE_LONGRANGEMODE_MASK         0x7F
#define AM_SX127X_LORA_RF_OPMODE_LONGRANGEMODE_OFF          0x00 /* Default */
#define AM_SX127X_LORA_RF_OPMODE_LONGRANGEMODE_ON           0x80

#define AM_SX127X_LORA_RF_OPMODE_ACCESSSHAREDREG_MASK       0xBF
#define AM_SX127X_LORA_RF_OPMODE_ACCESSSHAREDREG_ENABLE     0x40
#define AM_SX127X_LORA_RF_OPMODE_ACCESSSHAREDREG_DISABLE    0x00 /* Default */

#define AM_SX127X_LORA_RF_OPMODE_FREQMODE_ACCESS_MASK       0xF7
#define AM_SX127X_LORA_RF_OPMODE_FREQMODE_ACCESS_LF         0x08 /* Default */
#define AM_SX127X_LORA_RF_OPMODE_FREQMODE_ACCESS_HF         0x00

#define AM_SX127X_LORA_RF_OPMODE_MASK                       0xF8
#define AM_SX127X_LORA_RF_OPMODE_SLEEP                      0x00
#define AM_SX127X_LORA_RF_OPMODE_STANDBY                    0x01 /* Default */
#define AM_SX127X_LORA_RF_OPMODE_SYNTHESIZER_TX             0x02
#define AM_SX127X_LORA_RF_OPMODE_TRANSMITTER                0x03
#define AM_SX127X_LORA_RF_OPMODE_SYNTHESIZER_RX             0x04
#define AM_SX127X_LORA_RF_OPMODE_RECEIVER                   0x05
/* LoRa specific modes */
#define AM_SX127X_LORA_RF_OPMODE_RECEIVER_SINGLE            0x06
#define AM_SX127X_LORA_RF_OPMODE_CAD                        0x07

/**
 * \brief RegFrf (MHz)
 */
#define AM_SX127X_LORA_RF_FRFMSB_434_MHZ                    0x6C /* Default */
#define AM_SX127X_LORA_RF_FRFMID_434_MHZ                    0x80 /* Default */
#define AM_SX127X_LORA_RF_FRFLSB_434_MHZ                    0x00 /* Default */

/**
 * \brief RegPaConfig
 */
#define AM_SX127X_LORA_RF_PACONFIG_PASELECT_MASK            0x7F
#define AM_SX127X_LORA_RF_PACONFIG_PASELECT_PABOOST         0x80
#define AM_SX127X_LORA_RF_PACONFIG_PASELECT_RFO             0x00 /* Default */

#define AM_SX127X_LORA_RF_PACONFIG_MAX_POWER_MASK           0x8F

#define AM_SX127X_LORA_RF_PACONFIG_OUTPUTPOWER_MASK         0xF0

/**
 * \brief RegPaRamp
 */
#define AM_SX127X_LORA_RF_PARAMP_TXBANDFORCE_MASK           0xEF
#define AM_SX127X_LORA_RF_PARAMP_TXBANDFORCE_BAND_SEL       0x10
#define AM_SX127X_LORA_RF_PARAMP_TXBANDFORCE_AUTO           0x00 /* Default */

#define AM_SX127X_LORA_RF_PARAMP_MASK                       0xF0
#define AM_SX127X_LORA_RF_PARAMP_3400_US                    0x00
#define AM_SX127X_LORA_RF_PARAMP_2000_US                    0x01
#define AM_SX127X_LORA_RF_PARAMP_1000_US                    0x02
#define AM_SX127X_LORA_RF_PARAMP_0500_US                    0x03
#define AM_SX127X_LORA_RF_PARAMP_0250_US                    0x04
#define AM_SX127X_LORA_RF_PARAMP_0125_US                    0x05
#define AM_SX127X_LORA_RF_PARAMP_0100_US                    0x06
#define AM_SX127X_LORA_RF_PARAMP_0062_US                    0x07
#define AM_SX127X_LORA_RF_PARAMP_0050_US                    0x08
#define AM_SX127X_LORA_RF_PARAMP_0040_US                    0x09 /* Default */
#define AM_SX127X_LORA_RF_PARAMP_0031_US                    0x0A
#define AM_SX127X_LORA_RF_PARAMP_0025_US                    0x0B
#define AM_SX127X_LORA_RF_PARAMP_0020_US                    0x0C
#define AM_SX127X_LORA_RF_PARAMP_0015_US                    0x0D
#define AM_SX127X_LORA_RF_PARAMP_0012_US                    0x0E
#define AM_SX127X_LORA_RF_PARAMP_0010_US                    0x0F

/**
 * \brief RegOcp
 */
#define AM_SX127X_LORA_RF_OCP_MASK                          0xDF
#define AM_SX127X_LORA_RF_OCP_ON                            0x20 /* Default */
#define AM_SX127X_LORA_RF_OCP_OFF                           0x00

#define AM_SX127X_LORA_RF_OCP_TRIM_MASK                     0xE0
#define AM_SX127X_LORA_RF_OCP_TRIM_045_MA                   0x00
#define AM_SX127X_LORA_RF_OCP_TRIM_050_MA                   0x01
#define AM_SX127X_LORA_RF_OCP_TRIM_055_MA                   0x02
#define AM_SX127X_LORA_RF_OCP_TRIM_060_MA                   0x03
#define AM_SX127X_LORA_RF_OCP_TRIM_065_MA                   0x04
#define AM_SX127X_LORA_RF_OCP_TRIM_070_MA                   0x05
#define AM_SX127X_LORA_RF_OCP_TRIM_075_MA                   0x06
#define AM_SX127X_LORA_RF_OCP_TRIM_080_MA                   0x07
#define AM_SX127X_LORA_RF_OCP_TRIM_085_MA                   0x08
#define AM_SX127X_LORA_RF_OCP_TRIM_090_MA                   0x09
#define AM_SX127X_LORA_RF_OCP_TRIM_095_MA                   0x0A
#define AM_SX127X_LORA_RF_OCP_TRIM_100_MA                   0x0B /* Default */
#define AM_SX127X_LORA_RF_OCP_TRIM_105_MA                   0x0C
#define AM_SX127X_LORA_RF_OCP_TRIM_110_MA                   0x0D
#define AM_SX127X_LORA_RF_OCP_TRIM_115_MA                   0x0E
#define AM_SX127X_LORA_RF_OCP_TRIM_120_MA                   0x0F
#define AM_SX127X_LORA_RF_OCP_TRIM_130_MA                   0x10
#define AM_SX127X_LORA_RF_OCP_TRIM_140_MA                   0x11
#define AM_SX127X_LORA_RF_OCP_TRIM_150_MA                   0x12
#define AM_SX127X_LORA_RF_OCP_TRIM_160_MA                   0x13
#define AM_SX127X_LORA_RF_OCP_TRIM_170_MA                   0x14
#define AM_SX127X_LORA_RF_OCP_TRIM_180_MA                   0x15
#define AM_SX127X_LORA_RF_OCP_TRIM_190_MA                   0x16
#define AM_SX127X_LORA_RF_OCP_TRIM_200_MA                   0x17
#define AM_SX127X_LORA_RF_OCP_TRIM_210_MA                   0x18
#define AM_SX127X_LORA_RF_OCP_TRIM_220_MA                   0x19
#define AM_SX127X_LORA_RF_OCP_TRIM_230_MA                   0x1A
#define AM_SX127X_LORA_RF_OCP_TRIM_240_MA                   0x1B

/**
 * \brief RegLna
 */
#define AM_SX127X_LORA_RF_LNA_GAIN_MASK                     0x1F
#define AM_SX127X_LORA_RF_LNA_GAIN_G1                       0x20 /* Default */
#define AM_SX127X_LORA_RF_LNA_GAIN_G2                       0x40
#define AM_SX127X_LORA_RF_LNA_GAIN_G3                       0x60
#define AM_SX127X_LORA_RF_LNA_GAIN_G4                       0x80
#define AM_SX127X_LORA_RF_LNA_GAIN_G5                       0xA0
#define AM_SX127X_LORA_RF_LNA_GAIN_G6                       0xC0

#define AM_SX127X_LORA_RF_LNA_BOOST_LF_MASK                 0xE7
#define AM_SX127X_LORA_RF_LNA_BOOST_LF_DEFAULT              0x00 /* Default */

#define AM_SX127X_LORA_RF_LNA_BOOST_HF_MASK                 0xFC
#define AM_SX127X_LORA_RF_LNA_BOOST_HF_OFF                  0x00 /* Default */
#define AM_SX127X_LORA_RF_LNA_BOOST_HF_ON                   0x03

/**
 * \brief RegFifoAddrPtr
 */
#define AM_SX127X_LORA_RF_FIFOADDRPTR                       0x00 /* Default */

/**
 * \brief RegFifoTxBaseAddr
 */
#define AM_SX127X_LORA_RF_FIFOTXBASEADDR                    0x80 /* Default */

/**
 * \brief RegFifoTxBaseAddr
 */
#define AM_SX127X_LORA_RF_FIFORXBASEADDR                    0x00 /* Default */

/**
 * \brief RegFifoRxCurrentAddr (Read Only)
 */

/**
 * \brief RegIrqFlagsMask
 */
#define AM_SX127X_LORA_RF_IRQFLAGS_RXTIMEOUT_MASK           0x80
#define AM_SX127X_LORA_RF_IRQFLAGS_RXDONE_MASK              0x40
#define AM_SX127X_LORA_RF_IRQFLAGS_PAYLOADCRCERROR_MASK     0x20
#define AM_SX127X_LORA_RF_IRQFLAGS_VALIDHEADER_MASK         0x10
#define AM_SX127X_LORA_RF_IRQFLAGS_TXDONE_MASK              0x08
#define AM_SX127X_LORA_RF_IRQFLAGS_CADDONE_MASK             0x04
#define AM_SX127X_LORA_RF_IRQFLAGS_FHSSCHANGEDCHANNEL_MASK  0x02
#define AM_SX127X_LORA_RF_IRQFLAGS_CADDETECTED_MASK         0x01

/**
 * \brief RegIrqFlags
 */
#define AM_SX127X_LORA_RF_IRQFLAGS_RXTIMEOUT                0x80
#define AM_SX127X_LORA_RF_IRQFLAGS_RXDONE                   0x40
#define AM_SX127X_LORA_RF_IRQFLAGS_PAYLOADCRCERROR          0x20
#define AM_SX127X_LORA_RF_IRQFLAGS_VALIDHEADER              0x10
#define AM_SX127X_LORA_RF_IRQFLAGS_TXDONE                   0x08
#define AM_SX127X_LORA_RF_IRQFLAGS_CADDONE                  0x04
#define AM_SX127X_LORA_RF_IRQFLAGS_FHSSCHANGEDCHANNEL       0x02
#define AM_SX127X_LORA_RF_IRQFLAGS_CADDETECTED              0x01

/**
 * \brief RegFifoRxNbBytes (Read Only)
 */

/**
 * \brief RegRxHeaderCntValueMsb (Read Only)
 */

/**
 * \brief RegRxHeaderCntValueLsb (Read Only)
 */

/**
 * \brief RegRxPacketCntValueMsb (Read Only)
 */

/**
 * \brief RegRxPacketCntValueLsb (Read Only)
 */

/**
 * \brief RegModemStat (Read Only)
 */
#define AM_SX127X_LORA_RF_MODEMSTAT_RX_CR_MASK              0x1F
#define AM_SX127X_LORA_RF_MODEMSTAT_MODEM_STATUS_MASK       0xE0

/**
 * \brief RegPktSnrValue (Read Only)
 */

/**
 * \brief RegPktRssiValue (Read Only)
 */

/**
 * \brief RegRssiValue (Read Only)
 */

/**
 * \brief RegHopChannel (Read Only)
 */
#define AM_SX127X_LORA_RF_HOPCHANNEL_PLL_LOCK_TIMEOUT_MASK  0x7F
#define AM_SX127X_LORA_RF_HOPCHANNEL_PLL_LOCK_FAIL          0x80
#define AM_SX127X_LORA_RF_HOPCHANNEL_PLL_LOCK_SUCCEED       0x00 /* Default */

#define AM_SX127X_LORA_RF_HOPCHANNEL_CRCONPAYLOAD_MASK      0xBF
#define AM_SX127X_LORA_RF_HOPCHANNEL_CRCONPAYLOAD_ON        0x40
#define AM_SX127X_LORA_RF_HOPCHANNEL_CRCONPAYLOAD_OFF       0x00 /* Default */

#define AM_SX127X_LORA_RF_HOPCHANNEL_CHANNEL_MASK           0x3F

/**
 * \brief RegModemConfig1
 */
#define AM_SX127X_LORA_RF_MODEMCONFIG1_BW_MASK              0x0F
#define AM_SX127X_LORA_RF_MODEMCONFIG1_BW_7_81_KHZ          0x00
#define AM_SX127X_LORA_RF_MODEMCONFIG1_BW_10_41_KHZ         0x10
#define AM_SX127X_LORA_RF_MODEMCONFIG1_BW_15_62_KHZ         0x20
#define AM_SX127X_LORA_RF_MODEMCONFIG1_BW_20_83_KHZ         0x30
#define AM_SX127X_LORA_RF_MODEMCONFIG1_BW_31_25_KHZ         0x40
#define AM_SX127X_LORA_RF_MODEMCONFIG1_BW_41_66_KHZ         0x50
#define AM_SX127X_LORA_RF_MODEMCONFIG1_BW_62_50_KHZ         0x60
#define AM_SX127X_LORA_RF_MODEMCONFIG1_BW_125_KHZ           0x70 /* Default */
#define AM_SX127X_LORA_RF_MODEMCONFIG1_BW_250_KHZ           0x80
#define AM_SX127X_LORA_RF_MODEMCONFIG1_BW_500_KHZ           0x90

#define AM_SX127X_LORA_RF_MODEMCONFIG1_CODINGRATE_MASK      0xF1
#define AM_SX127X_LORA_RF_MODEMCONFIG1_CODINGRATE_4_5       0x02
#define AM_SX127X_LORA_RF_MODEMCONFIG1_CODINGRATE_4_6       0x04 /* Default */
#define AM_SX127X_LORA_RF_MODEMCONFIG1_CODINGRATE_4_7       0x06
#define AM_SX127X_LORA_RF_MODEMCONFIG1_CODINGRATE_4_8       0x08

#define AM_SX127X_LORA_RF_MODEMCONFIG1_IMPLICITHEADER_MASK  0xFE
#define AM_SX127X_LORA_RF_MODEMCONFIG1_IMPLICITHEADER_ON    0x01
#define AM_SX127X_LORA_RF_MODEMCONFIG1_IMPLICITHEADER_OFF   0x00 /* Default */

/**
 * \brief RegModemConfig2
 */
#define AM_SX127X_LORA_RF_MODEMCONFIG2_SF_MASK              0x0F
#define AM_SX127X_LORA_RF_MODEMCONFIG2_SF_6                 0x60
#define AM_SX127X_LORA_RF_MODEMCONFIG2_SF_7                 0x70 /* Default */
#define AM_SX127X_LORA_RF_MODEMCONFIG2_SF_8                 0x80
#define AM_SX127X_LORA_RF_MODEMCONFIG2_SF_9                 0x90
#define AM_SX127X_LORA_RF_MODEMCONFIG2_SF_10                0xA0
#define AM_SX127X_LORA_RF_MODEMCONFIG2_SF_11                0xB0
#define AM_SX127X_LORA_RF_MODEMCONFIG2_SF_12                0xC0

#define AM_SX127X_LORA_RF_MODEMCONFIG2_TXCONTINUOUSMODE_MASK 0xF7
#define AM_SX127X_LORA_RF_MODEMCONFIG2_TXCONTINUOUSMODE_ON   0x08
#define AM_SX127X_LORA_RF_MODEMCONFIG2_TXCONTINUOUSMODE_OFF  0x00

#define AM_SX127X_LORA_RF_MODEMCONFIG2_RXPAYLOADCRC_MASK     0xFB
#define AM_SX127X_LORA_RF_MODEMCONFIG2_RXPAYLOADCRC_ON       0x04
#define AM_SX127X_LORA_RF_MODEMCONFIG2_RXPAYLOADCRC_OFF      0x00 /* Default */

#define AM_SX127X_LORA_RF_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK   0xFC
#define AM_SX127X_LORA_RF_MODEMCONFIG2_SYMBTIMEOUTMSB        0x00 /* Default */

/**
 * \brief RegSymbTimeoutLsb
 */
#define AM_SX127X_LORA_RF_SYMBTIMEOUTLSB_SYMBTIMEOUT        0x64 /* Default */

/**
 * \brief RegPreambleLengthMsb
 */
#define AM_SX127X_LORA_RF_PREAMBLELENGTHMSB                 0x00 /* Default */

/**
 * \brief RegPreambleLengthLsb
 */
#define AM_SX127X_LORA_RF_PREAMBLELENGTHLSB                 0x08 /* Default */

/**
 * \brief RegPayloadLength
 */
#define AM_SX127X_LORA_RF_PAYLOADLENGTH                     0x0E /* Default */

/**
 * \brief RegPayloadMaxLength
 */
#define AM_SX127X_LORA_RF_PAYLOADMAXLENGTH                  0xFF /* Default */

/**
 * \brief RegHopPeriod
 */
#define AM_SX127X_LORA_RF_HOPPERIOD_FREQFOPPINGPERIOD       0x00 /* Default */

/**
 * \brief RegFifoRxByteAddr (Read Only)
 */

/**
 * \brief RegModemConfig3
 */
#define AM_SX127X_LORA_RF_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK  0xF7
#define AM_SX127X_LORA_RF_MODEMCONFIG3_LOWDATARATEOPTIMIZE_ON    0x08
#define AM_SX127X_LORA_RF_MODEMCONFIG3_LOWDATARATEOPTIMIZE_OFF   0x00 /* Default */

#define AM_SX127X_LORA_RF_MODEMCONFIG3_AGCAUTO_MASK              0xFB
#define AM_SX127X_LORA_RF_MODEMCONFIG3_AGCAUTO_ON                0x04 /* Default */
#define AM_SX127X_LORA_RF_MODEMCONFIG3_AGCAUTO_OFF               0x00

/**
 * \brief RegFeiMsb (Read Only)
 */

/**
 * \brief RegFeiMid (Read Only)
 */

/**
 * \brief RegFeiLsb (Read Only)
 */

/**
 * \brief RegRssiWideband (Read Only)
 */

/**
 * \brief RegDetectOptimize
 */
#define AM_SX127X_LORA_RF_DETECTIONOPTIMIZE_MASK            0xF8
#define AM_SX127X_LORA_RF_DETECTIONOPTIMIZE_SF7_TO_SF12     0x03 /* Default */
#define AM_SX127X_LORA_RF_DETECTIONOPTIMIZE_SF6             0x05

/**
 * \brief RegInvertIQ
 */
#define AM_SX127X_LORA_RF_INVERTIQ_RX_MASK                  0xBF
#define AM_SX127X_LORA_RF_INVERTIQ_RX_OFF                   0x00
#define AM_SX127X_LORA_RF_INVERTIQ_RX_ON                    0x40
#define AM_SX127X_LORA_RF_INVERTIQ_TX_MASK                  0xFE
#define AM_SX127X_LORA_RF_INVERTIQ_TX_OFF                   0x01
#define AM_SX127X_LORA_RF_INVERTIQ_TX_ON                    0x00

/**
 * \brief RegDetectionThreshold
 */
#define AM_SX127X_LORA_RF_DETECTIONTHRESH_SF7_TO_SF12       0x0A /* Default */
#define AM_SX127X_LORA_RF_DETECTIONTHRESH_SF6               0x0C

/**
 * \brief RegInvertIQ2
 */
#define AM_SX127X_LORA_RF_INVERTIQ2_ON                      0x19
#define AM_SX127X_LORA_RF_INVERTIQ2_OFF                     0x1D

/**
 *\brief  RegDioMapping1
 */
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO0_MASK             0x3F
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO0_00               0x00 /* Default */
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO0_01               0x40
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO0_10               0x80
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO0_11               0xC0

#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO1_MASK             0xCF
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO1_00               0x00 /* Default */
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO1_01               0x10
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO1_10               0x20
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO1_11               0x30

#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO2_MASK             0xF3
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO2_00               0x00 /* Default */
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO2_01               0x04
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO2_10               0x08
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO2_11               0x0C

#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO3_MASK             0xFC
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO3_00               0x00 /* Default */
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO3_01               0x01
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO3_10               0x02
#define AM_SX127X_LORA_RF_DIOMAPPING1_DIO3_11               0x03

/**
 * \brief RegDioMapping2
 */
#define AM_SX127X_LORA_RF_DIOMAPPING2_DIO4_MASK             0x3F
#define AM_SX127X_LORA_RF_DIOMAPPING2_DIO4_00               0x00 /* Default */
#define AM_SX127X_LORA_RF_DIOMAPPING2_DIO4_01               0x40
#define AM_SX127X_LORA_RF_DIOMAPPING2_DIO4_10               0x80
#define AM_SX127X_LORA_RF_DIOMAPPING2_DIO4_11               0xC0

#define AM_SX127X_LORA_RF_DIOMAPPING2_DIO5_MASK             0xCF
#define AM_SX127X_LORA_RF_DIOMAPPING2_DIO5_00               0x00 /* Default */
#define AM_SX127X_LORA_RF_DIOMAPPING2_DIO5_01               0x10
#define AM_SX127X_LORA_RF_DIOMAPPING2_DIO5_10               0x20
#define AM_SX127X_LORA_RF_DIOMAPPING2_DIO5_11               0x30

#define AM_SX127X_LORA_RF_DIOMAPPING2_MAP_MASK              0xFE
#define AM_SX127X_LORA_RF_DIOMAPPING2_MAP_PREAMBLEDETECT    0x01
#define AM_SX127X_LORA_RF_DIOMAPPING2_MAP_RSSI              0x00 /* Default */

/**
 * \brief RegVersion (Read Only)
 */

/**
 * \brief RegPllHop
 */
#define AM_SX127X_LORA_RF_PLLHOP_FASTHOP_MASK               0x7F
#define AM_SX127X_LORA_RF_PLLHOP_FASTHOP_ON                 0x80
#define AM_SX127X_LORA_RF_PLLHOP_FASTHOP_OFF                0x00 /* Default */

/**
 * \brief RegTcxo
 */
#define AM_SX127X_LORA_RF_TCXO_TCXOINPUT_MASK               0xEF
#define AM_SX127X_LORA_RF_TCXO_TCXOINPUT_ON                 0x10
#define AM_SX127X_LORA_RF_TCXO_TCXOINPUT_OFF                0x00 /* Default */

/**
 * \brief RegPaDac
 */
#define AM_SX127X_LORA_RF_PADAC_20DBM_MASK                  0xF8
#define AM_SX127X_LORA_RF_PADAC_20DBM_ON                    0x07
#define AM_SX127X_LORA_RF_PADAC_20DBM_OFF                   0x04 /* Default */

/**
 * \brief RegFormerTemp
 */

/**
 * \brief RegBitrateFrac
 */
#define AM_SX127X_LORA_RF_BITRATEFRAC_MASK                  0xF0

/**
 * \brief RegAgcRef
 */

/**
 * \brief RegAgcThresh1
 */

/**
 * \brief RegAgcThresh2
 */

/**
 * \brief RegAgcThresh3
 */

/**
 * \brief RegPll
 */
#define AM_SX127X_LORA_RF_PLL_BANDWIDTH_MASK                0x3F
#define AM_SX127X_LORA_RF_PLL_BANDWIDTH_75                  0x00
#define AM_SX127X_LORA_RF_PLL_BANDWIDTH_150                 0x40
#define AM_SX127X_LORA_RF_PLL_BANDWIDTH_225                 0x80
#define AM_SX127X_LORA_RF_PLL_BANDWIDTH_300                 0xC0 /* Default */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_SX127X_REGS_LORA_H */

/* end of file */
