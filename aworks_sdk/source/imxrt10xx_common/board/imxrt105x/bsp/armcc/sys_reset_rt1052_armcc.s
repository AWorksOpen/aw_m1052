/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

                PRESERVE8
                THUMB

REG_SCB_AIRCR      EQU  0xE000ED0C
REG_IOMUXC_GPR14   EQU  0x400AC038
REG_IOMUXC_GPR16   EQU  0x400AC040
REG_IOMUXC_GPR17   EQU  0x400AC044

BIT_GPR16_ITCM_EN  EQU  (1<<0)
BIT_GPR16_DTCM_EN  EQU  (1<<1)
BIT_GPR16_FLEX_RAM_BANK_CFG_SEL  EQU  (1<<2)
    
MASK_GPR14_DTCM_SZ EQU  (0xF << 20)
MASK_GPR14_ITCM_SZ EQU  (0xF << 16)
MASK_SYSRESETREQ   EQU  ((0x5FA << 16) | (1 << 2))

                
                AREA    |.text|, CODE, READONLY
                IMPORT  __rtk_interrupt_disable
                EXPORT  imx1050_SystemReset
                EXPORT  imx1050_SystemReset_End
                EXPORT  imx1050_SystemReset_Size
             

imx1050_SystemReset  PROC
                ; 关闭所有中断
                ; 因为此函数不需要返回所以无需保存LR的值
                bl     __rtk_interrupt_disable

                ; 修改flexram的配置，需保证ocram至少有64KB的空间
                ; 首先关闭DTCM和ITCM
                ldr    r0, = REG_IOMUXC_GPR16;
                ldr    r1,[r0];
                bic    r1,BIT_GPR16_ITCM_EN
                bic    r1,BIT_GPR16_DTCM_EN;
                str    r1,[r0];
                dsb

                ; 选择fuse来配置FlexRam
                ldr     r0, = REG_IOMUXC_GPR16;
                ldr     r1,[r0];
                bic     r1,BIT_GPR16_FLEX_RAM_BANK_CFG_SEL
                str     r1,[r0];
                dsb
                isb
 
                ; 配置GPR17中的FlexRam,OCRAM 64KB
                ldr     r1, = 0xAAAAAAA5;
                ldr     r0, = REG_IOMUXC_GPR17;
                str     r1,[r0];
            
                ; 使用配置寄存器来配置FLEXRAM
                ldr     r0, = REG_IOMUXC_GPR16;
                ldr     r1,[r0];
                orr     r1,BIT_GPR16_FLEX_RAM_BANK_CFG_SEL
                str     r1,[r0];
                dsb
                isb

                ; 配置IOMUXC_GPR_GPR14寄存器，DTCM大小为256KB
                ldr     r0, = REG_IOMUXC_GPR14;
                ldr     r1, [r0]
                bic     r1,MASK_GPR14_ITCM_SZ
                bic     r1,MASK_GPR14_DTCM_SZ
                orr     r1,(0x9 << 20);
                str     r1,[r0];
                dsb
            
                ; GPR16 中使能DTCM
                ldr     r0, = REG_IOMUXC_GPR16;
                ldr     r1,[r0];
                orr     r1,BIT_GPR16_DTCM_EN;
                str     r1,[r0];
                dsb
                isb

                ; 设置NVIC SYSRESETREQ，等待系统复位
                ldr     r0, = REG_SCB_AIRCR
                ldr     r1, = MASK_SYSRESETREQ
                str     r1, [r0]
                dsb
                
                b       .
                ENDP
imx1050_SystemReset_End
    
imx1050_SystemReset_Size  EQU     imx1050_SystemReset_End - imx1050_SystemReset
                END

    