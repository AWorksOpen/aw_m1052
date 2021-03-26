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
 * - 1.00 16-11-18  win, first implementation
 * \endinternal
 */

#include "soc/awsa_soc_internal.h"

/**
 * \brief dapm power sequences - make this per codec in the future
 */
static int __g_soc_dapm_up_seq[] = {
    [AWSA_SOC_DAPM_TYPE_PRE]             = 0,
    [AWSA_SOC_DAPM_TYPE_SUPPLY]          = 1,
    [AWSA_SOC_DAPM_TYPE_MICBIAS]         = 2,
    [AWSA_SOC_DAPM_TYPE_AIF_IN]          = 3,
    [AWSA_SOC_DAPM_TYPE_AIF_OUT]         = 3,
    [AWSA_SOC_DAPM_TYPE_MIC]             = 4,
    [AWSA_SOC_DAPM_TYPE_MUX]             = 5,
    [AWSA_SOC_DAPM_TYPE_VIRT_MUX]        = 5,
    [AWSA_SOC_DAPM_TYPE_VALUE_MUX]       = 5,
    [AWSA_SOC_DAPM_TYPE_DAC]             = 6,
    [AWSA_SOC_DAPM_TYPE_MIXER]           = 7,
    [AWSA_SOC_DAPM_TYPE_MIXER_NAMED_CTL] = 7,
    [AWSA_SOC_DAPM_TYPE_PGA]             = 8,
    [AWSA_SOC_DAPM_TYPE_ADC]             = 9,
    [AWSA_SOC_DAPM_TYPE_OUT_DRV]         = 10,
    [AWSA_SOC_DAPM_TYPE_HP]              = 10,
    [AWSA_SOC_DAPM_TYPE_SPK]             = 10,
    [AWSA_SOC_DAPM_TYPE_POST]            = 11,
};

/**
 * \brief dapm power sequences - make this per codec in the future
 */
static int __g_soc_dapm_down_seq[] = {
    [AWSA_SOC_DAPM_TYPE_PRE]             = 0,
    [AWSA_SOC_DAPM_TYPE_ADC]             = 1,
    [AWSA_SOC_DAPM_TYPE_HP]              = 2,
    [AWSA_SOC_DAPM_TYPE_SPK]             = 2,
    [AWSA_SOC_DAPM_TYPE_OUT_DRV]         = 2,
    [AWSA_SOC_DAPM_TYPE_PGA]             = 4,
    [AWSA_SOC_DAPM_TYPE_MIXER_NAMED_CTL] = 5,
    [AWSA_SOC_DAPM_TYPE_MIXER]           = 5,
    [AWSA_SOC_DAPM_TYPE_DAC]             = 6,
    [AWSA_SOC_DAPM_TYPE_MIC]             = 7,
    [AWSA_SOC_DAPM_TYPE_MICBIAS]         = 8,
    [AWSA_SOC_DAPM_TYPE_MUX]             = 9,
    [AWSA_SOC_DAPM_TYPE_VIRT_MUX]        = 9,
    [AWSA_SOC_DAPM_TYPE_VALUE_MUX]       = 9,
    [AWSA_SOC_DAPM_TYPE_AIF_IN]          = 10,
    [AWSA_SOC_DAPM_TYPE_AIF_OUT]         = 10,
    [AWSA_SOC_DAPM_TYPE_SUPPLY]          = 11,
    [AWSA_SOC_DAPM_TYPE_POST]            = 12,
};

/**
 * \brief reduce pop noise
 */
static void __soc_dapm_pop_wait (uint32_t pop_time)
{
    if (pop_time) {
        aw_task_delay(aw_ms_to_ticks(pop_time));
    }
}

/**
 * \brief set the bias level for the system
 */
static
int __soc_dapm_bias_level_set (struct awsa_soc_card         *p_card,
                               struct awsa_soc_dapm_context *p_dapm,
                               enum   awsa_soc_bias_level    level)
{
    aw_err_t ret = 0;

    switch (level) {

    case AWSA_SOC_BIAS_ON:
        AWSA_SOC_DEBUG_INFO("setting full bias");
        break;

    case AWSA_SOC_BIAS_PREPARE:
        AWSA_SOC_DEBUG_INFO("setting bias prepare");
        break;

    case AWSA_SOC_BIAS_STANDBY:
        AWSA_SOC_DEBUG_INFO("setting standby bias");
        break;

    case AWSA_SOC_BIAS_OFF:
        AWSA_SOC_DEBUG_INFO("setting bias off");
        break;

    default:
        AWSA_SOC_DEBUG_WARN("setting invalid bias %d", level);
        return -AW_EINVAL;
    }

    if (p_card && p_card->p_driver->pfn_bias_level_pre_set != NULL) {
        ret = p_card->p_driver->pfn_bias_level_pre_set(p_card, level);
    }

    if (ret == 0) {
        if (p_dapm->p_codec && p_dapm->p_codec->p_driver->pfn_bias_level_set) {
            ret = p_dapm->p_codec->p_driver->pfn_bias_level_set(
                      p_dapm->p_codec, level);
        } else {
            p_dapm->bias_level = level;
        }
    }

    if (ret == 0) {
        if (p_card && p_card->p_driver->pfn_bias_level_post_set) {
            ret = p_card->p_driver->pfn_bias_level_post_set(p_card, level);
        }
    }

    return ret;
}

/**
 * \brief get power state
 */
static inline
uint32_t __soc_dapm_power_state_get (struct awsa_core_card *p_card)
{
    return p_card->power_state;
}

/**
 * \brief reset 'walked' bit for each dapm path
 */
static inline
void __soc_dapm_walk_clear (struct awsa_soc_dapm_context *p_dapm)
{
    struct awsa_soc_dapm_path *p_path;

    aw_list_for_each_entry(p_path,
                           &p_dapm->p_card->paths,
                           struct awsa_soc_dapm_path,
                           list) {
        p_path->walked = 0;
    }
}

/* set up initial codec paths */
static void __soc_dapm_path_status_set (struct awsa_soc_dapm_widget *p_widget,
                                        struct awsa_soc_dapm_path   *p_path,
                                        int                          i)
{
    switch (p_widget->p_driver->type) {

    case AWSA_SOC_DAPM_TYPE_SWITCH :
    case AWSA_SOC_DAPM_TYPE_MIXER :
    case AWSA_SOC_DAPM_TYPE_MIXER_NAMED_CTL : {
        uint32_t                   val;
        struct awsa_soc_mixer_ctl *p_mc   = (struct awsa_soc_mixer_ctl *)
             p_widget->p_driver->p_elems_driver[i].private_value;
        uint32_t                   reg    = p_mc->reg_l;
        uint32_t                   shift  = p_mc->shift_l;
        int                        max    = p_mc->max;
        uint32_t                   mask   = (1 << awsa_soc_fls(max)) - 1;
        uint32_t                   invert = p_mc->invert;

        awsa_soc_codec_read(p_widget->p_codec, reg, &val);
        val = (val >> shift) & mask;

        if ((invert && !val) || (!invert && val)) {
            p_path->connected = AW_TRUE;
        } else {
            p_path->connected = AW_FALSE;
        }

        break;
    }

    case AWSA_SOC_DAPM_TYPE_MUX : {
        struct awsa_soc_enum *p_enum = (struct awsa_soc_enum *)
            p_widget->p_driver->p_elems_driver[i].private_value;
        uint32_t              val;
        uint32_t              item;
        uint32_t              bitmask;

        for (bitmask = 1; bitmask < p_enum->max; bitmask <<= 1);

        awsa_soc_codec_read(p_widget->p_codec, p_enum->reg, &val);
        item = (val >> p_enum->shift_l) & (bitmask - 1);

        p_path->connected = 0;
        for (i = 0; i < p_enum->max; i++) {
            if (!(strcmp(p_path->p_name, p_enum->p_texts[i])) && item == i) {
                p_path->connected = 1;
            }
        }

        break;
    }

    case AWSA_SOC_DAPM_TYPE_VIRT_MUX : {
        struct awsa_soc_enum *p_enum = (struct awsa_soc_enum *)
            p_widget->p_driver->p_elems_driver[i].private_value;

        p_path->connected = 0;

        /*
         * since a virtual mux has no backing registers to
         * decide which path to connect, it will try to match
         * with the first enumeration.  This is to ensure
         * that the default mux choice (the first) will be
         * correctly powered up during initialization.
         */
        if (!strcmp(p_path->p_name, p_enum->p_texts[0])) {
            p_path->connected = 1;
        }

        break;
    }

    case AWSA_SOC_DAPM_TYPE_VALUE_MUX : {
        struct awsa_soc_enum *p_enum = (struct awsa_soc_enum *)
            p_widget->p_driver->p_elems_driver[i].private_value;
        uint32_t              val;
        uint32_t              item;

        awsa_soc_codec_read(p_widget->p_codec, p_enum->reg, &val);
        val = (val >> p_enum->shift_l) & p_enum->mask;
        for (item = 0; item < p_enum->max; item++) {
            if (val == p_enum->p_values[item]) {
                break;
            }
        }

        p_path->connected = 0;
        for (i = 0; i < p_enum->max; i++) {
            if (!(strcmp(p_path->p_name, p_enum->p_texts[i])) && item == i) {
                p_path->connected = 1;
            }
        }

        break;
    }

    /* does not effect routing - always connected */
    case AWSA_SOC_DAPM_TYPE_PGA :
    case AWSA_SOC_DAPM_TYPE_OUT_DRV :
    case AWSA_SOC_DAPM_TYPE_OUTPUT :
    case AWSA_SOC_DAPM_TYPE_ADC :
    case AWSA_SOC_DAPM_TYPE_INPUT :
    case AWSA_SOC_DAPM_TYPE_DAC :
    case AWSA_SOC_DAPM_TYPE_MICBIAS :
    case AWSA_SOC_DAPM_TYPE_VMID :
    case AWSA_SOC_DAPM_TYPE_SUPPLY :
    case AWSA_SOC_DAPM_TYPE_AIF_IN :
    case AWSA_SOC_DAPM_TYPE_AIF_OUT :
        p_path->connected = 1;
        break;

    /* does effect routing - dynamically connected */
    case AWSA_SOC_DAPM_TYPE_HP:
    case AWSA_SOC_DAPM_TYPE_MIC:
    case AWSA_SOC_DAPM_TYPE_SPK:
    case AWSA_SOC_DAPM_TYPE_LINE:
    case AWSA_SOC_DAPM_TYPE_PRE:
    case AWSA_SOC_DAPM_TYPE_POST:
        p_path->connected = 0;
        break;

    default:
        break;
    }
}

/* connect mux widget to its interconnecting audio paths */
static
aw_err_t __soc_dapm_mux_connect (
    struct awsa_soc_dapm_context           *p_dapm,
    struct awsa_soc_dapm_widget            *p_src,
    struct awsa_soc_dapm_widget            *p_dest,
    struct awsa_soc_dapm_path              *p_path,
    const char                             *p_ctl_name,
    const struct awsa_core_ctl_elem_driver *p_elem_driver)
{
    struct awsa_soc_enum *p_enum = (struct awsa_soc_enum *)
                                   p_elem_driver->private_value;
    int                   i;

    for (i = 0; i < p_enum->max; i++) {
        if (!(strcmp(p_ctl_name, p_enum->p_texts[i]))) {
            aw_list_add(&p_path->list,        &p_dapm->p_card->paths);
            aw_list_add(&p_path->list_sink,   &p_dest->sources);
            aw_list_add(&p_path->list_source, &p_src->sinks);

            p_path->p_name = (char*)p_enum->p_texts[i];
            __soc_dapm_path_status_set(p_dest, p_path, 0);
            return AW_OK;
        }
    }

    return -AW_ENODEV;
}

/**
 * \brief connect mixer widget to its interconnecting audio paths
 */
static
aw_err_t __soc_dapm_mixer_connect (struct awsa_soc_dapm_context *p_dapm,
                                   struct awsa_soc_dapm_widget  *p_src,
                                   struct awsa_soc_dapm_widget  *p_dest,
                                   struct awsa_soc_dapm_path    *p_path,
                                   const char                   *p_ctl_name)
{
    int i;

    /* search for mixer ctl */
    for (i = 0; i < p_dest->p_driver->num_elems; i++) {
        if (!strcmp(p_ctl_name,
                    (char *)p_dest->p_driver->p_elems_driver[i].p_name)) {

            aw_list_add(&p_path->list, &p_dapm->p_card->paths);
            aw_list_add(&p_path->list_sink, &p_dest->sources);
            aw_list_add(&p_path->list_source, &p_src->sinks);

            p_path->p_name = (char *)p_dest->p_driver->p_elems_driver[i].p_name;
            __soc_dapm_path_status_set(p_dest, p_path, i);
            return AW_OK;
        }
    }
    return -AW_ENODEV;
}

/**
 * \brief update dapm codec register bits
 */
static int __soc_dapm_bits_update (struct awsa_soc_dapm_widget *p_widget)
{
    int      power;
    uint32_t old, new;
    aw_err_t ret;

    struct awsa_soc_codec        *p_codec = p_widget->p_codec;
    struct awsa_soc_dapm_context *p_dapm  = p_widget->p_dapm;
    struct awsa_soc_card         *p_card  = p_dapm->p_card;

    /* check for valid widgets */
    if (p_widget->p_driver->reg < 0                           ||
        p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_INPUT  ||
        p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_OUTPUT ||
        p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_HP     ||
        p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_MIC    ||
        p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_LINE   ||
        p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_SPK) {
        return AW_OK;
    }

    power = p_widget->power;
    if (p_widget->p_driver->invert) {
        power = (power ? 0:1);
    }

    ret = awsa_soc_codec_read(p_codec, p_widget->p_driver->reg, &old);
    if (ret != AW_OK) {
        return ret;
    }
    new = (old & ~(0x1 << p_widget->p_driver->shift)) |
                  (power << p_widget->p_driver->shift);

    if (old != new) {
        AWSA_SOC_DEBUG_VERBOSE("pop test %s : %s in %d ms",
                                p_widget->p_driver->p_name,
                                p_widget->power ? "on" : "off",
                                p_card->pop_time);
        __soc_dapm_pop_wait(p_card->pop_time);
        ret = awsa_soc_codec_write(p_codec, p_widget->p_driver->reg, new);
        if (ret != AW_OK) {
            return ret;
        }
    }

    AWSA_SOC_DEBUG_VERBOSE("reg %x old %x new %x",
                            p_widget->p_driver->reg,
                            old,
                            new);
    return ret;
}

/**
 * \brief add ctl device
 */
static aw_err_t __soc_dapm_ctl_elem_add (
    struct awsa_soc_dapm_context           *p_dapm,
    struct awsa_soc_dapm_widget            *p_widget,
    struct awsa_core_ctl_elem              *p_elem,
    const struct awsa_core_ctl_elem_driver *p_driver,
    int                                     num_elem)
{
    struct awsa_soc_card *p_card = p_dapm->p_card;
    aw_err_t              ret;
    int                   i;

    for (i = 0; i < num_elem; i++) {
        ret = awsa_core_ctl_elem_add(&p_elem[i],
                                     &p_driver[i],
                                     p_widget,
                                     &p_card->ctl);
        if (ret != AW_OK) {
            AWSA_SOC_DEBUG_ERROR("%s: Failed to add %s: %d",
                                 p_widget->p_driver->p_name,
                                 p_driver[i].p_name,
                                 ret);
            return ret;
        }
    }

    return AW_OK;
}

/**
 * \brief create new dapm mixer control
 */
static aw_err_t __soc_dapm_mixer_add (struct awsa_soc_dapm_context *p_dapm,
                                      struct awsa_soc_dapm_widget  *p_widget)
{
    int                        i;
    aw_err_t                   ret;
    struct awsa_soc_dapm_path *p_path;

    /* add kcontrol */
    for (i = 0; i < p_widget->p_driver->num_elems; i++) {

        /* match name */
        aw_list_for_each_entry(p_path,
                               &p_widget->sources,
                               struct awsa_soc_dapm_path,
                               list_sink) {

            /* mixer/mux paths name must match control name */
            if (p_path->p_name !=
                p_widget->p_driver->p_elems_driver[i].p_name) {
                continue;
            }

            ret = __soc_dapm_ctl_elem_add(
                      p_dapm,
                      p_widget,
                      &p_widget->p_elem[i],
                      &p_widget->p_driver->p_elems_driver[i],
                      1);
            if (ret != AW_OK) {
                AWSA_SOC_DEBUG_ERROR(
                    "failed to add dapm ctl elems %s: %d",
                    p_path->p_name,
                    ret);
                return ret;
            }

            p_path->p_elem_driver = &p_widget->p_driver->p_elems_driver[i];
        }
    }

    return ret;
}

/**
 * \brief add new dapm mux control
 */
static aw_err_t __soc_dapm_mux_add (struct awsa_soc_dapm_context *p_dapm,
                                    struct awsa_soc_dapm_widget  *p_widget)
{
    struct awsa_soc_dapm_path *p_path  = NULL;
    aw_err_t                   ret     = AW_OK;

    if (!p_widget->p_driver->num_elems) {
        AWSA_SOC_DEBUG_ERROR("mux %s has no ctl elems",
                              p_widget->p_driver->p_name);
        return -AW_EINVAL;
    }

    ret = __soc_dapm_ctl_elem_add(p_dapm,
                                  p_widget,
                                  p_widget->p_elem,
                                  p_widget->p_driver->p_elems_driver,
                                  p_widget->p_driver->num_elems);
    if (ret != AW_OK) {
        AWSA_SOC_DEBUG_ERROR("failed to add ctl %s",
                              p_widget->p_driver->p_name);
        return ret;
    }

    aw_list_for_each_entry(p_path,
                           &p_widget->sources,
                           struct awsa_soc_dapm_path,
                           list_sink) {
        p_path->p_elem_driver = p_widget->p_driver->p_elems_driver;
    }

    return ret;
}

/**
 * \brief create new dapm volume control
 */
static aw_err_t __soc_dapm_pga_add (struct awsa_soc_dapm_context *p_dapm,
                                    struct awsa_soc_dapm_widget  *p_widget)
{
    if (p_widget->p_driver->num_elems != 0) {
        AWSA_SOC_DEBUG_WARN("pga ctl elems not supported: '%s'",
                             p_widget->p_driver->p_name);
    }

    return AW_OK;
}

/*
 * \brief power down on suspend by checking the power state of the card
 */
static aw_bool_t
__soc_dapm_suspend_check (struct awsa_soc_dapm_widget *p_widget)
{
    int level = __soc_dapm_power_state_get(
                    &p_widget->p_dapm->p_codec->p_card->core_card);

    switch (level) {

    case AWSA_CORE_CTL_POWER_D3hot:
    case AWSA_CORE_CTL_POWER_D3cold:
        if (p_widget->ignore_suspend) {
            AWSA_SOC_DEBUG_WARN("%s ignoring suspend",
                                 p_widget->p_driver->p_name);
        }
        return p_widget->ignore_suspend;

    default:
        return AW_TRUE;
    }
}

/*
 * Recursively check for a completed path to an active or physically connected
 * output widget. Returns number of complete paths.
 */
static
int __soc_dapm_is_connected_output_ep (struct awsa_soc_dapm_widget *p_widget)
{
    struct awsa_soc_dapm_path *p_path;
    int                        con = 0;

    if (p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_SUPPLY) {
        return AW_OK;
    }

    switch (p_widget->p_driver->type) {

    case AWSA_SOC_DAPM_TYPE_ADC :
    case AWSA_SOC_DAPM_TYPE_AIF_OUT :
        if (p_widget->active) {
            return __soc_dapm_suspend_check(p_widget);
        }
        break;

    default:
        break;
    }

    if (p_widget->connected) {
        /* connected pin ? */
        if (p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_OUTPUT &&
            !p_widget->ext) {
            return __soc_dapm_suspend_check(p_widget);
        }

        /* connected jack or spk ? */
        if (p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_HP   ||
            p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_SPK  ||
           (p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_LINE &&
            !aw_list_empty(&p_widget->sources))) {
            return __soc_dapm_suspend_check(p_widget);
        }
    }

    aw_list_for_each_entry(p_path,
                           &p_widget->sinks,
                           struct awsa_soc_dapm_path,
                           list_source) {
        if (p_path->walked) {
            continue;
        }

        if (p_path->p_sink && p_path->connected) {
            p_path->walked = 1;
            con += __soc_dapm_is_connected_output_ep(p_path->p_sink);
        }
    }

    return con;
}

/**
 * \brief Recursively check for a completed path to an active or
 *        physically connected input widget.
 */
static
int __soc_dapm_is_connected_input_ep (struct awsa_soc_dapm_widget *p_widget)
{
    struct awsa_soc_dapm_path *p_path;
    int                        con = 0;

    if (p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_SUPPLY) {
        return AW_OK;
    }

    /* active stream ? */
    switch (p_widget->p_driver->type) {

    case AWSA_SOC_DAPM_TYPE_DAC :
    case AWSA_SOC_DAPM_TYPE_AIF_IN :
        if (p_widget->active) {
            return __soc_dapm_suspend_check(p_widget);
        }
        break;

    default:
        break;
    }

    if (p_widget->connected) {
        /* connected pin ? */
        if (p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_INPUT && !p_widget->ext) {
            return __soc_dapm_suspend_check(p_widget);
        }

        /* connected VMID/Bias for lower pops */
        if (p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_VMID) {
            return __soc_dapm_suspend_check(p_widget);
        }

        /* connected jack ? */
        if (p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_MIC  ||
           (p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_LINE &&
            !aw_list_empty(&p_widget->sinks))) {
            return __soc_dapm_suspend_check(p_widget);
        }
    }

    aw_list_for_each_entry(p_path,
                           &p_widget->sources,
                           struct awsa_soc_dapm_path,
                           list_sink) {
        if (p_path->walked) {
            continue;
        }

        if (p_path->p_source && p_path->connected) {
            p_path->walked = 1;
            con += __soc_dapm_is_connected_input_ep(p_path->p_source);
        }
    }

    return con;
}

/**
 * \brief Standard power change method, used to apply power changes to most
 *        widgets.
 */
static
int __soc_dapm_generic_apply_power (struct awsa_soc_dapm_widget *p_widget)
{
    int ret;

    /* call any power change event handlers */
    if (p_widget->p_driver->pfn_event) {
        AWSA_SOC_DEBUG_VERBOSE("power %s event for %s flags %x",
                                p_widget->power ? "on" : "off",
                                p_widget->p_driver->p_name,
                                p_widget->p_driver->event_flags);
    }

    /* power up pre event */
    if (p_widget->power && p_widget->p_driver->pfn_event &&
        (p_widget->p_driver->event_flags & AWSA_SOC_DAPM_PRE_PMU)) {
        ret = p_widget->p_driver->pfn_event(p_widget,
                                            NULL,
                                            AWSA_SOC_DAPM_PRE_PMU);
        if (ret != AW_OK) {
            return ret;
        }
    }

    /* power down pre event */
    if (!p_widget->power && p_widget->p_driver->pfn_event &&
        (p_widget->p_driver->event_flags & AWSA_SOC_DAPM_PRE_PMD)) {
        ret = p_widget->p_driver->pfn_event(p_widget,
                                            NULL,
                                            AWSA_SOC_DAPM_PRE_PMD);
        if (ret != AW_OK) {
            return ret;
        }
    }

    __soc_dapm_bits_update(p_widget);

    /* power up post event */
    if (p_widget->power && p_widget->p_driver->pfn_event &&
        (p_widget->p_driver->event_flags & AWSA_SOC_DAPM_POST_PMU)) {
        ret = p_widget->p_driver->pfn_event(p_widget,
                                            NULL,
                                            AWSA_SOC_DAPM_POST_PMU);
        if (ret != AW_OK) {
            return ret;
        }
    }

    /* power down post event */
    if (!p_widget->power && p_widget->p_driver->pfn_event &&
        (p_widget->p_driver->event_flags & AWSA_SOC_DAPM_POST_PMD)) {
        ret = p_widget->p_driver->pfn_event(p_widget,
                                            NULL,
                                            AWSA_SOC_DAPM_POST_PMD);
        if (ret != AW_OK) {
            return ret;
        }
    }

    return AW_OK;
}

/**
 * \brief Generic check to see if a widget should be powered.
 */
static
int __soc_dapm_power_generic_check (struct awsa_soc_dapm_widget *p_widget)
{
    int in, out;

    in = __soc_dapm_is_connected_input_ep(p_widget);
    __soc_dapm_walk_clear(p_widget->p_dapm);
    out = __soc_dapm_is_connected_output_ep(p_widget);
    __soc_dapm_walk_clear(p_widget->p_dapm);

    return (out != 0 && in != 0);
}

/**
 * \brief Check to see if an ADC has power
 */
static
int __soc_dapm_adc_power_check (struct awsa_soc_dapm_widget *p_widget)
{
    int in;

    if (p_widget->active) {
        in = __soc_dapm_is_connected_input_ep(p_widget);
        __soc_dapm_walk_clear(p_widget->p_dapm);
        return (in != 0);
    } else {
        return __soc_dapm_power_generic_check(p_widget);
    }
}

/**
 * \brief Check to see if a DAC has power
 */
static int __soc_dapm_dac_power_check (struct awsa_soc_dapm_widget *p_widget)
{
    int out;

    if (p_widget->active) {
        out = __soc_dapm_is_connected_output_ep(p_widget);
        __soc_dapm_walk_clear(p_widget->p_dapm);
        return (out != 0);
    } else {
        return __soc_dapm_power_generic_check(p_widget);
    }
}

/**
 * \brief Check to see if a power supply is needed
 */
static
int __soc_dapm_supply_power_check (struct awsa_soc_dapm_widget *p_widget)
{
    struct awsa_soc_dapm_path *p_path;
    int                        power = 0;

    /* Check if one of our outputs is connected */
    aw_list_for_each_entry(p_path,
                           &p_widget->sinks,
                           struct awsa_soc_dapm_path,
                           list_source) {
        if (p_path->pfn_connected &&
            !p_path->pfn_connected(p_path->p_source, p_path->p_sink)) {
            continue;
        }

        if (!p_path->p_sink) {
            continue;
        }

        if (p_path->p_sink->force) {
            power = 1;
            break;
        }

        if (p_path->p_sink->pfn_power_check &&
            p_path->p_sink->pfn_power_check(p_path->p_sink)) {
            power = 1;
            break;
        }
    }

    __soc_dapm_walk_clear(p_widget->p_dapm);
    return power;
}

/**
 * \brief seq compare
 */
static int __soc_dapm_seq_compare (struct awsa_soc_dapm_widget *p_a,
                                   struct awsa_soc_dapm_widget *p_b,
                                   int                          sort[])
{
    if (sort[p_a->p_driver->type] != sort[p_b->p_driver->type]) {
        return sort[p_a->p_driver->type] - sort[p_b->p_driver->type];
    }

    if (p_a->p_driver->reg != p_b->p_driver->reg) {
        return p_a->p_driver->reg - p_b->p_driver->reg;
    }

    if (p_a->p_dapm != p_b->p_dapm) {
        return (uint32_t)p_a->p_dapm - (uint32_t)p_b->p_dapm;
    }

    return 0;
}

/**
 * \brief Insert a widget in order into a DAPM power sequence.
 */
static void __soc_dapm_seq_insert (struct awsa_soc_dapm_widget *p_new_widget,
                                   struct aw_list_head         *p_list,
                                   int                          sort[])
{
    struct awsa_soc_dapm_widget *p_widget;

    aw_list_for_each_entry(p_widget,
                           p_list,
                           struct awsa_soc_dapm_widget,
                           power_list) {
        if (__soc_dapm_seq_compare(p_new_widget, p_widget, sort) < 0) {
            aw_list_add_tail(&p_new_widget->power_list,
                             &p_widget->power_list);
            return;
        }
    }

    aw_list_add_tail(&p_new_widget->power_list, p_list);
}

/**
 * \brief check seq event
 */
static
void __soc_dapm_seq_event_check (struct awsa_soc_dapm_context *p_dapm,
                                 struct awsa_soc_dapm_widget  *p_widget,
                                 int                           event)
{
    const char *p_ev_name;
    int         power;
    aw_err_t    ret;

    switch (event) {

    case AWSA_SOC_DAPM_PRE_PMU:
        p_ev_name = "PRE_PMU";
        power = 1;
        break;

    case AWSA_SOC_DAPM_POST_PMU:
        p_ev_name = "POST_PMU";
        power = 1;
        break;

    case AWSA_SOC_DAPM_PRE_PMD:
        p_ev_name = "PRE_PMD";
        power = 0;
        break;

    case AWSA_SOC_DAPM_POST_PMD:
        p_ev_name = "POST_PMD";
        power = 0;
        break;

    default:
        return;
    }

    if (p_widget->power != power) {
        return;
    }

    if (p_widget->p_driver->pfn_event             &&
       (p_widget->p_driver->event_flags & event)) {
        AWSA_SOC_DEBUG_VERBOSE("pop test %s %s",
                               p_widget->p_driver->p_name,
                               p_ev_name);
        ret = p_widget->p_driver->pfn_event(p_widget, NULL, event);
        if (ret != AW_OK) {
            AWSA_SOC_DEBUG_ERROR("%s %s event failed: %d",
                                 p_ev_name,
                                 p_widget->p_driver->p_name,
                                 ret);
        }
    }
}

/**
 * \brief Apply the coalesced changes from a DAPM sequence
 */
static
void __soc_dapm_seq_run_coalesced (struct awsa_soc_dapm_context *p_dapm,
                                   struct aw_list_head          *p_pending)
{
    struct awsa_soc_card        *p_card      = p_dapm->p_card;
    struct awsa_soc_dapm_widget *p_widget;
    int                          reg, power;
    uint32_t                     value       = 0;
    uint32_t                     mask        = 0;
    uint32_t                     cur_mask;

    reg = aw_list_first_entry(p_pending,
                              struct awsa_soc_dapm_widget,
                              power_list)->p_driver->reg;

    aw_list_for_each_entry(p_widget,
                           p_pending,
                           struct awsa_soc_dapm_widget,
                           power_list) {
        cur_mask = 1 << p_widget->p_driver->shift;
//        awsa_soc_assert(reg == p_widget->p_driver->reg);

        if (p_widget->p_driver->invert) {
            power = !p_widget->power;
        } else {
            power = p_widget->power;
        }

        mask |= cur_mask;
        if (power) {
            value |= cur_mask;
        }

        AWSA_SOC_DEBUG_VERBOSE(
            "pop test - Queue %s: reg=0x%x, 0x%x/0x%x",
            p_widget->p_driver->p_name,
            reg,
            value,
            mask);

        /* Check for events */
        __soc_dapm_seq_event_check(p_dapm, p_widget, AWSA_SOC_DAPM_PRE_PMU);
        __soc_dapm_seq_event_check(p_dapm, p_widget, AWSA_SOC_DAPM_PRE_PMD);
    }

    if (reg >= 0) {
        AWSA_SOC_DEBUG_VERBOSE(
            "pop test - Applying 0x%x/0x%x to %x in %dms",
            value,
            mask,
            reg,
            p_card->pop_time);
        __soc_dapm_pop_wait(p_card->pop_time);
        awsa_soc_codec_bits_update(p_dapm->p_codec, reg, mask, value);
    }

    aw_list_for_each_entry(p_widget,
                           p_pending,
                           struct awsa_soc_dapm_widget,
                           power_list) {
        __soc_dapm_seq_event_check(p_dapm, p_widget, AWSA_SOC_DAPM_POST_PMU);
        __soc_dapm_seq_event_check(p_dapm, p_widget, AWSA_SOC_DAPM_POST_PMD);
    }
}

/**
 * \brief Apply a DAPM power sequence.
 */
static void __soc_dapm_seq_run (struct awsa_soc_dapm_context *p_dapm,
                                struct aw_list_head          *p_list,
                                int                           event,
                                int                           sort[])
{
    struct awsa_soc_dapm_widget  *p_widget   = NULL, *p_n = NULL;
    struct aw_list_head           pending;
    struct awsa_soc_dapm_context *p_cur_dapm = NULL;
    int                           cur_sort   = -1;
    int                           cur_reg    = AWSA_SOC_NOPM;
    aw_err_t                      ret;

    AW_INIT_LIST_HEAD(&pending);

    aw_list_for_each_entry_safe(p_widget,
                                p_n,
                                p_list,
                                struct awsa_soc_dapm_widget,
                                power_list) {
        ret = AW_OK;

        /* Do we need to apply any queued changes? */
        if (sort[p_widget->p_driver->type] != cur_sort    ||
            p_widget->p_driver->reg        != cur_reg     ||
            p_widget->p_dapm               != p_cur_dapm) {
            if (!aw_list_empty(&pending)) {
                __soc_dapm_seq_run_coalesced(p_cur_dapm, &pending);
            }

            AW_INIT_LIST_HEAD(&pending);
            cur_sort   = -1;
            cur_reg    = AWSA_SOC_NOPM;
            p_cur_dapm = NULL;
        }

        switch (p_widget->p_driver->type) {

        case AWSA_SOC_DAPM_TYPE_PRE :
            if (!p_widget->p_driver->pfn_event) {
                aw_list_for_each_entry_safe_continue(
                    p_widget,
                    p_n,
                    p_list,
                    struct awsa_soc_dapm_widget,
                    power_list);
            }

            if (event == AWSA_SOC_DAPM_STREAM_START) {
                ret = p_widget->p_driver->pfn_event(p_widget,
                                                    NULL,
                                                    AWSA_SOC_DAPM_PRE_PMU);
            } else if (event == AWSA_SOC_DAPM_STREAM_STOP) {
                ret = p_widget->p_driver->pfn_event(p_widget,
                                                    NULL,
                                                    AWSA_SOC_DAPM_PRE_PMD);
            }
            break;

        case AWSA_SOC_DAPM_TYPE_POST :
            if (!p_widget->p_driver->pfn_event) {
                aw_list_for_each_entry_safe_continue(
                    p_widget,
                    p_n,
                    p_list,
                    struct awsa_soc_dapm_widget,
                    power_list);
            }

            if (event == AWSA_SOC_DAPM_STREAM_START) {
                ret = p_widget->p_driver->pfn_event(p_widget,
                                                    NULL,
                                                    AWSA_SOC_DAPM_POST_PMU);
            } else if (event == AWSA_SOC_DAPM_STREAM_STOP) {
                ret = p_widget->p_driver->pfn_event(p_widget,
                                                    NULL,
                                                    AWSA_SOC_DAPM_POST_PMD);
            }
            break;

        case AWSA_SOC_DAPM_TYPE_INPUT :
        case AWSA_SOC_DAPM_TYPE_OUTPUT :
        case AWSA_SOC_DAPM_TYPE_HP :
        case AWSA_SOC_DAPM_TYPE_MIC :
        case AWSA_SOC_DAPM_TYPE_LINE :
        case AWSA_SOC_DAPM_TYPE_SPK :

            /* No register support currently */
            ret = __soc_dapm_generic_apply_power(p_widget);
            break;

        default:

            /* Queue it up for application */
            cur_sort   = sort[p_widget->p_driver->type];
            cur_reg    = p_widget->p_driver->reg;
            p_cur_dapm = p_widget->p_dapm;
            aw_list_move(&p_widget->power_list, &pending);
            break;
        }

        if (ret != AW_OK) {
            AWSA_SOC_DEBUG_WARN("failed to apply widget power: %d", ret);
        }
    }

    if (!aw_list_empty(&pending)) {
        __soc_dapm_seq_run_coalesced(p_cur_dapm, &pending);
    }
}

/**
 * \brief dapm waige update
 */
static void __soc_dapm_widget_update (struct awsa_soc_dapm_context *p_dapm)
{
    struct awsa_soc_dapm_update *p_update = p_dapm->p_update;
    struct awsa_soc_dapm_widget *p_widget;
    aw_err_t                     ret;

    if (p_update == NULL) {
        return;
    }

    p_widget = p_update->p_widget;

    if (p_widget->p_driver->pfn_event   &&
       (p_widget->p_driver->event_flags & AWSA_SOC_DAPM_PRE_REG)) {
        ret = p_widget->p_driver->pfn_event(p_widget,
                                            p_update->p_ctl,
                                            AWSA_SOC_DAPM_PRE_REG);
        if (ret != AW_OK) {
            AWSA_SOC_DEBUG_WARN("%s DAPM pre-event failed: %d",
                                p_widget->p_driver->p_name,
                                ret);
        }
    }

    ret = awsa_soc_codec_bits_update(p_widget->p_codec,
                                     p_update->reg,
                                     p_update->mask,
                                     p_update->val);
    if (ret != AW_OK) {
        AWSA_SOC_DEBUG_WARN("%s DAPM update failed: %d",
                             p_widget->p_driver->p_name,
                             ret);
    }

    if (p_widget->p_driver->pfn_event   &&
       (p_widget->p_driver->event_flags & AWSA_SOC_DAPM_POST_REG)) {
        ret = p_widget->p_driver->pfn_event(p_widget,
                                            p_update->p_ctl,
                                            AWSA_SOC_DAPM_POST_REG);
        if (ret != 0) {
            AWSA_SOC_DEBUG_WARN("%s DAPM post-event failed: %d",
                                 p_widget->p_driver->p_name,
                                 ret);
        }
    }
}

/**
 * \brief Scan each dapm widget for complete audio path.
 * \note A complete path is a route that has valid endpoints as:
 *        - DAC to output pin.
 *        - Input Pin to ADC.
 *        - Input pin to Output pin (bypass, sidetone)
 *        - DAC to ADC (loopback).
 */
static
aw_err_t __soc_dapm_widgets_power (struct awsa_soc_dapm_context *p_dapm,
                                   int                           event)
{
    struct awsa_soc_card         *p_card     = p_dapm->p_codec->p_card;
    struct awsa_soc_dapm_widget  *p_widget;
    struct awsa_soc_dapm_context *p_d;
    struct aw_list_head           up_list;
    struct aw_list_head           down_list;
    aw_err_t                      ret        = AW_OK;
    int                           power;

    AW_INIT_LIST_HEAD(&up_list);
    AW_INIT_LIST_HEAD(&down_list);

    aw_list_for_each_entry(p_d,
                           &p_card->dapm_list,
                           struct awsa_soc_dapm_context,
                           list) {
        if (p_d->n_widgets) {
            p_d->dev_power = 0;
        }
    }

    /*
     * Check which widgets we need to power and store them in
     * lists indicating if they should be powered up or down.
     */
    aw_list_for_each_entry(p_widget,
                           &p_card->widgets,
                           struct awsa_soc_dapm_widget,
                           list) {
        switch (p_widget->p_driver->type) {

        case AWSA_SOC_DAPM_TYPE_PRE :
            __soc_dapm_seq_insert(p_widget,
                                  &down_list,
                                  __g_soc_dapm_down_seq);
            break;

        case AWSA_SOC_DAPM_TYPE_POST :
            __soc_dapm_seq_insert(p_widget,
                                  &up_list,
                                  __g_soc_dapm_up_seq);
            break;

        default:
            if (!p_widget->pfn_power_check) {
                continue;
            }

            if (!p_widget->force) {
                power = p_widget->pfn_power_check(p_widget);
            } else {
                power = 1;
            }

            if (power) {
                p_widget->p_dapm->dev_power = 1;
            }

            AWSA_SOC_DEBUG_VERBOSE("widget '%s' power status %s",
                                   p_widget->p_driver->p_name,
                                   power ? "up": "down");

            if (p_widget->power == power) {
                continue;
            }

            if (power) {
                __soc_dapm_seq_insert(p_widget,
                                      &up_list,
                                      __g_soc_dapm_up_seq);
            } else {
                __soc_dapm_seq_insert(p_widget,
                                      &down_list,
                                      __g_soc_dapm_down_seq);
            }

            p_widget->power = power;
            break;
        }
    }

    /*
     * If there are no DAPM widgets then try to figure out power from the
     * event type.
     */
    if (!p_dapm->n_widgets) {

        switch (event) {

        case AWSA_SOC_DAPM_STREAM_START:
        case AWSA_SOC_DAPM_STREAM_RESUME:
            p_dapm->dev_power = 1;
            break;

        case AWSA_SOC_DAPM_STREAM_STOP:
            p_dapm->dev_power = !!p_dapm->p_codec->active;
            break;

        case AWSA_SOC_DAPM_STREAM_SUSPEND:
            p_dapm->dev_power = 0;
            break;

        case AWSA_SOC_DAPM_STREAM_NOP:

            switch (p_dapm->bias_level) {

            case AWSA_SOC_BIAS_STANDBY:
            case AWSA_SOC_BIAS_OFF:
                p_dapm->dev_power = 0;
                break;
            default:
                p_dapm->dev_power = 1;
                break;
            }

            break;
        default:
            break;
        }
    }

    aw_list_for_each_entry(p_d,
                           &p_dapm->p_card->dapm_list,
                           struct awsa_soc_dapm_context,
                           list) {
        if (p_d->dev_power && p_d->bias_level == AWSA_SOC_BIAS_OFF) {
            ret = __soc_dapm_bias_level_set(p_card,
                                            p_d,
                                            AWSA_SOC_BIAS_STANDBY);
            if (ret != 0) {
                AWSA_SOC_DEBUG_WARN("failed to turn on bias: %d", ret);
            }
        }

        /* If we're changing to all on or all off then prepare */
        if ((p_d->dev_power  && p_d->bias_level == AWSA_SOC_BIAS_STANDBY) ||
            (!p_d->dev_power && p_d->bias_level == AWSA_SOC_BIAS_ON)) {
            ret = __soc_dapm_bias_level_set(p_card,
                                            p_d,
                                            AWSA_SOC_BIAS_PREPARE);
            if (ret != 0) {
                AWSA_SOC_DEBUG_WARN("failed to prepare bias: %d", ret);
            }
        }
    }

    /* Power down widgets first; try to avoid amplifying pops. */
    __soc_dapm_seq_run(p_dapm, &down_list, event, __g_soc_dapm_down_seq);

    __soc_dapm_widget_update(p_dapm);

    /* Now power up. */
    __soc_dapm_seq_run(p_dapm, &up_list, event, __g_soc_dapm_up_seq);

    aw_list_for_each_entry(p_d,
                           &p_dapm->p_card->dapm_list,
                           struct awsa_soc_dapm_context,
                           list) {

        /* If we just powered the last thing off drop to standby bias */
        if (p_d->bias_level == AWSA_SOC_BIAS_PREPARE && !p_d->dev_power) {
            ret = __soc_dapm_bias_level_set(p_card,
                                            p_d,
                                            AWSA_SOC_BIAS_STANDBY);
            if (ret != AW_OK)
                AWSA_SOC_DEBUG_WARN("failed to apply standby bias: %d", ret);
        }

        /* If we're in standby and can support bias off then do that */
        if (p_d->bias_level == AWSA_SOC_BIAS_STANDBY &&
            p_d->idle_bias_off) {
            ret = __soc_dapm_bias_level_set(p_card,
                                            p_d,
                                            AWSA_SOC_BIAS_OFF);
            if (ret != AW_OK) {
                AWSA_SOC_DEBUG_WARN("failed to turn off bias: %d", ret);
            }
        }

        /* If we just powered up then move to active bias */
        if (p_d->bias_level == AWSA_SOC_BIAS_PREPARE && p_d->dev_power) {
            ret = __soc_dapm_bias_level_set(p_card, p_d, AWSA_SOC_BIAS_ON);
            if (ret != AW_OK) {
                AWSA_SOC_DEBUG_WARN("failed to apply active bias: %d", ret);
            }
        }
    }

    AWSA_SOC_DEBUG_INFO("DAPM sequencing finished, waiting %dms",
                        p_card->pop_time);
    __soc_dapm_pop_wait(p_card->pop_time);
    return AW_OK;
}

/**
 * \brief free all dapm widgets and resources
 */
static void __soc_dapm_widgets_free (struct awsa_soc_dapm_context *p_dapm)
{
    struct awsa_soc_dapm_widget *p_widget, *p_next_w;
    struct awsa_soc_dapm_path   *p_path,   *p_next_p;

    aw_list_for_each_entry_safe(p_widget,
                                p_next_w,
                                &p_dapm->p_card->widgets,
                                struct awsa_soc_dapm_widget,
                                list) {
        if (p_widget->p_dapm != p_dapm) {
            continue;
        }

        aw_list_del(&p_widget->list);

        /*
         * remove source and sink paths associated to this widget.
         * While removing the path, remove reference to it from both
         * source and sink widgets so that path is removed only once.
         */
        aw_list_for_each_entry_safe(p_path,
                                    p_next_p,
                                    &p_widget->sources,
                                    struct awsa_soc_dapm_path,
                                    list_sink) {
            aw_list_del(&p_path->list_sink);
            aw_list_del(&p_path->list_source);
            aw_list_del(&p_path->list);
        }

        aw_list_for_each_entry_safe(p_path,
                                    p_next_p,
                                    &p_widget->sinks,
                                    struct awsa_soc_dapm_path,
                                    list_source) {
            aw_list_del(&p_path->list_sink);
            aw_list_del(&p_path->list_source);
            aw_list_del(&p_path->list);
        }
    }
}

/**
 * \brief new a dapm control object
 */
aw_err_t awsa_soc_dapm_widget_add (
    struct awsa_soc_dapm_context             *p_dapm,
    struct awsa_soc_dapm_widget              *p_widget,
    const struct awsa_soc_dapm_widget_driver *p_driver,
    struct awsa_core_ctl_elem                *p_elem)
{

    p_dapm->n_widgets++;
    p_widget->p_dapm   = p_dapm;
    p_widget->p_codec  = p_dapm->p_codec;
    p_widget->p_driver = p_driver;
    p_widget->p_elem   = p_elem;

    AW_INIT_LIST_HEAD(&p_widget->sources);
    AW_INIT_LIST_HEAD(&p_widget->sinks);
    AW_INIT_LIST_HEAD(&p_widget->list);
    aw_list_add(&p_widget->list, &p_dapm->p_card->widgets);

    /* machine layer set ups unconnected pins and insertions */
    p_widget->connected = AW_TRUE;
    return AW_OK;
}

/**
 * \brief new mutilte dapm control
 */
aw_err_t awsa_soc_dapm_widgets_add (
    struct awsa_soc_dapm_context             *p_dapm,
    struct awsa_soc_dapm_widget              *p_widget,
    const struct awsa_soc_dapm_widget_driver *p_driver,
    int                                       num,
    struct awsa_core_ctl_elem                *p_elem)
{
    aw_err_t ret;
    int      i;

    for (i = 0; i < num; i++) {
        ret = awsa_soc_dapm_widget_add(p_dapm,
                                       &p_widget[i],
                                       &p_driver[i],
                                       p_elem);
        if (ret != AW_OK) {
            AWSA_SOC_DEBUG_ERROR("failed to create DAPM control %s: %d",
                                 p_widget->p_driver->p_name,
                                 ret);
            return ret;
        }
        p_elem += p_driver[i].num_elems;
    }

    return AW_OK;
}

/**
 * \brief dapm stream event
 */
aw_err_t awsa_soc_dapm_stream_event (struct awsa_soc_dapm_context *p_dapm,
                                     const char                   *p_stream,
                                     int                           event)
{
    struct awsa_soc_dapm_widget *p_w = NULL;

    aw_list_for_each_entry(p_w,
                           &p_dapm->p_card->widgets,
                           struct awsa_soc_dapm_widget,
                           list) {
        if (!p_w->p_driver->p_sname || p_w->p_dapm != p_dapm) {
            continue;
        }

        AWSA_SOC_DEBUG_INFO("widget %s\n %s stream %s event %d",
                             p_w->p_driver->p_name,
                             p_w->p_driver->p_sname,
                             p_stream,
                             event);

        if (strstr(p_w->p_driver->p_sname, p_stream)) {
            switch(event) {

            case AWSA_SOC_DAPM_STREAM_START:
                p_w->active = 1;
                break;

            case AWSA_SOC_DAPM_STREAM_STOP:
                p_w->active = 0;
                break;

            case AWSA_SOC_DAPM_STREAM_SUSPEND:
            case AWSA_SOC_DAPM_STREAM_RESUME:
            case AWSA_SOC_DAPM_STREAM_PAUSE_PUSH:
            case AWSA_SOC_DAPM_STREAM_PAUSE_RELEASE:
                break;
            }
        }
    }

    return __soc_dapm_widgets_power(p_dapm, event);
}

/**
 * \brief new mutile dapm widget object
 */
aw_err_t awsa_soc_dapm_new (struct awsa_soc_dapm_context *p_dapm)
{
    struct awsa_soc_dapm_widget *p_widget;
    uint32_t                     val;

    aw_list_for_each_entry(p_widget,
                           &p_dapm->p_card->widgets,
                           struct awsa_soc_dapm_widget,
                           list) {
        if (p_widget->cnew) {
            continue;
        }

        switch(p_widget->p_driver->type) {

        case AWSA_SOC_DAPM_TYPE_SWITCH :
        case AWSA_SOC_DAPM_TYPE_MIXER :
        case AWSA_SOC_DAPM_TYPE_MIXER_NAMED_CTL :
            p_widget->pfn_power_check = __soc_dapm_power_generic_check;
            __soc_dapm_mixer_add(p_dapm, p_widget);
            break;

        case AWSA_SOC_DAPM_TYPE_MUX :
        case AWSA_SOC_DAPM_TYPE_VIRT_MUX :
        case AWSA_SOC_DAPM_TYPE_VALUE_MUX :
            p_widget->pfn_power_check = __soc_dapm_power_generic_check;
            __soc_dapm_mux_add(p_dapm, p_widget);
            break;

        case AWSA_SOC_DAPM_TYPE_ADC :
        case AWSA_SOC_DAPM_TYPE_AIF_OUT :
            p_widget->pfn_power_check = __soc_dapm_adc_power_check;
            break;

        case AWSA_SOC_DAPM_TYPE_DAC :
        case AWSA_SOC_DAPM_TYPE_AIF_IN :
            p_widget->pfn_power_check = __soc_dapm_dac_power_check;
            break;

        case AWSA_SOC_DAPM_TYPE_PGA :
        case AWSA_SOC_DAPM_TYPE_OUT_DRV :
            p_widget->pfn_power_check = __soc_dapm_power_generic_check;
            __soc_dapm_pga_add(p_dapm, p_widget);
            break;

        case AWSA_SOC_DAPM_TYPE_INPUT :
        case AWSA_SOC_DAPM_TYPE_OUTPUT :
        case AWSA_SOC_DAPM_TYPE_MICBIAS :
        case AWSA_SOC_DAPM_TYPE_SPK :
        case AWSA_SOC_DAPM_TYPE_HP :
        case AWSA_SOC_DAPM_TYPE_MIC :
        case AWSA_SOC_DAPM_TYPE_LINE :
            p_widget->pfn_power_check = __soc_dapm_power_generic_check;
            break;

        case AWSA_SOC_DAPM_TYPE_SUPPLY :
            p_widget->pfn_power_check = __soc_dapm_supply_power_check;
            /* no break */
        case AWSA_SOC_DAPM_TYPE_VMID :
        case AWSA_SOC_DAPM_TYPE_PRE :
        case AWSA_SOC_DAPM_TYPE_POST :
            break;
        }

        /* Read the initial power state from the device */
        if (p_widget->p_driver->reg >= 0) {
            awsa_soc_codec_read(p_widget->p_codec,
                                p_widget->p_driver->reg,
                                &val);
            val &= 1 << p_widget->p_driver->shift;
            if (p_widget->p_driver->invert) {
                val = !val;
            }

            if (val) {
                p_widget->power = 1;
            }
        }

        p_widget->cnew = 1;
    }

    __soc_dapm_widgets_power(p_dapm, AWSA_SOC_DAPM_STREAM_NOP);
    return AW_OK;
}

/**
 * \brief free dapm widgets
 */
void awsa_soc_dapm_free (struct awsa_soc_dapm_context *p_dapm)
{
    __soc_dapm_widgets_free(p_dapm);
    aw_list_del(&p_dapm->list);
}

/**
 * \brief
 */
static
aw_err_t __soc_dapm_route_add (struct awsa_soc_dapm_context     *p_dapm,
                               struct awsa_soc_dapm_path        *p_path,
                               const struct awsa_soc_dapm_route *p_route)
{
    struct awsa_soc_dapm_widget *p_wsource  = NULL, *p_wsink  = NULL, *p_wdiget;
    struct awsa_soc_dapm_widget *p_wtsource = NULL, *p_wtsink = NULL;
    const char                  *p_sink;
    const char                  *p_ctl      = p_route->p_ctl;
    const char                  *p_source;
    aw_err_t                     ret        = AW_OK;

    awsa_soc_assert(p_dapm  != NULL);
    awsa_soc_assert(p_path  != NULL);
    awsa_soc_assert(p_route != NULL);

    p_sink   = p_route->p_sink;
    p_source = p_route->p_source;

    /*
     * find src and dest widgets over all widgets but favor a widget from
     * current DAPM context
     */
    aw_list_for_each_entry(p_wdiget,
                           &p_dapm->p_card->widgets,
                           struct awsa_soc_dapm_widget,
                           list) {
        if (!p_wsink && !(strcmp(p_wdiget->p_driver->p_name, p_sink))) {
            p_wtsink = p_wdiget;
            if (p_wdiget->p_dapm == p_dapm) {
                p_wsink = p_wdiget;
            }
            continue;
        }
        if (!p_wsource && !(strcmp(p_wdiget->p_driver->p_name, p_source))) {
            p_wtsource = p_wdiget;
            if (p_wdiget->p_dapm == p_dapm) {
                p_wsource = p_wdiget;
            }
        }
    }

    /* use widget from another DAPM context if not found from this */
    if (!p_wsink) {
        p_wsink = p_wtsink;
    }
    if (!p_wsource) {
        p_wsource = p_wtsource;
    }

    if (p_wsource == NULL || p_wsink == NULL) {
        return -AW_ENODEV;
    }

    p_path->p_source      = p_wsource;
    p_path->p_sink        = p_wsink;
    p_path->pfn_connected = p_route->pfn_connect_check;

    AW_INIT_LIST_HEAD(&p_path->list);
    AW_INIT_LIST_HEAD(&p_path->list_source);
    AW_INIT_LIST_HEAD(&p_path->list_sink);

    /* check for external widgets */
    if (p_wsink->p_driver->type == AWSA_SOC_DAPM_TYPE_INPUT) {
        if (p_wsource->p_driver->type == AWSA_SOC_DAPM_TYPE_MICBIAS ||
            p_wsource->p_driver->type == AWSA_SOC_DAPM_TYPE_MIC     ||
            p_wsource->p_driver->type == AWSA_SOC_DAPM_TYPE_LINE    ||
            p_wsource->p_driver->type == AWSA_SOC_DAPM_TYPE_OUTPUT) {
            p_wsink->ext  = 1;
        }
    }

    if (p_wsource->p_driver->type == AWSA_SOC_DAPM_TYPE_OUTPUT) {
        if (p_wsink->p_driver->type == AWSA_SOC_DAPM_TYPE_SPK   ||
            p_wsink->p_driver->type == AWSA_SOC_DAPM_TYPE_HP    ||
            p_wsink->p_driver->type == AWSA_SOC_DAPM_TYPE_LINE  ||
            p_wsink->p_driver->type == AWSA_SOC_DAPM_TYPE_INPUT) {
            p_wsource->ext = 1;
        }
    }

    /* connect static paths */
    if (p_ctl == NULL) {
        aw_list_add(&p_path->list, &p_dapm->p_card->paths);
        aw_list_add(&p_path->list_sink, &p_wsink->sources);
        aw_list_add(&p_path->list_source, &p_wsource->sinks);
        p_path->connected = 1;
        return AW_OK;
    }

    /* connect dynamic paths */
    switch(p_wsink->p_driver->type) {

    case AWSA_SOC_DAPM_TYPE_ADC:
    case AWSA_SOC_DAPM_TYPE_DAC:
    case AWSA_SOC_DAPM_TYPE_PGA:
    case AWSA_SOC_DAPM_TYPE_OUT_DRV:
    case AWSA_SOC_DAPM_TYPE_INPUT:
    case AWSA_SOC_DAPM_TYPE_OUTPUT:
    case AWSA_SOC_DAPM_TYPE_MICBIAS:
    case AWSA_SOC_DAPM_TYPE_VMID:
    case AWSA_SOC_DAPM_TYPE_PRE:
    case AWSA_SOC_DAPM_TYPE_POST:
    case AWSA_SOC_DAPM_TYPE_SUPPLY:
    case AWSA_SOC_DAPM_TYPE_AIF_IN:
    case AWSA_SOC_DAPM_TYPE_AIF_OUT:
        aw_list_add(&p_path->list, &p_dapm->p_card->paths);
        aw_list_add(&p_path->list_sink, &p_wsink->sources);
        aw_list_add(&p_path->list_source, &p_wsource->sinks);
        p_path->connected = 1;
        return AW_OK;

    case AWSA_SOC_DAPM_TYPE_MUX:
    case AWSA_SOC_DAPM_TYPE_VIRT_MUX:
    case AWSA_SOC_DAPM_TYPE_VALUE_MUX:
        ret = __soc_dapm_mux_connect(p_dapm,
                                     p_wsource,
                                     p_wsink,
                                     p_path,
                                     p_ctl,
                                     &p_wsink->p_driver->p_elems_driver[0]);
        if (ret != AW_OK) {
            goto __err;
        }
        break;

    case AWSA_SOC_DAPM_TYPE_SWITCH:
    case AWSA_SOC_DAPM_TYPE_MIXER:
    case AWSA_SOC_DAPM_TYPE_MIXER_NAMED_CTL:
        ret = __soc_dapm_mixer_connect(p_dapm,
                                       p_wsource,
                                       p_wsink,
                                       p_path,
                                       p_ctl);
        if (ret != AW_OK) {
            goto __err;
        }
        break;

    case AWSA_SOC_DAPM_TYPE_HP:
    case AWSA_SOC_DAPM_TYPE_MIC:
    case AWSA_SOC_DAPM_TYPE_LINE:
    case AWSA_SOC_DAPM_TYPE_SPK:
        aw_list_add(&p_path->list,        &p_dapm->p_card->paths);
        aw_list_add(&p_path->list_sink,   &p_wsink->sources);
        aw_list_add(&p_path->list_source, &p_wsource->sinks);
        p_path->connected = 0;

        return AW_OK;
    }

    return AW_OK;

__err:
    AWSA_SOC_DEBUG_ERROR("no dapm match for %s --> %s --> %s",
                          p_source,
                          p_ctl,
                          p_sink);
    return ret;
}

/**
 * \brief Add mutile dapm route to dapm context
 */
aw_err_t awsa_soc_dapm_routes_add (struct awsa_soc_dapm_context     *p_dapm,
                                   struct awsa_soc_dapm_path        *p_path,
                                   const struct awsa_soc_dapm_route *p_route,
                                   int                               num)
{
    int      i;
    aw_err_t ret;

    for (i = 0; i < num; i++) {
        ret = __soc_dapm_route_add(p_dapm, &p_path[i], &p_route[i]);
        if (ret != AW_OK) {
            AWSA_SOC_DEBUG_ERROR("Failed to add route %s->%s",
                                  p_route->p_source,
                                  p_route->p_sink);
            return ret;
        }
    }

    return AW_OK;
}

/**
 * \brief dapm pin set
 * \note pin level state change will cause dapm route reset
 */
static aw_err_t __soc_dapm_widget_set (
    struct awsa_soc_dapm_context *p_dapm,
    const char                   *p_name,
    int                           status)
{
    struct awsa_soc_dapm_widget *p_w;

    aw_list_for_each_entry(p_w,
                           &p_dapm->p_card->widgets,
                           struct awsa_soc_dapm_widget,
                           list) {
        if (p_w->p_dapm != p_dapm) {
            continue;
        }

        if (!strcmp(p_w->p_driver->p_name, p_name)) {
            AWSA_SOC_DEBUG_INFO("dapm widget %s = %d", p_name, status);
            p_w->connected = status;

            /* Allow disabling of forced pins */
            if (status == 0) {
                p_w->force = 0;
            }

            return AW_OK;
        }
    }

    AWSA_SOC_DEBUG_WARN("dapm unknown widget %s", p_name);
    return -AW_EINVAL;
}

/**
 * \brief update dapm context
 */
aw_err_t awsa_soc_dapm_sync (struct awsa_soc_dapm_context *p_dapm)
{
    return __soc_dapm_widgets_power(p_dapm, AWSA_SOC_DAPM_STREAM_NOP);
}

/**
 * \brief enable dapm pin
 */
aw_err_t awsa_soc_dapm_widget_enable (struct awsa_soc_dapm_context *p_dapm,
                                      const char                   *p_name)
{
    return __soc_dapm_widget_set(p_dapm, p_name, 1);
}

/**
 * \brief disable dapm pin
 */
aw_err_t awsa_soc_dapm_widget_disable (struct awsa_soc_dapm_context *p_dapm,
                                       const char                   *p_name)
{
    return __soc_dapm_widget_set(p_dapm, p_name, 0);
}

/**
 * \brief Handler for generic register modifier widget.
 */
aw_err_t awsa_soc_dapm_reg_event (struct awsa_soc_dapm_widget *p_widget,
                                  struct awsa_core_ctl        *p_ctl,
                                  int                          event)
{
    uint32_t val;

    if (AWSA_SOC_DAPM_EVENT_ON(event)) {
        val = p_widget->p_driver->on_val;
    } else {
        val = p_widget->p_driver->off_val;
    }

    return awsa_soc_codec_bits_update(
               p_widget->p_codec,
               -(p_widget->p_driver->reg + 1),
               p_widget->p_driver->mask << p_widget->p_driver->shift,
               val << p_widget->p_driver->shift);
}

/******************************************************************************/

/**
 * test register for change
 */
static
aw_bool_t __soc_dapm_test_bits (struct awsa_soc_dapm_widget *p_widget,
                                uint32_t                     reg,
                                uint32_t                     mask,
                                uint32_t                     value)
{
    aw_bool_t   change;
    uint32_t    old, new;

    awsa_soc_codec_read(p_widget->p_codec, reg, &old);
    new    = (old & ~mask) | value;
    change =  old != new;

    return change;
}

/**
 * \brief test and update the power status of a mux widget
 */
static
aw_err_t __soc_dapm_mux_power_update (struct awsa_soc_dapm_widget *p_widget,
                                      struct awsa_core_ctl_elem   *p_ctl,
                                      aw_bool_t                    change,
                                      int                          mux,
                                      struct awsa_soc_enum        *p_e)
{
    struct awsa_soc_dapm_path *p_path;
    int                        found = 0;

    if (p_widget->p_driver->type != AWSA_SOC_DAPM_TYPE_MUX        &&
        p_widget->p_driver->type != AWSA_SOC_DAPM_TYPE_VIRT_MUX   &&
        p_widget->p_driver->type != AWSA_SOC_DAPM_TYPE_VALUE_MUX) {
        return -AW_ENODEV;
    }

    if (!change) {
        return AW_OK;
    }

    /* find dapm widget path assoc with kcontrol */
    aw_list_for_each_entry(p_path,
                           &p_widget->p_dapm->p_card->paths,
                           struct awsa_soc_dapm_path,
                           list) {
        if (p_path->p_elem_driver != p_ctl->p_driver) {
            continue;
        }

        if (!p_path->p_name || !p_e->p_texts[mux]) {
            continue;
        }

        found = 1;

        /* we now need to match the string in the enum to the path */
        if (!(strcmp(p_path->p_name, p_e->p_texts[mux]))) {
            p_path->connected = 1; /* new connection */
        } else {
            p_path->connected = 0; /* old connection must be powered down */
        }
    }

    if (found) {
        __soc_dapm_widgets_power(p_widget->p_dapm, AWSA_SOC_DAPM_STREAM_NOP);
    }

    return AW_OK;
}

/**
 * \brief test and update the power status of a mixer or switch widget
 */
static
aw_err_t __soc_dapm_mixer_power_update (struct awsa_soc_dapm_widget *p_widget,
                                        struct awsa_core_ctl_elem   *p_ctl,
                                        int                          connect)
{
    struct awsa_soc_dapm_path *p_path;
    int                        found = 0;

    if (p_widget->p_driver->type != AWSA_SOC_DAPM_TYPE_MIXER           &&
        p_widget->p_driver->type != AWSA_SOC_DAPM_TYPE_MIXER_NAMED_CTL &&
        p_widget->p_driver->type != AWSA_SOC_DAPM_TYPE_SWITCH) {
        return -AW_ENODEV;
    }

    /* find dapm widget path assoc with kcontrol */
    aw_list_for_each_entry(p_path,
                           &p_widget->p_dapm->p_card->paths,
                           struct awsa_soc_dapm_path,
                           list) {
        if (p_path->p_elem_driver != p_ctl->p_driver) {
            continue;
        }

        /* found, now check type */
        found             = 1;
        p_path->connected = connect;
        break;
    }

    if (found) {
        __soc_dapm_widgets_power(p_widget->p_dapm, AWSA_SOC_DAPM_STREAM_NOP);
    }

    return AW_OK;
}

/**
 * \brief get audio pin status
 */
static
aw_err_t __soc_dapm_widget_status_get (struct awsa_soc_dapm_context *p_dapm,
                                       const char                   *p_pin)
{
    struct awsa_soc_dapm_widget *p_w;

    aw_list_for_each_entry(p_w,
                           &p_dapm->p_card->widgets,
                           struct awsa_soc_dapm_widget,
                           list) {
        if (p_w->p_dapm != p_dapm) {
            continue;
        }

        if (strcmp(p_w->p_driver->p_name, p_pin) == 0) {
            return p_w->connected;
        }
    }

    return AW_OK;
}

/**
 * \brief set mux value and update power status
 */
aw_err_t awsa_soc_dapm_mux_power_update (
    struct awsa_soc_dapm_context *p_dapm,
    const char                   *p_widget_name,
    int                           mux)
{
    struct awsa_soc_card         *p_card     = p_dapm->p_codec->p_card;
    struct awsa_soc_dapm_widget  *p_widget;
    struct awsa_soc_enum         *p_e;
    struct awsa_soc_dapm_update   update;

    uint32_t                      val;
    aw_bool_t                     change;
    uint32_t                      mask, bitmask;

    if (p_dapm == NULL || p_widget_name == NULL) {
        return -AW_EINVAL;
    }

    /*
     * Check which widgets we need to power and store them in
     * lists indicating if they should be powered up or down.
     */
    aw_list_for_each_entry(p_widget,
                           &p_card->widgets,
                           struct awsa_soc_dapm_widget,
                           list) {
        if (p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_MUX      ||
            p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_VIRT_MUX ||
            p_widget->p_driver->type == AWSA_SOC_DAPM_TYPE_VALUE_MUX) {
            if (!strcmp(p_widget->p_driver->p_name, p_widget_name)) {
                if (p_widget->p_driver->p_elems_driver !=
                    p_widget->p_elem->p_driver) {
                    continue;
                }

                p_e = (struct awsa_soc_enum *)
                       p_widget->p_elem->p_driver->private_value;

                if (mux > p_e->max - 1) {
                    continue;
                }

                for (bitmask = 1; bitmask < p_e->max; bitmask <<= 1) {
                    ;
                }

                val  = mux << p_e->shift_l;
                mask = (bitmask - 1) << p_e->shift_l;

                AW_MUTEX_LOCK(p_widget->p_codec->mutex, AW_SEM_WAIT_FOREVER);
                p_widget->value = val;
                change = __soc_dapm_test_bits(p_widget, p_e->reg, mask, val);

                update.p_ctl               = p_widget->p_elem->p_ctl;
                update.p_widget            = p_widget;
                update.reg                 = p_e->reg;
                update.mask                = mask;
                update.val                 = val;
                p_widget->p_dapm->p_update = &update;

                __soc_dapm_mux_power_update(p_widget,
                                            p_widget->p_elem,
                                            change,
                                            mux,
                                            p_e);

                p_widget->p_dapm->p_update = NULL;

                AW_MUTEX_UNLOCK(p_widget->p_codec->mutex);

                break;
            }
        }
    }

    return AW_OK;
}

/**
 * \brief set mixer connect value and update power status
 */
aw_err_t awsa_soc_dapm_mixer_power_update (
    struct awsa_soc_dapm_context *p_dapm,
    const char                   *p_ctl_elem_name,
    int                          *p_value)
{
    struct awsa_soc_card            *p_card     = p_dapm->p_codec->p_card;
    struct awsa_core_ctl_elem       *p_ctl_elem = NULL;
    struct awsa_core_ctl_elem_value  value;

    p_ctl_elem = awsa_core_ctl_elem_find(&p_card->ctl,
                                         p_ctl_elem_name,
                                         AWSA_CORE_CTL_ELEM_IF_MIXER,
                                         0);
    if (p_ctl_elem == NULL) {
        return -AW_ENODEV;
    }

    value.value.integer.p_value = (int32_t *)p_value;
    return p_ctl_elem->p_driver->pfn_value_set(p_ctl_elem, &value);
}

/**
 * dapm mixer set callback
 */
aw_err_t awsa_soc_dapm_volsw_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_dapm_widget *p_widget = p_ctl_elem->p_private_data;
    struct awsa_soc_mixer_ctl   *p_mc     = (struct awsa_soc_mixer_ctl *)
                                            p_ctl_elem->p_driver->private_value;
    struct awsa_soc_dapm_update  update;

    uint32_t  reg    = p_mc->reg_l;
    uint32_t  shift  = p_mc->shift_l;
    int       max    = p_mc->max;
    uint32_t  mask   = (1 << awsa_soc_fls(max)) - 1;
    uint32_t  invert = p_mc->invert;
    uint32_t  val;
    aw_bool_t connect;
    aw_bool_t change;

    val = (p_value->value.integer.p_value[0] & mask);

    if (invert) {
        val = max - val;
    }
    mask = mask << shift;
    val  = val  << shift;

    AW_MUTEX_LOCK(p_widget->p_codec->mutex, AW_SEM_WAIT_FOREVER);
    p_widget->value = val;

    change = __soc_dapm_test_bits(p_widget, reg, mask, val);
    if (change) {
        if (val) {
            /* new connection */
            connect = invert ? AW_FALSE : AW_TRUE;
        } else {
            /* old connection must be powered down */
            connect = invert ? AW_TRUE : AW_FALSE;
        }

        update.p_ctl               = p_ctl_elem->p_ctl;
        update.p_widget            = p_widget;
        update.reg                 = reg;
        update.mask                = mask;
        update.val                 = val;
        p_widget->p_dapm->p_update = &update;

        __soc_dapm_mixer_power_update(p_widget, p_ctl_elem, connect);

        p_widget->p_dapm->p_update = NULL;
    }

    AW_MUTEX_UNLOCK(p_widget->p_codec->mutex);
    return AW_OK;
}

/**
 * \brief dapm mixer get callback
 */
aw_err_t awsa_soc_dapm_volsw_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_dapm_widget *p_widget = p_ctl_elem->p_private_data;
    struct awsa_soc_mixer_ctl   *p_mc     = (struct awsa_soc_mixer_ctl *)
                                            p_ctl_elem->p_driver->private_value;
    uint32_t reg    = p_mc->reg_l;
    uint32_t shift  = p_mc->shift_l;
    uint32_t rshift = p_mc->shift_r;
    int      max    = p_mc->max;
    uint32_t invert = p_mc->invert;
    uint32_t mask   = (1 << awsa_soc_fls(max)) - 1;
    uint32_t val, valr;

    awsa_soc_codec_read(p_widget->p_codec, reg, &val);
    p_value->value.integer.p_value[0] = (val >> shift) & mask;

    if (shift != rshift) {
        awsa_soc_codec_read(p_widget->p_codec, reg, &valr);
        p_value->value.integer.p_value[1] = (valr >> rshift) & mask;
    }

    if (invert) {
        p_value->value.integer.p_value[0] = max -
                                            p_value->value.integer.p_value[0];
        if (shift != rshift) {
            p_value->value.integer.p_value[1] =
                max - p_value->value.integer.p_value[1];
        }
    }

    return AW_OK;
}

/**
 * \brief dapm enumerated double mixer get callback
 */
aw_err_t awsa_soc_dapm_enum_double_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_dapm_widget *p_widget = p_ctl_elem->p_private_data;
    struct awsa_soc_enum        *p_e      = (struct awsa_soc_enum *)
                                            p_ctl_elem->p_driver->private_value;
    uint32_t val, bitmask;

    for (bitmask = 1; bitmask < p_e->max; bitmask <<= 1) {
        ;
    }

    awsa_soc_codec_read(p_widget->p_codec, p_e->reg, &val);
    p_value->value.enumerated.p_item[0] = (val >> p_e->shift_l) & (bitmask - 1);
    if (p_e->shift_l != p_e->shift_r) {
        p_value->value.enumerated.p_item[1] = (val >> p_e->shift_r) &
                                              (bitmask - 1);
    }

    return AW_OK;
}

/**
 * \brief dapm enumerated double mixer set callback
 */
aw_err_t awsa_soc_dapm_enum_double_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_dapm_widget *p_widget = p_ctl_elem->p_private_data;
    struct awsa_soc_enum        *p_e      = (struct awsa_soc_enum *)
                                            p_ctl_elem->p_driver->private_value;
    struct awsa_soc_dapm_update  update;

    uint32_t  val, mux;
    aw_bool_t change;
    uint32_t  mask, bitmask;

    for (bitmask = 1; bitmask < p_e->max; bitmask <<= 1) {
        ;
    }

    if (p_value->value.enumerated.p_item[0] > p_e->max - 1) {
        return -AW_EINVAL;
    }

    mux  = p_value->value.enumerated.p_item[0];
    val  = mux << p_e->shift_l;
    mask = (bitmask - 1) << p_e->shift_l;

    if (p_e->shift_l != p_e->shift_r) {
        if (p_value->value.enumerated.p_item[1] > p_e->max - 1) {
            return -AW_EINVAL;
        }
        val  |= p_value->value.enumerated.p_item[1] << p_e->shift_r;
        mask |= (bitmask - 1) << p_e->shift_r;
    }

    AW_MUTEX_LOCK(p_widget->p_codec->mutex, AW_SEM_WAIT_FOREVER);
    p_widget->value = val;
    change = __soc_dapm_test_bits(p_widget, p_e->reg, mask, val);

    update.p_ctl               = p_ctl_elem->p_ctl;
    update.p_widget            = p_widget;
    update.reg                 = p_e->reg;
    update.mask                = mask;
    update.val                 = val;
    p_widget->p_dapm->p_update = &update;

    __soc_dapm_mux_power_update(p_widget, p_ctl_elem, change, mux, p_e);

    p_widget->p_dapm->p_update = NULL;

    AW_MUTEX_UNLOCK(p_widget->p_codec->mutex);
    return AW_OK;
}

/**
 * \brief Get virtual DAPM mux
 */
aw_err_t awsa_soc_dapm_enum_virt_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_dapm_widget *p_widget = p_ctl_elem->p_private_data;

    p_value->value.enumerated.p_item[0] = p_widget->value;
    return AW_OK;
}

/**
 * \brief Set virtual DAPM mux
 */
aw_err_t awsa_soc_dapm_enum_virt_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_dapm_widget *p_widget = p_ctl_elem->p_private_data;
    struct awsa_soc_enum        *p_e      = (struct awsa_soc_enum *)
                                            p_ctl_elem->p_driver->private_value;
    aw_bool_t                    change;

    if (p_value->value.enumerated.p_item[0] >= p_e->max) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_widget->p_codec->mutex, AW_SEM_WAIT_FOREVER);

    change          = p_widget->value != p_value->value.enumerated.p_item[0];
    p_widget->value = p_value->value.enumerated.p_item[0];

    __soc_dapm_mux_power_update(p_widget,
                                p_ctl_elem,
                                change,
                                p_widget->value,
                                p_e);

    AW_MUTEX_UNLOCK(p_widget->p_codec->mutex);
    return AW_OK;
}

/**
 * \brief dapm enumerated double mixer get callback
 */
aw_err_t awsa_soc_dapm_enum_double_value_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_dapm_widget *p_widget = p_ctl_elem->p_private_data;
    struct awsa_soc_enum        *p_e      = (struct awsa_soc_enum *)
                                            p_ctl_elem->p_driver->private_value;
    uint32_t reg_val, val, mux;

    awsa_soc_codec_read(p_widget->p_codec, p_e->reg, &reg_val);
    val = (reg_val >> p_e->shift_l) & p_e->mask;
    for (mux = 0; mux < p_e->max; mux++) {
        if (val == p_e->p_values[mux]) {
            break;
        }
    }

    p_value->value.enumerated.p_item[0] = mux;
    if (p_e->shift_l != p_e->shift_r) {
        val = (reg_val >> p_e->shift_r) & p_e->mask;
        for (mux = 0; mux < p_e->max; mux++) {
            if (val == p_e->p_values[mux]) {
                break;
            }
        }
        p_value->value.enumerated.p_item[1] = mux;
    }

    return AW_OK;
}

/**
 * \brief dapm enumerated double mixer set callback
 */
aw_err_t awsa_soc_dapm_enum_double_value_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_dapm_widget *p_widget = p_ctl_elem->p_private_data;
    struct awsa_soc_enum        *p_e      = (struct awsa_soc_enum *)
                                            p_ctl_elem->p_driver->private_value;
    struct awsa_soc_dapm_update  update;

    uint32_t  val, mux;
    uint32_t  mask;
    aw_bool_t change;

    if (p_value->value.enumerated.p_item[0] > p_e->max - 1) {
        return -AW_EINVAL;
    }

    mux  = p_value->value.enumerated.p_item[0];
    val  = p_e->p_values[p_value->value.enumerated.p_item[0]] << p_e->shift_l;
    mask = p_e->mask << p_e->shift_l;
    if (p_e->shift_l != p_e->shift_r) {
        if (p_value->value.enumerated.p_item[1] > p_e->max - 1) {
            return -AW_EINVAL;
        }
        val  |= p_e->p_values[p_value->value.enumerated.p_item[1]] <<
                p_e->shift_r;
        mask |= p_e->mask << p_e->shift_r;
    }

    AW_MUTEX_LOCK(p_widget->p_codec->mutex, AW_SEM_WAIT_FOREVER);
    p_widget->value = val;
    change = __soc_dapm_test_bits(p_widget, p_e->reg, mask, val);

    update.p_ctl               = p_ctl_elem->p_ctl;
    update.p_widget            = p_widget;
    update.reg                 = p_e->reg;
    update.mask                = mask;
    update.val                 = val;
    p_widget->p_dapm->p_update = &update;

    __soc_dapm_mux_power_update(p_widget, p_ctl_elem, change, mux, p_e);

    p_widget->p_dapm->p_update = NULL;

    AW_MUTEX_UNLOCK(p_widget->p_codec->mutex);
    return AW_OK;
}

/**
 * \brief Information for a pin switch
 */
aw_err_t awsa_soc_dapm_pin_switch_info (
    struct awsa_core_ctl_elem      *p_ctl_elem,
    struct awsa_core_ctl_elem_info *p_info)
{
    p_info->type             = AWSA_CORE_CTL_ELEM_TYPE_BOOLEAN;
    p_info->count            = 1;
    p_info->info.integer.min = 0;
    p_info->info.integer.max = 1;
    return AW_OK;
}

/**
 * \brief get audio pin status
 */
aw_err_t awsa_soc_dapm_pin_switch_get (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_codec *p_codec = p_ctl_elem->p_private_data;
    const char            *p_pin   = (const char *)
                                     p_ctl_elem->p_driver->private_value;

    AW_MUTEX_LOCK(p_codec->mutex, AW_SEM_WAIT_FOREVER);

    p_value->value.integer.p_value[0] = __soc_dapm_widget_status_get(
                                            &p_codec->dapm,
                                            p_pin);

    AW_MUTEX_UNLOCK(p_codec->mutex);

    return 0;
}

/**
 * \brief Set information for a pin switch
 */
aw_err_t awsa_soc_dapm_pin_switch_set (
    struct awsa_core_ctl_elem       *p_ctl_elem,
    struct awsa_core_ctl_elem_value *p_value)
{
    struct awsa_soc_codec *p_codec = p_ctl_elem->p_private_data;
    const char            *p_pin   = (const char *)
                                     p_ctl_elem->p_driver->private_value;

    AW_MUTEX_LOCK(p_codec->mutex, AW_SEM_WAIT_FOREVER);

    if (p_value->value.integer.p_value[0]) {
        awsa_soc_dapm_widget_enable(&p_codec->dapm, p_pin);
    } else {
        awsa_soc_dapm_widget_disable(&p_codec->dapm, p_pin);
    }

    awsa_soc_dapm_sync(&p_codec->dapm);

    AW_MUTEX_UNLOCK(p_codec->mutex);

    return 0;
}

/* end of file */
