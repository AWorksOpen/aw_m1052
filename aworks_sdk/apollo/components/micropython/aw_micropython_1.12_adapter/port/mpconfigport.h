/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __MPCONFIGPORT_H__
#define __MPCONFIGPORT_H__

#include "mphalport.h"
#include <stdio.h>

#define MICROPY_HW_BOARD_NAME      "AworksOS"   /** \brief 操作系统名称 */
#define MICROPY_PY_PATH_FIRST       "/libs/mpy/"
#define MICROPY_PY_PATH_SECOND      "/scripts/"


/** 适配端口函数*/
#define MP_PLAT_PRINT_STRN(str, len)             mp_hal_stdout_tx_strn_cooked(str, len)
#define MP_STATE_PORT                            MP_STATE_VM
#define MP_AW_NOT_IMPL_PRINT                     printf("Not implement on %s:%d, Please add for your board!\n", __FILE__, __LINE__)
#define __BYTE_ORDER__                           __ORDER_LITTLE_ENDIAN__

#define AW_NAME_MAX                             8
#define MICROPY_HEAP_SIZE                       (64 * 1024)/** \brief micropython 堆大小 */
#define MICROPY_STACK_SIZE                      (64 * 1024)/** \brief micropython 栈大小 */

#define MICROPY_OBJ_IMMEDIATE_OBJS              0/** \brief */
//#define MICROPY_MODULE_FROZEN_STR               1
#define MICROPY_MODULE_FROZEN_MPY               (1)
#define MICROPY_USE_INTERNAL_PRINTF             0
#define MICROPY_QSTR_BYTES_IN_HASH              1


#define MICROPY_PY_MICROPYTHON_MEM_INFO         (1)
#define MICROPY_STREAMS_NON_BLOCK               (1)
#define MICROPY_MODULE_WEAK_LINKS               (1)
#define MICROPY_CAN_OVERRIDE_BUILTINS           (1)
#define MICROPY_USE_INTERNAL_ERRNO              (0)

#define MICROPY_LONGINT_IMPL                    (MICROPY_LONGINT_IMPL_NONE)
#define MICROPY_FLOAT_IMPL                      (MICROPY_FLOAT_IMPL_FLOAT)

#define MICROPY_NLR_SETJMP                       (0)
#define MICROPY_ENABLE_COMPILER                  (1)
#define MICROPY_ENABLE_PYSTACK                   (0)
#define MICROPY_STACK_CHECK                      (1)
#define MICROPY_QSTR_EXTRA_POOL                  mp_qstr_frozen_const_pool
#define MICROPY_ALLOC_PATH_MAX                   (256)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT           (16)
#define MICROPY_EMIT_X64                         (0)
#define MICROPY_PERSISTENT_CODE_LOAD (1)
#define MICROPY_EMIT_THUMB                       (1)
#define MICROPY_EMIT_INLINE_THUMB                (1)
#define MICROPY_COMP_MODULE_CONST                (0)
#define MICROPY_COMP_CONST                       (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN         (0)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN         (0)
#define MICROPY_MEM_STATS                        (0)
#define MICROPY_DEBUG_PRINTERS                   (0)
#define MICROPY_ENABLE_GC                        (1)
#define MICROPY_ENABLE_FINALISER                 (1)
#define MICROPY_GC_ALLOC_THRESHOLD               (0)
#define MICROPY_REPL_EVENT_DRIVEN                (0)
#define MICROPY_REPL_AUTO_INDENT                 (1)
#define MICROPY_KBD_EXCEPTION                    (1)
#define MICROPY_HELPER_REPL                      (1)
#define MICROPY_HELPER_LEXER_UNIX                (0)
#define MICROPY_ENABLE_SOURCE_LINE               (0)
#define MICROPY_ENABLE_DOC_STRING                (0)
#define MICROPY_ERROR_REPORTING                  (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG    (0)
#define MICROPY_ENABLE_SCHEDULER 1
#define MICROPY_SCHEDULER_DEPTH 8
#define MICROPY_BEGIN_ATOMIC_SECTION()     __rtk_interrupt_disable()
#define MICROPY_END_ATOMIC_SECTION(state)  __rtk_interrupt_enable(state)


/**********************************************************************************************/
/**
 * \brief Builtin functions and exceptions
 */
#define MICROPY_PY_FUNCTION_ATTRS               1
#define MICROPY_PY_ALL_SPECIAL_METHODS          1
#define MICROPY_PY_BUILTINS_BYTEARRAY           1
#define MICROPY_PY_BUILTINS_COMPLEX             1
#define MICROPY_PY_BUILTINS_ENUMERATE           1
#define MICROPY_PY_BUILTINS_FILTER              1
#define MICROPY_PY_BUILTINS_SET                 1
#define MICROPY_PY_BUILTINS_FROZENSET           0
#define MICROPY_PY_BUILTINS_MEMORYVIEW          1
#define MICROPY_PY_BUILTINS_PROPERTY            1
#define MICROPY_PY_BUILTINS_REVERSED            1
#define MICROPY_PY_BUILTINS_SLICE               1
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED      1
#define MICROPY_PY_BUILTINS_COMPILE             0
#define MICROPY_CPYTHON_COMPAT                  1
#define MICROPY_PY_BUILTINS_EVAL_EXEC           1
#define MICROPY_PY_BUILTINS_EXECFILE            1
#define MICROPY_PY_BUILTINS_HELP                1
#define MICROPY_PY_BUILTINS_INPUT               1
#define MICROPY_PY_BUILTINS_MIN_MAX             1
#define MICROPY_PY_ASYNC_AWAIT                  1
#define MICROPY_PY_BUILTINS_STR_UNICODE         1

#define MICROPY_PY_BUILTINS_STR_CENTER          1/** \brief str.center() method provided*/
#define MICROPY_PY_BUILTINS_STR_PARTITION       1/** \brief str.partition()/str.rpartition() method provided*/
#define MICROPY_PY_BUILTINS_STR_SPLITLINES      1/** \brief str.splitlines() method provided*/
#define MICROPY_PY_BUILTINS_POW3                1// Support for calls to pow() with 3 integer arguments
#if MICROPY_PY_BUILTINS_HELP
extern const char aworks_help_text[];
#define MICROPY_PY_BUILTINS_HELP_TEXT           aworks_help_text
#define MICROPY_PY_BUILTINS_HELP_MODULES        1
#endif
#if MICROPY_PY_BUILTINS_SLICE
#define MICROPY_PY_BUILTINS_SLICE_ATTRS          1
#endif
/**********************************************************************************************/




/**********************************************************************************************/
/**
 * \brief Python standard libraries and micro-libraries
 */
#define MICROPY_PY_CMATH        1       /** \brief mathematical functions for complex numbers*/
#define MICROPY_PY_GC           1       /** \brief control the garbage collector*/
#define MICROPY_PY_MATH         1       /** \brief mathematical functions*/
#define MICROPY_PY_SYS          1       /** \brief system specific functions*/
#define MICROPY_PY_ARRAY        1       /** \brief arrays of numeric data*/
#define MICROPY_PY_UASYNCIO     1       /** \brief asynchronous I/O scheduler*/
#define MICROPY_PY_UBINASCII    1       /** \brief binary/ASCII conversions*/
#define MICROPY_PY_COLLECTIONS  1       /** \brief collection and container types*/
#define MICROPY_PY_UERRNO       1       /** \brief system error codes*/
#define MICROPY_PY_UHASHLIB     1       /** \brief hashing algorithms*/
#define MICROPY_PY_UHEAPQ       1       /** \brief heap queue algorithm*/
#define MICROPY_PY_IO           1       /** \brief input/output streams*/
#define MICROPY_PY_UJSON        1       /** \brief JSON encoding and decoding*/
#define MICROPY_PY_UOS          1       /** \brief basic “operating system” services*/
#define MICROPY_PY_URE          1       /** \brief simple regular expressions*/
#define MICROPY_PY_USELECT      1       /** \brief wait for events on a set of streams*/
#define MICROPY_PY_USOCKET      1       /** \brief socket module*/
#define MICROPY_PY_USSL         0       /** \brief SSL/TLS module*/
#define MICROPY_PY_STRUCT       1       /** \brief pack and unpack primitive data types*/
#define MICROPY_PY_UTIME        1       /** \brief time related functions*/
#define MICROPY_PY_UZLIB        1       /** \brief zlib decompression*/
#define MICROPY_PY_THREAD       1       /** \brief multithreading support*/

/**
 * \brief 标准模块下函数的控制
 */
#if  MICROPY_PY_SYS
#define MICROPY_PY_SYS_STDFILES              (1)    /** /brief 系统模块的标准文件*/
#endif

#if  MICROPY_PY_MATH
#define MICROPY_PY_MATH_SPECIAL_FUNCTIONS    (1)     /** /brief 数序运算模块特别的函数*/
#endif

#if  MICROPY_PY_COLLECTIONS
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT   (1)     /** /brief 集合和容器模块的有序字典*/
#endif

#if  MICROPY_PY_ARRAY
#define MICROPY_PY_ARRAY_SLICE_ASSIGN        (1)     /** /brief 数组模块，元素分配*/
#endif

#if  MICROPY_PY_URANDOM
#define MICROPY_PY_URANDOM_EXTRA_FUNCS       (1)     /** /brief 随机数生成模块特别的函数*/
#endif


#if  MICROPY_PY_UTIME
#define MICROPY_PY_UTIME_MP_HAL              (1)                        /**< /brief UTIME模块硬件*/
#define MICROPY_PY_UTIMEQ                    (1)                        /**< /brief UTIME队列*/
#endif

#if MICROPY_PY_UOS
#define MICROPY_PY_UOS_FILE                  (1)
#if MICROPY_PY_UOS_FILE
#define mp_import_stat(x)                        mp_posix_import_stat(x)
#endif//MICROPY_PY_UOS_FILE

#if MICROPY_PY_IO
#define MICROPY_PY_IO_RESOURCE_STREAM        (0)
#define MICROPY_PY_IO_FILEIO                 (1)
#define MICROPY_PY_IO_BYTESIO                (1)
#define MICROPY_PY_IO_BUFFEREDWRITER         (0)
#define MICROPY_READER_POSIX                 (1)
#endif//MICROPY_PY_IO

#endif//MICROPY_PY_UOS

 // extra built in names to add to the global namespace
 #define MICROPY_PORT_BUILTINS \
     { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

/**********************************************************************************************/



/**********************************************************************************************/
/**
 * \brief MicroPython-specific libraries
 */
#define MICROPY_PY_BTREE        0       /** \brief simple BTree database*/
#define MICROPY_PY_FRAMEBUF     0       /** \brief frame buffer manipulation*/
#define MICROPY_PY_MACHINE      1       /** \brief functions related to the hardware*/
#define MICROPY_PY_MICROPYTHON  0       /** \brief access and control MicroPython internals*/
#define MICROPY_PY_NETWORK      0       /** \brief network configuration*/
#define MICROPY_PY_UBLUETOOTH   0       /** \brief low-level Bluetooth*/
#define MICROPY_PY_UCCRYPTOLIB  0       /** \brief cryptographic ciphers*/
#define MICROPY_PY_UCTYPES      0       /** \brief access binary data in a structured way*/


/**
 * \brief machine模块下模块下类库、内置函数控制
 */
#if MICROPY_PY_MACHINE
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW      mp_pin_make_new             /**< /brief 引脚类库的构造函数*/
#define MICROPY_PY_MACHINE_I2C_MAKE_NEW      machine_hard_i2c_make_new   /**< /brief I2C类库的构造函数*/
#define MICROPY_PY_MACHINE_SPI_MAKE_NEW      machine_hard_spi_make_new   /**< /brief 开启SPI类库的构造函数*/
#define MICROPY_PY_MACHINE_LED      (1)       /**< /brief 开启LED类库*/
#define MICROPY_PY_MACHINE_UART     (1)       /**< /brief 开启串口类库*/
#define MICROPY_PY_MACHINE_PIN      (1)       /**< /brief 开启引脚类库*/
#define MICROPY_PY_MACHINE_I2C      (1)       /**< /brief 开启I2C类库*/
#define MICROPY_PY_MACHINE_SPI      (1)       /**< /brief 开启SPI类库*/
#define MICROPY_HW_ENABLE_SDCARD    (1)
#define MICROPY_HW_ENABLE_FLASH     (1)
#define MICROPY_PY_MACHINE_SIGNAL   (1)
#define MICROPY_PY_MACHINE_MEM      (1)
#define MICROPY_PY_MACHINE_PULSE    (1)
#endif
/**********************************************************************************************/


/**********************************************************************************************/
/**
 * \brief AWorks(port)-specific libraries
 */
#define MICROPY_PY_AWORKS           (1)       /**< /brief AWorks模块 */
#define MICROPY_PY_USRMOD           (1)       /** \brief 示例模块 */
/**********************************************************************************************/


/**
 * \brief include any root pointers defined by a port
 */
#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8]; \
    \
    mp_obj_t pyb_hid_report_desc; \
    \
    mp_obj_t pyb_config_main; \
    \
    mp_obj_t pyb_switch_callback; \
    \
    mp_obj_t pin_class_mapper; \
    mp_obj_t pin_class_map_dict; \
    \
    mp_obj_t pyb_extint_callback[MP_MACHINE_INTE_NUM_MAX]; \
    \
    mp_obj_t pyb_extint_callback_arg[MP_MACHINE_INTE_NUM_MAX];\
    \
    bool pyb_extint_hard_irq[MP_MACHINE_INTE_NUM_MAX];\
    /* pointers to all Timer objects (if they have been created)  \
    struct _pyb_timer_obj_t *pyb_timer_obj_all[MICROPY_HW_MAX_TIMER]; */\
    \
    /* stdio is repeated on this UART object if it's not null */ \
    struct _pyb_uart_obj_t *pyb_stdio_uart; \
    \
    /* pointers to all UART objects (if they have been created)  \
    struct _pyb_uart_obj_t *pyb_uart_obj_all[MICROPY_HW_MAX_UART];*/ \
    \
    /* pointers to all CAN objects (if they have been created) */ \
    struct _pyb_can_obj_t *pyb_can_obj_all[2]; \
    \
    /* list of registered NICs */ \
    mp_obj_list_t mod_network_nic_list; \



#if MICROPY_PY_THREAD
#include "py/mpthread.h"
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        MP_THREAD_GIL_EXIT(); \
        MP_THREAD_GIL_ENTER(); \
    } while (0);
#else
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        aw_mdelay(1); \
    } while (0);
#endif

#define MICROPY_PORT_INIT_FUNC\
//    mp_pins_init();
#define MICROPY_PORT_DEINIT_FUNC \
        extern void mp_pins_deinit(void);\
        mp_pins_deinit();
/**********************************************************************************************/
/**
 * \brief port模块定义
 */
#if MICROPY_PY_USRMOD
extern const struct _mp_obj_module_t mp_module_usrmod;
#define MICROPY_PORT_MOD_USRMOD      { MP_ROM_QSTR(MP_QSTR_usrmod), MP_ROM_PTR(&mp_module_usrmod) },
#else
#define MICROPY_PORT_MOD_USRMOD
#endif

#if MICROPY_PY_UTIME
extern const struct _mp_obj_module_t mp_module_utime;
#define MICROPY_PORT_MOD_UTIME      { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&mp_module_utime) },
#else
#define MICROPY_PORT_MOD_UTIME
#endif

#if MICROPY_PY_USOCKET
extern const struct _mp_obj_module_t mp_module_usocket;
#define MICROPY_PORT_MOD_USOCKET    { MP_ROM_QSTR(MP_QSTR_usocket), MP_ROM_PTR(&mp_module_usocket) },
#else
#define MICROPY_PORT_MOD_USOCKET
#endif

#if MICROPY_PY_UOS
extern const struct _mp_obj_module_t mp_module_uos;
#define MICROPY_PORT_MOD_UOS        { MP_ROM_QSTR(MP_QSTR_uos), MP_ROM_PTR(&mp_module_uos) },
#else
#define MICROPY_PORT_MOD_UOS
#endif

#if MICROPY_PY_AWORKS
extern const struct _mp_obj_module_t mp_module_aworks;
#define MICROPY_PORT_MOD_AWORKS     { MP_ROM_QSTR(MP_QSTR_aworks), MP_ROM_PTR(&mp_module_aworks) },
#else
#define MICROPY_PORT_MOD_AWORKS
#endif

#if MICROPY_PY_MACHINE
#define MICROPY_PORT_MOD_MACHINE    { MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&mp_module_machine) },
#else
#define MICROPY_PORT_MOD_MACHINE
#endif

/** \brief 注册适配模块 */
#define MICROPY_PORT_BUILTIN_MODULES \
        MICROPY_PORT_MOD_MACHINE \
        MICROPY_PORT_MOD_AWORKS \
        MICROPY_PORT_MOD_UOS \
        MICROPY_PORT_MOD_USOCKET \
        MICROPY_PORT_MOD_UTIME \
        MICROPY_PORT_MOD_USRMOD
/**********************************************************************************************/


#endif/* __MPCONFIGPORT_H */
