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
 * \brief AWBus ZLG600A interface implementation (lite)
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_zlg600a.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-05-04  win, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_gpio.h"

#include "awbus_lite.h"
#include "driver/rfid/awbl_zlg600a.h"

#include <string.h>

/*******************************************************************************
  locals
*******************************************************************************/

AWBL_METHOD_DEF(awbl_zlg600aserv_get, "awbl_zlg600aserv_get");

/* pointer to first rtc service */
struct awbl_zlg600a_service *__gp_zlg600a_serv_head = NULL;

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief allocate zlg600a service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval          AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __zlg600a_serv_alloc_helper (struct awbl_dev *p_dev, 
                                               void            *p_arg)
{
    awbl_method_handler_t         pfunc_zlg600a_serv = NULL;
    struct awbl_zlg600a_service  *p_zlg600a_serv     = NULL;

    /* find handler */
    pfunc_zlg600a_serv = awbl_dev_method_get(
                            p_dev,
                            AWBL_METHOD_CALL(awbl_zlg600aserv_get));

    /* call method handler to allocate ZLG600A service */
    if (pfunc_zlg600a_serv != NULL) {

        pfunc_zlg600a_serv(p_dev, &p_zlg600a_serv);

        /* found a ZLG600A service, insert it to the service list */
        if (p_zlg600a_serv != NULL) {
            struct awbl_zlg600a_service **pp_serv_cur = &__gp_zlg600a_serv_head;

            while (*pp_serv_cur != NULL) {
                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }

            *pp_serv_cur = p_zlg600a_serv;
            p_zlg600a_serv->p_next = NULL;

            AW_MUTEX_INIT(p_zlg600a_serv->devlock, AW_SEM_Q_PRIORITY);
        }
    }

    return AW_OK;   /* iterating continue */
}

/**
 * \brief allocate zlg600a services from instance tree
 */
aw_local aw_err_t __zlg600a_serv_alloc (void)
{
    /* todo: count the number of service */

    /* Empty the list head, as every zlg600a device will be find */
    __gp_zlg600a_serv_head = NULL;

    awbl_dev_iterate(__zlg600a_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

/**
 * \brief find out a service who accept the id
 */
aw_local struct awbl_zlg600a_service * __zlg600a_id_to_serv (int id)
{
    struct awbl_zlg600a_service *p_serv_cur = __gp_zlg600a_serv_head;

    while ((p_serv_cur != NULL)) {

        if (id == p_serv_cur->p_servinfo->dev_id) {

            return p_serv_cur;
        }

        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}

/**
 * \brief AWBus ZLG600A 接口模块初始化
 *
 * \attention 本函数应当在 awbl_dev_init2() 之后调用
 */
void awbl_zlg600a_init (void)
{
    /* allocate rtc services from instance tree */
    __zlg600a_serv_alloc();
}

/******************************************************************************/

/**
 * \brief 根据设备中的frame_fmt处理一个帧
 */
aw_local aw_err_t __zlg600a_frame_proc (
                     int                           id,
                     uint8_t                       cmd_class,
                     uint16_t                      cmd,
                     awbl_zlg600a_info_buf_desc_t *p_cmd_desc,
                     uint8_t                       cmd_buf_num,
                     awbl_zlg600a_info_buf_desc_t *p_res_desc,
                     uint8_t                       res_buf_num,
                     uint16_t                     *p_res_info_len)

{
    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    aw_err_t ret = -AW_ENOTSUP;

    awbl_zlg600a_frame_base_t frame;

    aw_err_t (*pfn_temp)(void *, awbl_zlg600a_frame_base_t *);

    int      i;
    uint16_t total_len;

    if (p_serv == NULL) {
        return -AW_ENXIO;  /* ZLG600A[id] is invalid */
    }

    if (cmd_buf_num != 0 && p_cmd_desc == NULL) {
        return -AW_EINVAL;
    }

    if (res_buf_num != 0 && p_res_desc == NULL) {
        return -AW_EINVAL;
    }

    total_len = 0;
    for (i = 0; i < cmd_buf_num; i++) {
        total_len += p_cmd_desc[i].len;
    }

    p_serv->frame_seq = (p_serv->frame_seq + 1) & 0xF;

    frame.frame_seq   = p_serv->frame_seq;
    frame.cmd_class   = cmd_class;
    frame.cmd_status  = cmd;
    frame.info_length = total_len;
    frame.p_info_desc = p_cmd_desc;
    frame.buf_num     = cmd_buf_num;

    pfn_temp = (aw_err_t (*)(void *, awbl_zlg600a_frame_base_t *))
                p_serv->p_servopts->pfn_frame_send;
    if (pfn_temp == NULL) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_serv->devlock, AW_SEM_WAIT_FOREVER);

    ret = pfn_temp(p_serv->p_cookie, &frame);
    if (ret != AW_OK) {
        return ret;
    }

    frame.p_info_desc = p_res_desc;
    frame.buf_num     = res_buf_num;

    pfn_temp = (aw_err_t (*)(void *, awbl_zlg600a_frame_base_t *))
                p_serv->p_servopts->pfn_frame_recv;
    if (pfn_temp == NULL) {
        return -AW_ENOTSUP;
    }

    /* 接收回应帧，并判断是否成功 */
    ret = pfn_temp(p_serv->p_cookie, &frame);

    AW_MUTEX_UNLOCK(p_serv->devlock);

    if (p_res_info_len != NULL) {
        *p_res_info_len = frame.info_length;
    }

    return ret;
}

/**
 * \brief 读ZLG600A的产品、版本信息，如：“ZLG600A V1.00”
 */
aw_err_t awbl_zlg600a_info_get (int id, uint8_t *p_info)
{
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    rx_desc[0].p_buf = p_info;
    rx_desc[0].len   = 0x14;          /* 固定接收长度0x14 */

    return __zlg600a_frame_proc(id,
                                1,
                                'A',
                                NULL,
                                0,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief 配置IC卡接口，执行成功后，启动IC卡接口，启动后默认支持TypeA卡
 */
aw_err_t awbl_zlg600a_ic_port_config (int id)
{
    return __zlg600a_frame_proc(id,
                                1,
                                'B',
                                NULL,
                                0,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief 关闭IC卡接口
 */
aw_err_t awbl_zlg600a_ic_port_close (int id)
{
    return __zlg600a_frame_proc(id,
                                1,
                                'C',
                                NULL,
                                0,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief 设置IC卡接口协议（工作模式）TypeA 或者 TypeB
 */
aw_err_t awbl_zlg600a_ic_isotype_set (int id, uint8_t isotype)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];

    tx_desc[0].p_buf = &isotype;
    tx_desc[0].len   = 1;

    return __zlg600a_frame_proc(id,
                                1,
                                'D',
                                tx_desc,
                                1,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief 装载IC卡密钥
 *
 * 将输入的密钥保存在模块内部，模块掉电后该密钥不丢失，ZLG600A模块共能保存
 * A密钥16组、B密钥16组。
 *
 */
aw_err_t awbl_zlg600a_ic_key_load (int      id,
                                   uint8_t  key_type,
                                   uint8_t  key_sec,
                                   uint8_t *p_key,
                                   uint8_t  key_length)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[3];

    if (p_key == NULL) {
        return -AW_EINVAL;
    }

    /* 密钥类型 */
    tx_desc[0].p_buf = &key_type;
    tx_desc[0].len   = 1;

    /* 密钥区号 */
    tx_desc[1].p_buf = &key_sec;
    tx_desc[1].len   = 1;

    /* 密钥    */
    tx_desc[2].p_buf = p_key;
    tx_desc[2].len   = key_length;

    return __zlg600a_frame_proc(id,
                                1,
                                'E',
                                tx_desc,
                                3,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief 设置IC卡接口的寄存器值
 *
 * 用于设置模块上读写卡芯片内部的寄存器值，通过该命令，可以实现很多现有命令
 * 不能完成的工作。
 */
aw_err_t awbl_zlg600a_ic_reg_set (int id, uint8_t reg_addr, uint8_t reg_val)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    /* 寄存器地址 */
    tx_desc[0].p_buf = &reg_addr;
    tx_desc[0].len   = 1;

    /* 寄存器值  */
    tx_desc[1].p_buf = &reg_val;
    tx_desc[1].len   = 1;

    return __zlg600a_frame_proc(id,
                                1,
                                'F',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief 获取IC卡接口的寄存器值
 *
 * 该命令用于设置模块上读写卡芯片内部的寄存器值，
 * 通过该命令，可以实现很多现有命令不能完成的工作。
 */
aw_err_t awbl_zlg600a_ic_reg_get (int id, uint8_t reg_addr, uint8_t *p_val)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    /* 寄存器地址     */
    tx_desc[0].p_buf = &reg_addr;
    tx_desc[0].len   = 1;

    /* 获取的寄存器值 */
    rx_desc[0].p_buf = p_val;
    rx_desc[0].len   = 1;

    return __zlg600a_frame_proc(id,
                                1,
                                'G',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief 设置波特率
 *
 * 用于在UART通信过程中改变通信的波特率，掉电后该设置值保留。
 */
aw_err_t awbl_zlg600a_baudrate_set (int id, uint32_t baudrate_flag)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[1];
    uint8_t                       num = 0;     /* 波特率编码，对应0 ~ 7 */

    aw_err_t ret;

    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    switch(baudrate_flag) {

    case AWBL_ZLG600A_BAUDRATE_9600:
        num = 0;
        break;

    case AWBL_ZLG600A_BAUDRATE_19200:
        num = 1;
        break;

    case AWBL_ZLG600A_BAUDRATE_28800:
        num = 2;
        break;

    case AWBL_ZLG600A_BAUDRATE_38400:
        num = 3;
        break;
    case AWBL_ZLG600A_BAUDRATE_57600:
        num = 4;
        break;

    case AWBL_ZLG600A_BAUDRATE_115200:
        num = 5;
        break;

    case AWBL_ZLG600A_BAUDRATE_172800:
        num = 6;
        break;

    case AWBL_ZLG600A_BAUDRATE_230400:
        num = 7;
        break;

    default:
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &num;
    tx_desc[0].len   = 1;

    ret = __zlg600a_frame_proc(id,
                               1,
                               'H',
                               tx_desc,
                               1,
                               NULL,
                               0,
                               NULL);

    if (ret != AW_OK) {
        return ret;
    }

    if (p_serv->p_servopts->pfn_dev_ioctl != NULL) {
        ret = p_serv->p_servopts->pfn_dev_ioctl(p_serv->p_cookie,
                                                AWBL_ZLG600A_DEV_IOCTL_BAUD_SET,
                                                (void *)baudrate_flag);
    }

    return ret;
}

/**
 * \brief 设置天线驱动方式，可以任意打开、关闭某个天线驱动引脚
 */
aw_err_t awbl_zlg600a_ant_mode_set (int id, uint8_t ant_mode)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[1];

    tx_desc[0].p_buf = &ant_mode;
    tx_desc[0].len   = 1;

    return __zlg600a_frame_proc(id,
                                1,
                                'I',
                                tx_desc,
                                1,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief 设置帧格式，用于切换新、旧帧格式，设置成功后掉电不丢失
 */
aw_err_t awbl_zlg600a_frame_fmt_set (int id, uint8_t frame_type)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[1];

    aw_err_t  ret;

    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    tx_desc[0].p_buf = &frame_type;
    tx_desc[0].len   = 1;

    ret = __zlg600a_frame_proc(id,
                               1,
                               'K',
                               tx_desc,
                               1,
                               NULL,
                               0,
                               NULL);

    if (ret != AW_OK) {
        return ret;
    }

    if (p_serv->p_servopts->pfn_dev_ioctl != NULL) {
        ret = p_serv->p_servopts->pfn_dev_ioctl(p_serv->p_cookie,
                                                AWBL_ZLG600A_DEV_IOCTL_FRAME_FMT_SET,
                                                (void *)(int)frame_type);
    }

    return ret;
}

/**
 * \brief 设置设备工作模式和从机地址
 */
aw_err_t awbl_zlg600a_mode_addr_set (int id, uint8_t mode, uint8_t new_addr)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[2];

    aw_err_t ret;

    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    tx_desc[0].p_buf = &mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &new_addr;
    tx_desc[1].len   = 1;

    ret = __zlg600a_frame_proc(id,
                               1,
                               'U',
                               tx_desc,
                               2,
                               NULL,
                               0,
                               NULL);

    if (ret != AW_OK) {
        return ret;
    }

    if (p_serv->p_servopts->pfn_dev_ioctl != NULL) {
        ret = p_serv->p_servopts->pfn_dev_ioctl(p_serv->p_cookie,
                                                AWBL_ZLG600A_DEV_IOCTL_ADDR_SET,
                                                (void *)(int)new_addr);
    }

    return AW_OK;
}

/**
 * \brief 获取设备工作模式和从机地址
 */
aw_err_t awbl_zlg600a_mode_addr_get (int id, uint8_t *p_mode, uint8_t *p_addr)
{
    awbl_zlg600a_info_buf_desc_t  rx_desc[2];

    rx_desc[0].p_buf = p_mode;
    rx_desc[0].len   = 1;

    rx_desc[1].p_buf = p_addr;
    rx_desc[1].len   = 1;

    return __zlg600a_frame_proc(id,
                                1,
                                'V',
                                NULL,
                                0,
                                rx_desc,
                                2,
                                NULL);
}

/**
 * \brief 装载用户密钥，模块里面提供了2个16字节的存储空间用于保存用户密钥。
 */
aw_err_t awbl_zlg600a_user_key_load (int id, uint8_t key_sec, uint8_t *p_key)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[2];

    if (p_key == NULL) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &key_sec;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = p_key;
    tx_desc[1].len   = 16;              /* 长度固定为16字节  */

    return __zlg600a_frame_proc(id,
                                1,
                                'a',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief 写EEPROM，模块内部拥有一个256Byte的EEPROM
 */
aw_err_t awbl_zlg600a_eeprom_write (int      id,
                                    uint8_t  eeprom_addr,
                                    uint8_t *p_buf,
                                    uint8_t  nbytes)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[2];
    awbl_zlg600a_info_buf_desc_t  rx_desc[1];

    tx_desc[0].p_buf = &eeprom_addr;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &nbytes;
    tx_desc[1].len   = 1;              /* 长度固定为16字节  */

    rx_desc[0].p_buf = p_buf;
    rx_desc[0].len   = nbytes;

    return __zlg600a_frame_proc(id,
                                1,
                                'c',
                                tx_desc,
                                2,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief 读EEPROM，模块内部拥有一个256Byte的EEPROM
 */
aw_err_t awbl_zlg600a_eeprom_read (int      id,
                                   uint8_t  eeprom_addr,
                                   uint8_t *p_buf,
                                   uint8_t  nbytes)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[3];

    tx_desc[0].p_buf = &eeprom_addr;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &nbytes;
    tx_desc[1].len   = 1;              /* 长度固定为16字节  */

    tx_desc[2].p_buf = p_buf;
    tx_desc[2].len   = nbytes;

    return __zlg600a_frame_proc(id,
                                1,
                                'b',
                                tx_desc,
                                3,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief 获取读卡芯片复位次数
 */
aw_err_t awbl_zlg600a_ic_reset_count_get (int id, uint32_t *p_count)
{
    awbl_zlg600a_info_buf_desc_t  rx_desc[1];

    rx_desc[0].p_buf = (uint8_t *)p_count;
    rx_desc[0].len   = 4;

    return __zlg600a_frame_proc(id,
                                1,
                                'd',
                                NULL,
                                0,
                                rx_desc,
                                1,
                                NULL);
}

/*******************************************************************************
   ISO14443通用函数（本地使用）
*******************************************************************************/

/*
 *     卡请求操作。以下命令是ISO14443协议规定好的，Mifare 卡 和 PICC卡均是一样的
 * 操作,这两种卡仅命令类型不同。
 */
aw_local aw_err_t __zlg600a_iso14443_request (int       id,
                                              uint8_t   cmd_type,
                                              uint8_t   req_mode,
                                              uint16_t *p_atq)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[1];
    awbl_zlg600a_info_buf_desc_t  rx_desc[1];

    tx_desc[0].p_buf = &req_mode;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = (uint8_t *)p_atq;
    rx_desc[0].len   = 2;

    return __zlg600a_frame_proc(id,
                                cmd_type,
                                'A',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                NULL);
}

/* 防碰撞 */
aw_local aw_err_t __zlg600a_iso14443_anticoll (int       id,
                                               uint8_t   cmd_type,
                                               uint8_t   anticoll_level,
                                               uint32_t  know_uid,
                                               uint8_t   nbit_cnt,
                                               uint32_t *p_uid)
{

    awbl_zlg600a_info_buf_desc_t  tx_desc[3];
    awbl_zlg600a_info_buf_desc_t  rx_desc[1];

    tx_desc[0].p_buf = &anticoll_level;
    tx_desc[0].len   = 1;

    /* 已知序列号的长度 */
    tx_desc[1].p_buf = &nbit_cnt;
    tx_desc[1].len   = 1;

    /* 已知UID，已知UID的位数由 nbit_cnt 决定 */
    tx_desc[2].p_buf = (uint8_t *)&know_uid;
    tx_desc[2].len   = 4;

    /* 接收的4字节UID */
    rx_desc[0].p_buf = (uint8_t *)p_uid;
    rx_desc[0].len   = 4;

    if (nbit_cnt == 0) {

        return __zlg600a_frame_proc(id,
                                    cmd_type,
                                    'B',
                                    tx_desc,
                                    2,
                                    rx_desc,
                                    1,
                                    NULL);
    } else {

        return __zlg600a_frame_proc(id,
                                    cmd_type,
                                    'B',
                                    tx_desc,
                                    3,
                                    rx_desc,
                                    1,
                                    NULL);
    }
}

/* 卡选择 */
aw_local aw_err_t __zlg600a_iso14443_select (int       id,
                                             uint8_t   cmd_type,
                                             uint8_t   anticoll_level,
                                             uint32_t  uid,
                                             uint8_t  *p_sak)
{

    awbl_zlg600a_info_buf_desc_t  tx_desc[2];
    awbl_zlg600a_info_buf_desc_t  rx_desc[1];

    tx_desc[0].p_buf = &anticoll_level;
    tx_desc[0].len   = 1;

    /* 已知序列号     */
    tx_desc[1].p_buf = (uint8_t *)&uid;
    tx_desc[1].len   = 4;

    /* 接收的1字节SAK */
    rx_desc[0].p_buf = p_sak;
    rx_desc[0].len   = 1;

    return __zlg600a_frame_proc(id,
                                cmd_type,
                                'C',
                                tx_desc,
                                2,
                                rx_desc,
                                1,
                                NULL);
}

/* 卡挂起 */
aw_local aw_err_t __zlg600a_iso14443_halt (int id, uint8_t cmd_type)
{
    return __zlg600a_frame_proc(id,
                                cmd_type,
                                'D',
                                NULL,
                                0,
                                NULL,
                                0,
                                NULL);
}

/* 卡复位 */
aw_local aw_err_t __zlg600a_iso14443_reset (int     id,
                                            uint8_t cmd_type,
                                            uint8_t time_ms)
{

    awbl_zlg600a_info_buf_desc_t  tx_desc[1];

    tx_desc[0].p_buf = &time_ms;
    tx_desc[0].len   = 1;

    return __zlg600a_frame_proc(id,
                                cmd_type,
                                'L',
                                tx_desc,
                                1,
                                NULL,
                                0,
                                NULL);
}

/* 卡激活*/
aw_local aw_err_t __zlg600a_iso14443_active (int       id,
                                             uint8_t   cmd_type,
                                             uint8_t   req_mode,
                                             uint16_t *p_atq,
                                             uint8_t  *p_sak,
                                             uint8_t  *p_len,
                                             uint8_t  *p_uid,
                                             uint8_t   uid_len)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[2];
    awbl_zlg600a_info_buf_desc_t  rx_desc[4];

    uint8_t data = 0x00;                  /* 保留字节，设置为0x00 */

    /* 第一字节为保留 */
    data = 0x00;
    tx_desc[0].p_buf = &data;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &req_mode;
    tx_desc[1].len   = 1;

    rx_desc[0].p_buf = (uint8_t *)p_atq; /* 请求应答（2字节）          */
    rx_desc[0].len   = 2;

    rx_desc[1].p_buf = p_sak;            /* 最后一级选择应答SAK（1字节）*/
    rx_desc[1].len   = 1;

    rx_desc[2].p_buf = p_len;            /* 序列号长度（1字节）        */
    rx_desc[2].len   = 1;

    rx_desc[3].p_buf = p_uid;            /* 序列号（N字节，由序列号长度决定） */
    rx_desc[3].len   = uid_len;          /* 最大10字节，直接按照10字节读取    */

    return __zlg600a_frame_proc(id,
                                cmd_type,
                                'M',
                                tx_desc,
                                2,
                                rx_desc,
                                4,
                                NULL);
}

/*******************************************************************************
  Mifare卡函数
*******************************************************************************/

/**
 * \brief Mifare卡请求操作
 *
 *  该函数作为卡的请求操作，只要符合ISO14443A标准的卡都应能发出响应。
 *
 * \note 卡进入天线后，从射频场中获取能量，从而得电复位，复位后卡处于IDLE模式，
 * 用两种请求模式的任一种请求时，此时的卡均能响应；若对某一张卡成功进行了挂起
 * 操作（Halt命令或DeSelect命令），则进入了Halt模式，此时的卡只响应ALL（0x52）
 * 模式的请求，除非将卡离开天线感应区后再进入。
 */
aw_err_t awbl_zlg600a_mifare_card_request (int       id,
                                           uint8_t   req_mode,
                                           uint16_t *p_atq)
{
    return __zlg600a_iso14443_request(id,
                                      0x02,
                                      req_mode,
                                      p_atq);
}

/**
 * \brief Mifare卡的防碰撞操作
 *
 *  需要成功执行一次请求命令，并返回请求成功，才能进行防碰撞操作，否则返回错误。
 *
 * \note  符合ISO14443A标准卡的序列号都是全球唯一的，正是这种唯一性，才能实现防
 * 碰撞的算法逻辑，若有若干张卡同时在天线感应区内则这个函数能够找到一张序列号较
 * 大的卡来操作。
 */
aw_err_t awbl_zlg600a_mifare_card_anticoll (int       id,
                                            uint8_t   anticoll_level,
                                            uint32_t  know_uid,
                                            uint8_t   nbit_cnt,
                                            uint32_t *p_uid)
{
    return __zlg600a_iso14443_anticoll(id,
                                       0x02,
                                       anticoll_level,
                                       know_uid,
                                       nbit_cnt,
                                       p_uid);
}

/**
 * \brief Mifare卡的选择操作
 *
 *  需要成功执行一次防碰撞命令，并返回成功，才能进行卡选择操作，否则返回错误。
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
                                          uint8_t  *p_sak)
{
    return __zlg600a_iso14443_select(id,
                                     0x02,
                                     anticoll_level,
                                     uid,
                                     p_sak);
}

/**
 * \brief Mifare卡的挂起操作，使所选择的卡进入HALT状态
 *
 *  在HALT状态下，卡将不响应读卡器发出的IDLE模式的请求，除非将卡复位或离开天线感
 *  应区后再进入。但它会响应读卡器发出的ALL请求。
 */
aw_err_t awbl_zlg600a_mifare_card_halt (int id)
{
    return __zlg600a_iso14443_halt(id, 0x02);
}


/**
 * \brief Mifare卡E2密钥验证
 *
 *     用模块内部已存入的密钥与卡的密钥进行验证，使用该命令前应先用“装载IC卡密钥”
 * 函数把密钥成功载入模块内。另外，需要验证的卡的扇区号不必与模块内密钥区号相等。
 *
 * \note PLUS CPU系列的卡的卡号有4字节和7字节之分，对于7字节卡号的卡，
 *       只需要将卡号的高4字节（等级2防碰撞得到的卡号）作为验证的卡号即可。
 */
aw_err_t awbl_zlg600a_mifare_card_e2_authent (int      id,
                                              uint8_t  key_type,
                                              uint8_t *p_uid,
                                              uint8_t  key_sec,
                                              uint8_t  nblock)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[4];

    if (key_sec > 7) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &key_type;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = p_uid;
    tx_desc[1].len   = 4;

    tx_desc[2].p_buf = &key_sec;
    tx_desc[2].len   = 1;

    tx_desc[3].p_buf = &nblock;
    tx_desc[3].len   = 1;

    return __zlg600a_frame_proc(id,
                                2,
                                'E',
                                tx_desc,
                                4,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief Mifare卡直接密钥验证
 *
 *    该命令将密码作为参数传递，因此在此之前不需用“装载IC卡密钥”命令。若当前卡
 * 为PLUS CPU卡的等级2或等级3，且输入的密码只有6字节，则模块自动将输入的密码
 * 复制2次，取前16字节作为当前验证密钥。
 *
 * \note PLUS CPU系列的卡的卡号有4字节和7字节之分，对于7字节卡号的卡，
 *       只需要将卡号的高4字节（等级2防碰撞得到的卡号）作为验证的卡号即可。
 */
aw_err_t awbl_zlg600a_mifare_card_direct_authent (int      id,
                                                  uint8_t  key_type,
                                                  uint8_t *p_uid,
                                                  uint8_t *p_key,
                                                  uint8_t  key_len,
                                                  uint8_t  nblock)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[4];

    if (((key_len != 6) && (key_len != 16))) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &key_type;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = p_uid;
    tx_desc[1].len   = 4;

    tx_desc[2].p_buf = p_key;
    tx_desc[2].len   = key_len;

    tx_desc[3].p_buf = &nblock;
    tx_desc[3].len   = 1;

    return __zlg600a_frame_proc(id,
                                2,
                                'F',
                                tx_desc,
                                4,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief Mifare卡数据读取
 *
 *     在验证成功之后，才能读相应的块数据，所验证的块号与读块号必须在同一个扇区内，
 * Mifare1卡从块号0开始按顺序每4个块1个扇区，若要对一张卡中的多个扇区进行操作，在
 * 对某一扇区操作完毕后，必须进行一条读命令才能对另一个扇区直接进行验证命令，否则
 * 必须从请求开始操作。对于PLUS CPU卡，若下一个读扇区的密钥和当前扇区的密钥相同，
 * 则不需要再次验证密钥，直接读即可。
 *
 * \param[in] p_dev       : 指向ZLG600A设备的指针
 * \param[in] nblock      : 读取数据的块号
 *                         - S50：0 ~ 63
 *                         - S70：0 ~ 255
 *                         - PLUS CPU 2K：0 ~ 127
 *                         - PLUS CPU 4K：0 ~ 255
 * \param[in] p_buf      : 存放读取的数据，大小为 16
 *
 * \retval AW_OK      : 读取成功
 * \retval -AW_EINVAL : 读取失败，参数错误
 * \retval -AW_EIO    : 读取失败，数据通信出错
 */
aw_err_t awbl_zlg600a_mifare_card_read (int      id,
                                        uint8_t  nblock,
                                        uint8_t *p_buf)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    if (p_buf == NULL) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &nblock;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = p_buf;
    rx_desc[0].len   = 16;

    return __zlg600a_frame_proc(id,
                                2,
                                'G',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief Mifare卡写数据,写之前必需成功进行密钥验证。
 *
 *  对卡内某一块进行验证成功后，即可对同一扇区的各个进行写操作（只要访问条件允许
 *  ），其中包括位于扇区尾的密码块，这是更改密码的唯一方法。对于PLUS CPU卡等级
 *  2、3的AES密钥则是在其他位置修改密钥。
 */
aw_err_t awbl_zlg600a_mifare_card_write (int      id,
                                         uint8_t  nblock,
                                         uint8_t *p_buf)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    if (p_buf == NULL) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &nblock;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = p_buf;
    tx_desc[1].len   = 16;

    return __zlg600a_frame_proc(id,
                                2,
                                'H',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief Mifare卡写数据
 *
 * 此命令只对UltraLight卡有效，对UltraLight卡进行读操作与Mifare1卡一样。
 */
aw_err_t awbl_zlg600a_ultralight_card_write (int      id,
                                             uint8_t  nblock,
                                             uint8_t *p_buf)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    if (p_buf == NULL) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &nblock;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = p_buf;
    tx_desc[1].len   = 4;

    return __zlg600a_frame_proc(id,
                                2,
                                'I',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief Mifare值操作，对Mifare卡的值块进行加减操作
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
                                               int32_t value)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[4];

    tx_desc[0].p_buf = &mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &nblock;
    tx_desc[1].len   = 1;

    tx_desc[2].p_buf = (uint8_t *)&value;
    tx_desc[2].len   = 4;

    tx_desc[3].p_buf = &ntransblk;
    tx_desc[3].len   = 4;

    return __zlg600a_frame_proc(id,
                                2,
                                'J',
                                tx_desc,
                                4,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief Mifare卡复位，通过将载波信号关闭指定的时间，再开启来实现卡片复位。
 *
 * \param[in] p_dev      : 指向ZLG600A设备的指针
 * \param[in] time_ms    : 关闭的时间（单位:ms），0为一直关闭
 *
 * \retval AW_OK      : 成功
 * \retval -AW_EINVAL : 失败，参数错误
 * \retval -AW_EIO    : 失败，数据通信出错
 *
 * \note 该函数将天线信号关闭数毫秒，若一直关闭，则等到执行一个请求命令时打开。
 */
aw_err_t awbl_zlg600a_mifare_card_reset (int     id,
                                         uint8_t time_ms)
{
    return __zlg600a_iso14443_reset(id, 0x02, time_ms);
}

/**
 * \brief Mifare卡激活，该函数用于激活卡片，是请求、防碰撞和选择三条命令的组合。
 */
aw_err_t awbl_zlg600a_mifare_card_active (int       id,
                                          uint8_t   req_mode,
                                          uint16_t *p_atq,
                                          uint8_t  *p_sak,
                                          uint8_t  *p_len,
                                          uint8_t  *p_uid,
                                          uint8_t   uid_len)
{
    return __zlg600a_iso14443_active(id,
                                     0x02,
                                     req_mode,
                                     p_atq,
                                     p_sak,
                                     p_len,
                                     p_uid,
                                     uid_len);
}

#if 0
/**
 * \brief Mifare卡自动检测连接一个回调函数
 */
aw_err_t awbl_zlg600a_mifare_card_auto_detect_connect (
    int             id,
    aw_pfuncvoid_t  pfn_callback,
    void           *p_arg)
{
    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    p_serv->pfn_callback = pfn_callback;
    p_serv->p_arg        = p_arg;

    return AW_OK;
}

/**
 * \brief 启动Mifare卡自动检测
 * \note 在自动检测期间，若主机发送任何除读自动检测数据外的，且数据长度小于3
 *       的命令，将退出自动检测模式，如请求awbl_zlg600a_mifare_card_active()
 *       命令，在此期间，模块将不接收数据长度大于2的命令
 */
aw_err_t awbl_zlg600a_mifare_card_auto_detect_start (
    int                              id,
    awbl_zlg600a_auto_detect_info_t *p_info)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[7];

    uint8_t  tx_desc_num;
    aw_err_t ret;

    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    __ZLG600A_AD_LOCK(p_serv);

    if ((p_serv->mode & 0xF0) == AWBL_ZLG600A_MODE_MASTER) {
        return AW_OK;
    }

    tx_desc[0].p_buf = &p_info->ad_mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &p_info->tx_mode;
    tx_desc[1].len   = 1;

    tx_desc[2].p_buf = &p_info->req_mode;
    tx_desc[2].len   = 1;

    tx_desc[3].p_buf = &p_info->auth_mode;
    tx_desc[3].len   = 1;

    tx_desc[4].p_buf = &p_info->key_type;
    tx_desc[4].len   = 1;

    switch (p_info->auth_mode) {

    case AWBL_ZLG600A_MIFARE_CARD_AUTH_DIRECT :
        tx_desc[5].p_buf = p_info->key;
        tx_desc[5].len   = p_info->key_len;

        tx_desc[6].p_buf = &p_info->nblock;
        tx_desc[6].len   = 1;

        tx_desc_num = 7;
        break;

    case AWBL_ZLG600A_MIFARE_CARD_AUTH_E2 :
        tx_desc[5].p_buf = &p_info->key[0];
        tx_desc[5].len   = 1;

        tx_desc[6].p_buf = &p_info->nblock;
        tx_desc[6].len   = 1;

        tx_desc_num = 7;
        break;

    case AWBL_ZLG600A_MIFARE_CARD_AUTH_NO :
        tx_desc_num = 4;
        break;

    default :
        return -AW_EINVAL;
    }

    ret = __zlg600a_frame_proc(id,
                               2,
                               'N',
                               tx_desc,
                               tx_desc_num,
                               NULL,
                               0,
                               NULL);

    if (ret == AW_OK) {
        p_serv->mode    = (p_serv->mode & 0x0F) | AWBL_ZLG600A_MODE_MASTER;
        p_serv->ad_read = AW_TRUE;

        AW_SEMB_GIVE(p_serv->semb_ad);
    }

    __ZLG600A_AD_UNLOCK(p_serv);

    return ret;
}

/**
 * \brief 关闭Mifare卡自动检测
 */
aw_err_t awbl_zlg600a_mifare_card_auto_detect_stop (int id)
{
    //uint8_t  mode, addr;
    aw_err_t ret;

    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    __ZLG600A_AD_LOCK(p_serv);

    /* 暂时退出主机模式 */
    p_serv->mode    = (p_serv->mode & 0x0F) | AWBL_ZLG600A_MODE_SLAVE;
    p_serv->ad_read = AW_FALSE;

    ret = awbl_zlg600a_mode_addr_get(id, NULL, NULL);

    __ZLG600A_AD_UNLOCK(p_serv);

    return ret;
}

/**
 * \brief Mifare卡读取自动检测数据命令
 */
aw_err_t awbl_zlg600a_mifare_card_auto_detect_read (
    int                              id,
    awbl_zlg600a_mifare_card_info_t *p_card_info)
{
    awbl_zlg600a_info_buf_desc_t rx_desc[4];

    aw_err_t ret;
    uint16_t real_len;
    uint8_t  i, j, k;

    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    __ZLG600A_AD_LOCK(p_serv);

    rx_desc[0].p_buf = (uint8_t *)&p_card_info->tx_mode;
    rx_desc[0].len   = 1;

    rx_desc[1].p_buf = NULL;
    rx_desc[1].len   = 3;

    rx_desc[2].p_buf = (uint8_t *)&p_card_info->uid_len;
    rx_desc[2].len   = 1;

    rx_desc[3].p_buf = (uint8_t *)p_card_info->uid;
    rx_desc[3].len   = 26;

    ret = __zlg600a_frame_proc(id,
                               2,
                               'O',
                               NULL,
                               0,
                               rx_desc,
                               4,
                               &real_len);

    if (ret != AW_OK) {
        return ret;
    }

    i = p_card_info->uid_len + 2;
    j = real_len - 3;

    if (i != j) {
        i = real_len - 3 - 1;
        j = sizeof(awbl_zlg600a_mifare_card_info_t) - 1;
        for (k = 0; k < sizeof(p_card_info->card_data); k++) {
            ((uint8_t *)p_card_info)[j] = ((uint8_t *)p_card_info)[i];
            j--;
            i--;
        }

        for (i = p_card_info->uid_len; i < sizeof(p_card_info->uid); i++) {
            p_card_info->uid[i] = 0;
        }
    }

    p_serv->mode    = (p_serv->mode & 0x0F) | AWBL_ZLG600A_MODE_SLAVE;
    p_serv->ad_read = AW_FALSE;

    __ZLG600A_AD_UNLOCK(p_serv);

    return ret;
}
#endif

/**
 * \brief Mifare值操作，设置值块的值
 */
aw_err_t awbl_zlg600a_mifare_card_val_set (int     id, 
                                           uint8_t nblock, 
                                           int32_t value)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    tx_desc[0].p_buf = &nblock;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = (uint8_t *)&value;
    tx_desc[1].len   = 4;

    return __zlg600a_frame_proc(id,
                                2,
                                'P',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief Mifare值操作，获取值块的值
 */
aw_err_t awbl_zlg600a_mifare_card_val_get (int     id, 
                                           uint8_t nblock,
                                           int32_t *p_value)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    tx_desc[0].p_buf = &nblock;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = (uint8_t *)p_value;
    rx_desc[0].len   = 4;

    return __zlg600a_frame_proc(id,
                                2,
                                'Q',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief 命令传输
 *
 *     该函数属于模块扩展功能，用于模块向卡片发送任意长度组合的数据串，例如针对
 * NXP新推出的NTAG213F是属于Ultralight C系列卡片，但是该卡片又新添加了扇区数据
 * 读写密钥保护功能。而这个密钥验证命令即可利用此命名传输函数来实现。
 *
 */
aw_err_t awbl_zlg600a_mifare_card_cmd_trans (int      id,
                                             uint8_t *p_tx_buf,
                                             uint8_t  tx_nbytes,
                                             uint8_t *p_rx_buf,
                                             uint8_t  rx_nbytes)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];
    awbl_zlg600a_info_buf_desc_t rx_desc[2];

    /* 实际传输的命令数据串 */
    tx_desc[1].p_buf = p_tx_buf;
    tx_desc[1].len   = tx_nbytes;

    /* 第一字节为实际数据长度，应该是（实际信息长度 + 1）*/
    tx_nbytes++;
    tx_desc[0].p_buf = &tx_nbytes;
    tx_desc[0].len   = 1;

    /* 实际接收到的信息 */
    rx_desc[0].p_buf = p_rx_buf;
    rx_desc[0].len   = rx_nbytes;

    return __zlg600a_frame_proc(id,
                                2,
                                'S',
                                tx_desc,
                                2,
                                rx_desc,
                                1,
                                NULL);
}


/**
 * \brief 数据交互
 *
 * 该命令用读写器与卡片的数据交互，通过该函数可以实现读写卡器的所有功能。
 *
 * \param[in] p_dev      : 指向ZLG600A设备的指针
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
                                                 uint16_t *p_real_len)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[3];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    /* 交互数据 */
    tx_desc[0].p_buf = p_data_buf;
    tx_desc[0].len   = len;

    tx_desc[1].p_buf = &wtxm_crc;
    tx_desc[1].len   = 1;

    tx_desc[2].p_buf = &fwi;
    tx_desc[2].len   = 1;

    /* 实际接收到的信息 */
    rx_desc[0].p_buf = p_rx_buf;
    rx_desc[0].len   = buf_size;

    return __zlg600a_frame_proc(id,
                                2,
                                'X',
                                tx_desc,
                                3,
                                rx_desc,
                                1,
                                p_real_len);
}

/**
 * \brief 接触式IC卡复位（自动处理PPS）
 *
 *     该函数是冷复位，执行成功后会自动根据IC卡的复位信息来执行PPS命令，然后再
 * 选择awbl_zlg600a_cicc_tpdu()函数使用的传输协议（T = 0 或T = 1）。
 *
 */
aw_err_t awbl_zlg600a_cicc_reset (int       id,
                                  uint8_t   baudrate_flag,
                                  uint8_t  *p_rx_buf,
                                  uint8_t   rx_bufsize,
                                  uint16_t *p_real_len)
{
    aw_err_t ret;

    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[2];

    tx_desc[0].p_buf = &baudrate_flag;
    tx_desc[0].len   = 1;

    /* 保留信息16字节丢弃 */
    rx_desc[0].p_buf = NULL;
    rx_desc[0].len   = 16;

    /* 实际信息 */
    rx_desc[1].p_buf = p_rx_buf;
    rx_desc[1].len   = rx_bufsize;

    ret = __zlg600a_frame_proc(id,
                               5,
                               'A',
                               tx_desc,
                               1,
                               rx_desc,
                               2,
                               p_real_len);

    if (ret == AW_OK) {
        *p_real_len -= 16;
    }

    return ret;
}

/**
 * \brief 接触式IC卡传输协议
 *
 *     该命令根据接触式IC卡的复位信息，自动选择T = 0或T = 1传输协议，整个过程
 * 不需要使用者干预。该命令用于传输APDU数据流。
 *
 */
aw_err_t awbl_zlg600a_cicc_tpdu(int       id,
                                uint8_t  *p_tx_buf,
                                uint8_t   tx_bufsize,
                                uint8_t  *p_rx_buf,
                                uint8_t   rx_bufsize,
                                uint16_t *p_real_len)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    /* 发送数据 */
    tx_desc[0].p_buf = p_tx_buf;
    tx_desc[0].len   = tx_bufsize;

    /* 接收信息 */
    rx_desc[0].p_buf = p_rx_buf;
    rx_desc[0].len   = rx_bufsize;

    return __zlg600a_frame_proc(id,
                                5,
                                'B',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                p_real_len);
}

/**
 * \brief 接触式IC卡冷复位
 *
 * 该函数是冷复位，执行了接触式IC卡上电时序，执行成功后会自动根据IC卡的复位信息
 * 来选择awbl_zlg600a_cicc_tpdu()函数使用的传输协议（T = 0 或T = 1）。该函数没有
 * 自动执行PPS命令，需要用户根据复位信息来判断是否使用awbl_zlg600a_cicc_pps()来
 * 修改协议和参数。
 *
 */
aw_err_t awbl_zlg600a_cicc_coldreset (int       id,
                                      uint8_t   baudrate_flag,
                                      uint8_t  *p_rx_buf,
                                      uint8_t   rx_bufsize,
                                      uint16_t *p_real_len)
{
    aw_err_t ret;

    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[2];

    tx_desc[0].p_buf = &baudrate_flag;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = NULL;
    rx_desc[0].len   = 16;

    rx_desc[1].p_buf = p_rx_buf;
    rx_desc[1].len   = rx_bufsize;

    ret = __zlg600a_frame_proc(id,
                               5,
                               'C',
                               tx_desc,
                               1,
                               rx_desc,
                               2,
                               p_real_len);

    if (ret == AW_OK) {
        *p_real_len -= 16;
    }

    return ret;
}

/**
 * \brief 接触式IC卡热复位
 *
 *    该函数是热复位，没有执行了接触式IC卡上电时序，执行成功后会自动根据IC卡的
 * 复位信息来选择awbl_zlg600a_cicc_tpdu()函数使用的传输协议（T = 0 或T = 1）。
 * 热复位与冷复位唯一的区别是没有执行IC卡上电操作。需要用户根据复位信息来判断
 * 是否使用awbl_zlg600a_cicc_pps()来修改协议和参数。该函数必须在IC卡时钟和电源均
 * 有效的情况下才能执行。
 *
 */
aw_err_t awbl_zlg600a_cicc_warmreset (int       id,
                                      uint8_t   baudrate_flag,
                                      uint8_t  *p_rx_buf,
                                      uint8_t   rx_bufsize,
                                      uint16_t *p_real_len)
{
    aw_err_t ret;

    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[2];

    tx_desc[0].p_buf = &baudrate_flag;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = NULL;
    rx_desc[0].len   = 16;

    rx_desc[1].p_buf = p_rx_buf;
    rx_desc[1].len   = rx_bufsize;

    ret = __zlg600a_frame_proc(id,
                               5,
                               'D',
                               tx_desc,
                               1,
                               rx_desc,
                               2,
                               p_real_len);

    if (ret == AW_OK) {
        *p_real_len -= 16;
    }

    return ret;
}

/**
 * \brief 接触式IC卡停活，关闭接触式IC卡的电源和时钟
 */
aw_err_t awbl_zlg600a_cicc_deactive (int id)
{
    return __zlg600a_frame_proc(id,
                                5,
                                'E',
                                NULL,
                                0,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief 接触式IC卡协议和参数选择
 *
 *     该函数是冷复位或热复位之后且必须首先执行（协商模式下需要执行，专用模式
 * 不需要执行）。若对接触式IC卡不了解，建议使用awbl_zlg600a_cicc_reset()进行复位。
 *
 */
aw_err_t awbl_zlg600a_cicc_pps (int      id,
                                uint8_t *p_pps)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];

    tx_desc[0].p_buf = p_pps;
    tx_desc[0].len   = 4;

    return __zlg600a_frame_proc(id,
                                5,
                                'F',
                                tx_desc,
                                1,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief 接触式IC卡传输协议（T = 0）
 *
 *     该命令用于T = 0传输协议。若接触式IC卡的传输协议为T = 0，该函数等同于
 * awbl_zlg600a_cicc_tpdu()。
 *
 */
aw_err_t awbl_zlg600a_cicc_tpdu_tp0 (int       id,
                                     uint8_t  *p_tx_buf,
                                     uint8_t   tx_bufsize,
                                     uint8_t  *p_rx_buf,
                                     uint8_t   rx_bufsize,
                                     uint16_t *p_real_len)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    /* 发送数据 */
    tx_desc[0].p_buf = p_tx_buf;
    tx_desc[0].len   = tx_bufsize;

    /* 接收信息 */
    rx_desc[0].p_buf = p_rx_buf;
    rx_desc[0].len   = rx_bufsize;

    return __zlg600a_frame_proc(id,
                                5,
                                'G',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                p_real_len);
}

/**
 * \brief 接触式IC卡传输协议（T = 1）
 *
 *     该命令用于T = 1传输协议。若接触式IC卡的传输协议为T = 1，该函数等同于
 * awbl_zlg600a_cicc_tpdu()。
 */
aw_err_t awbl_zlg600a_cicc_tpdu_tp1 (int       id,
                                     uint8_t  *p_tx_buf,
                                     uint8_t   tx_bufsize,
                                     uint8_t  *p_rx_buf,
                                     uint8_t   rx_bufsize,
                                     uint16_t  *p_real_len)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    /* 发送数据 */
    tx_desc[0].p_buf = p_tx_buf;
    tx_desc[0].len   = tx_bufsize;

    /* 接收信息 */
    rx_desc[0].p_buf = p_rx_buf;
    rx_desc[0].len   = rx_bufsize;

    return __zlg600a_frame_proc(id,
                                5,
                                'H',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                p_real_len);
}

/**
 * \brief PICCA卡请求操作
 *
 *  该函数作为卡的请求操作，只要符合ISO14443A标准的卡都应能发出响应。
 *
 * \note 卡进入天线后，从射频场中获取能量，从而得电复位，复位后卡处于IDLE模式，
 * 用两种请求模式的任一种请求时，此时的卡均能响应；若对某一张卡成功进行了挂起
 * 操作（Halt命令或DeSelect命令），则进入了Halt模式，此时的卡只响应ALL（0x52）
 * 模式的请求，除非将卡离开天线感应区后再进入。
 */
aw_err_t awbl_zlg600a_picca_request (int       id,
                                     uint8_t   req_mode,
                                     uint16_t *p_atq)
{
    return __zlg600a_iso14443_request(id, 6, req_mode, p_atq);
}

/**
 * \brief PICCA卡的防碰撞操作
 *
 *  需要成功执行一次请求命令，并返回请求成功，才能进行防碰撞操作，否则返回错误。
 *
 * \note  符合ISO14443A标准卡的序列号都是全球唯一的，正是这种唯一性，才能实现防
 * 碰撞的算法逻辑，若有若干张卡同时在天线感应区内则这个函数能够找到一张序列号较
 * 大的卡来操作。
 */
aw_err_t awbl_zlg600a_picca_anticoll (int       id,
                                      uint8_t   anticoll_level,
                                      uint32_t  know_uid,
                                      uint8_t   nbit_cnt,
                                      uint32_t *p_uid)
{
    return __zlg600a_iso14443_anticoll(id,
                                       0x06,
                                       anticoll_level,
                                       know_uid,
                                       nbit_cnt,
                                       p_uid);
}

/**
 * \brief PICC A卡的选择操作
 *
 *  需要成功执行一次防碰撞命令，并返回成功，才能进行卡选择操作，否则返回错误。
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
                                    uint8_t  *p_sak)
{
    return __zlg600a_iso14443_select(id,
                                     0x06,
                                     anticoll_level,
                                     uid,
                                     p_sak);
}

/**
 * \brief PICCA卡的挂起操作，使所选择的卡进入HALT状态
 *
 *  在HALT状态下，卡将不响应读卡器发出的IDLE模式的请求，除非将卡复位或离开天线感
 *  应区后再进入。但它会响应读卡器发出的ALL请求。
 *
 */
aw_err_t awbl_zlg600a_picca_halt (int id)
{
    return __zlg600a_iso14443_halt(id, 0x06);
}

/**
 * \brief PICC TypeA卡RATS（request for answer to select）
 *
 *     RATS（request for answer to select）是ISO14443-4协议的命令，模块发送RATS，
 * 卡片发出ATS（answer to select）作为RATS的应答，在执行该命令前，必需先进行一次
 * 卡选择操作，且执行过一次RATS命令后，想再次执行RATS命令，必需先解除激活。
 *
 */
aw_err_t awbl_zlg600a_picca_rats (int       id,
                                  uint8_t   cid,
                                  uint8_t  *p_ats_buf,
                                  uint8_t   bufsize,
                                  uint16_t *p_real_len)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    tx_desc[0].p_buf = &cid;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = p_ats_buf;
    rx_desc[0].len   = bufsize;

    return __zlg600a_frame_proc(id,
                                6,
                                'E',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                p_real_len);
}

/**
 * \brief PICC TypeA卡PPS（request for answer to select）
 *
 *     PPS（protocal and parameter selection）是ISO14443-4协议的命令，用于改变有
 * 关的专用协议参数，该命令不是必需的，命令只支持默认参数，即该命令的参数设置为
 * 0即可。在执行该命令前，必需先成功执行一次RATS命令。
 *
 */
aw_err_t awbl_zlg600a_picca_pps (int id, uint8_t dsi_dri)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];

    tx_desc[0].p_buf = &dsi_dri;
    tx_desc[0].len   = 1;

    return __zlg600a_frame_proc(id,
                                6,
                                'F',
                                tx_desc,
                                1,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PICC TypeA卡解除激活
 *
 *   该命令是ISO14443-4协议的命令，用于将卡片置为挂起（HALT）状态，处于挂起
 *（HALT）状态的卡可以用“请求”命令（请求代码为ALL）来重新激活卡，只有执行“RATS”
 * 命令的卡才用该命令。
 *
 */
aw_err_t awbl_zlg600a_picca_deselect (int id)
{
    return __zlg600a_frame_proc(id,
                                6,
                                'G',
                                NULL,
                                0,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PICC卡T=CL
 *
 *     T=CL是半双工分组传输协议，ISO14443-4协议命令，用于读写器与卡片之间的数据
 * 交互，一般符合ISO14443协议的CPU卡均用该协议与读写器通信。调用该命令时只需要
 * 将CPU卡COS命令的数据作为输入即可，其他的如分组类型、卡标识符CID、帧等待时间
 * FWT、等待时间扩展倍增因子WTXM（waiting time extensionmultiplier），等等由该
 * 命令自动完成。
 *
 */
aw_err_t awbl_zlg600a_picc_tpcl (int       id,
                                 uint8_t  *p_cos_buf,
                                 uint8_t   cos_bufsize,
                                 uint8_t  *p_res_buf,
                                 uint8_t   res_bufsize,
                                 uint16_t *p_real_len)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    tx_desc[0].p_buf = p_cos_buf;
    tx_desc[0].len   = cos_bufsize;

    rx_desc[0].p_buf = p_res_buf;
    rx_desc[0].len   = res_bufsize;

    return __zlg600a_frame_proc(id,
                                6,
                                'H',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                p_real_len);
}

/**
 * \brief 数据交换
 *
 * 该命令用读写器与卡片的数据交互，通过该函数可以实现读写卡器的所有功能。
 *
 */
aw_err_t awbl_zlg600a_picc_data_exchange (int       id,
                                          uint8_t  *p_data_buf,
                                          uint8_t   len,
                                          uint8_t   wtxm_crc,
                                          uint8_t   fwi,
                                          uint8_t  *p_rx_buf,
                                          uint8_t   bufsize,
                                          uint16_t *p_real_len)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[3];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    /* 交互数据 */
    tx_desc[0].p_buf = p_data_buf;
    tx_desc[0].len   = len;

    tx_desc[1].p_buf = &wtxm_crc;
    tx_desc[1].len   = 1;

    tx_desc[2].p_buf = &fwi;
    tx_desc[2].len   = 1;

    /* 实际接收到的信息 */
    rx_desc[0].p_buf = p_rx_buf;
    rx_desc[0].len   = bufsize;

    return __zlg600a_frame_proc(id,
                                6,
                                'J',
                                tx_desc,
                                3,
                                rx_desc,
                                1,
                                p_real_len);
}

/**
 * \brief PICC A卡复位，通过将载波信号关闭指定的时间，再开启来实现卡片复位。
 */
aw_err_t awbl_zlg600a_picca_reset (int id, uint8_t time_ms)
{
    return __zlg600a_iso14443_reset(id, 0x06, time_ms);
}

/**
 * \brief PICC A卡激活，该函数用于激活卡片，是请求、防碰撞和选择三条命令的组合。
 */
aw_err_t awbl_zlg600a_picca_active (int       id,
                                    uint8_t   req_mode,
                                    uint16_t *p_atq,
                                    uint8_t  *p_saq,
                                    uint8_t  *p_len,
                                    uint8_t  *p_uid,
                                    uint8_t   uid_len)
{
    return __zlg600a_iso14443_active(id,
                                     0x06,
                                     req_mode,
                                     p_atq,
                                     p_saq,
                                     p_len,
                                     p_uid,
                                     uid_len);
}

/**
 * \brief PICC B卡激活
 *
 *    在调用该函数前，需要确保IC工作模式已经配置为TypeB模式。
 *
 */
aw_err_t awbl_zlg600a_piccb_active (int id, uint8_t req_mode, uint8_t *p_uid)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    uint8_t  data = 0x00;  /* 应用标识（1字节）：默认为0x00 */

    tx_desc[0].p_buf = &req_mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &data;
    tx_desc[1].len   = 1;

    rx_desc[0].p_buf = p_uid;
    rx_desc[0].len   = 12;

    return __zlg600a_frame_proc(id,
                                6,
                                'N',
                                tx_desc,
                                2,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief PICC B卡复位，通过将载波信号关闭指定的时间，再开启来实现卡片复位。
 *
 * \note 该函数将天线信号关闭数毫秒，若一直关闭，则等到执行一个请求命令时打开。
 */
aw_err_t awbl_zlg600a_piccb_reset (int id, uint8_t time_ms)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];

    tx_desc[0].p_buf = &time_ms;
    tx_desc[0].len   = 1;

    return __zlg600a_frame_proc(id,
                                6,
                                'O',
                                tx_desc,
                                1,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PICCA卡请求操作
 *
 *  该函数作为卡的请求操作，只要符合ISO14443A标准的卡都应能发出响应。
 *
 */
aw_err_t awbl_zlg600a_piccb_request (int      id,
                                     uint8_t  req_mode,
                                     uint8_t  slot_time,
                                     uint8_t *p_uid)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[3];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    uint8_t data = 0x00;  /* 应用标识（1字节）：默认为0x00 */

    tx_desc[0].p_buf = &req_mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &data;
    tx_desc[1].len   = 1;

    tx_desc[2].p_buf = &slot_time;
    tx_desc[2].len   = 1;

    rx_desc[0].p_buf = p_uid;
    rx_desc[0].len   = 12;

    return __zlg600a_frame_proc(id,
                                6,
                                'P',
                                tx_desc,
                                3,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief PICC B 卡的防碰撞操作
 */
aw_err_t awbl_zlg600a_piccb_anticoll (int      id,
                                      uint8_t  slot_flag,
                                      uint8_t *p_uid)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    tx_desc[0].p_buf = &slot_flag;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = p_uid;
    rx_desc[0].len   = 12;

    return __zlg600a_frame_proc(id,
                                6,
                                'Q',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                NULL);
}


/**
 * \brief PICC B卡的挂起操作，使所选择的卡进入HALT状态
 *
 *     该函数用于B型卡挂起，在执行挂起命令前，必需先执行成功过一次请求命令。执行
 * 挂起命令成功后，卡片处于挂起状态，模块必需通过ALL方式请求卡片，而不能用IDLE方
 * 式请求。
 *
 */
aw_err_t awbl_zlg600a_piccb_halt (int id, uint8_t *p_pupi)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];

    tx_desc[0].p_buf = p_pupi;
    tx_desc[0].len   = 4;

    return __zlg600a_frame_proc(id,
                                6,
                                'S',
                                tx_desc,
                                1,
                                NULL,
                                0,
                                NULL);
}


/**
 * \brief 修改B卡的传输属性
 *
 *     该函数用于B型卡挂起，在执行挂起命令前，必需先执行成功过一次请求命令。执行
 * 挂起命令成功后，卡片处于挂起状态，模块必需通过ALL方式请求卡片，而不能用IDLE方
 * 式请求。
 *
 */
aw_err_t awbl_zlg600a_piccb_attrib_set (int      id,
                                        uint8_t *p_pupi,
                                        uint8_t  cid,
                                        uint8_t  protype)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[3];

    tx_desc[0].p_buf = p_pupi;
    tx_desc[0].len   = 4;

    tx_desc[1].p_buf = &cid;
    tx_desc[1].len   = 1;

    tx_desc[2].p_buf = &protype;
    tx_desc[2].len   = 1;

    return __zlg600a_frame_proc(id,
                                6,
                                'R',
                                tx_desc,
                                3,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief 获取8字节身份证ID
 *
 *   该函数用于获取二代身份证ID，在执行命令前，必需先调用
 * awbl_zlg600a_ic_isotype_set()配置协议为ISO14443B。
 */
aw_err_t awbl_zlg600a_sid_get (int id, uint8_t req_mode, uint8_t *p_sid)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    tx_desc[0].p_buf = &req_mode;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = p_sid;
    rx_desc[0].len   = 8;

    return __zlg600a_frame_proc(id,
                                6,
                                'T',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                NULL);
}

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
 */
aw_err_t awbl_zlg600a_pluscpu_persotcl_write (int       id,
                                              uint16_t  sector_addr,
                                              uint8_t  *p_buf)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    if ((p_buf == NULL)) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = (uint8_t *)&sector_addr;
    tx_desc[0].len   = 2;

    tx_desc[1].p_buf = p_buf;
    tx_desc[1].len   = 16;

    return __zlg600a_frame_proc(id,
                                7,
                                'B',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

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
 */
aw_err_t awbl_zlg600a_pluscpu_persotcl_commit (int id)
{
    return __zlg600a_frame_proc(id,
                                7,
                                'C',
                                NULL,
                                0,
                                NULL,
                                0,
                                NULL);
}


/**
 * \brief PLUS CPU卡首次验证（直接密钥验证）
 *
 * \note PLUS CPU卡的密钥A/B是通过地址的奇偶数来区分，AES的密钥地址与数据块的
 * 扇区关系对应如下:
 * 密钥A地址=0x4000 + 扇区 × 2
 * 密钥B地址=0x4000 + 扇区 × 2 + 1
 * 因此，如验证1扇区的密钥A，则密钥地址为：0x4002
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_first_direct_authent (int       id,
                                                        uint16_t  authent_addr,
                                                        uint8_t  *p_key)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    if ((p_key == NULL)) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = (uint8_t *)&authent_addr;
    tx_desc[0].len   = 2;

    tx_desc[1].p_buf = p_key;
    tx_desc[1].len   = 16;

    return __zlg600a_frame_proc(id,
                                7,
                                'J',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PLUS CPU卡首次验证（E2密钥验证），验证的密钥来自模块内部，掉电不丢失
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_first_e2_authent (int      id,
                                                    uint16_t authent_addr,
                                                    uint8_t  key_sector)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    tx_desc[0].p_buf = (uint8_t *)&authent_addr;
    tx_desc[0].len   = 2;

    tx_desc[1].p_buf = &key_sector;
    tx_desc[1].len   = 1;

    return __zlg600a_frame_proc(id,
                                7,
                                'K',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PLUS CPU卡跟随验证（直接密钥验证）
 *
 *     该函数用于SL3 PLUS CPU卡的跟随密钥验证，验证的密钥来自命令参数，只有执行
 * 过“首次验证”命令成功后才能使用该命令。两种验证的区别在于使用的时间不同，
 * “首次验证”所需要的时间比“跟随验证”的时间要长些。
 *
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_follow_direct_authent (int       id,
                                                         uint16_t  authent_addr,
                                                         uint8_t  *p_key)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    if ((p_key == NULL)) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = (uint8_t *)&authent_addr;
    tx_desc[0].len   = 2;

    tx_desc[1].p_buf = p_key;
    tx_desc[1].len   = 16;

    return __zlg600a_frame_proc(id,
                                    7,
                                    'L',
                                    tx_desc,
                                    2,
                                    NULL,
                                    0,
                                    NULL);
}

/**
 * \brief PLUS CPU卡跟随验证（E2密钥验证）,验证的密钥来自模块内部，掉电不丢失
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_follow_e2_authent (int      id,
                                                     uint16_t authent_addr,
                                                     uint8_t  key_sector)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    tx_desc[0].p_buf = (uint8_t *)&authent_addr;
    tx_desc[0].len   = 2;

    tx_desc[1].p_buf = &key_sector;
    tx_desc[1].len   = 1;

    return __zlg600a_frame_proc(id,
                                7,
                                'M',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PLUS CPU卡SL3复位验证（E2密钥验证）,验证的密钥来自模块内部，掉电不丢失
 *
 *  该命令用于PLUS CPU卡通过首次验证后的使用过程中，复位读写计数器和验证等信息。
 *
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_reset_authent (int id)
{
    return __zlg600a_frame_proc(id,
                                7,
                                'N',
                                NULL,
                                0,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PLUS CPU卡SL3读数据块
 *
 *  该函数用于读取SL3的数据块，在读数据块之前必需成功执行一次密钥验证。
 *
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_read (int       id,
                                        uint8_t   read_mode,
                                        uint16_t  start_block,
                                        uint8_t   block_num,
                                        uint8_t  *p_buf)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[3];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    if ((p_buf == NULL)) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &read_mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = (uint8_t *)&start_block;
    tx_desc[1].len   = 2;

    tx_desc[2].p_buf = &block_num;
    tx_desc[2].len   = 1;

    rx_desc[0].p_buf = p_buf;
    rx_desc[0].len   = block_num * 16;

    return __zlg600a_frame_proc(id,
                                7,
                                'O',
                                tx_desc,
                                3,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief PLUS CPU卡SL3写数据块
 *
 *  该函数用于写SL3的数据块，在写数据块之前必需成功执行一次密钥验证。
 *
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_write (int       id,
                                         uint8_t   write_mode,
                                         uint16_t  start_block,
                                         uint8_t   block_num,
                                         uint8_t  *p_buf)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[4];

    if ((p_buf == NULL)) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &write_mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = (uint8_t *)&start_block;
    tx_desc[1].len   = 2;

    tx_desc[2].p_buf = &block_num;
    tx_desc[2].len   = 1;

    tx_desc[3].p_buf = p_buf;
    tx_desc[3].len   = block_num * 16;

    return __zlg600a_frame_proc(id,
                                7,
                                'P',
                                tx_desc,
                                4,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PLUS CPU卡值块操作
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
                                           int32_t  value)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[4];

    tx_desc[0].p_buf = &mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = (uint8_t *)&src_block;
    tx_desc[1].len   = 2;

    tx_desc[2].p_buf = (uint8_t *)&dst_block;
    tx_desc[2].len   = 2;

    tx_desc[3].p_buf = (uint8_t *)&value;
    tx_desc[3].len   = 4;

    return __zlg600a_frame_proc(id,
                                7,
                                'S',
                                tx_desc,
                                4,
                                NULL,
                                0,
                                NULL);
}


/* end of file */
