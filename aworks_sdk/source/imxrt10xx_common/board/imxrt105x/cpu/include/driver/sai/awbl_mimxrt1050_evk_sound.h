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

#ifndef __AWBL_MIMXRT1050_EVK_SOUND_H
#define __AWBL_MIMXRT1050_EVK_SOUND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "soc/awsa_soc_internal.h"

#include "soc/codec/awbl_wm8960.h"

/** \brief 驱动名字 */
#define AWBL_MIMXRT1050_EVK_SOUND_NAME   "awbl_mimxrt1050_evk_sound"

/**
 * \brief MIMXRT1050-EVK声卡连接信息
 */
struct awbl_mimxrt1050_evk_sound_link {
    const char *p_name;           /**< \brief dai_link name */

    const char *p_stream_name;    /**< \brief Stream name */
    const char *p_codec_name;     /**< \brief for multi-codec */
    const char *p_plfm_name;      /**< \brief for multi-platform */
    const char *p_cpu_dai_name;   /**< \brief platform dai name */
    const char *p_codec_dai_name; /**< \brief codec dai name */
};

/**
 * \brief MIMXRT1050声卡设备信息
 */
struct awbl_mimxrt1050_evk_sound_devinfo {

    struct awbl_mimxrt1050_evk_sound_link link[2];

    const char  *p_name;   /**< \brief 声卡名称 */
    int          card_id;  /**< \brief 声卡编号 */
    int          hp_jd;    /**< \brief 耳机插入检测引脚  */

    void (*pfn_plfm_init) (void);
};

/**
 * \brief MIMXRT1050-EVK声卡设备
 */
struct awbl_mimxrt1050_evk_sound_dev {
    struct awbl_dev             dev;
    struct awsa_soc_card        soc_card;
    struct awsa_soc_pcm         soc_pcm[2];
    struct awsa_soc_jack        hp_jack;
    struct awsa_soc_jack_gpio   hp_jack_gpio;

    struct awsa_soc_dapm_widget widget_pkt[3];
    struct awsa_soc_dapm_path   map_pkt[6];

    struct awsa_soc_dapm_widget widget_cpt[2];
    struct awsa_soc_dapm_path   map_cpt[5];

    struct awsa_soc_dai_link    link[2];
    struct awsa_soc_card_driver card_driver;
};

/**
 * \brief 驱动注册
 */
void awbl_mimxrt1050_evk_sound_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_MIMXRT1050_EVK_SOUND_H */

/* end of file */
