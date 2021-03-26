/*
 * awbl_gc0308.c
 *
 *  Created on: 2019年8月8日
 *      Author: weike
 */

#include "Aworks.h"
#include "awbus_lite.h"
#include "aw_sem.h"
#include "awbl_plb.h"
#include "aw_delay.h"
#include "aw_mem.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"
#include "driver/camera/awbl_gc0308.h"
#include "aw_cache.h"
#include "aw_camera.h"

//#define likely(x)   __builtin_expect(!!(x), 1)
//#define unlikely(x) __builtin_expect(!!(x), 0)

#define __GC0308_DEBUG      0

#define __GC0308_I2C_ADDR   0x21
#define __GC0308_ID         0x9B

#define __GC0308_DEV_DECL(p_this, p_dev) \
            awbl_gc0308_dev_t *(p_this) = \
                (awbl_gc0308_dev_t *)(p_dev)

#define __GC0308_DEVINFO_DECL(p_info, p_dev) \
            awbl_gc0308_devinfo_t *(p_info) = \
                (awbl_gc0308_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

#define VIDEO_SIZE(r)       (VIDEO_EXTRACT_WIDTH(r) * \
                             VIDEO_EXTRACT_HEIGHT(r))
#define PHOTO_SIZE(r)       (VIDEO_EXTRACT_WIDTH(r) * \
                             VIDEO_EXTRACT_WIDTH(r))
#define PHOTO_MEM_SIZE(p, n, bn) \
                            (((p)*(n)*(bn)+AW_CACHE_LINE_SIZE-1) \
                             / AW_CACHE_LINE_SIZE * AW_CACHE_LINE_SIZE)

#define __GC0308_MAX_WIDTH  640 //644
#define __GC0308_MAX_HIGHT  480 //482

#define __GC0308_MAX_RATE   60
#define __GC0308_HB_MIN     100
#define __GC0308_VB_MIN     150

#define __APWD_MASK         0x01
#define __GC0308_PAGE_MASK  0x01

#define __SUBSAMPLE_GATE_ENABLE_MASK    0x80
#define __SUBSAMPLE_ENABLE_MASK         0x01

#define __SUBSAMPLE_CUT     0x00
#define __SUBSAMPLE_USE     0x08

#define __STATUS_SET(x, v)  ((x) |= (v))
#define __STATUS_CLR(x, v)  ((x) &= (~(v)))
#define __STATUS_IS(x, v)   ((x) & (v))

#define __FIX_WIN_STATE     0x01
#define __STANDBY_STATE     0x02

#define __VIDEO_MODE_STATE      0x40000000
#define __PHOTO_MODE_STATE      0x20000000
#define __PHOTO_SEAL_STATE      0x10000000

#define __VIDEO_RUNNING_STATE   0x04000000
#define __PHOTO_RUNNING_STATE   0x02000000

typedef struct gc0308_subsample {
    uint8_t row_ratio;
    uint8_t col_ratio;

    uint8_t row[4];
    uint8_t col[4];
} __subsample_t ;

typedef struct gc0305_window {
    uint16_t col_start;
    uint16_t row_start;
    uint16_t width;
    uint16_t height;
} __window_t ;

typedef enum gc0308_format {
    __CbYCrY = 0x00,    __CrYCbY = 0x01,    __YCbYCr = 0x02,
    __YCrYCb = 0x03,    __RGB565 = 0x06,    __RGBx555 = 0x07,
    __RGB555x = 0x08,   __RGBx444 = 0x09,   __RGB444x = 0x0A,
    __BGRG = 0x0B,      __RGBG = 0x0C,      __GBGR = 0x0D,
    __GRGB = 0x0E,      __bypass_10bit = 0x0F,
    __Y = 0x11,         __Cb = 0x12,        __Cr = 0x13,
    __R = 0x14,         __G = 0x15,         __B = 0x16,
    __bayer_pattern = 0x17,
    __DNDD = 0x18,       __LSC = 0x19,
} __format_t ;

typedef enum gc0308_divider {
    none_devide  = 0x00,
    two_devide   = 0x11,
    three_devide = 0x21,
    four_devide  = 0x32,
    five_devide  = 0x42,
    six_devide   = 0x53,
    seven_devide = 0x63,
    eight_devide = 0x74,
} __divider_t ;

const gc0308_reg_t gc0308_init_list[] =
{
    {0xfe, 0x80},
    {0xfe, 0x00},   // set page0

    {0xd2, 0x10},   // close AEC
    {0x22, 0x55},   // close AWB

    {0x03, 0x01}, {0x04, 0x2c}, {0x5a, 0x56}, {0x5b, 0x40},
    {0x5c, 0x4a},

    {0x22, 0x57},   // Open AWB

    {0x01, 0xfa}, {0x02, 0x70}, {0x0f, 0x01},
    {0xe2, 0x00},   //anti-flicker step [11:8]
    {0xe3, 0x64},   //anti-flicker step [7:0]
    {0xe4, 0x02}, {0xe5, 0x58},  //exp level 1  16.67fps

    {0xe6, 0x03}, {0xe7, 0x20},   //exp level 2  12.5fps

    {0xe8, 0x04}, {0xe9, 0xb0},   //exp level 3  8.33fps

    {0xea, 0x09}, {0xeb, 0xc4},   //exp level 4  4.00fps

    //{0xec, 0x20},
    {0x05, 0x00}, {0x06, 0x00}, {0x07, 0x00}, {0x08, 0x00},
    {0x09, 0x01}, {0x0a, 0xe8}, {0x0b, 0x02}, {0x0c, 0x88},
    {0x0d, 0x02}, {0x0e, 0x02}, {0x10, 0x22}, {0x11, 0xfd},
    {0x12, 0x2a}, {0x13, 0x00}, {0x15, 0x0a}, {0x16, 0x05},
    {0x17, 0x01}, {0x18, 0x44}, {0x19, 0x44}, {0x1a, 0x1e},
    {0x1b, 0x00}, {0x1c, 0xc1}, {0x1d, 0x08}, {0x1e, 0x60},
    {0x1f, 0x17}, {0x20, 0xff}, {0x21, 0xf8}, {0x22, 0x57},
    {0x24, 0xa2}, {0x25, 0x0f},

    //output sync_mode
    {0x26, 0x02},   //0x03  20101016 zhj
    {0x2f, 0x01}, {0x30, 0xf7}, {0x31, 0x50}, {0x32, 0x00},
    {0x39, 0x04}, {0x3a, 0x18}, {0x3b, 0x20}, {0x3c, 0x00},
    {0x3d, 0x00}, {0x3e, 0x00}, {0x3f, 0x00}, {0x50, 0x10},
    {0x53, 0x82}, {0x54, 0x80}, {0x55, 0x80}, {0x56, 0x82},
    {0x8b, 0x40}, {0x8c, 0x40}, {0x8d, 0x40}, {0x8e, 0x2e},
    {0x8f, 0x2e}, {0x90, 0x2e}, {0x91, 0x3c}, {0x92, 0x50},
    {0x5d, 0x12}, {0x5e, 0x1a}, {0x5f, 0x24}, {0x60, 0x07},
    {0x61, 0x15}, {0x62, 0x08}, {0x64, 0x03}, {0x66, 0xe8},
    {0x67, 0x86}, {0x68, 0xa2}, {0x69, 0x18}, {0x6a, 0x0f},
    {0x6b, 0x00}, {0x6c, 0x5f}, {0x6d, 0x8f}, {0x6e, 0x55},
    {0x6f, 0x38}, {0x70, 0x15}, {0x71, 0x33}, {0x72, 0xdc},
    {0x73, 0x80}, {0x74, 0x02}, {0x75, 0x3f}, {0x76, 0x02},
    {0x77, 0x36}, {0x78, 0x88}, {0x79, 0x81}, {0x7a, 0x81},
    {0x7b, 0x22}, {0x7c, 0xff}, {0x93, 0x48}, {0x94, 0x00},
    {0x95, 0x05}, {0x96, 0xe8}, {0x97, 0x40}, {0x98, 0xf0},
    {0xb1, 0x38}, {0xb2, 0x38}, {0xbd, 0x38}, {0xbe, 0x36},
    {0xd0, 0xc9}, {0xd1, 0x10}, {0xd3, 0x80}, {0xd5, 0xf2},
    {0xd6, 0x16}, {0xdb, 0x92}, {0xdc, 0xa5}, {0xdf, 0x23},
    {0xd9, 0x00}, {0xda, 0x00}, {0xe0, 0x09}, {0xed, 0x04},
    {0xee, 0xa0}, {0xef, 0x40}, {0x80, 0x03}, {0x80, 0x03},
    {0x9F, 0x10}, {0xA0, 0x20}, {0xA1, 0x38}, {0xA2, 0x4E},
    {0xA3, 0x63}, {0xA4, 0x76}, {0xA5, 0x87}, {0xA6, 0xA2},
    {0xA7, 0xB8}, {0xA8, 0xCA}, {0xA9, 0xD8}, {0xAA, 0xE3},
    {0xAB, 0xEB}, {0xAC, 0xF0}, {0xAD, 0xF8}, {0xAE, 0xFD},
    {0xAF, 0xFF}, {0xc0, 0x00}, {0xc1, 0x10}, {0xc2, 0x1C},
    {0xc3, 0x30}, {0xc4, 0x43}, {0xc5, 0x54}, {0xc6, 0x65},
    {0xc7, 0x75}, {0xc8, 0x93}, {0xc9, 0xB0}, {0xca, 0xCB},
    {0xcb, 0xE6}, {0xcc, 0xFF}, {0xf0, 0x02}, {0xf1, 0x01},
    {0xf2, 0x01}, {0xf3, 0x30}, {0xf9, 0x9f}, {0xfa, 0x78},

    //---------------------------------------------------------------
    {0xfe, 0x01},// set page1
    {0x00, 0xf5}, {0x02, 0x1a}, {0x0a, 0xa0}, {0x0b, 0x60},
    {0x0c, 0x08}, {0x0e, 0x4c}, {0x0f, 0x39}, {0x11, 0x3f},
    {0x12, 0x72}, {0x13, 0x13}, {0x14, 0x42}, {0x15, 0x43},
    {0x16, 0xc2}, {0x17, 0xa8}, {0x18, 0x18}, {0x19, 0x40},
    {0x1a, 0xd0}, {0x1b, 0xf5}, {0x70, 0x40}, {0x71, 0x58},
    {0x72, 0x30}, {0x73, 0x48}, {0x74, 0x20}, {0x75, 0x60},
    {0x77, 0x20}, {0x78, 0x32}, {0x30, 0x03}, {0x31, 0x40},
    {0x32, 0xe0}, {0x33, 0xe0}, {0x34, 0xe0}, {0x35, 0xb0},
    {0x36, 0xc0}, {0x37, 0xc0}, {0x38, 0x04}, {0x39, 0x09},
    {0x3a, 0x12}, {0x3b, 0x1C}, {0x3c, 0x28}, {0x3d, 0x31},
    {0x3e, 0x44}, {0x3f, 0x57}, {0x40, 0x6C}, {0x41, 0x81},
    {0x42, 0x94}, {0x43, 0xA7}, {0x44, 0xB8}, {0x45, 0xD6},
    {0x46, 0xEE}, {0x47, 0x0d},

    {0xfe, 0x00}, // set page0
    //-----------Update the registers 2010/07/06-------------//
    //Registers of Page0
    {0xfe, 0x00}, // set page0
    {0x10, 0x26},
    {0x11, 0x0d},  // fd,modified by mormo 2010/07/06
    {0x1a, 0x2a},  // 1e,modified by mormo 2010/07/06

    {0x1c, 0x49}, // c1,modified by mormo 2010/07/06
    {0x1d, 0x9a}, // 08,modified by mormo 2010/07/06
    {0x1e, 0x61}, // 60,modified by mormo 2010/07/06
    {0x3a, 0x20},
    {0x50, 0x14},  // 10,modified by mormo 2010/07/06
    {0x53, 0x80},
    {0x56, 0x80},
    {0x8b, 0x20}, {0x8c, 0x20}, {0x8d, 0x20}, {0x8e, 0x14}, //LSC
    {0x8f, 0x10}, {0x90, 0x14}, {0x94, 0x02}, {0x95, 0x07}, //LSC
    {0x96, 0xe0}, //LSC
    {0xb1, 0x40}, // YCPT
    {0xb2, 0x40},
    {0xb3, 0x40},
    {0xb6, 0xe0},
    {0xd0, 0xcb}, // AECT  c9,modifed by mormo 2010/07/06
    {0xd3, 0x48}, // 80,modified by mormor 2010/07/06
    {0xf2, 0x02},
    {0xf7, 0x12},
    {0xf8, 0x0a},
    //Registers of Page1
    {0xfe, 0x01},// set page1
    {0x02, 0x20}, {0x04, 0x10}, {0x05, 0x08}, {0x06, 0x20},
    {0x08, 0x0a}, {0x0e, 0x44}, {0x0f, 0x32}, {0x10, 0x41},
    {0x11, 0x37}, {0x12, 0x22}, {0x13, 0x19}, {0x14, 0x44},
    {0x15, 0x44}, {0x19, 0x50}, {0x1a, 0xd8}, {0x32, 0x10},
    {0x35, 0x00}, {0x36, 0x80}, {0x37, 0x00},
    //-----------Update the registers end---------//

    {0xfe, 0x00}, // set page0
    {0xd2, 0x90},

    //-----------GAMMA Select(3)---------------//
    {0x9F, 0x10}, {0xA0, 0x20}, {0xA1, 0x38}, {0xA2, 0x4E},
    {0xA3, 0x63}, {0xA4, 0x76}, {0xA5, 0x87}, {0xA6, 0xA2},
    {0xA7, 0xB8}, {0xA8, 0xCA}, {0xA9, 0xD8}, {0xAA, 0xE3},
    {0xAB, 0xEB}, {0xAC, 0xF0}, {0xAD, 0xF8}, {0xAE, 0xFD},
    {0xAF, 0xFF},

     /*GC0308_GAMMA_Select,
        1:                                             //smallest gamma curve
            {0x9F, 0x0B}, {0xA0, 0x16}, {0xA1, 0x29}, {0xA2, 0x3C},
            {0xA3, 0x4F}, {0xA4, 0x5F}, {0xA5, 0x6F}, {0xA6, 0x8A},
            {0xA7, 0x9F}, {0xA8, 0xB4}, {0xA9, 0xC6}, {0xAA, 0xD3},
            {0xAB, 0xDD}, {0xAC, 0xE5}, {0xAD, 0xF1}, {0xAE, 0xFA},
            {0xAF, 0xFF},

        2:
            {0x9F, 0x0E}, {0xA0, 0x1C}, {0xA1, 0x34}, {0xA2, 0x48},
            {0xA3, 0x5A}, {0xA4, 0x6B}, {0xA5, 0x7B}, {0xA6, 0x95},
            {0xA7, 0xAB}, {0xA8, 0xBF}, {0xA9, 0xCE}, {0xAA, 0xD9},
            {0xAB, 0xE4}, {0xAC, 0xEC}, {0xAD, 0xF7}, {0xAE, 0xFD},
            {0xAF, 0xFF},

        3:
            {0x9F, 0x10}, {0xA0, 0x20}, {0xA1, 0x38}, {0xA2, 0x4E},
            {0xA3, 0x63}, {0xA4, 0x76}, {0xA5, 0x87}, {0xA6, 0xA2},
            {0xA7, 0xB8}, {0xA8, 0xCA}, {0xA9, 0xD8}, {0xAA, 0xE3},
            {0xAB, 0xEB}, {0xAC, 0xF0}, {0xAD, 0xF8}, {0xAE, 0xFD},
            {0xAF, 0xFF},

        4:
            {0x9F, 0x14}, {0xA0, 0x28}, {0xA1, 0x44}, {0xA2, 0x5D},
            {0xA3, 0x72}, {0xA4, 0x86}, {0xA5, 0x95}, {0xA6, 0xB1},
            {0xA7, 0xC6}, {0xA8, 0xD5}, {0xA9, 0xE1}, {0xAA, 0xEA},
            {0xAB, 0xF1}, {0xAC, 0xF5}, {0xAD, 0xFB}, {0xAE, 0xFE},
            {0xAF, 0xFF},

        5:                              //largest gamma curve
            {0x9F, 0x15}, {0xA0, 0x2A}, {0xA1, 0x4A}, {0xA2, 0x67},
            {0xA3, 0x79}, {0xA4, 0x8C}, {0xA5, 0x9A}, {0xA6, 0xB3},
            {0xA7, 0xC5}, {0xA8, 0xD5}, {0xA9, 0xDF}, {0xAA, 0xE8},
            {0xAB, 0xEE}, {0xAC, 0xF3}, {0xAD, 0xFA}, {0xAE, 0xFD},
            {0xAF, 0xFF}, */
    //-----------GAMMA Select End--------------//

    //-------------H_V_Switch(4)---------------//
            {0x14, 0x12},  //0x10

     /*GC0308_H_V_Switch,

        1:  // normal
                {0x14, 0x10},

        2:  // IMAGE_H_MIRROR
                {0x14, 0x11},

        3:  // IMAGE_V_MIRROR
                {0x14, 0x12},

        4:  // IMAGE_HV_MIRROR
                {0x14, 0x13},
    */
    //-------------H_V_Select End--------------//

};

aw_local void __i2c_init(void *p_cookie);
aw_local aw_err_t __i2c_reg_write(void *p_cookie, uint8_t reg, uint8_t data);
aw_local aw_err_t __i2c_reg_read (void *p_cookie, uint8_t reg, uint8_t *data);
aw_local aw_err_t __i2c_reg_set  (void *p_cookie, uint8_t reg, uint8_t data, uint8_t mask);

aw_local aw_err_t __gc0308_page_choose(void *p_cookie, uint8_t page);
aw_local aw_err_t gc0308_check_id(void *p_cookie);
aw_local aw_err_t __gc0308_startup(void *p_cookie);
aw_local aw_err_t __gc0308_standby(void *p_cookie);
void __gc0308_center_count(__window_t *p_win);
aw_local aw_err_t __gc0308_window_set(void *p_cookie, __window_t *p_win);
aw_local aw_err_t __gc0308_window_get(void *p_cookie, __window_t *p_win);
aw_local aw_err_t __gc0308_measure_window_set(void *p_cookie, __window_t *p_win);
aw_local aw_err_t __gc0308_measure_window_get(void *p_cookie, __window_t *p_win);
aw_local aw_err_t __gc0308_subsample_set(void *p_cookie, __subsample_t *p_sample);
aw_local aw_err_t __gc0308_format_set(void *p_cookie, __format_t format);
aw_local aw_err_t __gc0308_format_get(void *p_cookie, __format_t *format);
aw_local aw_err_t __gc0308_exposure_time_set(void *p_cookie, uint16_t exposure_time);
aw_local aw_err_t __gc0308_exposure_time_get(void *p_cookie, uint16_t *exposure_time);
aw_local aw_err_t __gc0308_row_time_get(void *p_cookie, uint16_t *time);
aw_local aw_err_t __gc0308_clock_get(void *p_cookie, uint32_t *clock);
aw_local aw_err_t __gc0308_exp_level_set(void *p_cookie, uint16_t *exp_level);
aw_local aw_err_t __gc0308_rate_calculate(void *p_cookie, uint8_t flicker, __window_t win,
                                          uint16_t *hb, uint16_t *vb, uint16_t *step,
                                          uint32_t *clock, float *frame_rate);
aw_local aw_err_t __gc0308_frame_rate_set(void *p_cookie);
aw_local aw_err_t __gc0308_init_set(void *p_cookie);

aw_local aw_err_t __gc0308_aec_set(void *p_cookie, aw_camera_cfg_t *p_cfg);
aw_local aw_err_t __gc0308_aec_get(void *p_cookie, aw_camera_cfg_t *p_cfg);
aw_local aw_err_t __gc0308_awb_set(void *p_cookie, aw_camera_cfg_t *p_cfg);
aw_local aw_err_t __gc0308_awb_get(void *p_cookie, aw_camera_cfg_t *p_cfg);
aw_local aw_err_t __gc0308_conversion_set(void *p_cookie, aw_camera_cfg_t *p_cfg);
aw_local aw_err_t __gc0308_conversion_get(void *p_cookie, aw_camera_cfg_t *p_cfg);
aw_local aw_err_t __gc0308_window_fix(void *p_cookie, __window_t win,
                                      uint32_t photo_resolution, uint32_t video_resolution);
aw_local aw_err_t __gc0308_resolution_set(void *p_cookie, uint32_t resolution);


/******************************I2C TRANSLATE**********************************/

inline void __i2c_init(void *p_cookie)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    __GC0308_DEVINFO_DECL(p_devinfo, p_cookie);

    aw_i2c_mkdev(&p_this->i2c_device,
                 p_devinfo->i2c_master_busid,
                 __GC0308_I2C_ADDR,
                 AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);
}

inline aw_err_t __i2c_reg_write(void *p_cookie, uint8_t reg, uint8_t data)
{
    __GC0308_DEV_DECL(p_this, p_cookie);

    return aw_i2c_write(&p_this->i2c_device, reg, &data, 1);
}

inline aw_err_t __i2c_reg_read(void *p_cookie, uint8_t reg, uint8_t *data)
{
    __GC0308_DEV_DECL(p_this, p_cookie);

    return aw_i2c_read(&p_this->i2c_device, reg, data, 1);
}

aw_local aw_err_t __i2c_reg_set(void *p_cookie, uint8_t reg, uint8_t data, uint8_t mask)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    uint8_t  tmp;

    __i2c_reg_read(p_this, reg, &tmp);

    tmp &= (~mask);
    tmp |= (mask & data);

    return __i2c_reg_write(p_this, reg, tmp);
}

/*****************************************************************************/


/******************************GC0308 SETTING*********************************/

aw_local aw_err_t __gc0308_page_choose(void *p_cookie, uint8_t page)
{
    __GC0308_DEV_DECL(p_this, p_cookie);

    return __i2c_reg_set(p_this, REST_RELATED, page, __GC0308_PAGE_MASK);
}

aw_local aw_err_t gc0308_check_id(void *p_cookie)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;
    uint8_t tmp;

    ret = __i2c_reg_read(p_this, CHIP_ID, &tmp);

    if ((tmp != __GC0308_ID) || (ret != AW_OK)) {
        aw_kprintf("gc0308 id identify failed \n\r");
        return -AW_ENODEV;
    }

    return AW_OK;
}

aw_local aw_err_t __gc0308_startup(void *p_cookie)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;

    ret = __i2c_reg_set(p_this, ANALOG_MODE1, 0x00, __APWD_MASK);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, OUTPUT_EN, 0xFF);
    if (ret != AW_OK) {
        return ret;
    }

    return AW_OK;
}

aw_local aw_err_t __gc0308_standby(void *p_cookie)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;

    ret = __i2c_reg_set(p_this, ANALOG_MODE1, 0x01, __APWD_MASK);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, OUTPUT_EN, 0x00);
    if (ret != AW_OK) {
        return ret;
    }

    return AW_OK;
}

inline void __gc0308_center_count(__window_t *p_win)
{
    p_win->col_start = (__GC0308_MAX_WIDTH - p_win->width) / 2;
    p_win->row_start = (__GC0308_MAX_HIGHT - p_win->height) / 2;
}

aw_local aw_err_t __gc0308_window_set(void *p_cookie, __window_t *p_win)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;

    if (((p_win->col_start + p_win->width) > __GC0308_MAX_WIDTH) && \
        ((p_win->row_start + p_win->height) > __GC0308_MAX_HIGHT)){
        return -AW_EINVAL;
    }

    ret = __i2c_reg_write(p_this, COLUMNSTART_H, p_win->col_start>>8);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, COLUMNSTART_L, p_win->col_start);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, ROWSTART_H, p_win->row_start>>8);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, ROWSTART_L, p_win->row_start);
    if (ret != AW_OK) {
        return ret;
    }

    ret = __i2c_reg_write(p_this, WINDOWWIDTH_H, (p_win->width + 8)>>8);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, WINDOWWIDTH_L, p_win->width + 8);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, WINDOWHEIGH_H, (p_win->height + 8)>>8);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, WINDOWHEIGH_L, p_win->height + 8);
    if (ret != AW_OK) {
        return ret;
    }

    return AW_OK;
}

aw_local aw_err_t __gc0308_window_get(void *p_cookie, __window_t *p_win)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;
    uint8_t tmp;

    ret = __i2c_reg_read(p_this, COLUMNSTART_H, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    p_win->col_start = (uint16_t)tmp<<8;
    ret = __i2c_reg_read(p_this, COLUMNSTART_L, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    p_win->col_start |= tmp;

    ret = __i2c_reg_read(p_this, ROWSTART_H, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    p_win->row_start = (uint16_t)tmp<<8;
    ret = __i2c_reg_read(p_this, ROWSTART_L, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    p_win->row_start |= tmp;

    aw_mdelay(2);
    ret = __i2c_reg_read(p_this, WINDOWWIDTH_H, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    p_win->width = (uint16_t)tmp<<8;
    aw_mdelay(2);
    ret = __i2c_reg_read(p_this, WINDOWWIDTH_L, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    p_win->width |= tmp;

    aw_mdelay(2);
    ret = __i2c_reg_read(p_this, WINDOWHEIGH_H, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    p_win->height = (uint16_t)tmp<<8;
    aw_mdelay(2);
    ret = __i2c_reg_read(p_this, WINDOWHEIGH_L, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    p_win->height |= tmp;

    p_win->width -= 8;
    p_win->height -= 8;

    return AW_OK;
}

aw_local aw_err_t __gc0308_measure_window_set(void *p_cookie, __window_t *p_win)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;

    if (((p_win->col_start + p_win->width) > __GC0308_MAX_WIDTH) && \
        ((p_win->row_start + p_win->height) > __GC0308_MAX_HIGHT)){
        return -AW_EINVAL;
    }

    ret = __i2c_reg_write(p_this, BIG_WIN_X0, p_win->col_start>>2);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, BIG_WIN_Y0, p_win->row_start>>2);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, BIG_WIN_X1, (p_win->col_start + p_win->width) >> 2);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, BIG_WIN_Y1, (p_win->row_start + p_win->height) >> 2);
    if (ret != AW_OK) {
        return ret;
    }

    return AW_OK;
}

aw_local aw_err_t __gc0308_measure_window_get(void *p_cookie, __window_t *p_win)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;
    uint8_t tmp;

    ret = __i2c_reg_read(p_this, BIG_WIN_X0, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    p_win->col_start = (uint16_t)tmp << 2;
    ret = __i2c_reg_read(p_this, BIG_WIN_Y0, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    p_win->row_start = (uint16_t)tmp << 2;
    ret = __i2c_reg_read(p_this, BIG_WIN_X1, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    p_win->width = ((uint16_t)tmp << 2) - p_win->col_start;
    ret = __i2c_reg_read(p_this, BIG_WIN_Y1, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    p_win->height = ((uint16_t)tmp << 2) - p_win->height;

    return AW_OK;
}

aw_local aw_err_t __gc0308_subsample_set(void *p_cookie, __subsample_t *p_sample)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;
    uint8_t tmp = 0;

    ret = __gc0308_page_choose(p_this, 1);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_set(p_this, BAYER_MODE,  0x80, __SUBSAMPLE_GATE_ENABLE_MASK);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_set(p_this, BAYER_MODE,  0x01, __SUBSAMPLE_ENABLE_MASK);
    if (ret != AW_OK) {
        return ret;
    }

    tmp = p_sample->col[0]<<4 | p_sample->col[1];
    ret = __i2c_reg_write(p_this, SUB_COL_N1, tmp);
    if (ret != AW_OK) {
        return ret;
    }
    tmp = p_sample->col[2]<<4 | p_sample->col[3];
    ret = __i2c_reg_write(p_this, SUB_COL_N2, tmp);
    if (ret != AW_OK) {
        return ret;
    }

    tmp = p_sample->row[0]<<4 | p_sample->row[1];
    ret = __i2c_reg_write(p_this, SUB_ROW_N1, tmp);
    if (ret != AW_OK) {
        return ret;
    }
    tmp = p_sample->row[2]<<4 | p_sample->row[3];
    ret = __i2c_reg_write(p_this, SUB_ROW_N2, tmp);
    if (ret != AW_OK) {
        return ret;
    }

    tmp = (p_sample->row_ratio << 4) | (p_sample->col_ratio & 0x0F);
    ret = __i2c_reg_write(p_this, SUBSAMPLE, tmp);
    if (ret != AW_OK) {
        return ret;
    }

    return __gc0308_page_choose(p_this, 0);
}


aw_local aw_err_t __gc0308_format_set(void *p_cookie, __format_t format)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;
    uint8_t tmp;

    ret = __gc0308_page_choose(p_this, 1);
    if (ret != AW_OK) {
        return ret;
    }

    tmp = (format == __bayer_pattern) ? 1<<5 : 0;
    ret = __i2c_reg_set(p_this, BAYER_MODE, tmp, 0x60);
    if (ret != AW_OK) {
        return ret;
    }

    ret = __gc0308_page_choose(p_this, 0);
    if (ret != AW_OK) {
        return ret;
    }

    return __i2c_reg_write(p_this, OUTPUT_FORMAT, 0xa0|format);
}

aw_local aw_err_t __gc0308_format_get(void *p_cookie, __format_t *format)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;

    ret = __i2c_reg_read(p_this, OUTPUT_FORMAT, format);
    if (ret != AW_OK) {
        return ret;
    }

    *format &= 0x1F;
    return AW_OK;
}

aw_local aw_err_t __gc0308_exposure_time_set(void *p_cookie, uint16_t exposure_time)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;

    ret = __i2c_reg_write(p_this, EXPOSURE_H, (exposure_time & 0x0F00) >> 8);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, EXPOSURE_L, (exposure_time & 0xFF));
    if (ret != AW_OK) {
        return ret;
    }
    return AW_OK;
}

aw_local aw_err_t __gc0308_exposure_time_get(void *p_cookie, uint16_t *exposure_time)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;
    uint8_t tmp;

    ret = __i2c_reg_read(p_this, EXPOSURE_H, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    *exposure_time = (uint16_t)(tmp & 0x0F) << 8;

    ret = __i2c_reg_read(p_this, EXPOSURE_L, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    *exposure_time |= tmp;

    return AW_OK;
}

aw_local aw_err_t __gc0308_row_time_get(void *p_cookie, uint16_t *time)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;
    uint8_t sh_delay, tmp;
    uint16_t hb, width;

    ret = __i2c_reg_read(p_this, SH_DELAY, &sh_delay);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_read(p_this, VB_HB, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    hb = (tmp & 0x0F) << 8;
    ret = __i2c_reg_read(p_this, HB, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    hb |= tmp;

    ret = __i2c_reg_read(p_this, WINDOWWIDTH_H, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    width = tmp;
    width <<= 8;
    ret = __i2c_reg_read(p_this, WINDOWWIDTH_H, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    width |= tmp;

    *time = width + (uint16_t)sh_delay + hb + 4;
    return AW_OK;
}

aw_local aw_err_t __gc0308_clock_get(void *p_cookie, uint32_t *clock)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    __GC0308_DEVINFO_DECL(p_devinfo, p_cookie);
    uint8_t devide;
    aw_err_t ret;

    ret = __i2c_reg_read(p_this, CLK_DIV_MODE, &devide);
    if (ret != AW_OK) {
        return ret;
    }
    devide >>= 4;
    devide +=1 ;
    *clock = p_devinfo->input_clock / (devide * p_this->cfg.bytes_per_pix);

    return AW_OK;
}

aw_local aw_err_t __gc0308_exp_level_set(void *p_cookie, uint16_t *exp_level)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;
    ret = __i2c_reg_write(p_this, EXP_LEVEL_1_H, (exp_level[0] & 0x0F00) >> 8);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, EXP_LEVEL_1_L, exp_level[0] & 0xFF);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, EXP_LEVEL_2_H, (exp_level[1] & 0x0F00) >> 8);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, EXP_LEVEL_2_L, exp_level[1] & 0xFF);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, EXP_LEVEL_3_H, (exp_level[2] & 0x0F00) >> 8);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, EXP_LEVEL_3_L, exp_level[2] & 0xFF);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, EXP_LEVEL_4_H, (exp_level[3] & 0x0F00) >> 8);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, EXP_LEVEL_4_L, exp_level[3] & 0xFF);
    if (ret != AW_OK) {
        return ret;
    }

    return AW_OK;
}

/*
 * Frame time(min) = vertical blank + vertical time + 8 (unit: row time)
 * Vertical blank = blank time + start time + end time (unit: row time)
 *
 * Row time =  horizontal blank + Sh_delay + win width + 4(unit: pixel clock)
 *
 * Anti flicker time:
 *      step * row time = N(any integer) * T(frequency of modulator tube)
 *      row time should be (N * T / step)
 *      (vertical blank + vertical) time should be (N * T)
 *
 * Frame time:
 *      when exposure time larger than Frame time(min),frame is determined by
 *    exposure time(should be multiple of step); else determined by Frame time(min).
 *
 */
aw_local aw_err_t __gc0308_rate_calculate(void *p_cookie,
                                          uint8_t flicker,
                                          __window_t win,
                                          uint16_t *hb,
                                          uint16_t *vb,
                                          uint16_t *step,
                                          uint32_t *clock,
                                          float *frame_rate)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    uint8_t  sh_delay, et, st;
    uint16_t  vb_unit;
    uint32_t  flicker_time, frame_time, row_time;
    aw_err_t ret;

    if (flicker == 0) {
        //todo: could be optimized
        flicker = 50;
    }

    //get current information
    flicker_time = *clock / (flicker * 2);
    if (flicker_time == 0) {
        return -AW_EINVAL;
    }

    //get current row_time(without hb)
    ret = __i2c_reg_read(p_this, SH_DELAY, &sh_delay);
    row_time = win.width+8 + sh_delay +4;

    //count hb and step
    *step = flicker_time/row_time;
    while (1) {
        if ((flicker_time % (--(*step))) != 0) {
            continue;
        }
        *hb = flicker_time / *step - row_time;
        if ((*hb > 0x0FFF) || \
            (*hb < __GC0308_HB_MIN) ||  \
            (*step > 0x0FFF)) {
            continue;
        }
        break;
    }

    if (*step == 0) {
        return -AW_EINVAL;
    }
    row_time = flicker_time / (*step);
    if ((*hb < 1) || (*step < 1)) {
        //todo:  return what?
    }

    ret = __i2c_reg_read(p_this, VS_ST, &st);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_read(p_this, VS_ET, &et);
    if (ret != AW_OK) {
        return ret;
    }
    vb_unit = flicker_time/row_time;
    while (1) {
        *vb += vb_unit;
        if ((*vb < (win.height+8)) ||\
            (*vb-(win.height+8) < (st+et+6)) ||\
            (*vb < __GC0308_VB_MIN)) {
            continue;
        }

        *frame_rate = (float)(*clock * 1.0 / (*vb * row_time));
        if (*frame_rate > __GC0308_MAX_RATE) {
            continue;
        }
        break;
    }

    *vb -= (win.height+8);

    //get frame rate
    frame_time = *vb + (win.height+8);
    *frame_rate = (float)(*clock * 1.0 / (frame_time * row_time));

    return AW_OK;
}

aw_local aw_err_t __gc0308_frame_rate_set(void *p_cookie)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;
    int i;
    uint16_t vb = 0, hb = 0, step = 0, exp_level[4];
    __window_t win;
    float frame_rate;
    uint32_t clock;

    ret = __gc0308_clock_get(p_this, &clock);
    if (ret != AW_OK) {
        return ret;
    }

    ret = __gc0308_window_get(p_this, &win);
    if (ret != AW_OK) {
        return ret;
    }

    if ((clock == 12000000) && (win.height == 480) && (win.width == 640)) {
        hb = 106;
        step = 150;
        vb = 112;
        frame_rate = 25;
    }
    else if ((clock == 12000000) && (win.height == 240) && (win.width == 320)) {
        hb = 226;
        step = 200;
        vb = 152;
        frame_rate = 50;
    }
    else{
        ret = __gc0308_rate_calculate(p_this, 50, win, &hb, &vb, &step, &clock, &frame_rate);
        if (ret != AW_OK) {
            return ret;
        }
    }

    ret = __i2c_reg_set(p_this, VB_HB, (uint8_t)((hb&0xF00)>>8), 0x0F);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, HB, (uint8_t)(hb&0xFF));
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_set(p_this, ANTI_FLICKER_STEP_H, (uint8_t)((step&0x0F00)>>8), 0x0F);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, ANTI_FLICKER_STEP_L, (uint8_t)(step&0xFF));
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_set(p_this, VB_HB, (uint8_t)((vb&0x0F00)>>4), 0xF0);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, VB, (uint8_t)(vb&0xFF));
    if (ret != AW_OK) {
        return ret;
    }

    exp_level[0] = vb + win.height+8 + step;

    for (i = 1; i < 4; i++) {
        exp_level[i] = exp_level[i-1];
        while(exp_level[i] < (i * 1000)) {
            exp_level[i] += step;
        }
    }
    ret = __gc0308_exp_level_set(p_this, exp_level);
    if (ret != AW_OK) {
        return ret;
    }

    ret = __i2c_reg_set(p_this, MAX_EXP_LEVEL, 0x20, 0x30);
    if (ret != AW_OK) {
        return ret;
    }

#if __GC0308_DEBUG
    aw_kprintf("Max frame is %lf\n\r", frame_rate);
#endif
    return AW_OK;
}

aw_local aw_err_t __gc0308_init_set(void *p_cookie)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    __GC0308_DEVINFO_DECL(p_devinfo, p_cookie);
    aw_err_t ret;
    uint8_t tmp;

    //aec init
    ret = __gc0308_page_choose(p_this, 0);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_set(p_this, AEC_MODE3, 0x80, 0x80);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_set(p_this, AEC_MODE2, 1<<4, 0x70);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_set(p_this, EXP_MIN_I_H, 0x00, 0x0F);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, EXP_MIN_I_L, 0x04);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, MAX_POST_DG_GAIN, 0x40);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, MAX_PRE_DG_GAIN, 0x80);
    if (ret != AW_OK) {
        return ret;
    }

    //awb
    ret = __gc0308_page_choose(p_this, 1);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, AWB_R_GAIN_LIMIT, 0x80);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, AWB_G_GAIN_LIMIT, 0x80);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, AWB_B_GAIN_LIMIT, 0x80);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_set(p_this, CT_MODE, 1<<4, 0x30);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __gc0308_page_choose(p_this, 0);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, R_RATIO, 0x80);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, G_RATIO, 0x80);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, B_RATIO, 0x80);
    if (ret != AW_OK) {
        return ret;
    }

    //LSC
    ret = __i2c_reg_set(p_this, BLOCK_ENABLE_1, 0x01, 0x01);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_set(p_this, LSC_ROW_CENTER, 60, 0x7F);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, LSC_COLUMN_CENTER, 80);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, LSC_RED_B2, 96);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, LSC_GREEN_B2, 96);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, LSC_BLUE_B2, 96);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, LSC_RED_B4, 50);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, LSC_GREEN_B4, 40);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, LSC_BLUE_B4, 50);
    if (ret != AW_OK) {
        return ret;
    }

    //dndd
    ret = __i2c_reg_set(p_this, BLOCK_ENABLE_1, 3<<1, 0x06);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_set(p_this, DD_DARK_BRIGHT_TH, 3<<4, 0xF0);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_set(p_this, DD_DARK_BRIGHT_TH, 7<<0, 0x0F);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, DN_BILAT_B_BASE, 0x0c);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_set(p_this, DN_BILAT_N_BASE, 3<<0, 0x0F);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_set(p_this, DN_MODE_RATIO, 2<<4|2<<0, 0x33);
    if (ret != AW_OK) {
        return ret;
    }

    //edge
    ret = __i2c_reg_set(p_this, BLOCK_ENABLE_1, 1<<4, 0x10);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, ASDE_AUTO_EE1_EFFECT_START, 0x0A<<4 | 0x05<<0);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, ASDE_AUTO_EE2_EFFECT_START, 0x0A<<4 | 0x08<<0);
    if (ret != AW_OK) {
        return ret;
    }

    //contrast
    ret = __i2c_reg_write(p_this, LUMA_CONTRAST, 0x40);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, CONTRAST_CENTER, 0x80);
    if (ret != AW_OK) {
        return ret;
    }

    //saturation
    ret = __i2c_reg_write(p_this, GLOBAL_STATURATION, 0x40);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, SATURATION_CB, 0x20);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, SATURATION_CR, 0x20);
    if (ret != AW_OK) {
        return ret;
    }

    //special effect
    ret = __i2c_reg_set(p_this, SPECIAL_EFFECT, 0<<0|0<<1, 0x03);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, FIXED_CB, 0x00);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_write(p_this, FIXED_CR, 0x00);
    if (ret != AW_OK) {
        return ret;
    }

    //init config
    tmp = 0;
    if (p_devinfo->control_flags & AW_CAM_DATA_LATCH_ON_RISING_EDGE) {
        tmp |= 0x04;
    }
    if (p_devinfo->control_flags & AW_CAM_HREF_ACTIVE_HIGH) {
        tmp |= 0x02;
    }
    if (p_devinfo->control_flags & AW_CAM_VSYNC_ACTIVE_HIGH) {
        tmp |= 0x01;
    }
    ret = __i2c_reg_set(p_this, SYNC_MODE, tmp, 0x07);
    if (ret != AW_OK) {
        return ret;
    }

    ret = __i2c_reg_write(p_this, PAD_DRV, 0x15);
    if (ret != AW_OK) {
        return ret;
    }
    return AW_OK;
}

#if __GC0308_DEBUG
aw_local void __gc0308_reg_get(void *p_cookie)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    uint8_t tmp;
    uint32_t value;

    __i2c_reg_read(p_this, COLUMNSTART_H, &tmp);
    value = (uint16_t)tmp<<8;
    __i2c_reg_read(p_this, COLUMNSTART_L, &tmp);
    value |= tmp;
    aw_kprintf("col start is %u\n\r", value);
    __i2c_reg_read(p_this, ROWSTART_H, &tmp);
    value = (uint16_t)tmp<<8;
    __i2c_reg_read(p_this, ROWSTART_L, &tmp);
    value |= tmp;
    aw_kprintf("row start is %u\n\r", value);
    __i2c_reg_read(p_this, WINDOWWIDTH_H, &tmp);
    value = (uint16_t)tmp<<8;
    __i2c_reg_read(p_this, WINDOWWIDTH_L, &tmp);
    value |= tmp;
    aw_kprintf("width is %u\n\r", value);
    __i2c_reg_read(p_this, WINDOWHEIGH_H, &tmp);
    value = (uint16_t)tmp<<8;
    __i2c_reg_read(p_this, WINDOWHEIGH_L, &tmp);
    value |= tmp;
    aw_kprintf("heigh is %u\n\r", value);
    aw_kprintf("OUTPUT_FORMAT is %02X\n\r", tmp);
    __i2c_reg_read(p_this, SUBSAMPLE, &tmp);
    aw_kprintf("subsample row:%d col:%\n\r", tmp>>4, tmp&0x03);
    __i2c_reg_read(p_this, EXP_LEVEL_1_H, &tmp);
    value = (uint16_t)tmp<<8;
    __i2c_reg_read(p_this, EXP_LEVEL_1_L, &tmp);
    value |= tmp;
    aw_kprintf("level 1 is %u\n\r", value);
    __i2c_reg_read(p_this, EXP_LEVEL_2_H, &tmp);
    value = (uint16_t)tmp<<8;
    __i2c_reg_read(p_this, EXP_LEVEL_2_L, &tmp);
    value |= tmp;
    aw_kprintf("level 2 is %u\n\r", value);
    __i2c_reg_read(p_this, EXP_LEVEL_3_H, &tmp);
    value = (uint16_t)tmp<<8;
    __i2c_reg_read(p_this, EXP_LEVEL_3_L, &tmp);
    value |= tmp;
    aw_kprintf("level 3 is %u\n\r", value);
    __i2c_reg_read(p_this, EXP_LEVEL_4_H, &tmp);
    value = (uint16_t)tmp<<8;
    __i2c_reg_read(p_this, EXP_LEVEL_4_L, &tmp);
    value |= tmp;
    aw_kprintf("level 4 is %u\n\r", value);

    __i2c_reg_read(p_this, SH_DELAY, &tmp);
    aw_kprintf("SH_DELAY is %02X\n\r", tmp);
    __i2c_reg_read(p_this, VB_HB, &tmp);
    value = (uint16_t)(tmp &0x0F)<<8;
    __i2c_reg_read(p_this, HB, &tmp);
    value |= tmp;
    aw_kprintf("HB is %u\n\r", value);
    __i2c_reg_read(p_this, VB_HB, &tmp);
    value = (uint16_t)(tmp &0xF0)<<4;
    __i2c_reg_read(p_this, VB, &tmp);
    value |= tmp;
    aw_kprintf("VB is %u\n\r", value);
    __i2c_reg_read(p_this, ANTI_FLICKER_STEP_H, &tmp);
    value = (uint16_t)tmp<<8;
    __i2c_reg_read(p_this, ANTI_FLICKER_STEP_L, &tmp);
    value |= tmp;
    aw_kprintf("STEP is %u\n\r", value);
    __i2c_reg_read(p_this, VS_ST, &tmp);
    aw_kprintf("ST is %02X\n\r", tmp);
    __i2c_reg_read(p_this, VS_ET, &tmp);
    aw_kprintf("ET is %02X\n\r", tmp);
    __i2c_reg_read(p_this, MAX_EXP_LEVEL, &tmp);
    aw_kprintf("MAX_EXP_LEVEL is %02X\n\r", tmp);

    __i2c_reg_read(p_this, EXPOSURE_H, &tmp);
    value = (uint16_t)(tmp & 0x0F) << 8;
    __i2c_reg_read(p_this, EXPOSURE_L, &tmp);
    value |= tmp;
    aw_kprintf("exposure is %u\n\r", value);
//    __i2c_reg_read(p_this, , &tmp);
}
#endif

aw_local aw_err_t __gc0308_aec_set(void *p_cookie, aw_camera_cfg_t *p_cfg)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;

    if (p_cfg->aec_enable == AW_CAMERA_OPT_DISABLE) {
        ret = __i2c_reg_set(p_this, AEC_MODE3, 0x00, 0x80);
        if (ret != AW_OK) {
            aw_kprintf("GC0308 AEC configure error!\n\r");
            return ret;
        }
        ret = __gc0308_exposure_time_set(p_this, p_cfg->exposure);
        if (ret != AW_OK) {
            aw_kprintf("GC0308 AEC configure error!\n\r");
            return ret;
        }
    }
    else {
        ret = __i2c_reg_set(p_this, AEC_MODE3, 0x80, 0x80);
        if (ret != AW_OK) {
            aw_kprintf("GC0308 AEC configure error!\n\r");
            return ret;
        }
        ret = __i2c_reg_write(p_this, AEC_TARGET_Y, p_cfg->brightness);
        if (ret != AW_OK) {
            aw_kprintf("GC0308 AEC configure error!\n\r");
            return ret;
        }
    }

    return AW_OK;
}

aw_local aw_err_t __gc0308_aec_get(void *p_cookie, aw_camera_cfg_t *p_cfg)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;
    uint8_t tmp;

    ret = __i2c_reg_read(p_this, AEC_MODE3, &tmp);
    if (ret != AW_OK) {
        return ret;
    }
    if (tmp & 0x80) {
        p_cfg->aec_enable = AW_CAMERA_OPT_ENABLE;
    } else {
        p_cfg->aec_enable = AW_CAMERA_OPT_DISABLE;
    }

    p_cfg->exposure = 0;
    p_cfg->brightness = 0;

    ret = __gc0308_exposure_time_get(p_this, (uint16_t *)&p_cfg->exposure);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __i2c_reg_read(p_this, AEC_TARGET_Y, (uint8_t *)&p_cfg->brightness);
    if (ret != AW_OK) {
        return ret;
    }

    return AW_OK;
}

aw_local aw_err_t __gc0308_awb_set(void *p_cookie, aw_camera_cfg_t *p_cfg)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;

    if (p_cfg->awb_enable == AW_CAMERA_OPT_DISABLE) {
        __gc0308_page_choose(p_this, 0);
        ret = __i2c_reg_set(p_this, AAAA_ENABLE, 0x00, 0x02);
        if (ret != AW_OK) {
            aw_kprintf("GC0308 AWB configure error!\n\r");
            return ret;
        }
        __i2c_reg_write(p_this, AWB_R_GAIN, (uint8_t)p_cfg->red_gain);
        __i2c_reg_write(p_this, AWB_G_GAIN, (uint8_t)p_cfg->green_gain);
        __i2c_reg_write(p_this, AWB_B_GAIN, (uint8_t)p_cfg->blue_gain);
    } else {
        __gc0308_page_choose(p_this, 0);
        ret = __i2c_reg_set(p_this, AAAA_ENABLE, 0x02, 0x02);
        if (ret != AW_OK) {
            aw_kprintf("GC0308 AWB configure error!\n\r");
            return ret;
        }
    }

    return AW_OK;
}

aw_local aw_err_t __gc0308_awb_get(void *p_cookie, aw_camera_cfg_t *p_cfg)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;
    uint8_t tmp;

    ret = __gc0308_page_choose(p_this, 0);
    if (ret != AW_OK) {
        aw_kprintf("GC0308 AWB configure error!\n\r");
        return ret;
    }
    __i2c_reg_read(p_this, AAAA_ENABLE, &tmp);
    p_cfg->awb_enable = (tmp & 0x02) ? AW_CAMERA_OPT_ENABLE : AW_CAMERA_OPT_DISABLE;
    p_cfg->red_gain = 0;
    p_cfg->green_gain = 0;
    p_cfg->blue_gain = 0;
    __i2c_reg_read(p_this, AWB_R_GAIN, (uint8_t *)&p_cfg->red_gain);
    __i2c_reg_read(p_this, AWB_G_GAIN, (uint8_t *)&p_cfg->green_gain);
    __i2c_reg_read(p_this, AWB_B_GAIN, (uint8_t *)&p_cfg->blue_gain);

    return AW_OK;
}

aw_local aw_err_t __gc0308_conversion_set(void *p_cookie, aw_camera_cfg_t *p_cfg)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    uint8_t tmp = 0;

    if (p_cfg->conversion & MIRROR) {
        tmp |= 0x01;
    }
    if (p_cfg->conversion & UPSIDE_DOWN) {
        tmp |= 0x02;
    }

    return __i2c_reg_set(p_this, CISCTL_MODE1, tmp, 0x03);
}

aw_local aw_err_t __gc0308_conversion_get(void *p_cookie, aw_camera_cfg_t *p_cfg)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;
    uint8_t tmp = 0;

    ret = __gc0308_page_choose(p_this, 0);
    if (ret != AW_OK) {
        aw_kprintf("GC0308 AWB configure error!\n\r");
        return ret;
    }
    __i2c_reg_read(p_this, CISCTL_MODE1, &tmp);

    p_cfg->conversion = 0;
    p_cfg->conversion |= (tmp & 0x01) ? MIRROR :0;
    p_cfg->conversion |= (tmp & 0x02) ? UPSIDE_DOWN :0;

    return AW_OK;
}

aw_local aw_err_t __gc0308_window_fix(void *p_cookie, __window_t win,
                                       uint32_t photo_resolution,
                                       uint32_t video_resolution)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    uint16_t photo_width  = VIDEO_EXTRACT_WIDTH(photo_resolution);
    uint16_t photo_height = VIDEO_EXTRACT_HEIGHT(photo_resolution);
    uint16_t video_width  = VIDEO_EXTRACT_WIDTH(video_resolution);
    uint16_t video_height = VIDEO_EXTRACT_HEIGHT(video_resolution);

    if ((win.width < photo_width) && (win.width < video_width) &&\
        (win.height < photo_height) && (win.height < video_height)) {
        return -AW_EPERM;
    }

    __gc0308_center_count(&win);
    return __gc0308_window_set(p_this, &win);
}

aw_local aw_err_t __gc0308_resolution_set(void *p_cookie, uint32_t resolution)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;
    int i;
    __window_t win;
    __subsample_t subsample;
    uint16_t width,height;

    width  = VIDEO_EXTRACT_WIDTH(resolution);
    height = VIDEO_EXTRACT_HEIGHT(resolution);

    for (i=0;i<4;i++) {
        subsample.col[i] = 0;
        subsample.row[i] = 0;
    }

    if (__STATUS_IS(p_this->status, __FIX_WIN_STATE)) {
        ret = __gc0308_window_get(p_this, &win);
        if (ret != AW_OK) {
            return ret;
        }

        if ((win.width % width) && (win.height % height)) {
            return -AW_EINVAL;
        }

        subsample.col_ratio = win.width / width;
        subsample.row_ratio = win.height / height;
        if ((subsample.col_ratio > 7) || (subsample.row_ratio > 7)) {
            return -AW_EINVAL;
        }

        if (p_this->cfg.bytes_per_pix == 1) {
            if ((subsample.col_ratio > 2) || (subsample.row_ratio > 2)) {
                return -AW_EINVAL;
            }
            if ((subsample.col_ratio == 2) ) {
                subsample.col_ratio = 4;
                subsample.col[1] = 1;
            }
            if ((subsample.row_ratio == 2) ) {
                subsample.row_ratio = 4;
                subsample.row[1] = 1;
            }
        }

    } else {
        subsample.col_ratio = 1;
        subsample.row_ratio = 1;

        win.height = height;
        win.width  = width;
        __gc0308_center_count(&win);
        ret = __gc0308_window_set(p_this, &win);
        if (ret != AW_OK) {
            return ret;
        }
        ret = __gc0308_frame_rate_set(p_this);
        if (ret != AW_OK) {
            return ret;
        }
    }


    return __gc0308_subsample_set(p_this, &subsample);
}

/*****************************************************************************/


/*********************************INIT SETTING********************************/

/**
 * \brief find out a service who accept the pin
 */
aw_local struct awbl_camera_controller_service * __camera_controller_to_serv (void *p_cookie)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    __GC0308_DEVINFO_DECL(p_devinfo, p_cookie);
    struct awbl_camera_controller_service *p_serv_cur = p_this->p_controller_serv;

    /* todo: if only one service, just return it directly */

    while ((p_serv_cur != NULL)) {

        if (p_devinfo->camera_data_bus_id == p_serv_cur->id) {

            return p_serv_cur;
        }
        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}


/**  relative configuration of initialize interface
 *  connect CSI,allocate memory,configure IO...
 */

AWBL_METHOD_IMPORT(awbl_cameracontrollerserv_get);

aw_local aw_err_t __camera_controller_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t      pfunc_camera_controller_serv = NULL;
    struct awbl_camera_service  *p_camera_serv     = NULL;

    /** find handler */

    pfunc_camera_controller_serv = awbl_dev_method_get(p_dev,
                                          AWBL_METHOD_CALL(awbl_cameracontrollerserv_get));

    /** call method handler to allocate camera service */

    if (pfunc_camera_controller_serv != NULL) {

        pfunc_camera_controller_serv(p_dev, &p_camera_serv);

        /** found a camera service, insert it to the service list */

        if (p_camera_serv != NULL) {
            struct awbl_camera_service **pp_serv_cur = (struct awbl_camera_service **)p_arg;

            while (*pp_serv_cur != NULL) {
                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }

            *pp_serv_cur = p_camera_serv;
             p_camera_serv->p_next = NULL;
        }
    }

    return AW_OK;
}

/**
 * \brief 获取gc0308对应csi的子方法
 */
aw_local void __gc0308_sub_method_init(void *p_cookie)
{
    /* 初始化CSI */
    __GC0308_DEV_DECL(p_this, p_cookie);

    p_this->p_controller_serv = NULL;

    /* 在打开中枚举相应的服务  */
    awbl_dev_iterate(__camera_controller_serv_alloc_helper, &p_this->p_controller_serv, AWBL_ITERATE_INSTANCES);

}


aw_local aw_err_t __gc0308_video_mem_reset(void *p_cookie, uint32_t video_resolution, uint8_t pix)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    __GC0308_DEVINFO_DECL(p_devinfo, p_cookie);
    uint32_t mem_size;

    /* 如果修改的分辨率与原分辨率一致，则返回 */
    if ((video_resolution == p_this->video_resolution)&&
            (pix == p_this->cfg.bytes_per_pix)) {
        return AW_OK;
    }

    p_this->video_resolution = video_resolution;

    if (p_this->cfg.p_video_buf_addr != NULL) {
        aw_mem_free(p_this->cfg.p_video_buf_addr);
    }

    mem_size = PHOTO_MEM_SIZE(p_this->cfg.bytes_per_pix,
                              VIDEO_SIZE(p_this->video_resolution),
                              p_devinfo->buf_num);

    if (mem_size == 0) {
        p_this->cfg.p_video_buf_addr = NULL;
        __STATUS_CLR(p_this->status, __VIDEO_MODE_STATE);
    } else {
        p_this->cfg.p_video_buf_addr =
                        (uint32_t *)aw_mem_align(mem_size, AW_CACHE_LINE_SIZE);
        if (p_this->cfg.p_video_buf_addr == NULL) {
            return -AW_ENOMEM;
        }
        __STATUS_SET(p_this->status, __VIDEO_MODE_STATE);
    }

    return AW_OK;
}

aw_local aw_err_t __gc0308_photo_mem_reset(void *p_cookie, uint32_t photo_resolution, uint8_t pix)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    uint32_t mem_size;

    /* 如果修改的分辨率与原分辨率一致，则返回 */
    if ((photo_resolution == p_this->photo_resolution) &&
            (pix == p_this->cfg.bytes_per_pix)){
        return AW_OK;
    }

    p_this->photo_resolution = photo_resolution;

    if (p_this->cfg.p_photo_buf_addr != NULL) {
        aw_mem_free(p_this->cfg.p_photo_buf_addr);
    }

    mem_size = PHOTO_MEM_SIZE(p_this->cfg.bytes_per_pix,
                              PHOTO_SIZE(p_this->photo_resolution),
                              2);

    if (mem_size == 0) {
        p_this->cfg.p_photo_buf_addr = NULL;
        __STATUS_CLR(p_this->status, __PHOTO_MODE_STATE);
    } else {
        p_this->cfg.p_photo_buf_addr =
                        (uint32_t *)aw_mem_align(mem_size, AW_CACHE_LINE_SIZE);
        if (p_this->cfg.p_photo_buf_addr == NULL) {
            return -AW_ENOMEM;
        }
        __STATUS_SET(p_this->status, __PHOTO_MODE_STATE);
    }

    return AW_OK;
}


aw_local aw_err_t __gc0308_restart(void *p_cookie,
                                   uint32_t video_resolution,
                                   uint32_t photo_resolution,
                                   uint8_t  pix_byte)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    int ret = AW_OK;

    if (!__STATUS_IS(p_this->status, __STANDBY_STATE)) {
        __gc0308_standby(p_this);
        ret = p_this->p_controller_serv->p_servfuncs->close(p_this->p_controller_serv->p_cookie);

    }

    ret = __gc0308_video_mem_reset(p_this, video_resolution, pix_byte);
    if (ret != AW_OK) {
        return ret;
    }

    ret = __gc0308_photo_mem_reset(p_this, photo_resolution, pix_byte);
    if (ret != AW_OK) {
        return ret;
    }
    p_this->cfg.bytes_per_pix = pix_byte;

    if (!__STATUS_IS(p_this->status, __STANDBY_STATE)) {
        if (__STATUS_IS(p_this->status, __VIDEO_RUNNING_STATE)) {
            p_this->cfg.resolution   = p_this->video_resolution;
            p_this->cfg.current_mode = VIDEO;
            ret = __gc0308_resolution_set(p_this, p_this->photo_resolution);
            if (ret != AW_OK) {
                AW_MUTEX_UNLOCK(p_this->dev_lock);
                return ret;
            }
            ret = p_this->p_controller_serv->p_servfuncs->open(p_this->p_controller_serv->p_cookie, &p_this->cfg);
        } else if (__STATUS_IS(p_this->status, __PHOTO_RUNNING_STATE)) {
            p_this->cfg.resolution   = p_this->photo_resolution;
            p_this->cfg.current_mode = PHOTO;
            ret = __gc0308_resolution_set(p_this, p_this->photo_resolution);
            if (ret != AW_OK) {
                AW_MUTEX_UNLOCK(p_this->dev_lock);
                return ret;
            }
            ret = p_this->p_controller_serv->p_servfuncs->open(p_this->p_controller_serv->p_cookie, &p_this->cfg);
        }
        __gc0308_startup(p_this);
    }

    return ret;
}

aw_local aw_err_t __mem_init (void *p_cookie)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    __GC0308_DEVINFO_DECL(p_devinfo, p_cookie);

    p_this->cfg.p_photo_buf_addr = NULL;
    p_this->cfg.p_photo_buf_addr = NULL;

    p_this->cfg.buf_num = p_devinfo->buf_num;
    p_this->cfg.p_buff_nodes_addr = (uint32_t *)aw_mem_alloc(sizeof(buffer_node_t) * \
                                                             p_devinfo->buf_num);
    if (p_this->cfg.p_buff_nodes_addr == 0) {
        return -AW_ENOMEM;
    }

    return __gc0308_restart(p_this,
                            p_devinfo->video_resolution,
                            p_devinfo->photo_resolution,
                            p_this->cfg.bytes_per_pix);
}

aw_local aw_err_t __gc0308_init(void *p_cookie)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    __GC0308_DEVINFO_DECL(p_devinfo, p_cookie);
    aw_err_t ret;
    __format_t format;
    int i;
    int size;
    __window_t win;

    __gc0308_page_choose(p_this, 0);

    ret = gc0308_check_id(p_this);
    if (ret != AW_OK) {
        return ret;
    }

    size = sizeof(gc0308_init_list)/sizeof(gc0308_reg_t);
    for (i=0; i<size; i++) {
        ret = __i2c_reg_write(p_this, gc0308_init_list[i].reg, gc0308_init_list[i].val);
        if (ret != AW_OK) {
            return ret;
        }
    }

    win.height = VIDEO_EXTRACT_HEIGHT(p_devinfo->video_resolution);
    win.width = VIDEO_EXTRACT_WIDTH(p_devinfo->video_resolution);
    __gc0308_center_count(&win);

    __gc0308_window_set(p_this, &win);

    win.col_start += 10;
    win.row_start += 10;
    win.height -= 20;
    win.width -= 20;
    __gc0308_measure_window_set(p_this, &win);

    __gc0308_frame_rate_set(p_this);

    switch (p_devinfo->pix_format) {
    case RGB565:
        format = __RGB565;
        break;
    case RGB555:
        format = __RGBx555;
        break;
    case RGB444:
        format = __RGBx444;
        break;
    case YUV422:
        format = __CbYCrY;
        break;
    case BAYER_RAW:
        format = __bayer_pattern;
        break;
    default:
        return -AW_EPERM;
    }
    __gc0308_format_set(p_this, format);

    __gc0308_init_set(p_this);

    __gc0308_standby(p_this);

#if __GC0308_DEBUG
    __gc0308_reg_get(p_this);
#endif

    return AW_OK;
}

aw_local void __gc0308_inst_init1(awbl_dev_t *p_dev) {}

aw_local void __gc0308_inst_init2(awbl_dev_t *p_dev)
{
    __GC0308_DEVINFO_DECL(p_devinfo, p_dev);
    p_devinfo->pfunc_plfm_init();
}

aw_local void __gc0308_inst_connect(awbl_dev_t *p_dev)
{
    __GC0308_DEV_DECL(p_this, p_dev);
    __GC0308_DEVINFO_DECL(p_devinfo, p_dev);
    aw_err_t ret;

    AW_MUTEX_INIT(p_this->dev_lock, AW_PSP_SEM_Q_FIFO);


    __i2c_init(p_this);

    /* 当前摄像头没有被打开  */
    p_this->status = 0;

    p_this->photo_resolution = 0;
    p_this->video_resolution = 0;

    p_this->cfg.bytes_per_pix  = 2;
    if (p_devinfo->pix_format == BAYER_RAW) {
        p_this->cfg.bytes_per_pix = 1;
    }

    p_this->cfg.control_flags = p_devinfo->control_flags;
    p_this->cfg.resolution = 0;

    __STATUS_SET(p_this->status, __STANDBY_STATE);
    ret = __mem_init(p_this);
    if (ret != AW_OK) {
        aw_kprintf("gc0308 memory init failed: %d\n\r", ret);
        return;
    }

    ret = __gc0308_init(p_this);
    if (ret != AW_OK) {
        aw_kprintf("gc0308 init failed: %d\n\r", ret);
        return;
    }

    __gc0308_sub_method_init(p_this);

}

/*****************************************************************************/


/***************************APPLICATION INTERFACE*****************************/

aw_local aw_err_t __camera_open(void *p_cookie)
{
    int ret = AW_OK;
    __GC0308_DEV_DECL(p_this, p_cookie);

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

    p_this->p_controller_serv = __camera_controller_to_serv(p_cookie);
    if (p_this->p_controller_serv == NULL) {
        return -AW_ENXIO;
    }

    ret = gc0308_check_id(p_this);
    if (ret !=  AW_OK) {
        AW_MUTEX_UNLOCK(p_this->dev_lock);
        return ret;
    }
    __STATUS_CLR(p_this->status, __STANDBY_STATE);
    __STATUS_CLR(p_this->status, __VIDEO_RUNNING_STATE | __PHOTO_RUNNING_STATE);

    AW_MUTEX_UNLOCK(p_this->dev_lock);
    return AW_OK;
}

aw_local aw_err_t __camera_close(void *p_cookie)
{
    aw_err_t ret = 0;
    __GC0308_DEV_DECL(p_this, p_cookie);

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

    if (__STATUS_IS(p_this->status, __PHOTO_RUNNING_STATE) ||
            __STATUS_IS(p_this->status, __VIDEO_RUNNING_STATE)) {
        ret = p_this->p_controller_serv->p_servfuncs->close(p_this->p_controller_serv->p_cookie);
    }
    __gc0308_standby(p_this);
    __STATUS_SET(p_this->status, __STANDBY_STATE);

    AW_MUTEX_UNLOCK(p_this->dev_lock);
    return ret;
}


aw_local aw_err_t __camera_get_photo_buf(void *p_cookie, uint32_t **p_buf)
{
    __GC0308_DEV_DECL(p_this, p_cookie);

    uint32_t ret = 0;
    int i,j;
    uint16_t width, height;
    uint8_t tmp, *buf;

    if (!__STATUS_IS(p_this->status, __PHOTO_MODE_STATE)||
            __STATUS_IS(p_this->status, __STANDBY_STATE)) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

    /* 如果照片地址没有被释放  */
    if (__STATUS_IS(p_this->status, __PHOTO_SEAL_STATE)) {
        *p_buf = NULL;
        AW_MUTEX_UNLOCK(p_this->dev_lock);
        return -AW_EIO;
    }

    if (!__STATUS_IS(p_this->status, __PHOTO_RUNNING_STATE)) {

        if (p_this->photo_resolution != p_this->cfg.resolution) {
            ret = __gc0308_resolution_set(p_this, p_this->photo_resolution);
            if (ret != AW_OK) {
                AW_MUTEX_UNLOCK(p_this->dev_lock);
                return ret;
            }
        }

#if __GC0308_DEBUG
        __gc0308_reg_get(p_this);
#endif
        /* 打开默认是预览的分辨率 */
        p_this->cfg.resolution = p_this->photo_resolution;
        p_this->cfg.current_mode = PHOTO;
        __STATUS_SET(p_this->status, __PHOTO_RUNNING_STATE);
        __STATUS_CLR(p_this->status, __VIDEO_RUNNING_STATE);

        /* 以拍照模式打开摄像头 */
        ret = p_this->p_controller_serv->p_servfuncs->open(p_this->p_controller_serv->p_cookie, &p_this->cfg);
        if (ret != AW_OK) {
            AW_MUTEX_UNLOCK(p_this->dev_lock);
            return ret;
        }

        __gc0308_startup(p_this);
    }

    /* 等待拍照完成 */
    ret = p_this->p_controller_serv->p_servfuncs->get_photo_buf(p_this->p_controller_serv->p_cookie, p_buf);
    if (ret != AW_OK) {
        AW_MUTEX_UNLOCK(p_this->dev_lock);
        return ret;
    }

    /* 获取到一帧图片,标记为没有被释放  */
    __STATUS_SET(p_this->status, __PHOTO_SEAL_STATE);

    width  = VIDEO_EXTRACT_WIDTH(p_this->photo_resolution);
    height = VIDEO_EXTRACT_HEIGHT(p_this->photo_resolution);

    buf = (uint8_t *)*p_buf;

    if (p_this->cfg.bytes_per_pix == 2) {
        for (i=0; i<height; i++) {
            for (j=0; j<width; j++) {
                tmp = *((uint8_t*)buf + width * 2 * i + j * 2);
                *((uint8_t*)buf + width * 2 * i + j *2) = *((uint8_t*)buf + width * 2 * i + j *2 + 1);
                *((uint8_t*)buf + width * 2 * i + j *2 + 1) = tmp;
            }
        }
    }

    AW_MUTEX_UNLOCK(p_this->dev_lock);

    return AW_OK;
}

aw_local aw_err_t __camera_release_photo_buf(void *p_cookie, uint32_t addr)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    __GC0308_DEVINFO_DECL(p_devinfo, p_cookie);
    int ret = AW_OK;

    if (!__STATUS_IS(p_this->status, __PHOTO_MODE_STATE)||
            __STATUS_IS(p_this->status, __STANDBY_STATE)) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

    /* 释放photo的地址  */
    ret = p_this->p_controller_serv->p_servfuncs->release_photo_buf(p_this->p_controller_serv->p_cookie, addr);
    __STATUS_CLR(p_this->status, __PHOTO_SEAL_STATE);

    AW_MUTEX_UNLOCK(p_this->dev_lock);
    return ret;
}

aw_local aw_err_t __camera_get_video_buf(void *p_cookie, uint32_t **p_buf)
{
    aw_err_t ret = 0;
    int i,j;
    uint16_t width, height;
    uint8_t tmp, *buf;

    __GC0308_DEV_DECL(p_this, p_cookie);
    __GC0308_DEVINFO_DECL(p_devinfo, p_cookie);

    if (!__STATUS_IS(p_this->status, __VIDEO_MODE_STATE)||
            __STATUS_IS(p_this->status, __STANDBY_STATE)) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);


    if (!__STATUS_IS(p_this->status, __VIDEO_RUNNING_STATE)) {

        if (p_this->video_resolution != p_this->cfg.resolution) {
            ret = __gc0308_resolution_set(p_this, p_this->video_resolution);
            if (ret != AW_OK) {
                AW_MUTEX_UNLOCK(p_this->dev_lock);
                return ret;
            }
        }


#if __GC0308_DEBUG
        __gc0308_reg_get(p_this);
#endif
        /* 打开默认是预览的分辨率 */
        p_this->cfg.resolution = p_this->video_resolution;
        p_this->cfg.current_mode = VIDEO;
        __STATUS_SET(p_this->status, __VIDEO_RUNNING_STATE);
        __STATUS_CLR(p_this->status, __PHOTO_RUNNING_STATE);

        /* 以拍照模式打开摄像头 */
        ret = p_this->p_controller_serv->p_servfuncs->open(p_this->p_controller_serv->p_cookie, &p_this->cfg);
        if (ret != AW_OK) {
            AW_MUTEX_UNLOCK(p_this->dev_lock);
            return ret;
        }
        __gc0308_startup(p_this);
    }

    ret = p_this->p_controller_serv->p_servfuncs->get_video_buf(p_this->p_controller_serv->p_cookie, p_buf);

    width  = VIDEO_EXTRACT_WIDTH(p_this->video_resolution);
    height = VIDEO_EXTRACT_HEIGHT(p_this->video_resolution);

    buf = (uint8_t *)*p_buf;

    if (p_this->cfg.bytes_per_pix == 2) {
        for (i=0; i<height; i++) {
            for (j=0; j<width; j++) {
                tmp = *((uint8_t*)buf + width * 2 * i + j *2);
                *((uint8_t*)buf + width * 2 * i + j *2) = *((uint8_t*)buf + width * 2 * i + j *2 + 1);
                *((uint8_t*)buf + width * 2 * i + j *2 + 1) = tmp;
            }
        }
    }

    AW_MUTEX_UNLOCK(p_this->dev_lock);
    return ret;
}

aw_local aw_err_t __camera_release_video_buf(void *p_cookie, uint32_t  addr)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    __GC0308_DEVINFO_DECL(p_devinfo, p_cookie);
    int ret = AW_OK;

    if (!__STATUS_IS(p_this->status, __VIDEO_MODE_STATE)||
            __STATUS_IS(p_this->status, __STANDBY_STATE)) {
        return -AW_ENOTSUP;
    }

    if (!__STATUS_IS(p_this->status, __VIDEO_MODE_STATE)||
            __STATUS_IS(p_this->status, __STANDBY_STATE)) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);
    ret = p_this->p_controller_serv->p_servfuncs->release_video_buf(p_this->p_controller_serv->p_cookie, addr);
    AW_MUTEX_UNLOCK(p_this->dev_lock);

    return ret;
}

aw_local aw_err_t __camera_config_set(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    __window_t win;
    int ret = AW_OK;
    uint8_t  pix_byte;
    uint8_t  mem_reset = 0;
    uint32_t restart   = 0;
    uint32_t video_resolution, photo_resolution;

    pix_byte = p_this->cfg.bytes_per_pix;
    video_resolution = p_this->video_resolution;
    photo_resolution = p_this->photo_resolution;

    /* 摄像头支持的配置 */
    *option &= AW_CAMERA_RESOLUTION|AW_CAMERA_WINDOW|AW_CAMERA_EXPOSURE|\
               AW_CAMERA_CONVERSION|AW_CAMERA_FORMAT|AW_CAMERA_GAIN;

    if (p_this == NULL) {
        return -AW_ENXIO;
    }

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

    ret = __gc0308_page_choose(p_this, 0);
    if (ret != AW_OK) {
        return ret;
    }

    if (*option & AW_CAMERA_EXPOSURE) {
        if (__gc0308_aec_set(p_this, p_cfg) != AW_OK) {
            *option &= (~AW_CAMERA_EXPOSURE);
        }
    }

    if (*option & AW_CAMERA_GAIN) {
        if (__gc0308_awb_set(p_this, p_cfg) != AW_OK) {
            *option &= (~AW_CAMERA_GAIN);
        }
    }

    if (*option & AW_CAMERA_CONVERSION) {
        if (__gc0308_conversion_set(p_this, p_cfg) != AW_OK) {
            *option &= (~AW_CAMERA_CONVERSION);
        }
    }

    if(*option & AW_CAMERA_FORMAT) {
        __format_t format=__RGB565;
        pix_byte = 2;
        switch (p_cfg->format) {
        case RGB565: format = __RGB565;  break;
        case RGB555: format = __RGBx555; break;
        case RGB444: format = __RGBx444; break;
        case YUV422: format = __CbYCrY;  break;
        case BAYER_RAW:
            format = __bayer_pattern;
            pix_byte = 1;
            break;
        default:
            *option &= (~AW_CAMERA_FORMAT);
            break;
        }
        if (__gc0308_format_set(p_this, format) != AW_OK) {
            *option &= (~AW_CAMERA_FORMAT);
        } else {
            if (pix_byte != p_this->cfg.bytes_per_pix) {
                mem_reset |= 1;
                restart |= 1;
            }
        }
    }

    if (*option & AW_CAMERA_WINDOW) {
        if (p_cfg->win_fix == AW_CAMERA_OPT_ENABLE) {
            win.height = p_cfg->win_height;
            win.width = p_cfg->win_width;
            if (*option & AW_CAMERA_RESOLUTION) {
                ret = __gc0308_window_fix(p_this, win,
                                           p_cfg->photo_resolution,
                                           p_cfg->video_resolution);
                if (ret != AW_OK) {
                    *option &= (~AW_CAMERA_RESOLUTION);
                    ret = __gc0308_window_fix(p_this, win,
                                               p_this->photo_resolution,
                                               p_this->video_resolution);
                    if (ret != AW_OK) {
                        *option &= (~AW_CAMERA_WINDOW);
                    }
                }
            }
            __gc0308_frame_rate_set(p_this);
            __STATUS_SET(p_this->status, __FIX_WIN_STATE);
        } else {
            __STATUS_CLR(p_this->status, __FIX_WIN_STATE);
        }
    }

    if (*option & AW_CAMERA_RESOLUTION) {
        photo_resolution    = p_cfg->photo_resolution;
        video_resolution    = p_cfg->video_resolution;
        restart |= 1;
        mem_reset |= 1;
    }

    if (restart) {
        __gc0308_restart(p_this, video_resolution, photo_resolution, pix_byte);
    }
    
    AW_MUTEX_UNLOCK(p_this->dev_lock);
    return AW_OK;
}

aw_local aw_err_t __camera_config_get(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option)
{
    __GC0308_DEV_DECL(p_this, p_cookie);
    int ret = AW_OK;

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

    /* 摄像头支持的配置 */
    *option &= AW_CAMERA_RESOLUTION|AW_CAMERA_WINDOW|AW_CAMERA_EXPOSURE|\
               AW_CAMERA_CONVERSION|AW_CAMERA_FORMAT|AW_CAMERA_GAIN;

    if (*option & AW_CAMERA_EXPOSURE) {
        __gc0308_aec_get(p_this, p_cfg);
    }

    if (*option & AW_CAMERA_GAIN) {
        __gc0308_awb_get(p_this, p_cfg);
    }

    if (*option & AW_CAMERA_CONVERSION) {
        __gc0308_conversion_get(p_this, p_cfg);
    }

    if (*option & AW_CAMERA_FORMAT) {
        __format_t format;
        __gc0308_format_get(p_this, &format);
        switch (format) {
        case __CbYCrY:
        case __CrYCbY:
        case __YCbYCr:
        case __YCrYCb:
            p_cfg->format = YUV422; break;
        case __RGB565:
            p_cfg->format = RGB565;  break;
        case __RGBx555:
        case __RGB555x:
            p_cfg->format = RGB555; break;
        case __RGB444x:
        case __RGBx444:
            p_cfg->format = RGB444; break;
        case __DNDD:
        case __LSC:
        case __bayer_pattern:
            p_cfg->format = BAYER_RAW; break;
        default:
            break;
        }
    }

    if (*option & AW_CAMERA_RESOLUTION) {
        p_cfg->photo_resolution = p_this->photo_resolution;
        p_cfg->video_resolution = p_this->video_resolution;
    }

    if (*option & AW_CAMERA_WINDOW) {
        __window_t win;
        __gc0308_window_get(p_this, &win);
        p_cfg->win_width = win.width;
        p_cfg->win_height = win.height;
        if (__STATUS_IS(p_this->status, __FIX_WIN_STATE)) {
            p_cfg->win_fix = AW_CAMERA_OPT_ENABLE;
        } else {
            p_cfg->win_fix = AW_CAMERA_OPT_DISABLE;
        }
    }

    AW_MUTEX_UNLOCK(p_this->dev_lock);
    return ret;
}


/*****************************************************************************/


/*****************************************************************************/

aw_local aw_const struct awbl_camera_servfuncs __g_camera_servfuncs = {

    __camera_open,

    __camera_close,

    __camera_get_photo_buf,

    __camera_get_video_buf,

    __camera_release_photo_buf,

    __camera_release_video_buf,

    __camera_config_set,

    __camera_config_get,

};

/**
 * \brief 获取OV75640服务的函数列表
 *
 * awbl_gc0308serv_get的运行时实现
 * \param   p_dev   AWbus传下来表征当前设备
 * \param   p_arg   用于输出GC0308服务函数列表的指针
 * \return  无
 */
aw_local aw_err_t __gc0308_cameraserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __GC0308_DEV_DECL(p_this, p_dev);
    __GC0308_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_camera_service *p_serv = &p_this->camera_serv;

    /* initialize the gc0308 service instance */
    p_serv->p_next      = NULL;
    p_serv->p_name      = p_devinfo->p_name;
    p_serv->p_servfuncs = &__g_camera_servfuncs;
    p_serv->p_cookie    = (void *)p_this;
    p_serv->flag        = AW_CAMERA_FLAG_ALIVE;
    p_serv->used_count  = 1;

    /* send back the service instance */
    *(struct awbl_camera_service **)p_arg = p_serv;

    return AW_OK;
}

/**
 * \brief GC0308驱动AWbus驱动函数
 */
aw_local aw_const struct awbl_drvfuncs __g_gc0308_drvfuncs = {
    __gc0308_inst_init1,
    __gc0308_inst_init2,
    __gc0308_inst_connect
};

/******************************************************************************/

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_cameraserv_get);


/**
 * \brief GC0308驱动设备专有函数
 */
aw_local aw_const struct awbl_dev_method __g_gc0308_dev_methods[] = {
    AWBL_METHOD(awbl_cameraserv_get, __gc0308_cameraserv_get),
    AWBL_METHOD_END
};


/**
 * \brief GC0308驱动注册信息
 */
aw_local aw_const awbl_plb_drvinfo_t __g_gc0308_drv_registration = {
    {
        AWBL_VER_1,                 /* awb_ver */
        AWBL_BUSID_PLB,             /* bus_id */
        AWBL_GC0308_NAME,           /* p_drvname */
        &__g_gc0308_drvfuncs,       /* p_busfuncs */
        &__g_gc0308_dev_methods[0], /* p_methods */
        NULL                        /* pfunc_drv_probe */
    }
};

/******************************************************************************/
void awbl_gc0308_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_gc0308_drv_registration);
}
