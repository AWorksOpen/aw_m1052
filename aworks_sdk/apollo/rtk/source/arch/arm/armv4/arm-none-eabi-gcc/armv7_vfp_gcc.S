#include "../armv7_context.h"
#include "armv7_gcc_common.h"


// static void armv7_vfp_init(void);
ARM_FUNC_BEGIN(armv7_vfp_init)

    // 配置NSACR
    // 确保Non-secure状态也能使用SIMD和VFP D16-D31寄存器
    // read NSACR
    mrc             p15, 0, r0 , c1, c1, 2
    bic             r0,#(NSACR_NSASEDIS|NSACR_NSD32DIS)
#if __RTK_ARMV7_USE_FPU
    // 支持FPU，开启Non-secure状态对CP10和CP11的访问
    orr             r0,#(NSACR_CP11|NSACR_CP10)
#else
    // 配置不支持FPU，关闭Non-secure状态对CP10和CP11的访问
    //
    bic             r0,#(NSACR_CP11|NSACR_CP10)
#endif
    mcr             p15, 0, r0 , c1, c1, 2

    // 配置CPACR
    // 确保SIMD和VFP D16-D31寄存器可以使用
    mrc             p15, 0, r0, c1, c0, 2
    bic             r0,#(CPACR_ASEDIS|CPACR_D32DIS)
#if __RTK_ARMV7_USE_FPU
    // 支持FPU,CP10和CP11完全访问
    orr             r0,#(CPACR_CP11_FULL|CPACR_CP10_FULL)
#else
    // 支持FPU,CP10和CP11禁止访问
    bic             r0,#(CPACR_CP11_FULL|CPACR_CP10_FULL)
#endif
    mcr             p15, 0, r0, c1, c0, 2

#if __RTK_ARMV7_USE_FPU
    // 初始状态下FPU被禁用
    mov             r0,#0
    vmsr            FPEXC,r0
#endif
    bx              lr
ARM_FUNC_END()


