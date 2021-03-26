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
 * \brief 串口通讯标准接口
 *
 * 本模块提供串口读写，控制等服务
 *
 * 使用本模块需要包含头文件 aw_serial.h
 *
 * \par 简单示例
 * \code
 * aw_serial_write(COM0, "Hello!", 7);  // COM0输出"Hello!"
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 12-10-29  orz, first implementation.
 * - 1.00 18-10-24  hsg, add serial dcb.
 * \endinternal
 */

#ifndef __AW_SERIAL_H
#define __AW_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_serial
 * \copydoc aw_serial.h
 * @{
 */

#include "aw_common.h"
#include "aw_sio_common.h"

/**
 * \name 串口索引
 * \brief 目前提供最多10个串口，从0开始编号
 * @{
 */
#define COM0    0
#define COM1    1
#define COM2    2
#define COM3    3
#define COM4    4
#define COM5    5
#define COM6    6
#define COM7    7
#define COM8    8
#define COM9    9
/** @} */


/**
 * \name 串口校验方法取值
 *  @{
 */
#define AW_SERIAL_EVENPARITY     (0)    /** \breif 偶效验  */
#define AW_SERIAL_ODDPARITY      (1)    /** \breif 奇效验  */


/**
 * \name 串口使用的停止位数取值
 *  @{
 */
#define AW_SERIAL_ONESTOPBIT     (0)    /** \brief 1位停止位  */
#define AW_SERIAL_ONE5STOPTS     (1)    /** \brief 1.5 位停止位  */
#define AW_SERIAL_TWOSTOPBITS    (2)    /** \brief 2   位停止位  */
#define AW_SERIAL_STOP_INVALID   (0x3)  /** \brief 3是无效停止位  */


/**
 * \name 串口设备RTS(request-to-send)流控取值
 *  @{
 */
#define AW_SERIAL_RTS_HANDSHAKE  (0)    /** \brief 硬件流控  */
#define AW_SERIAL_RTS_DISABLE    (1)    /** \brief 软件控制RTS无效  */
#define AW_SERIAL_RTS_ENABLE     (2)    /** \brief 软件控制RTS有效  */
#define AW_SERIAL_RTS_INVALID    (0x3)  /** \brief 无效RTS  */


/**
 * \name 串口设备DTR(data-terminal-ready)流控取值
 *  @{
 */
#define AW_SERIAL_DTR_HANDSHAKE  (0)
#define AW_SERIAL_DTR_DISABLE    (1)
#define AW_SERIAL_DTR_ENABLE     (2)
#define AW_SERIAL_DTR_INVALID    (0x3)


#pragma pack(push)
#pragma pack(1)

/** @} */
/**
 * \brief  串口配置结构描述
 */
struct aw_serial_dcb {
	
    /**
     * \brief 指定当前端口使用的数据位
     *
     * 范围：5 ~ 8
     */
    uint32_t                byte_size:4;

    /**
     * \brief 是否允许奇偶校验,为0不允许校验，为1则校验方式看parity的值
     */
    uint32_t                f_parity:1;

    /**
     * \brief 指定端口数据传输的校验方法
     */
    uint32_t                parity:1;

    /**
     * \brief 指定端口当前使用的停止位数
     */
    uint32_t                stop_bits:2;

    /**
     * \brief 是否监控CTS(clear-to-send)信号来做输出流控。
     *
     * 当设置为1时：CTS无效时数据发送被挂起，直至CTS有效。
     */
    uint32_t                f_ctsflow:1;

    /**
     * \brief 设置RTS(request-to-send)流控。
     */
    uint32_t                f_rtsctrl:2;

    /**
     * \brief 设置dsr_sensitivity, 如果为1,则对DSR信号敏感,除非DSR信号有效，
     *        否则将忽略所有接收的字节
     */
    uint32_t                f_dsrsensitivity:1;

    /**
     * \brief 是否监控DSR(data-set-ready信号来做输出流控)。
     *
     * 当设置为1时：若DSR无效时数据发送被挂起，直至DSR有效。
     */
    uint32_t                f_dsrflow:1;

    /**
     * \brief 设置DTR(data-termial-ready)流控。
     */
    uint32_t                f_dtrctrl:2;

    /**
     * \brief XON/XOFF流量控制在发送时是否可用。
     *
     * 当设置为1时：当xoff值被收到时，发送停止；当xon值被收到时，发送继续。
     */
    uint32_t                f_outx:1;

    /**
     * \brief XON/XOFF流量控制在接收时是否可用。
     *
     * 当设置为1时：当接收缓冲区中空余容量小于xoff_lim字节时，发送xoff字符；
     *              当接收缓冲区中已有xon_lim字节的空余容量时，发送xon字符。
     */
    uint32_t                f_inx:1;

    uint32_t                f_dummy:15;

    /**
     * \brief 波特率
     */
    uint32_t                baud_rate;

    /**
     * \brief 在XON字符发送前接收缓冲区内空余容量的最小字节数
     */
    uint32_t                xon_lim;
    /**
     * \brief 在XOFF字符发送前接收缓冲区内空余容量的最大字节数
     */
    uint32_t                xoff_lim;

    /**
     * \brief 指定XON字符
     */
    char                    xon_char;

    /**
     * \brief 指定XOFF字符
     */
    char                    xoff_char;

    /**
     * \brief 快速响应接收模式
     *
     * 当设置为1时：串口接收到一个数据后将会立即触发中断，适用于一些
     *              对于串口响应速度要求较高的情况，例如Modbus的RTU模式；
     *              需要注意的是：使能该标志后，串口的接收中断将被
     *              频繁的触发，增加CPU的负荷。
     */
    uint8_t                 f_fast_reaction:1;
};
#pragma pack(pop)


/**
 *  \brief 串口超时参数
 *
 *  \note: 例如：设置rd_timeout为100ms(表示等待接收第1个数据的时间为100ms)，
 *             设置rd_interval_timeout为10ms(表示在接收到第1个数据后，
 *                                         以后每2字节数据之间的超时时间为10ms)。
 *             调用aw_serial_read()，设置需要读取20字节数据。
 *
 *             情况1：第1个字节是在第101ms时来临，串口将读到0字节数据并返回0表示超时。
 *             情况2：第1个字节是在第99ms时来临的，第103ms时来了第2字节数据，以后每4ms来1字节数据，一共
 *                  来了20字节数据，则串口将读到20字节数据，正常返回读取了20字节数据。
 *             情况3：第1个字节是在第99ms时来临的，第114ms时来了第2字节数据，以后第15ms来1字节数据，一共
 *                  来了20字节数据，则串口将读到1字节数据，并返回1表示读到了1字节数据。
 */
struct aw_serial_timeout {

    /**
     * \brief 读超时时间，单位为tick，这个时间是等待接收到第1个字节数据的超时时间。
     *        如果在这个时间内没能接收到1个数据就超时返回，
     *        如果在这个时间内接收到第1个数据，后续就每次判断是否码间超时。
     */
    uint32_t         rd_timeout;

    /**
     * \brief 码间超时，单位为tick，这个超时用于每2个字节之间的超时等待。
     *        如果第n个字节与第n+1个字节之间的时间超过了这个时间就超时返回。
     *        否则继续等待后续数据，每次等待的超时时间为这个超时时间。
     */
    uint32_t         rd_interval_timeout;
};


/**
 * \brief 串口设备控制
 *
 * - 这个函数处理所有的串口设备ioctl命令
 * - FIOBAUDRATE映射到了sio_ioct的SIO_BAUD_SET命令，其它所有的ioctl命令都直接
 *   传给了sio_ioctl函数
 *
 * \param[in] com     串口编号
 *
 * \param[in] request iotcl命令请求，参见 aw_ioctl.h 和 aw_sio_common.h ，
 *              可用的请求及参数如下：
 *              @arg AW_FIONREAD，可读数据个数，参数为int型指针
 *              @arg AW_FIOFLUSH，清空读缓冲，无参数
 *              @arg AW_FIOSYNC，同FIOFLUSH
 *              @arg AW_FIORFLUSH，同FIOFLUSH
 *              @arg AW_TIOCRDTIMEOUT，设置读等待全局超时时间，单位毫秒，参数为
 *                   int整数
 *              @arg AW_TIOISATTY，检查设备是否TTY设置，无参数，TTY设备返回
 *                   true值
 *              @arg SIO_BAUD_SET，设置串行设备的波特率，参数为波特率值，如9600
 *              @arg SIO_BAUD_GET，获取串行设备的波特率，参数为int型指针
 *              @arg SIO_MODE_SET，设置串行设备的模式，参数为：
 *                  - SIO_MODE_INT，中断模式
 *              @arg SIO_MODE_GET，获取串行设备的当前模式，参数为int指针，值为
 *                   SIO_MODE_INT
 *              @arg SIO_AVAIL_MODES_GET，获取串行设备支持的模式，参数为int指针，
 *                   SIO_MODE_INT 或两者相“或”后的值
 *              @arg SIO_HW_OPTS_SET，设置串行设备的通讯参数，
 *                  支持的参数（这些参数可以相互“或”）：
 *                  - CLOCAL， 忽略Modem线状态，使用3线UART时必须使用这个参数
 *                  - CREAD，使能接收数据，默认已经使能
 *                  - 数据位数：
 *                      - CS5： 5位数据位
 *                      - CS6： 6位数据位
 *                      - CS7： 7位数据位
 *                      - CS8： 8位数据位
 *                  - STOPB，设置停止位个数，不设置就默认为1位，设置则为2位
 *                  - PARENB，奇偶校验使能设置，不设置则关闭奇偶校验，如设置则：
 *                      - PARENB：使能偶校验
 *                      - (PARENB | PARODD)：使能奇校验
 *              @arg SIO_HW_OPTS_GET，获取串行设备的通讯参数，参数为int指针，值
 *                  请参考 SIO_HW_OPTS_SET 可设置的值
 *
 * \param[in,out] p_arg   ioctl命令请求参数，参数类型和取值取决于对应的请求命令
 *
 * \return 返回值为负的错误编码，见 aw_errno.h 。具体取决于请求命令，典型返回值：
 *  - \b  AW_OK       请求成功
 *  - \b -AW_EINVAL   参数错误
 *  - \b -AW_ENODEV   com对应的串口设备没有找到（系统内部错误）
 *  - \b -AW_ENOTSUP  该设备不支持此操作或者不支持此功能
 *
 * \par 范例
 * \code
 * int baud;
 * aw_serial_ioctl(COM0, SIO_BAUD_SET, (void *)115200); // 设置波特率为115200bps
 * aw_serial_ioctl(COM0, SIO_BAUD_GET, (void *)&baud);  // 获取当前波特率值
 * aw_serial_ioctl(COM0, AW_TIOCRDTIMEOUT, (void *)20); // 设置读超时为20毫秒
 * // 设置串口参数：忽略modem线状态，8个数据位，1个停止位，使能偶校验
 * aw_serial_ioctl(COM0, SIO_HW_OPTS_SET, (void *)(CLOCAL | CS8 | PARENB));
 * \endcode
 */
aw_err_t aw_serial_ioctl (uint32_t com, int request, void *p_arg);

/**
 * \brief 向串口设备写入nbytes个字节数据
 *
 * 这个接口向串口设备写入nbytes个字节数据。若设备的内部缓冲不够，将做如下处理：
 *
 * \param[in] com       串口编号
 * \param[in] p_buffer  要写入设备的数据缓冲区指针
 * \param[in] nbytes    要写入的字节个数，不能超过int的大小
 *
 * \return 成功写入的数据个数，小于0为失败，失败原因为负的错误码，可查看
 *         aw_errno.h
 *
 * \note 当写返回要写入的字节数后,不一定写就完成了,这个接口只是将要写的数据放入
 *       发送缓冲区中。具体什么时候发送完成是未知的, 总之会将缓冲区中的数据发送
 *       完成为止。
 */
int aw_serial_write (uint32_t com, aw_const void *p_buffer, size_t nbytes);


/**
 * \brief 从串口设备读取maxbytes个字节数据
 *
 * - 这个接口从串口设备读入最多maxbytes个字节数据
 * - 若设备可读数据个数为0并且读超时设置为永久，将做如下处理：
 *   - 调用会被阻塞直到读完maxbytes个数据
 *
 * \param[in]  com      串口编号
 * \param[out] p_buffer 存放读取数据的缓冲区指针
 * \param[in]  maxbytes 最多要读取的字节个数
 *
 * \return 成功读取的数据个数，小于0为失败，失败原因为负的错误码，可查看
 *         aw_errno.h
 */
int aw_serial_read (uint32_t com, void *p_buffer, size_t maxbytes);


/**
 * \brief 轮询方式向串口设备写入nbytes个字节数据
 *
 * 这个接口以轮询方式向串口设备写入nbytes个字节数据
 *
 * \param[in] com       串口编号
 * \param[in] p_buffer  要写入设备的数据缓冲区指针
 * \param[in] nbytes    要写入的字节个数
 *
 * \return 成功写入的数据个数，小于0为失败，失败原因为负的错误码，可查看
 *         aw_errno.h
 *
 * \note 此接口只能在异常错误(data_abort, prefetch_abort, undef_instruction)中间
 *       上下文中调用,在非异常中调用无反应,返回0
 */
int aw_serial_poll_write (uint32_t com, const void *p_buffer, size_t nbytes);


/**
 * \brief 配置串口设备参数
 *
 * \param[in]  com      串口编号
 * \param[in]  dcb      要写入串口配置结构的指针
 *
 *
 * \return 返回值为负的错误编码，见 aw_errno.h，典型返回值：
 *  - \b  AW_OK         配置成功，没有错误
 *  - \b -AW_EINVAL     参数错误
 *  - \b -AW_ENODEV     com对应的串口设备没有找到（系统内部错误）
 *  - \b -AW_ENOTSUP    在中断上下文中调用，或者某些功能不支持
 *
 * \note 在进行配置前，需要先调用aw_serial_dcb_get函数获取当前配置，然后修改需要
 *       配置的项，再使用aw_serial_dcb_set函数进行配置
 *
 * \par 范例
 * \code
 * struct aw_serial_dcb dcb;
 * //获取当前配置
 * aw_serial_dcb_get(COM0, &dcb);
 * //修改需要配置的项，如波特率
 * dcb.baud_rate = 9600;
 * //配置串口
 * aw_serial_dcb_set(COM0, &dcb);
 * \endcode
 */
aw_err_t aw_serial_dcb_set (uint32_t com, const struct aw_serial_dcb *p_dcb);


/**
 * \brief 获取串口设备配置参数
 *
 * \param[in]  com      串口编号
 * \param[out] p_dcb    存放串口配置参数的指针
 *
 * \return 返回值为负的错误编码，见 aw_errno.h，典型返回值：
 *  - \b  AW_OK       获取成功，没有错误
 *  - \b -AW_EINVAL   com参数错误，com值超出系统可用串口数量
 *  - \b -AW_ENODEV   com对应的串口设备没有找到（系统内部错误）
 */
aw_err_t aw_serial_dcb_get (uint32_t com, struct aw_serial_dcb *p_dcb);


/**
 * \brief 配置超时参数
 *
 * \param[in]  com      串口编号
 * \param[out] p_dcb    存放串口超时参数的指针
 *
 * \return 返回值为负的错误编码，见 aw_errno.h，典型返回值：
 *  - \b  AW_OK      获取成功，没有错误
 *  - \b -AW_EINVAL  com参数错误，com值超出系统可用串口数量, 或者码间超时大于总
 *                   超时
 *  - \b -AW_ENODEV  com对应的串口设备没有找到（系统内部错误）
 *  - \b -AW_ENOTSUP 不支持，在中断上下文中调用
 */
aw_err_t aw_serial_timeout_set(uint32_t com, struct aw_serial_timeout *p_cfg);


/**
 * \brief 获取超时参数
 *
 * \param[in]  com      串口编号
 * \param[out] p_cfg    存放串口超时参数的指针
 *
 * \return 返回值为负的错误编码，见 aw_errno.h，典型返回值：
 *  - \b  AW_OK       获取成功，没有错误
 *  - \b -AW_EINVAL   com参数错误，com值超出系统可用串口数量
 *  - \b -AW_ENODEV   com对应的串口设备没有找到（系统内部错误）
 */
aw_err_t aw_serial_timeout_get(uint32_t com, struct aw_serial_timeout *p_cfg);

/** @} grp_aw_if_serial */

#ifdef __cplusplus
}
#endif

#endif /* __AW_SERIAL_H */

/* end of file */
