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
 * \brief CSM300X driver
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-22  zengqingyuhang, first implementation
 * \endinternal
 */

/**
 * ���ļ���������can_service��can_servinfo�Ƚṹ����Ϣ��
 * ���ﲻ�Ժ�����������
 * can����ṹ����can������Ϣ������awbl_can.h�ļ��У�
 *
 */
#ifndef _AWBL_CSM300X_H_
#define _AWBL_CSM300X_H_

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_can.h"

#define AWBL_CSM300X_NAME   "csm300x"    /**< \brief:csm300xģ���������� */
#define MAX_SERIAL_LEN      16           /**< \brief ����������󳤶� */


/*****************************************************************************/
struct csm300x_config_info {
    volatile uint8_t    fixed0;             /**< \brief ֡��ʼ�̶��ֽ�1 0xF7*/
    volatile uint8_t    fixed1;             /**< \brief ֡��ʼ�̶��ֽ�2 0xF8*/
    volatile uint8_t    cmd;                /**< \brief ������  0x01-0x03*/
    volatile uint8_t    datalen;            /**< \brief ����֡����,�̶�����60�ֽ�  0x3C*/
    volatile uint8_t    uartbaud;           /**< \brief uart�����ʣ�0x00~0x10*/
    volatile uint8_t    uartdatabits;       /**< \brief uart����λ���ȣ��̶�8λ  0x08*/
    volatile uint8_t    uartstopbits;       /**< \brief uartֹͣλ���̶�1λ  0x01*/
    volatile uint8_t    uartvarify;         /**< \brief uartУ��λ���̶���  0x00*/
    volatile uint8_t    canbaud;            /**< \brief can�����ʣ�0x00~0x0F*/
    volatile uint8_t    reserved0[3];
    volatile uint8_t    canfilter;          /**< \brief can�˲���ʹ��*/
    volatile uint8_t    canrecvfiltermode;  /**< \brief can���չ���ģʽ*/
    volatile uint8_t    canmask[4];         /**< \brief ������*/
    volatile uint8_t    checkcode0[4];      /**< \brief ������0*/
    volatile uint8_t    checkcode1[4];      /**< \brief ������1*/
    volatile uint8_t    reserved1[4];
    volatile uint8_t    checkcode2[4];      /**< \brief ������2*/
    volatile uint8_t    checkcode3[4];      /**< \brief ������3*/
    volatile uint8_t    checkcode4[4];      /**< \brief ������4*/
    volatile uint8_t    checkcode5[4];      /**< \brief ������5*/

    volatile uint8_t    xfermode;           /**< \brief canת��ģʽ*/
    volatile uint8_t    xferdir;            /**< \brief canת������*/
    volatile uint8_t    uartframegap;       /**< \brief uart֡���*/
    
    /** \brief can֡��Ϣת��������֡ ������͸��ת��*/
    volatile uint8_t    xtoserialframe;  
    /** \brief canidת��������֡ ������͸��ת��*/    
    volatile uint8_t    xidtoserialframe;   
    /** \brief can֡���� ������͸��ת����͸������ʶ��ת��*/
    volatile uint8_t    xframetype;         
    /** \brief can���ͱ�ʶ�� ������͸��ת��*/
    volatile uint8_t    xsendflag[4];
    /** \brief can��ʶ������ ������͸������ʶ��ת��*/    
    volatile uint8_t    xflaglen;      
    /** \brief can��ʶ����ʼ��ַ ������͸������ʶ��ת��*/    
    volatile uint8_t    xflagaddr;          
    volatile uint8_t    framenode;          /**< \brief ֡ͷ*/
    volatile uint8_t    frameend;           /**< \brief ֡β*/

    volatile uint8_t    spifeedbackcnt;     /**< \brief spi��������֡��*/
    volatile uint8_t    spifeedbacktime;    /**< \brief spi��������ʱ��*/
    volatile uint8_t    spicpollevel;       /**< \brief cpol���� ��֧��0x01*/
    volatile uint8_t    spicphalevel;       /**< \brief cpha���� ��֧��0x01*/
    volatile uint8_t    crccheck;           /**< \brief ǰ�������ֽڵ����*/
};

/*����֡��Ϣ����*/
typedef struct aw_serial_msg {
    uint8_t     node;           /**< \brief ֡ͷ */
    char        len ;           /**< \brief ֡���� */
    uint8_t     type;           /**< \brief ֡���� */
    uint8_t     id_len;         /**< \brief ֡id���� */
    uint8_t     id[4];          /**< \brief ֡id */
    char        buf_len;        /**< \brief ���ݳ��� */
    uint8_t    *buf;            /**< \brief ���� */
    uint8_t     end;            /**< \brief ֡β */
}aw_serial_msg_t;

typedef struct serial_std_msg {
    uint8_t     buf[MAX_SERIAL_LEN];
    uint8_t     len;
}serial_std_msg_t;

/**< \brief csm300xģ�鹤��ģʽ */
typedef aw_bool_t   csm300x_work_mode_t;
#define UART_TO_CAN           0
#define SPI_TO_CAN            1

/**< \brief csm300xģ��canת��ģʽ */
typedef uint8_t     csm300x_xfer_mode_t;
#define TRANSPARENT           0x01      /**< \brief ͸��ת�� */
#define MARK_TRANSF           0x02      /**< \brief ͸������ʶת�� */
#define CUSTOMIZE             0x03      /**< \brief �Զ���ת�� */

/**< \brief csm300xģ��ת������ */
typedef uint8_t     csm300x_xfer_dir_t;
#define DOUBLE_DIR            0x00      /**< \brief ˫��ת�� */
#define SERIAL_TO_CAN         0x01      /**< \brief uart/spi--can */
#define CAN_TO_SERIAL         0x02      /**< \brief can--uart/spi */

typedef uint8_t     csm300x_transfer_status_t;
#define TC_FLAGE    0x08
#define RC_FLAGE    0x80


/******************************************************************************/
typedef struct csm300x_xferinfo {
    uint8_t     workmode;               /**< \brief csm300xģ��ת��ģʽ */
    uint8_t     xfermode;               /**< \brief canת��ģʽ */
    uint8_t     xferdir;                /**< \brief ת������ */
    /** 
     * \brief ����CAN֡��Ϣת��������֡ ����͸��ת����ʽ����Ч
     */
    aw_bool_t   canframe;     
    /** 
     * \brief ����CAN��ʶ����CAN-ID��ת��������֡�� ��͸��ת����ʽ����Ч
     */    
    aw_bool_t   canid;      
    /**
     * \brief CAN��ʶ����ʼ��ַ ����͸������ʶת����ʽ����Ч
     */    
    uint8_t     canflagaddr;            
    uint8_t     framenode;              /**< \brief ֡ͷ �����Զ���ת����ʽ��Ч*/
    uint8_t     frameend;               /**< \brief ֡β �����Զ���ת����ʽ��Ч*/
}csm300x_xfer_info_t;


typedef struct csm300x_uartinfo {
    uint32_t     uartcom;               /**< \brief  ���ں�*/
    uint32_t     uartbaud;              /**< \brief  ���ڲ�����*/
    uint32_t     uartgap;               /**< \brief  ����֡���ʱ��*/
    AW_SEMC_DECL(uart_semc);            /**< \brief  ���ڼ����ź���*/
}csm300x_uart_info_t;



/* �ṩ�ӿڶ�̬�޸� */
typedef struct csm300x_caninfo {
    uint32_t    canbaud;                /**< \brief CANͨ�Ų����� */
    aw_bool_t   frametype;              /**< \brief ֡���� */
    uint8_t     canid[4];               /**< \brief CAN-ID��ʶ�� */
    aw_bool_t   filter;                 /**< \brief �˲���ʹ�� */
    aw_bool_t   recvfilter;             /**< \brief ���չ���ģʽ */
    uint8_t     maskcode[4];            /**< \brief ������ */
    uint8_t     reccode0[4];            /**< \brief ������0 */
    uint8_t     reccode1[4];            /**< \brief ������1 */
    uint8_t     reccode2[4];            /**< \brief ������2 */
    uint8_t     reccode3[4];            /**< \brief ������3 */
    uint8_t     reccode4[4];            /**< \brief ������4 */
    uint8_t     reccode5[4];            /**< \brief ������5 */
}csm300x_can_info_t;

/**
 * brief CSM300Xģ��Ӳ��������Ϣ
 */
struct awbl_csm300x_hwinfo {

    int         csm_cfg_pin;            /**< \brief CFG���� */
    int         csm_mode_pin;           /**< \brief MODE�������� */
    int         csm_rst_pin;            /**< \brief RST��λ���� */

    void (*pfun_plfm_init)(void);       /**< \brief ƽ̨��ʼ������ */
};

typedef struct csm300x_dev_info{

    struct awbl_can_servinfo    servinfo;
    struct csm300x_xferinfo     xferinfo;
    struct csm300x_uartinfo     uartinfo;
    struct awbl_csm300x_hwinfo  hwinfo;
    void (*pfun_common_pin_set)(int cfg_sta,
                                int mode_sta,
                                int rst_pin);   /**< \brief ����cfg���ŵ�ƽ���� */
    uint8_t  task_priority;                     /**< \brief �豸���ȼ� */

    aw_can_std_msg_t            p_can_msg;      /**< \brief �豸���յ���can��Ϣ */
    csm300x_transfer_status_t   dev_sta;        /**< \brief �豸����״̬ */
    AW_SEMB_DECL(tx_semb);                      /**< \brief �����ź��� */

}csm300x_dev_info_t;


typedef struct csm300x_dev{

    struct awbl_dev             dev;
    struct awbl_can_service     can_serv;
    struct csm300x_config_info  p_reg;

}csm300x_dev_t;


/**
 * \brief �豸����ע��ӿ�
 */
void awbl_csm300x_drv_register (void);



#endif /* _AWBL_CSM300X_H_ */

/* end of file*/
