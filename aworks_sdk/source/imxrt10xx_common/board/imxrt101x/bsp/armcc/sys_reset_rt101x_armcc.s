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
                EXPORT  imx1010_SystemReset
                EXPORT  imx1010_SystemReset_End
                EXPORT  imx1010_SystemReset_Size
             

imx1010_SystemReset  PROC
                ; �ر������ж�
                ; ��Ϊ�˺�������Ҫ�����������豣��LR��ֵ
                bl     __rtk_interrupt_disable

                ; �޸�flexram�����ã��豣֤ocram������64KB�Ŀռ�
                ; ���ȹر�DTCM��ITCM
                ldr    r0, = REG_IOMUXC_GPR16;
                ldr    r1,[r0];
                bic    r1,BIT_GPR16_ITCM_EN
                bic    r1,BIT_GPR16_DTCM_EN;
                str    r1,[r0];
                dsb

                ; ѡ��fuse������FlexRam
                ldr     r0, = REG_IOMUXC_GPR16;
                ldr     r1,[r0];
                bic     r1,BIT_GPR16_FLEX_RAM_BANK_CFG_SEL
                str     r1,[r0];
                dsb
                isb
 
                ; ����GPR17�е�FlexRam,OCRAM 64KB
                ldr     r1, = 0xAAA5;
                ldr     r0, = REG_IOMUXC_GPR17;
                str     r1,[r0];
            
                ; ʹ�����üĴ���������FLEXRAM
                ldr     r0, = REG_IOMUXC_GPR16;
                ldr     r1,[r0];
                orr     r1,BIT_GPR16_FLEX_RAM_BANK_CFG_SEL
                str     r1,[r0];
                dsb
                isb

                ; ����IOMUXC_GPR_GPR14�Ĵ�����DTCM��СΪ128KB
                ldr     r0, = REG_IOMUXC_GPR14;
                ldr     r1, [r0]
                bic     r1,MASK_GPR14_ITCM_SZ
                bic     r1,MASK_GPR14_DTCM_SZ
                orr     r1,(0x8 << 20);
                str     r1,[r0];
                dsb
            
                ; GPR16 ��ʹ��DTCM
                ldr     r0, = REG_IOMUXC_GPR16;
                ldr     r1,[r0];
                orr     r1,BIT_GPR16_DTCM_EN;
                str     r1,[r0];
                dsb
                isb

                ; ����NVIC SYSRESETREQ���ȴ�ϵͳ��λ
                ldr     r0, = REG_SCB_AIRCR
                ldr     r1, = MASK_SYSRESETREQ
                str     r1, [r0]
                dsb
                
                b       .
                ENDP
imx1010_SystemReset_End
    
imx1010_SystemReset_Size  EQU     imx1010_SystemReset_End - imx1010_SystemReset
                END

    
