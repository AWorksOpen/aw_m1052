    PRESERVE8
    THUMB

    AREA   IMG_WIFI_FW, DATA, READONLY, ALIGN=4
    EXPORT __firmware_cyw43362_mfgtest_start
    EXPORT __firmware_cyw43362_mfgtest_end

__firmware_cyw43362_mfgtest_start
    INCBIN apollo/resources/firmware/cyw43362/3.7.0/43362A2-mfgtest.bin
__firmware_cyw43362_mfgtest_end
    END
