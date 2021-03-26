/******************************************************************************
 * @file     startup_rt1050.s
 * @brief    CMSIS Cortex-M4 Core Device Startip File for Device iMX1050
 * @version  V0.0.1
 * @date     10. Oct 2015
 *
 ******************************************************************************/
    .syntax  unified
    .thumb

    .word __rom_size__

    /* imxrt1052 boot ivt hdr */
    .section .boot_hdr.ivt,#alloc
    .global _boot_data
    .global _dcd_data
    .global image_vector_table
    .type  image_vector_table, %object
    .align 4
image_vector_table:
    .word   0x412000d1                  /* IVT Header */
    .word   Reset_Handler               /* Image Entry Function */
    .word   0                           /* Reserved = 0 */
    .word   _dcd_data                   /* Address where DCD information is stored */
    .word   _boot_data                  /* Address where BOOT Data Structure is stored */
    .word   image_vector_table          /* Pointer to IVT Self (absolute address */
    .word   0                           /* Address where CSF file is stored */
    .word   0                           /* Reserved = 0 */
_boot_data:
    .word   0x60000000                  /* boot start location */
    .word   __rom_size__                /* size */
    .word   0                           /* Plugin flag*/
    .word   0xFFFFFFFF                  /* empty - extra data word */
_dcd_data:
    .incbin "imx1052.dcd"
    .size  image_vector_table, . - image_vector_table

