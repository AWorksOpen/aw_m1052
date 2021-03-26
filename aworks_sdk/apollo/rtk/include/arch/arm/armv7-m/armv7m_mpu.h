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
 * \brief MPU function for armv7m arch.
 *
 * \internal
 * \par History
 * - 1.00 17-07-11  sni, first implementation.
 * \endinternal
 */

#ifndef __ARMV7M_MPU_H /* { */
#define __ARMV7M_MPU_H

#include <aw_types.h>

//! @brief Memory region attributes.
typedef enum
{
    kStronglyOrdered = 0,       /**< \brief 需要严格按照顺序访问的区域 */
    kDevice,                    /**< \brief 共享的外设 */
    kOuterInner_WT_NOWA,        /**< \brief 写通型cache,无write allocate */
    kOuterInner_WB_NOWA,        /**< \brief 写回型cache,无write allocate */
    kNoncacheable,              /**< \brief 不带cache */
    kOuterInner_WB_WA,          /**< \brief 写回型cache,有write allocate */
} mpu_memory_type_t;

/** \brief Memory region shareability options.*/
typedef enum
{
    kShareable = 1,
    kNonshareable = 0
} mpu_shareability_t;

/** \brief @brief Access permissions for a memory region.*/
typedef enum
{
    kNoAccess = 0x0,
    kPrivilegedRWAccess = 0x1,
    kUnPrivilegedROAccess = 0x2,
    kRWAccess = 0x3,
    kPrivilegedROAccess = 0x5,
    kROAccess = 0x6,
} mpu_access_t;

/** \brief Memory region shareability options.*/
typedef enum
{
    kExecutable = 0,
    kNonExecutable = 1,
} mpu_executable_t;

/******************************************************************************/

struct mpu_region_desc {
    uint32_t            start;
    uint32_t            end;

    mpu_memory_type_t   mem_type;
    mpu_shareability_t  shareable;
    mpu_access_t        access;
    mpu_executable_t    executable;
};

/******************************************************************************/
extern void arch_mpu_disable (void);

/******************************************************************************/
void aw_hw_mpu_init (const struct mpu_region_desc *p_desc,
                     uint32_t desc_count);

#endif /* } __ARMV4_MMU_H */

/* end of file */
