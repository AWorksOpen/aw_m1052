/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief The imx1020 Synchronous Audio Interface driver
 *
 * \internal
 * \par modification history:
 * - 1.00 17-12-01, hsg, first implementation.
 * \endinternal
 */
#include "aworks.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include <string.h>
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_vdebug.h"
#include "aw_assert.h"
#include "aw_delay.h"
#include "aw_sem.h"
#include "aw_system.h"
#include "aw_clk.h"
#include "driver/edma/aw_imx10xx_edma.h"
#include "driver/sai/imx1020_sai_regs.h"
#include "driver/sai/awbl_imx1020_sai.h"

#include "soc/awsa_soc_internal.h"


#define CONFIG_SAI_TX_WATER_MARK    15

#define __SAI_RATES (AWSA_PCM_RATE_8000  | \
                     AWSA_PCM_RATE_11025 | \
                     AWSA_PCM_RATE_16000 | \
                     AWSA_PCM_RATE_22050 | \
                     AWSA_PCM_RATE_32000 | \
                     AWSA_PCM_RATE_44100 | \
                     AWSA_PCM_RATE_48000 | \
                     AWSA_PCM_RATE_88200 | \
                     AWSA_PCM_RATE_96000)

/*******************************************************************************
  Local defines
*******************************************************************************/

/* declare imx1020 sai infomation */
#define __SAI_DEV_DECL(p_this, p_dev) \
    struct awbl_imx1020_sai_dev *p_this = \
        (struct awbl_imx1020_sai_dev *)(p_dev)

#define __SAI_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx1020_sai_devinfo *p_devinfo = \
        (struct awbl_imx1020_sai_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __SAI_REGS_DECL(p_regs, p_devinfo) \
    struct imx1020_sai_regs *p_regs = \
        (struct imx1020_sai_regs *)(p_devinfo->regbase)

/*******************************************************************************
  locals functions
*******************************************************************************/

aw_local void __sai_reg_write (volatile uint32_t *p_reg,
                               uint32_t           mask,
                               uint32_t           val)
{
    volatile uint32_t tmp;

    tmp = *p_reg;
    tmp &= ~mask;
    tmp |= val & mask;

    *p_reg = tmp;
}


aw_local aw_err_t __sai_tx_reset (struct awbl_imx1020_sai_dev *p_this)
{
    volatile uint32_t           tmp;
    __SAI_DEVINFO_DECL(p_devinfo, p_this);
    __SAI_REGS_DECL(p_regs, p_devinfo);

    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);
    tmp = p_regs->tcsr;
    /* Software Reset assert */
    p_regs->tcsr = tmp | __SAI_TCSR_SR;
    aw_udelay(1000);
    /* Software Reset de-assert */
    p_regs->tcsr = tmp & (~__SAI_TCSR_SR);
    /* FIFO Reset */
    p_regs->tcsr = tmp | __SAI_TCSR_FR;
    aw_udelay(1000);
    AW_MUTEX_UNLOCK(p_this->mutex_dev);
    return AW_OK;
}


/* sai dma tx complete call back */
void __imx1020_sai_tx_dma_callback (void *p_arg, unsigned int len)
{
    __SAI_DEV_DECL(p_this, p_arg);

    p_this->app_ptr += p_this->dma_period;
    if (p_this->app_ptr > (uint32_t)p_this->p_dma_area + p_this->dma_size) {
        p_this->app_ptr -= p_this->dma_size;
    }

     awsa_soc_pcm_period_elapsed(p_this->soc_plfm.p_soc_pcm);
}


/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sai_inst_init1(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sai_inst_init2 (awbl_dev_t *p_dev)
{
    __SAI_DEV_DECL(p_this, p_dev);
    __SAI_DEVINFO_DECL(p_devinfo, p_this);

    /* platform initialization */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }


    /* 初始化设备锁 */
    AW_MUTEX_INIT(p_this->mutex_dev, AW_SEM_Q_FIFO);
    return;
}



/**
 * SAI system clock configuration.
 * Should only be called when port is inactive.
 */
aw_local
aw_err_t __imx1020_sai_dai_sysclk_set (struct awsa_soc_dai *p_dai,
                                       uint32_t             clk_id,
                                       uint32_t             freq,
                                       int                  dir)
{
    return AW_OK;
}



/**
 * Startup a PCM substream
 */
aw_local
aw_err_t __imx1020_sai_dai_startup (struct awsa_soc_dai *p_dai)
{
    struct awsa_soc_platform *p_plfm = p_dai->plfm_codec.p_plfm;
    __SAI_DEV_DECL(p_this, p_plfm->p_private_data);
    __SAI_DEVINFO_DECL(p_devinfo, p_this);

    /* 使能时钟 */
    aw_clk_enable(p_devinfo->ref_clk_id);

    /* reset sai tx */
    __sai_tx_reset(p_this);

    return AW_OK;
}


/**
 * Shutdown a PCM substream
 */
aw_local
aw_err_t __imx1020_sai_dai_shutdown (struct awsa_soc_dai *p_dai,
                                     awsa_pcm_stream_t    stream)
{

    struct awsa_soc_platform *p_plfm = p_dai->plfm_codec.p_plfm;
    __SAI_DEV_DECL(p_this, p_plfm->p_private_data);
    __SAI_DEVINFO_DECL(p_devinfo, p_this);

    /* 禁能时钟 */
    aw_clk_disable(p_devinfo->ref_clk_id);

    return AW_OK;
}

/*
 * Should only be called when port is inactive.
 * although can be called multiple times by upper layers.
 */
aw_local
aw_err_t __imx1020_sai_dai_config (struct awsa_soc_dai *p_dai,
                                   awsa_pcm_stream_t    stream,
                                   awsa_pcm_config_t   *p_config)
{
    struct awsa_soc_platform *p_plfm = p_dai->plfm_codec.p_plfm;
    __SAI_DEV_DECL(p_this, p_plfm->p_private_data);
    __SAI_DEVINFO_DECL(p_devinfo, p_this);
    __SAI_REGS_DECL(p_regs, p_devinfo);

    uint32_t     bit_clock = 0;
    uint16_t     div = 0;
    int          ret = 0;
    uint32_t     clk_rate = 0;
    uint32_t     desired_rate = 0;
    uint8_t      width = 0;

    switch (p_config->format) {
    case AWSA_PCM_FORMAT_S8:
        width = 8;
        break;
    case AWSA_PCM_FORMAT_S16_LE:
        width = 16;
        break;
    case AWSA_PCM_FORMAT_S24_LE:
        width = 32;
        break;
    default:
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);
    /* Set HW params (Set bclk) */
    desired_rate = 32 * 2 * 8 * p_config->rate;
    bit_clock = p_config->channels *  width * p_config->rate;
    if (desired_rate % (bit_clock*2) != 0 ) {
        ret = -AW_ENOTSUP;
        goto cleanup;
    }
    // 尝试找到并设置合适的频率
    clk_rate = aw_clk_rate_get(p_devinfo->ref_clk_id);
    ret = 0;
    if (clk_rate % desired_rate != 0) {
        ret = 1;
        clk_rate = desired_rate;

        while (clk_rate/bit_clock <=256) {
            aw_clk_rate_set(p_devinfo->ref_clk_id,clk_rate);
            clk_rate = aw_clk_rate_get(p_devinfo->ref_clk_id);
            if (clk_rate % desired_rate == 0) {
                ret = 0;
                break;
            }
            clk_rate += desired_rate;
        }
    }
    if (0 != ret) {
        ret = -AW_ENOTSUP;
        goto cleanup;
    }

    clk_rate = aw_clk_rate_get(p_devinfo->ref_clk_id);
    div      = (clk_rate / bit_clock) >> 1;            /* 因为位时钟的分频值为(DIV+1)*2 所以将写入寄存器的值 >>2后再--  */

    if (div) {
        div--;
        div &= 0xFF;
    }


    /* disable the interrupt */
    p_regs->tcsr &= ~(__SAI_TCSR_FEIE | __SAI_TCSR_FWIE | __SAI_TCSR_FRIE);

    /* Clear the flag (w1c) */
    p_regs->tcsr |= (__SAI_TCSR_WSF | __SAI_TCSR_SEF | __SAI_TCSR_FEF);

    /* 1. MSEL (p_devinfo->mclk_sel)
     * 2. sample inputs on rising edge
     * 3. Bit clock is generated internally in Master mode
     * 4. DIV = (255 + 1) * 2
     */
    __sai_reg_write(&p_regs->tcr2,
                    (0x03 << 26)                | (1 << 25) | (1 << 24) | 0xFF,
                    (p_devinfo->mclk_sel << 26) | (1 << 25) | (1 << 24) | div);

    /* Transmit FIFO Watermark, Half of the TX FIFO */
    __sai_reg_write(&p_regs->tcr1, 0x1F, CONFIG_SAI_TX_WATER_MARK);

    /* enable transmit data channel（现在配置的是通道0） */
    __sai_reg_write(&p_regs->tcr3, (1 << 16), (1 << 16));

    /* Set Frame size and Sync Width */
    __sai_reg_write(&p_regs->tcr4,
                    (0x1F << 16) | (0x1F << 8)  |
                    (1 << 4)     | (1 << 3)     | (1 << 1)     | (1 << 0),
                    (1 << 16)    | ((width - 1) << 8) |
                    (1 << 4)     | (0 << 3)     | (0 << 1)     | (1 << 0));

    /* Set Word N Width and Word 0 Width and First Bit Shifted */
    __sai_reg_write(&p_regs->tcr5,
                    (0x1F << 24) | (0x1F << 16) | (0x1F << 8),
                    ((width - 1) << 24) |
                    ((width - 1) << 16) |
                    ((width - 1) << 8));

    /* Set The MASK, Word 0 and Word1 (left and right not mask)  */
    p_regs->tmr = 0xFFFFFFFF - ((1 << 2) - 1);
cleanup:
    AW_MUTEX_UNLOCK(p_this->mutex_dev);

    return AW_OK;
}

/******************************************************************************/

/* dai servopts */
aw_local struct awsa_soc_dai_servopts __g_imx1020_sai_dai_opts = {
    __imx1020_sai_dai_sysclk_set,
    NULL,
    NULL,
    NULL,
    NULL,
    __imx1020_sai_dai_startup,
    __imx1020_sai_dai_shutdown,
    __imx1020_sai_dai_config,
    NULL,
    NULL,
    NULL
};


/* dai driver */
aw_local struct awsa_soc_dai_driver __g_imx1020_sai_dai_driver = {
    "imx1020",
    &__g_imx1020_sai_dai_opts,

    /* capture */
    {
        NULL,
        AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_S8) |
        AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_S16_LE),
        __SAI_RATES,
        AWSA_PCM_RATE_8000,
        AWSA_PCM_RATE_96000,
        2,
        2,
    },

    /* playback */
    {
        NULL,
        AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_S8) |
        AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_S16_LE),
        __SAI_RATES,
        AWSA_PCM_RATE_8000,
        AWSA_PCM_RATE_96000,
        2,
        2,
    },
    0,
    NULL,
    NULL
};


aw_local
aw_err_t __imx1020_sai_pcm_config (struct awsa_soc_platform *p_plfm,
                                   awsa_pcm_stream_t         stream,
                                   awsa_pcm_config_t        *p_config)
{
    __SAI_DEV_DECL(p_this, p_plfm->p_private_data);

    p_this->dma_period = p_config->period_size * p_config->channels *
                         awsa_core_pcm_format_phys_get(p_config->format) / 8;
    p_this->dma_size   = p_config->period_count * p_this->dma_period;
    p_this->format     = p_config->format;
    p_this->p_dma_area = (uint32_t *) aw_cache_dma_align(p_this->dma_size, p_this->dma_size);

    if (p_this->p_dma_area == NULL) {
        return -AW_ENOMEM;
    }

    p_this->app_ptr = (uint32_t)p_this->p_dma_area;
    return AW_OK;
}


aw_local
aw_err_t __imx1020_sai_dma_alloc (struct awsa_soc_platform  *p_plfm,
                                  void                     **p_dma_mem)
{
     struct   imx_dma_data  data = {0};
     aw_err_t ret = AW_OK;
    __SAI_DEV_DECL(p_this, p_plfm->p_private_data);
    __SAI_DEVINFO_DECL(p_devinfo, p_this);

    uint8_t unit_id = AWBL_DEV_UNIT_GET(p_this);

    /* SAI_NUM range: 0 ~ 2 */
    if (unit_id >= AWBL_IMX1020_SAI_NUM) {
        return AW_ERROR;
    }

    /* 申请dma通道  */
    data.peripheral_type  = p_devinfo->tx_dma_prep_type;
    data.priority         = DMA_PRIO_3 + unit_id;
    p_this->p_tx_dma_chan = imx10xx_edma_alloc_chan(&data);

    if (NULL == p_this->p_tx_dma_chan) {
        ret = AW_ERROR;
        goto clean_up;
    }

clean_up:

    AW_MUTEX_UNLOCK(p_this->mutex_dev);
    *p_dma_mem = p_this->p_dma_area;
    return ret;
}


aw_local
aw_err_t __imx1020_sai_dma_free (struct awsa_soc_platform  *p_plfm,
                                 void                      *p_dma_mem)
{
    __SAI_DEV_DECL(p_this, p_plfm->p_private_data);

    if (p_dma_mem != NULL) {
        aw_cache_dma_free(p_dma_mem);
    }

    if (p_this->p_tx_dma_chan != NULL) {
        imx10xx_edma_free_chan(p_this->p_tx_dma_chan);
    }

    return AW_OK;
}


/* dma prepare */
aw_local
aw_err_t __imx1020_sai_pcm_prepare (struct awsa_soc_platform *p_plfm)
{

    __SAI_DEV_DECL(p_this, p_plfm->p_private_data);
    __SAI_DEVINFO_DECL(p_devinfo, p_this);
    struct imx1020_sai_regs  *p_regs = (struct imx1020_sai_regs *)p_devinfo->regbase;
    aw_err_t ret = AW_OK;
    struct dma_slave_config cfg = {0};

   // aw_cache_flush(p_this->app_ptr, p_this->dma_size);

    /* 设置通道的属性 */
    cfg.direction       = DMA_MEM_TO_DEV;
    cfg.dst_addr_width  = DMA_SLAVE_BUSWIDTH_2_BYTES;
    cfg.src_addr_width  = DMA_SLAVE_BUSWIDTH_2_BYTES;
    cfg.dst_addr        = (dma_addr_t)&p_regs->tdr[0];
    cfg.src_addr        = (dma_addr_t)0;
    ret = imx10xx_edma_control(p_this->p_tx_dma_chan,
                               DMA_SLAVE_CONFIG,
                               (unsigned long)&cfg);
    if (ret) {
        aw_kprintf("%s@%d::err imx10xx_edma_control:%d\n",
                    __func__,__LINE__,ret);

        ret = AW_ERROR;
        goto cleanup;
    }

    /* 准备一个发送循环传输  */
    ret = imx10xx_edma_prep_dma_cyclic(p_this->p_tx_dma_chan,
                                       (uint32_t)p_this->app_ptr,
                                       p_this->dma_size,
                                       p_this->dma_period,
                                       DMA_MEM_TO_DEV);
    if (ret) {

        aw_kprintf("%s@%d::err imx10xx_edma_prep_dma_cyclic:%d\n",
                __func__,__LINE__,ret);

        ret = AW_ERROR;
    }

cleanup:
    return ret;
}

/* pcm trigger */
aw_local
aw_err_t __imx1020_sai_pcm_trigger (struct awsa_soc_platform  *p_plfm,
                                    int                        cmd)
{
    __SAI_DEV_DECL(p_this, p_plfm->p_private_data);
    __SAI_DEVINFO_DECL(p_devinfo, p_this);
    struct imx1020_sai_regs  *p_regs = (struct imx1020_sai_regs *)p_devinfo->regbase;

    aw_err_t ret = AW_OK;

    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);

    switch (cmd) {

    case AWSA_CORE_PCM_TRIGGER_START:

        ret = imx10xx_edma_chan_start_async(
                  p_this->p_tx_dma_chan, __imx1020_sai_tx_dma_callback, p_this);

        if (ret) {
            aw_kprintf("%s@%d::err imx10xx_edma_chan_start_async:%d\n",
                    __func__,__LINE__,ret);

            ret = AW_ERROR;
            goto cleanup;
        }

        /* 使能sai的FIFO DMA请求 */
        p_regs->tcsr |= (__SAI_TCSR_FRDE | __SAI_TCSR_FWDE);
        p_regs->tdr[0] = 0;
        p_regs->tdr[0] = 0;

        /* enable the Transmitter */
        p_regs->tcsr |= (__SAI_TCSR_TE | __SAI_TCSR_STOPE);

        break;

    case AWSA_CORE_PCM_TRIGGER_STOP:

        /* 禁能DMA请求 */
        p_regs->tcsr &= ~(__SAI_TCSR_FRDE | __SAI_TCSR_FWDE );

        /* disable the transmitter and bit clock */
        p_regs->tcsr &= ~(__SAI_TCSR_TE | __SAI_TCSR_BCE);
        while (p_regs->tcsr & __SAI_TCSR_TE);
        while (p_regs->tcsr & __SAI_TCSR_BCE);

        /* To restart the Transmitter, should soft-reset */
        p_regs->tcsr |= __SAI_TCSR_SR;
        p_regs->tcsr &= ~__SAI_TCSR_SR;

        imx10xx_edma_chan_stop(p_this->p_tx_dma_chan);

        break;

    case AWSA_CORE_PCM_TRIGGER_RESUME:
    case AWSA_CORE_PCM_TRIGGER_PAUSE_RELEASE:

        /* SAI恢复传输数据 */

        /* 使能sai的FIFO DMA请求 */
        p_regs->tcsr |= (__SAI_TCSR_FRDE | __SAI_TCSR_FWDE);
        p_regs->tdr[0] = 0;
        p_regs->tdr[0] = 0;

        /* enable the Transmitter */
        p_regs->tcsr |= (__SAI_TCSR_TE | __SAI_TCSR_STOPE);

        break;

    case AWSA_CORE_PCM_TRIGGER_SUSPEND:
    case AWSA_CORE_PCM_TRIGGER_PAUSE_PUSH:

        /* SAI暂停传输数据 */

        /* 禁能DMA请求 */
        p_regs->tcsr &= ~(__SAI_TCSR_FRDE | __SAI_TCSR_FWDE );

        /* disable the transmitter and bit clock */
        p_regs->tcsr &= ~(__SAI_TCSR_TE | __SAI_TCSR_BCE);
        while (p_regs->tcsr & __SAI_TCSR_TE);
        while (p_regs->tcsr & __SAI_TCSR_BCE);

        /* To restart the Transmitter, should soft-reset */
        p_regs->tcsr |= __SAI_TCSR_SR;
        p_regs->tcsr &= ~__SAI_TCSR_SR;

        break;

    default:
        ret = -AW_EINVAL;
        break;
    }

    AW_MUTEX_UNLOCK(p_this->mutex_dev);

cleanup:
    return ret;
}


/* pcm pointer */
aw_local
awsa_pcm_uframes_t __imx1020_sai_pcm_pointer (struct awsa_soc_platform  *p_plfm)
{
    __SAI_DEV_DECL(p_this, p_plfm->p_private_data);

    return (p_this->app_ptr - (uint32_t)p_this->p_dma_area) /
            (awsa_core_pcm_format_phys_get(p_this->format) * 2 / 8);
}


/* platform driver */
aw_local struct awsa_soc_platform_driver __g_imx1020_sai_plfm_driver = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    __imx1020_sai_pcm_config,
    __imx1020_sai_dma_alloc,
    __imx1020_sai_dma_free,
    __imx1020_sai_pcm_prepare,
    __imx1020_sai_pcm_trigger,
    __imx1020_sai_pcm_pointer,
    NULL,
    NULL,
};

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sai_inst_connect(awbl_dev_t *p_dev)
{
    __SAI_DEV_DECL(p_this, p_dev);
    __SAI_DEVINFO_DECL(p_devinfo, p_this);

    /* platform initialization */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    awsa_soc_platform_register(&p_this->soc_plfm,
                               AWBL_IMX1020_SAI_NAME,
                               &__g_imx1020_sai_plfm_driver,
                               &p_this->plfm_dai,
                               &__g_imx1020_sai_dai_driver,
                               1,
                               (void *)p_this);
}

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_sai_drvfuncs = {
    __sai_inst_init1,
    __sai_inst_init2,
    __sai_inst_connect
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_sai_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,    /* bus_id */
        AWBL_IMX1020_SAI_NAME,                  /* p_drvname */
        &__g_sai_drvfuncs,                      /* p_busfuncs */
        NULL,                                   /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};

/******************************************************************************/
void awbl_imx1020_sai_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_sai_drv_registration);
    return;
}

/* end of file */
