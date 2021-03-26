#ifndef __AWBL_OV7725_H
#define __AWBL_OV7725_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbus_lite.h"
#include "aw_sem.h"
#include "aw_i2c.h"
#include "awbl_camera.h"

#define AWBL_OV7725_NAME   "ov7725"


/** \brief Register definitions for the OV7725.*/

#define OV7725_GAIN_REG 0x00U       /**< \brief Gain control gain setting */
#define OV7725_BLUE_REG 0x01U       /**< \brief Blue channel gain setting */
#define OV7725_RED_REG 0x02U        /**< \brief Red channel gain setting. */
#define OV7725_GREEN_REG 0x03U      /**< \brief Green channel gain setting */
#define OV7725_BAVG_REG 0x05U       /**< \brief B Average Level */
#define OV7725_GAVG_REG 0x06U       /**< \brief G Average Level */
#define OV7725_RAVG_REG 0x07U       /**< \brief R Average Level */
#define OV7725_AECH_REG 0x08U       /**< \brief Exposure Value - AEC MSBs */
#define OV7725_COM2_REG 0x09U       /**< \brief Common Control 2 */
#define OV7725_PID_REG 0x0AU        /**< \brief Product ID Number MSB */
#define OV7725_VER_REG 0x0BU        /**< \brief Product ID Number LSB */
#define OV7725_COM3_REG 0x0CU       /**< \brief Common Control 3 */
#define OV7725_COM4_REG 0x0DU       /**< \brief Common Control 4 */
#define OV7725_COM5_REG 0x0EU       /**< \brief Common Control 5 */
#define OV7725_COM6_REG 0x0FU       /**< \brief Common Control 6 */
#define OV7725_AEC_REG 0x10U        /**< \brief Exposure Value */
#define OV7725_CLKRC_REG 0x11U      /**< \brief Internal Clock */
#define OV7725_COM7_REG 0x12U       /**< \brief Common Control 7 */
#define OV7725_COM8_REG 0x13U       /**< \brief Common Control 8 */
#define OV7725_COM9_REG 0x14U       /**< \brief Common Control 9 */
#define OV7725_COM10_REG 0x15U      /**< \brief Common Control 10 */
#define OV7725_REG16_REG 0x16U      /**< \brief Register 16 */
#define OV7725_HSTART_REG 0x17U     /**< \brief Horizontal Frame (HREF column) Start 8 MSBs */
#define OV7725_HSIZE_REG 0x18U      /**< \brief Horizontal Sensor Size */
#define OV7725_VSTART_REG 0x19U     /**< \brief Vertical Frame (row) Start 8 MSBs */
#define OV7725_VSIZE_REG 0x1AU      /**< \brief Vertical Sensor Size */
#define OV7725_PSHFT_REG 0x1BU      /**< \brief Data format */
#define OV7725_MIDH_REG 0x1CU       /**< \brief Manufacturer ID Byte - High */
#define OV7725_MIDL_REG 0x1DU       /**< \brief Manufacturer ID Byte - Low  */
#define OV7725_LAEC_REG 0x1FU       /**< \brief Fine AEC Value */
#define OV7725_COM11_REG 0x20U      /**< \brief Common Control 11 */
#define OV7725_BDBASE_REG 0x22U     /**< \brief Banding Filter Minimum AEC Value */
#define OV7725_BDMSTEP_REG 0x23U    /**< \brief Banding Filter Maximum Step */
#define OV7725_AEW_REG 0x24U        /**< \brief AGC/AEC Stable Operating Region (Upper Limit) */
#define OV7725_AEB_REG 0x25U        /**< \brief AGC/AEC Stable Operating Region (Lower Limit) */
#define OV7725_VPT_REG 0x26U        /**< \brief AGC/AEC Fast Mode Operating Region */
#define OV7725_REG28_REG 0x28U      /**< \brief Register 28 */
#define OV7725_HOUTSIZE_REG 0x29U   /**< \brief Horizontal Data Output Size 8 MSBs */
#define OV7725_EXHCH_REG 0x2AU      /**< \brief Dummy Pixel Insert MSB */
#define OV7725_EXHCL_REG 0x2BU      /**< \brief Dummy Pixel Insert LSB */
#define OV7725_VOUTSIZE_REG 0x2CU   /**< \brief Vertical Data Output Size MSBs */
#define OV7725_ADVFL_REG 0x2DU      /**< \brief LSB of Insert Dummy Rows in Vertical Sync (1 bit equals 1 row) */
#define OV7725_ADVFH_REG 0x2EU      /**< \brief MSB of Insert Dummy Rows in Vertical Sync */
#define OV7725_YAVE_REG 0x2FU       /**< \brief Y/G Channel Average Value */
#define OV7725_LUMHTH_REG 0x30U     /**< \brief Histogram AEC/AGC Luminance High Level Threshold */
#define OV7725_LUMLTH_REG 0x31U     /**< \brief Histogram AEC/AGC Luminance Low Level Threshold */
#define OV7725_HREF_REG 0x32U       /**< \brief Image Start and Size Control */
#define OV7725_DM_LNL_REG 0x33U     /**< \brief Low 8 Bits of the Number of Dummy Rows */
#define OV7725_DM_LNH_REG 0x34U     /**< \brief High 8 Bits of the Number of Dummy Rows */
#define OV7725_ADOFF_B_REG 0x35U    /**< \brief AD Offset Compensation Value for B Channel */
#define OV7725_ADOFF_R_REG 0x36U    /**< \brief AD Offset Compensation Value for R Channel */
#define OV7725_ADOFF_GB_REG 0x37U   /**< \brief AD Offset Compensation Value for Gb Channel */
#define OV7725_ADOFF_GR_REG 0x38U   /**< \brief AD Offset Compensation Value for Gr Channel */
#define OV7725_OFF_B_REG 0x39U      /**< \brief B Channel Offset Compensation Value */
#define OV7725_OFF_R_REG 0x3AU      /**< \brief R Channel Offset Compensation Value */
#define OV7725_OFF_GB_REG 0x3BU     /**< \brief Gb Channel Offset Compensation Value */
#define OV7725_OFF_GR_REG 0x3CU     /**< \brief Gr Channel Offset Compensation Value */
#define OV7725_COM12_REG 0x3DU      /**< \brief Common Control 12 */
#define OV7725_COM13_REG 0x3EU      /**< \brief Common Control 13 */
#define OV7725_COM14_REG 0x3FU      /**< \brief Common Control 14 */
#define OV7725_COM16_REG 0x41U      /**< \brief Common Control 16 */
#define OV7725_TGT_B_REG 0x42U      /**< \brief BLC Blue Channel Target Value */
#define OV7725_TGT_R_REG 0x43U      /**< \brief BLC Red Channel Target Value */
#define OV7725_TGT_GB_REG 0x44U     /**< \brief BLC Gb Channel Target Value */
#define OV7725_TGT_GR_REG 0x45U     /**< \brief BLC Gr Channel Target Value */
#define OV7725_LC_CTR_REG 0x46U     /**< \brief Lens Correction Control */
#define OV7725_LC_XC_REG 0x47U      /**< \brief X Coordinate of Lens Correction Center Relative to Array Center */
#define OV7725_LC_YC_REG 0x48U      /**< \brief Y Coordinate of Lens Correction Center Relative to Array Center */
#define OV7725_LC_COEF_REG 0x49U    /**< \brief Lens Correction Coefficient */
#define OV7725_LC_RADI_REG 0x4AU    /**< \brief Lens Correction Radius */
#define OV7725_LC_COEFB_REG 0x4BU   /**< \brief Lens Correction B Channel Compensation Coefficient */
#define OV7725_LC_COEFR_REG 0x4CU   /**< \brief Lens Correction R Channel Compensation Coefficient */
#define OV7725_FIXGAIN_REG 0x4DU    /**< \brief Analog Fix Gain Amplifier */
#define OV7725_AREF1_REG 0x4FU      /**< \brief Sensor Reference Current Control */
#define OV7725_AREF6_REG 0x54U      /**< \brief Analog Reference Control */
#define OV7725_UFIX_REG 0x60U       /**< \brief U Channel Fixed Value Output */
#define OV7725_VFIX_REG 0x61U       /**< \brief V Channel Fixed Value Output */
#define OV7725_AWBB_BLK_REG 0x62U   /**< \brief AWB Option for Advanced AWBA */
#define OV7725_AWB_CTRL0_REG 0x63U  /**< \brief AWB Control Byte 0 */
#define OV7725_DSP_CTRL1_REG 0x64U  /**< \brief DSP Control Byte 1 */
#define OV7725_DSP_CTRL2_REG 0x65U  /**< \brief DSP Control Byte 2 */
#define OV7725_DSP_CTRL3_REG 0x66U  /**< \brief DSP Control Byte 3 */
#define OV7725_DSP_CTRL4_REG 0x67U  /**< \brief DSP Control Byte 4 */
#define OV7725_AWB_BIAS_REG 0x68U   /**< \brief AWB BLC Level Clip */
#define OV7725_AWB_CTRL1_REG 0x69U  /**< \brief AWB Control 1 */
#define OV7725_AWB_CTRL2_REG 0x6AU  /**< \brief AWB Control 2 */
#define OV7725_AWB_CTRL3_REG 0x6BU  /**< \brief AWB Control 3 */
#define OV7725_AWB_CTRL4_REG 0x6CU  /**< \brief AWB Control 4 */
#define OV7725_AWB_CTRL5_REG 0x6DU  /**< \brief AWB Control 5 */
#define OV7725_AWB_CTRL6_REG 0x6EU  /**< \brief AWB Control 6 */
#define OV7725_AWB_CTRL7_REG 0x6FU  /**< \brief AWB Control 7 */
#define OV7725_AWB_CTRL8_REG 0x70U  /**< \brief AWB Control 8 */
#define OV7725_AWB_CTRL9_REG 0x71U  /**< \brief AWB Control 9 */
#define OV7725_AWB_CTRL10_REG 0x72U /**< \brief AWB Control 10 */
#define OV7725_AWB_CTRL11_REG 0x73U /**< \brief AWB Control 11 */
#define OV7725_AWB_CTRL12_REG 0x74U /**< \brief AWB Control 12 */
#define OV7725_AWB_CTRL13_REG 0x75U /**< \brief AWB Control 13 */
#define OV7725_AWB_CTRL14_REG 0x76U /**< \brief AWB Control 14 */
#define OV7725_AWB_CTRL15_REG 0x77U /**< \brief AWB Control 15 */
#define OV7725_AWB_CTRL16_REG 0x78U /**< \brief AWB Control 16 */
#define OV7725_AWB_CTRL17_REG 0x79U /**< \brief AWB Control 17 */
#define OV7725_AWB_CTRL18_REG 0x7AU /**< \brief AWB Control 18 */
#define OV7725_AWB_CTRL19_REG 0x7BU /**< \brief AWB R Gain Range */
#define OV7725_AWB_CTRL20_REG 0x7CU /**< \brief AWB G Gain Range */
#define OV7725_AWB_CTRL21_REG 0x7DU /**< \brief AWB B Gain Range */
#define OV7725_GAM1_REG 0x7EU       /**< \brief Gamma Curve 1st Segment Input End Point  0x04 Output Value */
#define OV7725_GAM2_REG 0x7FU       /**< \brief Gamma Curve 2nd Segment Input End Point  0x08 Output Value */
#define OV7725_GAM3_REG 0x80U       /**< \brief Gamma Curve 3rd Segment Input End Point  0x10 Output Value */
#define OV7725_GAM4_REG 0x81U       /**< \brief Gamma Curve 4th Segment Input End Point  0x20 Output Value */
#define OV7725_GAM5_REG 0x82U       /**< \brief Gamma Curve 5th Segment Input End Point  0x28 Output Value */
#define OV7725_GAM6_REG 0x83U       /**< \brief Gamma Curve 6th Segment Input End Point  0x30 Output Value */
#define OV7725_GAM7_REG 0x84U       /**< \brief Gamma Curve 7th Segment Input End Point  0x38 Output Value */
#define OV7725_GAM8_REG 0x85U       /**< \brief Gamma Curve 8th Segment Input End Point  0x40 Output Value */
#define OV7725_GAM9_REG 0x86U       /**< \brief Gamma Curve 9th Segment Input End Point  0x48 Output Value */
#define OV7725_GAM10_REG 0x87U      /**< \brief Gamma Curve 10th Segment Input End Point 0x50 Output Value */
#define OV7725_GAM11_REG 0x88U      /**< \brief Gamma Curve 11th Segment Input End Point 0x60 Output Value */
#define OV7725_GAM12_REG 0x89U      /**< \brief Gamma Curve 12th Segment Input End Point 0x70 Output Value */
#define OV7725_GAM13_REG 0x8AU      /**< \brief Gamma Curve 13th Segment Input End Point 0x90 Output Value */
#define OV7725_GAM14_REG 0x8BU      /**< \brief Gamma Curve 14th Segment Input End Point 0xB0 Output Value */
#define OV7725_GAM15_REG 0x8CU      /**< \brief Gamma Curve 15th Segment Input End Point 0xD0 Output Value */
#define OV7725_SLOP_REG 0x8DU       /**< \brief Gamma Curve Highest Segment Slope */
#define OV7725_DNSTH_REG 0x8EU      /**< \brief De-noise Threshold */
#define OV7725_EDGE0_REG 0x8FU      /**< \brief Sharpness (Edge Enhancement) Control 0 */
#define OV7725_EDGE1_REG 0x90U      /**< \brief Sharpness (Edge Enhancement) Control 1 */
#define OV7725_DNSOFF_REG 0x91U     /**< \brief Lower Limit of De-noise Threshold - effective in auto mode only */
#define OV7725_EDGE2_REG 0x92U      /**< \brief Sharpness (Edge Enhancement) Strength Upper Limit */
#define OV7725_EDGE3_REG 0x93U      /**< \brief Sharpness (Edge Enhancement) Strength Lower Limit */
#define OV7725_MTX1_REG 0x94U       /**< \brief Matrix Coefficient 1 */
#define OV7725_MTX2_REG 0x95U       /**< \brief Matrix Coefficient 2 */
#define OV7725_MTX3_REG 0x96U       /**< \brief Matrix Coefficient 3 */
#define OV7725_MTX4_REG 0x97U       /**< \brief Matrix Coefficient 4 */
#define OV7725_MTX5_REG 0x98U       /**< \brief Matrix Coefficient 5 */
#define OV7725_MTX6_REG 0x99U       /**< \brief Matrix Coefficient 6 */
#define OV7725_MTX_CTRL_REG 0x9AU   /**< \brief Matrix Control */
#define OV7725_BRIGHT_REG 0x9BU     /**< \brief Brightness */
#define OV7725_CNST_REG 0x9CU       /**< \brief Contrast */
#define OV7725_UVADJ0_REG 0x9EU     /**< \brief Auto UV Adjust Control 0 */
#define OV7725_UVADJ1_REG 0x9FU     /**< \brief Auto UV Adjust Control 1 */
#define OV7725_SCAL0_REG 0xA0U      /**< \brief DCW Ratio Control */
#define OV7725_SCAL1_REG 0xA1U      /**< \brief Horizontal Zoom Out Control */
#define OV7725_SCAL2_REG 0xA2U      /**< \brief Vertical Zoom Out Control */
#define OV7725_SDE_REG 0xA6U        /**< \brief Special Digital Effect (SDE) Control */
#define OV7725_USAT_REG 0xA7U       /**< \brief U Component Saturation Gain */
#define OV7725_VSAT_REG 0xA8U       /**< \brief V Component Saturation Gain */
#define OV7725_HUECOS_REG 0xA9U     /**< \brief Cosine value x 0x80 */
#define OV7725_HUESIN_REG 0xAAU     /**< \brief |Sine value| x 0x80 */
#define OV7725_SIGN_REG 0xABU       /**< \brief Sign Bit for Hue and Brightness */
#define OV7725_DSPAUTO_REG 0xACU    /**< \brief DSP Auto Function ON/OFF Control */

#define OV7725_COM10_VSYNC_NEG_MASK (1U << 1U)
#define OV7725_COM10_HREF_REVERSE_MASK (1U << 3U)
#define OV7725_COM10_PCLK_REVERSE_MASK (1U << 4U)
#define OV7725_COM10_PCLK_OUT_MASK (1U << 5U)
#define OV7725_COM10_DATA_NEG_MASK (1U << 7U)

/** \brief OV7725寄存器 */
typedef struct __ov7725_reg
{
    uint8_t reg;
    uint8_t val;
} ov7725_reg_t;


/** \brief OV7725支持的分辨率    */
typedef enum __resolution {
    VGA  = (640 | (480 << 16)),        /* 640x480 */
    QVGA = (320 | (240 << 16)),        /* 320x240 */
} resolution_t;


/** \brief OV7725拍照模工能的状态    */
typedef enum __photo_release {
    BUSY,
    IDLE
}photo_release_t;


/** \brief OV7725的设备信息    */
typedef struct awbl_ov7725_devinfo {

   uint8_t          camera_data_bus_id;   /**< \brief 配置数据总线的ID,如使用CSI1还是CSI2 */

   uint8_t          i2c_master_busid;      /**< \brief i2设备号 */
   
   resolution_t     video_resolution;           /**< \brief 默认分辨率  */

   resolution_t     photo_resolution;           /**< \brief 默认分辨率  */

   pix_format_t     pix_format;           /**< \brief 像素输出格式  */

   uint32_t         input_clock;          /**< \brief 摄像头的输入时钟 */

   uint32_t         frame_rate;           /**< \brief 帧率，目前只支持30、25、50 */

   uint32_t         control_flags;        /**< \brief 控制标志位   */

   uint8_t          buf_num;              /**< \brief 缓冲区个数设置  */

   char             *p_name;              /**< \brief 摄像头名  */

   void (*pfunc_plfm_init)(void);   /**< \brief 平台相关初始化：开启时钟、初始化引脚等操作 */

} awbl_ov7725_devinfo_t;


/**
 * \brief 0V7725 设备实例
 */
typedef struct awbl_ov7725_dev {

    struct awbl_dev         super;             /**< \brief 继承自 AWBus 设备 */

    struct awbl_camera_service   camera_serv;  /**< \brief 摄像头服务  */

    aw_i2c_device_t              i2c_device;    /**< \brief i2c设备 */

    /** \brief 保存配置信息的结构体  */
    awbl_camera_controller_cfg_info_t cfg;

    /** \brief 保存服务的头指针  */
    struct awbl_camera_controller_service *p_controller_serv;

    uint8_t      pix_byte;                 /**< \brief 每像素占用字节 */

    uint32_t     status;                   /**< \brief 状态标志位  */

    uint32_t     photo_resolution;        /**< \brief 拍照模式的分辨率  */

    uint32_t     video_resolution;        /**< \brief 视频模式的分辨率  */

    uint32_t     *buff_nodes;             /**< \brief 动态分配的结点首地址 */

    AW_MUTEX_DECL(dev_lock);              /**< \brief 设备锁 */

} awbl_ov7725_dev_t;


void awbl_ov7725_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_OV7725_H */
