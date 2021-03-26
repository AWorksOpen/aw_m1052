    PRESERVE8
    THUMB

    AREA   IMG_WIFI_FW, DATA, READONLY, ALIGN=4
    EXPORT __firmware_cyw43438_apsta_start
    EXPORT __firmware_cyw43438_apsta_end
    EXPORT __firmware_cyw43438_clm_blob_start
    EXPORT __firmware_cyw43438_clm_blob_end

__firmware_cyw43438_apsta_start
    INCBIN 43438A1.bin
__firmware_cyw43438_apsta_end

__firmware_cyw43438_clm_blob_start
    INCBIN 43438A1.clm_blob
__firmware_cyw43438_clm_blob_end
    END
