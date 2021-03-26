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
 * \brief iMX RT10XX LPSPI driver
 *
 * \internal
 * \par modification history:
 * - 1.00 17-10-17  mex, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_gpio.h"
#include "aw_list.h"
#include "aw_assert.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_spibus.h"
#include "aw_clk.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include "aw_io.h"
#include "aw_sem.h"

#include "driver/lpspi/awbl_imx10xx_lpspi.h"

#include "driver/lpspi/imx10xx_lpspi_regs.h"


/** \brief 限制SPI的最大速度为30Mhz */
#define __MAX_SPEED                  (30000000)


/** \brief DMA 传输最大的个数 */
#define __SPI_DMA_TRANS_MAX          (1024 * 2)

/* 发送FIFO为的16字 */
#define __LPSPI_TX_FIFO_SIZE          16

/* 接收FIFO为的16字 */
#define __LPSPI_RX_FIFO_SIZE          16


/*******************************************************************************
  macro operate
*******************************************************************************/

/* write register */
#define __LPSPI_REG_WRITE(reg_addr, data) \
        AW_REG_WRITE32(reg_addr, (data))

/* read register */
#define __LPSPI_REG_READ(reg_addr) \
        AW_REG_READ32(reg_addr)

/* bit is get */
#define __LPSPI_REG_BIT_ISSET(reg_addr, bit) \
        AW_REG_BIT_ISSET32(reg_addr, bit)

/* set bit */
#define __LPSPI_REG_BIT_SET(reg_addr, bit) \
        AW_REG_BIT_SET32(reg_addr, bit)

/* clear bit */
#define __LPSPI_REG_BIT_CLR(reg_addr, bit) \
        AW_REG_BIT_CLR32(reg_addr, bit)

/* get register bits */
#define __LPSPI_REG_BITS_GET(reg_addr, start, len) \
        AW_REG_BITS_GET32(reg_addr, start, len)

/* set register bits */
#define __LPSPI_REG_BITS_SET(reg_addr, start, len, value) \
        AW_REG_BITS_SET32(reg_addr, start, len, value)

/* declare lpspi device instance */
#define __LPSPI_DEV_DECL(p_this, p_dev) \
    struct awbl_imx10xx_lpspi_dev *p_this = \
        (struct awbl_imx10xx_lpspi_dev *)(p_dev)

/* declare lpspi device infomation */
#define __LPSPI_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx10xx_lpspi_devinfo *p_devinfo = \
        (struct awbl_imx10xx_lpspi_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* get imx10xx spi hardware infomation */
#define __LPSPI_HW_DECL(p_hw_lpspi, p_devinfo)      \
        imx10xx_lpspi_regs_t *p_hw_lpspi =        \
        ((imx10xx_lpspi_regs_t *)((p_devinfo)->regbase))

/* get spi master instance */
#define __LPSPI_MASTER_DEV_DECL(p_master, p_dev) \
        struct awbl_spi_master *p_master = (struct awbl_spi_master *)(p_dev)

/* lpspi enable */
#define  lpspi_enable(p_hw_lpspi)   __LPSPI_REG_BIT_SET(&p_hw_lpspi->CR, 0)

/* lpspi disable */
#define  lpspi_disable(p_hw_lpspi)  __LPSPI_REG_BIT_CLR(&p_hw_lpspi->CR, 0)


/* write 8-bit width repeatly */
#define __LPSPI_DATA_WRITE_DUMP_REP(p_hw_lpspi, len) {\
        size_t i = len; \
        while (i--) { \
            writel(0, (void *)&p_hw_lpspi->TDR);\
        } \
    }

/* write 8-bit width repeatly */
#define __LPSPI_DATA_READ_DUMP_REP(p_hw_lpspi, len) {\
        size_t i = len; \
        while (i--) { \
            readl((void *)&p_hw_lpspi->RDR);\
        } \
    }

#define __LPSPI_DATA_READ_REP(p_hw_lpspi, p_buf, len, bpw) { \
        size_t i = len; \
        uint##bpw##_t *ptr = (uint##bpw##_t *)(p_buf); \
        while (i--) { \
            *ptr++ = readl((void *)&p_hw_lpspi->RDR);\
        } \
}


#define __LPSPI_DATA_READ(p_hw_lpspi, p_buf, offset, len, bpw)    { \
             if (p_buf == NULL) {                                   \
                 __LPSPI_DATA_READ_DUMP_REP(p_hw_lpspi, len);       \
             } else {                                               \
                 __LPSPI_DATA_READ_REP(p_hw_lpspi,                  \
                         ((uint##bpw##_t *)p_buf) + offset,         \
                         len,                                       \
                         bpw)                                       \
             }                                                      \
}

#define __LPSPI_DATA_WRITE_REP(p_hw_lpspi, p_buf, len, bpw) { \
        size_t i = len; \
        uint##bpw##_t *ptr = (uint##bpw##_t *)(p_buf); \
        while (i--) { \
            __LPSPI_REG_WRITE(&p_hw_lpspi->TDR, *ptr++);\
        } \
    }


#define __LPSPI_DATA_WRITE(p_hw_lpspi, p_buf, offset, len, bpw)    {\
             if (p_buf == NULL) {                                   \
                 __LPSPI_DATA_WRITE_DUMP_REP(p_hw_lpspi, len);      \
             } else {                                               \
                 __LPSPI_DATA_WRITE_REP(p_hw_lpspi,                 \
                                ((uint##bpw##_t*)p_buf) + offset,   \
                                 len ,                              \
                                 bpw)                               \
             }                                                      \
}

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __lpspi_inst_init1 (awbl_dev_t *p_dev);
aw_local void __lpspi_inst_init2 (awbl_dev_t *p_dev);
aw_local void __lpspi_inst_connect (awbl_dev_t *p_dev);

/** \brief spi set up */
aw_local aw_err_t __lpspi_setup (struct awbl_spi_master *p_master,
                                 aw_spi_device_t        *p_dev);

/** \brief interrupt service */
aw_local void __lpspi_isr (void *p_arg);

/** \brief hardware initial */
aw_local void __lpspi_hard_init (struct awbl_imx10xx_lpspi_dev *p_this);

aw_err_t __lpspi_write_and_read(struct awbl_spi_master   *p_master,
                                const void               *tx_buf,
                                void                     *rx_buf,
                                uint32_t                  nbytes);

aw_err_t __lpspi_config(struct awbl_spi_master *p_master,
                        struct awbl_spi_config *config);

aw_local aw_err_t  __lpspi_get_default_config(struct awbl_spi_config *p_cfg);

aw_local void __bwp8_isr(void *p_arg);
aw_local void __bwp16_isr(void *p_arg);
aw_local void __bwp32_isr(void *p_arg);


aw_local aw_err_t __spi_dma_init (struct awbl_imx10xx_lpspi_dev *p_this);

aw_local aw_err_t __spi_dma_transfer (struct awbl_imx10xx_lpspi_dev  *p_this,
                                      uint32_t                        len,
                                      uint8_t                        *p_txbuf,
                                      uint8_t                        *p_rxbuf);

aw_local aw_err_t __spi_dma_config (struct awbl_imx10xx_lpspi_dev *p_this,
                                    struct awbl_spi_config        *config);

/*******************************************************************************
  locals
*******************************************************************************/

/** \brief Defines constant value arrays for the baud rate pre-scalar and scalar divider values.*/
aw_local const uint8_t prescaler_table[] = {1, 2, 4, 8, 16, 32, 64, 128};


/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_lpspi_drvfuncs = {
    __lpspi_inst_init1,
    __lpspi_inst_init2,
    __lpspi_inst_connect
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_lpspi_drv_registration = {
    {
        AWBL_VER_1,                           /* awb_ver */
        AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,  /* bus_id */
        AWBL_IMX10XX_LPSPI_NAME,              /* p_drvname */
        &__g_lpspi_drvfuncs,                  /* p_busfuncs */
        NULL,                                 /* p_methods */
        NULL                                  /* pfunc_drv_probe */
    }
};

/*  device information submited by driver (must defined as aw_const) */
aw_local aw_const struct awbl_spi_master_devinfo2 __g_lpspi_devinfo2 = {
    AWBL_FEATURE_SPI_CPOL_L |    /* 支持空闲时钟为低  */
    AWBL_FEATURE_SPI_CPOL_H |    /* 支持空闲时钟为高  */
    AWBL_FEATURE_SPI_CPHA_U |    /* 支持上升沿采样      */
    AWBL_FEATURE_SPI_CPHA_D ,    /* 支持下降沿采样      */
    0,                          /* flags */
    __lpspi_setup,
    __lpspi_write_and_read,
    NULL,
    __lpspi_config,
    __lpspi_get_default_config,

};

/*******************************************************************************
  implementation
*******************************************************************************/


/**
 * \brief CS deactive
 */
aw_local aw_inline
void __lpspi_set_fifo_watermark(imx10xx_lpspi_regs_t *p_hw_lpspi,
                                uint32_t              tx_water,
                                uint32_t              rx_water)
{
    __LPSPI_REG_WRITE(&p_hw_lpspi->FCR,
                     ((tx_water) & 0xFUL) | ((rx_water << 16) & 0xF0000UL));
}

/**
 * \brief 清除发送和接收FIFO
 */
aw_local aw_inline
void __lpspi_flush_fifo(imx10xx_lpspi_regs_t *p_hw_lpspi,
                        int                tx_flush,
                        int                rx_flush)
{
    uint32_t reg = 0;

    reg = __LPSPI_REG_READ(&p_hw_lpspi->CR);
    reg |= ((tx_flush << 8UL) | (rx_flush << 9UL));
    __LPSPI_REG_WRITE(&p_hw_lpspi->CR, reg);
}

/**
 * \brief 使能LPSPI的中断
 */
aw_local aw_inline
void  __lpspi_interrupts_enable(imx10xx_lpspi_regs_t *p_hw_lpspi, uint32_t mask)
{
    uint32_t reg = 0;

    reg = __LPSPI_REG_READ(&p_hw_lpspi->IER);
    reg |= mask;
    __LPSPI_REG_WRITE(&p_hw_lpspi->IER, reg);
}

/**
 * \brief 禁能LPSPI的中断
 */
aw_local aw_inline
void  __lpspi_interrupts_disable(imx10xx_lpspi_regs_t *p_hw_lpspi, uint32_t mask)
{
    uint32_t reg = 0;

    reg = __LPSPI_REG_READ(&p_hw_lpspi->IER);
    reg &= ~mask;
    __LPSPI_REG_WRITE(&p_hw_lpspi->IER, reg);
}

/**
 * \brief 清除LPSPI中断标志
 */
aw_local aw_inline
void  __lpspi_int_status_clr(imx10xx_lpspi_regs_t *p_hw_lpspi, uint32_t flags)
{
    uint32_t reg = 0;

    reg = __LPSPI_REG_READ(&p_hw_lpspi->SR);
    reg |= flags;
    __LPSPI_REG_WRITE(&p_hw_lpspi->SR, reg);
}



/* 返回默认配置  */
aw_local aw_err_t  __lpspi_get_default_config(struct awbl_spi_config *p_cfg)
{
    p_cfg->bpw = 8;
    p_cfg->mode = AW_SPI_MODE_0;
    p_cfg->speed_hz = 1000000;

    return AW_OK;
}



/**
 * \brief find last (most-significant) bit set
 * \param[in] x : the word to search
 *
 * \note This is defined the same way as ffs.
 * Note fls(0) = 0, fls(1) = 1, fls(0x80000000) = 32.
 */

aw_static_inline int __ecspi_fls(int x)
{
    int r = 32;

    if (!x) {
        return 0;
    }

    if (!(x & 0xffff0000u)) {
        x <<= 16;
        r -= 16;
    }

    if (!(x & 0xff000000u)) {
        x <<= 8;
        r -= 8;
    }

    if (!(x & 0xf0000000u)) {
        x <<= 4;
        r -= 4;
    }

    if (!(x & 0xc0000000u)) {
        x <<= 2;
        r -= 2;
    }

    if (!(x & 0x80000000u)) {
        x <<= 1;
        r -= 1;
    }

    return r;
}


/**
 * \brief ECSPI时钟配置
 */
aw_local uint32_t __lpspi_clk_config (uint32_t  in_clk,
                                      uint32_t  desir_speed,
                                      uint32_t  *prescale_val,
                                      uint32_t  *p_out_clk)
{


    /*
     * there are two 4-bit dividers, the pre-divider divides by
     * $pre, the post-divider by 2^$post
     */
    unsigned int prescale, scaler;

    if (desir_speed > in_clk) {
        return 0;
    }

    /* 找到期望速率和输入时钟的数量级之差 */
    prescale = __ecspi_fls(in_clk) - __ecspi_fls(desir_speed);
    if (in_clk > desir_speed << prescale) {
        prescale++;
    }

    /* now we have: (in_clk <= speed << post) with post being minimal */
    prescale = max(8U, prescale) - 8;


    if (prescale > 0x7) {
        return 0xff;
    }

    scaler = AW_DIV_ROUND_UP(in_clk, desir_speed << prescale) - 2;

    /* Resulting frequency for the SCLK line. */
    *p_out_clk = (in_clk / (scaler + 2)) >> prescale;

    *prescale_val = prescale;

    return scaler;
}


aw_local void __lpspi_entry_continuous_mode (imx10xx_lpspi_regs_t *p_hw_lpspi)
{
    uint32_t    tcr = 0;
    /* 使能连续发送模式 */
    tcr = __LPSPI_REG_READ(&p_hw_lpspi->TCR);
    tcr |= (1 << 21);
    tcr |= (1 << 20);
    __LPSPI_REG_WRITE(&p_hw_lpspi->TCR, tcr);
}



aw_local void __lpspi_exit_continuous_mode (imx10xx_lpspi_regs_t *p_hw_lpspi)
{
    uint32_t    tcr = 0;
    /* 关闭连续发送模式 */
    tcr = __LPSPI_REG_READ(&p_hw_lpspi->TCR);
    tcr &= ~(3 << 20);
    __LPSPI_REG_WRITE(&p_hw_lpspi->TCR, tcr);
}




/**
 * \brief 根据设置的延时时间和延时类型，计算CCR寄存器SCKPCS、PCSSCK、DBT的值。
 */
aw_local uint8_t  __lpspi_get_delay_times(imx10xx_lpspi_regs_t *p_hw_lpspi,
                                          uint32_t              clk_in,
                                          uint32_t              delay_ns,
                                          lpspi_delay_type_t    which_delay)
{
    register uint64_t real_delay;
    register uint32_t scaler;
    register uint32_t diff, min_diff;
    uint32_t best_scaler;

    uint64_t initial_delay;
    uint32_t clk_div_prescaler;
    uint32_t prescaler = 0;
    register uint32_t addition_scaler = 0;

    prescaler = (__LPSPI_REG_READ(&p_hw_lpspi->TCR) >> 27) & 0x07;
    clk_div_prescaler = clk_in / prescaler_table[prescaler];

    min_diff = 0xFFFFFFFFUL;
    best_scaler = 0xFFUL;

    if (which_delay == kLPSPI_BetweenTransfer) {

        /* min delay is 2 clock cycles */
        initial_delay = 1000000000UL * 2;
        initial_delay /= clk_div_prescaler;
        addition_scaler = 1;

    } else {

        /* min delay is 1 clock cycle */
        initial_delay = 1000000000UL;
        initial_delay /= clk_div_prescaler;
        addition_scaler = 0;
    }

    if (initial_delay >= delay_ns) {
        return 0;
    }

    for (scaler = 0; (scaler < 256U) && min_diff; scaler++) {

        real_delay = 1000000000U;
        real_delay *= (scaler + 1 + addition_scaler);
        real_delay /= clk_div_prescaler;

        if (real_delay > delay_ns) {
            diff = real_delay - delay_ns;
            if (min_diff > diff) {
                min_diff = diff;
                best_scaler = scaler;
            }
        }
    }

    return best_scaler;
}


aw_err_t __lpspi_config(struct awbl_spi_master *p_master,
                        struct awbl_spi_config *config)
{
    __LPSPI_DEV_DECL(p_this, p_master);
    __LPSPI_DEVINFO_DECL(p_devinfo, p_this);
    __LPSPI_HW_DECL(p_hw_lpspi, p_devinfo);

    uint32_t tcr = 0, ccr = 0;
    uint32_t clk_in = 0;
    uint32_t scaler = 0,  prescaler = 0;
    uint32_t min_speed  = 0;
    uint32_t psc_to_sck = 0;

    uint32_t clk_out = 0;
    uint32_t clk_period_ns = 0;

    /* 获取输入时钟 */
    clk_in = aw_clk_rate_get(p_devinfo->ref_clk_id);

    /* 计算理论的最小速度 */
    /* 最大速度限定为30Mhz */
    min_speed = clk_in / (128 * 256);

    if (config->speed_hz > __MAX_SPEED) {
        config->speed_hz = __MAX_SPEED;
    } else if (config->speed_hz < min_speed) {
        return -AW_ENOTSUP;
    }

    /* 根据期望的速度获取预分频的值 */
    scaler = __lpspi_clk_config(clk_in, config->speed_hz, &prescaler, &clk_out);

    /* 当速率不变的时候就不计算，节省时间 */
    if (p_this->cur_speed != config->speed_hz) {
        clk_period_ns = 1000000000 / config->speed_hz ;
        psc_to_sck = __lpspi_get_delay_times(p_hw_lpspi,
                                             clk_in,
                                             clk_period_ns / 2,
                                             kLPSPI_PcsToSck);
        p_this->cur_cs_sck_t = psc_to_sck;
    }

    p_this->cur_speed = config->speed_hz;

    lpspi_disable(p_hw_lpspi);

    /* 先清空FIFO */
    __lpspi_flush_fifo(p_hw_lpspi, 1, 1);

    /* 清除所有的中断标志和所有的中断 */
    __lpspi_int_status_clr(p_hw_lpspi, kLPSPI_AllStatusFlag);
    __lpspi_interrupts_disable(p_hw_lpspi, kLPSPI_AllInterrupt);

    /* 设置数据位大小 */
    if ((config->bpw < 8) || (config->bpw > 32)) {
        return -AW_ENOTSUP;
    }

    tcr = __LPSPI_TCR_FRAMESZ(config->bpw - 1);

    /* 如果时钟相位和极性可以设置 */
    if (config->mode & AW_SPI_CPHA) {
        tcr |= __LPSPI_TCR_CPHA;
    }
    if (config->mode & AW_SPI_CPOL) {
        tcr |= __LPSPI_TCR_CPOL;
    }

    /* 如果按低位先出的方式发送数据帧 */
    if (config->mode & AW_SPI_LSB_FIRST) {
        tcr |= __LPSPI_TCR_LSBF;
    }

    /* 设置预分频 */
    tcr |= __LPSPI_TCR_PRESCALE(prescaler);
    __LPSPI_REG_WRITE(&p_hw_lpspi->TCR, tcr);

    if (p_devinfo->hard_cs) {
        /* 设置时序 */
        ccr  = __LPSPI_CCR_SCKPCS(p_devinfo->sck_cs) |
               __LPSPI_CCR_PCSSCK(p_devinfo->cs_sck) |
               __LPSPI_CCR_DBT(10)                   |
               __LPSPI_CCR_SCKDIV(scaler);
    } else {
        ccr  = __LPSPI_CCR_SCKPCS(2)                    |
               __LPSPI_CCR_PCSSCK(p_this->cur_cs_sck_t) |
               __LPSPI_CCR_DBT(2)                       |
               __LPSPI_CCR_SCKDIV(scaler);
    }

    __LPSPI_REG_WRITE(&p_hw_lpspi->CCR, ccr);

    /* 使能lpspi模块 */
    lpspi_enable(p_hw_lpspi);
    return AW_OK;
}


/* SPI读写  */
aw_err_t __lpspi_write_and_read(struct awbl_spi_master   *p_master,
                                const void               *tx_buf,
                                void                     *rx_buf,
                                uint32_t                  nbytes)
{
    __LPSPI_DEV_DECL(p_this, p_master);
    __LPSPI_DEVINFO_DECL(p_devinfo, p_this);
    __LPSPI_HW_DECL(p_hw_lpspi, p_devinfo);

    uint8_t  bits_per_word = p_master->cur_config.bpw;
    uint32_t len = 0;

    p_this->p_write_buf  = tx_buf;
    p_this->p_read_buf   = rx_buf;
    p_this->write_index  = 0;
    p_this->read_nbytes  = 0;
    p_this->nbytes_to_recv = nbytes;

    if (p_devinfo->hard_cs && (p_this->continuous_flg) == AW_FALSE) {
        __lpspi_entry_continuous_mode(p_hw_lpspi);
        p_this->continuous_flg = AW_TRUE;
    }

    /* 如果使用中断传输  */
    /* 先清空FIFO */
    __lpspi_flush_fifo(p_hw_lpspi, 1, 1);

    if (bits_per_word < 9) {

        len = nbytes  > __LPSPI_TX_FIFO_SIZE ? __LPSPI_TX_FIFO_SIZE : nbytes;

        __LPSPI_DATA_WRITE(p_hw_lpspi, tx_buf, 0, len, 8);
        p_this->write_index = len;

        p_this->bpw_func = __bwp8_isr;
    } else if (bits_per_word < 17) {

        len = (nbytes / 2)  > __LPSPI_TX_FIFO_SIZE ? __LPSPI_TX_FIFO_SIZE : (nbytes / 2 );
        __LPSPI_DATA_WRITE(p_hw_lpspi, tx_buf, 0, len, 16);
        p_this->write_index = len;
        p_this->bpw_func = __bwp16_isr;
    } else {
        len = (nbytes / 4)  > __LPSPI_TX_FIFO_SIZE ? __LPSPI_TX_FIFO_SIZE : (nbytes / 4);
        __LPSPI_DATA_WRITE(p_hw_lpspi, tx_buf, 0, len, 32);
        p_this->write_index = len;
        p_this->bpw_func = __bwp32_isr;
    }

    /* 使能发送阈值中断和发送空中断 */
    __lpspi_interrupts_enable(p_hw_lpspi, kLPSPI_TxInterrupt | kLPSPI_TransferCompleteInterrupt);

    /* 等待信号量  */
    AW_SEMB_TAKE(p_this->lpspi_sem_sync, AW_SEM_WAIT_FOREVER);

    return AW_OK;
}


aw_local void __bwp8_isr(void *p_arg)
{
    __LPSPI_DEV_DECL(p_this, p_arg);
    __LPSPI_DEVINFO_DECL(p_devinfo, p_this);
    __LPSPI_HW_DECL(p_hw_lpspi, p_devinfo);

    int len = (__LPSPI_REG_READ(&p_hw_lpspi->FSR) >> 16) & 0x1F;

    /* 读空recieve buffer  */
    __LPSPI_DATA_READ(p_hw_lpspi, p_this->p_read_buf, p_this->read_nbytes, len, 8);
    p_this->read_nbytes += len;

    if (p_this->read_nbytes == p_this->nbytes_to_recv) {

        /* 关闭所有的中断  */
        //__lpspi_int_status_clr(p_hw_lpspi, kLPSPI_AllInterrupt);
        __LPSPI_REG_WRITE(&p_hw_lpspi->IER, 0x00);

         p_this->write_index = 0;
         p_this->read_nbytes = 0;
         AW_SEMB_GIVE(p_this->lpspi_sem_sync);
         return;
    }

    if (p_this->nbytes_to_recv - p_this->write_index > len) {

        __LPSPI_DATA_WRITE(p_hw_lpspi,
                           p_this->p_write_buf,
                           p_this->write_index,
                           len,
                           8);
        p_this->write_index += len;

    } else {

        __LPSPI_DATA_WRITE(p_hw_lpspi,
                           p_this->p_write_buf,
                           p_this->write_index,
                           p_this->nbytes_to_recv - p_this->write_index,
                           8);
        p_this->write_index += p_this->nbytes_to_recv - p_this->write_index;

        /* 禁能阀值中断，允许FIFO空中断 */
        __lpspi_interrupts_disable(p_hw_lpspi, kLPSPI_TxInterrupt);

        /* 关闭连续发送模式 */
        if (p_devinfo->hard_cs && (p_this->continuous_flg) == AW_TRUE) {
                __lpspi_exit_continuous_mode(p_hw_lpspi);
                p_this->continuous_flg = AW_FALSE;
        }
    }

}

aw_local void __bwp16_isr(void *p_arg)
{
    __LPSPI_DEV_DECL(p_this, p_arg);
    __LPSPI_DEVINFO_DECL(p_devinfo, p_this);
    __LPSPI_HW_DECL(p_hw_lpspi, p_devinfo);

    int len = (__LPSPI_REG_READ(&p_hw_lpspi->FSR) >> 16) & 0x1F;

    /* 读空recieve buffer */

    __LPSPI_DATA_READ(p_hw_lpspi, p_this->p_read_buf, p_this->read_nbytes / 2, len, 16);

    p_this->read_nbytes += (len * 2);

    if (p_this->read_nbytes == p_this->nbytes_to_recv) {


        /* 关闭所有的中断  */
        //__lpspi_int_status_clr(p_hw_lpspi, kLPSPI_AllInterrupt);
        __LPSPI_REG_WRITE(&p_hw_lpspi->IER, 0x00);

        p_this->write_index = 0;
        p_this->read_nbytes = 0;
        AW_SEMB_GIVE(p_this->lpspi_sem_sync);
        return;
    }

    if ((p_this->nbytes_to_recv / 2) - p_this->write_index > len) {

        __LPSPI_DATA_WRITE(p_hw_lpspi,
                           p_this->p_write_buf,
                           p_this->write_index,
                           len,
                           16);
        p_this->write_index += len;

    } else {

        __LPSPI_DATA_WRITE(p_hw_lpspi,
                           p_this->p_write_buf,
                           p_this->write_index,
                           (p_this->nbytes_to_recv / 2) - p_this->write_index,
                           16);

        p_this->write_index += (p_this->nbytes_to_recv / 2) - p_this->write_index;

        /* 禁能阀值中断，允许FIFO空中断 */
        __lpspi_interrupts_disable(p_hw_lpspi, kLPSPI_TxInterrupt);

        /* 关闭连续发送模式 */
        if (p_devinfo->hard_cs && (p_this->continuous_flg) == AW_TRUE) {
                __lpspi_exit_continuous_mode(p_hw_lpspi);
                p_this->continuous_flg = AW_FALSE;
        }
    }
}


aw_local void __bwp32_isr(void *p_arg)
{
    __LPSPI_DEV_DECL(p_this, p_arg);
    __LPSPI_DEVINFO_DECL(p_devinfo, p_this);
    __LPSPI_HW_DECL(p_hw_lpspi, p_devinfo);


    int len = (__LPSPI_REG_READ(&p_hw_lpspi->FSR) >> 16) & 0x1F;

    /* 读空recieve buffer */

    __LPSPI_DATA_READ(p_hw_lpspi, p_this->p_read_buf, p_this->read_nbytes / 4, len, 32);
    p_this->read_nbytes += (len * 4);

    if (p_this->read_nbytes == p_this->nbytes_to_recv) {

        /* 关闭所有的中断  */
        //__lpspi_int_status_clr(p_hw_lpspi, kLPSPI_AllInterrupt);
        __LPSPI_REG_WRITE(&p_hw_lpspi->IER, 0x00);

        p_this->write_index = 0;
        p_this->read_nbytes = 0;
        AW_SEMB_GIVE(p_this->lpspi_sem_sync);
        return ;
    }

    if ((p_this->nbytes_to_recv / 4) - p_this->write_index > len) {

        __LPSPI_DATA_WRITE(p_hw_lpspi,
                           p_this->p_write_buf,
                           p_this->write_index,
                           len,
                           32);
        p_this->write_index += len;

    } else {
        __LPSPI_DATA_WRITE(p_hw_lpspi,
                           p_this->p_write_buf,
                           p_this->write_index,
                           (p_this->nbytes_to_recv / 4) - p_this->write_index,
                           32);

        p_this->write_index += (p_this->nbytes_to_recv / 4) - p_this->write_index;

        /* 禁能阀值中断，允许FIFO空中断 */
        __lpspi_interrupts_disable(p_hw_lpspi, kLPSPI_TxInterrupt);

        /* 关闭连续发送模式 */
        if (p_devinfo->hard_cs && (p_this->continuous_flg) == AW_TRUE) {
            __lpspi_exit_continuous_mode(p_hw_lpspi);
            p_this->continuous_flg = AW_FALSE;
        }
    }

}


/**
 *
 * \brief LPSPI controller hardware initialize
 */
aw_local void __lpspi_hard_init (struct awbl_imx10xx_lpspi_dev *p_this)
{

    __LPSPI_DEVINFO_DECL(p_devinfo, p_this);
    __LPSPI_HW_DECL(p_hw_lpspi, p_devinfo);

    uint32_t reg = 0;

    lpspi_disable(p_hw_lpspi);
    aw_udelay(10);

    /* reset the lpspi */
    reg = __LPSPI_REG_READ(&p_hw_lpspi->CR);
    reg |= __LPSPI_CR_RST_MASK |  /* Software Reset */
           __LPSPI_CR_RRF_MASK |  /* Reset Receive FIFO */
           __LPSPI_CR_RTF_MASK |  /* Reset Transmit FIFO */
           (0x1UL << 2);          /* Disable Doze Mode */
    __LPSPI_REG_WRITE(&p_hw_lpspi->CR, reg);

    aw_udelay(10);

    reg = 0;
    reg |= (0x1UL << 3);   /* Debug Enable */
    __LPSPI_REG_WRITE(&p_hw_lpspi->CR, reg);

    reg = __LPSPI_CFGR1_MASTER_MASK     |  /* Master mode */
          __LPSPI_CFGR1_PCSCFG_MASK     |  /* PCS[3:2] are disabled  */
          __LPSPI_CFGR1_CFGR1_PINCFG(0) |  /* SIN is used for input data and SOUT for output data */
          __LPSPI_CFGR1_OUTCFG(0)       |  /* Output data retains last value when chip select is negated */
          __LPSPI_CFGR1_NOSTALL_CFG(0);    /* Transfers will stall when transmit FIFO is empty or receive FIFO is full */
    if (p_devinfo->cs_active_H) {
        reg |= (0x1UL << 8);
    }
    __LPSPI_REG_WRITE(&p_hw_lpspi->CFGR1, reg);

    lpspi_enable(p_hw_lpspi);

     /* 发送和接收都设置为半FIFO中断 */
    __lpspi_set_fifo_watermark(p_hw_lpspi,
                              (__LPSPI_TX_FIFO_SIZE / 2),
                              (__LPSPI_RX_FIFO_SIZE / 2));

    /* disable all interruputs */
    __lpspi_interrupts_disable(p_hw_lpspi, kLPSPI_AllInterrupt);

}



/**
 * \brief SPI interrupt service routine
 *
 * \param[in]   p_arg
 */
aw_local void __lpspi_isr (void *p_arg)
{
    __LPSPI_DEV_DECL(p_this, p_arg);
    __LPSPI_DEVINFO_DECL(p_devinfo, p_this);
    __LPSPI_HW_DECL(p_hw_lpspi, p_devinfo);

    __lpspi_int_status_clr(p_hw_lpspi, kLPSPI_AllInterrupt);

    p_this->bpw_func(p_arg);
}


/**
 * \brief set up spi controller
 */
aw_local aw_err_t __lpspi_setup (struct awbl_spi_master *p_master,
                                 aw_spi_device_t        *p_spi_dev)
{
    __LPSPI_DEV_DECL(p_this, p_master);
    __LPSPI_DEVINFO_DECL(p_devinfo, p_this);

    /*
     * check bits_per_word
     * keystone spi suppurt max 32bit
     */
    if ((p_spi_dev->bits_per_word < 1) || (p_spi_dev->bits_per_word > 32)) {

        return -AW_ENOTSUP;
    }

    /*
     * check if we can provide the requested rate
     */
    {
        uint32_t min_speed;
        uint32_t clk_in;

        clk_in = aw_clk_rate_get(p_devinfo->ref_clk_id);

        /* 计算理论的最小速度 */
        min_speed = clk_in / (128 * 257);

        /* 最大速度限定为30Mhz */
        if (p_spi_dev->max_speed_hz > __MAX_SPEED) {
            p_spi_dev->max_speed_hz = __MAX_SPEED;
        } else if (p_spi_dev->max_speed_hz < min_speed) {
            return -AW_ENOTSUP;
        }
    }

    return AW_OK;
}


/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __lpspi_inst_init1(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __lpspi_inst_init2(awbl_dev_t *p_dev)
{
    __LPSPI_MASTER_DEV_DECL(p_master, p_dev);
    __LPSPI_DEV_DECL(p_this, p_dev);
    __LPSPI_DEVINFO_DECL(p_devinfo, p_dev);

    /* platform initialization */
    if (p_devinfo->pfunc_plfm_init != NULL) {
        p_devinfo->pfunc_plfm_init();
    }

    /* AWBus spi master general init2 */
    p_master->p_devinfo2 = &__g_lpspi_devinfo2;
    p_this->cur_bpw      = 0;
    p_this->cur_speed    = 0;
    p_this->cur_cs_sck_t = 0;
    p_this->continuous_flg = AW_FALSE;

    /* init controller hardware */
    __lpspi_hard_init(p_this);

    /* connect and enable isr */
    aw_int_connect(p_devinfo->inum, (aw_pfuncvoid_t)__lpspi_isr, (void *)p_dev);
    aw_int_enable(p_devinfo->inum);

    /* create spi bus */
    awbl_spibus_create(p_master);

    /* initialize the stack binary semaphore */
    AW_SEMB_INIT(p_this->lpspi_sem_sync, 0, AW_SEM_Q_FIFO);

    return;
}


/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __lpspi_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/******************************************************************************/
void awbl_imx10xx_lpspi_drv_register (void)
{

    awbl_drv_register((struct awbl_drvinfo *)&__g_lpspi_drv_registration);
}


/* end of file */
