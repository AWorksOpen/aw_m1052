
                PRESERVE8
                THUMB

                AREA    IMG_FLEXSPI_DRV, DATA, READONLY, ALIGN=4
                EXPORT  flexspi_drv_ram
                EXPORT  end_of_drv

flexspi_drv_ram
               INCBIN     source\imxrt10xx_common\board\imxrt105x\cpu\source\driver\flexspi\flexspi_drv_ram_boot.bin
end_of_drv
                END


