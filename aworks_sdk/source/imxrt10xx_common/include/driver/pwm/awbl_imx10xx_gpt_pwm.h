/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx GPT ����
 *
 * ������ʵ���˶�ʱ����ƥ��������ܡ�
 *
 * \internal
 * \par modification history
 * - 1.00 20-09-11  licl, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_GPT_PWM_H
#define __AWBL_IMX10xx_GPT_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_pwm.h"
#include "aw_sem.h"
#include "aw_clk.h"
#include "driver/timer/imx10xx_gpt_regs.h"

#define AWBL_IMX10XX_GPT_PWM_NAME   "imx10xx_gpt_pwm"

/**
 * \brief QTimer PWM ͨ��ѡ���б�
 */
typedef enum __gpt_channel_sel
{
    GPT_Channel_0 = 0,    /**< \brief GPT Channel 0 */
    GPT_Channel_1,        /**< \brief GPT Channel 1 */
    GPT_Channel_2,        /**< \brief GPT Channel 2 */
    GPT_Channel_all,
} gpt_channel_sel_t;

/**
 * \brief iMX RT10xx GPT PWM �豸����
 */
typedef struct awbl_imx10xx_gpt_pwm_devcfg {
    uint8_t            pid;             /**< \brief PWM ID */
    gpt_channel_sel_t  ch;              /**< \brief PWM��Ӧ��ͨ�� */
    int                gpio;            /**< \brief PWM��Ӧ������ */
    uint32_t           func;            /**< \brief PWM���Ź��� */
    uint32_t           dfunc;           /**< \brief ����PWM���Ĭ�����Ź��� */
} awbl_imx10xx_gpt_pwm_devcfg_t;

/**
 * \brief iMX RT10xx GPT PWM ͨ������
 */
typedef struct {
    uint32_t period_num;
    AW_SEMB_DECL(sync_sem);
} awbl_imx10xx_gpt_pwm_channel_t;

/**
 * \brief iMX RT10xx  GPT PWM �豸��Ϣ
 */
typedef struct awbl_imx10xx_gpt_pwm_devinfo {
    uint32_t                 regbase;           /**< \brief PWM�Ĵ�������ַ */
    uint32_t                 inum;              /**< \brief �жϺ� */
    aw_clk_id_t              ref_clk_id;        /**< \brief PWMʱ��ID */
    struct awbl_pwm_servinfo pwm_servinfo;      /**< \brief PWM���������Ϣ */

    /** \brief PWM������Ϣ */
    aw_const struct awbl_imx10xx_gpt_pwm_devcfg *pwm_devcfg;
    uint8_t                  pnum;              /**< \brief ʹ�õ���PWMͨ����*/
    void (*pfunc_plfm_init)(void);              /** \brief ƽ̨��س�ʼ��������ʱ�ӡ���ʼ�����ŵȲ��� */
} awbl_imx10xx_gpt_pwm_devinfo_t;

/**
 * \brief �豸ʵ��
 */
typedef struct awbl_imx10xx_gpt_pwm_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev super;

    /** \brief PWM ����*/
    struct awbl_pwm_service pwm_serv;

    /** \brief PWMͨ�� */
    awbl_imx10xx_gpt_pwm_channel_t channel[3];

} awbl_imx10xx_gpt_pwm_dev_t;

/**
 * \brief �� IMX10xx GPT PWM ����ע�ᵽ AWBus ��ϵͳ��
 *
 * \note ������Ӧ�� aw_prj_early_init() �г�ʼ�����������н�ֹ���ò���ϵͳ��غ���
 */
void awbl_imx10xx_gpt_pwm_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_IMX10xx_GPT_PWM_H */

/* end of file */
