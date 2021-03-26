/*
 * adv7280.h
 *
 *  Created on: 2020年6月9日
 *      Author: weike
 */

#ifndef USER_CODE_ADV7280_H_
#define USER_CODE_ADV7280_H_

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

#define AWBL_ADV7280_NAME              "adv7280"

/******************************** USER MAP ***********************************/

/* USER SUB MAP1 */
#define INPUT_CONTROL                   0x00
#define VIDEO_SELECTION1                0x01
#define VIDEO_SELECTION2                0x02
#define OUTPUT_CONTROL                  0x03
#define EXTENDED_OUTPUT_CONTROL         0x04
#define AUTODETECT_ENABLE               0x07
#define CONTRAST                        0x08
#define BRIGHTNESS_ADJUST               0x0A
#define HUE_ADJUST                      0x0B
#define DEFAULT_VALUE_Y                 0x0C
#define DEFAULT_VALUE_C                 0x0D
#define ADI_CONTROL                     0x0E
#define POWER_MANAGEMENT                0x0F
#define STATUS1                         0x10
#define IDENT                           0x11
#define STATUS2                         0x12
#define STATUS3                         0x13
#define ANALOG_CLAMP_CONTROL            0x14
#define DIGITAL_CLAMP_CONTROL1          0x15
#define SHAPING_FILTER_CONTROL1         0x17
#define SHAPING_FILTER_CONTROL2         0x18
#define COMB_FILTER_CONTROL             0x19
#define ADI_CONTROL2                    0x1D
#define PIXEL_DELAY_CONTROL             0x27
#define MISC_GAIN_CONTROL               0x2B
#define AGC_MODE_CONTROL                0x2C
#define CHROMA_GAIN_CONTROL1            0x2D
#define CHROMA_GAIN1                    0x2D
#define CHROMA_GAIN_CONTROL2            0x2E
#define CHROMA_GAIN2                    0x2E
#define LUMA_GAIN_CONTROL1              0x2F
#define LUMA_GAIN1                      0x2F
#define LUMA_GAIN_CONTROL2              0x30
#define LUMA_GAIN2                      0x30
#define VS_FIELD_CONTROL1               0x31
#define VS_FIELD_CONTROL2               0x32
#define VS_FIELD_CONTROL3               0x33
#define HS_POSITION_CONTROL1            0x34
#define HS_POSITION_CONTROL2            0x35
#define HS_POSITION_CONTROL3            0x36
#define POLARITY                        0x37  //
#define NTSC_COMB_CONTROL               0x38
#define PAL_COMB_CONTROL                0x39
#define ADC_CONTROL                     0x3A
#define MANUAL_WINDOW_CONTROL           0x3D
#define RESAMPLE_CONTROL1               0x41
#define CTI_DNR_CONTROL1                0x4D
#define CTI_DNR_CONTROL2                0x4E
#define DNR_NOISE_THRESHOLD1            0x50
#define LOCK_COUNT                      0x51
#define ADC_SWITCH3                     0x60
#define OUTPUT_SYNC_SELECT1             0x6A
#define OUTPUT_SYNC_SELECT2             0x6B
#define FREE_RUN_LINE_LENGTH1           0x8f
#define CCAP1                           0x99
#define CCAP2                           0x9A
#define LETTERBOX1                      0x9B
#define LETTERBOX2                      0x9C
#define LETTERBOX3                      0x9D
#define CRC_ENABLE                      0xB2
#define ADC_SWITCH1                     0xC3
#define ADC_SWITCH2                     0xC4
#define LETTERBOX_CONTROL1              0xDC
#define LETTERBOX_CONTROL2              0xDd
#define ST_NOISE_READBACK1              0xDE
#define ST_NOISE_READBACK2              0xDF
#define SD_OFFSET_CB_CHANNEL            0xE1
#define SD_OFFSET_CR_CHANNEL            0xE2
#define SD_STATURATION_CB_CHANNEL       0xE3
#define SD_STATURATION_CR_CHANNEL       0xE4
#define NTSC_V_BIT_BEGIN                0xE5
#define NTSC_V_BIT_END                  0xE6
#define NTSC_F_BIT_TOGGLE               0xE7
#define PAL_V_BIT_BEGIN                 0xE8
#define PAL_V_BIT_END                   0xE9
#define PAL_F_BIT_TOGGLE                0xEA
#define VBLANK_CONTROL1                 0xEB
#define VBLANK_CONTROL2                 0xEC
#define AFE_CONTROL1                    0xF3
#define DRIVE_STRENGTH                  0xF4
#define IF_COMP_CONTROL                 0xF8
#define VS_MODE_CONTROL                 0xF9
#define PEAKING_GAIN                    0xFB
#define DNR_NOISE_THRESHOLD2            0xFC
#define VPP_SLAVE_ADDRESS               0xFD
#define CSI_TX_SLAVE_ADDRESS            0xFE

/* USER SUB MAP2 */
#define ACE_CONTROL1                    0x80
#define ACE_CONTROL4                    0x83
#define ACE_CONTROL5                    0x84
#define ACE_CONTROL6                    0x85
#define DITHER_CONTROL                  0x92
#define MIN_MAX0                        0xD9
#define MIN_MAX1                        0xDA
#define MIN_MAX2                        0xDB
#define MIN_MAX3                        0xDC
#define MIN_MAX4                        0xDD
#define MIN_MAX5                        0xDE
#define FL_CONTROL                      0xE0
#define Y_ACERAGE0                      0xE1
#define Y_ACERAGE1                      0xE2
#define Y_ACERAGE2                      0xE3
#define Y_ACERAGE3                      0xE4
#define Y_ACERAGE4                      0xE5
#define Y_ACERAGE5                      0xE6
#define Y_AVERAGE_DATA_MSB              0xE7
#define Y_AVERAGE_DATA_LSB              0xE8

/* INTERRUPT/VDP SUB MAP */
#define INTERRUPT_CONFIGURATION1        0x40
#define INTERRUPT_STATUS1               0x42
#define INTERRUPT_CLEAR1                0x43
#define INTERRUPT_MASK1                 0x44
#define RAW_STATUS2                     0x45
#define INTERRUPT_STATUS2               0x46
#define INTERRUPT_CLEAR2                0x47
#define INTERRUPT_MASK2                 0x48
#define RAW_STATUS3                     0x49
#define INTERRUPT_STATUS3               0x4A
#define INTERRUPT_CLEAR3                0x4B
#define INTERRUPT_MASK3                 0x4C
#define INTERRUPT_STATUS4               0x4E
#define INTERRUPT_CLEAR4                0x4F
#define INTERRUPT_MASK4                 0x50
#define INTERRUPT_LATCH0                0x51
#define VDP_CONFIG_1                    0x60
#define VDP_ADF_CONFIG_1                0x62
#define VDP_ADF_CONFIG_2                0x63
#define VDP_LINE_00E                    0x64
#define VDP_LINE_00F                    0x65
#define VDP_LINE_010                    0x66
#define VDP_LINE_011                    0x67
#define VDP_LINE_012                    0x68
#define VDP_LINE_013                    0x69
#define VDP_LINE_014                    0x6A
#define VDP_LINE_015                    0x6B
#define VDP_LINE_016                    0x6C
#define VDP_LINE_017                    0x6D
#define VDP_LINE_018                    0x6E
#define VDP_LINE_019                    0x6F
#define VDP_LINE_01A                    0x70
#define VDP_LINE_01B                    0x71
#define VDP_LINE_01C                    0x72
#define VDP_LINE_01D                    0x73
#define VDP_LINE_01E                    0x74
#define VDP_LINE_01F                    0x75
#define VDP_LINE_020                    0x76
#define VDP_LINE_021                    0x77
#define VDP_STATUS                      0x78
#define VDP_STATUS_CLEAR                0x78
#define VDP_CCAP_DATA_0                 0x79
#define VDP_CCAP_DATA_1                 0x7A
#define VDP_CGMS_WSS_DATA_0             0x7D
#define VDP_CGMS_WSS_DATA_1             0x7E
#define VDP_CGMS_WSS_DATA_2             0x7F
#define VDP_OUTPUT_SEL                  0x9C

/********************************* VPP MAP ***********************************/

#define DEINT_RESET                     0x41
#define I2C_DEINT_ENABLE                0x55
#define ADV_TIMING_MODE_EN              0x5B

/********************************* CSI MAP ***********************************/

#define CSITX_PWRDN                     0x00
#define TLPX                            0x01
#define THSPREP                         0x02
#define THSZEROS                        0x03
#define THSTRAIL                        0x04
#define THSEXIT                         0x05
#define TCLK_PRE                        0x06
#define TCLK_ZEROS                      0x07
#define TCLK_TRAIL                      0x08
#define ANCILLARY_DI                    0x09
#define VBIVIDEO_DI                     0x0A
#define LSPKT_DI                        0x0B
#define LEPKT_DI                        0x0C
#define VC_REF                          0x0D
#define CKSUM_EN                        0x0E
#define CSI_FRAME_NUM_CTL               0x1F
#define CSI_LINENUM_INCR_INTERLACED     0x20
#define LINENUMBER1_F1_INTERLACED       0x21
#define LINENUMBER1_F2_INTERLACED       0x22
#define ESC_MODE_CTL                    0x26
#define DPHY_PWDN_CTL                   0xDE


/********************************* DEV INFO **********************************/


#define ADV7280_IIC_USER_ADDR0       0x20
#define ADV7280_IIC_USER_ADDR1       0x21
#define ADV7280_IIC_VPP_ADDR         0x42
#define ADV7280_IIC_CSI_ADDR         0x44

typedef struct {
    uint16_t reg;
    uint8_t  val;
} gc0308_reg_t ;

#define CCIR_PROGRESSIVE_MODE  0
#define CCIR_INTERLACE_MODE    1

typedef struct awbl_adv7280_devinfo {

    uint8_t          camera_data_bus_id;    /**< \brief 配置数据总线的ID,如使用CSI1还是CSI2 */

    uint8_t          i2c_master_busid;      /**< \brief i2c设备号 */

    uint8_t          i2c_user_addr;         /**< \brief i2c USER MAP的设备地址 */

    uint8_t          ain_channel;          /**< \brief 默认输入通道 */

    uint32_t         video_resolution;     /**< \brief 初始视频分辨率 */

    uint32_t         photo_resolution;     /**< \brief 初始视频分辨率 */

    uint32_t         control_flags;        /**< \brief 控制标志位   */

    /**< \brief  CCIR模式下的video模式, 此模式需要与CSI的CCIR video模式配置一致 */
    uint8_t          ccir_mode;

    /**< \brief  快速切换通道模式使能，此功能需要在interlace模式下使能  */
    uint8_t          fast_enbale;

    uint8_t          buf_num;              /**< \brief video缓冲区个数设置  */

    char             *p_name;              /**< \brief 摄像头名  */

    void (*pfunc_plfm_init)(void);         /**< \brief 平台相关初始化：开启时钟、初始化引脚等操作 */

} awbl_adv7280_devinfo_t ;

typedef struct awbl_adv7280_dev {

    struct awbl_dev              super;             /**< \brief 继承自 AWBus 设备 */

    struct awbl_camera_service   camera_serv;       /**< \brief 摄像头服务  */

    aw_i2c_device_t              i2c_device;        /**< \brief i2c设备 */

    /** \brief 保存配置信息的结构体  */
    awbl_camera_controller_cfg_info_t cfg;

    /** \brief 保存服务的头指针  */
    struct awbl_camera_controller_service *p_controller_serv;

    uint32_t       status;                   /**< \brief 状态标志位  */

    uint32_t      photo_resolution;         /**< \brief 拍照模式的分辨率  */

    uint32_t      video_resolution;         /**< \brief 视频模式的分辨率  */

    AW_MUTEX_DECL(dev_lock);                /**< \brief 设备锁 */

} awbl_adv7280_dev_t ;

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* USER_CODE_ADV7280_H_ */
