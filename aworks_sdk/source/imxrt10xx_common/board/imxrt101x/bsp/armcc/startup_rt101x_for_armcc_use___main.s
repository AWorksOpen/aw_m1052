
                PRESERVE8
                THUMB

STACK_SIZE      EQU     0x2000


                IMPORT armv7m_exc_nmi_handler
                IMPORT armv7m_exc_hardfault_handler
                IMPORT armv7m_exc_memmanage_handler
                IMPORT armv7m_exc_busfault_handler
                IMPORT armv7m_exc_usagefault_handler
                IMPORT armv7m_exc_svcall_handler
                IMPORT armv7m_exc_pendsv_Handler
                IMPORT armv7m_exc_systick_handler
                IMPORT aw_armv7m_nvic_gen_isr
                IMPORT qspiflash_config
                IMPORT image_vector_table

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
__initial_sp_start
                SPACE   STACK_SIZE
__initial_sp
                EXPORT  __initial_sp_start
                EXPORT  __initial_sp


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY, ALIGN=3
                EXPORT  g_pfnVectors
                EXPORT  g_pfnVectors_End
                EXPORT  g_pfnVectors_Size
                KEEP    g_pfnVectors               ;保证节不被删除

g_pfnVectors
                DCD     __initial_sp   ;Top of Stack
                DCD     Reset_Handler
                DCD     armv7m_exc_nmi_handler
                DCD     armv7m_exc_hardfault_handler
                DCD     armv7m_exc_memmanage_handler
                DCD     armv7m_exc_busfault_handler
                DCD     armv7m_exc_usagefault_handler
                DCD     0
                DCD     0
                DCD     0
                DCD     0
                DCD     armv7m_exc_svcall_handler
                DCD     DebugMon_Handler
                DCD     0
                DCD     armv7m_exc_pendsv_Handler
                DCD     armv7m_exc_systick_handler
; External Interrupts 
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
                DCD     aw_armv7m_nvic_gen_isr
g_pfnVectors_End

g_pfnVectors_Size  EQU     g_pfnVectors_End - g_pfnVectors

IOMUX_GPR16_REG_ADDR  EQU  0X400AC040
IOMUX_GPR17_REG_ADDR  EQU  0X400AC044
SYS_TICK_CTL_ADDR     EQU  0XE000E010
NVIC_INT_CTRL         EQU  0XE000ED04
PEND_SYSTICK_SET      EQU  1 << 26
PEND_SYSTICK_CLR      EQU  1 << 25
REG_CCM_CLPCR_ADDR    EQU  0x400FC054

                AREA    |.text.Reset_Handler|, CODE, READONLY, ALIGN=3

; Reset Handler  ;首先应该禁用中断
Reset_Handler   PROC

                IMPORT  |Load$$ER_RW_DATA$$RW$$Base|        ;ER_RW_DATA执行区中的RW 输出节的加载地址
                IMPORT  |Image$$ER_RW_DATA$$RW$$Base|       ;ER_RW_DATA执行区中RW输出节的执行地址
                IMPORT  |Image$$ER_RW_DATA$$RW$$Limit|      ;ER_RW_DATA执行区中RW 输出节的长度
                IMPORT  |Image$$ER_RW_DATA$$ZI$$Base|
                IMPORT  |Image$$ER_RW_DATA$$ZI$$Limit|
                IMPORT  flex_ram_config
                IMPORT  armv7m_dcache_invalidate_all_no_context_save
                IMPORT  SystemInit
                IMPORT  __main
                IMPORT  main
                EXPORT  Reset_Handler
                EXPORT  Reset_Handler_End
                EXPORT  Reset_Handler_Size
                

                CPSID   I
                bl      armv7m_dcache_invalidate_all_no_context_save

; 检查systick中断是否被使能，并禁用systick中断
                ldr     r0, = SYS_TICK_CTL_ADDR;
                ldr     r0,[r0];
                mov     r1,#0
                ldr     r0, = SYS_TICK_CTL_ADDR;
                str     r1,[r0];
                dsb

; 清除NVIC 里面可能pending的systick中断
                ldr     r0,=NVIC_INT_CTRL;
                ldr     r1,=PEND_SYSTICK_SET;
                str     r1,[r0];
                dsb
                ldr     r1,=PEND_SYSTICK_CLR;
                str     r1,[r0];
                dsb
                ldr     r1,[r0];

; 使能了所有时钟
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

                ; 处理RTWDOG
                ldr     r1,=0x400bc004
                ldr     r2,=0x400bc000
                ldr     r3,=0xD928C520
                ldr     r4,=0x400bc008;
                ldr     r5,=0xFFFF
                ldr     r6,=0xB480A602

                ldr     r0,[r2]
                tst     r0,#(1<<7)
                beq     rt_wdog_end

                ; refresh
                str     r6,[r1];
                dsb

                ; unlock
                str     r3,[r1]
                dsb
                ; 新的周期
                str     r5,[r4]
                dsb

                ; update
                ldr     r0,[r2];
                orr     r0,#1<<5
                bic     r0,#1<<7
                str     r0,[r2]
                dsb
                ldr     r0,[r2]
rt_wdog_end

;判断代码段是否在SDRAM上
                mov     r3, pc
                ldr     r1, =0x80000000
                cmp     r3, r1
                bcc     code_is_flxspi
                ldr     r1, =0xDFFFFFFF
                cmp     r3, r1
                bcs     code_is_flxspi
                b       to_config_flex_ram

;判断代码段是否在Flexspi Flash上
code_is_flxspi
                mov     r3, pc
                ldr     r1, =0x60000000
                cmp     r3, r1
                bcc     init_data_addr
                ldr     r1, =0x64000000
                cmp     r3, r1
                bcs     init_data_addr
;配置FlexRAM
to_config_flex_ram
                ;保持RAM的时钟为打开
                ldr     r0, = 0x400B0000
                mov     r1,#4
                str     r1,[r0]
                dsb
                bl      flex_ram_config

init_data_addr  
                ldr  sp, =__initial_sp
;                ldr  r1, =|Load$$ER_RW_DATA$$RW$$Base|
;                ldr  r2, =|Image$$ER_RW_DATA$$RW$$Base|
;                ldr  r3, =|Image$$ER_RW_DATA$$RW$$Limit|
;                cmp  r2, r3
;                bcs  init_data_loop_end

;init_data_loop
;                ldr  r0, [r1], #4
;                str  r0, [r2], #4
;                cmp  r2, r3
;                bcc  init_data_loop

;init_data_loop_end
;                mov  r0, #0
;                ldr  r2, =|Image$$ER_RW_DATA$$ZI$$Base|
;                ldr  r3, =|Image$$ER_RW_DATA$$ZI$$Limit|
;                cmp  r2, r3
;                bcs  init_bss_loop_end

;init_bss_loop
;                str  r0, [r2], #4
;                cmp  r2, r3
;                bcc  init_bss_loop


;init_bss_loop_end
;                nop
                ; 设置CCM_CLPCR，使得idle状态CPU不进入休眠状态，正常执行
                ldr     r1,= REG_CCM_CLPCR_ADDR
                ldr     r2,[r1]
                and     r2,r2,#0xFFFFFFFC
                str     r2,[r1]
                bl  SystemInit
;                bl  __libc_init_array
;                bl  main
                ; 打开浮点
                ldr     r0, =0xe000ed88
                mov     r1, #0xF
                lsl     r1, r1, #20
                ldr     r2, [r0]
                orr     r2, r1
                str     r2, [r0]
                ldr     r0, =__main
                bx      r0
                ENDP
Reset_Handler_End

Reset_Handler_Size  EQU     Reset_Handler_End - Reset_Handler

                AREA    |.text.DebugMon_Handler|, CODE, READONLY
                EXPORT  DebugMon_Handler
                EXPORT  DebugMon_Handler_End
                EXPORT  DebugMon_Handler_Size

DebugMon_Handler  PROC

    LDR    R0, =DebugMon_Handler
    BX     R0
    ENDP
DebugMon_Handler_End

DebugMon_Handler_Size  EQU     DebugMon_Handler_End - DebugMon_Handler

                END
