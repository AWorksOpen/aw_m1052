/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2014 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief System initialization configure
 *
 *
 * \internal
 * \par Modification history
 * - 1.00 2017-07-10  sni, first implementation.
 * \endinternal
 */
#include "aworks.h"
#include "string.h"

#ifndef __CC_ARM
extern unsigned int relocode_origin;
extern unsigned int relocode_start;
extern unsigned int relocode_end;
#define RELODE_ORIGIN_ADDR ((uint32_t)&relocode_origin)
#define RELODE_START_ADDR  ((uint32_t)&relocode_start)
#define RELODE_END_ADDR    ((uint32_t)&relocode_end)
#endif

extern unsigned int aw_const g_pfnVectors;

/**
 * \brief The SystemInit() called after reset.
 */
void SystemInit()
{
    volatile unsigned int *p ;

    p = (volatile unsigned int *)0x400bc004;
    *p = 0xB480A602;
    p = (volatile unsigned int *)0x400bc004;
    *p = 0xD928C520; //unlock watchdog
    p = (volatile unsigned int *)0x400bc008;
    * p = 0xFFFF;
    p = (volatile unsigned int *)0x400bc000;
    *p = ((*p ) & ~(1<<7)) | (1<< 5);
    
    *((uint32_t*)0xE000ED08) = (uint32_t)&g_pfnVectors;

#ifndef __CC_ARM
    /* reload attribute to section ".ram.func" */
    memcpy((void *)&relocode_start,
           (void *)&relocode_origin,
           ((uint32_t)&relocode_end - (uint32_t)&relocode_start));

    aw_arm_barrier_dsb();
#endif
}

/** 
 * \brief System initialization
 * The function should be called in aw_bsp_early_init()
 */
void aw_bsp_system_init (void)
{
    return;
}

/* end of file */
