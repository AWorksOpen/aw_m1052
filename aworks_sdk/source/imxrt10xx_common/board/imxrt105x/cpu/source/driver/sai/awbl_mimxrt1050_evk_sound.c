/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Stock Co.,  Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief 
 *
 * \internal
 * \par Modification history
 * - 1.00 20-09-19  win, first implementation.
 * \endinternal
 */

#include "driver/sai/awbl_mimxrt1050_evk_sound.h"

#define __MIMXRT1050_EVK_SOUND_DEV_DECL(p_this, p_dev) \
    struct awbl_mimxrt1050_evk_sound_dev *p_this = \
        (struct awbl_mimxrt1050_evk_sound_dev *)p_dev;

#define __MIMXRT1050_EVK_SOUND_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_mimxrt1050_evk_sound_devinfo *p_devinfo =     \
        (struct awbl_mimxrt1050_evk_sound_devinfo *)AWBL_DEVINFO_GET(p_dev);

/*******************************************************************************
 Common
*******************************************************************************/

/**
 * \brief MIMXRT1050-EVK config
 */
static aw_err_t
__mimxrt1050_evk_sound_dai_config (struct awsa_soc_pcm *p_soc_pcm,
                                   awsa_pcm_stream_t    stream,
                                   awsa_pcm_config_t   *p_config)
{
    struct awsa_soc_dai   *p_cpu_dai   = p_soc_pcm->p_cpu_dai;
    struct awsa_soc_dai   *p_codec_dai = p_soc_pcm->p_codec_dai;

    aw_err_t ret = AW_OK;
    uint32_t dai_format;
    uint32_t dacdiv;

    /*
     * mclk   = 512 * fs
     * sysclk = mclk / 2;
     */
    ret = awsa_soc_dai_clkdiv_set(p_codec_dai,
                                  AWBL_WM8960_CLK_SYSCLKDIV,
                                  AWBL_WM8960_SYSCLK_DIV_2);
    if (ret != AW_OK) {
        return ret;
    }

    switch (p_config->format) {

    case AWSA_PCM_FORMAT_S16_LE:
    case AWSA_PCM_FORMAT_S24_LE:
    case AWSA_PCM_FORMAT_S32_LE:
        dacdiv = AWBL_WM8960_DAC_DIV_1;  /* dac_clk = 256 * fs */
        break;

    default:
        AW_ERRF(("MIMXRT1050-EVK: cannot support foramt\r\n"));
        return -AW_ENOTSUP;
    }

    ret = awsa_soc_dai_clkdiv_set(p_codec_dai,
                                  AWBL_WM8960_CLK_DACDIV,
                                  dacdiv);
    if (ret != AW_OK) {
        return ret;
    }

    /* set codec to slave mode */
    dai_format = AWSA_SOC_DAI_FMT_I2S    |
                 AWSA_SOC_DAI_FMT_NB_NF  |
                 AWSA_SOC_DAI_FMT_CBS_CFS;

    /* set codec DAI configuration */
    ret = awsa_soc_dai_fmt_set(p_codec_dai, dai_format);
    if (ret != AW_OK) {
        return ret;
    }

    /* set cpu to slave mode */
    dai_format = AWSA_SOC_DAI_FMT_I2S    |
                 AWSA_SOC_DAI_FMT_NB_NF  |
                 AWSA_SOC_DAI_FMT_CBM_CFM;

    /* set cpu DAI configuration */
    ret = awsa_soc_dai_fmt_set(p_cpu_dai, dai_format);
    if (ret != AW_OK) {
        return ret;
    }

    return ret;
}

/*******************************************************************************
 Sound capture
*******************************************************************************/

/** \brief widget */
static const struct awsa_soc_dapm_widget_driver
__g_mimxrt1050_evk_sound_cpt_widgets_driver[] = {
    AWSA_SOC_DAPM_MIC("HP MIC",         NULL),
    AWSA_SOC_DAPM_MIC("Main Board MIC", NULL),
};

/** \brief route */
static const
struct awsa_soc_dapm_route __g_mimxrt1050_evk_sound_cpt_map[] = {
    {"LINPUT1",  NULL, "HP MIC"},
    {"LINPUT3",  NULL, "HP MIC"},

    {"RINPUT1",  NULL, "Main Board MIC"},
    {"RINPUT2",  NULL, "Main Board MIC"},

    {"RINPUT2",  NULL, "MICB"},
};

/**
 * \brief MIMXRT1050-EVK close
 */
static
aw_err_t __mimxrt1050_evk_sound_cpt_dai_close (struct awsa_soc_pcm *p_soc_pcm)
{
    struct awsa_soc_codec        *p_codec = p_soc_pcm->p_codec;
    struct awsa_soc_dapm_context *p_dapm  = &p_codec->dapm;

    awsa_soc_dapm_widget_disable(p_dapm, "HP MIC");
    awsa_soc_dapm_widget_disable(p_dapm, "Main Board MIC");
    awsa_soc_dapm_widget_disable(p_dapm, "MICB");
    return AW_OK;
}

/**
 * \brief MIMXRT1050-EVK prepare
 */
static aw_err_t
__mimxrt1050_evk_sound_cpt_dai_prepare (struct awsa_soc_pcm *p_soc_pcm)
{
    struct awsa_soc_codec        *p_codec = p_soc_pcm->p_codec;
    struct awsa_soc_dapm_context *p_dapm  = &p_codec->dapm;
    int                           value   = 1;

    awsa_soc_dapm_widget_enable(p_dapm, "HP MIC");
    awsa_soc_dapm_widget_enable(p_dapm, "Main Board MIC");
    awsa_soc_dapm_widget_enable(p_dapm, "MICB");

    awsa_soc_dapm_mixer_power_update(p_dapm, "Left Boost Switch",  &value);
    awsa_soc_dapm_mixer_power_update(p_dapm, "LINPUT1 Switch",     &value);
    awsa_soc_dapm_mixer_power_update(p_dapm, "LINPUT3 Switch",     &value);

    awsa_soc_dapm_mixer_power_update(p_dapm, "Right Boost Switch", &value);
    awsa_soc_dapm_mixer_power_update(p_dapm, "RINPUT1 Switch",     &value);
    awsa_soc_dapm_mixer_power_update(p_dapm, "RINPUT2 Switch",     &value);
    return AW_OK;
}

/**
 * \brief MIMXRT1050-EVK init
 */
static int __mimxrt1050_evk_sound_cpt_init (struct awsa_soc_pcm *p_soc_pcm)
{
    struct awsa_soc_codec        *p_codec = p_soc_pcm->p_codec;
    struct awsa_soc_dapm_context *p_dapm  = &p_codec->dapm;
    uint32_t                      reg;

    struct awbl_mimxrt1050_evk_sound_dev *p_this = \
        p_soc_pcm->p_card->p_private_data;

    aw_err_t ret;

    /* Add specific widgets */
    ret = awsa_soc_dapm_widgets_add(
              p_dapm,
              p_this->widget_cpt,
              __g_mimxrt1050_evk_sound_cpt_widgets_driver,
              AWSA_SOC_ARRAY_SIZE(__g_mimxrt1050_evk_sound_cpt_widgets_driver),
              NULL);
    if (ret != AW_OK) {
        return ret;
    }

    /* Set up specific audio path audio_mapnects */
    ret = awsa_soc_dapm_routes_add(
              p_dapm,
              p_this->map_cpt,
              __g_mimxrt1050_evk_sound_cpt_map,
              AW_NELEMENTS(__g_mimxrt1050_evk_sound_cpt_map));
    if (ret != AW_OK) {
        return ret;
    }

    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_IFACE2, 0x40);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_LINVOL, &reg);
    awsa_soc_codec_write(p_codec,
                         AWBL_WM8960_REG_LINVOL,
                         (reg & ~(0x80)) | 0x100 | (1 << 6) | 0x3F);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_RINVOL, &reg);
    awsa_soc_codec_write(p_codec,
                         AWBL_WM8960_REG_RINVOL,
                         (reg & ~(0x80)) | 0x100 | (1 << 6) | 0x3F);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_LADC,   &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_LADC,   0x1A0);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_RADC,   &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_RADC,   0x1A0);

    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_NOISEG, 0xf1);
    return AW_OK;
}

/**
 * \brief MIMXRT1050-EVK OPTS
 */
static const
struct awsa_soc_servopts __g_mimxrt1050_evk_sound_cpt_opts = {
    NULL,
    __mimxrt1050_evk_sound_cpt_dai_close,
    NULL,
    NULL,
    __mimxrt1050_evk_sound_dai_config,
    __mimxrt1050_evk_sound_cpt_dai_prepare,
    NULL,
    NULL,
    NULL,
    NULL,
};

/*******************************************************************************
 Sound Playback
*******************************************************************************/

/** \brief widget */
static const struct awsa_soc_dapm_widget_driver
__g_mimxrt1050_evk_sound_pkt_widgets_driver[] = {
    AWSA_SOC_DAPM_HP("HP Jack", NULL),

    AWSA_SOC_DAPM_SPK("SPK_R",  NULL),
    AWSA_SOC_DAPM_SPK("SPK_L",  NULL),
};

/** \brief route */
static const
struct awsa_soc_dapm_route __g_mimxrt1050_evk_sound_pkt_map[] = {
    {"HP Jack",  NULL, "HP_L"},
    {"HP Jack",  NULL, "HP_R"},

    {"SPK_L",    NULL, "SPK_LN"},
    {"SPK_L",    NULL, "SPK_LP"},

    {"SPK_R",    NULL, "SPK_RN"},
    {"SPK_R",    NULL, "SPK_RP"},
};

/**
 * \brief MIMXRT1050-EVK prepare
 */
static aw_err_t
__mimxrt1050_evk_sound_pkt_dai_prepare (struct awsa_soc_pcm *p_soc_pcm)
{
    struct awsa_soc_codec        *p_codec = p_soc_pcm->p_codec;
    struct awsa_soc_dapm_context *p_dapm  = &p_codec->dapm;
    int                           value   = 1;

    awsa_soc_dapm_widget_enable(p_dapm, "HP Jack");
    awsa_soc_dapm_widget_enable(p_dapm, "SPK_R");
    awsa_soc_dapm_widget_enable(p_dapm, "SPK_L");

    awsa_soc_dapm_mixer_power_update(p_dapm,
                                     "Left PCM Playback Switch",
                                     &value);
    awsa_soc_dapm_mixer_power_update(p_dapm,
                                     "Right PCM Playback Switch",
                                     &value);
    return AW_OK;
}

/** \brief jack gpio */
static const
struct awsa_soc_jack_gpio_driver __g_mimxrt1050_evk_sound_jack_gpio_driver = {
    "hp_jack_gpio",
    1,
    0,
    20,
    NULL,
};


/**
 * \brief MIMXRT1050-EVK init
 */
static int __mimxrt1050_evk_sound_pkt_init (struct awsa_soc_pcm *p_soc_pcm)
{
    struct awsa_soc_codec        *p_codec = p_soc_pcm->p_codec;
    struct awsa_soc_dapm_context *p_dapm  = &p_codec->dapm;

    struct awbl_mimxrt1050_evk_sound_dev *p_this = \
        p_soc_pcm->p_card->p_private_data;

    __MIMXRT1050_EVK_SOUND_DEVINFO_DECL(p_devinfo, p_this);

    aw_err_t ret;
    uint32_t reg;

    /* Add specific widgets */
    ret = awsa_soc_dapm_widgets_add(
              p_dapm,
              p_this->widget_pkt,
              __g_mimxrt1050_evk_sound_pkt_widgets_driver,
              AWSA_SOC_ARRAY_SIZE(__g_mimxrt1050_evk_sound_pkt_widgets_driver),
              NULL);
    if (ret != AW_OK) {
        return ret;
    }

    /* Set up specific audio path audio_mapnects */
    ret = awsa_soc_dapm_routes_add(
              p_dapm,
              p_this->map_pkt,
              __g_mimxrt1050_evk_sound_pkt_map,
              AW_NELEMENTS(__g_mimxrt1050_evk_sound_pkt_map));
    if (ret != AW_OK) {
        return ret;
    }

    /* ADCLRC≈‰÷√Œ™GPIOπ¶ƒ‹ */
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_IFACE2, 0x40);

    /* GPIO∂∂∂Ø ±÷”≈‰÷√ */
    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_ADDCTL1, &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_ADDCTL1, reg | 0x1);

    /* GPIO≈‰÷√Œ™ºÏ≤‚ ‰≥ˆ */
    awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_ADDCTL4, &reg);
    reg = (reg & ~(0x7 << 4)) | (0x3 << 4) | 0xc;
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_ADDCTL4, reg);

    awsa_soc_jack_init(&p_this->hp_jack,
                       p_codec,
                       "HP_JD",
                       AWSA_CORE_JACK_EVT_HEADPHONE);

    awsa_soc_jack_gpios_add(&p_this->hp_jack,
                            &p_this->hp_jack_gpio,
                            &__g_mimxrt1050_evk_sound_jack_gpio_driver,
                            1,
                            &p_devinfo->hp_jd);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_LOUT1,  &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_LOUT1,  reg | 0x170);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_ROUT1,  &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_ROUT1,  reg | 0x170);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_LOUT2,  &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_LOUT2,  reg | 0x170);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_ROUT2,  &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_ROUT2,  reg | 0x170);
    return AW_OK;
}

/**
 * \brief MIMXRT1050-EVK OPTS
 */
static const
struct awsa_soc_servopts __g_mimxrt1050_evk_sound_pkt_opts = {
    NULL,
    NULL,
    NULL,
    NULL,
    __mimxrt1050_evk_sound_dai_config,
    __mimxrt1050_evk_sound_pkt_dai_prepare,
    NULL,
    NULL,
    NULL,
    NULL,
};

#if 0

/**
 * \brief DAI link
 */
static const
struct awsa_soc_dai_link __g_mimxrt1050_evk_sound_dai_link[] = {
    {
        "mimxrt1050_evk_link0",
        "mimxrt1050_evk_playback",
        "wm8960_codec0",
        "imxrt1050_sai0_playback",
        "imxrt1050_dai_sai0",
        "wm8960_dai_hifi0",
        __mimxrt1050_evk_sound_pkt_init,
        &__g_mimxrt1050_evk_sound_pkt_opts,
    },
    {
        "mimxrt1050_evk_link0",
        "mimxrt1050_evk_capture",
        "wm8960_codec0",
        "imxrt1050_sai0_capture",
        "imxrt1050_dai_sai0",
        "wm8960_dai_hifi0",
        __mimxrt1050_evk_sound_cpt_init,
        &__g_mimxrt1050_evk_sound_cpt_opts,
    },
};

/**
 * \brief card driver
 */
static const
struct awsa_soc_card_driver __g_mimxrt1050_evk_sound_card_driver = {
    __g_mimxrt1050_evk_sound_dai_link,
    2,
    NULL,
    NULL,
    NULL,
    NULL,
};

#endif

/*******************************************************************************
 AWBus lite
*******************************************************************************/

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __mimxrt1050_evk_sound_inst_init1 (awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __mimxrt1050_evk_sound_inst_init2 (awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __mimxrt1050_evk_sound_inst_connect (awbl_dev_t *p_dev)
{
    __MIMXRT1050_EVK_SOUND_DEV_DECL(p_this, p_dev);
    __MIMXRT1050_EVK_SOUND_DEVINFO_DECL(p_devinfo, p_dev);

    int i;

    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    for (i = 0; i < 2; i++) {
        p_this->link[i].p_name           = p_devinfo->link[i].p_name;
        p_this->link[i].p_stream_name    = p_devinfo->link[i].p_stream_name;
        p_this->link[i].p_codec_name     = p_devinfo->link[i].p_codec_name;
        p_this->link[i].p_plfm_name      = p_devinfo->link[i].p_plfm_name;
        p_this->link[i].p_cpu_dai_name   = p_devinfo->link[i].p_cpu_dai_name;
        p_this->link[i].p_codec_dai_name = p_devinfo->link[i].p_codec_dai_name;
    }

    p_this->link[0].pfn_init = __mimxrt1050_evk_sound_pkt_init;
    p_this->link[0].p_opts   = &__g_mimxrt1050_evk_sound_pkt_opts;
    p_this->link[1].pfn_init = __mimxrt1050_evk_sound_cpt_init;
    p_this->link[1].p_opts   = &__g_mimxrt1050_evk_sound_cpt_opts;

    p_this->card_driver.p_dai_link = \
         (const struct awsa_soc_dai_link *)p_this->link;
    p_this->card_driver.num_links  = 2;

    awsa_soc_card_register(&p_this->soc_card,
                           p_devinfo->card_id,
                           p_devinfo->p_name,
                           p_this->soc_pcm,
                           2,
                           NULL,
                           0,
                           &p_this->card_driver,
                           (void *)p_this);
}

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_mimxrt1050_evk_sound_drvfuncs = {
    __mimxrt1050_evk_sound_inst_init1,
    __mimxrt1050_evk_sound_inst_init2,
    __mimxrt1050_evk_sound_inst_connect
};

/* driver registration (must defined as aw_const) */
aw_local aw_const
awbl_plb_drvinfo_t __g_mimxrt1050_evk_sound_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB,                         /* bus_id */
        AWBL_MIMXRT1050_EVK_SOUND_NAME,         /* p_drvname */
        &__g_mimxrt1050_evk_sound_drvfuncs,     /* p_busfuncs */
        NULL,                                   /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};

/**
 * \brief «˝∂Ø◊¢≤·
 */
void awbl_mimxrt1050_evk_sound_drv_register (void)
{
    awbl_drv_register(
        (struct awbl_drvinfo *)&__g_mimxrt1050_evk_sound_drv_registration);
}

/* end of file */
