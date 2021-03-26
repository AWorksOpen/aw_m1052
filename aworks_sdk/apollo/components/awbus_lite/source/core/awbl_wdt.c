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
 * \brief 基于AWBus的看门狗驱动
 *
 * \internal
 * \par modification history
 * - 1.01 16-01-12  may, add awbl_hwwdt_feed()
 * - 1.00 14-08-27  may, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "aw_wdt.h"
#include "awbl_wdt.h"
#include "aw_int.h"


/*******************************************************************************
  locals
*******************************************************************************/

/** \brief 该定时器用于定时喂硬件看门狗 */
volatile aw_timer_t  g_sys_wdt_timer;

/** \brief 硬件看门狗信息 */
aw_local volatile struct awbl_hwwdt_info  *__gp_hwwdt_info = NULL;

/** \brief 喂硬件临界区*/
AW_INT_CPU_LOCK_DECL_STATIC(__g_hwwdt_lock);

/*******************************************************************************
  forward declarations
*******************************************************************************/

/* 软件看门狗管理器定时回调函数，用于硬件看门狗喂狗 */
aw_local  void  __my_hwwdt_timer_callback (void  *p_param);

/* 各个看门狗的定时回调函数，用于通知看门狗管理器没有喂狗 */
aw_local  void  __my_wdt_callback (void  *p_param);


/******************************************************************************/
aw_local  void  __my_wdt_callback (void *p_param)
{
    aw_timer_stop((aw_timer_t *)&g_sys_wdt_timer);
}


/******************************************************************************/
aw_err_t  aw_wdt_add (struct awbl_wdt  *p_wdt, uint32_t  t_ms)
{
    if (NULL == p_wdt) {
        return  -AW_EINVAL;
    }

    if (NULL == __gp_hwwdt_info) {
        return  -AW_ENXIO;
    }

    p_wdt->period_ms = t_ms;

    /* 初始化各个软件看门狗的定时器  */
    aw_timer_init(&p_wdt->sftimer, __my_wdt_callback, NULL);

    /* 启动各个软件看门狗的定时器，定时为t_ms ms */
    aw_timer_start(&p_wdt->sftimer, aw_ms_to_ticks(p_wdt->period_ms));

    return  AW_OK;
}

/******************************************************************************/
aw_err_t  aw_wdt_feed (struct awbl_wdt  *p_wdt)
{
    if (NULL == p_wdt) {
        return  -AW_EINVAL;
    }

    if (NULL == __gp_hwwdt_info) {
        return  -AW_ENXIO;
    }

    aw_timer_start(&p_wdt->sftimer, aw_ms_to_ticks(p_wdt->period_ms));

    return  AW_OK;
}

/******************************************************************************/
aw_local  void  __my_hwwdt_timer_callback (void  *p_param)
{
    uint32_t  time_val = 0;

    AW_INT_CPU_LOCK(__g_hwwdt_lock);
    __gp_hwwdt_info->feed((struct awbl_hwwdt_info *)__gp_hwwdt_info);
    AW_INT_CPU_UNLOCK(__g_hwwdt_lock);

    /* 启动硬件看门狗相应的定时器，定时为__gp_hwwdt_info->wdt_time ms */
    aw_timer_start((aw_timer_t *)&g_sys_wdt_timer,
        aw_ms_to_ticks(__gp_hwwdt_info->wdt_time));
}

/******************************************************************************/
aw_err_t awbl_hwwdt_feed(void)
{
    if ((NULL == __gp_hwwdt_info) ||
        (NULL == __gp_hwwdt_info->feed)) {
        return  -AW_EPERM;
    }

    AW_INT_CPU_LOCK(__g_hwwdt_lock);
    __gp_hwwdt_info->feed((struct awbl_hwwdt_info *)__gp_hwwdt_info);
    AW_INT_CPU_UNLOCK(__g_hwwdt_lock);

    return  AW_OK;
}

/******************************************************************************/
void  awbl_wdt_init (void)
{
    uint32_t  time_val = 0;

    /* 将硬件看门狗时间设置为初始的定时器时间 */
    if ( (__gp_hwwdt_info != NULL) &&
         (__gp_hwwdt_info->feed != NULL) &&
         (__gp_hwwdt_info->wdt_time > 1)) {

        /* 初始化定时器  */
        aw_timer_init((aw_timer_t *)&g_sys_wdt_timer,
                      __my_hwwdt_timer_callback, 
                      NULL);

        /* 
         * 由于软件定时器与硬件看门狗定时存在误差，要保证能够在规定时间内正常喂
         * 狗，就需要将软件定时器周期调小 ，这里调整为硬件看门狗定时周期的60%。
         */
        if (__gp_hwwdt_info->wdt_time < 10) {
            time_val = __gp_hwwdt_info->wdt_time * 9 / 10;
        } else {
            time_val = __gp_hwwdt_info->wdt_time / 5 * 3;
        }
        if (time_val < 1) {
            time_val = 1;
        }
        __gp_hwwdt_info->wdt_time = time_val;

        /* 启动硬件看门狗相应的定时器，定时为__gp_hwwdt_info->wdt_time ms */
        aw_timer_start((aw_timer_t *)&g_sys_wdt_timer,
            aw_ms_to_ticks((uint32_t)(time_val)));

        AW_INT_CPU_LOCK(__g_hwwdt_lock);
        __gp_hwwdt_info->feed((struct awbl_hwwdt_info *)__gp_hwwdt_info);
        AW_INT_CPU_UNLOCK(__g_hwwdt_lock);
    }
}

/******************************************************************************/
void  awbl_wdt_device_register (struct awbl_hwwdt_info  *pwdt_info)
{
    if ( (pwdt_info != NULL) &&
         (pwdt_info->feed != NULL) ) {

        __gp_hwwdt_info = pwdt_info;
    }
}


/* end of file */
