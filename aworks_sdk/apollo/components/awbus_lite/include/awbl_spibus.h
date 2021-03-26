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
 * \brief AWBus SPI总线接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_spibus.h"
 * \endcode
 * 本模块为 SPI 总线的精简版实现
 *
 * \internal
 * \par modification history:
 * - 2.00 18-10-09  sls, update spi arch
 * - 1.00 12-11-19  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_SPIBUS_H
#define __AWBL_SPIBUS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_list.h"
#include "aw_spi.h"         /* 通用SPI接口文件 */
#include "aw_sem.h"
#include "aw_task.h"
#include "awbus_lite.h"


#define AWBL_SPI_ASYNC_TASK_STACK_SIZE   (4096)

/* 驱动应该使用系统保留优先级   */
#define AWBL_SPI_ASYNC_TASK_PRIO         AW_TASK_SYS_PRIORITY(6)

/**************************硬件片选控制标志************************************/

/** \brief 控制传输完成后给片选无效的信号   */
#define AWBL_SPI_END_OF_HARDCS_INVALID    (0)

/** \brief 控制传输完成后不给片选无效的信号   */
#define AWBL_SPI_END_OF_HARDCS_VALID      (1)

/** \brief 极性低有效标志   */
#define AWBL_SPI_HARDCS_LOW_VALID         (0)

/** \brief 极性高有效标志   */
#define AWBL_SPI_HARDCS_HIGH_VALID        (1)


/**************************驱动支持的功能 *************************************/

/** \brief 空闲时钟是低  */
#define AWBL_FEATURE_SPI_CPOL_L             0x0001u

/** \brief 空闲时钟是高  */
#define AWBL_FEATURE_SPI_CPOL_H             0x0002u

/** \brief 上升沿采样  */
#define AWBL_FEATURE_SPI_CPHA_U             0x0004u

/** \brief 下降沿采样  */
#define AWBL_FEATURE_SPI_CPHA_D             0x0008u

/** \brief SPI回环模式  */
#define AWBL_FEATURE_SPI_LOOP               0x0010u

/** \brief 3线SPI模式  */
#define AWBL_FEATURE_SPI_3WIRE              0x0020u

/** \brief 支持低位先发   */
#define AWBL_FEATURE_SPI_LSB_FIRST          0x0040u

/** \brief SPI读取的时候MOSI发送的值是0xFF, 否则发送0x00 */
#define AWBL_FEATURE_SPI_READ_MOSI_HIGH     0x0080u

/** \brief SPI支持硬件CS */
#define AWBL_FEATURE_SPI_AUTO_CS            0x0100u


/**
 * \name SPI 控制器功能限制标志
 * @{
  */
#define AWBL_SPI_MASTER_HALF_DUPLEX AW_BIT(0)  /**< /brief 不支持全双工 */
#define AWBL_SPI_MASTER_NO_RX       AW_BIT(1)  /**< /brief 不支持缓冲区读 */
#define AWBL_SPI_MASTER_NO_TX       AW_BIT(2)  /**< /brief 不支持缓冲区写 */
/** @} */

/** \biref 得到SPI从机设备的父控制器 */
#define AWBL_SPI_PARENT_GET(p_dev) \
    ((struct awbl_spi_master *) \
        (((struct awbl_dev *)(p_dev))->p_parentbus->p_ctlr))

/** \biref 得到SPI从机设备的父控制器编号 */
#define AWBL_SPI_PARENT_BUSID_GET(p_dev) \
        (((struct awbl_spi_master_devinfo *) \
            AWBL_DEVINFO_GET( \
                ((struct awbl_dev *)(p_dev))->p_parentbus->p_ctlr))->bus_index)

struct awbl_spi_master;
struct awbl_spi_config;

/** \biref AWBus SPI 总线控制器 (SPI 主机) 设备信息 (平台配置部分) */
struct awbl_spi_master_devinfo {

    /** \brief 控制器所对应的总线编号 */
    uint8_t     bus_index;
};

/** \biref AWBus SPI 总线控制器 (SPI 主机) 设备信息 (驱动配置部分) */
struct awbl_spi_master_devinfo2 {

    /** \brief 控制器的特性(支持的“SPI模式标帧本,见 aw_spi.h)  */
    uint16_t    features;

    /** \brief 控制器的限制(见“SPI 控制器功能限制标志”) */
    uint16_t    flags;

    /**
     * \brief 设置控制器模式、时钟等参数 ( SPI 驱动可能多次调用)
     * \attention 该函数可能在传输活动时被调用，不要直接更新那些公共的寄存器，
     *            否则会破环当前传输。
     */
    aw_err_t (*pfunc_setup)(struct awbl_spi_master *p_master,
                            aw_spi_device_t        *p_dev);

   /**
    * \brief 读写操作(软件片选)
    */
    aw_err_t  (*write_and_read)  (struct awbl_spi_master   *p_master,
                                  const void               *tx_buf,
                                  void                     *rx_buf,
                                  uint32_t                  nbytes);

    /**
     * \brief 读写操作(硬件片选)
     *
     * \ param cs_pin 是用来判断传入的cs_pin是否合法
     * \ param cs_polarity 是用来判断硬件片选的极性, 如片选高有效还是低有效
     * \ param cs_state    是用来判断是否有效片先, 非0为有效片选, 0为无效片选
     */
    aw_err_t  (*write_and_read_hard_cs)  (struct awbl_spi_master   *p_master,
                                          const void               *tx_buf,
                                          void                     *rx_buf,
                                          uint32_t                  nbytes,
                                          uint32_t                  cs_pin,
                                          uint8_t                   cs_polarity,
                                          uint8_t                   cs_state);

    /**
     * \brief SPI配置接口
     */
    aw_err_t  (*config)  (struct awbl_spi_master *p_master,
                          struct awbl_spi_config *p_cfg);

    /**
     * \brief 获取默认SPI配置接口
     */
    aw_err_t  (*get_default_config) (struct awbl_spi_config *p_cfg);
};

/* \biref SPI bus 从机设备驱动注册信息结构体 */
typedef struct awbl_spi_drvinfo {
    struct awbl_drvinfo super;  /**< \brief 继承自 AWBus 设备驱动信息 */
} awbl_spi_drvinfo_t;

/** \brief AWBus SPI 总线设备 (SPI从机) 实例 */
struct awbl_spi_device {
    struct awbl_dev super;          /**< \brief 继承自 AWBus 设备 */
    struct aw_spi_device spi_dev;   /**< \brief SPI 设备 */
};

struct awbl_spi_config {
    /**
     * brief 当前配置的bpw */
    uint8_t     bpw;

    /** \brief 当前配置的模式  */
    uint16_t    mode;

    /** \brief 当前配置的速度 */
    uint32_t    speed_hz;
};

typedef struct awbl_spi_tgl_pfunc_cs {
    /**
     * \brief 片选引脚编号
     *
     * 当 aw_spi_device.pfunc_cs 不为NULL时，使用此设置。默认低电平有效；若
     * aw_spi_device.mode 中 AW_SPI_CS_HIGH 标志被设置，则高电平有效。
     */
    int         cs_pin;

    /**
     * \brief 片选控制函数
     *
     * 使用此函数控制设备的片选: 参数 \a state 为1时片选有效，为0时片选无效。
     * 若此配置为NULL时，将使用 aw_spi_device.cs_pin 作为片选引脚，且驱动可能
     * 重新设置一个合适的片选控制函数。
     *
     * \param[in] cs_pin  设备片选引脚
     * \param[in] state  片选控制
     */
    void      (*pfunc_cs)(int cs_pin, int state);
}awbl_spi_tgl_pfunc_cs_t;

/** \brief AWBus SPI 总线控制器 (SPI主机) 实例 */
struct awbl_spi_master {
    struct awbl_busctlr super;      /**< \brief 继承自 AWBus 总线控制器 */
    struct awbl_spi_master *p_next; /**< \brief 指向下一个 SPI 控制器 */

    /** \brief SPI 控制器相关信息 (驱动配置部分) */
    const struct awbl_spi_master_devinfo2 *p_devinfo2;

    /** \brief 控制器互斥量 */
    AW_MUTEX_DECL(dev_mutex);

    /** \brief 总线锁定标志, 1-总线被锁定 0-总线未被锁定 */
    uint8_t         bus_lock_flag;

    /**< /brief 片选自锁标志 */
    uint8_t         cs_toggle;

    /**< /brief 当前自锁设备的CS脚  */
    int             cs_tgl_pin;

    /**< /brief 当前自锁设备的模式 */
    int             cs_tgl_mode;

    /**< /brief 当前自锁设备的函数指针  */
    void           (*tgl_pfunc_cs) (int state);

    /**< /brief 当前配置信息  */
    struct awbl_spi_config cur_config;
};


/**
 * \brief AWBus SPI 总线模块初始化函数
 *
 * \attention 本函数应当在 awbus_lite_init() 之后、awbl_dev_init1() 之前调用
 */
void awbl_spibus_init(void);

/**
 * \brief 创建 AWBus SPI 总线实例以及枚举总线上的设备
 *
 * 本接口提供给 AWBus SPI 总线控制器(主机设备)驱动使用
 *
 */
aw_err_t awbl_spibus_create(struct awbl_spi_master *p_master);

/**
 * \brief 设置SPI设备结构体参数
 * \param p_dev         AWBus SPI从机设备实例
 * \param bits_per_word 字大小，为0时使用默认的“8-bit字大小”
 * \param mode          设备模式标志，见“SPI模式标志”
 * \param max_speed_hz  设备支持的最大速度
 * \param cs_pin        片选引脚编号(pfunc_cs 为NULL时，本参数有效)
 * \param pfunc_cs      片选控制函数(本参数不为 NULL 时， cs_pin 参数无效)
 *
 * \par 范例
 * 见 awbl_spi_setup()
 */
aw_local aw_inline void awbl_spi_mkdev (struct awbl_spi_device *p_dev,
                                        uint8_t                 bits_per_word,
                                        uint16_t                mode,
                                        uint32_t                max_speed_hz,
                                        int                     cs_pin,
                                        void (*pfunc_cs)(int state))
{
    aw_spi_mkdev(&p_dev->spi_dev,
                 AWBL_SPI_PARENT_BUSID_GET(p_dev),
                 bits_per_word,
                 mode,
                 max_speed_hz,
                 cs_pin,
                 pfunc_cs);
}

/**
 * \brief 设置SPI从机设备
 *
 * 本接口提供给 AWBus SPI 从设备驱动使用
 *
 * \attention 在SPI从机设备被使用前，必须先调用本函数进行设置，且如本函数返回
 *            错误，则一定不能再使用此SPI从机
 *
 * \param[in,out]   p_dev   SPI从机设备
 *
 * \retval   AW_OK      成功
 * \retval  -ENXIO      未找到指定的SPI总线
 * \retval  -AW_ENOTSUP    某些特性不支持
 *
 * \par 示例
 * \code
 *
 * // 初始化设备描述结构
 * awbl_spi_mkdev(p_dev,          // AWBus SPI 设备实例
 *                8，             // 字大小为8-bit
 *                AW_SPI_MODE_0,  // SPI 模式0
 *                500000,         // 支持的最大速度 500000 Hz
 *                PIO0_3,         // 片选引脚为 PIO0_3
 *                NULL);          // 无自定义的片选控制函数
 *
 * // 设置设备
 * awbl_spi_setup(p_dev);
 *
 * \endcode
 */
aw_err_t awbl_spi_setup(struct awbl_spi_device *p_dev);

/**
 * \brief 处理消息、异步模式
 *
 * 本接口提供给 AWBus SPI 从设备驱动使用
 *
 * 以异步的方式处理消息，消息的处理状态和结果反映在 p_msg->status。
 * 消息处理结束(成功或出错)时，将会调用 p_msg->pfunc_complete 并传递参数
 * p_msg->p_arg。
 *
 * \param[in]       p_dev   AWBus SPI 设备实例
 * \param[in,out]   p_msg   要处理的消息
 *
 * 函数的返回值如下：
 * \retval  AW_OK       消息排队成功，等待处理
 * \retval  -EINVAL     参数错误
 *
 * 成功发送/接收的数据个数反映在 p_msg->actual_length
 *
 * 消息的处理状态和结果反映在 p_msg->status ：
 *
 *      \li  -AW_ENOTCONN       消息尚未排队
 *      \li  -AW_EISCONN        消息正在排队
 *      \li  -AW_EINPROGRESS    消息正在被处理
 * 下面为消息处理完毕的结果
 *      \li  AW_OK              所有传输成功处理
 *      \li  -AW_ENOTSUP        消息中某个传输的配置不支持(例如，字大小、速度等)
 *      \li  -AW_ECANCELED      因控制器出错或处理队列满，消息被取消，可稍后再试
 *
 * \par 范例
 * \code
 *  aw_spi_device_t   spi_dev;
 *  aw_spi_message_t  msg;
 *  aw_spi_transfer_t trans[3];
 *  uint8_t           txbuf[16];
 *  uint8_t           rxbuf[16];
 *
 *  // 设置设备
 *  if (aw_spi_setup(&spi_dev) != AW_OK) {
 *      // 配置设备失败
 *  }
 *
 *  //  第一个传输,发送16个字节
 *  aw_spi_mktrans(&trans[0],       // 传输描述结构 0
 *                 &txbuf[0],       // 发送缓冲区
 *                 NULL,            // 接收缓冲
 *                 16,              // 发送个数
 *                 0,               // cs_change 无改变
 *                 0,               // bpw 使用默认配置
 *                 0,               // udelay 无延时
 *                 0);              // speed 使用默认值
 *
 *  //  第二个传输,接收16个字节
 *  aw_spi_mktrans(&trans[1],       // 传输描述结构 1
 *                 NULL,            // 发送缓冲区
 *                 &rxbuf[0],       // 接收缓冲
 *                 16,              // 接收个数
 *                 0,               // cs_change 无改变
 *                 0,               // bpw 使用默认配置
 *                 0,               // udelay 无延时
 *                 0);              // speed 使用默认值
 *
 *  //  第三个传输,发送且接收16个字节
 *  aw_spi_mktrans(&trans[2],       // 传输描述结构 2
 *                 &txbuf[0],       // 发送缓冲区
 *                 &rxbuf[0],       // 接收缓冲
 *                 16,              // 接收个数
 *                 0,               // cs_change 无改变
 *                 0,               // bpw 使用默认配置
 *                 0,               // udelay 无延时
 *                 0);              // speed 使用默认值
 *
 *  // 将传输组成消息
 *  aw_spi_msg_init(&msg, my_callback, my_arg); // 初始化消息
 *  aw_spi_trans_add_tail(&msg, &trans[0]);     // 添加第1个传输
 *  aw_spi_trans_add_tail(&msg, &trans[1]);     // 添加第2个传输
 *  aw_spi_trans_add_tail(&msg, &trans[2]);     // 添加第3个传输
 *
 *  // 异步发送此消息，函数立即返回，发送完成后会调用函数 my_callback (my_arg)
 *  awbl_spi_async(p_dev, &msg);
 *
 *  // 同步发送此消息，发送完成后函数才返回
 *  awbl_spi_sync(p_dev, &msg);
 *
 * \endcode
 *
 * \sa aw_spi_sync()
 */
aw_err_t awbl_spi_async(struct awbl_spi_device *p_dev,
                        struct aw_spi_message  *p_msg);

/**
 * \brief 处理消息、同步模式
 *
 * 本接口提供给 AWBus SPI 从设备驱动使用
 *
 * 以同步的方式处理消息，消息的处理状态和结果反映在 p_msg->status。
 *
 * \param[in]       p_dev   AWBus SPI 设备实例
 * \param[in,out]   p_msg   要处理的消息
 *
 * 函数的返回值如下：
 * \retval AW_OK            消息处理成功
 * \retval -AW_EINVAL       参数错误，p_msg 中某些成员参数无效，请检查
 * \retval -AW_ENOTSUP      消息中某个传输的配置不支持(例如，字大小、速度等)
 * \retval -AW_ECANCELED    因控制器出错或处理队列满，消息被取消，可稍后再试
 *
 * 成功发送/接收的数据个数反映在 p_msg->actual_length
 *
 * 消息的处理状态和结果反映在 p_msg->status ：
 *
 *      \li  -AW_ENOTCONN           消息尚未排队
 *      \li  -AW_EISCONN            消息正在排队
 *      \li  -AW_EINPROGRESS        消息正在被处理
 * 下面为消息处理完毕的结果
 *      \li  AW_OK              所有传输成功处理
 *      \li  -AW_ENOTSUP        消息中某个传输的配置不支持(例如，字大小、速度等)
 *      \li  -AW_ECANCELED      因控制器出错或处理队列满，消息被取消，可稍后再试
 *
 * \par 范例
 * 见 aw_spi_async()
 *
 * \sa aw_spi_async()
 */
aw_err_t awbl_spi_sync(struct awbl_spi_device *p_dev,
                       struct aw_spi_message  *p_msg);


/**
 * \brief 先写后读
 *
 * 本接口提供给 AWBus SPI 从设备驱动使用
 *
 * 本函数先进行SPI写操作，再进行读操作。
 * 在实际应用中，写缓冲区可以为地址，读缓冲区为要从该地址读取的数据。
 *
 * \param[in]   p_dev   AWBus SPI 设备实例
 * \param[in]   p_txbuf 数据发送缓冲区
 * \param[in]   n_tx    要发送的数据字节个数
 * \param[out]  p_rxbuf 数据接收缓冲区
 * \param[in]   n_rx    要接收的数据字节个数
 *
 * \retval AW_OK        消息处理成功
 * \retval -AW_EINVAL   参数错误，p_msg 中某些成员参数无效，请检查
 * \retval -AW_ENOTSUP     消息中某些传输设置的特性不支持
 */
aw_err_t awbl_spi_write_then_read(struct awbl_spi_device *p_dev,
                                  const uint8_t          *p_txbuf,
                                  size_t                  n_tx,
                                  uint8_t                *p_rxbuf,
                                  size_t                  n_rx);


/**
 * \brief 执行两次写操作
 *
 * 本接口提供给 AWBus SPI 从设备驱动使用
 *
 * 本函数连续执行两次写操作，依次发送数据缓冲区0和缓冲区1中的数据。
 * 在实际应用中，缓冲区0可以为地址，缓冲区1为要写入该地址的数据。
 *
 * \param[in]   p_dev    AWBus SPI 设备实例
 * \param[in]   p_txbuf0 数据发送缓冲区0
 * \param[in]   n_tx0    缓冲区0数据个数
 * \param[out]  p_txbuf1 数据发送缓冲区1
 * \param[in]   n_tx1    缓冲区1数据个数
 *
 * \retval AW_OK            消息处理成功
 * \retval -AW_EINVAL       参数错误，p_msg 中某些成员参数无效，请检查
 * \retval -AW_ENOTSUP      消息中某些传输设置的特性不支持
 */
aw_err_t awbl_spi_write_then_write(struct awbl_spi_device *p_dev,
                                   const uint8_t          *p_txbuf0,
                                   size_t                  n_tx0,
                                   const uint8_t          *p_txbuf1,
                                   size_t                  n_tx1);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_SPIBUS_H */

/* end of file */
