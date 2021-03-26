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
 * \brief AWBus I2C总线接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_i2cbus.h"
 * \endcode
 * 本模块为 I2C 总线的精简版实现
 *
 * 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.01 18-11-02  sls, update i2c arch
 * - 1.00 12-10-10  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_I2CBUS_H
#define __AWBL_I2CBUS_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "aw_list.h"
#include "aw_i2c.h"         
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "aw_task.h"

/** \brief I2C 异步任务使用的栈  */
#define AWBL_I2C_ASYNC_TASK_STACK_SIZE (4096)

/** \brief 驱动优先级   */
#define AWBL_I2C_ASYNC_TASK_PRIO       AW_TASK_SYS_PRIORITY(6)

/** \brief 永久等待 */
#define AWBL_I2C_WAITFOREVER           (AW_SEM_WAIT_FOREVER)

/** \brief 驱动支持的功能 */
#define AWBL_FEATURE_I2C_7BIT          0x0001u  /**< \brief 7-bit设备地址支持 */
#define AWBL_FEATURE_I2C_10BIT         0x0002u  /**< \brief 10-bit设备地址支持 */
#define AWBL_FEATURE_I2C_REV_DIR_ADDR  0x0004u  /**< \brief 读写标志位反转支持 */

/** \brief 忽略所有无应答错误 (包括从机地址无应答和数据无应答)支持 */
#define AWBL_FEATURE_I2C_IGNORE_NAK    0x0008u

/** \brief 在读操作中接收数据时不发送应答支持 */
#define AWBL_FEATURE_I2C_NO_RD_ACK     0x0010u


/** \biref 得到I2C从机设备的父控制器 */
#define AWBL_I2C_PARENT_GET(p_dev) \
    ((struct awbl_i2c_master *) \
        (((struct awbl_dev *)(p_dev))->p_parentbus->p_ctlr))

/** \biref 得到I2C从机设备的父控制器编号 */
#define AWBL_I2C_PARENT_BUSID_GET(p_dev) \
        (((struct awbl_i2c_master_devinfo *) \
            AWBL_DEVINFO_GET( \
                ((struct awbl_dev *)(p_dev))->p_parentbus->p_ctlr))->bus_index)

/** \brief 控制写是否给停止信号  */
#define AWBL_I2C_WRITE_NO_STOP      (0)
#define AWBL_I2C_WRITE_STOP         (1)

/** \brief 控制读是否给重复起始信号  */
#define AWBL_I2C_READ_START         (0)
#define AWBL_I2C_READ_RESTART       (1)

typedef struct awbl_trans_buf {
    uint8_t *p_buf;
    uint8_t  buf_size;
} awbl_trans_buf_t;


struct awbl_i2c_master;

/** \biref AWBus I2C 总线控制器 (I2C 主机) 设备信息 (平台配置部分) */
struct awbl_i2c_master_devinfo {

    /** \brief 控制器所对应的总线编号 */
    uint8_t     bus_index;

    /** \brief 控制器总线速度 */
    uint32_t    speed;

    /** \brief 超时, 单位 ms，永久等待： #AWBL_I2C_WAITFOREVER */
    int         timeout;
};


/** \biref AWBus I2C 总线控制器 (I2C 主机) 设备信息 (驱动配置部分) */
struct awbl_i2c_master_devinfo2 {

    /** \brief 控制器的特性(支持的“I2C模式,见 awbl_i2c.h)  */
    uint16_t    features;

    /**
     * \brief i2c写操作
     *
     * \param[in]  p_master         i2c控制器指针
     * \param[in]  chip_addr        i2c从器件的设备地址
     * \param[in]  p_trans_buf      写数据缓冲区首地址
     * \param[in]  trans_buf_num    写数据缓冲区个数
     * \param[in]  stop_ctl         停止信号设置(AWBL_I2C_WRITE_STOP发送停止信号,
     *                                       AWBL_I2C_WRITE_NO_STOP不发停止信号)
     * \param[in]  dr_flag          传递给驱动层处理的一些标志参数
     *                              (如: AW_I2C_M_IGNORE_NAK,
     *                                   AW_I2C_M_NO_RD_ACK,
     *                                   AW_I2C_M_REV_DIR_ADDR,
     *                                   AW_I2C_M_10BIT等)
     */
    aw_err_t (*pfunc_i2c_write) (struct awbl_i2c_master  *p_master,
                                 uint16_t                 chip_addr,
                                 struct awbl_trans_buf   *p_trans_buf,
                                 uint32_t                 trans_buf_num,
                                 uint8_t                  stop_ctl,
                                 uint16_t                 dr_flag);

    /**
     * \brief i2c读操作
     *
     * \param[in]       p_master  i2c控制器指针
     * \param[in]       chip_addr i2c从器件的设备地址
     * \param[in]       p_trans_buf 记录传输的缓冲区和缓冲区的大小
     * \param[in]       start_ctl 起始信号控制, 发送start信号,还是restart信号(
     *                            AWBL_I2C_READ_START发送起始信号,
     *                            AWBL_I2C_READ_RESTART 发送重复起始信号)
     * \param[in]       dr_flag   传递给驱动层处理的一些标志参数
     *                                    (如: AW_I2C_M_IGNORE_NAK,
     *                                         AW_I2C_M_NO_RD_ACK,
     *                                         AW_I2C_M_REV_DIR_ADDR,
     *                                         AW_I2C_M_10BIT等)
     */
    aw_err_t  (*pfunc_i2c_read) (struct awbl_i2c_master *p_master,
                                 uint16_t                chip_addr,
                                 struct awbl_trans_buf  *p_trans_buf,
                                 uint8_t                 start_ctl,
                                 uint16_t                dr_flag);

    /**
     * \brief i2c配置
     *
     * \param[in]       p_master  i2c控制器指针
     * \param[in]       p_cfg     指针配置结构
     */
    aw_err_t  (*pfunc_i2c_set_cfg) (struct awbl_i2c_master *p_master,
                                    struct aw_i2c_config   *p_cfg);


    /**
     * \brief 获取i2c配置
     *
     * \param[in]       p_master  i2c控制器指针
     * \param[out]      p_cfg     取出当前配置
     */
    aw_err_t (*pfunc_i2c_get_cfg)(struct awbl_i2c_master *p_master,
                                  struct aw_i2c_config   *p_cfg);

};


/** \biref I2C bus 从机设备驱动注册信息结构体 */
typedef struct awbl_i2c_drvinfo {
    struct awbl_drvinfo super;  /**< \brief 继承自 AWBus 设备驱动信息 */
} awbl_i2c_drvinfo_t;

/** \brief AWBus I2C 总线设备 (I2C从机) 实例 */
struct awbl_i2c_device {
    struct awbl_dev super;      /**< \brief 继承自 AWBus 设备 */
};

/** \brief AWBus I2C 总线控制器 (I2C主机) 实例 */
struct awbl_i2c_master {
    /** \brief 继承自 AWBus 总线控制器 */
    struct awbl_busctlr super;

    /** \brief 指向下一个 I2C 控制器 */
    struct awbl_i2c_master *p_next;

    /** \brief I2C 控制器相关信息 (驱动配置部分) */
    const struct awbl_i2c_master_devinfo2 *p_devinfo2;

    /** \brief 设备互斥量 */
    AW_MUTEX_DECL(dev_mux);
};


/**
 * \brief AWBus i2c 总线模块初始化函数
 *
 * \attention 本函数应当在 awbus_lite_init() 之后、awbl_dev_init1() 之前调用
 */
void awbl_i2cbus_init(void);

/**
 * \brief 创建 AWBus I2C 总线实例以及枚举总线上的设备
 *
 * 本接口提供给 AWBus I2C 总线控制器(主机设备)驱动使用
 *
 */
aw_err_t awbl_i2cbus_create(struct awbl_i2c_master *p_master);

/**
 * \brief 处理消息、异步模式
 *
 * 本接口提供给 AWBus I2C 从设备驱动使用
 *
 * 以异步的方式处理消息，消息的处理状态和结果反映在 p_msg->status。
 * 消息处理结束(成功、超时或出错)时，将会调用 p_msg->pfunc_complete 并传递
 * 参数 p_msg->p_arg。
 *
 * \param[in]       p_dev   AWBus I2C 从设备实例
 * \param[in,out]   p_msg   要处理的消息
 *
 * 函数的返回值如下：
 * \retval  AW_OK          成功
 * \retval -AW_ENXIO       未找到\a p_dev->masterid 所指定的主机
 * \retval -AW_EINVAL      参数错误，nbytes 为 0,p_buf为空等
 * \retval -AW_ENOTSUP     \a flags 中某些特性不支持
 *                         (例如, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       超时
 * \retval -AW_ENODEV      从机无应答(若\a p_dev->flags 设置了
 *                         #AW_I2C_IGNORE_NAK, 则不会产生此错误)
 * \retval -AW_EIO         读写数据时发生总线错误、总线仲裁失败过多等错误
 *
 * 成功处理的传输个数反映在 p_msg->done_num
 *
 * 消息的处理状态和结果反映在 p_msg->status ：
 *
 *      \li  -ENOTCONN       消息尚未排队
 *      \li  -EISCONN        消息正在排队处理
 *      \li  -EINPROGRESS    消息正在被处理
 * 下面为消息处理完毕的结果 (可通过 p_msg->done_num 定位出错的传输)
 *      \li   AW_OK          所有传输成功处理
 *      \li  -ENOTSUP        某次传输的控制标志不支持
 *      \li  -ETIME          超时，消息未在设定的时间内处理，在控制器的devinfo中
 *                           设置超时时间
 *      \li  -ENODEV         某次传输无应答
 *      \li  -EIO            传输消息时发生总线错误、总线仲裁失败过多等错误
 */
aw_err_t awbl_i2c_async(struct awbl_i2c_device *p_dev,
                        struct aw_i2c_message  *p_msg);

/**
 * \brief 处理消息、同步模式
 *
 * 本接口提供给 AWBus I2C 从设备驱动使用
 *
 * 以同步的方式处理消息，消息的处理状态和结果反映在 p_msg->status。
 * 参数 p_msg->pfunc_complete 和 p_msg->p_arg 由控制器使用。
 *
 * \param[in]       p_dev   AWBus I2C 从设备实例
 * \param[in,out]   p_msg   要处理的消息
 *
 * 函数的返回值如下：
 * \retval  AW_OK          成功
 * \retval -AW_ENXIO       未找到\a p_dev->masterid 所指定的主机
 * \retval -AW_EINVAL      参数错误，nbytes 为 0,p_buf为空等
 * \retval -AW_ENOTSUP     \a p_dev->flags 中某些特性不支持
 *                         (例如, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       超时
 * \retval -AW_ENODEV      从机无应答(若\a flags 设置了
 *                         #AW_I2C_IGNORE_NAK, 则不会产生此错误)
 * \retval -AW_EIO         读写数据时发生总线错误、总线仲裁失败过多等错误
 *
 * 成功处理的传输个数反映在 p_msg->done_num
 *
 * 消息的处理状态和结果反映在 p_msg->status ：
 *
 *      \li  -ENOTCONN       消息尚未排队
 *      \li  -EISCONN        消息正在排队处理
 *      \li  -EINPROGRESS    消息正在被处理
 * 下面为消息处理完毕的结果 (可通过 p_msg->done_num 定位出错的传输)
 *      \li   AW_OK           所有传输成功处理
 *      \li  -ENOTSUP        某次传输的控制标志不支持
 *      \li  -ETIME          超时，消息未在设定的时间内处理
 *      \li  -ENODEV         某次传输无应答
 *      \li  -EIO            传输消息时发生总线错误、总线仲裁失败过多等错误
 */
aw_err_t awbl_i2c_sync(struct awbl_i2c_device *p_dev,
                       struct aw_i2c_message  *p_msg);

/**
 * \brief 读I2C从机数据
 *
 * 本接口提供给 AWBus I2C 从设备驱动使用
 *
 * \param[in]  p_dev    AWBus I2C从机设备实例， 见 \ref struct awbl_i2c_device
 * \param[in]  flags    从机设备属性，见“I2C从设备属性标志 (aw_i2c.h)”
 * \param[in]  addr     从机设备地址，7位或10位
 * \param[in]  subaddr  器件内子地址，从此地址开始读取数据，若 flags 设置子地址
 *                      宽度为0 (AW_I2C_SUBADDR_NONE), 则此参数不起作用
 * \param[out] p_buf    数据缓冲区，读取的数据存放于此
 * \param[in]  nbytes   要写入的数据个数
 *
 * \retval  AW_OK          成功
 * \retval -AW_ENXIO       未找到\a p_dev->masterid 所指定的主机
 * \retval -AW_EINVAL      参数错误，nbytes 为 0,p_buf为空等
 * \retval -AW_ENOTSUP     \a p_dev->flags 中某些特性不支持
 *                         (例如, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       超时
 * \retval -AW_ENODEV      从机无应答(若\a flags 设置了
 *                         #AW_I2C_IGNORE_NAK, 则不会产生此错误)
 * \retval -AW_EIO         读写数据时发生总线错误、总线仲裁失败过多等错误
 *
 * 注意:写子地址和读之间是用的重复起始信号，没有停止信号。
 *
 * \par 范例
 * \code
 *  uint8_t read_buf[16];
 *
 *  awbl_i2c_read((struct awbl_i2c_device *)p_dev,  // AWBus 从设备实例
 *                 AW_I2C_ADDR_7BIT |
 *                 AW_I2C_SUBADDR_1BYTE,  // 7-bit 器件地址 1-byte 器件内子地址
 *                 0x50,                  // 器件地址，0x50
 *                 0,                     // 器件内子地址，0x00
 *                 p_buf,                 // 读缓冲区
 *                 16);                   // 读取数据长度，16字节
 * \endcode
 */
aw_err_t awbl_i2c_read(struct awbl_i2c_device *p_dev,
                       uint16_t                flags,
                       uint16_t                addr,
                       uint32_t                subaddr,
                       void                   *p_buf,
                       size_t                  nbytes);

/**
 * \brief 写数据到I2C从机
 *
 * 本接口提供给 AWBus I2C 从设备驱动使用
 *
 * \param[in]  p_dev    AWBus I2C从机设备实例， 见 \ref struct awbl_i2c_device
 * \param[in]  flags    从机设备属性，见“I2C从设备属性标志 (aw_i2c.h)”
 * \param[in]  addr     从机设备地址，7位或10位
 * \param[in]  subaddr  器件内子地址，从此地址开始读取数据，若 flags 设置子地址
 *                      宽度为0 (AW_I2C_SUBADDR_NONE), 则此参数不起作用
 * \param[out] p_buf    数据缓冲区，要写入的数据存放于此
 * \param[in]  nbytes   要写入的数据个数
 *
 * \retval  AW_OK          成功
 * \retval -AW_ENXIO       未找到\a p_dev->masterid 所指定的主机
 * \retval -AW_EINVAL      参数错误，nbytes 为 0,p_buf为空等
 * \retval -AW_ENOTSUP     \a flags 中某些特性不支持
 *                         (例如, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       超时
 * \retval -AW_ENODEV      从机无应答(若\a p_dev->flags 设置了
 *                         #AW_I2C_IGNORE_NAK, 则不会产生此错误)
 * \retval -AW_EIO         读写数据时发生总线错误、总线仲裁失败过多等错误
 *
 * 注意: 写子地址和写数据中间是没有停止信号的
 *
 * \par 范例
 * \code
 *  uint8_t write_buf[16];
 *
 *  awbl_i2c_write((struct awbl_i2c_device *)p_dev,  // AWBus 从设备实例
 *                  AW_I2C_ADDR_7BIT |
 *                  AW_I2C_SUBADDR_1BYTE,  // 7-bit 器件地址 1-byte 器件内子地址
 *                  0x50,                  // 器件地址，0x50
 *                  0,                     // 器件内子地址，0x00
 *                  p_buf,                 // 读缓冲区
 *                  16);                   // 读取数据长度，16字节
 * \endcode
 */
aw_err_t awbl_i2c_write(struct awbl_i2c_device *p_dev,
                        uint16_t                flags,
                        uint16_t                addr,
                        uint32_t                subaddr,
                        const void             *p_buf,
                        size_t                  nbytes);

/**
 * \brief 原始写到I2C从机(不发送器件内的子地址)
 *
 * \param[in]  p_dev    AWBus I2C从机设备实例， 见 \ref struct awbl_i2c_device
 * \param[in]  addr     从机设备地址，7位或10位
 * \param[in]  flags    从机设备属性，见“I2C从设备属性标志 (aw_i2c.h)”
 * \param[out] p_buf    数据缓冲区，要写入的数据存放于此
 * \param[in]  nbytes   要写入的数据个数
 *
 *
 * \retval  AW_OK          成功
 * \retval -AW_ENXIO       未找到\a p_dev->masterid 所指定的主机
 * \retval -AW_EINVAL      参数错误，nbytes 为 0,p_buf为空等
 * \retval -AW_ENOTSUP     \a flags 中某些特性不支持
 *                         (例如, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       超时
 * \retval -AW_ENODEV      从机无应答(若\a p_dev->flags 设置了
 *                         #AW_I2C_IGNORE_NAK, 则不会产生此错误)
 * \retval -AW_EIO         读写数据时发生总线错误、总线仲裁失败过多等错误
 *
 * \par 范例
 * \code
 *  uint8_t write_buf[16];
 *
 *  awbl_i2c_raw_write((struct awbl_i2c_device *)p_dev,  // AWBus 从设备实例
 *                      0x50,                 // 器件地址，0x50
 *                      AW_I2C_ADDR_7BIT      // 7位子地址
 *                      p_buf,                // 读缓冲区
 *                      16);                  // 读取数据长度，16字节
 * \endcode
 */
aw_err_t awbl_i2c_raw_write (struct awbl_i2c_device  *p_dev,
                             uint16_t                 addr,
                             uint16_t                 flags,
                             const void              *p_buf,
                             size_t                   nbytes);


/**
 * \brief 原始读I2C从机(不发送器件内的子地址)
 *
 * \param[in]  p_dev    AWBus I2C从机设备实例， 见 \ref struct awbl_i2c_device
 * \param[in]  addr     从机设备地址，7位或10位
 * \param[in]  flags    从机设备属性，见“I2C从设备属性标志 (aw_i2c.h)”
 * \param[out] p_buf    数据缓冲区，要读出的数据存放于此
 * \param[in]  nbytes   要读的数据个数
 *
 *
 * \retval  AW_OK          成功
 * \retval -AW_ENXIO       未找到\a p_dev->masterid 所指定的主机
 * \retval -AW_EINVAL      参数错误，nbytes 为 0,p_buf为空等
 * \retval -AW_ENOTSUP     \a p_dev->flags 中某些特性不支持
 *                         (例如, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       超时
 * \retval -AW_ENODEV      从机无应答(若\a flags 设置了
 *                         #AW_I2C_IGNORE_NAK, 则不会产生此错误)
 * \retval -AW_EIO         读写数据时发生总线错误、总线仲裁失败过多等错误
 *
 */
aw_err_t awbl_i2c_raw_read (struct awbl_i2c_device   *p_dev,
                            uint16_t                  addr,
                            uint16_t                  flags,
                            void                     *p_buf,
                            size_t                    nbytes);


/**
 * \brief I2C写后读
 *
 * \param[in]  p_dev         AWBus I2C从机设备实例， 见 \ref struct awbl_i2c_device
 * \param[in]  addr          从机设备地址，7位或10位
 * \param[in]  flags         从机设备属性，见“I2C从设备属性标志 (aw_i2c.h)”
 * \param[in]  p_txbuf       发送数据缓冲区
 * \param[in]  n_tx          发送数据缓冲区长度
 * \param[in]  p_rxbuf       接收数据缓冲区
 * \param[in]  n_rx          接收数据缓冲区长度
 * \param[in]  is_send_stop  AW_TRUE  则写完地址后发送STOP条件,再发送START,
 *                           AW_FALSE 写完地址后发达RESTART条件
 *
 *
 * \retval  AW_OK          成功
 * \retval -AW_ENXIO       未找到\a p_dev->masterid 所指定的主机
 * \retval -AW_EINVAL      参数错误，nbytes 为 0,p_buf为空等
 * \retval -AW_ENOTSUP     \a p_dev->flags 中某些特性不支持
 *                         (例如, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       超时
 * \retval -AW_ENODEV      从机无应答(若\a flags 设置了
 *                         #AW_I2C_IGNORE_NAK, 则不会产生此错误)
 * \retval -AW_EIO         读写数据时发生总线错误、总线仲裁失败过多等错误
 *
 */
aw_err_t awbl_i2c_write_then_read (struct awbl_i2c_device  *p_dev,
                                   uint16_t                 addr,
                                   uint16_t                 flags,
                                   const void              *p_txbuf,
                                   size_t                   n_tx,
                                   void                    *p_rxbuf,
                                   size_t                   n_rx,
                                   uint8_t                  is_send_stop);


/**
 * \brief I2C写后写
 *
 * \param[in]  p_dev     AWBus I2C从机设备实例， 见 \ref struct awbl_i2c_device
 * \param[in]  addr      从机设备地址，7位或10位
 * \param[in]  flags     从机设备属性，见“I2C从设备属性标志 (aw_i2c.h)”
 * \param[in]  p_buf0    数据缓冲区0，要写的数据存放于此
 * \param[in]  n_tx0     数据缓冲区0的长度
 * \param[out] p_buf1    数据缓冲区1，要写的数据存放于此
 * \param[in]  n_tx1     数据缓冲区1的长度
 *
 *
 * \retval  AW_OK          成功
 * \retval -AW_ENXIO       未找到\a p_dev->masterid 所指定的主机
 * \retval -AW_EINVAL      参数错误，nbytes 为 0,p_buf为空等
 * \retval -AW_ENOTSUP     \a flags 中某些特性不支持
 *                         (例如, #AW_I2C_IGNORE_NAK)
 * \retval -AW_ETIME       超时
 * \retval -AW_ENODEV      从机无应答(若\a p_dev->flags 设置了
 *                         #AW_I2C_IGNORE_NAK, 则不会产生此错误)
 * \retval -AW_EIO         读写数据时发生总线错误、总线仲裁失败过多等错误
 *
 * \note  注意:两个写中间是没有停止信号的
 */
aw_err_t awbl_i2c_write_then_write (struct awbl_i2c_device   *p_dev,
                                    uint16_t                  addr,
                                    uint16_t                  flags,
                                    const void               *p_buf0,
                                    size_t                    n_tx0,
                                    const void               *p_buf1,
                                    size_t                    n_tx1);

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AWBL_I2CBUS_H */

/* end of file */
