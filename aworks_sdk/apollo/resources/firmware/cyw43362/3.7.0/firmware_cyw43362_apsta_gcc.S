    .syntax  unified
    .thumb

    .align 4
    .globl __firmware_cyw43362_apsta_start
    .globl __firmware_cyw43362_apsta_end

__firmware_cyw43362_apsta_start:
    .incbin "43362A2.bin"
__firmware_cyw43362_apsta_end:
    .end
