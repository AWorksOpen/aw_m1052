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
 * \brief lpc11xx adc driver private header file
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-06 zyr, first implementation
 * \endinternal
 */

#ifndef __AWBL_LPC11XX_ADC_PRIVATE_H
#define __AWBL_LPC11XX_ADC_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  register offset
*******************************************************************************/
#define __CR          0x000
#define __GDR         0x004
#define __RESERVED0   0x008
#define __INTEN       0x00c
#define __DR0         0x010
#define __STAT        0x030


/*******************************************************************************
  macros
*******************************************************************************/
/* __CR¼Ä´æÆ÷Î»²Ù×÷ */
#define __CH_BIT_START            0
#define __CH_BIT_LEN              8
#define __CH_EN(base,n)           AW_REG_BIT_SET32((base) + __CR, (n))
#define __CH_DIS(base,n)          AW_REG_BIT_CLR32((base) + __CR, (n))
#define __CH_SETS(base,chs)       AW_REG_BITS_SET32((base) + __CR, \
                                                    __CH_BIT_START, \
                                                    __CH_BIT_LEN, \
                                                    (chs))

#define __DIV_BIT_START           8
#define __DIV_BIT_LEN             8
#define __CLK_DIV_SET(base, div)  AW_REG_BITS_SET32((base) + __CR, \
                                                    __DIV_BIT_START, \
                                                    __DIV_BIT_LEN, \
                                                    (div))
                                                    
#define __CLK_DIV_GET(base)       AW_REG_BITS_GET32((base) + __CR, \
                                                    __DIV_BIT_START, \
                                                    __DIV_BIT_LEN)

#define __BURST_BIT               16
#define __BURST_MODE(base)        AW_REG_BIT_SET32((base) + __CR, __BURST_BIT)
#define __NORMAL_MODE(base)       AW_REG_BIT_CLR32((base) + __CR, __BURST_BIT)
#define __MODE_GET(base)          AW_REG_BIT_GET32((base) + __CR, __BURST_BIT)

#define __CLKS_BIT_START          17
#define __CLKS_BIT_LEN            3
#define __CLKS_SET(base, clks)    AW_REG_BITS_SET32((base) + __CR, \
                                                    __CLKS_BIT_START, \
                                                    __CLKS_BIT_LEN, \
                                                    (clks))
                                                    
#define __CLKS_GET(base)          AW_REG_BITS_GET32((base) + __CR, \
                                                    __CLKS_BIT_START, \
                                                    __CLKS_BIT_LEN)

#define __PDN_BIT                 21
#define __PDN_SET(base)           AW_REG_BIT_SET32((base) + __CR, __PDN_BIT)
#define __PDN_CLR(base)           AW_REG_BIT_CLR32((base) + __CR, __PDN_BIT)

#define __START_BIT_START         24
#define __START_BIT_LEN           3
#define __START_MODE(base, mode)  AW_REG_BITS_SET32((base) + __CR, \
                                                    __START_BIT_START, \
                                                    __START_BIT_LEN, \
                                                    (mode))

#define __EDGE_BIT                27
#define __EDGE_RISE_SET(base)     AW_REG_BIT_CLR32((base) + __CR, __EDGE_BIT)
#define __EDGE_FALL_SET(base)     AW_REG_BIT_SET32((base) + __CR, __EDGE_BIT)

/*  ¼Ä´æÆ÷¶ÁÐ´  */
#define __CR_READ(base)           AW_REG_READ32((base) + __CR)
#define __CR_WRITE(base, val)     AW_REG_WRITE32((base) + __CR, (val))

#define __INTEN_WRITE(base, val)  AW_REG_WRITE32((base) + __INTEN, (val))
#define __INTEN_READ(base)        AW_REG_READ32((base) + __INTEN)

#define __GDR_READ(base)          AW_REG_READ32((base) + __GDR)

#define __DR_READ(base,n)         AW_REG_READ32((base) + __DR0 + (n)*4)

#define __STAT_READ(base)         AW_REG_READ32((base) + __STAT)

#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_LPC11XX_ADC_PRIVATE_H */

/* end of file */
