/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file armv7m_cache_gcc.S
 * \brief rtk中armv7m架构cache相关函数的arm-none-eabi-gcc汇编实现 *
 *
 * \par 源代码
 * \internal
 * \par Modification history
 * - 1.00 2018-06-13 sni, first version.
 * \endinternal
 */

#include "armv7m_gcc_common.h"

    .set CACHE_CLIDR_ADDR           ,0xE000ED78
    .set CACHE_CTR_ADDR             ,0xE000ED7c
    .set CACHE_CCSIDR_ADDR          ,0xE000ED80
    .set CACHE_CSSELR_ADDR          ,0xE000ED84

    .set CACHE_ICIALLU_ADDR         ,0xE000EF50
    .set CACHE_ICIMVAU_ADDR         ,0xE000EF58
    .set CACHE_DCIMVAC_ADDR         ,0xE000EF5C
    .set CACHE_DCISW_ADDR           ,0xE000EF60
    .set CACHE_DCCMVAU_ADDR         ,0xE000EF64
    .set CACHE_DCCMVAC_ADDR         ,0xE000EF68
    .set CACHE_DCCSW_ADDR           ,0xE000EF6c

    .set CACHE_DCCIMVAC_ADDR        ,0xE000EF70
    .set CACHE_DCCISW_ADDR          ,0xE000EF74
    .set CACHE_BPIALL_ADDR          ,0xE000EF78

    .set  __BIT_SCB_CCR_ICACHE      , (1 << 17) /* I Cache */
    .set  __BIT_SCB_CCR_BP          , (1 << 18) /* branch predictors */
    .set  __BIT_SCB_CCR_DCACHE      , (1 << 16) /* D Cache */
    .set SCB_CCR_ADDR               ,0xE000ED14



/*******************************************************************************
   global functions
*******************************************************************************/
ARM_FUNC_BEGIN(armv7m_dcache_invalidate_by_mva)
    ldr     r1,=CACHE_DCIMVAC_ADDR
    str     r0,[r1]
    dsb                                 //DSB
    bx      lr
ARM_FUNC_END()

ARM_FUNC_BEGIN(armv7m_dcache_clean_by_mva)
    ldr     r1,=CACHE_DCCMVAC_ADDR
    str     r0,[r1]
    DSB                                 //DSB
    bx      lr
ARM_FUNC_END()


ARM_FUNC_BEGIN(armv7m_icache_invalidate_all)
    mov     r0, #0
    ldr     r1, =CACHE_ICIALLU_ADDR;
    ldr     r2, =CACHE_BPIALL_ADDR;
    str     r0,[r1];                    //清空指令cache
    str     r0,[r2];                    //清空分支预测
    dsb
    isb
    bx      lr
ARM_FUNC_END()

ARM_FUNC_BEGIN(armv7m_icache_enable)
    push    {lr};
    bl      armv7m_icache_invalidate_all
    ldr     r1,=SCB_CCR_ADDR;
    ldr     r0,[r1];
    orr     r0, r0, #__BIT_SCB_CCR_ICACHE
    orr     r0, r0, #__BIT_SCB_CCR_BP
    str     r0,[r1];
    dsb
    isb
    pop    {lr};
    bx      lr
ARM_FUNC_END()

    //
ARM_FUNC_BEGIN(armv7m_icache_disable)
    ldr     r1,=SCB_CCR_ADDR;
    ldr     r0,[r1];
    bic     r0, r0, #__BIT_SCB_CCR_ICACHE
    str     r0,[r1];
    dsb
    isb
    bx      lr
ARM_FUNC_END()

ARM_FUNC_BEGIN(armv7m_dcache_get_line_size)
    ldr     r0,=CACHE_CTR_ADDR
    ldr     r0,[r0];
    lsr     r0,r0,#16
    and     r0,r0,#0xf
    add     r0,r0,#2
    mov     r1,#1
    lsl     r0,r1,r0
    bx      lr
ARM_FUNC_END()

    // int armv7m_cache_get_cache_level_count()
ARM_FUNC_BEGIN(armv7m_cache_get_cache_level_count)
    ldr     r0,=CACHE_CLIDR_ADDR;
    ldr     r3,[r0];                    //获取Cache Level ID Register
                                        // r3 -> cache_level_id
    mov     r2,#7                       // r2 -> mask
                                        // mask = 0x7;

    mov     r0,#0                       // r0 -> level_count
                                        // level_count = 0
                                        // r1 -> i
    mov     r1,#0                       // i = 0;

loop_start:                             //  do {
    tst     r3,r2;                      //      if (cache_level_id & mask) {
    ite ne
    addne   r0,r0,#1                    //          level_count ++
                                        //      }
    beq     loop_exit;                  //      else {
                                        //          break;
                                        //      }
    lsl     r2,r2,#3                    //      mask = mask << 3;
    add     r1,r1,#1                    //      i ++;
    cmp     r1,#7
    blt     loop_start                  //  } while(i < 7)
loop_exit:

    bx      lr
ARM_FUNC_END()

    // int armv7m_cache_level_is_support_dcache(int level)
ARM_LOCAL_FUNC_BEGIN(armv7m_cache_level_is_support_dcache)
                                        // r0 -> level
    ldr     r1,=CACHE_CLIDR_ADDR;
    ldr     r3,[r1];                    //获取Cache Level ID Register
                                        // r3 -> cache_level_id
    mov     r2,#7                       // r2 -> mask
                                        // mask = 0x7;
    mov     r1,#3
    mul     r0,r0,r1                    // level = level * 3

    lsr     r3,r3,r0                    // cache_level_id = cache_level_id << level
    and     r0,r3,r2                    // level = cache_level_id & mask;
    tst     r0,#0x6                     // if (level & 0x6) {
    ite     ne
    movne   r0,#1                       //      level = 1;
                                        // }
                                        // else {
    moveq   r0,#0                       //      level = 0;
                                        // }

    bx      lr
ARM_FUNC_END()

    // int armv7m_dcache_level_get_set_count(int level)
ARM_LOCAL_FUNC_BEGIN(armv7m_dcache_level_get_set_count)
                                        // r0 -> level
    lsl     r0,#1                       // r0 = r0 << 1;

    ldr     r1,= CACHE_CSSELR_ADDR;
    str     r0,[r1]                     // 写入r0至CSSELR
    dsb

    ldr     r0, = CACHE_CCSIDR_ADDR;
    ldr     r1,[r0]                     // Read current CCSIDR into R1
    lsr     r1,#13                      // CCSIDR = CCSIDR >> 13
    ldr     r2, =0x7fff                 // mask = 0x7fff
    and     r0,r1,r2                    // level = CCSIDR & mask;
    add     r0,r0,#1                    // level = level + 1;

    bx      lr                          // return level;
ARM_FUNC_END()

    // static int __find_pow_of_2_not_below(unsigned int num)
ARM_LOCAL_FUNC_BEGIN(__find_pow_of_2_not_below)
                                        // r0 -> num
    mov     r1,#0                       // r1 -> pow,pow = 0
    mov     r2,#1                       // r2 -> val,val = 1
label_while_val_below_num_start:
    cmp     r2,r0                       // while(val < num) {
    bcs     label_while_val_below_num_exit;

    lsl     r2,#1                       //      val = val *2;
    add     r1,r1,#1                    //      pow ++;
    b       label_while_val_below_num_start;
                                        // }
label_while_val_below_num_exit:
    mov     r0,r1
    bx      lr                          // return pow;
ARM_FUNC_END()

    // int armv7m_dcache_level_get_way_count(int level)
ARM_LOCAL_FUNC_BEGIN(armv7m_dcache_level_get_way_count)
                                        // r0 -> level
    lsl     r0,#1                       // r0 = r0 << 1;
    ldr     r1,=CACHE_CSSELR_ADDR;
    str     r0,[r1]                     // 写入r0至CSSELR
    dsb

    ldr     r0, = CACHE_CCSIDR_ADDR;
    ldr     r1,[r0]                     // Read current CCSIDR into R1
    lsr     r1,#3                       // CCSIDR = CCSIDR >> 3
    ldr     r2, =0x3ff                  // mask = 0x3ff
    and     r0,r1,r2                    // way = CCSIDR & mask;
    add     r0,r0,#1                    // way = way + 1;

    bx      lr                          // return way;
ARM_FUNC_END()

    // int armv7m_dcache_level_get_line_size(int level)
ARM_LOCAL_FUNC_BEGIN(armv7m_dcache_level_get_line_size)
                                        // r0 -> level
    lsl     r0,#1                       // r0 = r0 << 1;
    ldr     r1,=CACHE_CSSELR_ADDR;
    str     r0,[r1]                     // 写入r0至CSSELR
    dsb

    ldr     r0, = CACHE_CCSIDR_ADDR;
    ldr     r1,[r0]                     // Read current CCSIDR into R1
    ldr     r2, =0x7                    // mask = 0x7
                                        // r0 ->line_size
    and     r0,r1,r2                    // line_size = CCSIDR & mask;
    add     r0,r0,#4                    // line_size = line_size + 4;
                                        // temp -> r1
    mov     r1,#1                       // temp = 1
    lsl     r0,r1,r0                    // line_size = temp << line_size;
    bx      lr                          // return line_size;
ARM_FUNC_END()





    // void armv7m_dcache_clean_all_no_context_save(void)
    // 此函数不使用任何内存，以免出现意外情况
ARM_FUNC_BEGIN(armv7m_dcache_clean_all_no_context_save)
    mov     r12,lr                     //save lr

    bl      armv7m_cache_get_cache_level_count;// cache_level_count -> r4
    mov     r4,r0;
    cmp     r4,#0
    beq     armv7m_dcache_clean_all_exit;   // if (cache_level_count == 0) {return;}
                                            // level -> r7
    mov     r7,#0                           // for (level = 0;level <cache_level_count; level ++ ) {
dcache_level_loop_start:
    cmp     r7,r4
    bge     dcache_level_loop_exit;

    mov     r0,r7
    bl      armv7m_cache_level_is_support_dcache // if (armv7m_cache_level_is_support_dcache(level) ) {
    cmp     r0,#0
    beq     dcache_armv7_cache_level_is_support_dcache_if_exit

    mov     r0,r7
    bl      armv7m_dcache_level_get_set_count   //      set_count = armv7m_dcache_level_get_set_count(level);
    mov     r6,r0;                              //      r6 ->set_count

    mov     r0,r7
    bl      armv7m_dcache_level_get_way_count   //      way_count = armv7m_dcache_level_get_way_count(level);
    mov     r5,r0                               //      r5 -> way_count

                                                //      L -> r11
                                                //      temp -> r0
    mov     r0,r7                               //      temp = level
    bl  armv7m_dcache_level_get_line_size;      //      temp = armv7m_dcache_level_get_line_size(temp)
    bl  __find_pow_of_2_not_below               //      L = __find_pow_of_2_not_below(temp);
    mov     r11,r0

                                                //      A -> r10
    mov     r0,r5
    bl      __find_pow_of_2_not_below           //      A = __find_pow_of_2_not_below(way_count);
    mov     r10,r0
    mov     r0,#32
    sub     r10,r0,r10                          //      A = 32 - A;

    mov     r9,#0                               //      r9 -> set
dcache_label_set_loop_start:                    //      for (set = 0; set < set_count;set ++) {
    cmp     r9,r6
    bge     dcache_label_set_loop_exit;

        // r8 -> way
        mov     r8,#0
dcache_label_way_loop_start:                    //          for (way = 0; way < way_count;way ++) {
        cmp     r8,r5
        bge     dcache_label_way_loop_exit;

        // r0 -> data,r1 -> t
        mov     r0,#0                           //              data = 0;
        lsl     r1,r7,#1                        //              t = level << 1
        orr     r0,r0,r1                        //              data = data | t;

        lsl     r1,r9,r11                       //              t = set << L;
        orr     r0,r0,r1                        //              data = data | t;

        lsl     r1,r8,r10                       //              t = way << A;
        orr     r0 ,r0,r1                       //              data = data | t

        ldr     r1,=CACHE_DCCSW_ADDR
        str     r0,[r1]                         //              DCCSW = data;
        dsb                                     //              DSB

        add     r8,r8,#1
        b       dcache_label_way_loop_start;    //          }
dcache_label_way_loop_exit:

    add     r9,r9,#1
    b       dcache_label_set_loop_start         //      }
dcache_label_set_loop_exit:

dcache_armv7_cache_level_is_support_dcache_if_exit:

    add     r7,r7,#1
    b       dcache_level_loop_start             // }
dcache_level_loop_exit:
armv7m_dcache_clean_all_exit:
    mov     lr,r12                  //恢复lr
    bx      lr
ARM_FUNC_END()

    // void armv7m_dcache_invalidate_all_no_context_save(void)
    // 除了初始部分保存上下文，此函数推荐不再使用内存，以避免意外情况
ARM_FUNC_BEGIN(armv7m_dcache_invalidate_all_no_context_save)
    mov     r12,lr                     //save lr

    bl      armv7m_cache_get_cache_level_count;// cache_level_count -> r4
    mov     r4,r0;
    cmp     r4,#0
    beq     armv7m_dcache_invalidate_all_exit; // if (cache_level_count == 0) {return;}
                                            // level -> r7
    sub     r7,r4,#1                        // for (level = cache_level_count -1;level >=0 ; level -- ) {
level_loop_start:
    cmp     r7,#0
    blt     level_loop_exit;

    mov     r0,r7
    bl      armv7m_cache_level_is_support_dcache // if (armv7m_cache_level_is_support_dcache(level) ) {
    cmp     r0,#0
    beq     armv7_cache_level_is_support_dcache_if_exit

    mov     r0,r7
    bl      armv7m_dcache_level_get_set_count   //      set_count = armv7m_dcache_level_get_set_count(level);
    mov     r6,r0;                              //      r6 ->set_count

    mov     r0,r7
    bl      armv7m_dcache_level_get_way_count   //      way_count = armv7m_dcache_level_get_way_count(level);
    mov     r5,r0                               //      r5 -> way_count

                                                //      L -> r11
                                                //      temp -> r0
    mov     r0,r7                               //      temp = level
    bl  armv7m_dcache_level_get_line_size;       //     temp = armv7m_dcache_level_get_line_size(temp)
    bl  __find_pow_of_2_not_below               //      L = __find_pow_of_2_not_below(temp);
    mov     r11,r0

                                                //      A -> r10
    mov     r0,r5
    bl      __find_pow_of_2_not_below           //      A = __find_pow_of_2_not_below(way_count);
    mov     r10,r0
    mov     r0,#32
    sub     r10,r0,r10                          //      A = 32 - A;

    mov     r9,#0                               //      r9 -> set
label_set_loop_start:                           //      for (set = 0; set < set_count;set ++) {
    cmp     r9,r6
    bge     label_set_loop_exit;

        // r8 -> way
        mov     r8,#0
label_way_loop_start:                           //          for (way = 0; way < way_count;way ++) {
        cmp     r8,r5
        bge     label_way_loop_exit;

        // r0 -> data,r1 -> t
        mov     r0,#0                           //              data = 0;
        lsl     r1,r7,#1                        //              t = level << 1
        orr     r0,r0,r1                        //              data = data | t;

        lsl     r1,r9,r11                       //              t = set << L;
        orr     r0,r0,r1                        //              data = data | t;

        lsl     r1,r8,r10                       //              t = way << A;
        orr     r0 ,r0,r1                       //              data = data | t

        ldr     r1,=CACHE_DCISW_ADDR
        str     r0,[r1]                         //              DCISW = data;
        dsb                                     //              DSB

        add     r8,r8,#1
        b       label_way_loop_start;           //          }
label_way_loop_exit:

    add     r9,r9,#1
    b       label_set_loop_start                //      }
label_set_loop_exit:

armv7_cache_level_is_support_dcache_if_exit:

    sub     r7,r7,#1
    b       level_loop_start                    // }
level_loop_exit:
armv7m_dcache_invalidate_all_exit:
    mov     lr,r12                              //恢复lr
    bx      lr
ARM_FUNC_END()

ARM_FUNC_BEGIN(armv7m_dcache_disable)
    push    {r0-r12,lr}
    bl      armv7m_cache_get_cache_level_count; // cache_level_count -> r0
    cmp     r0,#0
    beq     armv7m_dcache_disable_exit;         // if (cache_level_count == 0) {return;}

    bl      armv7m_dcache_clean_all_no_context_save

    ldr     r1,=SCB_CCR_ADDR;
    ldr     r0,[r1];
    bic     r0, r0, #__BIT_SCB_CCR_DCACHE
    str     r0,[r1];
    dsb
armv7m_dcache_disable_exit:
    pop     {r0-r12,lr}
    bx      lr
ARM_FUNC_END()

ARM_FUNC_BEGIN(armv7m_dcache_enable)
    push    {r0-r12,lr}
    bl      armv7m_cache_get_cache_level_count;// cache_level_count -> r0
    cmp     r0,#0
    beq     armv7m_dcache_enable_exit;          // if (cache_level_count == 0) {return;}

    bl      armv7m_dcache_invalidate_all_no_context_save

    ldr     r1,=SCB_CCR_ADDR;
    ldr     r0,[r1];
    orr     r0, r0, #__BIT_SCB_CCR_DCACHE
    str     r0,[r1];
    dsb
armv7m_dcache_enable_exit:
    pop     {r0-r12,lr}
    bx      lr
ARM_FUNC_END()

    .end

/* end of file */
