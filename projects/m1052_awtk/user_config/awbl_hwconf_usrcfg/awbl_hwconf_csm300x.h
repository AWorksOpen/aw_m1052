/*
 * awbl_hwconf_csm300x.h
 *
 *  Created on: 2019年4月23日
 *      Author: zengqingyuhang
 */

#ifndef _AWBL_HWCONF_CSM300X_H_
#define _AWBL_HWCONF_CSM300X_H_

#include "driver/can/awbl_csm300x.h"              /**< \brief 此文件路径暂时这样使用，后续需改成绝对路径*/
#include "aw_can.h"
#include "aw_rngbuf.h"
#include "aw_gpio.h"
#include "aw_serial.h"
#include "imx1050_pin.h"
#include "aw_prj_params.h"


#ifdef  AW_DEV_CSM300X


#define AW_CFG_CSM300X_CTRL_HS          AW_CAN_CTLR_HS  /**< \brief 高速CAN */
#define AW_CFG_CSM300X_RX_BUF_SIZE      512             /**< \brief 环形缓冲区大小 */

#define CSM_CFG         GPIO3_2         /**< \brief pin3.2做为cfg引脚*/
#define CSM_MODE        GPIO3_4         /**< \brief pin3.4做为mode引脚*/
#define CSM_RST         GPIO3_0         /**< \brief pin3.8做为rst引脚*/

#define ENABLE          1
#define DISABLE         0
/*
//\brief csm300x模块工作模式
//typedef aw_bool_t   csm300x_work_mode_t;
//#define UART_TO_CAN           0
//#define SPI_TO_CAN            1
//
//\brief csm300x模块can转换模式
//typedef uint8_t     csm300x_xfer_mode_t;
//#define TRANSPARENT           0x01      *< \brief 透明转换
//#define MARK_TRANSF           0x02      *< \brief 透明带标识转换
//#define CUSTOMIZE             0x03      *< \brief 自定义转换
//
//\brief csm300x模块转换方向
//typedef uint8_t     csm300x_xfer_dir_t;
//#define DOUBLE_DIR            0x00      *< \brief 双向转换
//#define SERIAL_TO_CAN         0x01      *< \brief uart/spi--can
//#define CAN_TO_SERIAL         0x02      *< \brief can--uart/spi
 *
 *
 * 此模块支持的CAN波特率如下：
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


aw_local struct aw_rngbuf  __g_csm300x_rd_buf_ctr;          /**< \brief CAN2接收报文环形缓冲 */

AW_RNGBUF_SPACE_DEFINE(
        __g_csm300x_rx_msg_buff, struct aw_can_std_msg, AW_CFG_CSM300X_RX_BUF_SIZE);

/**
 * 设备特性信息实例赋值
 */
aw_local struct csm300x_dev_info __g_csm300x_devinfo = {
    /**< brief  struct awbl_can_servinfo */
    {
        AWBL_UART_CAN_BUSID,                            /**< \brief 当前can通道号*/
        AW_CFG_CSM300X_CTRL_HS,                         /**< \brief 当前can速度模式，*/
        sizeof(struct aw_can_std_msg),                  /**< \brief 一帧报文长度*/
        AW_CFG_CSM300X_RX_BUF_SIZE,                     /**< \brief 环形缓冲区大小*/
        AW_RNGBUF_SPACE_GET(__g_csm300x_rx_msg_buff),   /**< \brief 接收缓冲区首地址*/
        &__g_csm300x_rd_buf_ctr                         /**< \brief 环形缓冲区控制字*/
    },

    /**< brief  struct csm300x_xfer_info */
    {
        UART_TO_CAN,            /**< \brief csm300x工作模式  */
        CUSTOMIZE,              /**< \brief csm300x can转换模式 ,自定义转换 */
        DOUBLE_DIR,             /**< \brief csm300x 转换方向  ，双向转换*/
        DISABLE,                  /**< \brief can帧转换到串行帧，透明转换有效 禁能*/
        DISABLE,                  /**< \brief 允许canid转换到串行帧中， 透明转换方式下有效 ，禁能*/
        0,                   /**< \brief canid在串行帧中的位置  第0个字节*/
        0xDE,                   /**< \brief 帧头信息，自定义转换有效  */
        0xED                    /**< \brief 帧尾信息，自定义转换有效  */
    },

    /**< brief  struct csm300x_uart_info */
    {
        COM2,            /**< \brief csm300x 串口号  */
        115200,          /**< \brief csm300x 串口波特率  */
        2                /**< \brief csm300x 串口帧结束等待字节数  */
    },

    /**< brief  struct awbl_csm300x_hwinfo */
    {
        CSM_CFG,                            /**< \brief csm300x CFG引脚  */
        CSM_MODE,                           /**< \brief csm300x MODE引脚  */
        CSM_RST,                            /**< \brief csm300x RST引脚  */

        awbl_csm300x_gpio_init,       /**< \brief csm300x 引脚平台初始化  */
    }
};


/**
 * 设备引脚平台初始化
 */
aw_local void awbl_csm300x_gpio_init (void)
{
    int csm_gpio[3];

    csm_gpio[0] = __g_csm300x_devinfo.hwinfo.csm_cfg_pin;
    csm_gpio[1] = __g_csm300x_devinfo.hwinfo.csm_mode_pin;
    csm_gpio[2] = __g_csm300x_devinfo.hwinfo.csm_rst_pin;

    /**< \brief rst引脚初始化为低电平，模块将一直处于复位状态*/
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
 * 设备实例内存静态分配
 */
aw_local struct csm300x_dev __g_csm300x_dev;


/**
 * 设备实例赋值
 * param[0]     device_name设备名
 *
 * param[1]     unit设备单元号
 *
 * param[2]     bus_type总线类型
 *
 * param[3]     bus_index总线编号
 *
 * param[4]     （awbl_dev *）指向设备实例内存
 *
 * param[5]     （void *）指向设备信息实例内存
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
