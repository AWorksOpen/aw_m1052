/******************************************************************************
 * @file     startup_rt1010.s
 * @brief    CMSIS Cortex-M4 Core Device Startip File for Device iMX1010
 * @version  V0.0.1
 * @date     10. Oct 2015
 *
 ******************************************************************************/
    .syntax  unified
    .thumb





    .section .boot_hdr.ivt,#alloc
    .global Reset_Handler
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
    .word   0x00800000                  /* size */
    .word   0                           /* Plugin flag*/
    .word   0xFFFFFFFF                  /* empty - extra data word */
_dcd_data:
    .incbin "easyarm-rt1011.dcd"
    .size  image_vector_table, . - image_vector_table

