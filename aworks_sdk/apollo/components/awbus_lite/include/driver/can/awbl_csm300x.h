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
 * 该文件用于声明can_service、can_servinfo等结构体信息，
 * 这里不对函数进行声明
 * can服务结构体与can服务信息定义在awbl_can.h文件中，
 *
 */
#ifndef _AWBL_CSM300X_H_
#define _AWBL_CSM300X_H_

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_can.h"

#define AWBL_CSM300X_NAME   "csm300x"    /**< \brief:csm300x模块驱动名字 */
#define MAX_SERIAL_LEN      16           /**< \brief 串行数据最大长度 */


/*****************************************************************************/
struct csm300x_config_info {
    volatile uint8_t    fixed0;             /**< \brief 帧起始固定字节1 0xF7*/
    volatile uint8_t    fixed1;             /**< \brief 帧起始固定字节2 0xF8*/
    volatile uint8_t    cmd;                /**< \brief 命令字  0x01-0x03*/
    volatile uint8_t    datalen;            /**< \brief 配置帧数据,固定长度60字节  0x3C*/
    volatile uint8_t    uartbaud;           /**< \brief uart波特率，0x00~0x10*/
    volatile uint8_t    uartdatabits;       /**< \brief uart数据位长度，固定8位  0x08*/
    volatile uint8_t    uartstopbits;       /**< \brief uart停止位，固定1位  0x01*/
    volatile uint8_t    uartvarify;         /**< \brief uart校验位，固定无  0x00*/
    volatile uint8_t    canbaud;            /**< \brief can波特率，0x00~0x0F*/
    volatile uint8_t    reserved0[3];
    volatile uint8_t    canfilter;          /**< \brief can滤波器使能*/
    volatile uint8_t    canrecvfiltermode;  /**< \brief can接收过滤模式*/
    volatile uint8_t    canmask[4];         /**< \brief 屏蔽码*/
    volatile uint8_t    checkcode0[4];      /**< \brief 验收码0*/
    volatile uint8_t    checkcode1[4];      /**< \brief 验收码1*/
    volatile uint8_t    reserved1[4];
    volatile uint8_t    checkcode2[4];      /**< \brief 验收码2*/
    volatile uint8_t    checkcode3[4];      /**< \brief 验收码3*/
    volatile uint8_t    checkcode4[4];      /**< \brief 验收码4*/
    volatile uint8_t    checkcode5[4];      /**< \brief 验收码5*/

    volatile uint8_t    xfermode;           /**< \brief can转换模式*/
    volatile uint8_t    xferdir;            /**< \brief can转换方向*/
    volatile uint8_t    uartframegap;       /**< \brief uart帧间隔*/
    
    /** \brief can帧信息转换到串行帧 仅用于透明转换*/
    volatile uint8_t    xtoserialframe;  
    /** \brief canid转换到串行帧 仅用于透明转换*/    
    volatile uint8_t    xidtoserialframe;   
    /** \brief can帧类型 仅用于透明转换、透明带标识符转换*/
    volatile uint8_t    xframetype;         
    /** \brief can发送标识符 仅用于透明转换*/
    volatile uint8_t    xsendflag[4];
    /** \brief can标识符长度 仅用于透明带标识符转换*/    
    volatile uint8_t    xflaglen;      
    /** \brief can标识符起始地址 仅用于透明带标识符转换*/    
    volatile uint8_t    xflagaddr;          
    volatile uint8_t    framenode;          /**< \brief 帧头*/
    volatile uint8_t    frameend;           /**< \brief 帧尾*/

    volatile uint8_t    spifeedbackcnt;     /**< \brief spi反馈触发帧数*/
    volatile uint8_t    spifeedbacktime;    /**< \brief spi反馈触发时间*/
    volatile uint8_t    spicpollevel;       /**< \brief cpol极性 仅支持0x01*/
    volatile uint8_t    spicphalevel;       /**< \brief cpha极性 仅支持0x01*/
    volatile uint8_t    crccheck;           /**< \brief 前面所有字节的异或*/
};

/*串行帧信息定义*/
typedef struct aw_serial_msg {
    uint8_t     node;           /**< \brief 帧头 */
    char        len ;           /**< \brief 帧长度 */
    uint8_t     type;           /**< \brief 帧类型 */
    uint8_t     id_len;         /**< \brief 帧id长度 */
    uint8_t     id[4];          /**< \brief 帧id */
    char        buf_len;        /**< \brief 数据长度 */
    uint8_t    *buf;            /**< \brief 数据 */
    uint8_t     end;            /**< \brief 帧尾 */
}aw_serial_msg_t;

typedef struct serial_std_msg {
    uint8_t     buf[MAX_SERIAL_LEN];
    uint8_t     len;
}serial_std_msg_t;

/**< \brief csm300x模块工作模式 */
typedef aw_bool_t   csm300x_work_mode_t;
#define UART_TO_CAN           0
#define SPI_TO_CAN            1

/**< \brief csm300x模块can转换模式 */
typedef uint8_t     csm300x_xfer_mode_t;
#define TRANSPARENT           0x01      /**< \brief 透明转换 */
#define MARK_TRANSF           0x02      /**< \brief 透明带标识转换 */
#define CUSTOMIZE             0x03      /**< \brief 自定义转换 */

/**< \brief csm300x模块转换方向 */
typedef uint8_t     csm300x_xfer_dir_t;
#define DOUBLE_DIR            0x00      /**< \brief 双向转换 */
#define SERIAL_TO_CAN         0x01      /**< \brief uart/spi--can */
#define CAN_TO_SERIAL         0x02      /**< \brief can--uart/spi */

typedef uint8_t     csm300x_transfer_status_t;
#define TC_FLAGE    0x08
#define RC_FLAGE    0x80


/******************************************************************************/
typedef struct csm300x_xferinfo {
    uint8_t     workmode;               /**< \brief csm300x模块转换模式 */
    uint8_t     xfermode;               /**< \brief can转换模式 */
    uint8_t     xferdir;                /**< \brief 转换方向 */
    /** 
     * \brief 允许CAN帧信息转换到串行帧 ，仅透明转换方式下有效
     */
    aw_bool_t   canframe;     
    /** 
     * \brief 允许CAN标识符（CAN-ID）转换到串行帧， 仅透明转换方式下有效
     */    
    aw_bool_t   canid;      
    /**
     * \brief CAN标识符起始地址 ，仅透明带标识转换方式下有效
     */    
    uint8_t     canflagaddr;            
    uint8_t     framenode;              /**< \brief 帧头 ，仅自定义转换方式有效*/
    uint8_t     frameend;               /**< \brief 帧尾 ，仅自定义转换方式有效*/
}csm300x_xfer_info_t;


typedef struct csm300x_uartinfo {
    uint32_t     uartcom;               /**< \brief  串口号*/
    uint32_t     uartbaud;              /**< \brief  串口波特率*/
    uint32_t     uartgap;               /**< \brief  串行帧间隔时间*/
    AW_SEMC_DECL(uart_semc);            /**< \brief  串口计数信号量*/
}csm300x_uart_info_t;



/* 提供接口动态修改 */
typedef struct csm300x_caninfo {
    uint32_t    canbaud;                /**< \brief CAN通信波特率 */
    aw_bool_t   frametype;              /**< \brief 帧类型 */
    uint8_t     canid[4];               /**< \brief CAN-ID标识符 */
    aw_bool_t   filter;                 /**< \brief 滤波器使能 */
    aw_bool_t   recvfilter;             /**< \brief 接收过滤模式 */
    uint8_t     maskcode[4];            /**< \brief 屏蔽码 */
    uint8_t     reccode0[4];            /**< \brief 验收码0 */
    uint8_t     reccode1[4];            /**< \brief 验收码1 */
    uint8_t     reccode2[4];            /**< \brief 验收码2 */
    uint8_t     reccode3[4];            /**< \brief 验收码3 */
    uint8_t     reccode4[4];            /**< \brief 验收码4 */
    uint8_t     reccode5[4];            /**< \brief 验收码5 */
}csm300x_can_info_t;

/**
 * brief CSM300X模块硬件配置信息
 */
struct awbl_csm300x_hwinfo {

    int         csm_cfg_pin;            /**< \brief CFG引脚 */
    int         csm_mode_pin;           /**< \brief MODE控制引脚 */
    int         csm_rst_pin;            /**< \brief RST复位引脚 */

    void (*pfun_plfm_init)(void);       /**< \brief 平台初始化函数 */
};

typedef struct csm300x_dev_info{

    struct awbl_can_servinfo    servinfo;
    struct csm300x_xferinfo     xferinfo;
    struct csm300x_uartinfo     uartinfo;
    struct awbl_csm300x_hwinfo  hwinfo;
    void (*pfun_common_pin_set)(int cfg_sta,
                                int mode_sta,
                                int rst_pin);   /**< \brief 公用cfg引脚电平设置 */
    uint8_t  task_priority;                     /**< \brief 设备优先级 */

    aw_can_std_msg_t            p_can_msg;      /**< \brief 设备接收到的can信息 */
    csm300x_transfer_status_t   dev_sta;        /**< \brief 设备传输状态 */
    AW_SEMB_DECL(tx_semb);                      /**< \brief 接收信号量 */

}csm300x_dev_info_t;


typedef struct csm300x_dev{

    struct awbl_dev             dev;
    struct awbl_can_service     can_serv;
    struct csm300x_config_info  p_reg;

}csm300x_dev_t;


/**
 * \brief 设备驱动注册接口
 */
void awbl_csm300x_drv_register (void);



#endif /* _AWBL_CSM300X_H_ */

/* end of file*/
