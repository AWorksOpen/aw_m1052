/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#ifndef __V4AW_CONTROLS_H
#define __V4AW_CONTROLS_H

/* 控制类*/
#define V4AW_CTRL_CLASS_USER        0x00980000         /* 旧类型的“用户”控制*/
#define V4AW_CTRL_CLASS_MPEG        0x00990000         /* MPEG压缩控制*/
#define V4AW_CTRL_CLASS_CAMERA      0x009a0000         /* 摄像头类控制*/
#define V4AW_CTRL_CLASS_FM_TX       0x009b0000         /* FM调制器控制*/
#define V4AW_CTRL_CLASS_FLASH       0x009c0000         /* 摄像头闪光灯控制*/
#define V4AW_CTRL_CLASS_JPEG        0x009d0000         /* JPEG压缩控制*/
#define V4AW_CTRL_CLASS_IMAGE_SOURCE    0x009e0000     /* 图像源控制*/
#define V4AW_CTRL_CLASS_IMAGE_PROC  0x009f0000         /* 图像处理控制*/
#define V4AW_CTRL_CLASS_DV      0x00a00000             /* 数字视频控制*/
#define V4AW_CTRL_CLASS_FM_RX       0x00a10000         /* FM接收器控制*/
#define V4AW_CTRL_CLASS_RF_TUNER    0x00a20000         /* RF调谐器控制*/
#define V4AW_CTRL_CLASS_DETECT      0x00a30000         /* 检测控制*/

/* 用户类控制ID*/

#define V4AW_CID_BASE           (V4AW_CTRL_CLASS_USER | 0x900)
#define V4AW_CID_USER_BASE      V4AW_CID_BASE
#define V4AW_CID_USER_CLASS         (V4AW_CTRL_CLASS_USER | 1)
#define V4AW_CID_BRIGHTNESS     (V4AW_CID_BASE+0)
#define V4AW_CID_CONTRAST       (V4AW_CID_BASE+1)
#define V4AW_CID_SATURATION     (V4AW_CID_BASE+2)
#define V4AW_CID_HUE            (V4AW_CID_BASE+3)
#define V4AW_CID_AUDIO_VOLUME       (V4AW_CID_BASE+5)
#define V4AW_CID_AUDIO_BALANCE      (V4AW_CID_BASE+6)
#define V4AW_CID_AUDIO_BASS     (V4AW_CID_BASE+7)
#define V4AW_CID_AUDIO_TREBLE       (V4AW_CID_BASE+8)
#define V4AW_CID_AUDIO_MUTE     (V4AW_CID_BASE+9)
#define V4AW_CID_AUDIO_LOUDNESS     (V4AW_CID_BASE+10)
#define V4AW_CID_BLACK_LEVEL        (V4AW_CID_BASE+11) /* Deprecated */
#define V4AW_CID_AUTO_WHITE_BALANCE (V4AW_CID_BASE+12)
#define V4AW_CID_DO_WHITE_BALANCE   (V4AW_CID_BASE+13)
#define V4AW_CID_RED_BALANCE        (V4AW_CID_BASE+14)
#define V4AW_CID_BLUE_BALANCE       (V4AW_CID_BASE+15)
#define V4AW_CID_GAMMA          (V4AW_CID_BASE+16)
#define V4AW_CID_WHITENESS      (V4AW_CID_GAMMA) /* Deprecated */
#define V4AW_CID_EXPOSURE       (V4AW_CID_BASE+17)
#define V4AW_CID_AUTOGAIN       (V4AW_CID_BASE+18)
#define V4AW_CID_GAIN           (V4AW_CID_BASE+19)
#define V4AW_CID_HFLIP          (V4AW_CID_BASE+20)
#define V4AW_CID_VFLIP          (V4AW_CID_BASE+21)

#define V4AW_CID_POWER_LINE_FREQUENCY   (V4AW_CID_BASE+24)

#define V4AW_CID_HUE_AUTO           (V4AW_CID_BASE+25)
#define V4AW_CID_WHITE_BALANCE_TEMPERATURE  (V4AW_CID_BASE+26)
#define V4AW_CID_SHARPNESS          (V4AW_CID_BASE+27)
#define V4AW_CID_BACKLIGHT_COMPENSATION     (V4AW_CID_BASE+28)
#define V4AW_CID_CHROMA_AGC                     (V4AW_CID_BASE+29)
#define V4AW_CID_COLOR_KILLER                   (V4AW_CID_BASE+30)
#define V4AW_CID_COLORFX            (V4AW_CID_BASE+31)

/*  摄像头类控制ID*/

#define V4AW_CID_CAMERA_CLASS_BASE  (V4AW_CTRL_CLASS_CAMERA | 0x900)
#define V4AW_CID_CAMERA_CLASS       (V4AW_CTRL_CLASS_CAMERA | 1)

#define V4AW_CID_EXPOSURE_AUTO          (V4AW_CID_CAMERA_CLASS_BASE+1)
enum  v4aw_exposure_auto_type {
    V4AW_EXPOSURE_AUTO = 0,
    V4AW_EXPOSURE_MANUAL = 1,
    V4AW_EXPOSURE_SHUTTER_PRIORITY = 2,
    V4AW_EXPOSURE_APERTURE_PRIORITY = 3
};
#define V4AW_CID_EXPOSURE_ABSOLUTE      (V4AW_CID_CAMERA_CLASS_BASE+2)
#define V4AW_CID_EXPOSURE_AUTO_PRIORITY     (V4AW_CID_CAMERA_CLASS_BASE+3)

#define V4AW_CID_PAN_RELATIVE           (V4AW_CID_CAMERA_CLASS_BASE+4)
#define V4AW_CID_TILT_RELATIVE          (V4AW_CID_CAMERA_CLASS_BASE+5)
#define V4AW_CID_PAN_RESET          (V4AW_CID_CAMERA_CLASS_BASE+6)
#define V4AW_CID_TILT_RESET         (V4AW_CID_CAMERA_CLASS_BASE+7)

#define V4AW_CID_PAN_ABSOLUTE           (V4AW_CID_CAMERA_CLASS_BASE+8)
#define V4AW_CID_TILT_ABSOLUTE          (V4AW_CID_CAMERA_CLASS_BASE+9)

#define V4AW_CID_FOCUS_ABSOLUTE         (V4AW_CID_CAMERA_CLASS_BASE+10)
#define V4AW_CID_FOCUS_RELATIVE         (V4AW_CID_CAMERA_CLASS_BASE+11)
#define V4AW_CID_FOCUS_AUTO         (V4AW_CID_CAMERA_CLASS_BASE+12)

#define V4AW_CID_ZOOM_ABSOLUTE          (V4AW_CID_CAMERA_CLASS_BASE+13)
#define V4AW_CID_ZOOM_RELATIVE          (V4AW_CID_CAMERA_CLASS_BASE+14)
#define V4AW_CID_ZOOM_CONTINUOUS        (V4AW_CID_CAMERA_CLASS_BASE+15)

#define V4AW_CID_PRIVACY            (V4AW_CID_CAMERA_CLASS_BASE+16)

#define V4AW_CID_IRIS_ABSOLUTE          (V4AW_CID_CAMERA_CLASS_BASE+17)
#define V4AW_CID_IRIS_RELATIVE          (V4AW_CID_CAMERA_CLASS_BASE+18)

#define V4AW_CID_AUTO_EXPOSURE_BIAS     (V4AW_CID_CAMERA_CLASS_BASE+19)

#define V4AW_CID_AUTO_N_PRESET_WHITE_BALANCE    (V4AW_CID_CAMERA_CLASS_BASE+20)


#define V4AW_CID_PAN_SPEED          (V4AW_CID_CAMERA_CLASS_BASE+32)
#define V4AW_CID_TILT_SPEED         (V4AW_CID_CAMERA_CLASS_BASE+33)

#endif

