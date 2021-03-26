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
 * \brief 基于AWBus的硬件定时器驱动
 *
 * \internal
 * \par modification history
 * - 1.00 12-11-14  orz, first implementation
 * \endinternal
 */

#include "aw_hwtimer.h"   /* for timer flags */
#include "awbl_timer.h"
#include <string.h>

/**
 * \addtogroup awbl_timer
 * @{
 */

/** \brief convert aw_hwtimer_handle_t struct awbl_timer */
#define to_awbl_timer(timer)    ((struct awbl_timer *)timer)

/******************************************************************************/

/** \brief structure used for timer finding */
struct timer_search_info {
    struct awbl_dev    *p_inst;     /**< \brief timer instance selected */
    uint32_t            timer_no;   /**< \brief timer no of selected timer */
    struct awbl_timer  *p_timer;    /**< \brief timer functionality */
    uint32_t            freq;       /**< \brief requested frequency */
    uint32_t            min_freq;   /**< \brief requested minimum frequency */
    uint32_t            max_freq;   /**< \brief requested maximum frequency */
    uint32_t            features;   /**< \brief requested timer features */
    /**
     * \brief flag indicating that a timer is selected based on the
     * 'min_freq' and 'max_freq' values and there can be a better timer
     */
    int better_timer_desired;
};

/******************************************************************************/

/**
 * \brief finds the appropriate timer suiting the requirements
 *
 * The function selects the appropriate timer based on the frequency and the
 * timer features. If the requested frequency is not supported, then a timer
 * which has its min and max frequencies between the requested
 * 'min_freq' and 'max_freq' values will be selected.
 * desired is used.
 *
 * \param p_dev     AWBux device  遍历时传入设备参数
 * \param p_info    information for searching appropriate timer 搜索的相关信息
 *
 * \return AW_OK or AW_ERROR
 */
aw_local aw_err_t __awbl_timer_find (struct awbl_dev          *p_dev,   
                                     struct timer_search_info *p_info)   
{
    /* to hold the Method retrieved from the AWBus device */
    pfunc_timer_func_get_t  pfunc_timer;

    struct awbl_timer *p_candidate    = NULL;  /* timer related data */
    
    /* timer device number，初始化时，这个为0 */
    int                timer_no       = 0; 

    /* a timer is selected or not */
    int                timer_selected = AW_FALSE; 


    /* check for the validity of the function parameters */
    if ((p_dev == NULL) || (p_info == NULL)) {
        return (AW_ERROR);
    }

    /* 已经发现了一个相关定时器了，并且是最佳的，不需要继续寻找 */
    /* if a timer is identified and no better timer is required, return */
    if ((p_info->p_inst != NULL) && (! p_info->better_timer_desired)) {
        return AW_OK;
    }

    /* retrieve the method supported by a timer */ 
    /* 利用方法ID获取服务函数 */
    pfunc_timer = (pfunc_timer_func_get_t)awbl_dev_method_get(p_dev,
                    AWBL_METHOD_CALL(awbl_timer_func_get));      

    /* pfunc_timer is invalid if this is not a timer device */
    if (pfunc_timer == NULL) {
        return AW_OK;
    }

    /*
     * the following loop runs until the function returns an ERROR or
     * the best timer is not selected.
     */
    p_candidate = NULL;
    
    /* 调用方法函数，p_candidata作为函数返回参数 */
    /* 得到的是根据p_dev得到的 awbl_timer 指针，由此得到了服务函数和相关信息 */
    /* awbl_timer 指针 放在p_candidate 中 */
    
    /* 该设备下找到一个定时器 */
    while (((pfunc_timer(p_dev, &p_candidate, timer_no)) == AW_OK) &&      
           ((p_info->p_inst == NULL) || (p_info->better_timer_desired))) {

        /* reset the timer selected flag */
        timer_selected = AW_FALSE;

        /* check if the candidate is valid and the timer is not allocated */
        if ((p_candidate != NULL) && (! p_candidate->allocated) &&
             (! p_candidate->alloc_by_name)) {

            /* check if the timer supports the requested features */
            /* 该定时器支持所要获得的定时器要求 */
            if ((p_candidate->p_param->features & p_info->features) ==
                p_info->features) {
                /*
                 * check if the timer's frequency is the same as the requested
                 * frequency or if the timer's min_frequency and max_frequency
                 * lie within the requested min and max frequencies
                 */
                const struct awbl_timer_param *p_param = p_candidate->p_param;
                if (p_param->clk_frequency != p_info->freq) {

                    if ((p_candidate->p_param->min_frequency <= p_info->min_freq) &&
                        (p_candidate->p_param->max_frequency >= p_info->max_freq)) {
                        
                        /* 标志可能还有更好的定时器 */
                        p_info->better_timer_desired = AW_TRUE;   
                        timer_selected               = AW_TRUE;
                    }

                } else {
                    
                    /* 不需要更好的定时器了   */
                    p_info->better_timer_desired = AW_FALSE;      
                    timer_selected               = AW_TRUE;
                }

                /* if this timer is selected, update the p_info structure */
                if (timer_selected ) {
                    p_info->p_inst   = p_dev;         /* 记录下该设备     */
                    p_info->timer_no = timer_no;      /* 记录下定时器序号 */
                    p_info->p_timer  = p_candidate;   /* 保存awbl_timer结构体 */
                }
            }
        }

        timer_no++;
    }

    return AW_OK;
}

/**
 * \brief finds the appropriate timer according device name and device uint
 *
 * The function selects the appropriate timer based on the frequency and the
 * timer features. If the requested frequency is not supported, then a timer
 * which has its min and max frequencies between the requested
 * 'min_freq' and 'max_freq' values will be selected.
 * desired is used.
 *
 * \param p_dev     AWBux device
 * \param p_info    information for searching appropriate timer
 *
 * \return AW_OK or AW_ERROR
 */
aw_err_t awbl_timer_find_byname (struct awbl_dev          *p_dev,     
                                 struct timer_search_info_byname *p_info)    
{
    /* to hold the Method retrieved from the AWBus device */
    pfunc_timer_func_get_t  pfunc_timer;

    struct awbl_timer *p_candidate    = NULL;  /* timer related data */
   
    /* check for the validity of the function parameters */
    if ((p_dev == NULL) || (p_info == NULL)) {
        return (AW_ERROR);
    }

    /* 已经发现了一个相关定时器了，不需要继续寻找 */
    /* if a timer is identified and no better timer is required, return */
    if (p_info->p_inst != NULL) {
        return AW_OK;
    }
 
    /* retrieve the method supported by a timer */
    /* 利用方法ID获取服务函数 */
    pfunc_timer = (pfunc_timer_func_get_t)awbl_dev_method_get(p_dev,
                    AWBL_METHOD_CALL(awbl_timer_func_get));       

    /* pfunc_timer is invalid if this is not a timer device */
    if (pfunc_timer == NULL) {
        return AW_OK;
    }

    /** check the timer device's name and uint */
    if ((strcmp(awbl_dev_name_get(p_dev),p_info->pname) != 0) ||   
          awbl_dev_uint_get(p_dev) != p_info->uint ) {
              
        return AW_OK;
              
    }
    
    /*
     * the following loop runs until the function returns an ERROR or
     * the best timer is not selected.
     */
    p_candidate = NULL;
    
    /* 调用方法函数，p_candidata作为函数返回参数 */
    /* 得到的是根据p_dev得到的 awbl_timer 指针，由此得到了服务函数和相关信息 */
    /* awbl_timer 指针 放在p_candidate 中 */
    /** 指定timer no分配*/
    if (((pfunc_timer(p_dev, &p_candidate, p_info->timer_no)) == AW_OK) &&       
           (p_info->p_inst == NULL)) {
              
        /* check if the candidate is valid and the timer is not allocated */
        /* and check the timer the alloc_by name filed */
        if ((p_candidate != NULL) && (! p_candidate->allocated) &&
             (p_candidate->alloc_by_name )) {
                               
            /* check if the timer supports the requested features */
            /* 该定时器支持所要获得的定时器要求 */             
            p_info->p_inst   = p_dev;            /* 记录下该设备         */
            p_info->p_timer  = p_candidate;      /* 保存awbl_timer结构体 */
                           
        }
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_hwtimer_features_get (aw_hwtimer_handle_t timer,
                                  uint32_t           *p_min_freq,
                                  uint32_t           *p_max_freq,
                                  uint32_t           *p_features)
{
    struct awbl_timer *p_timer = to_awbl_timer(timer);

    /* if handle is invalid, return ERROR */
    if (p_timer == NULL) {
        return -AW_EINVAL;
    }

    /** the timer have not be allocated,can't be enable */
    if (!p_timer->allocated ) {
        return -AW_EPERM;
    }
    
    if (p_min_freq != NULL) {
        *p_min_freq = p_timer->p_param->min_frequency;
    }
    if (p_max_freq != NULL) {
        *p_max_freq = p_timer->p_param->max_frequency;
    }
    if (p_features != NULL) {
        *p_features = p_timer->p_param->features;
    }

    return AW_OK;
}

aw_hwtimer_handle_t aw_hwtimer_alloc_byname (const char *pname,
                                              uint8_t  uint,  
                                              uint8_t  time_no,
                                              void    (*pfunc_isr) (void *p_arg),
                                              void     *p_arg)
{
    
    /* 用于搜索设备的结构体，包含搜索相关信息 */
    struct timer_search_info_byname  timer_info;    
    void                              *p_drv = NULL;
    struct awbl_timer                 *p_timer;
    const struct awbl_timer_functions *p_func;

    /* initialize the timer details structure */
    /* 清零结构体元素 */
    memset(&timer_info, 0, sizeof(struct timer_search_info_byname));   

    timer_info.pname = pname;
    timer_info.uint  = uint;
    timer_info.timer_no = time_no;
    
    /*
     * Iterates through all the available instances and finds the best
     * available timer with the requested characteristics
     */
    
    /* 遍历所有设备，每个设备均调用函数__awbl_timer_find， 参数 timer_info */
    /* 寻找到合适的设备 */
    awbl_dev_iterate(
    
        /* find函数在一个设备下只寻找一次 */
        (awbl_iterate_func_t)awbl_timer_find_byname,    
         &timer_info,         /* 可能有多个设备        */
         AWBL_ITERATE_INSTANCES);

    /* if no timer is selected, return ERROR */
    if (timer_info.p_inst == NULL) {
        return (NULL);
    }
    

    /* allocate the timer */
    p_timer = timer_info.p_timer;
    p_func  = p_timer->p_func;              /* 相关的服务函数 */
    
    if (NULL == p_func->pfunc_allocate) {
        return  NULL;
    }

    /* 
     *是否成功分配一个定时器，timer_no必须为0 ？ 
     *通过p_drv返回一个 p_lpctimer 指针
     */
    /** 指定名字分配，默认具有AW_HWTIMER_AUTO_RELOAD的特性 */
    if ((p_func->pfunc_allocate(timer_info.p_inst,
                                (AW_HWTIMER_AUTO_RELOAD),
                                &p_drv,
                                timer_info.timer_no) != AW_OK) ||
        (p_drv == NULL)) {

        return (NULL);
    }

    /* set the ISR function */
    if ((NULL != p_func->pfunc_release) && (NULL != p_func->pfunc_isr_set)) {
        if (p_func->pfunc_isr_set(p_drv, pfunc_isr, p_arg) != AW_OK) { 
        
            /* 填充中断函数 */
            p_func->pfunc_release(p_drv);
            return (NULL);
        }
    }

    p_timer->p_drv  = p_drv;

    return p_timer;   
}

/******************************************************************************/
aw_hwtimer_handle_t aw_hwtimer_alloc (uint32_t  freq,
                                      uint32_t  min_freq,
                                      uint32_t  max_freq,
                                      uint32_t  features,
                                      void    (*pfunc_isr) (void *p_arg),
                                      void     *p_arg)
{
    
    /* 用于搜索设备的结构体，包含搜索相关信息 */
    struct timer_search_info           timer_info;    
    void                              *p_drv = NULL;
    struct awbl_timer                 *p_timer;
    const struct awbl_timer_functions *p_func;
  
    /* initialize the timer details structure */
    memset(&timer_info, 0, sizeof(struct timer_search_info)); /* 清零结构体元素 */

    /* Initialize the timer details based on input requirment */
    timer_info.freq     = freq;
    timer_info.min_freq = min_freq;
    timer_info.max_freq = max_freq;
    timer_info.features = features;

    /*
     * Iterates through all the available instances and finds the best
     * available timer with the requested characteristics
     */
    
    /* 遍历所有设备，每个设备均调用函数__awbl_timer_find， 参数 timer_info */
    /* 寻找到合适的设备 */
    awbl_dev_iterate(
    
        /* find函数在一个设备下只寻找一次 */
        (awbl_iterate_func_t)__awbl_timer_find,  
        &timer_info,                             /* 可能有多个设备 */
         AWBL_ITERATE_INSTANCES);

    /* if no timer is selected, return ERROR */
    if (timer_info.p_inst == NULL) {
        return (NULL);
    }

    /* allocate the timer */
    p_timer = timer_info.p_timer;
    p_func  = p_timer->p_func;              /* 相关的服务函数 */

    if (NULL == p_func->pfunc_allocate) {
        return  NULL;
    }

    /* 
     *是否成功分配一个定时器，timer_no必须为0 ？ 
     *通过p_drv返回一个 p_lpctimer 指针 
     */
    if ((p_func->pfunc_allocate(timer_info.p_inst,
                                (features & AW_HWTIMER_AUTO_RELOAD),
                                &p_drv,
                                timer_info.timer_no) != AW_OK) ||
        (p_drv == NULL)) {
        return (NULL);
    }

    if ((NULL != p_func->pfunc_release) && (NULL != p_func->pfunc_isr_set)) {
        /* set the ISR function */
        if (p_func->pfunc_isr_set(p_drv, pfunc_isr, p_arg) != AW_OK) {
            
            /* 填充中断函数 */
            p_func->pfunc_release(p_drv);
            return (NULL);
        }
    }

    p_timer->p_drv  = p_drv;

    return p_timer;
}

/******************************************************************************/
aw_err_t aw_hwtimer_release (aw_hwtimer_handle_t timer)
{
    struct awbl_timer *p_timer = to_awbl_timer(timer);

    if (p_timer == NULL) {
        return -AW_EINVAL;
    }

    if (NULL == p_timer->p_func->pfunc_release) {
        return  -AW_ENOTSUP;
    }

    /* call the timer release function pointer */
    return p_timer->p_func->pfunc_release(p_timer->p_drv);
}

/******************************************************************************/
aw_err_t aw_hwtimer_enable (aw_hwtimer_handle_t timer, uint32_t frequency_hz)
{
    struct awbl_timer             *p_timer = to_awbl_timer(timer);
    uint32_t                       max_timer_count;
    aw_err_t                       err;

    if (p_timer == NULL) {
        return -AW_EINVAL;
    }

    if (NULL == p_timer->p_func->pfunc_enable) {
        return  -AW_ENOTSUP;
    }

    if ((frequency_hz < p_timer->p_param->min_frequency) ||
        (frequency_hz > p_timer->p_param->max_frequency)) {
        return -AW_EINVAL;
    }

    max_timer_count = p_timer->p_param->clk_frequency / frequency_hz;
    
    err = p_timer->p_func->pfunc_enable(p_timer->p_drv, max_timer_count);
    
    if (AW_OK == err) {
        /* update ticks_per_second */
        p_timer->ticks_per_second = p_timer->p_param->clk_frequency
                                  / max_timer_count;
    }
    return err;
}

/******************************************************************************/
aw_err_t aw_hwtimer_enable64 (aw_hwtimer_handle_t timer, uint64_t frequency_hz)
{
    struct awbl_timer             *p_timer = to_awbl_timer(timer);
 
    uint64_t                       max_timer_count;
    
    aw_err_t                       err = AW_ERROR;

    if (p_timer == NULL) {
        return -AW_EINVAL;
    }
      
    if ((frequency_hz < p_timer->p_param->min_frequency) ||
        (frequency_hz > p_timer->p_param->max_frequency)) {
        return -AW_EINVAL;
    }

    max_timer_count = p_timer->p_param->clk_frequency / frequency_hz;
    
    if (p_timer->p_func->pfunc_enable64 != NULL) {
        err = p_timer->p_func->pfunc_enable64(p_timer->p_drv, max_timer_count);
    } else if (p_timer->p_func->pfunc_enable != NULL) {
        err = p_timer->p_func->pfunc_enable(p_timer->p_drv, 
                                            (uint32_t)max_timer_count);
    } else {
        return  -AW_ENOTSUP;
    }
    
    if (AW_OK == err) {
        /* update ticks_per_second */
        p_timer->ticks_per_second = p_timer->p_param->clk_frequency
                                  / max_timer_count;
    }
    return err;
}

/******************************************************************************/
aw_err_t aw_hwtimer_disable (aw_hwtimer_handle_t timer)
{
    struct awbl_timer *p_timer = to_awbl_timer(timer);

    if (p_timer == NULL) {
        return -AW_EINVAL;
    }

    if (NULL == p_timer->p_func->pfunc_disable) {
        return  -AW_ENOTSUP;
    }

    return p_timer->p_func->pfunc_disable(p_timer->p_drv);
}

/******************************************************************************/
aw_err_t aw_hwtimer_count_get (aw_hwtimer_handle_t timer, uint32_t *p_count)
{
    struct awbl_timer *p_timer = to_awbl_timer(timer);

    if (p_timer == NULL) {
        return -AW_EINVAL;
    }

    if (NULL == p_timer->p_func->pfunc_count_get) {
        return  -AW_ENOTSUP;
    }

    return p_timer->p_func->pfunc_count_get(p_timer->p_drv, p_count);
}

/******************************************************************************/
aw_err_t aw_hwtimer_count_get64 (aw_hwtimer_handle_t timer, uint64_t *p_count)
{
    struct awbl_timer *p_timer = to_awbl_timer(timer);

    if (p_timer == NULL) {
        return -AW_EINVAL;
    }

    if (NULL == p_timer->p_func->pfunc_count_get64) {
        return  -AW_ENOTSUP;
    }
     
    return p_timer->p_func->pfunc_count_get64(p_timer->p_drv, p_count);
}

/******************************************************************************/
aw_err_t aw_hwtimer_rollover_get (aw_hwtimer_handle_t timer, uint32_t *p_count)
{
    struct awbl_timer *p_timer = to_awbl_timer(timer);

    if (p_timer == NULL) {
        return -AW_EINVAL;
    }

    if (NULL == p_timer->p_func->pfunc_rollover_get) {
        return  -AW_ENOTSUP;
    }

    return p_timer->p_func->pfunc_rollover_get(p_timer->p_drv, p_count);
}


aw_err_t aw_hwtimer_clk_freq_get (aw_hwtimer_handle_t timer,
                                  uint32_t           *p_clk_freq)
{
    struct awbl_timer *p_timer = to_awbl_timer(timer);

    if (p_timer == NULL) {
        return -AW_EINVAL;
    }

    *p_clk_freq = p_timer->p_param->clk_frequency;
    return AW_OK;
}
/******************************************************************************/
aw_err_t aw_hwtimer_rollover_get64 (aw_hwtimer_handle_t timer, uint64_t *p_count)
{
    struct awbl_timer *p_timer = to_awbl_timer(timer);

    if (p_timer == NULL) {
        return -AW_EINVAL;
    }

    if (NULL == p_timer->p_func->pfunc_rollover_get64) {
        return  -AW_ENOTSUP;
    }

    return p_timer->p_func->pfunc_rollover_get64(p_timer->p_drv, p_count);
}

uint32_t aw_hwtimer_ms_to_freq (uint32_t ms)
{
     if (ms == 0) {
         return 0xFFFFFFFFul;    /* 最大的频率值，通常是一个无效的值 */
     }
     
     return 1000 / ms; 
}


uint32_t aw_hwtimer_us_to_freq (uint32_t us)
{
     if (us == 0) {
         return 0xFFFFFFFFul;     /* 最大的频率值，通常是一个无效的值 */
     }
     
     return 1000000ul / us; 
}

/* end of file */
