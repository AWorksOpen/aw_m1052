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
 * \brief DAPM(Dynamic Audio Power Management)
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-11-11  win, first implementation
 * \endinternal
 */

#ifndef __AWSA_SOC_DAPM_H
#define __AWSA_SOC_DAPM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "soc/awsa_soc_internal.h"

struct awsa_soc_dapm_widget;
struct awsa_soc_pcm_runtime;
enum awsa_soc_bias_level;

/** \brief widget has no power managment register bit */
#define AWSA_SOC_NOPM    (-1)

/**
 * \name  dapm stream operations
 * @{
 */

#define AWSA_SOC_DAPM_STREAM_NOP             0x0
#define AWSA_SOC_DAPM_STREAM_START           0x1
#define AWSA_SOC_DAPM_STREAM_STOP            0x2
#define AWSA_SOC_DAPM_STREAM_SUSPEND         0x4
#define AWSA_SOC_DAPM_STREAM_RESUME          0x8
#define AWSA_SOC_DAPM_STREAM_PAUSE_PUSH      0x10
#define AWSA_SOC_DAPM_STREAM_PAUSE_RELEASE   0x20

/** @} */

/**
 * \name dapm event types
 * @{
 */

/** \brief before widget power up */
#define AWSA_SOC_DAPM_PRE_PMU          0x1

/** \brief after widget power up */
#define AWSA_SOC_DAPM_POST_PMU         0x2

/** \brief before widget power down */
#define AWSA_SOC_DAPM_PRE_PMD          0x4

/** \brief after widget power down */
#define AWSA_SOC_DAPM_POST_PMD         0x8

/** \brief before audio path register */
#define AWSA_SOC_DAPM_PRE_REG          0x10

/** \brief after audio path register */
#define AWSA_SOC_DAPM_POST_REG         0x20

/** \brief power down */
#define AWSA_SOC_DAPM_PRE_POST_PMD \
        (AWSA_SOC_DAPM_PRE_PMD | AWSA_SOC_DAPM_POST_PMD)

/** @} */

/**
 * \name convenience event type detection
 * @{
 */

/** \brief event on */
#define AWSA_SOC_DAPM_EVENT_ON(e)    \
        (e & (AWSA_SOC_DAPM_PRE_PMU | AWSA_SOC_DAPM_POST_PMU))

/** \brief event off */
#define AWSA_SOC_DAPM_EVENT_OFF(e)   \
        (e & (AWSA_SOC_DAPM_PRE_PMD | AWSA_SOC_DAPM_POST_PMD))

/** @} */

/**
 * \brief Bias levels
 */
enum awsa_soc_bias_level {

    /** \brief Power Off. No restrictions on transition times. */
    AWSA_SOC_BIAS_OFF,

    /**
     * \brief Low power standby state when no playback/capture
     *        operations are in progress.
     * \note The transition time between STANDBY and ON should
     *       be as fast as possible and no longer than 10ms.
     */
    AWSA_SOC_BIAS_STANDBY,

    /**
     * \brief Prepare for audio operations. Called before
     *        DAPM switching for stream start and stop operations.
     */
    AWSA_SOC_BIAS_PREPARE,

    /** \brief Bias is fully on for audio playback and capture operations. */
    AWSA_SOC_BIAS_ON,
};

/**
 * \brief dpm widget types
 */
enum awsa_soc_dapm_type {

    /** \brief input pin */
    AWSA_SOC_DAPM_TYPE_INPUT = 0,

    /** \brief output pin */
    AWSA_SOC_DAPM_TYPE_OUTPUT,

    /** \brief selects 1 analog signal from many inputs */
    AWSA_SOC_DAPM_TYPE_MUX,

    /** \brief virtual version of awsa_soc_dapm_mux */
    AWSA_SOC_DAPM_TYPE_VIRT_MUX,

    /** \brief selects 1 analog signal from many inputs */
    AWSA_SOC_DAPM_TYPE_VALUE_MUX,

    /** \brief mixes several analog signals together */
    AWSA_SOC_DAPM_TYPE_MIXER,

    /** \brief mixer with named ctls */
    AWSA_SOC_DAPM_TYPE_MIXER_NAMED_CTL,

    /** \brief programmable gain/attenuation (volume) */
    AWSA_SOC_DAPM_TYPE_PGA,

    /** \brief output driver */
    AWSA_SOC_DAPM_TYPE_OUT_DRV,

    /** \brief analog to digital converter */
    AWSA_SOC_DAPM_TYPE_ADC,

    /** \brief digital to analog converter */
    AWSA_SOC_DAPM_TYPE_DAC,

    /** \brief microphone bias (power) */
    AWSA_SOC_DAPM_TYPE_MICBIAS,

    /** \brief microphone */
    AWSA_SOC_DAPM_TYPE_MIC,

    /** \brief headphones */
    AWSA_SOC_DAPM_TYPE_HP,

    /** \brief speaker */
    AWSA_SOC_DAPM_TYPE_SPK,

    /** \brief line input/output */
    AWSA_SOC_DAPM_TYPE_LINE,

    /** \brief analog switch */
    AWSA_SOC_DAPM_TYPE_SWITCH,

    /** \brief codec bias/vmid (half voltage)- to minimise pops */
    AWSA_SOC_DAPM_TYPE_VMID,

    /** \brief machine specific pre widget - exec first */
    AWSA_SOC_DAPM_TYPE_PRE,

    /** \brief machine specific post widget - exec last */
    AWSA_SOC_DAPM_TYPE_POST,

    /** \brief power/clock supply */
    AWSA_SOC_DAPM_TYPE_SUPPLY,

    /** \brief audio interface input */
    AWSA_SOC_DAPM_TYPE_AIF_IN,

    /** \brief audio interface output */
    AWSA_SOC_DAPM_TYPE_AIF_OUT,
};

/**
 * \brief DAPM audio route definition.
 * \details Defines an audio route originating at source via
 *          control and finishing at sink.
 */
struct awsa_soc_dapm_route {
    const char  *p_sink;     /**< \brief source link point */
    const char  *p_ctl;      /**< \brief control point */
    const char  *p_source;   /**< \brief source point */

    /** \brief currently only supported for links where source is a supply */
    aw_err_t   (*pfn_connect_check) (struct awsa_soc_dapm_widget *p_source,
                                     struct awsa_soc_dapm_widget *p_sink);
};

/**
 * \brief dapm audio path between two widgets
 */
struct awsa_soc_dapm_path {

    /** \brief route path name */
    char                             *p_name;

    /** \brief source (input) widgets */
    struct awsa_soc_dapm_widget      *p_source;

    /** \brief sink (output) widgets */
    struct awsa_soc_dapm_widget      *p_sink;

    /** \brief controls */
    const struct awsa_core_ctl_elem_driver *p_elem_driver;

    /** \brief source and sink widgets are connected */
    aw_bool_t                         connected;

    /** \brief path has been walked */
    aw_bool_t                         walked;

    /**
     * \brief connected callback
     *
     * \param[in] p_source : source (input) widgets
     * \param[in] p_sink   : sink (output) widgets
     *
     * \retval AW_OK : success
     */
    aw_err_t (*pfn_connected) (struct awsa_soc_dapm_widget *p_source,
                               struct awsa_soc_dapm_widget *p_sink);


    struct aw_list_head list_source; /**< \brief source widget list */
    struct aw_list_head list_sink;   /**< \brief sink widget list */
    struct aw_list_head list;        /**< \brief route path list */
};

/**
 * \brief delcare widget dirver
 */
#define AWSA_SOC_DAPM_WIDGET_DECL(type, p_name, p_sname, reg, shift,           \
    mask, on_val, off_val, invert, num_elems, p_elems, event_flags, pfn_event) \
{                                                                              \
    type, p_name, p_sname, reg, shift, mask, on_val, off_val, invert,          \
    num_elems, p_elems, event_flags, pfn_event                                 \
}

/**
 * \brief widget driver
 */
struct awsa_soc_dapm_widget_driver {

    /** \brief dapm type */
    enum awsa_soc_dapm_type                 type;

    /** \brief widget name */
    char                                   *p_name;

    /** \brief stream name */
    char                                   *p_sname;

    /** \brief negative reg = no direct dapm */
    int16_t                                 reg;

    /** \brief bits to shift */
    uint8_t                                 shift;

    /** \brief non-shifted mask */
    uint32_t                                mask;

    /** \brief on state value */
    uint32_t                                on_val;

    /** \brief off state value */
    uint32_t                                off_val;

    /** \brief invert the power bit */
    aw_bool_t                               invert;

    /** \brief controls that relate to this widget */
    int                                     num_elems;

    /** \brief control information */
    const struct awsa_core_ctl_elem_driver *p_elems_driver;

    /** \brief flags to specify event types */
    uint16_t                                event_flags;

    /**
     * \brief event callback
     *
     * \param[in] p_widget  : widget
     * \param[in] p_control : sound control device
     * \param[in] event     : event
     *
     * \retval AW_OK : success
     */
    aw_err_t (*pfn_event) (struct awsa_soc_dapm_widget *p_widget,
                           struct awsa_core_ctl        *p_ctl,
                           int                          event);
};

/**
 * \brief dapm widget
 */
struct awsa_soc_dapm_widget {

    /** \brief widget driver */
    const struct awsa_soc_dapm_widget_driver *p_driver;

    /** \brief ctl elememnt */
    struct awsa_core_ctl_elem                *p_elem;

    aw_bool_t    cnew;            /**< \brief cnew complete */
    aw_bool_t    connected;       /**< \brief connected codec pin */
    aw_bool_t    ext;             /**< \brief has external widgets */
    aw_bool_t    power;           /**< \brief block power status */
    aw_bool_t    active;          /**< \brief active stream on DAC, ADC's */
    aw_bool_t    force;           /**< \brief force state */
    aw_bool_t    ignore_suspend;  /**< \brief kept enabled over suspend */

    uint32_t     value;           /**< \brief widget current value */


    /** \brief soc codec device */
    struct awsa_soc_codec           *p_codec;

    /** \brief dapm widget list */
    struct aw_list_head              list;

    /** \brief dapm context */
    struct awsa_soc_dapm_context    *p_dapm;

    /** \brief widget inputs */
    struct aw_list_head              sources;

    /** \brief widget outputs */
    struct aw_list_head              sinks;

    /** \brief used during DAPM updates */
    struct aw_list_head              power_list;

    /**
     * \brief check power status
     * \param[in] p_widget : widget for check
     * \return check result
     */
    aw_err_t (*pfn_power_check) (struct awsa_soc_dapm_widget *p_widget);
};

/**
 * \brief update dapm
 */
struct awsa_soc_dapm_update {

    /** \brief widget */
    struct awsa_soc_dapm_widget *p_widget;

    /** \brief control */
    struct awsa_core_ctl        *p_ctl;

    /** \brief negative reg = no direct dapm */
    int                          reg;

    /** \brief non-shifted mask */
    int                          mask;

    /** \brief widget current value */
    int                          val;
};

/**
 * \brief DAPM context
 */
struct awsa_soc_dapm_context {

    /** \brief number of widgets in this context */
    int                          n_widgets;

#if 1

    /** \brief bias level */
    enum awsa_soc_bias_level     bias_level;

    /** \brief suspend bias level */
    enum awsa_soc_bias_level     suspend_bias_level;
#endif

    /** \brief delay deal with */
    struct aw_delayed_work       delayed_work;

    /** \brief Use BIAS_OFF instead of STANDBY */
    aw_bool_t                    idle_bias_off;

    /** \brief dapm update */
    struct awsa_soc_dapm_update *p_update;

    /** \brief parent codec */
    struct awsa_soc_codec       *p_codec;

    /** \brief parent card */
    struct awsa_soc_card        *p_card;


    /** \brief used during DAPM updates */
    int                          dev_power;

    /** \brief dapm context list */
    struct aw_list_head          list;
};

/**
 * \brief new a dapm control object
 *
 * \param[in] p_dapm   : dapm context
 * \param[in] p_widget : dapm widget
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_widget_add (
    struct awsa_soc_dapm_context             *p_dapm,
    struct awsa_soc_dapm_widget              *p_widget,
    const struct awsa_soc_dapm_widget_driver *p_driver,
    struct awsa_core_ctl_elem                *p_elem);

/**
 * \brief new mutilte dapm control
 *
 * \param[in] p_dapm   : dapm context
 * \param[in] p_widget : dapm widget
 * \param[in] num      : dapm control number
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_widgets_add (
    struct awsa_soc_dapm_context             *p_dapm,
    struct awsa_soc_dapm_widget              *p_widget,
    const struct awsa_soc_dapm_widget_driver *p_driver,
    int                                       num,
    struct awsa_core_ctl_elem                *p_elem);

/**
 * \brief new mutile dapm widget object
 * \param[in] p_dapm   : dapm context
 * \retval    AW_OK    : success
 */
aw_err_t awsa_soc_dapm_new (struct awsa_soc_dapm_context *p_dapm);

/**
 * \brief free dapm widgets
 * \param[in] p_dapm   : dapm context
 * \retval    AW_OK    : success
 */
void awsa_soc_dapm_free (struct awsa_soc_dapm_context *p_dapm);

/**
 * \brief update dapm context
 * \param[in] p_dapm : dapm context
 * \retval    AW_OK  : success
 */
aw_err_t awsa_soc_dapm_sync (struct awsa_soc_dapm_context *p_dapm);

/**
 * \brief Add mutile dapm route to dapm context
 *
 * \param[in] p_dapm  : dapm context
 * \param[in] p_route : route array
 * \param[in] num     : route number
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_routes_add (
    struct awsa_soc_dapm_context     *p_dapm,
    struct awsa_soc_dapm_path        *p_path,
    const struct awsa_soc_dapm_route *p_route,
    int                               num);

/**
 * \brief enable dapm widget
 *
 * \param[in] p_dapm   : dapm context
 * \param[in] p_widget : widget name
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_widget_enable (struct awsa_soc_dapm_context *p_dapm,
                                      const char                   *p_name);

/**
 * \brief disable dapm pin
 *
 * \param[in] p_dapm  : dapm context
 * \param[in] p_pin   : pin name
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_widget_disable (struct awsa_soc_dapm_context *p_dapm,
                                       const char                   *p_name);

/**
 * \brief dapm event trigger
 *
 * \param[in] p_rt     : pcm stream runtime data
 * \param[in] p_stream : stream name
 * \param[in] event    : event type
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_stream_event (struct awsa_soc_dapm_context *p_dapm,
                                     const char                   *p_stream,
                                     int                           event);

/**
 * \brief Handler for generic register modifier widget.
 *
 * \param[in] p_widget : dapm widget
 * \param[in] p_ctl    : core ctl
 * \param[in] event    : event flags
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_reg_event (struct awsa_soc_dapm_widget *p_widget,
                                  struct awsa_core_ctl        *p_ctl,
                                  int                          event);

/**
 * \brief set mux value and update power status
 *
 * \param[in] p_dapm        : dapm
 * \param[in] p_widget_name : dapm widget
 * \param[in] mux           : mux select value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_mux_power_update (
    struct awsa_soc_dapm_context *p_dapm,
    const char                   *p_widget_name,
    int                           mux);

/**
 * \brief set mixer connect value and update power status
 *
 * \param[in] p_dapm          : dapm
 * \param[in] p_ctl_elem_name : dapm widget
 * \param[in] connect         : connect select value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dapm_mixer_power_update (
    struct awsa_soc_dapm_context *p_dapm,
    const char                   *p_ctl_elem_name,
    int                          *p_value);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_SOC_DAPM_H */

/* end of file */
