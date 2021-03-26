    .syntax  unified
    .thumb

    .align 4
    .globl __firmware_cyw43362_mfgtest_start
    .globl __firmware_cyw43362_mfgtest_end

__firmware_cyw43362_mfgtest_start:
    .incbin "apollo/resources/firmware/cyw43362/3.7.0/43362A2-mfgtest.bin"
__firmware_cyw43362_mfgtest_end:
    .end
