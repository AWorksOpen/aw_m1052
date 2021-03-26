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
 * \brief arm中关于io操作的定义
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
 * \brief IO访问定义
 * @{
 * \par
 * readX/writeX()用于访问内存被映射的设备。不同体系结构中IO访问方式不尽相同。
 * 在一些简单的体系结构中，如没有MMU的ARM7，cortex-m3等，可以直接访问IO空间。
 * 而在其它体系结构中，如x86、开启了MMU的ARM9等，则不能直接访问IO空间，必须使用
 * 对应体系结构提供的方法进行访问，如进行物理地址转换等
 * \par
 * 可移植的驱动应该使用IO操作接口进行寄存器的读写，而不是直接对IO地址进行操作
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

/* 定义iormb和iowmb */
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
 * \brief io arm处理器的IO空间本身是强序列化的，唯一需要考虑的是
 *        write带buffer的情况，io空间带buffer,本身也是强序列化的
 *       只有dma区域，因为和外设空间不在一个映射空间中，所以必须考虑
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

/* 定义IO操作函数 */
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
