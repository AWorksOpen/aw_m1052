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
 * - 1.00 20-09-18  win, first implementation.
 * \endinternal
 */

#include "aw_types.h"
#include "aw_vdebug.h"
#include "aw_delay.h"

#include "soc/codec/awbl_wm8960.h"

/* declare sim device instance */
#define __WM8960_DEV_DECL(p_this, p_dev) \
    struct awbl_wm8960_dev *p_this = \
        (struct awbl_wm8960_dev *)(p_dev)

/* declare sim device infomation */
#define __WM8960_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_wm8960_devinfo *p_devinfo = \
        (struct awbl_wm8960_devinfo *)AWBL_DEVINFO_GET(p_dev)


#define __WM8960_REG_NUM               56

/**
 * \brief R25 - Power 1
 */
#define __WM8960_VMID_MASK             0x180
#define __WM8960_VREF                  0x40

/**
 * \brief R26 - Power 2
 */
#define __WM8960_PWR2_LOUT1            0x40
#define __WM8960_PWR2_ROUT1            0x20
#define __WM8960_PWR2_OUT3             0x02

/**
 * \brief R28 - Anti-pop 1
 */
#define __WM8960_POBCTRL               0x80
#define __WM8960_BUFDCOPEN             0x10
#define __WM8960_BUFIOEN               0x08
#define __WM8960_SOFT_ST               0x04
#define __WM8960_HPSTBY                0x01

/**
 * \brief R29 - Anti-pop 2
 */
#define __WM8960_DISOP                 0x40
#define __WM8960_DRES_MASK             0x30

/*******************************************************************************
 local
*******************************************************************************/

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

/** \brief adc */
static const AWSA_CORE_TLV_DB_SCALE_DECL(__g_wm8960_adc_tlv, -9700, 50, 0);

/** \brief dac */
static const AWSA_CORE_TLV_DB_SCALE_DECL(__g_wm8960_dac_tlv, -12700, 50, 1);

/** \brief out */
static const AWSA_CORE_TLV_DB_SCALE_DECL(__g_wm8960_out_tlv, -12100, 100, 1);

/** \brief bypass */
static const AWSA_CORE_TLV_DB_SCALE_DECL(__g_wm8960_bypass_tlv, -2100, 300, 0);

/** \brief deemph */
static const char *__g_wm8960_deemph[] = {
    "None", "32Khz", "44.1Khz", "48Khz"
};

/**
 * \brief polarity
 */
static const char *__g_wm8960_polarity[] = {
    "No Inversion",
    "Left Inverted",
    "Right Inverted",
    "Stereo Inversion"
};

/** \brief 2d upper cutoff */
static const char *__g_wm8960_3d_upper_cutoff[] = {"High", "Low"};

/** \biref 3d lower cutoff */
static const char *__g_wm8960_3d_lower_cutoff[] = {"Low", "High"};

/** \brief alc func */
static const char *__g_wm8960_alc_func[] = {"Off", "Right", "Left", "Stereo"};

/** \brief alcmode */
static const char *__g_wm8960_alc_mode[] = {"ALC", "Limiter"};

/** \brief enum */
static const struct awsa_soc_enum __g_wm8960_enum[] = {
    AWSA_SOC_ENUM_SINGLE(AWBL_WM8960_REG_DACCTL1, 1, 4, __g_wm8960_deemph),
    AWSA_SOC_ENUM_SINGLE(AWBL_WM8960_REG_DACCTL1, 5, 4, __g_wm8960_polarity),
    AWSA_SOC_ENUM_SINGLE(AWBL_WM8960_REG_DACCTL2, 5, 4, __g_wm8960_polarity),

    AWSA_SOC_ENUM_SINGLE(
        AWBL_WM8960_REG_3D, 6, 2, __g_wm8960_3d_upper_cutoff),

    AWSA_SOC_ENUM_SINGLE(
        AWBL_WM8960_REG_3D, 5, 2, __g_wm8960_3d_lower_cutoff),

    AWSA_SOC_ENUM_SINGLE(AWBL_WM8960_REG_ALC1,    7, 4, __g_wm8960_alc_func),
    AWSA_SOC_ENUM_SINGLE(AWBL_WM8960_REG_ALC3,    8, 2, __g_wm8960_alc_mode),
};

/** \brief control element */
static const
struct awsa_core_ctl_elem_driver __g_wm8960_ctl_elems_driver[] = {

    AWSA_SOC_DOUBLE_R_TLV("ADC Capture Volume",
                          AWBL_WM8960_REG_LADC,
                          AWBL_WM8960_REG_RADC,
                          0,
                          255,
                          0,
                          __g_wm8960_adc_tlv),

    AWSA_SOC_DOUBLE_R("Capture Volume ZC Switch",
                      AWBL_WM8960_REG_LINVOL,
                      AWBL_WM8960_REG_RINVOL,
                      6,
                      1,
                      0),

    AWSA_SOC_DOUBLE_R("Capture Switch",
                      AWBL_WM8960_REG_LINVOL,
                      AWBL_WM8960_REG_RINVOL,
                      7,
                      1,
                      0),

    AWSA_SOC_DOUBLE_R_TLV("DAC Playback Volume",
                          AWBL_WM8960_REG_LDAC,
                          AWBL_WM8960_REG_RDAC,
                          0,
                          255,
                          0,
                          __g_wm8960_dac_tlv),

    AWSA_SOC_DOUBLE_R_TLV("Headphone Playback Volume",
                          AWBL_WM8960_REG_LOUT1,
                          AWBL_WM8960_REG_ROUT1,
                          0,
                          127,
                          0,
                          __g_wm8960_out_tlv),

    AWSA_SOC_DOUBLE_R("Headphone Playback ZC Switch",
                      AWBL_WM8960_REG_LOUT1,
                      AWBL_WM8960_REG_ROUT1,
                      7,
                      1,
                      0),

    AWSA_SOC_DOUBLE_R_TLV("Speaker Playback Volume",
                          AWBL_WM8960_REG_LOUT2,
                          AWBL_WM8960_REG_ROUT2,
                          0,
                          127,
                          0,
                          __g_wm8960_out_tlv),

    AWSA_SOC_DOUBLE_R("Speaker Playback ZC Switch",
                      AWBL_WM8960_REG_LOUT2,
                      AWBL_WM8960_REG_ROUT2,
                      7,
                      1,
                      0),

    AWSA_SOC_SINGLE("Speaker DC Volume",
                    AWBL_WM8960_REG_CLASSD3,
                    3,
                    5,
                    0),

    AWSA_SOC_SINGLE("Speaker AC Volume",
                    AWBL_WM8960_REG_CLASSD3,
                    0,
                    5,
                    0),

    AWSA_SOC_SINGLE("PCM Playback -6dB Switch",
                    AWBL_WM8960_REG_DACCTL1,
                    7,
                    1,
                    0),

    AWSA_SOC_ENUM("ADC Polarity", __g_wm8960_enum[1]),

    AWSA_SOC_ENUM("Playback De-emphasis", __g_wm8960_enum[0]),

    AWSA_SOC_SINGLE(
        "ADC High Pass Filter Switch", AWBL_WM8960_REG_DACCTL1, 0, 1, 0),

    AWSA_SOC_ENUM("DAC Polarity", __g_wm8960_enum[2]),

    AWSA_SOC_ENUM("3D Filter Upper Cut-Off", __g_wm8960_enum[3]),
    AWSA_SOC_ENUM("3D Filter Lower Cut-Off", __g_wm8960_enum[4]),
    AWSA_SOC_SINGLE("3D Volume", AWBL_WM8960_REG_3D, 1, 15, 0),
    AWSA_SOC_SINGLE("3D Switch", AWBL_WM8960_REG_3D, 0, 1, 0),

    AWSA_SOC_ENUM("ALC Function", __g_wm8960_enum[5]),
    AWSA_SOC_SINGLE("ALC Max Gain", AWBL_WM8960_REG_ALC1, 4, 7, 0),
    AWSA_SOC_SINGLE("ALC Target", AWBL_WM8960_REG_ALC1, 0, 15, 1),
    AWSA_SOC_SINGLE("ALC Min Gain", AWBL_WM8960_REG_ALC2, 4, 7, 0),
    AWSA_SOC_SINGLE("ALC Hold Time", AWBL_WM8960_REG_ALC2, 0, 15, 0),
    AWSA_SOC_ENUM("ALC Mode", __g_wm8960_enum[6]),
    AWSA_SOC_SINGLE("ALC Decay", AWBL_WM8960_REG_ALC3, 4, 15, 0),
    AWSA_SOC_SINGLE("ALC Attack", AWBL_WM8960_REG_ALC3, 0, 15, 0),

    AWSA_SOC_SINGLE("Noise Gate Threshold", AWBL_WM8960_REG_NOISEG, 3, 31, 0),
    AWSA_SOC_SINGLE("Noise Gate Switch", AWBL_WM8960_REG_NOISEG, 0, 1, 0),

    AWSA_SOC_DOUBLE_R("ADC PCM Capture Volume",
                      AWBL_WM8960_REG_LINPATH,
                      AWBL_WM8960_REG_RINPATH,
                      0,
                      127,
                      0),

    AWSA_SOC_SINGLE_TLV("Left Output Mixer Boost Bypass Volume",
                        AWBL_WM8960_REG_BYPASS1,
                        4,
                        7,
                        1,
                        __g_wm8960_bypass_tlv),

    AWSA_SOC_SINGLE_TLV("Left Output Mixer LINPUT3 Volume",
                        AWBL_WM8960_REG_LOUTMIX,
                        4,
                        7,
                        1,
                        __g_wm8960_bypass_tlv),

    AWSA_SOC_SINGLE_TLV("Right Output Mixer Boost Bypass Volume",
                        AWBL_WM8960_REG_BYPASS2,
                        4,
                        7,
                        1,
                        __g_wm8960_bypass_tlv),

    AWSA_SOC_SINGLE_TLV("Right Output Mixer RINPUT3 Volume",
                        AWBL_WM8960_REG_ROUTMIX,
                        4,
                        7,
                        1,
                        __g_wm8960_bypass_tlv),
};

/** \brief lin boost */
static const struct awsa_core_ctl_elem_driver __g_wm8960_lin_boost[] = {
    AWSA_SOC_DAPM_SINGLE("LINPUT2 Switch", AWBL_WM8960_REG_LINPATH, 6, 1, 0),
    AWSA_SOC_DAPM_SINGLE("LINPUT3 Switch", AWBL_WM8960_REG_LINPATH, 7, 1, 0),
    AWSA_SOC_DAPM_SINGLE("LINPUT1 Switch", AWBL_WM8960_REG_LINPATH, 8, 1, 0),
};

/** \brief lin */
static const struct awsa_core_ctl_elem_driver __g_wm8960_lin[] = {
    AWSA_SOC_DAPM_SINGLE("Left Boost Switch", AWBL_WM8960_REG_LINPATH, 3, 1, 0),
};

/** \brief rin boost */
static const struct awsa_core_ctl_elem_driver __g_wm8960_rin_boost[] = {
    AWSA_SOC_DAPM_SINGLE("RINPUT2 Switch", AWBL_WM8960_REG_RINPATH, 6, 1, 0),
    AWSA_SOC_DAPM_SINGLE("RINPUT3 Switch", AWBL_WM8960_REG_RINPATH, 7, 1, 0),
    AWSA_SOC_DAPM_SINGLE("RINPUT1 Switch", AWBL_WM8960_REG_RINPATH, 8, 1, 0),
};

/** \brief rin */
static const struct awsa_core_ctl_elem_driver __g_wm8960_rin[] = {
    AWSA_SOC_DAPM_SINGLE(
        "Right Boost Switch", AWBL_WM8960_REG_RINPATH, 3, 1, 0),
};

/** \brief loutput mixer */
static const struct awsa_core_ctl_elem_driver __g_wm8960_loutput_mixer[] = {
    AWSA_SOC_DAPM_SINGLE("Left PCM Playback Switch",
                         AWBL_WM8960_REG_LOUTMIX,
                         8,
                         1,
                         0),

    AWSA_SOC_DAPM_SINGLE("LINPUT3 Output Switch",
                         AWBL_WM8960_REG_LOUTMIX,
                         7,
                         1,
                         0),

    AWSA_SOC_DAPM_SINGLE("Left Boost Bypass Switch",
                         AWBL_WM8960_REG_BYPASS1,
                         7,
                         1,
                         0),
};

/** \brief routput mixer */
static const struct awsa_core_ctl_elem_driver __g_wm8960_routput_mixer[] = {
    AWSA_SOC_DAPM_SINGLE(
        "Right PCM Playback Switch", AWBL_WM8960_REG_ROUTMIX, 8, 1, 0),

    AWSA_SOC_DAPM_SINGLE(
        "RINPUT3 Output Switch", AWBL_WM8960_REG_ROUTMIX, 7, 1, 0),

    AWSA_SOC_DAPM_SINGLE(
        "Right Boost Bypass Switch", AWBL_WM8960_REG_BYPASS2, 7, 1, 0),
};

/**
 * \brief widgets
 */
static const
struct awsa_soc_dapm_widget_driver __g_wm8960_dapm_widgets_driver[] = {
    AWSA_SOC_DAPM_INPUT("LINPUT1"),
    AWSA_SOC_DAPM_INPUT("RINPUT1"),
    AWSA_SOC_DAPM_INPUT("LINPUT2"),
    AWSA_SOC_DAPM_INPUT("RINPUT2"),
    AWSA_SOC_DAPM_INPUT("LINPUT3"),
    AWSA_SOC_DAPM_INPUT("RINPUT3"),

    AWSA_SOC_DAPM_SUPPLY("MICB", AWBL_WM8960_REG_POWER1, 1, 0, NULL, 0),

    AWSA_SOC_DAPM_MIXER("Left Boost Mixer",
                        AWBL_WM8960_REG_POWER1,
                        5,
                        0,
                        __g_wm8960_lin_boost,
                        AWSA_SOC_ARRAY_SIZE(__g_wm8960_lin_boost)),

    AWSA_SOC_DAPM_MIXER("Right Boost Mixer",
                        AWBL_WM8960_REG_POWER1,
                        4,
                        0,
                        __g_wm8960_rin_boost,
                        AWSA_SOC_ARRAY_SIZE(__g_wm8960_rin_boost)),

    AWSA_SOC_DAPM_MIXER("Left Input Mixer",
                        AWBL_WM8960_REG_POWER3,
                        5,
                        0,
                        __g_wm8960_lin,
                        AWSA_SOC_ARRAY_SIZE(__g_wm8960_lin)),

    AWSA_SOC_DAPM_MIXER("Right Input Mixer",
                        AWBL_WM8960_REG_POWER3,
                        4,
                        0,
                        __g_wm8960_rin,
                        AWSA_SOC_ARRAY_SIZE(__g_wm8960_rin)),

    AWSA_SOC_DAPM_ADC("Left ADC",  "Capture",  AWBL_WM8960_REG_POWER1, 3, 0),
    AWSA_SOC_DAPM_ADC("Right ADC", "Capture",  AWBL_WM8960_REG_POWER1, 2, 0),

    AWSA_SOC_DAPM_DAC("Left DAC",  "Playback", AWBL_WM8960_REG_POWER2, 8, 0),
    AWSA_SOC_DAPM_DAC("Right DAC", "Playback", AWBL_WM8960_REG_POWER2, 7, 0),

    AWSA_SOC_DAPM_MIXER("Left Output Mixer",
                        AWBL_WM8960_REG_POWER3,
                        3,
                        0,
                        &__g_wm8960_loutput_mixer[0],
                        AWSA_SOC_ARRAY_SIZE(__g_wm8960_loutput_mixer)),

    AWSA_SOC_DAPM_MIXER("Right Output Mixer",
                        AWBL_WM8960_REG_POWER3,
                        2,
                        0,
                        &__g_wm8960_routput_mixer[0],
                        AWSA_SOC_ARRAY_SIZE(__g_wm8960_routput_mixer)),

    AWSA_SOC_DAPM_PGA("LOUT1 PGA", AWBL_WM8960_REG_POWER2, 6, 0, NULL, 0),
    AWSA_SOC_DAPM_PGA("ROUT1 PGA", AWBL_WM8960_REG_POWER2, 5, 0, NULL, 0),

    AWSA_SOC_DAPM_PGA(
        "Left Speaker PGA",  AWBL_WM8960_REG_POWER2, 4, 0, NULL, 0),

    AWSA_SOC_DAPM_PGA(
        "Right Speaker PGA", AWBL_WM8960_REG_POWER2, 3, 0, NULL, 0),

    AWSA_SOC_DAPM_PGA(
        "Right Speaker Output", AWBL_WM8960_REG_CLASSD1, 7, 0, NULL, 0),
    AWSA_SOC_DAPM_PGA(
        "Left Speaker Output",  AWBL_WM8960_REG_CLASSD1, 6, 0, NULL, 0),

    AWSA_SOC_DAPM_OUTPUT("SPK_LP"),
    AWSA_SOC_DAPM_OUTPUT("SPK_LN"),
    AWSA_SOC_DAPM_OUTPUT("HP_L"),
    AWSA_SOC_DAPM_OUTPUT("HP_R"),
    AWSA_SOC_DAPM_OUTPUT("SPK_RP"),
    AWSA_SOC_DAPM_OUTPUT("SPK_RN"),
    AWSA_SOC_DAPM_OUTPUT("OUT3"),
};

/** \brief mono out */
static const struct awsa_core_ctl_elem_driver __g_wm8960_mono_out[] = {
    AWSA_SOC_DAPM_SINGLE("Left Switch",  AWBL_WM8960_REG_MONOMIX1, 7, 1, 0),
    AWSA_SOC_DAPM_SINGLE("Right Switch", AWBL_WM8960_REG_MONOMIX2, 7, 1, 0),
};

/** \brief out3 */
static const
struct awsa_soc_dapm_widget_driver __g_wm8960_dapm_widgets_out3[] = {
    AWSA_SOC_DAPM_MIXER("Mono Output Mixer",
                        AWBL_WM8960_REG_POWER2,
                        1,
                        0,
                        &__g_wm8960_mono_out[0],
                        AWSA_SOC_ARRAY_SIZE(__g_wm8960_mono_out)),
};

/**
 * \brief  Represent OUT3 as a PGA so that it gets turned on with LOUT1/ROUT1
 */
static const
struct awsa_soc_dapm_widget_driver __g_wm8960_dapm_widgets_capless[] = {
    AWSA_SOC_DAPM_PGA("OUT3 VMID", AWBL_WM8960_REG_POWER2, 1, 0, NULL, 0),
};

/**
 * \brief audio paths
 */
static const struct awsa_soc_dapm_route __g_wm8960_audio_paths[] = {
    { "Left Boost Mixer", "LINPUT1 Switch", "LINPUT1" },
    { "Left Boost Mixer", "LINPUT2 Switch", "LINPUT2" },
    { "Left Boost Mixer", "LINPUT3 Switch", "LINPUT3" },

    /* Really Boost Switch */
    { "Left Input Mixer", "Left Boost Switch", "Left Boost Mixer", },
    { "Left Input Mixer", NULL,                "LINPUT1", },
    { "Left Input Mixer", NULL,                "LINPUT2" },
    { "Left Input Mixer", NULL,                "LINPUT3" },

    { "Right Boost Mixer", "RINPUT1 Switch", "RINPUT1" },
    { "Right Boost Mixer", "RINPUT2 Switch", "RINPUT2" },
    { "Right Boost Mixer", "RINPUT3 Switch", "RINPUT3" },

    /* Really Boost Switch */
    { "Right Input Mixer", "Right Boost Switch", "Right Boost Mixer", },
    { "Right Input Mixer", NULL,                 "RINPUT1", },
    { "Right Input Mixer", NULL,                 "RINPUT2" },
    { "Right Input Mixer", NULL,                 "RINPUT3" },

    { "Left ADC",  NULL, "Left Input Mixer" },
    { "Right ADC", NULL, "Right Input Mixer" },

    { "Left Output Mixer", "LINPUT3 Output Switch",    "LINPUT3" },
    { "Left Output Mixer", "Left Boost Bypass Switch", "Left Boost Mixer"} ,
    { "Left Output Mixer", "Left PCM Playback Switch", "Left DAC" },

    { "Right Output Mixer", "RINPUT3 Output Switch",     "RINPUT3" },
    { "Right Output Mixer", "Right Boost Bypass Switch", "Right Boost Mixer" } ,
    { "Right Output Mixer", "Right PCM Playback Switch", "Right DAC" },

    { "LOUT1 PGA", NULL, "Left Output Mixer" },
    { "ROUT1 PGA", NULL, "Right Output Mixer" },

    { "HP_L", NULL, "LOUT1 PGA" },
    { "HP_R", NULL, "ROUT1 PGA" },

    { "Left Speaker PGA",  NULL, "Left Output Mixer" },
    { "Right Speaker PGA", NULL, "Right Output Mixer" },

    { "Left Speaker Output",  NULL, "Left Speaker PGA" },
    { "Right Speaker Output", NULL, "Right Speaker PGA" },

    { "SPK_LN", NULL, "Left Speaker Output" },
    { "SPK_LP", NULL, "Left Speaker Output" },
    { "SPK_RN", NULL, "Right Speaker Output" },
    { "SPK_RP", NULL, "Right Speaker Output" },
};

/** \brief path out3 */
static const struct awsa_soc_dapm_route __g_wm8960_audio_paths_out3[] = {
    { "Mono Output Mixer", "Left Switch",  "Left Output Mixer" },
    { "Mono Output Mixer", "Right Switch", "Right Output Mixer" },
    { "OUT3",              NULL,           "Mono Output Mixer", }
};

/** \brief path capless */
static const struct awsa_soc_dapm_route __g_wm8960_audio_paths_capless[] = {
    { "HP_L",      NULL, "OUT3 VMID" },
    { "HP_R",      NULL, "OUT3 VMID" },

    { "OUT3 VMID", NULL, "Left Output Mixer" },
    { "OUT3 VMID", NULL, "Right Output Mixer" },
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

/** \brief deemph */
static const int __g_wm8960_deemph_settings[] = { 0, 32000, 44100, 48000 };

/** \brief alc rates */
static const struct {
    int      rate;
    uint32_t val;
} __g_wm8960_alc_rates[] = {
    { 48000, 0 },
    { 44100, 0 },
    { 32000, 1 },
    { 22050, 2 },
    { 24000, 2 },
    { 16000, 3 },
    { 11250, 4 },
    { 12000, 4 },
    {  8000, 5 },
};

/**
 * \brief wm8960 set deemph
 */
static int __wm8960_deemph_set (struct awsa_soc_codec *p_codec)
{
    __WM8960_DEV_DECL(p_this, p_codec->p_private_data);

    int val, i, best;

    /*
     * If we're using deemphasis select the nearest available sample
     * rate.
     */
    if (p_this->deemph) {
        best = 1;
        for (i = 2; i < AWSA_SOC_ARRAY_SIZE(__g_wm8960_deemph_settings); i++) {
            if (abs(__g_wm8960_deemph_settings[i]    - p_this->rate) <
                abs(__g_wm8960_deemph_settings[best] - p_this->rate))
                best = i;
        }

        val = best << 1;
    } else {
        val = 0;
    }

    AW_DBGF(("WM8960: Set deemphasis %d\n", val));

    return awsa_soc_codec_bits_update(p_codec,
                                      AWBL_WM8960_REG_DACCTL1,
                                      0x6,
                                      val);
}

/**
 * \brief wm8960 set bias level out3
 */
static int __wm8960_bias_level_out3_set (struct awsa_soc_codec    *p_codec,
                                         enum awsa_soc_bias_level  level)
{
    uint32_t reg;

    switch (level) {

    case AWSA_SOC_BIAS_ON:
        break;

    case AWSA_SOC_BIAS_PREPARE:

        /* Set VMID to 2x50k */
        awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_POWER1, &reg);
        reg &= ~0x180;
        reg |= 0x80;
        awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_POWER1, reg);
        break;

    case AWSA_SOC_BIAS_STANDBY:

        if (p_codec->dapm.bias_level == AWSA_SOC_BIAS_OFF) {

            /* Enable anti-pop features */
            awsa_soc_codec_write(p_codec,
                                 AWBL_WM8960_REG_APOP1,
                                 __WM8960_POBCTRL   |
                                 __WM8960_SOFT_ST   |
                                 __WM8960_BUFDCOPEN |
                                 __WM8960_BUFIOEN);

            /* Enable & ramp VMID at 2x50k */
            awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_POWER1, &reg);
            reg |= 0x80;
            awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_POWER1, reg);

            aw_mdelay(100);

            /* Enable VREF */
            awsa_soc_codec_write(p_codec,
                                 AWBL_WM8960_REG_POWER1,
                                 reg | __WM8960_VREF);

            /* Disable anti-pop features */
            awsa_soc_codec_write(p_codec,
                                 AWBL_WM8960_REG_APOP1,
                                 __WM8960_BUFIOEN);
        }

        /* Set VMID to 2x250k */
        awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_POWER1, &reg);
        reg &= ~0x180;
        reg |= 0x100;
        awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_POWER1, reg);
        break;

    case AWSA_SOC_BIAS_OFF:

        /* Enable anti-pop features */
        awsa_soc_codec_write(p_codec,
                             AWBL_WM8960_REG_APOP1,
                             __WM8960_POBCTRL   |
                             __WM8960_SOFT_ST   |
                             __WM8960_BUFDCOPEN |
                             __WM8960_BUFIOEN);

        /* Disable VMID and VREF, let them discharge */
        awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_POWER1, 0);

        aw_mdelay(600);
        break;
    }

    p_codec->dapm.bias_level = level;
    return AW_OK;
}

/**
 * \brief wm8960 set bias level capless
 */
static int __wm8960_bias_level_capless_set (struct awsa_soc_codec    *p_codec,
                                            enum awsa_soc_bias_level  level)
{
    __WM8960_DEV_DECL(p_this, p_codec->p_private_data);

    uint32_t reg;

    switch (level) {

    case AWSA_SOC_BIAS_ON:
        break;

    case AWSA_SOC_BIAS_PREPARE:

        switch (p_codec->dapm.bias_level) {

        case AWSA_SOC_BIAS_STANDBY:

            /* Enable anti pop mode */
            awsa_soc_codec_bits_update(p_codec,
                                       AWBL_WM8960_REG_APOP1,
                                       __WM8960_POBCTRL |
                                       __WM8960_SOFT_ST |
                                       __WM8960_BUFDCOPEN,
                                       __WM8960_POBCTRL |
                                       __WM8960_SOFT_ST |
                                       __WM8960_BUFDCOPEN);

            /* Enable LOUT1, ROUT1 and OUT3 if they're enabled */
            reg = 0;

            if (p_this->p_lout1 != NULL && p_this->p_lout1->power) {
                reg |= __WM8960_PWR2_LOUT1;
            }

            if (p_this->p_rout1 != NULL && p_this->p_rout1->power) {
                reg |= __WM8960_PWR2_ROUT1;
            }

            if (p_this->p_out3 && p_this->p_out3->power) {
                reg |= __WM8960_PWR2_OUT3;
            }

            awsa_soc_codec_bits_update(p_codec,
                                       AWBL_WM8960_REG_POWER2,
                                       __WM8960_PWR2_LOUT1 |
                                       __WM8960_PWR2_ROUT1 |
                                       __WM8960_PWR2_OUT3,
                                       reg);

            /* Enable VMID at 2*50k */
            awsa_soc_codec_bits_update(p_codec,
                                       AWBL_WM8960_REG_POWER1,
                                       __WM8960_VMID_MASK,
                                       0x80);

            /* Ramp */
            aw_mdelay(100);

            /* Enable VREF */
            awsa_soc_codec_bits_update(p_codec,
                                       AWBL_WM8960_REG_POWER1,
                                       __WM8960_VREF,
                                       __WM8960_VREF);

            aw_mdelay(100);
            break;

        case AWSA_SOC_BIAS_ON:

            /* Enable anti-pop mode */
            awsa_soc_codec_bits_update(p_codec,
                                       AWBL_WM8960_REG_APOP1,
                                       __WM8960_POBCTRL |
                                       __WM8960_SOFT_ST |
                                       __WM8960_BUFDCOPEN,
                                       __WM8960_POBCTRL |
                                       __WM8960_SOFT_ST |
                                       __WM8960_BUFDCOPEN);

            /* Disable VMID and VREF */
            awsa_soc_codec_bits_update(p_codec,
                                       AWBL_WM8960_REG_POWER1,
                                       __WM8960_VREF |
                                       __WM8960_VMID_MASK,
                                       0);
            break;

        default:
            break;
        }
        break;

    case AWSA_SOC_BIAS_STANDBY:

        switch (p_codec->dapm.bias_level) {

        case AWSA_SOC_BIAS_PREPARE:

            /* Disable HP discharge */
            awsa_soc_codec_bits_update(p_codec,
                                       AWBL_WM8960_REG_APOP2,
                                       __WM8960_DISOP |
                                       __WM8960_DRES_MASK,
                                       0);

            /* Disable anti-pop features */
            awsa_soc_codec_bits_update(p_codec,
                                       AWBL_WM8960_REG_APOP1,
                                       __WM8960_POBCTRL |
                                       __WM8960_SOFT_ST |
                                       __WM8960_BUFDCOPEN,
                                       __WM8960_POBCTRL |
                                       __WM8960_SOFT_ST |
                                       __WM8960_BUFDCOPEN);
            break;

        default:
            break;
        }
        break;

    case AWSA_SOC_BIAS_OFF:
        break;
    }

    p_codec->dapm.bias_level = level;
    return AW_OK;
}

/**
 * \brief wm8960 set bias level
 */
static int __wm8960_bias_level_set (struct awsa_soc_codec    *p_codec,
                                    enum awsa_soc_bias_level  level)
{
    __WM8960_DEV_DECL(p_this, p_codec->p_private_data);
    __WM8960_DEVINFO_DECL(p_devinfo, p_this);

    if (p_devinfo->capless) {
        __wm8960_bias_level_capless_set(p_codec, level);
    } else {
        __wm8960_bias_level_out3_set(p_codec, level);
    }

    return AW_OK;
}

/**
 * \brief wm8960 add widgets
 */
static int __wm8960_widgets_add (struct awsa_soc_codec *p_codec)
{
    __WM8960_DEV_DECL(p_this, p_codec->p_private_data);
    __WM8960_DEVINFO_DECL(p_devinfo, p_this);

    struct awsa_soc_dapm_widget *p_w = NULL;

    awsa_soc_dapm_widgets_add(
        &p_codec->dapm,
        p_this->widget,
        __g_wm8960_dapm_widgets_driver,
        AWSA_SOC_ARRAY_SIZE(__g_wm8960_dapm_widgets_driver),
        p_this->widget_ctl_elem);

    awsa_soc_dapm_routes_add(&p_codec->dapm,
                             p_this->path,
                             __g_wm8960_audio_paths,
                             AWSA_SOC_ARRAY_SIZE(__g_wm8960_audio_paths));

    if (p_devinfo->capless) {
        awsa_soc_dapm_widgets_add(
            &p_codec->dapm,
            &p_this->widget_capless,
            __g_wm8960_dapm_widgets_capless,
            AWSA_SOC_ARRAY_SIZE(__g_wm8960_dapm_widgets_capless),
            NULL);

        awsa_soc_dapm_routes_add(
            &p_codec->dapm,
            p_this->path_capless,
            __g_wm8960_audio_paths_capless,
            AWSA_SOC_ARRAY_SIZE(__g_wm8960_audio_paths_capless));
    } else {
        awsa_soc_dapm_widgets_add(
            &p_codec->dapm,
            &p_this->widget_capless,
            __g_wm8960_dapm_widgets_out3,
            AWSA_SOC_ARRAY_SIZE(__g_wm8960_dapm_widgets_out3),
            p_this->out_ctl_elem);

        awsa_soc_dapm_routes_add(
            &p_codec->dapm,
            p_this->path_capless,
            __g_wm8960_audio_paths_out3,
            AWSA_SOC_ARRAY_SIZE(__g_wm8960_audio_paths_out3));
    }

    /*
     * We need to power up the headphone output stage out of
     * sequence for capless mode.  To save scanning the widget
     * list each time to find the desired power state do so now
     * and save the result.
     */
    aw_list_for_each_entry(p_w,
                           &p_codec->p_card->widgets,
                           struct awsa_soc_dapm_widget,
                           list) {
        if (strcmp(p_w->p_driver->p_name, "LOUT1 PGA") == 0) {
            p_this->p_lout1 = p_w;
        }

        if (strcmp(p_w->p_driver->p_name, "ROUT1 PGA") == 0) {
            p_this->p_rout1 = p_w;
        }

        if (strcmp(p_w->p_driver->p_name, "OUT3 VMID") == 0) {
            p_this->p_out3 = p_w;
        }
    }

    return AW_OK;
}

/*******************************************************************************
  dai
*******************************************************************************/

/** \brief format */
#define __WM8960_FORMATS (AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_S16_LE)  | \
                          AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_S20_3LE) | \
                          AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_S24_LE))

/** \brief rates */
#define __WM8960_RATES  (AWSA_PCM_RATE_8000  | \
                         AWSA_PCM_RATE_11025 | \
                         AWSA_PCM_RATE_16000 | \
                         AWSA_PCM_RATE_22050 | \
                         AWSA_PCM_RATE_44100 | \
                         AWSA_PCM_RATE_48000)

/**
 * \brief The size in bits of the pll divide multiplied by 10
 *        to allow rounding later
 */
#define __WM8960_FIXED_PLL_SIZE ((1 << 24) * 10)

/**
 * \brief PLL factors
 */
static int __wm8960_pll_factors (struct awbl_wm8960_pll_div *p_pll_div,
                                 uint32_t                    source,
                                 uint32_t                    target)
{
    uint64_t k_part;
    uint32_t k, n_div, n_mod;

    AW_DBGF(("WM8960 PLL: setting %dHz->%dHz\n", source, target));

    /* Scale up target to PLL operating frequency */
    target *= 4;

    n_div = target / source;

    if (n_div < 6) {
        source             >>= 1;
        p_pll_div->pre_div   = 1;
        n_div                = target / source;
    } else {
        p_pll_div->pre_div = 0;
    }

    if ((n_div < 6) || (n_div > 12)) {
        AW_ERRF(("WM8960 PLL: Unsupported N=%d\n", n_div));
        return -AW_EINVAL;
    }

    p_pll_div->n = n_div;
    n_mod        = target % source;
    k_part       = __WM8960_FIXED_PLL_SIZE * (uint64_t)n_mod;
    k_part       = k_part / source;
    k            = k_part & 0xFFFFFFFF;

    /* Check if we need to round */
    if ((k % 10) >= 5) {
        k += 5;
    }

    /* Move down to proper range now rounding is done */
    k            /= 10;
    p_pll_div->k  = k;

    AW_DBGF(("WM8960 PLL: N=%x K=%x pre_div=%d\n",
             pll_div->n,
             pll_div->k,
             pll_div->pre_div));
    return AW_OK;
}

/**
 * \brief wm8960 dai set pll
 */
static int __wm8960_dai_pll_set (struct awsa_soc_dai *p_dai,
                                 int                  pll_id,
                                 int                  source,
                                 uint32_t             freq_in,
                                 uint32_t             freq_out)
{
    struct awsa_soc_codec *p_codec = p_dai->plfm_codec.p_codec;
    uint32_t               reg;
    int                    ret;

    __WM8960_DEV_DECL(p_this, p_codec->p_private_data);

    if (freq_in && freq_out) {
        ret = __wm8960_pll_factors(&p_this->pll_div, freq_in, freq_out);
        if (ret != AW_OK) {
            return ret;
        }
    }

    /*
     * Disable the PLL: even if we are changing the frequency the
     * PLL needs to be disabled while we do so.
     */
    awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_CLOCK1, &reg);
    reg &= ~1;
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_CLOCK1, reg);

    awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_POWER2, &reg);
    reg &= ~1;
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_POWER2, reg);

    if (!freq_in || !freq_out) {
        return AW_OK;
    }

    awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_PLL1, &reg);
    reg &= ~0x3f;
    reg |= p_this->pll_div.pre_div << 4;
    reg |= p_this->pll_div.n;

    if (p_this->pll_div.k) {
        reg |= 0x20;

        awsa_soc_codec_write(p_codec,
                             AWBL_WM8960_REG_PLL2,
                             (p_this->pll_div.k >> 18) & 0x3f);
        awsa_soc_codec_write(p_codec,
                             AWBL_WM8960_REG_PLL3,
                             (p_this->pll_div.k >> 9) & 0x1ff);
        awsa_soc_codec_write(p_codec,
                             AWBL_WM8960_REG_PLL4,
                             p_this->pll_div.k & 0x1ff);
    }

    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_PLL1, reg);

    /* Turn it on */
    awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_POWER2, &reg);
    reg |= 1;
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_POWER2, reg);

    aw_mdelay(250);

    awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_CLOCK1, &reg);
    reg |= 1;
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_CLOCK1, reg);
    return AW_OK;
}

/**
 * \brief wm8960 dai set clkdiv
 */
static int __wm8960_dai_clkdiv_set (struct awsa_soc_dai *p_dai,
                                    int                  clk_id,
                                    int                  div)
{
    struct awsa_soc_codec *p_codec = p_dai->plfm_codec.p_codec;
    uint32_t               reg;

    switch (clk_id) {

    case AWBL_WM8960_CLK_SYSCLKDIV:
        awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_CLOCK1, &reg);
        reg &= 0x1f9;
        awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_CLOCK1, reg | div);
        break;

    case AWBL_WM8960_CLK_DACDIV:
        awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_CLOCK1, &reg);
        reg &= 0x1c7;
        awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_CLOCK1, reg | div);
        break;

    case AWBL_WM8960_CLK_OPCLKDIV:
        awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_PLL1, &reg);
        reg &= 0x03f;
        awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_PLL1, reg | div);
        break;

    case AWBL_WM8960_CLK_DCLKDIV:
        awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_CLOCK2, &reg);
        reg &= 0x03f;
        awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_CLOCK2, reg | div);
        break;

    case AWBL_WM8960_CLK_TOCLKSEL:
        awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_ADDCTL1, &reg);
        reg &= 0x1fd;
        awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_ADDCTL1, reg | div);
        break;

    default:
        return -AW_EINVAL;
    }

    return AW_OK;
}

/**
 * \brief wm8960 dai set fmt
 */
static int __wm8960_dai_fmt_set (struct awsa_soc_dai *p_dai,
                                 uint32_t             fmt)
{
    struct awsa_soc_codec *p_codec = p_dai->plfm_codec.p_codec;
    uint16_t               iface   = 0;

    /* set master/slave audio interface */
    switch (fmt & AWSA_SOC_DAI_FMT_MASTER_MASK) {

    case AWSA_SOC_DAI_FMT_CBM_CFM:
        iface |= 0x0040;
        break;

    case AWSA_SOC_DAI_FMT_CBS_CFS:
        break;

    default:
        return -AW_EINVAL;
    }

    /* interface format */
    switch (fmt & AWSA_SOC_DAI_FMT_MASK) {

    case AWSA_SOC_DAI_FMT_I2S:
        iface |= 0x0002;
        break;

    case AWSA_SOC_DAI_FMT_RIGHT_J:
        break;

    case AWSA_SOC_DAI_FMT_LEFT_J:
        iface |= 0x0001;
        break;

    case AWSA_SOC_DAI_FMT_DSP_A:
        iface |= 0x0003;
        break;

    case AWSA_SOC_DAI_FMT_DSP_B:
        iface |= 0x0013;
        break;

    default:
        return -AW_EINVAL;
    }

    /* clock inversion */
    switch (fmt & AWSA_SOC_DAI_FMT_INV_MASK) {

    case AWSA_SOC_DAI_FMT_NB_NF:
        break;

    case AWSA_SOC_DAI_FMT_IB_IF:
        iface |= 0x0090;
        break;

    case AWSA_SOC_DAI_FMT_IB_NF:
        iface |= 0x0080;
        break;

    case AWSA_SOC_DAI_FMT_NB_IF:
        iface |= 0x0010;
        break;

    default:
        return -AW_EINVAL;
    }

    /* set iface */
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_IFACE1, iface);
    return 0;
}

/**
 * \brief wm8960 dai mute
 */
static int __wm8960_dai_mute (struct awsa_soc_dai *p_dai,
                              int                  mute)
{
    struct awsa_soc_codec *p_codec  = p_dai->plfm_codec.p_codec;
    uint32_t               mute_reg;

    awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_DACCTL1, &mute_reg);
    mute_reg &= 0xfff7;

    if (mute) {
        awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_DACCTL1, mute_reg | 0x8);
    } else {
        awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_DACCTL1, mute_reg);
    }

    return AW_OK;
}

/**
 * \brief wm8960 dai config
 */
static int __wm8960_dai_config (struct awsa_soc_dai *p_dai,
                                awsa_pcm_stream_t    stream,
                                awsa_pcm_config_t   *p_config)
{
    struct awsa_soc_codec *p_codec = p_dai->plfm_codec.p_codec;
    uint32_t               iface;
    int                    i;

    __WM8960_DEV_DECL(p_this, p_codec->p_private_data);

    awsa_soc_codec_read(p_codec, AWBL_WM8960_REG_IFACE1, &iface);
    iface &= 0xfff3;

    /* bit size */
    switch (p_config->format) {

    case AWSA_PCM_FORMAT_S16_LE:
        break;

    case AWSA_PCM_FORMAT_S20_3LE:
        iface |= 0x0004;
        break;

    case AWSA_PCM_FORMAT_S24_LE:
        iface |= 0x0008;
        break;

    default:
        break;
    }

    /* Update filters for the new rate */
    if (stream == AWSA_PCM_STREAM_PLAYBACK) {
        p_this->rate = p_config->rate;
        __wm8960_deemph_set(p_codec);
    } else {
        for (i = 0; i < AWSA_SOC_ARRAY_SIZE(__g_wm8960_alc_rates); i++)
            if (__g_wm8960_alc_rates[i].rate == p_config->rate) {
                awsa_soc_codec_bits_update(p_codec,
                                           AWBL_WM8960_REG_ADDCTL3,
                                           0x7,
                                           __g_wm8960_alc_rates[i].val);
            }
    }

    /* set iface */
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_IFACE1, iface);
    return AW_OK;
}

/* playabck & capture */
static const struct awsa_soc_dai_servopts __g_wm8960_dai_opts = {
    NULL,
    __wm8960_dai_pll_set,
    __wm8960_dai_clkdiv_set,
    __wm8960_dai_fmt_set,
    __wm8960_dai_mute,
    NULL,
    NULL,
    __wm8960_dai_config,
    NULL,
    NULL,
    NULL
};

/** \brief dai driver */
static const struct awsa_soc_dai_driver __g_wm8960_dai_driver[] = {
    {
        &__g_wm8960_dai_opts,
        {
            "Capture",
            __WM8960_FORMATS,
            __WM8960_RATES,
            AWSA_PCM_RATE_8000,
            AWSA_PCM_RATE_48000,
            1,
            2,
        },
        {
            "Playback",
            __WM8960_FORMATS,
            __WM8960_RATES,
            AWSA_PCM_RATE_8000,
            AWSA_PCM_RATE_48000,
            1,
            2,
        },
        AW_TRUE,
        NULL,
        NULL,
    }
};

/*******************************************************************************
  codec
*******************************************************************************/

/*
 * uda1380 register cache
 */
static const uint16_t __g_wm8960_default_reg[__WM8960_REG_NUM] = {
    0x0097, 0x0097, 0x0000, 0x0000,
    0x0000, 0x0008, 0x0000, 0x000a,
    0x01c0, 0x0000, 0x00ff, 0x00ff,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x007b, 0x0100, 0x0032,
    0x0000, 0x00c3, 0x00c3, 0x01c0,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0100, 0x0100, 0x0050, 0x0050,
    0x0050, 0x0050, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0040, 0x0000,
    0x0000, 0x0050, 0x0050, 0x0000,
    0x0002, 0x0037, 0x004d, 0x0080,
    0x0008, 0x0031, 0x0026, 0x00e9,
};

static aw_err_t __wm8960_probe (struct awsa_soc_codec *p_codec)
{
    int      ret;
    uint32_t reg;

    __WM8960_DEV_DECL(p_this, p_codec->p_private_data);

    if (p_this->probed) {
        return AW_OK;
    }

    memcpy(p_this->reg_cache,
           __g_wm8960_default_reg,
           sizeof(p_this->reg_cache));

    /* reset */
    ret = awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_RESET, 0);
    if (ret != AW_OK) {
        AW_ERRF(("WM8960 failed to issue reset\n"));
        return ret;
    }

    __wm8960_bias_level_set(p_codec, AWSA_SOC_BIAS_STANDBY);

    /* Latch the update bits */
    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_LINVOL, &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_LINVOL, reg | 0x100);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_RINVOL, &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_RINVOL, reg | 0x100);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_LADC,   &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_LADC,   reg | 0x100);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_RADC,   &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_RADC,   reg | 0x100);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_LDAC,   &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_LDAC,   reg | 0x100);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_RDAC,   &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_RDAC,   reg | 0x100);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_LOUT1,  &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_LOUT1,  reg | 0x100);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_ROUT1,  &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_ROUT1,  reg | 0x100);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_LOUT2,  &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_LOUT2,  reg | 0x100);

    awsa_soc_codec_read(p_codec,  AWBL_WM8960_REG_ROUT2,  &reg);
    awsa_soc_codec_write(p_codec, AWBL_WM8960_REG_ROUT2,  reg | 0x100);

    awsa_soc_codec_ctl_elem_add(
        p_codec,
        p_this->ctl_elem,
        __g_wm8960_ctl_elems_driver,
        AWSA_SOC_ARRAY_SIZE(__g_wm8960_ctl_elems_driver));

    __wm8960_widgets_add(p_codec);;

    p_this->probed = AW_TRUE;
    return AW_OK;
}

/*
 * read wm8960 register cache
 */
static aw_err_t __wm8960_reg_cache_read (struct awsa_soc_codec *p_codec,
                                         uint32_t               reg,
                                         uint32_t              *p_value)
{
    __WM8960_DEV_DECL(p_this, p_codec->p_private_data);

    if (reg == AWBL_WM8960_REG_RESET) {
        *p_value = 0;
    }else if (reg >= __WM8960_REG_NUM) {
        *p_value = (uint32_t)-1;
    } else {
        *p_value = p_this->reg_cache[reg];
    }

    AW_DBGF(("wm8960: read reg %04x <= %04x\r\n", reg, *p_value));

    return AW_OK;
}

/*
 * write wm8960 register cache
 */
static void __wm8960_reg_cache_write (struct awsa_soc_codec *p_codec,
                                      uint16_t               reg,
                                      uint32_t               value)
{
    __WM8960_DEV_DECL(p_this, p_codec->p_private_data);

    if (reg >= __WM8960_REG_NUM) {
        return;
    }

    if ((reg >= 0x10) && (p_this->reg_cache[reg] != value)) {

        aw_spinlock_isr_take(&p_this->lock);
        p_this->cache_dirty |= (1UL << (reg - 0x10));
        aw_spinlock_isr_give(&p_this->lock);
    }

    p_this->reg_cache[reg] = value;
}

/**
 * \brief wm8960 register write
 */
static aw_err_t __wm8960_write (struct awsa_soc_codec *p_codec,
                                uint32_t               reg,
                                uint32_t               value)
{
    uint8_t   data[3];
    aw_err_t  ret;

    __WM8960_DEV_DECL(p_this, p_codec->p_private_data);
    __WM8960_DEVINFO_DECL(p_devinfo, p_this);

    /* write cache */
    __wm8960_reg_cache_write(p_codec, reg, value);

    /*
     * data is
     *   data[0] is MS byte
     *   data[1] is LS byte
     */
    data[0] = (((value & 0xff00) >> 8) & 0x01) | (reg << 1);
    data[1] = value & 0x00ff;

    /*
     * the interpolator & decimator regs must only be written when the
     * codec DAI is active.
     */
    if (reg >= __WM8960_REG_NUM) {
        return AW_OK;
    }

    ret = awbl_i2c_write((struct awbl_i2c_device *)p_this,
                         AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                         p_devinfo->addr,
                         data[0],
                         &data[1],
                         1);
    if (ret != AW_OK) {
        AW_ERRF(("%s: write reg 0x%04x failed\n", __func__, reg));
        return ret;
    }

    AW_DBGF(("wm8960: write reg 0x%04x => 0x%04x\r\n", reg, value));

    aw_spinlock_isr_take(&p_this->lock);
    p_this->cache_dirty &= ~(1UL << (reg - 0x10));
    aw_spinlock_isr_give(&p_this->lock);
    return AW_OK;
}

/* codec driver */
static const struct awsa_soc_codec_driver __g_wm8960_codec_driver = {
    __wm8960_probe,
    NULL,
    __wm8960_reg_cache_read,
    __wm8960_write,
    __wm8960_bias_level_set,
};

/*******************************************************************************
  AWBus lite
*******************************************************************************/

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
static void __wm8960_inst_init1 (awbl_dev_t *p_dev)
{
    return ;
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
static void __wm8960_inst_init2 (awbl_dev_t *p_dev)
{

}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
static void __wm8960_inst_connect (awbl_dev_t *p_dev)
{
    __WM8960_DEV_DECL(p_this, p_dev);
    __WM8960_DEVINFO_DECL(p_devinfo, p_this);

    aw_spinlock_isr_init(&p_this->lock, 0);

    awsa_soc_codec_register(&p_this->codec,
                            p_devinfo->p_codec_name,
                            &__g_wm8960_codec_driver,
                            p_this->dai,
                            p_devinfo->p_dais_name,
                            __g_wm8960_dai_driver,
                            AWSA_SOC_ARRAY_SIZE(__g_wm8960_dai_driver),
                            (void *)p_dev);
}

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_wm8960_drvfuncs = {
    __wm8960_inst_init1,
    __wm8960_inst_init2,
    __wm8960_inst_connect
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_i2c_drvinfo_t __g_wm8960_drv_registration = {
    {
        AWBL_VER_1,                 /* awb_ver */
        AWBL_BUSID_I2C,             /* bus_id */
        AWBL_WM8960_NAME,           /* p_drvname */
        &__g_wm8960_drvfuncs,       /* p_busfuncs */
        NULL,                       /* p_methods */
        NULL                        /* pfunc_drv_probe */
    }
};

/**
 * \brief driver register
 */
void awbl_wm8960_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_wm8960_drv_registration);
}

/* end of file */
