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
 * \brief AWBus ZLG600A 接口头文件
 *
 * \internal
 * \par modification history:
 * - 1.00 16-05-03  win, first implementation
 * \endinternal
 */

#ifndef __AWBL_ZLG600A_H
#define __AWBL_ZLG600A_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_sem.h"
#include "aw_ioctl_generic.h"
#include "awbus_lite.h"

/**
 * \brief ZLG600A缓冲区描述符
 */
typedef struct awbl_zlg600a_info_buf_desc {
    uint8_t   *p_buf;
    uint16_t   len;
} awbl_zlg600a_info_buf_desc_t;

/** \brief 帧信息 */
typedef struct awbl_zlg600a_frame_base {
    uint8_t                    frame_seq;   /**< \brief 命令帧号（低4位有效） */
    uint8_t                    cmd_class;
    uint16_t                   cmd_status;
    uint16_t                   info_length;
    awbl_zlg600a_info_buf_desc_t *p_info_desc; /**< \brief 信息描述结构体指针 */
    uint8_t                       buf_num;     /**< \brief 信息缓冲区的个数   */
} awbl_zlg600a_frame_base_t;

/**
 * \brief AWBus ZLG600A 服务实例的相关信息
 */
struct awbl_zlg600a_servinfo {

    /** \brief ZLG600A 设备编号 */
    int dev_id;
};

/*
 * \name device ioctl function codes
 *       pfn_dev_ioctl() params 'request' use
 * @{
 */

/** \brief ioctl base */
#define AWBL_ZLG600A_DEV_IOCTL_BASE              (0xD0)

/** \brief get current card status */
#define AWBL_ZLG600A_DEV_IOCTL_BAUD_SET          \
     __AW_IOW(AWBL_ZLG600A_DEV_IOCTL_BASE, 1, int)

/** \brief get current card status */
#define AWBL_ZLG600A_DEV_IOCTL_ADDR_SET          \
    __AW_IOW(AWBL_ZLG600A_DEV_IOCTL_BASE, 2, int)

/** \brief get current card status */
#define AWBL_ZLG600A_DEV_IOCTL_FRAME_FMT_SET     \
    __AW_IOW(AWBL_ZLG600A_DEV_IOCTL_BASE, 3, int)

/** @} */

/**
 * \brief AWBus ZLG600A 服务函数
 */
struct awbl_zlg600a_servopts {

    /** \brief 帧发送函数         */
    aw_err_t (*pfn_frame_send) (void                      *p_cookie, 
                                awbl_zlg600a_frame_base_t *p_frame);

    /** \brief 帧接收函数         */
    aw_err_t (*pfn_frame_recv) (void                      *p_cookie, 
                                awbl_zlg600a_frame_base_t *p_frame);
    /** \brief 设备控制类函数 */
    aw_err_t (*pfn_dev_ioctl) (void *p_cookie, int request, void *p_arg);
};

/**
 * \brief ZLG600A服务结构体
 */
typedef struct awbl_zlg600a_service {

    /** \brief 指向下一个 ZLG600A 服务 */
    struct awbl_zlg600a_service *p_next;

    /** \brief ZLG600A 服务的相关信息 */
    const struct awbl_zlg600a_servinfo *p_servinfo;

    /** \brief ZLG600A 服务的相关函数 */
    const struct awbl_zlg600a_servopts *p_servopts;

    void    *p_cookie;
    uint8_t  frame_seq;    /**< \brief 记录包号（低4位有效） */

    AW_MUTEX_DECL(devlock);  /**< \brief 设备锁 */

} awbl_zlg600a_service_t;

/**
 * \brief AWBus ZLG600A 接口模块初始化
 *
 * \attention 本函数应当在 awbl_dev_connect()之后调用
 */
void awbl_zlg600a_init (void);

/**
 * \name ZLG600A的通信模式
 * @{
 */

#define AWBL_ZLG600A_MODE_I2C         1            /**< \brief I2C模式       */
#define AWBL_ZLG600A_MODE_UART        2            /**< \brief 串口模式      */
#define AWBL_ZLG600A_MODE_AUTO_CHECK  0            /**< \brief 自动侦测模式 */

/** @} */

/**
 * \name ZLG600A的帧格式
 * @{
 */

#define AWBL_ZLG600A_FRAME_FMT_OLD  0             /**< \brief  旧帧格式      */
#define AWBL_ZLG600A_FRAME_FMT_NEW  1             /**< \brief  新帧格式      */

 /** @} */

/**
 * \name 支持的波特率
 * @{
 */

#define  AWBL_ZLG600A_BAUDRATE_9600      9600     /**< \brief 波特率 9600   */
#define  AWBL_ZLG600A_BAUDRATE_19200     19200    /**< \brief 波特率 19200  */
#define  AWBL_ZLG600A_BAUDRATE_28800     28800    /**< \brief 波特率 28800  */
#define  AWBL_ZLG600A_BAUDRATE_38400     38400    /**< \brief 波特率 38400  */
#define  AWBL_ZLG600A_BAUDRATE_57600     57600    /**< \brief 波特率 57600  */
#define  AWBL_ZLG600A_BAUDRATE_115200    115200   /**< \brief 波特率 115200 */
#define  AWBL_ZLG600A_BAUDRATE_172800    172800   /**< \brief 波特率 172800 */
#define  AWBL_ZLG600A_BAUDRATE_230400    230400   /**< \brief 波特率 230400 */

/** @}*/

/**
 * \brief 读ZLG600A的产品、版本信息，如：“ZLG600A V1.00”
 *
 * \param[in] id     : 设备ID号
 * \param[in] p_info : 用于装载获取信息的缓冲区，大小应为 20 字节
 *
 * \retval AW_OK      : 读取信息成功
 * \retval -AW_EINVAL : 读取信息失败，参数错误
 * \retval -AW_EIO    : 读取信息失败，数据通信出错
 */
aw_err_t awbl_zlg600a_info_get (int id, uint8_t *p_info);

/**
 * \brief 配置IC卡接口，执行成功后，启动IC卡接口，启动后默认支持TypeA卡
 *
 * \param[in] id      : 设备ID号
 *
 * \retval AW_OK      : 配置成功
 * \retval -AW_EINVAL : 配置失败，参数错误
 * \retval -AW_EIO    : 配置失败，数据通信出错
 */
aw_err_t awbl_zlg600a_ic_port_config (int id);

/**
 * \brief 关闭IC卡接口
 *
 * \param[in] id  : 设备ID号
 *
 * \retval AW_OK      : 关闭成功
 * \retval -AW_EINVAL : 关闭失败，参数错误
 * \retval -AW_EIO    : 关闭失败，数据通信出错
 */
aw_err_t awbl_zlg600a_ic_port_close (int id);

/**
 * \name IC卡接口的协议（工作模式）
 * @{
 */
#define  AWBL_ZLG600A_IC_ISOTYPE_A      0x00    /**< \brief ISO14443-3A  */
#define  AWBL_ZLG600A_IC_ISOTYPE_B      0x04    /**< \brief ISO14443-3B  */
/** @}*/

/**
 * \brief 设置IC卡接口协议（工作模式）TypeA 或者 TypeB
 *
 * \param[in] id      : 设备ID号
 * \param[in] isotype : IC卡接口的协议类型，使用下列宏 ：
 *                          - #AWBL_ZLG600A_IC_ISOTYPE_A
 *                          - #AWBL_ZLG600A_IC_ISOTYPE_B
 *
 * \retval AW_OK      : 设置成功
 * \retval -AW_EINVAL : 设置失败，参数错误
 * \retval -AW_EIO    : 设置失败，数据通信出错
 */
aw_err_t awbl_zlg600a_ic_isotype_set (int id, uint8_t isotype);

/**
 * \name IC卡密匙类型
 * @{
 */

#define  AWBL_ZLG600A_IC_KEY_TYPE_A      0x60    /**< \brief 类型 A  */
#define  AWBL_ZLG600A_IC_KEY_TYPE_B      0x61    /**< \brief 类型 B  */

/** @}*/

/**
 * \brief 装载IC卡密钥
 *
 * 将输入的密钥保存在模块内部，模块掉电后该密钥不丢失，ZLG600A模块共能保存
 * A密钥16组、B密钥16组。
 *
 * \param[in] id         : 设备ID号
 * \param[in] key_type   : 密钥类型，可以使用下列值：
 *                          - #AWBL_ZLG600A_IC_KEY_TYPE_A
 *                          - #AWBL_ZLG600A_IC_KEY_TYPE_B
 * \param[in] key_sec    : 密钥保存的区号，共可保存16组密钥，区号：0 ~ 15
 * \param[in] p_key      : 密钥缓冲区
 * \param[in] key_length : 密钥的长度，只能为6（6字节密钥）或16（16字节密钥）
 *
 * \retval AW_OK      : 装载密钥成功
 * \retval -AW_EINVAL : 装载失败，参数错误
 * \retval -AW_EIO    : 装载失败，数据通信出错
 */
aw_err_t awbl_zlg600a_ic_key_load (int      id,
                                   uint8_t  key_type,
                                   uint8_t  key_sec,
                                   uint8_t *p_key,
                                   uint8_t  key_length);

/**
 * \brief 设置IC卡接口的寄存器值
 *
 * 用于设置模块上读写卡芯片内部的寄存器值，通过该命令，可以实现很多现有命令
 * 不能完成的工作。
 *
 * \param[in] id         : 设备ID号
 * \param[in] reg_addr   : 寄存器地址，范围：0x00 ~ 0x3F
 * \param[in] reg_val    : 设置的寄存器值
 *
 * \retval AW_OK      : 设置成功
 * \retval -AW_EINVAL : 设置失败，参数错误
 * \retval -AW_EIO    : 设置失败，数据通信出错
 */
aw_err_t awbl_zlg600a_ic_reg_set (int id, uint8_t reg_addr, uint8_t reg_val);

/**
 * \brief 获取IC卡接口的寄存器值
 *
 * 该命令用于设置模块上读写卡芯片内部的寄存器值，
 * 通过该命令，可以实现很多现有命令不能完成的工作。
 *
 * \param[in]  id         : 设备ID号
 * \param[in]  reg_addr   : 寄存器地址，范围：0x00 ~ 0x3F
 * \param[out] p_val      : 获取寄存器值的指针
 *
 * \retval AW_OK      : 获取成功
 * \retval -AW_EINVAL : 获取失败，参数错误
 * \retval -AW_EIO    : 获取失败，数据通信出错
 */
aw_err_t awbl_zlg600a_ic_reg_get (int id, uint8_t reg_addr, uint8_t *p_val);

/**
 * \brief 设置波特率
 *
 * 用于在UART通信过程中改变通信的波特率，掉电后该设置值保留。
 *
 * \param[in] id            : 设备ID号
 * \param[in] baudrate_flag : 波特率标志，并不支持所有的波特率，应使用宏
 *                            AWBL_ZLG600A_BAUDRATE_* ，如设置波特率为115200，
 *                            则使用宏 #AWBL_ZLG600A_BAUDRATE_115200
 *
 * \retval AW_OK      : 设置成功
 * \retval -AW_EINVAL : 设置失败，参数错误
 * \retval -AW_EIO    : 设置失败，数据通信出错
 */
aw_err_t awbl_zlg600a_baudrate_set (int id, uint32_t baudrate_flag);

/**
 * \name 天线驱动方式
 * @{
 */

#define  AWBL_ZLG600A_ANT_TX1_ONLY      1   /**< \brief 仅TX1驱动天线         */
#define  AWBL_ZLG600A_ANT_TX2_ONLY      2   /**< \brief 仅TX2驱动天线         */
#define  AWBL_ZLG600A_ANT_TX1_AND_TX2   3   /**< \brief TX1、TX2同时驱动天线  */
#define  AWBL_ZLG600A_ANT_NONE          0   /**< \brief 同时关闭TX1、TX2      */

/** @}*/

/**
 * \brief 设置天线驱动方式，可以任意打开、关闭某个天线驱动引脚
 *
 * \param[in] id       : 设备ID号
 * \param[in] ant_mode : 使用宏 AWBL_ZLG600A_ANT_*，如#AWBL_ZLG600A_ANT_TX1_ONLY
 *
 * \retval AW_OK      : 设置成功
 * \retval -AW_EINVAL : 设置失败，参数错误
 * \retval -AW_EIO    : 设置失败，数据通信出错
 */
aw_err_t awbl_zlg600a_ant_mode_set (int id, uint8_t ant_mode);

/**
 * \brief 设置帧格式，用于切换新、旧帧格式，设置成功后掉电不丢失
 *
 * \param[in] id          : 设备ID号
 * \param[in] frame_type  : 帧格式宏标志，可使用下列宏：
 *                          - #AWBL_ZLG600A_FRAME_FMT_NEW
 *                          - #AWBL_ZLG600A_FRAME_FMT_OLD
 *
 * \retval AW_OK      : 设置成功
 * \retval -AW_EINVAL : 设置失败，参数错误
 * \retval -AW_EIO    : 设置失败，数据通信出错
 *
 * \note 在串口通信方式时设置了新旧帧格式后将以原来的帧格式返回成功信息，但在I2C
 *       通信方式时，收到“设置新旧帧格式”命令后，即默认进入了设置后帧格式，
 *       不返回成功信息，下一次与模块通信需要使用设置后的帧格式与模块通信。
 *       同时为了保证I2C通信方式下，帧格式设置成功，下一次与模块通信需要延时至少
 *       125ms
 */
aw_err_t awbl_zlg600a_frame_fmt_set (int id, uint8_t frame_type);

/**
 * \name 主从模式
 * @{
 */

#define AWBL_ZLG600A_MODE_MASTER (1 << 4) /**< \brief 主机模式（保留功能） */
#define AWBL_ZLG600A_MODE_SLAVE  (0 << 4) /**< \brief 从机模式（默认）     */

/** @}*/

/**
 * \brief 设置设备工作模式和从机地址
 *
 * \param[in] id         : 设备ID号
 * \param[in] mode       : 模块的工作模式，可以使用下列宏值：
 *                          - #AWBL_ZLG600A_MODE_MASTER      主机模式
 *                          - #AWBL_ZLG600A_MODE_SLAVE       从机模式 （默认）
 *                          - #AWBL_ZLG600A_MODE_AUTO_CHECK  自动侦测模式
 *                          - #AWBL_ZLG600A_MODE_I2C         I2C模式
 *                          - #AWBL_ZLG600A_MODE_UART        UART模式
 *
 * \param[in] new_addr   : 设置的ZLG600A从机地址，最后一位为0，有效值0x02 ~ 0xFE
 *
 * \retval AW_OK      : 设置成功,设置成功后，后续应该使用新的地址通信
 * \retval -AW_EINVAL : 设置失败，参数错误
 * \retval -AW_EIO    : 设置失败，数据通信出错
 */
aw_err_t awbl_zlg600a_mode_addr_set (int id, uint8_t mode, uint8_t new_addr);

/**
 * \brief 获取设备工作模式和从机地址
 *
 * \param[in]  id         : 设备ID号
 * \param[out] p_mode     : 用于获取模块工作模式的指针，获取的值各位含义如下：
 *                          - bit7 ~ 4：0,从机模式; 1,自动检测卡片模式（主机模式）
 *                          - bit3 ~ 0：0,自动侦测模式; 1,I2C通信模式; 2,UART模式
 * \param[out] p_addr     : 用于获取当前的ZLG600A的从机地址的指针
 *
 * \retval AW_OK      : 获取成功
 * \retval -AW_EINVAL : 获取失败，参数错误
 * \retval -AW_EIO    : 获取失败，数据通信出错
 */
aw_err_t awbl_zlg600a_mode_addr_get (int id, uint8_t *p_mode, uint8_t *p_addr);

/**
 * \brief 装载用户密钥，模块里面提供了2个16字节的存储空间用于保存用户密钥。
 *
 * \param[in] id         : 设备ID号
 * \param[in] key_sec    : 密钥保存的区号，共可保存2组用户密钥，区号：0 ~ 1
 * \param[in] p_key      : 密钥缓冲区，密钥的长度固定为16字节
 *
 * \retval AW_OK      : 装载密钥成功
 * \retval -AW_EINVAL : 装载失败，参数错误
 * \retval -AW_EIO    : 装载失败，数据通信出错
 */
aw_err_t awbl_zlg600a_user_key_load (int id, uint8_t key_sec, uint8_t *p_key);

/**
 * \brief 写EEPROM，模块内部拥有一个256Byte的EEPROM
 *
 * \param[in] id          : 设备ID号
 * \param[in] eeprom_addr : EEPROM的片内地址，0 ~ 255
 * \param[in] p_buf       : 写入数据的缓冲区
 * \param[in] nbytes      : 写入数据的字节数，最大247字节
 *
 * \retval AW_OK      : 写入数据成功
 * \retval -AW_EINVAL : 写入失败，参数错误
 * \retval -AW_EIO    : 写入失败，数据通信出错
 */
aw_err_t awbl_zlg600a_eeprom_write (int      id,
                                    uint8_t  eeprom_addr,
                                    uint8_t *p_buf,
                                    uint8_t  nbytes);

/**
 * \brief 读EEPROM，模块内部拥有一个256Byte的EEPROM
 *
 * \param[in]  id          : 设备ID号
 * \param[in]  eeprom_addr : EEPROM的片内地址，0 ~ 255
 * \param[out] p_buf       : 读出数据的缓冲区
 * \param[in]  nbytes      : 读出数据的字节数，最大249字节
 *
 * \retval AW_OK      : 读取数据成功
 * \retval -AW_EINVAL : 读取失败，参数错误
 * \retval -AW_EIO    : 读取失败，数据通信出错
 */
aw_err_t awbl_zlg600a_eeprom_read (int      id,
                                   uint8_t  eeprom_addr,
                                   uint8_t *p_buf,
                                   uint8_t  nbytes);

/**
 * \brief 获取读卡芯片复位次数
 *
 * \param[in]  id      : 设备ID号
 * \param[out] p_count : 返回复位次数
 *
 * \retval AW_OK      : 读取数据成功
 * \retval -AW_EINVAL : 读取失败，参数错误
 * \retval -AW_EIO    : 读取失败，数据通信出错
 */
aw_err_t awbl_zlg600a_ic_reset_count_get (int id, uint32_t *p_count);

/**
 * \name Mifare卡请求模式
 * @{
 */
 
/** \brief IDLE模式,请求空闲的卡 */
#define AWBL_ZLG600A_MIFARE_CARD_REQ_IDLE 0x26 

/** \brief ALL模式,请求所有的卡  */
#define AWBL_ZLG600A_MIFARE_CARD_REQ_ALL  0x52 

/** @}*/

/**
 * \brief Mifare卡请求操作
 *
 *  该函数作为卡的请求操作，只要符合ISO14443A标准的卡都应能发出响应。
 *
 * \param[in] id          : 设备ID号
 * \param[in] req_mode    : 请求模式，可使用下列宏：
 *                           - #AWBL_ZLG600A_MIFARE_CARD_REQ_IDLE
 *                           - #AWBL_ZLG600A_MIFARE_CARD_REQ_ALL
 * \param[in] p_atq       : 获取请求应答信息（ATQ）的指针，其值为16位，不同卡
 *                          类型返回的信息不同,常见类型返回的ATQ如下：
 *
 *            Mifare1 S50    | Mifare1 S70 | Mifare1 Light | Mifare0 UltraLight
 *       --------------------|-------------|---------------|-------------------
 *              0x0004       |    0x0002   |    0x0010     |      0x0044
 *       ----------------------------------------------------------------------
 *          Mifare3 DESFire  |   SHC1101   |    SHC1102    |      11RF32
 *       --------------------|-------------|---------------|-------------------
 *               0x0344      |    0x0004   |    0x3300     |      0x0004
 *
 * \retval AW_OK      : 请求成功
 * \retval -AW_EINVAL : 请求失败，参数错误
 * \retval -AW_EIO    : 请求失败，数据通信出错
 *
 * \note 卡进入天线后，从射频场中获取能量，从而得电复位，复位后卡处于IDLE模式，
 * 用两种请求模式的任一种请求时，此时的卡均能响应；若对某一张卡成功进行了挂起
 * 操作（Halt命令或DeSelect命令），则进入了Halt模式，此时的卡只响应ALL（0x52）
 * 模式的请求，除非将卡离开天线感应区后再进入。
 */
aw_err_t awbl_zlg600a_mifare_card_request (int       id,
                                           uint8_t   req_mode,
                                           uint16_t *p_atq);

/**
 * \name Mifare卡防碰撞等级
 * @{
 */
#define AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_1  0x93  /**< \brief 第一级防碰撞  */
#define AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_2  0x95  /**< \brief 第二级防碰撞  */
#define AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_3  0x97  /**< \brief 第三级防碰撞  */
/** @}*/

/**
 * \brief Mifare卡的防碰撞操作
 *
 *  需要成功执行一次请求命令，并返回请求成功，才能进行防碰撞操作，否则返回错误。
 *
 * \param[in] id             : 设备ID号
 * \param[in] anticoll_level : 防碰撞等级，可使用下列宏：
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_1
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_2
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_3
 *
 * \param[in] know_uid : 已知的卡序列号
 * \param[in] nbit_cnt : 位计数，表明已知的序列号uid的位数。其值小于32。
 *                       - 若nbit_cnt值为0,则表明没有已知序列号，序列号的所有位
 *                         都要从本函数获得。
 *                       - 若nbit_cnt不为0，则序列号中有已知的序列号的值，表示
 *                         序列号的前nbit_cnt位为已知序列号中所示前nbit_cnt位的
 *                         值，其余位需要本函数获得。
 * \param[in] *p_uid   : 防碰撞后获取到的卡序列号
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 *
 * \note  符合ISO14443A标准卡的序列号都是全球唯一的，正是这种唯一性，才能实现防
 * 碰撞的算法逻辑，若有若干张卡同时在天线感应区内则这个函数能够找到一张序列号较
 * 大的卡来操作。
 */
aw_err_t awbl_zlg600a_mifare_card_anticoll (int       id,
                                            uint8_t   anticoll_level,
                                            uint32_t  know_uid,
                                            uint8_t   nbit_cnt,
                                            uint32_t *p_uid);

/**
 * \brief Mifare卡的选择操作
 *
 *  需要成功执行一次防碰撞命令，并返回成功，才能进行卡选择操作，否则返回错误。
 *
 * \param[in] id             : 设备ID号
 * \param[in] anticoll_level : 防碰撞等级，可使用下列宏：
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_1
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_2
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_3
 *
 * \param[in] know_uid : 已知的卡序列号
 * \param[in] nbit_cnt : 位计数，表明已知的序列号uid的位数。其值小于32。
 *                       - 若nbit_cnt值为0,则表明没有已知序列号，序列号的所有位
 *                         都要从本函数获得。
 *                       - 若nbit_cnt不为0，则序列号中有已知的序列号的值，表示
 *                         序列号的前nbit_cnt位为已知序列号中所示前nbit_cnt位的
 *                         值，其余位需要本函数获得。
 * \param[in] uid   : 前一个防碰撞函数获取的UID
 * \param[in] p_sak : 返回的信息，若bit2为1，则表明UID不完整
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 *
 * \note  卡的序列号长度有三种：4字节、7字节和10字节。 4字节的只要用一级选择即可
 * 得到完整的序列号，如Mifare1 S50/S70等；7字节的要用二级选择才能得到完整的序列
 * 号，前一级所得到的序列号的最低字节为级联标志0x88，在序列号内只有后3字节可用，
 * 后一级选择能得到4字节序列号，两者按顺序连接即为7字节序列号，如UltraLight
 * 和DesFire等；10字节的以此类推，但至今还未发现此类卡。
 */
aw_err_t awbl_zlg600a_mifare_card_select (int       id,
                                          uint8_t   anticoll_level,
                                          uint32_t  uid,
                                          uint8_t  *p_sak);

/**
 * \brief Mifare卡的挂起操作，使所选择的卡进入HALT状态
 *
 *  在HALT状态下，卡将不响应读卡器发出的IDLE模式的请求，除非将卡复位或离开天线感
 *  应区后再进入。但它会响应读卡器发出的ALL请求。
 *
 * \param[in] id      : 设备ID号
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_mifare_card_halt (int id);


/**
 * \brief Mifare卡E2密钥验证
 *
 *     用模块内部已存入的密钥与卡的密钥进行验证，使用该命令前应先用“装载IC卡密钥”
 * 函数把密钥成功载入模块内。另外，需要验证的卡的扇区号不必与模块内密钥区号相等。
 *
 * \param[in] id         : 设备ID号
 * \param[in] key_type   : 密钥类型，可以使用下列值：
 *                          - #AWBL_ZLG600A_IC_KEY_TYPE_A
 *                          - #AWBL_ZLG600A_IC_KEY_TYPE_B
 * \param[in] p_uid      : 卡序列号，4字节
 * \param[in] key_sec    : 密钥区号：0 ~ 7
 * \param[in] nblock     : 需要验证的卡块号，取值范围与卡类型有关，
 *                         - S50：0 ~ 63
 *                         - S70：0 ~ 255
 *                         - PLUS CPU 2K：0 ~ 127
 *                         - PLUS CPU 4K：0 ~ 255
 *
 * \retval AW_OK      : 验证成功
 * \retval -AW_EINVAL : 验证失败，参数错误
 * \retval -AW_EIO    : 验证失败，数据通信出错
 *
 * \note PLUS CPU系列的卡的卡号有4字节和7字节之分，对于7字节卡号的卡，
 *       只需要将卡号的高4字节（等级2防碰撞得到的卡号）作为验证的卡号即可。
 */
aw_err_t awbl_zlg600a_mifare_card_e2_authent (int      id,
                                              uint8_t  key_type,
                                              uint8_t *p_uid,
                                              uint8_t  key_sec,
                                              uint8_t  nblock);

/**
 * \brief Mifare卡直接密钥验证
 *
 *    该命令将密码作为参数传递，因此在此之前不需用“装载IC卡密钥”命令。若当前卡
 * 为PLUS CPU卡的等级2或等级3，且输入的密码只有6字节，则模块自动将输入的密码
 * 复制2次，取前16字节作为当前验证密钥。
 *
 * \param[in] id         : 设备ID号
 * \param[in] key_type   : 密钥类型，可以使用下列值：
 *                          - #AWBL_ZLG600A_IC_KEY_TYPE_A
 *                          - #AWBL_ZLG600A_IC_KEY_TYPE_B
 * \param[in] p_uid      : 卡序列号，4字节
 * \param[in] p_key      : 密钥缓冲区
 * \param[in] key_len    : 密钥的长度，只能为6（6字节密钥）或16（16字节密钥）
 * \param[in] nblock     : 需要验证的卡块号，取值范围与卡类型有关，
 *                         - S50：0 ~ 63
 *                         - S70：0 ~ 255
 *                         - PLUS CPU 2K：0 ~ 127
 *                         - PLUS CPU 4K：0 ~ 255
 *
 * \retval AW_OK      : 验证成功
 * \retval -AW_EINVAL : 验证失败，参数错误
 * \retval -AW_EIO    : 验证失败，数据通信出错
 *
 * \note PLUS CPU系列的卡的卡号有4字节和7字节之分，对于7字节卡号的卡，
 *       只需要将卡号的高4字节（等级2防碰撞得到的卡号）作为验证的卡号即可。
 */
aw_err_t awbl_zlg600a_mifare_card_direct_authent (int      id,
                                                  uint8_t  key_type,
                                                  uint8_t *p_uid,
                                                  uint8_t *p_key,
                                                  uint8_t  key_len,
                                                  uint8_t  nblock);

/**
 * \brief Mifare卡数据读取
 *
 *     在验证成功之后，才能读相应的块数据，所验证的块号与读块号必须在同一个扇区内，
 * Mifare1卡从块号0开始按顺序每4个块1个扇区，若要对一张卡中的多个扇区进行操作，在
 * 对某一扇区操作完毕后，必须进行一条读命令才能对另一个扇区直接进行验证命令，否则
 * 必须从请求开始操作。对于PLUS CPU卡，若下一个读扇区的密钥和当前扇区的密钥相同，
 * 则不需要再次验证密钥，直接读即可。
 *
 * \param[in] id      : 设备ID号
 * \param[in] nblock  : 读取数据的块号
 *                      - S50：0 ~ 63
 *                      - S70：0 ~ 255
 *                      - PLUS CPU 2K：0 ~ 127
 *                      - PLUS CPU 4K：0 ~ 255
 * \param[in] p_buf   : 存放读取的数据，大小为 16
 *
 * \retval AW_OK      : 读取成功
 * \retval -AW_EINVAL : 读取失败，参数错误
 * \retval -AW_EIO    : 读取失败，数据通信出错
 */
aw_err_t awbl_zlg600a_mifare_card_read (int      id,
                                        uint8_t  nblock,
                                        uint8_t *p_buf);

/**
 * \brief Mifare卡写数据,写之前必需成功进行密钥验证。
 *
 *  对卡内某一块进行验证成功后，即可对同一扇区的各个进行写操作（只要访问条件允
 *  许），其中包括位于扇区尾的密码块，这是更改密码的唯一方法。对于PLUS CPU卡等级
 *  2、3的AES密钥则是在其他位置修改密钥。
 *
 * \param[in] id         : 设备ID号
 * \param[in] nblock     : 读取数据的块号
 *                         - S50：0 ~ 63
 *                         - S70：0 ~ 255
 *                         - PLUS CPU 2K：0 ~ 127
 *                         - PLUS CPU 4K：0 ~ 255
 * \param[in] p_buf      : 写入数据缓冲区，大小必须为 16
 *
 * \retval AW_OK      : 写入成功
 * \retval -AW_EINVAL : 写入失败，参数错误
 * \retval -AW_EIO    : 写入失败，数据通信出错
 */
aw_err_t awbl_zlg600a_mifare_card_write (int      id,
                                         uint8_t  nblock,
                                         uint8_t *p_buf);

/**
 * \brief Mifare卡写数据
 *
 * \param[in] id         : 设备ID号
 * \param[in] nblock     : 读取数据的块号, 1 ~ 15
 * \param[in] p_buf      : 写入数据缓冲区，大小必须为 4
 *
 * \retval AW_OK      : 写入成功
 * \retval -AW_EINVAL : 写入失败，参数错误
 * \retval -AW_EIO    : 写入失败，数据通信出错
 *
 * 此命令只对UltraLight卡有效，对UltraLight卡进行读操作与Mifare1卡一样。
 */
aw_err_t awbl_zlg600a_ultralight_card_write (int      id,
                                             uint8_t  nblock,
                                             uint8_t *p_buf);


/**
 * \name Mifare卡值操作模式
 * @{
 */

#define AWBL_ZLG600A_MIFARE_CARD_VALUE_ADD  0xC1  /**< \brief 加  */
#define AWBL_ZLG600A_MIFARE_CARD_VALUE_SUB  0xC0  /**< \brief 减  */

/** @}*/

/**
 * \brief Mifare值操作，对Mifare卡的值块进行加减操作
 *
 * \param[in] id         : 设备ID号
 * \param[in] mode       : 值操作的模式，可以是加或减，使用下列宏：
 *                          - AWBL_ZLG600A_MIFARE_CARD_VALUE_ADD
 *                          - AWBL_ZLG600A_MIFARE_CARD_VALUE_SUB
 * \param[in] nblock     : 进行值操作的块号
 *                            - S50：0 ~ 63
 *                            - S70：0 ~ 255
 *                            - PLUS CPU 2K：0 ~ 127
 *                            - PLUS CPU 4K：0 ~ 255
 * \param[in] ntransblk  : 传输块号，计算结果值存放的块号
 * \param[in] value      : 4字节有符号数
 *
 * \retval AW_OK      : 写入成功
 * \retval -AW_EINVAL : 写入失败，参数错误
 * \retval -AW_EIO    : 写入失败，数据通信出错
 *
 * \note 要进行此类操作，块数据必须要有值块的格式，可参考NXP的相关文档。若卡块
 * 号与传输块号相同，则将操作后的结果写入原来的块内；若卡块号与传输块号不相同，
 * 则将操作后的结果写入传输块内，结果传输块内的数据被覆盖，原块内的值不变。
 * 处于等级2的PLUS CPU卡不支持值块操作，等级1、3支持。
 */
aw_err_t awbl_zlg600a_mifare_card_val_operate (int     id,
                                               uint8_t mode,
                                               uint8_t nblock,
                                               uint8_t ntransblk,
                                               int32_t value);

/**
 * \brief Mifare卡复位，通过将载波信号关闭指定的时间，再开启来实现卡片复位。
 *
 * \param[in] id         : 设备ID号
 * \param[in] time_ms    : 关闭的时间（单位:ms），0为一直关闭
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 *
 * \note 该函数将天线信号关闭数毫秒，若一直关闭，则等到执行一个请求命令时打开。
 */
aw_err_t awbl_zlg600a_mifare_card_reset (int     id,
                                         uint8_t time_ms);

/**
 * \brief Mifare卡激活，该函数用于激活卡片，是请求、防碰撞和选择三条命令的组合。
 *
 * \param[in] id         : 设备ID号
 * \param[in] req_mode   : 请求模式，可使用下列宏：
 *                           - #AWBL_ZLG600A_MIFARE_CARD_REQ_IDLE
 *                           - #AWBL_ZLG600A_MIFARE_CARD_REQ_ALL
 * \param[in] p_atq      : 请求应答
 * \param[in] p_saq      : 最后一级选择应答SAK
 * \param[in] p_len      : 序列号的实际长度，4字节、7字节或10字节
 * \raram[in] p_uid      : 序列号缓冲区，长度应该与序列号长度保持一致，
 *                         如不确定，建议先使用最大长度10。
 * \pram[in]  uid_len    : p_uid 所指缓冲区的长度
 *
 * \retval AW_OK      : 写入成功
 * \retval -AW_EINVAL : 写入失败，参数错误
 * \retval -AW_EIO    : 写入失败，数据通信出错
 */
aw_err_t awbl_zlg600a_mifare_card_active (int       id,
                                          uint8_t   req_mode,
                                          uint16_t *p_atq,
                                          uint8_t  *p_sak,
                                          uint8_t  *p_len,
                                          uint8_t  *p_uid,
                                          uint8_t   uid_len);

/**
 * \brief Mifare值操作，设置值块的值
 *
 * \param[in] id         : 设备ID号
 * \param[in] nblock     : 读取数据的值块地址
 * \param[in] value      : 设置的值
 *
 * \retval AW_OK      : 设置成功
 * \retval -AW_EINVAL : 设置失败，参数错误
 * \retval -AW_EIO    : 设置失败，数据通信出错
 */
aw_err_t awbl_zlg600a_mifare_card_val_set (int id, uint8_t nblock, int32_t value);

/**
 * \brief Mifare值操作，获取值块的值
 *
 * \param[in] id         : 设备ID号
 * \param[in] nblock     : 读取数据的值块地址
 * \param[in] p_value    : 获取值的指针
 *
 * \retval AW_OK      : 读取成功
 * \retval -AW_EINVAL : 读取失败，参数错误
 * \retval -AW_EIO    : 读取失败，数据通信出错
 */
aw_err_t awbl_zlg600a_mifare_card_val_get (int     id, 
                                           uint8_t nblock,
                                           int32_t *p_value);

/**
 * \brief 命令传输
 *
 *     该函数属于模块扩展功能，用于模块向卡片发送任意长度组合的数据串，例如针对
 * NXP新推出的NTAG213F是属于Ultralight C系列卡片，但是该卡片又新添加了扇区数据
 * 读写密钥保护功能。而这个密钥验证命令即可利用此命名传输函数来实现。
 *
 * \param[in] id         : 设备ID号
 * \param[in] p_tx_buf   : 发送数据的缓冲区
 * \param[in] tx_nbytes  : 发送字节数
 * \param[in] p_rx_buf   : 接收数据的缓冲区
 * \param[in] rx_nbytes  : 接收字节数
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_mifare_card_cmd_trans (int      id,
                                             uint8_t *p_tx_buf,
                                             uint8_t  tx_nbytes,
                                             uint8_t *p_rx_buf,
                                             uint8_t  rx_nbytes);


/**
 * \brief 数据交互
 *
 * 该命令用读写器与卡片的数据交互，通过该函数可以实现读写卡器的所有功能。
 *
 * \param[in] id         : 设备ID号
 * \param[in] p_data_buf : 需要交互发送的数据
 * \param[in] len        : 交互数据的长度
 * \param[in] wtxm_crc   : b7~b2,wtxm；b1,RFU(0); b0,0,CRC禁止能，1,CRC使能。
 * \param[in] fwi        : 超时等待时间编码, 超时时间=（（0x01 << fwi*302us）
 * \param[in] p_rx_buf   : 数据交互回应帧信息
 * \param[in] buf_size   : 接收回应信息的缓冲区大小
 * \param[in] p_real_len : 用于获取实际接收的信息长度，如果缓冲区大小小于实际信
 *                         息长度，则多余部分将丢弃
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_mifare_card_data_exchange (int       id,
                                                 uint8_t  *p_data_buf,
                                                 uint8_t   len,
                                                 uint8_t   wtxm_crc,
                                                 uint8_t   fwi,
                                                 uint8_t  *p_rx_buf,
                                                 uint8_t   buf_size,
                                                 uint16_t *p_real_len);

/**
 * \name IC卡复位后的波特率
 *
 *      注意，该波特率并非主机与ZLG600A的通信波特率，而是指ZLG600模块与接触式IC
 *  卡的通信波特率。
 *
 * @{
 */
#define  AWBL_ZLG600A_CICC_RST_BAUDRATE_9600   0x11  /**< \brief 波特率 9600  */
#define  AWBL_ZLG600A_CICC_RST_BAUDRATE_38400  0x13  /**< \brief 波特率 38400 */

/** @}*/

/**
 * \brief 接触式IC卡复位（自动处理PPS）
 *
 *     该函数是冷复位，执行成功后会自动根据IC卡的复位信息来执行PPS命令，然后再
 * 选择awbl_zlg600a_cicc_tpdu()函数使用的传输协议（T = 0 或T = 1）。
 *
 * \param[in]  p_dev         : 指向ZLG600A设备的指针
 * \param[in]  baudrate_flag : 读卡器与IC卡通信使用的波特率，使用下列宏：
 *                             - AWBL_ZLG600A_CICC_RST_BAUDRATE_9600
 *                             - AWBL_ZLG600A_CICC_RST_BAUDRATE_38400
 * \param[out] p_rx_buf      : 存放复位信息的缓冲区
 * \param[in]  rx_bufsize    : 缓冲区的长度
 * \param[out] p_real_len    : 获取复位信息长度的指针，如果接收缓冲区的长度小于
 *                             实际长度，则多余部分丢弃。
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_cicc_reset (int       id,
                                  uint8_t   baudrate_flag,
                                  uint8_t  *p_rx_buf,
                                  uint8_t   rx_bufsize,
                                  uint16_t *p_real_len);

/**
 * \brief 接触式IC卡传输协议
 *
 *     该命令根据接触式IC卡的复位信息，自动选择T = 0或T = 1传输协议，整个过程
 * 不需要使用者干预。该命令用于传输APDU数据流。
 *
 * \param[in] id            : 设备ID号
 * \param[in] p_tx_buf      : 发送缓冲区
 * \param[in] tx_bufsize    : 发送缓冲区的长度， 1 ~ 272
 * \param[in] p_rx_buf      : 接收缓冲区
 * \param[in] rx_bufsize    : 接收缓冲区的长度
 * \param[in] p_real_len    : 用于获取实际接收信息长度的指针，如果接收缓冲区
 *                            的长度小于实际长度，则多余部分丢弃。
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_cicc_tpdu (int       id,
                                 uint8_t  *p_tx_buf,
                                 uint8_t   tx_bufsize,
                                 uint8_t  *p_rx_buf,
                                 uint8_t   rx_bufsize,
                                 uint16_t *p_real_len);

/**
 * \brief 接触式IC卡冷复位
 *
 *     该函数是冷复位，执行了接触式IC卡上电时序，执行成功后会自动根据IC卡的复位
 * 信息来选择awbl_zlg600a_cicc_tpdu()函数使用的传输协议（T = 0 或T = 1）。该函数
 * 没有自动执行PPS命令，需要用户根据复位信息来判断是否使用awbl_zlg600a_cicc_pps()
 * 来修改协议和参数。
 *
 * \param[in] id            : 设备ID号
 * \param[in] baudrate_flag : IC卡复位后的波特率，使用下列宏：
 *                             - AWBL_ZLG600A_CICC_RST_BAUDRATE_9600
 *                             - AWBL_ZLG600A_CICC_RST_BAUDRATE_38400
 * \param[in] p_rx_buf      : 存放复位信息的缓冲区
 * \param[in] rx_bufsize    : 缓冲区的长度
 * \param[in] p_real_len    : 获取复位信息长度的指针，如果接收缓冲区的长度小于
 *                            实际长度，则多余部分丢弃。
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_cicc_coldreset (int       id,
                                      uint8_t   baudrate_flag,
                                      uint8_t  *p_rx_buf,
                                      uint8_t   rx_bufsize,
                                      uint16_t *p_real_len);

/**
 * \brief 接触式IC卡热复位
 *
 *    该函数是热复位，没有执行了接触式IC卡上电时序，执行成功后会自动根据IC卡的
 * 复位信息来选择awbl_zlg600a_cicc_tpdu()函数使用的传输协议（T = 0 或T = 1）。
 * 热复位与冷复位唯一的区别是没有执行IC卡上电操作。需要用户根据复位信息来判断
 * 是否使用awbl_zlg600a_cicc_pps()来修改协议和参数。该函数必须在IC卡时钟和电源均
 * 有效的情况下才能执行。
 *
 * \param[in] id            : 设备ID号
 * \param[in] baudrate_flag : IC卡复位后的波特率，使用下列宏：
 *                             - AWBL_ZLG600A_CICC_RST_BAUDRATE_9600
 *                             - AWBL_ZLG600A_CICC_RST_BAUDRATE_38400
 * \param[in] p_rx_buf      : 存放复位信息的缓冲区
 * \param[in] rx_bufsize    : 缓冲区的长度
 * \param[in] p_real_len    : 获取复位信息长度的指针，如果接收缓冲区的长度小于
 *                            实际长度，则多余部分丢弃。
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_cicc_warmreset (int       id,
                                      uint8_t   baudrate_flag,
                                      uint8_t  *p_rx_buf,
                                      uint8_t   rx_bufsize,
                                      uint16_t *p_real_len);

/**
 * \brief 接触式IC卡停活，关闭接触式IC卡的电源和时钟
 *
 * \param[in] id      : 设备ID号
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_cicc_deactive (int id);

/**
 * \brief 接触式IC卡协议和参数选择
 *
 *     该函数是冷复位或热复位之后且必须首先执行（协商模式下需要执行，专用模式
 * 不需要执行）。若对接触式IC卡不了解，建议使用awbl_zlg600a_cicc_reset()进行复位。
 *
 * \param[in] id         : 设备ID号
 * \param[in] p_pps      : PPS参数缓冲区，大小一定为4字节
 *                         - PPS[0] bit 3 ~ bit0 和 bit7保留
 *                         - PPS[0] bit 4：1 — PPS1存在；0 — PPS1不存在
 *                         - PPS[0] bit 5：1 — PPS2存在；0 — PPS2不存在
 *                         - PPS[0] bit 6：1 — PPS3存在；0 — PPS3不存在
                           - PPS[1] ：F/D
                           - PPS[2] ：N
                           - PPS[3] ：待定
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_cicc_pps (int      id,
                                uint8_t *p_pps);

/**
 * \brief 接触式IC卡传输协议（T = 0）
 *
 *     该命令用于T = 0传输协议。若接触式IC卡的传输协议为T = 0，该函数等同于
 * awbl_zlg600a_cicc_tpdu()。
 *
 * \param[in] id            : 设备ID号
 * \param[in] p_tx_buf      : 发送缓冲区
 * \param[in] tx_bufsize    : 发送缓冲区的长度， 1 ~ 272
 * \param[in] p_rx_buf      : 接收缓冲区
 * \param[in] rx_bufsize    : 接收缓冲区的长度
 * \param[in] p_real_len    : 用于获取实际接收信息长度的指针，如果接收缓冲区
 *                            的长度小于实际长度，则多余部分丢弃。
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_cicc_tpdu_tp0 (int       id,
                                     uint8_t  *p_tx_buf,
                                     uint8_t   tx_bufsize,
                                     uint8_t  *p_rx_buf,
                                     uint8_t   rx_bufsize,
                                     uint16_t *p_real_len);

/**
 * \brief 接触式IC卡传输协议（T = 1）
 *
 *     该命令用于T = 1传输协议。若接触式IC卡的传输协议为T = 1，该函数等同于
 * awbl_zlg600a_cicc_tpdu()。
 *
 * \param[in] id            : 设备ID号
 * \param[in] p_tx_buf      : 发送缓冲区
 * \param[in] tx_bufsize    : 发送缓冲区的长度， 1 ~ 272
 * \param[in] p_rx_buf      : 接收缓冲区
 * \param[in] rx_bufsize    : 接收缓冲区的长度
 * \param[in] p_real_len    : 用于获取实际接收信息长度的指针，如果接收缓冲区
 *                            的长度小于实际长度，则多余部分丢弃。
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_cicc_tpdu_tp1 (int       id,
                                     uint8_t  *p_tx_buf,
                                     uint8_t   tx_bufsize,
                                     uint8_t  *p_rx_buf,
                                     uint8_t   rx_bufsize,
                                     uint16_t  *p_real_len);

/**
 * \name PICCA卡请求模式
 * @{
 */
#define AWBL_ZLG600A_PICCA_REQ_IDLE 0x26 /**< \brief IDLE模式,请求空闲的卡 */
#define AWBL_ZLG600A_PICCA_REQ_ALL  0x52 /**< \brief ALL模式,请求所有的卡  */
/** @}*/

/**
 * \brief PICCA卡请求操作
 *
 *  该函数作为卡的请求操作，只要符合ISO14443A标准的卡都应能发出响应。
 *
 * \param[in] id          : 设备ID号
 * \param[in] req_mode    : 请求模式，可使用下列宏：
 *                           - #AWBL_ZLG600A_PICCA_REQ_IDLE
 *                           - #AWBL_ZLG600A_PICCA_REQ_ALL
 * \param[in] p_atq       : 获取请求应答信息（ATQ）的指针，其值为16位，不同卡
 *                          类型返回的信息不同,常见类型返回的ATQ如下：
 *
 *            Mifare1 S50    | Mifare1 S70 | Mifare1 Light | Mifare0 UltraLight
 *       --------------------|-------------|---------------|-------------------
 *              0x0004       |    0x0002   |    0x0010     |      0x0044
 *       ----------------------------------------------------------------------
 *          Mifare3 DESFire  |   SHC1101   |    SHC1102    |      11RF32
 *       --------------------|-------------|---------------|-------------------
 *               0x0344      |    0x0004   |    0x3300     |      0x0004
 *
 * \retval AW_OK      : 请求成功
 * \retval -AW_EINVAL : 请求失败，参数错误
 * \retval -AW_EIO    : 请求失败，数据通信出错
 *
 * \note 卡进入天线后，从射频场中获取能量，从而得电复位，复位后卡处于IDLE模式，
 * 用两种请求模式的任一种请求时，此时的卡均能响应；若对某一张卡成功进行了挂起
 * 操作（Halt命令或DeSelect命令），则进入了Halt模式，此时的卡只响应ALL（0x52）
 * 模式的请求，除非将卡离开天线感应区后再进入。
 */
aw_err_t awbl_zlg600a_picca_request (int       id,
                                     uint8_t   req_mode,
                                     uint16_t *p_atq);

/**
 * \name PICCA卡防碰撞等级
 * @{
 */
#define AWBL_ZLG600A_PICCA_ANTICOLL_1  0x93  /**< \brief 第一级防碰撞  */
#define AWBL_ZLG600A_PICCA_ANTICOLL_2  0x95  /**< \brief 第二级防碰撞  */
#define AWBL_ZLG600A_PICCA_ANTICOLL_3  0x97  /**< \brief 第三级防碰撞  */
/** @}*/

/**
 * \brief PICCA卡的防碰撞操作
 *
 *  需要成功执行一次请求命令，并返回请求成功，才能进行防碰撞操作，否则返回错误。
 *
 * \param[in] id             : 设备ID号
 * \param[in] anticoll_level : 防碰撞等级，可使用下列宏：
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_1
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_2
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_3
 *
 * \param[in] know_uid : 已知的卡序列号
 * \param[in] nbit_cnt : 位计数，表明已知的序列号uid的位数。其值小于32。
 *                       - 若nbit_cnt值为0,则表明没有已知序列号，序列号的所有位
 *                         都要从本函数获得。
 *                       - 若nbit_cnt不为0，则序列号中有已知的序列号的值，表示
 *                         序列号的前nbit_cnt位为已知序列号中所示前nbit_cnt位的
 *                         值，其余位需要本函数获得。
 * \param[in] *p_uid   : 防碰撞后获取到的卡序列号
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 *
 * \note  符合ISO14443A标准卡的序列号都是全球唯一的，正是这种唯一性，才能实现防
 * 碰撞的算法逻辑，若有若干张卡同时在天线感应区内则这个函数能够找到一张序列号较
 * 大的卡来操作。
 */
aw_err_t awbl_zlg600a_picca_anticoll (int       id,
                                      uint8_t   anticoll_level,
                                      uint32_t  know_uid,
                                      uint8_t   nbit_cnt,
                                      uint32_t *p_uid);

/**
 * \brief PICC A卡的选择操作
 *
 *  需要成功执行一次防碰撞命令，并返回成功，才能进行卡选择操作，否则返回错误。
 *
 * \param[in] id             : 设备ID号
 * \param[in] anticoll_level : 防碰撞等级，可使用下列宏：
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_1
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_2
 *                             - #AWBL_ZLG600A_MIFARE_CARD_ANTICOLL_3
 *
 * \param[in] know_uid : 已知的卡序列号
 * \param[in] nbit_cnt : 位计数，表明已知的序列号uid的位数。其值小于32。
 *                       - 若nbit_cnt值为0,则表明没有已知序列号，序列号的所有位
 *                         都要从本函数获得。
 *                       - 若nbit_cnt不为0，则序列号中有已知的序列号的值，表示
 *                         序列号的前nbit_cnt位为已知序列号中所示前nbit_cnt位的
 *                         值，其余位需要本函数获得。
 * \param[in] uid   : 前一个防碰撞函数获取的UID
 * \param[in] p_sak : 返回的信息，若bit2为1，则表明UID不完整
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 *
 * \note  卡的序列号长度有三种：4字节、7字节和10字节。 4字节的只要用一级选择即可
 * 得到完整的序列号，如Mifare1 S50/S70等；7字节的要用二级选择才能得到完整的序列
 * 号，前一级所得到的序列号的最低字节为级联标志0x88，在序列号内只有后3字节可用，
 * 后一级选择能得到4字节序列号，两者按顺序连接即为7字节序列号，如UltraLight
 * 和DesFire等；10字节的以此类推，但至今还未发现此类卡。
 */
aw_err_t awbl_zlg600a_picca_select (int       id,
                                    uint8_t   anticoll_level,
                                    uint32_t  uid,
                                    uint8_t  *p_sak);

/**
 * \brief PICCA卡的挂起操作，使所选择的卡进入HALT状态
 *
 *  在HALT状态下，卡将不响应读卡器发出的IDLE模式的请求，除非将卡复位或离开天线感
 *  应区后再进入。但它会响应读卡器发出的ALL请求。
 *
 * \param[in] id      : 设备ID号
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_picca_halt (int id);

/**
 * \brief PICC TypeA卡RATS（request for answer to select）
 *
 *     RATS（request for answer to select）是ISO14443-4协议的命令，模块发送RATS，
 * 卡片发出ATS（answer to select）作为RATS的应答，在执行该命令前，必需先进行一次
 * 卡选择操作，且执行过一次RATS命令后，想再次执行RATS命令，必需先解除激活。
 *
 * \param[in] id         : 设备ID号
 * \param[in] cid        : 卡标识符（card IDentifier，取值范围0x00～0x0E）
 * \param[in] p_ats_buf  : 用于接收回应的信息，不同卡，字节数会有不同
 * \param[in] bufsize    : 缓冲区大小
 * \param[in] p_real_len : 用于获取实际接收的信息长度，如果缓冲区大小小于实际信
 *                         息长度，则多余部分将丢弃
 *
 * \retval AW_OK      : 写入成功
 * \retval -AW_EINVAL : 写入失败，参数错误
 * \retval -AW_EIO    : 写入失败，数据通信出错
 */
aw_err_t awbl_zlg600a_picca_rats (int       id,
                                  uint8_t   cid,
                                  uint8_t  *p_ats_buf,
                                  uint8_t   bufsize,
                                  uint16_t *p_real_len);

/**
 * \brief PICC TypeA卡PPS（request for answer to select）
 *
 *     PPS（protocal and parameter selection）是ISO14443-4协议的命令，用于改变有
 * 关的专用协议参数，该命令不是必需的，命令只支持默认参数，即该命令的参数设置为
 * 0即可。在执行该命令前，必需先成功执行一次RATS命令。
 *
 * \param[in] id         : 设备ID号
 * \param[in] dsi_dri    : 模块与卡通信波特率,设置为0（默认）即可
 *
 * \retval AW_OK      : 写入成功
 * \retval -AW_EINVAL : 写入失败，参数错误
 * \retval -AW_EIO    : 写入失败，数据通信出错
 */
aw_err_t awbl_zlg600a_picca_pps (int id, uint8_t dsi_dri);

/**
 * \brief PICC TypeA卡解除激活
 *
 *   该命令是ISO14443-4协议的命令，用于将卡片置为挂起（HALT）状态，处于挂起
 *（HALT）状态的卡可以用“请求”命令（请求代码为ALL）来重新激活卡，只有执行“RATS”
 * 命令的卡才用该命令。
 *
 * \param[in] id      : 设备ID号
 *
 * \retval AW_OK      : 写入成功
 * \retval -AW_EINVAL : 写入失败，参数错误
 * \retval -AW_EIO    : 写入失败，数据通信出错
 */
aw_err_t awbl_zlg600a_picca_deselect (int id);

/**
 * \brief PICC卡T=CL
 *
 *     T=CL是半双工分组传输协议，ISO14443-4协议命令，用于读写器与卡片之间的数据
 * 交互，一般符合ISO14443协议的CPU卡均用该协议与读写器通信。调用该命令时只需要
 * 将CPU卡COS命令的数据作为输入即可，其他的如分组类型、卡标识符CID、帧等待时间
 * FWT、等待时间扩展倍增因子WTXM（waiting time extensionmultiplier），等等由该
 * 命令自动完成。
 *
 * \param[in] id          : 设备ID号
 * \param[in] p_cos_buf   : COS命令
 * \param[in] cos_bufsize ：COS命令的长度
 * \param[in] p_res_buf   : 用于接收回应的信息，不同COS命令，字节数会有不同
 * \param[in] res_bufsize : 接收回应信息的缓冲区大小
 * \param[in] p_real_len  : 用于获取实际接收的信息长度，如果缓冲区大小小于实际信
 *                          息长度，则多余部分将丢弃
 *
 * \retval AW_OK      : 写入成功
 * \retval -AW_EINVAL : 写入失败，参数错误
 * \retval -AW_EIO    : 写入失败，数据通信出错
 */
aw_err_t awbl_zlg600a_picc_tpcl (int       id,
                                 uint8_t  *p_cos_buf,
                                 uint8_t   cos_bufsize,
                                 uint8_t  *p_res_buf,
                                 uint8_t   res_bufsize,
                                 uint16_t *p_real_len);

/**
 * \brief 数据交换
 *
 * 该命令用读写器与卡片的数据交互，通过该函数可以实现读写卡器的所有功能。
 *
 * \param[in] id         : 设备ID号
 * \param[in] p_data_buf : 需要交互发送的数据（其内容与实际使用的卡有关）
 * \param[in] len        : 交互数据的长度
 * \param[in] wtxm_crc   : b7~b2,wtxm；b1,RFU(0); b0,0-CRC禁止能，1-CRC使能。
 * \param[in] fwi        : 超时等待时间编码, 超时时间=（（0x01 << fwi*302us）
 * \param[in] p_rx_buf   : 数据交互回应帧信息
 * \param[in] bufsize    : 接收回应信息的缓冲区大小
 * \param[in] p_real_len : 用于获取实际接收的信息长度，如果缓冲区大小小于实际信
 *                         息长度，则多余部分将丢弃
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_picc_data_exchange (int       id,
                                          uint8_t  *p_data_buf,
                                          uint8_t   len,
                                          uint8_t   wtxm_crc,
                                          uint8_t   fwi,
                                          uint8_t  *p_rx_buf,
                                          uint8_t   bufsize,
                                          uint16_t *p_real_len);

/**
 * \brief PICC A卡复位，通过将载波信号关闭指定的时间，再开启来实现卡片复位。
 *
 * \param[in] id      : 设备ID号
 * \param[in] time_ms : 关闭的时间（单位:ms），0为一直关闭
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 *
 * \note 该函数将天线信号关闭数毫秒，若一直关闭，则等到执行一个请求命令时打开。
 */
aw_err_t awbl_zlg600a_picca_reset (int id, uint8_t time_ms);

/**
 * \brief PICC A卡激活，该函数用于激活卡片，是请求、防碰撞和选择三条命令的组合。
 *
 * \param[in] id         : 设备ID号
 * \param[in] req_mode   : 请求模式，可使用下列宏：
 *                           - #AWBL_ZLG600A_PICCA_REQ_IDLE
 *                           - #AWBL_ZLG600A_PICCA_REQ_ALL
 * \param[in] p_atq      : 请求应答
 * \param[in] p_saq      : 最后一级选择应答SAK
 * \param[in] p_len      : 序列号长度，4字节或7字节
 * \raram[in] p_uid      : 序列号缓冲区，长度应该与序列号长度保持一致
 *
 * \retval AW_OK      : 写入成功
 * \retval -AW_EINVAL : 写入失败，参数错误
 * \retval -AW_EIO    : 写入失败，数据通信出错
 */
aw_err_t awbl_zlg600a_picca_active (int       id,
                                    uint8_t   req_mode,
                                    uint16_t *p_atq,
                                    uint8_t  *p_saq,
                                    uint8_t  *p_len,
                                    uint8_t  *p_uid,
                                    uint8_t   uid_len);

/**
 * \name PICCB卡请求模式
 * @{
 */
#define AWBL_ZLG600A_PICCB_REQ_IDLE 0x00 /**< \brief IDLE模式,请求空闲的卡 */
#define AWBL_ZLG600A_PICCB_REQ_ALL  0x08 /**< \brief ALL模式,请求所有的卡  */
/** @}*/

/**
 * \brief PICC B卡激活
 *
 *    在调用该函数前，需要确保IC工作模式已经配置为TypeB模式。
 *
 * \param[in] id         : 设备ID号
 * \param[in] req_mode   : 请求模式，可使用下列宏：
 *                           - #AWBL_ZLG600A_PICCB_REQ_IDLE
 *                           - #AWBL_ZLG600A_PICCB_REQ_ALL
 * \raram[in] p_uid      : UID相关信息，长度为12
 *
 * \retval AW_OK      : 写入成功
 * \retval -AW_EINVAL : 写入失败，参数错误
 * \retval -AW_EIO    : 写入失败，数据通信出错
 */
aw_err_t awbl_zlg600a_piccb_active (int id, uint8_t req_mode, uint8_t *p_uid);

/**
 * \brief PICC B卡复位，通过将载波信号关闭指定的时间，再开启来实现卡片复位。
 *
 * \param[in] id       : 设备ID号
 * \param[in] time_ms  : 关闭的时间（单位:ms），0为一直关闭
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 *
 * \note 该函数将天线信号关闭数毫秒，若一直关闭，则等到执行一个请求命令时打开。
 */
aw_err_t awbl_zlg600a_piccb_reset (int id, uint8_t time_ms);

/**
 * \brief PICCA卡请求操作
 *
 *  该函数作为卡的请求操作，只要符合ISO14443A标准的卡都应能发出响应。
 *
 * \param[in] id          : 设备ID号
 * \param[in] req_mode    : 请求模式，可使用下列宏：
 *                           - #AWBL_ZLG600A_PICCB_REQ_IDLE
 *                           - #AWBL_ZLG600A_PICCB_REQ_ALL
 * \param[in] slot_time   : 时隙总数，0 ~ 4
 * \param[in] p_uid       : 返回的UID相关信息，长度为12
 *
 * \retval AW_OK      : 请求成功
 * \retval -AW_EINVAL : 请求失败，参数错误
 * \retval -AW_EIO    : 请求失败，数据通信出错
 */
aw_err_t awbl_zlg600a_piccb_request (int      id,
                                     uint8_t  req_mode,
                                     uint8_t  slot_time,
                                     uint8_t *p_uid);

/**
 * \brief PICC B 卡的防碰撞操作
 *
 * \param[in] id          : 设备ID号
 * \param[in] slot_flag   : 时隙标记，2 ~ 16，该参数值与请求命令的时隙总数有关
 *                          系，假如请求命令的时隙总数为n，侧该时隙标记N < 2^n
 * \param[in] p_uid       : 返回的UID相关信息，长度为12
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_piccb_anticoll (int      id,
                                      uint8_t  slot_flag,
                                      uint8_t *p_uid);


/**
 * \brief PICC B卡的挂起操作，使所选择的卡进入HALT状态
 *
 *     该函数用于B型卡挂起，在执行挂起命令前，必需先执行成功过一次请求命令。执行
 * 挂起命令成功后，卡片处于挂起状态，模块必需通过ALL方式请求卡片，而不能用IDLE方
 * 式请求。
 *
 * \param[in] id      : 设备ID号
 * \param[in] p_pupi  : PUPI,大小为4字节的标识符
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_piccb_halt (int id, uint8_t *p_pupi);


/**
 * \brief 修改B卡的传输属性
 *
 *     该函数用于B型卡挂起，在执行挂起命令前，必需先执行成功过一次请求命令。执行
 * 挂起命令成功后，卡片处于挂起状态，模块必需通过ALL方式请求卡片，而不能用IDLE方
 * 式请求。
 *
 * \param[in] id         : 设备ID号
 * \param[in] p_pupi     : PUPI,大小为4字节的标识符
 * \param[in] cid        : 取值范围为 0 - 14, 若不支持CID，则设置为0
 * \param[in] protype    : 支持的协议
 *                          - bit3 : 1-PCD中止与PICC继续通信,0-PCD与PICC继续通信
 *                          - bit2 ~ bit1 :
 *                              - 11 : 10 etu + 512 / fs
 *                              - 10 : 10 etu + 256 / fs
 *                              - 01 : 10 etu + 128 / fs
 *                              - 00 : 10 etu + 32 / fs
 *                          - bit0 : 1-遵循ISO14443-4,0-不遵循ISO14443-4
 *                                   (二代身份证必须该位必须为1)
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_piccb_attrib_set (int      id,
                                        uint8_t *p_pupi,
                                        uint8_t  cid,
                                        uint8_t  protype);

/**
 * \brief 获取8字节身份证ID
 *
 *     该函数用于获取二代身份证ID，在执行命令前，必需先调用awbl_zlg600a_ic_isotype_set()
 * 配置协议为ISO14443B。
 *
 * \param[in]  id      : 设备ID号
 * \param[in] req_mode : 请求模式，可使用下列宏：
 *                       - #AWBL_ZLG600A_PICCB_REQ_IDLE
 *                       - #AWBL_ZLG600A_PICCB_REQ_ALL
 * \param[out] p_sid   : 8字节ID
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_sid_get (int id, uint8_t req_mode, uint8_t *p_sid);

/**
 * \brief PLUS CPU卡个人化更新数据
 *
 *     该命令用于SL0（Security Level 0，安全等级0）的PLUS CPU卡个人化，PLUS CPU
 * 卡出厂时的安全等级为SL0，该等级下，不需要任何验证就可以向卡里写数据，写入的数
 * 据是作为其它安全等级的初始值，例如：向SL0的0x0003块写入：0xA0 0xA1 0xA2 0xA3
 * 0xA4 0xA5 0xFF 0x07 0x80 0x69 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF，当卡片升级到SL1
 * 后，扇区0的A密钥为0xA0 0xA1 0xA2 0xA3 0xA4 0xA5，而不是默认的0xFF 0xFF 0xFF
 * 0xFF 0xFF 0xFF，即可以在SL0修改卡片的默认数据和密钥。
 *
 * \param[in] id            : 设备ID号
 * \param[in] sector_addr   : PLUS CPU卡存储器地址
 * \param[in] p_buf         : 发送缓冲区,数据个数为确定16个
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_pluscpu_persotcl_write (int       id,
                                              uint16_t  sector_addr,
                                              uint8_t  *p_buf);

/**
 * \brief PLUS CPU卡提交个人化
 *
 *  awbl_zlg600a_pluscpu_persotcl_write()函数只是更新卡中的数据，但数据还未生效。
 * 提交后数据才生效。执行该命令后，PLUS CPU卡的安全等级提高到SL1或者SL3
 * （若是支持SL1的卡，则执行该命令后卡片安全等级提高到SL1；若是只支持SL0和SL3的
 * 卡，则执行该命令后卡片安全等级提高到SL3）。
 * 注意：在SL0中，只有修改了以下地址才能执行提交操作：
 *   - 0x9000（主控密钥）
 *   - 0x9001（配置块密钥）
 *   - 0x9002（SL2提升密钥，只有支持SL2的卡才有该密钥）
 *   - 0x9003（SL3主控密钥，只有支持SL3的卡才有该密钥）
 *
 * \param[in] id      : 设备ID号
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_pluscpu_persotcl_commit (int id);


/**
 * \brief PLUS CPU卡首次验证（直接密钥验证）
 *
 * \param[in] id           : 设备ID号
 * \param[in] authent_addr : 待验证的16位密钥地址
 * \param[in] p_key        : 密钥缓冲区，大小为16字节
 *
 * \retval AW_OK      : 验证成功
 * \retval -AW_EINVAL : 验证失败，参数错误
 * \retval -AW_EIO    : 验证失败，数据通信出错
 *
 * \note PLUS CPU卡的密钥A/B是通过地址的奇偶数来区分，AES的密钥地址与数据块的
 * 扇区关系对应如下:
 * 密钥A地址=0x4000 + 扇区 × 2
 * 密钥B地址=0x4000 + 扇区 × 2 + 1
 * 因此，如验证1扇区的密钥A，则密钥地址为：0x4002
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_first_direct_authent (int       id,
                                                        uint16_t  authent_addr,
                                                        uint8_t  *p_key);

/**
 * \brief PLUS CPU卡首次验证（E2密钥验证），验证的密钥来自模块内部，掉电不丢失
 *
 * \param[in] id            : 设备ID号
 * \param[in] authent_addr  : 待验证的16位密钥地址
 * \param[in] key_sector    : 密钥所在扇区
 *
 * \retval AW_OK      : 验证成功
 * \retval -AW_EINVAL : 验证失败，参数错误
 * \retval -AW_EIO    : 验证失败，数据通信出错
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_first_e2_authent (int      id,
                                                    uint16_t authent_addr,
                                                    uint8_t  key_sector);

/**
 * \brief PLUS CPU卡跟随验证（直接密钥验证）
 *
 *     该函数用于SL3 PLUS CPU卡的跟随密钥验证，验证的密钥来自命令参数，只有执行
 * 过“首次验证”命令成功后才能使用该命令。两种验证的区别在于使用的时间不同，
 * “首次验证”所需要的时间比“跟随验证”的时间要长些。
 *
 * \param[in] id           : 设备ID号
 * \param[in] authent_addr : 待验证的16位密钥地址
 * \param[in] p_key        : 密钥缓冲区，大小为16字节
 *
 * \retval AW_OK      : 验证成功
 * \retval -AW_EINVAL : 验证失败，参数错误
 * \retval -AW_EIO    : 验证失败，数据通信出错
 *
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_follow_direct_authent (int       id,
                                                         uint16_t  authent_addr,
                                                         uint8_t  *p_key);

/**
 * \brief PLUS CPU卡跟随验证（E2密钥验证）,验证的密钥来自模块内部，掉电不丢失
 *
 * \param[in] id            : 设备ID号
 * \param[in] authent_addr  : 待验证的16位密钥地址
 * \param[in] key_sector    : 密钥所在扇区
 *
 * \retval AW_OK      : 验证成功
 * \retval -AW_EINVAL : 验证失败，参数错误
 * \retval -AW_EIO    : 验证失败，数据通信出错
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_follow_e2_authent (int      id,
                                                     uint16_t authent_addr,
                                                     uint8_t  key_sector);

/**
 * \brief PLUS CPU卡SL3复位验证（E2密钥验证）,验证的密钥来自模块内部，掉电不丢失
 *
 *  该命令用于PLUS CPU卡通过首次验证后的使用过程中，复位读写计数器和验证等信息。
 *
 * \param[in] id      : 设备ID号
 *
 * \retval AW_OK      : 验证成功
 * \retval -AW_EINVAL : 验证失败，参数错误
 * \retval -AW_EIO    : 验证失败，数据通信出错
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_reset_authent (int id);

/**
 * \name PLUS CPU卡读数据的模式
 * @{
 */

/** \brief 命令有MAC；数据密文(CIPHER)；回应无MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_M_C_NM    0x30

/** \brief 命令有MAC；数据密文(CIPHER)；回应有MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_M_C_M     0x31

/** \brief 命令有MAC；数据明文；回应无MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_M_NC_NM   0x32

/** \brief 命令有MAC；数据明文；回应有MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_M_NC_M    0x33

/** \brief 命令无MAC；数据密文；回应无MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_NM_C_NM   0x34

/** \brief 命令无MAC；数据密文；回应有MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_NM_C_M    0x35

/** \brief 命令无MAC；数据明文；回应无MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_NM_NC_NM  0x36

/** \brief 命令无MAC；数据明文；回应有MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_READMODE_NM_NC_M   0x37

/** @}*/

/**
 * \brief PLUS CPU卡SL3读数据块
 *
 *  该函数用于读取SL3的数据块，在读数据块之前必需成功执行一次密钥验证。
 *
 * \param[in] id            : 设备ID号
 * \param[in] read_mode     : 读数据模式，使用宏AWBL_ZLG600A_PLUSCPU_SL3_READMODE_*
 *                            (#AWBL_ZLG600A_PLUSCPU_SL3_READMODE_M_C_M)
 * \param[in] start_block   : 读数据的起始块号
 * \param[in] block_num     : 读的块数，1 ~ 3
 * \param[in] p_buf         : 读取的数据存放缓冲区，大小应该为：block_num * 16
 *
 * \retval AW_OK      : 验证成功
 * \retval -AW_EINVAL : 验证失败，参数错误
 * \retval -AW_EIO    : 验证失败，数据通信出错
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_read (int       id,
                                        uint8_t   read_mode,
                                        uint16_t  start_block,
                                        uint8_t   block_num,
                                        uint8_t  *p_buf);

/**
 * \name PLUS CPU卡写数据的模式
 * @{
 */

/** \brief 命令有MAC；数据密文(CIPHER)；回应无MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_WRITEMODE_M_C_NM     0xA0

/** \brief 命令有MAC；数据密文(CIPHER)；回应有MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_WRITEMODE_M_C_M      0xA1

/** \brief 命令有MAC；数据明文；回应无MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_WRITEMODE_M_NC_NM    0xA2

/** \brief 命令有MAC；数据明文；回应有MAC */
#define  AWBL_ZLG600A_PLUSCPU_SL3_WRITEMODE_M_NC_M     0xA3

/** @}*/

/**
 * \brief PLUS CPU卡SL3写数据块
 *
 *  该函数用于写SL3的数据块，在写数据块之前必需成功执行一次密钥验证。
 *
 * \param[in] id            : 设备ID号
 * \param[in] write_mode    : 写数据模式，使用宏aAWBL_ZLG600A_PLUSCPU_SL3_WRITEMODE_*
 *                            (#AWBL_ZLG600A_PLUSCPU_SL3_WRITEMODE_M_C_NM)
 * \param[in] start_block   : 写数据的起始块号
 * \param[in] block_num     : 写的块数，1 ~ 3
 * \param[in] p_buf         : 写入数据存放缓冲区，大小应该为：block_num * 16
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_write (int       id,
                                         uint8_t   write_mode,
                                         uint16_t  start_block,
                                         uint8_t   block_num,
                                         uint8_t  *p_buf);

/**
 * \name PLUS CPU 卡值操作模式
 * @{
 */
#define AWBL_ZLG600A_PLUSCPU_VALUE_ADD  0xB7  /**< \brief 加  */
#define AWBL_ZLG600A_PLUSCPU_VALUE_SUB  0xB9  /**< \brief 减  */
/** @}*/

/**
 * \brief PLUS CPU卡值块操作
 *
 * \param[in] id         : 设备ID号
 * \param[in] mode       : 值操作的模式，可以是加或减，使用下列宏：
 *                          - AWBL_ZLG600A_PLUSCPU_VALUE_ADD
 *                          - AWBL_ZLG600A_PLUSCPU_VALUE_SUB
 * \param[in] src_block  : 进行值块操作的数据源块号
 * \param[in] dst_block  : 值块操作结束后数据存放的目的块号
 * \param[in] value      : 4字节有符号数
 *
 * \retval AW_OK      : 写入成功
 * \retval -AW_EINVAL : 写入失败，参数错误
 * \retval -AW_EIO    : 写入失败，数据通信出错
 *
 * \note 要进行此类操作，块数据必须要有值块的格式，可参考NXP的相关文档。若卡块
 * 号与传输块号相同，则将操作后的结果写入原来的块内；若卡块号与传输块号不相同，
 * 则将操作后的结果写入传输块内，结果传输块内的数据被覆盖，原块内的值不变。
 * 处于等级2的PLUS CPU卡不支持值块操作，等级1、3支持。
 */
aw_err_t awbl_zlg600a_pluscpu_val_operate (int      id,
                                           uint8_t  mode,
                                           uint16_t src_block,
                                           uint16_t dst_block,
                                           int32_t  value);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_ZLG600A_H */

/* end of file */
