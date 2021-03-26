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
 * \brief AWBus PWM interface implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 1.01 14-07-01  ops, modified to match the device by using pid(pwm channel id)
 * - 1.00 14-06-05  ops, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"

#include "awbus_lite.h"
#include "aw_pwm.h"
#include "awbl_pwm.h"

/*******************************************************************************
  import
*******************************************************************************/

AWBL_METHOD_IMPORT(awbl_pwmserv_get);

/*******************************************************************************
  locals
*******************************************************************************/

/* pointer to first pwm service */
struct awbl_pwm_service *__gp_pwm_serv_head = NULL;

/**
 * \brief allocate PWM service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval          AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __pwm_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t    pfunc_pwm_serv = NULL;
    struct awbl_pwm_service  *p_pwm_serv    = NULL;

    /* 获取method的handler */
    pfunc_pwm_serv = awbl_dev_method_get(p_dev,
                                         AWBL_METHOD_CALL(awbl_pwmserv_get));

    if (pfunc_pwm_serv != NULL) {

        pfunc_pwm_serv(p_dev, &p_pwm_serv);

        /* 找到一个PWM服务后，插入服务链表（service list） */

        if (p_pwm_serv != NULL) {
            struct awbl_pwm_service **pp_serv_cur = &__gp_pwm_serv_head;

            while (*pp_serv_cur != NULL) {
                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }

            *pp_serv_cur = p_pwm_serv;
            p_pwm_serv->p_next = NULL;
        }
    }

    return AW_OK;
}

/**
 * \brief
 */
aw_err_t __pwm_serv_alloc (void)
{
    /* Empty the list head, as every pwm device will be find */
    __gp_pwm_serv_head = NULL;

    /**
     * 对设备执行的遍历函数 : __pwm_serv_alloc_helper
     * 传递给遍历函数的参数 : NULL
     * 遍历行为             : AWBL_ITERATE_INSTANCES (遍历所有实例)
     */
    awbl_dev_iterate(__pwm_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

struct awbl_pwm_service * __pwm_pnum_to_serv(int pid)
{
    struct awbl_pwm_service *p_serv_cur = __gp_pwm_serv_head;

    /* 遍历服务链表，找到对应的服务 */
    while (p_serv_cur != NULL) {
         if ((pid >= p_serv_cur->p_servinfo->pwm_min_id) &&
             (pid <= p_serv_cur->p_servinfo->pwm_max_id)) {

            return p_serv_cur;
        }

        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}


/**
 * \brief 设置PWM设备的有效时间和周期
 *
 * \retval      AW_OK         成功
 * \retval     -AW_ENXIO   \a pid 为不存在的通道号
 * \retval     -EINVAL     \a 无效参数
 */
aw_err_t aw_pwm_config(int pid, unsigned long duty_ns, unsigned long period_ns)
{
    struct awbl_pwm_service *p_serv = __pwm_pnum_to_serv(pid);

    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    return p_serv->p_servfuncs->pfunc_pwm_config(p_serv->p_cookie,
                                                 pid,
                                                 duty_ns,
                                                 period_ns);
}

/**
 *\brief 使能PWM设备
 */
aw_err_t aw_pwm_enable(int pid)
{
    struct awbl_pwm_service *p_serv = __pwm_pnum_to_serv(pid);

    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    return p_serv->p_servfuncs->pfunc_pwm_enable(p_serv->p_cookie,
                                                 pid);
}
/**
 *\brief 禁用PWM设备
 */
aw_err_t aw_pwm_disable(int pid)
{
    struct awbl_pwm_service *p_serv = __pwm_pnum_to_serv(pid);

    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    return p_serv->p_servfuncs->pfunc_pwm_disable(p_serv->p_cookie,
                                                  pid);
}

/**
 *\brief pwm精确输出
 */
aw_err_t aw_pwm_output(int pid, unsigned long period_num)
{
    struct awbl_pwm_service *p_serv = __pwm_pnum_to_serv(pid);

    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    return p_serv->p_servfuncs->pfunc_pwm_output(p_serv->p_cookie,
                                                 pid,
                                                 period_num);
}

/******************************************************************************/
void awbl_pwm_init (void)
{
    /* 从实例树中分配pwm服务 */
    __pwm_serv_alloc();
}

/* end of file */
