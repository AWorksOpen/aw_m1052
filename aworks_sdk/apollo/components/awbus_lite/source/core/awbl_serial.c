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
 * \brief ����ͨѶ��׼�ӿ�
 *
 * \internal
 * \par modification history
 * - 1.00 12-10-30  orz, first implementation
 * - 1.00 15-06-20  deo, add aw_serdev_remove
 * -      18-10-15  hsg, add aw_serdev operate instead of aw_tydev operate
 * \endinternal
 */
#include "aw_common.h"
#include "aw_serial.h"
#include "driver/serial/awbl_serial_private.h"
#include "awbus_lite.h"
#include "string.h" /* for memset() */
#include "aw_assert.h"
#include "aw_sem.h"
#include "aw_errno.h"
#include "aw_int.h"

/** \brief ��ʱ�ٶ�ϵͳֻ��10������ */
#define     AW_NUM_COM      10

/** \brief  �����豸�ṹ����ͷ */
static struct aw_list_head __g_awbl_serdev_head;

static struct awbl_serial *__gp_serial[AW_NUM_COM] = {NULL};

#define com_to_serial(com) (__gp_serial[com])

/**
 * \brief ���ڳ�ʼ������
 *
 * \note  �ڵ�һ�׶γ�ʼ��֮ǰ����
 */
void aw_serial_init (void)
{
    aw_list_head_init(&__g_awbl_serdev_head);               /* ��ʼ������ */
    memset(__gp_serial, 0, sizeof(__gp_serial[AW_NUM_COM]));
}


/**
 * \brief �����豸����
 *
 * \param[in]   p_ser       �����豸ָ��
 * \param[in]   p_recv      ���ջ�������ַ
 * \param[in]   recv_size   ���ջ�������С
 * \param[in]   p_send      ���ͻ�������ַ
 * \param[in]   send_size   ���ͻ�������С
 * \param[in]   no          �豸��
 * \param[in]   p_funcs     �����ṩ�ķ���
 *
 *
 * \return  AW_OK   �����ɹ�
 * \return  -AW_EINVAL ����ʧ�ܣ���������
 * \return  -EEXIST ����ʧ�ܣ��豸�Ѵ���
 */
aw_err_t awbl_serial_create (struct awbl_serial      *p_ser,
                             char                    *p_recv_buf,
                             uint32_t                 recv_size,
                             int                      no,
                             struct aw_sio_chan      *p_sio_chan)
{
    struct awbl_serdev *p_serdev;

    if ((p_ser      == NULL)  ||
        (p_recv_buf == NULL)  ||
        ((no >= AW_NUM_COM) || (no < 0)) ||
        (p_sio_chan == NULL)) {
        return -AW_EINVAL;
    }

    /* �жϸ��豸���Ƿ��Ѵ��� */
    if (com_to_serial(no) == NULL) {
        com_to_serial(no) = p_ser;
    } else {    /* ���豸���Ѵ��� */
        return -AW_EEXIST;
    }
    p_serdev = &p_ser->serdev;

    /* ��ʼ��������  */
    aw_rngbuf_init(&p_serdev->rdbuf, p_recv_buf, recv_size);

    aw_rngbuf_flush(&p_serdev->rdbuf);

    AW_MUTEX_INIT(p_serdev->wr_mutex_sem, AW_SEM_Q_PRIORITY  |
                                          AW_SEM_DELETE_SAFE |
                                          AW_SEM_INVERSION_SAFE);

    AW_MUTEX_INIT(p_serdev->rd_mutex_sem, AW_SEM_Q_PRIORITY  |
                                          AW_SEM_DELETE_SAFE |
                                          AW_SEM_INVERSION_SAFE);

    p_serdev->rd_timeout = AW_SEM_WAIT_FOREVER;
    p_serdev->rd_interval_timeout = AW_WAIT_FOREVER;
    AW_SEMB_INIT(p_serdev->rdsync_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    p_ser->no = no;
    p_ser->sio_chan = *p_sio_chan;

    /* �����豸 */
    aw_list_add(&p_serdev->nod, &__g_awbl_serdev_head);

    return AW_OK;
}


/**
 * \brief �������յ����ݺ��򴮿��ϱ�
 *
 * \param[in]   p_ser       �����豸ָ��
 * \param[in]   inchar      ����
 *
 * \return ��
 */
void awbl_serial_rx (struct awbl_serial *p_ser, const char inchar)
{
    aw_rngbuf_putchar(&p_ser->serdev.rdbuf, inchar);
}


/**
 * \brief ��������idle�жϺ��򴮿��ϱ��������Ѿ����һ��ͻ������
 */
void awbl_serial_int_idle (struct awbl_serial *p_ser)
{
    AW_SEMB_GIVE(p_ser->serdev.rdsync_sem);
}


/**
 * \brief �жϽ��ջ�����Ϊ��
 *
 * \return 1 ���ջ���Ϊ��
 * \return 0 ���ջ���ǿ�
 *
 */
int awbl_serial_recv_is_empty(struct awbl_serial *p_ser)
{
    return aw_rngbuf_isempty(&p_ser->serdev.rdbuf);
}


/**
 * \brief ��ȡ������Ŀ��пռ��С
 */
uint32_t awbl_serial_recv_freebytes(struct awbl_serial *p_ser)
{
    return aw_rngbuf_freebytes(&p_ser->serdev.rdbuf);
}


aw_local void __update_dcb_from_options(int                    options, 
                                        struct aw_serial_dcb  *p_dcb)
{
    /* λ������  */
    switch (options & CSIZE) {
    case CS5:
        p_dcb->byte_size = 5;
        break;
    case CS6:
        p_dcb->byte_size = 6;
        break;
    case CS7:
        p_dcb->byte_size = 7;
        break;
    case CS8:
        p_dcb->byte_size = 8;
        break;
    default:
        /* don't go into there */
        break;
    }

    /* ֹͣλ����  */
    if (options & STOPONE5) {
        p_dcb->stop_bits = AW_SERIAL_ONE5STOPTS;
    } else if (options & STOPB) {
        p_dcb->stop_bits = AW_SERIAL_TWOSTOPBITS;
    } else {
        p_dcb->stop_bits = AW_SERIAL_ONESTOPBIT;
    }

    /* ��������  */
    if(options & PARENB) {
        p_dcb->f_parity = 1;

        /* Parity Odd/Even */
        if(options & PARODD) {
            p_dcb->parity = AW_SERIAL_ODDPARITY;
        } else {
            p_dcb->parity = AW_SERIAL_EVENPARITY;
        }
    } else {
        p_dcb->f_parity = 0;
    }

    /* ��������  */
    if (!(options & CLOCAL)) {
        p_dcb->f_rtsctrl = AW_SERIAL_RTS_HANDSHAKE;
        p_dcb->f_ctsflow = 1;
    } else {
        p_dcb->f_rtsctrl = AW_SERIAL_RTS_DISABLE;
        p_dcb->f_ctsflow = 0;
    }
}


aw_local void __update_options_from_dcb(int *p_opt, struct aw_serial_dcb *p_dcb)
{
    *p_opt = 0;

    /* λ������  */
    switch (p_dcb->byte_size) {
    case 5:
        *p_opt |= CS5;
        break;
    case 6:
        *p_opt |= CS6;
        break;
    case 7:
        *p_opt |= CS7;
        break;
    case 8:
        *p_opt |= CS8;
        break;
    default:
        *p_opt |= CS8;
        break;
    }

    /* ֹͣλ����  */
    if (p_dcb->stop_bits == AW_SERIAL_ONE5STOPTS) {
        *p_opt |= STOPONE5;
    } else if (p_dcb->stop_bits == AW_SERIAL_TWOSTOPBITS) {
        *p_opt |= STOPB;
    } else {
        *p_opt &= ~STOPB;
        *p_opt &= ~STOPONE5;
    }

    /* ��������  */
    if(p_dcb->f_parity) {
        *p_opt |= PARENB;
        if(p_dcb->parity == AW_SERIAL_EVENPARITY) {
            *p_opt &= ~PARODD;
        } else {
            *p_opt |= PARODD;
        }
    } else {
        *p_opt &= ~PARENB;
    }

    /* 
     *ע��, ���ͨ��dcb��������,ͨ��ioctl��ȡ,
     *��iotct��ȡ����ֵ��dcb���õ�ֵ���ܲ�һ��
     */
    if ((p_dcb->f_rtsctrl == AW_SERIAL_RTS_HANDSHAKE) &&
        (p_dcb->f_ctsflow )) {
        *p_opt &= ~CLOCAL;
    } else {
        *p_opt |= CLOCAL;
    }
}


/******************************************************************************/
aw_err_t aw_serial_ioctl (uint32_t com, int request, void *p_arg)
{
    aw_err_t          err = AW_OK;
    struct awbl_serial *p_ser;
    struct awbl_serdev *p_serdev;
    struct aw_serial_dcb  dcb;
    int    options = 0;

    if (AW_INT_CONTEXT()) {
        return -AW_ENOTSUP;
    }

    if (com >= AW_NUM_COM) {
        return -AW_EINVAL;
    }

    p_ser = com_to_serial(com);
    if (NULL == p_ser) {
        return -AW_ENODEV;
    }
    p_serdev = &p_ser->serdev;

    /* ��ȡ��д�� */
    AW_MUTEX_LOCK(p_serdev->wr_mutex_sem, AW_SEM_WAIT_FOREVER);
    AW_MUTEX_LOCK(p_serdev->rd_mutex_sem, AW_SEM_WAIT_FOREVER);

    switch (request)
    {

    case SIO_BAUD_SET:
        err = aw_serial_dcb_get(com, &dcb);
        if (err) {
            goto __end;
        }

        dcb.baud_rate = (uint32_t)p_arg;
        err = aw_serial_dcb_set(com, &dcb);
        if (err) {
            goto __end;
        }
        break;

    case SIO_BAUD_GET:
        err = aw_serial_dcb_get(com, &dcb);
        if (err) {
            goto __end;
        }
        *(int *)p_arg = dcb.baud_rate;
        break;

    case SIO_MODE_SET:
        if ((int)p_arg != SIO_MODE_INT) {
            err = -AW_ENOTSUP;
            goto __end;
        }
        break;
    case SIO_MODE_GET:
        *(int *)p_arg = SIO_MODE_INT;
        break;
    case SIO_AVAIL_MODES_GET:
        *(int *)p_arg = SIO_MODE_INT;
        break;
    case SIO_HW_OPTS_SET:
        options = (int)p_arg;
        err = aw_serial_dcb_get(com, &dcb);
        if (err) {
           goto __end;
        }

        __update_dcb_from_options(options, &dcb);
        err = aw_serial_dcb_set(com, &dcb);
        if (err) {
            goto __end;
        }

        break;
    case SIO_HW_OPTS_GET:
        err = aw_serial_dcb_get(com, &dcb);
        if (err) {
            goto __end;
        }

        __update_options_from_dcb((int*)p_arg, &dcb);
        break;

    case AW_TIOCRDTIMEOUT:      /* ���ö���ʱ */
        if ((int)p_arg <= 0) {
            p_serdev->rd_timeout = (int)p_arg;
        } else {
            p_serdev->rd_timeout = aw_ms_to_ticks((int)p_arg);
        }
        break;

    case AW_TIOISATTY:          /* �ж��Ƿ�ΪTTY�豸 */
        err = AW_FALSE;
        break;

    case AW_FIOFLUSH:
    case AW_FIOSYNC:
    case AW_FIORFLUSH:
        aw_rngbuf_flush(&p_serdev->rdbuf);
        aw_sio_recv_decrease(p_ser->sio_chan,
                             aw_rngbuf_freebytes(&p_serdev->rdbuf));

        AW_SEMB_RESET(p_serdev->rdsync_sem, 0);

        break;

    case AW_FIONREAD:
        *((int *)p_arg) = aw_rngbuf_nbytes(&p_serdev->rdbuf);
        break;

    default:
        err = -AW_EINVAL;
        break;
    }

__end:
    AW_MUTEX_UNLOCK(p_serdev->rd_mutex_sem);
    AW_MUTEX_UNLOCK(p_serdev->wr_mutex_sem);

    return err;
}

/******************************************************************************/
int aw_serial_write (uint32_t com, const void *p_buffer, size_t nbytes)
{
    struct awbl_serial *p_ser;
    struct awbl_serdev *p_serdev;
    size_t              idx      = 0;

    if (AW_INT_CONTEXT()) {
        return -AW_ENOTSUP;
    }

    if ((com >= AW_NUM_COM) || (p_buffer == NULL) || (nbytes == 0)) {
        return -AW_EINVAL;
    }
    p_ser = com_to_serial(com);
    if (NULL == p_ser) {
        return -AW_ENODEV;
    }

    if ((int)nbytes < 0) {
        return -AW_E2BIG;
    }

    p_serdev = &p_ser->serdev;

    AW_MUTEX_LOCK(p_serdev->wr_mutex_sem, AW_SEM_WAIT_FOREVER);

    idx = aw_sio_tx(p_ser->sio_chan, (aw_const uint8_t *)p_buffer, nbytes);

    AW_MUTEX_UNLOCK(p_serdev->wr_mutex_sem);

    return idx;
}


/******************************************************************************/
int aw_serial_read (uint32_t com, void *p_buf, size_t maxbytes)
{
    struct awbl_serial *p_ser    = NULL;
    struct awbl_serdev *p_serdev = NULL;
    uint32_t  timeout = 0;
    size_t    idx     = 0;
    uint32_t  len     = 0;
    uint8_t  *p_buffer;

    if (AW_INT_CONTEXT()) {
        return -AW_ENOTSUP;
    }

    if ((com >= AW_NUM_COM) || (p_buf == NULL)) {
        return -AW_EINVAL;
    }
    p_ser = com_to_serial(com);
    if (NULL == p_ser) {
        return -AW_ENODEV;
    }

    if ((int)maxbytes < 0) {
        return -AW_E2BIG;
    }

    p_serdev = &p_ser->serdev;
    p_buffer = (uint8_t *)p_buf;
    AW_MUTEX_LOCK(p_serdev->rd_mutex_sem, AW_WAIT_FOREVER);

    timeout = p_ser->serdev.rd_timeout;     /* ���õȴ���1���ֽڵĳ�ʱʱ�� */
    while (maxbytes) {

        if (AW_SEMB_TAKE(p_serdev->rdsync_sem, timeout) != AW_OK) {

            /* ��䳬ʱ���ܳ�ʱ����һ����  */
            AW_MUTEX_UNLOCK(p_serdev->rd_mutex_sem);
            return idx;
        }

        len      =  aw_rngbuf_get(&p_serdev->rdbuf, &p_buffer[idx], maxbytes);
        idx      += len;
        maxbytes -= len;

        /*
         * ����Ƿ�ֹ���������������128���ֽڣ����ǽӿ�ֻ����64����
         * ��ôʣ�µ�ҲҪ�ͷ��ź����������´ν���ӿ��ܹ�ֱ�Ӵӻ������ж�,
         * ���ź���,ֱ�����ա�
         */
        if (!aw_rngbuf_isempty(&p_serdev->rdbuf)) {
            AW_SEMB_GIVE(p_serdev->rdsync_sem);
        }

        /* ������һ�εĳ�ʱΪ��䳬ʱ */
        timeout = p_ser->serdev.rd_interval_timeout;
    }

    aw_sio_recv_decrease(p_ser->sio_chan, aw_rngbuf_freebytes(&p_serdev->rdbuf));

    AW_MUTEX_UNLOCK(p_serdev->rd_mutex_sem);

    return idx;
}


/******************************************************************************/
int aw_serial_poll_write (uint32_t com, const void *p_buffer, size_t nbytes)
{
    struct awbl_serial     *p_ser;
    size_t                  len;
    uint8_t                 ch;
    aw_const uint8_t       *p_buf_uint8;

    if (!AW_FAULT_CONTEXT()) {
        return 0;
    }

    if ((com >= AW_NUM_COM) || (p_buffer == NULL)) {
        return -AW_EINVAL;
    }
    p_ser = com_to_serial(com);
    if (NULL == p_ser) {
        return -AW_ENODEV;
    }

    if ((int)nbytes < 0) {
        return -AW_E2BIG;
    }

    p_buf_uint8 = (aw_const uint8_t *)p_buffer;
    len = nbytes;

    while (len-- > 0) {
        ch = *(p_buf_uint8 ++);
        while (-AW_EAGAIN == aw_sio_poll_output(p_ser->sio_chan, ch)) {
        }
    }

    return nbytes;
}


/******************************************************************************/
aw_err_t aw_serial_dcb_set (uint32_t com, const struct aw_serial_dcb *p_dcb)
{
    aw_err_t err;
    struct awbl_serial *p_ser;
    struct awbl_serdev *p_serdev;

    if (AW_INT_CONTEXT()) {
        return -AW_ENOTSUP;
    }

    if ((com >= AW_NUM_COM) || (p_dcb == NULL)) {
        return -AW_EINVAL;
    }

    p_ser = com_to_serial(com);
    if (NULL == p_ser) {
        return -AW_ENODEV;
    }
    p_serdev = &p_ser->serdev;

    /* ��ȡ��д�� */
    AW_MUTEX_LOCK(p_serdev->wr_mutex_sem, AW_SEM_WAIT_FOREVER);
    AW_MUTEX_LOCK(p_serdev->rd_mutex_sem, AW_SEM_WAIT_FOREVER);

    err = aw_sio_dcb_set(p_ser->sio_chan, p_dcb);

    AW_MUTEX_UNLOCK(p_serdev->rd_mutex_sem);
    AW_MUTEX_UNLOCK(p_serdev->wr_mutex_sem);

    return err;
}


/******************************************************************************/
aw_err_t aw_serial_dcb_get (uint32_t com, struct aw_serial_dcb *p_dcb)
{
    aw_err_t err;
    struct awbl_serial *p_ser;
    struct awbl_serdev *p_serdev;

    if (AW_INT_CONTEXT()) {
        return -AW_ENOTSUP;
    }

    if ((com >= AW_NUM_COM) || (p_dcb == NULL)) {
        return -AW_EINVAL;
    }

    p_ser = com_to_serial(com);
    if (NULL == p_ser) {
        return -AW_ENODEV;
    }
    p_serdev = &p_ser->serdev;

    /* ��ȡ��д�� */
    AW_MUTEX_LOCK(p_serdev->wr_mutex_sem, AW_SEM_WAIT_FOREVER);
    AW_MUTEX_LOCK(p_serdev->rd_mutex_sem, AW_SEM_WAIT_FOREVER);

    err = aw_sio_dcb_get(p_ser->sio_chan, p_dcb);

    AW_MUTEX_UNLOCK(p_serdev->rd_mutex_sem);
    AW_MUTEX_UNLOCK(p_serdev->wr_mutex_sem);

    return err;
}


/* ����ӵ�һ���ӿ�, �ṩ���û����ó�ʱʱ��Ľӿ�  */
aw_err_t aw_serial_timeout_set(uint32_t com, struct aw_serial_timeout *p_cfg)
{
    struct awbl_serial *p_ser;
    struct awbl_serdev *p_serdev;

    if (AW_INT_CONTEXT()) {
        return -AW_ENOTSUP;
    }

    if ((com >= AW_NUM_COM) || (p_cfg == NULL)) {
        return -AW_EINVAL;
    }

    p_ser = com_to_serial(com);
    if (NULL == p_ser) {
        return -AW_ENODEV;
    }
    p_serdev = &p_ser->serdev;

    /* ��ȡ��д�� */
    AW_MUTEX_LOCK(p_serdev->wr_mutex_sem, AW_SEM_WAIT_FOREVER);
    AW_MUTEX_LOCK(p_serdev->rd_mutex_sem, AW_SEM_WAIT_FOREVER);

    p_serdev->rd_timeout          = p_cfg->rd_timeout;
    p_serdev->rd_interval_timeout = p_cfg->rd_interval_timeout;

    AW_MUTEX_UNLOCK(p_serdev->rd_mutex_sem);
    AW_MUTEX_UNLOCK(p_serdev->wr_mutex_sem);

    return AW_OK;
}


aw_err_t aw_serial_timeout_get(uint32_t com, struct aw_serial_timeout *p_cfg)
{

    struct awbl_serial *p_ser;
    struct awbl_serdev *p_serdev;

    if (AW_INT_CONTEXT()) {
        return -AW_ENOTSUP;
    }

    if ((com >= AW_NUM_COM) || (p_cfg == NULL)) {
        return -AW_EINVAL;
    }

    p_ser = com_to_serial(com);
    if (NULL == p_ser) {
        return -AW_ENODEV;
    }
    p_serdev = &p_ser->serdev;

    /* ��ȡ��д�� */
    AW_MUTEX_LOCK(p_serdev->wr_mutex_sem, AW_SEM_WAIT_FOREVER);
    AW_MUTEX_LOCK(p_serdev->rd_mutex_sem, AW_SEM_WAIT_FOREVER);

    p_cfg->rd_timeout          = p_serdev->rd_timeout;
    p_cfg->rd_interval_timeout = p_serdev->rd_interval_timeout;

    AW_MUTEX_UNLOCK(p_serdev->rd_mutex_sem);
    AW_MUTEX_UNLOCK(p_serdev->wr_mutex_sem);

    return AW_OK;
}

/** @} */

/* end of file */
