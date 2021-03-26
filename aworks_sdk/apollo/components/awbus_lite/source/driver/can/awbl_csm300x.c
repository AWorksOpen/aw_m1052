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
 * \brief driver for CSM300X.
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-22  zengqingyuhang, first implementation.
 * \endinternal
 */

/**
 * ���ļ�Ϊcsm300xģ����м��������
 * ����ײ�����ʵ����awbl_uart_csm300x.c��awbl_spi_csm300x.c�ļ��У�
 *
 */
#include "aworks.h"
#include "awbl_can.h"
#include "aw_vdebug.h"
#include "driver/can/awbl_csm300x.h"
#include "aw_gpio.h"
#include "aw_delay.h"
#include "aw_mem.h"
#include "aw_sem.h"
#include "string.h"
#include "aw_can.h"
#include "aw_ioctl.h"
#include "aw_task.h"
#include "aw_serial.h"
#include "aw_rngbuf.h"


#if defined(AW_FALSE)
#define CSM300X_FALSE AW_FALSE
#else
#define CSM300X_FALSE FALSE
#endif

#define UART_BAUD       0
#define CAN_BAUD        1

/**< brief ģ�鹤��ģʽ */
#define UART_CFG_MODE   0
#define SPI_CFG_MODE    1
#define UART_WORK_MODE  2
#define SPI_WORK_MODE   3

#define D_BAUD                      125000  /**< brief Ĭ�ϵ�can������ */
#define CSM_TX_TASK_STACK_SIZE      2048
#define CSM_RX_TASK_STACK_SIZE      2048

#define NORMAL_CAN_ID_LEN           2       /**< brief ��׼CAN֡ID���� */
#define EXTEND_CAN_ID_LEN           4       /**< brief ��չCAN֡ID���� */
#define MAX_DATA_BUF_LEN            8       /**< brief ÿ֡������ݳ��� */
#define UART_MSG_MIN_LEN            3       /**< brief uart֡��Ϣ��С����*/
#define UART_MSG_MAX_LEN            13      /**< brief uart֡��Ϣ��󳤶� */

#define NORMAL_CAN_ID_MASK          0x7FF   /**< brief ��׼CAN֡ID���ֵ */
#define EXTEND_CAN_ID_MASK          0x1FFFFFFF/**< brief ��չCAN֡���ֵ */

#define NORMAL_CAN_MSG              0x00    /**< brief ��׼CAN֡ */
#define EXTEND_CAN_MSG              0x08    /**< brief ��չCAN֡ */

AW_MUTEX_DECL_STATIC(__g_cfg_lock);

/*****************************************************************************/
/** \brief ��p_serv���can_dev����� */
#define __CSM300X_CAN_GET_THIS(p_serv) \
        AW_CONTAINER_OF(p_serv, struct csm300x_dev, can_serv)

/** \brief ��p_dev���can_dev����� */
#define __CSM300X_CAN_GET_DEV(p_dev) \
        AW_CONTAINER_OF(p_dev, struct csm300x_dev, dev)

/** \brief ��p_dev���can_devinfo����� */
#define __CSM300X_CAN_GET_INFO(p_dev) \
        ((struct csm300x_dev_info *)AWBL_DEVINFO_GET(p_dev))


aw_can_baud_param_t CAN_BAUD_10K = {15,  4,   4,   0,   200};
aw_can_baud_param_t CAN_BAUD_20K = {15,  4,   4,   0,   100};
aw_can_baud_param_t CAN_BAUD_50K = {15,  4,   4,   0,   40 };
aw_can_baud_param_t CAN_BAUD_100K = {15,  4,   4,   0,   20 };
aw_can_baud_param_t CAN_BAUD_125K = {15,  4,   4,   0,   16 };
aw_can_baud_param_t CAN_BAUD_250K = {15,  4,   4,   0,   8  };
aw_can_baud_param_t CAN_BAUD_500K = {15,  4,   4,   0,   4  };
aw_can_baud_param_t CAN_BAUD_800K = {6,   3,   3,   0,   5  };
aw_can_baud_param_t CAN_BAUD_1000K = {15,  4,   4,   0,   2  };

/*****************************************************************************/
aw_err_t csm_uart_cfg (int com, int baud)
{
    /* ���ڳ�ʼ������ ������ */
    aw_serial_ioctl(com, SIO_BAUD_SET, (void *)baud);
    /* ���ڲ��� ��8������λ 1��ֹͣλ������żУ�� */
    aw_serial_ioctl(com, SIO_HW_OPTS_SET, (void *)(CS8 | CLOCAL | CREAD));
    /* ��մ���������������� */
    aw_serial_ioctl(com, AW_FIOFLUSH, NULL);
    /* ���ô��ڶ�ȡ��ʱʱ�� */
    aw_serial_ioctl(com, AW_TIOCRDTIMEOUT, (void *)10);
    /* ����Ϊ�ж�ģʽ */
    aw_serial_ioctl(com, SIO_MODE_SET, (void *)SIO_MODE_INT);
    return AW_OK;
}

/*****************************************************************************/
aw_err_t csm_uart_write (int com, void *pbuf, uint8_t len)
{
    ssize_t     send_bytes = 0;
    send_bytes = aw_serial_write(com, pbuf, len);
    if (send_bytes == len) {
        return AW_OK;
    }
    return AW_ERROR;
}

/*****************************************************************************/
aw_err_t csm_uart_read (int com, void *pbuf, uint8_t len, int timeout)
{
    int        rbytes = 0;
    aw_tick_t  tick1, tick2;
    char      *ptr = (char *)pbuf;

    tick1 = aw_sys_tick_get();
    while (len) {
        rbytes = aw_serial_read(com, ptr, len);
        if (rbytes < 0) {
            break;
        }
        len -= rbytes;
        ptr += rbytes;

        tick2 = aw_sys_tick_diff(tick1, aw_sys_tick_get());
        if (aw_ticks_to_ms(tick2) >= timeout) {
            break;
        }
    }

    if(len){
        return AW_ERROR;
    }

    return AW_OK;
}

/*****************************************************************************/
aw_local uint8_t __baud_to_hex (uint8_t func, uint32_t baud)
{
    uint8_t tbaud = 0;
    if(func == UART_BAUD){
        switch(baud){
            case 115200:  tbaud = 0x01;break;
            case 57600:   tbaud = 0x02;break;
            case 38400:   tbaud = 0x03;break;
            case 19200:   tbaud = 0x04;break;
            case 14400:   tbaud = 0x05;break;
            case 9600:    tbaud = 0x06;break;
            case 4800:    tbaud = 0x07;break;
            case 2400:    tbaud = 0x08;break;
            case 1200:    tbaud = 0x09;break;
            case 600:     tbaud = 0x0A;break;
            case 300:     tbaud = 0x0B;break;
            case 128000:  tbaud = 0x0C;break;
            case 230400:  tbaud = 0x0D;break;
            case 256000:  tbaud = 0x0E;break;
            case 460800:  tbaud = 0x0F;break;
            case 921600:  tbaud = 0x10;break;
            default:      tbaud = 0x00;break;
        }
        return tbaud;
    }
    if(func == CAN_BAUD){
        switch(baud){
            case 5000:    tbaud = 0x01;break;
            case 10000:   tbaud = 0x02;break;
            case 20000:   tbaud = 0x03;break;
            case 40000:   tbaud = 0x04;break;
            case 50000:   tbaud = 0x05;break;
            case 80000:   tbaud = 0x06;break;
            case 100000:  tbaud = 0x07;break;
            case 125000:  tbaud = 0x08;break;
            case 200000:  tbaud = 0x09;break;
            case 250000:  tbaud = 0x0A;break;
            case 400000:  tbaud = 0x0B;break;
            case 500000:  tbaud = 0x0C;break;
            case 666000:  tbaud = 0x0D;break;
            case 800000:  tbaud = 0x0E;break;
            case 1000000: tbaud = 0x0F;break;
            default:      tbaud = 0x00;break;
        }
        return tbaud;
    }
    return AW_ERROR;
}

/*****************************************************************************/
aw_local uint8_t __get_config_info_xor (struct csm300x_config_info *p_reg)
{
    uint8_t *pbuf = (uint8_t *)p_reg;
    uint8_t i = 0;
    uint8_t xor = 0;
    xor = pbuf[i];
    for(i = 1; i < (sizeof(struct csm300x_config_info) - 1); i ++){
        xor ^= pbuf[i];
    }
    p_reg->crccheck = xor;
    return xor;
}

/*****************************************************************************/
aw_local aw_err_t __buffer_xor_check (void * pbuf, uint8_t len)
{
    uint8_t *tbuf = (uint8_t *)pbuf;
    uint8_t     i = 0;
    uint8_t   xor = 0;
    xor = tbuf[i];
    for(i = 1; i < (len - 1); i ++){
        xor ^= tbuf[i];
    }
    if(xor == tbuf[len - 1]){
        return AW_OK;
    }
    return AW_ERROR;
}

/*****************************************************************************/
aw_local void __csm300x_config_info_init (struct csm300x_config_info *p_reg,
                                          csm300x_dev_info_t         *p_info )
{
    uint8_t tbaud = 0;
    AW_MUTEX_LOCK(__g_cfg_lock, AW_SEM_WAIT_FOREVER);
    
    /* �������֡��Ϣ������ */
    memset((uint8_t *)p_reg, 0x00, sizeof(struct csm300x_config_info));

    /* ��ʼ������֡ĳЩ���ɸ��ĵĹ̶�ֵ���� */
    p_reg->fixed0 = 0xF7;
    p_reg->fixed1 = 0xF8;
    p_reg->cmd = 0x01;
    p_reg->datalen = 0x3C;

    tbaud = __baud_to_hex (UART_BAUD, p_info->uartinfo.uartbaud);
    if (tbaud != 0) {
        p_reg->uartbaud = tbaud;
    } else {
        AW_INFOF(("uart baud rate error\r\n"));
    }

    p_reg->uartdatabits = 0x08;
    p_reg->uartstopbits= 0x01;
    p_reg->uartvarify = 0x00;

    /* can baud Ĭ��can������Ϊ125K*/
    tbaud = 0;
    tbaud = __baud_to_hex (CAN_BAUD, D_BAUD);
    if (tbaud != 0) {
        p_reg->canbaud = tbaud;
    } else {
        AW_INFOF(("can baud rate error\r\n"));
    }

    p_reg->canmask[0] = 0xFF;
    p_reg->canmask[1] = 0xFF;
    p_reg->canmask[2] = 0xFF;
    p_reg->canmask[3] = 0xFF;

    p_reg->xfermode = p_info->xferinfo.xfermode;
    p_reg->xferdir = p_info->xferinfo.xferdir;
    p_reg->uartframegap = p_info->uartinfo.uartgap;

    p_reg->framenode = p_info->xferinfo.framenode;
    p_reg->frameend = p_info->xferinfo.frameend;

    p_reg->spifeedbackcnt = 0x08;
    p_reg->spifeedbacktime = 0x01;
    p_reg->spicphalevel = 0x01;
    p_reg->spicpollevel = 0x01;

    __get_config_info_xor (p_reg);      /* ���ǰ�������ֽڵ����ֵ */

    AW_MUTEX_UNLOCK(__g_cfg_lock);
}

aw_local void __csm300x_rst (int rst_pin)
{
    aw_gpio_set(rst_pin, 0);
    aw_mdelay(1);
    aw_gpio_set(rst_pin, 1);
    aw_mdelay(200);
}

/*****************************************************************************/
aw_local aw_err_t __csm300x_mode_set (struct csm300x_dev_info *p_info,
                                        uint8_t                mode)
{
    int cfg_sta = 0;
    int mode_sta = 0;
    switch(mode){
        case UART_CFG_MODE:  cfg_sta = 0; mode_sta = 0; break;
        case SPI_CFG_MODE:   cfg_sta = 0; mode_sta = 1; break;
        case UART_WORK_MODE: cfg_sta = 1; mode_sta = 0; break;
        case SPI_WORK_MODE:  cfg_sta = 1; mode_sta = 1; break;
        default: return AW_ERROR; break;
    }

    if (p_info->pfun_common_pin_set != NULL) {
        p_info->pfun_common_pin_set(cfg_sta,
                                    mode_sta,
                                    p_info->hwinfo.csm_rst_pin);
    } else {
        if (p_info->hwinfo.csm_cfg_pin >= 0) {
            aw_gpio_set(p_info->hwinfo.csm_cfg_pin, cfg_sta);
        }
        if (p_info->hwinfo.csm_mode_pin >= 0) {
            aw_gpio_set(p_info->hwinfo.csm_mode_pin, mode_sta);
        }
        __csm300x_rst(p_info->hwinfo.csm_rst_pin);
    }
    return AW_OK;
}

/*****************************************************************************/
aw_local aw_err_t __can_to_uart_msg ( aw_can_std_msg_t    *p_can,
                                      csm300x_dev_info_t  *p_info,
                                      serial_std_msg_t    *p_serial)
{
    struct aw_can_msg *p_canmsg = &p_can->can_msg;
    aw_serial_msg_t     s_msg ;
    memset (&s_msg, 0, sizeof(s_msg) );
    uint8_t             i = 0;
    uint8_t             j = 0;
    uint32_t            canmsg_id = p_canmsg->id;

    s_msg.node = p_info->xferinfo.framenode;        /* ֡ͷ*/
    s_msg.end = p_info->xferinfo.frameend;          /* ֡β*/

    /* ��չ֡ */
    if (p_canmsg->flags & AW_CAN_MSG_FLAG_EXTERND) {
        s_msg.id_len = EXTEND_CAN_ID_LEN;                  /* ֡ID����*/
        s_msg.type = EXTEND_CAN_MSG;
    } else {      /* ��׼֡ */
        s_msg.id_len = NORMAL_CAN_ID_LEN;
        s_msg.type = NORMAL_CAN_MSG;                       /* ֡����*/
        canmsg_id = (canmsg_id << 16) & 0XFFFF0000;
    }
    for (i = 0; i < 4; i ++) {
       s_msg.id[i] = (canmsg_id & 0xFF000000) >> 24;       /* ֡ID����*/
       canmsg_id <<= 8;
    }
    s_msg.buf_len = p_canmsg->length;                      /* ���ݳ���*/
    s_msg.buf = p_can->msgbuff;                            /* �������ַ*/

    s_msg.len = 1 + s_msg.id_len + s_msg.buf_len;   /* ����֡����:֡����+֡ID+���ݳ���*/

    for (i = 0; i < 6; i ++) {
        switch (i) {
        case 0:p_serial->buf[j] = s_msg.node; j++; break;
        case 1:p_serial->buf[j] = s_msg.len; j++; break;
        case 2:p_serial->buf[j] = s_msg.type; j++; break;
        case 3:memcpy(&p_serial->buf[j], &s_msg.id, s_msg.id_len);
               j +=  s_msg.id_len;
               break;
        case 4:memcpy(&p_serial->buf[j], s_msg.buf, s_msg.buf_len);
               j +=  s_msg.buf_len;
               break;
        case 5:p_serial->buf[j] = s_msg.end; j++; break;
        }
    }
    p_serial->len = s_msg.len + 3;
    return AW_OK;
}

/*****************************************************************************/
aw_local aw_err_t __csm300x_task_init ( const char     *name,
                           int             priority,
                           size_t          stack_size,
                           aw_pfuncvoid_t  func,
                           void           *parg )
{
    aw_task_id_t    tsk;
    tsk = aw_task_create ( name,
                           priority,
                           stack_size,
                           func,
                           parg);

    if (tsk == NULL) {
        AW_INFOF(("ERROR:%s creat failed, line %d\r\n", name, __LINE__));
        return AW_ERROR;;
    }

    aw_task_startup(tsk);
    return AW_OK;
}

aw_err_t __csm_read_bytes (int com, char *buf, uint8_t varify, uint8_t rty)
{
    uint8_t     i = 0;
    char        read = 0;
    aw_err_t    ret = 0;
    for (i = 0; i < rty; i++) {
        ret = aw_serial_read(com, (char *)&read, 1);
        if (ret > 0 || ret < 0) {
            break;
        }
    }
    if (read == varify) {
        *buf = read;
        return AW_OK;/* ��ȡ������ */
    }
    *buf = 0;
    return AW_ERROR;
}

/*****************************************************************************/
void __csm300x_tx_task ( void *p_arg )
{
    struct awbl_can_service *p_serv = (struct awbl_can_service *)p_arg;
    struct csm300x_dev      *p_dev = __CSM300X_CAN_GET_THIS(p_serv);
    struct csm300x_dev_info *p_info = __CSM300X_CAN_GET_INFO(p_dev);

    AW_FOREVER {
        AW_SEMB_TAKE (p_info->tx_semb, AW_WAIT_FOREVER);
        /* �����жϷ�����ģ������ж� */
        awbl_can_isr_handler (p_serv);
    }
}

/*****************************************************************************/
void __csm300x_rx_task ( void *p_arg )
{
    struct awbl_can_service *p_serv = (struct awbl_can_service *)p_arg;
    struct csm300x_dev      *p_dev = __CSM300X_CAN_GET_THIS(p_serv);
    struct csm300x_dev_info *p_info = __CSM300X_CAN_GET_INFO(p_dev);

    uint8_t     i = 0;
    uint32_t    temp_id = 0;
    uint8_t     read = 0;
    uint8_t     id_buf[4] = {0};
    uint8_t     t_buf[MAX_DATA_BUF_LEN] = {0};
    aw_err_t    ret = 0;
    aw_serial_msg_t     p_serial;

    AW_FOREVER {

__node:
        /* ��ȡ֡ͷ */
        ret = csm_uart_read (p_info->uartinfo.uartcom, &read, 1, 50);
        if (ret != AW_OK) {
            goto __node;/* δ��ȡ�����ݣ����ص����¶�ȡ֡ͷ���� */
        }
        if (read == p_info->xferinfo.framenode) {
            p_serial.node = read;
            read = 0;
            temp_id = 0;
            memset(&p_serial, 0, sizeof(p_serial));
            memset(&id_buf, 0, sizeof(id_buf));
            memset(&t_buf, 0, sizeof(t_buf));
        } else {
            goto __node;/* ���ݴ��󣬷��ص����¶�ȡ֡ͷ���� */
        }

        /* ��ȡ֡���� */
        ret = csm_uart_read (p_info->uartinfo.uartcom, &read, 1, 50);
        if (ret != AW_OK) {
            goto __node;/* δ��ȡ�����ݣ����ص����¶�ȡ֡ͷ���� */
        }
        if (read >= UART_MSG_MIN_LEN && read <= UART_MSG_MAX_LEN) {
            p_serial.len = read;
            read = 0;
        } else {
            goto __node;/* ���ݴ��󣬷��ص����¶�ȡ֡ͷ���� */
        }

        /* ��ȡ֡���� */
        ret = csm_uart_read (p_info->uartinfo.uartcom, &read, 1, 50);
        if (ret != AW_OK) {
            goto __node;/* δ��ȡ�����ݣ����ص����¶�ȡ֡ͷ���� */
        }
        if (read == NORMAL_CAN_MSG || read == EXTEND_CAN_MSG) {
            if (read == NORMAL_CAN_MSG) {
                p_serial.type = NORMAL_CAN_MSG;
                p_serial.id_len = NORMAL_CAN_ID_LEN;
                p_serial.buf_len = p_serial.len - NORMAL_CAN_ID_LEN - 1;
            } else {
                p_serial.type = EXTEND_CAN_MSG;
                p_serial.id_len = EXTEND_CAN_ID_LEN;
                p_serial.buf_len = p_serial.len - EXTEND_CAN_ID_LEN - 1;
            }
        } else {
            goto __node;/* ���ݴ��󣬷��ص����¶�ȡ֡ͷ���� */
        }

        /* ��ȡ֡ID */
        ret = csm_uart_read (p_info->uartinfo.uartcom, &id_buf, p_serial.id_len, 50);
        if (ret != AW_OK) {
            goto __node;/* δ��ȡ�����ݣ����ص����¶�ȡ֡ͷ���� */
        }
        for (i = 0; i < p_serial.id_len; i ++) {
            temp_id <<= 8;
            temp_id |= id_buf[i];
        }
        if (p_serial.type == NORMAL_CAN_MSG ) {
            if (temp_id > NORMAL_CAN_ID_MASK) {
                goto __node;/* ���ݴ��󣬷��ص����¶�ȡ֡ͷ���� */
            }
        } else {
            if (temp_id > EXTEND_CAN_ID_MASK) {
                goto __node;/* ���ݴ��󣬷��ص����¶�ȡ֡ͷ���� */
            }
        }

        /* ��ȡ���� */
        if (p_serial.buf_len < 0 || p_serial.buf_len > MAX_DATA_BUF_LEN) {
            goto __node;/* ���ݴ��󣬷��ص����¶�ȡ֡ͷ���� */
        }
        ret = csm_uart_read (p_info->uartinfo.uartcom, &t_buf, p_serial.buf_len, 50);
        if (ret != AW_OK) {
            goto __node;/* δ��ȡ�����ݣ����ص����¶�ȡ֡ͷ���� */
        }

        /* ��ȡ֡β */
        ret = csm_uart_read (p_info->uartinfo.uartcom, &read, 1, 50);
        if (ret != AW_OK) {
            goto __node;/* δ��ȡ�����ݣ����ص����¶�ȡ֡ͷ���� */
        }
        if (read == p_info->xferinfo.frameend) {
            p_serial.end = read;
            read = 0;
        } else {
            goto __node;/* ���ݴ��󣬷��ص����¶�ȡ֡ͷ���� */
        }

        if (p_serial.type == EXTEND_CAN_MSG) {
            p_info->p_can_msg.can_msg.flags = AW_CAN_MSG_FLAG_EXTERND;
        } else {
            p_info->p_can_msg.can_msg.flags = AW_CAN_MSG_FLAG_SEND_NORMAL;
        }
        p_info->p_can_msg.can_msg.id = temp_id;
        p_info->p_can_msg.can_msg.length = p_serial.buf_len;
        memcpy(&p_info->p_can_msg.msgbuff, &t_buf, p_serial.buf_len);

        p_info->dev_sta = RC_FLAGE;
        /* �����жϷ�����ģ������ж� */
        awbl_can_isr_handler (p_serv);
    }
}

aw_local aw_err_t __csm300x_task_start (struct csm300x_dev *p_dev)
{
    csm300x_dev_info_t          *p_info = __CSM300X_CAN_GET_INFO(p_dev);
    aw_err_t    err = 0;
    if (p_info->task_priority == 0) {
        p_info->task_priority = 5;   /* Ĭ�����ȼ�Ϊ5����������ȴ����ȼ�Сһ�� */
    }
    err = __csm300x_task_init ( "csm300_rx_task",
                                p_info->task_priority,
                                CSM_RX_TASK_STACK_SIZE,
                                __csm300x_rx_task,
                                &p_dev->can_serv);
    if (err != AW_OK) {
        goto __unavaliable;
    }

    err = __csm300x_task_init ( "csm300_tx_task",
                                (p_info->task_priority - 1),
                                CSM_TX_TASK_STACK_SIZE,
                                __csm300x_tx_task,
                                &p_dev->can_serv);
    if (err != AW_OK) {
        goto __unavaliable;
    }

    return AW_OK;

__unavaliable:

    AW_INFOF(("CAN%d device rx/tx task start up error!",p_info->servinfo.chn));
    return AW_ERROR;
}

/*****************************************************************************/
aw_local aw_can_err_t __csm300x_dev_cfg (struct csm300x_dev *p_dev)
{
    csm300x_dev_info_t          *p_info = __CSM300X_CAN_GET_INFO(p_dev);
    char                      ackmsg[5] = {0};
    uint8_t                         rty = 10;
    aw_err_t                        err = 0;
    if (p_info->xferinfo.workmode == UART_TO_CAN) {
        /**< brief UART����Ϊ9600������ */
        csm_uart_cfg (p_info->uartinfo.uartcom, 9600);
        /**< brief �豸����UART����ģʽ */
        __csm300x_mode_set (p_info, UART_CFG_MODE);
        while (rty) {
            /* ��մ��ڻ��� */
            aw_serial_ioctl(p_info->uartinfo.uartcom, AW_FIOFLUSH, NULL);
            /**< brief ���豸������������֡��Ϣ */
            csm_uart_write (p_info->uartinfo.uartcom,
                           (char *)&p_dev->p_reg,
                           sizeof(struct csm300x_config_info));
            /**< brief �ȴ�д��������Ļ�Ӧ֡��Ϣ */
            AW_INFOF(("wait ack msg...\r\n"));
            err = csm_uart_read (p_info->uartinfo.uartcom,
                                 &ackmsg,
                                 sizeof(ackmsg),
                                 200);
            if (err == AW_OK) {
                break;
            }
            rty--;
            aw_mdelay(100);
        }

        if (err != AW_OK) {
            AW_INFOF(("uart config csm300x timeout! line:%d\r\n",__LINE__));
            return AW_CAN_ERR_CTRL_NOT_START;/*������δ����*/
        }

        /*�жϽ��յ���������Ч��*/
        err = __buffer_xor_check(&ackmsg, sizeof(ackmsg));
        if (err != AW_OK) {
            AW_INFOF(("uart config csm300x recv error! line:%d\r\n",__LINE__));
            return AW_CAN_ERR_UNKNOWN_ERROR;/*δ֪����*/
        }

        /*�ж�д�������״̬*/
        if (ackmsg[3] == 0x13) {
            AW_INFOF(("uart config csm300x success! line:%d\r\n",__LINE__));
            /**< brief UART����Ϊ�����ļ������õĲ����� */
            csm_uart_cfg (p_info->uartinfo.uartcom, p_info->uartinfo.uartbaud);
            /**< brief ����������ģ���ƽ���л�����ģʽ */
            __csm300x_mode_set (p_info, UART_WORK_MODE);
            /**< brief ����ģ��ķ������������ */
            __csm300x_task_start(p_dev);
            return AW_CAN_ERR_NONE;
        } else if(ackmsg[3] == 0x07) {
            AW_INFOF(("uart config csm300x failed! line:%d\r\n",__LINE__));
            return AW_CAN_ERR_NOT_INITIALIZED;/*�豸δ��ʼ��*/
        } else {
            AW_INFOF(("uart config csm300x error! line:%d\r\n",__LINE__));
            return AW_CAN_ERR_UNKNOWN_ERROR;/*δ֪����*/
        }
    }
    /*SPI_TO_CAN MODE*/
    AW_INFOF(("not support work mode! line:%d\r\n",__LINE__));
    return AW_CAN_ERR_NONE;
}


/**
 * ��ʵ�ֵķ�������������
 */
/*****************************************************************************/
aw_local aw_can_err_t __csm300x_can_init (
        struct awbl_can_service *p_serv, aw_can_work_mode_type_t work_mode)
{
    return AW_CAN_ERR_NONE;
}

/*****************************************************************************/
aw_local aw_can_err_t __csm300x_can_baudrate_set (
        struct awbl_can_service *p_serv, const aw_can_baud_param_t *p_can_btr0)
{
    csm300x_dev_t               *p_dev  = __CSM300X_CAN_GET_THIS(p_serv);

    uint8_t                     tbaud = 0;
    AW_MUTEX_LOCK(__g_cfg_lock, AW_SEM_WAIT_FOREVER);

    uint32_t    baud = p_can_btr0->brp;
    switch (baud) {
        case 2:     baud = 1000000; break;
        case 5:     baud = 800000;  break;
        case 4:     baud = 500000;  break;
        case 8:     baud = 250000;  break;
        case 16:    baud = 125000;  break;
        case 20:    baud = 100000;  break;
        case 40:    baud = 50000;   break;
        case 100:   baud = 20000;   break;
        case 200:   baud = 10000;   break;
        default:
            AW_INFOF(("current chn not support this can baudrate\r\n"));
            return AW_CAN_ERR_INVALID_PARAMETER;
            break;
    }
    tbaud = __baud_to_hex (CAN_BAUD, baud);
    if (tbaud != 0) {
        p_dev->p_reg.canbaud = tbaud;
    } else {
        AW_INFOF(("can baud rate error\r\n"));
    }

    __get_config_info_xor (&p_dev->p_reg);
    __csm300x_dev_cfg (p_dev);
    AW_MUTEX_UNLOCK(__g_cfg_lock);
    return AW_CAN_ERR_NONE;
}

/*****************************************************************************/
aw_local aw_can_err_t __csm300x_can_baudrate_get (
        struct awbl_can_service *p_serv, aw_can_baud_param_t *p_can_btr0)
{
    csm300x_dev_t               *p_dev  = __CSM300X_CAN_GET_THIS(p_serv);

    uint8_t baud = p_dev->p_reg.canbaud;
    switch (baud) {
        case 0x02: *p_can_btr0 = CAN_BAUD_10K;break;
        case 0x03: *p_can_btr0 = CAN_BAUD_20K;break;
        case 0x05: *p_can_btr0 = CAN_BAUD_50K;break;
        case 0x07: *p_can_btr0 = CAN_BAUD_100K;break;
        case 0x08: *p_can_btr0 = CAN_BAUD_125K;break;
        case 0x0A: *p_can_btr0 = CAN_BAUD_250K;break;
        case 0x0C: *p_can_btr0 = CAN_BAUD_500K;break;
        case 0x0E: *p_can_btr0 = CAN_BAUD_800K;break;
        case 0x0F: *p_can_btr0 = CAN_BAUD_1000K;break;
        default:
            AW_INFOF(("can baudrate error!\r\n"));
            return AW_CAN_ERR_INVALID_PARAMETER;
    }
    return AW_CAN_ERR_NONE;
}


/*****************************************************************************/
aw_local aw_can_err_t __csm300x_can_reg_msg_write (
        struct awbl_can_service *p_serv, struct aw_can_msg *p_canmsg)
{
    csm300x_dev_t           *p_dev  = __CSM300X_CAN_GET_THIS(p_serv);
    csm300x_dev_info_t      *p_info = __CSM300X_CAN_GET_INFO(p_dev);
    aw_can_std_msg_t        *p_can_msg = (aw_can_std_msg_t *)p_canmsg;
    aw_err_t                 err = 0;

    serial_std_msg_t        serial_msg;
    memset( &serial_msg, 0, sizeof( serial_msg ));
    double        gap_time = 0;
    uint32_t      gaptime = 0;

    gap_time = ( p_info->uartinfo.uartgap * 10 );
    gap_time /= p_info->uartinfo.uartbaud;
    gap_time *= 1000000;                /*ת��Ϊus*/
    gaptime = gap_time;

    __can_to_uart_msg ( p_can_msg,
                        p_info,
                       &serial_msg );

    err = csm_uart_write ( p_info->uartinfo.uartcom,
                          &serial_msg.buf,
                           serial_msg.len );
    if (err != AW_OK) {
        return AW_CAN_ERR_UNKNOWN_ERROR;
    }
    aw_udelay ( gaptime );
    p_info->dev_sta |= TC_FLAGE;
    AW_SEMB_GIVE ( p_info->tx_semb );

    return AW_CAN_ERR_NONE;
}

/*****************************************************************************/
aw_local aw_can_err_t __csm300x_can_reg_msg_read (
        struct awbl_can_service *p_serv, struct aw_can_msg *p_canmsg)
{
    csm300x_dev_t           *p_dev  = __CSM300X_CAN_GET_THIS(p_serv);
    csm300x_dev_info_t      *p_info = __CSM300X_CAN_GET_INFO(p_dev);

    struct aw_can_std_msg *p_can_msg = (struct aw_can_std_msg *)p_canmsg;

    p_can_msg->can_msg.flags = p_info->p_can_msg.can_msg.flags;
    p_can_msg->can_msg.id = p_info->p_can_msg.can_msg.id;
    p_can_msg->can_msg.length = p_info->p_can_msg.can_msg.length;

    memcpy(&p_can_msg->msgbuff,
           &p_info->p_can_msg.msgbuff,
            p_info->p_can_msg.can_msg.length);

    return AW_CAN_ERR_NONE;
}

/*****************************************************************************/
aw_local aw_can_err_t __csm300x_can_int_status_get (
        struct awbl_can_service *p_serv,
        awbl_can_int_type_t     *p_int_type,
        aw_can_bus_err_t        *p_bus_err)
{
    csm300x_dev_t           *p_dev  = __CSM300X_CAN_GET_THIS(p_serv);
    csm300x_dev_info_t      *p_info = __CSM300X_CAN_GET_INFO(p_dev);

    if (p_info->dev_sta & TC_FLAGE) {
        *p_int_type |= AWBL_CAN_INT_TX;
        p_info->dev_sta &= ~TC_FLAGE;
    } else {
        *p_int_type &= ~AWBL_CAN_INT_TX;
    }

    if (p_info->dev_sta & RC_FLAGE) {
        *p_int_type |= AWBL_CAN_INT_RX;
        p_info->dev_sta &= ~RC_FLAGE;
    } else {
        *p_int_type &= ~AWBL_CAN_INT_RX;
    }

    *p_bus_err  = AW_CAN_BUS_ERR_NONE;
    return AW_CAN_ERR_NONE;
}



/**
 * δʵ�ֵķ�������������
 */
/******************************************************************************/
aw_local aw_can_err_t __csm300x_can_start (struct awbl_can_service *p_serv)
{
    return AW_CAN_ERR_NONE;
}

/******************************************************************************/
aw_local aw_can_err_t __csm300x_can_errreg_cnt_get (
        struct awbl_can_service *p_serv, aw_can_err_cnt_t *p_can_err_reg)
{
    return AW_CAN_ERR_NONE;
}

/******************************************************************************/
aw_local aw_can_err_t __csm300x_can_errreg_cnt_clr (
        struct awbl_can_service *p_serv)
{
    return AW_CAN_ERR_NONE;
}

/******************************************************************************/
aw_local aw_can_err_t __csm300x_can_set_haltfrz (
        struct awbl_can_service *p_serv)
{
    return AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_local aw_can_err_t __csm300x_can_sleep (struct awbl_can_service *p_serv)
{
    return AW_CAN_ERR_NONE_DRIVER_FUNC;
}

/******************************************************************************/
aw_local aw_can_err_t __csm300x_can_wakeup (struct awbl_can_service *p_serv)
{
    return AW_CAN_ERR_NONE_DRIVER_FUNC;
}


/******************************************************************************/
aw_local aw_can_err_t __csm300x_can_int_enable (
        struct awbl_can_service *p_serv, awbl_can_int_type_t int_mask)
{
    return AW_CAN_ERR_NONE;
}

/******************************************************************************/
aw_local aw_can_err_t __csm300x_can_int_disable (
        struct awbl_can_service *p_serv, awbl_can_int_type_t int_mask)
{
    return AW_CAN_ERR_NONE;
}

/******************************************************************************/
aw_local aw_can_err_t __csm300x_can_msg_snd_stop (
        struct awbl_can_service *p_serv)
{
    return AW_CAN_ERR_NONE;
}

/******************************************************************************/
aw_local aw_can_err_t __csm300x_can_filter_table_set (
        struct awbl_can_service *p_serv, uint8_t *p_filterbuff, size_t length)
{
    return AW_CAN_ERR_NONE;
}

/******************************************************************************/
aw_local aw_can_err_t __csm300x_can_filter_table_get (
        struct awbl_can_service *p_serv,
        uint8_t                 *p_filterbuff,
        size_t                  *p_length)
{
    return AW_CAN_ERR_NONE;
}

/******************************************************************************/
aw_local aw_can_err_t __csm300x_can_reg_write (
        struct awbl_can_service *p_serv,
        uint32_t                 offset,
        uint8_t                 *p_data,
        size_t                   length)
{
    return AW_CAN_ERR_NONE;
}

/******************************************************************************/
aw_local aw_can_err_t __csm300x_can_reg_read (
        struct awbl_can_service *p_serv,
        uint32_t                 offset,
        uint8_t                 *p_data,
        size_t                   length)
{
    return AW_CAN_ERR_NONE;
}



/******************************************************************************/
/**
 * ��׼can�豸��������
 */
struct awbl_can_drv_funcs __g_csm300x_servfuncs = {
    __csm300x_can_init,             /**< \brief ��ʼ����غ���  1. pfn_can_init �� */
    __csm300x_can_start,            /**< \brief CAN����         2. pfn_can_start �� */
    __csm300x_can_set_haltfrz,      /**< \brief CANֹͣ         3. pfn_can_stop �� */
    __csm300x_can_sleep,            /**< \brief CAN����         4. pfn_can_sleep*/
    __csm300x_can_wakeup,           /**< \brief CAN����         5. pfn_can_wakeup*/
    __csm300x_can_int_enable,       /**< \brief �ж�ʹ�� ������ 6. pfn_can_int_enable �� */
    __csm300x_can_int_disable,      /**< \brief �ж�ʹ�� ������ 7. pfn_can_int_disable �� */
    __csm300x_can_baudrate_set,     /**< \brief ����������      8. pfn_can_baud_set �� */
    __csm300x_can_baudrate_get,     /**< \brief �����ʻ�ȡ      9. pfn_can_baud_get �� */
    __csm300x_can_errreg_cnt_get,   /**< \brief ��ȡ�������    10. pfn_can_err_cnt_get */
    __csm300x_can_errreg_cnt_clr,   /**< \brief ����������    11. pfn_can_err_cnt_clr */
    __csm300x_can_reg_msg_read,     /**< \brief CAN�շ���غ��� 12. pfn_can_reg_msg_read �� */
    __csm300x_can_reg_msg_write,    /**< \brief CAN�շ���غ��� 13. pfn_can_reg_msg_write �� */
    __csm300x_can_msg_snd_stop,     /**< \brief CAN�շ���غ��� 14. pfn_can_msg_snd_stop �� */
    __csm300x_can_filter_table_set, /**< \brief �����˲���غ��� 15. pfn_can_filter_table_set �� */
    __csm300x_can_filter_table_get, /**< \brief �����˲���غ��� 16. pfn_can_filter_table_get �� */
    __csm300x_can_int_status_get,   /**< \brief CAN bus���״̬��ȡ 17. pfn_can_int_status_get �� */
    __csm300x_can_reg_write,        /**< \brief �����Ĵ��������ӿڣ������CAN�������õ� */
    __csm300x_can_reg_read,         /**< \brief �����Ĵ��������ӿڣ������CAN�������õ� */
};


/******************************************************************************/
aw_local void awbl_csm300x_inst_init1 (struct awbl_dev *p_dev)
{
    return;
}

/******************************************************************************/
aw_local void awbl_csm300x_inst_init2 (struct awbl_dev *p_dev)
{
    AW_MUTEX_INIT(__g_cfg_lock, AW_SEM_Q_PRIORITY);

    csm300x_dev_t *p_this = __CSM300X_CAN_GET_DEV(p_dev);
    csm300x_dev_info_t *p_info = __CSM300X_CAN_GET_INFO(p_dev);

    AW_SEMB_INIT(p_info->tx_semb, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    if (p_info->hwinfo.pfun_plfm_init != NULL) {
        p_info->hwinfo.pfun_plfm_init();
    }

    __csm300x_config_info_init (&p_this->p_reg, p_info);

    awbl_can_service_init(&(p_this->can_serv),
                          &(p_info->servinfo),
                          &(__g_csm300x_servfuncs));

    awbl_can_service_register(&p_this->can_serv);

    return;
}

/******************************************************************************/
aw_local void awbl_csm300x_inst_connect (struct awbl_dev *p_dev)
{
    return;
}


/******************************************************************************/
aw_local aw_const struct awbl_drvfuncs __g_csm300x_drvfuncs = {
    awbl_csm300x_inst_init1,
    awbl_csm300x_inst_init2,
    awbl_csm300x_inst_connect
};


/******************************************************************************/
/*
 * csm300x�豸����ע����Ϣ
 */
aw_local aw_const struct awbl_drvinfo __g_csm300x_drv_registration = {
    AWBL_VER_1,
    AWBL_BUSID_PLB,
    AWBL_CSM300X_NAME,
    &__g_csm300x_drvfuncs,
    NULL,
    NULL
};


/******************************************************************************/
/**
 * ע���豸����
 */
void awbl_csm300x_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_csm300x_drv_registration);
}

/* end of file*/
