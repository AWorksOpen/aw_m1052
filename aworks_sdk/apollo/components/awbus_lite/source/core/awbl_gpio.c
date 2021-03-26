/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus GPIO interface implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 1.10 13-05-08  zen, add gpio toggle function
 * - 1.00 12-09-29  zen, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "aw_assert.h"
#include "aw_spinlock.h"

#include "awbus_lite.h"
#include "aw_gpio.h"
#include "awbl_gpio.h"

/*******************************************************************************
  import
*******************************************************************************/

AWBL_METHOD_IMPORT(awbl_gpioserv_get);

/*******************************************************************************
  locals
*******************************************************************************/

/** \brief pointer to first gpio service */
struct awbl_gpio_service *__gp_gpio_serv_head = NULL;

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief allocate GPIO service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval          AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __gpio_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t      pfunc_gpio_serv = NULL;
    struct awbl_gpio_service  *p_gpio_serv     = NULL;

    /* find handler */

    pfunc_gpio_serv = awbl_dev_method_get(p_dev,
                                          AWBL_METHOD_CALL(awbl_gpioserv_get));

    /* call method handler to allocate GPIO service */

    if (pfunc_gpio_serv != NULL) {

        pfunc_gpio_serv(p_dev, &p_gpio_serv);

        /* found a GPIO service, insert it to the service list */

        if (p_gpio_serv != NULL) {
            struct awbl_gpio_service **pp_serv_cur = &__gp_gpio_serv_head;

            while (*pp_serv_cur != NULL) {
                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }

            *pp_serv_cur = p_gpio_serv;
             p_gpio_serv->p_next = NULL;
        }
    }

    return AW_OK;   /* iterating continue */
}

/**
 * \brief allocate gpio services from instance tree
 */
aw_err_t __gpio_serv_alloc (void)
{
    /* todo: count the number of service */

    /* Empty the list head, as every gpio device will be find */
    __gp_gpio_serv_head = NULL;

    awbl_dev_iterate(__gpio_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

/**
 * \brief find out a service who accept the pin
 */
struct awbl_gpio_service * __gpio_pin_to_serv (int pin)
{
    struct awbl_gpio_service *p_serv_cur = __gp_gpio_serv_head;

    /* todo: if only one service, just return it directly */

    while ((p_serv_cur != NULL)) {

        if ((pin >= p_serv_cur->p_servinfo->pin_min) &&
            (pin <= p_serv_cur->p_servinfo->pin_max)) {

            return p_serv_cur;
        }

        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}

/******************************************************************************/
void awbl_gpio_init (void)
{
    /* allocate gpio services from instance tree */

    __gpio_serv_alloc();
}

/******************************************************************************/
aw_err_t aw_gpio_get (int pin)
{
    struct awbl_gpio_service *p_serv = __gpio_pin_to_serv(pin);

    if (p_serv == NULL) {
        return -AW_ENXIO;  /* GPIO[pin] is invalid */
    }
    pin -= p_serv->p_servinfo->pin_min;
    aw_assert(p_serv->p_servfuncs->pfunc_gpio_get != NULL);

    return p_serv->p_servfuncs->pfunc_gpio_get(p_serv->p_cookie, pin);
}

/******************************************************************************/
aw_err_t aw_gpio_set(int pin, int value)
{
    struct awbl_gpio_service *p_serv = __gpio_pin_to_serv(pin);
    uint8_t *p_pin_state = NULL;

    if (p_serv == NULL) {
        return -AW_ENXIO;  /* GPIO[pin] is invalid */
    }
    pin -= p_serv->p_servinfo->pin_min;
    aw_assert(p_serv->p_servfuncs->pfunc_gpio_set != NULL);

    /* if pin state memory exist, update pin's state */
    p_pin_state = p_serv->p_servinfo->p_pin_state;
    if (p_pin_state) {
        int index  = pin >> 3;
        int offset = pin & 0x07;
        aw_spinlock_isr_t lock = AW_SPINLOCK_ISR_UNLOCK;

        aw_spinlock_isr_take(&lock);
        if (value == 0) {
            AW_BIT_CLR(p_pin_state[index], offset);
        } else {
            AW_BIT_SET(p_pin_state[index], offset);
        }
        aw_spinlock_isr_give(&lock);
    }

    return p_serv->p_servfuncs->pfunc_gpio_set(p_serv->p_cookie, pin, value);
}

/******************************************************************************/
aw_err_t aw_gpio_toggle(int pin)
{
    struct awbl_gpio_service *p_serv = __gpio_pin_to_serv(pin);
    uint8_t *p_pin_state = NULL;

    if (p_serv == NULL) {
        return -AW_ENXIO;  /* GPIO[pin] is invalid */
    }
    pin -= p_serv->p_servinfo->pin_min;

    /* use driver's toggle function */
    if (p_serv->p_servfuncs->pfunc_gpio_toggle) {
        return p_serv->p_servfuncs->pfunc_gpio_toggle(p_serv->p_cookie, pin);
    }

    /* use general implementation */
    p_pin_state = p_serv->p_servinfo->p_pin_state;
    if (p_pin_state) {
        int index  = pin >> 3;
        int offset = pin & 0x07;
        int new_state;
        aw_spinlock_isr_t lock = AW_SPINLOCK_ISR_UNLOCK;

        aw_spinlock_isr_take(&lock);
        new_state = !AW_BIT_ISSET(p_pin_state[index], offset);
        AW_BIT_TOGGLE(p_pin_state[index], offset);
        aw_spinlock_isr_give(&lock);

        aw_gpio_set(pin, new_state);

        return new_state;
    }

    return -AW_EIO;
}

/******************************************************************************/
aw_err_t aw_gpio_pin_cfg (int pin, uint32_t flags)
{
    struct awbl_gpio_service *p_serv = __gpio_pin_to_serv(pin);

    if (p_serv == NULL) {
        return -AW_ENXIO;  /* GPIO[pin] is invalid */
    }
    pin -= p_serv->p_servinfo->pin_min;
    aw_assert(p_serv->p_servfuncs->pfunc_gpio_pin_cfg != NULL);

    return p_serv->p_servfuncs->pfunc_gpio_pin_cfg(p_serv->p_cookie,
                                                   pin,
                                                   flags);
}

/******************************************************************************/
aw_err_t aw_gpio_pin_request(const char *p_name, const int *p_pins, int num)
{
    int i   = 0;
    int pin = 0;
    aw_err_t ret = AW_OK;

    for (i = 0; i < num; i++) {

        pin = p_pins[i];
        struct awbl_gpio_service *p_serv = __gpio_pin_to_serv(pin);

        if (p_serv == NULL) {
            return -AW_ENXIO;  /* GPIO[pin] is invalid */
        }
        pin -= p_serv->p_servinfo->pin_min;
        aw_assert(p_serv->p_servfuncs->pfunc_gpio_pin_request != NULL);

        ret = p_serv->p_servfuncs->pfunc_gpio_pin_request(p_serv->p_cookie,
                                                          p_name,
                                                          pin);
        if (ret != AW_OK) {
            break;
        }
    }

    return ret;
}

/******************************************************************************/
aw_err_t aw_gpio_pin_release(const int *p_pins, int num)
{
    int i   = 0;
    int pin = 0;
    aw_err_t ret = AW_OK;

    for (i = 0; i < num; i++) {

        pin = p_pins[i];
        struct awbl_gpio_service *p_serv = __gpio_pin_to_serv(pin);

        if (p_serv == NULL) {
            return -AW_ENXIO;  /* GPIO[pin] is invalid */
        }
        pin -= p_serv->p_servinfo->pin_min;
        aw_assert(p_serv->p_servfuncs->pfunc_gpio_pin_release != NULL);

        ret = p_serv->p_servfuncs->pfunc_gpio_pin_release(p_serv->p_cookie,
                                                          pin);
        if (ret != AW_OK) {
            break;
        }
    }

    return ret;
}

/******************************************************************************/
aw_err_t aw_gpio_trigger_cfg(int pin, uint32_t flags)
{
    struct awbl_gpio_service *p_serv = __gpio_pin_to_serv(pin);

    if (p_serv == NULL) {
        return -AW_ENXIO;  /* GPIO[pin] is invalid */
    }
    pin -= p_serv->p_servinfo->pin_min;
    aw_assert(p_serv->p_servfuncs->pfunc_gpio_trigger_cfg != NULL);

    return p_serv->p_servfuncs->pfunc_gpio_trigger_cfg(p_serv->p_cookie,
                                                       pin,
                                                       flags);
}

/******************************************************************************/
aw_err_t aw_gpio_trigger_on (int pin)
{
    struct awbl_gpio_service *p_serv = __gpio_pin_to_serv(pin);

    if (p_serv == NULL) {
        return -AW_ENXIO;  /* GPIO[pin] is invalid */
    }
    pin -= p_serv->p_servinfo->pin_min;
    aw_assert(p_serv->p_servfuncs->pfunc_gpio_trigger_on != NULL);

    return p_serv->p_servfuncs->pfunc_gpio_trigger_on(p_serv->p_cookie, pin);
}

/******************************************************************************/
aw_err_t aw_gpio_trigger_off (int pin)
{
    struct awbl_gpio_service *p_serv = __gpio_pin_to_serv(pin);

    if (p_serv == NULL) {
        return -AW_ENXIO;
    }
    pin -= p_serv->p_servinfo->pin_min;
    aw_assert(p_serv->p_servfuncs->pfunc_gpio_trigger_off != NULL);

    return p_serv->p_servfuncs->pfunc_gpio_trigger_off(p_serv->p_cookie, pin);
}

/******************************************************************************/
aw_err_t aw_gpio_trigger_connect (int             pin,
                                  aw_pfuncvoid_t  pfunc_callback,
                                  void           *p_arg)
{
    struct awbl_gpio_service *p_serv = __gpio_pin_to_serv(pin);

    if (p_serv == NULL) {
        return -AW_ENXIO;      /* GPIO[pin] is invalid */
    }

    if (pfunc_callback == NULL) {
        return -AW_EFAULT;     /* pfunc_callback is NULL*/
    }
    pin -= p_serv->p_servinfo->pin_min;
    aw_assert(p_serv->p_servfuncs->pfunc_gpio_trigger_connect != NULL);

    return p_serv->p_servfuncs->pfunc_gpio_trigger_connect(p_serv->p_cookie,
                                                           pin,
                                                           pfunc_callback,
                                                           p_arg);
}

/******************************************************************************/
aw_err_t aw_gpio_trigger_disconnect(int             pin,
                                    aw_pfuncvoid_t  pfunc_callback,
                                    void           *p_arg)
{
    struct awbl_gpio_service *p_serv = __gpio_pin_to_serv(pin);

    if (p_serv == NULL) {
        return -AW_ENXIO;      /* GPIO[pin] is invalid */
    }

    if (pfunc_callback == NULL) {
        return -AW_EFAULT;     /* pfunc_callback is NULL*/
    }
    pin -= p_serv->p_servinfo->pin_min;
    aw_assert(p_serv->p_servfuncs->pfunc_gpio_trigger_disconnect != NULL);

    return p_serv->p_servfuncs->pfunc_gpio_trigger_disconnect(p_serv->p_cookie,
                                                              pin,
                                                              pfunc_callback,
                                                              p_arg);
}

/* end of file */
