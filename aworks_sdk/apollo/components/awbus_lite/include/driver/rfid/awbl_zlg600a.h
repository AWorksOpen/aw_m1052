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
 * \brief AWBus ZLG600A �ӿ�ͷ�ļ�
 *
 * \internal
 * \par modification history:
 * - 1.00 16-05-03  win, first implementation
 * \endinternal
 */

#ifndef __AWBL_ZLG600A_H
#define __AWBL_ZLG600A_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_sem.h"
#include "aw_ioctl_generic.h"
#include "awbus_lite.h"

/**
 * \brief ZLG600A������������
 */
typedef struct awbl_zlg600a_info_buf_desc {
    uint8_t   *p_buf;
    uint16_t   len;
} awbl_zlg600a_info_buf_desc_t;

/** \brief ֡��Ϣ */
typedef struct awbl_zlg600a_frame_base {
    uint8_t                    frame_seq;   /**< \brief ����֡�ţ���4λ��Ч�� */
    uint8_t                    cmd_class;
    uint16_t                   cmd_status;
    uint16_t                   info_length;
    awbl_zlg600a_info_buf_desc_t *p_info_desc; /**< \brief ��Ϣ�����ṹ��ָ�� */
    uint8_t                       buf_num;     /**< \brief ��Ϣ�������ĸ���   */
} awbl_zlg600a_frame_base_t;

/**
 * \brief AWBus ZLG600A ����ʵ���������Ϣ
 */
struct awbl_zlg600a_servinfo {

    /** \brief ZLG600A �豸��� */
    int dev_id;
};

/*
 * \name device ioctl function codes
 *       pfn_dev_ioctl() params 'request' use
 * @{
 */

/** \brief ioctl base */
#define AWBL_ZLG600A_DEV_IOCTL_BASE              (0xD0)

/** \brief get current card status */
#define AWBL_ZLG600A_DEV_IOCTL_BAUD_SET          \
     __AW_IOW(AWBL_ZLG600A_DEV_IOCTL_BASE, 1, int)

/** \brief get current card status */
#define AWBL_ZLG600A_DEV_IOCTL_ADDR_SET          \
    __AW_IOW(AWBL_ZLG600A_DEV_IOCTL_BASE, 2, int)

/** \brief get current card status */
#define AWBL_ZLG600A_DEV_IOCTL_FRAME_FMT_SET     \
    __AW_IOW(AWBL_ZLG600A_DEV_IOCTL_BASE, 3, int)

/** @} */

/**
 * \brief AWBus ZLG600A ������
 */
struct awbl_zlg600a_servopts {

    /** \brief ֡���ͺ���         */
    aw_err_t (*pfn_frame_send) (void                      *p_cookie, 
                                awbl_zlg600a_frame_base_t *p_frame);

    /** \brief ֡���պ���         */
    aw_err_t (*pfn_frame_recv) (void                      *p_cookie, 
                                awbl_zlg600a_frame_base_t *p_frame);
    /** \brief �豸�����ຯ�� */
    aw_err_t (*pfn_dev_ioctl) (void *p_cookie, int request, void *p_arg);
};

/**
 * \brief ZLG600A����ṹ��
 */
typedef struct awbl_zlg600a_service {

    /** \brief ָ����һ�� ZLG600A ���� */
    struct awbl_zlg600a_service *p_next;

    /** \brief ZLG600A ����������Ϣ */
    const struct awbl_zlg600a_servinfo *p_servinfo;

    /** \brief ZLG600A �������غ��� */
    const struct awbl_zlg600a_servopts *p_servopts;

    void    *p_cookie;
    uint8_t  frame_seq;    /**< \brief ��¼���ţ���4λ��Ч�� */

    AW_MUTEX_DECL(devlock);  /**< \brief �豸�� */

} awbl_zlg600a_service_t;

/**
 * \brief AWBus ZLG600A �ӿ�ģ���ʼ��
 *
 * \attention ������Ӧ���� awbl_dev_connect()֮�����
 */
void awbl_zlg600a_init (void);

/**
 * \name ZLG600A��ͨ��ģʽ
 * @{
 */

#define AWBL_ZLG600A_MODE_I2C         1            /**< \brief I2Cģʽ       */
#define AWBL_ZLG600A_MODE_UART        2            /**< \brief ����ģʽ      */
#define AWBL_ZLG600A_MODE_AUTO_CHECK  0            /**< \brief �Զ����ģʽ */

/** @} */

/**
 * \name ZLG600A��֡��ʽ
 * @{
 */

#define AWBL_ZLG600A_FRAME_FMT_OLD  0             /**< \brief  ��֡��ʽ      */
#define AWBL_ZLG600A_FRAME_FMT_NEW  1             /**< \brief  ��֡��ʽ      */

 /** @} */

/**
 * \name ֧�ֵĲ�����
 * @{
 */

#define  AWBL_ZLG600A_BAUDRATE_9600      9600     /**< \brief ������ 9600   */
#define  AWBL_ZLG600A_BAUDRATE_19200     19200    /**< \brief ������ 19200  */
#define  AWBL_ZLG600A_BAUDRATE_28800     28800    /**< \brief ������ 28800  */
#define  AWBL_ZLG600A_BAUDRATE_38400     38400    /**< \brief ������ 38400  */
#define  AWBL_ZLG600A_BAUDRATE_57600     57600    /**< \brief ������ 57600  */
#define  AWBL_ZLG600A_BAUDRATE_115200    115200   /**< \brief ������ 115200 */
#define  AWBL_ZLG600A_BAUDRATE_172800    172800   /**< \brief ������ 172800 */
#define  AWBL_ZLG600A_BAUDRATE_230400    230400   /**< \brief ������ 230400 */

/** @}*/

/**
 * \brief ��ZLG600A�Ĳ�Ʒ���汾��Ϣ���磺��ZLG600A V1.00��
 *
 * \param[in] id     : �豸ID��
 * \param[in] p_info : ����װ�ػ�ȡ��Ϣ�Ļ���������СӦΪ 20 �ֽ�
 *
 * \retval AW_OK      : ��ȡ��Ϣ�ɹ�
 * \retval -AW_EINVAL : ��ȡ��Ϣʧ�ܣ���������
 * \retval -AW_EIO    : ��ȡ��Ϣʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_info_get (int id, uint8_t *p_info);

/**
 * \brief ����IC���ӿڣ�ִ�гɹ�������IC���ӿڣ�������Ĭ��֧��TypeA��
 *
 * \param[in] id      : �豸ID��
 *
 * \retval AW_OK      : ���óɹ�
 * \retval -AW_EINVAL : ����ʧ�ܣ���������
 * \retval -AW_EIO    : ����ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_ic_port_config (int id);

/**
 * \brief �ر�IC���ӿ�
 *
 * \param[in] id  : �豸ID��
 *
 * \retval AW_OK      : �رճɹ�
 * \retval -AW_EINVAL : �ر�ʧ�ܣ���������
 * \retval -AW_EIO    : �ر�ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_ic_port_close (int id);

/**
 * \name IC���ӿڵ�Э�飨����ģʽ��
 * @{
 */
#define  AWBL_ZLG600A_IC_ISOTYPE_A      0x00    /**< \brief ISO14443-3A  */
#define  AWBL_ZLG600A_IC_ISOTYPE_B      0x04    /**< \brief ISO14443-3B  */
/** @}*/

/**
 * \brief ����IC���ӿ�Э�飨����ģʽ��TypeA ���� TypeB
 *
 * \param[in] id      : �豸ID��
 * \param[in] isotype : IC���ӿڵ�Э�����ͣ�ʹ�����к� ��
 *                          - #AWBL_ZLG600A_IC_ISOTYPE_A
 *                          - #AWBL_ZLG600A_IC_ISOTYPE_B
 *
 * \retval AW_OK      : ���óɹ�
 * \retval -AW_EINVAL : ����ʧ�ܣ���������
 * \retval -AW_EIO    : ����ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_ic_isotype_set (int id, uint8_t isotype);

/**
 * \name IC���ܳ�����
 * @{
 */

#define  AWBL_ZLG600A_IC_KEY_TYPE_A      0x60    /**< \brief ���� A  */
#define  AWBL_ZLG600A_IC_KEY_TYPE_B      0x61    /**< \brief ���� B  */

/** @}*/

/**
 * \brief װ��IC����Կ
 *
 * ���������Կ������ģ���ڲ���ģ���������Կ����ʧ��ZLG600Aģ�鹲�ܱ���
 * A��Կ16�顢B��Կ16�顣
 *
 * \param[in] id         : �豸ID��
 * \param[in] key_type   : ��Կ���ͣ�����ʹ������ֵ��
 *                          - #AWBL_ZLG600A_IC_KEY_TYPE_A
 *                          - #AWBL_ZLG600A_IC_KEY_TYPE_B
 * \param[in] key_sec    : ��Կ��������ţ����ɱ���16����Կ�����ţ�0 ~ 15
 * \param[in] p_key      : ��Կ������
 * \param[in] key_length : ��Կ�ĳ��ȣ�ֻ��Ϊ6��6�ֽ���Կ����16��16�ֽ���Կ��
 *
 * \retval AW_OK      : װ����Կ�ɹ�
 * \retval -AW_EINVAL : װ��ʧ�ܣ���������
 * \retval -AW_EIO    : װ��ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_ic_key_load (int      id,
                                   uint8_t  key_type,
                                   uint8_t  key_sec,
                                   uint8_t *p_key,
                                   uint8_t  key_length);

/**
 * \brief ����IC���ӿڵļĴ���ֵ
 *
 * ��������ģ���϶�д��оƬ�ڲ��ļĴ���ֵ��ͨ�����������ʵ�ֺܶ���������
 * ������ɵĹ�����
 *
 * \param[in] id         : �豸ID��
 * \param[in] reg_addr   : �Ĵ�����ַ����Χ��0x00 ~ 0x3F
 * \param[in] reg_val    : ���õļĴ���ֵ
 *
 * \retval AW_OK      : ���óɹ�
 * \retval -AW_EINVAL : ����ʧ�ܣ���������
 * \retval -AW_EIO    : ����ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_ic_reg_set (int id, uint8_t reg_addr, uint8_t reg_val);

/**
 * \brief ��ȡIC���ӿڵļĴ���ֵ
 *
 * ��������������ģ���϶�д��оƬ�ڲ��ļĴ���ֵ��
 * ͨ�����������ʵ�ֺܶ������������ɵĹ�����
 *
 * \param[in]  id         : �豸ID��
 * \param[in]  reg_addr   : �Ĵ�����ַ����Χ��0x00 ~ 0x3F
 * \param[out] p_val      : ��ȡ�Ĵ���ֵ��ָ��
 *
 * \retval AW_OK      : ��ȡ�ɹ�
 * \retval -AW_EINVAL : ��ȡʧ�ܣ���������
 * \retval -AW_EIO    : ��ȡʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_ic_reg_get (int id, uint8_t reg_addr, uint8_t *p_val);

/**
 * \brief ���ò�����
 *
 * ������UARTͨ�Ź����иı�ͨ�ŵĲ����ʣ�����������ֵ������
 *
 * \param[in] id            : �豸ID��
 * \param[in] baudrate_flag : �����ʱ�־������֧�����еĲ����ʣ�Ӧʹ�ú�
 *                            AWBL_ZLG600A_BAUDRATE_* �������ò�����Ϊ115200��
 *                            ��ʹ�ú� #AWBL_ZLG600A_BAUDRATE_115200
 *
 * \retval AW_OK      : ���óɹ�
 * \retval -AW_EINVAL : ����ʧ�ܣ���������
 * \retval -AW_EIO    : ����ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_baudrate_set (int id, uint32_t baudrate_flag);

/**
 * \name ����������ʽ
 * @{
 */

#define  AWBL_ZLG600A_ANT_TX1_ONLY      1   /**< \brief ��TX1��������         */
#define  AWBL_ZLG600A_ANT_TX2_ONLY      2   /**< \brief ��TX2��������         */
#define  AWBL_ZLG600A_ANT_TX1_AND_TX2   3   /**< \brief TX1��TX2ͬʱ��������  */
#define  AWBL_ZLG600A_ANT_NONE          0   /**< \brief ͬʱ�ر�TX1��TX2      */

/** @}*/

/**
 * \brief ��������������ʽ����������򿪡��ر�ĳ��������������
 *
 * \param[in] id       : �豸ID��
 * \param[in] ant_mode : ʹ�ú� AWBL_ZLG600A_ANT_*����#AWBL_ZLG600A_ANT_TX1_ONLY
 *
 * \retval AW_OK      : ���óɹ�
 * \retval -AW_EINVAL : ����ʧ�ܣ���������
 * \retval -AW_EIO    : ����ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_ant_mode_set (int id, uint8_t ant_mode);

/**
 * \brief ����֡��ʽ�������л��¡���֡��ʽ�����óɹ�����粻��ʧ
 *
 * \param[in] id          : �豸ID��
 * \param[in] frame_type  : ֡��ʽ���־����ʹ�����к꣺
 *                          - #AWBL_ZLG600A_FRAME_FMT_NEW
 *                          - #AWBL_ZLG600A_FRAME_FMT_OLD
 *
 * \retval AW_OK      : ���óɹ�
 * \retval -AW_EINVAL : ����ʧ�ܣ���������
 * \retval -AW_EIO    : ����ʧ�ܣ�����ͨ�ų���
 *
 * \note �ڴ���ͨ�ŷ�ʽʱ�������¾�֡��ʽ����ԭ����֡��ʽ���سɹ���Ϣ������I2C
 *       ͨ�ŷ�ʽʱ���յ��������¾�֡��ʽ������󣬼�Ĭ�Ͻ��������ú�֡��ʽ��
 *       �����سɹ���Ϣ����һ����ģ��ͨ����Ҫʹ�����ú��֡��ʽ��ģ��ͨ�š�
 *       ͬʱΪ�˱�֤I2Cͨ�ŷ�ʽ�£�֡��ʽ���óɹ�����һ����ģ��ͨ����Ҫ��ʱ����
 *       125ms
 */
aw_err_t awbl_zlg600a_frame_fmt_set (int id, uint8_t frame_type);

/**
 * \name ����ģʽ
 * @{
 */

#define AWBL_ZLG600A_MODE_MASTER (1 << 4) /**< \brief ����ģʽ���������ܣ� */
#define AWBL_ZLG600A_MODE_SLAVE  (0 << 4) /**< \brief �ӻ�ģʽ��Ĭ�ϣ�     */

/** @}*/

/**
 * \brief �����豸����ģʽ�ʹӻ���ַ
 *
 * \param[in] id         : �豸ID��
 * \param[in] mode       : ģ��Ĺ���ģʽ������ʹ�����к�ֵ��
 *                          - #AWBL_ZLG600A_MODE_MASTER      ����ģʽ
 *                          - #AWBL_ZLG600A_MODE_SLAVE       �ӻ�ģʽ ��Ĭ�ϣ�
 *                          - #AWBL_ZLG600A_MODE_AUTO_CHECK  �Զ����ģʽ
 *                          - #AWBL_ZLG600A_MODE_I2C         I2Cģʽ
 *                          - #AWBL_ZLG600A_MODE_UART        UARTģʽ
 *
 * \param[in] new_addr   : ���õ�ZLG600A�ӻ���ַ�����һλΪ0����Чֵ0x02 ~ 0xFE
 *
 * \retval AW_OK      : ���óɹ�,���óɹ��󣬺���Ӧ��ʹ���µĵ�ַͨ��
 * \retval -AW_EINVAL : ����ʧ�ܣ���������
 * \retval -AW_EIO    : ����ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_mode_addr_set (int id, uint8_t mode, uint8_t new_addr);

/**
 * \brief ��ȡ�豸����ģʽ�ʹӻ���ַ
 *
 * \param[in]  id         : �豸ID��
 * \param[out] p_mode     : ���ڻ�ȡģ�鹤��ģʽ��ָ�룬��ȡ��ֵ��λ�������£�
 *                          - bit7 ~ 4��0,�ӻ�ģʽ; 1,�Զ���⿨Ƭģʽ������ģʽ��
 *                          - bit3 ~ 0��0,�Զ����ģʽ; 1,I2Cͨ��ģʽ; 2,UARTģʽ
 * \param[out] p_addr     : ���ڻ�ȡ��ǰ��ZLG600A�Ĵӻ���ַ��ָ��
 *
 * \retval AW_OK      : ��ȡ�ɹ�
 * \retval -AW_EINVAL : ��ȡʧ�ܣ���������
 * \retval -AW_EIO    : ��ȡʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_mode_addr_get (int id, uint8_t *p_mode, uint8_t *p_addr);

/**
 * \brief װ���û���Կ��ģ�������ṩ��2��16�ֽڵĴ洢�ռ����ڱ����û���Կ��
 *
 * \param[in] id         : �豸ID��
 * \param[in] key_sec    : ��Կ��������ţ����ɱ���2���û���Կ�����ţ�0 ~ 1
 * \param[in] p_key      : ��Կ����������Կ�ĳ��ȹ̶�Ϊ16�ֽ�
 *
 * \retval AW_OK      : װ����Կ�ɹ�
 * \retval -AW_EINVAL : װ��ʧ�ܣ���������
 * \retval -AW_EIO    : װ��ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_user_key_load (int id, uint8_t key_sec, uint8_t *p_key);

/**
 * \brief дEEPROM��ģ���ڲ�ӵ��һ��256Byte��EEPROM
 *
 * \param[in] id          : �豸ID��
 * \param[in] eeprom_addr : EEPROM��Ƭ�ڵ�ַ��0 ~ 255
 * \param[in] p_buf       : д�����ݵĻ�����
 * \param[in] nbytes      : д�����ݵ��ֽ��������247�ֽ�
 *
 * \retval AW_OK      : д�����ݳɹ�
 * \retval -AW_EINVAL : д��ʧ�ܣ���������
 * \retval -AW_EIO    : д��ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_eeprom_write (int      id,
                                    uint8_t  eeprom_addr,
                                    uint8_t *p_buf,
                                    uint8_t  nbytes);

/**
 * \brief ��EEPROM��ģ���ڲ�ӵ��һ��256Byte��EEPROM
 *
 * \param[in]  id          : �豸ID��
 * \param[in]  eeprom_addr : EEPROM��Ƭ�ڵ�ַ��0 ~ 255
 * \param[out] p_buf       : �������ݵĻ�����
 * \param[in]  nbytes      : �������ݵ��ֽ��������249�ֽ�
 *
 * \retval AW_OK      : ��ȡ���ݳɹ�
 * \retval -AW_EINVAL : ��ȡʧ�ܣ���������
 * \retval -AW_EIO    : ��ȡʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_eeprom_read (int      id,
                                   uint8_t  eeprom_addr,
                                   uint8_t *p_buf,
                                   uint8_t  nbytes);

/**
 * \brief ��ȡ����оƬ��λ����
 *
 * \param[in]  id      : �豸ID��
 * \param[out] p_count : ���ظ�λ����
 *
 * \retval AW_OK      : ��ȡ���ݳɹ�
 * \retval -AW_EINVAL : ��ȡʧ�ܣ���������
 * \retval -AW_EIO    : ��ȡʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_ic_reset_count_get (int id, uint32_t *p_count);

/**
 * \name Mifare������ģʽ
 * @{
 */
 
/** \brief IDLEģʽ,������еĿ� */
#define AWBL_ZLG600A_MIFARE_CARD_REQ_IDLE 0x26 

/** \brief ALLģʽ,�������еĿ�  */
#define AWBL_ZLG600A_MIFARE_CARD_REQ_ALL  0x52 

/** @}*/

/**
 * \brief Mifare���������
 *
 *  �ú�����Ϊ�������������ֻҪ����ISO14443A��׼�Ŀ���Ӧ�ܷ�����Ӧ��
 *
 * \param[in] id          : �豸ID��
 * \param[in] req_mode    : ����ģʽ����ʹ�����к꣺
 *                           - #AWBL_ZLG600A_MIFARE_CARD_REQ_IDLE
 *                           - #AWBL_ZLG600A_MIFARE_CARD_REQ_ALL
 * \param[in] p_atq       : ��ȡ����Ӧ����Ϣ��ATQ����ָ�룬��ֵΪ16λ����ͬ��
 *                          ���ͷ��ص���Ϣ��ͬ,�������ͷ��ص�ATQ���£�
 *
 *            Mifare1 S50    | Mifare1 S70 | Mifare1 Light | Mifare0 UltraLight
 *       --------------------|-------------|---------------|-------------------
 *              0x0004       |    0x0002   |    0x0010     |      0x0044
 *       ----------------------------------------------------------------------
 *          Mifare3 DESFire  |   SHC1101   |    SHC1102    |      11RF32
 *       --------------------|-------------|---------------|-------------------
 *               0x0344      |    0x0004   |    0x3300     |      0x0004
 *
 * \retval AW_OK      : ����ɹ�
 * \retval -AW_EINVAL : ����ʧ�ܣ���������
 * \retval -AW_EIO    : ����ʧ�ܣ�����ͨ�ų���
 *
 * \note ���������ߺ󣬴���Ƶ���л�ȡ�������Ӷ��õ縴λ����λ�󿨴���IDLEģʽ��
 * ����������ģʽ����һ������ʱ����ʱ�Ŀ�������Ӧ������ĳһ�ſ��ɹ������˹���
 * ������Halt�����DeSelect������������Haltģʽ����ʱ�Ŀ�ֻ��ӦALL��0x52��
 * ģʽ�����󣬳��ǽ����뿪���߸�Ӧ�����ٽ��롣
 */
aw_err_t awbl_zlg600a_mifare_card_request (int       id,
                                           uint8_t   req_mode,
                                           uint16_t *p_atq);

/**
 * \name Mifare������ײ�ȼ�
 * @{
 */
#define AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_1  0x93  /**< \brief ��һ������ײ  */
#define AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_2  0x95  /**< \brief �ڶ�������ײ  */
#define AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_3  0x97  /**< \brief ����������ײ  */
/** @}*/

/**
 * \brief Mifare���ķ���ײ����
 *
 *  ��Ҫ�ɹ�ִ��һ�������������������ɹ������ܽ��з���ײ���������򷵻ش���
 *
 * \param[in] id             : �豸ID��
 * \param[in] anticoll_level : ����ײ�ȼ�����ʹ�����к꣺
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_1
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_2
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_3
 *
 * \param[in] know_uid : ��֪�Ŀ����к�
 * \param[in] nbit_cnt : λ������������֪�����к�uid��λ������ֵС��32��
 *                       - ��nbit_cntֵΪ0,�����û����֪���кţ����кŵ�����λ
 *                         ��Ҫ�ӱ�������á�
 *                       - ��nbit_cnt��Ϊ0�������к�������֪�����кŵ�ֵ����ʾ
 *                         ���кŵ�ǰnbit_cntλΪ��֪���к�����ʾǰnbit_cntλ��
 *                         ֵ������λ��Ҫ��������á�
 * \param[in] *p_uid   : ����ײ���ȡ���Ŀ����к�
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 *
 * \note  ����ISO14443A��׼�������кŶ���ȫ��Ψһ�ģ���������Ψһ�ԣ�����ʵ�ַ�
 * ��ײ���㷨�߼������������ſ�ͬʱ�����߸�Ӧ��������������ܹ��ҵ�һ�����кŽ�
 * ��Ŀ���������
 */
aw_err_t awbl_zlg600a_mifare_card_anticoll (int       id,
                                            uint8_t   anticoll_level,
                                            uint32_t  know_uid,
                                            uint8_t   nbit_cnt,
                                            uint32_t *p_uid);

/**
 * \brief Mifare����ѡ�����
 *
 *  ��Ҫ�ɹ�ִ��һ�η���ײ��������سɹ������ܽ��п�ѡ����������򷵻ش���
 *
 * \param[in] id             : �豸ID��
 * \param[in] anticoll_level : ����ײ�ȼ�����ʹ�����к꣺
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_1
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_2
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_3
 *
 * \param[in] know_uid : ��֪�Ŀ����к�
 * \param[in] nbit_cnt : λ������������֪�����к�uid��λ������ֵС��32��
 *                       - ��nbit_cntֵΪ0,�����û����֪���кţ����кŵ�����λ
 *                         ��Ҫ�ӱ�������á�
 *                       - ��nbit_cnt��Ϊ0�������к�������֪�����кŵ�ֵ����ʾ
 *                         ���кŵ�ǰnbit_cntλΪ��֪���к�����ʾǰnbit_cntλ��
 *                         ֵ������λ��Ҫ��������á�
 * \param[in] uid   : ǰһ������ײ������ȡ��UID
 * \param[in] p_sak : ���ص���Ϣ����bit2Ϊ1�������UID������
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 *
 * \note  �������кų��������֣�4�ֽڡ�7�ֽں�10�ֽڡ� 4�ֽڵ�ֻҪ��һ��ѡ�񼴿�
 * �õ����������кţ���Mifare1 S50/S70�ȣ�7�ֽڵ�Ҫ�ö���ѡ����ܵõ�����������
 * �ţ�ǰһ�����õ������кŵ�����ֽ�Ϊ������־0x88�������к���ֻ�к�3�ֽڿ��ã�
 * ��һ��ѡ���ܵõ�4�ֽ����кţ����߰�˳�����Ӽ�Ϊ7�ֽ����кţ���UltraLight
 * ��DesFire�ȣ�10�ֽڵ��Դ����ƣ�������δ���ִ��࿨��
 */
aw_err_t awbl_zlg600a_mifare_card_select (int       id,
                                          uint8_t   anticoll_level,
                                          uint32_t  uid,
                                          uint8_t  *p_sak);

/**
 * \brief Mifare���Ĺ��������ʹ��ѡ��Ŀ�����HALT״̬
 *
 *  ��HALT״̬�£���������Ӧ������������IDLEģʽ�����󣬳��ǽ�����λ���뿪���߸�
 *  Ӧ�����ٽ��롣��������Ӧ������������ALL����
 *
 * \param[in] id      : �豸ID��
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_mifare_card_halt (int id);


/**
 * \brief Mifare��E2��Կ��֤
 *
 *     ��ģ���ڲ��Ѵ������Կ�뿨����Կ������֤��ʹ�ø�����ǰӦ���á�װ��IC����Կ��
 * ��������Կ�ɹ�����ģ���ڡ����⣬��Ҫ��֤�Ŀ��������Ų�����ģ������Կ������ȡ�
 *
 * \param[in] id         : �豸ID��
 * \param[in] key_type   : ��Կ���ͣ�����ʹ������ֵ��
 *                          - #AWBL_ZLG600A_IC_KEY_TYPE_A
 *                          - #AWBL_ZLG600A_IC_KEY_TYPE_B
 * \param[in] p_uid      : �����кţ�4�ֽ�
 * \param[in] key_sec    : ��Կ���ţ�0 ~ 7
 * \param[in] nblock     : ��Ҫ��֤�Ŀ���ţ�ȡֵ��Χ�뿨�����йأ�
 *                         - S50��0 ~ 63
 *                         - S70��0 ~ 255
 *                         - PLUS CPU 2K��0 ~ 127
 *                         - PLUS CPU 4K��0 ~ 255
 *
 * \retval AW_OK      : ��֤�ɹ�
 * \retval -AW_EINVAL : ��֤ʧ�ܣ���������
 * \retval -AW_EIO    : ��֤ʧ�ܣ�����ͨ�ų���
 *
 * \note PLUS CPUϵ�еĿ��Ŀ�����4�ֽں�7�ֽ�֮�֣�����7�ֽڿ��ŵĿ���
 *       ֻ��Ҫ�����ŵĸ�4�ֽڣ��ȼ�2����ײ�õ��Ŀ��ţ���Ϊ��֤�Ŀ��ż��ɡ�
 */
aw_err_t awbl_zlg600a_mifare_card_e2_authent (int      id,
                                              uint8_t  key_type,
                                              uint8_t *p_uid,
                                              uint8_t  key_sec,
                                              uint8_t  nblock);

/**
 * \brief Mifare��ֱ����Կ��֤
 *
 *    �����������Ϊ�������ݣ�����ڴ�֮ǰ�����á�װ��IC����Կ���������ǰ��
 * ΪPLUS CPU���ĵȼ�2��ȼ�3�������������ֻ��6�ֽڣ���ģ���Զ������������
 * ����2�Σ�ȡǰ16�ֽ���Ϊ��ǰ��֤��Կ��
 *
 * \param[in] id         : �豸ID��
 * \param[in] key_type   : ��Կ���ͣ�����ʹ������ֵ��
 *                          - #AWBL_ZLG600A_IC_KEY_TYPE_A
 *                          - #AWBL_ZLG600A_IC_KEY_TYPE_B
 * \param[in] p_uid      : �����кţ�4�ֽ�
 * \param[in] p_key      : ��Կ������
 * \param[in] key_len    : ��Կ�ĳ��ȣ�ֻ��Ϊ6��6�ֽ���Կ����16��16�ֽ���Կ��
 * \param[in] nblock     : ��Ҫ��֤�Ŀ���ţ�ȡֵ��Χ�뿨�����йأ�
 *                         - S50��0 ~ 63
 *                         - S70��0 ~ 255
 *                         - PLUS CPU 2K��0 ~ 127
 *                         - PLUS CPU 4K��0 ~ 255
 *
 * \retval AW_OK      : ��֤�ɹ�
 * \retval -AW_EINVAL : ��֤ʧ�ܣ���������
 * \retval -AW_EIO    : ��֤ʧ�ܣ�����ͨ�ų���
 *
 * \note PLUS CPUϵ�еĿ��Ŀ�����4�ֽں�7�ֽ�֮�֣�����7�ֽڿ��ŵĿ���
 *       ֻ��Ҫ�����ŵĸ�4�ֽڣ��ȼ�2����ײ�õ��Ŀ��ţ���Ϊ��֤�Ŀ��ż��ɡ�
 */
aw_err_t awbl_zlg600a_mifare_card_direct_authent (int      id,
                                                  uint8_t  key_type,
                                                  uint8_t *p_uid,
                                                  uint8_t *p_key,
                                                  uint8_t  key_len,
                                                  uint8_t  nblock);

/**
 * \brief Mifare�����ݶ�ȡ
 *
 *     ����֤�ɹ�֮�󣬲��ܶ���Ӧ�Ŀ����ݣ�����֤�Ŀ�������ű�����ͬһ�������ڣ�
 * Mifare1���ӿ��0��ʼ��˳��ÿ4����1����������Ҫ��һ�ſ��еĶ���������в�������
 * ��ĳһ����������Ϻ󣬱������һ����������ܶ���һ������ֱ�ӽ�����֤�������
 * ���������ʼ����������PLUS CPU��������һ������������Կ�͵�ǰ��������Կ��ͬ��
 * ����Ҫ�ٴ���֤��Կ��ֱ�Ӷ����ɡ�
 *
 * \param[in] id      : �豸ID��
 * \param[in] nblock  : ��ȡ���ݵĿ��
 *                      - S50��0 ~ 63
 *                      - S70��0 ~ 255
 *                      - PLUS CPU 2K��0 ~ 127
 *                      - PLUS CPU 4K��0 ~ 255
 * \param[in] p_buf   : ��Ŷ�ȡ�����ݣ���СΪ 16
 *
 * \retval AW_OK      : ��ȡ�ɹ�
 * \retval -AW_EINVAL : ��ȡʧ�ܣ���������
 * \retval -AW_EIO    : ��ȡʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_mifare_card_read (int      id,
                                        uint8_t  nblock,
                                        uint8_t *p_buf);

/**
 * \brief Mifare��д����,д֮ǰ����ɹ�������Կ��֤��
 *
 *  �Կ���ĳһ�������֤�ɹ��󣬼��ɶ�ͬһ�����ĸ�������д������ֻҪ����������
 *  �������а���λ������β������飬���Ǹ��������Ψһ����������PLUS CPU���ȼ�
 *  2��3��AES��Կ����������λ���޸���Կ��
 *
 * \param[in] id         : �豸ID��
 * \param[in] nblock     : ��ȡ���ݵĿ��
 *                         - S50��0 ~ 63
 *                         - S70��0 ~ 255
 *                         - PLUS CPU 2K��0 ~ 127
 *                         - PLUS CPU 4K��0 ~ 255
 * \param[in] p_buf      : д�����ݻ���������С����Ϊ 16
 *
 * \retval AW_OK      : д��ɹ�
 * \retval -AW_EINVAL : д��ʧ�ܣ���������
 * \retval -AW_EIO    : д��ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_mifare_card_write (int      id,
                                         uint8_t  nblock,
                                         uint8_t *p_buf);

/**
 * \brief Mifare��д����
 *
 * \param[in] id         : �豸ID��
 * \param[in] nblock     : ��ȡ���ݵĿ��, 1 ~ 15
 * \param[in] p_buf      : д�����ݻ���������С����Ϊ 4
 *
 * \retval AW_OK      : д��ɹ�
 * \retval -AW_EINVAL : д��ʧ�ܣ���������
 * \retval -AW_EIO    : д��ʧ�ܣ�����ͨ�ų���
 *
 * ������ֻ��UltraLight����Ч����UltraLight�����ж�������Mifare1��һ����
 */
aw_err_t awbl_zlg600a_ultralight_card_write (int      id,
                                             uint8_t  nblock,
                                             uint8_t *p_buf);


/**
 * \name Mifare��ֵ����ģʽ
 * @{
 */

#define AWBL_ZLG600A_MIFARE_CARD_VALUE_ADD  0xC1  /**< \brief ��  */
#define AWBL_ZLG600A_MIFARE_CARD_VALUE_SUB  0xC0  /**< \brief ��  */

/** @}*/

/**
 * \brief Mifareֵ��������Mifare����ֵ����мӼ�����
 *
 * \param[in] id         : �豸ID��
 * \param[in] mode       : ֵ������ģʽ�������Ǽӻ����ʹ�����к꣺
 *                          - AWBL_ZLG600A_MIFARE_CARD_VALUE_ADD
 *                          - AWBL_ZLG600A_MIFARE_CARD_VALUE_SUB
 * \param[in] nblock     : ����ֵ�����Ŀ��
 *                            - S50��0 ~ 63
 *                            - S70��0 ~ 255
 *                            - PLUS CPU 2K��0 ~ 127
 *                            - PLUS CPU 4K��0 ~ 255
 * \param[in] ntransblk  : �����ţ�������ֵ��ŵĿ��
 * \param[in] value      : 4�ֽ��з�����
 *
 * \retval AW_OK      : д��ɹ�
 * \retval -AW_EINVAL : д��ʧ�ܣ���������
 * \retval -AW_EIO    : д��ʧ�ܣ�����ͨ�ų���
 *
 * \note Ҫ���д�������������ݱ���Ҫ��ֵ��ĸ�ʽ���ɲο�NXP������ĵ���������
 * ���봫������ͬ���򽫲�����Ľ��д��ԭ���Ŀ��ڣ���������봫���Ų���ͬ��
 * �򽫲�����Ľ��д�봫����ڣ����������ڵ����ݱ����ǣ�ԭ���ڵ�ֵ���䡣
 * ���ڵȼ�2��PLUS CPU����֧��ֵ��������ȼ�1��3֧�֡�
 */
aw_err_t awbl_zlg600a_mifare_card_val_operate (int     id,
                                               uint8_t mode,
                                               uint8_t nblock,
                                               uint8_t ntransblk,
                                               int32_t value);

/**
 * \brief Mifare����λ��ͨ�����ز��źŹر�ָ����ʱ�䣬�ٿ�����ʵ�ֿ�Ƭ��λ��
 *
 * \param[in] id         : �豸ID��
 * \param[in] time_ms    : �رյ�ʱ�䣨��λ:ms����0Ϊһֱ�ر�
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 *
 * \note �ú����������źŹر������룬��һֱ�رգ���ȵ�ִ��һ����������ʱ�򿪡�
 */
aw_err_t awbl_zlg600a_mifare_card_reset (int     id,
                                         uint8_t time_ms);

/**
 * \brief Mifare������ú������ڼ��Ƭ�������󡢷���ײ��ѡ�������������ϡ�
 *
 * \param[in] id         : �豸ID��
 * \param[in] req_mode   : ����ģʽ����ʹ�����к꣺
 *                           - #AWBL_ZLG600A_MIFARE_CARD_REQ_IDLE
 *                           - #AWBL_ZLG600A_MIFARE_CARD_REQ_ALL
 * \param[in] p_atq      : ����Ӧ��
 * \param[in] p_saq      : ���һ��ѡ��Ӧ��SAK
 * \param[in] p_len      : ���кŵ�ʵ�ʳ��ȣ�4�ֽڡ�7�ֽڻ�10�ֽ�
 * \raram[in] p_uid      : ���кŻ�����������Ӧ�������кų��ȱ���һ�£�
 *                         �粻ȷ����������ʹ����󳤶�10��
 * \pram[in]  uid_len    : p_uid ��ָ�������ĳ���
 *
 * \retval AW_OK      : д��ɹ�
 * \retval -AW_EINVAL : д��ʧ�ܣ���������
 * \retval -AW_EIO    : д��ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_mifare_card_active (int       id,
                                          uint8_t   req_mode,
                                          uint16_t *p_atq,
                                          uint8_t  *p_sak,
                                          uint8_t  *p_len,
                                          uint8_t  *p_uid,
                                          uint8_t   uid_len);

/**
 * \brief Mifareֵ����������ֵ���ֵ
 *
 * \param[in] id         : �豸ID��
 * \param[in] nblock     : ��ȡ���ݵ�ֵ���ַ
 * \param[in] value      : ���õ�ֵ
 *
 * \retval AW_OK      : ���óɹ�
 * \retval -AW_EINVAL : ����ʧ�ܣ���������
 * \retval -AW_EIO    : ����ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_mifare_card_val_set (int id, uint8_t nblock, int32_t value);

/**
 * \brief Mifareֵ��������ȡֵ���ֵ
 *
 * \param[in] id         : �豸ID��
 * \param[in] nblock     : ��ȡ���ݵ�ֵ���ַ
 * \param[in] p_value    : ��ȡֵ��ָ��
 *
 * \retval AW_OK      : ��ȡ�ɹ�
 * \retval -AW_EINVAL : ��ȡʧ�ܣ���������
 * \retval -AW_EIO    : ��ȡʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_mifare_card_val_get (int     id, 
                                           uint8_t nblock,
                                           int32_t *p_value);

/**
 * \brief �����
 *
 *     �ú�������ģ����չ���ܣ�����ģ����Ƭ�������ⳤ����ϵ����ݴ����������
 * NXP���Ƴ���NTAG213F������Ultralight Cϵ�п�Ƭ�����Ǹÿ�Ƭ�����������������
 * ��д��Կ�������ܡ��������Կ��֤��������ô��������亯����ʵ�֡�
 *
 * \param[in] id         : �豸ID��
 * \param[in] p_tx_buf   : �������ݵĻ�����
 * \param[in] tx_nbytes  : �����ֽ���
 * \param[in] p_rx_buf   : �������ݵĻ�����
 * \param[in] rx_nbytes  : �����ֽ���
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_mifare_card_cmd_trans (int      id,
                                             uint8_t *p_tx_buf,
                                             uint8_t  tx_nbytes,
                                             uint8_t *p_rx_buf,
                                             uint8_t  rx_nbytes);


/**
 * \brief ���ݽ���
 *
 * �������ö�д���뿨Ƭ�����ݽ�����ͨ���ú�������ʵ�ֶ�д���������й��ܡ�
 *
 * \param[in] id         : �豸ID��
 * \param[in] p_data_buf : ��Ҫ�������͵�����
 * \param[in] len        : �������ݵĳ���
 * \param[in] wtxm_crc   : b7~b2,wtxm��b1,RFU(0); b0,0,CRC��ֹ�ܣ�1,CRCʹ�ܡ�
 * \param[in] fwi        : ��ʱ�ȴ�ʱ�����, ��ʱʱ��=����0x01 << fwi*302us��
 * \param[in] p_rx_buf   : ���ݽ�����Ӧ֡��Ϣ
 * \param[in] buf_size   : ���ջ�Ӧ��Ϣ�Ļ�������С
 * \param[in] p_real_len : ���ڻ�ȡʵ�ʽ��յ���Ϣ���ȣ������������СС��ʵ����
 *                         Ϣ���ȣ�����ಿ�ֽ�����
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_mifare_card_data_exchange (int       id,
                                                 uint8_t  *p_data_buf,
                                                 uint8_t   len,
                                                 uint8_t   wtxm_crc,
                                                 uint8_t   fwi,
                                                 uint8_t  *p_rx_buf,
                                                 uint8_t   buf_size,
                                                 uint16_t *p_real_len);

/**
 * \name IC����λ��Ĳ�����
 *
 *      ע�⣬�ò����ʲ���������ZLG600A��ͨ�Ų����ʣ�����ָZLG600ģ����Ӵ�ʽIC
 *  ����ͨ�Ų����ʡ�
 *
 * @{
 */
#define  AWBL_ZLG600A_CICC_RST_BAUDRATE_9600   0x11  /**< \brief ������ 9600  */
#define  AWBL_ZLG600A_CICC_RST_BAUDRATE_38400  0x13  /**< \brief ������ 38400 */

/** @}*/

/**
 * \brief �Ӵ�ʽIC����λ���Զ�����PPS��
 *
 *     �ú������临λ��ִ�гɹ�����Զ�����IC���ĸ�λ��Ϣ��ִ��PPS���Ȼ����
 * ѡ��awbl_zlg600a_cicc_tpdu()����ʹ�õĴ���Э�飨T = 0 ��T = 1����
 *
 * \param[in]  p_dev         : ָ��ZLG600A�豸��ָ��
 * \param[in]  baudrate_flag : ��������IC��ͨ��ʹ�õĲ����ʣ�ʹ�����к꣺
 *                             - AWBL_ZLG600A_CICC_RST_BAUDRATE_9600
 *                             - AWBL_ZLG600A_CICC_RST_BAUDRATE_38400
 * \param[out] p_rx_buf      : ��Ÿ�λ��Ϣ�Ļ�����
 * \param[in]  rx_bufsize    : �������ĳ���
 * \param[out] p_real_len    : ��ȡ��λ��Ϣ���ȵ�ָ�룬������ջ������ĳ���С��
 *                             ʵ�ʳ��ȣ�����ಿ�ֶ�����
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_cicc_reset (int       id,
                                  uint8_t   baudrate_flag,
                                  uint8_t  *p_rx_buf,
                                  uint8_t   rx_bufsize,
                                  uint16_t *p_real_len);

/**
 * \brief �Ӵ�ʽIC������Э��
 *
 *     ��������ݽӴ�ʽIC���ĸ�λ��Ϣ���Զ�ѡ��T = 0��T = 1����Э�飬��������
 * ����Ҫʹ���߸�Ԥ�����������ڴ���APDU��������
 *
 * \param[in] id            : �豸ID��
 * \param[in] p_tx_buf      : ���ͻ�����
 * \param[in] tx_bufsize    : ���ͻ������ĳ��ȣ� 1 ~ 272
 * \param[in] p_rx_buf      : ���ջ�����
 * \param[in] rx_bufsize    : ���ջ������ĳ���
 * \param[in] p_real_len    : ���ڻ�ȡʵ�ʽ�����Ϣ���ȵ�ָ�룬������ջ�����
 *                            �ĳ���С��ʵ�ʳ��ȣ�����ಿ�ֶ�����
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_cicc_tpdu (int       id,
                                 uint8_t  *p_tx_buf,
                                 uint8_t   tx_bufsize,
                                 uint8_t  *p_rx_buf,
                                 uint8_t   rx_bufsize,
                                 uint16_t *p_real_len);

/**
 * \brief �Ӵ�ʽIC���临λ
 *
 *     �ú������临λ��ִ���˽Ӵ�ʽIC���ϵ�ʱ��ִ�гɹ�����Զ�����IC���ĸ�λ
 * ��Ϣ��ѡ��awbl_zlg600a_cicc_tpdu()����ʹ�õĴ���Э�飨T = 0 ��T = 1�����ú���
 * û���Զ�ִ��PPS�����Ҫ�û����ݸ�λ��Ϣ���ж��Ƿ�ʹ��awbl_zlg600a_cicc_pps()
 * ���޸�Э��Ͳ�����
 *
 * \param[in] id            : �豸ID��
 * \param[in] baudrate_flag : IC����λ��Ĳ����ʣ�ʹ�����к꣺
 *                             - AWBL_ZLG600A_CICC_RST_BAUDRATE_9600
 *                             - AWBL_ZLG600A_CICC_RST_BAUDRATE_38400
 * \param[in] p_rx_buf      : ��Ÿ�λ��Ϣ�Ļ�����
 * \param[in] rx_bufsize    : �������ĳ���
 * \param[in] p_real_len    : ��ȡ��λ��Ϣ���ȵ�ָ�룬������ջ������ĳ���С��
 *                            ʵ�ʳ��ȣ�����ಿ�ֶ�����
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_cicc_coldreset (int       id,
                                      uint8_t   baudrate_flag,
                                      uint8_t  *p_rx_buf,
                                      uint8_t   rx_bufsize,
                                      uint16_t *p_real_len);

/**
 * \brief �Ӵ�ʽIC���ȸ�λ
 *
 *    �ú������ȸ�λ��û��ִ���˽Ӵ�ʽIC���ϵ�ʱ��ִ�гɹ�����Զ�����IC����
 * ��λ��Ϣ��ѡ��awbl_zlg600a_cicc_tpdu()����ʹ�õĴ���Э�飨T = 0 ��T = 1����
 * �ȸ�λ���临λΨһ��������û��ִ��IC���ϵ��������Ҫ�û����ݸ�λ��Ϣ���ж�
 * �Ƿ�ʹ��awbl_zlg600a_cicc_pps()���޸�Э��Ͳ������ú���������IC��ʱ�Ӻ͵�Դ��
 * ��Ч������²���ִ�С�
 *
 * \param[in] id            : �豸ID��
 * \param[in] baudrate_flag : IC����λ��Ĳ����ʣ�ʹ�����к꣺
 *                             - AWBL_ZLG600A_CICC_RST_BAUDRATE_9600
 *                             - AWBL_ZLG600A_CICC_RST_BAUDRATE_38400
 * \param[in] p_rx_buf      : ��Ÿ�λ��Ϣ�Ļ�����
 * \param[in] rx_bufsize    : �������ĳ���
 * \param[in] p_real_len    : ��ȡ��λ��Ϣ���ȵ�ָ�룬������ջ������ĳ���С��
 *                            ʵ�ʳ��ȣ�����ಿ�ֶ�����
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_cicc_warmreset (int       id,
                                      uint8_t   baudrate_flag,
                                      uint8_t  *p_rx_buf,
                                      uint8_t   rx_bufsize,
                                      uint16_t *p_real_len);

/**
 * \brief �Ӵ�ʽIC��ͣ��رսӴ�ʽIC���ĵ�Դ��ʱ��
 *
 * \param[in] id      : �豸ID��
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_cicc_deactive (int id);

/**
 * \brief �Ӵ�ʽIC��Э��Ͳ���ѡ��
 *
 *     �ú������临λ���ȸ�λ֮���ұ�������ִ�У�Э��ģʽ����Ҫִ�У�ר��ģʽ
 * ����Ҫִ�У������ԽӴ�ʽIC�����˽⣬����ʹ��awbl_zlg600a_cicc_reset()���и�λ��
 *
 * \param[in] id         : �豸ID��
 * \param[in] p_pps      : PPS��������������Сһ��Ϊ4�ֽ�
 *                         - PPS[0] bit 3 ~ bit0 �� bit7����
 *                         - PPS[0] bit 4��1 �� PPS1���ڣ�0 �� PPS1������
 *                         - PPS[0] bit 5��1 �� PPS2���ڣ�0 �� PPS2������
 *                         - PPS[0] bit 6��1 �� PPS3���ڣ�0 �� PPS3������
                           - PPS[1] ��F/D
                           - PPS[2] ��N
                           - PPS[3] ������
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_cicc_pps (int      id,
                                uint8_t *p_pps);

/**
 * \brief �Ӵ�ʽIC������Э�飨T = 0��
 *
 *     ����������T = 0����Э�顣���Ӵ�ʽIC���Ĵ���Э��ΪT = 0���ú�����ͬ��
 * awbl_zlg600a_cicc_tpdu()��
 *
 * \param[in] id            : �豸ID��
 * \param[in] p_tx_buf      : ���ͻ�����
 * \param[in] tx_bufsize    : ���ͻ������ĳ��ȣ� 1 ~ 272
 * \param[in] p_rx_buf      : ���ջ�����
 * \param[in] rx_bufsize    : ���ջ������ĳ���
 * \param[in] p_real_len    : ���ڻ�ȡʵ�ʽ�����Ϣ���ȵ�ָ�룬������ջ�����
 *                            �ĳ���С��ʵ�ʳ��ȣ�����ಿ�ֶ�����
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_cicc_tpdu_tp0 (int       id,
                                     uint8_t  *p_tx_buf,
                                     uint8_t   tx_bufsize,
                                     uint8_t  *p_rx_buf,
                                     uint8_t   rx_bufsize,
                                     uint16_t *p_real_len);

/**
 * \brief �Ӵ�ʽIC������Э�飨T = 1��
 *
 *     ����������T = 1����Э�顣���Ӵ�ʽIC���Ĵ���Э��ΪT = 1���ú�����ͬ��
 * awbl_zlg600a_cicc_tpdu()��
 *
 * \param[in] id            : �豸ID��
 * \param[in] p_tx_buf      : ���ͻ�����
 * \param[in] tx_bufsize    : ���ͻ������ĳ��ȣ� 1 ~ 272
 * \param[in] p_rx_buf      : ���ջ�����
 * \param[in] rx_bufsize    : ���ջ������ĳ���
 * \param[in] p_real_len    : ���ڻ�ȡʵ�ʽ�����Ϣ���ȵ�ָ�룬������ջ�����
 *                            �ĳ���С��ʵ�ʳ��ȣ�����ಿ�ֶ�����
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_cicc_tpdu_tp1 (int       id,
                                     uint8_t  *p_tx_buf,
                                     uint8_t   tx_bufsize,
                                     uint8_t  *p_rx_buf,
                                     uint8_t   rx_bufsize,
                                     uint16_t  *p_real_len);

/**
 * \name PICCA������ģʽ
 * @{
 */
#define AWBL_ZLG600A_PICCA_REQ_IDLE 0x26 /**< \brief IDLEģʽ,������еĿ� */
#define AWBL_ZLG600A_PICCA_REQ_ALL  0x52 /**< \brief ALLģʽ,�������еĿ�  */
/** @}*/

/**
 * \brief PICCA���������
 *
 *  �ú�����Ϊ�������������ֻҪ����ISO14443A��׼�Ŀ���Ӧ�ܷ�����Ӧ��
 *
 * \param[in] id          : �豸ID��
 * \param[in] req_mode    : ����ģʽ����ʹ�����к꣺
 *                           - #AWBL_ZLG600A_PICCA_REQ_IDLE
 *                           - #AWBL_ZLG600A_PICCA_REQ_ALL
 * \param[in] p_atq       : ��ȡ����Ӧ����Ϣ��ATQ����ָ�룬��ֵΪ16λ����ͬ��
 *                          ���ͷ��ص���Ϣ��ͬ,�������ͷ��ص�ATQ���£�
 *
 *            Mifare1 S50    | Mifare1 S70 | Mifare1 Light | Mifare0 UltraLight
 *       --------------------|-------------|---------------|-------------------
 *              0x0004       |    0x0002   |    0x0010     |      0x0044
 *       ----------------------------------------------------------------------
 *          Mifare3 DESFire  |   SHC1101   |    SHC1102    |      11RF32
 *       --------------------|-------------|---------------|-------------------
 *               0x0344      |    0x0004   |    0x3300     |      0x0004
 *
 * \retval AW_OK      : ����ɹ�
 * \retval -AW_EINVAL : ����ʧ�ܣ���������
 * \retval -AW_EIO    : ����ʧ�ܣ�����ͨ�ų���
 *
 * \note ���������ߺ󣬴���Ƶ���л�ȡ�������Ӷ��õ縴λ����λ�󿨴���IDLEģʽ��
 * ����������ģʽ����һ������ʱ����ʱ�Ŀ�������Ӧ������ĳһ�ſ��ɹ������˹���
 * ������Halt�����DeSelect������������Haltģʽ����ʱ�Ŀ�ֻ��ӦALL��0x52��
 * ģʽ�����󣬳��ǽ����뿪���߸�Ӧ�����ٽ��롣
 */
aw_err_t awbl_zlg600a_picca_request (int       id,
                                     uint8_t   req_mode,
                                     uint16_t *p_atq);

/**
 * \name PICCA������ײ�ȼ�
 * @{
 */
#define AWBL_ZLG600A_PICCA_ANTICOLL_1  0x93  /**< \brief ��һ������ײ  */
#define AWBL_ZLG600A_PICCA_ANTICOLL_2  0x95  /**< \brief �ڶ�������ײ  */
#define AWBL_ZLG600A_PICCA_ANTICOLL_3  0x97  /**< \brief ����������ײ  */
/** @}*/

/**
 * \brief PICCA���ķ���ײ����
 *
 *  ��Ҫ�ɹ�ִ��һ�������������������ɹ������ܽ��з���ײ���������򷵻ش���
 *
 * \param[in] id             : �豸ID��
 * \param[in] anticoll_level : ����ײ�ȼ�����ʹ�����к꣺
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_1
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_2
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_3
 *
 * \param[in] know_uid : ��֪�Ŀ����к�
 * \param[in] nbit_cnt : λ������������֪�����к�uid��λ������ֵС��32��
 *                       - ��nbit_cntֵΪ0,�����û����֪���кţ����кŵ�����λ
 *                         ��Ҫ�ӱ�������á�
 *                       - ��nbit_cnt��Ϊ0�������к�������֪�����кŵ�ֵ����ʾ
 *                         ���кŵ�ǰnbit_cntλΪ��֪���к�����ʾǰnbit_cntλ��
 *                         ֵ������λ��Ҫ��������á�
 * \param[in] *p_uid   : ����ײ���ȡ���Ŀ����к�
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 *
 * \note  ����ISO14443A��׼�������кŶ���ȫ��Ψһ�ģ���������Ψһ�ԣ�����ʵ�ַ�
 * ��ײ���㷨�߼������������ſ�ͬʱ�����߸�Ӧ��������������ܹ��ҵ�һ�����кŽ�
 * ��Ŀ���������
 */
aw_err_t awbl_zlg600a_picca_anticoll (int       id,
                                      uint8_t   anticoll_level,
                                      uint32_t  know_uid,
                                      uint8_t   nbit_cnt,
                                      uint32_t *p_uid);

/**
 * \brief PICC A����ѡ�����
 *
 *  ��Ҫ�ɹ�ִ��һ�η���ײ��������سɹ������ܽ��п�ѡ����������򷵻ش���
 *
 * \param[in] id             : �豸ID��
 * \param[in] anticoll_level : ����ײ�ȼ�����ʹ�����к꣺
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_1
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_2
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_3
 *
 * \param[in] know_uid : ��֪�Ŀ����к�
 * \param[in] nbit_cnt : λ������������֪�����к�uid��λ������ֵС��32��
 *                       - ��nbit_cntֵΪ0,�����û����֪���кţ����кŵ�����λ
 *                         ��Ҫ�ӱ�������á�
 *                       - ��nbit_cnt��Ϊ0�������к�������֪�����кŵ�ֵ����ʾ
 *                         ���кŵ�ǰnbit_cntλΪ��֪���к�����ʾǰnbit_cntλ��
 *                         ֵ������λ��Ҫ��������á�
 * \param[in] uid   : ǰһ������ײ������ȡ��UID
 * \param[in] p_sak : ���ص���Ϣ����bit2Ϊ1�������UID������
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 *
 * \note  �������кų��������֣�4�ֽڡ�7�ֽں�10�ֽڡ� 4�ֽڵ�ֻҪ��һ��ѡ�񼴿�
 * �õ����������кţ���Mifare1 S50/S70�ȣ�7�ֽڵ�Ҫ�ö���ѡ����ܵõ�����������
 * �ţ�ǰһ�����õ������кŵ�����ֽ�Ϊ������־0x88�������к���ֻ�к�3�ֽڿ��ã�
 * ��һ��ѡ���ܵõ�4�ֽ����кţ����߰�˳�����Ӽ�Ϊ7�ֽ����кţ���UltraLight
 * ��DesFire�ȣ�10�ֽڵ��Դ����ƣ�������δ���ִ��࿨��
 */
aw_err_t awbl_zlg600a_picca_select (int       id,
                                    uint8_t   anticoll_level,
                                    uint32_t  uid,
                                    uint8_t  *p_sak);

/**
 * \brief PICCA���Ĺ��������ʹ��ѡ��Ŀ�����HALT״̬
 *
 *  ��HALT״̬�£���������Ӧ������������IDLEģʽ�����󣬳��ǽ�����λ���뿪���߸�
 *  Ӧ�����ٽ��롣��������Ӧ������������ALL����
 *
 * \param[in] id      : �豸ID��
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_picca_halt (int id);

/**
 * \brief PICC TypeA��RATS��request for answer to select��
 *
 *     RATS��request for answer to select����ISO14443-4Э������ģ�鷢��RATS��
 * ��Ƭ����ATS��answer to select����ΪRATS��Ӧ����ִ�и�����ǰ�������Ƚ���һ��
 * ��ѡ���������ִ�й�һ��RATS��������ٴ�ִ��RATS��������Ƚ�����
 *
 * \param[in] id         : �豸ID��
 * \param[in] cid        : ����ʶ����card IDentifier��ȡֵ��Χ0x00��0x0E��
 * \param[in] p_ats_buf  : ���ڽ��ջ�Ӧ����Ϣ����ͬ�����ֽ������в�ͬ
 * \param[in] bufsize    : ��������С
 * \param[in] p_real_len : ���ڻ�ȡʵ�ʽ��յ���Ϣ���ȣ������������СС��ʵ����
 *                         Ϣ���ȣ�����ಿ�ֽ�����
 *
 * \retval AW_OK      : д��ɹ�
 * \retval -AW_EINVAL : д��ʧ�ܣ���������
 * \retval -AW_EIO    : д��ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_picca_rats (int       id,
                                  uint8_t   cid,
                                  uint8_t  *p_ats_buf,
                                  uint8_t   bufsize,
                                  uint16_t *p_real_len);

/**
 * \brief PICC TypeA��PPS��request for answer to select��
 *
 *     PPS��protocal and parameter selection����ISO14443-4Э���������ڸı���
 * �ص�ר��Э�������������Ǳ���ģ�����ֻ֧��Ĭ�ϲ�������������Ĳ�������Ϊ
 * 0���ɡ���ִ�и�����ǰ�������ȳɹ�ִ��һ��RATS���
 *
 * \param[in] id         : �豸ID��
 * \param[in] dsi_dri    : ģ���뿨ͨ�Ų�����,����Ϊ0��Ĭ�ϣ�����
 *
 * \retval AW_OK      : д��ɹ�
 * \retval -AW_EINVAL : д��ʧ�ܣ���������
 * \retval -AW_EIO    : д��ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_picca_pps (int id, uint8_t dsi_dri);

/**
 * \brief PICC TypeA���������
 *
 *   ��������ISO14443-4Э���������ڽ���Ƭ��Ϊ����HALT��״̬�����ڹ���
 *��HALT��״̬�Ŀ������á���������������ΪALL�������¼����ֻ��ִ�С�RATS��
 * ����Ŀ����ø����
 *
 * \param[in] id      : �豸ID��
 *
 * \retval AW_OK      : д��ɹ�
 * \retval -AW_EINVAL : д��ʧ�ܣ���������
 * \retval -AW_EIO    : д��ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_picca_deselect (int id);

/**
 * \brief PICC��T=CL
 *
 *     T=CL�ǰ�˫�����鴫��Э�飬ISO14443-4Э��������ڶ�д���뿨Ƭ֮�������
 * ������һ�����ISO14443Э���CPU�����ø�Э�����д��ͨ�š����ø�����ʱֻ��Ҫ
 * ��CPU��COS�����������Ϊ���뼴�ɣ���������������͡�����ʶ��CID��֡�ȴ�ʱ��
 * FWT���ȴ�ʱ����չ��������WTXM��waiting time extensionmultiplier�����ȵ��ɸ�
 * �����Զ���ɡ�
 *
 * \param[in] id          : �豸ID��
 * \param[in] p_cos_buf   : COS����
 * \param[in] cos_bufsize ��COS����ĳ���
 * \param[in] p_res_buf   : ���ڽ��ջ�Ӧ����Ϣ����ͬCOS����ֽ������в�ͬ
 * \param[in] res_bufsize : ���ջ�Ӧ��Ϣ�Ļ�������С
 * \param[in] p_real_len  : ���ڻ�ȡʵ�ʽ��յ���Ϣ���ȣ������������СС��ʵ����
 *                          Ϣ���ȣ�����ಿ�ֽ�����
 *
 * \retval AW_OK      : д��ɹ�
 * \retval -AW_EINVAL : д��ʧ�ܣ���������
 * \retval -AW_EIO    : д��ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_picc_tpcl (int       id,
                                 uint8_t  *p_cos_buf,
                                 uint8_t   cos_bufsize,
                                 uint8_t  *p_res_buf,
                                 uint8_t   res_bufsize,
                                 uint16_t *p_real_len);

/**
 * \brief ���ݽ���
 *
 * �������ö�д���뿨Ƭ�����ݽ�����ͨ���ú�������ʵ�ֶ�д���������й��ܡ�
 *
 * \param[in] id         : �豸ID��
 * \param[in] p_data_buf : ��Ҫ�������͵����ݣ���������ʵ��ʹ�õĿ��йأ�
 * \param[in] len        : �������ݵĳ���
 * \param[in] wtxm_crc   : b7~b2,wtxm��b1,RFU(0); b0,0-CRC��ֹ�ܣ�1-CRCʹ�ܡ�
 * \param[in] fwi        : ��ʱ�ȴ�ʱ�����, ��ʱʱ��=����0x01 << fwi*302us��
 * \param[in] p_rx_buf   : ���ݽ�����Ӧ֡��Ϣ
 * \param[in] bufsize    : ���ջ�Ӧ��Ϣ�Ļ�������С
 * \param[in] p_real_len : ���ڻ�ȡʵ�ʽ��յ���Ϣ���ȣ������������СС��ʵ����
 *                         Ϣ���ȣ�����ಿ�ֽ�����
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_picc_data_exchange (int       id,
                                          uint8_t  *p_data_buf,
                                          uint8_t   len,
                                          uint8_t   wtxm_crc,
                                          uint8_t   fwi,
                                          uint8_t  *p_rx_buf,
                                          uint8_t   bufsize,
                                          uint16_t *p_real_len);

/**
 * \brief PICC A����λ��ͨ�����ز��źŹر�ָ����ʱ�䣬�ٿ�����ʵ�ֿ�Ƭ��λ��
 *
 * \param[in] id      : �豸ID��
 * \param[in] time_ms : �رյ�ʱ�䣨��λ:ms����0Ϊһֱ�ر�
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 *
 * \note �ú����������źŹر������룬��һֱ�رգ���ȵ�ִ��һ����������ʱ�򿪡�
 */
aw_err_t awbl_zlg600a_picca_reset (int id, uint8_t time_ms);

/**
 * \brief PICC A������ú������ڼ��Ƭ�������󡢷���ײ��ѡ�������������ϡ�
 *
 * \param[in] id         : �豸ID��
 * \param[in] req_mode   : ����ģʽ����ʹ�����к꣺
 *                           - #AWBL_ZLG600A_PICCA_REQ_IDLE
 *                           - #AWBL_ZLG600A_PICCA_REQ_ALL
 * \param[in] p_atq      : ����Ӧ��
 * \param[in] p_saq      : ���һ��ѡ��Ӧ��SAK
 * \param[in] p_len      : ���кų��ȣ�4�ֽڻ�7�ֽ�
 * \raram[in] p_uid      : ���кŻ�����������Ӧ�������кų��ȱ���һ��
 *
 * \retval AW_OK      : д��ɹ�
 * \retval -AW_EINVAL : д��ʧ�ܣ���������
 * \retval -AW_EIO    : д��ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_picca_active (int       id,
                                    uint8_t   req_mode,
                                    uint16_t *p_atq,
                                    uint8_t  *p_saq,
                                    uint8_t  *p_len,
                                    uint8_t  *p_uid,
                                    uint8_t   uid_len);

/**
 * \name PICCB������ģʽ
 * @{
 */
#define AWBL_ZLG600A_PICCB_REQ_IDLE 0x00 /**< \brief IDLEģʽ,������еĿ� */
#define AWBL_ZLG600A_PICCB_REQ_ALL  0x08 /**< \brief ALLģʽ,�������еĿ�  */
/** @}*/

/**
 * \brief PICC B������
 *
 *    �ڵ��øú���ǰ����Ҫȷ��IC����ģʽ�Ѿ�����ΪTypeBģʽ��
 *
 * \param[in] id         : �豸ID��
 * \param[in] req_mode   : ����ģʽ����ʹ�����к꣺
 *                           - #AWBL_ZLG600A_PICCB_REQ_IDLE
 *                           - #AWBL_ZLG600A_PICCB_REQ_ALL
 * \raram[in] p_uid      : UID�����Ϣ������Ϊ12
 *
 * \retval AW_OK      : д��ɹ�
 * \retval -AW_EINVAL : д��ʧ�ܣ���������
 * \retval -AW_EIO    : д��ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_piccb_active (int id, uint8_t req_mode, uint8_t *p_uid);

/**
 * \brief PICC B����λ��ͨ�����ز��źŹر�ָ����ʱ�䣬�ٿ�����ʵ�ֿ�Ƭ��λ��
 *
 * \param[in] id       : �豸ID��
 * \param[in] time_ms  : �رյ�ʱ�䣨��λ:ms����0Ϊһֱ�ر�
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 *
 * \note �ú����������źŹر������룬��һֱ�رգ���ȵ�ִ��һ����������ʱ�򿪡�
 */
aw_err_t awbl_zlg600a_piccb_reset (int id, uint8_t time_ms);

/**
 * \brief PICCA���������
 *
 *  �ú�����Ϊ�������������ֻҪ����ISO14443A��׼�Ŀ���Ӧ�ܷ�����Ӧ��
 *
 * \param[in] id          : �豸ID��
 * \param[in] req_mode    : ����ģʽ����ʹ�����к꣺
 *                           - #AWBL_ZLG600A_PICCB_REQ_IDLE
 *                           - #AWBL_ZLG600A_PICCB_REQ_ALL
 * \param[in] slot_time   : ʱ϶������0 ~ 4
 * \param[in] p_uid       : ���ص�UID�����Ϣ������Ϊ12
 *
 * \retval AW_OK      : ����ɹ�
 * \retval -AW_EINVAL : ����ʧ�ܣ���������
 * \retval -AW_EIO    : ����ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_piccb_request (int      id,
                                     uint8_t  req_mode,
                                     uint8_t  slot_time,
                                     uint8_t *p_uid);

/**
 * \brief PICC B ���ķ���ײ����
 *
 * \param[in] id          : �豸ID��
 * \param[in] slot_flag   : ʱ϶��ǣ�2 ~ 16���ò���ֵ�����������ʱ϶�����й�
 *                          ϵ���������������ʱ϶����Ϊn�����ʱ϶���N < 2^n
 * \param[in] p_uid       : ���ص�UID�����Ϣ������Ϊ12
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_piccb_anticoll (int      id,
                                      uint8_t  slot_flag,
                                      uint8_t *p_uid);


/**
 * \brief PICC B���Ĺ��������ʹ��ѡ��Ŀ�����HALT״̬
 *
 *     �ú�������B�Ϳ�������ִ�й�������ǰ��������ִ�гɹ���һ���������ִ��
 * ��������ɹ��󣬿�Ƭ���ڹ���״̬��ģ�����ͨ��ALL��ʽ����Ƭ����������IDLE��
 * ʽ����
 *
 * \param[in] id      : �豸ID��
 * \param[in] p_pupi  : PUPI,��СΪ4�ֽڵı�ʶ��
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_piccb_halt (int id, uint8_t *p_pupi);


/**
 * \brief �޸�B���Ĵ�������
 *
 *     �ú�������B�Ϳ�������ִ�й�������ǰ��������ִ�гɹ���һ���������ִ��
 * ��������ɹ��󣬿�Ƭ���ڹ���״̬��ģ�����ͨ��ALL��ʽ����Ƭ����������IDLE��
 * ʽ����
 *
 * \param[in] id         : �豸ID��
 * \param[in] p_pupi     : PUPI,��СΪ4�ֽڵı�ʶ��
 * \param[in] cid        : ȡֵ��ΧΪ 0 - 14, ����֧��CID��������Ϊ0
 * \param[in] protype    : ֧�ֵ�Э��
 *                          - bit3 : 1-PCD��ֹ��PICC����ͨ��,0-PCD��PICC����ͨ��
 *                          - bit2 ~ bit1 :
 *                              - 11 : 10 etu + 512 / fs
 *                              - 10 : 10 etu + 256 / fs
 *                              - 01 : 10 etu + 128 / fs
 *                              - 00 : 10 etu + 32 / fs
 *                          - bit0 : 1-��ѭISO14443-4,0-����ѭISO14443-4
 *                                   (�������֤�����λ����Ϊ1)
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_piccb_attrib_set (int      id,
                                        uint8_t *p_pupi,
                                        uint8_t  cid,
                                        uint8_t  protype);

/**
 * \brief ��ȡ8�ֽ����֤ID
 *
 *     �ú������ڻ�ȡ�������֤ID����ִ������ǰ�������ȵ���awbl_zlg600a_ic_isotype_set()
 * ����Э��ΪISO14443B��
 *
 * \param[in]  id      : �豸ID��
 * \param[in] req_mode : ����ģʽ����ʹ�����к꣺
 *                       - #AWBL_ZLG600A_PICCB_REQ_IDLE
 *                       - #AWBL_ZLG600A_PICCB_REQ_ALL
 * \param[out] p_sid   : 8�ֽ�ID
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_sid_get (int id, uint8_t req_mode, uint8_t *p_sid);

/**
 * \brief PLUS CPU�����˻���������
 *
 *     ����������SL0��Security Level 0����ȫ�ȼ�0����PLUS CPU�����˻���PLUS CPU
 * ������ʱ�İ�ȫ�ȼ�ΪSL0���õȼ��£�����Ҫ�κ���֤�Ϳ�������д���ݣ�д�����
 * ������Ϊ������ȫ�ȼ��ĳ�ʼֵ�����磺��SL0��0x0003��д�룺0xA0 0xA1 0xA2 0xA3
 * 0xA4 0xA5 0xFF 0x07 0x80 0x69 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF������Ƭ������SL1
 * ������0��A��ԿΪ0xA0 0xA1 0xA2 0xA3 0xA4 0xA5��������Ĭ�ϵ�0xFF 0xFF 0xFF
 * 0xFF 0xFF 0xFF����������SL0�޸Ŀ�Ƭ��Ĭ�����ݺ���Կ��
 *
 * \param[in] id            : �豸ID��
 * \param[in] sector_addr   : PLUS CPU���洢����ַ
 * \param[in] p_buf         : ���ͻ�����,���ݸ���Ϊȷ��16��
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_pluscpu_persotcl_write (int       id,
                                              uint16_t  sector_addr,
                                              uint8_t  *p_buf);

/**
 * \brief PLUS CPU���ύ���˻�
 *
 *  awbl_zlg600a_pluscpu_persotcl_write()����ֻ�Ǹ��¿��е����ݣ������ݻ�δ��Ч��
 * �ύ�����ݲ���Ч��ִ�и������PLUS CPU���İ�ȫ�ȼ���ߵ�SL1����SL3
 * ������֧��SL1�Ŀ�����ִ�и������Ƭ��ȫ�ȼ���ߵ�SL1������ֻ֧��SL0��SL3��
 * ������ִ�и������Ƭ��ȫ�ȼ���ߵ�SL3����
 * ע�⣺��SL0�У�ֻ���޸������µ�ַ����ִ���ύ������
 *   - 0x9000��������Կ��
 *   - 0x9001�����ÿ���Կ��
 *   - 0x9002��SL2������Կ��ֻ��֧��SL2�Ŀ����и���Կ��
 *   - 0x9003��SL3������Կ��ֻ��֧��SL3�Ŀ����и���Կ��
 *
 * \param[in] id      : �豸ID��
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_pluscpu_persotcl_commit (int id);


/**
 * \brief PLUS CPU���״���֤��ֱ����Կ��֤��
 *
 * \param[in] id           : �豸ID��
 * \param[in] authent_addr : ����֤��16λ��Կ��ַ
 * \param[in] p_key        : ��Կ����������СΪ16�ֽ�
 *
 * \retval AW_OK      : ��֤�ɹ�
 * \retval -AW_EINVAL : ��֤ʧ�ܣ���������
 * \retval -AW_EIO    : ��֤ʧ�ܣ�����ͨ�ų���
 *
 * \note PLUS CPU������ԿA/B��ͨ����ַ����ż�������֣�AES����Կ��ַ�����ݿ��
 * ������ϵ��Ӧ����:
 * ��ԿA��ַ=0x4000 + ���� �� 2
 * ��ԿB��ַ=0x4000 + ���� �� 2 + 1
 * ��ˣ�����֤1��������ԿA������Կ��ַΪ��0x4002
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_first_direct_authent (int       id,
                                                        uint16_t  authent_addr,
                                                        uint8_t  *p_key);

/**
 * \brief PLUS CPU���״���֤��E2��Կ��֤������֤����Կ����ģ���ڲ������粻��ʧ
 *
 * \param[in] id            : �豸ID��
 * \param[in] authent_addr  : ����֤��16λ��Կ��ַ
 * \param[in] key_sector    : ��Կ��������
 *
 * \retval AW_OK      : ��֤�ɹ�
 * \retval -AW_EINVAL : ��֤ʧ�ܣ���������
 * \retval -AW_EIO    : ��֤ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_first_e2_authent (int      id,
                                                    uint16_t authent_addr,
                                                    uint8_t  key_sector);

/**
 * \brief PLUS CPU��������֤��ֱ����Կ��֤��
 *
 *     �ú�������SL3 PLUS CPU���ĸ�����Կ��֤����֤����Կ�������������ֻ��ִ��
 * �����״���֤������ɹ������ʹ�ø����������֤����������ʹ�õ�ʱ�䲻ͬ��
 * ���״���֤������Ҫ��ʱ��ȡ�������֤����ʱ��Ҫ��Щ��
 *
 * \param[in] id           : �豸ID��
 * \param[in] authent_addr : ����֤��16λ��Կ��ַ
 * \param[in] p_key        : ��Կ����������СΪ16�ֽ�
 *
 * \retval AW_OK      : ��֤�ɹ�
 * \retval -AW_EINVAL : ��֤ʧ�ܣ���������
 * \retval -AW_EIO    : ��֤ʧ�ܣ�����ͨ�ų���
 *
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_follow_direct_authent (int       id,
                                                         uint16_t  authent_addr,
                                                         uint8_t  *p_key);

/**
 * \brief PLUS CPU��������֤��E2��Կ��֤��,��֤����Կ����ģ���ڲ������粻��ʧ
 *
 * \param[in] id            : �豸ID��
 * \param[in] authent_addr  : ����֤��16λ��Կ��ַ
 * \param[in] key_sector    : ��Կ��������
 *
 * \retval AW_OK      : ��֤�ɹ�
 * \retval -AW_EINVAL : ��֤ʧ�ܣ���������
 * \retval -AW_EIO    : ��֤ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_follow_e2_authent (int      id,
                                                     uint16_t authent_addr,
                                                     uint8_t  key_sector);

/**
 * \brief PLUS CPU��SL3��λ��֤��E2��Կ��֤��,��֤����Կ����ģ���ڲ������粻��ʧ
 *
 *  ����������PLUS CPU��ͨ���״���֤���ʹ�ù����У���λ��д����������֤����Ϣ��
 *
 * \param[in] id      : �豸ID��
 *
 * \retval AW_OK      : ��֤�ɹ�
 * \retval -AW_EINVAL : ��֤ʧ�ܣ���������
 * \retval -AW_EIO    : ��֤ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_reset_authent (int id);

/**
 * \name PLUS CPU�������ݵ�ģʽ
 * @{
 */

/** \brief ������MAC����������(CIPHER)����Ӧ��MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_M_C_NM    0x30

/** \brief ������MAC����������(CIPHER)����Ӧ��MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_M_C_M     0x31

/** \brief ������MAC���������ģ���Ӧ��MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_M_NC_NM   0x32

/** \brief ������MAC���������ģ���Ӧ��MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_M_NC_M    0x33

/** \brief ������MAC���������ģ���Ӧ��MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_NM_C_NM   0x34

/** \brief ������MAC���������ģ���Ӧ��MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_NM_C_M    0x35

/** \brief ������MAC���������ģ���Ӧ��MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_NM_NC_NM  0x36

/** \brief ������MAC���������ģ���Ӧ��MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_NM_NC_M   0x37

/** @}*/

/**
 * \brief PLUS CPU��SL3�����ݿ�
 *
 *  �ú������ڶ�ȡSL3�����ݿ飬�ڶ����ݿ�֮ǰ����ɹ�ִ��һ����Կ��֤��
 *
 * \param[in] id            : �豸ID��
 * \param[in] read_mode     : ������ģʽ��ʹ�ú�AWBL_ZLG600A_PLUSCPU_SL3_READMODE_*
 *                            (#AWBL_ZLG600A_PLUSCPU_SL3_READMODE_M_C_M)
 * \param[in] start_block   : �����ݵ���ʼ���
 * \param[in] block_num     : ���Ŀ�����1 ~ 3
 * \param[in] p_buf         : ��ȡ�����ݴ�Ż���������СӦ��Ϊ��block_num * 16
 *
 * \retval AW_OK      : ��֤�ɹ�
 * \retval -AW_EINVAL : ��֤ʧ�ܣ���������
 * \retval -AW_EIO    : ��֤ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_read (int       id,
                                        uint8_t   read_mode,
                                        uint16_t  start_block,
                                        uint8_t   block_num,
                                        uint8_t  *p_buf);

/**
 * \name PLUS CPU��д���ݵ�ģʽ
 * @{
 */

/** \brief ������MAC����������(CIPHER)����Ӧ��MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_WRITEMODE_M_C_NM     0xA0

/** \brief ������MAC����������(CIPHER)����Ӧ��MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_WRITEMODE_M_C_M      0xA1

/** \brief ������MAC���������ģ���Ӧ��MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_WRITEMODE_M_NC_NM    0xA2

/** \brief ������MAC���������ģ���Ӧ��MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_WRITEMODE_M_NC_M     0xA3

/** @}*/

/**
 * \brief PLUS CPU��SL3д���ݿ�
 *
 *  �ú�������дSL3�����ݿ飬��д���ݿ�֮ǰ����ɹ�ִ��һ����Կ��֤��
 *
 * \param[in] id            : �豸ID��
 * \param[in] write_mode    : д����ģʽ��ʹ�ú�aAWBL_ZLG600A_PLUSCPU_SL3_WRITEMODE_*
 *                            (#AWBL_ZLG600A_PLUSCPU_SL3_WRITEMODE_M_C_NM)
 * \param[in] start_block   : д���ݵ���ʼ���
 * \param[in] block_num     : д�Ŀ�����1 ~ 3
 * \param[in] p_buf         : д�����ݴ�Ż���������СӦ��Ϊ��block_num * 16
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_write (int       id,
                                         uint8_t   write_mode,
                                         uint16_t  start_block,
                                         uint8_t   block_num,
                                         uint8_t  *p_buf);

/**
 * \name PLUS CPU ��ֵ����ģʽ
 * @{
 */
#define AWBL_ZLG600A_PLUSCPU_VALUE_ADD  0xB7  /**< \brief ��  */
#define AWBL_ZLG600A_PLUSCPU_VALUE_SUB  0xB9  /**< \brief ��  */
/** @}*/

/**
 * \brief PLUS CPU��ֵ�����
 *
 * \param[in] id         : �豸ID��
 * \param[in] mode       : ֵ������ģʽ�������Ǽӻ����ʹ�����к꣺
 *                          - AWBL_ZLG600A_PLUSCPU_VALUE_ADD
 *                          - AWBL_ZLG600A_PLUSCPU_VALUE_SUB
 * \param[in] src_block  : ����ֵ�����������Դ���
 * \param[in] dst_block  : ֵ��������������ݴ�ŵ�Ŀ�Ŀ��
 * \param[in] value      : 4�ֽ��з�����
 *
 * \retval AW_OK      : д��ɹ�
 * \retval -AW_EINVAL : д��ʧ�ܣ���������
 * \retval -AW_EIO    : д��ʧ�ܣ�����ͨ�ų���
 *
 * \note Ҫ���д�������������ݱ���Ҫ��ֵ��ĸ�ʽ���ɲο�NXP������ĵ���������
 * ���봫������ͬ���򽫲�����Ľ��д��ԭ���Ŀ��ڣ���������봫���Ų���ͬ��
 * �򽫲�����Ľ��д�봫����ڣ����������ڵ����ݱ����ǣ�ԭ���ڵ�ֵ���䡣
 * ���ڵȼ�2��PLUS CPU����֧��ֵ��������ȼ�1��3֧�֡�
 */
aw_err_t awbl_zlg600a_pluscpu_val_operate (int      id,
                                           uint8_t  mode,
                                           uint16_t src_block,
                                           uint16_t dst_block,
                                           int32_t  value);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_ZLG600A_H */

/* end of file */
