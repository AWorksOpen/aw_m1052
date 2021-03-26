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
 * \brief LPC11XX��LPC11Cxxϵ�ж�ʱ������
 *
 * ������ֻʵ���� LPC11XX 16-bit/32-bit ��ʱ���Ķ�ʱ�����ܣ����ṩ������ƥ�䡢
 * �����PWM���ܡ�
 *
 * \par 1.������Ϣ
 *
 *  - ������:  "lpc11xx_timer32"(32-bit timer) ; "lpc11xx_timer16"(16-bit timer)
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_lpc11xx_timer_param
 *
 * \par 2.�����豸
 *
 *  - NXP LPC11XX ϵ��MCU
 *  - NXP LPC13XX ϵ��MCU
 *  - ������LPC11XX TIMER ���ݵ��豸
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_drv_lpc11xx_16_32timer_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_hwtimer
 *
 * \internal
 * \par modification history
 * - 1.00 12-11-15  orz, first implementation
 * \endinternal
 */

#ifndef __AWBL_LPC11XX_TIMER_H
#define __AWBL_LPC11XX_TIMER_H

#include "awbl_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_lpc11xx_16_32timer
 * \copydetails awbl_lpc11xx_timer.h
 * @{
 */

/**
 * \defgroup  grp_awbl_drv_lpc11xx_16_32timer_hwconf �豸����/����
 *
 * LPC11XX TIMER Ӳ���豸�Ķ���������Ĵ�����ʾ���û��ڶ���Ŀ��ϵͳ��Ӳ����Դʱ,
 * ����ֱ�ӽ���δ���Ƕ�뵽 awbus_lite_hwconf_usrcfg.c �ж�Ӧ��λ�ã�Ȼ����豸
 * ��Ϣ���б�Ҫ������(�����õ���Ŀ�á�x����ʶ)��
 *
 * \note �ж��ٸ�TIMER���ͽ�����Ĵ���Ƕ����ٴΣ�ע�⽫�����޸�Ϊ��ͬ�����֡�
 *
 * \include  hwconf_usrcfg_hwtimer_lpc11xx.c
 * \note �����������д����û����á���ʶ�ģ���ʾ�û��ڿ���Ӧ��ʱ�������ô˲�����
 *
 * - ��1��ƽ̨��س�ʼ������ \n
 * ��ƽ̨��ʼ�����������ʹ��ʱ�ӵ���ƽ̨��صĲ�����
 *
 * - ��2����ʱ���ɴﵽ����СƵ�� \n
 * ������ֲ��ж�ʱ���Ŀ��÷�Ƶ������ȷ���ò���
 *
 * - ��3����ʱ���ɴﵽ�����Ƶ�� \n
 * ������ֲ��ж�ʱ���Ŀ��÷�Ƶ������ȷ���ò���
 *
 * - ��4����ʱ��������ʱ��Ƶ�� \n
 * ������ֲἰϵͳʱ��������ȷ���ò���
 *
 * - ��5����ʱ���������ת���� \n
 *
 * - ��6����ʱ���Ĵ�����ʼ��ַ \n
 * ������ֲ�ȷ���ò���
 *
 * - ��8���豸���� \n
 * 16λ��ʱ�����lpc11xx_timer16����32λ��ʱ������д��lpc11xx_timer32��
 *
 * - ��9����ʱ���豸�ĵ�Ԫ�� \n
 * ��������ͬ���豸����16λ��ʱ��0��16λ��ʱ��1
 */

/** @} */

/**
 * \name register defines for lpc11xx and lpc11cxx
 * @{
 */
/* IR, n = [0, 3] */
#define IR_MR(n)   (1u << (n))  /**< \brief interrupt flag for match chan n */
#define IR_CR0      0x10u       /**< \brief interrupt flag for capture chan 0 */

/* TCR */
#define TCR_CEN     0x01u       /**< \brief Count Enable */
#define TCR_CRST    0x02u       /**< \brief Counter Reset */

/* MCR, n = [0, 3] */
#define MCR_MRI(n)  (1u << ((n) * 3u))  /**< \brief Interrupt on MRn */
#define MCR_MRR(n)  (2u << ((n) * 3u))  /**< \brief Reset on MRn */
#define MCR_MRS(n)  (4u << ((n) * 3u))  /**< \brief Stop on MRn */

/* CCR, n = [0] */
#define CCR_CAPRE(n) (1u << ((n) * 3u)) /**< \brief Capture on rising edge */
#define CCR_CAPFE(n) (2u << ((n) * 3u)) /**< \brief Capture on falling edge */
#define CCR_CAPI(n)  (4u << ((n) * 3u)) /**< \brief Interrupt on CAP event */

/* EMR, n = [0, 4] */
#define EMR_EM(n)       (1u << (n))     /**< \brief External Match n */
#define EMR_EMC(n, v)   ((v) << ( 4u + ((n) << 1u)))
                                        /**< \brief External Match Control n */
#define EMR_EMC_NONE    0x0u    /**< \brief Do Nothing */
#define EMR_EMC_CLR     0x1u    /**< \brief Clear External Match bit/output */
#define EMR_EMC_SET     0x2u    /**< \brief Set External Match bit/output */
#define EMR_EMC_TOGGLE  0x3u    /**< \brief Toggle External Match bit/output */

/* CTCR */
/** \brief Timer Mode: every rising PCLK edge*/
#define CTCR_CTM_TIMER      0x0u
/** \brief Counter Mode: TC incremented on rising edges on the CAP input */
#define CTCR_CTM_COUNTER_RE 0x1u
/** \brief Counter Mode: TC incremented on falling edges on the CAP input */
#define CTCR_CTM_COUNTER_FE 0x2u
/** \brief Counter Mode: TC incremented on both edges on the CAP input */
#define CTCR_CTM_COUNTER_BE 0x3u
/** \brief Count Input Select. CAP0 pin is sampled for clocking */
#define CTCR_CIS_CAP0       0x0u

/** \brief PWMC, n = [0, 3] */
#define PWMC_PWMEN(n)       (1u << (n)) /**< \brief PWM is enabled for MATn */
/** @} */

/******************************************************************************/

/** \brief lpc11xx counter/timer registers */
struct lpc11xx_timer_reg {
    uint32_t ir;        /**< \brief [00] Interrupt Register */
    uint32_t tcr;       /**< \brief [04] Timer Control Register */
    uint32_t tc;        /**< \brief [08] Timer Counter Register */
    uint32_t pr;        /**< \brief [0C] Prescale Register */
    uint32_t pc;        /**< \brief [10] Prescale Counter Register */
    uint32_t mcr;       /**< \brief [14] Match Control Register */
    uint32_t mr[4];     /**< \brief [18] Match Register n, n = [0, 3] */
    uint32_t ccr;       /**< \brief [28] Capture Control Register */
    uint32_t cr0;       /**< \brief [2C] Capture Register 0 */
    uint32_t emr;       /**< \brief [30] External Match Register */
    uint32_t rsv[12];   /**< \brief [40 - 6C] reserved */
    uint32_t ctcr;      /**< \brief [70] Count Control Register */
    uint32_t pwmc;      /**< \brief [74] PWM Control Register */
};

/******************************************************************************/

/** \brief timer fixed parameter */
struct awbl_lpc11xx_timer_param {
    uint32_t    clk_frequency;          /**< \brief clock frequency         */
    
    /** 
     * \brief the bit with 1 means the chan the timer can any be alloc by name 
     */
    uint32_t     alloc_byname;          
    uint32_t         reg_addr;          /**< \brief register base address   */
    int                  inum;          /**< \brief interrupt number        */
    void (*pfunc_plfm_init) (void);     /**< \brief platform initializing   */
};

/** \brief convert AWBus timer device info to lpc11xx timer parameter */
#define awbldev_get_lpctimer_param(p_dev) \
    ((struct awbl_lpc11xx_timer_param *)AWBL_DEVINFO_GET(p_dev))

/******************************************************************************/

/** \brief lpc11xx Timer type for AWBus */
struct awbl_lpc11xx_timer {
    
    /** \brief always go first */   
    awbl_dev_t  dev;                    /**< \brief AWBus device data */
    
    struct awbl_timer           timer;  /**< \brief awbl timer  data */
    
    struct lpc11xx_timer_reg   *reg;    /**< \brief register base    */
    
    uint32_t                    count;  /**< \brief current max counts */
    uint32_t          rollover_period;  /**< \brief rollover_period */

    struct awbl_timer_param  param;     /**< \brief the timer's param */
    
    void (*pfunc_isr) (void *p_arg);    /**< \brief ISR */
    void  *p_arg;                       /**< \brief ISR argument */
};

 
/** \brief convert a awbl_dev pointer to awbl_lpc11xx_timer pointer */
#define awbldev_to_lpc_timer(p_dev) \
        ((struct awbl_lpc11xx_timer *)(p_dev))

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_LPC11XX_TIMER_H */

/* end of file */
