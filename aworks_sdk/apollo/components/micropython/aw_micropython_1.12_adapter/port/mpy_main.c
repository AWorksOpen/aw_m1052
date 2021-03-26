/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Armink (armink.ztl@gmail.com)
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/stackctrl.h"
#include "py/frozenmod.h"
#include "lib/utils/pyexec.h"
#include "mpconfigport.h"
#include "lib/utils/interrupt_char.h"
#include "aw_getchar.h"
#include "aw_mem.h"
#include "aw_assert.h"

#if MICROPY_ENABLE_COMPILER
void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
#endif

char *stack_top = MP_OBJ_NULL;
static char *heap = MP_OBJ_NULL;
void mpy_main(const char *filename) {
    int stack_dummy;

    stack_top = (char*)&stack_dummy;

    printf("\n");

#if MICROPY_PY_THREAD
    uint32_t stack_size;
    void *stack;
soft_reset:
    stack_size = rtk_task_self()->stack_high - rtk_task_self()->stack_low;
    stack = (void *)rtk_task_self()->stack_low;
    mp_thread_init(stack, stack_size / 4);
#endif

    aw_getchar_init();

//    mp_stack_ctrl_init();
    mp_stack_set_top(stack_top);
    // Make MicroPython's stack limit somewhat smaller than full stack available
    mp_stack_set_limit(MICROPY_STACK_SIZE);

    #if MICROPY_ENABLE_GC
    heap = aw_mem_alloc(MICROPY_HEAP_SIZE);
    if (!heap) {
        printf("No memory for MicroPython Heap!\n");
        return;
    }
    gc_init(heap, heap + MICROPY_HEAP_SIZE);
    #endif

//    #if MICROPY_ENABLE_PYSTACK
//    static mp_obj_t pystack[AW_THREAD_STACK_SIZE];
//    mp_pystack_init(pystack, &pystack[AW_THREAD_STACK_SIZE]);
//    #endif

    mp_init();

    /* system path initialization */
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_));
    mp_obj_list_init(mp_sys_argv, 0);

    if (filename) {
#ifndef MICROPY_PY_UOS
        mp_printf(&mp_plat_print, "Please enable uos module in sys module option first.\n");
#else
        pyexec_file(filename);
#endif
        } else {

        void *frozen_data;
        int frozen_type = mp_find_frozen_module("_boot.py", strlen("_boot.py"), &frozen_data);
        if(frozen_type != MP_FROZEN_NONE){
            pyexec_frozen_module("_boot.py");
        }
        pyexec_file_if_exists("boot.py");
        if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
            pyexec_file_if_exists("main.py");
        }

        mp_printf(&mp_plat_print, "\n");

        for (;;) {
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                if (pyexec_raw_repl() != 0) {
                    break;
                }
            } else {
                if (pyexec_friendly_repl() != 0) {
                    break;
                }
            }
        }

    }

    gc_sweep_all();

#if MICROPY_PY_THREAD
    mp_thread_deinit();
#endif

    mp_deinit();

    /* 释放分配的内存 */
    aw_mem_free((void *)heap);

    aw_getchar_deinit();

    if(filename){
        return ;
    }else{
        goto soft_reset;
    }

}

void aw_python_file(const char *filename){
    mpy_main(filename);
}

void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    aw_assert(0);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    (void)func;
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    aw_assert(0);
}
#endif


