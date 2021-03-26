    .syntax  unified
    .thumb

    .align 4
    .globl __firmware_cyw43438_apsta_start
    .globl __firmware_cyw43438_apsta_end
    .globl __firmware_cyw43438_clm_blob_start
    .globl __firmware_cyw43438_clm_blob_end

__firmware_cyw43438_apsta_start:
    .incbin "43438A1.bin"
__firmware_cyw43438_apsta_end:

__firmware_cyw43438_clm_blob_start:
    .incbin "43438A1.clm_blob"
__firmware_cyw43438_clm_blob_end:

    .end

