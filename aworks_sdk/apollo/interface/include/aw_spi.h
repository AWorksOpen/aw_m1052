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
 * \brief SPI标准接口
 *
 * 使用本服务需要包含头文件 aw_spi.h
 *
 * \par 读取FLASH ID
 * \code
 *
 *  uint32_t cmd_rdid (aw_spi_device_t *p_dev)
 *  {
 *      uint8_t  cmd = 0x9f;    // RDID 命令
 *      uint32_t id;
 *
 *      // 发送 RDID 命令
 *      aw_spi_write_then_read(p_dev,
 *                             &cmd,
 *                             1,
 *                             (uint8_t *)&id,
 *                             3);
 *      return id;
 *  }
 * \endcode
 *
 * \par SPI 测试
 * \code
 *  
 *  int spi_test (void)
 *  {
 *      int i;
 *      aw_spi_device_t spi_flash;
 *
 *      // 生成 SPI FLASH 设备
 *      aw_spi_mkdev(&spi_flash,
 *                   SPI0,              // 位于哪条SPI总线上
 *                   8,                 // 字大小
 *                   AW_SPI_MODE_0,     // SPI 模式
 *                   3000000,           // 支持的最大速度
 *                   PIO0_2,            // 片选引脚
 *                   NULL);             // 无自定义的片选控制函数
 *
 *      // 设置 SPI FLASH 设备
 *      if (aw_spi_setup(&spi_flash) != AW_OK) {
 *          return AW_ERROR;            // 设置失败
 *      }
 *
 *      // 读取 JEDEC ID
 *      if (cmd_rdid(&spi_flash) != FLASH_JEDEC_ID) {
 *          return AW_ERROR;            // JEDEC ID 不正确
 *      }
 *
 *      return AW_OK
 *  }
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-16  zen, first implementation
 * \endinternal
 */

#ifndef __AW_SPI_H
#define __AW_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "aw_list.h"

/*lint ++flb */

/**
 * \addtogroup grp_aw_if_spi
 * \copydoc aw_spi.h
 * @{
 */

/**
 * \name SPI模式标志
 * @{
 */
 
/** \brief 设置后第2个时钟沿进行采样,否则在第1个时钟沿进行采样 */
#define AW_SPI_CPHA         0x01

/** \brief 设置后空闲时钟是高电平, 否则为低电平 */
#define AW_SPI_CPOL         0x02  
#define AW_SPI_CS_HIGH      0x04  /**< \brief 片选高有效, 否则片选低有效  */
#define AW_SPI_LSB_FIRST    0x08  /**< \brief 按低位先出的方式发送数据帧 */
#define AW_SPI_3WIRE        0x10  /**< \brief 3线模式，SI/SO 信号线共享*/
#define AW_SPI_LOOP         0x20  /**< \brief 回环模式  */
#define AW_SPI_NO_CS        0x40  /**< \brief 单设备总线, 无片选 */

/** \brief READY信号,从机拉低此信号暂停传输 */
#define AW_SPI_READY        0x80  
#define AW_SPI_AUTO_CS      0x100 /**< \brief 硬件自动控制 CS 输出 */

/**
 *  \brief SPI模式0 (MicroWire) 空闲时钟是低电平, 第1个时钟沿采样
 */
#define AW_SPI_MODE_0   (0 | 0)

/**
 *  \brief SPI模式1 空闲时钟是低电平, 第2个时钟沿采样
 */
#define AW_SPI_MODE_1   (0 | AW_SPI_CPHA)

/**
 *  \brief SPI模式2 空闲时钟是高电平, 第1个时钟沿采样
 */
#define AW_SPI_MODE_2   (AW_SPI_CPOL | 0)

/**
 *  \brief SPI模式3 空闲时钟是高电平, 第2个时钟沿采样
 */
#define AW_SPI_MODE_3   (AW_SPI_CPOL | AW_SPI_CPHA)

/** \brief SPI在读取过程中MOSI引脚输出高电平（默认为低电平）*/
#define AW_SPI_READ_MOSI_HIGH    0x0200

/** @} */

/**
 * \brief SPI 传输
 *
 * - SPI 传输写的个数总是等于读的个数。协议驱动应该总是提供 \a rx_buf 和/或
 * \a tx_buf 。在某些情况下，它们同样可能想提供数据传输的DMA地址，这样的话，底
 * 层驱动可以使用DMA，以降低数据传输的代价。
 *
 * - 如果传输缓冲区\a tx_buf 为NULL，则在填充\a rx_buf 时，0将会被移出到总线上。
 * 如果接收缓冲区 \a rx_buf 为NULL，则从总线上移入的数据将会被丢弃。只有 \a len
 * 个字节会被移出或移入。尝试移出部分字是错误的操作，例如，移出3个字节而字大小
 * 是 16 或 20 bit，前者每个字使用2个字节，而后者使用4个字节。
 *
 * - 在内存中，数据总是按CPU的的本地字节顺序存放，以线字节顺序传输 (即大端，除非
 * 设备模式或传输标志中被设置了 AW_SPI_LSB_FIRST)。例如，当\a bits_per_word 为
 * 16，缓冲区为 2N 字节长 (\a len = 2N) 并且以CPU字节顺便保存N个16-bit字。
 *
 * - 当 SPI 传输的字大小不是2的幂次倍8-bit时，这些内存中的字将包含额外的bit。在
 * 内中，字总是向右对齐的，因此，未定义 (rx) 或未使用 (tx) 的位总是最高有效位。
 *
 * - 所有SPI传输开始时，片选变得有效，通常它一直保持有效，直到消息的最后一个传输
 * 完成。驱动可以使用 \a cs_change 来影响片选:
 *  -# 如果该传输不是消息的最后一个，则该标志被用来在消息的中间让片选变为无效。
 *     以这种方式来翻转片选，可能需要终止一个芯片命令，让单个 aw_spi_message_t
 *     执行所有芯片事务组。
 *  -# 如果该传输是消息的最后一个，则片选保持有效直到下一个传输。在多设备SPI总线
 *     上，若没有要发送给其它设备的消息被阻塞，这便是提高性能的一个技巧；
 *     发送给另一个设备的消息开始执行后，便会使当前设备片选无效。但是，在其它某
 *     些情况下，该标志可以被用来保证正确性。某些设备需要协议事务由一系列的
 *     aw_spi_message_t 组成，其中一条消息的内容由前一条消息的结果决定，且整个
 *     事务由片选变为无效而结束。
 *
 *  -# 如果上一个message的片选还是有效的，此时来一个message的片选引脚和上一个引
 *     脚不相同，这种形为是不允许的，会返回一个相应的错误。
 *
 * \attention
 * 向底层提交 aw_spi_message_t 的代码负责管理它自己的内存。清零所有的你不需要明
 * 确设置的域，推荐使用 aw_spi_msg_init() 来初始化 aw_spi_message_t 。
 * 在提交消息以及它的传输之后，不能操作这些内存,直到消息的传输完成回调函数被调用
 * (使用异步函数 aw_spi_async())或函数返回(使用同步函数 aw_spi_sync())。
 */
typedef struct aw_spi_transfer {

    /** \brief 发送数据缓冲区(DMA安全) */
    const void *p_txbuf;

    /** \brief 接收数据缓冲区(DMA安全) */
    void       *p_rxbuf;

    /** \brief 发送/接收缓冲区的数据长度 */
    size_t      nbytes;

    /** \brief 字大小，若为0，则使用 aw_spi_device_t 中设置的值 */
    uint8_t     bits_per_word;

    /** \brief 本次传输完成后，是否影响片选: 1-影响 0-不影响 */
    uint8_t     cs_change;

    /**
     * \brief 在本次传之后，在改变片选状态之前延时的微秒数，然后开始下一个传输，
     *        或结束当前消息。
     */
    uint16_t    delay_usecs;

    /**
     * \brief 为本次传输选择一个与 aw_spi_device_t 中的默认设置不同的速度，
     *        0 表示使用默认值
     */
    uint32_t    speed_hz;

    /**
     * \brief 本次传输过程的一些特殊设置，比如在读取过程MOSI引脚的状态
     */
    uint32_t    flag;

    /** \brief 链表结点 */
    struct aw_list_head trans_node;

} aw_spi_transfer_t;

/**
 * \brief SPI 消息
 *
 * 1个SPI消息被用来执行一个原子的数据传输序列，每个传输用 aw_spi_transfer 表示。
 * 该序列是原子的，意味着任何其它SPI消息不可以使用SPI总线，直到该序列执行完成。
 * 在所有系统中，消息是被排队的，发送给某个 aw_spi_device_t 的消息总是以FIFO的
 * 顺序被处理。
 *
 * \attention
 * 向底层提交 aw_spi_message_t 的代码负责管理它自己的内存。清零所有的你不需要明
 * 确设置的域，推荐使用 aw_spi_msg_init() 来初始化 aw_spi_message_t 。
 * 在提交消息以及它的传输之后，不能操作这些内存,直到消息的传输完成回调函数被调用
 * (使用异步函数 aw_spi_async())或函数返回(使用同步函数 aw_spi_sync())。
 */
typedef struct aw_spi_message {

    /** \brief 指向当前message的控制器 */
    struct awbl_spi_master *p_master;

    /** \brief 组成消息的传输段 */
    struct aw_list_head transfers;   

    /** \brief 发送消息的SPI从设备 */    
    struct aw_spi_device *p_spi_dev; 

    /** \brief 传输完成回调函数 */
    aw_pfuncvoid_t  pfunc_complete;  

    /** \brief 传递给 pfunc_complete 的参数 */
    void           *p_arg;           

    /** \brief 所有成功发送的数据字节总数 */
    size_t          actual_length;

    /** \brief 消息的状态 */
    int             status;          

    /** \brief msg节点 */
    struct aw_list_head msg;

} aw_spi_message_t;

/** \brief SPI从设备描述结构 */
typedef struct aw_spi_device {

    /** \brief 设备所属主机接口编号 */
    uint8_t     busid;

    /**
     * brief 数据传输包含1个或多个字，比较常见的字大小为8-bit或12-bit，在内存
     * 中，字大小应该是2的幂次方，例如，20-bit自大小应使用32-bit内存。本设置可以
     * 被设备驱动改变，或者设置为0表示使用默认的“字大小为8-bit”。
     * “传输”的 aw_spi_transfer_t.bits_per_word 可以重载此配置。
     */
    uint8_t     bits_per_word;

    /** \brief 从设备的SPI模式标志，请参考“SPI模式标志”*/
    uint16_t    mode;

    /**
     * \brief 设备支持的最高速度
     *
     * “传输”的 aw_spi_transfer.speed_hz 可重载此设置。
     */
    uint32_t    max_speed_hz;

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
     * \param[in] state  片选控制
     */
    void      (*pfunc_cs) (int state);

    /**
     * \brief 函数调用判断
     *
     * 如果调用了aw_spi_mkdev,则setuped等于0x55555555，
     * 如果调用了aw_spi_setup,则setuped等于负的设备地址。
     */
    uint32_t   setuped;

} aw_spi_device_t;

/**
 * \brief 设置SPI设备结构体参数
 *
 * \param[in] p_dev         SPI从机设备描述符指针
 * \param[in] busid         设备所属主机接口编号
 * \param[in] bits_per_word 字大小，为0时使用默认的“8-bit字大小”
 * \param[in] mode          设备模式标志，见“SPI模式标志”
 * \param[in] max_speed_hz  设备支持的最大速度
 * \param[in] cs_pin        片选引脚编号(\a pfunc_cs 为NULL时，本参数有效)
 * \param[in] pfunc_cs      片选控制函数(本参数不为 NULL 时， \a cs_pin 参数无效)
 *
 * \par 范例
 * 见 aw_spi_setup()
 */
aw_local aw_inline void aw_spi_mkdev (aw_spi_device_t *p_dev,
                                      uint8_t          busid,
                                      uint8_t          bits_per_word,
                                      uint16_t         mode,
                                      uint32_t         max_speed_hz,
                                      int              cs_pin,
                                      void (*pfunc_cs) (int state))
{
    p_dev->busid         = busid;
    p_dev->bits_per_word = bits_per_word;
    p_dev->mode          = mode;
    p_dev->max_speed_hz  = max_speed_hz;
    p_dev->cs_pin        = cs_pin;
    p_dev->pfunc_cs      = pfunc_cs;
    p_dev->setuped       = 0x55555555;
}

/**
 * \brief 设置SPI传输结构体参数
 *
 * \param[in] p_trans       传输结构体描述符指针
 * \param[in] p_txbuf       发送数据缓冲区
 * \param[in] p_rxbuf       接收数据缓冲区
 * \param[in] nbytes        发送/接收缓冲区的数据长度
 * \param[in] cs_change     是否影响片选
 * \param[in] bits_per_word 字大小
 * \param[in] delay_usecs   在改变片选状态之前延时的微秒数
 * \param[in] speed_hz      次传输速度
 * \param[in] flag          本次传输过程的一些特殊设置
 *
 * \par sa aw_spi_async()
 */
aw_local aw_inline void aw_spi_mktrans (aw_spi_transfer_t *p_trans,
                                        const void        *p_txbuf,
                                        void              *p_rxbuf,
                                        size_t             nbytes,
                                        uint8_t            cs_change,
                                        uint8_t            bits_per_word,
                                        uint16_t           delay_usecs,
                                        uint32_t           speed_hz,
                                        uint32_t           flag)
{
    p_trans->p_txbuf        = p_txbuf;
    p_trans->p_rxbuf        = p_rxbuf;
    p_trans->nbytes         = nbytes;
    p_trans->cs_change      = cs_change;
    p_trans->bits_per_word  = bits_per_word;
    p_trans->delay_usecs    = delay_usecs;
    p_trans->speed_hz       = speed_hz;
    p_trans->flag           = flag;
}

/**
 * \brief 初始化消息
 *
 * \param[in] p_msg           SPI消息描述符指针
 * \param[in] pfunc_complete 传输完成回调函数(仅调用 aw_i2c_async() 时此参数有效,
 *                           调用 aw_i2c_sync()时本参数无效)
 * \param[in] p_arg          传递给回调函数的参数
 *
 * \sa aw_spi_async()
 */
aw_local aw_inline void aw_spi_msg_init (aw_spi_message_t  *p_msg,
                                         aw_pfuncvoid_t     pfunc_complete,
                                         void              *p_arg)
{
    memset(p_msg, 0, sizeof(*p_msg));

    AW_INIT_LIST_HEAD(&p_msg->transfers);

    p_msg->pfunc_complete = pfunc_complete;
    p_msg->p_arg          = p_arg;
    p_msg->status         = -AW_ENOTCONN;
}

/**
 * \brief 将传输添加到消息末尾
 *
 * \param[in] p_msg    SPI消息描述符指针
 * \param[in] p_trans  SPI传输描述符指针
 */
aw_local aw_inline void aw_spi_trans_add_tail (aw_spi_message_t  *p_msg,
                                               aw_spi_transfer_t *p_trans)
{
    aw_list_add_tail(&p_trans->trans_node, &p_msg->transfers);
}

/**
 * \brief 将传输从消息中删除
 *
 * \param[in] p_trans   SPI传输描述符指针
 */
aw_local aw_inline void aw_spi_trans_del (aw_spi_transfer_t *p_trans)
{
    aw_list_del(&p_trans->trans_node);
}

/**
 * \brief 设置SPI从机设备
 *
 * \attention 在SPI从机设备被使用前，必须先调用本函数进行设置，且如本函数返回
 *            错误，则一定不能再使用此SPI从机
 *
 * \param[in,out]   p_dev   SPI从机设备
 *
 * \retval   AW_OK      成功
 * \retval  -AW_ENXIO   未找到指定的SPI总线
 * \retval  -AW_ENOTSUP 某些特性不支持
 *
 * \par 示例
 * \code
 * aw_spi_device_t spi_dev;     // 设备描述结构
 *
 * // 初始化设备描述结构
 * aw_spi_mkdev(&spi_dev,
 *              SPI0,           // 位于SPI0总线上
 *              8，             // 字大小为8-bit
 *              AW_SPI_MODE_0,  // SPI 模式0
 *              500000,         // 支持的最大速度 500000 Hz
 *              PIO0_3,         // 片选引脚为 PIO0_3
 *              NULL);          // 无自定义的片选控制函数
 *
 * // 设置设备
 * aw_spi_setup(&spi_dev);
 *
 * \endcode
 */
aw_err_t aw_spi_setup(aw_spi_device_t *p_dev);

/**
 * \brief 处理消息、异步模式
 *
 * 以异步的方式处理消息，消息的处理状态和结果反映在\a p_msg->status。
 * 消息处理结束(成功或出错)时，将会调用\a p_msg->pfunc_complete 并传递参数
 * \a p_msg->p_arg。
 *
 * \param[in]       p_dev   SPI设备描述
 * \param[in,out]   p_msg   要处理的消息
 *
 * 函数的返回值如下：
 * \retval  AW_OK       消息排队成功，等待处理
 * \retval  -AW_EINVAL  参数错误
 *
 * 成功发送/接收的数据个数反映在\a p_msg->actual_length
 *
 * 消息的处理状态和结果反映在\a p_msg->status ：
 *
 *      \li  -AW_ENOTCONN    消息尚未排队
 *      \li  -AW_EISCONN     消息正在排队
 *      \li  -AW_EINPROGRESS 消息正在被处理
 * 下面为消息处理完毕的结果
 *      \li  AW_OK           所有传输成功处理
 *      \li  -AW_ENOTSUP     消息中某个传输的配置不支持(例如，字大小、速度等)
 *      \li  -AW_ECANCELED   因控制器出错或处理队列满，消息被取消，可稍后再试
 *
 * \par 范例
 * \code
 *  aw_spi_device_t   spi_dev;
 *  aw_spi_message_t  msg;
 *  aw_spi_transfer_t trans[3];
 *  uint8_t           txbuf[16];
 *  uint8_t           rxbuf[16];
 *
 *  // 初始化设备描述结构
 *  aw_spi_mkdev(&spi_dev,
 *               SPI0,           // 位于SPI0总线上
 *               8，             // 字大小为8-bit
 *               AW_SPI_MODE_0,  // SPI 模式0
 *               500000,         // 支持的最大速度 500000 Hz
 *               PIO0_3,         // 片选引脚为 PIO0_3
 *               NULL);          // 无自定义的片选控制函数
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
 *  aw_spi_async(&spi_dev, &msg);
 *
 *  // 同步发送此消息，发送完成后函数才返回
 *  aw_spi_sync(&spi_dev, &msg);
 *
 * \endcode
 *
 * \sa aw_spi_sync()
 */
aw_err_t aw_spi_async(aw_spi_device_t  *p_dev,
                      aw_spi_message_t *p_msg);

/**
 * \brief 处理消息、同步模式
 *
 * 以同步的方式处理消息，消息的处理状态和结果反映在\a p_msg->status。
 *
 * \param[in]       p_dev   SPI设备描述
 * \param[in,out]   p_msg   要处理的消息
 *
 * 函数的返回值如下：
 * \retval AW_OK          消息处理成功
 * \retval -AW_EINVAL     参数错误，\a p_msg 中某些成员参数无效，请检查
 * \retval -AW_ENOTSUP    消息中某个传输的配置不支持(例如，字大小、速度等)
 * \retval -AW_ECANCELED  因控制器出错或处理队列满，消息被取消，可稍后再试
 *
 * 成功发送/接收的数据个数反映在\a p_msg->actual_length，
 * 消息的处理状态和结果反映在\a p_msg->status:
 *      \li  -AW_ENOTCONN       消息尚未排队
 *      \li  -AW_EISCONN        消息正在排队
 *      \li  -AW_EINPROGRESS    消息正在被处理
 *
 * 下面为消息处理完毕的结果
 *      \li  AW_OK              所有传输成功处理
 *      \li  -AW_ENOTSUP        消息中某个传输的配置不支持(例如，字大小、速度等)
 *      \li  -AW_ECANCELED      因控制器出错或处理队列满，消息被取消，可稍后再试
 *
 * \sa aw_spi_async()
 */
aw_err_t aw_spi_sync(aw_spi_device_t  *p_dev,
                     aw_spi_message_t *p_msg);

/**
 * \brief 先写后读
 *
 * 本函数先进行SPI写操作，再进行读操作。
 * 在实际应用中，写缓冲区可以为地址，读缓冲区为要从该地址读取的数据。
 *
 * \param[in]      p_dev   SPI设备描述
 * \param[in]      p_txbuf 数据发送缓冲区
 * \param[in]      n_tx    要发送的数据字节个数
 * \param[out]     p_rxbuf 数据接收缓冲区
 * \param[in]      n_rx    要接收的数据字节个数
 *
 * \retval AW_OK        消息处理成功
 * \retval -AW_EINVAL   参数错误，\a p_msg 中某些成员参数无效，请检查
 * \retval -AW_ENOTSUP  消息中某些传输设置的特性不支持
 */
aw_err_t aw_spi_write_then_read(aw_spi_device_t *p_dev,
                                const uint8_t   *p_txbuf,
                                size_t           n_tx,
                                uint8_t         *p_rxbuf,
                                size_t           n_rx);

/**
 * \brief 执行两次写操作
 *
 * 本函数连续执行两次写操作，依次发送数据缓冲区0和缓冲区1中的数据。
 * 在实际应用中，缓冲区0可以为地址，缓冲区1为要写入该地址的数据。
 *
 * \param[in]   p_dev    SPI设备描述
 * \param[in]   p_txbuf0 数据发送缓冲区0
 * \param[in]   n_tx0    缓冲区0数据个数
 * \param[in]   p_txbuf1 数据发送缓冲区1
 * \param[in]   n_tx1    缓冲区1数据个数
 *
 * \retval AW_OK        消息处理成功
 * \retval -AW_EINVAL   参数错误，\a p_msg 中某些成员参数无效，请检查
 * \retval -AW_ENOTSUP  消息中某些传输设置的特性不支持
 */
aw_err_t aw_spi_write_then_write(aw_spi_device_t *p_dev,
                                 const uint8_t   *p_txbuf0,
                                 size_t           n_tx0,
                                 const uint8_t   *p_txbuf1,
                                 size_t           n_tx1);


/**
 * \brief 写并且读操作
 *
 *
 * \param[in]   p_dev    SPI设备描述
 * \param[in]   p_txbuf 数据发送缓冲区
 * \param[out]  p_rxbuf 数据发送缓冲区
 * \param[in]   n_tx1    缓冲区1数据个数
 *
 * \retval AW_OK        消息处理成功
 * \retval -AW_EINVAL   参数错误，\a p_msg 中某些成员参数无效，请检查
 * \retval -AW_ENOTSUP  消息中某些传输设置的特性不支持
 */
aw_err_t aw_spi_write_and_read (struct aw_spi_device   *p_dev,
                                const void             *p_txbuf,
                                void                   *p_rxbuf,
                                size_t                  nbytes);

/** @} grp_aw_if_spi */

#ifdef __cplusplus
}
#endif

#endif /* __AW_SPI_H */

/* end of file */
