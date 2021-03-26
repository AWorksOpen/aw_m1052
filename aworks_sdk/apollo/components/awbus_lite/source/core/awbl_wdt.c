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
 * \brief ����AWBus�Ŀ��Ź�����
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

/** \brief �ö�ʱ�����ڶ�ʱιӲ�����Ź� */
volatile aw_timer_t  g_sys_wdt_timer;

/** \brief Ӳ�����Ź���Ϣ */
aw_local volatile struct awbl_hwwdt_info  *__gp_hwwdt_info = NULL;

/** \brief ιӲ���ٽ���*/
AW_INT_CPU_LOCK_DECL_STATIC(__g_hwwdt_lock);

/*******************************************************************************
  forward declarations
*******************************************************************************/

/* ������Ź���������ʱ�ص�����������Ӳ�����Ź�ι�� */
aw_local  void  __my_hwwdt_timer_callback (void  *p_param);

/* �������Ź��Ķ�ʱ�ص�����������֪ͨ���Ź�������û��ι�� */
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

    /* ��ʼ������������Ź��Ķ�ʱ��  */
    aw_timer_init(&p_wdt->sftimer, __my_wdt_callback, NULL);

    /* ��������������Ź��Ķ�ʱ������ʱΪt_ms ms */
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

    /* ����Ӳ�����Ź���Ӧ�Ķ�ʱ������ʱΪ__gp_hwwdt_info->wdt_time ms */
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

    /* ��Ӳ�����Ź�ʱ������Ϊ��ʼ�Ķ�ʱ��ʱ�� */
    if ( (__gp_hwwdt_info != NULL) &&
         (__gp_hwwdt_info->feed != NULL) &&
         (__gp_hwwdt_info->wdt_time > 1)) {

        /* ��ʼ����ʱ��  */
        aw_timer_init((aw_timer_t *)&g_sys_wdt_timer,
                      __my_hwwdt_timer_callback, 
                      NULL);

        /* 
         * ���������ʱ����Ӳ�����Ź���ʱ������Ҫ��֤�ܹ��ڹ涨ʱ��������ι
         * ��������Ҫ�������ʱ�����ڵ�С ���������ΪӲ�����Ź���ʱ���ڵ�60%��
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

        /* ����Ӳ�����Ź���Ӧ�Ķ�ʱ������ʱΪ__gp_hwwdt_info->wdt_time ms */
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
