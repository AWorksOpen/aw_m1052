/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief IMX10XX UART Driver
 *
 * \internal
 * \par modification history:
 * - 1.00 2017-09-30  sni, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_clk.h"
#include "awbus_lite.h"
#include "aw_common.h"
#include "aw_int.h"
#include "awbl_sio.h"
#include "aw_serial.h"
#include "aw_assert.h"
#include "aw_bitops.h"
#include "aw_spinlock.h"
#include "aw_delay.h"
#include "string.h"
#include "driver/serial/awbl_imx10xx_lpuart.h"
#include "driver/serial/imx10xx_lpuart_regs.h"



#define __UART_GET_CHAN_FROM_DEV(p_dev) \
        &(((struct awbl_imx10xx_uart_dev *)p_dev)->chan)

#define __UART_GET_DEV_INFO_FROM_DEV(p_dev) \
        ((struct awbl_imx10xx_lpuart_dev_info *)AWBL_DEVINFO_GET(p_dev))

#define __UART_GET_DEV_FROM_CHAN(p_chan) \
    &AW_CONTAINER_OF(p_chan, struct awbl_imx10xx_uart_dev, chan)->dev;

#define __IMX10XX_RS485_TX_MODE           0x01
#define __IMX10XX_RS485_RX_MODE           0x00

#define __IMX10XX_FIFO_SIZE               4

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __uart_inst_init1 (struct awbl_dev *p_dev);
aw_local void __uart_inst_init2 (struct awbl_dev *p_dev);
aw_local void __uart_inst_connect (struct awbl_dev *p_dev);


/* imx10xx uart drv  functions definitions */
aw_local aw_err_t __uart_tx (
        struct aw_sio_chan *p_siochan,
        const uint8_t *p_buffer,
        size_t nbytes);

aw_local aw_err_t __uart_poll_output (
        struct aw_sio_chan *p_chan,
        uint8_t outchar);

aw_local int __uart_dcb_set (
        struct aw_sio_chan *p_sio_chan,
        const struct aw_serial_dcb *p_dcb);

aw_local int __uart_dcb_get (
        struct aw_sio_chan *p_sio_chan,
        struct aw_serial_dcb *p_dcb);

/* imx10xx uart drv  functions definitions end */
aw_local void __uart_interrupt (struct awbl_imx10xx_uart_dev *p_uart_dev);

/*
 * 添加这个函数用于处理软件流控，如果需要的话
 */
aw_local int __uart_recv_decrease (
        struct aw_sio_chan *p_siochan,
        const int freebytes)
{
    return AW_OK;
}

/** \brief sio driver functions */
aw_local aw_const struct aw_sio_drv_funcs __g_imx10xx_lpuart_sio_drv_funcs = {
    __uart_tx,
    __uart_poll_output,
    __uart_dcb_set,
    __uart_dcb_get,
    __uart_recv_decrease
};

/*******************************************************************************
    globals
*******************************************************************************/

/*******************************************************************************
    locals
*******************************************************************************/
static const uint32_t __g_typical_baudrate[] = {
    110,
    300,
    600,
    1200,
    2400,
    4800,
    9600,
    14400,
    19200,
    38400,
    43000,
    56000,
    57600,
    76800,
    115200,
    128000,
    230400,
    256000,
    460800,
    600000,
    750000,
    921600,
};

static const int __g_num_typical_baudrate = AW_NELEMENTS(__g_typical_baudrate);

static int uart_baudrate_config(
        rt105x_lpuart_regs_t *p_reg,
        uint32_t fclk,
        uint32_t baud_rate)
{
    uint32_t        osr,sbr,i;
    uint32_t        osr_temp,sbr_temp,temp_diff,baud_diff,cal_baud;

    baud_diff = baud_rate;
    osr = 0;
    sbr = 0;

    /* 最大理论支持baud_reate = fclk/ (1 x 4) */
    if (baud_rate > (fclk / 4)) {
        return -AW_EINVAL;
    }

    for (osr_temp = 32;osr_temp >= 4;osr_temp --) {
        /* calculate the temporary sbr value   */
        sbr_temp = (fclk / (baud_rate * osr_temp));
        if (0 == sbr_temp) {
            sbr_temp = 1;
        }
        for(i = 0;i<2;i++) {
            sbr_temp = sbr_temp + i;
            cal_baud = fclk / (sbr_temp * osr_temp);
            temp_diff = baud_rate > cal_baud?baud_rate-cal_baud:cal_baud - baud_rate;
            if (temp_diff < baud_diff) {
                osr = osr_temp;
                sbr = sbr_temp;
                baud_diff = temp_diff;
            }
        }
    }

    if (sbr > 8191) {
        return -AW_EINVAL;
    }

    osr_temp = readl(&p_reg->BAUD);
    if ((osr > 3) && (osr < 8))
    {
        osr_temp |= (1<<17);
    }

    osr_temp &= ~(0x1F << 24);
    osr_temp |= (osr -1) << 24;
    osr_temp &= ~(0x1FFF << 0);
    osr_temp |= sbr << 0;

    // 禁用10 bit模式
    osr_temp &= ~(1 << 29);
    writel( osr_temp,&p_reg->BAUD);

    return AW_OK;
}


aw_local int __dcd_param_check(const struct aw_serial_dcb *p_dcb)
{
    int      i;

    /* 检查配置参数, 只支持7,8位宽*/
    if ((p_dcb->byte_size < 7) || (p_dcb->byte_size > 8)) {
        return -AW_EINVAL;          /* 无效参数 */
    }

    /* 检查波特率  */
    if ((p_dcb->baud_rate >= __g_typical_baudrate[0]) &&
        (p_dcb->baud_rate <= __g_typical_baudrate[__g_num_typical_baudrate - 1])) {
        for (i = 0; i < __g_num_typical_baudrate; i++) {
            if (p_dcb->baud_rate == __g_typical_baudrate[i]) {
                break;
            }
        }

        /* 在标准范围, 非标准波特率  */
        if (i >= __g_num_typical_baudrate) {

            /* 超过标准范围的波特率  */
            return -AW_EINVAL;      /* 无效参数 */
        }
    } else if (p_dcb->baud_rate < __g_typical_baudrate[0]) {
        /* 不在标准范围  */
        return -AW_EINVAL;          /* 无效参数 */
    }

    /* 判断是否有错误的停止位  */
    if (p_dcb->stop_bits % 4 == AW_SERIAL_STOP_INVALID) {
        return -AW_EINVAL;          /* 无效参数 */
    }

    /* 不支持1.5 位停止位   */
    if (p_dcb->stop_bits == AW_SERIAL_ONE5STOPTS) {
        return -AW_ENOTSUP;
    }

    /* 检测RTS 是否是无效位  */
    if (p_dcb->f_rtsctrl % 4 == AW_SERIAL_RTS_INVALID) {
        return -AW_EINVAL;          /* 无效参数 */
    }


    /* 检测DTR */
    if (p_dcb->f_dtrctrl % 4 == AW_SERIAL_DTR_INVALID) {
        return -AW_EINVAL;          /* 无效参数 */
    }

    /* 检查软件流控 */
    if ((p_dcb->f_outx) || (p_dcb->f_inx)) {
        return -AW_ENOTSUP;    /* 不支持软件流控 */
    }

    if (p_dcb->f_dsrsensitivity) {
        return -AW_ENOTSUP;    /* 不支持dsrsensitivity */
    }

    if (p_dcb->f_dsrflow) {
        return -AW_ENOTSUP;
    }

    return AW_OK;
}

aw_local int __uart_dcb_set (
        struct aw_sio_chan *p_sio_chan,
        const struct aw_serial_dcb *p_dcb)
{
    aw_err_t                            err = AW_OK;
    awbl_imx10xx_lpuart_dev_info_t     *p_dev_info;
    rt105x_lpuart_regs_t               *p_reg;
    struct awbl_serial                 *p_serial;
    uint32_t                            temp;
    awbl_imx10xx_uart_dev_t            *p_uart_dev;

    p_serial = AW_CONTAINER_OF(p_sio_chan,
                               struct awbl_serial,
                                sio_chan);
    p_uart_dev   = AW_CONTAINER_OF(p_serial,
                awbl_imx10xx_uart_dev_t,
                serial);
    p_dev_info = __UART_GET_DEV_INFO_FROM_DEV(p_uart_dev);
    p_reg       = (rt105x_lpuart_regs_t *)p_dev_info->regbase;

    /* dcb参数检测  */
    err = __dcd_param_check(p_dcb);

    if (err) {
        return err;
    }

    // 首先我们停止发送器和接收器
    AW_REG_BIT_CLR32(&p_reg->CTRL,19);
    while( AW_REG_BIT_GET32(&p_reg->CTRL,19) );
    AW_REG_BIT_CLR32(&p_reg->CTRL,18);
    while( AW_REG_BIT_GET32(&p_reg->CTRL,18) );
    // 复位整个LPUART
    // First we must reset the lpuart
    AW_REG_BIT_SET32(&p_reg->GLOBAL,1);
    aw_udelay(100);
    AW_REG_BIT_CLR32(&p_reg->GLOBAL,1);
    // 因为LPUART发送接收都已经禁用，从此刻开始应该不会再产生中断了


    // 设置波特率
    temp = aw_clk_rate_get(p_dev_info->async_parent_clk_id);
    err = uart_baudrate_config(p_reg,temp,p_dcb->baud_rate);

    if (err) {
        return err;
    }

    temp = 0;
    // 配置数据位
    if (7 == p_dcb->byte_size) {
        // 7位数据位
        if (p_dcb->f_parity) {
            // 且使能校验，使用8bit模式
        }
        else {
            // 不使能校验，使用7bit模式
            temp |= (1 << 11);
        }
    }
    else {
        // 8为数据位
        if (p_dcb->f_parity) {
            // 且使能校验，使用9bit模式
            temp |= (1 << 4);
        }
        else {
            // 不使能校验，使用8bit模式
        }
    }

    // 配置校验
    if (p_dcb->f_parity) {
        temp |= (1 << 1);
        if (AW_SERIAL_ODDPARITY == p_dcb->parity) {
            temp |= (1 << 0);
        }
    }

    // 使能接收，IDLE和各种错误中断
    temp |= (1 << 20)       // IDLE
            | AW_BIT(21)    // 接收
            | AW_BIT(24)    // 校验错误
            | AW_BIT(25)    // 帧错误
            | AW_BIT(26)    // Noise ERROR
            | AW_BIT(27)    // Overrun ERROR
            ;
    // IDLE字符从stop信号开始计数
    temp |= (1 << 2);
    // IDLE中断需要4个IDLE字符
    temp |= (0x2 << 8);
    writel(temp,&p_reg->CTRL);
    // 停止位
    temp = readl(&p_reg->BAUD);
    if (AW_SERIAL_TWOSTOPBITS == p_dcb->stop_bits) {
        temp |= AW_BIT(13);
    }
    else {
        temp &= (~(AW_BIT(13)));
    }
    writel(temp,&p_reg->BAUD);


    if (p_dcb->f_fast_reaction) {

        /* 关闭发送和接收  */
        AW_REG_BIT_CLR32(&p_reg->CTRL, 18);
        AW_REG_BIT_CLR32(&p_reg->CTRL, 19);

        /* 关闭接收FIFO */
        writel(((1<<7)|(1<<15)), &p_reg->FIFO);
        writel((2<<0), &p_reg->WATER);

    } else {
        // 流控
        // 使能发送和接收FIFO
        writel(((1<<3)|(1<<7)), &p_reg->FIFO);
        writel(((1<<3)|(1<<7)|(1<<14)|(1<<15)), &p_reg->FIFO);
        // 配置WATERMARK;
        writel(((1<<0)|(1<<16)), &p_reg->WATER);
    }

    memcpy(&p_serial->dcb,p_dcb,sizeof(*p_dcb));

    // 使能发送和接收,此刻才会有中断到来
    AW_REG_BIT_SET32(&p_reg->CTRL,19);
    AW_REG_BIT_SET32(&p_reg->CTRL,18);

    return err;
}

aw_local int __uart_dcb_get (struct aw_sio_chan   *p_sio_chan,
                                    struct aw_serial_dcb *p_dcb)
{
    struct awbl_serial *p_serial;

    p_serial = AW_CONTAINER_OF(p_sio_chan,
                               struct awbl_serial,
                                sio_chan);
    memcpy(p_dcb,&p_serial->dcb,sizeof(p_serial->dcb));

    return AW_OK;
}

static struct aw_serial_dcb aw_const __g_default_dcb = {
        8,                      //byte_size
        0,                      //f_parity
        AW_SERIAL_EVENPARITY,   //parity
        AW_SERIAL_ONESTOPBIT,   //stop_bits
        0,                      //f_ctsflow
        AW_SERIAL_RTS_DISABLE,  //f_rtsctrl
        0,                      //f_dsrsensitivity
        0,                      //f_dsrflow
        AW_SERIAL_DTR_DISABLE,  //f_dtrctrl
        0,                      //f_outx
        0,                      //f_inx
        0,                      //f_dummy
        115200,                 //baud_rate
        0,                      //xon_lim
        0,                      //xoff_lim
        0x11,                   //xon_char
        0x13,                   //xoff_char
        AW_FALSE,
};

/**
 * \brief initialize imx10xx auart device
 *
 * This is the imx10xx auart initialization routine.
 *
 * \note
 */
aw_local void __uart_inst_init1 (struct awbl_dev *p_dev)
{
    struct aw_serial_dcb            temp_dcb;
    int                             ret = AW_OK;
    struct aw_sio_chan              sio_chan;
    awbl_imx10xx_uart_dev_t        *p_uart_dev;
    awbl_imx10xx_lpuart_dev_info_t *p_dev_info;

    p_uart_dev = AW_CONTAINER_OF(p_dev,
                awbl_imx10xx_uart_dev_t,
                dev);

    /* get the channel parameter */
    p_dev_info  = __UART_GET_DEV_INFO_FROM_DEV(p_uart_dev);

    /** 参数判断 */
    if ((p_dev_info->recv_buf == NULL)) {
        while (1);
    }

    /* Call platform initialize function first */
    if (NULL != p_dev_info->pfunc_plfm_init) {
        p_dev_info->pfunc_plfm_init();
    }

    /* initialize the driver function pointers in the channel */
    sio_chan.p_drv_funcs = &__g_imx10xx_lpuart_sio_drv_funcs;   /** to up user */

    /* Initialize uart DCB */
    memset(&p_uart_dev->serial.dcb, 0, sizeof(struct aw_serial_dcb));
    memcpy(&temp_dcb,&__g_default_dcb,sizeof(struct aw_serial_dcb));
    temp_dcb.baud_rate = p_dev_info->init_baudrate;

    /* initialize hardware*/
    AW_SEMB_INIT(p_uart_dev->w_semb, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
    AW_SEMB_INIT(p_uart_dev->w_485semb, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    /* 创建串口设备 */
    ret = awbl_serial_create(&p_uart_dev->serial,
                            p_dev_info->recv_buf,
                            p_dev_info->recv_size,
                            p_uart_dev->dev.p_devhcf->unit,
                            &sio_chan);

    if (ret != AW_OK) { /* 设备创建失败  */
        while (1);
    }

    //RS485
    if (NULL != p_dev_info->pfunc_rs485_dir) {
        p_dev_info->pfunc_rs485_dir(__IMX10XX_RS485_RX_MODE);
    }

    __uart_dcb_set(&p_uart_dev->serial.sio_chan,&temp_dcb);
    aw_int_connect(p_dev_info->inum,(aw_pfuncvoid_t)__uart_interrupt,p_uart_dev);
    aw_int_enable(p_dev_info->inum);

}

/**
 * \brief initialize imx10xx auart device
 */
aw_local void __uart_inst_init2 (struct awbl_dev *p_dev)
{

}

/**
 * \brief connect imx10xx auart device to system
 */
aw_local void __uart_inst_connect (struct awbl_dev *p_dev)
{

}


/**
 * \brief deferred handle a receiver interrupt
 *
 * This routine handles read interrupts from the AUART.
 */
aw_local void __uart_int_rd (
        awbl_imx10xx_uart_dev_t *p_uart_dev,
        rt105x_lpuart_regs_t *p_reg
        )
{
    uint32_t                        temp;
    register uint32_t               mask = 0xFF;

    /* get the channel parameter */
    temp = readl(&p_reg->CTRL);

    // 此刻对byte_size的访问应该是安全的，
    // 参见__uart_dcb_set,唯一会改变dcb结构的函数，改变dcb结构的
    // 过程中lpuart不会发生中断
    if (7 == p_uart_dev->serial.dcb.byte_size) {
        mask = 0x7F;
    }


    while (!AW_REG_BIT_ISSET32(&p_reg->FIFO, 22)) {
        temp = readl(&p_reg->DATA);
        if ( !(temp & (0x7<<13) ) ) {
            temp &= mask;
            awbl_serial_rx(&p_uart_dev->serial, (char)temp);
        }

    }

}

/**
 * \brief deferred handle a transmitter interrupt
 *
 * This routine handles transmitter interrupts from the AUART.
 */
aw_local void __uart_int_wr(
        awbl_imx10xx_uart_dev_t *p_uart_dev,
        rt105x_lpuart_regs_t *p_reg)
{
    uint32_t                        temp;

    temp = readl(&p_reg->CTRL);
    temp &= (~(1 << 23));
    writel(temp,&p_reg->CTRL);
    AW_SEMB_GIVE(p_uart_dev->w_semb);
}

/**
 * \brief interrupt level processing
 *
 * This routine handles four sources of interrupts from the UART. They are
 * prioritized in the following order by the Interrupt Identification Register:
 * Receiver Line Status, Received Data Ready, Transmit Holding Register Empty
 * and Modem Status.
 *
 * When a modem status interrupt occurs, the transmit interrupt is enabled if
 * the CTS signal is TRUE.
 */
aw_local void __uart_interrupt (struct awbl_imx10xx_uart_dev *p_uart_dev)
{
    register uint32_t                       sr;
    uint32_t                                fifo;
    struct awbl_imx10xx_lpuart_dev_info    *p_this_dvif;
    rt105x_lpuart_regs_t                   *p_reg;
    uint32_t                                ctrl;

    p_this_dvif = __UART_GET_DEV_INFO_FROM_DEV(p_uart_dev);
    p_reg = (rt105x_lpuart_regs_t *)p_this_dvif->regbase;

    sr = readl(&p_reg->STAT);
    fifo = readl(&p_reg->FIFO);
    ctrl = readl(&p_reg->CTRL);

    if ((sr & (0x1 << 21)) || (sr & (0x1 << 20))) {
        __uart_int_rd(p_uart_dev,p_reg);
        awbl_serial_int_idle(&p_uart_dev->serial);
    }

    if ((sr & (0x1 << 23))) {
        __uart_int_wr(p_uart_dev,p_reg);
    }

    if ( (ctrl & AW_BIT(22)) &&
            (sr & (0x1 << 22))) {
        AW_SEMB_GIVE(p_uart_dev->w_485semb);
        ctrl &= ~(AW_BIT(22));
        writel(ctrl,&p_reg->CTRL);
    }

    writel(fifo,&p_reg->FIFO);
    writel(sr,&p_reg->STAT);
}


aw_local aw_err_t __uart_tx (struct aw_sio_chan *p_siochan,
                                    const uint8_t *p_buffer ,
                                    size_t nbytes)
{
    struct awbl_serial             *p_serial;
    rt105x_lpuart_regs_t           *p_reg;
    awbl_imx10xx_lpuart_dev_info_t *p_dev_info;
    awbl_imx10xx_uart_dev_t        *p_uart_dev;
    uint32_t                        index  = 0;
    uint32_t                        fifo_count;

    p_serial = AW_CONTAINER_OF(p_siochan,
                               struct awbl_serial,
                                sio_chan);
    p_uart_dev   = AW_CONTAINER_OF(p_serial,
                awbl_imx10xx_uart_dev_t,
                serial);

    /* get the channel parameter */
    p_dev_info = __UART_GET_DEV_INFO_FROM_DEV(&p_uart_dev->dev);
    p_reg = (rt105x_lpuart_regs_t *)p_dev_info->regbase;

    /* rs485 开启  */
    if (p_dev_info->pfunc_rs485_dir) {
        p_dev_info->pfunc_rs485_dir(__IMX10XX_RS485_TX_MODE);
    }

    while (index < nbytes) {
        fifo_count = AW_REG_BITS_GET32(&p_reg->WATER,8,3);
        if (fifo_count < 4) {
            writel(p_buffer[index], &p_reg->DATA);
            index ++ ;
        }
        else {
            /* 打开发送中断 */
            AW_REG_BIT_SET32(&p_reg->CTRL, 23);
            /* 有信号量证明发送FIFO中有空间  */
            AW_SEMB_TAKE(p_uart_dev->w_semb, AW_WAIT_FOREVER);
        }
    }

    /* 处理485  */
    if (p_dev_info->pfunc_rs485_dir) {

        /* 由于不是关闭的nvic的中断,所以可能出现多触发一次中断的情况,这里在打开完成中断之前先将信号量给重置 */
        AW_SEMB_RESET(p_uart_dev->w_485semb, 0);

        /* 打开发送完成中断, 只要打开发送完成中断,即使FIFO中没有数据也会触发中断  */
        AW_REG_BIT_SET32(&p_reg->CTRL, 22);

        /* 等待485 发送完成   */
        AW_SEMB_TAKE(p_uart_dev->w_485semb, AW_WAIT_FOREVER);
        p_dev_info->pfunc_rs485_dir(__IMX10XX_RS485_RX_MODE);

    }

    return nbytes;
}


/**
 * \brief output a character in polled mode
 *
 * This routine transmits a character in polled mode.
 *
 * \return AW_OK if a character sent, -AW_EAGAIN if the output buffer is full
 */
aw_local aw_err_t __uart_poll_output (
        struct aw_sio_chan *p_sio_chan,
        uint8_t outchar)
{
    struct awbl_serial             *p_serial;
    rt105x_lpuart_regs_t           *p_reg;
    awbl_imx10xx_lpuart_dev_info_t *p_dev_info;
    awbl_imx10xx_uart_dev_t        *p_uart_dev;
    uint32_t                        fifo_count;
    uint32_t                        temp;
    aw_err_t                        ret = AW_OK;

    p_serial = AW_CONTAINER_OF(p_sio_chan,
                               struct awbl_serial,
                                sio_chan);
    p_uart_dev   = AW_CONTAINER_OF(p_serial,
                awbl_imx10xx_uart_dev_t,
                serial);

    /* get the channel parameter */
    p_dev_info = __UART_GET_DEV_INFO_FROM_DEV(&p_uart_dev->dev);
    p_reg = (rt105x_lpuart_regs_t *)p_dev_info->regbase;

    fifo_count = AW_REG_BITS_GET32(&p_reg->WATER,8,3);
    if (fifo_count < 4) {
        temp = (uint32_t)outchar;
        writel(temp, &p_reg->DATA);
    }
    else {
        ret = -AW_EAGAIN;
    }

    return ret;
}

aw_const struct awbl_drvfuncs g_uart_awbl_drvfuncs = {
    __uart_inst_init1,        /* devInstanceInit */
    __uart_inst_init2,       /* devInstanceInit2 */
    __uart_inst_connect      /* devConnect */
};



/**
 * driver registration (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvinfo __g_uart_drv_registration = {
    AWBL_VER_1,                         /* awb_ver */
    AWBL_BUSID_PLB,                     /* bus_id */
    AWBL_IMX10XX_LPUART_DRV_NAME,       /* p_drvname */
    &g_uart_awbl_drvfuncs,              /* p_busfuncs */
    NULL,                               /* p_methods */
    NULL                                /* pfunc_drv_probe */
};

/**
 * \brief register imx10xx uart driver
 *
 * This routine registers the imx10xx uart driver and device recognition
 * data with the AWBus subsystem.
 *
 * NOTE: This routine is called early during system initialization, and
 * *MUST NOT* make calls to OS facilities such as memory allocation
 * and I/O.
 */
void awbl_imx10xx_lpuart_drv_register (void)
{
    awbl_drv_register(&__g_uart_drv_registration);
}


/* end of file */

