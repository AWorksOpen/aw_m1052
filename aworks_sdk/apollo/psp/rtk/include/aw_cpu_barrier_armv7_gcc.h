#ifndef __AW_CPU_BARRIER_ARMV7_GCC_H__
#define __AW_CPU_BARRIER_ARMV7_GCC_H__

aw_psp_static_inline void aw_barrier_mb()
{
    asm volatile("dsb" : : : "memory");
}

aw_psp_static_inline void aw_barrier_wmb()
{
    asm volatile("dsb" : : : "memory");
}

aw_psp_static_inline void aw_barrier_rmb()
{
    asm volatile("dsb" : : : "memory");
}

aw_psp_static_inline void aw_barrier_isb()
{
    asm volatile("isb" : : : "memory");
}

#endif /* __AW_CPU_BARRIER_ARMV7_GCC_H__ */

/* end of file */

