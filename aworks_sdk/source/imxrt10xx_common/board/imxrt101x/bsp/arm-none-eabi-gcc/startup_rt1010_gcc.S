/******************************************************************************
 * @file     startup_rt1020.s
 * @brief    CMSIS Cortex-M4 Core Device Startip File for Device iMX1020
 * @version  V0.0.1
 * @date     10. Oct 2015
 *
 ******************************************************************************/


  .syntax  unified
  .thumb

  .word  _sidata
  .word  _sdata
  .word  _edata
  .word  _sbss
  .word  _ebss

  .global boot_data
  .global qspiflash_config
  .global image_vector_table
  
  .section  .isr_vector
  .global  g_pfnVectors
  .type  g_pfnVectors, %object
  .align 4

g_pfnVectors:
  .word  _estack
  .word  Reset_Handler
  .word  armv7m_exc_nmi_handler
  .word  armv7m_exc_hardfault_handler
  .word  armv7m_exc_memmanage_handler
  .word  armv7m_exc_busfault_handler
  .word  armv7m_exc_usagefault_handler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  armv7m_exc_svcall_handler
  .word  DebugMon_Handler
  .word  0
  .word  armv7m_exc_pendsv_Handler
  .word  armv7m_exc_systick_handler
  /* External Interrupts */
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr
  .word aw_armv7m_nvic_gen_isr

  .size  g_pfnVectors, . - g_pfnVectors

    .set IOMUX_GPR16_REG_ADDR,0x400ac040
    .set IOMUX_GPR17_REG_ADDR,0x400ac044
    .set SYS_TICK_CTL_ADDR,0xE000E010
    .set NVIC_INT_CTRL,0xE000ED04              /* interrupt control state register */
    .set PEND_SYSTICK_SET,1 << 26
    .set PEND_SYSTICK_CLR,1 << 25
    .set REG_CCM_CLPCR_ADDR, 0x400FC054

    .global flex_ram_config;
/**
 * Reset Handler
 *
 * @param  None
 * @return None
 *
 * @brief  This is the code that gets called when the processor first
 *         starts execution following a reset event. Only the absolutely
 *         necessary set is performed, after which the application
 *         supplied main() routine is called.
 */

  .section  .text.Reset_Handler
  .global  Reset_Handler
  .type  Reset_Handler, %function

Reset_Handler:
    // 首先应该禁用中断
    CPSID   I
    
    // 设置CLPCR寄存器，使WFI处于RUN状态
    ldr     r0,=REG_CCM_CLPCR_ADDR
    ldr     r1,[r0]
    bic     r1,#0x3
    str     r1,[r0]

    // 检查systick中断是否被使能，并禁用systick中断
    ldr     r0, = SYS_TICK_CTL_ADDR;
    ldr     r0,[r0];
    mov     r1,#0
    ldr     r0, = SYS_TICK_CTL_ADDR;
    str     r1,[r0];
    dsb

    // 清除NVIC 里面可能pending的systick中断
    ldr     r0,=NVIC_INT_CTRL;
    ldr     r1,=PEND_SYSTICK_SET;
    str     r1,[r0];
    dsb
    ldr     r1,=PEND_SYSTICK_CLR;
    str     r1,[r0];
    dsb
    ldr     r1,[r0];

    // 使能了所有时钟
    ldr     r1, = 0xFFFFFFFF;
    ldr     r0, = 0x400FC068
    str     r1,[r0];
    ldr     r0, = 0x400FC06C
    str     r1,[r0];
    ldr     r0, = 0x400FC070
    str     r1,[r0];
    ldr     r0, = 0x400FC074
    str     r1,[r0];
    ldr     r0, = 0x400FC078
    str     r1,[r0];
    ldr     r0, = 0x400FC07C
    str     r1,[r0];
    ldr     r0, = 0x400FC080
    str     r1,[r0];

    //判断代码段是否在SDRAM上
    mov     r3, pc
    ldr     r1, =0x80000000
    cmp     r3, r1
    bcc     code_is_flxspi
    ldr     r1, =0xDFFFFFFF
    cmp     r3, r1
    bcs     code_is_flxspi
    b       to_config_flex_ram

    //判断代码段是否在Flexspi Flash上
code_is_flxspi:
    mov     r3, pc
    ldr     r1, =0x60000000
    cmp     r3, r1
    bcc     init_data_addr
    ldr     r1, =0x64000000
    cmp     r3, r1
    bcs     init_data_addr

to_config_flex_ram:
    //保持RAM的时钟为打开
    ldr     r0, = 0x400B0000
    mov     r1,#4
    str     r1,[r0]
    dsb

    bl      flex_ram_config;

init_data_addr:
    ldr  sp, =_estack

    ldr  r1, =_sidata
    ldr  r2, =_sdata
    ldr  r3, =_edata
    cmp  r2, r3
    bcs  init_data_loop_end
init_data_loop:
    ldr  r0, [r1], #4
    str  r0, [r2], #4
    cmp  r2, r3
    bcc  init_data_loop
init_data_loop_end:

    mov  r0, #0
    ldr  r2, =_sbss
    ldr  r3, =_ebss
    cmp  r2, r3
    bcs  init_bss_loop_end
init_bss_loop:
    str  r0, [r2], #4
    cmp  r2, r3
    bcc  init_bss_loop
init_bss_loop_end:
    nop
    bl  SystemInit
    //bl  __libc_init_array
    bl  main
    b  .

  .size  Reset_Handler, . - Reset_Handler

/**
 * DebugMon Handler
 *
 * @param  None
 * @return None
 */

  .section  .text.DebugMon_Handler
  .weak  DebugMon_Handler
  .type  DebugMon_Handler, %function

DebugMon_Handler:
  B  .

  .size  DebugMon_Handler, . - DebugMon_Handler
