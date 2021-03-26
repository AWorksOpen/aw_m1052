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
 * \brief ����ϵͳ�����붨��
 *
 * \internal
 * \par modification history
 * - 1.01 14-09-26  ops, add code definition.
 * - 1.00 14-07-18  ops, first implementation.
 * \endinternal
 */


#ifndef __AW_INPUT_CODE_H
#define __AW_INPUT_CODE_H

#include "aw_common.h"
#include "aw_types.h"
#include "aw_errno.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

/**
 * \addtogroup grp_aw_if_input_code
 * \copydoc aw_input_code.h
 * @{
 */
 
/**
 * \name ABS code ��������
 * @{
 */
#define ABS_RESERVED        0x00
#define ABS_COORD           0x01
#define ABS_THROTTLE        0x06
#define ABS_RUDDER          0x07
#define ABS_WHEEL           0x08
#define ABS_GAS             0x09
#define ABS_BRAKE           0x0a
#define ABS_HAT0X           0x10
#define ABS_HAT0Y           0x11
#define ABS_HAT1X           0x12
#define ABS_HAT1Y           0x13
#define ABS_HAT2X           0x14
#define ABS_HAT2Y           0x15
#define ABS_HAT3X           0x16
#define ABS_HAT3Y           0x17
#define ABS_PRESSURE        0x18
#define ABS_DISTANCE        0x19
#define ABS_TILT_X          0x1a
#define ABS_TILT_Y          0x1b
#define ABS_TOOL_WIDTH      0x1c
#define ABS_VOLUME          0x20
#define ABS_MISC            0x28

#define ABS_MAX             0x3f
#define ABS_CNT             (ABS_MAX + 1)
/** @} ABS code */

/**
 * \name REL code �������
 * @{
 */
#define REL_RESERVED        0x00
#define REL_COORD           0x01
#define REL_HWHEEL          0x06
#define REL_DIAL            0x07
#define REL_WHEEL           0x08
#define REL_MISC            0x09
#define REL_MAX             0x0f
#define REL_CNT             (REL_MAX + 1)
/** @} REL code */

/**
 * \name key code�����Լ��̰���ֵ
 * @{
 */
#define KEY_RESERVED            0  /**< \brief ����ֵ */
#define KEY_ESC                 1  /**< \brief 'ESC' */
#define KEY_1                   2  /**< \brief '1'  */
#define KEY_2                   3  /**< \brief '2'  */
#define KEY_3                   4  /**< \brief '3'  */
#define KEY_4                   5  /**< \brief '4'  */
#define KEY_5                   6  /**< \brief '5'  */
#define KEY_6                   7  /**< \brief '6'  */
#define KEY_7                   8  /**< \brief '7'  */
#define KEY_8                   9  /**< \brief '8'  */
#define KEY_9                   10 /**< \brief '9'  */
#define KEY_0                   11 /**< \brief '0'  */
#define KEY_MINUS               12 /**< \brief '-'  */
#define KEY_EQUAL               13 /**< \brief '='  */
#define KEY_BACKSPACE           14 /**< \brief �˸� */
#define KEY_TAB                 15 /**< \brief table */
#define KEY_Q                   16 /**< \brief 'Q' */
#define KEY_W                   17 /**< \brief 'W' */
#define KEY_E                   18 /**< \brief 'E' */
#define KEY_R                   19 /**< \brief 'R' */
#define KEY_T                   20 /**< \brief 'T' */
#define KEY_Y                   21 /**< \brief 'Y' */
#define KEY_U                   22 /**< \brief 'U' */
#define KEY_I                   23 /**< \brief 'I' */
#define KEY_O                   24 /**< \brief 'O' */
#define KEY_P                   25 /**< \brief 'P' */
#define KEY_LEFTBRACE           26 /**< \brief '[' */
#define KEY_RIGHTBRACE          27 /**< \brief ']' */
#define KEY_ENTER               28 /**< \brief �س� */
#define KEY_LEFTCTRL            29 /**< \brief ���'CTRL'�� */
#define KEY_A                   30 /**< \brief 'A' */
#define KEY_S                   31 /**< \brief 'S' */
#define KEY_D                   32 /**< \brief 'D' */
#define KEY_F                   33 /**< \brief 'F' */
#define KEY_G                   34 /**< \brief 'G' */
#define KEY_H                   35 /**< \brief 'H' */
#define KEY_J                   36 /**< \brief 'J' */
#define KEY_K                   37 /**< \brief 'K' */
#define KEY_L                   38 /**< \brief 'L' */
#define KEY_SEMICOLON           39 /**< \brief �ֺ�';' */
#define KEY_APOSTROPHE          40 /**< \brief ��Ʋ��''' */
#define KEY_GRAVE               41 /**< \brief '\' */
#define KEY_LEFTSHIFT           42 /**< \brief ���shift�� */
#define KEY_BACKSLASH           43 /**< \brief '\' */
#define KEY_Z                   44 /**< \brief 'Z' */
#define KEY_X                   45 /**< \brief 'X' */
#define KEY_C                   46 /**< \brief 'C' */
#define KEY_V                   47 /**< \brief 'V' */
#define KEY_B                   48 /**< \brief 'B' */
#define KEY_N                   49 /**< \brief 'N' */
#define KEY_M                   50 /**< \brief 'M' */
#define KEY_COMMA               51 /**< \brief ',' */
#define KEY_DOT                 52 /**< \brief '.' */
#define KEY_SLASH               53 /**< \brief '/' */
#define KEY_RIGHTSHIFT          54 /**< \brief �ұ�shift�� */
#define KEY_KPASTERISK          55 /**< \brief ���ֺ� */
#define KEY_LEFTALT             56 /**< \brief ���'Alt'�� */
#define KEY_SPACE               57 /**< \brief �ո� */
#define KEY_CAPSLOCK            58 /**< \brief ��Сд��ĸת���� "Caps Lock" */
#define KEY_F1                  59 /**< \brief 'F1' */
#define KEY_F2                  60 /**< \brief 'F2' */
#define KEY_F3                  61 /**< \brief 'F3' */
#define KEY_F4                  62 /**< \brief 'F4' */
#define KEY_F5                  63 /**< \brief 'F5' */
#define KEY_F6                  64 /**< \brief 'F6' */
#define KEY_F7                  65 /**< \brief 'F7' */
#define KEY_F8                  66 /**< \brief 'F8' */
#define KEY_F9                  67 /**< \brief 'F9' */
#define KEY_F10                 68 /**< \brief 'F10' */
#define KEY_NUMLOCK             69 /**< \brief 'NUMLOCK' */
#define KEY_SCROLLLOCK          70 /**< \brief 'SCROLLLOCK' */
#define KEY_KP7                 71 /**< \brief  */
#define KEY_KP8                 72 /**< \brief  */
#define KEY_KP9                 73 /**< \brief  */
#define KEY_KPMINUS             74 /**< \brief  */
#define KEY_KP4                 75 /**< \brief  */
#define KEY_KP5                 76 /**< \brief  */
#define KEY_KP6                 77 /**< \brief  */
#define KEY_KPPLUS              78 /**< \brief  */
#define KEY_KP1                 79 /**< \brief  */
#define KEY_KP2                 80 /**< \brief  */
#define KEY_KP3                 81 /**< \brief  */
#define KEY_KP0                 82 /**< \brief  */
#define KEY_KPDOT               83 /**< \brief  */

#define KEY_ZENKAKUHANKAKU      85
#define KEY_102ND               86
#define KEY_F11                 87
#define KEY_F12                 88
#define KEY_RO                  89
#define KEY_KATAKANA            90
#define KEY_HIRAGANA            91
#define KEY_HENKAN              92
#define KEY_KATAKANAHIRAGANA    93
#define KEY_MUHENKAN            94
#define KEY_KPJPCOMMA           95
#define KEY_KPENTER             96
#define KEY_RIGHTCTRL           97
#define KEY_KPSLASH             98
#define KEY_SYSRQ               99
#define KEY_RIGHTALT            100
#define KEY_LINEFEED            101
#define KEY_HOME                102
#define KEY_UP                  103
#define KEY_PAGEUP              104
#define KEY_LEFT                105
#define KEY_RIGHT               106
#define KEY_END                 107
#define KEY_DOWN                108
#define KEY_PAGEDOWN            109
#define KEY_INSERT              110
#define KEY_DELETE              111
#define KEY_MACRO               112
#define KEY_MUTE                113
#define KEY_VOLUMEDOWN          114
#define KEY_VOLUMEUP            115
#define KEY_POWER               116 /**< \brief SC System Power Down */
#define KEY_KPEQUAL             117
#define KEY_KPPLUSMINUS         118
#define KEY_PAUSE               119
#define KEY_SCALE               120 /**< \brief AL Compiz Scale (Expose) */

#define KEY_KPCOMMA             121
#define KEY_HANGEUL             122
#define KEY_HANGUEL             KEY_HANGEUL
#define KEY_HANJA               123
#define KEY_YEN                 124
#define KEY_LEFTMETA            125
#define KEY_RIGHTMETA           126
#define KEY_COMPOSE             127

#define KEY_STOP                128 /**< \brief AC Stop */
#define KEY_AGAIN               129
#define KEY_PROPS               130 /**< \brief AC Properties */
#define KEY_UNDO                131 /**< \brief AC Undo */
#define KEY_FRONT               132
#define KEY_COPY                133 /**< \brief AC Copy */
#define KEY_OPEN                134 /**< \brief AC Open */
#define KEY_PASTE               135 /**< \brief AC Paste */
#define KEY_FIND                136 /**< \brief AC Search */
#define KEY_CUT                 137 /**< \brief AC Cut */
#define KEY_HELP                138 /**< \brief AL Integrated Help Center */
#define KEY_MENU                139 /**< \brief Menu (show menu) */
#define KEY_CALC                140 /**< \brief AL Calculator */
#define KEY_SETUP               141
#define KEY_SLEEP               142 /**< \brief SC System Sleep */
#define KEY_WAKEUP              143 /**< \brief System Wake Up */
#define KEY_FILE                144 /**< \brief AL Local Machine Browser */
#define KEY_SENDFILE            145
#define KEY_DELETEFILE          146
#define KEY_XFER                147
#define KEY_PROG1               148
#define KEY_PROG2               149
#define KEY_WWW                 150 /**< \brief AL Internet Browser */
#define KEY_MSDOS               151
#define KEY_COFFEE              152 /**< \brief AL Terminal Lock/Screensaver */
#define KEY_SCREENLOCK          KEY_COFFEE
#define KEY_DIRECTION           153
#define KEY_CYCLEWINDOWS        154
#define KEY_MAIL                155
#define KEY_BOOKMARKS           156 /**< \brief AC Bookmarks */
#define KEY_COMPUTER            157
#define KEY_BACK                158 /**< \brief AC Back */
#define KEY_FORWARD             159 /**< \brief AC Forward */
#define KEY_CLOSECD             160
#define KEY_EJECTCD             161
#define KEY_EJECTCLOSECD        162
#define KEY_NEXTSONG            163
#define KEY_PLAYPAUSE           164
#define KEY_PREVIOUSSONG        165
#define KEY_STOPCD              166
#define KEY_RECORD              167
#define KEY_REWIND              168
#define KEY_PHONE               169 /**< \brief Media Select Telephone */
#define KEY_ISO                 170
#define KEY_CONFIG              171 /**< \brief AL Consumer Control Configuration */
#define KEY_HOMEPAGE            172 /**< \brief AC Home */
#define KEY_REFRESH             173 /**< \brief AC Refresh */
#define KEY_EXIT                174 /**< \brief AC Exit */
#define KEY_MOVE                175
#define KEY_EDIT                176
#define KEY_SCROLLUP            177
#define KEY_SCROLLDOWN          178
#define KEY_KPLEFTPAREN         179
#define KEY_KPRIGHTPAREN        180
#define KEY_NEW                 181 /**< \brief AC New */
#define KEY_REDO                182 /**< \brief AC Redo/Repeat */

#define KEY_F13                 183
#define KEY_F14                 184
#define KEY_F15                 185
#define KEY_F16                 186
#define KEY_F17                 187
#define KEY_F18                 188
#define KEY_F19                 189
#define KEY_F20                 190
#define KEY_F21                 191
#define KEY_F22                 192
#define KEY_F23                 193
#define KEY_F24                 194

#define KEY_PLAYCD              200
#define KEY_PAUSECD             201
#define KEY_PROG3               202
#define KEY_PROG4               203
#define KEY_DASHBOARD           204 /**< \brief AL Dashboard */
#define KEY_SUSPEND             205
#define KEY_CLOSE               206 /**< \brief AC Close */
#define KEY_PLAY                207
#define KEY_FASTFORWARD         208
#define KEY_BASSBOOST           209
#define KEY_PRINT               210 /**< \brief AC Print */
#define KEY_HP                  211
#define KEY_CAMERA              212
#define KEY_SOUND               213
#define KEY_QUESTION            214
#define KEY_EMAIL               215
#define KEY_CHAT                216
#define KEY_SEARCH              217
#define KEY_CONNECT             218
#define KEY_FINANCE             219 /**< \brief AL Checkbook/Finance */
#define KEY_SPORT               220
#define KEY_SHOP                221
#define KEY_ALTERASE            222
#define KEY_CANCEL              223 /**< \brief AC Cancel */
#define KEY_BRIGHTNESSDOWN      224
#define KEY_BRIGHTNESSUP        225
#define KEY_MEDIA               226

/**
 * \brief Cycle between available video
 *        outputs (Monitor/LCD/TV-out/etc)
 */
#define KEY_SWITCHVIDEOMODE     227 
#define KEY_KBDILLUMTOGGLE      228
#define KEY_KBDILLUMDOWN        229
#define KEY_KBDILLUMUP          230

#define KEY_SEND                231 /**< \brief AC Send */
#define KEY_REPLY               232 /**< \brief AC Reply */
#define KEY_FORWARDMAIL         233 /**< \brief AC Forward Msg */
#define KEY_SAVE                234 /**< \brief AC Save */
#define KEY_DOCUMENTS           235

#define KEY_BATTERY             236

#define KEY_BLUETOOTH           237
#define KEY_WLAN                238
#define KEY_UWB                 239

#define KEY_UNKNOWN             240

#define KEY_VIDEO_NEXT          241 /**< \brief drive next video source */
#define KEY_VIDEO_PREV          242 /**< \brief drive previous video source */
#define KEY_BRIGHTNESS_CYCLE    243 /**< \brief brightness up, after max is min */
#define KEY_BRIGHTNESS_ZERO     244 /**< \brief brightness off, use ambient */
#define KEY_DISPLAY_OFF         245 /**< \brief display device to off state */

#define KEY_WIMAX               246
#define KEY_RFKILL              247 /**< \brief Key that controls all radios */

#define KEY_MICMUTE             248 /**< \brief Mute / unmute the microphone */

/* Code 255 is reserved for special needs of AT keyboard driver */

#define BTN_MISC                0x100
#define BTN_0                   0x100
#define BTN_1                   0x101
#define BTN_2                   0x102
#define BTN_3                   0x103
#define BTN_4                   0x104
#define BTN_5                   0x105
#define BTN_6                   0x106
#define BTN_7                   0x107
#define BTN_8                   0x108
#define BTN_9                   0x109

#define BTN_MOUSE               0x110
#define BTN_LEFT                0x110
#define BTN_RIGHT               0x111
#define BTN_MIDDLE              0x112
#define BTN_SIDE                0x113
#define BTN_EXTRA               0x114
#define BTN_FORWARD             0x115
#define BTN_BACK                0x116
#define BTN_TASK                0x117

#define BTN_JOYSTICK            0x120
#define BTN_TRIGGER             0x120
#define BTN_THUMB               0x121
#define BTN_THUMB2              0x122
#define BTN_TOP                 0x123
#define BTN_TOP2                0x124
#define BTN_PINKIE              0x125
#define BTN_BASE                0x126
#define BTN_BASE2               0x127
#define BTN_BASE3               0x128
#define BTN_BASE4               0x129
#define BTN_BASE5               0x12a
#define BTN_BASE6               0x12b
#define BTN_DEAD                0x12f

#define BTN_GAMEPAD             0x130
#define BTN_A                   0x130
#define BTN_B                   0x131
#define BTN_C                   0x132
#define BTN_X                   0x133
#define BTN_Y                   0x134
#define BTN_Z                   0x135
#define BTN_TL                  0x136
#define BTN_TR                  0x137
#define BTN_TL2                 0x138
#define BTN_TR2                 0x139
#define BTN_SELECT              0x13a
#define BTN_START               0x13b
#define BTN_MODE                0x13c
#define BTN_THUMBL              0x13d
#define BTN_THUMBR              0x13e

#define BTN_DIGI                0x140
#define BTN_TOOL_PEN            0x140
#define BTN_TOOL_RUBBER         0x141
#define BTN_TOOL_BRUSH          0x142
#define BTN_TOOL_PENCIL         0x143
#define BTN_TOOL_AIRBRUSH       0x144
#define BTN_TOOL_FINGER         0x145
#define BTN_TOOL_MOUSE          0x146
#define BTN_TOOL_LENS           0x147
#define BTN_TOOL_QUINTTAP       0x148 /**< \brief Five fingers on trackpad */
#define BTN_TOUCH               0x14a
#define BTN_STYLUS              0x14b
#define BTN_STYLUS2             0x14c
#define BTN_TOOL_DOUBLETAP      0x14d
#define BTN_TOOL_TRIPLETAP      0x14e
#define BTN_TOOL_QUADTAP        0x14f /**< \brief Four fingers on trackpad */

#define BTN_WHEEL               0x150
#define BTN_GEAR_DOWN           0x150
#define BTN_GEAR_UP             0x151

#define KEY_OK                  0x160
#define KEY_SELECT              0x161
#define KEY_GOTO                0x162
#define KEY_CLEAR               0x163
#define KEY_POWER2              0x164
#define KEY_OPTION              0x165
#define KEY_INFO                0x166 /**< \brief AL OEM Features/Tips/Tutorial */
#define KEY_TIME                0x167
#define KEY_VENDOR              0x168
#define KEY_ARCHIVE             0x169
#define KEY_PROGRAM             0x16a /**< \brief Media Select Program Guide */
#define KEY_CHANNEL             0x16b
#define KEY_FAVORITES           0x16c
#define KEY_EPG                 0x16d
#define KEY_PVR                 0x16e /**< \brief Media Select Home */
#define KEY_MHP                 0x16f
#define KEY_LANGUAGE            0x170
#define KEY_TITLE               0x171
#define KEY_SUBTITLE            0x172
#define KEY_ANGLE               0x173
#define KEY_ZOOM                0x174
#define KEY_MODE                0x175
#define KEY_KEYBOARD            0x176
#define KEY_SCREEN              0x177
#define KEY_PC                  0x178 /**< \brief Media Select Computer */
#define KEY_TV                  0x179 /**< \brief Media Select TV */
#define KEY_TV2                 0x17a /**< \brief Media Select Cable */
#define KEY_VCR                 0x17b /**< \brief Media Select VCR */
#define KEY_VCR2                0x17c /**< \brief VCR Plus */
#define KEY_SAT                 0x17d /**< \brief Media Select Satellite */
#define KEY_SAT2                0x17e
#define KEY_CD                  0x17f /**< \brief Media Select CD */
#define KEY_TAPE                0x180 /**< \brief Media Select Tape */
#define KEY_RADIO               0x181
#define KEY_TUNER               0x182 /**< \brief Media Select Tuner */
#define KEY_PLAYER              0x183
#define KEY_TEXT                0x184
#define KEY_DVD                 0x185 /**< \brief Media Select DVD */
#define KEY_AUX                 0x186
#define KEY_MP3                 0x187
#define KEY_AUDIO               0x188 /**< \brief AL Audio Browser */
#define KEY_VIDEO               0x189 /**< \brief AL Movie Browser */
#define KEY_DIRECTORY           0x18a
#define KEY_LIST                0x18b
#define KEY_MEMO                0x18c /**< \brief Media Select Messages */
#define KEY_CALENDAR            0x18d
#define KEY_RED                 0x18e
#define KEY_GREEN               0x18f
#define KEY_YELLOW              0x190
#define KEY_BLUE                0x191
#define KEY_CHANNELUP           0x192 /**< \brief Channel Increment */
#define KEY_CHANNELDOWN         0x193 /**< \brief Channel Decrement */
#define KEY_FIRST               0x194
#define KEY_LAST                0x195 /**< \brief Recall Last */
#define KEY_AB                  0x196
#define KEY_NEXT                0x197
#define KEY_RESTART             0x198
#define KEY_SLOW                0x199
#define KEY_SHUFFLE             0x19a
#define KEY_BREAK               0x19b
#define KEY_PREVIOUS            0x19c
#define KEY_DIGITS              0x19d
#define KEY_TEEN                0x19e
#define KEY_TWEN                0x19f
#define KEY_VIDEOPHONE          0x1a0 /**< \brief Media Select Video Phone */
#define KEY_GAMES               0x1a1 /**< \brief Media Select Games */
#define KEY_ZOOMIN              0x1a2 /**< \brief AC Zoom In */
#define KEY_ZOOMOUT             0x1a3 /**< \brief AC Zoom Out */
#define KEY_ZOOMRESET           0x1a4 /**< \brief AC Zoom */
#define KEY_WORDPROCESSOR       0x1a5 /**< \brief AL Word Processor */
#define KEY_EDITOR              0x1a6 /**< \brief AL Text Editor */
#define KEY_SPREADSHEET         0x1a7 /**< \brief AL Spreadsheet */
#define KEY_GRAPHICSEDITOR      0x1a8 /**< \brief AL Graphics Editor */
#define KEY_PRESENTATION        0x1a9 /**< \brief AL Presentation App */
#define KEY_DATABASE            0x1aa /**< \brief AL Database App */
#define KEY_NEWS                0x1ab /**< \brief AL Newsreader */
#define KEY_VOICEMAIL           0x1ac /**< \brief AL Voicemail */
#define KEY_ADDRESSBOOK         0x1ad /**< \brief AL Contacts/Address Book */
#define KEY_MESSENGER           0x1ae /**< \brief AL Instant Messaging */
#define KEY_DISPLAYTOGGLE       0x1af /**< \brief Turn display (LCD) on and off */
#define KEY_SPELLCHECK          0x1b0 /**< \brief AL Spell Check */
#define KEY_LOGOFF              0x1b1 /**< \brief AL Logoff */

#define KEY_DOLLAR              0x1b2
#define KEY_EURO                0x1b3

#define KEY_FRAMEBACK           0x1b4 /**< \brief Consumer - transport controls */
#define KEY_FRAMEFORWARD        0x1b5
#define KEY_CONTEXT_MENU        0x1b6 /**< \brief GenDesc - system context menu */
#define KEY_MEDIA_REPEAT        0x1b7 /**< \brief Consumer - transport control */
#define KEY_10CHANNELSUP        0x1b8 /**< \brief 10 channels up (10+) */
#define KEY_10CHANNELSDOWN      0x1b9 /**< \brief 10 channels down (10-) */
#define KEY_IMAGES              0x1ba /**< \brief AL Image Browser */

#define KEY_DEL_EOL             0x1c0
#define KEY_DEL_EOS             0x1c1
#define KEY_INS_LINE            0x1c2
#define KEY_DEL_LINE            0x1c3

#define KEY_FN                  0x1d0
#define KEY_FN_ESC              0x1d1
#define KEY_FN_F1               0x1d2
#define KEY_FN_F2               0x1d3
#define KEY_FN_F3               0x1d4
#define KEY_FN_F4               0x1d5
#define KEY_FN_F5               0x1d6
#define KEY_FN_F6               0x1d7
#define KEY_FN_F7               0x1d8
#define KEY_FN_F8               0x1d9
#define KEY_FN_F9               0x1da
#define KEY_FN_F10              0x1db
#define KEY_FN_F11              0x1dc
#define KEY_FN_F12              0x1dd
#define KEY_FN_1                0x1de
#define KEY_FN_2                0x1df
#define KEY_FN_D                0x1e0
#define KEY_FN_E                0x1e1
#define KEY_FN_F                0x1e2
#define KEY_FN_S                0x1e3
#define KEY_FN_B                0x1e4

#define KEY_BRL_DOT1            0x1f1
#define KEY_BRL_DOT2            0x1f2
#define KEY_BRL_DOT3            0x1f3
#define KEY_BRL_DOT4            0x1f4
#define KEY_BRL_DOT5            0x1f5
#define KEY_BRL_DOT6            0x1f6
#define KEY_BRL_DOT7            0x1f7
#define KEY_BRL_DOT8            0x1f8
#define KEY_BRL_DOT9            0x1f9
#define KEY_BRL_DOT10           0x1fa

#define KEY_NUMERIC_0           0x200 /**< \brief used by phones, remote controls, */
#define KEY_NUMERIC_1           0x201 /**< \brief and other keypads */
#define KEY_NUMERIC_2           0x202
#define KEY_NUMERIC_3           0x203
#define KEY_NUMERIC_4           0x204
#define KEY_NUMERIC_5           0x205
#define KEY_NUMERIC_6           0x206
#define KEY_NUMERIC_7           0x207
#define KEY_NUMERIC_8           0x208
#define KEY_NUMERIC_9           0x209
#define KEY_NUMERIC_STAR        0x20a
#define KEY_NUMERIC_POUND       0x20b

#define KEY_CAMERA_FOCUS        0x210
#define KEY_WPS_BUTTON          0x211 /**< \brief WiFi Protected Setup key */

#define KEY_TOUCHPAD_TOGGLE     0x212 /**< \brief Request switch touchpad on or off */
#define KEY_TOUCHPAD_ON         0x213
#define KEY_TOUCHPAD_OFF        0x214

#define KEY_CAMERA_ZOOMIN       0x215
#define KEY_CAMERA_ZOOMOUT      0x216
#define KEY_CAMERA_UP           0x217
#define KEY_CAMERA_DOWN         0x218
#define KEY_CAMERA_LEFT         0x219
#define KEY_CAMERA_RIGHT        0x21a

#define BTN_TRIGGER_HAPPY               0x2c0
#define BTN_TRIGGER_HAPPY1              0x2c0
#define BTN_TRIGGER_HAPPY2              0x2c1
#define BTN_TRIGGER_HAPPY3              0x2c2
#define BTN_TRIGGER_HAPPY4              0x2c3
#define BTN_TRIGGER_HAPPY5              0x2c4
#define BTN_TRIGGER_HAPPY6              0x2c5
#define BTN_TRIGGER_HAPPY7              0x2c6
#define BTN_TRIGGER_HAPPY8              0x2c7
#define BTN_TRIGGER_HAPPY9              0x2c8
#define BTN_TRIGGER_HAPPY10             0x2c9
#define BTN_TRIGGER_HAPPY11             0x2ca
#define BTN_TRIGGER_HAPPY12             0x2cb
#define BTN_TRIGGER_HAPPY13             0x2cc
#define BTN_TRIGGER_HAPPY14             0x2cd
#define BTN_TRIGGER_HAPPY15             0x2ce
#define BTN_TRIGGER_HAPPY16             0x2cf
#define BTN_TRIGGER_HAPPY17             0x2d0
#define BTN_TRIGGER_HAPPY18             0x2d1
#define BTN_TRIGGER_HAPPY19             0x2d2
#define BTN_TRIGGER_HAPPY20             0x2d3
#define BTN_TRIGGER_HAPPY21             0x2d4
#define BTN_TRIGGER_HAPPY22             0x2d5
#define BTN_TRIGGER_HAPPY23             0x2d6
#define BTN_TRIGGER_HAPPY24             0x2d7
#define BTN_TRIGGER_HAPPY25             0x2d8
#define BTN_TRIGGER_HAPPY26             0x2d9
#define BTN_TRIGGER_HAPPY27             0x2da
#define BTN_TRIGGER_HAPPY28             0x2db
#define BTN_TRIGGER_HAPPY29             0x2dc
#define BTN_TRIGGER_HAPPY30             0x2dd
#define BTN_TRIGGER_HAPPY31             0x2de
#define BTN_TRIGGER_HAPPY32             0x2df
#define BTN_TRIGGER_HAPPY33             0x2e0
#define BTN_TRIGGER_HAPPY34             0x2e1
#define BTN_TRIGGER_HAPPY35             0x2e2
#define BTN_TRIGGER_HAPPY36             0x2e3
#define BTN_TRIGGER_HAPPY37             0x2e4
#define BTN_TRIGGER_HAPPY38             0x2e5
#define BTN_TRIGGER_HAPPY39             0x2e6
#define BTN_TRIGGER_HAPPY40             0x2e7

#define KEY_MIN_INTERESTING     KEY_MUTE
#define KEY_MAX                 0x2ff
#define KEY_CNT                 (KEY_MAX+1)

/** @} key code */


/** @}  grp_aw_if_input_code */

#ifdef __cplusplus
}
#endif

#endif /* __AW_INPUT_KEY_H */

/* end of file */
