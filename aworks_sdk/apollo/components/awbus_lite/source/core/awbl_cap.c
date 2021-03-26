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

    /* 获取method的handler */
    pfunc_cap_serv = awbl_dev_method_get(p_dev,
                                         AWBL_METHOD_CALL(awbl_capserv_get));

    if (pfunc_cap_serv != NULL) {

        pfunc_cap_serv(p_dev, &p_cap_serv);

        /* 找到一个cap服务后，插入服务链表（service list） */

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
     * 对设备执行的遍历函数 : __cap_serv_alloc_helper
     * 传递给遍历函数的参数 : NULL
     * 遍历行为             : AWBL_ITERATE_INSTANCES (遍历所有实例)
     */
    awbl_dev_iterate(__cap_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

struct awbl_cap_service * __cap_pnum_to_serv(int pid)
{
    struct awbl_cap_service *p_serv_cur = __gp_cap_serv_head;

    /* 遍历服务链表，找到对应的服务 */
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
 * \brief 设置cap设备的有效时间和周期
 *
 * \retval      AW_OK       成功
 * \retval     -AW_ENXIO    \a pid 为不存在的通道号
 * \retval     -AW_EINVAL   \a 无效参数
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
 *\brief 使能cap设备
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
 *\brief 禁用cap设备
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
 *\brief 复位cap设备
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
 *\brief 将两次捕获到的count差值转换为时间(ns)
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
    /* 从实例树中分配cap服务 */
    __cap_serv_alloc();
}

