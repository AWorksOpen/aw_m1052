    PRESERVE8
    THUMB

    AREA   IMG_WIFI_FW, DATA, READONLY, ALIGN=4
    EXPORT __firmware_cyw43362_apsta_start
    EXPORT __firmware_cyw43362_apsta_end

__firmware_cyw43362_apsta_start
    INCBIN 43362A2.bin
__firmware_cyw43362_apsta_end
    END
