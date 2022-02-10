MEMORY
{
    config (rx)            : ORIGIN = 0x60000000, LENGTH = 4K
    ivt (rx)               : ORIGIN = 0x60001000, LENGTH = 4k
    code (rx)              : ORIGIN = 0x60002000, LENGTH = 4088K
    flash (rx)             : ORIGIN = 0x60400000, LENGTH = 4M
    data (xrw)             : ORIGIN = 0x80A00000, LENGTH = 1018K
    relocode (rx)          : ORIGIN = 0x80AFF000, LENGTH = 4K
    flexspi_drv(rx)        : ORIGIN = 0x80000000, LENGTH = 4K
    dma_heap (rw)          : ORIGIN = 0x80100000, LENGTH = 1M
    heap (rw)              : ORIGIN = 0x80200000, LENGTH = 13M
    lcd_vram (rw)          : ORIGIN = 0x80F00000, LENGTH = 1M
}

PROTECT_SIZE = 0x2000;
RAMFUNC_SIZE = 0x2000;
STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : 0x2000;
ENTRY(Reset_Handler)

SECTIONS
{
    .config :
    {
        KEEP(*(.boot_hdr.conf))
    }>config
    
    .ivt :
    {
        KEEP(*(.boot_hdr.ivt))
        KEEP(*(.boot_hdr.boot_data))
        KEEP(*(.boot_hdr.dcd_data))
    }>ivt
  
    /* 代码段  LMA == VMA == 0x30000000 */
    /* 具有常量属性段 */
    . = ORIGIN(code);
    . = ALIGN(0x100000);
    __code_start = .;
    .text :
    {
        KEEP(*(.isr_vector))       /* 使用KEEP使.vector段不被移除 */
        aw_backtrace_text_start = .;
        *(.text)
        *(.text.*)              /*  cpp namespace function      */
        *(.romrun)              /*  rom中必须的函数             */
        aw_backtrace_text_end = .;
        *(.rodata)              /*  read-only data (constants)  */
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
    }>code

    /* section information for simple shell symbols */
    .text :
    {
        . = ALIGN(4);
        __shellsym_tab_start = .;
        KEEP(*(.shellsymbol))
        __shellsym_tab_end = .;
    }>code

    .ARM.extab : 
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    }>code

    /* .ARM.exidx is sorted, so has to go in its own output section */
    . = ALIGN(4);
     __exidx_start = .;
     PROVIDE(__exidx_start = __exidx_start);
    .ARM.exidx :
    {
        /* __exidx_start = .; */
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        /* __exidx_end = .;   */
    }>code
    . = ALIGN(4);
     __exidx_end = .;
     PROVIDE(__exidx_end = __exidx_end);

    /*
     * C++ 全局对象构造与析构函数表
     * 这里放在 .text 和 .ARM.exidx 之后, .data 之前,
     * 这里的  LMA 和 VMA 相同, 如果放在 .data 之后, LMA 与 VMA 不同,
     * 则需要启动程序从装载区搬运到运行区
     */

    . = ALIGN(4);
    .ctors :
    {
        PROVIDE (__AW_CTOR_LIST__ = . );
        KEEP (*(.preinit_array))
        KEEP (*(.init_array))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
        PROVIDE (__AW_CTOR_END__ = . );
    }>code

    .dtors :
    {
        PROVIDE (__AW_DTOR_LIST__ = . );
        KEEP (*(.fini_array))
        KEEP (*(SORT(.dtors.*)))        
        KEEP (*(.dtors))
        PROVIDE (__AW_DTOR_END__ = . );
    }>code

    PROVIDE(relocode_origin = .);

    . = ORIGIN(code) + LENGTH(code);
    PROVIDE (__code_end = .);

    .stack (NOLOAD) :
    {
        _sstack = . ;
        . = ALIGN(8);
        . = . + STACK_SIZE;
        . = ALIGN(8);
        _estack = . ;
    } > data
    
    _sidata = LOADADDR(.data);

    . = ALIGN(4);

    .relocode :
    {
        relocode_start = . ;
        KEEP (*(SORT(.ram.func)))
        relocode_end = . ;
    } > relocode AT > code

    . = ALIGN(4);

    .data :
    {
        _sdata = . ;
        *(.data .data.*)
        . = ALIGN(8);
        _edata = . ;
    } > data AT > code

    .bss (NOLOAD) :
    {
        . = ALIGN(8);
        _sbss = . ;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(8);
        _ebss = . ;
    } > data
    
    . = ALIGN(8);
    /* 系统堆 */
    PROVIDE (__heap_start__ = ORIGIN(heap) );
    PROVIDE (__heap_end__ = ORIGIN(heap) + LENGTH(heap) );

    /* dma 堆 */
    PROVIDE (__heap_dma_start__ = ORIGIN(dma_heap) );
    PROVIDE (__heap_dma_end__ = ORIGIN(dma_heap) + LENGTH(dma_heap) );

    /* lcd 显存 */ 
    PROVIDE (__lcd_vram_start__ = ORIGIN(lcd_vram) );
    PROVIDE (__lcd_vram_end__ = ORIGIN(lcd_vram) + LENGTH(lcd_vram) );

    /* flash */ 
    PROVIDE (__flash_start__ = ORIGIN(flash) );
    PROVIDE (__flash_end__ = ORIGIN(flash) + LENGTH(flash) );

    /*MPU依赖的关键信息*/
    PROVIDE (__rom_start__ = ORIGIN(code));
    PROVIDE (__rom_end__ = ORIGIN(code) + LENGTH(code) );
    
    PROVIDE (__ram_start__ = ORIGIN(data));
    PROVIDE (__ram_end__ = ORIGIN(data) + LENGTH(data) );

    /* Flexspi驱动(Flexspi Flash XIP执行时保存Flash的读写驱动到该内存) */    
    PROVIDE (__flexspi_drv_start__ = ORIGIN(flexspi_drv));
    PROVIDE (__flexspi_drv_end__ = ORIGIN(flexspi_drv) + LENGTH(flexspi_drv) );
    
    /* 重定位代码(启动时将部分代码保存到该内存中) */
    PROVIDE (__relocode_start__ = ORIGIN(relocode));
    PROVIDE (__relocode_end__ = ORIGIN(relocode) + LENGTH(relocode));
    
    /* 固件大小 (用于XIP描述固件大小) */
    PROVIDE (__rom_size__ = ((relocode_origin - ORIGIN(config)) + SIZEOF(.data) + SIZEOF(.relocode)));
}
