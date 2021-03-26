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
 * \brief The IMX1020 Medium Quality Sound driver
 *
 * \internal
 * \par modification history:
 * - 1.00 17-12-07, hsg, first implementation.
 * \endinternal
 */
#include "aworks.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_cache.h"

#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_vdebug.h"
#include "aw_assert.h"
#include "aw_delay.h"
#include "aw_sem.h"
#include "aw_clk.h"

#include "driver/mqs/awbl_imx1020_mqs.h"
#include "soc/awsa_soc_internal.h"

#define __MQS_IOMUXC_GRP2_MASK ((1 << 26) | (1 << 24) | (0xFF << 16))


/*******************************************************************************
  Global defines
*******************************************************************************/
struct awbl_imx1020_mqs_dev *__gp_mqs_dev = NULL;

/*******************************************************************************
  Local defines
*******************************************************************************/

#define __MQS_DEV_DECL(p_this, p_dev) \
    struct awbl_imx1020_mqs_dev *p_this = \
        (struct awbl_imx1020_mqs_dev *)(p_dev)

#define __MQS_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx1020_mqs_devinfo *p_devinfo = \
        (struct awbl_imx1020_mqs_devinfo *)AWBL_DEVINFO_GET(p_dev)

void awbl_imx1020_mqs_init (
    struct awbl_imx1020_mqs_dev            *p_dev,
    const struct awbl_imx1020_mqs_devinfo  *p_devinfo);


#define __MQS_RATES (AWSA_PCM_RATE_44100)

/*******************************************************************************
  locals functions
*******************************************************************************/

/* Reset The MQS */
aw_local
aw_err_t awbl_imx1020_mqs_reset (void)
{
    __MQS_DEVINFO_DECL(p_devinfo, __gp_mqs_dev);

    volatile uint32_t *p_reg = (volatile uint32_t *)p_devinfo->iomux_grp2_regaddr;

    if (__gp_mqs_dev == NULL) {
        return -AW_ENODEV;
    }

    *p_reg |= (1 << 24);
    *p_reg &= ~(1 << 24);

    return AW_OK;
}

/******************************************************************************/


/* Enable The MQS */
aw_local
aw_err_t awbl_imx1020_mqs_config (uint32_t samples_rate,
                                  uint8_t  width,
                                  uint8_t  channels)
{
    __MQS_DEVINFO_DECL(p_devinfo, __gp_mqs_dev);
    volatile uint32_t  *p_reg = (volatile uint32_t *)p_devinfo->iomux_grp2_regaddr;
    volatile uint32_t   tmp;
    int                 div;
    uint32_t            clk_rate;
    uint32_t            desired_rate;

    if (__gp_mqs_dev == NULL) {
        return -AW_ENODEV;
    }

    if ((width != 16)) {

        AW_INFOF(("MQS only supports LSB-valid 16-bit, MSB shift-out first serial data.\r\n"));
        return -AW_EINVAL;
    }

    if (channels != 2) {

        AW_INFOF(("MQS only supports two-channel audio data.\r\n"));
        return -AW_EINVAL;
    }

    /* The MQS only support the 44100 and 48000 */
    if ((samples_rate != 44100) && (samples_rate != 48000)) {

        AW_INFOF(("MQS only supports audio data with a sampling rate of 44.1kHz or 48kHz.\r\n"));
        return -AW_EINVAL;
    }

    /*
     * mclk_rate / (oversample(32,64) * FS * 2 * divider ) = repeat_rate;
     * if repeat_rate is 8, mqs can achieve better quality.
     * oversample rate is fix to 32 currently.
     * When the samples_rate is 44100, the div = clk_rate / 22579200
     * When the samples_rate is 48000, the div = clk_rate / 24576000
     */
    desired_rate = (32 * 2 * samples_rate * 8);
    clk_rate = aw_clk_rate_get(p_devinfo->ref_clk_id);

    if (clk_rate % desired_rate != 0) {
        return -AW_ENOTSUP;
    }

    div = clk_rate / desired_rate;

    if (div) {
        div--;
    }

    tmp = *p_reg;

    tmp &= ~__MQS_IOMUXC_GRP2_MASK;
    tmp |= (0 << 26) | (div << 16);

    *p_reg = tmp;

    return AW_OK;
}

/******************************************************************************/

/* Enable The MQS */
aw_err_t awbl_imx1020_mqs_enable (void)
{
    __MQS_DEVINFO_DECL(p_devinfo, __gp_mqs_dev);

    volatile uint32_t *p_reg = (volatile uint32_t *)p_devinfo->iomux_grp2_regaddr;

    if (__gp_mqs_dev == NULL) {
        return -AW_ENODEV;
    }

    *p_reg |= (1 << 25);
    return AW_OK;
}

/******************************************************************************/


/* Disable The MQS */
aw_err_t awbl_imx1020_mqs_disable (void)
{
    __MQS_DEVINFO_DECL(p_devinfo, __gp_mqs_dev);

    volatile uint32_t *p_reg = (volatile uint32_t *)p_devinfo->iomux_grp2_regaddr;

    if (__gp_mqs_dev == NULL) {
        return -AW_ENODEV;
    }

    *p_reg &= ~(1 << 25);

    return AW_OK;
}

/******************************************************************************/

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sai_inst_init1(awbl_dev_t *p_dev)
{
    return;
}

/******************************************************************************/

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sai_inst_init2 (awbl_dev_t *p_dev)
{

    return;
}

/******************************************************************************/

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sai_inst_connect(awbl_dev_t *p_dev)
{
    __MQS_DEVINFO_DECL(p_devinfo, p_dev);

    /* platform initialization */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    __gp_mqs_dev = (struct awbl_imx1020_mqs_dev *)p_dev;


    /* ³õÊ¼»¯Éù¿¨ */
    awbl_imx1020_mqs_init (__gp_mqs_dev, p_devinfo);

    return;
}

/******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_sai_drvfuncs = {
    __sai_inst_init1,
    __sai_inst_init2,
    __sai_inst_connect
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_mqs_drv_registration = {
    {
        AWBL_VER_1,             /* awb_ver */
        AWBL_BUSID_PLB,         /* bus_id */
        AWBL_IMX1020_MQS_NAME,  /* p_drvname */
        &__g_sai_drvfuncs,      /* p_busfuncs */
        NULL,                   /* p_methods */
        NULL                    /* pfunc_drv_probe */
    }
};



/* imx1020 mqs init */
aw_local aw_err_t __imx1020_mqs_init (struct awsa_soc_pcm *p_soc_pcm)
{

    awbl_imx1020_mqs_reset();
    awbl_imx1020_mqs_enable();
    return AW_OK;
}


/* imx1020 mqs startup  */
aw_local
aw_err_t __imx1020_mqs_startup (struct awsa_soc_pcm *p_soc_pcm)
{
    return AW_OK;
}


/* imx1020 mqs config */
aw_local
aw_err_t __imx1020_mqs_config (struct awsa_soc_pcm *p_soc_pcm,
                               awsa_pcm_stream_t    stream,
                               awsa_pcm_config_t   *p_config)
{
    int     ret   = AW_OK;
    uint8_t width = 0;

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

    ret = awbl_imx1020_mqs_config (p_config->rate,
                                   width,
                                   p_config->channels);
    return ret;
}

/* imx1020 mqs shutdown */
aw_local
aw_err_t __imx1020_mqs_shutdown(struct awsa_soc_pcm *p_soc_pcm,
                                awsa_pcm_stream_t    stream)
{

    return AW_OK;
}


/* servopts */
aw_local const struct awsa_soc_servopts __g_imx1020_mqs_opts = {
    NULL,
    NULL,
    __imx1020_mqs_startup,
    __imx1020_mqs_shutdown,
    __imx1020_mqs_config,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/* connects codec <--> plfm */
aw_local const struct awsa_soc_dai_link __g_imx1020_mqs_dai_link = {
    "imx1020_mqs",           /**< \brief dai_link name */
    "mqs_duplex",            /**< \brief Stream name */
    "mqs_codec",             /**< \brief for multi-codec */
    "awbl_imx1020_sai",      /**< \brief for multi-platform */
    "imx1020",               /**< \brief platform dai name */
    "mqs_playback",          /**< \brief codec dai name */
    __imx1020_mqs_init,
    &__g_imx1020_mqs_opts,
};


/* card driver */
aw_local const
struct awsa_soc_card_driver __g_imx1020_mqs_card_driver = {
    &__g_imx1020_mqs_dai_link,
    1,
    NULL,
    NULL,
    NULL,
    NULL
};


/*  MQS dai Service operate interface */
aw_local const struct awsa_soc_dai_servopts __g_mqs_dai_opts_playback = {0};

/* codec driver */
aw_local struct awsa_soc_codec_driver __g_mqs_codec_driver = {0};

/* dai driver */
aw_local const struct awsa_soc_dai_driver __g_mqs_dai_driver[] = {

    /* playback only - dual interface */
    {
        &__g_mqs_dai_opts_playback,
        {
            NULL, 0, 0, 0, 0, 0, 0
        },
        {
            "Playback",
            AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_S16_LE),
            __MQS_RATES,
            AWSA_PCM_RATE_8000,
            AWSA_PCM_RATE_48000,
            1,
            2
        },
        0,
        NULL,
        NULL,
    },
};


/**
 * \brief machine init
 */
void awbl_imx1020_mqs_init (
    struct awbl_imx1020_mqs_dev            *p_dev,
    const struct awbl_imx1020_mqs_devinfo  *p_devinfo)
{
    const char *dai[] = {"mqs_playback"};
        
    p_dev->p_devinfo = p_devinfo;

    /* ×¢²á½âÂëÆ÷ */
    awsa_soc_codec_register(&p_dev->codec,
                            "mqs_codec",
                            &__g_mqs_codec_driver,
                            p_dev->dai,
                            dai,
                            __g_mqs_dai_driver,
                            AWSA_SOC_ARRAY_SIZE(__g_mqs_dai_driver),
                            (void *)p_dev);

    /* ×¢²áÉù¿¨ */
    awsa_soc_card_register(&p_dev->soc_card,
                           p_devinfo->card_id,
                           "imx1020_mqs_soundcard",
                           &p_dev->soc_pcm,
                           1,
                           NULL,
                           0,
                           &__g_imx1020_mqs_card_driver,
                           (void *)p_dev);
}

/******************************************************************************/
void awbl_imx1020_mqs_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_mqs_drv_registration);
    return;
}

/* end of file */
