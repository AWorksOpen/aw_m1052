; /*******************************************************************************
; *                                 AWorks
; *                       ----------------------------
; *                       innovating embedded platform
; *
; * Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
; * All rights reserved.
; *
; * Contact information:
; * web site:    http://www.zlg.cn/
; *******************************************************************************/

; /**
; * \file
; * \brief WAV文件包含
; *
; *  将一个Flexspi驱动bin文件包含在内，Flexpsi XIP执行时,会自动把Flexspi驱动
; *  复制到内存，从内存中执行Flexspi的基本读、擦除、编程等操作。
; *
; * \internal
; * \par History
; * - 1.00 18-02-02  mex, first implementation.
; * \endinternal
; */

    .syntax  unified
    .thumb

    .align 4

    .globl flexspi_drv_ram
    .globl end_of_drv

    /* bin文件 */
flexspi_drv_ram:
    .incbin "source/imxrt10xx_common/board/imxrt105x/cpu/source/driver/flexspi/flexspi_drv_ram_boot.bin"
    
    /* .endif */
end_of_drv:
    .end
