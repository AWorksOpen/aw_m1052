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
 * \brief AWBus ZLG600A UART driver
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_zlg600a.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-05-03  win, first implementation
 * \endinternal
 */

#include "apollo.h"

#include "awbus_lite.h"
#include "aw_serial.h"
#include "aw_ioctl.h"
#include "aw_sem.h"
#include "aw_delay.h"
#include "aw_gpio.h"

#include "driver/rfid/awbl_zlg600a_uart.h"

#include <string.h>

/******************************************************************************/

/* declare pcf8563 device instance */
#define __ZLG600A_UART_DEV_DECL(p_this, p_dev) \
    struct awbl_zlg600a_uart_dev *p_this = \
        (struct awbl_zlg600a_uart_dev *)(p_dev)

/* declare pcf8563 device infomation */
#define __ZLG600A_UART_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_zlg600a_uart_devinfo *p_devinfo = \
        (struct awbl_zlg600a_uart_devinfo *)AWBL_DEVINFO_GET(p_dev)

/*******************************************************************************
  forward declarations
*******************************************************************************/

/** \brief 发送一个旧帧命令 */
aw_local aw_err_t __zlg600a_uart_frame_old_cmd_send (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame);

/** \brief 接收一个旧回应帧 */
aw_local aw_err_t __zlg600a_uart_frame_old_respond_recv (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame);

/** \brief 发送一个新命令帧 */
aw_local aw_err_t __zlg600a_uart_frame_new_cmd_send (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame);

/** \brief 接收一个新回应帧 */
aw_local aw_err_t __zlg600a_uart_frame_new_respond_recv (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame);

/** \brief 串口设备控制函数 */
aw_local aw_err_t __zlg600a_uart_device_ioctl(void *p_cookie, 
                                              int   request, 
                                              void *p_arg);

/** \brief first level initialization routine */
aw_local void __zlg600a_uart_inst_init1 (awbl_dev_t *p_dev);

/** \brief second level initialization routine */
aw_local void __zlg600a_uart_inst_init2 (awbl_dev_t *p_dev);

/** \brief third level initialization routine */
aw_local void __zlg600a_uart_inst_connect (awbl_dev_t *p_dev);

/** \brief method "awbl_zlg600a_uartserv_get" handler */
aw_local aw_err_t __zlg600a_uart_zlg600aserv_get (struct awbl_dev *p_dev, 
                                                  void            *p_arg);

/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_zlg600a_uart_drvfuncs = {
    __zlg600a_uart_inst_init1,
    __zlg600a_uart_inst_init2,
    __zlg600a_uart_inst_connect
};

/* ZLG600A uart old frame service functions (must defined as aw_const) */
aw_local aw_const struct awbl_zlg600a_servopts __g_zlg600a_uart_old_servopts = {
    __zlg600a_uart_frame_old_cmd_send,
    __zlg600a_uart_frame_old_respond_recv,
    __zlg600a_uart_device_ioctl,
};

/* ZLG600A uart new frame service functions (must defined as aw_const) */
aw_local aw_const struct awbl_zlg600a_servopts __g_zlg600a_uart_new_servopts = {
    __zlg600a_uart_frame_new_cmd_send,
    __zlg600a_uart_frame_new_respond_recv,
    __zlg600a_uart_device_ioctl,
};

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_zlg600aserv_get);

aw_local aw_const struct awbl_dev_method __g_zlg600a_uart_dev_methods[] = {
    AWBL_METHOD(awbl_zlg600aserv_get, __zlg600a_uart_zlg600aserv_get),
    AWBL_METHOD_END
};

/* driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_zlg600a_uart_drv_registration = {
    AWBL_VER_1,                       /* awb_ver */
    AWBL_BUSID_PLB,                   /* bus_id */
    AWBL_ZLG600A_UART_NAME,           /* p_drvname */
    &__g_zlg600a_uart_drvfuncs,       /* p_busfuncs */
    &__g_zlg600a_uart_dev_methods[0], /* p_methods */
    NULL                              /* pfunc_drv_probe */
};

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief 发送一个旧帧命令
 */
aw_local aw_err_t __zlg600a_uart_frame_old_cmd_send (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame)
{
    int      uart_id;
    int      i, j;
    uint8_t *p_buf, *p_txbuf;
    uint8_t  info_len;
    uint8_t  bcc = 0;

    uart_id = AWBL_DEV_UNIT_GET(p_cookie);

    __ZLG600A_UART_DEVINFO_DECL(p_devinfo, p_cookie);

    /*
     * 如果ZLG600A在 4.44毫秒内未接收到一个字节，则表示下一个接收的字节为一帧的
     * 开始,发送命令帧任务被更高优先级任务打断，则有可能导致当前命令发送失败，为
     * 了避免这种情况，需要使用一个缓冲区，将数据完整送入到串口。
     */

    p_devinfo->p_txbuf[0] = p_frame->info_length + 0x6;
    bcc ^= p_devinfo->p_txbuf[0];

    p_devinfo->p_txbuf[1] = p_frame->cmd_class | (p_frame->frame_seq << 4);
    bcc ^= p_devinfo->p_txbuf[1];

    p_devinfo->p_txbuf[2] = p_frame->cmd_status;
    bcc ^= p_devinfo->p_txbuf[2];

    p_devinfo->p_txbuf[3] = p_frame->info_length;
    bcc ^= p_devinfo->p_txbuf[3];

    p_txbuf = &p_devinfo->p_txbuf[4];

    /* 发送各个信息缓冲区中的信息 */
    for (i = 0; i < p_frame->buf_num; i++) {
        p_buf     = p_frame->p_info_desc[i].p_buf;
        info_len  = p_frame->p_info_desc[i].len;

        for (j = 0; j < info_len; j++) {
            bcc ^= *p_buf;
            *p_txbuf++ = *p_buf++;
        }
    }

    bcc = ~bcc;  /* 将异或值取反后作为校验和 */

    /* 发送校验和   */
    *p_txbuf++ = bcc;

    /* 发送帧结束符 */
    *p_txbuf++ = 0x03;

    /* 一次性将命令发送出去，避免被高优先级任务打断 */
    aw_serial_write(uart_id, 
                    (const char *)p_devinfo->p_txbuf, 
                    p_txbuf - p_devinfo->p_txbuf);

    return AW_OK;
}

/**
 * \brief 接收一个旧回应帧
 */
aw_local aw_err_t __zlg600a_uart_frame_old_respond_recv (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame)
{
    int      uart_id;
    int      len, i, j;
    uint8_t  info_len;
    uint8_t *p_buf;
    uint8_t  temp[4];
    uint8_t  bcc = 0, frame_bcc;  /* 接收时，可能存在数据丢弃，直接边读边校验 */
    uint8_t  ext;

    uint8_t  remain_len;
    uint8_t  discard_data;        /* 用于将接收的无用数据丢弃 */

    uart_id = AWBL_DEV_UNIT_GET(p_cookie);

    /* 接收帧长 + 命令类型 + 状态 + 信息长度 （共计4个字节）*/
    len = aw_serial_read(uart_id, (char *)temp, 4);

    if (len != 4) {
        return -AW_ETIME;
    }

    bcc ^= temp[0];
    bcc ^= temp[1];
    bcc ^= temp[2];
    bcc ^= temp[3];

    p_frame->cmd_status  = temp[2];
    p_frame->info_length = temp[3];

    remain_len = temp[3];      /* 剩余待接收信息的字符总数 */

    /* 接收各个信息缓冲区中的数据 */
    for (i = 0; i < p_frame->buf_num; i++) {
        p_buf     = p_frame->p_info_desc[i].p_buf;
        info_len  = p_frame->p_info_desc[i].len;

        /* 读取的字节数不能大于剩余字节 */
        info_len = info_len > remain_len ? remain_len : info_len;

        remain_len -= info_len;

        p_frame->p_info_desc[i].len = info_len;

        if (info_len != 0) {                       /* 存在信息，则接收 */

            if (p_buf == NULL) {                   /* 接收的数据丢弃   */
                while (info_len != 0) {
                    len = aw_serial_read(uart_id,
                                         (char *)&discard_data,
                                         1);
                    if (len != 1) {
                        return -AW_ETIME;
                    }
                    info_len--;
                    bcc ^= discard_data;
                }
            } else {
                len = aw_serial_read(uart_id, (char *)p_buf, info_len);
                if (len != info_len) {
                    return -AW_ETIME;
                }

                for (j = 0; j < info_len; j++) {
                    bcc ^= p_buf[j];
                }
            }
        }
    }

    /* 未接收的信息丢弃 */
    while (remain_len != 0) {
        len = aw_serial_read(uart_id,
                             (char *)&discard_data,
                             1);
        if (len != 1) {
            return -AW_ETIME;
        }
        remain_len--;
        bcc ^= discard_data;
    }

    /* 接收校验和 */
    len = aw_serial_read(uart_id, (char *)&frame_bcc, 1);
    if (len != 1) {
        return -AW_ETIME;
    }

    /* 接收帧结束符 */
    len = aw_serial_read(uart_id, (char *)&ext, 1);
    if (len != 1) {
        return -AW_ETIME;
    }

    bcc = ~bcc;

    if (bcc != frame_bcc) {
        return -AW_EIO;
    }

    if (temp[1] != (p_frame->cmd_class | (p_frame->frame_seq << 4))) {
        return -AW_EIO;
    }

    if (temp[2] != 0x00) {
        return -AW_EIO;
    }

    return AW_OK;
}

/******************************************************************************/

/**
 * \brief 发送一个新命令帧
 */
aw_local aw_err_t __zlg600a_uart_frame_new_cmd_send (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame)
{
    int       uart_id;
    int       i, j;
    uint8_t  *p_buf, *p_txbuf;
    uint16_t  info_len;
    uint16_t  sum = 0;

    uart_id = AWBL_DEV_UNIT_GET(p_cookie);

    __ZLG600A_UART_DEVINFO_DECL(p_devinfo, p_cookie);
    __ZLG600A_UART_DEV_DECL(p_this, p_cookie);

    /*
     * 如果ZLG600A在 4.44毫秒内未接收到一个字节，则表示下一个接收的字节为一帧的
     * 开始,发送命令帧任务被更高优先级任务打断，则有可能导致当前命令发送失败，为
     * 了避免这种情况，需要使用一个缓冲区，将数据完整送入到串口。
     */

    p_devinfo->p_txbuf[0] = p_this->addr << 1;
    p_devinfo->p_txbuf[1] = 0x00;
    p_devinfo->p_txbuf[2] = p_frame->frame_seq;
    p_devinfo->p_txbuf[3] = p_frame->cmd_class;
    p_devinfo->p_txbuf[4] = p_frame->cmd_status & 0xFF;
    p_devinfo->p_txbuf[5] = (p_frame->cmd_status >> 8) & 0xFF;
    p_devinfo->p_txbuf[6] = p_frame->info_length & 0xFF;
    p_devinfo->p_txbuf[7] = (p_frame->info_length >> 8) & 0xFF;

    for (i = 0; i < 8; i++) {
        sum += p_devinfo->p_txbuf[i];
    }

    p_txbuf = &p_devinfo->p_txbuf[8];

    /* 发送各缓冲区中的信息 */
    for (i = 0; i < p_frame->buf_num; i++) {
        p_buf     = p_frame->p_info_desc[i].p_buf;
        info_len  = p_frame->p_info_desc[i].len;
        if (info_len != 0) {
            for (j = 0; j < info_len; j++) {
                sum += *p_buf;
                *p_txbuf++ = *p_buf++;
            }
        }
    }

    sum = ~sum;   /* 将得到的各个数据的和值取反 */

    *p_txbuf++ = sum & 0xFF;
    *p_txbuf++ = (sum >> 8) & 0xFF;

    aw_serial_write(uart_id, 
                    (const char *)p_devinfo->p_txbuf, 
                    p_txbuf - p_devinfo->p_txbuf);

    return AW_OK;
}

/******************************************************************************/

/**
 * \brief 接收一个新回应帧
 */
aw_local aw_err_t __zlg600a_uart_frame_new_respond_recv (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame)
{
    int       uart_id;
    int       i, j, len;
    uint8_t  *p_buf;
    uint16_t  info_len;
    uint16_t  remain_len;
    uint8_t   discard_data;
    uint16_t  sum = 0, frame_sum;  /* 接收时，可能存在数据丢弃，直接边读边校验 */
    uint8_t   temp[4];

    uart_id = AWBL_DEV_UNIT_GET(p_cookie);

    /* 接收地址(1) + 卡槽索引(1) + 安全报文/包号(1) + 命令类型 （共计4个字节）*/
    len = aw_serial_read(uart_id, (char *)temp, 4);

    if (len != 4) {
        return -AW_ETIME;
    }

    sum += temp[0];
    sum += temp[1];
    sum += temp[2];
    sum += temp[3];

    /* 接收命令/状态码(2) + 信息长度(2)（共计4个字节） */
    len = aw_serial_read(uart_id,
                        (char *)(&p_frame->cmd_status),
                         4);
    if (len != 4) {
        return -AW_ETIME;
    }

    sum += p_frame->cmd_status & 0xFF;            /* 低字节 */
    sum += p_frame->cmd_status >> 8;              /* 高字节 */

    sum += p_frame->info_length & 0xFF;           /* 低字节 */
    sum += p_frame->info_length >> 8;             /* 低字节 */

    remain_len = p_frame->info_length;            /* 剩余待接收信息的字符总数 */

    for (i = 0; i < p_frame->buf_num; i++) {
        p_buf     = p_frame->p_info_desc[i].p_buf;
        info_len  = p_frame->p_info_desc[i].len;

        /* 读取的字节数不能大于剩余字节 */
        info_len = info_len > remain_len ? remain_len : info_len;

        remain_len -= info_len;

        p_frame->p_info_desc[i].len = info_len;   /* 本缓冲区实际读取字符长度 */

        if (info_len != 0) {                      /* 存在信息，则接收 */

            if (p_buf == NULL) {                  /* 接收的数据丢弃   */
                while (info_len != 0) {
                    len = aw_serial_read(uart_id,
                                         (char *)&discard_data,
                                         1);
                    if (len != 1) {
                        return -AW_ETIME;
                    }
                    info_len--;
                    sum += discard_data;
                }
            } else {
                len = aw_serial_read(uart_id, (char *)p_buf, info_len);
                if (len != info_len) {
                    return -AW_ETIME;
                }

                for (j = 0; j < info_len; j++) {
                    sum += p_buf[j];
                }
            }
        }
    }

    while (remain_len != 0) {
        len = aw_serial_read(uart_id,
                             (char *)&discard_data,
                             1);
        if (len != 1) {
            return -AW_ETIME;
        }
        remain_len--;
        sum += discard_data;
    }
    sum = ~sum;

    /* 接收校验和 */
    len = aw_serial_read(uart_id,
                        (char *)&frame_sum,
                         2);

    if (len != 2) {
        return -AW_ETIME;
    }

    if (sum != frame_sum) {
        return -AW_EIO;
    }

    /* 状态不为0x00，表示命令执行失败               */
    if (p_frame->cmd_status != 0x00) {
        return -AW_EIO;
    }

    /* 回应帧和命令帧的 命令类型 不同，表示出现错误 */
    if (p_frame->cmd_class != temp[3]) {
        return -AW_EIO;
    }

    /* 回应帧和命令帧的 包号 不同，表示出现错误     */
    if (p_frame->frame_seq != temp[2]) {
        return -AW_EIO;
    }

    return AW_OK;
}

/** \brief 串口设备控制函数 */
aw_local aw_err_t __zlg600a_uart_device_ioctl (void *p_cookie, 
                                               int   request, 
                                               void *p_arg)
{
    __ZLG600A_UART_DEV_DECL(p_this, p_cookie);
    int uart_id = AWBL_DEV_UNIT_GET(p_this);;

    aw_err_t ret;

    switch (request) {

    case AWBL_ZLG600A_DEV_IOCTL_BAUD_SET:
        ret = aw_serial_ioctl(uart_id,
                              SIO_BAUD_SET,
                             (void *)p_arg);
        break;

    case AWBL_ZLG600A_DEV_IOCTL_ADDR_SET:
        p_this->addr = ((int)p_arg) >> 1;
        ret = AW_OK;
        break;

    case AWBL_ZLG600A_DEV_IOCTL_FRAME_FMT_SET:
        if ((int)p_arg == AWBL_ZLG600A_FRAME_FMT_NEW) {
            p_this->serv.p_servopts = &__g_zlg600a_uart_new_servopts;
        }

        if ((int)p_arg == AWBL_ZLG600A_FRAME_FMT_OLD) {
            p_this->serv.p_servopts = &__g_zlg600a_uart_old_servopts;
        }
        ret = AW_OK;
        break;

    default :
        ret = -AW_ENOTSUP;
        break;
    }

    return ret;
}

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __zlg600a_uart_inst_init1 (awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __zlg600a_uart_inst_init2 (awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __zlg600a_uart_inst_connect (awbl_dev_t *p_dev)
{
    __ZLG600A_UART_DEV_DECL(p_this, p_dev);
    __ZLG600A_UART_DEVINFO_DECL(p_devinfo, p_dev);

    uint8_t tx_data, rx_data;

    int uart_id = AWBL_DEV_UNIT_GET(p_dev);

    /* platform initialization */
    if (p_devinfo->pfunc_plfm_init != NULL) {
        p_devinfo->pfunc_plfm_init();
    }

    /* 设置波特率为  baudrate */
    aw_serial_ioctl(uart_id,
                    SIO_BAUD_SET,
                    (void *)p_devinfo->baudrate);

    aw_serial_ioctl(uart_id, SIO_MODE_SET, (void *)SIO_MODE_INT);

    /* 设置串口超时时间 */
    aw_serial_ioctl(uart_id,
                    AW_TIOCRDTIMEOUT,
                   (void *)p_devinfo->timeout);

    if (p_devinfo->mode == AWBL_ZLG600A_MODE_AUTO_CHECK) {

        /* 当前处于自动侦测模式，连续发送两次0x20 */
        tx_data = 0x20;
        aw_serial_write(uart_id, (const char *)&tx_data, 1);
        aw_mdelay(3);
        aw_serial_write(uart_id, (const char *)&tx_data, 1);
        aw_serial_read(uart_id, (char *)&rx_data, 1);
    }

    p_this->serv.frame_seq = 0;
    p_this->addr           = p_devinfo->addr;

    return;
}

/**
 * \brief method "awbl_zlg600a_uartserv_get" handler
 */
aw_local aw_err_t __zlg600a_uart_zlg600aserv_get (struct awbl_dev *p_dev, 
                                                  void            *p_arg)
{
    __ZLG600A_UART_DEV_DECL(p_this, p_dev);
    __ZLG600A_UART_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_zlg600a_service *p_serv = NULL;

    /* get intcltr service instance */
    p_serv = &p_this->serv;

    /* initialize the ZLG600A service instance */
    p_serv->p_next     = NULL;
    p_serv->p_servinfo = &p_devinfo->zlg600a_servinfo;

    if (p_devinfo->frame_fmt == AWBL_ZLG600A_FRAME_FMT_NEW) {
        p_serv->p_servopts = &__g_zlg600a_uart_new_servopts;
    } else {
        p_serv->p_servopts = &__g_zlg600a_uart_old_servopts;
    }

    p_serv->p_cookie = (void *)p_dev;

    /* send back the service instance */
    *(struct awbl_zlg600a_service **)p_arg = p_serv;

    return AW_OK;
}

/**
 * \brief ZLG600A UART driver register
 */
void awbl_zlg600a_uart_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_zlg600a_uart_drv_registration);
}

/* end of file */
