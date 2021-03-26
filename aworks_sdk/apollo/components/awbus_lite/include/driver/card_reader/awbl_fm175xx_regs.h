/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief FM175XX register define.
 *
 * \internal
 * \par Modification History
 * - 1.00 18-10-19 mkr, first implementation.
 * \endinternal
 */
#ifndef __AWBL_FM175XX_REGS_H
#define __AWBL_FM175XX_REGS_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \brief FM175XX register definition
 * @{
 */
/* Page register in page 0 */
#define AWBL_FM175XX_PAGE0_REG         0x00 
/* Contains Command bits, PowerDown bit and bit to switch receiver off. */
#define AWBL_FM175XX_COMMAND_REG       0x01 
/* Contains Communication interrupt enable bits and bit for Interrupt inversion. */
#define AWBL_FM175XX_COMMIEN_REG       0x02 
/* Contains CRC Interrupt enable and bit to switch Interrupt pin to PushPull mode.*/
#define AWBL_FM175XX_DIVIEN_REG        0x03  
/* Contains Communication interrupt request bits. */                                       
#define AWBL_FM175XX_COMMIRQ_REG       0x04  
/* Contains CRC Interrupt request.*/                                      
#define AWBL_FM175XX_DIVIRQ_REG        0x05                                                                                         
/* 
 * Contains Protocol, Parity, CRC, Collision, 
 * Buffer overflow, Temperature and RF error flags. 
 */
#define AWBL_FM175XX_ERROR_REG         0x06                            
/* 
 * Contains status information about Lo- and HiAlert, 
 * Timer, Interrupt request and CRC status.
 */
#define AWBL_FM175XX_STATUS1_REG       0x07    
/* 
 * Contains information about internal states (Modemstate),Mifare 
 * states and possibility to switch Temperature sensor off.
 */                         
#define AWBL_FM175XX_STATUS2_REG       0x08 
/* 
 * Gives access to FIFO. Writing to register increments 
 * theFIFO level (register 0x0A), reading decrements it.
 */
#define AWBL_FM175XX_FIFODATA_REG      0x09 
/* Contains the actual level of the FIFO. */
#define AWBL_FM175XX_FIFOLEVEL_REG     0x0A 
/* Contains the Waterlevel value for the FIFO*/
#define AWBL_FM175XX_WATERLEVEL_REG    0x0B 
/* 
 * Contains information about last received bits, 
 * bit to copy NFCID to FIFO and Start Send bit .
 */
#define AWBL_FM175XX_CONTROL_REG       0x0C 
/* 
 * Contains information of last bits to send, 
 * to align received bits in FIFO and activate sending in Transceive
 */
#define AWBL_FM175XX_BITFRAMING_REG    0x0D 
/* Contains all necessary bits for Collision handling*/
#define AWBL_FM175XX_COLL_REG          0x0E 
/* extension register. */
#define AWBL_FM175XX_EX_REG            0x0F 

/* Page register in page 1*/
#define AWBL_FM175XX_PAGE1_REG         0x10 
/* Contains bits for crc preset, auto wait on Rf, MSB first for CRC calculation*/
#define AWBL_FM175XX_MODE_REG          0x11 
/* Contains Speed, CRC enable, bit for inverse mode in TX.*/
#define AWBL_FM175XX_TXMODE_REG        0x12 
/* Contains Speed, CRC enable, bit for multiple receive and to filter errors in RCV. */
#define AWBL_FM175XX_RXMODE_REG        0x13 
/* TX pin control in aerial driver.*/
#define AWBL_FM175XX_TXCONTROL_REG     0x14 
/* aerial driver setting.*/
#define AWBL_FM175XX_TXASK_REG         0x15 
/* aerial driver signal source selection.*/
#define AWBL_FM175XX_TXSEL_REG         0x16 
/* Contains UartSel and RxWait bits.*/
#define AWBL_FM175XX_RXSEL_REG         0x17 
/* Contains MinLevel and CollLevel for detection.*/
#define AWBL_FM175XX_RXTHRESHOLD_REG   0x18 
/* demodulator settings.*/
#define AWBL_FM175XX_DEMOD_REG         0x19 
/* reserve for used.*/
#define AWBL_FM175XX_RFU1A_REG         0x1A 
/* reserve for used.*/
#define AWBL_FM175XX_RFU1B_REG         0x1B 
/* Contains TxWait settings bit. */
#define AWBL_FM175XX_TX_REG            0x1C 
/* contains parity disable bit.*/
#define AWBL_FM175XX_RX_REG            0x1D 
/* Currently not used.*/
#define AWBL_FM175XX_RFU1E_REG         0x1E 
/* Currently not used.*/
#define AWBL_FM175XX_RFU1F_REG         0x1F 

/* Page register in page 2*/
#define AWBL_FM175XX_PAGE2_REG         0x20 
/* Contains MSByte of CRC Result.*/
#define AWBL_FM175XX_CRCRESULTMSB_REG  0x21 
/* Contains LSByte of CRC Result.*/
#define AWBL_FM175XX_CRCRESULTLSB_REG  0x22 
/* Currently not used.*/
#define AWBL_FM175XX_RFU23_REG         0x23 
/* Contains modulation width setting.*/
#define AWBL_FM175XX_MODWIDTH_REG      0x24 
/* Currently not used.*/
#define AWBL_FM175XX_RFU25_REG         0x25 
/* Contains sensitivity of the receiver gain factor. */
#define AWBL_FM175XX_RFCFG_REG         0x26 
/* 
 * Contains the conductance and the modulation settings for the N-MOS 
 * transistor during active modulation (no load modulation setting!).
 */
#define AWBL_FM175XX_GSN_REG           0x27 
/* Contains the conductance for the P-Mos transistor. */
#define AWBL_FM175XX_CWGSP_REG         0x28 
/* Contains the modulation index for the PMos transistor.*/
#define AWBL_FM175XX_MODGSP_REG        0x29 
/* Contains all settings for the timer and the highest 4 bits of the prescaler.*/
#define AWBL_FM175XX_TMODE_REG         0x2A 
/* Contais the lowest byte of the prescaler. */
#define AWBL_FM175XX_TPRESCALER_REG    0x2B 
/* Contains the high byte of the reload value.*/
#define AWBL_FM175XX_TRELOADHI_REG     0x2C 
/* Contains the low byte of the reload value. */
#define AWBL_FM175XX_TRELOADLO_REG     0x2D 
/* Contains the high byte of the counter value. */
#define AWBL_FM175XX_TCOUNTERVALHI_REG 0x2E 
/* Contains the low byte of the counter value.*/
#define AWBL_FM175XX_TCOUNTERVALLO_REG 0x2F 

/* Page register in page 3*/
#define AWBL_FM175XX_PAGE3_REG         0x30 
/* setting for test signal.*/
#define AWBL_FM175XX_TESTSEL1_REG      0x31 
/* setting for test signal and PRBS control*/
#define AWBL_FM175XX_TESTSEL2_REG      0x32 
/* Current no used.*/
#define AWBL_FM175XX_RFU33_REG         0x33 
/* Current no used. */
#define AWBL_FM175XX_RFU34_REG         0x34 
/* Display internal testing bus state. */
#define AWBL_FM175XX_TESTBUS_REG       0x35 
/* Test control*/
#define AWBL_FM175XX_TESTCTRL_REG      0x36 
/* current not used.*/
#define AWBL_FM175XX_RFT_REG           0x37 
/* AUX pin control setting.*/
#define AWBL_FM175XX_ANALOGTEST_REG    0x38 
/* define TestDAC value.*/
#define AWBL_FM175XX_TESTDAC_REG       0x39 
/* current not used.*/
#define AWBL_FM175XX_RFU3A_REG         0x3A 
/* display ADC I and ADC Q channel actual value.*/
#define AWBL_FM175XX_TESTADC_REG       0x3B 
/* current not used.*/
#define AWBL_FM175XX_RFT3C_REG         0x3C 
/* current not used.*/
#define AWBL_FM175XX_RFT3D_REG         0x3D 
/* current not used.*/
#define AWBL_FM175XX_RFT3E_REG         0x3E 
/* current not used.*/
#define AWBL_FM175XX_RFT3F_REG         0x3F 

#include "awbl_fm175xx_lpcd_regs.h"

/* @} */

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#endif



