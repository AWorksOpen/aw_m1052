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
 * \brief ARMv8-M mpu
 *
 * \internal
 * \par modification history:
 * - 1.00 2019-08-15, A.J, first implemetation.
 * \endinternal
 */

#include "arch/arm/armv8-m/armv8m_mpu.h"
#include "rtk.h"
#include "string.h"
#include "aw_io.h"

/* 定义MPU操作结构体 */
typedef struct
{
    volatile uint32_t TYPE;         /**< \brief Offset: 0x000 (R/ )  MPU Type Register                              */
    volatile uint32_t CTRL;         /**< \brief Offset: 0x004 (R/W)  MPU Control Register                           */
    volatile uint32_t RNR;          /**< \brief Offset: 0x008 (R/W)  MPU Region RNRber Register                     */
    volatile uint32_t RBAR;         /**< \brief Offset: 0x00C (R/W)  MPU Region Base Address Register               */
    volatile uint32_t RLAR;         /**< \brief Offset: 0x010 (R/W)  MPU Region Limit Address Register              */
    volatile uint32_t RBAR_A1;      /**< \brief Offset: 0x014 (R/W)  MPU Alias 1 Region Base Address Register       */
    volatile uint32_t RLAR_A1;      /**< \brief Offset: 0x018 (R/W)  MPU Alias 1 Region Limit Address Register      */
    volatile uint32_t RBAR_A2;      /**< \brief Offset: 0x01C (R/W)  MPU Alias 2 Region Base Address Register       */
    volatile uint32_t RLAR_A2;      /**< \brief Offset: 0x020 (R/W)  MPU Alias 2 Region Limit Address Register      */
    volatile uint32_t RBAR_A3;      /**< \brief Offset: 0x024 (R/W)  MPU Alias 3 Region Base Address Register       */
    volatile uint32_t RLAR_A3;      /**< \brief Offset: 0x028 (R/W)  MPU Alias 3 Region Limit Address Register      */
    volatile uint32_t RESERVED;
    volatile uint32_t MAIR0;        /**< \brief Offset: 0x030 (R/W)  MPU Memory Attribute Indirection Registers 0   */
    volatile uint32_t MAIR1;        /**< \brief Offset: 0x030 (R/W)  MPU Memory Attribute Indirection Registers 1   */
} MPU_Type;

#define MPU_BASE                0xe000ed90u /**< \brief Memory Protection Unit             */
#define MPU                     (MPU_Type *)MPU_BASE;
#define ARMV8M_MPU_ALIGN_LEN    32

#define FATAL_ERROR()           while(1)

static const uint8_t __g_mem_attri_map[] = {
    0x00, /* Device-nGnRnE */
    0x04, /* Device-nGnRE */
    0x22, /* Outer/Inner write-through transient, read allocate, write not allocate */
    0x66, /* Outer/Inner write-back transient, read allocate, write not allocate */
    0x44, /* Outer/Inner non-cacheable */
    0x77, /* Outer/Inner write-back transient, read allocate, write allocate */
};

/* 对输入的地址,进行32字节的向下对齐 */
static uint32_t align_mpuaddr_floor(uint32_t addr)
{
    uint32_t align_addr;

    align_addr = addr;
    if (addr % ARMV8M_MPU_ALIGN_LEN != 0) {
        align_addr = addr/ARMV8M_MPU_ALIGN_LEN * ARMV8M_MPU_ALIGN_LEN;
    }

    return align_addr;
}

/* 对输入的地址,进行32字节的向上对齐 */
static uint32_t align_mpuaddr_ceil(uint32_t addr)
{
    uint32_t align_addr;

    align_addr = addr;
    align_addr = (addr + ARMV8M_MPU_ALIGN_LEN) / ARMV8M_MPU_ALIGN_LEN * ARMV8M_MPU_ALIGN_LEN;

    return align_addr;
}

union mpu_mair_t
{
    uint8_t attri[8];
    struct
    {
        uint32_t mair0;
        uint32_t mair1;
    };
};

static int mpu_get_max_region_count(MPU_Type *p_mpu)
{
    return (p_mpu->TYPE >> 8) & 0xFF ;
}

void __armv8m_mpu_enable(void);
void __armv8m_mpu_disable(void);

void aw_hw_mpu_init (const struct mpu_region_desc *p_desc,
                     uint32_t desc_count)
{
    MPU_Type               *p_mpu;
    uint32_t                i;
    uint32_t                RNR, RBAR, RLAR, MAIR0, MAIR1, mem_type;
    int                     max_region_count;
    union mpu_mair_t        mpu_mair;
    uint32_t                addr;

    p_mpu = (MPU_Type *)MPU_BASE;

    /* check if the mpu is present */
    max_region_count = mpu_get_max_region_count(p_mpu);
    if (0 == max_region_count) {
        FATAL_ERROR();
    }

    /* check parameters */
    if (NULL == p_desc || 0 == desc_count || max_region_count < desc_count) {
        FATAL_ERROR();
    }

#if 0
    /* 检查，避免区域交叉 */
    /* 虽然MPU运行区域交叉，但是区域交叉可能会引发未意料的后果，所以这里做出提醒 */
    for (i = 0; i < desc_count ;i++) {
        for (j = i+1;j < desc_count;j++) {
            if (address_aligned[i] < address_aligned[j] ) {
                addr = address_aligned[i];
                len = len_aligned[i];
                addr1 = address_aligned[j];
            }
            else {
                addr = address_aligned[j];
                len = len_aligned[j];
                addr1 = address_aligned[i];
            }
            if (addr + len > addr1) {
                /* 区域交叉，严重错误 */
                FATAL_ERROR();
            }

        }
    }
#endif

    __armv8m_mpu_disable();

    /* 开始配置 */
    for (i=0; i<desc_count; i++) {
        RNR = i;

        addr = align_mpuaddr_floor(p_desc[i].start);
        RBAR = (addr&0xffffffe0) | (p_desc[i].shareable<<3) | (p_desc[i].access<<1) | p_desc[i].executable;

        /* Limit address: This value is postfixed with 0x1F to provide the limit address to be checked against. */
        addr = align_mpuaddr_ceil(p_desc[i].end);
        if (addr != 0) {
            addr--;
        }
        RLAR = (addr&0xffffffe0) | (i<<1) | (1<<0);

        mpu_mair.attri[i] = __g_mem_attri_map[p_desc[i].mem_type];

        writel(RNR, &p_mpu->RNR);
        writel(RBAR, &p_mpu->RBAR);
        writel(RLAR, &p_mpu->RLAR);
    }

    MAIR0 = mpu_mair.mair0;
    MAIR1 = mpu_mair.mair1;

    writel(MAIR0, &p_mpu->MAIR0);
    writel(MAIR1, &p_mpu->MAIR1);

    for (i=desc_count; i<max_region_count; i++) {
        writel(i, &p_mpu->RNR);
        writel(0, &p_mpu->RLAR);
    }

    __armv8m_mpu_enable();
}
