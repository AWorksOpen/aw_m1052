/**
 * \file
 * \brief ZSN603���
 *
 */
#ifndef __ZSN603_H_
#define __ZSN603_H_
#include "zsn603_platform.h"

/**
 * \name �豸����״̬����,���ڱ�ʾ��ǰ�豸����״̬
 * ����ZSN603�豸�Ĺ���ģʽ����
 * @{
 */
/** \brief ZSN603��������ģʽ  ����-��Ӧ*/
#define  ZSN603_NORMAL_MODE                     0xA0

/** \brief  �Զ����ģʽ���ж�������
 * ��ģʽΪ��ǰ�豸�����Զ����ģʽ��,�ҵ�ǰͨ��ģʽΪUARTģʽ,���ҵ�ǰ�Զ����ģʽ����Ϊ���ж��һ������ش�����
 */
#define  ZSN603_AUTO_MODE_NO_INT                0xA1

/** \brief  �Զ����ģʽ���ж�������
 * ��ģʽΪ��ǰ�豸�����Զ����ģʽ��,�ҵ�ǰͨ��ģʽΪUARTģʽ,���ҵ�ǰ�Զ����ģʽ����Ϊ���ж��һ������ش�����
 */
#define  ZSN603_AUTO_MODE_INT_MESSAGE           0xA2

/** \brief  �Զ����ģʽ�����ж�
 * ��ģʽΪ��ǰ�豸�����Զ����ģʽ��,�ҵ�ǰͨ��ģʽΪUARTģʽ,���ҵ�ǰ�Զ����ģʽ����Ϊ���жϵ����������ش�����
 * ���ߵ�ǰͨ��ģʽ����I2Cģʽ��
 */
#define  ZSN603_AUTO_MODE_INT                   0xA3

/**
 * \name �豸֡���Ⱥ궨��
 *
 * @{
 */
#define  ZSN603_FRAME_HEAD_LENGHT               0x08 /* ֡ͷ����,֡��Ϣ֮ǰ���ֽڳ����ܹ�Ϊ8�ֽ�*/
#define  ZSN603_FRAME_CHECK_LENGHT              0x02 /* ֡У��ͳ���,2�ֽ� */
#define  ZSN603_FRAME_FRAME_MIN_LENGHT          0x0A /* ��С֡����10�ֽ� */
#define  ZSN603_BUFFER_SIZE_MAX                 282  /**< \brief ���ڻ�����������Ϊ272 + 10 */
/**
 * \name �豸Ӧ���ඨ��
 *   �豸����֡���ͱ�ʶ
 * @{
 */
#define  ZSN603_DEVICE_CONTROL_CMD              0x01 /**< \brief �豸����������*/
#define  ZSN603_MIFARE_CARD_CMD                 0x02 /**< \brief Mifare������ */
#define  ZSN603_ISO7816_PROTOCOL_CMD            0x05 /**< \brief ISO7816Э������ */
#define  ZSN603_ISO14443_PROTOCOL_CMD           0x06 /**< \brief ISO14443Э������ */
#define  ZSN603_CPU_CARD_CMD                    0x07 /**< \brief CPU������ */

/**
 * \name �豸���������
 *   ����ZSN603�豸������������
 * @{
 */
#define  ZSN603_GET_DEVICE_INFO                 0x41 /**< \brief ��ȡ�豸��Ϣ*/
#define  ZSN603_CONFIG_ICC_INTERFACE            0x42 /**< \brief ����IC���ӿ� */
#define  ZSN603_CLOSE_ICC_INTERFACE             0x43 /**< \brief �ر�IC���ӿ� */
#define  ZSN603_SET_ICC_INTERFACE_PROTOCOL      0x44 /**< \brief IC�ӿ�Э�� */
#define  ZSN603_LOAD_ICC_KEY                    0x45 /**< \brief װ��IC��Կ */
#define  ZSN603_SET_ICC_INTERFACE_REG           0x46 /**< \brief ����IC�ӿڼĴ��� */
#define  ZSN603_GET_ICC_INTERFACE_REG           0x47 /**< \brief ��ȡIC�ӿڼĴ��� */
#define  ZSN603_SET_BAUD_RATE                   0x48 /**< \brief ���ò����� */
#define  ZSN603_SET_ICC_INTERFACE_OUTPUT        0x49 /**< \brief ������������ģʽ */
#define  ZSN603_SET_ANTENNA_CHANNAL             0x4B /**< \brief ��������ͨ�� */
#define  ZSN603_SET_LOCAL_ADDRESS               0x4C /**< \brief ���ôӻ���ַ */
#define  ZSN603_LED_CONTROL                     0x4D /**< \brief LED���� */
#define  ZSN603_BUZZER_CONTROL                  0x4E /**< \brief ���������� */
#define  ZSN603_READ_EEPROM                     0x62 /**< \brief ��EEPROM */
#define  ZSN603_WRITE_EEPROM                    0x63 /**< \brief дEEPROM */
#define  ZSN603_AUTO_TEST                       0x48 /**< \brief ִ���Զ����� */
#define  ZSN603_READ_AUTO_TEST                  0x49 /**< \brief ���Զ����Խ�� */
/**
 * \name Mifare�࿨�����
 *   ����ZSN603  Mifare��������
 * @{
 */
#define  ZSN603_MIFARE_REQUEST                  0x41 /**< \brief ������*/
#define  ZSN603_MIFARE_ANTICOLL                 0x42 /**< \brief ������ײ */
#define  ZSN603_MIFARE_SELECT                   0x43 /**< \brief ��ѡ�� */
#define  ZSN603_MIFARE_HALT                     0x44 /**< \brief ������ */
#define  ZSN603_MIFARE_AUTH_E2                  0x45 /**< \brief װ��E2��Կ */
#define  ZSN603_MIFARE_AUTH_KEY                 0x46 /**< \brief ֱ����Կ��֤ */
#define  ZSN603_MIFARE_READ                     0x47 /**< \brief Mifare���� */
#define  ZSN603_MIFARE_WRITE                    0x48 /**< \brief Mifare��д */
#define  ZSN603_MIFARE0_WRITE                   0x49 /**< \brief UltraLight��д */
#define  ZSN603_MIFARE_VALUE                    0x4A /**< \brief Mifare��ֵ���� */
#define  ZSN603_MIFARE_RESET                    0x4C /**< \brief ����λ */
#define  ZSN603_MIFARE_ACTIVE                   0x4D /**< \brief ������ */
#define  ZSN603_MIFARE_AUTO_DETECT              0x4E /**< \brief �Զ���� */
#define  ZSN603_MIFARE_GET_AUTO_DETECT          0x4F /**< \brief ���Զ�������� */
#define  ZSN603_MIFARE_SET_VALUE                0x50 /**< \brief ����ֵ���ֵ */
#define  ZSN603_MIFARE_GET_VALUE                0x51 /**< \brief ��ȡֵ���ֵ */
#define  ZSN603_MIFARE_EXCHANGE_BLOCK1          0x58 /**< \brief ���ݽ������� */
#define  ZSN603_MIFARE_MF0_CMD_TRANS            0x53 /**< \brief MF0��Ƭ��PCD֮�����ݽ�����չ���� */

/**
 * \name ISO7816-3Э���������
 *   ���� ZSN603  ISO7816-3Э����������
 * @{
 */
#define  ZSN603_CICC_TPDU                       0x42 /**< \brief �Ӵ�ʽIC������Э�飨�Զ�����T=0��T=1Э�� */
#define  ZSN603_CICC_COLD_RESET                 0x43 /**< \brief �Ӵ�ʽIC���临λ */
#define  ZSN603_CICC_WARM_RESET                 0x44 /**< \brief �Ӵ�ʽIC���ȸ�λ */
#define  ZSN603_CICC_DEACTIVATION               0x45 /**< \brief �Ӵ�ʽIC��ͣ��رյ�Դ��ʱ�ӣ� */
#define  ZSN603_CICC_TP0                        0x47 /**< \brief �Ӵ�ʽIC��T=0����Э�� */
#define  ZSN603_CICC_TP1                        0x48 /**< \brief �Ӵ�ʽIC��T=1����Э�� */

/**
 * \name ISO1443(PICC)Э���������
 *   ���� ZSN603  ISO1443(PICC)Э����������
 * @{
 */
#define  ZSN603_PICCA_REQUEST                   0x41 /**< \brief A�Ϳ����� */
#define  ZSN603_PICCA_ANTICOLL                  0x42 /**< \brief A�Ϳ�����ײ */
#define  ZSN603_PICCA_SELECT                    0x43 /**< \brief A�Ϳ�ѡ�� */
#define  ZSN603_PICCA_HALT                      0x44 /**< \brief A�Ϳ����� */
#define  ZSN603_PICCA_RATS                      0x45 /**< \brief A�Ϳ�RATS */
#define  ZSN603_PICCA_PPS                       0x46 /**< \brief A�Ϳ�PPS */
#define  ZSN603_PICCA_DESELECT                  0x47 /**< \brief DESELECT */
#define  ZSN603_PICCA_TPCL                      0x48 /**< \brief T=CL */
#define  ZSN603_PICCA_EXCHANGE_BLOCK2           0x4A /**< \brief ���ݽ���*/
#define  ZSN603_PICCA_RESET                     0x4C /**< \brief A�Ϳ���λ����֧��ISO14443-4, ���Զ�ִ��RATS��*/
#define  ZSN603_PICCA_ACTIVE                    0x4D /**< \brief A�Ϳ�����*/
#define  ZSN603_PICCB_ACTIVE                    0x4E /**< \brief B�Ϳ�����*/
#define  ZSN603_PICCB_RESET                     0x4F /**< \brief B�Ϳ���λ��ִ��ATTRIB��*/
#define  ZSN603_PICCB_REQUEST                   0x50 /**< \brief B�Ϳ�����*/
#define  ZSN603_PICCB_ATTRIB                    0x52 /**< \brief B�Ϳ��޸Ĵ������ԣ�ѡ�񿨣�*/
#define  ZSN603_PICCB_HALT                      0x53 /**< \brief B�Ϳ�����*/
#define  ZSN603_PICCB_GETUID                    0x54 /**< \brief ��ȡ����֤UID*/


/**
 * \name PLUS CPU���������
 *   ���� ZSN603  PLUS CPUЭ����������
 * @{
 */
#define  ZSN603_SL0_WRITE_PERSO                  0x42 /**< \brief SL0���˻����ݸ��� */
#define  ZSN603_SL0_COMMIT_PERSO                 0x43 /**< \brief SL0�ύ���˻� */
#define  ZSN603_SL3_FIRST_AUTH                   0x4A /**< \brief SL3�״���֤(ֱ����Կ��֤) */
#define  ZSN603_SL3_FIRST_AUTH_E2                0x4B /**< \brief SL3�״���֤(E2��Կ��֤) */
#define  ZSN603_SL3_FOLLOWING_AUTH               0x4C /**< \brief SL3������֤(ֱ����Կ��֤) */
#define  ZSN603_SL3_FOLLOWING_AUTH_E2            0x4D /**< \brief SL3������֤(E2��Կ��֤)*/
#define  ZSN603_SL3_RESET_AUTH                   0x4E /**< \brief SL3��λ��֤ */
#define  ZSN603_SL3_READ                         0x4F /**< \brief SL3�����ݿ� */
#define  ZSN603_SL3_WRITE                        0x50 /**< \brief SL3д���ݿ�*/
#define  ZSN603_SL3_VALUE                        0x53 /**< \brief SL3ֵ�����*/

/**
 * \name �豸   ��ʶ�궨��
 * ����ZSN603�豸�Ĺ���ʱ��һЩ��ر�ʶ
 * @{
 */
#define   ZSN603_EXECUTE_SUCCESS                 0x00 /**< \brief �豸�����ɹ�  */
#define   ZSN603_DEV_SERVER_ERR                  0xA1 /**< \brief �豸������� �豸Ϊ��  */
#define   ZSN603_FRAME_LENGHT_RX_ERR             0xA2 /**< \brief ��ȡ��Ӧ֡�ܳ��ȴ���  */
#define   ZSN603_FRAME_INFO_RX_ERR               0xA3 /**< \brief ��ȡ��Ӧ֡��Ϣ����  */
#define   ZSN603_FRAME_LENGHT_ERR                0xA4 /**< \brief ������ȡ��Ϣʱ,֡���ȴ���  */
#define   ZSN603_FRAME_NO_SLV_ADDRS_ERR          0xA5 /**< \brief ������ȡ��Ϣʱ,��local address����  */
#define   ZSN603_FRAME_CHECK_SUM_ERR             0xA6 /**< \brief ������ȡ��Ϣʱ,У��ʹ���  */
#define   ZSN603_IIC_TRANS_ERR                   0xA7 /**< \brief ����ʱ IIC ͨ�ŷ��ش���  */
#define   ZSN603_WORKMODE_INVALID                0xA8 /**< \brief ��������ʱ  �豸������Ч����ģʽ  */
#define   ZSN603_UART_RX_TIMEOUT                 0xA9 /**< \brief ���ڽ�������ʱ��ʱ  */
#define   ZSN603_I2C_SEMB_TIMEOUT                0xAA /**< \brief I2C�жϵȴ���ʱ  */
#define   ZSN603_INPUT_PARA_ERROR                0xAB /**< \brief ֡���������������  */
/**
 * \name �豸��������ģʽ
 * ����ZSN603�豸�Ĺ���ģʽ���á�
 * @{
 */
#define   ZSN603_WORK_MODE_IIC                   0x00 /**< \brief IIC����ģʽ  */
#define   ZSN603_WORK_MODE_UART                  0x01 /**< \brief UART����ģʽ  */

/**
 * \name �豸������ָ��  IC��Э�����ò����궨��
 * ����ZSN603�豸���豸������ָ���е�IC��Э����������
 * @{
 */
#define   ZSN603_ICC_ISO_TYPE_A                  0x00 /**< \brief iso14443-3AЭ��  */
#define   ZSN603_ICC_ISO_TYPE_B                  0x04 /**< \brief iso14443-3BЭ��  */

/**
 * \name �豸������ָ��  IC����Կ���Ͳ����궨��
 *   ����ZSN603�豸����Կ������ز���
 * @{
 */
#define   ZSN603_ICC_KEY_TYPE_A                  0x60 /**< \brief ��Կ A  */
#define   ZSN603_ICC_KEY_TYPE_B                  0x61 /**< \brief ��Կ B  */

/**
 * \name �豸������ָ��  ���������ò����궨��
 *   ����ZSN603�豸�Ĳ�����������ز�������
 * @{
 */
#define  ZSN603_BAUD_230400                      0x07 /**< \brief 230400 */
#define  ZSN603_BAUD_172800                      0x06 /**< \brief 172800 */
#define  ZSN603_BAUD_115200                      0x05 /**< \brief 115200 */
#define  ZSN603_BAUD_57600                       0x04 /**< \brief 57600 */
#define  ZSN603_BAUD_38400                       0x03 /**< \brief 38400 */
#define  ZSN603_BAUD_28800                       0x02 /**< \brief 28800 */
#define  ZSN603_BAUD_19200                       0x01 /**< \brief 19200 */
#define  ZSN603_BAUD_9600                        0x00 /**< \brief 9600 */

/**
 * \name �豸������ָ��  ����ģʽ���ò����궨��
 *   ����ZSN603�豸������ģʽ������ز�������
 * @{
 */
#define  ZSN603_ANT_MODE_TX12_LATERNATE          0x01 /**< \brief ����TX1��TX2�������� */
#define  ZSN603_ANT_MODE_TX1                     0x01 /**< \brief ���߽�TX1���� */
#define  ZSN603_ANT_MODE_TX2                     0x02 /**< \brief ���߽�TX2���� */
#define  ZSN603_ANT_MODE_TX12                    0x03 /**< \brief ����TX1��TX2ͬʱ���� */

/**
 * \name �豸������ָ��  ����LED���ò����궨��
 *   ����ZSN603�豸�Ŀ���LED������ز�������
 * @{
 */
#define  ZSN603_CONTROL_LED_ON                   0x01 /**< \brief LED�ر� */
#define  ZSN603_CONTROL_LED_OFF                  0x00 /**< \brief LED���� */

/**
 * \name A�� ���� ָ������궨��
 *   ����ZSN603�豸��A�������������
 * @{
 */
#define  ZSN603_MIFARE_REQUEST_IDLE              0x26 /**< \brief LED�ر� */
#define  ZSN603_MIFARE_REQUEST_ALL               0x52 /**< \brief LED���� */

/**
 * \name A�� �Զ��쿨ģʽ  �����궨��
 *   ����ZSN603�豸��A���Զ��쿨ģʽ�������
 * @{
 */
#define  ZSN603_AUTO_DETECT_EXECUTE_HALT         0x08 /**< \brief ���ִ��Halt() */
#define  ZSN603_AUTO_DETECT_CONTINUE             0x04 /**< \brief ��������ִ�� */
#define  ZSN603_AUTO_DETECT_INTERRUPT            0x02 /**< \brief �����ж� */
#define  ZSN603_AUTO_DETECT_SEND                 0x01 /**< \brief �������� */

#define  ZSN603_AUTO_DETECT_NO_AUTH              0x00 /**< \brief ��������֤ */
#define  ZSN603_AUTO_DETECT_E2_AUTH              0x45 /**< \brief ʹ��E2��Կ������֤ */
#define  ZSN603_AUTO_DETECT_KEY_AUTH             0x46 /**< \brief ʹ���û�������Կ������֤ */

/*****************************************************************
 *  ZSN603  ����ָ�����Ͷ���
 *****************************************************************/
/** \brief  ZSN603����亯��ָ�����Ͷ���*/
typedef uint8_t (*zsn603_cmd_tx_func_t) (void          *p_drv,
                                         uint8_t       *p_cmd_data,
                                         uint32_t       cmd_lenght);
/** \brief  ZSN603������ս�������ָ�����Ͷ���*/
typedef int (*zsn603_cmd_rx_analysis_func_t) (void      *p_drv,
                                              uint32_t   buffer_len,
                                              uint8_t   *p_data,
                                              uint32_t  *p_data_lenght);
/** \brief  ZSN603�жϻص�����ָ�����Ͷ���*/
typedef void (*zsn603_int_cb) (void   *p_arg);

/** \brief  �Զ����ģʽ�¼�⵽���жϻص�����ָ�����Ͷ���*/
typedef void (*zsn603_card_input) (void   *p_arg);


/** \brief ��Ӧ֡��Ϣ�ṹ�� */
typedef struct zsn603_frame_info {
    uint32_t  status;                      /** \brief ִ��״̬ */
    uint32_t  info_lenght;                 /** \brief ��Ϣ���� */
}zsn603_frame_info_t;

/** \brief ZSN603ͨ���豸  ����ʹ�ú���ͨ�ŷ�ʽ  ����ʹ�õ����豸*/
typedef struct zsn603_dev {
    uint8_t               work_mode;       /** \brief �豸��ַ  */
    uint8_t               local_addr;      /** \brief �豸��ַ  */

    /* �����ĸ���Ա�����Զ����ģʽ�»�ȡ���� */
    volatile uint8_t      status;          /** \brief �豸״̬*/
    uint8_t               is_continue;     /** \brief �Զ���� ��⵽�����Ƿ�������*/
    zsn603_card_input     pfn_card_cb;     /** \brief �Զ���� ��⵽������û��ص�*/
    void                 *p_arg;           /** \brief �Զ���� ��⵽������û��ص�����*/

    zsn603_cmd_tx_func_t  pfn_cmd_tx;      /** \brief ����亯��*/
    /** \brief ����亯��*/
    zsn603_cmd_rx_analysis_func_t pfn_cmd_rx_analysis;
}zsn603_dev_t;

/** \brief ����ZSN603�������  ���еĽӿں�������ʹ�õ��þ�� */
typedef zsn603_dev_t *   zsn603_handle_t;

/*****************************************************************
 *  ZSN603  UARTͨ�ŷ�ʽ�豸��ض���
 *****************************************************************/
/** \brief zsn603 �豸�ṹ��(UARTģʽ) */
typedef struct zsn603_uart_devinfo {
    uint8_t  local_addr;                                  /** \brief �豸��ַ  */
    zsn603_platform_uart_devinfo_t  platform_info;        /** \brief ƽ̨����豸��Ϣ  */
}zsn603_uart_devinfo_t;


/** \brief zsn603 �豸�ṹ��(UARTģʽ) */
typedef struct zsn603_uart_dev {
    zsn603_dev_t               zsn603_dev;                /** \brief ZSN603 �豸*/
    uint8_t                    is_wait ;                  /** \brief Ŀǰ�ȴ����ݵĸ���*/
    uint8_t                    rx_count;                  /** \brief ���յ������ݵĸ���*/
    /** \brief ���ڽ������ݻ�����*/
    uint8_t                    buffer[ZSN603_BUFFER_SIZE_MAX];
    ZSN603_SEMB_DECL(uart_semb);                          /** \brief �����ź��������ڻ�ȡ�������ݵĳ�ʱ*/
    zsn603_platform_uart_t     platform_dev;              /** \brief I2Cģʽƽ̨�豸*/
    }zsn603_uart_dev_t;

/*****************************************************************
 *  ZSN603 I2Cͨ�ŷ�ʽ�豸��ض���
 *****************************************************************/
/** \brief zsn603 �豸��Ϣ�ṹ��(I2Cģʽ) */
typedef struct zsn603_i2c_devinfo {
    uint8_t  local_addr;                          /** \brief �豸��ַ  */
    zsn603_platform_i2c_devinfo_t platform_info;  /** \brief ƽ̨����豸��Ϣ  */
}zsn603_i2c_devinfo_t;

/** \brief zsn603�豸�ṹ��(I2Cģʽ) */
typedef struct zsn603_i2c_dev {
    zsn603_dev_t              zsn603_dev;         /** \brief ZSN603 �豸*/
    ZSN603_SEMB_DECL(int_semb);                   /** \brief �ж��ź�������*/
    zsn603_platform_i2c_t     platform_dev;       /** \brief I2Cģʽƽ̨�豸*/
}zsn603_i2c_dev_t;

/**************************************************************
 * �Զ����ģʽ�ṹ���������
 **************************************************************/
/** \brief zsn603�Զ����ģʽ���ò����ṹ�嶨�� */
typedef struct zsn603_auto_detect_ctrl {
    uint8_t             ad_mode;         /** \brief �Զ����ģʽ(ADMode)*/
    uint8_t             tx_mode;         /** \brief ����������ʽ(TXMode)*/
    uint8_t             req_code;        /** \brief �������(ReqCode)*/
    uint8_t             auth_mode;       /** \brief ��֤ģʽ(AuthMode)*/
    uint8_t             key_type;        /** \brief ��Կ����(KeyType)*/
    uint8_t             key_num;         /** \brief E2��Կ����*/
    uint8_t            *p_key;           /** \brief ��Կ(Key)*/
    uint8_t             key_len;         /** \brief ��Կ����(KeyLen)*/
    uint8_t             block;           /** \brief ��֤�Ŀ����(Block)*/
    zsn603_card_input   pfn_card_input;  /** \brief ��⵽��ʱ�Ļص�����*/
    void               *p_arg;           /** \brief ��⵽��ʱ�Ļص���������*/
}zsn603_auto_detect_ctrl_t;

/** \brief zsn603�Զ����ģʽ���ò����ṹ�嶨�� */
typedef struct zsn603_auto_detect_data {
    uint8_t             ant_mode;        /** \brief ����ģʽ(TXMode)*/
    uint16_t            atq;             /** \brief ����Ӧ��(ATQ)*/
    uint8_t             sak;             /** \brief ѡ��Ӧ��(SAK)*/
    uint8_t             id_len;          /** \brief ���кų���(UIDLen)*/
    uint8_t             uid[10];         /** \brief ���к�(UID)*/
    uint8_t             data_len;        /** \brief �����ݳ���*/
    uint8_t             data[16];        /** \brief �����ݻ����ַ*/
}zsn603_auto_detect_data_t;
/**
 * \brief  ZSN603 ��ʼ��(UARTͨ��ģʽ)
 *
 * \param[in] p_dev           : ZSN603�豸
 * \param[in] local_address   : ZSN603�豸local address
 * \param[in] p_trans_arg     : ���亯������
 *
 * \retval NULL   : ��ʼ������
 * \retval handle : ִ�гɹ�
 */
zsn603_handle_t  zsn603_uart_init(zsn603_uart_dev_t            *p_dev,
                                  const zsn603_uart_devinfo_t  *p_devinfo);

/**
 * \brief  ZSN603 ���ʼ��(UARTͨ��ģʽ)
 *
 * \param[in] handle       : ZSN603���
 * \retval NULL   : ��ʼ������
 * \retval handle : ִ�гɹ�
 */
int  zsn603_uart_deinit(zsn603_handle_t handle);

/**
 * \brief  ZSN603 ��ʼ��(I2Cͨ��ģʽ)
 *
 * \param[in] p_dev           : ZSN603�豸
 * \param[in] p_devinfo       : ZSN603�豸�豸��Ϣ
 *
 * \retval NULL   : ��ʼ������
 * \retval handle : ִ�гɹ�
 */
zsn603_handle_t  zsn603_i2c_init(zsn603_i2c_dev_t           *p_dev,
                                 const zsn603_i2c_devinfo_t *p_devinfo);

/**
 * \brief  ZSN603 ���ʼ��(I2Cͨ��ģʽ)
 *
 * \param[in] handle       : ZSN603���
 * \retval NULL   : ��ʼ������
 * \retval handle : ִ�гɹ�
 */
int  zsn603_i2c_deinit(zsn603_handle_t handle);

/**
 * \brief  ��ZSN603 ��ʼ��ʹ��IICģʽ     ���뽫���ն˺�����Ϊ�ж����ŵĻص�����
 *
 *         ��IICģʽ�£�ZSN603�������ִ��֮���ж����Ž��ᱻ����
 *         �����û����Խ� int_pin ����Ϊ�½��ش����������������жϻص�����Ϊ�˺�������������Ϊ ZSN603 �豸��� ����
 *
 * \param[in] p_arg           : �жϻص���������
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
void zsn603_int_pin_hander(void *p_arg);


/**
 * \brief ��ȡ�豸��Ϣ  ���᷵�ع̼��汾��
 *
 * \param[in]  handle          : ZSN603�豸������
 * \param[in]  buffer_len      : ���뻺�����Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[out] p_rx_data       : �����豸��Ϣ�Ļ�����
 * \param[out] p_rx_data_count : ���յ����豸��Ϣ
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_get_device_info(zsn603_handle_t      handle,
                               uint32_t             buffer_len,
                               uint8_t             *p_rx_data,
                               uint32_t            *p_rx_data_count);


/**
 * \brief ����IC���ӿ�
 *
 * \param[in] handle  : ZSN603�豸������
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_config_icc_interface(zsn603_handle_t      handle);

/**
 * \brief �ر�IC���ӿ�
 *
 * \param[in] handle  : ZSN603�豸������
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_close_icc_interface(zsn603_handle_t      handle);

/**
 * \brief ����IC��Э��
 *
 * \param[in] handle       : ZSN603�豸������
 * \param[in] isotype      : Э������   ZSN603_ICC_ISO_TYPE_A
 *                                 ZSN603_ICC_ISO_TYPE_B
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_set_ios_type(zsn603_handle_t  handle,
                            uint8_t          isotype);
/**
 * \brief װ��IC����Կ
 *
 * \param[in] handle      : ZSN603�豸������
 * \param[in] key_type    : ��Կ����(1�ֽ�):   0x60������ԿA
 *                                        0x61������ԿB
 * \param[in] key_block   :��Կ����(1�ֽ�):  ȡֵ��Χ0~15
 * \param[in] p_key       :��Կ(6�ֽڻ�16�ֽ�)
 * \param[in] key_length  : ��Կ����     6 /16
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_load_icc_key(zsn603_handle_t  handle,
                            uint8_t          key_type,
                            uint8_t          key_block,
                            uint8_t         *p_key,
                            uint32_t         key_length);

/**
 * \brief ����IC���ӿڵļĴ���ֵ
 *
 * \param[in] handle      : ZSN603�豸������
 * \param[in] reg_addr    : �Ĵ�����ַ(1�ֽ�):  0x00~0x3F
 * \param[in] reg_val     : �Ĵ���ֵ(1�ֽ�)
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_set_icc_reg(zsn603_handle_t handle,
                           uint8_t         reg_addr,
                           uint8_t         reg_val);

/**
 * \brief ��ȡIC���ӿڵļĴ���ֵ
 *
 * \param[in] handle      : ZSN603�豸������
 * \param[in] p_info      : �Ĵ�����ַ(1�ֽ�):  0x00~0x3F
 * \param[out] p_val      : �Ĵ���ֵ(1�ֽ�)
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_get_icc_reg(zsn603_handle_t  handle,
                           uint8_t          reg_addr,
                           uint8_t         *p_val);

/**
 * \brief ���ô���ģʽ����������
 *
 * \param[in] handle            : ZSN603�豸������
 * \param[in] baudrate_flag     : �����ʱ��(1�ֽ�):
 *                      ZSN603_BAUD_172800     ZSN603_BAUD_115200
 *                      ZSN603_BAUD_57600      ZSN603_BAUD_38400
 *                      ZSN603_BAUD_28800      ZSN603_BAUD_19200
 *                      ZSN603_BAUD_230400     ZSN603_BAUD_9600
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�   ע
 *  ��Ҫֵ��ע��    ��ִ�иú����ɹ�֮���û��������ڶ�Ӧ�Ĳ�����
 */
uint8_t zsn603_set_baud_rate(zsn603_handle_t  handle,
                             uint8_t          baudrate_flag);

/**
 * \brief ������������ģʽ
 *
 * \param[in] handle        : ZSN603�豸������
 * \param[in] antmode_flag  : ��������ģʽ(1�ֽ�):
 *                ZSN603_ANT_MODE_TX12_LATERNATE     ZSN603_ANT_MODE_TX1
 *                ZSN603_ANT_MODE_TX2                ZSN603_ANT_MODE_TX12
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_set_ant_mode(zsn603_handle_t  handle,
                            uint8_t          antmode_flag);

/**
 * \brief �л�����ͨ��
 *
 * \param[in] handle       : ZSN603�豸������
 * \param[in] ant_channel  : ���߱��(1�ֽ�): 0x00~0x07
 *
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_set_ant_channel(zsn603_handle_t  handle,
                               uint8_t          ant_channel);

/**
 * \brief  �����豸�ӻ���ַ
 *
 * \param[in] handle      : ZSN603�豸������
 * \param[in] slv_addr    : �ӻ���ַ(1�ֽ�)
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�  ��ִ�гɹ���  ZSN603�豸��ַ���Զ���Ϊ�µ����õ�ֵ
 */
uint8_t zsn603_set_local_addr(zsn603_handle_t  handle,
                              uint8_t          slv_addr);

/**
 * \brief  LED�ƿ���
 *
 * \param[in] handle      : ZSN603�豸������
 * \param[in] control_led : ����ָ��(1�ֽ�)  :ZSN603_CONTROL_LED_ON
 *                                            ZSN603_CONTROL_LED_OFF
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_control_led(zsn603_handle_t    handle,
                           uint8_t            control_led);

/**
 * \brief  ����������
 *
 * \param[in] handle       : ZSN603�豸������
 * \param[in] control_byte : ����ָ��(1�ֽ�)   ����λ�������д���    ǰ��λ��������ʱ��
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_control_buzzer(zsn603_handle_t  handle,
                              uint8_t           control_byte);

/**
 * \brief  ��EEPROM
 *
 * \param[in]  handle      : ZSN603�豸������
 * \param[in]  eeprom_addr : E2PROMַַ(1�ֽ�):��Χ0~255
 * \param[in]  nbytes:     : ��ȡ���ݵĳ���(1�ֽ�)
 * \param[in]  buffer_len  : ���뻺�����Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[out] p_buf       : �������ݵĻ�����
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_read_eeprom(zsn603_handle_t  handle,
                           uint8_t          eeprom_addr,
                           uint8_t          nbytes,
                           uint32_t         buffer_len,
                           uint8_t         *p_buf);

/**
 * \brief  дEEPROM
 *
 * \param[in] handle       : ZSN603�豸������
 * \param[in] eeprom_addr  : E2PROMַַ(1�ֽ�):��Χ0~255
 * \param[in] nbytes       : д���ݵĳ���(1�ֽ�)
 * \param[in] p_buf        : Ҫд���������Ϣ(nbytes�ֽ�)
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_write_eeprom(zsn603_handle_t      handle,
                                  uint8_t        eeprom_addr,
                                  uint8_t        nbytes,
                                  uint8_t       *p_buf);

/**
 * \brief  ����Mifare�����������
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] req_mode   : ����ģʽ(1�ֽ�): 0x26����IDLEģʽ
 *                                      0x52����ALLģʽ
 * \param[out] p_atq     : ���ص�atq��Ϣ    (2�ֽ�)
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_mifare_request(zsn603_handle_t   handle,
                              uint8_t           req_mode,
                              uint16_t         *p_atq);

/**
 * \brief  ����Mifare���ķ���ײ����,��Ҫִ�гɹ�һ����������,����������ɹ�,���ܽ��з���ײ����,���򷵻ش���
 *
 * \param[in] handle         : ZSN603�豸������
 * \param[in] anticoll_level : ����ײ�ȼ�(1�ֽ�):  0x93������һ������ײ
 *                                            0x95�����ڶ�������ײ
 *                                            0x97��������������ײ
 * \param[in] p_know_uid     : ���к�(n�ֽ�):  (��λ������0)
 * \param[in] nbit_cnt       : λ����(1�ֽ�):   ��֪�����кŵĳ���n
 * \param[out] p_uid         : ���ص�UID��4�ֽڣ����ֽ����ȣ�����UID��������������ֽ�Ϊ������־0x88����Ҫ���и���һ���ķ���ײ��
 * \param[out] p_uid_cnt     : ���ص�UID����
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_mifare_anticoll(zsn603_handle_t  handle,
                              uint8_t           anticoll_level,
                              uint8_t          *p_know_uid,
                              uint8_t           nbit_cnt,
                              uint8_t          *p_uid,
                              uint32_t         *p_uid_cnt);

/**
 * \brief  ����������Mifare����ѡ�������
 *
 *   ��Ҫ�ɹ�ִ��һ�η���ײ��������سɹ������ܽ��п�ѡ����������򷵻ش���
 *
 * \param[in]  handle     : ZSN603�豸������
 * \param[in]  p_info     : ѡ�����(1�ֽ�)   0x93������һ������ײ
 *                                        0x95�����ڶ�������ײ
 *                                        0x97��������������ײ
 * \param[in]  p_uid      : UID(4�ֽ�)      ǰһ������ײ����ص�UID
 * \param[out] p_sak      : ���ص���Ϣ����bit2Ϊ1�������UID������
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_mifare_select(zsn603_handle_t  handle,
                             uint8_t          anticoll_level,
                             uint8_t         *p_uid,
                             uint8_t         *p_sak);

/**
 * \brief  ����������Mifare���Ĺ������,ʹ��ѡ��Ŀ�����HALT״̬,��HALT״̬��,
 *         ��������Ӧ������������IDLEģʽ������,���ǽ�����λ���뿪���߸�Ӧ�����ٽ��롣
 *         ��������Ӧ������������ALL����
 *
 * \param[in] handle      : ZSN603�豸������
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */

uint8_t zsn603_mifare_halt(zsn603_handle_t      handle);
/**
 * \brief  ��������оƬ�ڲ��Ѵ������Կ�뿨����Կ������֤,����ʹ�ø�����ǰ,
 *         Ӧ�����á�װ��IC����Կ���������Կ�ɹ�����оƬ��,
 *         ����,��Ҫ��֤�Ŀ��������Ų�����оƬ����Կ������ȡ�
 * \param[in] smcseq     : ��ȫ����/����
 * \param[in] p_info     : ��Կ����(1�ֽ�)  - 0x60������ԿA
 *                                      - 0x61������ԿB
 * \param[in] p_uid      : �����кţ�4�ֽ�
 * \param[in] key_sec    : ��Կ����:0 ~ 7
 * \param[in] nblock     : ��Ҫ��֤�Ŀ���ţ�ȡֵ��Χ�뿨�����йأ�
 *                         - S50:0 ~ 63
 *                         - S70:0 ~ 255
 *                         - PLUS CPU 2K:0 ~ 127
 *                         - PLUS CPU 4K:0 ~ 255
 * \param[in] data_count : ���ݳ���      7
 * \param[out] p_rx_frame_info : �����Ӧ֡�Ľṹ��
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_eeprom_auth(zsn603_handle_t  handle,
                           uint8_t          key_type,
                           uint8_t         *p_uid,
                           uint8_t          key_sec,
                           uint8_t          nblock);
/**
 * \brief  �����������Ϊ��������,����ڴ�֮ǰ�����á�װ��IC����Կ�����
 *         ����ǰ��ΪPLUS CPU���ĵȼ�2��ȼ�3,�����������ֻ��6�ֽ�,
 *         ��оƬ�Զ�����������븴��2��,ȡǰ16�ֽ���Ϊ��ǰ��֤��Կ��
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] key_type   : ��Կ���ͣ�����ʹ������ֵ��
 *                          - ZSN603_ICC_ISO_TYPE_A
 *                          - ZSN603_ICC_ISO_TYPE_B
 * \param[in] p_uid      : �����кţ�4�ֽ�
 * \param[in] p_key      : ��Կ������
 * \param[in] key_len    : ��Կ�ĳ��ȣ�ֻ��Ϊ6��6�ֽ���Կ����16��16�ֽ���Կ��
 * \param[in] nblock     : ��Ҫ��֤�Ŀ���ţ�ȡֵ��Χ�뿨�����йأ�
 *                         - S50��0 ~ 63
 *                         - S70��0 ~ 255
 *                         - PLUS CPU 2K��0 ~ 127
 *                         - PLUS CPU 4K��0 ~ 255
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_key_auth(zsn603_handle_t   handle,
                        uint8_t           key_type,
                        uint8_t          *p_uid,
                        uint8_t          *p_key,
                        uint8_t           key_len,
                        uint8_t           nblock);


/**
 * \brief  �������Mifare�����ж�����,��֮ǰ����ɹ�������Կ��֤��
 *
 * \param[in] handle      : ZSN603�豸������
 * \param[in] smcseq      : ��ȫ����/����
 * \param[in] nblock      : �����(1�ֽ�) - S50(0~63)
 *                                     - S70(0~255)
 *                                     - PLUS CPU 2K(0~127)
 *                                     - PLUS CPU 4K(0~255)
 * \param[in]  buffer_len : ���뻺�����Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[out] p_buf      : �����ȡ�Ŀ������
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_mifare_read(zsn603_handle_t  handle,
                           uint8_t          nblock,
                           uint32_t         buffer_len,
                           uint8_t         *p_buf);

/**
 * \brief  �������Mifare������д����,д֮ǰ����ɹ�������Կ��֤��
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] p_info     : �����(1�ֽ�)  - S50(0~63)
 *                                       S70(0~255)
 *                                       PLUS CPU 2K(0~127)
 *                                       PLUS CPU 4K(0~255)
 * \param[in] p_buf      : ��д�������(16�ֽ�)
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_mifare_write(zsn603_handle_t  handle,
                            uint8_t          nblock,
                            uint8_t         *p_buf);

/**
 * \brief  �������UltraLight������д������
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] nblock     : �����(1�ֽ�):1~15
 * \param[in] p_buf      : ��д�������(4�ֽ�)
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_ultralight_write(zsn603_handle_t   handle,
                                uint8_t           nblock,
                                uint8_t          *p_buf);

/**
 * \brief  �������Mifare����ֵ����мӼ�������д��ָ����
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] smcseq     : ��ȫ����/����
 * \param[in] mode       : ģʽ(1�ֽ�):    0xC0~��
 *                                      0xC1~��
 * \param[in] nblock     : �����(1�ֽ�):  S50(0~63)
 *                                      S70(0~255)
 *                                      PLUS CPU 2K(0~127)
 *                                      PLUS CPU 4K(0~255)
 * \param[in] value      : ֵ(4�ֽ��з�����,���ֽ�����)
 * \param[in] ntransblk  : ������(1�ֽ�)
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_mifare_value(zsn603_handle_t   handle,
                            uint8_t           mode,
                            uint8_t           nblock,
                            uint8_t           ntransblk,
                            uint32_t          value);

/**
 * \brief  ��������ͨ�����ز��źŹر�ָ����ʱ��,�ٿ�����ʵ�ֿ�Ƭ��λ��
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] time_ms    : ʱ��(1�ֽ�),�Ժ���Ϊ��λ,0Ϊһֱ�ر�
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_card_reset(zsn603_handle_t  handle,
                                uint8_t    time_ms);

/**
 * \brief  ���������ڼ��Ƭ,�����󡢷���ײ��ѡ�������������ϡ�
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] req_mode   : �������(1�ֽ�):  0x26~IDLE
 *                                       0x52~ALL
 *
 * \param[out] p_atq     : ���ص�atq��Ϣ
 * \param[out] p_saq     : ���ص�saq��Ϣ
 * \param[out] p_len     : ���ص�UID����
 * \param[out] p_uid     : ���ص�UID
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_mifare_card_active(zsn603_handle_t   handle,
                                  uint8_t           req_mode,
                                  uint16_t         *p_atq,
                                  uint8_t          *p_saq,
                                  uint8_t          *p_len,
                                  uint8_t          *p_uid);

/**
 * \brief  ���������ڿ�Ƭ���Զ����,ִ�и�����ɹ���,��UARTģʽ��,оƬ���������Ͷ�ȡ����Ƭ�����ݡ�
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] smcseq     : ��ȫ����/����
 * \param[in] admode     : �Զ����ģʽADMode�����ֽ�(1�ֽ�)
 *                bit7 ~ bit4:����
 *                bit3       :ִ����֮��Ķ���.
 *                            0-�޶���;         1 - ���ִ��HALT����(ZSN603_AUTO_DETECT_EXECUTE_HALT)
 *                bit2       :���������.
 *                            0-���������;      1 - �������(ZSN603_AUTO_DETECT_CONTINUE)
 *                bit1       :��UART�ӿڼ�⵽�п�ʱ���Ƿ�����ж�,I2Cģʽ������Ч���ض�������ж�
 *                            0-�������ж�;      1 - �����ж�(ZSN603_AUTO_DETECT_INTERRUPT)
 *                bit0       :��UART�ӿ�ʱ��⵽�п�ʱ���Ƿ�������������,I2Cģʽ������Ч,�ӻ�����������������
 *                            0��������������;    1 - ������������(ZSN603_AUTO_DETECT_SEND)
 * \param[in] txmode     : ����ģʽTxMode�����ֽ�.   - 0x00:Tx1 Tx2 ��������
 *                                              - 0x01:��Tx1����
 *                                              - 0x02:��Tx2����
 *                                              - 0x03:Tx1 Tx2 ͬʱ����
 * \param[in] reqcode    : �������ReqCode(1�ֽ�)   - 0x26~IDLE
 *                                              - 0x52~ALL
 * \param[in] authmode   : ��֤ģʽAuthMode(1�ֽ�)  -'E'~��E2��Կ��֤ (ZSN603_AUTO_DETECT_E2_AUTH)
 *                                              -'F'~��ֱ����Կ��֤(ZSN603_AUTO_DETECT_KEY_AUTH)
 *                                              - 0 ~����֤  (ZSN603_AUTO_DETECT_NO_AUTH)
 * \param[in] key_type   : ��ԿAB KeyType(1�ֽ�):   0x60~��ԿA  (ZSN603_ICC_KEY_TYPE_A)
 *                                               0x61~��ԿB (ZSN603_ICC_KEY_TYPE_B)
 * \param[in] p_key      : ��ԿKey:  ����֤ģʽΪ'E',��Ϊ��Կ����(1�ֽ�)
 *                                  ����֤ģʽΪ'F',��Ϊ��Կ(6��16�ֽ�)
 * \param[in] block      : ��Կ����   (6/16�ֽ�)
 * \param[in] block      : �����Block(1�ֽ�): S50(0~63)
 *                                          S70(0~255)
 *                                          PLUS CPU 2K(0~127)
 *                                          PLUS CPU 4K(0~255)
 *
 * \param[in] data_count : ����֤ģʽ = 'E', ��Ϊ7
 *                         ����֤ģʽ = 'F', ��Ϊ12��22
 *                         ����֤ģʽ =  0,  ��Ϊ4
 * \param[out] p_rx_frame_info : �����Ӧ֡�Ľṹ��
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_auto_detect(zsn603_handle_t              handle,
                           zsn603_auto_detect_ctrl_t   *p_ctrl);
/**
 * \brief  ���������ڶ�ȡ�Զ���������,�ر��ʺ���I2Cͨ��ģʽ��ʹ�á�
 *         ͨ���ö�ȡ�Զ������������,���Ծ�����ȡ���ݺ��Ƿ������⡣
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] ctrl_mode  : ��ģʽ(1�ֽ�) - 0x00 ȡ�����
 *                                    - 0x01 �������
 * \param[out] p_data    : ���淵�����ݵĽṹ��ָ��
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_get_auto_detect(zsn603_handle_t             handle,
                               uint8_t                     ctrl_mode,
                               zsn603_auto_detect_data_t  *p_data);

/**
 * \brief  ��������������ֵ���ֵ��
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] smcseq     : ��ȫ����/����
 * \param[in] block      : ���ַ(1�ֽ�):��Ҫд����ֵ�Ŀ��ַ
 * \param[in] data       : ��ֵ(4�ֽ�):�з��ŵ�32λ����,���ֽ���ǰ
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_mifare_set_value(zsn603_handle_t  handle,
                                uint8_t          block,
                                int              data);
/**
 * \brief  ���������ڻ�ȡֵ���ֵ,ֵ�����������ֻ���ǰ���ֵ��ʽ�洢ʱ,����ͨ���������ȡ�ɹ�,���򷵻�ʧ�ܡ�
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] block      : ���ַ(1�ֽ�):��Ҫ��ȡ��ֵ�Ŀ��ַ
 * \param[out] p_value   : �����ȡֵ��ָ��
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_mifare_get_value(zsn603_handle_t  handle,
                                uint8_t          block,
                                int             *p_value);

/**
 * \brief  ����������оƬ��չ����,����оƬ��Ƭ�������ⳤ����ϵ����ݴ�,
 *         �������NXP���Ƴ���NTAG213F������Ultralight Cϵ�п�Ƭ,
 *         ���Ǹÿ�Ƭ����������������ݶ�д��Կ�������ܡ��������Կ��֤��������ô���������������ʵ�֡���
 *
 * \param[in]  handle       : ZSN603�豸������
 * \param[in]  len         : ���ݳ���(1�ֽ�):ʵ�����ݳ���
 * \param[in]  p_data_buf  : ����(n-1�ֽ�):ʵ�ʴ�����������ݴ�
 * \param[in]  data_count  : 0x02
 * \param[in]  wtxm_crc    : b7~b2,wtxm��b1,RFU(0); b0,0,CRC��ֹ�ܣ�1,CRCʹ�ܡ�
 * \param[in]  fwi         : ��ʱ�ȴ�ʱ�����, ��ʱʱ��=����0x01 << fwi��ms
 * \param[in]  buffer_len  : ���뻺�����Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[out] p_rx_buf    : ���ݽ�����Ӧ֡��Ϣ
 * \param[out] p_real_len  : ���ڻ�ȡ���յ���Ϣ����
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_mifare_exchange_block(zsn603_dev_t  *handle,
                                     uint8_t       *p_data_buf,
                                     uint8_t        len,
                                     uint8_t        wtxm_crc,
                                     uint8_t        fwi,
                                     uint32_t       buffer_len,
                                     uint8_t       *p_rx_buf,
                                     uint32_t      *p_len);
/**
 * \brief  �������ö�д���뿨Ƭ�����ݽ���,ͨ�����������ʵ�ֶ�д���������й��ܡ�
 *
 * \param[in] handle      : ZSN603�豸������
 * \param[in] p_tx_buf    : �������ݵĻ�����
 * \param[in] tx_nbytes   : �����ֽ���
 * \param[in]  buffer_len : ���뻺�����Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[in] p_rx_buf    : �������ݵĻ�����
 * \param[in] p_rx_nbytes : �����ֽ���
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_mifare_cmd_trans(zsn603_handle_t   handle,
                                uint8_t          *p_tx_buf,
                                uint8_t           tx_nbytes,
                                uint32_t          buffer_len,
                                uint8_t          *p_rx_buf,
                                uint32_t         *p_rx_nbytes);
/**
 * \brief  ��������ݽӴ�ʽIC���ĸ�λ��Ϣ,�Զ�ѡ��T = 0��T = 1����Э��,�������̲���Ҫʹ���߸�Ԥ�����������ڴ���APDU��������
 *
 * \param[in] handle        : ZSN603�豸������
 * \param[in] p_tx_buf      : ���ͻ�����
 * \param[in] tx_bufsize    : ���ͻ������ĳ��ȣ� 1 ~ 272
 * \param[in] buffer_len    : ���ջ������Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[in] p_rx_buf      : ���ջ�����
 * \param[in] p_rx_len      : �������ݵĳ���
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_cicc_tpdu(zsn603_handle_t   handle,
                         uint8_t          *p_tx_buf,
                         uint32_t          tx_bufsize,
                         uint32_t          buffer_len,
                         uint8_t          *p_rx_buf,
                         uint32_t         *p_rx_len);

/**
 * \brief  ���������临λ,ִ���˽Ӵ�ʽIC���ϵ�ʱ��,ִ�гɹ�����Զ�����IC���ĸ�λ��Ϣ��ѡ��B������ʹ�õĴ���Э��(T = 0 ��T = 1)��
 *
 * \param[in] handle        : ZSN603�豸������
 * \param[in] buffer_len    : ��λ��Ϣ�������Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[in] p_rx_buf      : ��Ÿ�λ��Ϣ�Ļ�����
 * \param[in] p_rx_len      : ��ȡ��λ��Ϣ���ȵ�ָ�롣
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_cicc_cold_reset(zsn603_handle_t   handle,
                               uint32_t          buffer_len,
                               uint8_t          *p_rx_buf,
                               uint32_t         *p_rx_len);

/**
 * \brief  ���������ȸ�λ,û��ִ���˽Ӵ�ʽIC���ϵ�ʱ��,
 *         ִ�гɹ�����Զ�����IC���ĸ�λ��Ϣ��ѡ��B������ʹ�õĴ���Э��(T = 0 ��T = 1)��
 *         ��������临λ��Ƚ�ֻ��û��ִ��IC���ϵ������
 *
 * \param[in] handle        : ZSN603�豸������
 * \param[in] buffer_len    : ��λ��Ϣ�������Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[in] p_rx_buf      : ��Ÿ�λ��Ϣ�Ļ�����
 * \param[in] p_rx_len      : ��ȡ��λ��Ϣ���ȵ�ָ�롣
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_cicc_warm_reset(zsn603_handle_t    handle,
                               uint32_t           buffer_len,
                               uint8_t           *p_rx_buf,
                               uint32_t          *p_rx_len);

/**
 * \brief  �������ǹرսӴ�ʽIC���ĵ�Դ��ʱ�ӡ�
 *
 * \param[in] handle      : ZSN603�豸������
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_cicc_deactivation(zsn603_handle_t  handle);

/**
 * \brief  ����������T = 0����Э�顣
 *
 * \param[in]  handle        : ZSN603�豸������
 * \param[in]  p_tx_buf      : ���ͻ�����
 * \param[in]  tx_bufsize    : ���ͻ������ĳ��ȣ� 1 ~ 272
 * \param[in]  buffer_len    : ���ջ������Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[out] p_rx_buf      : ���ջ�����
 * \param[out] p_rx_len      : �������ݵĳ���
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_cicc_tp0(zsn603_handle_t    handle,
                        uint8_t           *p_tx_buf,
                        uint32_t           tx_bufsize,
                        uint32_t           buffer_len,
                        uint8_t           *p_rx_buf,
                        uint32_t          *p_rx_len);

/**
 * \brief  ����������T=1����Э�顣
 *
 * \param[in]  handle        : ZSN603�豸������
 * \param[in]  p_tx_buf      : ���ͻ�����
 * \param[in]  tx_bufsize    : ���ͻ������ĳ��ȣ� 1 ~ 272
 * \param[in]  buffer_len    : ���ջ������Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[out] p_rx_buf      : ���ջ�����
 * \param[out] p_rx_len      : �������ݵĳ���
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_cicc_tp1(zsn603_handle_t   handle,
                        uint8_t          *p_tx_buf,
                        uint32_t          tx_bufsize,
                        uint32_t          buffer_len,
                        uint8_t          *p_rx_buf,
                        uint32_t         *p_rx_len);

/**
 * \brief  ����������A�Ϳ����������,������Ĳ�����Mifare S50/S70������������ʱһ���ġ�
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] req_mode   : ����ģʽ(1�ֽ�): 0x26����IDLEģʽ
 *                                      0x52����ALLģʽ
 * \param[out] p_atq     : ���ص�atq��Ϣ    (2�ֽ�)
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_picca_request(zsn603_handle_t  handle,
                             uint8_t          req_mode,
                             uint16_t        *p_atq);

/**
 * \brief  ����������A�Ϳ��ķ���ײ����,������Ĳ�����Mifare S50/S70����ķ���ײ����ʱһ�µġ�
 *
 * \param[in] handle         : ZSN603�豸������
 * \param[in] anticoll_level : ����ײ�ȼ�(1�ֽ�):  0x93������һ������ײ
 *                                            0x95�����ڶ�������ײ
 *                                            0x97��������������ײ
 * \param[in] p_know_uid     : ���к�(n�ֽ�):  (��λ������0)
 * \param[in] nbit_cnt       : λ������������֪�����к�uid��λ������ֵС��32��
 *                               - ��nbit_cntֵΪ0,�����û����֪���кţ����кŵ�����λ
 *                                 ��Ҫ�ӱ�������á�
 *                               - ��nbit_cnt��Ϊ0�������к�������֪�����кŵ�ֵ����ʾ
 *                                 ���кŵ�ǰnbit_cntλΪ��֪���к�����ʾǰnbit_cntλ��
 *                                 ֵ������λ��Ҫ��������á�
 * \param[in]  buffer_len    : ����UID�������Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[out] p_uid         : �������ײ���ȡ���Ŀ����кŵĻ���
 * \param[out] p_uid_cnt     : ���ص�UID����
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_picca_anticoll(zsn603_handle_t   handle,
                              uint8_t           anticoll_level,
                              uint8_t          *p_know_uid,
                              uint8_t           nbit_cnt,
                              uint32_t          buffer_len,
                              uint8_t          *p_uid,
                              uint32_t         *p_uid_cnt);

/**
 * \brief  ����������A�Ϳ���ѡ��,������Ĳ�����Mifare S50/S70�����ѡ�����ʱһ�µġ�
 *
 * \param[in]  handle     : ZSN603�豸������
 * \param[in]  p_info     : ѡ�����(1�ֽ�) :  0x93������һ������ײ
 *                                         0x95�����ڶ�������ײ
 *                                         0x97��������������ײ
 * \param[in]  p_uid      : UID(4�ֽ�)   : ǰһ������ײ����ص�UID
 * \param[in]  uid_cnt    : UID����(4�ֽ�): ǰһ������ײ����ص�UID�ĳ���
 * \param[out] p_sak      : ���ص���Ϣ����bit2Ϊ1�������UID������
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_picca_select(zsn603_handle_t  handle,
                            uint8_t          anticoll_level,
                            uint8_t         *p_uid,
                            uint8_t          uid_cnt,
                            uint8_t         *p_sak);

/**
 * \brief  ����������A�Ϳ��Ĺ���,������Ĳ�����Mifare S50/S70����Ĺ�������һ�¡�
 *
 * \param[in] handle      : ZSN603�豸������
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_picca_halt(zsn603_handle_t      handle);

/**
 * \brief  RATS(request for answer to select)��ISO14443-4Э�������,
 *         оƬ����RATS,��Ƭ����ATS(answer to select)��ΪRATS��Ӧ��,��ִ�и�����ǰ,
 *         �����Ƚ���һ�ο�ѡ�����,��ִ�й�һ��RATS�����,���ٴ�ִ��RATS����,�����Ƚ�����
 *
 * \param[in]  handle     : ZSN603�豸������
 * \param[in]  cid        : CID(1�ֽ�):  ����ʶ��(card IDentifier,ȡֵ��Χ0x00~0x0E)
 * \param[in]  buffer_len : ��Ž������ݻ������Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[out] p_ats_buf  : ���ڽ��ջ�Ӧ����Ϣ����ͬ�����ֽ������в�ͬ
 * \param[in]  p_rx_len   : ���ڻ�ȡ���յ���Ϣ����
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_picca_rats(zsn603_handle_t   handle,
                          uint8_t           cid,
                          uint32_t          buffer_len,
                          uint8_t          *p_ats_buf,
                          uint32_t         *p_rx_len);

/**
 * \brief  PPS(protocal and parameter selection)��ISO14443-4Э�������,
 *         ���ڸı��йص�ר��Э�����,������Ǳ����,����ֻ֧��Ĭ�ϲ���,��������Ĳ�������Ϊ0���ɡ�
 *         ��ִ�и�����ǰ,�����ȳɹ�ִ��һ��RATS���
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] smcseq     : ��ȫ����/����
 * \param[in] dsi_dri    : DSI_DRI(1�ֽ�): оƬ�뿨ͨ�Ų�����,����Ϊ0(106Kb/s)
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_picca_pps(zsn603_handle_t  handle,
                         uint8_t          dsi_dri);

/**
 * \brief  ��������ISO14443-4Э�������,���ڽ���Ƭ��Ϊ����(HALT)״̬,
 *         ���ڹ���(HALT)״̬�Ŀ������á���������(�������ΪALL)�����¼��,ֻ��ִ�С�RATS������Ŀ����ø����
 *
 * \param[in] handle      : ZSN603�豸������
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_picca_deselect(zsn603_handle_t      handle);


/**
 * \brief  T=CL�ǰ�˫�����鴫��Э��,ISO14443-4Э������,���ڶ�д���뿨Ƭ֮������ݽ���,
 *         һ�����ISO14443Э���CPU�����ø�Э�����д��ͨ�š�
 *         ���ø�����ʱֻ��Ҫ��CPU��COS�����������Ϊ���뼴��,��������������͡�����ʶ��CID��֡�ȴ�ʱ��FWT��
 *         �ȴ�ʱ����չ��������WTXM(waiting time extensionmultiplier)�ȵ��ɸ������Զ���ɡ�
 *
 * \param[in] handle      : ZSN603�豸������
 * \param[in] p_cos_buf   : COS����
 * \param[in] cos_bufsize : COS����ĳ���
 * \param[in]  buffer_len : ��Ž�����Ϣ�������Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[out] p_res_buf  : ���ڽ��ջ�Ӧ����Ϣ����ͬCOS����ֽ������в�ͬ
 * \param[out] p_rx_len   : ���ڻ�ȡ���յ���Ϣ����
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_picca_tpcl(zsn603_handle_t   handle,
                          uint8_t          *p_cos_buf,
                          uint8_t           cos_bufsize,
                          uint32_t          buffer_len,
                          uint8_t          *p_res_buf,
                          uint32_t         *p_rx_len);

/**
 * \brief  �������ö�д���뿨Ƭ�����ݽ���,ͨ�����������ʵ�ֶ�д���������й��ܡ�
 *
 * \param[in]  handle      : ZSN603�豸������
 * \param[in]  len         : ���ݳ���(1�ֽ�):ʵ�����ݳ���
 * \param[in]  p_data_buf  : ����(n-1�ֽ�):ʵ�ʴ�����������ݴ�
 * \param[in]  data_count  : 0x02
 * \param[in]  wtxm_crc    : b7~b2,wtxm��b1,RFU(0); b0,0,CRC��ֹ�ܣ�1,CRCʹ�ܡ�
 * \param[in]  fwi         : ��ʱ�ȴ�ʱ�����, ��ʱʱ��=((0x01 << fwi)ms)
 * \param[in]  buffer_len  : �����Ϣ�������Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[out] p_rx_buf    : ���ݽ�����Ӧ֡��Ϣ
 * \param[out] p_real_len  : ���ڻ�ȡ���յ���Ϣ����
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_picca_exchange_block(zsn603_handle_t   handle,
                                    uint8_t          *p_data_buf,
                                    uint8_t           len,
                                    uint8_t           wtxm_crc,
                                    uint8_t           fwi,
                                    uint32_t          buffer_len,
                                    uint8_t          *p_rx_buf,
                                    uint32_t         *p_rx_len);

/**
 * \brief  ��������ͨ�����ز��źŹر�ָ����ʱ��,�ٿ�����ʵ�ֿ�Ƭ��λ��
 *
 * \param[in] handle      : ZSN603�豸������
 * \param[in] time_ms     : ʱ��(1�ֽ�),�Ժ���Ϊ��λ,0Ϊһֱ�ر�
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_picca_reset(zsn603_handle_t  handle,
                           uint8_t          time_ms);

/**
 * \brief  ���������ڼ��Ƭ,�����󡢷���ײ��ѡ�������������ϡ�
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] req_mode   : �������(1�ֽ�):  0x26~IDLE
 *                                       0x52~ALL
 *
 * \param[out] p_atq     : ���ص�atq��Ϣ(2�ֽ�)
 * \param[out] p_saq     : ���ص�saq��Ϣ(1�ֽ�)
 * \param[out] p_len     : ���ص�UID����(1�ֽ�)
 * \param[out] p_uid     : ���ص�UID(*p_len���ֽ�  ���10�ֽ�)
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_picca_active(zsn603_handle_t  handle,
                            uint8_t          req_mode,
                            uint16_t        *p_atq,
                            uint8_t         *p_saq,
                            uint8_t         *p_len,
                            uint8_t         *p_uid);

/**
 * \brief  ���������ڼ���B�Ϳ�Ƭ,�ڵ��ø�����ǰ,��Ҫ��ִ���豸������ġ�����IC���ӿ�Э��(����ģʽ)(Cmd = D)��,��оƬ�����ó�TypeBģʽ��
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] req_mode   : �������(1�ֽ�): 0x00~IDLE
 *                                      0x08~ALL
 *                         Ӧ�ñ�ʶ(1�ֽ�): Ĭ��Ϊ0x00
 * \param[out] p_uid     : ����UID   ����Ϊ12�ֽ�
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_piccb_active(zsn603_handle_t  handle,
                            uint8_t          req_mode,
                            uint8_t         *p_info);

/**
 * \brief  ��������ͨ�����ز��źŹر�ָ����ʱ��,�ٿ�����ʵ�ֿ�Ƭ��λ,��ʵ�ַ�ʽ��A�Ϳ���λһ����
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] time_ms    : ʱ��(1�ֽ�),�Ժ���Ϊ��λ,0Ϊһֱ�ر�
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_piccb_reset(zsn603_handle_t  handle,
                           uint8_t          time_ms);

/**
 * \brief  ����������B�Ϳ�����
 *
 * \param[in]  handle     : ZSN603�豸������
 * \param[in]  smcseq     : ��ȫ����/����
 * \param[in]  req_mode   : �������(1�ֽ�): 0x00~IDLE
 *                                       0x08~ALL
 * \param[in]  slot_time  : ʱ϶����(1�ֽ�): ��Χ0~4
 * \param[in]  buffer_len : ���淵��UID�������Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[out] p_uid      : ���ص�UID�����Ϣ������Ϊ12
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_piccb_request(zsn603_handle_t   handle,
                             uint8_t           req_mode,
                             uint8_t           slot_time,
                             uint32_t          buffer_len,
                             uint8_t          *p_uid);
/**
 * \brief  ����������B�Ϳ��޸Ĵ�������(��ѡ��)��
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] smcseq     : ��ȫ����/����
 * \param[in] p_pupi     : PUPI(4�ֽ�):   ��Ƭ��ʶ��
 * \param[in] cid        : CID(1�ֽ�) :   ȡֵ��ΧΪ 0 - 14, ����֧��CID,������Ϊ0
 *
 * \param[in] protype    : proType(1�ֽ�):֧�ֵ�Э��,�������Ӧ�е�ProtocolTypeָ��
 *                         proType.3:PCD��PICC�Ƿ����ͨ��
 *                                   1:PCD��ֹ��PICC����ͨ��
 *                                   0:PCD ��PICC����ͨ��
 *                         proType.2~1:PICC EOF��PCD SOF�����С�ӳ�
 *                                    11: etu + 512 / fs
 *                                    10: etu + 256 / fs
 *                                    01: etu + 128 / fs
 *                                    00: etu + 32 / fs
 *                         proType.0: �Ƿ���ѭISO14443-4
 *                                    1:��ѭISO14443-4;
 *                                    0:����ѭISO14443-4. (����֤����Ϊ1)
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_piccb_attrib(zsn603_handle_t  handle,
                            uint8_t         *p_pupi,
                            uint8_t          cid,
                            uint8_t          protype);
/**
 * \brief  ����������B�Ϳ�����,��ִ�й�������ǰ,������ִ�гɹ���һ���������
 *         ִ�й�������ɹ���,��Ƭ���ڹ���״̬,оƬ����ͨ��ALL��ʽ����Ƭ,��������IDLE��ʽ����
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] p_pupi     : PUPI(4�ֽ�):   4�ֽڱ�ʶ��
 *
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_piccb_halt(zsn603_handle_t      handle,
                          uint8_t       *p_pupi);
/**
 * \brief  ���������ڶ�ȡ�������ID������������˿����󡢿�ѡ���Լ���ȡIDָ�
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] req_mode   : �������(1�ֽ�):  0x00~IDLE
 *                                         0x08~ALL
 * \param[in] p_uid      : ���ص����֤ID(8�ֽ�)
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_piccb_getid(zsn603_handle_t  handle,
                           uint8_t          req_mode,
                           uint8_t         *p_uid);

/**
 * \brief  ������SL0���¸��˻����ݡ�����������SL0��Security Level 0����ȫ�ȼ�0����PLUS CPU�����˻���
 * PLUS CPU������ʱ�İ�ȫ�ȼ�ΪSL0���õȼ��£�����Ҫ�κ���֤�Ϳ�������д���ݣ�д�����������Ϊ������ȫ�ȼ��ĳ�ʼֵ
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] addr       : PLUS CPU���洢����ַ(2�ֽ�):
 * \param[in] p_data     : ����/AES��Կ/������(16�ֽ�)
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_plus_cpu_write_perso(zsn603_handle_t  handle,
                                    uint16_t         addr,
                                    uint8_t         *p_data);

/**
 * \brief  ����������SL0��Security Level 0����ȫ�ȼ�0����PLUS CPU���ύ���˻����ݣ�
 * ���SL0���˻��������ݡ�ֻ�Ǹ��¿��е����ݣ��������ݻ�δ��Ч���û�������ֱ��ʹ�á�
 * ��SL0�ύ���˻���ʹ���µĸ��˻�������Ч��ִ�и������PLUS CPU���İ�ȫ�ȼ���ߵ�SL1����SL3������֧��SL1�Ŀ���
 * ��ִ�и������Ƭ��ȫ�ȼ���ߵ�SL1������ֻ֧��SL0��SL3�Ŀ�����ִ�и������Ƭ��ȫ�ȼ���ߵ�SL3����
 *
 * \param[in] handle     : ZSN603�豸������
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_plus_cpu_commit_perso(zsn603_handle_t      handle);


/**
 * \brief  ����������SL3 PLUS CPU������Կ��֤����֤����Կͨ��������Ĳ������롣
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] addr       : AES��Կ��ַ��2�ֽڣ�
 * \param[out] p_data    : AES��Կ��16�ֽڣ�
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_plus_cpu_first_auth(zsn603_handle_t  handle,
                                   uint16_t         addr,
                                   uint8_t         *p_data);

/**
 * \brief  ����������SL3 PLUS CPU�����״���Կ��֤����֤����Կ����оƬ�ڲ������粻��ʧ�����ݡ�
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] addr       : AES��Կ��ַ��2�ֽڣ�
 * \param[in] key_block  : ��Կ���ţ�1�ֽڣ�
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_plus_cpu_first_auth_e2(zsn603_handle_t      handle,
                                      uint16_t       addr,
                                      uint8_t        key_block);

/**
 * \brief  ����������SL3 PLUS CPU���ĸ�����Կ��֤����֤����Կ�������������
 *         ֻ��ִ�й����״���֤������ɹ������ʹ�ø����
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] addr       : AES��Կ��ַ��2�ֽڣ�
 * \param[out] p_data    : AES��Կ��16�ֽڣ�
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_plus_cpu_follow_auth(zsn603_handle_t      handle,
                                    uint16_t       addr,
                                    uint8_t       *p_data);

/**
 * \brief  ����������SL3 PLUS CPU���ĸ�����Կ��֤����֤����Կ����оƬ�ڲ����粻��ʧ�����ݣ�
 *         ֻ��ִ�й����״���֤������ɹ������ʹ�ø����
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] addr       : AES��Կ��ַ��2�ֽڣ�
 * \param[in] key_block  : ��Կ���ţ�1�ֽڣ�
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_plus_cpu_follow_auth_e2(zsn603_handle_t      handle,
                                       uint16_t       addr,
                                       uint8_t        key_block);


/**
 * \brief  ����������PLUS CPU��ͨ���״���֤���ʹ�ù����У���λ��д����������֤����Ϣ��
 *
 * \param[in] handle     : ZSN603�豸������
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_plus_cpu_sl3_reset_auth(zsn603_handle_t      handle);

/**
 * \brief  ���������ڶ�ȡSL3�����ݿ飬�ڶ����ݿ�֮ǰ����ɹ�ִ��һ����Կ��֤��
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] read_mode  : ��ģʽ(1�ֽ�):0x30��������MAC���������ģ���Ӧ��MAC
 *                                    0x31��������MAC���������ģ���Ӧ��MAC
 *                                    0x32��������MAC���������ģ���Ӧ��MAC
 *                                    0x33��������MAC���������ģ���Ӧ��MAC
 *                                    0x34��������MAC���������ģ���Ӧ��MAC
 *                                    0x35��������MAC���������ģ���Ӧ��MAC
 *                                    0x36��������MAC���������ģ���Ӧ��MAC
 *                                    0x37��������MAC���������ģ���Ӧ��MAC
 *
 * \param[in] start_addr  : ��ʼ���(2�ֽ�)
 * \param[in] block_num   : ���Ŀ���(1�ֽ�):    ��Χ1��3
 * \param[in]  buffer_len : ��Ŷ�ȡ���ݻ������Ĵ�С�����������Ĵ�СС���豸��Ϣ���ȣ���ֻ�᷵�ػ�������С����
 * \param[out] p_rx_data  : ��ȡ���ݵĻ�����
 * \param[out] p_rx_lenght: ��ȡ���ݵĳ���
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_plus_cpu_sl3_read(zsn603_handle_t   handle,
                                 uint8_t           read_mode,
                                 uint16_t          start_addr,
                                 uint8_t           block_num,
                                 uint32_t          buffer_len,
                                 uint8_t          *p_rx_data,
                                 uint32_t         *p_rx_lenght);

/**
 * \brief  ���������ڶ�ȡSL3�����ݿ飬�ڶ����ݿ�֮ǰ����ɹ�ִ��һ����Կ��֤��
 *
 * \param[in] handle     : ZSN603�豸������
 * \param[in] read_mode  : дģʽ(1�ֽ�):0xA0��������MAC���������ģ���Ӧ��MAC
 *                                    0xA1��������MAC���������ģ���Ӧ��MAC
 *                                    0xA2��������MAC���������ģ���Ӧ��MAC
 *                                    0xA3��������MAC���������ģ���Ӧ��MAC
 *
 * \param[in] start_addr  : ��ʼ���(2�ֽ�)
 * \param[in] block_num   : д�Ŀ���(1�ֽ�):    ��Χ1��3
 * \param[out] p_rx_data  : �����д�����ݵĻ�����
 * \param[out] p_rx_lenght: д�����ݵĳ���  (block_num * 16)
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_plus_cpu_sl3_write(zsn603_handle_t  handle,
                                  uint8_t          write_mode,
                                  uint16_t         start_addr,
                                  uint8_t          block_num,
                                  uint8_t         *p_tx_data,
                                  uint8_t          tx_lenght);
/**
 * \brief  ���������ڶ�SL3�Ľ���ֵ���� �ɽ�Ŀ�������ݽ��мӼ�ֵ������д��Ŀ��顣
 *
 * \param[in] handle       : ZSN603�豸������
 * \param[in] write_mode   : ֵ����ģʽ(1�ֽ�):    0xB7����ֵ
 *                                            0xB9����ֵ
 * \param[in] src_addr     : Դ���(2�ֽ�)
 * \param[in] dst_addr     : Ŀ�Ŀ��(2�ֽ�)
 * \param[in] data         : ֵ����(4�ֽ�):  4�ֽ��з����������ֽ���ǰ�����ֽڵķ���λ������
 * \retval ���� : ��Ӧ������
 * \retval  0 : ִ�гɹ�
 */
uint8_t zsn603_plus_cpu_sl3_value_opr(zsn603_handle_t  handle,
                                      uint8_t          write_mode,
                                      uint16_t         src_addr,
                                      uint16_t         dst_addr,
                                      int              data);
#endif
