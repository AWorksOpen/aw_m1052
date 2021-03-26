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
 * - 1.00 18-05-24  win, first implementation
 * \endinternal
 */

#ifndef __AWSA_SOC_DECL_H
#define __AWSA_SOC_DECL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \brief mixer control(ctl elem -> p_private_data)
 */
struct awsa_soc_mixer_ctl {
    int      min;
    int      max;
    int      plfm_max;
    uint32_t reg_l;
    uint32_t reg_r;
    uint32_t shift_l;
    uint32_t shift_r;
    uint32_t invert;
};

/**
 * \brief declare mixer ctl
 */
#define AWSA_SOC_MIXER_CTL_DECL(                                  \
    xmin, xmax, plfm_max, reg_l, reg_r, shift_l, shift_r, invert) \
{                  \
    xmin,          \
    xmax,          \
    plfm_max,      \
    reg_l,         \
    reg_r,         \
    shift_l,       \
    shift_r,       \
    invert,        \
}

/**
 * \brief enumerated control
 */
struct awsa_soc_enum {
    uint16_t         reg;
    uint16_t         reg2;
    uint8_t          shift_l;
    uint8_t          shift_r;
    uint32_t         max;
    uint32_t         mask;
    const char     **p_texts;
    const uint32_t  *p_values;
    void            *p_dapm;
};

/**
 * \brief declare soc enum
 */
#define AWSA_SOC_ENUM_DECL(                                      \
    reg, reg2, shift_l, shift_r, max, mask, texts, values, dapm) \
{                  \
    reg,           \
    reg2,          \
    shift_l,       \
    shift_r,       \
    max,           \
    mask,          \
    texts,         \
    values,        \
    dapm,          \
}


/**
 *  \brief Convenience ctl builders
 */
#define AWSA_SOC_SINGLE_VALUE(xreg, xshift, xmax, xinvert)                   \
    (uint32_t)&(struct awsa_soc_mixer_ctl)                                   \
    AWSA_SOC_MIXER_CTL_DECL(0, xmax, xmax, xreg, 0, xshift, xshift, xinvert)

/**
 *  \brief Convenience ctl builders
 */
#define AWSA_SOC_SINGLE_VALUE_EXT(xreg, xmax, xinvert)              \
    (uint32_t)&(struct awsa_soc_mixer_ctl)                          \
    AWSA_SOC_MIXER_CTL_DECL(0, xmax, xmax, xreg, 0, 0, 0, xinvert)

/**
 *  \brief Convenience ctl builders
 */
#define AWSA_SOC_SINGLE(xname, xreg, xshift, xmax, xinvert)    \
    AWSA_CORE_CTL_ELEM_DECL(xname,                             \
                            AWSA_CORE_CTL_ELEM_IF_MIXER,       \
                            awsa_soc_volsw_info,               \
                            awsa_soc_volsw_get,                \
                            awsa_soc_volsw_set,                \
                            NULL,                              \
                            AWSA_SOC_SINGLE_VALUE(             \
                                xreg, xshift, xmax, xinvert))

/**
 *  \brief Convenience ctl builders
 */
#define AWSA_SOC_SINGLE_TLV(xname, xreg, xshift, xmax, xinvert, tlv_array) \
    AWSA_CORE_CTL_ELEM_DECL(xname,                                         \
                            AWSA_CORE_CTL_ELEM_IF_MIXER,                   \
                            awsa_soc_volsw_info,                           \
                            awsa_soc_volsw_get,                            \
                            awsa_soc_volsw_set,                            \
                            tlv_array,                                     \
                            AWSA_SOC_SINGLE_VALUE(                         \
                                xreg, xshift, xmax, xinvert))

/**
 *  \brief Convenience ctl builders
 */
#define AWSA_SOC_DOUBLE(                                           \
    xname, xreg, xshift_l, xshift_r, xmax, xinvert)                \
    AWSA_CORE_CTL_ELEM_DECL(                                       \
        xname,                                                     \
        AWSA_CORE_CTL_ELEM_IF_MIXER,                               \
        awsa_soc_volsw_info,                                       \
        awsa_soc_volsw_get,                                        \
        awsa_soc_volsw_set,                                        \
        NULL,                                                      \
        (uint32_t)&(struct awsa_soc_mixer_ctl)                     \
        AWSA_SOC_MIXER_CTL_DECL(                                   \
            0, xmax, xmax, xreg, 0, xshift_l, xshift_r, xinvert))

/**
 *  \brief Convenience ctl builders
 */
#define AWSA_SOC_DOUBLE_R(                                         \
    xname, xreg_l, xreg_r, xshift, xmax, xinvert)                  \
    AWSA_CORE_CTL_ELEM_DECL(                                       \
        xname,                                                     \
        AWSA_CORE_CTL_ELEM_IF_MIXER,                               \
        awsa_soc_volsw_info_2r,                                    \
        awsa_soc_volsw_get_2r,                                     \
        awsa_soc_volsw_set_2r,                                     \
        NULL,                                                      \
        (uint32_t)&(struct awsa_soc_mixer_ctl)                     \
        AWSA_SOC_MIXER_CTL_DECL(                                   \
            0, xmax, xmax, xreg_l, xreg_r, xshift, 0, xinvert))

/**
 *  \brief Convenience ctl builders
 */
#define AWSA_SOC_DOUBLE_TLV(                                       \
    xname, xreg, xshift_l, xshift_r, xmax, xinvert, tlv_array)     \
    AWSA_CORE_CTL_ELEM_DECL(                                       \
        xname,                                                     \
        AWSA_CORE_CTL_ELEM_IF_MIXER,                               \
        awsa_soc_volsw_info,                                       \
        awsa_soc_volsw_get,                                        \
        awsa_soc_volsw_set,                                        \
        tlv_array,                                                 \
        (uint32_t)&(struct awsa_soc_mixer_ctl)                     \
        AWSA_SOC_MIXER_CTL_DECL(                                   \
            0, xmax, xmax, xreg, 0, xshift_l, xshift_r, xinvert))

/**
 *  \brief Convenience ctl builders
 */
#define AWSA_SOC_DOUBLE_R_TLV(                                     \
    xname, xreg_l, xreg_r, xshift, xmax, xinvert, tlv_array)       \
    AWSA_CORE_CTL_ELEM_DECL(                                       \
        xname,                                                     \
        AWSA_CORE_CTL_ELEM_IF_MIXER,                               \
        awsa_soc_volsw_info_2r,                                    \
        awsa_soc_volsw_get_2r,                                     \
        awsa_soc_volsw_set_2r,                                     \
        tlv_array,                                                 \
        (uint32_t)&(struct awsa_soc_mixer_ctl)                     \
        AWSA_SOC_MIXER_CTL_DECL(                                   \
            0, xmax, xmax, xreg_l, xreg_r, xshift, 0, xinvert))

/**
 *  \brief Convenience ctl builders
 */
#define AWSA_SOC_DOUBLE_S8_TLV(xname, xreg, xmin, xmax, tlv_array)  \
    AWSA_CORE_CTL_ELEM_DECL(                                        \
        xname,                                                      \
        AWSA_CORE_CTL_ELEM_IF_MIXER,                                \
        awsa_soc_volsw_info_s8,                                     \
        awsa_soc_volsw_get_s8,                                      \
        awsa_soc_volsw_set_s8,                                      \
        tlv_array,                                                  \
        (uint32_t)&(struct awsa_soc_mixer_ctl)                      \
        AWSA_SOC_MIXER_CTL_DECL(                                    \
            xmin, xmax, xmax, 0, xreg, 0, 0, 0))

/**
 *  \brief Convenience ctl builders
 */
#define AWSA_SOC_ENUM_DOUBLE(xreg, xshift_l, xshift_r, xmax, xtexts)  \
    AWSA_SOC_ENUM_DECL(                                               \
        xreg, 0, xshift_l, xshift_r, xmax, 0, xtexts, NULL, NULL)

/**
 *  \brief Convenience ctl builders
 */
#define AWSA_SOC_ENUM_SINGLE(xreg, xshift, xmax, xtexts) \
    AWSA_SOC_ENUM_DOUBLE(xreg, xshift, xshift, xmax, xtexts)

/**
 *  \brief Convenience ctl builders
 */
#define AWSA_SOC_ENUM_SINGLE_EXT(xmax, xtexts) \
    AWSA_SOC_ENUM_DECL(0, 0, 0, 0, xmax, 0, xtexts, NULL, NULL)

/**
 * \brief Convenience ctl builders
 */
#define AWSA_SOC_VALUE_ENUM_DOUBLE(                                       \
    xreg, xshift_l, xshift_r, xmask, xmax, xtexts, xvalues)               \
    AWSA_SOC_ENUM_DECL(                                                   \
        xreg, 0, xshift_l, xshift_r, xmax, xmask, xtexts, xvalues, NULL)  \

/**
 * \brief Convenience ctl builders
 */
#define AWSA_SOC_VALUE_ENUM_SINGLE(xreg, xshift, xmask, xmax, xtexts, xvalues) \
    AWSA_SOC_VALUE_ENUM_DOUBLE(                                                \
        xreg, xshift, xshift, xmask, xmax, xtexts, xvalues)

/**
 * \brief Convenience ctl builders
 */
#define AWSA_SOC_ENUM(xname, xenum)                       \
    AWSA_CORE_CTL_ELEM_DECL(xname,                        \
                            AWSA_CORE_CTL_ELEM_IF_MIXER,  \
                            awsa_soc_enum_double_info,    \
                            awsa_soc_enum_double_get,     \
                            awsa_soc_enum_double_set,     \
                            NULL,                         \
                            (uint32_t)&xenum)

/**
 * \brief Convenience ctl builders
 */
#define AWSA_SOC_VALUE_ENUM(xname, xenum)                     \
    AWSA_CORE_CTL_ELEM_DECL(xname,                            \
                            AWSA_CORE_CTL_ELEM_IF_MIXER,      \
                            awsa_soc_enum_double_info,        \
                            awsa_soc_enum_double_value_get,   \
                            awsa_soc_enum_double_value_set,   \
                            NULL,                             \
                            (uint32_t)&xenum)

/**
 * \brief Convenience ctl builders
 */
#define AWSA_SOC_SINGLE_EXT(                                  \
    xname, xreg, xshift, xmax, xinvert, pfn_get, pfn_set)     \
    AWSA_CORE_CTL_ELEM_DECL(                                  \
        xname,                                                \
        AWSA_CORE_CTL_ELEM_IF_MIXER,                          \
        awsa_soc_volsw_info,                                  \
        pfn_get,                                              \
        pfn_set,                                              \
        NULL,                                                 \
        AWSA_SOC_SINGLE_VALUE(xreg, xshift, xmax, xinvert))

/**
 * \brief Convenience ctl builders
 */
#define AWSA_SOC_DOUBLE_EXT(                                            \
    xname, xreg, xshift_l, xshift_r, xmax, xinvert, pfn_get, pfn_set)   \
    AWSA_CORE_CTL_ELEM_DECL(                                            \
        xname,                                                          \
        AWSA_CORE_CTL_ELEM_IF_MIXER,                                    \
        awsa_soc_volsw_info,                                            \
        pfn_get,                                                        \
        pfn_set,                                                        \
        NULL,                                                           \
        (uint32_t)&(struct awsa_soc_mixer_ctl)                          \
        AWSA_SOC_MIXER_CTL_DECL(                                        \
            0, xmax, xmax, xreg, 0, xshift_l, xshift_r, xinvert))

/**
 * \brief Convenience ctl builders
 */
#define AWSA_SOC_SINGLE_EXT_TLV(                                        \
    xname, xreg, xshift, xmax, xinvert, pfn_get, pfn_set, tlv_array)    \
    AWSA_CORE_CTL_ELEM_DECL(                                            \
        xname,                                                          \
        AWSA_CORE_CTL_ELEM_IF_MIXER,                                    \
        awsa_soc_volsw_info,                                            \
        pfn_get,                                                        \
        pfn_set,                                                        \
        tlv_array,                                                      \
        AWSA_SOC_SINGLE_VALUE(xreg, xshift, xmax, xinvert))

/**
 * \brief Convenience ctl builders
 */
#define AWSA_SOC_DOUBLE_EXT_TLV(                                     \
    xname, xreg, xshift_l, xshift_r, xmax, xinvert,                  \
    pfn_get, pfn_set, tlv_array)                                     \
    AWSA_CORE_CTL_ELEM_DECL(                                         \
        xname,                                                       \
        AWSA_CORE_CTL_ELEM_IF_MIXER,                                 \
        awsa_soc_volsw_info,                                         \
        pfn_get,                                                     \
        pfn_put,                                                     \
        tlv_array,                                                   \
        (uint32_t)&(struct awsa_soc_mixer_ctl)                       \
        AWSA_SOC_MIXER_CTL_DECL(                                     \
            0, xmax, xmax, xreg, 0, xshift_l, xshift_r, xinvert))

/**
 * \brief Convenience ctl builders
 */
#define AWSA_SOC_DOUBLE_R_EXT_TLV(                                   \
    xname, xreg_l, xreg_r, xshift, xmax, xinvert,                    \
    pfn_get, pfn_set, tlv_array)                                     \
    AWSA_CORE_CTL_ELEM_DECL(                                         \
        xname,                                                       \
        AWSA_CORE_CTL_ELEM_IF_MIXER,                                 \
        awsa_soc_volsw_info_2r,                                      \
        pfn_get,                                                     \
        pfn_set,                                                     \
        tlv_array,                                                   \
        (uint32_t)&(struct awsa_soc_mixer_ctl)                       \
        AWSA_SOC_MIXER_CTL_DECL(                                     \
            0, xmax, xmax, xreg_l, xreg_r, xshift, 0, xinvert))

/**
 * \brief Convenience ctl builders
 */
#define AWSA_SOC_SINGLE_BOOL_EXT(xname, xdata, pfn_get, pfn_set) \
    AWSA_CORE_CTL_ELEM_DECL(xname,                               \
                            AWSA_CORE_CTL_ELEM_IF_MIXER,         \
                            awsa_soc_info_bool_ext,              \
                            pfn_get,                             \
                            pfn_set,                             \
                            NULL,                                \
                            xdata)

/**
 * \brief Convenience ctl builders
 */
#define AWSA_SOC_ENUM_EXT(xname, xenum, pfn_get, pfn_set)        \
    AWSA_CORE_CTL_ELEM_DECL(xname,                               \
                            AWSA_CORE_CTL_ELEM_IF_MIXER,         \
                            awsa_soc_enum_info_ext,              \
                            pfn_get,                             \
                            pfn_set,                             \
                            NULL,                                \
                            (uint32_t)&xenum)

/**
 * \brief Convenience ctl builders
 */
#define AWSA_SOC_DOUBLE_R_SX_TLV(                                \
    xname, xreg_l, xreg_r, xshift, xmin, xmax, tlv_array)        \
    AWSA_CORE_CTL_ELEM_DECL(                                     \
        xname,                                                   \
        AWSA_CORE_CTL_ELEM_IF_MIXER,                             \
        awsa_soc_volsw_info_2r_sx,                               \
        awsa_soc_volsw_get_2r_sx,                                \
        awsa_soc_put_volsw_2r_sx,                                \
        tlv_array,                                               \
        (uint32_t)&(struct awsa_soc_mixer_ctl)                   \
        AWSA_SOC_MIXER_CTL_DECL(                                 \
            xmin, xmax, 0, xreg_l, xreg_r, xshift, 0, 0))        \

/**
 *  \brief declaring a struct and calculating ARRAY_SIZE internally
 */
#define AWSA_SOC_ENUM_DOUBLE_DECL(xname, xreg, xshift_l, xshift_r, xtexts) \
    struct awsa_soc_enum name = AWSA_SOC_ENUM_DOUBLE(                      \
                                    xreg,                                  \
                                    xshift_l,                              \
                                    xshift_r,                              \
                                    AWSA_SOC_ARRAY_SIZE(xtexts),           \
                                    xtexts)

/**
 * \brief single
 */
#define AWSA_SOC_ENUM_SINGLE_DECL(xname, xreg, xshift, xtexts) \
    AWSA_SOC_ENUM_DOUBLE_DECL(xname, xreg, xshift, xshift, xtexts)

/**
 * \brief sigle ext
 */
#define AWSA_SOC_ENUM_SINGLE_EXT_DECL(xname, xtexts)                \
    struct awsa_soc_enum name = AWSA_SOC_ENUM_SINGLE_EXT(           \
                                    AWSA_SOC_ARRAY_SIZE(xtexts),    \
                                    xtexts)

/**
 * \brief value enum double
 */
#define AWSA_SOC_VALUE_ENUM_DOUBLE_DECL(                           \
    xname, xreg, xshift_l, xshift_r, xmask, xtexts, xvalues)       \
    struct awsa_soc_enum name = AWSA_SOC_VALUE_ENUM_DOUBLE(        \
                                    xreg,                          \
                                    xshift_l,                      \
                                    xshift_r,                      \
                                    xmask,                         \
                                    AWSA_SOC_ARRAY_SIZE(xtexts),   \
                                    xtexts,                        \
                                    xvalues)

/**
 * \brief value enum sigle
 */
#define AWSA_SOC_VALUE_ENUM_SINGLE_DECL(               \
    xname, xreg, xshift, xmask, xtexts, xvalues)       \
    AWSA_SOC_VALUE_ENUM_DOUBLE_DECL(xname,             \
                                    xreg,              \
                                    xshift,            \
                                    xshift,            \
                                    xmask,             \
                                    xtexts,            \
                                    xvalues)           \

/******************************************************************************/

/**
 * \brief codec domain
 */
#define AWSA_SOC_DAPM_VMID(xname)                      \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_VMID, \
                              xname,                   \
                              NULL,                    \
                              0, 0, 0, 0, 0, 0, 0,     \
                              NULL,                    \
                              0,                       \
                              NULL)                    \

/**
 * \brief platform domain
 */
#define AWSA_SOC_DAPM_INPUT(xname)                       \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_INPUT,  \
                              xname,                     \
                              NULL,                      \
                              0, 0, 0, 0, 0, 0, 0,       \
                              NULL,                      \
                              0,                         \
                              NULL)

/**
 * brief dapm output domain
 */
#define AWSA_SOC_DAPM_OUTPUT(xname)                      \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_OUTPUT, \
                              xname,                     \
                              NULL,                      \
                              0, 0, 0, 0, 0, 0, 0,       \
                              NULL,                      \
                              0,                         \
                              NULL)

/**
 * brief dapm mic domain
 */
#define AWSA_SOC_DAPM_MIC(xname, xevent)                 \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_MIC,    \
                              xname,                     \
                              NULL,                      \
                              0, 0, 0, 0, 0, 0, 0,       \
                              NULL,                      \
                              AWSA_SOC_DAPM_PRE_PMU |    \
                              AWSA_SOC_DAPM_POST_PMD,    \
                              xevent)

/**
 * brief dapm hp domain
 */
#define AWSA_SOC_DAPM_HP(xname, xevent)                  \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_HP,     \
                              xname,                     \
                              NULL,                      \
                              0, 0, 0, 0, 0, 0, 0,       \
                              NULL,                      \
                              AWSA_SOC_DAPM_POST_PMU |   \
                              AWSA_SOC_DAPM_PRE_PMD,     \
                              xevent)

/**
 * brief dapm spk domain
 */
#define AWSA_SOC_DAPM_SPK(xname, xevent)                 \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_SPK,    \
                              xname,                     \
                              NULL,                      \
                              0, 0, 0, 0, 0, 0, 0,       \
                              NULL,                      \
                              AWSA_SOC_DAPM_POST_PMU |   \
                              AWSA_SOC_DAPM_PRE_PMD,     \
                              xevent)

/**
 * brief dapm line domain
 */
#define AWSA_SOC_DAPM_LINE(xname, xevent)                \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_LINE,   \
                              xname,                     \
                              NULL,                      \
                              0, 0, 0, 0, 0, 0, 0,       \
                              NULL,                      \
                              AWSA_SOC_DAPM_POST_PMU |   \
                              AWSA_SOC_DAPM_PRE_PMD,     \
                              xevent)

/**
 * \brief dapm pga domain
 */
#define AWSA_SOC_DAPM_PGA(xname, xreg, xshift, xinvert, xctls, xnctls)  \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_PGA,                   \
                              xname,                                    \
                              NULL,                                     \
                              xreg,                                     \
                              xshift,                                   \
                              0, 0, 0,                                  \
                              xinvert,                                  \
                              0, NULL, 0, NULL)

/**
 * \brief dapm out driver domain
 */
#define AWSA_SOC_DAPM_OUT_DRV(xname, xreg, xshift, xinvert, xctls, xnctls) \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_OUT_DRV,                  \
                              xname,                                       \
                              NULL,                                        \
                              xreg,                                        \
                              xshift,                                      \
                              0, 0, 0,                                     \
                              xinvert,                                     \
                              xnctls,                                      \
                              xctls,                                       \
                              0, NULL)

/**
 * \brief dapm mixer domain
 */
#define AWSA_SOC_DAPM_MIXER(xname, xreg, xshift, xinvert, xctls, xnctls)   \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_MIXER,                    \
                              xname,                                       \
                              NULL,                                        \
                              xreg,                                        \
                              xshift,                                      \
                              0, 0, 0,                                     \
                              xinvert,                                     \
                              xnctls,                                      \
                              xctls,                                       \
                              0, NULL)

/**
 * \brief dapm mixer named ctl domain
 */
#define AWSA_SOC_DAPM_MIXER_NAMED_CTL(                             \
    xname, xreg, xshift, xinvert, xctls, xnctls)                   \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_MIXER_NAMED_CTL,  \
                              xname,                               \
                              NULL,                                \
                              xreg,                                \
                              xshift,                              \
                              0, 0, 0,                             \
                              xinvert,                             \
                              xnctls,                              \
                              xctls,                               \
                              0, NULL)

/**
 * \brief dapm micbias domain
 */
#define AWSA_SOC_DAPM_MICBIAS(xname, xreg, xshift, xinvert)   \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_MICBIAS,     \
                              xname,                          \
                              NULL,                           \
                              xreg,                           \
                              xshift,                         \
                              0, 0, 0,                        \
                              xinvert,                        \
                              0, NULL, 0, NULL)

/**
 * \brief dapm switch domain
 */
#define AWSA_SOC_DAPM_SWITCH(xname, xreg, xshift, xinvert, xctl) \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_SWITCH,         \
                              xname,                             \
                              NULL,                              \
                              xreg,                              \
                              xshift,                            \
                              0, 0, 0,                           \
                              xinvert,                           \
                              1,                                 \
                              xctl,                              \
                              0,                                 \
                              NULL)

/**
 * \brief dapm mux domain
 */
#define AWSA_SOC_DAPM_MUX(xname, xreg, xshift, xinvert, xctl) \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_MUX,         \
                              xname,                          \
                              NULL,                           \
                              xreg,                           \
                              xshift,                         \
                              0, 0, 0,                        \
                              xinvert,                        \
                              1,                              \
                              xctl,                           \
                              0,                              \
                              NULL)

/**
 * \brief dapm virtual domain
 */
#define AWSA_SOC_DAPM_VIRT_MUX(xname, xreg, xshift, xinvert, xctl) \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_VIRT_MUX,         \
                              xname,                               \
                              NULL,                                \
                              xreg,                                \
                              xshift,                              \
                              0, 0, 0,                             \
                              xinvert,                             \
                              1,                                   \
                              xctl,                                \
                              0,                                   \
                              NULL)

/**
 * \brief dapm value mux domain
 */
#define AWSA_SOC_DAPM_VALUE_MUX(xname, xreg, xshift, xinvert, xctl) \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_VALUE_MUX,         \
                              xname,                                \
                              NULL,                                 \
                              xreg,                                 \
                              xshift,                               \
                              0, 0, 0,                              \
                              xinvert,                              \
                              1,                                    \
                              xctl,                                 \
                              0,                                    \
                              NULL)

/**
 * \brief dapm pga array domain
 */
#define AWSA_SOC_DAPM_PGA_ARRAY(xname, xreg, xshift, xinvert, xctls)  \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_PGA,                 \
                              xname,                                  \
                              NULL,                                   \
                              xreg,                                   \
                              xshift,                                 \
                              0, 0, 0,                                \
                              xinvert,                                \
                              AWSA_SOC_ARRAY_SIZE(xctls),             \
                              xctls,                                  \
                              0,                                      \
                              NULL)

/**
 * \brief dapm mixer domain
 */
#define AWSA_SOC_DAPM_MIXER_ARRAY(xname, xreg, xshift, xinvert, xctls) \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_MIXER,                \
                              xname,                                   \
                              NULL,                                    \
                              xreg,                                    \
                              xshift,                                  \
                              0, 0, 0,                                 \
                              xinvert,                                 \
                              AWSA_SOC_ARRAY_SIZE(xctls),              \
                              xctls,                                   \
                              0,                                       \
                              NULL)

/**
 * \brief dapm mixer named ctl array domain
 */
#define AWSA_SOC_DAPM_MIXER_NAMED_CTL_ARRAY(                      \
    xname, xreg, xshift, xinvert, xctls)                          \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_MIXER_TYPE_NAMED_CTL, \
                              xname,                              \
                              NULL,                               \
                              xreg,                               \
                              xshift,                             \
                              0, 0, 0,                            \
                              xinvert,                            \
                              AWSA_SOC_ARRAY_SIZE(xctls),         \
                              xctls,                              \
                              0,                                  \
                              NULL)

/**
 * \brief  dapm pga domain with event
 */
#define AWSA_SOC_DAPM_PGA_E(                                      \
    xname, xreg, xshift, xinvert, xctls, xnctls, xevent, xflags)  \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_PGA,             \
                              xname,                              \
                              NULL,                               \
                              xreg,                               \
                              xshift,                             \
                              0, 0, 0,                            \
                              xinvert,                            \
                              xnctls,                             \
                              xctls,                              \
                              xflags,                             \
                              xevent)

/**
 * \brief  dapm out driver domain with event
 */
#define AWSA_SOC_DAPM_OUT_DRV_E(                                  \
    xname, xreg, xshift, xinvert, xctls, xnctls, xevent, xflags)  \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_OUT_DRV,         \
                              xname,                              \
                              NULL,                               \
                              xreg,                               \
                              xshift,                             \
                              0, 0, 0,                            \
                              xinvert,                            \
                              xnctls,                             \
                              xctls,                              \
                              xflags,                             \
                              xevent)

/**
 * \brief  dapm out driver domain with event
 */
#define AWSA_SOC_DAPM_MIXER_E(                                   \
    xname, xreg, xshift, xinvert, xctls, xnctls, xevent, xflags) \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_MIXER,          \
                              xname,                             \
                              NULL,                              \
                              xreg,                              \
                              xshift,                            \
                              0, 0, 0,                           \
                              xinvert,                           \
                              xnctls,                            \
                              xctls,                             \
                              xflags,                            \
                              xevent)

/**
 * \brief  dapm mixer named ctl domain with event
 */
#define AWSA_SOC_DAPM_MIXER_NAMED_CTL_E(                         \
    xname, xreg, xshift, xinvert, xctls, xnctls, xevent, xflags) \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_MIXER,          \
                              xname,                             \
                              NULL,                              \
                              xreg,                              \
                              xshift,                            \
                              0, 0, 0,                           \
                              xinvert,                           \
                              xnctls,                            \
                              xctls,                             \
                              xflags,                            \
                              xevent)

/**
 * \brief  dapm mixer named ctl domain with event
 */
#define AWSA_SOC_DAPM_MICBIAS_E(xname, xreg, xshift, xinvert, xevent, xflags) \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_MICBIAS,                     \
                              xname,                                          \
                              NULL,                                           \
                              xreg,                                           \
                              xshift,                                         \
                              0, 0, 0,                                        \
                              xinvert,                                        \
                              0, NULL,                                        \
                              xflags,                                         \
                              xevent)

/**
 * \brief  dapm switch domain with event
 */
#define AWSA_SOC_DAPM_SWITCH_E(                           \
    xname, xreg, xshift, xinvert, xctl, xevent, xflags)   \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_SWITCH,  \
                              xname,                      \
                              NULL,                       \
                              xreg,                       \
                              xshift,                     \
                              0, 0, 0,                    \
                              xinvert,                    \
                              1,                          \
                              xctl,                       \
                              xflags,                     \
                              xevent)

/**
 * \brief  dapm switch domain with event
 */
#define AWSA_SOC_DAPM_MUX_E(                             \
    xname, xreg, xshift, xinvert, xctl, xevent, xflags)  \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_MUX,    \
                              xname,                     \
                              NULL,                      \
                              xreg,                      \
                              xshift,                    \
                              0, 0, 0,                   \
                              xinvert,                   \
                              1,                         \
                              xctl,                      \
                              xflags,                    \
                              xevent)

/**
 * \brief  dapm switch domain with event
 */
#define AWSA_SOC_DAPM_VIRT_MUX_E(                           \
    xname, xreg, xshift, xinvert, xctl, xevent, xflags)     \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_VIRT_MUX,  \
                              xname,                        \
                              NULL,                         \
                              xreg,                         \
                              xshift,                       \
                              0, 0, 0,                      \
                              xinvert,                      \
                              1,                            \
                              xctl,                         \
                              xflags,                       \
                              xevent)

/**
 * \brief dapm pga domain with event
 */
#define AWSA_SOC_DAPM_PGA_E_ARRAY(                          \
    xname, xreg, xshift, xinvert, xctl, xevent, xflags)     \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_PGA,       \
                              xname,                        \
                              NULL,                         \
                              xreg,                         \
                              xshift,                       \
                              0, 0, 0,                      \
                              xinvert,                      \
                              AWSA_SOC_ARRAY_SIZE(xctl),    \
                              xctl,                         \
                              xflags,                       \
                              xevent)

/**
 * \brief dapm mixer domain with event
 */
#define AWSA_SOC_DAPM_MIXER_E_ARRAY(                        \
    xname, xreg, xshift, xinvert, xctls, xevent, xflags)    \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_MIXER,     \
                              xname,                        \
                              NULL,                         \
                              xreg,                         \
                              xshift,                       \
                              0, 0, 0,                      \
                              xinvert,                      \
                              AWSA_SOC_ARRAY_SIZE(xctl),    \
                              xctl,                         \
                              xflags,                       \
                              xevent)

/**
 * \brief dapm mixer named ctl domain with event
 */
#define AWSA_SOC_DAPM_MIXER_NAMED_CTL_E_ARRAY(              \
    xname, xreg, xshift, xinvert, xctls, xevent, xflags)    \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_MIXER,     \
                              xname,                        \
                              NULL,                         \
                              xreg,                         \
                              xshift,                       \
                              0, 0, 0,                      \
                              xinvert,                      \
                              AWSA_SOC_ARRAY_SIZE(xctl),    \
                              xctls,                        \
                              xflags,                       \
                              xevent)

/**
 * \brief events that are pre and post DAPM
 */
#define AWSA_SOC_DAPM_PRE(xname, xevent)                    \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_PRE,       \
                              xname,                        \
                              NULL,                         \
                              0, 0, 0, 0, 0, 0, 0,          \
                              NULL,                         \
                              AWSA_SOC_DAPM_PRE_PMU |       \
                              AWSA_SOC_DAPM_PRE_PMD,        \
                              xevent)

/**
 * \brief events that are pre and post DAPM
 */
#define AWSA_SOC_DAPM_POST(xname, xevent)                   \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_POST,      \
                              xname,                        \
                              NULL,                         \
                              0, 0, 0, 0, 0, 0, 0,          \
                              NULL,                         \
                              AWSA_SOC_DAPM_POST_PMU |      \
                              AWSA_SOC_DAPM_POST_PMD,       \
                              xevent)

/**
 * \brief dapm aif input domain
 */
#define AWSA_SOC_DAPM_AIF_IN(xname, xsname, xreg, xshift, xinvert)  \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_AIF_IN,            \
                              xname,                                \
                              xsname,                               \
                              xreg,                                 \
                              xshift,                               \
                              0, 0, 0,                              \
                              xinvert,                              \
                              0, NULL, 0, NULL)

/**
 * \brief dapm aif input domain with event
 */
#define AWSA_SOC_DAPM_AIF_IN_E(                                \
    xname, xsname, xreg, xshift, xinvert, xevent, xflags)      \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_AIF_IN,       \
                              xname,                           \
                              xsname,                          \
                              xreg,                            \
                              xshift,                          \
                              0, 0, 0,                         \
                              xinvert,                         \
                              0, NULL,                         \
                              xflags,                          \
                              xevent)

/**
 * \brief dapm aif output domain
 */
#define AWSA_SOC_DAPM_AIF_OUT(xname, xsname, xreg, xshift, xinvert) \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_AIF_OUT,           \
                              xname,                                \
                              xsname,                               \
                              xreg,                                 \
                              xshift,                               \
                              0, 0, 0,                              \
                              xinvert,                              \
                              0, NULL, 0, NULL)

/**
 * \brief dapm aif output domain with event
 */
#define AWSA_SOC_DAPM_AIF_OUT_E(                             \
    xname, xsname, xreg, xshift, xinvert, xevent, xflags)    \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_AIF_OUT,    \
                              xname,                         \
                              xsname,                        \
                              xreg,                          \
                              xshift,                        \
                              0, 0, 0,                       \
                              xinvert,                       \
                              0, NULL,                       \
                              xflags,                        \
                              xevent)

/**
 * \brief dapm dac domain
 */
#define AWSA_SOC_DAPM_DAC(xname, xsname, xreg, xshift, xinvert) \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_DAC,           \
                              xname,                            \
                              xsname,                           \
                              xreg,                             \
                              xshift,                           \
                              0, 0, 0,                          \
                              xinvert,                          \
                              0, NULL, 0, NULL)

/**
 * \brief dapm dac domain with event
 */
#define AWSA_SOC_DAPM_DAC_E(                                \
    xname, xsname, xreg, xshift, xinvert, xevent, xflags)   \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_DAC,       \
                              xname,                        \
                              xsname,                       \
                              xreg,                         \
                              xshift,                       \
                              0, 0, 0,                      \
                              xinvert,                      \
                              0, NULL,                      \
                              xflags,                       \
                              xevent)

/**
 * \brief dapm adc domain
 */
#define AWSA_SOC_DAPM_ADC(xname, xsname, xreg, xshift, xinvert)  \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_ADC,            \
                              xname,                             \
                              xsname,                            \
                              xreg,                              \
                              xshift,                            \
                              0, 0, 0,                           \
                              xinvert,                           \
                              0, NULL, 0, NULL)

/**
 * \brief dapm adc domain with event
 */
#define AWSA_SOC_DAPM_ADC_E(                               \
    xname, xsname, xreg, xshift, xinvert, xevent, xflags)  \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_ADC,      \
                              xname,                       \
                              xsname,                      \
                              xreg,                        \
                              xshift,                      \
                              0, 0, 0,                     \
                              xinvert,                     \
                              0, NULL,                     \
                              xflags,                      \
                              xevent)

/**
 * \brief dapm reg domain
 */
#define AWSA_SOC_DAPM_REG(                                 \
    xtype, xname, xreg, xshift, xmask, xon_val, xoff_val)  \
    AWSA_SOC_DAPM_WIDGET_DECL(xtype,                       \
                              xname,                       \
                              NULL,                        \
                              -((xreg) + 1),               \
                              xshift,                      \
                              xmask,                       \
                              xon_val,                     \
                              xoff_val,                    \
                              xinvert,                     \
                              0, NULL,                     \
                              AWSA_SOC_DAPM_PRE_PMU |      \
                              AWSA_SOC_DAPM_POST_PMD,      \
                              awsa_soc_dapm_reg_event)

/**
 * \brief dapm supply domain
 */
#define AWSA_SOC_DAPM_SUPPLY(                              \
    xname, xreg, xshift, xinvert, xevent, xflags)          \
    AWSA_SOC_DAPM_WIDGET_DECL(AWSA_SOC_DAPM_TYPE_SUPPLY,   \
                              xname,                       \
                              NULL,                        \
                              xreg,                        \
                              xshift,                      \
                              0, 0, 0,                     \
                              xinvert,                     \
                              0, NULL,                     \
                              xflags,                      \
                              xevent)

/**
 * \brief dapm single domain
 */
#define AWSA_SOC_DAPM_SINGLE(xname, xreg, xshift, xmax, xinvert) \
    AWSA_CORE_CTL_ELEM_DECL(xname,                               \
                            AWSA_CORE_CTL_ELEM_IF_MIXER,         \
                            awsa_soc_volsw_info,                 \
                            awsa_soc_dapm_volsw_get,             \
                            awsa_soc_dapm_volsw_set,             \
                            NULL,                                \
                            AWSA_SOC_SINGLE_VALUE(               \
                                xreg, xshift, xmax, xinvert))

/**
 * \brief dapm single domain with tlv
 */
#define AWSA_SOC_DAPM_SINGLE_TLV(                                \
    xname, xreg, xshift, xmax, xinvert, tlv_array)               \
    AWSA_CORE_CTL_ELEM_DECL(xname,                               \
                            AWSA_CORE_CTL_ELEM_IF_MIXER,         \
                            awsa_soc_volsw_info,                 \
                            awsa_soc_dapm_volsw_get,             \
                            awsa_soc_dapm_volsw_set,             \
                            tlv_array,                           \
                            AWSA_SOC_SINGLE_VALUE(               \
                                xreg, xshift, xmax, xinvert))

/**
 * \brief dapm enum domain
 */
#define AWSA_SOC_DAPM_ENUM(xname, xenum)                         \
    AWSA_CORE_CTL_ELEM_DECL(xname,                               \
                            AWSA_CORE_CTL_ELEM_IF_MIXER,         \
                            awsa_soc_enum_double_info,           \
                            awsa_soc_dapm_enum_double_get,       \
                            awsa_soc_dapm_enum_double_set,       \
                            NULL,                                \
                            (uint32_t)&xenum)

/**
 * \brief dapm enum virtual domain
 */
#define AWSA_SOC_DAPM_ENUM_VIRT(xname, xenum)                    \
    AWSA_CORE_CTL_ELEM_DECL(xname,                               \
                            AWSA_CORE_CTL_ELEM_IF_MIXER,         \
                            awsa_soc_enum_double_info,           \
                            awsa_soc_dapm_enum_virt_get,         \
                            awsa_soc_dapm_enum_virt_set,         \
                            NULL,                                \
                            (uint32_t)&xenum)

/**
 * \brief dapm enum value domain
 */
#define AWSA_SOC_DAPM_VALUE_ENUM(xname, xenum)                    \
    AWSA_CORE_CTL_ELEM_DECL(xname,                                \
                            AWSA_CORE_CTL_ELEM_IF_MIXER,          \
                            awsa_soc_enum_double_info,            \
                            awsa_soc_dapm_enum_double_value_get,  \
                            awsa_soc_dapm_enum_double_value_set,  \
                            NULL,                                 \
                            (uint32_t)&xenum)

/**
 * \brief dapm pin switch domain
 */
#define AWSA_SOC_DAPM_PIN_SWITCH(xname)                       \
    AWSA_CORE_CTL_ELEM_DECL(xname " Switch",                  \
                            AWSA_CORE_CTL_ELEM_IF_MIXER,      \
                            awsa_soc_dapm_pin_switch_info,    \
                            awsa_soc_dapm_pin_switch_get,     \
                            awsa_soc_dapm_pin_switch_set,     \
                            NULL,                             \
                            (uint32_t)xname)


/******************************************************************************/

/**
 * \brief enum double information get
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_info     : elemeent inforation
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_enum_double_info (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info);

/**
 * \brief external enumerated single mixer info callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_info     : elemeent inforation
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_enum_info_ext (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info);

/**
 * \brief enumerated double mixer get callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_enum_double_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief enumerated double mixer put callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_enum_double_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief enumerated double mixer get callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_enum_double_value_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief enumerated double mixer put callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_enum_double_value_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief single mixer info callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_info     : ctl element information
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_volsw_info (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info);

/**
 * \brief external single mixer info callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_info     : ctl element information
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_volsw_info_ext (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info);

#define awsa_soc_info_bool_ext       awsa_soc_ctl_boolean_mono_info

/**
 * \brief single mixer value get callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_volsw_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief single mixer value set callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_volsw_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief double mixer info callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_info     : ctl element information
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_volsw_info_2r (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info);

/**
 * \brief double mixer get callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_volsw_get_2r (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief double mixer set callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_volsw_set_2r (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief signed mixer info callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_info     : ctl element information
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_volsw_info_s8 (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info);

/**
 * \brief signed mixer value get callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_volsw_get_s8 (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief signed mixer value set callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_volsw_set_s8 (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief double with tlv and variable data size mixer info callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_info     : ctl element information
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_volsw_info_2r_sx (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info);

/**
 * \brief double with tlv and variable data size
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_volsw_get_2r_sx (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief double with tlv and variable data size mixer set callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_volsw_set_2r_sx (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief dapm mixer set callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_volsw_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief dapm mixer get callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_volsw_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief dapm enumerated double mixer get callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_enum_double_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief dapm enumerated double mixer set callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_enum_double_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief Get virtual DAPM mux
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_enum_virt_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief Set virtual DAPM mux
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_enum_virt_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief dapm enumerated double mixer get callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_enum_double_value_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief dapm enumerated double mixer set callback
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_enum_double_value_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief Information for a pin switch
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_info     : ctl element information
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_pin_switch_info (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info);

/**
 * \brief get audio pin status
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_pin_switch_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief Set information for a pin switch
 *
 * \param[in] p_ctl_elem : ctl element
 * \param[in] p_value    : ctl element value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_pin_switch_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_SOC_DECL_H */

/* end of file */
