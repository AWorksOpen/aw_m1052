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
 * \brief ��׼�źŲɼ�ģ�� TPS08U ����
 *
 * ������Ϊ TPS08U ģ��Ĳ����ӿڿ⣬�����ĵײ�ʹ�� SPI ��׼�ӿ������� TPS08U ģ
 * ��
 *
 * TPS08U ���� 8 ��ͨ��(CH1~CH8)��ÿ 2 ��ͨ��Ϊһ�飬�� CH1 �� CH2 Ϊһ�飬CH3 
 * ��CH4 Ϊһ�飬�Դ����ơ�ÿ��ͨ��������������Ϊ��ѹͨ�����ߵ���ͨ����
 *
 * ��ʹ�ô������ӿ�(aw_sensor.h)���� TPS08U ��ʱ��TPS08U �ṩ�� 8 ����ѹ/����
 * ͨ����ͨ�� ID Ϊ�豸��Ϣ�е� start_id ~ start_id + 7��
 *
 * �û������ֶ�ͨ�� aw_sensor_data_get() ��ȡͨ�����ݣ�Ҳ����ʹ�� 
 * aw_sensor_attr_set()���ò���Ƶ�ʣ��Ƽ��Ĳ���Ƶ��Ϊ 50Hz������ TPS08U ����ͨ��
 * ������һ����µģ��������ò���Ƶ��ʱ��ͨ�����������ԡ������ò���Ƶ�ʺ�ϵͳ
 * ���Զ����ղ���Ƶ�ʻ�ȡ����������ʹ�ܵ�ͨ���Ĳ���ֵ���������ڲ������С����û�
 * ����������׼��������������ÿ�����ݲ��������󣬶��������û��ص��������û�����
 * �ص�������ʹ�� aw_sensor_data_get() ��ȡͨ�����ݡ�
 *
 * ���� TPS08U ����֧�ִ������ܣ������е�����׼���������������޴����Լ�������
 * ������ͨ�����ģ��ʵ�ֵģ�������Ҫ���ò���Ƶ�ʴ��� 0 �����úõĴ����ص�����
 * �Żᱻ�����ص���
 *
 * ����Ҫע����ǣ�������Ƶ�����ô��� 0 ��ʱ��ϵͳ���Զ���ȡ������ֵ����ʱʹ��
 * �����ӿڲ���TPS08U �᷵�ش��� -AW_EBUSY�����Խ����û����������Ҫ���õ�ѡ��֮
 * �������ò���Ƶ�ʡ�
 *
 * \internal
 * \par modification history
 * - 1.00 18-05-03  pea, first implementation
 * \endinternal
 */

#ifndef __AWBL_TPS08U_H
#define __AWBL_TPS08U_H

#include "awbus_lite.h"
#include "aw_spi.h"
#include "aw_adc.h"
#include "aw_types.h"
#include "aw_delayed_work.h"
#include "awbl_sensor.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief TPS08U �豸���� */
#define    AWBL_TPS08U_NAME    "awbl_tps08u"

/** \brief TPS08U �豸��Ϣ */
struct awbl_tps08u_devinfo {
    int      start_id;    /**< \brief ��������ʼ ID */
    uint32_t sclk_freq;   /**< \brief SPI ʱ��Ƶ�� */
    int      cs_pin;      /**< \brief SPI Ƭѡ���ź� */
    
    /** \brief ͨ��ģʽ��ÿλ��ʾһ��ͨ����0 Ϊ��ѹ��1 Ϊ���� */
    uint8_t  chan_mode;   
};

/** \brief TPS08U �豸ʵ�� */
struct awbl_tps08u_dev {
    struct awbl_dev         dev;             /**< \brief AWBus �豸ʵ���ṹ�� */

    awbl_sensor_service_t   sensor_service;     /**< \brief ����������  */
    uint32_t                sampling_rate;      /**< \brief ����������Ƶ�� */
    int32_t                 offset[8];          /**< \brief ������ƫ��ֵ */
    int32_t                 threshold_lower[8]; /**< \brief ������������ */
    int32_t                 threshold_upper[8]; /**< \brief ������������ */
    int32_t                 threshold_slope[8]; /**< \brief �������������� */
    int32_t                 duration_data[8];   /**< \brief �������ݳ������� */
    int32_t                 duration_slope[8];  /**< \brief ������������ */
    int32_t                 threshold_type[8];  /**< \brief ��ǰ���޴������� */
    uint32_t                threshold_cnt[8];   /**< \brief ���޴������� */
    uint32_t                slope_cnt[8];       /**< \brief б�ʴ������� */
    uint32_t                flags[8];           /**< \brief ������־ */
    
    /** \brief ����ʱִ�еĻص�������ָ�� */
    aw_sensor_trigger_cb_t  pfn_cb[8];  
    void                   *p_arg[8];           /**< \brief �ص������û����� */
    
    /** \brief ����ʹ��λ��ÿλ��Ӧһ��ͨ�� */
    uint8_t                 trigger;    

    struct aw_delayed_work  sampling_work;     /**< \brief �����������ӳ���ҵ */

    aw_spi_device_t         spi_dev;           /**< \brief SPI ���豸�����ṹ */
    aw_spi_message_t        msg[2];            /**< \brief SPI ��Ϣ */
    aw_spi_transfer_t       trans[2];          /**< \brief SPI ���� */

    volatile aw_bool_t is_busy;          /**< \brief �豸��ǰ�Ƿ�æ */
    volatile uint8_t   spi_state;        /**< \brief SPI ״̬ */
    uint8_t            reg_addr;         /**< \brief ��ǰ�����ļĴ�����ַ */
    void              *p_buf;            /**< \brief ��ǰ�����Ļ�������ַ */
    aw_bool_t          is_read;          /**< \brief ��ǰ�����Ƿ�Ϊ������ */
    uint8_t            cmd;              /**< \brief ��ǰ����� CMD */
    uint8_t            current_chan;     /**< \brief �豸��ǰת����ͨ�� */
    uint8_t            read_state;       /**< \brief ��ȡ״̬ */
    uint8_t            reg_state;        /**< \brief �����ȡ��״̬�Ĵ��� */
    uint8_t            reg_chan_enable;  /**< \brief �����ȡ��ͨ��ʹ�ܼĴ��� */
    uint8_t            reg_chan_mode;    /**< \brief �����ȡ��ͨ��ģʽ�Ĵ��� */
    uint32_t           reg_data[8];      /**< \brief ��ǰ�ɼ�����ͨ������ */
    uint32_t           reg_data_last[8]  /**< \brief ��һ�βɼ�����ͨ������ */
};

#define AWBL_TPS08U_MODE_CH1_2_V  0         /**< \breif ͨ�� 1~2 ����Ϊ��ѹͨ�� */
#define AWBL_TPS08U_MODE_CH1_2_C  AW_BIT(0) /**< \breif ͨ�� 1~2 ����Ϊ����ͨ�� */
#define AWBL_TPS08U_MODE_CH3_4_V  0         /**< \breif ͨ�� 3~4 ����Ϊ��ѹͨ�� */
#define AWBL_TPS08U_MODE_CH3_4_C  AW_BIT(1) /**< \breif ͨ�� 3~4 ����Ϊ����ͨ�� */
#define AWBL_TPS08U_MODE_CH5_6_V  0         /**< \breif ͨ�� 5~6 ����Ϊ��ѹͨ�� */
#define AWBL_TPS08U_MODE_CH5_6_C  AW_BIT(2) /**< \breif ͨ�� 5~6 ����Ϊ����ͨ�� */
#define AWBL_TPS08U_MODE_CH7_8_V  0         /**< \breif ͨ�� 7~8 ����Ϊ��ѹͨ�� */
#define AWBL_TPS08U_MODE_CH7_8_C  AW_BIT(3) /**< \breif ͨ�� 7~8 ����Ϊ����ͨ�� */

#define AWBL_TPS08U_STATE_ADDR_ERROR   AW_BIT(0) /**< \breif �Ĵ�����ַ���� */
#define AWBL_TPS08U_STATE_UNABLE_WRITE AW_BIT(1) /**< \breif �Ĵ�������д�� */
#define AWBL_TPS08U_STATE_WRITE_FAILED AW_BIT(2) /**< \breif �Ĵ���д��ʧ�� */
#define AWBL_TPS08U_STATE_SPI_ERROR    AW_BIT(3) /**< \breif SPI ״̬�쳣 */
#define AWBL_TPS08U_STATE_ADC_ERROR    AW_BIT(4) /**< \breif ADC �쳣 */
#define AWBL_TPS08U_STATE_DATA_ERROR   AW_BIT(5) /**< \breif д�������쳣 */
#define AWBL_TPS08U_STATE_DATA_UPDATE  AW_BIT(6) /**< \breif ͨ�����ݸ��±�־ */

/**
 * \brief ����� TPS08U ��ͨ���Ƿ�����
 *
 * \param[in] id �豸��
 *
 * \retval  AW_OK     ͨ�ųɹ�
 * \retval -AW_ENODEV ͨ��ʧ��
 */
aw_err_t awbl_tps08u_detect (int id);

/**
 * \brief ��ȡ TPS08U ʹ�ܵ�ͨ��
 *
 * \param[in]  id            �豸��
 * \param[in]  chan          ͨ���ţ�Ϊ 1~8 ʱ��ȡ��Ӧͨ��״̬��Ϊ 0 ʱ��ȡ����
 *                           ͨ��ʹ��״̬
 * \param[out] p_chan_enable ָ��洢��ȡ����ʹ�ܵ�ͨ����ָ�룬
 *                           ��ͨ����Ϊ 1~8 ʱ�� 0 ��ʾ��ͨ��ʧ�ܣ�1 ��ʾ��ͨ��ʹ
 *                           �ܣ�
 *                           ��ͨ��������Ϊ 0 ʱ����ȡ����ͨ��״̬��ÿһλָʾһ
 *                           ��ͨ����״̬��Bit0 ��ʾͨ�� 1��Bit1 ��ʾͨ�� 2��
 *                           Bit2 ��ʾͨ�� 3���Դ����ƣ���ӦλΪ 0 ʱ��ʾ��ͨ��
 *                           ʧ�ܣ�Ϊ 1 ʱ��ʾ��ͨ��ʹ��
 *
 * \retval  AW_OK ��ȡ�ɹ�
 * \retval   <0   ��ȡʧ��
 */
aw_err_t awbl_tps08u_chan_enable_get (int      id,
                                      uint8_t  chan,
                                      uint8_t *p_chan_enable);

/**
 * \brief ���� TPS08U ʹ�ܵ�ͨ��
 *
 * \param[in] id          �豸��
 * \param[in] chan        ͨ���ţ�Ϊ 1~8 ʱ���ö�Ӧͨ��״̬��Ϊ 0 ʱ��������ͨ��
 *                        ʹ��״̬
 * \param[in] chan_enable �����õ�ͨ��ʹ�����ã�
 *                        ��ͨ����Ϊ 1~8 ʱ��0 ��ʾ��ͨ��ʧ�ܣ�1 ��ʾ��ͨ��ʹ�ܣ�
 *                        ��ͨ��������Ϊ 0 ʱ����������ͨ��״̬��ÿһλָʾһ��
 *                        ͨ����״̬��Bit0 ��ʾͨ�� 1��Bit1 ��ʾͨ�� 2��Bit2 ��
 *                        ʾͨ�� 3���Դ����ƣ���ӦλΪ 0 ʱ��ʾ��ͨ��ʧ�ܣ�Ϊ 1 
 *                        ʱ��ʾ��ͨ��ʹ��
 *
 * \retval  AW_OK ���óɹ�
 * \retval   <0   ����ʧ��
 *
 * \note �û�������������ÿ��ͨ����ʧ�ܻ���ʹ�ܣ�ʧ�ܵ�ͨ�������в�����
 *       ���Թرղ�ʹ�õ�ͨ�����Լӿ����ݸ�������
 */
aw_err_t awbl_tps08u_chan_enable_set (int id, uint8_t chan, uint8_t chan_enable);

/**
 * \brief ��ȡ TPS08U ͨ��ģʽ
 *
 * \param[in]  id          �豸��
 * \param[in]  chan        ͨ���ţ�Ϊ 1~8 ʱ��ȡ��Ӧͨ��״̬��Ϊ 0 ʱ��ȡ����ͨ
 *                         ��ģʽ
 * \param[out] p_chan_mode ָ��洢��ȡ����ͨ��ģʽ��ָ�룬
 *                         ��ͨ����Ϊ 1~8 ʱ��0 ��ʾ��ͨ��Ϊ��ѹͨ����1 ��ʾ��ͨ
 *                         ��Ϊ����ͨ����
 *                         ��ͨ��������Ϊ 0 ʱ��ÿһλ���� 2 ��ͨ����
 *                         Bit0 ��ʾͨ�� 1 ��ͨ�� 2��Bit1 ��ʾͨ�� 3 ��ͨ�� 4��
 *                         Bit2 ��ʾͨ�� 5 ��ͨ�� 6���Դ����ƣ�
 *                         ��ӦλΪ 0 ʱ��ʾ��ͨ��Ϊ��ѹͨ����Ϊ 1 ʱ��ʾ��ͨ��
 *                         Ϊ����ͨ��
 *
 * \retval  AW_OK ��ȡ�ɹ�
 * \retval   <0   ��ȡʧ��
 */
aw_err_t awbl_tps08u_chan_mode_get (int id, uint8_t  chan, uint8_t *p_chan_mode);

/**
 * \brief ���� TPS08U ͨ��ģʽ
 *
 * \param[in] id        �豸��
 * \param[in] chan      ͨ���ţ�Ϊ 1~8 ʱ���ö�Ӧͨ��״̬��Ϊ 0 ʱ��������ͨ��ģ
 *                      ʽ
 * \param[in] chan_mode �����õ�ͨ��ģʽ��
 *                      ��ͨ����Ϊ 1~8 ʱ��0 ��ʾ��ͨ��Ϊ��ѹͨ����1 ��ʾ��ͨ��
 *                      Ϊ����ͨ����
 *                      ��ͨ��������Ϊ 0 ʱ��ÿһλ���� 2 ��ͨ����
 *                      Bit0 ��ʾͨ�� 1 ��ͨ�� 2��Bit1 ��ʾͨ�� 3 ��ͨ�� 4��
 *                      Bit2 ��ʾͨ�� 5 ��ͨ�� 6���Դ����ƣ�
 *                      ��ӦλΪ 0 ʱ��ʾ��ͨ��Ϊ��ѹͨ����Ϊ 1 ʱ��ʾ��ͨ��Ϊ
 *                      ����ͨ��
 *
 * \retval  AW_OK ���óɹ�
 * \retval   <0   ����ʧ��
 *
 * \note ���� 2 ��ͨ��Ϊһ�飬���Ըı䵥��ͨ��ʱ������ͬ���ͨ��ģʽҲ�ᱻ�ı�
 */
aw_err_t awbl_tps08u_chan_mode_set (int id, uint8_t chan, uint8_t chan_mode);

/**
 * \brief ��ȡ TPS08U ͨ������
 *
 * \param[in]  id          �豸��
 * \param[in]  chan        ͨ���ţ�Ϊ 1~8 ʱ��ȡ��Ӧͨ�����ݣ�Ϊ 0 ʱ��ȡ����ͨ
 *                         ������
 * \param[out] p_chan_data ָ��洢��ȡ����ͨ�����ݵ�ָ�룬
 *                         ��ͨ����Ϊ 1~8 ʱ����������СӦΪ 1 �������ȵĴ�С��
 *                         ��ͨ��������Ϊ 0 ʱ����������СӦΪ 8 �������ȵĴ�С��
 *
 * \retval  AW_OK ��ȡ�ɹ�
 * \retval   <0   ��ȡʧ��
 *
 * \note ��ȡ����ͨ�����ݵĵ�λ��ͨ��ģʽ������
 *       ���Ϊ��ѹͨ������λΪ V����ΧΪ 0~5V��
 *       ���Ϊ����ͨ������λΪ mA����ΧΪ 4~20mA
 */
aw_err_t awbl_tps08u_chan_data_get (int id, uint8_t chan, uint32_t *p_chan_data);

/**
 * \brief ��ȡ TPS08U ״̬
 *
 * \param[in]  id      �豸��
 * \param[out] p_state ָ��洢��ȡ���� TPS08U ״̬��ָ�룬ÿһλָʾһ��״̬��
 *                     �ο� AWBL_TPS08U_STATE_*���ɣ���
 *                     AWBL_TPS08U_STATE_DATA_UPDATE
 *                     ��ʾͨ�������Ѹ���
 *
 * \retval  AW_OK ��ȡ�ɹ�
 * \retval   <0   ��ȡʧ��
 */
aw_err_t awbl_tps08u_state_get (int id, uint8_t *p_state);

/**
 * \brief ��λ TPS08U
 *
 * \param[in] id �豸��
 *
 * \retval  AW_OK ��λ�ɹ�
 * \retval   <0   ��λʧ��
 */
aw_err_t awbl_tps08u_reset (int id);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_TPS08U_H */

/* end of file */
