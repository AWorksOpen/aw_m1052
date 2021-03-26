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

#ifndef __AWBL_TIMER_H
#define __AWBL_TIMER_H

#include "aw_common.h"
#include "awbus_lite.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup awbl_timer
 * @{
 */

/** \brief timer functions */
struct awbl_timer_functions {

    /** \brief allocate dedicated timer */
    aw_err_t (*pfunc_allocate) (struct awbl_dev *p_dev,
                                uint32_t         flags,
                                void           **pp_drv,
                                int              timer_no);

    /** \brief release dedicated timer */
    aw_err_t (*pfunc_release) (void *p_drv);

    /** \brief get count between rollover/ISR events */
    aw_err_t (*pfunc_rollover_get) ( void *p_drv, uint32_t *p_count);

    /**
     * \brief get current time stamp timer count value.
     * The driver should ensure returning the count as that of an up counter.
     */
    aw_err_t (*pfunc_count_get) (void *p_drv, uint32_t *p_count);

    /** \brief disable the timer */
    aw_err_t (*pfunc_disable) (void *p_drv);

    /** \brief enable the timer */
    aw_err_t (*pfunc_enable) (void *p_drv, uint32_t max_timer_count);

    /** \brief set the function to be called on every interrupt */
    aw_err_t (*pfunc_isr_set) (void  *p_drv,
                              void  (*pfunc_isr) (void *p_arg),
                              void   *p_arg);

    /** \brief enable the 64 bits timer */
    aw_err_t (*pfunc_enable64) (void *p_drv, uint64_t maxTimerCount);

    /** \brief get 64 bits count between rollover/ISR events */
    aw_err_t (*pfunc_rollover_get64) (void *p_drv, uint64_t *p_count);

    /**
     * \brief
     * get current time stamp timer 64 bits count value.
     * The driver should ensure returning the count as that of an up counter.
     */
    aw_err_t (*pfunc_count_get64) (void *p_drv, uint64_t *p_count);
};

/** \brief timer fixed parameter */
struct awbl_timer_param {
    uint32_t    clk_frequency;      /**< \brief clock frequency              */
    uint32_t    min_frequency;      /**< \brief min period timer can provide */
    uint32_t    max_frequency;      /**< \brief max period timer can provide */
    uint32_t    features;           /**< \brief features of timer */    
};

/** \brief Timer type for AWBus */
struct awbl_timer {
    
    uint8_t    allocated;         /**< \brief denoting if timer allocated */
    uint8_t    alloc_by_name;     /**< \brief true if the timer alloc by name */
    uint32_t   ticks_per_second;  /**< \brief ticks per second */  
    void      *p_drv;             /**< \brief driver private data */
  
    /** \brief timer operating functions */
    const struct awbl_timer_functions *p_func;

    /** \brief timer parameter */
    struct awbl_timer_param *p_param;
    
};


/** \brief get timer's name */
#define awbl_dev_name_get(p_dev)      AWBL_DEV_NAME_GET(p_dev)

/** \brief get timer's uint */
#define awbl_dev_uint_get(p_dev)      AWBL_DEV_UNIT_GET(p_dev)

 
/******************************************************************************/

/** \brief timer method awbl_timer_func_get return this type */
typedef aw_err_t (*pfunc_timer_func_get_t) (struct awbl_dev    *p_dev,
                                            struct awbl_timer **pp_timer,
                                            int                 timer_no);

/** \brief structure used for timer finding by name */
struct timer_search_info_byname {
    struct awbl_dev    *p_inst;     /**< \brief timer instance selected */
    uint32_t            timer_no;   /**< \brief timer no of selected timer */
    struct awbl_timer  *p_timer;    /**< \brief timer functionality */
    const char          *pname;     /**< \brief name of timer to find */
    uint8_t              uint;      /**< \brief uint of timer to find */
};
                                            
aw_err_t awbl_timer_find_byname (struct awbl_dev          *p_dev,     
                                 struct timer_search_info_byname *p_info);                                             
                                            
/** \brief import timer method */
AWBL_METHOD_IMPORT(awbl_timer_func_get);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_TIMER_H */

/* end of file */
