/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/



#ifndef __AW_USBD_CANDTU_H
#define __AW_USBD_CANDTU_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/**
 * \addtogroup grp_aw_if_usbd_candtu
 * \copydoc aw_usbd_candtu.h
 * @{
 */
#include "aw_usb_os.h"
#include "device/aw_usbd.h"


/** \brief 任务优先级和堆栈大小 */
#define AW_USBD_TASK_PRIO                  8
#define AW_USBD_CANDTU_STACK_SIZE          4096
#define AW_USBD_CANDTU_USBCAN_STACK_SIZE   2048

/** \brief 命令列表结束标志 */
#define AW_USBD_CANDTU_CMD_END      {0, 0, 0, 0, NULL}


struct aw_usbd_candtu;


/** \brief candtu命令定义 */
struct aw_usbd_candtu_cmd {
    uint8_t     start;      /**< \brief 开始标志 */
    uint8_t     cmd;        /**< \brief 命令 */
    uint8_t     info;       /**< \brief 参数 */
    uint8_t     res;        /**< \brief 保留 */
    uint32_t    len;        /**< \brief 数据长度 */
};


/** \brief candtu应答格式定义 */
struct aw_usbd_candtu_ack {
    uint8_t     start;      /**< \brief 开始标志 */
    uint8_t     ack;        /**< \brief 命令 */
    uint8_t     status;     /**< \brief 结果标志： 0 OK, 1 FAIL, 2 BUSY */
    uint8_t     res;        /**< \brief 保留 */
    uint32_t    len;        /**< \brief 数据长度 */
};



/** \brief candtu命令信息定义 */
struct aw_usbd_candtu_cmd_info {
    uint8_t         cmd;                /**< \brief 命令 */
    uint8_t         info;               /**< \brief 信息 */
    uint32_t        len;                /**< \brief 长度 */
    uint8_t         flag;               /**< \brief 标志 */
#define AW_USBD_CMD_CHECH_CMD   0x01
#define AW_USBD_CMD_CHECH_INFO  0x02
#define AW_USBD_CMD_CHECH_LEN   0x04

    /** \brief 命令执行函数 */
    int           (*pf_do) (struct aw_usbd_candtu     *p_cdtu,
                            struct aw_usbd_candtu_cmd *p_cmd,
                            void                      *p_buf,
                            int                        buf_len);
};



/** \brief candtu usbcan功能结构定义 */
struct aw_usbd_candtu_usbcan {
    struct aw_usbd_pipe  in;            /**< \brief IN数据管道 */
    struct aw_usbd_pipe  out;           /**< \brief OUT数据管道 */

    aw_usb_task_handle_t    usbcan_task;
    aw_usb_sem_handle_t     semb;

    aw_bool_t            buf_auto;      /**< \brief 是否内存动态分配 */
    void                *p_rxbuf;       /**< \brief 数据接收缓存 */
    void                *p_txbuf;       /**< \brief 数据发送缓存 */
    int                  rxsize;        /**< \brief 数据接收缓存区大小 */
    int                  txsize;        /**< \brief 数据发送缓存区大小 */

    /** \brief 消息发送函数  */
    int                (*put_msg) (struct aw_usbd_candtu_usbcan *p_candtu,
                                   void                         *p_msg,
                                   int                           len,
                                   void                         *p_buf,
                                   int                           buf_size,
                                   uint32_t                      timeout);
};



/** \brief candtu自定义类结构定义 */
struct aw_usbd_candtu {
    struct aw_usbd_fun                     ufun;        /**< \brief 功能接口 */
    struct aw_usbd_pipe                    cin;         /**< \brief IN数据管道 */
    struct aw_usbd_pipe                    cout;        /**< \brief OUT数据管道 */

    void                                  *p_buf;       /**< \brief 数据缓冲区 */
    uint32_t                               buf_size;    /**< \brief 缓冲区大小 */
    aw_bool_t                              buf_auto;    /**< \brief 是否动态分配内存 */

    const struct aw_usbd_candtu_cmd_info  *p_cmd_tab;   /**< \brief 命令列表指针 */

    aw_usb_task_handle_t                   cdtu_task;
    aw_usb_sem_handle_t                    semb;
    aw_usb_mutex_handle_t                  mutex;

    struct aw_usbd_candtu_usbcan          *p_usbcan;    /**< \brief usbcan功能 */
};




/**
 * \brief 创建candtu自定义设备
 *
 * \param[in] p_cdtu    candtu设备指针
 * \param[in] p_ctab    命令列表
 * \param[in] p_buf     缓存区首地址（为NULL则自动动态分配）
 * \param[in] size      缓存区大小
 * \param[in] evt_cb    事件回调
 *
 * \retval  AW_OK       创建成功
 * \retval  -AW_EINVAL  无效参数
 * \retval  -AW_ENOMEM  存储空间不足
 */
int aw_usbd_candtu_create (struct aw_usbd_candtu                 *p_cdtu,
                           const struct aw_usbd_candtu_cmd_info  *p_ctab,
                           void                                  *p_buf,
                           uint32_t                               size,
                           void (*evt_cb) (struct aw_usbd_candtu*, int));



/**
 * \brief 创建candtu usbcan功能
 *
 * \param[in] p_cdtu    candtu设备指针
 * \param[in] p_usbcan  uabcan设备指针
 * \param[in] p_buf     缓存区首地址（为NULL则自动动态分配）
 * \param[in] size      缓存区大小
 * \param[in] put_msg   消息发送函数
 *
 * \retval  AW_OK       创建成功
 * \retval  -AW_EINVAL  无效参数
 * \retval  -AW_EEXIST  设备已存在
 * \retval  -AW_ENOMEM  存储空间不足
 */
int aw_usbd_candtu_usbcan_create (struct aw_usbd_candtu         *p_cdtu,
                                  struct aw_usbd_candtu_usbcan  *p_usbcan,
                                  void                          *p_buf,
                                  uint32_t                       size,
                                  int (*put_msg) (struct aw_usbd_candtu_usbcan *p_candtu,
                                                  void                         *p_msg,
                                                  int                           len,
                                                  void                         *p_buf,
                                                  int                           buf_size,
                                                  uint32_t                      timeout));



/**
 * \brief 销毁candtu设备
 *
 * \param[in] p_cdtu    candtu设备指针
 *
 * \return  无
 */
void aw_usbd_candtu_destroy (struct aw_usbd_candtu  *p_cdtu);



/**
 * \brief candtu数据上传
 *
 * \param[in] p_cdtu    candtu设备指针
 * \param[in] p_data    上传数据首地址
 * \param[in] blk_size  数据块大小
 * \param[in] nblks     块数量
 * \param[in] timeout   发送超时（毫秒）
 *
 * \retval  AW_OK       上传成功
 * \retval  -AW_EINVAL  无效参数
 * \retval  -AW_ENOMEM  存储空间不足
 */
int aw_usbd_candtu_usbcan_upload (struct aw_usbd_candtu  *p_cdtu,
                                  void                   *p_data,
                                  uint32_t                blk_size,
                                  uint32_t                nblks,
                                  int                     timeout);

/** @} grp_aw_if_usbd_candtu */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_USBD_CANDTU_H */

/* end of file */
