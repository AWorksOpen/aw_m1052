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
 * \brief CAN演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *   - 对应平台的CAN设备宏(AW_DEV_XXXXX_CAN1)
 *   - AW_COM_CONSOLE
 *   - 对应平台的串口设备宏(AW_DEV_XXXXX_UART1)
 *   2. 连接CAN1的相关引脚到CAN收发器，再将CAN收发器接口与USBCAN-II设备相连。
 *   3. 打开CAN上位机后，设置波特率为500k。
 *   4. 打开串口调试终端，波特率115200-8-N-1。
 *
 * - 实验现象：
 *   1. 上位机发送数据后，开发板收到数据，并回发数据给上位机。
 *   2. 串口打印相关调试信息。
 *
 * - 备注：
 *   1. 其中 CAN ID 和 波特率配置 需要根据具体硬件平台修改。
 *
 * \par 源代码
 * \snippet demo_can.c src_demo_can
 *
 * \internal
 * \par modification history:
 * - 17-12-15, mex, first implementation.
 * \endinternal
 */

/**
 * \page demo_if_can
 * \copydoc demo_can.c
 */

#include "aworks.h"
#include "aw_can.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_prj_params.h"
#include "aw_sem.h"

#define  CAN_CHN              0     /*CAN ID，根据具体平台修改*/
#define  CAN_RCV_BUFF_LENTH   1024

/** \brief 波特率表的配置取决于具体硬件平台，需要根据具体硬件平台做出相应的修改*/
/** \brief tseg1 tseg2 sjw smp brp*/
static aw_can_baud_param_t can_btr_1000k = {15,  4,   4,   0,   2  };   /* 1000k, sample point 80% */
static aw_can_baud_param_t can_btr_800k  = {6,   3,   3,   0,   5  };   /* 800k,  sample point 70% */
static aw_can_baud_param_t can_btr_500k  = {15,  4,   4,   0,   4  };   /* 500k,  sample point 80% */
static aw_can_baud_param_t can_btr_250k  = {15,  4,   4,   0,   8  };   /* 250k,  sample point 80% */
static aw_can_baud_param_t can_btr_125k  = {15,  4,   4,   0,   16 };   /* 125k,  sample point 80% */
static aw_can_baud_param_t can_btr_100k  = {15,  4,   4,   0,   20 };   /* 100k,  sample point 80% */
static aw_can_baud_param_t can_btr_50k   = {15,  4,   4,   0,   40 };   /* 50k,   sample point 80% */
static aw_can_baud_param_t can_btr_20k   = {15,  4,   4,   0,   100};   /* 20k,   sample point 80% */
static aw_can_baud_param_t can_btr_10k   = {15,  4,   4,   0,   200};   /* 10k,   sample point 80% */
static aw_can_baud_param_t can_btr_6_25k = {16,  8,   4,   0,   256};   /* （最小波特率）6.25k, sample point 68% */


aw_can_std_msg_t    can_rcv_msg[CAN_RCV_BUFF_LENTH];
aw_can_bus_status_t can_bus_status = 0;

/**< \brief 定义互斥信号量，用于同步发送和接收（发送和接收不能同时进行） */
AW_MUTEX_DECL_STATIC(mutex);

aw_can_std_msg_t can_send_test_msg = {
    {
        0x111,
        0x01,
        AW_CAN_MSG_FLAG_SEND_NORMAL,    /**< \brief 发送类型 */
        {0, 0},
        CAN_CHN,                        /**< \brief chn 报文所在通道号 */
        8                               /**< \brief msglen 报文长度 */
    },

    {1,2,3,4,5,6,7,8}
};

/** \brief 滤波表格式
 * 第1段是滤波方式选择,通常选择全ID+标志滤波 参数是0
 *
 * 第2段及之后是全ID滤波的格式,如下
 * --------------------------------
 * | rmt(31) | ext(30) | id(29~1) |
 * --------------------------------
 * rmt 远程帧标志.若此位为1,则接收对应ID的远程帧,否则接收数据帧
 * ext 扩展帧标志.若此位为1,则接收对应ID的扩展帧,否则接收标准帧
 * id  仲裁ID.其中(29~19)为标准帧ID段,(29~1)为扩展帧ID段
 *
 */
#define __ID_ARB(id, ext, rmt)  (rmt << 31) | (ext << 30) | (id << (ext ? 1 : 19))
#define __ID_MSK_STD(id, ext, rmt)  ((rmt << 31) | (ext << 30) | (id << 19))  /* 标准帧掩码 */
#define __ID_MSK_EXT(id, ext, rmt)  ((rmt << 31) | (ext << 30) | (id << 1))   /* 扩展帧掩码 */

/** \brief 验收滤波表 */
uint32_t filter_mr[] = {
    __ID_ARB(0x1, 0, 0),                       /* 仲裁位(标准数据帧) */
    __ID_ARB(0x2, 0, 0),                       /* 仲裁位(标准数据帧) */
    __ID_ARB(0x3, 0, 0),                       /* 仲裁位(标准数据帧) */
    __ID_ARB(0x4, 0, 0),                       /* 仲裁位(标准数据帧) */
    __ID_ARB(0x5, 0, 0),                       /* 仲裁位(标准数据帧) */
    __ID_ARB(0x6, 0, 0),                       /* 仲裁位(标准数据帧) */
    __ID_ARB(0x7, 0, 0),                       /* 仲裁位(标准数据帧) */
    __ID_ARB(0x8, 1, 0),                       /* 仲裁位(扩展数据帧) */
    __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1),   /* 掩码位(全标准帧滤波) */
    __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1),   /* 掩码位(全标准帧滤波) */
    __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1),   /* 掩码位(全标准帧滤波) */
    __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1),   /* 掩码位(全标准帧滤波) */
    __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1),   /* 掩码位(全标准帧滤波) */
    __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1),   /* 掩码位(全标准帧滤波) */
    __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1),   /* 掩码位(全标准帧滤波) */
    __ID_MSK_EXT(AW_CAN_EXT_IDS_MASK, 1, 1)    /* 掩码位(全扩展帧滤波) */
};

/******************************************************************************/
uint32_t __can_timestamp_get_cb (uint32_t p_arg);
aw_bool_t __can_err_sta_cb (int chn, aw_can_bus_err_t err, aw_can_bus_status_t status);

aw_can_app_callbacks_t can_app_callback_fun = {__can_timestamp_get_cb,
                                               NULL,
                                               NULL,
                                               __can_err_sta_cb,
                                               NULL
                                               };

/******************************************************************************/
/* 用户回调程序入口 */
uint32_t __can_timestamp_get_cb (uint32_t p_arg)
{
    return 0;
}

/******************************************************************************/
aw_bool_t __can_err_sta_cb (int                 chn,
                            aw_can_bus_err_t    err,
                            aw_can_bus_status_t status)
{
    aw_can_err_cnt_t  can_err_reg;

    if (err & AW_CAN_BUS_ERR_BIT) {    /**< \brief 位错误 */
        aw_kprintf(("AW_CAN_BUS_ERR_BIT\n"));
    }
    if (err &AW_CAN_BUS_ERR_ACK) {     /**< \brief 应答错误 */
        aw_kprintf(("AW_CAN_BUS_ERR_ACK\n"));
    }
    if (err &AW_CAN_BUS_ERR_CRC) {     /**< \brief CRC错误 */
        aw_kprintf(("AW_CAN_BUS_ERR_CRC\n"));
    }
    if (err &AW_CAN_BUS_ERR_FORM) {    /**< \brief 格式错误 */
        aw_kprintf(("AW_CAN_BUS_ERR_FORM\n"));
    }
    if (err &AW_CAN_BUS_ERR_STUFF) {   /**< \brief 填充错误 */
        aw_kprintf(("AW_CAN_BUS_ERR_STUFF\n"));
    }
    aw_can_err_cnt_get (chn, &can_err_reg);
    if (can_err_reg.tx_err_cnt > 127) {
        aw_can_err_cnt_clr(chn);
    }

    switch (status){
    case AW_CAN_BUS_STATUS_OK:
        break;

    case AW_CAN_BUS_STATUS_WARN:
        break;

    case AW_CAN_BUS_STATUS_OFF:
        //AW_INFOF(("\r\nCAN: Buss-Off! \r\n"));
        break;

    default:
        break;
    }
    can_bus_status = status;
    return AW_TRUE;
}

/**
 * \brief CAN demo
 *
 * \return 无
 */
void demo_can_entry (void)
{
    uint32_t rcv_num_real  = 0;
    uint32_t send_done_num = 0;
    uint32_t send_done_all = 0;

    uint32_t rcv_fifo_size = 0;

    size_t   filter_number = 0;

    //uint32_t timeout = 0;
    aw_can_err_t ret;

    /**< \brief 初始化互斥信号量 */
    AW_MUTEX_INIT(mutex, AW_SEM_Q_FIFO);

    /** \brief 初始化CAN */
    ret = aw_can_init(CAN_CHN,                 /** \brief CAN通道号*/
                      AW_CAN_WORK_MODE_NORMAL, /** \brief 控制器工作模式*/
                      &can_btr_500k,           /** \brief 当前波特率*/
                      &can_app_callback_fun);  /** \brief 用户回调函数*/

    if (ret == AW_CAN_ERR_NONE) {
        AW_INFOF(("\r\nCAN: controller initialize ok. \r\n"));
    } else {
        AW_INFOF(("\r\nCAN: controller initialize error! \r\n"));
    }

    /** \brief 获取接收FIFO大小 */
    rcv_fifo_size = aw_can_rcv_fifo_size (CAN_CHN);

    AW_INFOF(("CAN: receive fifo size %d messages.\r\n", rcv_fifo_size));

    /** \brief 设置滤波表 */
    aw_can_filter_table_set (CAN_CHN, (uint8_t*)&filter_mr, sizeof(filter_mr));
    aw_can_filter_table_get (CAN_CHN, (uint8_t*)&filter_mr, &filter_number);

    AW_INFOF(("CAN: filter table set.\r\n"));

    /** \brief 启动CAN */
    aw_can_start(CAN_CHN);
    AW_INFOF(("CAN: controller start!\r\n"));

    AW_FOREVER {
        AW_MUTEX_LOCK(mutex, AW_SEM_WAIT_FOREVER);
#if 0
        aw_can_std_msgs_send(CAN_CHN,
                             &can_send_test_msg,
                             1,
                             &send_done_num,
                             500);
        if (1 != send_done_num) {
            AW_INFOF(("\r\nSend error(发送错误)!\r\n"));
        }
#else
        /** \brief 读取CAN接收fifo中报文个数 */
        rcv_num_real = aw_can_rcv_msgs_num (CAN_CHN);
        if (rcv_num_real) {
            aw_can_std_msgs_rcv(CAN_CHN,
                                can_rcv_msg,
                                CAN_RCV_BUFF_LENTH / 2,
                                &rcv_num_real,
                                0);

            if (rcv_num_real) {
                aw_can_std_msgs_send(CAN_CHN,
                                     can_rcv_msg,
                                     rcv_num_real,
                                     &send_done_num,
                                     3000);
                if (rcv_num_real != send_done_num) {
                    AW_INFOF(("\r\nSend error!\r\n"));
                }
                AW_INFOF(("\r\ndone:%d  all:%d \r\n",
                          send_done_num,
                          send_done_all += send_done_num));
                rcv_num_real = 0;
            }

        } else {
            aw_mdelay(10); /* 延时10ms */
            if (can_bus_status == AW_CAN_BUS_STATUS_OFF) {
                aw_can_stop(CAN_CHN);
                aw_can_start(CAN_CHN);
            }
        }
        AW_MUTEX_UNLOCK(mutex);
        aw_mdelay(10);
#endif
    }
}

/** [src_can] */

/* end of file */
