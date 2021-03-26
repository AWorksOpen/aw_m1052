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

#ifndef __AWSA_SOC_JACK_H
#define __AWSA_SOC_JACK_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "soc/awsa_soc_internal.h"

/**
 * \brief jack widget driver
 */
struct awsa_soc_jack_widget_driver {

    /** \brief name */
    const  char  *p_name;

    /** \brief bits to check for in reported jack status */
    int           mask;

    /** \brief if non-zero then pin is enabled when status is not reported */
    aw_bool_t     invert;
} ;

/**
 * \brief Describes a widget to update based on jack detection
 */
struct awsa_soc_jack_widget {

    /** \brief list */
    struct aw_list_head                       list;

    /** \brief jack widget driver */
    const struct awsa_soc_jack_widget_driver *p_driver;
};

/**
 * \brief jack gpio driver
 */
struct awsa_soc_jack_gpio_driver {

    /** \brief gpio name */
    const char             *p_name;

    /** \brief value to report when jack detected */
    int                     report;

    /** \brief report presence in low state */
    int                     invert;

    /** \brief debouce time in ms */
    int                     debounce_time;

    /** \brief get pin level status */
    int (*pfn_jack_status_check) (void);
};

/**
 * \brief Describes a gpio pin for jack detection
 */
struct awsa_soc_jack_gpio {

    /** \brief jack device */
    struct awsa_soc_jack                   *p_jack;

    /** \brief jack gpio driver */
    const struct awsa_soc_jack_gpio_driver *p_driver;

    /** \brief gpio number */
    int                      gpio;

    /** \brief interrupt delay deal with */
    struct aw_delayed_work   work;
};

/**
 * \brief Soc Jack structure
 */
struct awsa_soc_jack {

    /** \brief jack device */
    struct awsa_core_jack  core_jack;

    /** \brief codec device */
    struct awsa_soc_codec *p_codec;

    /** \brief list */
    struct aw_list_head    widgets;

    /** \brief jack device status */
    int                    status;
};

/**
 * \brief Create a new jack
 *
 * \param[in] p_card  : card
 * \param[in] p_name  : an identifying string for this jack
 * \param[in] type    : a bitmask of enum awsa_core_jack_type values that
 *                      can be detected by this jack
 * \param[in] p_jack  : structure to use for the jack
 *
 * \retval AW_OK      : success
 * \retval -AW_ENOMEM : allocate failed
 */
aw_err_t awsa_soc_jack_init (struct awsa_soc_jack  *p_jack,
                             struct awsa_soc_codec *p_codec,
                             const char            *p_name,
                             int                    type);

/**
 * \brief Report the current status for a jack
 *        If configured using awsa_soc_jack_add_pins() then the associated
 *        DAPM pins will be enabled or disabled as appropriate and DAPM
 *        synchronised.
 *
 * \param[in] p_jack : the jack
 * \param[in] status : a bitmask of enum awsa_core_jack_type values
 *                     that are currently detected.
 * \param[in] mask   : a bitmask of enum awsa_core_jack_type values
 *                     that being reported.
 *
 * \return no
 * \note This function uses mutexes and should be called from a
 *       context which can sleep (such as a workqueue).
 */
void awsa_soc_jack_report (struct awsa_soc_jack *p_jack,
                           int                   status,
                           int                   mask);

/**
 * \brief Associate DAPM pins with an jack
 *
 * \param[in] p_jack    : Soc jack
 * \param[in] count     : Number of jack widgets
 * \param[in] p_widgets : Array of jack widgets
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : invalid parameter
 *
 * \note After this function has been called the DAPM pins specified in the
 *       pins array will have their status updated to reflect the current
 *       state of the jack whenever the jack status is updated.
 */
aw_err_t awsa_soc_jack_widgets_add (
    struct awsa_soc_jack                     *p_jack,
    struct awsa_soc_jack_widget              *p_widgets,
    const struct awsa_soc_jack_widget_driver *p_driver,
    int                                       count);

/**
 * \nroef Associate GPIO pins with an jack
 *
 * \param[in] p_jack  : SoC jack
 * \param[in] count   : number of pins
 * \param[in] p_gpios : array of gpio pins
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : invalid parameter
 *
 * \note This function will register interrupt for each gpio in the array.
 */
aw_err_t awsa_soc_jack_gpios_add (
    struct awsa_soc_jack                   *p_jack,
    struct awsa_soc_jack_gpio              *p_jack_gpios,
    const struct awsa_soc_jack_gpio_driver *p_driver,
    int                                     count,
    const int                              *p_gpio);

/**
 * \brief Release GPIO pins' resources of an SoC jack
 *
 * \param[in] p_jack  : jack
 * \param[in] count   : number of pins
 * \param[in] p_gpios : array of gpio pins
 *
 * \return no
 * \note This function will unregister interrupt for each gpio in the array.
 */
void awsa_soc_jack_gpios_del (struct awsa_soc_jack       *p_jack,
                              struct awsa_soc_jack_gpio  *p_gpios,
                              int                         count);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_SOC_JACK_H */

/* end of file */
