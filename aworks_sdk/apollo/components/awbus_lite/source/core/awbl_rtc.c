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
 * \brief AWBus RTC interface implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-05  zen, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "aw_assert.h"
#include "awbus_lite.h"
#include "aw_rtc.h"
#include "awbl_rtc.h"
#include "time.h"

/*******************************************************************************
  import
*******************************************************************************/

AWBL_METHOD_IMPORT(awbl_rtcserv_get);

/*******************************************************************************
  locals
*******************************************************************************/

/* pointer to first rtc service */
struct awbl_rtc_service *__gp_rtc_serv_head = NULL;

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief allocate RTC service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval          AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __rtc_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t     pfunc_rtc_serv = NULL;
    struct awbl_rtc_service  *p_rtc_serv     = NULL;

    /* find handler */

    pfunc_rtc_serv = awbl_dev_method_get(p_dev,
                                         AWBL_METHOD_CALL(awbl_rtcserv_get));

    /* call method handler to allocate RTC service */

    if (pfunc_rtc_serv != NULL) {

        pfunc_rtc_serv(p_dev, &p_rtc_serv);

        /* found a RTC service, insert it to the service list */

        if (p_rtc_serv != NULL) {
            struct awbl_rtc_service **pp_serv_cur = &__gp_rtc_serv_head;

            while (*pp_serv_cur != NULL) {
                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }

            *pp_serv_cur = p_rtc_serv;
             p_rtc_serv->p_next = NULL;
        }
    }

    return AW_OK;   /* iterating continue */
}

/**
 * \brief allocate rtc services from instance tree
 */
aw_err_t __rtc_serv_alloc (void)
{
    /* todo: count the number of service */

    /* Empty the list head, as every rtc device will be find */
    __gp_rtc_serv_head = NULL;

    awbl_dev_iterate(__rtc_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

/**
 * \brief find out a service who accept the id
 */
struct awbl_rtc_service * __rtc_id_to_serv (int id)
{
    struct awbl_rtc_service *p_serv_cur = __gp_rtc_serv_head;

    while ((p_serv_cur != NULL)) {

        if (id == p_serv_cur->p_servinfo->rtc_id) {

            return p_serv_cur;
        }

        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}

/******************************************************************************/
void awbl_rtc_init (void)
{
    /* allocate rtc services from instance tree */

    __rtc_serv_alloc();
}

/******************************************************************************/
aw_err_t aw_rtc_time_get (int id, aw_tm_t *p_tm)
{
    aw_err_t                 ret;
    struct awbl_rtc_service *p_serv = __rtc_id_to_serv(id);

    if (p_tm == NULL) {
        return -AW_EINVAL;
    }
    if (p_serv == NULL) {
        return -AW_ENXIO;  /* RTC[id] is invalid */
    }

    if (p_serv->p_servopts->time_get != NULL) {
        aw_time_t time;

        /* get time */
        ret = p_serv->p_servopts->time_get(p_serv->p_cookie, p_tm);
        if (ret != AW_OK) {
            return ret;
        }

        /* check if tm is correct, and update the wday and yday */
        p_tm->tm_isdst = -1;
        if (aw_tm_to_time(p_tm, &time) != AW_OK) {
            return -AW_EBADF;
        }

        return AW_OK;
    }

    return -AW_ENOTSUP;
}

/******************************************************************************/
aw_err_t aw_rtc_time_set(int id, aw_tm_t *p_tm)
{
    struct awbl_rtc_service *p_serv = __rtc_id_to_serv(id);
    aw_time_t time;

    if (p_tm == NULL) {
        return -AW_EINVAL;
    }
    if (p_serv == NULL) {
        return -AW_ENXIO;  /* RTC[id] is invalid */
    }

    /* check if tm is correct, and update the wday and yday */
    if (aw_tm_to_time(p_tm, &time) != AW_OK) {
        return -AW_EBADF;
    }

    if (p_serv->p_servopts->time_set != NULL) {
        return p_serv->p_servopts->time_set(p_serv->p_cookie, p_tm);
    }

    return -AW_ENOTSUP;
}

/******************************************************************************/
aw_err_t aw_rtc_ctrl(int id, int req, void *arg)
{
    struct awbl_rtc_service *p_serv = __rtc_id_to_serv(id);

    if (p_serv == NULL) {
        return -AW_ENXIO;  /* RTC[id] is invalid */
    }
    if (p_serv->p_servopts->dev_ctrl != NULL) {
        return p_serv->p_servopts->dev_ctrl(p_serv->p_cookie, req, arg);
    }

    return -AW_ENOTSUP;
}


/******************************************************************************/
aw_err_t aw_rtc_timespec_get(int id, aw_timespec_t *p_tv)
{
    aw_err_t                 ret;
    struct awbl_rtc_service *p_serv = __rtc_id_to_serv(id);

    if (p_tv == NULL) {
        return -AW_EINVAL;
    }
    if (p_serv == NULL) {
        return -AW_ENXIO;  /* RTC[id] is invalid */
    }

    if (p_serv->p_servopts->timespec_get != NULL) {

        /* get timespec */
        ret = p_serv->p_servopts->timespec_get(p_serv->p_cookie, p_tv);

        if (ret != AW_OK) {
            return ret;
        }
        return AW_OK;
    }

    return -AW_ENOTSUP;
}

/******************************************************************************/
aw_err_t aw_rtc_timespec_set(int id, aw_timespec_t *p_tv)
{
    aw_err_t                 ret;
    struct awbl_rtc_service *p_serv = __rtc_id_to_serv(id);
    aw_time_t time;

    if (p_tv == NULL) {
        return -AW_EINVAL;
    }
    if (p_serv == NULL) {
        return -AW_ENXIO;  /* RTC[id] is invalid */
    }

    if (p_serv->p_servopts->timespec_set != NULL) {

        /* set timespec */
        ret = p_serv->p_servopts->timespec_set(p_serv->p_cookie, p_tv);

        if (ret != AW_OK) {
            return ret;
        }
        return AW_OK;
    }

    return -AW_ENOTSUP;
}


/* end of file */

