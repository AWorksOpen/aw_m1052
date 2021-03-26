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
 * \brief AWBus input capture interface implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 1.00 14-10-14  tee, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_cap.h"
#include "awbl_cap.h"
#include "aw_errno.h"

/*******************************************************************************
  import
*******************************************************************************/

AWBL_METHOD_IMPORT(awbl_capserv_get);

/*******************************************************************************
  locals
*******************************************************************************/

/** \brief pointer to first cap service */
struct awbl_cap_service *__gp_cap_serv_head = NULL;

/**
 * \brief allocate cap service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval          AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __cap_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t    pfunc_cap_serv = NULL;
    struct awbl_cap_service  *p_cap_serv    = NULL;

    /* ��ȡmethod��handler */
    pfunc_cap_serv = awbl_dev_method_get(p_dev,
                                         AWBL_METHOD_CALL(awbl_capserv_get));

    if (pfunc_cap_serv != NULL) {

        pfunc_cap_serv(p_dev, &p_cap_serv);

        /* �ҵ�һ��cap����󣬲����������service list�� */

        if (p_cap_serv != NULL) {
            struct awbl_cap_service **pp_serv_cur = &__gp_cap_serv_head;

            while (*pp_serv_cur != NULL) {
                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }

            *pp_serv_cur = p_cap_serv;
            p_cap_serv->p_next = NULL;
        }
    }

    return AW_OK;
}

aw_err_t __cap_serv_alloc (void)
{
    /* Empty the list head, as every cap device will be find */
    __gp_cap_serv_head = NULL;

    /**
     * ���豸ִ�еı������� : __cap_serv_alloc_helper
     * ���ݸ����������Ĳ��� : NULL
     * ������Ϊ             : AWBL_ITERATE_INSTANCES (��������ʵ��)
     */
    awbl_dev_iterate(__cap_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

struct awbl_cap_service * __cap_pnum_to_serv(int pid)
{
    struct awbl_cap_service *p_serv_cur = __gp_cap_serv_head;

    /* �������������ҵ���Ӧ�ķ��� */
    while (p_serv_cur != NULL) {
         if ((pid >= p_serv_cur->p_servinfo->cap_min_id) &&
             (pid <= p_serv_cur->p_servinfo->cap_max_id)) {

             return p_serv_cur;
         }

         p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}


/**
 * \brief ����cap�豸����Чʱ�������
 *
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_ENXIO    \a pid Ϊ�����ڵ�ͨ����
 * \retval     -AW_EINVAL   \a ��Ч����
 */
aw_err_t aw_cap_config(int             pid,
                       unsigned int    flags,
                       cap_callback_t  pfunc_callback,
                       void           *p_arg)
{
    struct awbl_cap_service *p_serv = __cap_pnum_to_serv(pid);

    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    return p_serv->p_servfuncs->pfunc_cap_config(p_serv->p_cookie,
                                                   pid,
                                                   flags,
                                                   pfunc_callback,
                                                   p_arg);
}

/**
 *\brief ʹ��cap�豸
 */
aw_err_t aw_cap_enable(int pid)
{
    struct awbl_cap_service *p_serv = __cap_pnum_to_serv(pid);

    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    return p_serv->p_servfuncs->pfunc_cap_enable(p_serv->p_cookie,
                                                 pid);
}
/**
 *\brief ����cap�豸
 */
aw_err_t aw_cap_disable(int pid)
{
    struct awbl_cap_service *p_serv = __cap_pnum_to_serv(pid);

    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    return p_serv->p_servfuncs->pfunc_cap_disable(p_serv->p_cookie,
                                                  pid);
}

/**
 *\brief ��λcap�豸
 */
aw_err_t aw_cap_reset(int cap_pid)
{
    struct awbl_cap_service *p_serv = __cap_pnum_to_serv(cap_pid);

    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    return p_serv->p_servfuncs->pfunc_cap_reset(p_serv->p_cookie,
                                                cap_pid);
}

/**
 *\brief �����β��񵽵�count��ֵת��Ϊʱ��(ns)
 */
aw_err_t aw_cap_count_to_time(int cap_pid,
                              unsigned int count1,
                              unsigned int count2,
                              unsigned int *p_time_ns)
{
    struct awbl_cap_service *p_serv = __cap_pnum_to_serv(cap_pid);

    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    return p_serv->p_servfuncs->pfunc_cap_count_to_time(p_serv->p_cookie,
                                                        cap_pid,
                                                        count1,
                                                        count2,
                                                        p_time_ns);
}

/******************************************************************************/
void awbl_cap_init (void)
{
    /* ��ʵ�����з���cap���� */
    __cap_serv_alloc();
}

