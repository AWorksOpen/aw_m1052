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
 * \brief AWBus IMX10xx on chip RTwatchdog interface header file
 *
 * use this module should include follows :
 * \code
 * #include "awbl_wdt.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-22  hsg, first implementation
 * \endinternal
 */

/**
 * \note RT���Ź�����ͨ���Ź�������֮���������д��ڹ��ܣ��û�������
 *       ����ֵ�����ޣ�Toval�Ĵ���ֵ���봰��ֵ�����ޣ�WIN�Ĵ�����ֵ��֮��ι����
 *
 * \details
 * RT���Ź��ڿ����жϺ����ǿ�Ƹ�λ֮ǰ�ӳ�256������ʱ�ӣ�Լ1.9us����
 * �������жϷ������̣�ISR��ִ�������������ջ�Ե��Դ��룩��
 *
 * RTWDOG��ʱ��Դѡ�����LPO_CLKʱ��Դ��32.768Khz����
 * RTWDOG�ڵ�һ�׶γ�ʼ���е�Ĭ�������ǣ��رմ���ģʽ������Ƶ���ر��жϣ��û�����Ҫʹ�ô���ģʽ�������������е���
 * RTWDOG�ṩ���ⲿ�Ľӿڸ��Ŀ��Ź����á���Ҫ����ͷ�ļ���driver/rtwdt/awbl_imx10xx_rtwdt.h��������
 * RT���Ź��ĳ�ʱ��ʽ��Time_out = TOVAL / Frtwdog / Pres
 * ���У�
 *     Time_out��RT���Ź���ʱʱ�䣨��λΪms��
 *     TOVAL��д��RT���Ź�toval�Ĵ�����ֵ
 *     Frtwdog:RT���Ź�����ʱ��Ƶ�ʣ���λΪKhz��
 *     Pres��RT���Ź���Ƶֵ������ѡ�񲻷�Ƶ��256��Ƶ��������Ƶʱ��ֵΪ1����Ƶʱ��ֵΪ256��
 *
 * ʱ��Դѡ�����LPO_CLKʱ��Դʱ���������Ϲ�ʽ�����Եõ���С-���ʱʱ�䣨�������룩�����±���ʾ��
 *    +---------+----------------+----------------+
      | PRES    | ��С��ʱ                             | ���ʱ                             |
      |         | TOVAL = 0x0001 | TOVAL = 0xFFFF |
      +=========+================+================+
      | ����Ƶ            | 30.5us         | 2s             |
      +---------+----------------+----------------+
      | 256��Ƶ       | 7.83ms         | 512s           |
      +---------+----------------+----------------+
*/

#ifndef __AWBL_IMX10xx_RTWDT_H
#define __AWBL_IMX10xx_RTWDT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/*******************************************************************************
  includes
*******************************************************************************/
#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_wdt.h"

/*******************************************************************************
  define
*******************************************************************************/

/** \brief define IMX10xx watchdog's name */
#define  AWBL_IMX10XX_RTWDT_NAME  "imx10xx_rtwdt"

/*******************************************************************************
awbl_imx10xx_rtwdt_parm_set������optionsѡ��꣨����Ϊ������Ļ�ֵ��
*******************************************************************************/

#define  AW_RTWDOG_WINDOW_MODE_OPEN       ((1) << 15)    /**< \brief �򿪿��Ź�����ģʽ  */
#define  AW_RTWDOG_WINDOW_MODE_CLOSE      ((0) << 15)    /**< \brief �رտ��Ź�����ģʽ  */

#define  AW_RTWDOG_INTERRUPT_ENABLE       ((1) << 6)     /**< \brief ���Ź��ж�ʹ��  */
#define  AW_RTWDOG_INTERRUPT_DISABLE      ((0) << 6)     /**< \brief ���Ź��жϽ���  */

#define  AW_RTWDOG_PRESCALER_ENABLE       ((1) << 12)    /**< \brief ���Ź���Ƶʹ��  */
#define  AW_RTWDOG_PRESCALER_DISABLE      ((0) << 12)    /**< \brief ���Ź���Ƶ����  */

/******************************************************************************/

#define  AW_RTWDOG_INTERRUPT_FLAG_MASK    ((1) << 14)    /**< \brief ���Ź��жϱ�־  */
#define  AW_RTWDOG_RECONFIG_SUCESS        ((1) << 10)    /**< \brief ���Ź��������óɹ�  */

/** \brief define IMX10xx watchdog's time info */
typedef struct  awbl_imx10xx_rtwdt_devinfo {
    uint32_t        reg_base;           /**< \brief RTWDT register base address */
    uint32_t        clk_id;             /**< \brief clk id */
    uint32_t        wdt_time;           /**< \brief RTWDT time(ms), use to init1 */
    const uint32_t  inum;               /**< \brief RTWDT inum */
    void (*pfn_plfm_init) (void);   /**< \brief platform init */
} awbl_imx10xx_rtwdt_devinfo_t;

/** \brief define IMX10xx watchdog's device */
typedef struct  awbl_imx10xx_rtwdt_dev {
    struct awbl_dev          dev;            /**< the AWBus device */
    struct awbl_hwwdt_info   info;           /**< hw wdt info */
    uint32_t                 reg_base;       /**< hw wdt reg_base */
    uint32_t                 wdt_time_up;    /**< hw wdt wdt_time_up(ms) �������Ĭ��Ϊ0��ֻ���ڵ���awbl_imx10xx_rtwdt_parm_set������Ż���и�ֵ */
    uint32_t                 wdt_time_down;  /**< hw wdt wdt_time_down(ms) �������Ĭ��Ϊ0��ֻ���ڵ���awbl_imx10xx_rtwdt_parm_set������Ż���и�ֵ */
} awbl_imx10xx_rtwdt_dev_t;

/**
 * \brief IMX10xx rtwdt driver register
 */
void awbl_imx10xx_rtwdt_drv_register (void);


/**
 * \brief ���ÿ��Ź���ʱ���Ƿ�ʹ�ô���ģʽ���������Ź� �ж�����Լ������䴰��ֵ�ͳ�ʱֵ
 *
 * ע�⣺�����������ʱһ��Ҫ��options������������Ƶ��񣬷���������֪����μ���toval��ֵ
 *
 * param[in] p_dev   ���Ź������豸��ָ��
 * param[in] options   ��Ҫ���õĿ��Ź�ѡ���Ļ�ֵ�����磺AW_RTWDOG_PRESCALER_ENABLE |
 *                                             AW_OPEN_RTWDOG_WINDOW_MODE |
 *                                             AW_ENABLE_RTWDOG_INTERRUPT��
 *                     ������optionsѡ�����ֳ�����Ƶ��񣬴���ģʽ���أ��ж�ʹ�����
 * param[in] wdt_time_down   ���Ź�����ֵ���ޣ���λms��,�����ʹ�ô���ģʽ��ֱ������Ϊ0���ɣ���Ϊwin�Ĵ����ĺ���ֵ
 * param[in]  wdt_time_up   ���Ź�����ֵ���ޣ�����Ϊ0�������Ź���һֱ��λ,��λ��ms��������Ϊtoval�Ĵ����ĺ���ֵ
 *
 * \return ��
 */
void awbl_imx10xx_rtwdt_parm_set (struct awbl_dev *p_dev,
                                  uint16_t         options,
                                  uint32_t         wdt_time_down,
                                  uint32_t         wdt_time_up);

/**
 * ע�⣺��Ҫ��ʹ�ܴ���ģʽ��ʹ��
 * \brief ���Ź���ǰ�Ѽ����ļ���ֵ��ȡ������ֵΪ�Ѽ����ĺ�������
 *
 * param[in] p_dev   ���Ź������豸��ָ��
 * param[in] cnt_val   ���ڴ洢��ȡ�ĺ�����
 *
 * \return ��
 */
void awbl_imx10xx_rtwdt_cnt_val_get (struct awbl_dev *p_dev,
                                     uint32_t        *cnt_val);

/**
 * ע�⣺ֻ���ڵ���awbl_imx10xx_rtwdt_parm_set��������ܻ�ȡ�����û��ʹ�ô���ģʽ����wdt_time_down��ֵ
 * \brief ���Ź����õĴ���ֵ���޺����޻�ȡ(��λ��ms)
 *
 * param[in] p_dev   ���Ź������豸��ָ��
 * param[in] wdt_time_down   ���ڿ��Ź����޻�ȡ
 * param[in] wdt_time_up     ���ڿ��Ź����޻�ȡ
 *
 * \return ��
 */
void awbl_imx10xx_rtwdt_parm_get (struct awbl_dev *p_dev,
                                  uint32_t        *wdt_time_down,
                                  uint32_t        *wdt_time_up);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10xx_RTWDT_H */

/* end of file */
