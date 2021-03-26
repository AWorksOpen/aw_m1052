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
 * \brief
 *
 * \internal
 * \par Modification History
 * - 1.00 19-1-25 mkr, first implementation.
 * \endinternal
 */
#ifndef __AWBL_FM175XX_LPCD_REGS_H
#define __AWBL_FM175XX_LPCD_REGS_H

/** \brief extension register handle */
#define AWBL_FM175XX_EXT_REG_WR_ADDR   (0x40) /**< \brief wrire address cycle */
#define AWBL_FM175XX_EXT_REG_RD_ADDR   (0x80) /**< \brief read address cycle */
#define AWBL_FM175XX_EXT_REG_WR_DATA   (0xC0) /**< \brief write data cycle */
#define AWBL_FM175XX_EXT_REG_RD_DATA   (0x00) /**< \brief read data cycle */

/**
 * \brief LPCD registers
 * @{
 */
/** \brief Lpcd Ctrl register1 */
#define AWBL_FM175XX_LPCDCTRL1_REG          (0x01) 
/** \brief Lpcd Ctrl register2 */
#define AWBL_FM175XX_LPCDCTRL2_REG          (0x02) 
/** \brief Lpcd Ctrl register3 */
#define AWBL_FM175XX_LPCDCTRL3_REG          (0x03) 
/** \brief Lpcd Ctrl register4 */
#define AWBL_FM175XX_LPCDCTRL4_REG          (0x04) 
/** \brief Lpcd bias current register */
#define AWBL_FM175XX_LPCDBIASCURRENT_REG    (0x05) 
/** \brief Lpcd adc reference register */
#define AWBL_FM175XX_LPCDADCREFERENCE_REG   (0x06) 
/** \brief T1Cfg[3:0] register */
#define AWBL_FM175XX_LPCDT1CFG_REG          (0x07) 
/** \brief T2Cfg[4:0] register */
#define AWBL_FM175XX_LPCDT2CFG_REG          (0x08) 
/** \brief T2Cfg[4:0] register */
#define AWBL_FM175XX_LPCDT3CFG_REG          (0x09) 
/** \brief VmidBdCfg[4:0] register */
#define AWBL_FM175XX_LPCDVMIDBDCFG_REG      (0x0A) 
/** \brief Auto_Wup_Cfg register */
#define AWBL_FM175XX_LPCDAUTOWUPCFG_REG     (0x0B) 
/** \brief ADCResult[5:0] Register */
#define AWBL_FM175XX_LPCDADCRESULTL_REG     (0x0C) 
/** \brief ADCResult[7:6] Register */
#define AWBL_FM175XX_LPCDADCRESULTH_REG     (0x0D) 
/** \brief LpcdThreshold_L[5:0] Register */
#define AWBL_FM175XX_LPCDTHRESHOLDMINL_REG  (0x0E) 
/** \brief LpcdThreshold_L[7:6] Register */
#define AWBL_FM175XX_LPCDTHRESHOLDMINH_REG  (0x0F) 
/** \brief LpcdThreshold_H[5:0] Register */
#define AWBL_FM175XX_LPCDTHRESHOLDMAXL_REG  (0x10) 
/** \brief LpcdThreshold_H[7:6] Register */
#define AWBL_FM175XX_LPCDTHRESHOLDMAXH_REG  (0x11) 
/** \brief LpcdStatus Register */
#define AWBL_FM175XX_LPCDIRQ_REG            (0x12) 
/** \brief Aux1 select Register */
#define AWBL_FM175XX_LPCDRFT1_REG           (0x13) 
/** \brief Aux2 select Register */
#define AWBL_FM175XX_LPCDRFT2_REG           (0x14) 
/** \brief LPCD test1 Register */
#define AWBL_FM175XX_LPCDRFT3_REG           (0x15) 
/** \brief LPCD test2 Register */
#define AWBL_FM175XX_LPCDRFT4_REG           (0x16) 
/** \brief lp_clk_cnt[5:0] Register */
#define AWBL_FM175XX_LPCLK_CNT1             (0x17) 
/** \brief lp_clk_cnt[7:6] Register */
#define AWBL_FM175XX_LPCLK_CNT2             (0x18) 
/** \brief VersionReg2[1:0] Register */
#define AWBL_FM175XX_VERSIONREG2_REG        (0x19) 
/** \brief Irq bak Register */
#define AWBL_FM175XX_IRQBAK_REG             (0x1A) 
/** \brief LPCD TEST3 Register */
#define AWBL_FM175XX_LPCDRFT5_REG           (0x1B)
/** \brief LPCD Misc Register */
#define AWBL_FM175XX_LPCDMISC_REG           (0x1C) 
/** \brief extension register of lvdctrl */
#define AWBL_FM175XX_LVDCTRL_REG            (0x1D) 


/** \brief AWBL_FM175XX_LPCDCTRL1_REG register bit define */
/** \brief enble LPCD */
#define AWBL_FM175XX_LPCDCTRL1_LPCD_EN         (0x01) 
/** \brief lpcd reset */
#define AWBL_FM175XX_LPCDCTRL1_LPCD_RSTN       (0X02) 
/** \brief into lpcd calibra mode */
#define AWBL_FM175XX_LPCDCTRL1_LPCD_CALIBRA_EN (0x04) 
/** \brief Compare times 1 or 3 */
#define AWBL_FM175XX_LPCDCTRL1_LPCD_CMP_1      (0x08) 
/** \brief Compare times 1 or 3 */
#define AWBL_FM175XX_LPCDCTRL1_LPCD_CMP_3      (0x13) 
/** \brief Enable LPCD IE */
#define AWBL_FM175XX_LPCDCTRL1_LPCD_IE         (0x10) 
/** \brief Lpcd register Bit ctrl set bit */
#define AWBL_FM175XX_LPCDCTRL1_BIT_CTRL_SET    (0x20) 
/** \brief Lpcd register Bit ctrl clear bit */
#define AWBL_FM175XX_LPCDCTRL1_BIT_CTRL_CLR    (0x00) 

/** \brief AWBL_FM175XX_LPCDIRQ_REG register bit define */
/** \brief irq of card in       */
#define AWBL_FM175XX_LPCDIRQ_CARD_IN_IRQ       (0x01) 
/** \brief irq of LPCD 23 end   */
#define AWBL_FM175XX_LPCDIRQ_LPCD23_IRQ        (0x02) 
/** \brief irq of calib end     */
#define AWBL_FM175XX_LPCDIRQ_CALIB_IRQ         (0x04) 
/** \brief irq of lp osc 10K ok */
#define AWBL_FM175XX_LPCDIRQ_LP10K_TESTOK_IRQ  (0x08) 
/** \brief irq of Auto wake up  */
#define AWBL_FM175XX_LPCDIRQ_AUTO_WUP_IRQ      (0x10) 

/** \brief AWBL_FM175XX_LPCDRFT1_REG register bit define */
/** \brief close aux1 out */
#define AWBL_FM175XX_LPCDRFT1_AUX1_CLOSE       (0x00) 
/** \brief vdem of lpcd */
#define AWBL_FM175XX_LPCDRFT1_AUX1_VDEM_LPCD   (0x01) 
/** \brief voltage of charecap */
#define AWBL_FM175XX_LPCDRFT1_AUX1_VP_LPCD     (0x02) 

/** \brief AWBL_FM175XX_LPCDRFT2_REG register bit define */
/** \brief close aux1 out */
#define AWBL_FM175XX_LPCDRFT2_AUX2_CLOSE       (0x00) 
/** \brief vdem of lpcd */
#define AWBL_FM175XX_LPCDRFT2_AUX2_VDEM_LPCD   (0x01) 
/** \brief voltage of charecap */
#define AWBL_FM175XX_LPCDRFT2_AUX2_VP_LPCD     (0x02) 

/** \brief AWBL_FM175XX_LPCDRFT3_REG register bit define */
/** \brief enable lp osc10k */
#define AWBL_FM175XX_LPCDRFT3_LP_OSC10K_EN        (0x01) 
/** \brief enable lp osc10k calibra mode */
#define AWBL_FM175XX_LPCDRFT3_LP_OSC_CALIBRA_EN   (0x02) 
/** \brief enable lp t1 current test */
#define AWBL_FM175XX_LPCDRFT3_LP_CURR_TEST        (0x04) 
/** \brief lpcd_test2[3]:LPCD_OUT */
#define AWBL_FM175XX_LPCDRFT3_LPCD_TEST2_LPCD_OUT (0x08) 

/** \brief AWBL_FM175XX_LPCDRFT4_REG register bit define */
/** \brief D5:T1_OUT */
#define AWBL_FM175XX_LPCDRFT4_T1_OUT_EN          (0x01) 
/** \brief D4:OSC_CLK_OUT */
#define AWBL_FM175XX_LPCDRFT4_OSCCLK_OUT_EN      (0x02) 
/** \brief D3:OSC_EN */
#define AWBL_FM175XX_LPCDRFT4_OSCEN_OUT_EN       (0x04) 
/** \brief D2:LP_CLK or LPCD_OUT */
#define AWBL_FM175XX_LPCDRFT4_LP_CLK_LPCD_OUT_EN (0x08) 
/** \brief D1:T3_OUT */
#define AWBL_FM175XX_LPCDRFT4_T3_OUT_EN          (0x10) 

/** \brief AWBL_FM175XX_IRQBAK_REG register bit define */
/** \brief Irq Inv backup */
#define AWBL_FM175XX_IRQBAK_IRQ_INV_BAK         (0x01) 
/** \brief Irq pushpull backup */
#define AWBL_FM175XX_IRQBAK_IRQ_PUSHPULL_BAK    (0x02) 

/** \brief AWBL_FM175XX_LPCDRFT5_REG register bit define */
/** \brief lPCD test mode */
#define AWBL_FM175XX_LPCDRFT5_LPCD_TESTEN      (0x01) 
/** \brief Auto wakeup test mode */
#define AWBL_FM175XX_LPCDRFT5_AWUP_TSEL        (0x02) 
/** \brief RNG  mode sel */
#define AWBL_FM175XX_LPCDRFT5_RNG_MODE_SEL     (0x04) 
/** \brief use retention mode */
#define AWBL_FM175XX_LPCDRFT5_USE_RET          (0x08) 

/** \brief AWBL_FM175XX_LPCDMISC_REG register bit define */
/** \brief lPCD test mode */
#define AWBL_FM175XX_LPCDMISC_CALIB_VMID_EN   (0x01)  
/** \brief LPCD amp en select */
#define AWBL_FM175XX_LPCDMISC_AMP_EN_SEL      (0x04)  

#endif

