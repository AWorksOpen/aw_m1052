/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx GPT Regs
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-09  pea, first implementation
 * \endinternal
 */

#ifndef __IMX10xx_GPT_REGS_H
#define __IMX10xx_GPT_REGS_H

#define __IO  volatile
#define __O   volatile
#define __I   volatile const

/**
 * \brief iMX RT10xx GPT �Ĵ����鶨��
 */
typedef struct {

    __IO uint32_t cr;      /**< GPT Control Register, offset: 0x0 */
    __IO uint32_t pr;      /**< GPT Prescaler Register, offset: 0x4 */
    __IO uint32_t sr;      /**< GPT Status Register, offset: 0x8 */
    __IO uint32_t ir;      /**< GPT Interrupt Register, offset: 0xC */
    __IO uint32_t ocr[3];  /**< GPT Output Compare Register 1..GPT Output Compare Register 3, array offset: 0x10, array step: 0x4 */
    __I  uint32_t icr[2];  /**< GPT Input Capture Register 1..GPT Input Capture Register 2, array offset: 0x1C, array step: 0x4 */
    __I  uint32_t cnt;     /**< GPT Counter Register, offset: 0x24 */

} imx10xx_gpt_regs_t;

/** \brief GPT ģʽ���� */
#define IMX10xx_GPT_MODE_TIMER   0           /**< \brief ��ʱ��ģʽ */
#define IMX10xx_GPT_MODE_INPUT   1           /**< \brief ���벶��ģʽ */
#define IMX10xx_GPT_MODE_OUTPUT  2           /**< \brief ����Ƚ�ģʽ */

/** \brief GPT ���Զ��� */
#define IMX10xx_GPT_INPUT_DIS        0       /**< \brief ����ʧЧ */
#define IMX10xx_GPT_INPUT_RISE       1       /**< \brief ���������ش��� */
#define IMX10xx_GPT_INPUT_FALL       2       /**< \brief �����½��ش��� */
#define IMX10xx_GPT_INPUT_BOTH       3       /**< \brief ����˫���ش��� */

#define IMX10xx_GPT_OUTPUT_DIS       0       /**< \brief ���ʧЧ */
#define IMX10xx_GPT_OUTPUT_TOG       1       /**< \brief �����ת */
#define IMX10xx_GPT_OUTPUT_CLR       2       /**< \brief ������� */
#define IMX10xx_GPT_OUTPUT_SET       3       /**< \brief �����λ */
#define IMX10xx_GPT_OUTPUT_LOW_PULSE 4       /**< \brief ��������� */

#endif /* __IMX10xx_GPT_REGS_H */

/* end of file */
