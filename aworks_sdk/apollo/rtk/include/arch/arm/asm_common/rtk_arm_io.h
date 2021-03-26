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
 * \file  rtk_arm_io.h
 * \brief arm�й���io�����Ķ���
 *
 * \internal
 * \par modification history:
 * - 1.00 18-11-19  sni, first implement
 * \endinternal
 */
#include "aw_compiler.h"
#include "aw_types.h"

/**
 * \name rtk_io
 * \brief IO���ʶ���
 * @{
 * \par
 * readX/writeX()���ڷ����ڴ汻ӳ����豸����ͬ��ϵ�ṹ��IO���ʷ�ʽ������ͬ��
 * ��һЩ�򵥵���ϵ�ṹ�У���û��MMU��ARM7��cortex-m3�ȣ�����ֱ�ӷ���IO�ռ䡣
 * ����������ϵ�ṹ�У���x86��������MMU��ARM9�ȣ�����ֱ�ӷ���IO�ռ䣬����ʹ��
 * ��Ӧ��ϵ�ṹ�ṩ�ķ������з��ʣ�����������ַת����
 * \par
 * ����ֲ������Ӧ��ʹ��IO�����ӿڽ��мĴ����Ķ�д��������ֱ�Ӷ�IO��ַ���в���
 *
 */

static inline uint8_t __raw_readb(const volatile void *addr)
{
    return *((const volatile uint8_t *) addr);
}

static inline uint16_t __raw_readw(const volatile void *addr)
{
    return *((const volatile uint16_t *) addr);
}

static inline uint32_t __raw_readl(const volatile void *addr)
{
    return *((const volatile uint32_t *) addr);
}

static inline void __raw_writeb(uint8_t val, volatile void *addr)
{
    *((volatile uint8_t *)addr) = val;
}

static inline void __raw_writew(uint16_t val, volatile void *addr)
{
    *((volatile uint16_t *)addr) = val;
}

static inline void __raw_writel(uint32_t val, volatile void *addr)
{
    *((volatile uint32_t *)addr) = val;
}

/* ����iormb��iowmb */
#ifdef CONFIG_RTK_ARM_DMA_MEM_BUFFERABLE
#include <rtk_barrier.h>
#define __iormb()       rtk_barrier()
#define __iowmb()       rtk_arm_barrier_dsb()
#define __iomb()        rtk_arm_barrier_dsb()
#else
#define __iormb()       rtk_barrier()
#define __iowmb()       rtk_barrier()
#define __iomb()        rtk_barrier()
#endif


/* relaxed_io */
#define readb_relaxed(addr)           __raw_readb( ((volatile void *)(addr)) )
#define readw_relaxed(addr)           __raw_readw( ((volatile void *)(addr)) )
#define readl_relaxed(addr)           __raw_readl( ((volatile void *)(addr)) )

#define writeb_relaxed(b, addr)       __raw_writeb(b, ((volatile void *)(addr)) )
#define writew_relaxed(b, addr)       __raw_writew(b, ((volatile void *)(addr)) )
#define writel_relaxed(b, addr)       __raw_writel(b, ((volatile void *)(addr)) )

/**
 * \brief io arm��������IO�ռ䱾����ǿ���л��ģ�Ψһ��Ҫ���ǵ���
 *        write��buffer�������io�ռ��buffer,����Ҳ��ǿ���л���
 *       ֻ��dma������Ϊ������ռ䲻��һ��ӳ��ռ��У����Ա��뿼��
 */


#if defined(__CC_ARM) || defined(__ARMCOMPILER_VERSION)

#define readb(c)        (__iormb(), readb_relaxed( (c) ))
#define readw(c)        (__iormb(), readw_relaxed( (c) ))
#define readl(c)        (__iormb(), readl_relaxed( (c) ))

#elif defined(__GNUC__)

#define readb(c)        ({ uint8_t  __v = readb_relaxed( (c) ); __iormb(); __v; })
#define readw(c)        ({ uint16_t __v = readw_relaxed( (c) ); __iormb(); __v; })
#define readl(c)        ({ uint32_t __v = readl_relaxed( (c) ); __iormb(); __v; })

#endif

#define writeb(v,c)     { rtk_barrier(); writeb_relaxed((v),(c) );__iomb(); }
#define writew(v,c)     { rtk_barrier(); writew_relaxed((v),(c) );__iomb(); }
#define writel(v,c)     { rtk_barrier(); writel_relaxed((v),(c) );__iomb(); }

/* ����IO�������� */
#define ioread8(addr)                       readb(addr)
#define ioread16(addr)                      readw(addr)
#define ioread32(addr)                      readl(addr)

#define iowrite8(b, addr)                   writeb((b), (addr))
#define iowrite16(b, addr)                  writew((b), (addr))
#define iowrite32(b, addr)                  writel((b), (addr))

static aw_inline void ioread8_rep (const void *addr, uint8_t *data, int count)
{
    while (count-- > 0) {
        *(data++)  = readb(addr);
    }
}

static aw_inline void ioread16_rep (const void *addr,
                                    uint16_t   *data,
                                    int         count)
{
    while (count-- > 0) {
        *(data++)  = readw(addr);
    }
}

static aw_inline void ioread32_rep (const void *addr,
                                    uint32_t   *data,
                                    int         count)
{
    while (count-- > 0) {
        *(data++)  = readl(addr);
    }
}

static aw_inline void iowrite8_rep (void          *addr,
                                    const uint8_t *data,
                                    int            count)
{
    while (count-- > 0) {
        writeb(*(data++) , addr);
    }
}


static aw_inline void iowrite16_rep (void           *addr,
                                     const uint16_t *data,
                                     int             count)
{
    while (count-- > 0) {
        writew(*(data++) , addr);
    }
}

static aw_inline void iowrite32_rep (void           *addr,
                                     const uint32_t *data,
                                     int             count)
{
    while (count-- > 0) {
        writel(*(data++) , addr);
    }
}
