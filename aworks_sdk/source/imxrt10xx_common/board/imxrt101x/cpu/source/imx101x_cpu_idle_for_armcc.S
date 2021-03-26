
                PRESERVE8
                THUMB

; section name
                AREA    |.text|, CODE, READONLY, ALIGN=2

pm_cpu_idle     PROC
                EXPORT  pm_cpu_idle

                wfi
                dsb
                bx      lr
                ENDP

                END



