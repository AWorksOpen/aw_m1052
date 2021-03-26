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
 * \brief Create a new jack
 */
aw_err_t awsa_soc_jack_init (struct awsa_soc_jack  *p_jack,
                             struct awsa_soc_codec *p_codec,
                             const char            *p_name,
                             int                    type)
{
    p_jack->p_codec = p_codec;
    AW_INIT_LIST_HEAD(&p_jack->widgets);

    return awsa_core_jack_init(&p_jack->core_jack,
                               p_name,
                               type,
                               p_jack,
                               &p_codec->p_card->core_card);
}

/**
 * \brief Report the current status for a jack
 *        If configured using awsa_soc_jack_add_pins() then the associated
 *        DAPM pins will be enabled or disabled as appropriate and DAPM
 *        synchronised.
 */
void awsa_soc_jack_report (struct awsa_soc_jack *p_jack,
                           int                   status,
                           int                   mask)
{
    struct awsa_soc_codec        *p_codec  = NULL;
    struct awsa_soc_dapm_context *p_dapm   = NULL;
    struct awsa_soc_jack_widget  *p_widget = NULL;
    int                           enable;
    int                           old_status;

    if (p_jack == NULL) {
        return;
    }

    p_codec = p_jack->p_codec;
    p_dapm  = &p_codec->dapm;

    AW_MUTEX_LOCK(p_codec->mutex, AW_SEM_WAIT_FOREVER);

    old_status      = p_jack->status;

    p_jack->status &= ~mask;
    p_jack->status |= status & mask;

    /*
     * The DAPM sync is expensive enough to be worth skipping.
     * However, empty mask means pin synchronization is desired.
     */
    if (mask && (p_jack->status == old_status)) {
        goto __out;
    }

    aw_list_for_each_entry(p_widget,
                           &p_jack->widgets,
                           struct awsa_soc_jack_widget,
                           list) {
        enable = p_widget->p_driver->mask & p_jack->status;

        if (p_widget->p_driver->invert) {
            enable = !enable;
        }

        if (enable) {
            awsa_soc_dapm_widget_enable(p_dapm, p_widget->p_driver->p_name);
        } else {
            awsa_soc_dapm_widget_disable(p_dapm, p_widget->p_driver->p_name);
        }
    }

    awsa_soc_dapm_sync(p_dapm);
    awsa_core_jack_report(&p_jack->core_jack, status);

__out:
    AW_MUTEX_UNLOCK(p_codec->mutex);
}

/**
 * \brief Associate DAPM pins with an jack
 */
aw_err_t awsa_soc_jack_widgets_add (
    struct awsa_soc_jack                     *p_jack,
    struct awsa_soc_jack_widget              *p_widgets,
    const struct awsa_soc_jack_widget_driver *p_driver,
    int                                       count)
{
    int i;

    for (i = 0; i < count; i++) {
        if (p_driver[i].p_name == NULL) {
            AWSA_SOC_DEBUG_ERROR("no name for widget %d", i);
            return -AW_EINVAL;
        }

        if (p_driver[i].mask == 0) {
            AWSA_SOC_DEBUG_ERROR("no mask for widget %d (%s)",
                                  i,
                                  p_driver[i].p_name);
            return -AW_EINVAL;
        }

        p_widgets[i].p_driver = p_driver;

        AW_INIT_LIST_HEAD(&p_widgets[i].list);
        aw_list_add(&(p_widgets[i].list), &p_jack->widgets);
    }

    /*
     * Update to reflect the last reported status; canned jack
     * implementations are likely to set their state before the
     * card has an opportunity to associate pins.
     */
    awsa_soc_jack_report(p_jack, 0, 0);
    return AW_OK;
}

/**
 * \brief gpio detect
 */
static void __soc_jack_gpio_detect (struct awsa_soc_jack_gpio *p_gpio)
{
    struct awsa_soc_jack *p_jack = p_gpio->p_jack;

    int enable;
    int report;

    enable = aw_gpio_get(p_gpio->gpio);
    if (p_gpio->p_driver->invert) {
        enable = !enable;
    }

    if (enable != 0) {
        report = p_gpio->p_driver->report;
    } else {
        report = 0;
    }

    if (p_gpio->p_driver->pfn_jack_status_check != NULL) {
        report = p_gpio->p_driver->pfn_jack_status_check();
    }

    awsa_soc_jack_report(p_jack, report, p_gpio->p_driver->report);
}

/**
 * \brief GPIO irq service
  */
static void __soc_jack_gpio_handler (void *p_data)
{
    struct awsa_soc_jack_gpio *p_gpio = p_data;

    aw_delayed_work_start(&p_gpio->work,
                          aw_ms_to_ticks(p_gpio->p_driver->debounce_time));
    aw_gpio_trigger_off(p_gpio->gpio);
}

/**
 * \brief gpio delay work
 */
static void __soc_jack_gpio_work (void *p_arg)
{
    struct aw_delayed_work    *p_work = p_arg;
    struct awsa_soc_jack_gpio *p_gpio;

    p_gpio = AW_CONTAINER_OF(p_work, struct awsa_soc_jack_gpio, work);
    __soc_jack_gpio_detect(p_gpio);

    aw_gpio_trigger_on(p_gpio->gpio);
}

/**
 * \nroef Associate GPIO pins with an jack
 */
aw_err_t awsa_soc_jack_gpios_add (
    struct awsa_soc_jack                   *p_jack,
    struct awsa_soc_jack_gpio              *p_jack_gpios,
    const struct awsa_soc_jack_gpio_driver *p_driver,
    int                                     count,
    const int                              *p_gpio)
{
    int i, ret;

    for (i = 0; i < count; i++) {

        if (p_driver[i].p_name == NULL) {
            AWSA_SOC_DEBUG_ERROR("No name for gpio %d", p_gpio[i]);
            ret = -AW_EINVAL;
            goto __undo;
        }

        /* 设置引脚触发类型，下降沿触发 */
        ret = aw_gpio_trigger_cfg(p_gpio[i], AW_GPIO_TRIGGER_BOTH_EDGES);
        if (ret != AW_OK) {
            goto __undo;
        }

        /* 连接回调函数到引脚 */
        ret = aw_gpio_trigger_connect(p_gpio[i],
                                      __soc_jack_gpio_handler,
                                     (void *)&p_jack_gpios[i]);
        if (ret != AW_OK) {
            goto __undo;
        }

        aw_delayed_work_init(&p_jack_gpios[i].work,
                             __soc_jack_gpio_work,
                             (void *)&p_jack_gpios[i].work);
        p_jack_gpios[i].p_jack = p_jack;

        /* 开启引脚上的触发器 */
        ret = aw_gpio_trigger_on(p_gpio[i]);
        if (ret != AW_OK) {
            goto __undo;
        }

        p_jack_gpios[i].p_driver = &p_driver[i];
        p_jack_gpios[i].gpio     = p_gpio[i];

        /* Update initial jack status */
        __soc_jack_gpio_detect(&p_jack_gpios[i]);
    }
    return AW_OK;

__undo :
    awsa_soc_jack_gpios_del(p_jack, p_jack_gpios, i);
    return ret;
}

/**
 * \brief Release GPIO pins' resources of an soc jack
 */
void awsa_soc_jack_gpios_del (struct awsa_soc_jack       *p_jack,
                              struct awsa_soc_jack_gpio  *p_gpios,
                              int                         count)
{
    int i;

    for (i = 0; i < count; i++) {
        aw_gpio_trigger_off(p_gpios[i].gpio);
        aw_gpio_trigger_disconnect(p_gpios[i].gpio,
                                   __soc_jack_gpio_handler,
                                   (void *)0);
        aw_delayed_work_stop(&p_gpios[i].work);
        p_gpios[i].p_jack = NULL;
    }
}

/* end of file */
