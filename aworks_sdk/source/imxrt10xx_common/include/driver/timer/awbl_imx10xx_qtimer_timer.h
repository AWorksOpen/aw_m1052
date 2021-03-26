/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief iMX RT1050 QTimer��ʱ������
 *
 * \internal
 * \par modification history:
 * - 1.00 19-06-28  ral, first implemetation
 * \endinternal
 */
#ifndef __AWBL_IMX10XX_QTIMER_TIMER_H
#define __AWBL_IMX10XX_QTIMER_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_clk.h"
#include "awbl_timer.h"

/* ������ */
#define AWBL_IMX10XX_QTIMER_TIMER_NAME   "imx10xx_qtmr"

/**
 * \brief QTimer Timer ͨ��ѡ���б�
 */
typedef enum qtimer_timer_channel_sel {
    kQTMR_Timer_Channel_0 = 0,    /**< \brief QTimer Timer Channel 0 */
    kQTMR_Timer_Channel_1,        /**< \brief QTimer Timer Channel 1 */
    kQTMR_Timer_Channel_2,        /**< \brief QTimer Timer Channel 2 */
    kQTMR_Timer_Channel_3,        /**< \brief QTimer Timer Channel 3 */
} qtimer_timer_channel_sel_t;

/**
 * \brief ʱ��Ԥ��Ƶѡ���б�
 */
typedef enum qtimer_timer_primary_count_source {
    QTimer_ClockDivide_1   = 1,             /**< \brief IP bus clock divide by 1   prescaler */
    QTimer_ClockDivide_2   = 2,             /**< \brief IP bus clock divide by 2   prescaler */
    QTimer_ClockDivide_4   = 4,             /**< \brief IP bus clock divide by 4   prescaler */
    QTimer_ClockDivide_8   = 8,             /**< \brief IP bus clock divide by 8   prescaler */
    QTimer_ClockDivide_16  = 16,            /**< \brief IP bus clock divide by 16  prescaler */
    QTimer_ClockDivide_32  = 32,            /**< \brief IP bus clock divide by 32  prescaler */
    QTimer_ClockDivide_64  = 64,            /**< \brief IP bus clock divide by 64  prescaler */
    QTimer_ClockDivide_128 = 128            /**< \brief IP bus clock divide by 128 prescaler */
} qtimer_timer_primary_count_source_t;

enum qtimer_timer_count_mode {
    QTimer_Count_Mode_Off = AW_FALSE,   /**< \brief QTimer count mode off */
    QTimer_Count_Mode_On  = AW_TRUE,    /**< \brief QTimer count mode on */
};

/**
 * \brief iMX RT10xx QTimer Timer �豸��Ϣ
 */
typedef struct awbl_imx10xx_qtimer_timer_devinfo {
    uint32_t                                alloc_byname;       /**< \brief �� 1 ��λ��ʾ�ö�ʱ������ͨ�����ƽ��з��� */
    uint32_t                                inum;               /**< \brief �ж������� */
    uint32_t                                regbase;            /**< \brief QTimer�Ĵ�������ַ */
    aw_clk_id_t                             clk_id;             /**< \brief QTimerʱ��ID */
    qtimer_timer_primary_count_source_t     prescaler_div;      /**< \brief ʱ��Ԥ��Ƶ */
    qtimer_timer_channel_sel_t              channel;            /**< \brief ͨ���� */
    void (*pfn_plfm_init) (void);                               /**< \breif platform initializing */
    aw_bool_t                               count_mode;         /**< \brief �������ģʽ���� */
} awbl_imx10xx_qtimer_timer_devinfo_t;

/**
 * \brief iMX RT10xx QTimer Timer device
 */
typedef struct awbl_imx10xx_qtimer_timer_dev {

    struct awbl_dev         dev;                    /**< \brief �̳��� AWBus �豸 */
    struct awbl_timer       tmr;                    /**< \brief ��׼��ʱ���ṹ */

    uint32_t                count;                  /**< \brief ��ǰ������ֵ */
    uint32_t                rollover_period;        /**< \brief ��ת���� */

    struct awbl_timer_param param;                  /**< \brief ��׼��ʱ������ */

    void                  (*pfn_isr)(void *p_arg);  /**< \brief ��ʱ���жϻص����� */
    void                   *p_arg;                  /**< \brief ��ʱ���жϻص��������� */
    volatile int            enabled;                /**< \brief ��ʱ���Ƿ�ʹ�� */

} awbl_imx10xx_qtimer_timer_dev_t;

/**
 * \brief iMX RT10xx QTimer Timer driver register
 */
void awbl_imx10xx_qtimer_timer_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10XX_QTIMER_TIMER_H */
