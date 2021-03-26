/*
 * awbl_gc0308.h
 *
 *  Created on: 2019年8月8日
 *      Author: weike
 */

#ifndef USER_CODE_AWBL_GC0308_H_
#define USER_CODE_AWBL_GC0308_H_

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN
#endif

#include "awbus_lite.h"
#include "aw_sem.h"
#include "aw_i2c.h"
#include "awbl_camera.h"

#define AWBL_GC0308_NAME              "gc0308"

//Analog & CISCTL
#define CHIP_ID                         0x00
#define HB                              0x01
#define VB                              0x02
#define EXPOSURE_H                      0x03
#define EXPOSURE_L                      0x04
#define ROWSTART_H                      0x05
#define ROWSTART_L                      0x06
#define COLUMNSTART_H                   0x07
#define COLUMNSTART_L                   0x08
#define WINDOWHEIGH_H                   0x09
#define WINDOWHEIGH_L                   0x0A
#define WINDOWWIDTH_H                   0x0B
#define WINDOWWIDTH_L                   0x0C
#define VS_ST                           0x0D
#define VS_ET                           0x0E
#define VB_HB                           0x0F
#define RSH_WIDTH                       0x10
#define TSP_WIDTH                       0x11
#define SH_DELAY                        0x12
#define ROW_TAIL_WIDTH                  0x13
#define CISCTL_MODE1                    0x14
#define CISCTL_MODE2                    0x15
#define CISCTL_MODE3                    0x16
#define CISCTL_MODE4                    0x17

#define ANALOG_MODE1                    0x1A
#define ANALOG_MODE2                    0x1B
#define HRST_RSG_V18                    0x1C
#define DA_RSG                          0x1C
#define TXHIGH_EN                       0x1C
#define DA18_R                          0x1C
#define VREF_V25                        0x1D
#define ADC_R                           0x1E
#define PAD_DRV                         0x1F
#define REST_RELATED                    0xFE

//ISP Related
#define BLOCK_ENABLE_1                  0x20
#define BLOCK_ENABLE_2                  0x21
#define AAAA_ENABLE                     0x22
#define SPECIAL_EFFECT                  0x23
#define OUTPUT_FORMAT                   0x24
#define OUTPUT_EN                       0x25
#define SYNC_MODE                       0x26

#define CLK_DIV_MODE                    0x28
#define BYPASS_MODE                     0x29
#define CLOCK_GATING_EN                 0x2A
#define DITHER_MODE                     0x2B
#define DITHER_BIT                      0x2C
#define DEBUG_MODE1                     0x2D
#define DEBUG_MODE2                     0x2E
#define DEBUG_MODE3                     0x2F
#define CROP_WIN_MODE                   0x46
#define CROP_WIN_Y1_H                   0x46
#define CROP_WIN_X1_H                   0x46
#define CROP_WIN_Y1_L                   0x47
#define CROP_WIN_X1_L                   0x48
#define CROP_WIN_HEIGHT_H               0x49
#define CROP_WIN_HEIGHT_L               0x4A
#define CROP_WIN_WIDTH_H                0x4B
#define CROP_WIN_WIDTH_L                0x4C

//BLK
#define BLK_MODE                        0x30
#define BLK_LIMIT_VALUE                 0x31
#define GLOBAL_OFFSET                   0x32
#define CURRENT_R_OFFSET                0x33
#define CURRENT_G_OFFSET                0x34
#define CURRENT_B_OFFSET                0x35
#define CURRENT_R_DARK_CURRENT          0x36
#define CURRENT_G_DARK_CURRENT          0x37
#define CURRENT_B_DARK_CURRENT          0x38
#define EXP_RATE_DARKC                  0x39

#define OFFSET_SUBMODE                  0x3A
#define OFFSET_RATIO                    0x3A
#define DAEKC_SUBMODE                   0x3B
#define DAEK_CURRENT_RATIO              0x3B

#define MANUAL_G1_OFFSET                0x3C
#define MANUAL_R1_OFFSET                0x3D
#define MANUAL_B2_OFFSET                0x3E
#define MANUAL_G2_OFFSET                0x3F

//PREGAIN
#define GLOBAL_GAIN                     0x50
#define AUTO_PREGAIN                    0x51
#define AUTO_POSTGAIN                   0x52
#define CHANNEL_GAIN_G1                 0x53
#define CHANNEL_GAIN_R                  0x54
#define CHANNEL_GAIN_B                  0x55
#define CHANNEL_GAIN_G2                 0x56
#define R_RATIO                         0x57
#define G_RATIO                         0x58
#define B_RATIO                         0x59
#define AWB_R_GAIN                      0x5A
#define AWB_G_GAIN                      0x5B
#define AWB_B_GAIN                      0x5C
#define LSC_DECREASE_LEVEL1             0x5D
#define LSC_DECREASE_LEVEL2             0x5E
#define LSC_DECREASE_LEVEL3             0x5F

//LSC
#define LSC_RED_B2                      0x8B
#define LSC_GREEN_B2                    0x8C
#define LSC_BLUE_B2                     0x8D
#define LSC_RED_B4                      0x8E
#define LSC_GREEN_B4                    0x8F
#define LSC_BLUE_B4                     0x90
#define SIGNED_B4                       0x91
#define LSC_ROW_CENTER                  0x91
#define LSC_COLUMN_CENTER               0x92

//ASDE
#define ASDE_GAIN_HIGH_THRESHOLD        0x69
#define ASDE_DN_C_SLOPE                 0x6A
#define ASDE_GAIN_MODE                  0x6A
#define ASDE_DN_B_SLOPE                 0x6B
#define ASDE_DN_N_SLOPE                 0x6B
#define ASDE_DD_BRIGHT_TH_START         0x6C
#define ASDE_DD_BRIGTH_TH_SLOPE         0x6C
#define ASDE_DD_LIMIT_START             0x6D
#define ASDE_DD_LIMIT_SLOPE             0x6D
#define ASDE_AUTO_EE1_EFFECT_START      0x6E
#define ASDE_AUTO_EE1_EFFECT_SLOPE      0x6E
#define ASDE_AUTO_EE2_EFFECT_START      0x6F
#define ASDE_AUTO_EE2_EFFECT_SLOPE      0x6F
#define ASDE_AUTO_SATURATION_DEC_SLOPE  0x70
#define ASDN_AUTO_SATURATION_LOW_LIMIT  0x71
#define ASDE_SUB_SATURATION_SLOPE       0x71

//DNDD
#define DN_MODE_EN                      0x60
#define DN_MODE_RATIO                   0x61
#define DN_BILAT_B_BASE                 0x62
#define DN_B_INCR                       0x63
#define DN_BILAT_N_BASE                 0x64
#define DN_C_WEIGHT                     0x64
#define DN_N_INCR                       0x65
#define DN_C_COEFF                      0x65
#define DD_DARK_BRIGHT_TH               0x66
#define DD_FLAT_TH                      0x67
#define DD_LIMIT                        0x68
#define DD_RATIO                        0x68

//INTPEE
#define EEINTP_MODE1                    0x72
#define EEINTP_MODE2                    0x73
#define DIRECTION_TH1                   0x74
#define DIRECTION_TH2                   0x75
#define DIFF_HV_TI_TH                   0x76
#define DIRECTION_DIFF_TH               0x76
#define EDGE1_EFFECT                    0x77
#define EDGE2_EFFECT                    0x77
#define EDGE_POS_RATIO                  0x78
#define EDGE_NEG_RATIO                  0x78
#define EDGE1_MAX                       0x79
#define EDGE1_MIN                       0x79
#define EDGE2_MAX                       0x7A
#define EDGE2_MIN                       0x7A
#define EDGE1_TH                        0x7B
#define EDGE2_TH                        0x7B
#define EDGE_POS_MAX                    0x7C
#define EDGE_NEG_MAX                    0x7C

//ABB
#define ABB_MODE                        0x80
#define ABB_TARGET_AVERAGE1             0x81
#define ABB_TARGET_AVERAGE2             0x82
#define ABB_LIMIT_VALUE                 0x83
#define ABB_SPEED                       0x84
#define CURRENT_R_BLACK_LEVEL           0x85
#define CURRENT_G_BLACK_LEVEL           0x86
#define CURRENT_B_BLACK_LEVEL           0x87
#define CURRENT_R_BLACK_FACTOR          0x88
#define CURRENT_G_BLACK_FACTOR          0x89
#define CURRENT_B_BLACK_FACTOR          0x8A

//CC
#define CC_MATRIX_C11                   0x93
#define CC_MATRIX_C12                   0x94
#define CC_MATRIX_C13                   0x95
#define CC_MATRIX_C21                   0x96
#define CC_MATRIX_C22                   0x97
#define CC_MATRIX_C23                   0x98
//#define CC_MATRIX_C31                   0x99
//#define CC_MATRIX_C32                   0x9A
//#define CC_MATRIX_C33                   0x9B
#define CC_MATRIX_C41                   0x9C
#define CC_MATRIX_C42                   0x9D
#define CC_MATRIX_C43                   0x9E

//GAMMA
#define GAMMA_OUT0                      0x9F
#define GAMMA_OUT1                      0xA0
#define GAMMA_OUT2                      0xA1
#define GAMMA_OUT3                      0xA2
#define GAMMA_OUT4                      0xA3
#define GAMMA_OUT5                      0xA4
#define GAMMA_OUT6                      0xA5
#define GAMMA_OUT7                      0xA6
#define GAMMA_OUT8                      0xA7
#define GAMMA_OUT9                      0xA8
#define GAMMA_OUT10                     0xA9
#define GAMMA_OUT11                     0xAA
#define GAMMA_OUT12                     0xAB
#define GAMMA_OUT13                     0xAC
#define GAMMA_OUT14                     0xAD
#define GAMMA_OUT15                     0xAE
#define GAMMA_OUT16                     0xAF

//YCP
#define GLOBAL_STATURATION              0xB0
#define SATURATION_CB                   0xB1
#define SATURATION_CR                   0xB2
#define LUMA_CONTRAST                   0xB3
#define CONTRAST_CENTER                 0xB4
#define LUMA_OFFSET                     0xB5
#define SKIN_CB_CENTER                  0xB6
#define SKIN_CR_SENTER                  0xB4
#define SKIN_RADIUS_SQUARE              0xB8
#define SKIN_BRIGHTNESS_HIGH            0xB9
#define SKIN_BRIGHTNESS_LOW             0xB9
#define FIXED_CB                        0xBA
#define FIXED_CR                        0xBB

#define EDGE_DEC_SA_EN                  0xBD
#define EDGE_DEC_SA_SLOPE               0xBD
#define AUTO_GRAY_MODE                  0xBE
#define SA_AUTOGRAY                     0xBE
#define SATURATION_SUB_STRENGTH         0xBF
#define Y_GAMMA_OUT0                    0xC0
#define Y_GAMMA_OUT1                    0xC1
#define Y_GAMMA_OUT2                    0xC2
#define Y_GAMMA_OUT3                    0xC3
#define Y_GAMMA_OUT4                    0xC4
#define Y_GAMMA_OUT5                    0xC5
#define Y_GAMMA_OUT6                    0xC6
#define Y_GAMMA_OUT7                    0xC7
#define Y_GAMMA_OUT8                    0xC8
#define Y_GAMMA_OUT9                    0xC9
#define Y_GAMMA_OUT10                   0xCA
#define Y_GAMMA_OUT11                   0xCB
#define Y_GAMMA_OUT12                   0xCC

//AEC
#define AEC_MODE1                       0xD0
#define AEC_MODE2                       0xD1
#define AEC_MODE3                       0xD2
#define AEC_TARGET_Y                    0xD3
#define Y_AVERAGE                       0xD4
#define AEC_HIGH_LOW_RANGE              0xD5
#define AEC_IGNORE                      0xD6
#define AEC_NUMBER_LIMIT_HIGH_RANGE     0xD7

#define AEC_SKIN_OFFSET                 0xD9
#define AEC_R_OFFSET                    0xD9
#define AEC_G_OFFSET                    0xDA
#define AEC_B_OFFSET                    0xDA
#define AEC_SLOW_MARGIN                 0xDB
#define AEC_SLOW_SPEED                  0xDB
#define AEC_FAST_MARGIN                 0xDC
#define AEC_FAST_SPEED                  0xDC
#define AEC_EXP_CHANGE_GAIN_RATIO       0xDD
#define AEC_STEP2_SUNLIGHT              0xDE
#define AEC_I_FRAMES                    0xDF
#define AEC_D_RATIO                     OXDF
#define AEC_I_STOP_L_MARGIN             0xE0
#define AEC_I_STOP_MARGIN               0xE1
#define AEC_I_RATIO                     0xE1
#define ANTI_FLICKER_STEP_H             0xE2
#define ANTI_FLICKER_STEP_L             0xE3
#define EXP_LEVEL_1_H                   0xE4
#define EXP_LEVEL_1_L                   0xE5
#define EXP_LEVEL_2_H                   0xE6
#define EXP_LEVEL_2_L                   0xE7
#define EXP_LEVEL_3_H                   0xE8
#define EXP_LEVEL_3_L                   0xE9
#define EXP_LEVEL_4_H                   0xEA
#define EXP_LEVEL_4_L                   0xEB
#define MAX_EXP_LEVEL                   0xEC
#define EXP_MIN_I_H                     0xEC
#define EXP_MIN_I_L                     0xED
#define MAX_POST_DG_GAIN                0xEE
#define MAX_PRE_DG_GAIN                 0xEF

//ABS
#define ABS_RANGE_COMPESATE             0xF0
#define ABS_SKIP_FRAME                  0xF0
#define ABS_STOP_MARGIN                 OXF1
#define Y_S_COMPESATE                   0xF2
#define ABS_MANUAL_K                    0xF2
#define Y_STRETCH_LIMIT                 0xF3
#define Y_TILT                          0xF4
#define Y_STRETCH_K                     0xF5

//Measure Window
#define BIG_WIN_X0                      0xF7
#define BIG_WIN_Y0                      0xF8
#define BIG_WIN_X1                      0xF9
#define BIG_WIN_Y1                      0xFA
#define DIFF_Y_BIG_THD                  0xFB


/*********************************P1******************************/
//OUT Module
#define CLOSE_FRAME_EN                  0x50
#define CLOSE_FRAME_NUM                 0x50
#define CLOSE_FRAME_NUM1                0x51
#define CLOSE_FRAME_NUM2                0x52
#define BAYER_MODE                      0x53
#define SUBSAMPLE                       0x54
#define SUB_MODE                        0x55
#define SUB_ROW_N1                      0x56
#define SUB_ROW_N2                      0x57
#define SUB_COL_N1                      0x58
#define SUB_COL_N2                      0x59

//AWB
#define AWB_RGB_HIGH_LOW                0x00
#define AWB_Y_TO_C_DIFF2                0x02

#define AWB_C_MAX                       0x04
#define AWB_C_INTER                     0x05
#define AWB_C_INTER2                    0x06

#define AWB_C_MAX_BIG                   0x08
#define AWB_Y_HIGH                      0x09
#define AWB_NUMBER_LIMIT                0x0A
#define KWIN_RATIO                      0x0B
#define SEL_POINT                       0x0B
#define SKIP_MODE                       0x0B
#define KWIN_THD                        0x0C
#define LIGHT_GAIN_RANGE                0x0D
#define SMALL_WIN_WIDTH_STEP            0x0E
#define SMALL_WIN_HEIGHT_STEP           0x0F
#define AWB_YELLOW_TH                   0x10
#define AWB_BIG_C_LIMIT                 0x10
#define AWB_MODE                        0x11
#define AWB_ADJUST_SPEED                0x12
#define AWB_ADJUST_MARGIN               0x12
#define AWB_ENERY_N                     0x13
#define AWB_LIGHT                       0x13
#define CT_MODE                         0x13
//#define AWB_SET1                        0x14~0x1E

#define AWB_B_5K_GAIN_1                 0x14
#define AWB_B_5K_GAIN_2                 0x15
#define AWB_SINT                        0x16
#define AWB_COST                        0x17
#define AWB_X1_CUT                      0x18
#define AWB_X2_CUT                      0x19
#define AWB_Y1_CUT                      0x1A
#define AWB_Y2_CUT                      0x1B
#define AWB_R_GAIN_LIMIT                0x1C
#define AWB_G_GAIN_LIMIT                0x1D
#define AWB_B_GAIN_LIMIT                0x1E

//#define AWB_SET2                        0x70~0x79
#define R_AVG_USE                       0xD0
#define G_AVG_USE                       0xD1
#define B_AVG_USE                       0xD2

#define AW_CAM_VIDEO_MODE       0x01
#define AW_CAM_PHOTO_MODE       0x02

typedef struct {
    uint16_t reg;
    uint8_t  val;
} gc0308_reg_t ;

typedef struct awbl_gc0308_devinfo {

    uint8_t          camera_data_bus_id;    /**< \brief 配置数据总线的ID,如使用CSI1还是CSI2 */

    uint8_t          i2c_master_busid;      /**< \brief i2设备号 */

    uint32_t         video_resolution;     /**< \brief 初始分辨率 */

    uint32_t         photo_resolution;     /**< \brief 初始分辨率 */

    pix_format_t     pix_format;           /**< \brief 像素输出格式  */

    uint32_t         input_clock;          /**< \brief 摄像头的输入时钟 */

    uint32_t         control_flags;        /**< \brief 控制标志位   */

    uint8_t          buf_num;              /**< \brief 缓冲区个数设置  */

    char             *p_name;              /**< \brief 摄像头名  */

    void (*pfunc_plfm_init)(void);          /**< \brief 平台相关初始化：开启时钟、初始化引脚等操作 */

} awbl_gc0308_devinfo_t ;

typedef struct awbl_gc0308_dev {

    struct awbl_dev              super;             /**< \brief 继承自 AWBus 设备 */

    struct awbl_camera_service   camera_serv;       /**< \brief 摄像头服务  */

    aw_i2c_device_t              i2c_device;        /**< \brief i2c设备 */

    /** \brief 保存配置信息的结构体  */
    awbl_camera_controller_cfg_info_t cfg;

    /** \brief 保存服务的头指针  */
    struct awbl_camera_controller_service *p_controller_serv;

    uint32_t      status;                   /**< \brief 状态标志位  */

    uint8_t       pix_byte;                 /**< \brief 每像素占用字节 */

    uint32_t      photo_resolution;         /**< \brief 拍照模式的分辨率  */

    uint32_t      video_resolution;         /**< \brief 视频模式的分辨率  */

    AW_MUTEX_DECL(dev_lock);                /**< \brief 设备锁 */

} awbl_gc0308_dev_t ;


#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* USER_CODE_AWBL_GC0308_H_ */
