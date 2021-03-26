/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Stock Co.,  Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief 
 *
 * \internal
 * \par Modification history
 * - 1.00 20-10-23  win, first implementation.
 * \endinternal
 */

#include "aw_mp3_internal.h"

/**
 * \brief FPM_DEFAULT without OPT_SSO will actually
 *        lose accuracy and performance
 */

#if (AW_MP3_FPM_DEFAULT) && !(AW_MP3_DEC_OPT_SSO)
#define AW_MP3_DEC_OPT_SSO     1
#endif

/** \brief second SSO shift, with rounding */

#if (AW_MP3_DEC_OPT_SSO)
#define AW_MP3_SHIFT(x)  (((x) + (1L << 11)) >> 12)
#else
#define AW_MP3_SHIFT(x)  (x)
#endif

/** \brief possible DCT speed optimization */

#if (AW_MP3_DEC_OPT_SPEED) && defined(AW_MP3_F_MLX)
#define AW_MP3_DEC_OPT_DCTO
#define AW_MP3_AW_MP3_MUL(x, y)  \
    ({                    \
         int64_t hi;      \
         int64_t lo;      \
         AW_MP3_F_MLX(hi, lo, (x), (y));  \
         hi << (32 - AW_MP3_F_SCALEBITS - 3);  \
    })
#else
#undef AW_MP3_DEC_OPT_DCTO
#define AW_MP3_MUL(x, y)  AW_MP3_F_MUL((x), (y))
#endif

/**
 * \brief perform fast in[32]->out[32] DCT
 */
static void __synth_dct32 (const int32_t in[32],
                           uint32_t      slot,
                           int32_t       lo[16][8],
                           int32_t       hi[16][8])
{
    int32_t t0,   t1,   t2,   t3,   t4,   t5,   t6,   t7;
    int32_t t8,   t9,   t10,  t11,  t12,  t13,  t14,  t15;
    int32_t t16,  t17,  t18,  t19,  t20,  t21,  t22,  t23;
    int32_t t24,  t25,  t26,  t27,  t28,  t29,  t30,  t31;
    int32_t t32,  t33,  t34,  t35,  t36,  t37,  t38,  t39;
    int32_t t40,  t41,  t42,  t43,  t44,  t45,  t46,  t47;
    int32_t t48,  t49,  t50,  t51,  t52,  t53,  t54,  t55;
    int32_t t56,  t57,  t58,  t59,  t60,  t61,  t62,  t63;
    int32_t t64,  t65,  t66,  t67,  t68,  t69,  t70,  t71;
    int32_t t72,  t73,  t74,  t75,  t76,  t77,  t78,  t79;
    int32_t t80,  t81,  t82,  t83,  t84,  t85,  t86,  t87;
    int32_t t88,  t89,  t90,  t91,  t92,  t93,  t94,  t95;
    int32_t t96,  t97,  t98,  t99,  t100, t101, t102, t103;
    int32_t t104, t105, t106, t107, t108, t109, t110, t111;
    int32_t t112, t113, t114, t115, t116, t117, t118, t119;
    int32_t t120, t121, t122, t123, t124, t125, t126, t127;
    int32_t t128, t129, t130, t131, t132, t133, t134, t135;
    int32_t t136, t137, t138, t139, t140, t141, t142, t143;
    int32_t t144, t145, t146, t147, t148, t149, t150, t151;
    int32_t t152, t153, t154, t155, t156, t157, t158, t159;
    int32_t t160, t161, t162, t163, t164, t165, t166, t167;
    int32_t t168, t169, t170, t171, t172, t173, t174, t175;
    int32_t t176;

    /* costab[i] = cos(PI / (2 * 32) * i) */

#if (AW_MP3_DEC_OPT_DCTO)
#define COSTAB1   AW_MP3_F(0x7fd8878e)
#define COSTAB2   AW_MP3_F(0x7f62368f)
#define COSTAB3   AW_MP3_F(0x7e9d55fc)
#define COSTAB4   AW_MP3_F(0x7d8a5f40)
#define COSTAB5   AW_MP3_F(0x7c29fbee)
#define COSTAB6   AW_MP3_F(0x7a7d055b)
#define COSTAB7   AW_MP3_F(0x78848414)
#define COSTAB8   AW_MP3_F(0x7641af3d)
#define COSTAB9   AW_MP3_F(0x73b5ebd1)
#define COSTAB10  AW_MP3_F(0x70e2cbc6)
#define COSTAB11  AW_MP3_F(0x6dca0d14)
#define COSTAB12  AW_MP3_F(0x6a6d98a4)
#define COSTAB13  AW_MP3_F(0x66cf8120)
#define COSTAB14  AW_MP3_F(0x62f201ac)
#define COSTAB15  AW_MP3_F(0x5ed77c8a)
#define COSTAB16  AW_MP3_F(0x5a82799a)
#define COSTAB17  AW_MP3_F(0x55f5a4d2)
#define COSTAB18  AW_MP3_F(0x5133cc94)
#define COSTAB19  AW_MP3_F(0x4c3fdff4)
#define COSTAB20  AW_MP3_F(0x471cece7)
#define COSTAB21  AW_MP3_F(0x41ce1e65)
#define COSTAB22  AW_MP3_F(0x3c56ba70)
#define COSTAB23  AW_MP3_F(0x36ba2014)
#define COSTAB24  AW_MP3_F(0x30fbc54d)
#define COSTAB25  AW_MP3_F(0x2b1f34eb)
#define COSTAB26  AW_MP3_F(0x25280c5e)
#define COSTAB27  AW_MP3_F(0x1f19f97b)
#define COSTAB28  AW_MP3_F(0x18f8b83c)
#define COSTAB29  AW_MP3_F(0x12c8106f)
#define COSTAB30  AW_MP3_F(0x0c8bd35e)
#define COSTAB31  AW_MP3_F(0x0647d97c)
#else
#define COSTAB1   AW_MP3_F(0x0ffb10f2)  /* 0.998795456 */
#define COSTAB2   AW_MP3_F(0x0fec46d2)  /* 0.995184727 */
#define COSTAB3   AW_MP3_F(0x0fd3aac0)  /* 0.989176510 */
#define COSTAB4   AW_MP3_F(0x0fb14be8)  /* 0.980785280 */
#define COSTAB5   AW_MP3_F(0x0f853f7e)  /* 0.970031253 */
#define COSTAB6   AW_MP3_F(0x0f4fa0ab)  /* 0.956940336 */
#define COSTAB7   AW_MP3_F(0x0f109082)  /* 0.941544065 */
#define COSTAB8   AW_MP3_F(0x0ec835e8)  /* 0.923879533 */
#define COSTAB9   AW_MP3_F(0x0e76bd7a)  /* 0.903989293 */
#define COSTAB10  AW_MP3_F(0x0e1c5979)  /* 0.881921264 */
#define COSTAB11  AW_MP3_F(0x0db941a3)  /* 0.857728610 */
#define COSTAB12  AW_MP3_F(0x0d4db315)  /* 0.831469612 */
#define COSTAB13  AW_MP3_F(0x0cd9f024)  /* 0.803207531 */
#define COSTAB14  AW_MP3_F(0x0c5e4036)  /* 0.773010453 */
#define COSTAB15  AW_MP3_F(0x0bdaef91)  /* 0.740951125 */
#define COSTAB16  AW_MP3_F(0x0b504f33)  /* 0.707106781 */
#define COSTAB17  AW_MP3_F(0x0abeb49a)  /* 0.671558955 */
#define COSTAB18  AW_MP3_F(0x0a267993)  /* 0.634393284 */
#define COSTAB19  AW_MP3_F(0x0987fbfe)  /* 0.595699304 */
#define COSTAB20  AW_MP3_F(0x08e39d9d)  /* 0.555570233 */
#define COSTAB21  AW_MP3_F(0x0839c3cd)  /* 0.514102744 */
#define COSTAB22  AW_MP3_F(0x078ad74e)  /* 0.471396737 */
#define COSTAB23  AW_MP3_F(0x06d74402)  /* 0.427555093 */
#define COSTAB24  AW_MP3_F(0x061f78aa)  /* 0.382683432 */
#define COSTAB25  AW_MP3_F(0x0563e69d)  /* 0.336889853 */
#define COSTAB26  AW_MP3_F(0x04a5018c)  /* 0.290284677 */
#define COSTAB27  AW_MP3_F(0x03e33f2f)  /* 0.242980180 */
#define COSTAB28  AW_MP3_F(0x031f1708)  /* 0.195090322 */
#define COSTAB29  AW_MP3_F(0x0259020e)  /* 0.146730474 */
#define COSTAB30  AW_MP3_F(0x01917a6c)  /* 0.098017140 */
#define COSTAB31  AW_MP3_F(0x00c8fb30)  /* 0.049067674 */
#endif

    t0   = in[0]  + in[31];
    t16  = AW_MP3_MUL(in[0] - in[31], COSTAB1);

    t1   = in[15] + in[16];
    t17  = AW_MP3_MUL(in[15] - in[16], COSTAB31);

    t41  = t16 + t17;
    t59  = AW_MP3_MUL(t16 - t17, COSTAB2);
    t33  = t0  + t1;
    t50  = AW_MP3_MUL(t0  - t1,  COSTAB2);

    t2   = in[7]  + in[24];  t18  = AW_MP3_MUL(in[7]  - in[24], COSTAB15);
    t3   = in[8]  + in[23];  t19  = AW_MP3_MUL(in[8]  - in[23], COSTAB17);

    t42  = t18 + t19;
    t60  = AW_MP3_MUL(t18 - t19, COSTAB30);
    t34  = t2  + t3;
    t51  = AW_MP3_MUL(t2  - t3,  COSTAB30);

    t4   = in[3]  + in[28];  t20  = AW_MP3_MUL(in[3]  - in[28], COSTAB7);
    t5   = in[12] + in[19];  t21  = AW_MP3_MUL(in[12] - in[19], COSTAB25);

    t43  = t20 + t21;
    t61  = AW_MP3_MUL(t20 - t21, COSTAB14);
    t35  = t4  + t5;
    t52  = AW_MP3_MUL(t4  - t5,  COSTAB14);

    t6   = in[4]  + in[27];  t22  = AW_MP3_MUL(in[4]  - in[27], COSTAB9);
    t7   = in[11] + in[20];  t23  = AW_MP3_MUL(in[11] - in[20], COSTAB23);

    t44  = t22 + t23;
    t62  = AW_MP3_MUL(t22 - t23, COSTAB18);
    t36  = t6  + t7;
    t53  = AW_MP3_MUL(t6  - t7,  COSTAB18);

    t8   = in[1]  + in[30];  t24  = AW_MP3_MUL(in[1]  - in[30], COSTAB3);
    t9   = in[14] + in[17];  t25  = AW_MP3_MUL(in[14] - in[17], COSTAB29);

    t45  = t24 + t25;
    t63  = AW_MP3_MUL(t24 - t25, COSTAB6);
    t37  = t8  + t9;
    t54  = AW_MP3_MUL(t8  - t9,  COSTAB6);

    t10  = in[6]  + in[25];  t26  = AW_MP3_MUL(in[6]  - in[25], COSTAB13);
    t11  = in[9]  + in[22];  t27  = AW_MP3_MUL(in[9]  - in[22], COSTAB19);

    t46  = t26 + t27;
    t64  = AW_MP3_MUL(t26 - t27, COSTAB26);
    t38  = t10 + t11;
    t55  = AW_MP3_MUL(t10 - t11, COSTAB26);

    t12  = in[2]  + in[29];  t28  = AW_MP3_MUL(in[2]  - in[29], COSTAB5);
    t13  = in[13] + in[18];  t29  = AW_MP3_MUL(in[13] - in[18], COSTAB27);

    t47  = t28 + t29;
    t65  = AW_MP3_MUL(t28 - t29, COSTAB10);
    t39  = t12 + t13;
    t56  = AW_MP3_MUL(t12 - t13, COSTAB10);

    t14  = in[5]  + in[26];  t30  = AW_MP3_MUL(in[5]  - in[26], COSTAB11);
    t15  = in[10] + in[21];  t31  = AW_MP3_MUL(in[10] - in[21], COSTAB21);

    t48  = t30 + t31;
    t66  = AW_MP3_MUL(t30 - t31, COSTAB22);
    t40  = t14 + t15;
    t57  = AW_MP3_MUL(t14 - t15, COSTAB22);

    t69  = t33 + t34;  t89  = AW_MP3_MUL(t33 - t34, COSTAB4);
    t70  = t35 + t36;  t90  = AW_MP3_MUL(t35 - t36, COSTAB28);
    t71  = t37 + t38;  t91  = AW_MP3_MUL(t37 - t38, COSTAB12);
    t72  = t39 + t40;  t92  = AW_MP3_MUL(t39 - t40, COSTAB20);
    t73  = t41 + t42;  t94  = AW_MP3_MUL(t41 - t42, COSTAB4);
    t74  = t43 + t44;  t95  = AW_MP3_MUL(t43 - t44, COSTAB28);
    t75  = t45 + t46;  t96  = AW_MP3_MUL(t45 - t46, COSTAB12);
    t76  = t47 + t48;  t97  = AW_MP3_MUL(t47 - t48, COSTAB20);

    t78  = t50 + t51;  t100 = AW_MP3_MUL(t50 - t51, COSTAB4);
    t79  = t52 + t53;  t101 = AW_MP3_MUL(t52 - t53, COSTAB28);
    t80  = t54 + t55;  t102 = AW_MP3_MUL(t54 - t55, COSTAB12);
    t81  = t56 + t57;  t103 = AW_MP3_MUL(t56 - t57, COSTAB20);

    t83  = t59 + t60;  t106 = AW_MP3_MUL(t59 - t60, COSTAB4);
    t84  = t61 + t62;  t107 = AW_MP3_MUL(t61 - t62, COSTAB28);
    t85  = t63 + t64;  t108 = AW_MP3_MUL(t63 - t64, COSTAB12);
    t86  = t65 + t66;  t109 = AW_MP3_MUL(t65 - t66, COSTAB20);

    t113 = t69  + t70;
    t114 = t71  + t72;

    /*  0 */
    hi[15][slot] = AW_MP3_SHIFT(t113 + t114);

    /* 16 */
    lo[ 0][slot] = AW_MP3_SHIFT(AW_MP3_MUL(t113 - t114, COSTAB16));

    t115 = t73  + t74;
    t116 = t75  + t76;

    t32  = t115 + t116;

    /*  1 */
    hi[14][slot] = AW_MP3_SHIFT(t32);

    t118 = t78  + t79;
    t119 = t80  + t81;

    t58  = t118 + t119;

    /*  2 */
    hi[13][slot] = AW_MP3_SHIFT(t58);

    t121 = t83  + t84;
    t122 = t85  + t86;

    t67  = t121 + t122;

    t49  = (t67 * 2) - t32;

    /*  3 */
    hi[12][slot] = AW_MP3_SHIFT(t49);

    t125 = t89  + t90;
    t126 = t91  + t92;

    t93  = t125 + t126;

    /*  4 */
    hi[11][slot] = AW_MP3_SHIFT(t93);

    t128 = t94  + t95;
    t129 = t96  + t97;

    t98  = t128 + t129;

    t68  = (t98 * 2) - t49;

    /*  5 */
    hi[10][slot] = AW_MP3_SHIFT(t68);

    t132 = t100 + t101;
    t133 = t102 + t103;

    t104 = t132 + t133;

    t82  = (t104 * 2) - t58;

    /*  6 */
    hi[ 9][slot] = AW_MP3_SHIFT(t82);

    t136 = t106 + t107;
    t137 = t108 + t109;

    t110 = t136 + t137;

    t87  = (t110 * 2) - t67;

    t77  = (t87 * 2) - t68;

    /*  7 */
    hi[ 8][slot] = AW_MP3_SHIFT(t77);

    t141 = AW_MP3_MUL(t69 - t70, COSTAB8);
    t142 = AW_MP3_MUL(t71 - t72, COSTAB24);
    t143 = t141 + t142;

    /*  8 */
    hi[ 7][slot] = AW_MP3_SHIFT(t143);

    /* 24 */
    lo[ 8][slot] = AW_MP3_SHIFT((AW_MP3_MUL(t141 - t142, COSTAB16) * 2) - t143);

    t144 = AW_MP3_MUL(t73 - t74, COSTAB8);
    t145 = AW_MP3_MUL(t75 - t76, COSTAB24);
    t146 = t144 + t145;

    t88  = (t146 * 2) - t77;

    /*  9 */
    hi[ 6][slot] = AW_MP3_SHIFT(t88);

    t148 = AW_MP3_MUL(t78 - t79, COSTAB8);
    t149 = AW_MP3_MUL(t80 - t81, COSTAB24);
    t150 = t148 + t149;

    t105 = (t150 * 2) - t82;

    /* 10 */
    hi[ 5][slot] = AW_MP3_SHIFT(t105);

    t152 = AW_MP3_MUL(t83 - t84, COSTAB8);
    t153 = AW_MP3_MUL(t85 - t86, COSTAB24);
    t154 = t152 + t153;

    t111 = (t154 * 2) - t87;

    t99  = (t111 * 2) - t88;

    /* 11 */
    hi[ 4][slot] = AW_MP3_SHIFT(t99);

    t157 = AW_MP3_MUL(t89 - t90, COSTAB8);
    t158 = AW_MP3_MUL(t91 - t92, COSTAB24);
    t159 = t157 + t158;

    t127 = (t159 * 2) - t93;

    /* 12 */
    hi[ 3][slot] = AW_MP3_SHIFT(t127);

    t160 = (AW_MP3_MUL(t125 - t126, COSTAB16) * 2) - t127;

    /* 20 */
    lo[ 4][slot] = AW_MP3_SHIFT(t160);

    /* 28 */
    lo[12][slot] = AW_MP3_SHIFT((((AW_MP3_MUL(t157 - t158, COSTAB16) * 2) -
                   t159) * 2) - t160);

    t161 = AW_MP3_MUL(t94 - t95, COSTAB8);
    t162 = AW_MP3_MUL(t96 - t97, COSTAB24);
    t163 = t161 + t162;

    t130 = (t163 * 2) - t98;

    t112 = (t130 * 2) - t99;

    /* 13 */
    hi[ 2][slot] = AW_MP3_SHIFT(t112);

    t164 = (AW_MP3_MUL(t128 - t129, COSTAB16) * 2) - t130;

    t166 = AW_MP3_MUL(t100 - t101, COSTAB8);
    t167 = AW_MP3_MUL(t102 - t103, COSTAB24);
    t168 = t166 + t167;

    t134 = (t168 * 2) - t104;

    t120 = (t134 * 2) - t105;

    /* 14 */
    hi[1][slot] = AW_MP3_SHIFT(t120);

    t135 = (AW_MP3_MUL(t118 - t119, COSTAB16) * 2) - t120;

    /* 18 */
    lo[ 2][slot] = AW_MP3_SHIFT(t135);

    t169 = (AW_MP3_MUL(t132 - t133, COSTAB16) * 2) - t134;

    t151 = (t169 * 2) - t135;

    /* 22 */
    lo[ 6][slot] = AW_MP3_SHIFT(t151);

    t170 = (((AW_MP3_MUL(t148 - t149, COSTAB16) * 2) - t150) * 2) - t151;

    /* 26 */
    lo[10][slot] = AW_MP3_SHIFT(t170);

    /* 30 */
    lo[14][slot] = AW_MP3_SHIFT((((((AW_MP3_MUL(t166 - t167, COSTAB16) * 2) -
                   t168) * 2) - t169) * 2) - t170);

    t171 = AW_MP3_MUL(t106 - t107, COSTAB8);
    t172 = AW_MP3_MUL(t108 - t109, COSTAB24);
    t173 = t171 + t172;

    t138 = (t173 * 2) - t110;

    t123 = (t138 * 2) - t111;

    t139 = (AW_MP3_MUL(t121 - t122, COSTAB16) * 2) - t123;

    t117 = (t123 * 2) - t112;

    /* 15 */
    hi[ 0][slot] = AW_MP3_SHIFT(t117);

    t124 = (AW_MP3_MUL(t115 - t116, COSTAB16) * 2) - t117;

    /* 17 */
    lo[ 1][slot] = AW_MP3_SHIFT(t124);

    t131 = (t139 * 2) - t124;

    /* 19 */
    lo[ 3][slot] = AW_MP3_SHIFT(t131);

    t140 = (t164 * 2) - t131;

    /* 21 */
    lo[ 5][slot] = AW_MP3_SHIFT(t140);

    t174 = (AW_MP3_MUL(t136 - t137, COSTAB16) * 2) - t138;

    t155 = (t174 * 2) - t139;

    t147 = (t155 * 2) - t140;

    /* 23 */
    lo[ 7][slot] = AW_MP3_SHIFT(t147);

    t156 = (((AW_MP3_MUL(t144 - t145, COSTAB16) * 2) - t146) * 2) - t147;

    /* 25 */
    lo[ 9][slot] = AW_MP3_SHIFT(t156);

    t175 = (((AW_MP3_MUL(t152 - t153, COSTAB16) * 2) - t154) * 2) - t155;

    t165 = (t175 * 2) - t156;

    /* 27 */
    lo[11][slot] = AW_MP3_SHIFT(t165);

    t176 = (((((AW_MP3_MUL(t161 - t162, COSTAB16) * 2) -
             t163) * 2) - t164) * 2) - t165;

    /* 29 */
    lo[13][slot] = AW_MP3_SHIFT(t176);

    /* 31 */
    lo[15][slot] = AW_MP3_SHIFT((((((((AW_MP3_MUL(t171 - t172, COSTAB16) * 2) -
                   t173) * 2) - t174) * 2) - t175) * 2) - t176);

    /*
     * Totals:
     *  80 AW_MP3_MULtiplies
     *  80 additions
     * 119 subtractions
     *  49 shifts (not counting SSO)
     */
}

#undef AW_MP3_MUL
#undef AW_MP3_SHIFT

/** \brief third SSO shift and/or D[] optimization preshift */
#if (AW_MP3_DEC_OPT_SSO)

#if AW_MP3_F_FRACBITS != 28
#error "MAD_F_FRACBITS must be 28 to use OPT_SSO"
#endif

#define AW_MP3_ML0(hi, lo, x, y) ((lo)  = (x) * (y))
#define AW_MP3_MLA(hi, lo, x, y) ((lo) += (x) * (y))
#define AW_MP3_MLN(hi, lo)       ((lo)  = -(lo))
#define AW_MP3_MLZ(hi, lo)       ((void) (hi), (int32_t) (lo))
#define AW_MP3_SHIFT(x)          ((x) >> 2)
#define AW_MP3_PRESHIFT(x)       ((AW_MP3_F(x) + (1L << 13)) >> 14)

#else

#define AW_MP3_ML0(hi, lo, x, y) AW_MP3_F_ML0((hi), (lo), (x), (y))
#define AW_MP3_MLA(hi, lo, x, y) AW_MP3_F_MLA((hi), (lo), (x), (y))
#define AW_MP3_MLN(hi, lo)       AW_MP3_F_MLN((hi), (lo))
#define AW_MP3_MLZ(hi, lo)       AW_MP3_F_MLZ((hi), (lo))
#define AW_MP3_SHIFT(x)          (x)

#if (AW_MP3_F_SCALEBITS)
#undef  AW_MP3_F_SCALEBITS
#define AW_MP3_F_SCALEBITS      (AW_MP3_F_FRACBITS - 12)
#define AW_MP3_PRESHIFT(x)      (AW_MP3_F(x) >> 12)
#else
#define AW_MP3_PRESHIFT(x)       AW_MP3_F(x)
#endif

#endif

static const int32_t __g_synth_d[17][32] = {

    /*
     * These are the coefficients for the subband synthesis window. This is a
     * reordered version of Table B.3 from ISO/IEC 11172-3.
     *
     * Every value is parameterized so that shift optimizations can be made at
     * compile-time. For example, every value can be right-shifted 12 bits to
     * minimize multiply instruction times without any loss of accuracy.
     */

    /*  0 */
    {
        AW_MP3_PRESHIFT(0x00000000)  /*  0.000000000 */,
        -AW_MP3_PRESHIFT(0x0001d000) /* -0.000442505 */,
         AW_MP3_PRESHIFT(0x000d5000) /*  0.003250122 */,
        -AW_MP3_PRESHIFT(0x001cb000) /* -0.007003784 */,
         AW_MP3_PRESHIFT(0x007f5000) /*  0.031082153 */,
        -AW_MP3_PRESHIFT(0x01421000) /* -0.078628540 */,
         AW_MP3_PRESHIFT(0x019ae000) /*  0.100311279 */,
        -AW_MP3_PRESHIFT(0x09271000) /* -0.572036743 */,
         AW_MP3_PRESHIFT(0x1251e000) /*  1.144989014 */,
         AW_MP3_PRESHIFT(0x09271000) /*  0.572036743 */,
         AW_MP3_PRESHIFT(0x019ae000) /*  0.100311279 */,
         AW_MP3_PRESHIFT(0x01421000) /*  0.078628540 */,
         AW_MP3_PRESHIFT(0x007f5000) /*  0.031082153 */,
         AW_MP3_PRESHIFT(0x001cb000) /*  0.007003784 */,
         AW_MP3_PRESHIFT(0x000d5000) /*  0.003250122 */,
         AW_MP3_PRESHIFT(0x0001d000) /*  0.000442505 */,

         AW_MP3_PRESHIFT(0x00000000) /*  0.000000000 */,
        -AW_MP3_PRESHIFT(0x0001d000) /* -0.000442505 */,
         AW_MP3_PRESHIFT(0x000d5000) /*  0.003250122 */,
        -AW_MP3_PRESHIFT(0x001cb000) /* -0.007003784 */,
         AW_MP3_PRESHIFT(0x007f5000) /*  0.031082153 */,
        -AW_MP3_PRESHIFT(0x01421000) /* -0.078628540 */,
         AW_MP3_PRESHIFT(0x019ae000) /*  0.100311279 */,
        -AW_MP3_PRESHIFT(0x09271000) /* -0.572036743 */,
         AW_MP3_PRESHIFT(0x1251e000) /*  1.144989014 */,
         AW_MP3_PRESHIFT(0x09271000) /*  0.572036743 */,
         AW_MP3_PRESHIFT(0x019ae000) /*  0.100311279 */,
         AW_MP3_PRESHIFT(0x01421000) /*  0.078628540 */,
         AW_MP3_PRESHIFT(0x007f5000) /*  0.031082153 */,
         AW_MP3_PRESHIFT(0x001cb000) /*  0.007003784 */,
         AW_MP3_PRESHIFT(0x000d5000) /*  0.003250122 */,
         AW_MP3_PRESHIFT(0x0001d000) /*  0.000442505 */,
    },

    /*  1 */
    {
        -AW_MP3_PRESHIFT(0x00001000) /* -0.000015259 */,
        -AW_MP3_PRESHIFT(0x0001f000) /* -0.000473022 */,
         AW_MP3_PRESHIFT(0x000da000) /*  0.003326416 */,
        -AW_MP3_PRESHIFT(0x00207000) /* -0.007919312 */,
         AW_MP3_PRESHIFT(0x007d0000) /*  0.030517578 */,
        -AW_MP3_PRESHIFT(0x0158d000) /* -0.084182739 */,
         AW_MP3_PRESHIFT(0x01747000) /*  0.090927124 */,
        -AW_MP3_PRESHIFT(0x099a8000) /* -0.600219727 */,
         AW_MP3_PRESHIFT(0x124f0000) /*  1.144287109 */,
         AW_MP3_PRESHIFT(0x08b38000) /*  0.543823242 */,
         AW_MP3_PRESHIFT(0x01bde000) /*  0.108856201 */,
         AW_MP3_PRESHIFT(0x012b4000) /*  0.073059082 */,
         AW_MP3_PRESHIFT(0x0080f000) /*  0.031478882 */,
         AW_MP3_PRESHIFT(0x00191000) /*  0.006118774 */,
         AW_MP3_PRESHIFT(0x000d0000) /*  0.003173828 */,
         AW_MP3_PRESHIFT(0x0001a000) /*  0.000396729 */,

        -AW_MP3_PRESHIFT(0x00001000) /* -0.000015259 */,
        -AW_MP3_PRESHIFT(0x0001f000) /* -0.000473022 */,
         AW_MP3_PRESHIFT(0x000da000) /*  0.003326416 */,
        -AW_MP3_PRESHIFT(0x00207000) /* -0.007919312 */,
         AW_MP3_PRESHIFT(0x007d0000) /*  0.030517578 */,
        -AW_MP3_PRESHIFT(0x0158d000) /* -0.084182739 */,
         AW_MP3_PRESHIFT(0x01747000) /*  0.090927124 */,
        -AW_MP3_PRESHIFT(0x099a8000) /* -0.600219727 */,
         AW_MP3_PRESHIFT(0x124f0000) /*  1.144287109 */,
         AW_MP3_PRESHIFT(0x08b38000) /*  0.543823242 */,
         AW_MP3_PRESHIFT(0x01bde000) /*  0.108856201 */,
         AW_MP3_PRESHIFT(0x012b4000) /*  0.073059082 */,
         AW_MP3_PRESHIFT(0x0080f000) /*  0.031478882 */,
         AW_MP3_PRESHIFT(0x00191000) /*  0.006118774 */,
         AW_MP3_PRESHIFT(0x000d0000) /*  0.003173828 */,
         AW_MP3_PRESHIFT(0x0001a000) /*  0.000396729 */,
    },

    /*  2 */
    {
        -AW_MP3_PRESHIFT(0x00001000) /* -0.000015259 */,
        -AW_MP3_PRESHIFT(0x00023000) /* -0.000534058 */,
         AW_MP3_PRESHIFT(0x000de000) /*  0.003387451 */,
        -AW_MP3_PRESHIFT(0x00245000) /* -0.008865356 */,
         AW_MP3_PRESHIFT(0x007a0000) /*  0.029785156 */,
        -AW_MP3_PRESHIFT(0x016f7000) /* -0.089706421 */,
         AW_MP3_PRESHIFT(0x014a8000) /*  0.080688477 */,
        -AW_MP3_PRESHIFT(0x0a0d8000) /* -0.628295898 */,
         AW_MP3_PRESHIFT(0x12468000) /*  1.142211914 */,
         AW_MP3_PRESHIFT(0x083ff000) /*  0.515609741 */,
         AW_MP3_PRESHIFT(0x01dd8000) /*  0.116577148 */,
         AW_MP3_PRESHIFT(0x01149000) /*  0.067520142 */,
         AW_MP3_PRESHIFT(0x00820000) /*  0.031738281 */,
         AW_MP3_PRESHIFT(0x0015b000) /*  0.005294800 */,
         AW_MP3_PRESHIFT(0x000ca000) /*  0.003082275 */,
         AW_MP3_PRESHIFT(0x00018000) /*  0.000366211 */,

        -AW_MP3_PRESHIFT(0x00001000) /* -0.000015259 */,
        -AW_MP3_PRESHIFT(0x00023000) /* -0.000534058 */,
         AW_MP3_PRESHIFT(0x000de000) /*  0.003387451 */,
        -AW_MP3_PRESHIFT(0x00245000) /* -0.008865356 */,
         AW_MP3_PRESHIFT(0x007a0000) /*  0.029785156 */,
        -AW_MP3_PRESHIFT(0x016f7000) /* -0.089706421 */,
         AW_MP3_PRESHIFT(0x014a8000) /*  0.080688477 */,
        -AW_MP3_PRESHIFT(0x0a0d8000) /* -0.628295898 */,
         AW_MP3_PRESHIFT(0x12468000) /*  1.142211914 */,
         AW_MP3_PRESHIFT(0x083ff000) /*  0.515609741 */,
         AW_MP3_PRESHIFT(0x01dd8000) /*  0.116577148 */,
         AW_MP3_PRESHIFT(0x01149000) /*  0.067520142 */,
         AW_MP3_PRESHIFT(0x00820000) /*  0.031738281 */,
         AW_MP3_PRESHIFT(0x0015b000) /*  0.005294800 */,
         AW_MP3_PRESHIFT(0x000ca000) /*  0.003082275 */,
         AW_MP3_PRESHIFT(0x00018000) /*  0.000366211 */,
    },

    /*  3 */
    {
        -AW_MP3_PRESHIFT(0x00001000) /* -0.000015259 */,
        -AW_MP3_PRESHIFT(0x00026000) /* -0.000579834 */,
         AW_MP3_PRESHIFT(0x000e1000) /*  0.003433228 */,
        -AW_MP3_PRESHIFT(0x00285000) /* -0.009841919 */,
         AW_MP3_PRESHIFT(0x00765000) /*  0.028884888 */,
        -AW_MP3_PRESHIFT(0x0185d000) /* -0.095169067 */,
         AW_MP3_PRESHIFT(0x011d1000) /*  0.069595337 */,
        -AW_MP3_PRESHIFT(0x0a7fe000) /* -0.656219482 */,
         AW_MP3_PRESHIFT(0x12386000) /*  1.138763428 */,
         AW_MP3_PRESHIFT(0x07ccb000) /*  0.487472534 */,
         AW_MP3_PRESHIFT(0x01f9c000) /*  0.123474121 */,
         AW_MP3_PRESHIFT(0x00fdf000) /*  0.061996460 */,
         AW_MP3_PRESHIFT(0x00827000) /*  0.031845093 */,
         AW_MP3_PRESHIFT(0x00126000) /*  0.004486084 */,
         AW_MP3_PRESHIFT(0x000c4000) /*  0.002990723 */,
         AW_MP3_PRESHIFT(0x00015000) /*  0.000320435 */,

        -AW_MP3_PRESHIFT(0x00001000) /* -0.000015259 */,
        -AW_MP3_PRESHIFT(0x00026000) /* -0.000579834 */,
         AW_MP3_PRESHIFT(0x000e1000) /*  0.003433228 */,
        -AW_MP3_PRESHIFT(0x00285000) /* -0.009841919 */,
         AW_MP3_PRESHIFT(0x00765000) /*  0.028884888 */,
        -AW_MP3_PRESHIFT(0x0185d000) /* -0.095169067 */,
         AW_MP3_PRESHIFT(0x011d1000) /*  0.069595337 */,
        -AW_MP3_PRESHIFT(0x0a7fe000) /* -0.656219482 */,
         AW_MP3_PRESHIFT(0x12386000) /*  1.138763428 */,
         AW_MP3_PRESHIFT(0x07ccb000) /*  0.487472534 */,
         AW_MP3_PRESHIFT(0x01f9c000) /*  0.123474121 */,
         AW_MP3_PRESHIFT(0x00fdf000) /*  0.061996460 */,
         AW_MP3_PRESHIFT(0x00827000) /*  0.031845093 */,
         AW_MP3_PRESHIFT(0x00126000) /*  0.004486084 */,
         AW_MP3_PRESHIFT(0x000c4000) /*  0.002990723 */,
         AW_MP3_PRESHIFT(0x00015000) /*  0.000320435 */,
    },

    /*  4 */
    {
        -AW_MP3_PRESHIFT(0x00001000) /* -0.000015259 */,
        -AW_MP3_PRESHIFT(0x00029000) /* -0.000625610 */,
         AW_MP3_PRESHIFT(0x000e3000) /*  0.003463745 */,
        -AW_MP3_PRESHIFT(0x002c7000) /* -0.010848999 */,
         AW_MP3_PRESHIFT(0x0071e000) /*  0.027801514 */,
        -AW_MP3_PRESHIFT(0x019bd000) /* -0.100540161 */,
         AW_MP3_PRESHIFT(0x00ec0000) /*  0.057617187 */,
        -AW_MP3_PRESHIFT(0x0af15000) /* -0.683914185 */,
         AW_MP3_PRESHIFT(0x12249000) /*  1.133926392 */,
         AW_MP3_PRESHIFT(0x075a0000) /*  0.459472656 */,
         AW_MP3_PRESHIFT(0x0212c000) /*  0.129577637 */,
         AW_MP3_PRESHIFT(0x00e79000) /*  0.056533813 */,
         AW_MP3_PRESHIFT(0x00825000) /*  0.031814575 */,
         AW_MP3_PRESHIFT(0x000f4000) /*  0.003723145 */,
         AW_MP3_PRESHIFT(0x000be000) /*  0.002899170 */,
         AW_MP3_PRESHIFT(0x00013000) /*  0.000289917 */,

        -AW_MP3_PRESHIFT(0x00001000) /* -0.000015259 */,
        -AW_MP3_PRESHIFT(0x00029000) /* -0.000625610 */,
         AW_MP3_PRESHIFT(0x000e3000) /*  0.003463745 */,
        -AW_MP3_PRESHIFT(0x002c7000) /* -0.010848999 */,
         AW_MP3_PRESHIFT(0x0071e000) /*  0.027801514 */,
        -AW_MP3_PRESHIFT(0x019bd000) /* -0.100540161 */,
         AW_MP3_PRESHIFT(0x00ec0000) /*  0.057617187 */,
        -AW_MP3_PRESHIFT(0x0af15000) /* -0.683914185 */,
         AW_MP3_PRESHIFT(0x12249000) /*  1.133926392 */,
         AW_MP3_PRESHIFT(0x075a0000) /*  0.459472656 */,
         AW_MP3_PRESHIFT(0x0212c000) /*  0.129577637 */,
         AW_MP3_PRESHIFT(0x00e79000) /*  0.056533813 */,
         AW_MP3_PRESHIFT(0x00825000) /*  0.031814575 */,
         AW_MP3_PRESHIFT(0x000f4000) /*  0.003723145 */,
         AW_MP3_PRESHIFT(0x000be000) /*  0.002899170 */,
         AW_MP3_PRESHIFT(0x00013000) /*  0.000289917 */,
    },

    /*  5 */
    {
        -AW_MP3_PRESHIFT(0x00001000) /* -0.000015259 */,
        -AW_MP3_PRESHIFT(0x0002d000) /* -0.000686646 */,
         AW_MP3_PRESHIFT(0x000e4000) /*  0.003479004 */,
        -AW_MP3_PRESHIFT(0x0030b000) /* -0.011886597 */,
         AW_MP3_PRESHIFT(0x006cb000) /*  0.026535034 */,
        -AW_MP3_PRESHIFT(0x01b17000) /* -0.105819702 */,
         AW_MP3_PRESHIFT(0x00b77000) /*  0.044784546 */,
        -AW_MP3_PRESHIFT(0x0b619000) /* -0.711318970 */,
         AW_MP3_PRESHIFT(0x120b4000) /*  1.127746582 */,
         AW_MP3_PRESHIFT(0x06e81000) /*  0.431655884 */,
         AW_MP3_PRESHIFT(0x02288000) /*  0.134887695 */,
         AW_MP3_PRESHIFT(0x00d17000) /*  0.051132202 */,
         AW_MP3_PRESHIFT(0x0081b000) /*  0.031661987 */,
         AW_MP3_PRESHIFT(0x000c5000) /*  0.003005981 */,
         AW_MP3_PRESHIFT(0x000b7000) /*  0.002792358 */,
         AW_MP3_PRESHIFT(0x00011000) /*  0.000259399 */,

        -AW_MP3_PRESHIFT(0x00001000) /* -0.000015259 */,
        -AW_MP3_PRESHIFT(0x0002d000) /* -0.000686646 */,
         AW_MP3_PRESHIFT(0x000e4000) /*  0.003479004 */,
        -AW_MP3_PRESHIFT(0x0030b000) /* -0.011886597 */,
         AW_MP3_PRESHIFT(0x006cb000) /*  0.026535034 */,
        -AW_MP3_PRESHIFT(0x01b17000) /* -0.105819702 */,
         AW_MP3_PRESHIFT(0x00b77000) /*  0.044784546 */,
        -AW_MP3_PRESHIFT(0x0b619000) /* -0.711318970 */,
         AW_MP3_PRESHIFT(0x120b4000) /*  1.127746582 */,
         AW_MP3_PRESHIFT(0x06e81000) /*  0.431655884 */,
         AW_MP3_PRESHIFT(0x02288000) /*  0.134887695 */,
         AW_MP3_PRESHIFT(0x00d17000) /*  0.051132202 */,
         AW_MP3_PRESHIFT(0x0081b000) /*  0.031661987 */,
         AW_MP3_PRESHIFT(0x000c5000) /*  0.003005981 */,
         AW_MP3_PRESHIFT(0x000b7000) /*  0.002792358 */,
         AW_MP3_PRESHIFT(0x00011000) /*  0.000259399 */,
    },

    /*  6 */
    {
        -AW_MP3_PRESHIFT(0x00001000) /* -0.000015259 */,
        -AW_MP3_PRESHIFT(0x00031000) /* -0.000747681 */,
         AW_MP3_PRESHIFT(0x000e4000) /*  0.003479004 */,
        -AW_MP3_PRESHIFT(0x00350000) /* -0.012939453 */,
         AW_MP3_PRESHIFT(0x0066c000) /*  0.025085449 */,
        -AW_MP3_PRESHIFT(0x01c67000) /* -0.110946655 */,
         AW_MP3_PRESHIFT(0x007f5000) /*  0.031082153 */,
        -AW_MP3_PRESHIFT(0x0bd06000) /* -0.738372803 */,
         AW_MP3_PRESHIFT(0x11ec7000) /*  1.120223999 */,
         AW_MP3_PRESHIFT(0x06772000) /*  0.404083252 */,
         AW_MP3_PRESHIFT(0x023b3000) /*  0.139450073 */,
         AW_MP3_PRESHIFT(0x00bbc000) /*  0.045837402 */,
         AW_MP3_PRESHIFT(0x00809000) /*  0.031387329 */,
         AW_MP3_PRESHIFT(0x00099000) /*  0.002334595 */,
         AW_MP3_PRESHIFT(0x000b0000) /*  0.002685547 */,
         AW_MP3_PRESHIFT(0x00010000) /*  0.000244141 */,

        -AW_MP3_PRESHIFT(0x00001000) /* -0.000015259 */,
        -AW_MP3_PRESHIFT(0x00031000) /* -0.000747681 */,
         AW_MP3_PRESHIFT(0x000e4000) /*  0.003479004 */,
        -AW_MP3_PRESHIFT(0x00350000) /* -0.012939453 */,
         AW_MP3_PRESHIFT(0x0066c000) /*  0.025085449 */,
        -AW_MP3_PRESHIFT(0x01c67000) /* -0.110946655 */,
         AW_MP3_PRESHIFT(0x007f5000) /*  0.031082153 */,
        -AW_MP3_PRESHIFT(0x0bd06000) /* -0.738372803 */,
         AW_MP3_PRESHIFT(0x11ec7000) /*  1.120223999 */,
         AW_MP3_PRESHIFT(0x06772000) /*  0.404083252 */,
         AW_MP3_PRESHIFT(0x023b3000) /*  0.139450073 */,
         AW_MP3_PRESHIFT(0x00bbc000) /*  0.045837402 */,
         AW_MP3_PRESHIFT(0x00809000) /*  0.031387329 */,
         AW_MP3_PRESHIFT(0x00099000) /*  0.002334595 */,
         AW_MP3_PRESHIFT(0x000b0000) /*  0.002685547 */,
         AW_MP3_PRESHIFT(0x00010000) /*  0.000244141 */,
    },

    /*  7 */
    {
        -AW_MP3_PRESHIFT(0x00002000) /* -0.000030518 */,
        -AW_MP3_PRESHIFT(0x00035000) /* -0.000808716 */,
         AW_MP3_PRESHIFT(0x000e3000) /*  0.003463745 */,
        -AW_MP3_PRESHIFT(0x00397000) /* -0.014022827 */,
         AW_MP3_PRESHIFT(0x005ff000) /*  0.023422241 */,
        -AW_MP3_PRESHIFT(0x01dad000) /* -0.115921021 */,
         AW_MP3_PRESHIFT(0x0043a000) /*  0.016510010 */,
        -AW_MP3_PRESHIFT(0x0c3d9000) /* -0.765029907 */,
         AW_MP3_PRESHIFT(0x11c83000) /*  1.111373901 */,
         AW_MP3_PRESHIFT(0x06076000) /*  0.376800537 */,
         AW_MP3_PRESHIFT(0x024ad000) /*  0.143264771 */,
         AW_MP3_PRESHIFT(0x00a67000) /*  0.040634155 */,
         AW_MP3_PRESHIFT(0x007f0000) /*  0.031005859 */,
         AW_MP3_PRESHIFT(0x0006f000) /*  0.001693726 */,
         AW_MP3_PRESHIFT(0x000a9000) /*  0.002578735 */,
         AW_MP3_PRESHIFT(0x0000e000) /*  0.000213623 */,

        -AW_MP3_PRESHIFT(0x00002000) /* -0.000030518 */,
        -AW_MP3_PRESHIFT(0x00035000) /* -0.000808716 */,
         AW_MP3_PRESHIFT(0x000e3000) /*  0.003463745 */,
        -AW_MP3_PRESHIFT(0x00397000) /* -0.014022827 */,
         AW_MP3_PRESHIFT(0x005ff000) /*  0.023422241 */,
        -AW_MP3_PRESHIFT(0x01dad000) /* -0.115921021 */,
         AW_MP3_PRESHIFT(0x0043a000) /*  0.016510010 */,
        -AW_MP3_PRESHIFT(0x0c3d9000) /* -0.765029907 */,
         AW_MP3_PRESHIFT(0x11c83000) /*  1.111373901 */,
         AW_MP3_PRESHIFT(0x06076000) /*  0.376800537 */,
         AW_MP3_PRESHIFT(0x024ad000) /*  0.143264771 */,
         AW_MP3_PRESHIFT(0x00a67000) /*  0.040634155 */,
         AW_MP3_PRESHIFT(0x007f0000) /*  0.031005859 */,
         AW_MP3_PRESHIFT(0x0006f000) /*  0.001693726 */,
         AW_MP3_PRESHIFT(0x000a9000) /*  0.002578735 */,
         AW_MP3_PRESHIFT(0x0000e000) /*  0.000213623 */,
    },

    /*  8 */
    {
        -AW_MP3_PRESHIFT(0x00002000) /* -0.000030518 */,
        -AW_MP3_PRESHIFT(0x0003a000) /* -0.000885010 */,
         AW_MP3_PRESHIFT(0x000e0000) /*  0.003417969 */,
        -AW_MP3_PRESHIFT(0x003df000) /* -0.015121460 */,
         AW_MP3_PRESHIFT(0x00586000) /*  0.021575928 */,
        -AW_MP3_PRESHIFT(0x01ee6000) /* -0.120697021 */,
         AW_MP3_PRESHIFT(0x00046000) /*  0.001068115 */,
        -AW_MP3_PRESHIFT(0x0ca8d000) /* -0.791213989 */,
         AW_MP3_PRESHIFT(0x119e9000) /*  1.101211548 */,
         AW_MP3_PRESHIFT(0x05991000) /*  0.349868774 */,
         AW_MP3_PRESHIFT(0x02578000) /*  0.146362305 */,
         AW_MP3_PRESHIFT(0x0091a000) /*  0.035552979 */,
         AW_MP3_PRESHIFT(0x007d1000) /*  0.030532837 */,
         AW_MP3_PRESHIFT(0x00048000) /*  0.001098633 */,
         AW_MP3_PRESHIFT(0x000a1000) /*  0.002456665 */,
         AW_MP3_PRESHIFT(0x0000d000) /*  0.000198364 */,

        -AW_MP3_PRESHIFT(0x00002000) /* -0.000030518 */,
        -AW_MP3_PRESHIFT(0x0003a000) /* -0.000885010 */,
         AW_MP3_PRESHIFT(0x000e0000) /*  0.003417969 */,
        -AW_MP3_PRESHIFT(0x003df000) /* -0.015121460 */,
         AW_MP3_PRESHIFT(0x00586000) /*  0.021575928 */,
        -AW_MP3_PRESHIFT(0x01ee6000) /* -0.120697021 */,
         AW_MP3_PRESHIFT(0x00046000) /*  0.001068115 */,
        -AW_MP3_PRESHIFT(0x0ca8d000) /* -0.791213989 */,
         AW_MP3_PRESHIFT(0x119e9000) /*  1.101211548 */,
         AW_MP3_PRESHIFT(0x05991000) /*  0.349868774 */,
         AW_MP3_PRESHIFT(0x02578000) /*  0.146362305 */,
         AW_MP3_PRESHIFT(0x0091a000) /*  0.035552979 */,
         AW_MP3_PRESHIFT(0x007d1000) /*  0.030532837 */,
         AW_MP3_PRESHIFT(0x00048000) /*  0.001098633 */,
         AW_MP3_PRESHIFT(0x000a1000) /*  0.002456665 */,
         AW_MP3_PRESHIFT(0x0000d000) /*  0.000198364 */,
    },

    /*  9 */
    {
        -AW_MP3_PRESHIFT(0x00002000) /* -0.000030518 */,
        -AW_MP3_PRESHIFT(0x0003f000) /* -0.000961304 */,
         AW_MP3_PRESHIFT(0x000dd000) /*  0.003372192 */,
        -AW_MP3_PRESHIFT(0x00428000) /* -0.016235352 */,
         AW_MP3_PRESHIFT(0x00500000) /*  0.019531250 */,
        -AW_MP3_PRESHIFT(0x02011000) /* -0.125259399 */,
        -AW_MP3_PRESHIFT(0x003e6000) /* -0.015228271 */,
        -AW_MP3_PRESHIFT(0x0d11e000) /* -0.816864014 */,
         AW_MP3_PRESHIFT(0x116fc000) /*  1.089782715 */,
         AW_MP3_PRESHIFT(0x052c5000) /*  0.323318481 */,
         AW_MP3_PRESHIFT(0x02616000) /*  0.148773193 */,
         AW_MP3_PRESHIFT(0x007d6000) /*  0.030609131 */,
         AW_MP3_PRESHIFT(0x007aa000) /*  0.029937744 */,
         AW_MP3_PRESHIFT(0x00024000) /*  0.000549316 */,
         AW_MP3_PRESHIFT(0x0009a000) /*  0.002349854 */,
         AW_MP3_PRESHIFT(0x0000b000) /*  0.000167847 */,

        -AW_MP3_PRESHIFT(0x00002000) /* -0.000030518 */,
        -AW_MP3_PRESHIFT(0x0003f000) /* -0.000961304 */,
         AW_MP3_PRESHIFT(0x000dd000) /*  0.003372192 */,
        -AW_MP3_PRESHIFT(0x00428000) /* -0.016235352 */,
         AW_MP3_PRESHIFT(0x00500000) /*  0.019531250 */,
        -AW_MP3_PRESHIFT(0x02011000) /* -0.125259399 */,
        -AW_MP3_PRESHIFT(0x003e6000) /* -0.015228271 */,
        -AW_MP3_PRESHIFT(0x0d11e000) /* -0.816864014 */,
         AW_MP3_PRESHIFT(0x116fc000) /*  1.089782715 */,
         AW_MP3_PRESHIFT(0x052c5000) /*  0.323318481 */,
         AW_MP3_PRESHIFT(0x02616000) /*  0.148773193 */,
         AW_MP3_PRESHIFT(0x007d6000) /*  0.030609131 */,
         AW_MP3_PRESHIFT(0x007aa000) /*  0.029937744 */,
         AW_MP3_PRESHIFT(0x00024000) /*  0.000549316 */,
         AW_MP3_PRESHIFT(0x0009a000) /*  0.002349854 */,
         AW_MP3_PRESHIFT(0x0000b000) /*  0.000167847 */,
    },

    /* 10 */
    {
        -AW_MP3_PRESHIFT(0x00002000) /* -0.000030518 */,
        -AW_MP3_PRESHIFT(0x00044000) /* -0.001037598 */,
         AW_MP3_PRESHIFT(0x000d7000) /*  0.003280640 */,
        -AW_MP3_PRESHIFT(0x00471000) /* -0.017349243 */,
         AW_MP3_PRESHIFT(0x0046b000) /*  0.017257690 */,
        -AW_MP3_PRESHIFT(0x0212b000) /* -0.129562378 */,
        -AW_MP3_PRESHIFT(0x0084a000) /* -0.032379150 */,
        -AW_MP3_PRESHIFT(0x0d78a000) /* -0.841949463 */,
         AW_MP3_PRESHIFT(0x113be000) /*  1.077117920 */,
         AW_MP3_PRESHIFT(0x04c16000) /*  0.297210693 */,
         AW_MP3_PRESHIFT(0x02687000) /*  0.150497437 */,
         AW_MP3_PRESHIFT(0x0069c000) /*  0.025817871 */,
         AW_MP3_PRESHIFT(0x0077f000) /*  0.029281616 */,
         AW_MP3_PRESHIFT(0x00002000) /*  0.000030518 */,
         AW_MP3_PRESHIFT(0x00093000) /*  0.002243042 */,
         AW_MP3_PRESHIFT(0x0000a000) /*  0.000152588 */,

        -AW_MP3_PRESHIFT(0x00002000) /* -0.000030518 */,
        -AW_MP3_PRESHIFT(0x00044000) /* -0.001037598 */,
         AW_MP3_PRESHIFT(0x000d7000) /*  0.003280640 */,
        -AW_MP3_PRESHIFT(0x00471000) /* -0.017349243 */,
         AW_MP3_PRESHIFT(0x0046b000) /*  0.017257690 */,
        -AW_MP3_PRESHIFT(0x0212b000) /* -0.129562378 */,
        -AW_MP3_PRESHIFT(0x0084a000) /* -0.032379150 */,
        -AW_MP3_PRESHIFT(0x0d78a000) /* -0.841949463 */,
         AW_MP3_PRESHIFT(0x113be000) /*  1.077117920 */,
         AW_MP3_PRESHIFT(0x04c16000) /*  0.297210693 */,
         AW_MP3_PRESHIFT(0x02687000) /*  0.150497437 */,
         AW_MP3_PRESHIFT(0x0069c000) /*  0.025817871 */,
         AW_MP3_PRESHIFT(0x0077f000) /*  0.029281616 */,
         AW_MP3_PRESHIFT(0x00002000) /*  0.000030518 */,
         AW_MP3_PRESHIFT(0x00093000) /*  0.002243042 */,
         AW_MP3_PRESHIFT(0x0000a000) /*  0.000152588 */,
    },

    /* 11 */
    {
        -AW_MP3_PRESHIFT(0x00003000) /* -0.000045776 */,
        -AW_MP3_PRESHIFT(0x00049000) /* -0.001113892 */,
         AW_MP3_PRESHIFT(0x000d0000) /*  0.003173828 */,
        -AW_MP3_PRESHIFT(0x004ba000) /* -0.018463135 */,
         AW_MP3_PRESHIFT(0x003ca000) /*  0.014801025 */,
        -AW_MP3_PRESHIFT(0x02233000) /* -0.133590698 */,
        -AW_MP3_PRESHIFT(0x00ce4000) /* -0.050354004 */,
        -AW_MP3_PRESHIFT(0x0ddca000) /* -0.866363525 */,
         AW_MP3_PRESHIFT(0x1102f000) /*  1.063217163 */,
         AW_MP3_PRESHIFT(0x04587000) /*  0.271591187 */,
         AW_MP3_PRESHIFT(0x026cf000) /*  0.151596069 */,
         AW_MP3_PRESHIFT(0x0056c000) /*  0.021179199 */,
         AW_MP3_PRESHIFT(0x0074e000) /*  0.028533936 */,
        -AW_MP3_PRESHIFT(0x0001d000) /* -0.000442505 */,
         AW_MP3_PRESHIFT(0x0008b000) /*  0.002120972 */,
         AW_MP3_PRESHIFT(0x00009000) /*  0.000137329 */,

        -AW_MP3_PRESHIFT(0x00003000) /* -0.000045776 */,
        -AW_MP3_PRESHIFT(0x00049000) /* -0.001113892 */,
         AW_MP3_PRESHIFT(0x000d0000) /*  0.003173828 */,
        -AW_MP3_PRESHIFT(0x004ba000) /* -0.018463135 */,
         AW_MP3_PRESHIFT(0x003ca000) /*  0.014801025 */,
        -AW_MP3_PRESHIFT(0x02233000) /* -0.133590698 */,
        -AW_MP3_PRESHIFT(0x00ce4000) /* -0.050354004 */,
        -AW_MP3_PRESHIFT(0x0ddca000) /* -0.866363525 */,
         AW_MP3_PRESHIFT(0x1102f000) /*  1.063217163 */,
         AW_MP3_PRESHIFT(0x04587000) /*  0.271591187 */,
         AW_MP3_PRESHIFT(0x026cf000) /*  0.151596069 */,
         AW_MP3_PRESHIFT(0x0056c000) /*  0.021179199 */,
         AW_MP3_PRESHIFT(0x0074e000) /*  0.028533936 */,
        -AW_MP3_PRESHIFT(0x0001d000) /* -0.000442505 */,
         AW_MP3_PRESHIFT(0x0008b000) /*  0.002120972 */,
         AW_MP3_PRESHIFT(0x00009000) /*  0.000137329 */,
    },

    /* 12 */
    {
        -AW_MP3_PRESHIFT(0x00003000) /* -0.000045776 */,
        -AW_MP3_PRESHIFT(0x0004f000) /* -0.001205444 */,
         AW_MP3_PRESHIFT(0x000c8000) /*  0.003051758 */,
        -AW_MP3_PRESHIFT(0x00503000) /* -0.019577026 */,
         AW_MP3_PRESHIFT(0x0031a000) /*  0.012115479 */,
        -AW_MP3_PRESHIFT(0x02326000) /* -0.137298584 */,
        -AW_MP3_PRESHIFT(0x011b5000) /* -0.069168091 */,
        -AW_MP3_PRESHIFT(0x0e3dd000) /* -0.890090942 */,
         AW_MP3_PRESHIFT(0x10c54000) /*  1.048156738 */,
         AW_MP3_PRESHIFT(0x03f1b000) /*  0.246505737 */,
         AW_MP3_PRESHIFT(0x026ee000) /*  0.152069092 */,
         AW_MP3_PRESHIFT(0x00447000) /*  0.016708374 */,
         AW_MP3_PRESHIFT(0x00719000) /*  0.027725220 */,
        -AW_MP3_PRESHIFT(0x00039000) /* -0.000869751 */,
         AW_MP3_PRESHIFT(0x00084000) /*  0.002014160 */,
         AW_MP3_PRESHIFT(0x00008000) /*  0.000122070 */,

        -AW_MP3_PRESHIFT(0x00003000) /* -0.000045776 */,
        -AW_MP3_PRESHIFT(0x0004f000) /* -0.001205444 */,
         AW_MP3_PRESHIFT(0x000c8000) /*  0.003051758 */,
        -AW_MP3_PRESHIFT(0x00503000) /* -0.019577026 */,
         AW_MP3_PRESHIFT(0x0031a000) /*  0.012115479 */,
        -AW_MP3_PRESHIFT(0x02326000) /* -0.137298584 */,
        -AW_MP3_PRESHIFT(0x011b5000) /* -0.069168091 */,
        -AW_MP3_PRESHIFT(0x0e3dd000) /* -0.890090942 */,
         AW_MP3_PRESHIFT(0x10c54000) /*  1.048156738 */,
         AW_MP3_PRESHIFT(0x03f1b000) /*  0.246505737 */,
         AW_MP3_PRESHIFT(0x026ee000) /*  0.152069092 */,
         AW_MP3_PRESHIFT(0x00447000) /*  0.016708374 */,
         AW_MP3_PRESHIFT(0x00719000) /*  0.027725220 */,
        -AW_MP3_PRESHIFT(0x00039000) /* -0.000869751 */,
         AW_MP3_PRESHIFT(0x00084000) /*  0.002014160 */,
         AW_MP3_PRESHIFT(0x00008000) /*  0.000122070 */,
    },

    /* 13 */
    {
        -AW_MP3_PRESHIFT(0x00004000) /* -0.000061035 */,
        -AW_MP3_PRESHIFT(0x00055000) /* -0.001296997 */,
         AW_MP3_PRESHIFT(0x000bd000) /*  0.002883911 */,
        -AW_MP3_PRESHIFT(0x0054c000) /* -0.020690918 */,
         AW_MP3_PRESHIFT(0x0025d000) /*  0.009231567 */,
        -AW_MP3_PRESHIFT(0x02403000) /* -0.140670776 */,
        -AW_MP3_PRESHIFT(0x016ba000) /* -0.088775635 */,
        -AW_MP3_PRESHIFT(0x0e9be000) /* -0.913055420 */,
         AW_MP3_PRESHIFT(0x1082d000) /*  1.031936646 */,
         AW_MP3_PRESHIFT(0x038d4000) /*  0.221984863 */,
         AW_MP3_PRESHIFT(0x026e7000) /*  0.151962280 */,
         AW_MP3_PRESHIFT(0x0032e000) /*  0.012420654 */,
         AW_MP3_PRESHIFT(0x006df000) /*  0.026840210 */,
        -AW_MP3_PRESHIFT(0x00053000) /* -0.001266479 */,
         AW_MP3_PRESHIFT(0x0007d000) /*  0.001907349 */,
         AW_MP3_PRESHIFT(0x00007000) /*  0.000106812 */,

        -AW_MP3_PRESHIFT(0x00004000) /* -0.000061035 */,
        -AW_MP3_PRESHIFT(0x00055000) /* -0.001296997 */,
         AW_MP3_PRESHIFT(0x000bd000) /*  0.002883911 */,
        -AW_MP3_PRESHIFT(0x0054c000) /* -0.020690918 */,
         AW_MP3_PRESHIFT(0x0025d000) /*  0.009231567 */,
        -AW_MP3_PRESHIFT(0x02403000) /* -0.140670776 */,
        -AW_MP3_PRESHIFT(0x016ba000) /* -0.088775635 */,
        -AW_MP3_PRESHIFT(0x0e9be000) /* -0.913055420 */,
         AW_MP3_PRESHIFT(0x1082d000) /*  1.031936646 */,
         AW_MP3_PRESHIFT(0x038d4000) /*  0.221984863 */,
         AW_MP3_PRESHIFT(0x026e7000) /*  0.151962280 */,
         AW_MP3_PRESHIFT(0x0032e000) /*  0.012420654 */,
         AW_MP3_PRESHIFT(0x006df000) /*  0.026840210 */,
        -AW_MP3_PRESHIFT(0x00053000) /* -0.001266479 */,
         AW_MP3_PRESHIFT(0x0007d000) /*  0.001907349 */,
         AW_MP3_PRESHIFT(0x00007000) /*  0.000106812 */,
    },

    /* 14 */
    {
        -AW_MP3_PRESHIFT(0x00004000) /* -0.000061035 */,
        -AW_MP3_PRESHIFT(0x0005b000) /* -0.001388550 */,
         AW_MP3_PRESHIFT(0x000b1000) /*  0.002700806 */,
        -AW_MP3_PRESHIFT(0x00594000) /* -0.021789551 */,
         AW_MP3_PRESHIFT(0x00192000) /*  0.006134033 */,
        -AW_MP3_PRESHIFT(0x024c8000) /* -0.143676758 */,
        -AW_MP3_PRESHIFT(0x01bf2000) /* -0.109161377 */,
        -AW_MP3_PRESHIFT(0x0ef69000) /* -0.935195923 */,
         AW_MP3_PRESHIFT(0x103be000) /*  1.014617920 */,
         AW_MP3_PRESHIFT(0x032b4000) /*  0.198059082 */,
         AW_MP3_PRESHIFT(0x026bc000) /*  0.151306152 */,
         AW_MP3_PRESHIFT(0x00221000) /*  0.008316040 */,
         AW_MP3_PRESHIFT(0x006a2000) /*  0.025909424 */,
        -AW_MP3_PRESHIFT(0x0006a000) /* -0.001617432 */,
         AW_MP3_PRESHIFT(0x00075000) /*  0.001785278 */,
         AW_MP3_PRESHIFT(0x00007000) /*  0.000106812 */,

        -AW_MP3_PRESHIFT(0x00004000) /* -0.000061035 */,
        -AW_MP3_PRESHIFT(0x0005b000) /* -0.001388550 */,
         AW_MP3_PRESHIFT(0x000b1000) /*  0.002700806 */,
        -AW_MP3_PRESHIFT(0x00594000) /* -0.021789551 */,
         AW_MP3_PRESHIFT(0x00192000) /*  0.006134033 */,
        -AW_MP3_PRESHIFT(0x024c8000) /* -0.143676758 */,
        -AW_MP3_PRESHIFT(0x01bf2000) /* -0.109161377 */,
        -AW_MP3_PRESHIFT(0x0ef69000) /* -0.935195923 */,
         AW_MP3_PRESHIFT(0x103be000) /*  1.014617920 */,
         AW_MP3_PRESHIFT(0x032b4000) /*  0.198059082 */,
         AW_MP3_PRESHIFT(0x026bc000) /*  0.151306152 */,
         AW_MP3_PRESHIFT(0x00221000) /*  0.008316040 */,
         AW_MP3_PRESHIFT(0x006a2000) /*  0.025909424 */,
        -AW_MP3_PRESHIFT(0x0006a000) /* -0.001617432 */,
         AW_MP3_PRESHIFT(0x00075000) /*  0.001785278 */,
         AW_MP3_PRESHIFT(0x00007000) /*  0.000106812 */,
    },

    /* 15 */
    {
        -AW_MP3_PRESHIFT(0x00005000) /* -0.000076294 */,
        -AW_MP3_PRESHIFT(0x00061000) /* -0.001480103 */,
         AW_MP3_PRESHIFT(0x000a3000) /*  0.002487183 */,
        -AW_MP3_PRESHIFT(0x005da000) /* -0.022857666 */,
         AW_MP3_PRESHIFT(0x000b9000) /*  0.002822876 */,
        -AW_MP3_PRESHIFT(0x02571000) /* -0.146255493 */,
        -AW_MP3_PRESHIFT(0x0215c000) /* -0.130310059 */,
        -AW_MP3_PRESHIFT(0x0f4dc000) /* -0.956481934 */,
         AW_MP3_PRESHIFT(0x0ff0a000) /*  0.996246338 */,
         AW_MP3_PRESHIFT(0x02cbf000) /*  0.174789429 */,
         AW_MP3_PRESHIFT(0x0266e000) /*  0.150115967 */,
         AW_MP3_PRESHIFT(0x00120000) /*  0.004394531 */,
         AW_MP3_PRESHIFT(0x00662000) /*  0.024932861 */,
        -AW_MP3_PRESHIFT(0x0007f000) /* -0.001937866 */,
         AW_MP3_PRESHIFT(0x0006f000) /*  0.001693726 */,
         AW_MP3_PRESHIFT(0x00006000) /*  0.000091553 */,

        -AW_MP3_PRESHIFT(0x00005000) /* -0.000076294 */,
        -AW_MP3_PRESHIFT(0x00061000) /* -0.001480103 */,
         AW_MP3_PRESHIFT(0x000a3000) /*  0.002487183 */,
        -AW_MP3_PRESHIFT(0x005da000) /* -0.022857666 */,
         AW_MP3_PRESHIFT(0x000b9000) /*  0.002822876 */,
        -AW_MP3_PRESHIFT(0x02571000) /* -0.146255493 */,
        -AW_MP3_PRESHIFT(0x0215c000) /* -0.130310059 */,
        -AW_MP3_PRESHIFT(0x0f4dc000) /* -0.956481934 */,
         AW_MP3_PRESHIFT(0x0ff0a000) /*  0.996246338 */,
         AW_MP3_PRESHIFT(0x02cbf000) /*  0.174789429 */,
         AW_MP3_PRESHIFT(0x0266e000) /*  0.150115967 */,
         AW_MP3_PRESHIFT(0x00120000) /*  0.004394531 */,
         AW_MP3_PRESHIFT(0x00662000) /*  0.024932861 */,
        -AW_MP3_PRESHIFT(0x0007f000) /* -0.001937866 */,
         AW_MP3_PRESHIFT(0x0006f000) /*  0.001693726 */,
         AW_MP3_PRESHIFT(0x00006000) /*  0.000091553 */,
    },

    /* 16 */
    {
        -AW_MP3_PRESHIFT(0x00005000) /* -0.000076294 */,
        -AW_MP3_PRESHIFT(0x00068000) /* -0.001586914 */,
         AW_MP3_PRESHIFT(0x00092000) /*  0.002227783 */,
        -AW_MP3_PRESHIFT(0x0061f000) /* -0.023910522 */,
        -AW_MP3_PRESHIFT(0x0002d000) /* -0.000686646 */,
        -AW_MP3_PRESHIFT(0x025ff000) /* -0.148422241 */,
        -AW_MP3_PRESHIFT(0x026f7000) /* -0.152206421 */,
        -AW_MP3_PRESHIFT(0x0fa13000) /* -0.976852417 */,
         AW_MP3_PRESHIFT(0x0fa13000) /*  0.976852417 */,
         AW_MP3_PRESHIFT(0x026f7000) /*  0.152206421 */,
         AW_MP3_PRESHIFT(0x025ff000) /*  0.148422241 */,
         AW_MP3_PRESHIFT(0x0002d000) /*  0.000686646 */,
         AW_MP3_PRESHIFT(0x0061f000) /*  0.023910522 */,
        -AW_MP3_PRESHIFT(0x00092000) /* -0.002227783 */,
         AW_MP3_PRESHIFT(0x00068000) /*  0.001586914 */,
         AW_MP3_PRESHIFT(0x00005000) /*  0.000076294 */,

        -AW_MP3_PRESHIFT(0x00005000) /* -0.000076294 */,
        -AW_MP3_PRESHIFT(0x00068000) /* -0.001586914 */,
         AW_MP3_PRESHIFT(0x00092000) /*  0.002227783 */,
        -AW_MP3_PRESHIFT(0x0061f000) /* -0.023910522 */,
        -AW_MP3_PRESHIFT(0x0002d000) /* -0.000686646 */,
        -AW_MP3_PRESHIFT(0x025ff000) /* -0.148422241 */,
        -AW_MP3_PRESHIFT(0x026f7000) /* -0.152206421 */,
        -AW_MP3_PRESHIFT(0x0fa13000) /* -0.976852417 */,
         AW_MP3_PRESHIFT(0x0fa13000) /*  0.976852417 */,
         AW_MP3_PRESHIFT(0x026f7000) /*  0.152206421 */,
         AW_MP3_PRESHIFT(0x025ff000) /*  0.148422241 */,
         AW_MP3_PRESHIFT(0x0002d000) /*  0.000686646 */,
         AW_MP3_PRESHIFT(0x0061f000) /*  0.023910522 */,
        -AW_MP3_PRESHIFT(0x00092000) /* -0.002227783 */,
         AW_MP3_PRESHIFT(0x00068000) /*  0.001586914 */,
         AW_MP3_PRESHIFT(0x00005000) /*  0.000076294 */,
    },

};


/**
 * \brief perform full frequency PCM synthesis
 */
static void __synth_full (struct aw_mp3_dec_synth       *p_synth,
                          const struct aw_mp3_dec_frame *p_frame,
                          uint32_t                       nch,
                          uint32_t                       ns)
{
    uint32_t                phase, ch, s, sb, pe, po;

    int32_t                 *p_pcm1, *p_pcm2, (*p_filter)[2][2][16][8];
    const int32_t          (*p_sbsample)[36][32];

    register int32_t       (*p_fe)[8], (*p_fx)[8], (*p_fo)[8];
    register const int32_t (*p_dptr)[32], *p_ptr;
    register int64_t         hi;
    register int64_t         lo;

    for (ch = 0; ch < nch; ++ch) {
        p_sbsample = &p_frame->sbsample[ch];
        p_filter   = &p_synth->filter[ch];
        phase      = p_synth->phase;
        p_pcm1     = p_synth->pcm.samples[ch];

        for (s = 0; s < ns; ++s) {
            __synth_dct32((*p_sbsample)[s], phase >> 1,
                          (*p_filter)[0][phase & 1], (*p_filter)[1][phase & 1]);

            pe = phase & ~1;
            po = ((phase - 1) & 0xf) | 1;

            /* calculate 32 samples */

            p_fe = &(*p_filter)[0][ phase & 1][0];
            p_fx = &(*p_filter)[0][~phase & 1][0];
            p_fo = &(*p_filter)[1][~phase & 1][0];

            p_dptr = &__g_synth_d[0];

            p_ptr = *p_dptr + po;

            AW_MP3_ML0(hi, lo, (*p_fx)[0], p_ptr[ 0]);
            AW_MP3_MLA(hi, lo, (*p_fx)[1], p_ptr[14]);
            AW_MP3_MLA(hi, lo, (*p_fx)[2], p_ptr[12]);
            AW_MP3_MLA(hi, lo, (*p_fx)[3], p_ptr[10]);
            AW_MP3_MLA(hi, lo, (*p_fx)[4], p_ptr[ 8]);
            AW_MP3_MLA(hi, lo, (*p_fx)[5], p_ptr[ 6]);
            AW_MP3_MLA(hi, lo, (*p_fx)[6], p_ptr[ 4]);
            AW_MP3_MLA(hi, lo, (*p_fx)[7], p_ptr[ 2]);
            AW_MP3_MLN(hi, lo);

            p_ptr = *p_dptr + pe;

            AW_MP3_MLA(hi, lo, (*p_fe)[0], p_ptr[ 0]);
            AW_MP3_MLA(hi, lo, (*p_fe)[1], p_ptr[14]);
            AW_MP3_MLA(hi, lo, (*p_fe)[2], p_ptr[12]);
            AW_MP3_MLA(hi, lo, (*p_fe)[3], p_ptr[10]);
            AW_MP3_MLA(hi, lo, (*p_fe)[4], p_ptr[ 8]);
            AW_MP3_MLA(hi, lo, (*p_fe)[5], p_ptr[ 6]);
            AW_MP3_MLA(hi, lo, (*p_fe)[6], p_ptr[ 4]);
            AW_MP3_MLA(hi, lo, (*p_fe)[7], p_ptr[ 2]);

            *p_pcm1++ = AW_MP3_SHIFT(AW_MP3_MLZ(hi, lo));

            p_pcm2 = p_pcm1 + 30;

            for (sb = 1; sb < 16; ++sb) {
                ++p_fe;
                ++p_dptr;

                /* D[32 - sb][i] == -D[sb][31 - i] */
                p_ptr = *p_dptr + po;

                AW_MP3_ML0(hi, lo, (*p_fo)[0], p_ptr[ 0]);
                AW_MP3_MLA(hi, lo, (*p_fo)[1], p_ptr[14]);
                AW_MP3_MLA(hi, lo, (*p_fo)[2], p_ptr[12]);
                AW_MP3_MLA(hi, lo, (*p_fo)[3], p_ptr[10]);
                AW_MP3_MLA(hi, lo, (*p_fo)[4], p_ptr[ 8]);
                AW_MP3_MLA(hi, lo, (*p_fo)[5], p_ptr[ 6]);
                AW_MP3_MLA(hi, lo, (*p_fo)[6], p_ptr[ 4]);
                AW_MP3_MLA(hi, lo, (*p_fo)[7], p_ptr[ 2]);
                AW_MP3_MLN(hi, lo);

                p_ptr = *p_dptr + pe;

                AW_MP3_MLA(hi, lo, (*p_fe)[7], p_ptr[ 2]);
                AW_MP3_MLA(hi, lo, (*p_fe)[6], p_ptr[ 4]);
                AW_MP3_MLA(hi, lo, (*p_fe)[5], p_ptr[ 6]);
                AW_MP3_MLA(hi, lo, (*p_fe)[4], p_ptr[ 8]);
                AW_MP3_MLA(hi, lo, (*p_fe)[3], p_ptr[10]);
                AW_MP3_MLA(hi, lo, (*p_fe)[2], p_ptr[12]);
                AW_MP3_MLA(hi, lo, (*p_fe)[1], p_ptr[14]);
                AW_MP3_MLA(hi, lo, (*p_fe)[0], p_ptr[ 0]);

                *p_pcm1++ = AW_MP3_SHIFT(AW_MP3_MLZ(hi, lo));

                p_ptr = *p_dptr - pe;

                AW_MP3_ML0(hi, lo, (*p_fe)[0], p_ptr[31 - 16]);
                AW_MP3_MLA(hi, lo, (*p_fe)[1], p_ptr[31 - 14]);
                AW_MP3_MLA(hi, lo, (*p_fe)[2], p_ptr[31 - 12]);
                AW_MP3_MLA(hi, lo, (*p_fe)[3], p_ptr[31 - 10]);
                AW_MP3_MLA(hi, lo, (*p_fe)[4], p_ptr[31 -  8]);
                AW_MP3_MLA(hi, lo, (*p_fe)[5], p_ptr[31 -  6]);
                AW_MP3_MLA(hi, lo, (*p_fe)[6], p_ptr[31 -  4]);
                AW_MP3_MLA(hi, lo, (*p_fe)[7], p_ptr[31 -  2]);

                p_ptr = *p_dptr - po;

                AW_MP3_MLA(hi, lo, (*p_fo)[7], p_ptr[31 -  2]);
                AW_MP3_MLA(hi, lo, (*p_fo)[6], p_ptr[31 -  4]);
                AW_MP3_MLA(hi, lo, (*p_fo)[5], p_ptr[31 -  6]);
                AW_MP3_MLA(hi, lo, (*p_fo)[4], p_ptr[31 -  8]);
                AW_MP3_MLA(hi, lo, (*p_fo)[3], p_ptr[31 - 10]);
                AW_MP3_MLA(hi, lo, (*p_fo)[2], p_ptr[31 - 12]);
                AW_MP3_MLA(hi, lo, (*p_fo)[1], p_ptr[31 - 14]);
                AW_MP3_MLA(hi, lo, (*p_fo)[0], p_ptr[31 - 16]);

                *p_pcm2-- = AW_MP3_SHIFT(AW_MP3_MLZ(hi, lo));

                ++p_fo;
            }

            ++p_dptr;

            p_ptr = *p_dptr + po;

            AW_MP3_ML0(hi, lo, (*p_fo)[0], p_ptr[ 0]);
            AW_MP3_MLA(hi, lo, (*p_fo)[1], p_ptr[14]);
            AW_MP3_MLA(hi, lo, (*p_fo)[2], p_ptr[12]);
            AW_MP3_MLA(hi, lo, (*p_fo)[3], p_ptr[10]);
            AW_MP3_MLA(hi, lo, (*p_fo)[4], p_ptr[ 8]);
            AW_MP3_MLA(hi, lo, (*p_fo)[5], p_ptr[ 6]);
            AW_MP3_MLA(hi, lo, (*p_fo)[6], p_ptr[ 4]);
            AW_MP3_MLA(hi, lo, (*p_fo)[7], p_ptr[ 2]);

            *p_pcm1 = AW_MP3_SHIFT(-AW_MP3_MLZ(hi, lo));
            p_pcm1 += 16;

            phase = (phase + 1) % 16;
        }
    }
}

/**
 * \brief perform half frequency PCM synthesis
 */
static void __synth_half (struct aw_mp3_dec_synth       *p_synth,
                          const struct aw_mp3_dec_frame *frame,
                          uint32_t                       nch,
                          uint32_t                       ns)
{
    uint32_t                 phase, ch, s, sb, pe, po;
    int32_t                 *p_pcm1, *p_pcm2, (*p_filter)[2][2][16][8];
    const int32_t          (*p_sbsample)[36][32];
    register int32_t       (*p_fe)[8], (*p_fx)[8], (*p_fo)[8];
    register const int32_t (*p_dptr)[32], *p_ptr;
    register int64_t         hi;
    register int64_t         lo;

    for (ch = 0; ch < nch; ++ch) {
        p_sbsample = &frame->sbsample[ch];
        p_filter   = &p_synth->filter[ch];
        phase      = p_synth->phase;
        p_pcm1     = p_synth->pcm.samples[ch];

        for (s = 0; s < ns; ++s) {
            __synth_dct32((*p_sbsample)[s], phase >> 1,
                          (*p_filter)[0][phase & 1],
                          (*p_filter)[1][phase & 1]);

            pe = phase & ~1;
            po = ((phase - 1) & 0xf) | 1;

            /* calculate 16 samples */
            p_fe = &(*p_filter)[0][ phase & 1][0];
            p_fx = &(*p_filter)[0][~phase & 1][0];
            p_fo = &(*p_filter)[1][~phase & 1][0];

            p_dptr = &__g_synth_d[0];

            p_ptr = *p_dptr + po;

            AW_MP3_ML0(hi, lo, (*p_fx)[0], p_ptr[ 0]);
            AW_MP3_MLA(hi, lo, (*p_fx)[1], p_ptr[14]);
            AW_MP3_MLA(hi, lo, (*p_fx)[2], p_ptr[12]);
            AW_MP3_MLA(hi, lo, (*p_fx)[3], p_ptr[10]);
            AW_MP3_MLA(hi, lo, (*p_fx)[4], p_ptr[ 8]);
            AW_MP3_MLA(hi, lo, (*p_fx)[5], p_ptr[ 6]);
            AW_MP3_MLA(hi, lo, (*p_fx)[6], p_ptr[ 4]);
            AW_MP3_MLA(hi, lo, (*p_fx)[7], p_ptr[ 2]);
            AW_MP3_MLN(hi, lo);

            p_ptr = *p_dptr + pe;

            AW_MP3_MLA(hi, lo, (*p_fe)[0], p_ptr[ 0]);
            AW_MP3_MLA(hi, lo, (*p_fe)[1], p_ptr[14]);
            AW_MP3_MLA(hi, lo, (*p_fe)[2], p_ptr[12]);
            AW_MP3_MLA(hi, lo, (*p_fe)[3], p_ptr[10]);
            AW_MP3_MLA(hi, lo, (*p_fe)[4], p_ptr[ 8]);
            AW_MP3_MLA(hi, lo, (*p_fe)[5], p_ptr[ 6]);
            AW_MP3_MLA(hi, lo, (*p_fe)[6], p_ptr[ 4]);
            AW_MP3_MLA(hi, lo, (*p_fe)[7], p_ptr[ 2]);

            *p_pcm1++ = AW_MP3_SHIFT(AW_MP3_MLZ(hi, lo));

            p_pcm2 = p_pcm1 + 14;

            for (sb = 1; sb < 16; ++sb) {
                ++p_fe;
                ++p_dptr;

                /* D[32 - sb][i] == -D[sb][31 - i] */
                if (!(sb & 1)) {
                    p_ptr = *p_dptr + po;

                    AW_MP3_ML0(hi, lo, (*p_fo)[0], p_ptr[ 0]);
                    AW_MP3_MLA(hi, lo, (*p_fo)[1], p_ptr[14]);
                    AW_MP3_MLA(hi, lo, (*p_fo)[2], p_ptr[12]);
                    AW_MP3_MLA(hi, lo, (*p_fo)[3], p_ptr[10]);
                    AW_MP3_MLA(hi, lo, (*p_fo)[4], p_ptr[ 8]);
                    AW_MP3_MLA(hi, lo, (*p_fo)[5], p_ptr[ 6]);
                    AW_MP3_MLA(hi, lo, (*p_fo)[6], p_ptr[ 4]);
                    AW_MP3_MLA(hi, lo, (*p_fo)[7], p_ptr[ 2]);
                    AW_MP3_MLN(hi, lo);

                    p_ptr = *p_dptr + pe;

                    AW_MP3_MLA(hi, lo, (*p_fe)[7], p_ptr[ 2]);
                    AW_MP3_MLA(hi, lo, (*p_fe)[6], p_ptr[ 4]);
                    AW_MP3_MLA(hi, lo, (*p_fe)[5], p_ptr[ 6]);
                    AW_MP3_MLA(hi, lo, (*p_fe)[4], p_ptr[ 8]);
                    AW_MP3_MLA(hi, lo, (*p_fe)[3], p_ptr[10]);
                    AW_MP3_MLA(hi, lo, (*p_fe)[2], p_ptr[12]);
                    AW_MP3_MLA(hi, lo, (*p_fe)[1], p_ptr[14]);
                    AW_MP3_MLA(hi, lo, (*p_fe)[0], p_ptr[ 0]);

                    *p_pcm1++ = AW_MP3_SHIFT(AW_MP3_MLZ(hi, lo));

                    p_ptr = *p_dptr - po;

                    AW_MP3_ML0(hi, lo, (*p_fo)[7], p_ptr[31 -  2]);
                    AW_MP3_MLA(hi, lo, (*p_fo)[6], p_ptr[31 -  4]);
                    AW_MP3_MLA(hi, lo, (*p_fo)[5], p_ptr[31 -  6]);
                    AW_MP3_MLA(hi, lo, (*p_fo)[4], p_ptr[31 -  8]);
                    AW_MP3_MLA(hi, lo, (*p_fo)[3], p_ptr[31 - 10]);
                    AW_MP3_MLA(hi, lo, (*p_fo)[2], p_ptr[31 - 12]);
                    AW_MP3_MLA(hi, lo, (*p_fo)[1], p_ptr[31 - 14]);
                    AW_MP3_MLA(hi, lo, (*p_fo)[0], p_ptr[31 - 16]);

                    p_ptr = *p_dptr - pe;

                    AW_MP3_MLA(hi, lo, (*p_fe)[0], p_ptr[31 - 16]);
                    AW_MP3_MLA(hi, lo, (*p_fe)[1], p_ptr[31 - 14]);
                    AW_MP3_MLA(hi, lo, (*p_fe)[2], p_ptr[31 - 12]);
                    AW_MP3_MLA(hi, lo, (*p_fe)[3], p_ptr[31 - 10]);
                    AW_MP3_MLA(hi, lo, (*p_fe)[4], p_ptr[31 -  8]);
                    AW_MP3_MLA(hi, lo, (*p_fe)[5], p_ptr[31 -  6]);
                    AW_MP3_MLA(hi, lo, (*p_fe)[6], p_ptr[31 -  4]);
                    AW_MP3_MLA(hi, lo, (*p_fe)[7], p_ptr[31 -  2]);

                    *p_pcm2-- = AW_MP3_SHIFT(AW_MP3_MLZ(hi, lo));
                }

                ++p_fo;
            }

            ++p_dptr;

            p_ptr = *p_dptr + po;

            AW_MP3_ML0(hi, lo, (*p_fo)[0], p_ptr[ 0]);
            AW_MP3_MLA(hi, lo, (*p_fo)[1], p_ptr[14]);
            AW_MP3_MLA(hi, lo, (*p_fo)[2], p_ptr[12]);
            AW_MP3_MLA(hi, lo, (*p_fo)[3], p_ptr[10]);
            AW_MP3_MLA(hi, lo, (*p_fo)[4], p_ptr[ 8]);
            AW_MP3_MLA(hi, lo, (*p_fo)[5], p_ptr[ 6]);
            AW_MP3_MLA(hi, lo, (*p_fo)[6], p_ptr[ 4]);
            AW_MP3_MLA(hi, lo, (*p_fo)[7], p_ptr[ 2]);

            *p_pcm1 = AW_MP3_SHIFT(-AW_MP3_MLZ(hi, lo));
            p_pcm1 += 8;

            phase = (phase + 1) % 16;
        }
    }
}

/******************************************************************************/

/**
 * \brief synth³õÊ¼»¯
 */
void aw_mp3_dec_synth_init (struct aw_mp3_dec_synth *p_synth)
{
    aw_mp3_dec_synth_mute(p_synth);

    p_synth->phase           = 0;
    p_synth->pcm.sample_rate = 0;
    p_synth->pcm.channels    = 0;
    p_synth->pcm.length      = 0;
}

/**
 * \brief synth mute
 */
void aw_mp3_dec_synth_mute (struct aw_mp3_dec_synth *p_synth)
{
    memset(p_synth->filter, 0, sizeof(p_synth->filter));
}

/**
 * \brief synth frame
 */
void aw_mp3_dec_synth_frame (struct aw_mp3_dec_synth       *p_synth,
                             const struct aw_mp3_dec_frame *p_frame)
{
    uint32_t nch, ns;

    nch = AW_MP3_DEC_FRAME_NCHANNELS(&p_frame->header);
    ns  = AW_MP3_DEC_FRAME_NSBSAMPLES(&p_frame->header);

    p_synth->pcm.sample_rate = p_frame->header.sample_rate;
    p_synth->pcm.channels    = nch;
    p_synth->pcm.length      = 32 * ns;

    if (p_frame->options & AW_MP3_DEC_OPTION_HALFSAMPLERATE) {
        p_synth->pcm.sample_rate /= 2;
        p_synth->pcm.length      /= 2;

        __synth_half(p_synth, p_frame, nch, ns);
    } else {
        __synth_full(p_synth, p_frame, nch, ns);
    }

    p_synth->phase = (p_synth->phase + ns) % 16;
}

/* end of file */
