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
 * \par 1.ºÊ»›…Ë±∏
 *
 *  - IMX1020
 *
 * \internal
 * \par modification history
 * - 1.00 2020-04-28  cml, first implementation
 * \endinternal
 */

#ifndef __IMX1020_XTAL_OSC24M_H__
#define __IMX1020_XTAL_OSC24M_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus*/

void xtal_osc24M_init(void);

void xtal_osc24M_switch_osc_to_xtal(void);
void xtal_osc24M_switch_osc_to_rc(void);

void xtal_osc24M_deinit_rc_24m(void);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX1020_XTAL_OSC24M_H__ */

/* end of file */
