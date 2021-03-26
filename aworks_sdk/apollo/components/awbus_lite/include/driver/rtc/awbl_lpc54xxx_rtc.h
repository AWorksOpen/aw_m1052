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
 * \brief lpc54xxx Ƭ��RTC ʵʱʱ�ӵ����� 
  *
 * ������ʵ���� lpc54xxx Ƭ��RTC ʵʱʱ�ӵ�����    
 *
 * \par 1.������Ϣ
 *
 *  - ������:  "lpc54xxx_rtc"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_lpc54xxx_rtc_param
 *
 * \par 2.�����豸
 *
 *  - NXP LPC54XXX ϵ��MCU
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
 * - 1.00 14-08-20  tee, first implementation
 * \endinternal
 */
 
#ifndef AWBL_LPC54XXX_RTC_H_
#define AWBL_LPC54XXX_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_rtc.h"
#include "aw_sem.h"

/** \brief lpc54xxx rtc ������ */
#define AWBL_LPC54XXX_RTC_NAME   "lpc54xxx-RTC"


/** \brief ���ƼĴ���λ���ܶ��� */
#define   RTC_CTRL_SWRESET  	 (1u << 0)
#define   RTC_CTRL_OFD           (1u << 1)
#define   RTC_CTRL_ALARM1HZ      (1u << 2)
#define   RTC_CTRL_WAKE1KHZ      (1u << 3)
#define   RTC_CTRL_ALARMDPD_EN   (1u << 4)
#define   RTC_CTRL_WAKEDPD_EN    (1u << 5)
#define   RTC_CTRL_RTC1KHZ_EN    (1u << 6)
#define   RTC_CTRL_RTC_EN        (1u << 7)
 
/** lpc54xxx �Ĵ����б� */
struct awbl_lpc54xxx_rtc_reg {
    
    uint32_t ctrl;                 /** \brief RTC control register */
    uint32_t match;                /** \brief RTC match register   */
    uint32_t count;                /** \brief RTC counter register */
    uint32_t wake;                 /** \brief RTC control register */
};

/**
 * \brief lpc54xxxRTC �豸��Ϣ
 * �����ϲ�������ײ�Ӳ����ƥ�䣬�����ϲ�ֻ��Ҫ�����豸�žͿ��Բ�����Ӧ���豸��
 */
 
typedef struct awbl_lpc54xxx_rtc_devinfo {

    /** \brief RTC ����������Ϣ */
    struct awbl_rtc_servinfo  rtc_servinfo;

    /** \brief RTC �Ĵ����Ļ���ַ */
    uint32_t  Base_Addr;
    
    /** \brief ƽ̨��ʼ������ */
    void (*pfunc_plfm_init)(void);   

} awbl_lpc54xxx_rtc_devinfo_t;

/**
 * \brief lpc54xxxRTC �豸ʵ��
 */
typedef struct awbl_lpc54xxx_rtc_dev {

    /** \brief �����豸 */
    struct awbl_dev super;   /**< \brief ����Ҫ�ĳ�Ա */

    /** \brief �ṩ RTC ���� */
    struct awbl_rtc_service rtc_serv;
    
    struct awbl_lpc54xxx_rtc_reg *reg;

    /** \brief �豸�� */
    AW_MUTEX_DECL(devlock);
} awbl_lpc54xxx_rtc_dev_t;




/**
 * \brief lpc54xxx RTC driver register
 */
void awbl_lpc54xxx_rtc_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* AWBL_lpc54xxx_RTC_H_ */

/* end of file */

