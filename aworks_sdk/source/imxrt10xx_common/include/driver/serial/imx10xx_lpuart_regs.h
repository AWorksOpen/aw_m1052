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
 * \brief stm32f412 uart regs
 *
 * \internal
 * \par modification history:
 * - 1.00 2017-09-30  sni, first implementation
 * \endinternal
 */

#ifndef __IMX10xx_LPUART_REGS_H__
#define __IMX10xx_LPUART_REGS_H__

typedef struct {
    volatile uint32_t   VERID;
    volatile uint32_t   PARAM;
    volatile uint32_t   GLOBAL;
    volatile uint32_t   PINCFG;
    volatile uint32_t   BAUD;
    volatile uint32_t   STAT;
    volatile uint32_t   CTRL;
    volatile uint32_t   DATA;
    volatile uint32_t   MATCH;
    volatile uint32_t   MODIR;
    volatile uint32_t   FIFO;
    volatile uint32_t   WATER;
}rt105x_lpuart_regs_t;

#endif   /* __IMX10xx_LPUART_REGS_H__ */

/* enf of file */
