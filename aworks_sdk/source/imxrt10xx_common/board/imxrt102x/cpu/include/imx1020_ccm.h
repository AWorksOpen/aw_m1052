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
 * \brief the definition of consts and struct of clock controler module in imx1020
 *
 * \par 1.�����豸
 *
 *  - IMX1020
 *
 * \internal
 * \par modification history
 * - 1.00 2020-04-29  cml, first implementation
 * \endinternal
 */

#ifndef __IMX1020_CCM_H
#define __IMX1020_CCM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus*/

/**
 * \addtogroup grp_imx1020_bsp IMX1020 BSP֧��
 */


/**
 * \addtogroup grp_imx1020_ccm ʱ�ӿ���ģ��Ĵ�������
 * @{
 */

// ������һ��ִ��WFI��ʱ�� ����MCU��Ȼ����RUNģʽ
void ccm_clpcr_set_run_mode(void);

// ���õ�arm ִ��WFI���Խ���͹���ģʽ��WAIT��STOP��ʱ��
// ������ж�pending���򱣳����ڲ�RAM��ʱ��
void ccm_cgpr_enable_int_memclk_lpm(void);

/* Enable XTAL 24MHz clock source. */
/*!
 * brief Initialize the external 24MHz clock.
 *
 * This function supports two modes:
 * 1. Use external crystal oscillator.
 * 2. Bypass the external crystal oscillator, using input source clock directly.
 *
 * After this function, please call ref CLOCK_SetXtal0Freq to inform clock driver
 * the external clock frequency.
 *
 * param bypassXtalOsc Pass in true to bypass the external crystal oscillator.
 * note This device does not support bypass external crystal oscillator, so
 * the input parameter should always be false.
 */
void ccm_init_external_clk(void);

void ccm_deinit_external_clk(void);


void ccm_clock_handshake_wait(void);
/** @} end of grp_imx1020_ccm */

/** @} end of grp_imx1020_bsp */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __LPC546XX_REG_BASE_H */

/* end of file */
