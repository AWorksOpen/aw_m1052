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
 * \brief National Semiconductor 16550 compatible UART driver
 *
 * \internal
 * \par modification history
 * - 2.00 20-02-12  deo, refactoring (to support new serial)
 * - 1.01 12-11-01  orz, fix __ns16550_tx_startup(), as it does not
 *                  trigger a THRE interrupt on some chips like lpc11xx
 * - 1.00 12-10-26  orz, written from vxbNs16550Sio.c
 * \endinternal
 */
#include "aworks.h"
#include "awbus_lite.h"
#include "driver/serial/awbl_ns16550.h"
#include "awbl_sio.h"
#include "driver/serial/awbl_serial_private.h"
#include "aw_int.h"
#include "aw_serial.h"

#ifndef ENDL
    #define ENDL "\r\n"
#endif

#ifdef NS16550_DBG
#define __NS16550_DBG(fmt, args...) \
              if (fmt) { \
                  printf(fmt ENDL, ##args); \
              }
#else
#define __NS16550_DBG(fmt, args...)
#endif

#define __FILED_RUN(c, run, fmt, args...) \
    do { \
        if (!(c)) { \
            __NS16550_DBG(fmt, ##args); \
            run; \
        } \
    } while (0);


/* Register offsets from base address */
#define __RBR     0x00    /**< \brief receiver buffer register */
#define __THR     0x00    /**< \brief transmit holding register */
#define __DLL     0x00    /**< \brief divisor latch */
#define __IER     0x01    /**< \brief interrupt enable register */
#define __DLM     0x01    /**< \brief divisor latch(MS) */
#define __IIR     0x02    /**< \brief interrupt identification register */
#define __FCR     0x02    /**< \brief FIFO control register */
#define __LCR     0x03    /**< \brief line control register */
#define __MCR     0x04    /**< \brief modem control register */
#define __LSR     0x05    /**< \brief line status register */
#define __MSR     0x06    /**< \brief modem status register */
#define __SCR     0x07    /**< \brief scratch register */

/* interrupt enable register */
#define __IER_EDSSI           (1 << 3)
#define __IER_ELSI            (1 << 2)
#define __IER_ETBEI           (1 << 1)
#define __IER_ERBI            (1 << 0)

/* FIFO control register */
#define __FCR_RXFIFTL(n)    ((n & 0x3) << 6)
    #define __RXFIFTL_1B    0
    #define __RXFIFTL_4B    1
    #define __RXFIFTL_8B    2
    #define __RXFIFTL_14B   3
#define __FCR_DMAMODE1      (1 << 3)
#define __FCR_TXCLR         (1 << 2)
#define __FCR_RXCLR         (1 << 1)
#define __FCR_FIFOEN        (1 << 0)

/* line control register */
#define __LCR_DLAB    (1 << 7)
#define __LCR_BC      (1 << 6)
#define __LCR_SP      (1 << 5)
#define __LCR_EPS     (1 << 4)
#define __LCR_PEN     (1 << 3)
#define __LCR_STB     (1 << 2)
#define __LCR_WLS(n)  ((((n) - 5) & 0x3) << 0)

/* modem control register */
#define __MCR_DTR     (1 << 0)
#define __MCR_RTS     (1 << 1)
#define __MCR_LOOP    (1 << 4)

/* interrupt identification register */
#define __IIR_INT_CODE  0x0F
    #define __INT_MODEM 0x00
    #define __INT_THRE  0x02
    #define __INT_RDA   0x04
    #define __INT_RLS   0x06
    #define __INT_CTI   0x0C

/* modem status register */
#define __MSR_DCTS (1 << 0)
#define __MSR_DDSR (1 << 1)
#define __MSR_TERI (1 << 2)
#define __MSR_DCD  (1 << 3)
#define __MSR_CTS  (1 << 4)
#define __MSR_DSR  (1 << 5)
#define __MSR_RI   (1 << 6)
#define __MSR_CD   (1 << 7)

/* line status register */
#define __LSR_RXFIFOE    (1 << 7)
#define __LSR_TEMT       (1 << 6)
#define __LSR_THRE       (1 << 5)
#define __LSR_BI         (1 << 4)
#define __LSR_FE         (1 << 3)
#define __LSR_PE         (1 << 2)
#define __LSR_OE         (1 << 1)
#define __LSR_DR         (1 << 0)


#define __SIO_TO_DEV(sio) \
            AW_CONTAINER_OF(AW_CONTAINER_OF(sio, struct awbl_serial, sio_chan), \
                            awbl_ns16550_dev_t, \
                            serial);




/* imx10xx uart drv  functions definitions */
aw_local aw_err_t __sio_tx (
        struct aw_sio_chan *p_sio,
        const uint8_t *p_buffer,
        size_t nbytes);

aw_local aw_err_t __sio_poll_output (
        struct aw_sio_chan *p_sio,
        uint8_t outchar);

aw_local int __sio_dcb_set (
        struct aw_sio_chan *p_sio,
        const struct aw_serial_dcb *p_dcb);

aw_local int __sio_dcb_get (
        struct aw_sio_chan *p_sio,
        struct aw_serial_dcb *p_dcb);

/*
 * 添加这个函数用于处理软件流控，如果需要的话
 */
aw_local int __sio_recv_decrease (
        struct aw_sio_chan *p_sio,
        const int freebytes)
{
    return AW_OK;
}



aw_local void __ns16550_inst_init (awbl_dev_t *p_awdev);
aw_local void __ns16550_inst_init2 (awbl_dev_t *p_awdev);
aw_local void __ns16550_inst_connect (awbl_dev_t *p_awdev);
aw_local aw_bool_t __ns16550_probe (awbl_dev_t *p_awdev);

aw_local aw_const struct aw_serial_dcb __g_default_dcb = {
        8,                      /* n bits with data */
        0,                      /* enable parity */
        AW_SERIAL_EVENPARITY,   /* parity type */
        AW_SERIAL_ONESTOPBIT,   /* stop bits */
        0,                      /* CTS flow */
        AW_SERIAL_RTS_DISABLE,  /* RTS control */
        0,                      /* sensitivity */
        0,                      /* DSR flow */
        AW_SERIAL_DTR_DISABLE,  /* DTR control */
        0,                      /* XON/XOFF for TX */
        0,                      /* XON/XOFF for RX */
        0,                      /* f_dummy */
        115200,                 /* baud rate */
        0,                      /* xon_lim */
        0,                      /* xoff_lim */
        0x11,                   /* xon_char */
        0x13,                   /* xoff_char */
        AW_FALSE,
};

/** \brief sio driver functions */
aw_local aw_const struct aw_sio_drv_funcs __g_ns16550_sio_drvfuncs = {
    __sio_tx,
    __sio_poll_output,
    __sio_dcb_set,
    __sio_dcb_get,
    __sio_recv_decrease
};

aw_const struct awbl_drvfuncs __g_ns16550_drvfuncs = {
    __ns16550_inst_init,             /* devInstanceInit */
    __ns16550_inst_init2,           /* devInstanceInit2 */
    __ns16550_inst_connect      /* devConnect */
};

aw_local aw_const struct awbl_drvinfo __g_ns16550_drv_registration = {
    AWBL_VER_1,                             /* awb_ver */
    AWBL_BUSID_PLB,                      /* bus_id */
    "ns16550",                                 /* p_drvname */
    &__g_ns16550_drvfuncs,               /* p_busfuncs */
    NULL,                                        /* p_methods */
    __ns16550_probe                                         /* pfunc_drv_probe */
};


/******************************************************************************/
void awbl_ns16550_drv_register (void)
{
    awbl_drv_register(&__g_ns16550_drv_registration);
}

/******************************************************************************/
aw_local aw_inline void __reg_wr (awbl_ns16550_dev_t *p_dev,
                                  uint8_t             reg,
                                  uint8_t             val)
{
    awbl_ns16550_info_t *p_info;

    p_info = (awbl_ns16550_info_t *)AWBL_DEVINFO_GET(&p_dev->awdev);
    writeb(val, (volatile void __iomem *)(p_info->regbase + (reg << p_info->regsize)));
}

/******************************************************************************/
aw_local aw_inline uint8_t __reg_rd (awbl_ns16550_dev_t  *p_dev, uint8_t reg)
{
    awbl_ns16550_info_t *p_info;
    p_info = (awbl_ns16550_info_t *)AWBL_DEVINFO_GET(&p_dev->awdev);
    return readb((volatile void __iomem *)(p_info->regbase + (reg << p_info->regsize)));
}

/******************************************************************************/
aw_local void __baud_rate_set (awbl_ns16550_dev_t *p_dev, uint32_t baud)
{
    awbl_ns16550_info_t *p_info = (awbl_ns16550_info_t *)AWBL_DEVINFO_GET(&p_dev->awdev);
    uint16_t             div = baud;

    div = p_info->pfn_divisor(baud);

    /* select divisor register */
    __reg_wr(p_dev, __LCR, __LCR_DLAB);
    /* set baud rate */
    __reg_wr(p_dev, __DLL, (uint8_t)div);
    __reg_wr(p_dev, __DLM, (uint8_t)(div >> 8));
    /* restore line control register */
    __reg_wr(p_dev, __LCR, p_dev->lcr);
}

/******************************************************************************/
aw_local void __control_line_set (awbl_ns16550_dev_t         *p_dev,
                                  const struct aw_serial_dcb *p_dcb)
{
    int i;
    /* word length */
    p_dev->lcr   = __LCR_WLS(p_dcb->byte_size);
    for (i = 0, p_dev->bmask = 0; i < p_dcb->byte_size; i++) {
        p_dev->bmask = (p_dev->bmask << 1) | 0x01;
    }

    /* stop bit */
    if (p_dcb->stop_bits != AW_SERIAL_ONESTOPBIT) {
        p_dev->lcr |= __LCR_STB;
    }

    /* enable parity */
    if (p_dcb->f_parity) {
        p_dev->lcr |= __LCR_PEN;
    }

    /* select even parity */
    if (p_dcb->parity == AW_SERIAL_EVENPARITY) {
        p_dev->lcr |= __LCR_EPS;
    }

    /* set control line */
    __reg_wr(p_dev, __LCR, p_dev->lcr);
}

/******************************************************************************/
aw_local void __modem_set (awbl_ns16550_dev_t         *p_dev,
                           const struct aw_serial_dcb *p_dcb)
{
    p_dev->mcr  = 0;

    if (p_dcb->f_dtrctrl == AW_SERIAL_DTR_ENABLE) {
        p_dev->mcr |= __MCR_DTR;
    }

    if (p_dcb->f_rtsctrl == AW_SERIAL_RTS_ENABLE) {
        p_dev->mcr |= __MCR_RTS;
    }

    /* set modem configure */
    __reg_wr(p_dev, __MCR, p_dev->mcr);
}

/******************************************************************************/
aw_local aw_err_t __dcd_param_check (const struct aw_serial_dcb *p_dcb)
{
    /* check word length */
    if ((p_dcb->byte_size < 5) || (p_dcb->byte_size > 8)) {
        return -AW_EINVAL;
    }

    /* when 5bits length, not support 2 stop bits */
    if ((p_dcb->byte_size == 5) &&
        ((p_dcb->stop_bits != AW_SERIAL_ONESTOPBIT) &&
         (p_dcb->stop_bits != AW_SERIAL_ONE5STOPTS))) {
        return -AW_EINVAL;
    }

    /* when 6/7/8 bits length, not support 1.5 stop bits */
    if ((p_dcb->byte_size > 5) &&
        ((p_dcb->stop_bits != AW_SERIAL_ONESTOPBIT) &&
         (p_dcb->stop_bits != AW_SERIAL_TWOSTOPBITS))) {
        return -AW_EINVAL;
    }

    /* check RTS */
    if ((p_dcb->f_rtsctrl != AW_SERIAL_RTS_HANDSHAKE) &&
        (p_dcb->f_rtsctrl != AW_SERIAL_RTS_DISABLE) &&
        (p_dcb->f_rtsctrl != AW_SERIAL_RTS_ENABLE)) {
        return -AW_EINVAL;
    }

    /* check DTR */
    if ((p_dcb->f_dtrctrl != AW_SERIAL_DTR_HANDSHAKE) &&
        (p_dcb->f_dtrctrl != AW_SERIAL_DTR_DISABLE) &&
        (p_dcb->f_dtrctrl != AW_SERIAL_DTR_ENABLE)) {
        return -AW_EINVAL;
    }

    /* check XON/XOFF */
    if ((p_dcb->f_outx) || (p_dcb->f_inx)) {
        return -AW_ENOTSUP;
    }

    /* 不支持dsr_sensitivity */
    if (p_dcb->f_dsrsensitivity) {
        return -AW_ENOTSUP;
    }

    /* 不支持data-set-ready */
    if (p_dcb->f_dsrflow) {
        return -AW_ENOTSUP;
    }

    return AW_OK;
}

/******************************************************************************/
aw_local int __sio_dcb_set (struct aw_sio_chan         *p_sio,
                            const struct aw_serial_dcb *p_dcb)
{
    aw_err_t            ret;
    awbl_ns16550_dev_t *p_dev;

    p_dev = __SIO_TO_DEV(p_sio);

    ret = __dcd_param_check(p_dcb);
    __FILED_RUN(ret == AW_OK, return ret, "invalid DCB.");

    /* disable all interrupt */
    p_dev->ier = 0;
    __reg_wr(p_dev, __IER, p_dev->ier);

    /* set baud rate */
    __baud_rate_set(p_dev, p_dcb->baud_rate);

    /* set control line */
    __control_line_set(p_dev, p_dcb);

    /* set modem mode */
    __modem_set(p_dev, p_dcb);

    /* enable interrupt */
    p_dev->ier = __IER_ELSI | __IER_ETBEI | __IER_ERBI;
    if (p_dcb->f_dtrctrl || p_dcb->f_rtsctrl) {
        p_dev->ier |= __IER_EDSSI;
    }
    __reg_wr(p_dev, __IER, p_dev->ier);

    p_dev->serial.dcb = *p_dcb;

    return AW_OK;
}

/******************************************************************************/
aw_local aw_err_t __sio_tx (struct aw_sio_chan *p_sio,
                            const uint8_t      *p_buffer,
                            size_t              nbytes)
{
    uint8_t               lsr;
    int                   tmp = 0, len = nbytes;
    awbl_ns16550_dev_t   *p_dev;
    awbl_ns16550_info_t  *p_info;
    int                   err_cnt = 0;

    p_dev  = __SIO_TO_DEV(p_sio);
    p_info = (awbl_ns16550_info_t *)AWBL_DEVINFO_GET(&p_dev->awdev);

    while (len) {
        lsr = __reg_rd(p_dev, __LSR);
        if (lsr & __LSR_THRE) {
            tmp  = min(len, p_info->fifosize);
            len -= tmp;
            while (tmp--) {
                __reg_wr(p_dev, __THR, (char)*p_buffer);
                p_buffer++;
            }
        } else {
            err_cnt += 1;
            if (err_cnt > 10) {
                return -AW_EAGAIN;
            }
        }
        AW_SEMB_TAKE(p_dev->semb, 10);
    }

    return nbytes;
}

/******************************************************************************/
aw_local aw_err_t __sio_poll_output (struct aw_sio_chan *p_sio,
                                     uint8_t             outchar)
{
    awbl_ns16550_dev_t   *p_dev;

    p_dev = __SIO_TO_DEV(p_sio);

    if (__reg_rd(p_dev, __LSR) & __LSR_THRE) {
        __reg_wr(p_dev, __THR, outchar);
        return AW_OK;
    } else {
        return -AW_EAGAIN;
    }
}

/******************************************************************************/
aw_local int __sio_dcb_get (struct aw_sio_chan   *p_sio,
                            struct aw_serial_dcb *p_dcb)
{
    awbl_ns16550_dev_t   *p_dev;

    p_dev  = __SIO_TO_DEV(p_sio);
    memcpy(p_dcb, &p_dev->serial.dcb, sizeof(p_dev->serial.dcb));

    return AW_OK;
}

/******************************************************************************/
aw_local void __ns16550_interrupt (void *arg)
{
    awbl_ns16550_dev_t *p_dev = (awbl_ns16550_dev_t *)arg;
    uint8_t             int_code;
    uint8_t             reg, ch;
    uint8_t             err_cnt = 0;

    int_code = __reg_rd(p_dev, __IIR) & __IIR_INT_CODE;

    /* disable all interrupt */
    __reg_wr(p_dev, __IER, 0);

    switch (int_code) {
    case __INT_MODEM:
        reg = __reg_rd(p_dev, __MSR);

        if (reg & __MSR_DCTS) {
            if (reg & __MSR_CTS) {
                p_dev->ier |= __IER_ETBEI;
            } else {
                p_dev->ier &= ~__IER_ETBEI;
            }
        }

        if (reg & __MSR_DDSR) {
            if (reg & __MSR_DSR) {//TODO check buffer
            } else {
            }
        }
        break;

    case __INT_THRE:
        AW_SEMB_GIVE(p_dev->semb);
        break;

    case __INT_RDA:
    case __INT_CTI:
        reg = 0;
        do {
            if (reg & __LSR_DR) {
                ch = __reg_rd(p_dev, __RBR) & p_dev->bmask;
                awbl_serial_rx(&p_dev->serial, (char)ch);
            }
            reg = __reg_rd(p_dev, __LSR);
        } while (reg & __LSR_DR);
        awbl_serial_int_idle(&p_dev->serial);
        break;

    case __INT_RLS:
        while ((__reg_rd(p_dev, __LSR) & (__LSR_BI | __LSR_FE | __LSR_PE | __LSR_OE)) &&
                err_cnt < 10) {
            if (__reg_rd(p_dev, __LSR) & __LSR_DR) {
                ch = __reg_rd(p_dev, __RBR);
            }
            err_cnt += 1;
        }

        /* configure FIFO */
        __reg_wr(p_dev, __FCR,
                __FCR_RXFIFTL(__RXFIFTL_1B) |   /* 1bytes to trigger interrupt todo */
                __FCR_DMAMODE1 |                /* enable DMA */
                __FCR_RXCLR |                   /* clear receiver FIFO */
                __FCR_FIFOEN);                  /* enable FIFO */
        break;
    default:break;
    }

    /* restore interrupt */
    __reg_wr(p_dev, __IER, p_dev->ier);
}

/******************************************************************************/
aw_local aw_bool_t __ns16550_probe (awbl_dev_t *p_awdev)
{
    awbl_ns16550_info_t  *p_info;

    p_info = (awbl_ns16550_info_t *)AWBL_DEVINFO_GET(p_awdev);

    __FILED_RUN(p_info->pfn_divisor != NULL, return AW_FALSE, "");
    __FILED_RUN(p_info->p_rxbuf != NULL, return AW_FALSE, "");
    __FILED_RUN(p_info->bufsize > 0, return AW_FALSE, "");

    return AW_TRUE;
}

/******************************************************************************/
aw_local void __ns16550_inst_init (awbl_dev_t *p_awdev)
{
    aw_err_t                    ret;
    awbl_ns16550_dev_t         *p_dev;
    awbl_ns16550_info_t        *p_info;
    struct aw_sio_chan          sio_chan;
    
    p_dev  = AW_CONTAINER_OF(p_awdev, awbl_ns16550_dev_t, awdev);
    p_info = (awbl_ns16550_info_t *)AWBL_DEVINFO_GET(p_awdev);


    /* Call platform initialize function first */
    if (p_info->pfn_plfm_init) {
        p_info->pfn_plfm_init();
    }

    /* disable all interrupt */
    __reg_wr(p_dev, __IER, 0);

    /* configure FIFO */
    __reg_wr(p_dev, __FCR,
            __FCR_RXFIFTL(__RXFIFTL_1B) |   /* 1bytes to trigger interrupt todo */
            __FCR_DMAMODE1 |                /* enable DMA */
            __FCR_TXCLR |                   /* clear transmitter FIFO */
            __FCR_RXCLR |                   /* clear receiver FIFO */
            __FCR_FIFOEN);                  /* enable FIFO */

    sio_chan.p_drv_funcs = &__g_ns16550_sio_drvfuncs;

    AW_SEMB_INIT(p_dev->semb, 0, AW_SEM_Q_PRIORITY);

    /* create serial device */
    ret = awbl_serial_create(&p_dev->serial,
                             (char *)p_info->p_rxbuf,
                             p_info->bufsize,
                             p_awdev->p_devhcf->unit,
                             &sio_chan);
    __FILED_RUN(ret == AW_OK, return, "ns16550 serial create failed(%d).", ret);

    /* enable rs485 receive */
    if (p_info->pfn_rs485_dir) {
        p_info->pfn_rs485_dir(NS16550_RS485_RX_MODE);
    }

    if (p_info->init_cfg) {
        __sio_dcb_set(&p_dev->serial.sio_chan, p_info->init_cfg);
    } else {
        __sio_dcb_set(&p_dev->serial.sio_chan, &__g_default_dcb);
    }

    aw_int_connect(p_info->inum,(aw_pfuncvoid_t)__ns16550_interrupt, p_dev);
    aw_int_enable(p_info->inum);
}

/******************************************************************************/
aw_local void __ns16550_inst_init2 (awbl_dev_t *p_awdev)
{
    return;
}

/******************************************************************************/
aw_local void __ns16550_inst_connect (awbl_dev_t *p_awdev)
{
    return;
}


/* end of file */
