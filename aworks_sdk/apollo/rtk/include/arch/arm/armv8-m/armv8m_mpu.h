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
 * \brief MPU function for armv8m arch.
 *
 * \internal
 * \par History
 * - 1.00 2019-08-15, A.J, first implemetation.
 * \endinternal
 */

#ifndef __ARMV8M_MPU_H /* { */
#define __ARMV8M_MPU_H

#include <aw_types.h>

//! @brief Memory region attributes.
typedef enum
{
    kDevice_nGnRnE = 0x00,      /**< \brief ��Ҫ�ϸ���˳����ʵ����� */
    kDevice_nGnRE,              /**< \brief ��������� */
    kOuterInner_WT_NOWA,        /**< \brief дͨ��cache,��write allocate */
    kOuterInner_WB_NOWA,        /**< \brief д����cache,��write allocate */
    kNoncacheable,              /**< \brief ����cache */
    kOuterInner_WB_WA,          /**< \brief д����cache,��write allocate */
} mpu_memory_type_t;

/** \brief Memory region shareability options.*/
typedef enum
{
    kNonshareable   = 0x00, /**< \brief Non-shareable */
    kOuterShareable = 0x02, /**< \brief Outer shareable */
    kInnerShareable = 0x03, /**< \brief Inner Shareable */
} mpu_shareability_t;

/** \brief @brief Access permissions for a memory region.*/
typedef enum
{
    kPrivilegedRWAccess = 0x00, /**< \brief Read/write by privileged code only */
    kRWAccess           = 0x01, /**< \brief Read/write by any privilege level */
    kPrivilegedROAccess = 0x02, /**< \brief Read-only by privileged code only */
    kROAccess           = 0x03, /**< \brief Read-only by any privilege level */
} mpu_access_t;

/** \brief Memory region shareability options.*/
typedef enum
{
    kExecutable     = 0x00, /**< \brief Execution only permitted if read permitted */
    kNonExecutable  = 0x01, /**< \brief Execution not permitted */
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
void aw_hw_mpu_init (const struct mpu_region_desc *p_desc,
                     uint32_t desc_count);

#endif /* } __ARMV8M_MPU_H */

/* end of file */
