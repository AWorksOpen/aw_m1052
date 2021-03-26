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
 * \brief ARMv7-M mpu
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-08  zen, first implemetation
 * \endinternal
 */

#include "arch/arm/armv7-m/armv7m_mpu.h"
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
    volatile uint32_t RASR;         /**< \brief Offset: 0x010 (R/W)  MPU Region Attribute and Size Register         */
    volatile uint32_t RBAR_A1;      /**< \brief Offset: 0x014 (R/W)  MPU Alias 1 Region Base Address Register       */
    volatile uint32_t RASR_A1;      /**< \brief Offset: 0x018 (R/W)  MPU Alias 1 Region Attribute and Size Register */
    volatile uint32_t RBAR_A2;      /**< \brief Offset: 0x01C (R/W)  MPU Alias 2 Region Base Address Register       */
    volatile uint32_t RASR_A2;      /**< \brief Offset: 0x020 (R/W)  MPU Alias 2 Region Attribute and Size Register */
    volatile uint32_t RBAR_A3;      /**< \brief Offset: 0x024 (R/W)  MPU Alias 3 Region Base Address Register       */
    volatile uint32_t RASR_A3;      /**< \brief Offset: 0x028 (R/W)  MPU Alias 3 Region Attribute and Size Register */
} MPU_Type;

#define MPU_BASE        0xe000ed90u /**< \brief Memory Protection Unit             */
#define MPU             (MPU_Type *)MPU_BASE;

#define FATAL_ERROR()   while(1);

static void cal_aligned_addr_and_length(
        uint32_t addr,
        uint32_t len,
        uint32_t *p_aligned_addr,
        uint32_t *p_aligned_len)
{
    int             t;
    uint32_t        t_len = len,t_addr = addr;
    /* 首先长度必须为2的整数次冥 */
    if ( (len &(len-1)) != 0 ) {
        t = rtk_cpu_ffs(len) + 1 ;
        if (t >31) {
            FATAL_ERROR();
        }
        t_len = 1<< t;
    }
    /* 地址必须能够region容量整除 */
    if (t_addr % t_len != 0) {
        t_addr = t_addr/t_len * t_len;
    }
    while (t_addr + t_len < addr + len) {
        t_len = t_len << 1;
        if (0 == t_len) {
            FATAL_ERROR();
        }
        t_addr = t_addr/t_len * t_len;
    }
    *p_aligned_addr = t_addr;
    *p_aligned_len = t_len;
}

struct mem_type_map
{
    uint8_t         TEX:3;
    uint8_t         C:1;
    uint8_t         B:1;
};

static const struct mem_type_map    __g_mem_type_map[] = {
        /* kStronglyOrdered */
        {
                .TEX = 0,
                .C = 0,
                .B = 0,
        },
        /* kDevice */
        {
                .TEX = 0,
                .C = 0,
                .B = 1,
        },
        /* kOuterInner_WT_NOWA */
        {
                .TEX = 0,
                .C = 1,
                .B = 0,
        },
        /* kOuterInner_WB_NOWA */
        {
                .TEX = 0,
                .C = 1,
                .B = 1,
        },
        /* kNoncacheable */
        {
                .TEX = 1,
                .C = 0,
                .B = 0,
        },
        /* kOuterInner_WB_WA */
        {
                .TEX = 1,
                .C = 1,
                .B = 1,
        }

};

static int mpu_get_max_region_count(MPU_Type *p_mpu)
{
    return (p_mpu->TYPE >> 8) & 0xFF ;
}

void __armv7m_mpu_enable(void);

void aw_hw_mpu_init (const struct mpu_region_desc *p_desc,
                     uint32_t desc_count)
{
    MPU_Type               *p_mpu;
    uint32_t                address_aligned[16];
    uint32_t                len_aligned[16];
    uint32_t                len,addr,sub_region_size;
    uint32_t                i,j;
    uint32_t                RNR,RBAR,RASR,mem_type;
    int                     max_region_count;


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

    for (i = 0;i<desc_count;i++) {
        len = p_desc[i].end - p_desc[i].start + 1;
        if (len < 32) {
            FATAL_ERROR();
        }
    }




    /* 开始计算aligned address和长度 */
    for (i = 0 ; i< desc_count; i++) {
        cal_aligned_addr_and_length(
                p_desc[i].start,
                p_desc[i].end - p_desc[i].start + 1,
                &addr,
                &len);
        address_aligned[i] = addr;
        len_aligned[i] = len;
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
    arch_mpu_disable();
    /* 开始配置 */
    for (i = 0; i< desc_count;i++) {
        addr = address_aligned[i];
        len = len_aligned[i];

        RNR = i;
        RBAR = addr;
        RASR = (rtk_cpu_ffs(len) - 2) << 1;
        RASR |= 1;

        RASR |= (p_desc[i].executable << 28);
        RASR |= (p_desc[i].access << 24);
        RASR |= (p_desc[i].shareable << 18);

        mem_type = p_desc[i].mem_type;
        RASR |= __g_mem_type_map[mem_type].TEX <<19;
        RASR |= __g_mem_type_map[mem_type].C <<17;
        RASR |= __g_mem_type_map[mem_type].B <<16;
        /* 尝试看能否禁用子Region */
        sub_region_size = len / 8;
        for (j= 0 ;j < 8 ;j++) {
            int         invalid;

            invalid =( (addr < p_desc[i].start) &&
                    (addr + sub_region_size <=p_desc[i].start ) ) ||
                            (addr > p_desc[i].end);
            if (invalid) {
                RASR |= (1<<j) << 8;
            }
            addr += sub_region_size;

        }

        writel(RNR,&p_mpu->RNR);
        writel(RBAR,&p_mpu->RBAR);
        writel(RASR,&p_mpu->RASR);
    }

    for (i = desc_count;i<max_region_count;i++) {
        writel(i,&p_mpu->RNR);
        writel(0,&p_mpu->RASR);
    }

    __armv7m_mpu_enable();

}

