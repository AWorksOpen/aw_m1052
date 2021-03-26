/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-11-10  win, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_delay.h"

#include "soc/codec/awbl_uda1380.h"

/*******************************************************************************
  defines
*******************************************************************************/
#define __UDA1380_REG_NUM              0x24

#define __UDA1380_CLK                  0x00
#define __UDA1380_IFACE                0x01
#define __UDA1380_PM                   0x02
#define __UDA1380_AMIX                 0x03
#define __UDA1380_HP                   0x04
#define __UDA1380_MVOL                 0x10
#define __UDA1380_MIXVOL               0x11
#define __UDA1380_MODE                 0x12
#define __UDA1380_DEEMP                0x13
#define __UDA1380_MIXER                0x14
#define __UDA1380_INTSTAT              0x18
#define __UDA1380_DEC                  0x20
#define __UDA1380_PGA                  0x21
#define __UDA1380_ADC                  0x22
#define __UDA1380_AGC                  0x23
#define __UDA1380_DECSTAT              0x28
#define __UDA1380_RESET                0x7f

/* Register flags */
#define __UDA1380_R00_EN_ADC           0x0800
#define __UDA1380_R00_EN_DEC           0x0400
#define __UDA1380_R00_EN_DAC           0x0200
#define __UDA1380_R00_EN_INT           0x0100
#define __UDA1380_R00_DAC_CLK          0x0010
#define __UDA1380_R01_SFORI_I2S        0x0000
#define __UDA1380_R01_SFORI_LSB16      0x0100
#define __UDA1380_R01_SFORI_LSB18      0x0200
#define __UDA1380_R01_SFORI_LSB20      0x0300
#define __UDA1380_R01_SFORI_MSB        0x0500
#define __UDA1380_R01_SFORI_MASK       0x0700
#define __UDA1380_R01_SFORO_I2S        0x0000
#define __UDA1380_R01_SFORO_LSB16      0x0001
#define __UDA1380_R01_SFORO_LSB18      0x0002
#define __UDA1380_R01_SFORO_LSB20      0x0003
#define __UDA1380_R01_SFORO_LSB24      0x0004
#define __UDA1380_R01_SFORO_MSB        0x0005
#define __UDA1380_R01_SFORO_MASK       0x0007
#define __UDA1380_R01_SEL_SOURCE       0x0040
#define __UDA1380_R01_SIM              0x0010
#define __UDA1380_R02_PON_PLL          0x8000
#define __UDA1380_R02_PON_HP           0x2000
#define __UDA1380_R02_PON_DAC          0x0400
#define __UDA1380_R02_PON_BIAS         0x0100
#define __UDA1380_R02_EN_AVC           0x0080
#define __UDA1380_R02_PON_AVC          0x0040
#define __UDA1380_R02_PON_LNA          0x0010
#define __UDA1380_R02_PON_PGAL         0x0008
#define __UDA1380_R02_PON_ADCL         0x0004
#define __UDA1380_R02_PON_PGAR         0x0002
#define __UDA1380_R02_PON_ADCR         0x0001
#define __UDA1380_R13_MTM              0x4000
#define __UDA1380_R14_SILENCE          0x0080
#define __UDA1380_R14_SDET_ON          0x0040
#define __UDA1380_R21_MT_ADC           0x8000
#define __UDA1380_R22_SEL_LNA          0x0008
#define __UDA1380_R22_SEL_MIC          0x0004
#define __UDA1380_R22_SKIP_DCFIL       0x0002
#define __UDA1380_R23_AGC_EN           0x0001

#define __UDA1380_RATES (AWSA_PCM_RATE_8000  | \
                         AWSA_PCM_RATE_11025 | \
                         AWSA_PCM_RATE_16000 | \
                         AWSA_PCM_RATE_22050 | \
                         AWSA_PCM_RATE_44100 | \
                         AWSA_PCM_RATE_48000)

/* declare sim device instance */
#define __UDA1380_DEV_DECL(p_this, p_dev) \
    struct awbl_uda1380_dev *p_this = \
        (struct awbl_uda1380_dev *)(p_dev)

/* declare sim device infomation */
#define __UDA1380_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_uda1380_devinfo *p_devinfo = \
        (struct awbl_uda1380_devinfo *)AWBL_DEVINFO_GET(p_dev)

/*******************************************************************************
  forward declarations
*******************************************************************************/
static void __uda1380_inst_init1   (awbl_dev_t *p_dev);
static void __uda1380_inst_init2   (awbl_dev_t *p_dev);
static void __uda1380_inst_connect (awbl_dev_t *p_dev);

static aw_err_t __uda1380_probe   (struct awsa_soc_codec *p_codec);

static aw_err_t __uda1380_reg_cache_read (struct awsa_soc_codec *p_codec,
                                          uint32_t               reg,
                                          uint32_t              *p_value);

static void __uda1380_reg_cache_write (struct awsa_soc_codec *p_codec,
                                       uint16_t               reg,
                                       uint32_t               value);

static aw_err_t __uda1380_write (struct awsa_soc_codec *p_codec,
                                 uint32_t               reg,
                                 uint32_t               value);

static
aw_err_t __uda1380_bias_level_set (struct awsa_soc_codec    *p_codec,
                                   enum awsa_soc_bias_level  level);

static aw_err_t __uda1380_dai_clkdiv_set (struct awsa_soc_dai *p_dai,
                                          int                  clk_id,
                                          int                  div);

static aw_err_t __uda1380_dai_fmt_both_set (struct awsa_soc_dai *p_dai,
                                            uint32_t             fmt);

static
aw_err_t __uda1380_dai_fmt_playback_set (struct awsa_soc_dai *p_dai,
                                         uint32_t             fmt);

static
aw_err_t __uda1380_dai_fmt_capture_set (struct awsa_soc_dai *p_dai,
                                        uint32_t             fmt);

static
aw_err_t __uda1380_trigger (struct awsa_soc_dai *p_dai,
                            int                  cmd);

static
aw_err_t __uda1380_pcm_shutdown (struct awsa_soc_dai *p_dai,
                                 awsa_pcm_stream_t    stream);

static
aw_err_t __uda1380_pcm_config (struct awsa_soc_dai    *p_dai,
                               awsa_pcm_stream_t       stream,
                               struct awsa_pcm_config *p_config);

/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_uda1380_drvfuncs = {
    __uda1380_inst_init1,
    __uda1380_inst_init2,
    __uda1380_inst_connect
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_i2c_drvinfo_t __g_uda1380_drv_registration = {
    {
        AWBL_VER_1,                     /* awb_ver */
        AWBL_BUSID_I2C,                 /* bus_id */
        AWBL_UDA1380_NAME,              /* p_drvname */
        &__g_uda1380_drvfuncs,          /* p_busfuncs */
        NULL,                           /* p_methods */
        NULL                            /* pfunc_drv_probe */
    }
};

/******************************************************************************/

/**
 * \brief Start of section armcc warning: extented constant initialiser used
 * @{
 */

#if defined(__CC_ARM)
  #pragma push
  #pragma diag_suppress 1296
#elif defined(__ICCARM__)
#elif defined(__GNUC__)
#elif defined(__TMS470__)
#elif defined(__TASKING__)
#endif

/** @} */

/* declarations of ALSA reg_elem_REAL controls */
static const char *__gp_uda1380_deemp[] = {
    "None",
    "32kHz",
    "44.1kHz",
    "48kHz",
    "96kHz",
};

/**
 * \brief deemp De-emphasis : off, 32kHz, 44.1kHz, 48kHz, 96kHz
 */
static const
struct awsa_soc_enum __g_uda1380_deemp_enum[] = {
    AWSA_SOC_ENUM_SINGLE(__UDA1380_DEEMP, 8, 5, __gp_uda1380_deemp),
    AWSA_SOC_ENUM_SINGLE(__UDA1380_DEEMP, 0, 5, __gp_uda1380_deemp),
};

/*
 * from -48 dB in 1.5 dB steps (mute instead of -49.5 dB)
 */
static const AWSA_CORE_TLV_DB_SCALE_DECL(amix_tlv, -4950, 150, 1);

/* from -63 to 24 dB in 0.5 dB steps (-128...48) */
static const AWSA_CORE_TLV_DB_SCALE_DECL(dec_tlv, -6400, 50, 1);

/*
 * from -78 dB in 1 dB steps (3 dB steps, really. LSB are ignored),
 * from -66 dB in 0.5 dB steps (2 dB steps, really) and
 * from -52 dB in 0.25 dB steps
 */
static const uint32_t mvol_tlv[] = {
    AWSA_CORE_TLV_DB_RANGE_HEAD(4),
    0,  7,   AWSA_CORE_TLV_DB_SCALE_ITEM(-8600, 150, 1),
    8,  15,  AWSA_CORE_TLV_DB_SCALE_ITEM(-7200, 75,  1),
    16, 43,  AWSA_CORE_TLV_DB_SCALE_ITEM(-6600, 50,  0),
    44, 252, AWSA_CORE_TLV_DB_SCALE_ITEM(-5200, 25,  0),
};

/*
 * from -72 dB in 1.5 dB steps (6 dB steps really),
 * from -66 dB in 0.75 dB steps (3 dB steps really),
 * from -60 dB in 0.5 dB steps (2 dB steps really) and
 * from -46 dB in 0.25 dB steps
 */
static const uint32_t vc_tlv[] = {
    AWSA_CORE_TLV_DB_RANGE_HEAD(4),
    0,  34,  AWSA_CORE_TLV_DB_SCALE_ITEM(-11850, 150, 1),
    35, 42,  AWSA_CORE_TLV_DB_SCALE_ITEM(-6600,  75,  0),
    43, 70,  AWSA_CORE_TLV_DB_SCALE_ITEM(-6000,  50,  0),
    71, 255, AWSA_CORE_TLV_DB_SCALE_ITEM(-4600,  25,  0),
};

/* from 0 to 24 dB in 3 dB steps */
static const AWSA_CORE_TLV_DB_SCALE_DECL(pga_tlv, 0, 300, 0);

/* from 0 to 30 dB in 2 dB steps */
static const AWSA_CORE_TLV_DB_SCALE_DECL(vga_tlv, 0, 200, 0);

/**
 * \brief ctls
 */
static const
struct awsa_core_ctl_elem_driver __g_uda1380_ctl_elems_driver[] = {

    /* AVCR, AVCL */
    AWSA_SOC_DOUBLE_TLV(
        "Analog Mixer Volume", __UDA1380_AMIX, 0, 8, 44, 1, amix_tlv),

    /* MVCL, MVCR */
    AWSA_SOC_DOUBLE_TLV(
        "Master Playback Volume", __UDA1380_MVOL, 0, 8, 252, 1, mvol_tlv),

    /* VC2 */
    AWSA_SOC_SINGLE_TLV(
        "ADC Playback Volume", __UDA1380_MIXVOL, 8, 255, 1, vc_tlv),

    /* VC1 */
    AWSA_SOC_SINGLE_TLV(
        "PCM Playback Volume", __UDA1380_MIXVOL, 0, 255, 1, vc_tlv),

    /* MTM */
    AWSA_SOC_SINGLE("Master Playback Switch", __UDA1380_DEEMP, 14, 1, 1),

    /* MT2 from decimation filter */
    AWSA_SOC_SINGLE("ADC Playback Switch", __UDA1380_DEEMP, 11, 1, 1),

    /* DE2 */
    AWSA_SOC_ENUM("ADC Playback De-emphasis", __g_uda1380_deemp_enum[0]),

    /* MT1, from digital data input */
    AWSA_SOC_SINGLE("PCM Playback Switch", __UDA1380_DEEMP, 3, 1, 1),

    /* DE1 */
    AWSA_SOC_ENUM("PCM Playback De-emphasis", __g_uda1380_deemp_enum[1]),

    /* ML_DEC, MR_DEC */
    AWSA_SOC_DOUBLE_S8_TLV(
        "ADC Capture Volume", __UDA1380_DEC, -128, 48, dec_tlv),

    /* MT_ADC */
    AWSA_SOC_SINGLE("ADC Capture Switch", __UDA1380_PGA, 15, 1, 1),

    /* PGA_GAINCTRLL, PGA_GAINCTRLR */
    AWSA_SOC_DOUBLE_TLV(
        "Line Capture Volume", __UDA1380_PGA, 0, 8, 8, 0, pga_tlv),

    /* ADCPOL_INV */
    AWSA_SOC_SINGLE(
        "ADC Polarity inverting Switch", __UDA1380_ADC, 12, 1, 0),

    /* VGA_CTRL */
    AWSA_SOC_SINGLE_TLV(
        "Mic Capture Volume", __UDA1380_ADC, 8, 15, 0, vga_tlv),

    /* SKIP_DCFIL (before decimator) */
    AWSA_SOC_SINGLE("DC Filter Bypass Switch", __UDA1380_ADC, 1, 1, 0),

    /* EN_DCFIL (at output of decimator) */
    AWSA_SOC_SINGLE("DC Filter Enable Switch", __UDA1380_ADC, 0, 1, 0),

    /* enum */
    AWSA_SOC_SINGLE("AGC Timing", __UDA1380_AGC, 8, 7, 0),

    /* AGC_LEVEL */
    AWSA_SOC_SINGLE("AGC Target level", __UDA1380_AGC, 2, 3, 1),

    /* -5.5, -8, -11.5, -14 dBFS */
    AWSA_SOC_SINGLE("AGC Switch", __UDA1380_AGC, 0, 1, 0),
};

/******************************************************************************/

static const char *__gp_uda1380_input_sel[] = {
    "Line",
    "Mic + Line R",
    "Line L",
    "Mic",
};

static const char *__gp_uda1380_output_sel[] = {
    "DAC",
    "Analog Mixer",
};

static const char *__gp_uda1380_capture_sel[] = {
    "ADC",
    "Digital Mixer"
};

/******************************************************************************/

/* SEL_MIC, SEL_LNA */
static const struct awsa_soc_enum __g_uda1380_input_sel_enum =
    AWSA_SOC_ENUM_SINGLE(__UDA1380_ADC, 2, 4, __gp_uda1380_input_sel);

/* R02_EN_AVC */
static const struct awsa_soc_enum __g_uda1380_output_sel_enum =
    AWSA_SOC_ENUM_SINGLE(__UDA1380_PM, 7, 2, __gp_uda1380_output_sel);

/* SEL_SOURCE */
static const struct awsa_soc_enum __g_uda1380_capture_sel_enum =
    AWSA_SOC_ENUM_SINGLE(__UDA1380_IFACE, 6, 2, __gp_uda1380_capture_sel);

/* Input mux */
static const struct awsa_core_ctl_elem_driver __g_uda1380_input_mux_ctl =
    AWSA_SOC_DAPM_ENUM("Input Route", __g_uda1380_input_sel_enum);

/* Output mux */
static const struct awsa_core_ctl_elem_driver __g_uda1380_output_mux_ctl =
    AWSA_SOC_DAPM_ENUM("Output Route", __g_uda1380_output_sel_enum);

/* Capture mux */
static const struct awsa_core_ctl_elem_driver __g_uda1380_capture_mux_ctl =
    AWSA_SOC_DAPM_ENUM("Capture Route", __g_uda1380_capture_sel_enum);

/* widget */
static const
struct awsa_soc_dapm_widget_driver __g_uda1380_dapm_widgets_driver[] = {
    AWSA_SOC_DAPM_MUX(
        "Input Mux", AWSA_SOC_NOPM, 0, 0, &__g_uda1380_input_mux_ctl),

    AWSA_SOC_DAPM_MUX(
        "Output Mux", AWSA_SOC_NOPM, 0, 0, &__g_uda1380_output_mux_ctl),

    AWSA_SOC_DAPM_MUX(
        "Capture Mux", AWSA_SOC_NOPM, 0, 0, &__g_uda1380_capture_mux_ctl),

    AWSA_SOC_DAPM_PGA("Left PGA", __UDA1380_PM, 3, 0, NULL, 0),
    AWSA_SOC_DAPM_PGA("Right PGA", __UDA1380_PM, 1, 0, NULL, 0),
    AWSA_SOC_DAPM_PGA("Mic LNA", __UDA1380_PM, 4, 0, NULL, 0),
    AWSA_SOC_DAPM_ADC("Left ADC", "Left Capture", __UDA1380_PM, 2, 0),
    AWSA_SOC_DAPM_ADC("Right ADC", "Right Capture", __UDA1380_PM, 0, 0),
    AWSA_SOC_DAPM_INPUT("VINM"),
    AWSA_SOC_DAPM_INPUT("VINL"),
    AWSA_SOC_DAPM_INPUT("VINR"),
    AWSA_SOC_DAPM_MIXER("Analog Mixer", __UDA1380_PM, 6, 0, NULL, 0),
    AWSA_SOC_DAPM_OUTPUT("VOUTLHP"),
    AWSA_SOC_DAPM_OUTPUT("VOUTRHP"),
    AWSA_SOC_DAPM_OUTPUT("VOUTL"),
    AWSA_SOC_DAPM_OUTPUT("VOUTR"),
    AWSA_SOC_DAPM_DAC("DAC", "Playback", __UDA1380_PM, 10, 0),
    AWSA_SOC_DAPM_PGA("HeadPhone Driver", __UDA1380_PM, 13, 0, NULL, 0),
};

/**
 * \brief End of section using anonymous unions
 * @{
 */

#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
#elif defined(__GNUC__)
#elif defined(__TMS470__)
#elif defined(__TASKING__)
#endif

/** @} */

/* route */
static const struct awsa_soc_dapm_route __g_uda1380_audio_map[] = {

    /* output mux */
    {"HeadPhone Driver", NULL,           "Output Mux"},
    {"VOUTR",            NULL,           "Output Mux"},
    {"VOUTL",            NULL,           "Output Mux"},

    {"Analog Mixer",     NULL,           "VINR"},
    {"Analog Mixer",     NULL,           "VINL"},
    {"Analog Mixer",     NULL,           "DAC"},

    {"Output Mux",       "DAC",          "DAC"},
    {"Output Mux",       "Analog Mixer", "Analog Mixer"},

    /* {"DAC", "Digital Mixer", "I2S" } */

    /* headphone driver */
    {"VOUTLHP",          NULL,           "HeadPhone Driver"},
    {"VOUTRHP",          NULL,           "HeadPhone Driver"},

    /* input mux */
    {"Left ADC",         NULL,           "Input Mux"},
    {"Input Mux",        "Mic",          "Mic LNA"},
    {"Input Mux",        "Mic + Line R", "Mic LNA"},
    {"Input Mux",        "Line L",       "Left PGA"},
    {"Input Mux",        "Line",         "Left PGA"},

    /* right input */
    {"Right ADC",        "Mic + Line R", "Right PGA"},
    {"Right ADC",        "Line",         "Right PGA"},

    /* inputs */
    {"Mic LNA",          NULL,           "VINM"},
    {"Left PGA",         NULL,           "VINL"},
    {"Right PGA",        NULL,           "VINR"},
};

/* playabck & capture */
static const struct awsa_soc_dai_servopts __g_uda1380_dai_opts = {
    NULL,
    NULL,
    __uda1380_dai_clkdiv_set,
    __uda1380_dai_fmt_both_set,
    NULL,
    NULL,
    __uda1380_pcm_shutdown,
    __uda1380_pcm_config,
    NULL,
    __uda1380_trigger,
    NULL
};

/* playback */
static const struct awsa_soc_dai_servopts __g_uda1380_dai_opts_playback = {
    NULL,
    NULL,
    __uda1380_dai_clkdiv_set,
    __uda1380_dai_fmt_playback_set,
    NULL,
    NULL,
    __uda1380_pcm_shutdown,
    __uda1380_pcm_config,
    NULL,
    __uda1380_trigger,
    NULL
};

/* capture */
static const struct awsa_soc_dai_servopts __g_uda1380_dai_opts_capture = {
    NULL,
    NULL,
    __uda1380_dai_clkdiv_set,
    __uda1380_dai_fmt_capture_set,
    NULL,
    NULL,
    __uda1380_pcm_shutdown,
    __uda1380_pcm_config,
    NULL,
    __uda1380_trigger,
    NULL
};

/* dai driver */
static const struct awsa_soc_dai_driver __g_uda1380_dai_driver[] = {

    /* both playback & cpature */
    {
        &__g_uda1380_dai_opts,
        {
            "Capture",
            AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_S16_LE),
            __UDA1380_RATES,
            AWSA_PCM_RATE_8000,
            AWSA_PCM_RATE_48000,
            1,
            2
        },
        {
            "Playback",
            AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_S16_LE),
            __UDA1380_RATES,
            AWSA_PCM_RATE_8000,
            AWSA_PCM_RATE_48000,
            1,
            2
        },
        0,
        NULL,
        NULL,
    },

    /* playback only - dual interface */
    {
        &__g_uda1380_dai_opts_playback,
        {
            NULL, 0, 0, 0, 0, 0, 0
        },
        {
            "Playback",
            AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_S16_LE),
            __UDA1380_RATES,
            AWSA_PCM_RATE_8000,
            AWSA_PCM_RATE_48000,
            1,
            2
        },
        0,
        NULL,
        NULL,
    },

    /* capture only - dual interface*/
    {
        &__g_uda1380_dai_opts_capture,
        {
            "Capture",
            AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_S16_LE),
            __UDA1380_RATES,
            AWSA_PCM_RATE_8000,
            AWSA_PCM_RATE_48000,
            1,
            2
        },
        {
            NULL, 0, 0, 0, 0, 0, 0
        },
        0,
        NULL,
        NULL,
    },
};

static aw_err_t __uda1380_dai_clkdiv_set (struct awsa_soc_dai *p_dai,
                                          int                  clk_id,
                                          int                  div)
{
    struct awsa_soc_codec *p_codec = p_dai->plfm_codec.p_codec;
    uint32_t               clk     = 0;

    __uda1380_reg_cache_read(p_codec, __UDA1380_CLK, &clk);

    if (clk_id == AWBL_UDA1380_DAC_CLK_SYSCLK) {
        switch (div) {

        case 384:
            clk |= (0x1 << 2);
            break;

        case 512:
            clk |= (0x2 << 2);
            break;

        case 768:
            clk |= (0x3 << 2);
            break;

        case 256:
        default:
            break;
        }
    }

    __uda1380_write(p_codec, __UDA1380_CLK, clk);
    return AW_OK;
}

static aw_err_t __uda1380_dai_fmt_both_set (struct awsa_soc_dai *p_dai,
                                            uint32_t             fmt)
{
    struct awsa_soc_codec *p_codec = p_dai->plfm_codec.p_codec;
    uint32_t               iface   = 0;

    /* set up DAI based upon fmt */
    __uda1380_reg_cache_read(p_codec, __UDA1380_IFACE, &iface);
    iface &= ~(__UDA1380_R01_SFORI_MASK |
               __UDA1380_R01_SIM        |
               __UDA1380_R01_SFORO_MASK);

    switch (fmt & AWSA_SOC_DAI_FMT_MASK) {

    case AWSA_SOC_DAI_FMT_I2S:
        iface |= (__UDA1380_R01_SFORI_I2S   | __UDA1380_R01_SFORO_I2S);
        break;

    case AWSA_SOC_DAI_FMT_LSB:
        iface |= (__UDA1380_R01_SFORI_LSB16 | __UDA1380_R01_SFORO_LSB16);
        break;

    case AWSA_SOC_DAI_FMT_MSB:
        iface |= (__UDA1380_R01_SFORI_MSB   | __UDA1380_R01_SFORO_MSB);
        break;

    default:
        break;
    }

    /* DATAI is slave only, so in single-link mode, this has to be slave */
    if ((fmt & AWSA_SOC_DAI_FMT_MASTER_MASK) != AWSA_SOC_DAI_FMT_CBS_CFS) {
        return -AW_EINVAL;
    }

    __uda1380_write(p_codec, __UDA1380_IFACE, iface);
    return AW_OK;
}

static
aw_err_t __uda1380_dai_fmt_playback_set (struct awsa_soc_dai *p_dai,
                                         uint32_t             fmt)
{
    struct awsa_soc_codec *p_codec = p_dai->plfm_codec.p_codec;
    uint32_t               iface   = 0;

    /* set up DAI based upon fmt */
    __uda1380_reg_cache_read(p_codec, __UDA1380_IFACE, &iface);
    iface &= ~__UDA1380_R01_SFORI_MASK;

    switch (fmt & AWSA_SOC_DAI_FMT_MASK) {

    case AWSA_SOC_DAI_FMT_I2S:
        iface |= __UDA1380_R01_SFORI_I2S;
        break;

    case AWSA_SOC_DAI_FMT_LSB:
        iface |= __UDA1380_R01_SFORI_LSB16;
        break;

    case AWSA_SOC_DAI_FMT_MSB:
        iface |= __UDA1380_R01_SFORI_MSB;
        break;

    default:
        break;
    }

    /* DATAI is slave only, so this has to be slave */
    if ((fmt & AWSA_SOC_DAI_FMT_MASTER_MASK) !=
               AWSA_SOC_DAI_FMT_CBS_CFS) {
        return -AW_EINVAL;
    }

    __uda1380_write(p_codec, __UDA1380_IFACE, iface);
    return AW_OK;
}

static
aw_err_t __uda1380_dai_fmt_capture_set (struct awsa_soc_dai *p_dai,
                                        uint32_t             fmt)
{
    struct awsa_soc_codec *p_codec = p_dai->plfm_codec.p_codec;
    uint32_t               iface   = 0;

    /* set up DAI based upon fmt */
    __uda1380_reg_cache_read(p_codec, __UDA1380_IFACE, &iface);
    iface &= ~(__UDA1380_R01_SIM | __UDA1380_R01_SFORO_MASK);

    switch (fmt & AWSA_SOC_DAI_FMT_MASK) {

    case AWSA_SOC_DAI_FMT_I2S:
        iface |= __UDA1380_R01_SFORO_I2S;
        break;

    case AWSA_SOC_DAI_FMT_LSB:
        iface |= __UDA1380_R01_SFORO_LSB16;
        break;

    case AWSA_SOC_DAI_FMT_MSB:
        iface |= __UDA1380_R01_SFORO_MSB;
        break;

    default:
        break;
    }

    if ((fmt & AWSA_SOC_DAI_FMT_MASTER_MASK) == AWSA_SOC_DAI_FMT_CBM_CFM) {
        iface |= __UDA1380_R01_SIM;
    }

    __uda1380_write(p_codec, __UDA1380_IFACE, iface);
    return AW_OK;
}

static
aw_err_t __uda1380_trigger (struct awsa_soc_dai *p_dai,
                            int                  cmd)
{
    struct awsa_soc_codec *p_codec = p_dai->plfm_codec.p_codec;
    uint32_t               mixer   = 0;
    uint32_t               deemp   = 0;
    int                    i;

    __UDA1380_DEV_DECL(p_this, p_codec->p_private_data);

    __uda1380_reg_cache_read(p_codec, __UDA1380_DEEMP, &deemp);
    __uda1380_reg_cache_read(p_codec, __UDA1380_MIXER, &mixer);

    switch (cmd) {

    case AWSA_CORE_PCM_TRIGGER_START:
    case AWSA_CORE_PCM_TRIGGER_PAUSE_RELEASE:

        __uda1380_write(p_codec,
                        __UDA1380_DEEMP,
                        (deemp & ~__UDA1380_R13_MTM));

        __uda1380_write(p_codec,
                        __UDA1380_MIXER,
                        (mixer & ~__UDA1380_R14_SILENCE));

        for (i = 0x10; i < __UDA1380_REG_NUM; i++) {
            if ((1UL << (i - 0x10)) & p_this->cache_dirty) {
                __uda1380_write(p_codec, i, p_this->reg_cache[i]);
            }
        }
        break;

    case AWSA_CORE_PCM_TRIGGER_STOP:
    case AWSA_CORE_PCM_TRIGGER_PAUSE_PUSH:

//        __uda1380_write(p_codec,
//                        __UDA1380_MIXER,
//                        (mixer | __UDA1380_R14_SILENCE));
//
//        __uda1380_write(p_codec,
//                        __UDA1380_DEEMP,
//                        (deemp | __UDA1380_R13_MTM));

        for (i = 0x10; i < __UDA1380_REG_NUM; i++) {
            if ((1UL << (i - 0x10)) & p_this->cache_dirty) {
                __uda1380_write(p_codec, i, p_this->reg_cache[i]);
            }
        }
        break;

    default:
        break;
    }

    return AW_OK;
}

static
aw_err_t __uda1380_pcm_shutdown (struct awsa_soc_dai *p_dai,
                                 awsa_pcm_stream_t    stream)
{
    struct awsa_soc_codec *p_codec = p_dai->plfm_codec.p_codec;
    uint32_t               clk = 0, pm = 0;

    __uda1380_reg_cache_read(p_codec, __UDA1380_CLK, &clk);

    /* shut down WSPLL power if running from this clock */
    if (clk & __UDA1380_R00_DAC_CLK) {
        __uda1380_reg_cache_read(p_codec, __UDA1380_PM, &pm);
        __uda1380_write(p_codec, __UDA1380_PM, ~__UDA1380_R02_PON_PLL & pm);
    }

    if (stream == AWSA_PCM_STREAM_PLAYBACK) {
        clk &= ~(__UDA1380_R00_EN_DAC | __UDA1380_R00_EN_INT);
    } else {
        clk &= ~(__UDA1380_R00_EN_ADC | __UDA1380_R00_EN_DEC);
    }

    __uda1380_write(p_codec, __UDA1380_CLK, clk);
    return AW_OK;
}

static
aw_err_t __uda1380_pcm_config (struct awsa_soc_dai    *p_dai,
                               awsa_pcm_stream_t       stream,
                               struct awsa_pcm_config *p_config)
{
    struct awsa_soc_codec *p_codec = p_dai->plfm_codec.p_codec;
    uint32_t               clk = 0, pm = 0;

    __uda1380_reg_cache_read(p_codec, __UDA1380_CLK, &clk);

    /* set WSPLL power and divider if running from this clock */
    if (clk & __UDA1380_R00_DAC_CLK) {
        __uda1380_reg_cache_read(p_codec, __UDA1380_PM, &pm);

        /* clear SEL_LOOP_DIV */
        clk &= ~0x3;

        if (p_config->rate >= 6250 && p_config->rate <= 12500) {
            clk |= 0x0;
        } else if (p_config->rate > 12500 && p_config->rate <= 25000) {
            clk |= 0x1;
        } else if (p_config->rate > 25000 && p_config->rate <= 50000) {
            clk |= 0x2;
        } else if (p_config->rate > 50000 && p_config->rate <= 100000) {
            clk |= 0x3;
        }

        __uda1380_write(p_codec, __UDA1380_PM, __UDA1380_R02_PON_PLL | pm);
    }

    if (stream == AWSA_PCM_STREAM_PLAYBACK) {
        clk |= __UDA1380_R00_EN_DAC | __UDA1380_R00_EN_INT;
    } else {
        clk |= __UDA1380_R00_EN_ADC | __UDA1380_R00_EN_DEC;
    }

    __uda1380_write(p_codec, __UDA1380_CLK, clk);
    return AW_OK;
}

static aw_err_t __uda1380_reset (struct awsa_soc_codec *p_codec)
{
    uint8_t  data[3];
    aw_err_t ret;

    __UDA1380_DEV_DECL(p_this, p_codec->p_private_data);
    __UDA1380_DEVINFO_DECL(p_devinfo, p_this);

    if (p_devinfo->gpio_is_valid) {

        /* reset gpio pin cause a reset, reset time must be at least 1us */
        aw_gpio_set(p_devinfo->gpio_reset, 1);
        aw_mdelay(1);
        aw_gpio_set(p_devinfo->gpio_reset, 0);
    } else {

        data[0] = 0;
        data[1] = 0;

        ret = awbl_i2c_write((struct awbl_i2c_device *)p_this,
                             AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                             p_devinfo->addr,
                             __UDA1380_RESET,
                            (uint8_t*)data,
                             2);
        if (ret != AW_OK) {
            AW_DBGF(("%s: write to reg 0x%x failed\n",
                      __func__, __UDA1380_RESET));
        }
    }

    return AW_OK;
}

/******************************************************************************/

/*
 * uda1380 register cache
 */
static const uint16_t __g_uda1380_default_reg[__UDA1380_REG_NUM] = {
    0x0502, 0x0000, 0x0000, 0x3f3f,
    0x0202, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0xff00, 0x0000, 0x4800,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x8000, 0x0002, 0x0000,
};

/* codec driver */
static const struct awsa_soc_codec_driver __g_uda1380_codec_driver = {
#if 0
    __UDA1380_REG_NUM,
    1,
    sizeof(uint16_t),
    __g_uda1380_default_reg,
#endif
    __uda1380_probe,
    NULL,
    __uda1380_reg_cache_read,
    __uda1380_write,
    __uda1380_bias_level_set,
};

static aw_err_t __uda1380_probe (struct awsa_soc_codec *p_codec)
{
    __UDA1380_DEV_DECL(p_this, p_codec->p_private_data);
    __UDA1380_DEVINFO_DECL(p_devinfo, p_this);

    aw_err_t ret;
    //uint32_t clk;

    memcpy(p_this->reg_cache,
           __g_uda1380_default_reg,
           __UDA1380_REG_NUM * 2);

    if (p_devinfo->gpio_is_valid) {
        ret = aw_gpio_pin_cfg(p_devinfo->gpio_reset, AW_GPIO_OUTPUT_INIT_LOW);
        if (ret != AW_OK) {
            goto __err;
        }
    }

    if (p_devinfo->gpio_is_valid) {
        aw_gpio_pin_cfg(p_devinfo->gpio_power, AW_GPIO_OUTPUT_INIT_LOW);
        if (ret != AW_OK) {
            goto __err;
        }
    } else {
        ret = __uda1380_reset(p_codec);
        if (ret != AW_OK) {
            goto __err;
        }
    }

    /* power on device */
    __uda1380_bias_level_set(p_codec, AWSA_SOC_BIAS_STANDBY);

//    __uda1380_reg_cache_read(p_codec, __UDA1380_CLK, &clk);
//
//    /* set clock input */
//    switch (p_devinfo->dac_clk) {
//
//    case AWBL_UDA1380_DAC_CLK_SYSCLK :
//        __uda1380_write(p_codec, __UDA1380_CLK, clk & ~__UDA1380_R00_DAC_CLK);
//        break;
//
//    case AWBL_UDA1380_DAC_CLK_WSPLL:
//        __uda1380_write(p_codec, __UDA1380_CLK, __UDA1380_R00_DAC_CLK);
//        break;
//
//    default :
//        break;
//    }

    awsa_soc_codec_ctl_elem_add(
        p_codec,
        p_this->ctl_elem,
        __g_uda1380_ctl_elems_driver,
        AWSA_SOC_ARRAY_SIZE(__g_uda1380_ctl_elems_driver));

    awsa_soc_dapm_widgets_add(
        &p_codec->dapm,
        p_this->widget,
        __g_uda1380_dapm_widgets_driver,
        AWSA_SOC_ARRAY_SIZE(__g_uda1380_dapm_widgets_driver),
        p_this->widget_ctl_elem);

    awsa_soc_dapm_routes_add(&p_codec->dapm,
                             p_this->path,
                             __g_uda1380_audio_map,
                             AWSA_SOC_ARRAY_SIZE(__g_uda1380_audio_map));
    return AW_OK;

__err:
    return ret;
}

#if 0
static aw_err_t __uda1380_read (struct awsa_soc_codec *p_codec,
                                uint32_t               reg,
                                uint32_t              *p_value)
{
    uint8_t      data[2];
    aw_err_t     ret;

    __UDA1380_DEV_DECL(p_this, p_codec->p_private_data);
    __UDA1380_DEVINFO_DECL(p_devinfo, p_this);

    ret = awbl_i2c_read((struct awbl_i2c_device *)p_this,
                        AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                        p_devinfo->addr,
                        reg,
                        data,
                        2);
    if (ret != AW_OK) {
        AW_INFOF(("%s: read reg 0x%04x failed!\n", __func__, reg));
        return ret;
    }

    *p_value = (data[0]<<8) | data[1];
    return ret;
}
#endif

/*
 * read uda1380 register cache
 */
static aw_err_t __uda1380_reg_cache_read (struct awsa_soc_codec *p_codec,
                                          uint32_t               reg,
                                          uint32_t              *p_value)
{
    __UDA1380_DEV_DECL(p_this, p_codec->p_private_data);

    if (reg == __UDA1380_RESET) {
        *p_value = 0;
    }else if (reg >= __UDA1380_REG_NUM) {
        *p_value = (uint32_t)-1;
    } else {
        *p_value = p_this->reg_cache[reg];
    }

    AW_INFOF(("uda1380: read reg %x <= %x\r\n", reg, *p_value));

    return AW_OK;
}

/*
 * write uda1380 register cache
 */
static void __uda1380_reg_cache_write (struct awsa_soc_codec *p_codec,
                                       uint16_t               reg,
                                       uint32_t               value)
{
    __UDA1380_DEV_DECL(p_this, p_codec->p_private_data);

    if (reg >= __UDA1380_REG_NUM) {
        return;
    }

    if ((reg >= 0x10) && (p_this->reg_cache[reg] != value)) {

        aw_spinlock_isr_take(&p_this->lock);
        p_this->cache_dirty |= (1UL << (reg - 0x10));
        aw_spinlock_isr_give(&p_this->lock);
    }

    p_this->reg_cache[reg] = value;
}

static aw_err_t __uda1380_write (struct awsa_soc_codec *p_codec,
                                 uint32_t               reg,
                                 uint32_t               value)
{
    uint8_t   data[3];
    aw_err_t  ret;
    uint32_t  val;

    __UDA1380_DEV_DECL(p_this, p_codec->p_private_data);
    __UDA1380_DEVINFO_DECL(p_devinfo, p_this);

    /* write cache */
    __uda1380_reg_cache_write(p_codec, reg, value);

    /*
     * data is
     *   data[0] is register offset
     *   data[1] is MS byte
     *   data[2] is LS byte
     */
    data[0] = (value & 0xff00) >> 8;
    data[1] = value & 0x00ff;

    /*
     * the interpolator & decimator regs must only be written when the
     * codec DAI is active.
     */
    if (!p_codec->active || (reg >= __UDA1380_REG_NUM)) {
        return AW_OK;
    }

    ret = awbl_i2c_write((struct awbl_i2c_device *)p_this,
                         AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                         p_devinfo->addr,
                         reg,
                        (uint8_t*)data,
                         2);
    if (ret != AW_OK) {
        AW_INFOF(("%s: write reg 0x%04x failed\n", __func__, reg));
        return ret;
    }

    data[0] = data[1] = 0;

    ret = awbl_i2c_read((struct awbl_i2c_device *)p_this,
                        AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                        p_devinfo->addr,
                        reg,
                        data,
                        2);
    if (ret != AW_OK) {
        AW_INFOF(("%s: read reg 0x%04x failed\n", __func__, reg));
        return ret;
    }

    val = (data[0] << 8) | data[1];
    if (val != value) {
        AW_INFOF(("uda1380: 0x%04x read back value %x\n",
                  reg,
                  (data[0] << 8) | data[1]));
        return -AW_EIO;
    }

    AW_INFOF(("uda1380: write reg %x => %x\r\n", reg, value));

    aw_spinlock_isr_take(&p_this->lock);
    p_this->cache_dirty &= ~(1UL << (reg - 0x10));
    aw_spinlock_isr_give(&p_this->lock);
    return AW_OK;
}

static
aw_err_t __uda1380_bias_level_set (struct awsa_soc_codec    *p_codec,
                                   enum awsa_soc_bias_level  level)
{
    __UDA1380_DEV_DECL(p_this, p_codec->p_private_data);
    __UDA1380_DEVINFO_DECL(p_devinfo, p_this);

    uint32_t pm = 0;

    __uda1380_reg_cache_read(p_codec, __UDA1380_PM, &pm);

    if (p_codec->dapm.bias_level == level) {
        return AW_OK;
    }

    switch (level) {

    case AWSA_SOC_BIAS_ON:
    case AWSA_SOC_BIAS_PREPARE:

        /* ADC, DAC on */
        __uda1380_write(p_codec, __UDA1380_PM, __UDA1380_R02_PON_BIAS | pm);
        break;

    case AWSA_SOC_BIAS_STANDBY:

        if (p_codec->dapm.bias_level == AWSA_SOC_BIAS_OFF) {
            if (p_devinfo->gpio_is_valid) {
                aw_gpio_set(p_devinfo->gpio_power, 1);
                aw_mdelay(1);
                __uda1380_reset(p_codec);
            }
        }
        __uda1380_write(p_codec, __UDA1380_PM, 0x0);
        break;

    case AWSA_SOC_BIAS_OFF:

        if (!p_devinfo->gpio_is_valid) {
            break;
        }

        aw_gpio_set(p_devinfo->gpio_power, 0);
        break;

    default:
        break;
    }

    p_codec->dapm.bias_level = level;
    return AW_OK;
}

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
static void __uda1380_inst_init1 (awbl_dev_t *p_dev)
{
    return ;
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
static void __uda1380_inst_init2 (awbl_dev_t *p_dev)
{

}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
static void __uda1380_inst_connect (awbl_dev_t *p_dev)
{
    __UDA1380_DEV_DECL(p_this, p_dev);
    __UDA1380_DEVINFO_DECL(p_devinfo, p_this);

    aw_spinlock_isr_init(&p_this->lock, 0);

    awsa_soc_codec_register(&p_this->codec,
                            p_devinfo->p_codec_name,
                            &__g_uda1380_codec_driver,
                            p_this->dai,
                            p_devinfo->dais_name,
                            __g_uda1380_dai_driver,
                            AWSA_SOC_ARRAY_SIZE(__g_uda1380_dai_driver),
                            (void *)p_dev);
}

/******************************************************************************/
void awbl_uda1380_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_uda1380_drv_registration);
}

/* end of file */
