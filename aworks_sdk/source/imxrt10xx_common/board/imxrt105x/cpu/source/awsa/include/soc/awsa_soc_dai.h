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
 * \brief DAI(Digital Audio Interface)
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

#ifndef __AWSA_SOC_DAI_H
#define __AWSA_SOC_DAI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "soc/awsa_soc_internal.h"

struct awsa_soc_dai;

/**
 * \name dai hardware audio formats.
 * \note Describes the physical pcm data formating and clocking.
 *       Add new formats to the end.
 * @{
 */

#define AWSA_SOC_DAI_FMT_I2S      0 /**< \brief I2S mode */
#define AWSA_SOC_DAI_FMT_RIGHT_J  1 /**< \brief Right Justified mode */
#define AWSA_SOC_DAI_FMT_LEFT_J   2 /**< \brief Left Justified mode */
#define AWSA_SOC_DAI_FMT_DSP_A    3 /**< \brief L data MSB after FRM LRC */
#define AWSA_SOC_DAI_FMT_DSP_B    4 /**< \brief L data MSB during FRM LRC */
#define AWSA_SOC_DAI_FMT_AC97     5 /**< \brief AC97 */
#define AWSA_SOC_DAI_FMT_PDM      6 /**< \brief Pulse density modulation */

/** \brief left justified also known as MSB and LSB respectively */
#define AWSA_SOC_DAI_FMT_MSB      AWSA_SOC_DAI_FMT_LEFT_J

/** \brief right justified also known as MSB and LSB respectively */
#define AWSA_SOC_DAI_FMT_LSB      AWSA_SOC_DAI_FMT_RIGHT_J

/** @} */

/**
 * \name dai Clock gating.
 * \note dai bit clocks can be be gated (disabled) when the DAI is not
 *       sending or receiving PCM data in a frame. This can be used to
 *       save power.
 * @{
 */
#define AWSA_SOC_DAI_FMT_CONT     (0 << 4) /**< \brief continuous clock */
#define AWSA_SOC_DAI_FMT_GATED    (1 << 4) /**< \brief clock is gated */

/** @} */

/**
 * \name DAI hardware signal inversions.
 * \note Specifies whether the DAI can also support inverted clocks
 *       for the specified format.
 * @{
 */

/** \brief normal BCLK + Frame */
#define AWSA_SOC_DAI_FMT_NB_NF    (0 << 8)

/** \brief normal BCLK + invert Frame */
#define AWSA_SOC_DAI_FMT_NB_IF    (1 << 8)

/** \brief invert BCLK + normal Frame */
#define AWSA_SOC_DAI_FMT_IB_NF    (2 << 8)

/** \brief invert BCLK + Frame */
#define AWSA_SOC_DAI_FMT_IB_IF    (3 << 8)

/** @} */

/**
 * \name DAI hardware clock masters.
 * \note This is wrt the codec, the inverse is true for the interface
 *       i.e. if the codec is BCLK and Frame master then the interface is
 *       clk and frame slave.
 * @{
 */

/** \brief codec BCLK & Frame master */
#define AWSA_SOC_DAI_FMT_CBM_CFM      (0 << 12)

/** \brief codec BCLK slave & Frame master */
#define AWSA_SOC_DAI_FMT_CBS_CFM      (1 << 12)

/** \brief codec BCLK master & Frame slave */
#define AWSA_SOC_DAI_FMT_CBM_CFS      (2 << 12)

/** \brief codec BCLK & Frame slave */
#define AWSA_SOC_DAI_FMT_CBS_CFS      (3 << 12)

/** \brief format mask */
#define AWSA_SOC_DAI_FMT_MASK          0x000f

/** \brief clock mask */
#define AWSA_SOC_DAI_FMT_CLOCK_MASK    0x00f0

/** \brief inv mask */
#define AWSA_SOC_DAI_FMT_INV_MASK      0x0f00

/** \brief master mask */
#define AWSA_SOC_DAI_FMT_MASTER_MASK   0xf000

/** @} */

/**
 * \name Master Clock Directions
 * @{
 */

#define AWSA_SOC_CLOCK_IN        0  /**< \brief input */
#define AWSA_SOC_CLOCK_OUT       1  /**< \brief output */

/** @} */

/** \brief advance statement */
struct awsa_soc_dai;

/**
 * \brief DAI Service operate interface
 */
struct awsa_soc_dai_servopts {
    /**
     * \brief DAI clocking configuration, all optional.
     *        Called by soc_card drivers, normally in their hw_params.
     *
     * \param[in] p_dai  : DAI
     * \param[in] clk_id : clock id
     * \param[in] freq   : clock frequency
     * \param[in] dir    : direction, #AWSA_PCM_STREAM_*
     *
     * \retval AW_OK : success
     */
    aw_err_t (*pfn_sysclk_set) (struct awsa_soc_dai *p_dai,
                                uint32_t             clk_id,
                                uint32_t             freq,
                                int                  dir);

    /**
     * \brief Set pll clock input and output frequency
     *
     * \param[in] p_dai    : DAI
     * \param[in] pll_id   : pll clock id
     * \param[in] source   : clock source, XTAL or other osc
     * \param[in] freq_in  : input frequency
     * \param[in] freq_out : output frequency
     *
     * \retval AW_OK : success
     */
    aw_err_t (*pfn_pll_set) (struct awsa_soc_dai *p_dai,
                             int                  pll_id,
                             int                  source,
                             uint32_t             freq_in,
                             uint32_t             freq_out);

    /**
     * \brief Set mclk clock divide
     *
     * \param[in] p_dai    : DAI runtime data
     * \param[in] div_id   : mclk id, user define
     * \param[in] div      : divide value
     *
     * \retval AW_OK : success
     */
    aw_err_t (*pfn_clkdiv_set) (struct awsa_soc_dai *p_dai,
                                int                  clk_id,
                                int                  div);

    /**
     * \brief DAI format configuration
     * \details Called by soc_card drivers, normally in their hw_params.
     *
     * \param[in] p_dai : DAI runtime data
     * \param[in] fmt   : format, #AWSA_SOC_DAIFMT*
     *
     * \retval AW_OK : success
     */
    aw_err_t (*pfn_fmt_set) (struct awsa_soc_dai *p_dai,
                             uint32_t             fmt);

    /**
     * \brief DAI digital mute - optional.
     * \details Called by soc-core to minimise any pops.
     *
     * \param[in] p_dai : DAI runtime data
     * \param[in] mute  : 1-mute, 0-unmute
     *
     * \retval AW_OK : success
     */
    aw_err_t (*pfn_digital_mute) (struct awsa_soc_dai *p_dai,
                                  int                  mute);

    /**
     * \brief Startup a PCM substream
     *
     * \param[in] p_substream : pcm substream
     * \param[in] p_dai       : DAI runtime data
     *
     * \retval AW_OK : success
     */
    aw_err_t (*pfn_startup) (struct awsa_soc_dai *p_dai);

    /**
     * \brief Shutdown a PCM substream
     *
     * \param[in] p_substream : pcm substream
     * \param[in] p_dai       : DAI runtime data
     *
     * \retval AW_OK : success
     */
    aw_err_t (*pfn_shutdown) (struct awsa_soc_dai *p_dai,
                              awsa_pcm_stream_t    stream);

    /**
     * \brief Set a PCM substream hardware params
     *
     * \param[in] p_substream : pcm substream
     * \param[in] p_dai       : DAI runtime data
     *
     * \retval AW_OK : success
     */
    aw_err_t (*pfn_config) (struct awsa_soc_dai *p_dai,
                            awsa_pcm_stream_t    stream,
                            awsa_pcm_config_t   *p_config);

    /**
     * \brief PCM substream operate prepare.
     *
     * \param[in] p_substream : pcm substream
     * \param[in] p_dai       : DAI runtime data
     *
     * \retval AW_OK : success
     */
    aw_err_t (*pfn_prepare) (struct awsa_soc_dai *p_dai);

    /**
     * \brief Trigger a PCM substream
     *
     * \param[in] p_substream : pcm substream
     * \param[in] state       : trigger state
     * \param[in] p_dai       : DAI runtime data
     *
     * \retval AW_OK : success
     */
    aw_err_t (*pfn_trigger) (struct awsa_soc_dai *p_dai,
                             int                  state);
    /**
     * \brief For hardware based FIFO caused delay reporting.
     *
     * \param[in] p_substream : pcm substream
     * \param[in] p_dai       : DAI runtime data
     *
     * \retval AW_OK : success
     */
    awsa_pcm_sframes_t (*pfn_delay) (struct awsa_soc_dai *p_dai);

#if 0

    /**
     * \brief configure DAI system or master clock.
     */
    aw_err_t (*pfn_tri_state_set) (struct awsa_soc_dai *p_dai,
                                   int                  state);
#endif
};

/**
 * \brief DAI Driver.
 * \details Describes the dai operations and capabilities.
 *          Codec and platform drivers will register this structure for every
 *          DAI they have. This structure covers the clocking, formating and
 *          operations for each interface.
 */
struct awsa_soc_dai_driver {

    /** \brief index */
    //uint32_t                            id;

    /** \brief dai operations */
    const struct awsa_soc_dai_servopts *p_opts;

    /** \brief capture stream information  */
    struct awsa_soc_pcm_stream          capture;

    /** \brief playback stream information  */
    struct awsa_soc_pcm_stream          playback;

    /** \brief rate symmetrical */
    aw_bool_t                           symmetric_rates;

    /** \brief DAI driver probe */
    aw_err_t (*pfn_probe) (struct awsa_soc_dai *p_dai);

    /** \brief DAI dirver remove */
    aw_err_t (*pfn_remove) (struct awsa_soc_dai *p_dai);
};


/**
 * \brief Digital Audio Interface runtime data.
 * \detail Holds runtime data for a DAI.
 */
struct awsa_soc_dai {

    /** \brief dai driver name */
    const char                         *p_name;

    /** \brief driver ops */
    const struct awsa_soc_dai_driver   *p_driver;

    /** \brief capture stream is in use */
    aw_bool_t                           capture_active;

    /** \brief playback stream is in use */
    aw_bool_t                           playback_active;

    /** \brief active stream number */
    uint32_t                            active;

    /** \brief pop noise delay time */
    aw_bool_t                           pop_wait;

    /** \brief success probe flag */
    aw_bool_t                           probed;

    /** \brief platform/codec */
    union __soc_dai_object {

        /** \brief parent platform */
        struct awsa_soc_platform       *p_plfm;

        /** \brief parent codec */
        struct awsa_soc_codec          *p_codec;
    } plfm_codec;

    /** \brief soc card device */
    struct awsa_soc_card               *p_card;

    /** \brief DAI list */
    struct aw_list_head                 list;

#if 0
    /** \brief card list */
    struct aw_list_head                 card_list;
#endif
};

/**
 * \brief Digital Audio Interface register
 *
 * \param[in] p_dev     : AWBus device
 * \param[in] p_dai_drv : DAI driver
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dai_register (struct awsa_soc_dai              *p_dai,
                                const char                       *p_name,
                                const struct awsa_soc_dai_driver *p_dai_drv);

/**
 * \brief Digital Audio Interface unregister
 *
 * \param[in] p_dev     : AWBus device
 * \param[in] p_dai_drv : DAI driver
 *
 * \retval AW_OK : success
 */
void awsa_soc_dai_unregister (struct awsa_soc_dai *p_dai);

/**
 * \brief Digital Audio Interfaces(mutile) register
 *
 * \param[in] p_dev     : AWBus device
 * \param[in] p_dai_drv : DAI driver array
 * \param[in] count     : DAI driver count
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dais_register (struct awsa_soc_dai              *p_dais,
                                 const char                       *name[],
                                 const struct awsa_soc_dai_driver *p_dais_drv,
                                 size_t                            count);

/**
 * \brief Digital Audio Interfaces(mutile) unregister
 *
 * \param[in] p_dev     : AWBus device
 * \param[in] p_dai_drv : DAI driver array
 * \param[in] count     : DAI driver count
 *
 * \retval AW_OK : success
 */
void awsa_soc_dais_unregister (struct awsa_soc_dai *p_dais, size_t count);

/**
 * \brief dai clock divide set
 *
 * \param[in] p_dai  : dai
 * \param[in] clk_id : clock id
 * \param[in] div    : divide value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dai_clkdiv_set (struct awsa_soc_dai *p_dai,
                                  int                  clk_id,
                                  int                  div);

/**
 * \brief set dai hardware audio format.
 *
 * \param[in] p_dai  : dai
 * \param[in] fmt    : AWSA_SOC_DAIFMT_* format value.
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dai_fmt_set (struct awsa_soc_dai *p_dai,
                               uint32_t             fmt);

/**
 * \brief set dai system clock frequency
 *
 * \param[in] p_dai  : dai
 * \param[in] clk_id : clock id
 * \param[in] freq   : clock frequency
 * \param[in] dir    : direction, #AWSA_PCM_STREAM_*
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dai_sysclk_set (struct awsa_soc_dai *p_dai,
                                  int                  clk_id,
                                  uint32_t             freq,
                                  int                  dir);

/**
 * \brief Set pll clock input and output frequency
 *
 * \param[in] p_dai    : DAI
 * \param[in] pll_id   : pll clock id
 * \param[in] source   : clock source, XTAL or other osc
 * \param[in] freq_in  : input frequency
 * \param[in] freq_out : output frequency
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_dai_pll_set (struct awsa_soc_dai *p_dai,
                               int                  pll_id,
                               int                  source,
                               uint32_t             freq_in,
                               uint32_t             freq_out);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_SOC_DAI_H */

/* end of file */
