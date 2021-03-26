/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Unicode defines.
 *
 * \internal
 * \par modification history
 * - 1.00 13-03-05  orz, first implementation.
 * \endinternal
 */

#ifndef __UNI_CODE_H
#define __UNI_CODE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name unicode key defines
 * @{
 */

/******************************************************************************/

/* Unicode */

#define UNICODE_NULL            0x0000
#define UNICODE_SOH             0x0001 /* Start Of heading */
#define UNICODE_STX             0x0002 /* Start Of Text */
#define UNICODE_ETX             0x0003 /* End Of Text */
#define UNICODE_EOT             0x0004 /* End Of Transmission */
#define UNICODE_ENQ             0x0005 /* Enquiry */
#define UNICODE_ACK             0x0006 /* Acknowledge */
#define UNICODE_BELL            0x0007 /* '\a' Bell */
#define UNICODE_BACKSPACE       0x0008 /* '\b' back space */
#define UNICODE_HTAB            0x0009 /* '\v' Horizontal tabulation */
#define UNICODE_LINE_FEED       0x000A /* '\n' Line Feed */
#define UNICODE_TAB             0x000B /* '\t' Vertical Tabulation */
#define UNICODE_FORM_FEED       0x000C /* '\f' Form Feed */
#define UNICODE_ENTER           0x000D /* '\r' Carriage Return */
#define UNICODE_SHIFT_OUT       0x000E /* Shift Out */
#define UNICODE_SHIFT_IN        0x000F /* Shit In */
#define UNICODE_DL_ESC          0x0010 /* Data Link escape */
#define UNICODE_DC1             0x0011 /* Device Control 1 */
#define UNICODE_DC2             0x0012 /* Device Control 2 */
#define UNICODE_DC3             0x0013 /* Device Control 3 */
#define UNICODE_DC4             0x0014 /* Device Control 4 */
#define UNICODE_NAK             0x0015 /* Negative Acknowledge */
#define UNICODE_SYN             0x0016 /* Synchronous Idle */
#define UNICODE_ETB             0x0017 /* End of Transmission block */
#define UNICODE_CANCEL          0x0018 /* Cancel */
#define UNICODE_EOM             0x0019 /* End Of Medium */
#define UNICODE_SUBSTITUTE      0x001A /* Substitute */
#define UNICODE_ESCAPE          0x001B /* Escape */
#define UNICODE_FS              0x001C /* File Separator */
#define UNICODE_GS              0x001D /* Group SeparaTor */
#define UNICODE_RS              0x001E /* Record Separator */
#define UNICODE_US              0x001F /* Unit Separator */
#define UNICODE_SPACE           0x0020 /* ' ' Space */
#define UNICODE_EXCLAMATION     0x0021 /* '!' Exclamation Mark */
#define UNICODE_QUOTATION       0x0022 /* '"' Quotation Mark */
#define UNICODE_NUMBER          0x0023 /* '#' Number Sign */
#define UNICODE_DOLLAR          0x0024 /* '$' Dollar Sign */
#define UNICODE_PERCENT         0x0025 /* '%' Percent Sign */
#define UNICODE_AMPERSAND       0x0026 /* '&' Ampersand */
#define UNICODE_APOSTROPHE      0x0027 /* ''' Apostrophe */
#define UNICODE_LEFT_PAREN      0x0028 /* '(' Left Parenthesis */
#define UNICODE_RIGHT_PAREN     0x0029 /* ')' Right Parenthesis */
#define UNICODE_ASTERISK        0x002A /* '*' Asterisk */
#define UNICODE_PLUS            0x002B /* '+' Plus sign */
#define UNICODE_COMMA           0x002C /* ',' Comma */
#define UNICODE_MINUS           0x002D /* '-' Hyphen Minus */
#define UNICODE_DOT             0x002E /* '.' Full Stop */
#define UNICODE_SLASH           0x002F /* '/' Slash */
#define UNICODE_0               0x0030 /* '0' */
#define UNICODE_1               0x0031 /* '1' */
#define UNICODE_2               0x0032 /* '2' */
#define UNICODE_3               0x0033 /* '3' */
#define UNICODE_4               0x0034 /* '4' */
#define UNICODE_5               0x0035 /* '5' */
#define UNICODE_6               0x0036 /* '6' */
#define UNICODE_7               0x0037 /* '7' */
#define UNICODE_8               0x0038 /* '8' */
#define UNICODE_9               0x0039 /* '9' */
#define UNICODE_COLON           0x003A /* ':' */
#define UNICODE_SEMICOLON       0x003B /* ';' */
#define UNICODE_LESS            0x003C /* '<' */
#define UNICODE_EQUALS          0x003D /* '=' */
#define UNICODE_GREATER         0x003E /* '>' */
#define UNICODE_QUESTION        0x003F /* '?' */
#define UNICODE_AT              0x0040 /* '@' */
#define UNICODE_CAP_A           0x0041 /* 'A' */
#define UNICODE_CAP_B           0x0042 /* 'B' */
#define UNICODE_CAP_C           0x0043 /* 'C' */
#define UNICODE_CAP_D           0x0044 /* 'D' */
#define UNICODE_CAP_E           0x0045 /* 'E' */
#define UNICODE_CAP_F           0x0046 /* 'F' */
#define UNICODE_CAP_G           0x0047 /* 'G' */
#define UNICODE_CAP_H           0x0048 /* 'H' */
#define UNICODE_CAP_I           0x0049 /* 'I' */
#define UNICODE_CAP_J           0x004A /* 'J' */
#define UNICODE_CAP_K           0x004B /* 'K' */
#define UNICODE_CAP_L           0x004C /* 'L' */
#define UNICODE_CAP_M           0x004D /* 'M' */
#define UNICODE_CAP_N           0x004E /* 'N' */
#define UNICODE_CAP_O           0x004F /* 'O' */
#define UNICODE_CAP_P           0x0050 /* 'P' */
#define UNICODE_CAP_Q           0x0051 /* 'Q' */
#define UNICODE_CAP_R           0x0052 /* 'R' */
#define UNICODE_CAP_S           0x0053 /* 'S' */
#define UNICODE_CAP_T           0x0054 /* 'T' */
#define UNICODE_CAP_U           0x0055 /* 'U' */
#define UNICODE_CAP_V           0x0056 /* 'V' */
#define UNICODE_CAP_W           0x0057 /* 'W' */
#define UNICODE_CAP_X           0x0058 /* 'X' */
#define UNICODE_CAP_Y           0x0059 /* 'Y' */
#define UNICODE_CAP_Z           0x005A /* 'Z' */
#define UNICODE_SQUARE_BRACE_L  0x005B /* '[' */
#define UNICODE_BACK_SLASH      0x005C /* '\' */
#define UNICODE_SQUARE_BRACE_R  0x005D /* ']' */
#define UNICODE_ACCENT          0x005E /* '^' */
#define UNICODE_LOW_LINE        0x005F /* '_' */
#define UNICODE_GRAVE_ACCENT    0x0060 /* '`' */
#define UNICODE_SML_A           0x0061 /* 'a' */
#define UNICODE_SML_B           0x0062 /* 'b' */
#define UNICODE_SML_C           0x0063 /* 'c' */
#define UNICODE_SML_D           0x0064 /* 'd' */
#define UNICODE_SML_E           0x0065 /* 'e' */
#define UNICODE_SML_F           0x0066 /* 'f' */
#define UNICODE_SML_G           0x0067 /* 'g' */
#define UNICODE_SML_H           0x0068 /* 'h' */
#define UNICODE_SML_I           0x0069 /* 'i' */
#define UNICODE_SML_J           0x006A /* 'j' */
#define UNICODE_SML_K           0x006B /* 'k' */
#define UNICODE_SML_L           0x006C /* 'l' */
#define UNICODE_SML_M           0x006D /* 'm' */
#define UNICODE_SML_N           0x006E /* 'n' */
#define UNICODE_SML_O           0x006F /* 'o' */
#define UNICODE_SML_P           0x0070 /* 'p' */
#define UNICODE_SML_Q           0x0071 /* 'q' */
#define UNICODE_SML_R           0x0072 /* 'r' */
#define UNICODE_SML_S           0x0073 /* 's' */
#define UNICODE_SML_T           0x0074 /* 't' */
#define UNICODE_SML_U           0x0075 /* 'u' */
#define UNICODE_SML_V           0x0076 /* 'v' */
#define UNICODE_SML_W           0x0077 /* 'w' */
#define UNICODE_SML_X           0x0078 /* 'x' */
#define UNICODE_SML_Y           0x0079 /* 'y' */
#define UNICODE_SML_Z           0x007A /* 'z' */
#define UNICODE_LEFT_BRACE      0x007B /* '{' */
#define UNICODE_VERTICAL_LINE   0x007C /* '|' */
#define UNICODE_RIGHT_BRACE     0x007D /* '}' */
#define UNICODE_TILDE           0x007E /* '~' */
#define UNICODE_DELETE          0x007F /* Delete */

#define UNICODE_NO_BREAK_SPACE             0x00A0
#define UNICODE_INVERTED_EXCLAMATION_MARK  0x00A1
#define UNICODE_CENT_SIGN                  0x00A2
#define UNICODE_POUND_SIGN                 0x00A3
#define UNICODE_CURRENCY_SIGN              0x00A4
#define UNICODE_YEN_SIGN                   0x00A5
#define UNICODE_BROKEN_BAR                 0x00A6
#define UNICODE_SECTION_SIGN               0x00A7
#define UNICODE_DIAERESIS                  0x00A8
#define UNICODE_COPYRIGHT_SIGN             0x00A9
#define UNICODE_FEMININE_ORDINAL_INDICATOR 0x00AA

#define UNICODE_LEFT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK          0x00AB
#define UNICODE_NOT_SIGN                                           0x00AC
#define UNICODE_SOFT_HYPHEN                                        0x00AD
#define UNICODE_REGISTERED_SIGN                                    0x00AE
#define UNICODE_MACRON                                             0x00AF
#define UNICODE_DEGREE_SIGN                                        0x00B0
#define UNICODE_PLUS_MINUS_SIGN                                    0x00B1
#define UNICODE_SUPERSCRIPT_TWO                                    0x00B2
#define UNICODE_SUPERSCRIPT_THREE                                  0x00B3
#define UNICODE_ACUTE_ACCENT                                       0x00B4
#define UNICODE_MICRO_SIGN                                         0x00B5
#define UNICODE_PILCROW_SIGN                                       0x00B6
#define UNICODE_MIDDLE_DOT                                         0x00B7
#define UNICODE_CEDILLA                                            0x00B8
#define UNICODE_SUPERSCRIPT_ONE                                    0x00B9
#define UNICODE_MASCULINE_ORDINAL_INDICATOR                        0x00BA
#define UNICODE_RIGHT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK         0x00BB
#define UNICODE_VULGAR_FRACTION_ONE_QUARTER                        0x00BC
#define UNICODE_VULGAR_FRACTION_ONE_HALF                           0x00BD
#define UNICODE_VULGAR_FRACTION_THREE_QUARTERS                     0x00BE
#define UNICODE_INVERTED_QUESTION_MARK                             0x00BF

#define UNICODE_CAP_A_WITH_GRAVE                  0x00C0
#define UNICODE_CAP_A_WITH_ACUTE                  0x00C1
#define UNICODE_CAP_A_WITH_CIRCUMFLEX             0x00C2
#define UNICODE_CAP_A_WITH_TILDE                  0x00C3
#define UNICODE_CAP_A_WITH_DIAERESIS              0x00C4
#define UNICODE_CAP_A_WITH_RING_ABOVE             0x00C5
#define UNICODE_CAP_AE                            0x00C6
#define UNICODE_CAP_C_WITH_CEDILLA                0x00C7
#define UNICODE_CAP_E_WITH_GRAVE                  0x00C8
#define UNICODE_CAP_E_WITH_ACUTE                  0x00C9
#define UNICODE_CAP_E_WITH_CIRCUMFLEX             0x00CA
#define UNICODE_CAP_E_WITH_DIAERESIS              0x00CB
#define UNICODE_CAP_I_WITH_GRAVE                  0x00CC
#define UNICODE_CAP_I_WITH_ACUTE                  0x00CD
#define UNICODE_CAP_I_WITH_CIRCUMFLEX             0x00CE
#define UNICODE_CAP_I_WITH_DIAERESIS              0x00CF
#define UNICODE_CAP_ETH                           0x00D0
#define UNICODE_CAP_N_WITH_TILDE                  0x00D1
#define UNICODE_CAP_O_WITH_GRAVE                  0x00D2
#define UNICODE_CAP_O_WITH_ACUTE                  0x00D3
#define UNICODE_CAP_O_WITH_CIRCUMFLEX             0x00D4
#define UNICODE_CAP_O_WITH_TILDE                  0x00D5
#define UNICODE_CAP_O_WITH_DIAERESIS              0x00D6
#define UNICODE_MULTIPLICATION_SIGN               0x00D7
#define UNICODE_CAP_O_WITH_STROKE                 0x00D8
#define UNICODE_CAP_U_WITH_GRAVE                  0x00D9
#define UNICODE_CAP_U_WITH_ACUTE                  0x00DA
#define UNICODE_CAP_U_WITH_CIRCUMFLEX             0x00DB
#define UNICODE_CAP_U_WITH_DIAERESIS              0x00DC
#define UNICODE_CAP_Y_WITH_ACUTE                  0x00DD
#define UNICODE_CAP_THORN                         0x00DE

#define UNICODE_SML_SHARP_S                         0x00DF
#define UNICODE_SML_A_WITH_GRAVE                    0x00E0
#define UNICODE_SML_A_WITH_ACUTE                    0x00E1
#define UNICODE_SML_A_WITH_CIRCUMFLEX               0x00E2
#define UNICODE_SML_A_WITH_TILDE                    0x00E3
#define UNICODE_SML_A_WITH_DIAERESIS                0x00E4
#define UNICODE_SML_A_WITH_RING_ABOVE               0x00E5
#define UNICODE_SML_AE                              0x00E6
#define UNICODE_SML_C_WITH_CEDILLA                  0x00E7
#define UNICODE_SML_E_WITH_GRAVE                    0x00E8
#define UNICODE_SML_E_WITH_ACUTE                    0x00E9
#define UNICODE_SML_E_WITH_CIRCUMFLEX               0x00EA
#define UNICODE_SML_E_WITH_DIAERESIS                0x00EB
#define UNICODE_SML_I_WITH_GRAVE                    0x00EC
#define UNICODE_SML_I_WITH_ACUTE                    0x00ED
#define UNICODE_SML_I_WITH_CIRCUMFLEX               0x00EE
#define UNICODE_SML_I_WITH_DIAERESIS                0x00EF
#define UNICODE_SML_ETH                             0x00F0
#define UNICODE_SML_N_WITH_TILDE                    0x00F1
#define UNICODE_SML_O_WITH_GRAVE                    0x00F2
#define UNICODE_SML_O_WITH_ACUTE                    0x00F3
#define UNICODE_SML_O_WITH_CIRCUMFLEX               0x00F4
#define UNICODE_SML_O_WITH_TILDE                    0x00F5
#define UNICODE_SML_O_WITH_DIAERESIS                0x00F6
#define UNICODE_DIVISION_SIGN                       0x00F7
#define UNICODE_SML_O_WITH_STROKE                   0x00F8
#define UNICODE_SML_U_WITH_GRAVE                    0x00F9
#define UNICODE_SML_U_WITH_ACUTE                    0x00FA
#define UNICODE_SML_U_WITH_CIRCUMFLEX               0x00FB
#define UNICODE_SML_U_WITH_DIAERESIS                0x00FC
#define UNICODE_SML_Y_WITH_ACUTE                    0x00FD
#define UNICODE_SML_THORN                           0x00FE
#define UNICODE_SML_Y_WITH_DIAERESIS                0x00FF

/* Private codes */

/*  Ucode describes E000 - F8FF as the private use area */

#define UNICODE_PRIVATE                   0xE000

#define UNICODE_HOME                      (UNICODE_PRIVATE + 0)
#define UNICODE_END                       (UNICODE_PRIVATE + 1)
#define UNICODE_INSERT                    (UNICODE_PRIVATE + 2)
#define UNICODE_PAGE_UP                   (UNICODE_PRIVATE + 3)
#define UNICODE_PAGE_DOWN                 (UNICODE_PRIVATE + 4)
#define UNICODE_LEFT_ARROW                (UNICODE_PRIVATE + 5)
#define UNICODE_RIGHT_ARROW               (UNICODE_PRIVATE + 6)
#define UNICODE_UP_ARROW                  (UNICODE_PRIVATE + 7)
#define UNICODE_DOWN_ARROW                (UNICODE_PRIVATE + 8)
#define UNICODE_PRINT_SCREEN              (UNICODE_PRIVATE + 9)
#define UNICODE_PAUSE                     (UNICODE_PRIVATE + 10)
#define UNICODE_CAPS_LOCK                 (UNICODE_PRIVATE + 11)
#define UNICODE_NUM_LOCK                  (UNICODE_PRIVATE + 12)
#define UNICODE_SCROLL_LOCK               (UNICODE_PRIVATE + 13)

#define UNICODE_LEFT_SHIFT                (UNICODE_PRIVATE + 14)
#define UNICODE_RIGHT_SHIFT               (UNICODE_PRIVATE + 15)
#define UNICODE_LEFT_CTRL                 (UNICODE_PRIVATE + 16)
#define UNICODE_RIGHT_CTRL                (UNICODE_PRIVATE + 17)
#define UNICODE_LEFT_ALT                  (UNICODE_PRIVATE + 18)
#define UNICODE_RIGHT_ALT                 (UNICODE_PRIVATE + 19)
#define UNICODE_PROGRAM                   (UNICODE_PRIVATE + 20)
#define UNICODE_BACKTAB                   (UNICODE_PRIVATE + 21)

/* Functions keys */

#define UNICODE_FUNC_BASE                 (UNICODE_PRIVATE + 0x0f00)
#define UNICODE_FUNC_KEY                  (UNICODE_FUNC_BASE + 0)
#define UNICODE_F1                        (UNICODE_FUNC_BASE + 1)
#define UNICODE_F2                        (UNICODE_FUNC_BASE + 2)
#define UNICODE_F3                        (UNICODE_FUNC_BASE + 3)
#define UNICODE_F4                        (UNICODE_FUNC_BASE + 4)
#define UNICODE_F5                        (UNICODE_FUNC_BASE + 5)
#define UNICODE_F6                        (UNICODE_FUNC_BASE + 6)
#define UNICODE_F7                        (UNICODE_FUNC_BASE + 7)
#define UNICODE_F8                        (UNICODE_FUNC_BASE + 8)
#define UNICODE_F9                        (UNICODE_FUNC_BASE + 9)
#define UNICODE_F10                       (UNICODE_FUNC_BASE + 10)
#define UNICODE_F11                       (UNICODE_FUNC_BASE + 11)
#define UNICODE_F12                       (UNICODE_FUNC_BASE + 12)
#define UNICODE_FUNC(n)                   (UNICODE_FUNC_BASE + (n))

#define UNICODE_FUNC_LAST                 (UNICODE_FUNC_BASE + 0x00ff)

/* User defined chars */
#define UNICODE_USER                      0xF000

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __UNI_CODE_H */

/* end of file */
