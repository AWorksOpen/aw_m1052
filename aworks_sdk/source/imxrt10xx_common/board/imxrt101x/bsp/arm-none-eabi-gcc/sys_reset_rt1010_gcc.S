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

    .syntax  unified
    .thumb
    
    .global imx1010_SystemReset
    
    .set    REG_SCB_AIRCR,   0xE000ED0C
    .set    REG_IOMUXC_GPR14,0x400AC038
    .set    REG_IOMUXC_GPR16,0x400AC040
    .set    REG_IOMUXC_GPR17,0x400AC044

    
    .set    BIT_GPR16_ITCM_EN, (1<<0)
    .set    BIT_GPR16_DTCM_EN, (1<<1)
    .set    BIT_GPR16_FLEX_RAM_BANK_CFG_SEL, (1<<2)
    
    .set    MASK_GPR14_DTCM_SZ, (0xF << 20)
    .set    MASK_GPR14_ITCM_SZ, (0xF << 16)
    .set    MASK_SYSRESETREQ,  ((0x5FA << 16) | (1 << 2))
    
    /**
     * void imx1010_SystemReset(void)
     *
     * \brief 此函数用于imxrt1010软件复位，默认复位模式为上一次硬件复位的模式，并不会查询当前的外部复位模式
     *        进行复位（例如上电时复位模式为sd卡启动，上电后将外部复位模式改为改为norflash启动，此时依旧会
     *        按照sd卡启动的模式来复位）。    
     */
    .section  .text.imx1010_SystemReset
    .type  imx1010_SystemReset, %function
imx1010_SystemReset:
    // 关闭所有中断
    // 因为此函数不需要返回所以无需保存LR的值
    bl     __rtk_interrupt_disable

    // 修改flexram的配置，需保证ocram至少有64KB的空间
    // 首先关闭DTCM和ITCM
    ldr    r0, = REG_IOMUXC_GPR16;
    ldr    r1,[r0];
    bic    r1,BIT_GPR16_ITCM_EN
    bic    r1,BIT_GPR16_DTCM_EN;
    str    r1,[r0];
    dsb

    // 选择fuse来配置FlexRam
    ldr     r0, = REG_IOMUXC_GPR16;
    ldr     r1,[r0];
    bic     r1,BIT_GPR16_FLEX_RAM_BANK_CFG_SEL
    str     r1,[r0];
    dsb
    isb
 
    // 配置GPR17中的FlexRam,OCRAM 64KB
    ldr     r1, = 0xAAA5;
    ldr     r0, = REG_IOMUXC_GPR17;
    str     r1,[r0];

    // 使用配置寄存器来配置FLEXRAM
    ldr     r0, = REG_IOMUXC_GPR16;
    ldr     r1,[r0];
    orr     r1,BIT_GPR16_FLEX_RAM_BANK_CFG_SEL
    str     r1,[r0];
    dsb
    isb

    //配置IOMUXC_GPR_GPR14寄存器，DTCM大小为128KB
    ldr     r0, = REG_IOMUXC_GPR14;
    ldr     r1, [r0]
    bic     r1,MASK_GPR14_ITCM_SZ
    bic     r1,MASK_GPR14_DTCM_SZ
    orr     r1,(0x8 << 20);
    str     r1,[r0];
    dsb

    // GPR16 中使能DTCM
    ldr     r0, = REG_IOMUXC_GPR16;
    ldr     r1,[r0];
    orr     r1,BIT_GPR16_DTCM_EN;
    str     r1,[r0];
    dsb
    isb

    //设置NVIC SYSRESETREQ，等待系统复位
    ldr     r0, = REG_SCB_AIRCR
    ldr     r1, = MASK_SYSRESETREQ
    str     r1, [r0]
    dsb
    
    b       .

    .size  imx1010_SystemReset, . - imx1010_SystemReset

