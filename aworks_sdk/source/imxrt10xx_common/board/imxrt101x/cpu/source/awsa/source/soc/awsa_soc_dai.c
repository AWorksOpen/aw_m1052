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

AW_MUTEX_IMPORT(__g_soc_client_mutex);  /**< \brief client lock */

struct aw_list_head __g_soc_dai_list;   /**< \brief dai list */

/**
 * \brief Attempt to initialise any uninitialised cards.
 * \attention Must be called with __g_soc_client_mutex.
 */
extern void __soc_instantiate_cards (void);

/**
 * \brief dai register
 */
aw_err_t awsa_soc_dai_register (struct awsa_soc_dai              *p_dai,
                                const char                       *p_name,
                                const struct awsa_soc_dai_driver *p_dai_drv)
{
    AWSA_SOC_DEBUG_INFO("dai register %s", p_name);

    p_dai->p_name   = p_name;
    p_dai->p_driver = p_dai_drv;
    if (p_dai->p_driver->p_opts == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(__g_soc_client_mutex, AW_SEM_WAIT_FOREVER);
    aw_list_add(&p_dai->list, &__g_soc_dai_list);
    __soc_instantiate_cards();
    AW_MUTEX_UNLOCK(__g_soc_client_mutex);

    AWSA_SOC_DEBUG_INFO("registered DAI '%s'", p_name);
    return AW_OK;
}

/**
 * \brief dai unregister
 */
void awsa_soc_dai_unregister (struct awsa_soc_dai *p_dai)
{
    struct awsa_soc_dai *p_tmp;

    aw_list_for_each_entry(p_tmp,
                           &__g_soc_dai_list,
                           struct awsa_soc_dai,
                           list) {
        if (p_tmp == p_dai) {
            goto __found;
        }
    }
    return;

__found:
    AW_MUTEX_LOCK(__g_soc_client_mutex, AW_SEM_WAIT_FOREVER);
    aw_list_del(&p_dai->list);
    AW_MUTEX_UNLOCK(__g_soc_client_mutex);

    AWSA_SOC_DEBUG_INFO("unregistered DAI '%s'", p_dai->p_name);
}

/**
 * \brief Digital Audio Interfaces(mutile) register
 */
aw_err_t awsa_soc_dais_register (struct awsa_soc_dai              *p_dais,
                                 const char                       *name[],
                                 const struct awsa_soc_dai_driver *p_dais_drv,
                                 size_t                            count)
{
    int      i;
    aw_err_t ret = AW_OK;

    for (i = 0; i < count; i++) {

        AWSA_SOC_DEBUG_INFO("register DAI '%s'", name[i]);

        p_dais[i].p_name   = name[i];
        p_dais[i].p_driver = &p_dais_drv[i];

        if (p_dais[i].p_driver->p_opts == NULL) {
            goto __err;
        }

        AW_MUTEX_LOCK(__g_soc_client_mutex, AW_SEM_WAIT_FOREVER);
        aw_list_add(&p_dais[i].list, &__g_soc_dai_list);
        AW_MUTEX_UNLOCK(__g_soc_client_mutex);

        AWSA_SOC_DEBUG_INFO(
            "registered DAI '%s'", p_dais[i].p_name);
    }

    AW_MUTEX_LOCK(__g_soc_client_mutex, AW_SEM_WAIT_FOREVER);
    __soc_instantiate_cards();
    AW_MUTEX_UNLOCK(__g_soc_client_mutex);
    return AW_OK;

__err:
    awsa_soc_dais_unregister(p_dais, count);
    return ret;
}

/**
 * \brief Digital Audio Interfaces(mutile) unregister
 */
void awsa_soc_dais_unregister (struct awsa_soc_dai *p_dais, size_t count)
{
    int i;

    for (i = 0; i < count; i++) {
        awsa_soc_dai_unregister(p_dais);
    }
}

/**
 * \brief dai clock divide set
 */
aw_err_t awsa_soc_dai_clkdiv_set (struct awsa_soc_dai *p_dai,
                                  int                  clk_id,
                                  int                  div)
{
    if (p_dai                                   != NULL &&
        p_dai->p_driver                         != NULL &&
        p_dai->p_driver->p_opts->pfn_clkdiv_set != NULL) {
        return p_dai->p_driver->p_opts->pfn_clkdiv_set(p_dai, clk_id, div);
    } else {
        return -AW_EINVAL;
    }
}

/**
 * \brief set dai hardware audio format.
 */
aw_err_t awsa_soc_dai_fmt_set (struct awsa_soc_dai *p_dai,
                               uint32_t             fmt)
{
    if (p_dai                                != NULL &&
        p_dai->p_driver                      != NULL &&
        p_dai->p_driver->p_opts->pfn_fmt_set != NULL) {
        return p_dai->p_driver->p_opts->pfn_fmt_set(p_dai, fmt);
    } else {
        return -AW_EINVAL;
    }
}

/**
 * \brief set dai system clock frequency
 */
aw_err_t awsa_soc_dai_sysclk_set (struct awsa_soc_dai *p_dai,
                                  int                  clk_id,
                                  uint32_t             freq,
                                  int                  dir)
{
    if (p_dai                                   != NULL &&
        p_dai->p_driver                         != NULL &&
        p_dai->p_driver->p_opts->pfn_sysclk_set != NULL) {
        return p_dai->p_driver->p_opts->pfn_sysclk_set(p_dai,
                                                       clk_id,
                                                       freq,
                                                       dir);
    } else {
        return -AW_EINVAL;
    }
}

/**
 * \brief Set pll clock input and output frequency
 */
aw_err_t awsa_soc_dai_pll_set (struct awsa_soc_dai *p_dai,
                               int                  pll_id,
                               int                  source,
                               uint32_t             freq_in,
                               uint32_t             freq_out)
{
    if (p_dai                                != NULL &&
        p_dai->p_driver                      != NULL &&
        p_dai->p_driver->p_opts->pfn_pll_set != NULL) {
        return p_dai->p_driver->p_opts->pfn_pll_set(p_dai,
                                                    pll_id,
                                                    source,
                                                    freq_in,
                                                    freq_out);
    } else {
        return -AW_EINVAL;
    }
}

/* end of file */
