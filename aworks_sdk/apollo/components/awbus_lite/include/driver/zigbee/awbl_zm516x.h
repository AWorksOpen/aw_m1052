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
 * \brief zigbee ģ�� ZM516X ����
 *
 * ������Ϊ ZM516X ģ��Ĳ����ӿڿ⣬�����ĵײ�ʹ�� UART ��׼�ӿ������� ZM516X 
 * ģ��
 *
 * \internal
 * \par modification history
 * - 1.02 18-03-30  pea, optimize API
 * - 1.01 18-01-22  pea, update API to v1.03 manual
 * - 1.00 14-11-05  afm, first implementation
 * \endinternal
 */

#ifndef __AWBL_ZM516X_H
#define __AWBL_ZM516X_H

#include "awbus_lite.h"
#include "aw_types.h"
#include "aw_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief ZM516X �豸���� */
#define    AWBL_ZM516X_DRV_NAME    "zm516x"

/** \brief ZM516X �豸��Ϣ */
struct awbl_zm516x_devinfo {
    int       com_id;               /**< \brief COM ID */
    uint32_t  baudrate;             /**< \brief ���ڲ����� */
    uint32_t  serial_opts;          /**< \brief ����ͨѶ�������� (CREAD | CS8) */
    int       rst_pin;              /**< \brief ��λ���� */
    int       ack_pin;              /**< \brief ACK ���� */
    uint16_t  ack_timeout;          /**< \brief ACK ��ʱʱ�� */
    uint8_t  *p_txbuf;              /**< \brief ���ͻ����� */
    uint32_t  txbuf_size;           /**< \brief ���ͻ�������С���ֽ����� */
    uint8_t  *p_rxbuf;              /**< \brief ���ջ����� */
    uint32_t  rxbuf_size;           /**< \brief ���ջ�������С���ֽ����� */
    void    (*pfn_plfm_init)(void); /**< \brief ƽ̨��ʼ������ */
};

/** \brief ZM516X �豸ʵ�� */
struct awbl_zm516x_dev {
    struct awbl_dev dev;       /**< \brief AWBus �豸ʵ���ṹ�� */
    AW_SEMB_DECL(ack_wait);    /**< \brief ACK �ȴ��ź��� */
};

#define AWBL_ZM516X_BAUDRATE_2400   1    /**< \brief ���ڲ����� 2400 */
#define AWBL_ZM516X_BAUDRATE_4800   2    /**< \brief ���ڲ����� 4800 */
#define AWBL_ZM516X_BAUDRATE_9600   3    /**< \brief ���ڲ����� 9600 */
#define AWBL_ZM516X_BAUDRATE_19200  4    /**< \brief ���ڲ����� 19200 */
#define AWBL_ZM516X_BAUDRATE_38400  5    /**< \brief ���ڲ����� 38400 */
#define AWBL_ZM516X_BAUDRATE_57600  6    /**< \brief ���ڲ����� 57600 */
#define AWBL_ZM516X_BAUDRATE_115200 7    /**< \brief ���ڲ����� 115200 */

#define AWBL_ZM516X_PARITY_NONE    0     /**< \brief ������У��λ */
#define AWBL_ZM516X_PARITY_ODD     1     /**< \brief ������У�� */
#define AWBL_ZM516X_PARITY_EVEN    2     /**< \brief ����żУ�� */

#define AWBL_ZM516X_ADC_CHANNEL0    0    /**< \brief ADC ͨ�� 0 */
#define AWBL_ZM516X_ADC_CHANNEL1    1    /**< \brief ADC ͨ�� 1 */
#define AWBL_ZM516X_ADC_CHANNEL2    2    /**< \brief ADC ͨ�� 2 */
#define AWBL_ZM516X_ADC_CHANNEL3    3    /**< \brief ADC ͨ�� 3 */

#define AWBL_ZM516X_GPIO_INPUT1     (0 << 0) /**< \brief GPIO1 ���� */
#define AWBL_ZM516X_GPIO_INPUT2     (0 << 1) /**< \brief GPIO2 ���� */
#define AWBL_ZM516X_GPIO_INPUT3     (0 << 2) /**< \brief GPIO3 ���� */
#define AWBL_ZM516X_GPIO_INPUT4     (0 << 3) /**< \brief GPIO4 ���� */

#define AWBL_ZM516X_GPIO_OUTPUT1    (1 << 0) /**< \brief GPIO1 ��� */
#define AWBL_ZM516X_GPIO_OUTPUT2    (1 << 1) /**< \brief GPIO2 ��� */
#define AWBL_ZM516X_GPIO_OUTPUT3    (1 << 2) /**< \brief GPIO3 ��� */
#define AWBL_ZM516X_GPIO_OUTPUT4    (1 << 3) /**< \brief GPIO4 ��� */

#define AWBL_ZM516X_GPIO_LOW1       (0 << 0) /**< \brief GPIO1 ����͵�ƽ */
#define AWBL_ZM516X_GPIO_LOW2       (0 << 1) /**< \brief GPIO2 ����͵�ƽ */
#define AWBL_ZM516X_GPIO_LOW3       (0 << 2) /**< \brief GPIO3 ����͵�ƽ */
#define AWBL_ZM516X_GPIO_LOW4       (0 << 3) /**< \brief GPIO4 ����͵�ƽ */

#define AWBL_ZM516X_GPIO_HIGH1      (1 << 0) /**< \brief GPIO1 ����ߵ�ƽ */
#define AWBL_ZM516X_GPIO_HIGH2      (1 << 1) /**< \brief GPIO2 ����ߵ�ƽ */
#define AWBL_ZM516X_GPIO_HIGH3      (1 << 2) /**< \brief GPIO3 ����ߵ�ƽ */
#define AWBL_ZM516X_GPIO_HIGH4      (1 << 3) /**< \brief GPIO4 ����ߵ�ƽ */

#define AWBL_ZM516X_EXTI_FALL1      (0 << 0) /**< \brief IO1 �½��ش��� */
#define AWBL_ZM516X_EXTI_FALL2      (0 << 1) /**< \brief IO2 �½��ش��� */
#define AWBL_ZM516X_EXTI_FALL3      (0 << 2) /**< \brief IO3 �½��ش��� */
#define AWBL_ZM516X_EXTI_FALL4      (0 << 3) /**< \brief IO4 �½��ش��� */

#define AWBL_ZM516X_EXTI_RISE1      (1 << 0) /**< \brief IO1 �����ش��� */
#define AWBL_ZM516X_EXTI_RISE2      (1 << 1) /**< \brief IO2 �����ش��� */
#define AWBL_ZM516X_EXTI_RISE3      (1 << 2) /**< \brief IO3 �����ش��� */
#define AWBL_ZM516X_EXTI_RISE4      (1 << 3) /**< \brief IO4 �����ش��� */

#define AWBL_ZM516X_SEND_MODE_UNICAST    0    /**< \brief ���� */
#define AWBL_ZM516X_SEND_MODE_BROADCAST  1    /**< \brief �㲥 */

#define AWBL_ZM516X_NETWORK_TYPE_MASTER  0    /**< \brief �����ڵ� */
#define AWBL_ZM516X_NETWORK_TYPE_SLAVE   1    /**< \brief �ӻ���� */

#define AWBL_ZM516X_NETWORK_STATE_MASTER_IDLE    0  /**< \brief �������� */
#define AWBL_ZM516X_NETWORK_STATE_MASTER_DETECT  1  /**< \brief �������ڼ������ */
#define AWBL_ZM516X_NETWORK_STATE_MASTER_JOIN    2  /**< \brief ��������ӻ��������� */

#define AWBL_ZM516X_NETWORK_STATE_SLAVE_JOIN     0   /**< \brief �ӻ����ڼ������� */
#define AWBL_ZM516X_NETWORK_STATE_SLAVE_ONLINE   1   /**< \brief �ӻ��Ѽ������� */
#define AWBL_ZM516X_NETWORK_STATE_SLAVE_OFFLINE  2   /**< \brief �ӻ����˳����� */

#define AWBL_ZM516X_SOFTWARE_FASTZIGBEE  1  /**< \brief �̼�����Ϊ Fastzigbee */
#define AWBL_ZM516X_SOFTWARE_ZLGNET      2  /**< \brief �̼�����Ϊ zlgnet */
#define AWBL_ZM516X_SOFTWARE_AW          3  /**< \brief �̼�����Ϊ aw ϵ�� */

/** \brief ZM516X ģ���������Ϣ */
typedef struct awbl_zm516x_cfg_info {
    char     dev_name[16];     /**< \brief �豸���� */
    char     dev_pwd[16];      /**< \brief �豸���� */
    uint8_t  dev_mode;         /**< \brief �豸���ͣ�0:�ն��豸 1:·���豸 */
    uint8_t  chan;             /**< \brief ͨ���� */
    uint16_t panid;            /**< \brief ���� ID (PanID) */
    uint16_t my_addr;          /**< \brief ���������ַ */
    uint8_t  my_mac[8];        /**< \brief ���������ַ(MAC) */
    uint16_t dst_addr;         /**< \brief Ŀ�������ַ */
    uint8_t  dst_mac[8];       /**< \brief Ŀ�������ַ(����) */

    /**
     * \brief ���书�ʣ�ֵΪ 0~3���ֱ��Ӧ���书�ʣ�
     *        -32dBm��-20.5dBm��-9dBm��+2.5dBm
     */
    uint8_t  power_level;
    uint8_t  retry_num;        /**< \brief �����������Դ��� */
    uint8_t  tran_timeout;     /**< \brief ������������ʱ����(10ms) */

    /**
     * \brief ���ڲ����ʣ�ֵΪ 1~7���ֱ��Ӧ�����ʣ�
     *        2400��4800��9600��19200��38400��57600��115200
     */
    uint8_t  serial_rate;
    uint8_t  serial_data;    /**< \brief ��������λ��ֵΪ 5~8 */
    uint8_t  serial_stop;    /**< \brief ����ֹͣλ��ֵΪ 1~2 */
    uint8_t  serial_parity;  /**< \brief ����У��λ 0:��У�� 1:��У�� 2:żУ�� */
    uint8_t  send_mode;      /**< \brief ����ģʽ 0:����  1:�㲥 */

    uint8_t  state;        /**< \brief ����״̬ */
    uint16_t software;     /**< \brief �̼����� 1:fastzigbee 2:zlgnet 3:awϵ�� */
    uint16_t version;      /**< \brief �̼��汾 */
} awbl_zm516x_cfg_info_t;

/** \brief ZM516X ģ��Ļ�����Ϣ(ͨ������������) */
typedef struct awbl_zm516x_base_info {
    uint16_t software;      /**< \brief �̼����� 1:fastzigbee 2:zlgnet 3:awϵ�� */
    uint8_t  chan;          /**< \brief ͨ���� */
    uint8_t  data_rate;     /**< \brief RF ͨѶ����  */
    uint16_t panid;         /**< \brief ���� ID (PanID) */
    uint16_t addr;          /**< \brief �����ַ */
    uint8_t  state;         /**< \brief ����״̬ */
} awbl_zm516x_base_info_t;

/** \brief ZM516X �ӻ�ģ����Ϣ(ͨ����ѯ����ģ��洢�Ĵӻ���Ϣ������) */
typedef struct awbl_zm516x_slave_info {
    uint8_t  mac[8];             /**< \brief MAC ��ַ */
    uint16_t addr;               /**< \brief �����ַ */
} awbl_zm516x_slave_info_t;

/**
 * \brief ZM516X ģ��� 4 ͨ�� PWM ��Ϣ
 */
typedef struct awbl_zm516x_pwm {

    /**
     * \brief PWM Ƶ�ʣ���λΪ Hz��ͨ�� 1��2��4 ��Ƶ�ʷ�Χ���趨�� 62Hz~800000Hz��
     *        ͨ�� 3 ��Ƶ�ʷ�Χ���趨�� 62Hz~60000Hz���� PWM �źŵ�Ƶ��Ϊ 0 ʱ��
     *        ��ʾ����� PWM �ź�
     */
    uint32_t freq[4];

    /**
     * \brief PWM ռ�ձȣ�ռ�ձȵ�ֵ���趨�� 1~99����ʾռ�ձȵİٷֱ�
     */
    uint8_t  duty_cycle[4];
} awbl_zm516x_pwm_t;

/**
 * \brief �� ZM516X ����ע�ᵽ AWBus ��ϵͳ��
 *
 * \note �������н�ֹ���ò���ϵͳ API
 */
void awbl_zm516x_drv_register (void);

/**
 * \brief ��λָ����ַ�� ZM516X ģ��
 *
 * \param[in] id       �豸��
 * \param[in] dst_addr Ŀ���ַ
 *
 * \retval AW_OK ��λ�ɹ�
 * \retval  <0   ��λʧ��
 *
 * \note ��λԶ��ģ��ʱ������ AW_OK ֻ����λ�����ѷ��ͣ�������λ�ɹ�
 */
aw_err_t awbl_zm516x_reset (int id, uint16_t dst_addr);

/**
 * \brief �ָ�ָ����ַ�� ZM516X ģ��Ϊ��������
 *
 * \param[in] id       �豸��
 * \param[in] dst_addr Ŀ���ַ
 *
 * \retval  AW_OK ���óɹ�
 * \retval -EPERM ����ʧ��
 */
aw_err_t awbl_zm516x_default_set (int id, uint16_t dst_addr);

/**
 * \brief ��ȡָ����ַ�� ZM516X ģ���������Ϣ
 *
 * \param[in]  id         �豸��
 * \param[in]  is_local   �Ƿ��ǻ�ȡ����ģ���������Ϣ
 * \param[in]  dst_addr   �����Ҫ��ȡԶ��ģ��������Ϣ����Ҫ����Ŀ���ַ�������
 *                        �Դ˲���
 * \param[out] p_cfg_info ָ��洢��ȡ���� ZM516X ģ���������Ϣ��ָ��
 *
 * \retval  AW_OK  ��ȡ�ɹ�
 * \retval -EPERM  ��ȡʧ��
 * \retval -EINVAL ��Ч����
 *
 * \attention ��������λ�������Ϊ 5��6��7 λ���򲻿��Ի�ȡ������Ϣ
 */
aw_err_t awbl_zm516x_cfg_info_get (int                     id,
                                   aw_bool_t                  is_local,
                                   uint16_t                dst_addr,
                                   awbl_zm516x_cfg_info_t *p_cfg_info);

/**
 * \brief �޸�ָ����ַ�� ZM516X ģ���������Ϣ
 *
 * \param[in] id         �豸��
 * \param[in] dst_addr   Ŀ���ַ
 * \param[in] p_cfg_info ָ�� ZM516X ģ��������Ϣ��ָ��
 *
 * \retval  AW_OK �޸ĳɹ�
 * \retval -EPERM �޸�ʧ��
 *
 * \attention ZM516X ģ��ֻ���ڴ�������λΪ 8 λ��У��Ϊ��У�飬ֹͣλΪ 1 λʱ
 *            �����޸�������Ϣ���ڸ���ģ�������λ��У��λ��ֹͣλʱ��Ҫע�⣬
 *            �����С�ĸĴ��ˣ���Ҫ��ģ��ָ�����Ĭ�ϲ��ܲ���
 */
aw_err_t awbl_zm516x_cfg_info_set (int                     id,
                                   uint16_t                dst_addr,
                                   awbl_zm516x_cfg_info_t *p_cfg_info);

/**
 * \brief ���� ZM516X ģ��ͨ����
 *
 * ģ��ɹ����� 16 ������ͨ���ϣ�����ģ����ز�Ƶ�ʲ�ͬ����ͬͨ����ģ��˴˼���
 * ��ͨ����ͨ�����������Ͽ���ʵ�������ϻ������ε�Ч��
 *
 * \param[in] id      �豸��
 * \param[in] chan    ͨ���� 11~26
 * \param[in] is_temp �����Ƿ���綪ʧ
 *
 * \retval  AW_OK ���óɹ�
 * \retval -EPERM ����ʧ��
 */
aw_err_t awbl_zm516x_chan_set (int id, uint8_t chan, aw_bool_t is_temp);

/**
 * \brief ���� ZM516X ģ����յ����ݰ���ͷ�Ƿ���ʾԴ��ַ
 *
 * \param[in] id      �豸��
 * \param[in] is_show ��ͷ�Ƿ���ʾԴ��ַ
 * \param[in] is_temp �����Ƿ���綪ʧ
 *
 * \retval  AW_OK ���óɹ�
 * \retval -EPERM ����ʧ��
 */
aw_err_t awbl_zm516x_display_head_set (int       id, 
                                       aw_bool_t is_show, 
                                       aw_bool_t is_temp);

/**
 * \brief ���� ZM516X ģ������������ģʽ
 *
 * \param[in] id �豸��
 *
 * \return ��
 *
 * \note �������ߺ󲻱�����ʱ�������ã���ͨ����λģ������� WAKE �ܽŽ��л��ѣ�
 *       ģ�鱻���Ѻ����йܽŽ��ָ�����ʼ״̬
 */
aw_err_t awbl_zm516x_enter_deep_sleep (int id);

/**
 * \brief ���� ZM516X ģ��ķ���ģʽ
 *
 * \param[in] id        �豸��
 * \param[in] send_mode ģ��ķ���ģʽ���μ� AWBL_ZM516X_SEND_MODE_*
 *
 * \retval  AW_OK ���óɹ�
 * \retval -EPERM ����ʧ��
 */
aw_err_t awbl_zm516x_send_mode_temp_set (int id, uint8_t send_mode);

/**
 * \brief ���� ZM516X ģ��Ŀ���ַ
 *
 * \param[in] id       �豸��
 * \param[in] dst_addr Ҫ���õ�Ŀ���ַ
 *
 * \retval  AW_OK ���óɹ�
 * \retval -EPERM ����ʧ��
 */
aw_err_t awbl_zm516x_dst_addr_temp_set (int id, uint16_t dst_addr);

/**
 * \brief ��ȡָ����ַ�� ZM516X ģ����ź�ǿ��
 *
 * \param[in]  id       �豸��
 * \param[in]  dst_addr Ҫ��ȡ�� ZM516X ģ��������ַ
 * \param[out] p_signal ָ�򱣴��ȡ�����ź�ǿ�ȵ�ָ��
 *
 * \retval  AW_OK ��ȡ�ɹ�
 * \retval -EPERM ��ȡʧ��
 */
aw_err_t awbl_zm516x_sigal_get (int id, uint16_t dst_addr, uint8_t *p_signal);

/**
 * \brief ��������ģ��
 *
 * ģ����յ�������󣬻��������ڵ���ͬͨ��������ģ�鷢���㲥�����������б���
 * ˾�̼��� ZM516X ģ�鶼��Ӧ��˹㲥��Ӧ�����ݻὫ�Լ�����ػ�����Ϣ���ص�����
 * ����Ŀ��ڵ㡣��������ǰ��Ҫ����ͨ���Ų�����������Ӧͨ����ģ��
 *
 * \param[in]  id          �豸��
 * \param[in]  buf_size    p_base_info ָ��Ļ�������С����λΪ
 *                         sizeof(awbl_zm516x_base_info_t)
 * \param[out] p_base_info ָ��洢���յ��Ĵӻ���Ϣ��ָ��
 * \param[out] p_get_size  ָ��洢ʵ����������ģ��������ָ��
 *
 * \retval  AW_OK  ��ȡ�ɹ�
 * \retval -EPERM  ��ȡʧ��
 * \retval -EINVAL ��Ч����
 */
aw_err_t awbl_zm516x_discover (int                      id,
                               uint8_t                  buf_size,
                               awbl_zm516x_base_info_t *p_base_info,
                               uint8_t                 *p_get_size);

/**
 * \brief ��ȡָ����ַ�� ZM516X ģ��� GPIO �����������
 *
 * \param[in]  id       �豸��
 * \param[in]  dst_addr Ҫ��ȡ�� ZM516X ģ��������ַ
 * \param[out] p_dir    ��ȡ���Ĺܽŷ��򣬲μ� AWBL_ZM516X_GPIO_INPUT* ��
 *                      AWBL_ZM516X_GPIO_OUTPUT*
 *
 * \retval  AW_OK  ��ȡ�ɹ�
 * \retval -EPERM  ��ȡʧ��
 * \retval -EINVAL ��Ч����
 */
aw_err_t awbl_zm516x_gpio_dir_get (int id, uint16_t dst_addr, uint8_t *p_dir);

/**
 * \brief ����ָ����ַ�� ZM516X ģ��� GPIO �����������
 *
 * \param[in] id       �豸��
 * \param[in] dst_addr Ҫ���õ� ZM516X ģ��������ַ
 * \param[in] dir      �ܽŷ�����Ҫʹ�û�����һ���Դ������йܽŵ����ã�Ĭ��Ϊ
 *                     ���룬�μ�AWBL_ZM516X_GPIO_INPUT* �� 
 *                     AWBL_ZM516X_GPIO_OUTPUT*���� AWBL_ZM516X_GPIO_INPUT1 | 
 *                     AWBL_ZM516X_GPIO_OUTPUT2�������� IO1��IO3��IO4 Ϊ���룬
 *                     IO2 Ϊ���
 *                     
 * \param[in] is_temp  �����Ƿ���綪ʧ
 *
 * \retval  AW_OK ���óɹ�
 * \retval -EPERM ����ʧ��
 */
aw_err_t awbl_zm516x_gpio_dir_set (int         id,
                                   uint16_t    dst_addr,
                                   uint8_t     dir,
                                   aw_bool_t   is_temp);

/**
 * \brief ��ȡָ����ַ�� ZM516X ģ��� GPIO ����ֵ
 *
 * \param[in]  id       �豸��
 * \param[in]  dst_addr Ҫ��ȡ�� ZM516X ģ��������ַ
 * \param[out] p_value  ��ȡ���Ĺܽ�����ֵ���μ� AWBL_ZM516X_GPIO_LOW* ��
 *                      AWBL_ZM516X_GPIO_HIGH*
 *
 * \retval  AW_OK  ��ȡ�ɹ�
 * \retval -EPERM  ��ȡʧ��
 * \retval -EINVAL ��Ч����
 */
aw_err_t awbl_zm516x_gpio_get (int id, uint16_t dst_addr, uint8_t *p_value);

/**
 * \brief ����ָ����ַ�� ZM516X ģ��� GPIO ���ֵ
 *
 * \param[in] id       �豸��
 * \param[in] dst_addr Ҫ���õ� ZM516X ģ��������ַ
 * \param[in] value    �ܽ����ֵ����Ҫʹ�û�����һ���Դ������йܽŵ����ֵ��
 *                     Ĭ������͵�ƽ���μ�AWBL_ZM516X_GPIO_LOW* �� 
 *                     AWBL_ZM516X_GPIO_HIGH*��
 *                     �� AWBL_ZM516X_GPIO_LOW1 | AWBL_ZM516X_GPIO_HIGH2����
 *                     ���� IO1��IO3��IO4 ����͵�ƽ��IO2 Ϊ����ߵ�ƽ
 * \param[in] is_temp  �����Ƿ���綪ʧ
 *
 * \retval  AW_OK ���óɹ�
 * \retval -EPERM ����ʧ��
 */
aw_err_t awbl_zm516x_gpio_set (int      id,
                               uint16_t dst_addr,
                               uint8_t  value,
                               aw_bool_t   is_temp);

/**
 * \brief ��ȡָ����ַ�� ZM516X ģ��� AD ת��ֵ
 *
 * ��ȡ���� AD ֵΪ AD ��ת��ֵ����Ҫ�ο� ZM516X ģ���û��ֲ�
 * ���� ZM516X ģ�� AD ��λ�����ο���ѹת��Ϊ��ѹֵ
 *
 * \param[in]  id       �豸��
 * \param[in]  dst_addr Ҫ��ȡ�� ZM516X ģ��������ַ
 * \param[in]  chan     AD ��ͨ���ţ��μ� AWBL_ZM516X_ADC_CHANNEL*
 * \param[out] p_value  AD ��ת��ֵ
 *
 * \retval  AW_OK  ��ȡ�ɹ�
 * \retval -EPERM  ��ȡʧ��
 * \retval -EINVAL ��Ч����
 */
aw_err_t awbl_zm516x_adc_get (int       id,
                              uint16_t  dst_addr,
                              uint8_t   chan,
                              uint16_t *p_value);

/**
 * \brief ��ȡָ����ַ�� ZM516X ģ��� IO/AD �ɼ�����
 *
 * \param[in]  id            �豸��
 * \param[in]  dst_addr      Ҫ��ȡ�� ZM516X ģ��������ַ
 * \param[out] p_trigger     ��ȡ���� IO �����������ã��������Ҫ��ȡ������ NULL 
 *                           ���ɡ����μ� AWBL_ZM516X_EXTI_FALL* ��
 *                           AWBL_ZM516X_EXTI_RISE*
 * \param[out] p_period_10ms ��ȡ�����ϴ��������ã���λΪ 10ms���������Ҫ��ȡ��
 *                           ���� NULL ����
 * \param[out] p_is_dormant  ��ȡ�����Ƿ��������ã��������Ҫ��ȡ������ NULL ��
 *                           �ɡ�ָʾģ���Ƿ�������״̬��TRUE ��ʾģ�鴦������״
 *                           ̬��IO �������õ�״̬�����仯ʱ����ģ�飬����Ŀ���
 *                           ���ַ���� IO/AD �ɼ�֡��������ڴ��� 0��ģ����˿�
 *                           �� IO�����⣬����������ڶ�ʱ��ʱ���ѣ�Ȼ����Ŀ���
 *                           ���ַ����IO/AD �ɼ�֡���������� IO/AD �ɼ�֡��ģ
 *                           ���������״̬��FALSE ��ʾģ�鲻����
 *
 * \retval  AW_OK ��ȡ�ɹ�
 * \retval -EPERM ��ȡʧ��
 */
aw_err_t awbl_zm516x_io_adc_upload_get (int       id,
                                        uint16_t  dst_addr,
                                        uint8_t  *p_trigger,
                                        uint16_t *p_period_10ms,
                                        aw_bool_t   *p_is_dormant);

/**
 * \brief ����ָ����ַ�� ZM516X ģ��� IO/AD �ɼ�����
 *
 * AW516x ϵ�� ZM516X ģ���� 4 ·�� IO �� 4 ·�� AD��IO/AD �ɼ�����������趨ģ��
 * IO �����ϴ� IO �� AD ״̬���������õ�����ʱ�䶨ʱ�ϴ� IO �� AD ״̬
 *
 * \param[in] id          �豸��
 * \param[in] p_zb_addr   Ҫ���õ� ZM516X ģ��������ַ
 * \param[in] trigger     IO �������أ��μ� AWBL_ZM516X_EXTI_FALL* ��
 *                        AWBL_ZM516X_EXTI_RISE*
 * \param[in] period_10ms �ϴ����ڣ���λΪ 10ms���������õ�ֵΪ 65535��
 *                        �����õ��������Ϊ 655350ms
 * \param[in] is_dormant  �Ƿ����ߣ�ָʾģ���Ƿ�������״̬�����ֽ�Ϊ 1 ��ʾģ
 *                        �鴦������״̬��IO �������õ�״̬�����仯ʱ����ģ�飬
 *                        ����Ŀ��ڵ��ַ���� IO/AD �ɼ�֡��������ڴ��� 0��ģ
 *                        ����˿��� IO�����⣬����������ڶ�ʱ��ʱ���ѣ�Ȼ����
 *                        Ŀ��ڵ��ַ����IO/AD �ɼ�֡���������� IO/AD �ɼ�֡��
 *                        ģ���������״̬�����ֽ�Ϊ 0 ��ʾģ�鲻����
 *
 * \retval  AW_OK ���óɹ�
 * \retval -EPERM ����ʧ��
 */
aw_err_t awbl_zm516x_io_adc_upload_set (int      id,
                                        uint16_t dst_addr,
                                        uint8_t  trigger,
                                        uint16_t period_10ms,
                                        aw_bool_t   is_dormant);

/**
 * \brief ��ȡָ����ַ�� ZM516X ģ��� PWM ���ֵ����
 *
 * \param[in]  id       �豸��
 * \param[in]  dst_addr Ҫ��ȡ�� ZM516X ģ��������ַ
 * \param[out] p_pwm    ָ��洢��ȡ���� awbl_zm516x_pwm_t �ṹ��ָ��
 *
 * \retval  AW_OK  ��ȡ�ɹ�
 * \retval -EPERM  ��ȡʧ��
 * \retval -EINVAL ��Ч����
 */
aw_err_t awbl_zm516x_pwm_get (int                id,
                              uint16_t           dst_addr,
                              awbl_zm516x_pwm_t *p_pwm);

/**
 * \brief ����ָ����ַ�� ZM516X ģ��� PWM ���ֵ
 *
 * \param[in] id       �豸��
 * \param[in] dst_addr Ҫ���õ� ZM516X ģ��������ַ
 * \param[in] p_pwm    ָ�� awbl_zm516x_pwm_t �ṹ��ָ��
 *
 * \retval  AW_OK  ���óɹ�
 * \retval -EPERM  ����ʧ��
 * \retval -EINVAL ��Ч����
 */
aw_err_t awbl_zm516x_pwm_set (int                id,
                              uint16_t           dst_addr,
                              awbl_zm516x_pwm_t *p_pwm);

/**
 * \brief ��������������
 *
 * ����������Ϊһ����ӵ������������˽ṹ�����������ӻ��źŲ��ɴ�ʱ���ɼ����м�
 * �ڵ�����źŴ��ݡ�ģ����������ģʽ�£�����ģ����Զ�ѡ����Χû�б�ʹ�õ� PANID 
 * ��ͨ�����γ�һ�����������磬�����Զ�����һ��Ψһ�ı��������ַ���ӻ�ģ�飬��
 * ��ģ��ʹ��ʱ��ʹ�������������ܺ�Ͳ���Ҫ�����κε����ò������ӻ�ģ���ڼ�������
 * ����ܸ���������ͨѶ��һ������ģ�������� 200 ���ӻ�ģ�顣��ϸ������ο��ĵ�
 * AW516x_ZigBee.pdf
 *
 * \param[in] id        �豸��
 * \param[in] type      �ڵ����ͣ��μ� AWBL_ZM516X_NETWORK_TYPE_*
 * \param[in] is_enable �Ƿ�ʹ��������
 *
 * \retval  AW_OK  ���óɹ�
 * \retval -EPERM  ����ʧ��
 * \retval -EINVAL ��Ч����
 */
aw_err_t awbl_zm516x_network_config (int id, uint8_t type, aw_bool_t is_enable);

/**
 * \brief ��������ӻ���������
 *
 * ����������翪���Ĵ���ʱ��Ϊ������������������������������������ʼ���ܴ�
 * �����������󣬵����������ʱ����������ٽ��ܴӻ����������󣬴���ʱ�������
 * �������������Ĺ���״̬
 *
 * \param[in] id          �豸��
 * \param[in] time_second ����������翪���Ĵ���ʱ�䣬��λΪ��
 *
 * \retval  AW_OK ���óɹ�
 * \retval -EPERM ����ʧ��
 */
aw_err_t awbl_zm516x_master_join_enable (int id, uint16_t time_second);

/**
 * \brief ��ѯ����ģ��洢�Ĵӻ���Ϣ
 *
 * \param[in]  id           �豸��
 * \param[in]  buf_size     p_slave_info ָ��Ļ�������С����λΪ
 *                          sizeof(awbl_zm516x_slave_info_t)
 * \param[out] p_slave_info ָ��洢���յ��Ĵӻ���Ϣ��ָ��
 * \param[out] p_get_size   ָ��洢ʵ�ʻ�ȡ���Ĵӻ���Ϣ������ָ��
 *
 * \retval  AW_OK  ��ѯ�ɹ�
 * \retval -EPERM  ��ѯʧ��
 * \retval -EINVAL ��Ч����
 */
aw_err_t awbl_zm516x_master_slave_info_get (int                       id,
                                            uint8_t                   buf_size,
                                            awbl_zm516x_slave_info_t *p_slave_info,
                                            uint8_t                  *p_get_size);

/**
 * \brief ��ѯ�ڵ����������Ǵӻ�
 *
 * \param[in]  id               �豸��
 * \param[out] p_type           ��ȡ���Ľڵ����ͣ��μ� AWBL_ZM516X_NETWORK_TYPE_*��
 *                              �粻��Ҫ��ȡ�ò��������� NULL ����
 * \param[out] p_response_state ��ȡ������Ӧ״̬����ģ������Ϊ����ʱ��
 *                                  �μ� AWBL_ZM516X_NETWORK_STATE_MASTER_*
 *                              ��ģ������Ϊ�ӻ�ʱ��
 *                                  �μ� AWBL_ZM516X_NETWORK_STATE_SLAVE_*
 *                              �粻��Ҫ��ȡ�ò��������� NULL ����
 *
 * \retval  AW_OK ��ѯ�ɹ�
 * \retval -EPERM ��ѯʧ��
 */
aw_err_t awbl_zm516x_network_state_get (int      id,
                                        uint8_t *p_type,
                                        uint8_t *p_response_state);

/**
 * \brief �� ZM516X ģ�鷢�� nbytes ���ֽ�����
 *
 * ����ӿ��� ZM516X ģ�鷢�� nbytes ���ֽ����ݡ����豸���ڲ����岻�����������´�
 * ��ʣ������ݲ��ᱻд�룬����ֵΪ�Ѿ����͵����ݸ���
 *
 * \param[in] id     �豸��
 * \param[in] p_buf  Ҫд��ģ������ݻ�����ָ��
 * \param[in] nbytes Ҫд����ֽڸ���
 *
 * \return �ɹ����͵����ݸ�����С�� 0 Ϊʧ�ܣ�ʧ��ԭ��ɲ鿴 errno
 *
 * \attention �÷��ͺ���Ϊ�첽����������ZM516X �����������Ǹ��ݴ����ַ�֡���ʱ
 *            �����ж���������Ľ���������ڵ����� awbl_zm516x_send ���ͺ�����
 *            Ҫ�����ŵ��� ZM516X �����������Ҫ��ʱ�ȴ����ͻ���������ȫ���͵�
 *            ʱ����� ZM516X ��֡���ʱ��󣬲��ܵ��� ZM516X ������������� 
 *            ZM516X����������ᵱ�������ݷ��� ZM516X �����ϣ���ʱ�ȴ�ʱ�佨���ǣ�
 *            T(s) = (1 / ������) * 10 * �ֽ��� + 0.05
 */
ssize_t awbl_zm516x_send (int id, const void *p_buf, size_t nbytes);

/**
 * \brief �� ZM516X ģ�鷢�� nbytes ���ֽ����ݣ����ȴ� ACK
 *
 * ����ӿ��� ZM516X ģ�鷢�� nbytes ���ֽ����ݡ����豸���ڲ����岻������������
 * ����ʣ������ݲ��ᱻд�룬����ֵΪ�Ѿ����͵����ݸ���
 *
 * \param[in] id     �豸��
 * \param[in] p_buf  Ҫд��ģ������ݻ�����ָ��
 * \param[in] nbytes Ҫд����ֽڸ���
 *
 * \retval  > 0       �ɹ����͵����ݸ���
 * \retval -ENOTSUP   û������ ACK ����
 * \retval -ETIMEDOUT ������Ӧ��
 *
 * \attention 1. �÷��ͺ���Ϊͬ�������������������ݺ󽫵ȴ� ACK�����յ� ACK ����
 *               ��ʱʱ�Ż᷵�أ�
 *            2. ����ģʽΪ�㲥ģʽʱ������� ACK��
 */
ssize_t awbl_zm516x_send_with_ack (int id, const void *p_buf, size_t nbytes);

/**
 * \brief �� ZM516X ģ����� maxbytes ���ֽ�����
 *
 * - ����ӿڴ� ZM516X ģ�������� maxbytes ���ֽ�����
 * - ���豸�ɽ������ݸ���Ϊ 0������������
 *
 * \param[in]  id       �豸��
 * \param[out] p_buf    ��Ŷ�ȡ���ݵĻ�����ָ��
 * \param[in]  maxbytes ���Ҫ��ȡ���ֽڸ���
 *
 * \return �ɹ����յ����ݸ�����С�� 0 Ϊʧ�ܣ�ʧ��ԭ��ɲ鿴 errno
 */
aw_err_t awbl_zm516x_receive (int id, void *p_buf, size_t maxbytes);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_ZM516X_H */

/* end of file */
