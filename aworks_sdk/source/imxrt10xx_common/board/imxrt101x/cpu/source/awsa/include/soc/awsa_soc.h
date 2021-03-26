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

#ifndef __AWSA_SOC_H
#define __AWSA_SOC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "soc/awsa_soc_internal.h"

struct awsa_soc_dai_link;
struct awsa_soc_pcm;

struct awsa_soc_card;
struct awsa_soc_platform;
struct awsa_soc_dai;
struct awsa_soc_codec;
struct awsa_soc_codec_driver;
struct awsa_soc_jack;
struct awsa_soc_jack_pin;
struct awsa_soc_dapm_context;
struct awsa_soc_dai_driver;

enum awsa_soc_bias_level;

/**
 * \brief soc service
 */
struct awsa_soc_servopts {

    /** \brief open a pcm device */
    aw_err_t (*pfn_open     ) (struct awsa_soc_pcm *p_soc_pcm);

    /** \brief close a pcm device */
    aw_err_t (*pfn_close    ) (struct awsa_soc_pcm *p_soc_pcm);

    /**
     * \brief start up a pcm stream
     */
    aw_err_t (*pfn_startup  ) (struct awsa_soc_pcm *p_soc_pcm);

    /**
     * \brief shutdown a pcm stream
     */
    aw_err_t (*pfn_shutdown ) (struct awsa_soc_pcm *p_soc_pcm,
                               awsa_pcm_stream_t    stream);

    /**
     * \brief hardware parameters set
     */
    aw_err_t (*pfn_config   ) (struct awsa_soc_pcm *p_soc_pcm,
                               awsa_pcm_stream_t    stream,
                               awsa_pcm_config_t   *p_config);

    /**
     * \brief stream prepare before startup
     */
    aw_err_t (*pfn_prepare  ) (struct awsa_soc_pcm *p_soc_pcm);

    /**
     * \brief trigger
     */
    aw_err_t (*pfn_trigger  ) (struct awsa_soc_pcm *p_soc_pcm,
                               int                  cmd);

    /** \brief get pcm trigger point */
    awsa_pcm_uframes_t
             (*pfn_pointer  ) (struct awsa_soc_pcm *p_soc_pcm);

    /** \brief pcm device stop transfer data */
    aw_err_t (*pfn_silence  ) (struct awsa_soc_pcm *p_soc_pcm,
                               int                  channel,
                               awsa_pcm_uframes_t   pos,
                               awsa_pcm_uframes_t   count);

    /** \brief ackledgement */
    aw_err_t (*pfn_ack      ) (struct awsa_soc_pcm *p_soc_pcm);
};

/**
 * \brief link two dai config information.
 */
struct awsa_soc_dai_link {

    const char *p_name;           /**< \brief dai_link name */

    const char *p_stream_name;    /**< \brief Stream name */
    const char *p_codec_name;     /**< \brief for multi-codec */
    const char *p_plfm_name;      /**< \brief for multi-platform */
    const char *p_cpu_dai_name;   /**< \brief platform dai name */
    const char *p_codec_dai_name; /**< \brief codec dai name */

    /** \brief codec/machine specific init - e.g. add machine controls */
    aw_err_t  (*pfn_init) (struct awsa_soc_pcm *p_soc_pcm);

    /** \brief machine stream operations */
    const struct awsa_soc_servopts *p_opts;
};

/**
 * \brief auxiliary device
 */
struct awsa_soc_aux_dev {
    const char *p_codec_name;  /**< \brief Codec name */
    const char *p_multi_codec; /**< \brief for multi-codec */

    /** \brief codec/machine specific init - e.g. add machine controls */
    aw_err_t  (*pfn_init) (struct awsa_soc_dapm_context *p_dapm);
};

/**
 * \brief soc pcm
 */
struct awsa_soc_pcm {

    /** \brief pcm device */
    struct awsa_core_pcm            core_pcm;

    /** \brief substream */
    struct awsa_core_pcm_substream  core_substream[2];

    /** \brief current */
    struct awsa_core_pcm_substream *p_substream;

    /** \brief sound object */
    struct awsa_soc_card           *p_card;

    /** \brief dai link */
    const struct awsa_soc_dai_link *p_dai_link;

    /** \brief codec */
    struct awsa_soc_codec          *p_codec;

    /** \brief platform */
    struct awsa_soc_platform       *p_plfm;

    /** \brief codec dai */
    struct awsa_soc_dai            *p_codec_dai;

    /** \brief cpu dai */
    struct awsa_soc_dai            *p_cpu_dai;

    /** \brief reduce pop noise */
    struct aw_delayed_work          delayed_work;

    /** \brief finish */
    aw_bool_t                       complete;

    /** \brief register finish */
    aw_bool_t                       dev_registered;

    /** \brief Symmetry */
    uint32_t                        rate;

    /** \brief power down time */
    int32_t                         pmdown_time;
};

/**
 * \brief pcm stream information
 */
struct awsa_soc_pcm_stream {

    /** \brief stream name */
    const char   *p_stream_name;

    /** \brief AWSA_CORE_PCM_FMTBIT(AWSA_PCM_FORMAT_*) */
    uint64_t      formats;

    uint32_t      rates;         /**< \brief AWSA_PCM_RATE_* */
    uint32_t      rate_min;      /**< \brief min rate */
    uint32_t      rate_max;      /**< \brief max rate */
    uint32_t      channels_min;  /**< \brief min channels */
    uint32_t      channels_max;  /**< \brief max channels */
};

/**
 * \brief soundcard driver
 */
struct awsa_soc_card_driver {

    const struct awsa_soc_dai_link *p_dai_link;     /**< \brief dai link */
    uint32_t                        num_links;      /**< \brief link nums */

    /** \brief (optional)  */
    aw_err_t (*pfn_probe              ) (struct awsa_soc_card *p_card);

    /** \brief (optional) */
    aw_err_t (*pfn_remove             ) (struct awsa_soc_card *p_card);

    /** \brief pre-set bias level */
    aw_err_t (*pfn_bias_level_pre_set ) (struct awsa_soc_card     *p_card,
                                         enum awsa_soc_bias_level  level);

    /** \brief post-set bias level */
    aw_err_t (*pfn_bias_level_post_set) (struct awsa_soc_card     *p_card,
                                         enum awsa_soc_bias_level  level);
};

/**
 * \brief sound card for embedded
 */
struct awsa_soc_card {

    /** \brief sound card (core) */
    struct awsa_core_card        core_card;

    /** \brief soundcard id */
    int                          id;

    /** \brief ctl device */
    struct awsa_core_ctl         ctl;

    /** \brief sound device use */
    struct aw_list_head          list;

    /** \brief name - awbus_lite */
    const char                  *p_name;

    /** \brief lock */
    AW_MUTEX_DECL(mutex);

    /** \brief init finish */
    aw_bool_t                    instantiated;

    /* lists of probed devices belonging to this card */
#if 0
    struct aw_list_head          codec_dev_list;
    struct aw_list_head          plfm_dev_list;
    struct aw_list_head          dai_dev_list;
#endif

    struct aw_list_head          widgets;
    struct aw_list_head          paths;
    struct aw_list_head          dapm_list;

    /** \brief reduce noise between power on or off */
    uint32_t                     pop_time;

    /** \brief pcm */
    struct awsa_soc_pcm         *p_soc_pcm;

    /** \brief pcm count */
    int                          num_pcm;

    /**
     * \brief optional auxiliary devices such as amplifiers or codecs with dai
     *        link unused
     */
    struct awsa_soc_aux_dev     *p_aux_dev;
    int                          num_aux_dev;

    /** \brief driver */
    const
    struct awsa_soc_card_driver *p_driver;

    /** \brief private data */
    void                        *p_private_data;
};

/**
 * \brief codec io driver functions
 */
struct awsa_soc_codec_driver {

#if 0
    /** \brief cache size */
    uint16_t    reg_cnt;

    /** \brief cahce step */
    int16_t     reg_step;

    /** \brief unit byte */
    uint16_t    reg_word;

    /** \brief cahce buffer */
    const void *p_default_reg_buf;
#endif

    /** \brief probe */
    aw_err_t  (*pfn_probe          ) (struct awsa_soc_codec *p_codec);

    /** \brief remove */
    aw_err_t  (*pfn_remove         ) (struct awsa_soc_codec *p_codec);

    /** \brief codec io read */
    aw_err_t  (*pfn_read           ) (struct awsa_soc_codec *p_codec,
                                      uint32_t               reg,
                                      uint32_t              *p_value);

    /** \brief codec io write */
    aw_err_t  (*pfn_write          ) (struct awsa_soc_codec *p_codec,
                                      uint32_t               reg,
                                      uint32_t               value);

#if 0
    /** \brief Report if a register is volatile */
    bool_t    (*pfn_reg_is_volatile) (struct awsa_soc_codec *p_codec,
                                      uint32_t               reg);

    /** \brief Report if a register is readable */
    bool_t    (*pfn_reg_is_readable) (struct awsa_soc_codec *p_codec,
                                      uint32_t               reg);
#endif

    /** \brief bias level set */
    aw_err_t  (*pfn_bias_level_set)  (struct awsa_soc_codec    *p_codec,
                                      enum awsa_soc_bias_level  level);
};

/**
 * \brief codec device
 */
struct awsa_soc_codec {

    /** \brief soc sound card object */
    struct awsa_soc_card               *p_card;

    /** \brief codec name */
    const char                         *p_name;

    /** \brief codec driver */
    const struct awsa_soc_codec_driver *p_driver;

    /** \brief dapm context */
    struct awsa_soc_dapm_context        dapm;

    /** \brief codec list */
    struct aw_list_head                 list;

    /** \brief card codec list */
    //struct aw_list_head                 card_list;

    /** \brief codec dai */
    struct awsa_soc_dai                *p_dai;

    /** \brief dai number */
    int                                 num_dai;

#if 0
    /** \brief codec id */
    int                                 id;
#endif

    /** \brief codec is in suspend PM state */
    aw_bool_t                           suspended;

    /** \brief codec has been probed */
    aw_bool_t                           probed;

    /** \brief active flg */
    uint32_t                            active;

#if 0
    /** \brief codec control (i2c/3wire) data */
    void                               *p_ctl_data;
#endif

    /** \brief soc pcm */
    struct awsa_soc_pcm                *p_soc_pcm;

    /** \brief private */
    void                               *p_private_data;

    /** \brief dev lock */
    AW_MUTEX_DECL(mutex);

    /** \brief reg read/write lock */
    AW_MUTEX_DECL(rw_mutex);
};

/**
 * \brief plfm driver
 */
struct awsa_soc_platform_driver {

    /** \brief plfm probe */
    aw_err_t (*pfn_probe    ) (struct awsa_soc_platform *p_plfm);

    /** \brief plfm remove */
    aw_err_t (*pfn_remove   ) (struct awsa_soc_platform *p_plfm);

    /** \brief open platform */
    aw_err_t (*pfn_open     ) (struct awsa_soc_platform *p_plfm);

    /** \brief close platform */
    aw_err_t (*pfn_close    ) (struct awsa_soc_platform *p_plfm);

    /** \brief pcm device io control */
    aw_err_t (*pfn_ioctl    ) (struct awsa_soc_platform *p_plfm,
                               int                       cmd,
                               void                     *p_arg);

    /** \brief pcm config */
    aw_err_t (*pfn_config   ) (struct awsa_soc_platform *p_plfm,
                               awsa_pcm_stream_t         stream,
                               awsa_pcm_config_t        *p_config);

    /**
     * \brief dma memory alloc
     * \attention dma memory is a special kind of memory, dynamic allocation
     *            is more suitable than static definition
     */
    aw_err_t (*pfn_dma_alloc) (struct awsa_soc_platform  *p_plfm,
                               void                     **p_dma_mem);

    /** \brief dma memory free */
    aw_err_t (*pfn_dma_free ) (struct awsa_soc_platform  *p_plfm,
                               void                      *p_dma_mem);

    /** \brief pcm device prepare function */
    aw_err_t (*pfn_prepare  ) (struct awsa_soc_platform  *p_plfm);

    /** \brief pcm device copy */
    aw_err_t (*pfn_copy) (struct awsa_soc_platform  *p_plfm,
                          int                        channel,
                          awsa_pcm_uframes_t         pos,
                          void                      *p_buf,
                          awsa_pcm_uframes_t         count);

    /** \brief pcm device trigger operation */
    aw_err_t (*pfn_trigger  ) (struct awsa_soc_platform  *p_plfm,
                               int                        cmd);

    /** \brief get pcm trigger point */
    awsa_pcm_uframes_t
             (*pfn_pointer  ) (struct awsa_soc_platform  *p_plfm);

    /** \brief pcm device stop transfer data */
    aw_err_t (*pfn_silence  ) (struct awsa_soc_platform  *p_plfm,
                               int                        channel,
                               awsa_pcm_uframes_t         pos,
                               awsa_pcm_uframes_t         count);

    /**
     * \brief (optional) For platform caused delay reporting.
     */
    awsa_pcm_sframes_t
             (*pfn_delay    ) (struct awsa_soc_platform *p_plfm,
                               struct awsa_soc_dai      *p_dai);
};

/**
 * \brief platform
 */
struct awsa_soc_platform {

    /** \brief plfm driver */
    struct awsa_soc_platform_driver *p_driver;

    /** \brief soc pcm */
    struct awsa_soc_pcm             *p_soc_pcm;

    /** \brief soc dai */
    struct awsa_soc_dai             *p_dai;

    /** \brief dai nums */
    int                              num_dai;

    /** \brief platform name */
    const char                      *p_name;

    /** \brief plfm list */
    struct aw_list_head              list;

#if 0
    /** \brief sound card list */
    struct aw_list_head              card_list;
#endif

    /** \brief platform is suspended */
    aw_bool_t                        suspended;

    /** \brief platform is probed */
    aw_bool_t                        probed;

    /** \brief plfm id */
    int                              id;

    /** \brief private data */
    void                            *p_private_data;
};

/**
 * \brief soc sound register
 * \param[in] p_card : soc sound object
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_card_register (
    struct awsa_soc_card              *p_card,
    int                                id,
    const char                        *p_name,
    struct awsa_soc_pcm               *p_soc_pcm,
    int                                num_pcms,
    struct awsa_soc_aux_dev           *p_aux_devs,
    int                                num_aux_devs,
    const struct awsa_soc_card_driver *p_card_drv,
    void                              *p_private_data);

/**
 * \brief soc sound unregister
 * \param[in] p_card : soc sound object
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_card_unregister (struct awsa_soc_card *p_card);

/**
 * \brief platform register
 *
 * \param[in] p_dev      : AWBus device
 * \param[in] p_plfm_drv : platform driver
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_platform_register (
    struct awsa_soc_platform         *p_plfm,
    const char                       *p_name,
    struct awsa_soc_platform_driver  *p_plfm_drv,
    struct awsa_soc_dai              *p_dais,
    const char                       *dais_name[],
    const struct awsa_soc_dai_driver *p_dais_drv,
    int                               num_dais,
    void                             *p_priave_data);

/**
 * \brief platform unregister
 * \param[in] p_dev : AWBus device
 * \return no
 */
void awsa_soc_platform_unregister (struct awsa_soc_platform *p_plfm);

/**
 * \brief codec register
 *
 * \param[in] p_dev       : AWBus device
 * \param[in] p_codec_drv : codec driver
 * \param[in] p_dai_drv   : dai driver
 * \param[in] num_dai     : dai number
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_codec_register (
    struct awsa_soc_codec              *p_codec,
    const char                         *p_name,
    const struct awsa_soc_codec_driver *p_codec_drv,
    struct awsa_soc_dai                *p_dais,
    const char                         *dais_name[],
    const struct awsa_soc_dai_driver   *p_dais_drv,
    int                                 num_dais,
    void                               *p_private_data);

/**
 * \brief codec unregister
 * \param[in] p_dev : AWBus device
 * \return no
 */
void awsa_soc_codec_unregister (struct awsa_soc_codec *p_codec);

/**
 * \brief add ctl device
 *
 * \param[in] p_codec  : codec device
 * \param[in] p_ctls   : ctl device array
 * \param[in] num_ctls : ctl device number
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_codec_ctl_elem_add (
    struct awsa_soc_codec                  *p_codec,
    struct awsa_core_ctl_elem              *p_elem,
    const struct awsa_core_ctl_elem_driver *p_driver,
    int                                     num_elem);


/**
 * \brief add widgets
 */
aw_err_t awsa_soc_codec_widgets_add (
    struct awsa_soc_codec                    *p_codec,
    struct awsa_soc_dapm_widget              *p_widget,
    const struct awsa_soc_dapm_widget_driver *p_driver,
    int                                       num,
    struct awsa_core_ctl_elem                *p_elem);

/**
 * \brief codec io read
 *
 * \param[in]  p_codec : codec device
 * \param[in]  reg     : reg address
 * \param[out] p_value : reg value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_codec_read (struct awsa_soc_codec *p_codec,
                              uint32_t               reg,
                              uint32_t              *p_value);

/**
 * \brief codec io write
 *
 * \param[in]  p_codec : codec device
 * \param[in]  reg     : reg address
 * \param[out] p_value : reg value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_codec_write (struct awsa_soc_codec *p_codec,
                               uint32_t               reg,
                               uint32_t               val);

/**
 * \brief codec register bit update
 *
 * \param[in] p_codec : codec device
 * \param[in] reg     : reg address
 * \param[in] mask    : reg mask
 * \param[in] value   : reg value
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_codec_bits_update (struct awsa_soc_codec *p_codec,
                                     uint16_t               reg,
                                     uint32_t               mask,
                                     uint32_t               value);

/**
 * \brief stream event
 *
 * \param[in] p_codec  : codec device
 * \param[in] p_stream : audio stream name
 * \param[in] event    : event type
 *
 * \retval AW_OK : success
 */
aw_err_t awsa_soc_codec_stream_event (struct awsa_soc_codec *p_codec,
                                      const char            *p_stream,
                                      int                    event);


/**
 * \brief update the pcm status for the next period
 * \note This function is called from the interrupt handler when the
 *       pcm has processed the period size.  It will update the current
 *       pointer, wake up mcu, etc.
 *
 *       Even if more than one periods have elapsed since the last call, you
 *       have to call this only once.
 *
 * \param[in] p_substream : the pcm substream instance
 * \return no
 */
void awsa_soc_pcm_period_elapsed (struct awsa_soc_pcm *p_soc_pcm);

/**
 * \brief awsa soc initialization
 */
void awsa_soc_init (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_SOC_H */

/* end of file */
