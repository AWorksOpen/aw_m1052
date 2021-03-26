/**
 * \file
 * \brief MMU function for ARMv4 arch.
 *
 * \internal
 * \par History
 * - 1.00 14-09-23  liuweiyun, first implementation.
 * \endinternal
 */

    .section  __cache_asm   /* section name */
    .text                   /* code section */
    .code       32          /* ARM mode */
    .align      2           /* aligned at 2^n */

/*******************************************************************************
    c1 bits mask of p15
*******************************************************************************/

    .set  __P15_R1_I,   (1 << 12) /* I Cache */
    .set  __P15_R1_BR,  (1 << 11) /* branch predictors */
    .set  __P15_R1_C,   (1 <<  2) /* D Cache */
    .set  __P15_R1_A,   (1 <<  1) /* align */
    .set  __P15_R1_M,   (1)       /* MMU */

    .set  __P15_R1_ACTLR_SMP,   (1 << 6)       /* SMP */

    .set  __P15_R1_W,   (1 <<  3) /* write buffer */
    .set  __P15_R1_RR,  (1 << 14) /* Cache */
    .set  __P15_R1_END, (1 <<  7) /* endian */

/******************************************************************************/
#define __FUNC_DECL(name)   .global name; .type name, %function; .func name
#define __LOCAL_FUNC_DECL(name)   .type name, %function; .func name
#define __FUNC_END          .endfunc

/*******************************************************************************
   global functions
*******************************************************************************/



    __FUNC_DECL(arch_mmu_icache_enable)
arch_mmu_icache_enable:
    mrc     p15, 0, r0, c1, c0, 0
    orr     r0, r0, #__P15_R1_I
    orr     r0, r0, #__P15_R1_BR
    mcr     p15, 0, r0, c1, c0, 0
    mcr     p15, 0, r0, c7, c10, 4      //DSB
    mcr     p15, 0, r0, c7, c5, 4       //ISB
    bx      lr
    __FUNC_END

    __FUNC_DECL(arch_mmu_icache_disable)
arch_mmu_icache_disable:
    mrc     p15, 0, r0, c1, c0, 0
    bic     r0, r0, #__P15_R1_I
    mcr     p15, 0, r0, c1, c0, 0
    mcr     p15, 0, r0, c7, c10, 4      //DSB
    mcr     p15, 0, r0, c7, c5, 4       //ISB
    bx      lr
    __FUNC_END

    __FUNC_DECL(arch_mmu_icache_invalidate)
arch_mmu_icache_invalidate:
    mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 0       //清空指令cache
    mcr     p15, 0, r0, c7, c5, 6       //清空分支预测
    mcr     p15, 0, r0, c7, c5, 4       //ISB
    mcr     p15, 0, r0, c7, c10, 4      //DSB
    bx      lr
    __FUNC_END

    __FUNC_DECL(armv7_dsb_cp15)
armv7_dsb_cp15:
    mcr     p15, 0, r0, c7, c10, 4      //DSB
    bx      lr
    __FUNC_END

    __FUNC_DECL(armv7_dmb_cp15)
armv7_dmb_cp15:
    mcr     p15, 0, r0, c7, c10, 5      //DMB
    bx      lr
    __FUNC_END

    __FUNC_DECL(armv7_isb_cp15)
armv7_isb_cp15:
    mcr     p15, 0, r0, c7, c5, 4      //ISB
    bx      lr
    __FUNC_END


    __FUNC_DECL(armv7_mmu_dcache_invalidate_mva)
armv7_mmu_dcache_invalidate_mva:
    mcr     p15, 0, r0, c7, c6, 1
    mcr     p15, 0, r0, c7, c10, 4      //DSB
    bx      lr
    __FUNC_END

    __FUNC_DECL(armv7_mmu_dcache_clean_mva)
armv7_mmu_dcache_clean_mva:
    mcr     p15, 0, r0, c7, c10, 1
    mcr     p15, 0, r0, c7, c10, 4      //DSB
    bx      lr
    __FUNC_END

    __FUNC_DECL(armv7_get_cache_line_size)
armv7_get_cache_line_size:
    mrc     p15, 0, r0, c0, c0, 1
    lsr     r0,r0,#16
    and     r0,r0,#0xf
    add     r0,r0,#2
    mov     r1,#1
    lsl     r0,r1,r0
    bx      lr
    __FUNC_END




    // int armv7_get_cache_level_count()
    __LOCAL_FUNC_DECL(armv7_get_cache_level_count)
armv7_get_cache_level_count:
    mrc     p15, 1, r3, c0, c0, 1       //获取Cache Level ID Register
                                        // r3 -> cache_level_id
    mov     r2,#7                       // r2 -> mask
                                        // mask = 0x7;

    mov     r0,#0                       // r0 -> level_count
                                        // level_count = 0
                                        // r1 -> i
    mov     r1,#0                       // i = 0;

loop_start:                             //  do {
    tst     r3,r2;                      //      if (cache_level_id & mask) {
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
    __FUNC_END

    // int armv7_cache_level_is_support_dcache(int level)
    __LOCAL_FUNC_DECL(armv7_cache_level_is_support_dcache)
armv7_cache_level_is_support_dcache:
                                        // r0 -> level
    mrc     p15, 1, r3, c0, c0, 1       //获取Cache Level ID Register
                                        // r3 -> cache_level_id
    mov     r2,#7                       // r2 -> mask
                                        // mask = 0x7;
    mov     r1,#3
    mul     r0,r0,r1                    // level = level * 3

    lsr     r3,r3,r0                    // cache_level_id = cache_level_id << level
    and     r0,r3,r2                    // level = cache_level_id & mask;
    tst     r0,#0x6                      // if (level & 0x6) {
    movne   r0,#1                       //      level = 1;
                                        // }
                                        // else {
    moveq   r0,#0                       //      level = 0;
                                        // }

    bx      lr
    __FUNC_END

    // int armv7_dcache_level_get_set_count(int level)
    __LOCAL_FUNC_DECL(armv7_dcache_level_get_set_count)
armv7_dcache_level_get_set_count:
                                        // r0 -> level
    lsl     r0,#1                       // r0 = r0 << 1;
    mcr     p15, 2, r0, c0, c0, 0       // 写入r0至CSSELR

    mrc     p15, 1, r1, c0, c0, 0       // Read current CCSIDR into R1
    lsr     r1,#13                      // CCSIDR = CCSIDR >> 13
    ldr     r2, =0x7fff                 // mask = 0x7fff
    and     r0,r1,r2                    // level = CCSIDR & mask;
    add     r0,r0,#1                    // level = level + 1;

    bx      lr                          // return level;
    __FUNC_END

    // static int __find_pow_of_2_not_below(unsigned int num)
    __LOCAL_FUNC_DECL(__find_pow_of_2_not_below)
__find_pow_of_2_not_below:
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
    __FUNC_END

    // int armv7_dcache_level_get_way_count(int level)
    __LOCAL_FUNC_DECL(armv7_dcache_level_get_way_count)
armv7_dcache_level_get_way_count:
                                        // r0 -> level
    lsl     r0,#1                       // r0 = r0 << 1;
    mcr     p15, 2, r0, c0, c0, 0       // 写入r0至CSSELR

    mrc     p15, 1, r1, c0, c0, 0       // Read current CCSIDR into R1
    lsr     r1,#3                       // CCSIDR = CCSIDR >> 3
    ldr     r2, =0x3ff                  // mask = 0x3ff
    and     r0,r1,r2                    // way = CCSIDR & mask;
    add     r0,r0,#1                    // way = way + 1;

    bx      lr                          // return way;
    __FUNC_END

    // int armv7_dcache_level_get_line_size(int level)
    __LOCAL_FUNC_DECL(armv7_dcache_level_get_line_size)
armv7_dcache_level_get_line_size:
                                        // r0 -> level
    lsl     r0,#1                       // r0 = r0 << 1;
    mcr     p15, 2, r0, c0, c0, 0       // 写入r0至CSSELR

    mrc     p15, 1, r1, c0, c0, 0       // Read current CCSIDR into R1
    ldr     r2, =0x7                    // mask = 0x7
                                        // r0 ->line_size
    and     r0,r1,r2                    // line_size = CCSIDR & mask;
    add     r0,r0,#4                    // line_size = line_size + 4;
                                        // temp -> r1
    mov     r1,#1                       // temp = 1
    lsl     r0,r1,r0                    // line_size = temp << line_size;
    bx      lr                          // return line_size;
    __FUNC_END

#if 0

#endif

// void arch_mmu_dcache_clean(void)
    // 除了初始部分保存上下文，此函数推荐不再使用内存，以避免意外情况
    __LOCAL_FUNC_DECL(arch_mmu_dcache_clean)
arch_mmu_dcache_clean:
    push    {r4-r12,lr}                     //保存寄存器上下文

    bl      armv7_get_cache_level_count;    // cache_level_count -> r4
    mov     r4,r0;
                                            // level -> r7
    mov     r7,#0                           // for (level = 0;level <cache_level_count; level ++ ) {
dcache_level_loop_start:
    cmp     r7,r4
    bge     dcache_level_loop_exit;

    mov     r0,r7
    bl      armv7_cache_level_is_support_dcache // if (armv7_cache_level_is_support_dcache(level) ) {
    cmp     r0,#0
    beq     dcache_armv7_cache_level_is_support_dcache_if_exit

    mov     r0,r7
    bl      armv7_dcache_level_get_set_count    //      set_count = armv7_dcache_level_get_set_count(level);
    mov     r6,r0;                              //      r6 ->set_count

    mov     r0,r7
    bl      armv7_dcache_level_get_way_count    //      way_count = armv7_dcache_level_get_way_count(level);
    mov     r5,r0                               //      r5 -> way_count

                                                //      L -> r11
                                                //      temp -> r0
    mov     r0,r7                               //      temp = level
    bl  armv7_dcache_level_get_line_size;       //      temp = armv7_dcache_level_get_line_size(temp)
    bl  __find_pow_of_2_not_below               //      L = __find_pow_of_2_not_below(temp);
    mov     r11,r0
                                                //      S -> r12
    mov     r0,r6
    bl      __find_pow_of_2_not_below           //      S = __find_pow_of_2_not_below(set_count);
    mov     r12,r0

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

        mcr     p15, 0, r0, c7, c10, 2          //              DCCSW = data;
        mcr     p15, 0, r0, c7, c10, 4          //              DSB

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

    pop    {r4-r12,lr}                      //恢复寄存器上下文
    bx      lr
    __FUNC_END

    // void arch_mmu_dcache_invalidate(void)
    // 除了初始部分保存上下文，此函数推荐不再使用内存，以避免意外情况
    __LOCAL_FUNC_DECL(arch_mmu_dcache_invalidate)
arch_mmu_dcache_invalidate:
    push    {r4-r12,lr}                     //保存寄存器上下文

    bl      armv7_get_cache_level_count;    // cache_level_count -> r4
    mov     r4,r0;
                                            // level -> r7
    sub     r7,r4,#1                        // for (level = cache_level_count -1;level >=0 ; level -- ) {
level_loop_start:
    cmp     r7,#0
    blt     level_loop_exit;

    mov     r0,r7
    bl      armv7_cache_level_is_support_dcache // if (armv7_cache_level_is_support_dcache(level) ) {
    cmp     r0,#0
    beq     armv7_cache_level_is_support_dcache_if_exit

    mov     r0,r7
    bl      armv7_dcache_level_get_set_count    //      set_count = armv7_dcache_level_get_set_count(level);
    mov     r6,r0;                              //      r6 ->set_count

    mov     r0,r7
    bl      armv7_dcache_level_get_way_count    //      way_count = armv7_dcache_level_get_way_count(level);
    mov     r5,r0                               //      r5 -> way_count

                                                //      L -> r11
                                                //      temp -> r0
    mov     r0,r7                               //      temp = level
    bl  armv7_dcache_level_get_line_size;       //      temp = armv7_dcache_level_get_line_size(temp)
    bl  __find_pow_of_2_not_below               //      L = __find_pow_of_2_not_below(temp);
    mov     r11,r0
                                                //      S -> r12
    mov     r0,r6
    bl      __find_pow_of_2_not_below           //      S = __find_pow_of_2_not_below(set_count);
    mov     r12,r0

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

        mcr     p15, 0, r0, c7, c6, 2           //              DCISW = data;
        mcr     p15, 0, r0, c7, c10, 4          //              DSB

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

    pop    {r4-r12,lr}                      //恢复寄存器上下文
    bx      lr
    __FUNC_END

    __FUNC_DECL(arch_mmu_dcache_enable)
arch_mmu_dcache_enable:
    push    {lr}

    mrc     p15, 0, r0, c1, c0, 0
    tst     r0, #__P15_R1_C
    bne     1f;                         // 如果dcache已经启用则退出

    bl      arch_mmu_dcache_invalidate
    mrc     p15, 0, r0, c1, c0, 0
    orr     r0, r0, #__P15_R1_C
    mcr     p15, 0, r0, c1, c0, 0

1:
    mcr     p15, 0, r0, c7, c10, 4      //DSB
    pop     {lr}
    bx      lr
    __FUNC_END

    __FUNC_DECL(arch_mmu_dcache_disable)
arch_mmu_dcache_disable:
    push    {lr}

    mrc     p15, 0, r0, c1, c0, 0
    tst     r0, #__P15_R1_C
    beq     1f;                         // 如果dcache已经禁用则退出

    bl      arch_mmu_dcache_clean
    mrc     p15, 0, r0, c1, c0, 0
    bic     r0, r0, #__P15_R1_C
    mcr     p15, 0, r0, c1, c0, 0
1:
    mcr     p15, 0, r0, c7, c10, 4      //DSB
    pop     {pc}
    __FUNC_END
    .end

/* end of file */
