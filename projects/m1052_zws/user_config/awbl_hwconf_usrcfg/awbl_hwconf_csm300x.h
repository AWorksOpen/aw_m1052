/*
 * awbl_hwconf_csm300x.h
 *
 *  Created on: 2019��4��23��
 *      Author: zengqingyuhang
 */

#ifndef _AWBL_HWCONF_CSM300X_H_
#define _AWBL_HWCONF_CSM300X_H_

#include "driver/can/awbl_csm300x.h"              /**< \brief ���ļ�·����ʱ����ʹ�ã�������ĳɾ���·��*/
#include "aw_can.h"
#include "aw_rngbuf.h"
#include "aw_gpio.h"
#include "aw_serial.h"
#include "imx1050_pin.h"
#include "aw_prj_params.h"


#ifdef  AW_DEV_CSM300X


#define AW_CFG_CSM300X_CTRL_HS          AW_CAN_CTLR_HS  /**< \brief ����CAN */
#define AW_CFG_CSM300X_RX_BUF_SIZE      512             /**< \brief ���λ�������С */

#define CSM_CFG         GPIO3_2         /**< \brief pin3.2��Ϊcfg����*/
#define CSM_MODE        GPIO3_4         /**< \brief pin3.4��Ϊmode����*/
#define CSM_RST         GPIO3_0         /**< \brief pin3.8��Ϊrst����*/

#define ENABLE          1
#define DISABLE         0
/*
//\brief csm300xģ�鹤��ģʽ
//typedef aw_bool_t   csm300x_work_mode_t;
//#define UART_TO_CAN           0
//#define SPI_TO_CAN            1
//
//\brief csm300xģ��canת��ģʽ
//typedef uint8_t     csm300x_xfer_mode_t;
//#define TRANSPARENT           0x01      *< \brief ͸��ת��
//#define MARK_TRANSF           0x02      *< \brief ͸������ʶת��
//#define CUSTOMIZE             0x03      *< \brief �Զ���ת��
//
//\brief csm300xģ��ת������
//typedef uint8_t     csm300x_xfer_dir_t;
//#define DOUBLE_DIR            0x00      *< \brief ˫��ת��
//#define SERIAL_TO_CAN         0x01      *< \brief uart/spi--can
//#define CAN_TO_SERIAL         0x02      *< \brief can--uart/spi
 *
 *
 * ��ģ��֧�ֵ�CAN���������£�
baud = 1000000;
baud = 800000;
baud = 500000;
baud = 250000;
baud = 125000;
baud = 100000;
baud = 50000;
baud = 20000;
baud = 10000;
*/
/******************************************************************************/

aw_local void awbl_csm300x_gpio_init (void);


aw_local struct aw_rngbuf  __g_csm300x_rd_buf_ctr;          /**< \brief CAN2���ձ��Ļ��λ��� */

AW_RNGBUF_SPACE_DEFINE(
        __g_csm300x_rx_msg_buff, struct aw_can_std_msg, AW_CFG_CSM300X_RX_BUF_SIZE);

/**
 * �豸������Ϣʵ����ֵ
 */
aw_local struct csm300x_dev_info __g_csm300x_devinfo = {
    /**< brief  struct awbl_can_servinfo */
    {
        AWBL_UART_CAN_BUSID,                            /**< \brief ��ǰcanͨ����*/
        AW_CFG_CSM300X_CTRL_HS,                         /**< \brief ��ǰcan�ٶ�ģʽ��*/
        sizeof(struct aw_can_std_msg),                  /**< \brief һ֡���ĳ���*/
        AW_CFG_CSM300X_RX_BUF_SIZE,                     /**< \brief ���λ�������С*/
        AW_RNGBUF_SPACE_GET(__g_csm300x_rx_msg_buff),   /**< \brief ���ջ������׵�ַ*/
        &__g_csm300x_rd_buf_ctr                         /**< \brief ���λ�����������*/
    },

    /**< brief  struct csm300x_xfer_info */
    {
        UART_TO_CAN,            /**< \brief csm300x����ģʽ  */
        CUSTOMIZE,              /**< \brief csm300x canת��ģʽ ,�Զ���ת�� */
        DOUBLE_DIR,             /**< \brief csm300x ת������  ��˫��ת��*/
        DISABLE,                  /**< \brief can֡ת��������֡��͸��ת����Ч ����*/
        DISABLE,                  /**< \brief ����canidת��������֡�У� ͸��ת����ʽ����Ч ������*/
        0,                   /**< \brief canid�ڴ���֡�е�λ��  ��0���ֽ�*/
        0xDE,                   /**< \brief ֡ͷ��Ϣ���Զ���ת����Ч  */
        0xED                    /**< \brief ֡β��Ϣ���Զ���ת����Ч  */
    },

    /**< brief  struct csm300x_uart_info */
    {
        COM2,            /**< \brief csm300x ���ں�  */
        115200,          /**< \brief csm300x ���ڲ�����  */
        2                /**< \brief csm300x ����֡�����ȴ��ֽ���  */
    },

    /**< brief  struct awbl_csm300x_hwinfo */
    {
        CSM_CFG,                            /**< \brief csm300x CFG����  */
        CSM_MODE,                           /**< \brief csm300x MODE����  */
        CSM_RST,                            /**< \brief csm300x RST����  */

        awbl_csm300x_gpio_init,       /**< \brief csm300x ����ƽ̨��ʼ��  */
    }
};


/**
 * �豸����ƽ̨��ʼ��
 */
aw_local void awbl_csm300x_gpio_init (void)
{
    int csm_gpio[3];

    csm_gpio[0] = __g_csm300x_devinfo.hwinfo.csm_cfg_pin;
    csm_gpio[1] = __g_csm300x_devinfo.hwinfo.csm_mode_pin;
    csm_gpio[2] = __g_csm300x_devinfo.hwinfo.csm_rst_pin;

    /**< \brief rst���ų�ʼ��Ϊ�͵�ƽ��ģ�齫һֱ���ڸ�λ״̬*/
    if(aw_gpio_pin_request("csm_gpio",
                            csm_gpio,
                            AW_NELEMENTS(csm_gpio)) == AW_OK){
        aw_gpio_pin_cfg(csm_gpio[0], AW_GPIO_OUTPUT_INIT_LOW | AW_GPIO_PULL_UP);
        aw_gpio_pin_cfg(csm_gpio[1], AW_GPIO_OUTPUT_INIT_LOW | AW_GPIO_PULL_UP);
        aw_gpio_pin_cfg(csm_gpio[2], AW_GPIO_OUTPUT_INIT_LOW | AW_GPIO_PULL_UP);
        AW_INFOF(("CSM300x gpio config successfully,line:%d\r\n",__LINE__));
        return;
    }
    AW_INFOF(("CSM300x gpio config failed,line:%d\r\n",__LINE__));
    return;
}


/**
 * �豸ʵ���ڴ澲̬����
 */
aw_local struct csm300x_dev __g_csm300x_dev;


/**
 * �豸ʵ����ֵ
 * param[0]     device_name�豸��
 *
 * param[1]     unit�豸��Ԫ��
 *
 * param[2]     bus_type��������
 *
 * param[3]     bus_index���߱��
 *
 * param[4]     ��awbl_dev *��ָ���豸ʵ���ڴ�
 *
 * param[5]     ��void *��ָ���豸��Ϣʵ���ڴ�
 */

#define AWBL_HWCONF_CSM300X         \
    {                               \
        AWBL_CSM300X_NAME,          \
        0,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &(__g_csm300x_dev.dev),     \
        &(__g_csm300x_devinfo)      \
    },



#else
#define AWBL_HWCONF_CSM300X

#endif /* #ifdef  AW_DEV_CSM300X */

#endif /* _AWBL_HWCONF_CSM300X_H_ */
