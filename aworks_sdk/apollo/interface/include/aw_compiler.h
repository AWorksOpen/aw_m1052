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
 * \brief 编译器相关定义头文件
 *
 * \internal
 * \par modification history:
 * - 1.10 14-09-28	zen, move some defines to aw_psp_complier.h
 * - 1.01 14-08-12  orz, add define of restrict
 * - 1.00 13-01-15  orz, move defines from aw_common.h
 * \endinternal
 */

#ifndef __AW_COMPILER /* { */
#define __AW_COMPILER

#include "aw_psp_compiler.h"


#define aw_inline           aw_psp_inline         /** \brief inline 定义  */
#define aw_static_inline    aw_psp_static_inline  /** \biref static inline 定义*/

#define aw_section(x)       aw_psp_section(x)     /** \brief __section() 定义 */

#define aw_inline           aw_psp_inline
#define aw_static_inline    aw_psp_static_inline

#define aw_local            aw_psp_local
#define aw_const            aw_psp_const
#define aw_import           aw_psp_import
#define aw_export           aw_psp_export

#include "aw_barrier.h"

static void aw_inline __write_once_8(uint8_t *p_addr,uint8_t data)
{
    *(volatile uint8_t *)p_addr = data;
}

static void aw_inline __write_once_16(uint16_t *p_addr,uint16_t data)
{
    *(volatile uint16_t *)p_addr = data;
}

static void aw_inline __write_once_32(uint32_t *p_addr,uint32_t data)
{
    *(volatile uint32_t *)p_addr = data;
}

static uint8_t aw_inline __read_once_8(uint8_t *p_addr)
{
    register uint8_t        data;

    data = *((volatile uint8_t *)p_addr);
    aw_smp_read_barrier_depends();
    return data;
}

static uint16_t aw_inline __read_once_16(uint16_t *p_addr)
{
    register uint16_t       data;

    data = *((volatile uint16_t *)p_addr);
    aw_smp_read_barrier_depends();
    return data;
}

static uint32_t aw_inline __read_once_32(uint32_t *p_addr)
{
    register uint32_t       data;

    data = *((volatile uint32_t *)p_addr);
    aw_smp_read_barrier_depends();
    return data;
}

/*
 * READ_ONCE和WRITE_ONCE系列接口，用于阻止编译器优化，具体来说，就是阻止编译器合
 * 并读写，多次读，和读写重排序。但是阻止读写重排序只有在编译器意识到有特殊的序
 * 列需求的时候，一种让编译器意识到排序的方法是在C代码中嵌入多个READ_ONCE和WRITE_ONCE
 *
 * 这些接口的主要用途有:调解任务代码和ISR代码之间的通信（任务代码和ISR都运行在
 * 同一个CPU上）;保证编译器不增加范围访问，撕裂访问或者增加减少访问次数
 *
 * 总而言之，这些宏从某种程度上可以替代volatile关键字，并且实际上被推荐作为代码中
 * 减少volatile使用的方法。使用这些宏，可以让程序员更加清晰地认识到自己的变量
 * 需要什么程度的保护，例如中断锁，互斥锁，还是不需要锁
 *
 * volatie的使用也许会使得很多程序员在变量是否需要保护的时候放松警惕，认为volatile
 * 就足够了，事实上，相当多的情况下并不足够
 *
 * 这些宏可以在某些不需要锁保护的情况下，帮助利用处理器或编译器的固有特性实现代码
 * 的中断或线程安全
 *
 * READ_ONCE系列函数含有一个aw_smp_read_barrier_depends，具体的原因可参加barrier
 * 相关的文档
 */

/** \brief 编译器角度保证8位写不被撕裂，合并，扩展，重复，删除，乱排序  */
#define WRITE_ONCE8(x, val)     __write_once_8((uint8_t *)&(x),(uint8_t)(val));
/** \brief 编译器角度保证16位写不被撕裂，合并，扩展，重复，删除，乱排序  */
#define WRITE_ONCE16(x, val)    __write_once_16((uint16_t *)&(x),(uint16_t)(val));
/** \brief 编译器角度保证32位写不被撕裂，合并，扩展，重复，删除，乱排序  */
#define WRITE_ONCE32(x, val)    __write_once_32((uint32_t *)&(x),(uint32_t)(val));

/** \brief 编译器角度保证8位读不被撕裂，合并，扩展，重复，删除，乱排序  */
#define READ_ONCE8(x)           __read_once_8((uint8_t *)&(x));
/** \brief 编译器角度保证16位读不被撕裂，合并，扩展，重复，删除，乱排序  */
#define READ_ONCE16(x)          __read_once_16((uint16_t *)&(x));
/** \brief 编译器角度保证16位读不被撕裂，合并，扩展，重复，删除，乱排序  */
#define READ_ONCE32(x)          __read_once_32((uint32_t *)&(x));

#endif /* } __AW_COMPILER */

/* end of file */
