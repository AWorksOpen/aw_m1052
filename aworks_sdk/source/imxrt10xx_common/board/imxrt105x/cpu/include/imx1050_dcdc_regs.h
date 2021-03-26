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
 * \brief the definition of consts and struct of clock controler module in imx1050
 *
 * \par 1.兼容设备
 *
 *  - IMX1050
 *
 * \internal
 * \par modification history
 * - 1.00 2017-09-01  sni, first implementation
 * \endinternal
 */

#ifndef __IMX1050_DCDC_REGS_H__
#define __IMX1050_DCDC_REGS_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus*/

/**
 * \addtogroup grp_imx1050_bsp IMX1050 BSP支持
 */


/**
 * \addtogroup grp_imx1050_dcdc_regs DCDC模块寄存器
 * @{
 */

typedef struct {
    uint32_t REG0;              /**< DCDC Register 0, offset: 0x0 */
    uint32_t REG1;              /**< DCDC Register 1, offset: 0x4 */
    uint32_t REG2;              /**< DCDC Register 2, offset: 0x8 */
    uint32_t REG3;              /**< DCDC Register 3, offset: 0xC */
}imx1050_dcdc_regs_t;

/** @} end of grp_imx1050_dcdc_regs */

/** @} end of grp_imx1050_bsp */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX1050_DCDC_REGS_H__ */

/* end of file */
