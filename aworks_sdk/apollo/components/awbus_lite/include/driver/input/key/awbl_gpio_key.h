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
 * \brief ���ڱ�׼GPIO�����İ�������
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "gpio-key"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct gpio_key_data
 *
 * \par 2.�����豸
 *
 *  - �κο���ʹ�ñ�׼GPIO�ӿڷ��ʵİ���
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_drv_input_gpio_key_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_input_key
 *
 * \internal
 * \par modification history
 * - 1.00 13-03-11  orz, first implementation.
 * \endinternal
 */

#ifndef __AWBL_GPIO_KEY_H
#define __AWBL_GPIO_KEY_H

#include "aw_common.h"
#include "awbus_lite.h"
#include "awbl_input.h"
#include "aw_types.h"
#include "aw_delayed_work.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_input_gpio_key
 * \copydetails awbl_gpio_key.h
 * @{
 */

/**
 * \defgroup  grp_awbl_drv_input_gpio_key_hwconf �豸����/����
 *
 * ͨ�ð��������豸�Ķ���������Ĵ�����ʾ���û��ڶ���Ŀ��ϵͳ��Ӳ����Դʱ��
 * ����ֱ�ӽ���δ���Ƕ�뵽 awbus_lite_hwconf_usrcfg.c �ж�Ӧ��λ�ã�Ȼ����豸
 * ��Ϣ���б�Ҫ������(�����õ���Ŀ�á�x����ʶ)��
 *
 * \note �ж��ٸ����������豸���ͽ�����Ĵ���Ƕ����ٴΣ�ע�⽫�����޸�Ϊ��ͬ
 *       �����֡�
 *
 * \include  hwconf_usrcfg_gpio_key.c
 * \note �����������д����û����á���ʶ�ģ���ʾ�û��ڿ���Ӧ��ʱ�������ô˲�����
 *
 * - ��1�� ������Ӧ��GPIO��źͼ�ֵӳ��� \n
 * ÿ��������Ӧһ��GPIO��һ����ֵ����ֵ�Ƽ�ʹ�� aw_input_key.h �ж���ı�׼��ֵ
 *
 * - ��2�� ��������ʱ�Ƿ�Ϊ�͵�ƽ��Ч \n
 * �����͵�ƽ��Ч��true��������false
 *
 * - ��3�� �����ж�ɨ�跽ʽ����ʱ����ʱ�� \n
 * ����ʱ��һ��Ϊ10~20����
 *
 * - ��4�� ��ʱɨ�跽ʽ�µ�ɨ����ʱ�� \n
 * Ϊ0��ʾʹ���ж�ģʽ����0��Ϊ��ʱɨ��ļ��ʱ�䡣ע�����GPIO������֧���ж���
 * ����ʹ���ж�ģʽ
 *
 * - ��5�� �ж�ģʽ�µ�GPIO���ء��жϺ� \n
 * �����������֧��ĳЩоƬ��һЩGPIO������һ���жϵ������Ϊ0��ʾ��ʹ��
 */

/** @} */

/** \brief name of GPIO key driver */
#define GPIO_KEY_DEV_NAME       "gpio-key"

/** \brief max keys a gpio-key device can handle */
#define GPIO_KEY_MAX_KEYS       32

/******************************************************************************/
struct gpio_key_map {
    uint16_t gpio;  /**< \brief GPIO connected to the key */
    uint16_t key;   /**< \brief key value */
};

/**
 * \brief GPIO key platform parameters
 * \note -  if scan_interval is not 0, the keypad driver will use a timer to
 *          scan the keypad in period, otherwise will use GPIO interrupt.
 */
struct gpio_key_data {
    void (*plat_init) (void);

    const struct gpio_key_map *keymap; /**< \brief key map */

    aw_bool_t    active_low;/**< \brief gpio polarity */
    uint8_t num_keys;       /**< \brief number of keys */
    uint8_t debounce_ms;    /**< \brief key debounce interval in ms */
    uint8_t scan_interval;  /**< \brief key scanning interval in ms */
    uint8_t clustered_irq;  /**< \brief irq that all key GPIOs bundled*/
};
#define to_gpio_key_data(dev)       ((void *)AWBL_DEVINFO_GET(dev))

/** \brief GPIO key structure */
struct gpio_key_dev {
    struct awbl_dev        dev;         /**< \brief AWBus device */
    struct aw_delayed_work work;        /**< \brief key scan delayed work */
    uint32_t               key_state;   /**< \brief last state of keys */
};
#define to_gpio_key(dev)            ((struct gpio_key_dev *)(dev))

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_GPIO_KEY_H */

/* end of file */
