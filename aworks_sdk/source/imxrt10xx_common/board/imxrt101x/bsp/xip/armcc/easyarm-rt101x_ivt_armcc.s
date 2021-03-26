
                PRESERVE8
                THUMB

                AREA    |.boot_hdr.ivt|, CODE, READONLY
                IMPORT  Reset_Handler
                EXPORT  _boot_data
                EXPORT  _dcd_data
                EXPORT  image_vector_table
                KEEP    image_vector_table      ;保证节不被删除
image_vector_table
            DCD     0x412000d1                  /* IVT Header */
            DCD     Reset_Handler               /* Image Entry Function */
            DCD     0                           /* Reserved = 0 */
            DCD     _dcd_data                   /* Address where DCD information is stored */
            DCD     _boot_data                  /* Address where BOOT Data Structure is stored */
            DCD     image_vector_table          /* Pointer to IVT Self (absolute address */
            DCD     0                           /* Address where CSF file is stored */
            DCD     0                           /* Reserved = 0 */
_boot_data
            DCD     0x60000000                  /* boot start location */
            DCD     0x00800000                  /* size */
            DCD     0                           /* Plugin flag*/
            DCD     0xFFFFFFFF                  /* empty - extra data word */
_dcd_data
            INCBIN  ../easyarm-rt1011.dcd



            END
