#include "../armv7_context.h"
#include "armv7_gcc_common.h"


// static void armv7_vfp_init(void);
ARM_FUNC_BEGIN(armv7_vfp_init)
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

    .section .text.rtk_monitor_vector,"ax"
    .align  5
_rtk_monitor_vector:
    .word   0
    .word   0
    ldr     pc, .Lmonitor_addr
    .word   0
    .word   0
    .word   0
    ldr     pc,=irq_handle
    ldr     pc,=fiq_handle
    .word   0                   // extra word in the RAM vectors

.Lmonitor_addr:
    .word     armv7_monitor_handler

ARM_LOCAL_FUNC_BEGIN(armv7_monitor_handler)

    // 修改SCR寄存器为不是Secure状态
    mov         r0,#0x5
    mcr         p15, 0, r0, c1, c1, 0
    dsb
    mov         r0,#0
    mrc         p15, 0, r0, c1, c1, 0
    movs        pc,lr
ARM_FUNC_END()

// static void armv7_basic_init_secure_state(void);
// 在secure状态下进行的初始化，需要的话可以从secure状态转换为non-secure状态
ARM_FUNC_BEGIN(armv7_basic_init_secure_state)
    push        {lr};
    // bl          gic_init_secure;

#if __RTK_ARMV7_USE_FPU
    // read NSACR
    mrc             p15, 0, r0 , c1, c1, 2
    // 确保Non-secure状态对CP10和CP11的访问
    mov             r1,#(NSACR_CP11|NSACR_CP10)
    orr             r1,#(1 <<18);
    orr             r1,#(1 <<17);
    orr             r1,#(1 <<16);

    mcr             p15, 0, r1 , c1, c1, 2
    mrc             p15, 0, r0 , c1, c1, 2
    and             r1,r0,#(NSACR_CP11|NSACR_CP10)
    cmp             r1,#(NSACR_CP11|NSACR_CP10)
    bne             .
#endif
#if 0
    // 使用smc指令，切换到monitor状态，以便获得Secure寄存器访问权限
    // 进行Secure相关寄存器的访问
    // 修改Monitor Vector Base Address Register
    MRC         p15, 0, r0, c12, c0, 1 ;
    ldr         r1,=_rtk_monitor_vector;
    MCR         p15, 0, r1, c12, c0, 1 ;
    dsb


    smc         #0
#endif
    pop         {pc}

ARM_FUNC_END()
