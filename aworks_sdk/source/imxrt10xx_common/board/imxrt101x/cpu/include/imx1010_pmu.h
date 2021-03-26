/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief the definition of consts and struct of clock controler module in imx1010
 *
 * \par 1.ºÊ»›…Ë±∏
 *
 *  - IMX1010
 *
 * \internal
 * \par modification history
 * - 1.00 2020-04-29  cml, first implementation
 * \endinternal
 */

#ifndef __IMX1010_PMU_H__
#define __IMX1010_PMU_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

void pmu_init (void);

void pmu_bandgap_on (void);
void pmu_enable_regular_ldo (void);
void pmu_disable_regular_ldo (void);
void pmu_disable_weak_ldo (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __IMX1010_PMU_REGS_H__ */

/* end of file */
