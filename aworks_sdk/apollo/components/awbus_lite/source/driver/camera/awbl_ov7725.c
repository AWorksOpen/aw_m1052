/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#include "Aworks.h"
#include "awbus_lite.h"
#include "aw_sem.h"
#include "awbl_plb.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "string.h"
#include "aw_mem.h"
#include "aw_gpio.h"
#include "aw_i2c.h"
#include "driver/camera/awbl_ov7725.h"
#include "aw_cache.h"
#include "aw_camera.h"

/** \brief OV7725的设备ID */
#define __OV7725_DEV_ID   (0x42 >> 1)

/** \brief OV7725的版本号  */
#define OV7725_REVISION   0x7721U

/** \brief 分辨率的宽高提取  */
#define VIDEO_EXTRACT_WIDTH(resolution) ((uint16_t)((resolution)&0xFFFFU))
#define VIDEO_EXTRACT_HEIGHT(resolution) ((uint16_t)((resolution) >> 16U))


/** \brief declare ov7725 device instance */
#define __OV7725_DEV_DECL(p_this, p_dev) \
    struct awbl_ov7725_dev *p_this = \
        (struct awbl_ov7725_dev *)(p_dev)

/** \brief declare ov7725 device infomation */
#define __OV7725_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_ov7725_devinfo *p_devinfo = \
        (struct awbl_ov7725_devinfo *)AWBL_DEVINFO_GET(p_dev)


#define VIDEO_SIZE(r)       (VIDEO_EXTRACT_WIDTH(r) * \
                             VIDEO_EXTRACT_HEIGHT(r))
#define PHOTO_SIZE(r)       (VIDEO_EXTRACT_WIDTH(r) * \
                             VIDEO_EXTRACT_WIDTH(r))
#define PHOTO_MEM_SIZE(p, n, bn) \
                            (((p)*(n)*(bn)+AW_CACHE_LINE_SIZE-1) \
                             / AW_CACHE_LINE_SIZE * AW_CACHE_LINE_SIZE)

#define __STATUS_SET(x, v)          ((x) |= (v))
#define __STATUS_CLR(x, v)          ((x) &= (~(v)))
#define __STATUS_IS(x, v)           ((x) & (v))

#define __STANDBY_STATE             0x80000000

#define __VIDEO_RUNNING_STATE       0x40000000
#define __PHOTO_RUNNING_STATE       0x20000000
#define __PHOTO_SEAL_STATE          0x10000000

#define __VIDEO_MODE_STATE          0x04000000
#define __PHOTO_MODE_STATE          0x02000000

typedef struct _ov7725_pixel_format_config
{
    pix_format_t fmt;
    uint8_t com7;
    uint8_t dsp_ctrl4;
} ov7725_pixel_format_config_t;

aw_local const ov7725_pixel_format_config_t ov7725_pix_format_conf[] = {
    {.fmt = RGB565,       .com7 = (1 << 2) | (2), .dsp_ctrl4 = 0x48},
    {.fmt = BAYER_RAW,    .com7 =  (3),           .dsp_ctrl4 = 0x4a},
    {.fmt = YUV422,       .com7 = 0x00,           .dsp_ctrl4 = 0x48},
    {.fmt = RGB444,       .com7 = (3 << 2) | (2), .dsp_ctrl4 = 0x48},
    {.fmt = RGB555,       .com7 = (2 << 2) | (2), .dsp_ctrl4 = 0x48},
};

/** \brief 初始化寄存器序列  */
aw_local const ov7725_reg_t __ov7725_init_regs[] = {
    {0x3d, 0x03},
    {0x42, 0x7f},
    {0x4d, 0x09},

    /* DSP */
    {0x64, 0xff},
    {0x65, 0x20},
    {0x66, 0x00},
    {0x0f, 0xc5},
    {0x13, 0xff},

    /* AEC/AGC/AWB */
    {0x63, 0xf0},
    {0x14, 0x11},
    {0x22, 0x98},
    {0x23, 0x03},
    {0x24, 0x40},
    {0x25, 0x30},
    {0x26, 0xa1},
    {0x2b, 0x00},
    {0x6b, 0xaa},
    {0x0d, 0x41},

    /* Sharpness. */
    {0x90, 0x05},
    {0x91, 0x01},
    {0x92, 0x03},
    {0x93, 0x00},

    /* Matrix. */
    {0x94, 0x90},
    {0x95, 0x8a},
    {0x96, 0x06},
    {0x97, 0x0b},
    {0x98, 0x95},
    {0x99, 0xa0},
    {0x9a, 0x1e},

    /* Brightness. */
    {0x9b, 0x08},
    /* Contrast. */
    {0x9c, 0x20},
    /* UV */
    {0x9e, 0x81},
    /* DSE */
    {0xa6, 0x04},

    /* Gamma. */
    {0x7e, 0x0c},
    {0x7f, 0x16},
    {0x80, 0x2a},
    {0x81, 0x4e},
    {0x82, 0x61},
    {0x83, 0x6f},
    {0x84, 0x7b},
    {0x85, 0x86},
    {0x86, 0x8e},
    {0x87, 0x97},
    {0x88, 0xa4},
    {0x89, 0xaf},
    {0x8a, 0xc5},
    {0x8b, 0xd7},
    {0x8c, 0xe8},

};

aw_local void __sccb_init(void *p_cookie);
aw_local aw_err_t __sccb_wr_reg(void *p_cookie, uint8_t reg, uint8_t data);
aw_local aw_err_t __sccb_rd_reg(void *p_cookie, uint8_t reg, uint8_t *data);
aw_local aw_err_t __sccb_modify_reg(void *p_cookie, uint8_t reg, uint8_t mask, uint8_t value);

aw_local aw_err_t __ov7725_soft_rst(void *p_cookie);
aw_local aw_err_t __ov7725_conf_frame_frq(void *p_cookie, resolution_t res);
aw_local aw_err_t __ov7725_set_pix_format(void *p_cookie, pix_format_t pix_format);
aw_local void __ov7725_sub_method_init(void *p_cookie);


/**************************** SCCB TRANSLATE *********************************/

aw_local void __sccb_init(void *p_cookie)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    __OV7725_DEVINFO_DECL(p_devinfo, p_cookie);

    aw_i2c_mkdev(&p_this->i2c_device,
                 p_devinfo->i2c_master_busid,
                 0x21,
                 AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);
}

aw_local aw_err_t __sccb_wr_reg(void *p_cookie, uint8_t reg, uint8_t data)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    uint8_t buf[2] = {reg, data};
    return aw_i2c_raw_write(&p_this->i2c_device, buf, 2);
}

aw_local aw_err_t __sccb_rd_reg(void *p_cookie, uint8_t reg, uint8_t *data)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    aw_i2c_raw_write(&p_this->i2c_device, &reg, 1);
    return aw_i2c_raw_read(&p_this->i2c_device, data, 1);
}

aw_local aw_err_t __sccb_modify_reg(void *p_cookie, uint8_t reg, uint8_t mask, uint8_t value)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    aw_err_t ret;
    uint8_t tmp = 0, regVal = 0;

    ret = __sccb_rd_reg(p_this, reg, &tmp);
    if (ret != AW_OK) {
        return ret;
    }

    regVal = (tmp & ~((uint32_t)mask)) | (value & mask);

    return __sccb_wr_reg(p_this, reg, regVal);
}


/*****************************************************************************/


/******************************GC0308 SETTING*********************************/

/**
 * \brief OV7725初始化序列
 */
aw_local aw_err_t __ov7725_init_squen(void *p_cookie)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    int i = 0, ret = AW_OK;

    for (i = 0; i < AW_NELEMENTS(__ov7725_init_regs); i++) {

        ret = __sccb_wr_reg(p_this, __ov7725_init_regs[i].reg, __ov7725_init_regs[i].val);
        if (ret != AW_OK) {
            return ret;
        }
    }
    return AW_OK;
}


/**
 * \brief OV7725检测pid
 */
aw_local aw_err_t __ov7725_check_pid(void *p_cookie)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    int ret = AW_OK;
    uint8_t pid = 0, ver = 0;

    ret = __sccb_rd_reg(p_this, OV7725_PID_REG, &pid);
    if (AW_OK != ret) {
        return ret;
    }
    ret = __sccb_rd_reg(p_this, OV7725_VER_REG, &ver);
    if (AW_OK != ret) {
        return ret;
    }
    if (OV7725_REVISION != (((uint32_t)pid << 8U) | (uint32_t)ver)) {
        return AW_ERROR;
    }
    return AW_OK;
}

aw_local aw_err_t __ov7725_standby(void *p_cookie)
{
    return __sccb_modify_reg(p_cookie, OV7725_COM2_REG, 1<<4, 1<<4);
}

aw_local aw_err_t __ov7725_startup(void *p_cookie)
{
    return __sccb_modify_reg(p_cookie, OV7725_COM2_REG, 1<<4, 0);
}


/**
 * \brief OV7725分辨率设置(只支持VGA和QVGA)
 */
aw_local aw_err_t __ov7725_resolutioin_set(void *p_cookie, current_mode_t mode)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    uint32_t width = 0, height = 0;
    uint16_t hstart, vstart, hsize;
    int ret = 0;

    /* 初始化 OV7725 为预览的分辨率  */
    if (mode == VIDEO) {
       width = VIDEO_EXTRACT_WIDTH(p_this->video_resolution);
       height = VIDEO_EXTRACT_HEIGHT(p_this->video_resolution);

       if (p_this->video_resolution == VGA) {

           /* 设置为VGA输出  */
           __sccb_modify_reg(p_this, OV7725_COM7_REG, (1 << 6), (0 << 6) );

           hstart = 0x23U << 2U;
           vstart = 0x07U << 1U;
           hsize = width;
       } else if (p_this->video_resolution == QVGA){
           /* 设置为QVGA输出  */
           __sccb_modify_reg(p_this, OV7725_COM7_REG, (1 << 6), (1 << 6) );

           hstart = 0x3FU << 2U;
           vstart = 0x03U << 1U;
           hsize = width;
       } else {
           return -AW_EINVAL;
       }

    } else if (mode == PHOTO) {
       width = VIDEO_EXTRACT_WIDTH(p_this->photo_resolution);
       height = VIDEO_EXTRACT_HEIGHT(p_this->photo_resolution);

       if (p_this->photo_resolution == VGA) {
           /* 设置为VGA输出  */
           __sccb_modify_reg(p_this, OV7725_COM7_REG, (1 << 6), (0 << 6) );

           hstart = 0x23U << 2U;
           vstart = 0x07U << 1U;
           hsize = width;
       } else if (p_this->photo_resolution == QVGA){
           /* 设置为QVGA输出  */
           __sccb_modify_reg(p_this, OV7725_COM7_REG, (1 << 6), (1 << 6) );

           hstart = 0x3FU << 2U;
           vstart = 0x03U << 1U;
           hsize = width;
       } else {
           return -AW_EINVAL;
       }
    } else {
       return -AW_EINVAL;
    }

    if ((width > 640) || (height > 480))
    {
       return -AW_EINVAL;
    }

    ret = __ov7725_check_pid(p_cookie);

    if (ret != AW_OK) {
     return -AW_ENODEV;
    }

    /* Set the window size. */
    __sccb_wr_reg(p_this,  OV7725_HSTART_REG, hstart >> 2U);
    __sccb_wr_reg(p_this,  OV7725_HSIZE_REG, hsize >> 2U);
    __sccb_wr_reg(p_this,  OV7725_VSTART_REG, vstart >> 1U);
    __sccb_wr_reg(p_this,  OV7725_VSIZE_REG, height >> 1U);
    __sccb_wr_reg(p_this,  OV7725_HOUTSIZE_REG, width >> 2U);
    __sccb_wr_reg(p_this,  OV7725_VOUTSIZE_REG, height >> 1U);
    __sccb_wr_reg(p_this,  OV7725_HREF_REG,
                 ((vstart & 1U) << 6U) | ((hstart & 3U) << 4U) | ((height & 1U) << 2U) | ((hsize & 3U) << 0U));
    __sccb_wr_reg(p_this,  OV7725_EXHCH_REG, ((height & 1U) << 2U) | ((width & 3U) << 0U));

    /* 重新设置帧率  */
    ret = __ov7725_conf_frame_frq(p_cookie, (resolution_t)p_this->cfg.resolution);
    if (ret != AW_OK) {
        AW_MUTEX_UNLOCK(p_this->dev_lock);
        return ret;
    }

    return AW_OK;
}

/**
 * \brief OV7725软件复位
 */
aw_local aw_err_t __ov7725_soft_rst(void *p_cookie)
{
    return __sccb_wr_reg(p_cookie, OV7725_COM7_REG, 0x80);
}



/**
 * \brief OV7725配置帧率
 */
aw_local aw_err_t __ov7725_conf_frame_frq(void *p_cookie, resolution_t res)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    __OV7725_DEVINFO_DECL(p_devinfo, p_cookie);
    uint8_t mul = 0;
    uint32_t pclk = p_devinfo->input_clock;
    int32_t dummy = 0;
    uint8_t dm_lnh = 0, dm_lnl = 0;

    if ((p_devinfo->input_clock < 10000000u) ||
        (p_devinfo->input_clock > 48000000u)) {

        return -AW_EINVAL;
    }

    if (p_devinfo->pix_format == BAYER_RAW) {
        mul = 1;
    } else {
        mul = 2;
    }

    if (p_devinfo->pix_format == BAYER_RAW) {

        /* 测出如果是raw格式，测试，设置的pclk是48M，则测出的是24M
         * 设置的是12M则测出的是6M.只要设置为RAW格式输出OV7725内部就会硬件自动2分频，
         * 这个数据手册上没有说明,所以如果是RAW格式，实际pclk是理论设置的1/2,固这里要
         * 将pclk除以2,否则帧率会是15帧每秒。
         *
         * RGB则设置是多少就是多少。
         */
        pclk /= 2;
    }


    /* 计算虚行，帧率固定为30帧，不提供给用户设置 */
    if (res == VGA) {
        /* VGA */
        dummy = (pclk / 25 / (784* mul)) - 510;
    } else if (res == QVGA) {

        /* QVGA */
        dummy = (pclk / 25 / (576* mul)) - 278;
    } else {
        return -AW_EINVAL;
    }

    if (dummy < 0) {
        return -AW_EINVAL;
    }

    dm_lnh = ((uint16_t)dummy >> 8) & 0xff;
    dm_lnl = (uint16_t)dummy & 0xff;

    if (p_devinfo->frame_rate > 30) {
        __sccb_wr_reg(p_this, OV7725_CLKRC_REG, 0);
    } else {
        __sccb_wr_reg(p_this, OV7725_CLKRC_REG, 1);
    }

    __sccb_wr_reg(p_this, OV7725_COM4_REG, (1 << 6) | 0x01);
    __sccb_wr_reg(p_this, OV7725_EXHCL_REG, 0x00);
    __sccb_wr_reg(p_this, OV7725_DM_LNL_REG, dm_lnl);
    __sccb_wr_reg(p_this, OV7725_DM_LNH_REG, dm_lnh);
    __sccb_wr_reg(p_this, OV7725_ADVFL_REG, 0x00);
    __sccb_wr_reg(p_this, OV7725_ADVFH_REG, 0x00);
    __sccb_wr_reg(p_this, OV7725_COM5_REG, 0x65);
    return AW_OK;
}


/**
 * \brief OV7725设置像素格式
 */
aw_local aw_err_t __ov7725_set_pix_format(void *p_cookie, pix_format_t pix_format)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    for (uint8_t i = 0; i < AW_NELEMENTS(ov7725_pix_format_conf); i++)
    {
        if (ov7725_pix_format_conf[i].fmt == pix_format)
        {
            /* 直接设置为0x48 摄像头不工作  */
            __sccb_modify_reg(p_this, OV7725_DSP_CTRL4_REG, 0x1FU, ov7725_pix_format_conf[i].dsp_ctrl4);

            __sccb_wr_reg(p_this, OV7725_COM7_REG, ov7725_pix_format_conf[i].com7);

            return AW_OK;
        }
    }

    return -AW_EINVAL;
}


aw_local aw_err_t __ov7725_get_pix_format(void *p_cookie, pix_format_t *pix_format)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    uint8_t tmp;

    __sccb_rd_reg(p_this, OV7725_COM7_REG, &tmp);

    switch (tmp&0x03) {
    case 0:
        *pix_format = YUV422;
        break;
    case 3:
        *pix_format = BAYER_RAW;
        break;
    case 2:
        switch ((tmp & 0x0C) >> 2) {
        case 1:
            *pix_format = RGB565;
            break;
        case 2:
            *pix_format = RGB555;
            break;
        case 3:
            *pix_format = RGB444;
            break;
        }
        break;
    default:
        break;
    }

    return AW_OK;
}


aw_local aw_err_t __ov7725_aec_set(void *p_cookie, aw_camera_cfg_t *p_cfg)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    int ret = AW_OK;

    if (p_cfg->aec_enable == AW_CAMERA_OPT_ENABLE) {
        ret = __sccb_modify_reg(p_this, OV7725_COM8_REG, 0x01<<0, 0x01);
        if (ret != AW_OK) {
            return ret;
        }
        ret = __sccb_wr_reg(p_this, OV7725_BRIGHT_REG, p_cfg->brightness);
        if (ret != AW_OK) {
            return ret;
        }
    } else if (p_cfg->aec_enable == AW_CAMERA_OPT_DISABLE) {
        ret = __sccb_modify_reg(p_this, OV7725_COM8_REG, 0x01<<0, 0x00);
        if (ret != AW_OK) {
            return ret;
        }
        ret = __sccb_wr_reg(p_this, OV7725_AECH_REG, (uint8_t)(p_cfg->exposure>>8));
        if (ret != AW_OK) {
            return ret;
        }
        ret = __sccb_wr_reg(p_this, OV7725_AEC_REG, (uint8_t)p_cfg->exposure);
        if (ret != AW_OK) {
            return ret;
        }
    }

    return AW_OK;
}


aw_local aw_err_t __ov7725_awb_set(void *p_cookie, aw_camera_cfg_t *p_cfg)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    int ret = AW_OK;

    if (p_cfg->awb_enable == AW_CAMERA_OPT_ENABLE) {
        ret = __sccb_modify_reg(p_this, OV7725_COM8_REG, 0x01<<1, 0x02);
        if (ret != AW_OK) {
            return ret;
        }
    } else if (p_cfg->awb_enable == AW_CAMERA_OPT_DISABLE) {
        ret = __sccb_modify_reg(p_this, OV7725_COM8_REG, 0x01<<1, 0);
        if (ret != AW_OK) {
            return ret;
        }
        ret = __sccb_wr_reg(p_this, OV7725_BLUE_REG, p_cfg->blue_gain);
        if (ret != AW_OK) {
            return ret;
        }
        ret = __sccb_wr_reg(p_this, OV7725_RED_REG, p_cfg->red_gain);
        if (ret != AW_OK) {
            return ret;
        }
        ret = __sccb_wr_reg(p_this, OV7725_GREEN_REG, p_cfg->green_gain);
        if (ret != AW_OK) {
            return ret;
        }
    }

    return ret;
}

aw_local aw_err_t __ov7725_conversion_set(void *p_cookie, aw_camera_cfg_t *p_cfg)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    int ret = AW_OK;

    if (p_cfg->conversion & MIRROR) {
        ret = __sccb_modify_reg(p_this, OV7725_COM3_REG, 0x01<<6, 0x01<<6);
        if (ret != AW_OK) {
            return ret;
        }
    } else {
        ret = __sccb_modify_reg(p_this, OV7725_COM3_REG, 0x01<<6, 0);
        if (ret != AW_OK) {
            return ret;
        }
    }

    if (p_cfg->conversion & UPSIDE_DOWN){
        ret = __sccb_modify_reg(p_this, OV7725_COM3_REG, 0x01<<7, 0x01<<7);
        if (ret != AW_OK) {
            return ret;
        }
    } else {
        ret = __sccb_modify_reg(p_this, OV7725_COM3_REG, 0x01<<7, 0);
        if (ret != AW_OK) {
            return ret;
        }
    }

    return AW_OK;
}



/**
 * \brief OV7725初始化
 */
aw_local aw_err_t __ov7725_init(void *p_cookie)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    __OV7725_DEVINFO_DECL(p_devinfo, p_cookie);

    int ret = 0;
    uint8_t com10 = 0;

    __ov7725_startup(p_this);
    aw_mdelay(2);

    __ov7725_soft_rst(p_cookie);
    aw_mdelay(10);

    ret = __ov7725_check_pid(p_cookie);
    if (ret != AW_OK) {
        return -AW_ENODEV;
    }

    ret = __ov7725_init_squen(p_cookie);
    if (ret != AW_OK) {
        return -AW_ENODEV;
    }

    /* 设置像素格式  */
    __ov7725_set_pix_format(p_cookie, p_devinfo->pix_format);

    /*  */
    if (AW_CAM_HREF_ACTIVE_HIGH
            != (p_devinfo->control_flags & AW_CAM_HREF_ACTIVE_HIGH)) {
       com10 |= OV7725_COM10_HREF_REVERSE_MASK;
    }

    if (AW_CAM_VSYNC_ACTIVE_HIGH
            != (p_devinfo->control_flags & AW_CAM_VSYNC_ACTIVE_HIGH)) {
       com10 |= OV7725_COM10_VSYNC_NEG_MASK;
    }

    if (AW_CAM_DATA_LATCH_ON_RISING_EDGE
            != (p_devinfo->control_flags & AW_CAM_DATA_LATCH_ON_RISING_EDGE)) {
       com10 |= OV7725_COM10_PCLK_REVERSE_MASK;
    }

    __sccb_wr_reg(p_this, OV7725_COM10_REG, com10);

    /* Don't swap output MSB/LSB. */
    __sccb_wr_reg(p_this, OV7725_COM3_REG, 0x00);

    /*
    * Output drive capability
    * 0: 1X
    * 1: 2X
    * 2: 3X
    * 3: 4X
    */
    __sccb_modify_reg(p_this,  OV7725_COM2_REG, 0x03, 0x03);

    if (p_devinfo->frame_rate == 30) {
        __sccb_wr_reg(p_this, OV7725_BDBASE_REG, 0x7f);
        __sccb_wr_reg(p_this, OV7725_BDMSTEP_REG, 0x03);
    }

    ret = __ov7725_standby(p_this);

    return ret;
}

/*****************************************************************************/


/*********************************INIT SETTING********************************/

/**
 * \brief find out a service who accept the pin
 */
struct awbl_camera_controller_service * __camera_controller_to_serv (void *p_cookie)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    __OV7725_DEVINFO_DECL(p_devinfo, p_cookie);
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


AWBL_METHOD_IMPORT(awbl_cameracontrollerserv_get);

aw_local aw_err_t __camera_controller_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t      pfunc_camera_controller_serv = NULL;
    struct awbl_camera_service  *p_camera_serv     = NULL;

    /* find handler */

    pfunc_camera_controller_serv = awbl_dev_method_get(p_dev,
                                          AWBL_METHOD_CALL(awbl_cameracontrollerserv_get));

    /* call method handler to allocate camera service */

    if (pfunc_camera_controller_serv != NULL) {

        pfunc_camera_controller_serv(p_dev, &p_camera_serv);

        /* found a camera service, insert it to the service list */

        if (p_camera_serv != NULL) {
            struct awbl_camera_service **pp_serv_cur = (struct awbl_camera_service **)p_arg;

            while (*pp_serv_cur != NULL) {
                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }

            *pp_serv_cur = p_camera_serv;
             p_camera_serv->p_next = NULL;
        }
    }

    return AW_OK;   /* iterating continue */
}



/**
 * \brief 获取ov7725对应csi的子方法
 */
aw_local void __ov7725_sub_method_init(void *p_cookie)
{
    /* 初始化CSI */
    __OV7725_DEV_DECL(p_this, p_cookie);

    p_this->p_controller_serv = NULL;

    /* 在打开中枚举相应的服务  */
    awbl_dev_iterate(__camera_controller_serv_alloc_helper, &p_this->p_controller_serv, AWBL_ITERATE_INSTANCES);

}

aw_local aw_err_t __ov7725_video_mem_reset(void *p_cookie, uint32_t video_resolution, uint8_t pix)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    __OV7725_DEVINFO_DECL(p_devinfo, p_cookie);
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

aw_local aw_err_t __ov7725_photo_mem_reset(void *p_cookie, uint32_t photo_resolution, uint8_t pix)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    __OV7725_DEVINFO_DECL(p_devinfo, p_cookie);
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

aw_local aw_err_t __ov7725_restart(void *p_cookie,
                                   uint32_t video_resolution,
                                   uint32_t photo_resolution,
                                   uint8_t pix_byte)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    int ret = AW_OK;


    if (!__STATUS_IS(p_this->status, __STANDBY_STATE)) {
        __ov7725_standby(p_this);
    }


    ret = __ov7725_video_mem_reset(p_this, video_resolution, pix_byte);
    if (ret != AW_OK) {
        return ret;
    }

    ret = __ov7725_photo_mem_reset(p_this, photo_resolution, pix_byte);
    if (ret != AW_OK) {
        return ret;
    }
    p_this->cfg.bytes_per_pix = pix_byte;

    if (!__STATUS_IS(p_this->status, __STANDBY_STATE)) {
        if (__STATUS_IS(p_this->status, __VIDEO_RUNNING_STATE)) {
            p_this->cfg.resolution   = p_this->video_resolution;
            p_this->cfg.current_mode = VIDEO;
            ret = __ov7725_resolutioin_set(p_cookie, p_this->cfg.current_mode);
            ret = p_this->p_controller_serv->p_servfuncs->open(p_this->p_controller_serv->p_cookie, &p_this->cfg);
        } else if (__STATUS_IS(p_this->status, __PHOTO_RUNNING_STATE)) {
            p_this->cfg.resolution   = p_this->photo_resolution;
            p_this->cfg.current_mode = PHOTO;
            ret = __ov7725_resolutioin_set(p_cookie, p_this->cfg.current_mode);
            ret = p_this->p_controller_serv->p_servfuncs->open(p_this->p_controller_serv->p_cookie, &p_this->cfg);
        }
        __ov7725_startup(p_this);
    }

    return AW_OK;
}

/**
 * \brief OV7725内存初始化
 */
aw_local aw_err_t __mem_init (void *p_cookie)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    __OV7725_DEVINFO_DECL(p_devinfo, p_cookie);

    p_this->cfg.p_photo_buf_addr = NULL;
    p_this->cfg.p_photo_buf_addr = NULL;

    p_this->cfg.buf_num = p_devinfo->buf_num;
    p_this->cfg.p_buff_nodes_addr = (uint32_t *)aw_mem_alloc(sizeof(buffer_node_t) * \
                                                             p_devinfo->buf_num);
    if (p_this->cfg.p_buff_nodes_addr == 0) {
        return -AW_ENOMEM;
    }

    return __ov7725_restart(p_this,
                            p_devinfo->video_resolution,
                            p_devinfo->photo_resolution,
                            p_this->cfg.bytes_per_pix);
}

/**
 * \brief OV7725驱动初始化阶段2
 *
 * \param   p_dev   AWbus传下来表征当前设备
 * \return  无
 */
aw_local void __ov7725_inst_init1(awbl_dev_t *p_dev)
{

}

/**
 * \brief OV7725驱动初始化步骤2
 *
 * \param   p_dev   AWbus传下来表征当前设备
 * \return  无
 */
aw_local void __ov7725_inst_init2 (awbl_dev_t *p_dev)
{
    __OV7725_DEVINFO_DECL(p_devinfo, p_dev);

    p_devinfo->pfunc_plfm_init();

}
/**
 * \brief OV7725驱动初始化阶段3
 *
 * 此阶段主要是使能并注册了OV7725端口中断
 * \param   p_dev   AWbus传下来表征当前设备
 * \return  无
 */
aw_local void __ov7725_inst_connect(awbl_dev_t *p_dev)
{
    int ret;
    __OV7725_DEV_DECL(p_this, p_dev);
    __OV7725_DEVINFO_DECL(p_devinfo, p_dev);

    __sccb_init(p_this);

    AW_MUTEX_INIT(p_this->dev_lock, AW_PSP_SEM_Q_FIFO);

    /* 当前摄像头没有被打开  */
    p_this->photo_resolution = 0;
    p_this->video_resolution = 0;

    memset(&p_this->cfg, 0, sizeof(awbl_camera_controller_cfg_info_t));
    p_this->cfg.bytes_per_pix  = 2;
    if (p_devinfo->pix_format == BAYER_RAW) {
        p_this->cfg.bytes_per_pix = 1;
    }
    p_this->cfg.control_flags = p_devinfo->control_flags;
    p_this->cfg.resolution = 0;

    __STATUS_SET(p_this->status, __STANDBY_STATE);

    /* 分配buffer的内存  */
    ret = __mem_init(p_dev);
    if (ret != AW_OK) {
        aw_kprintf("ov7725 memory init failed: %d\n\r", ret);
        return;
    }

    /* 初始化ov7725 */
    ret = __ov7725_init(p_dev);
    if (ret != AW_OK) {
        aw_kprintf("ov7725 init failed: %d\n\r", ret);
        return;
    }

    /* 枚举ov7725 对应的摄像头控制器 */
    __ov7725_sub_method_init(p_dev);
}


/*****************************************************************************/


/***************************APPLICATION INTERFACE*****************************/


/**
 * \brief 打开摄像头
 */
aw_local aw_err_t __camera_open(void *p_cookie)
{
    /* 初始化CSI */
    __OV7725_DEV_DECL(p_this, p_cookie);
    int ret = 0;

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

//    __ov7725_startup(p_this);

    ret = __ov7725_check_pid(p_cookie);
    if (ret != AW_OK) {
        AW_MUTEX_UNLOCK(p_this->dev_lock);
        return ret;
    }

    __STATUS_CLR(p_this->status, __STANDBY_STATE);
    __STATUS_CLR(p_this->status, __VIDEO_RUNNING_STATE | __PHOTO_RUNNING_STATE);

    AW_MUTEX_UNLOCK(p_this->dev_lock);
    return AW_OK;
}


/**
 * \brief 关闭摄像头
 */
aw_local aw_err_t __camera_close(void *p_cookie)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    int ret = 0;

    if ( __STATUS_IS(p_this->status, __STANDBY_STATE)) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

    __ov7725_standby(p_this);
    __STATUS_SET(p_this->status, __STANDBY_STATE);

    if (__STATUS_IS(p_this->status, __PHOTO_RUNNING_STATE) ||
            __STATUS_IS(p_this->status, __VIDEO_RUNNING_STATE)) {
        ret = p_this->p_controller_serv->p_servfuncs->close(p_this->p_controller_serv->p_cookie);
    }

    AW_MUTEX_UNLOCK(p_this->dev_lock);
    return ret;
}


/**
 * \brief 获取一帧拍照模式的图片
 */
aw_local aw_err_t __camera_get_photo_buf(void *p_cookie, uint32_t **p_buf)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    int ret = 0;

    if (p_this == NULL) {
        return -AW_ENXIO;
    }

    if (!__STATUS_IS(p_this->status, __PHOTO_MODE_STATE)||
            __STATUS_IS(p_this->status, __STANDBY_STATE)) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

    if (__STATUS_IS(p_this->status, __PHOTO_SEAL_STATE)) {
        *p_buf = 0;
        AW_MUTEX_UNLOCK(p_this->dev_lock);
        return -AW_EBADF;
    }

    if (!__STATUS_IS(p_this->status, __PHOTO_RUNNING_STATE)) {
        /* 拍照模式  */
        p_this->cfg.resolution = p_this->photo_resolution;
        p_this->cfg.current_mode = PHOTO;

        ret = __ov7725_resolutioin_set(p_cookie, p_this->cfg.current_mode);
        if (ret != AW_OK) {
            AW_MUTEX_UNLOCK(p_this->dev_lock);
            return ret;
        }

        /* 以拍照模式打开摄像头 */
        ret = p_this->p_controller_serv->p_servfuncs->open(p_this->p_controller_serv->p_cookie, &p_this->cfg);
        if (ret != AW_OK) {
            AW_MUTEX_UNLOCK(p_this->dev_lock);
            return ret;
        }

        __STATUS_SET(p_this->status, __PHOTO_RUNNING_STATE);
        __STATUS_CLR(p_this->status, __VIDEO_RUNNING_STATE);
        __ov7725_startup(p_this);
    }

    /* 等待拍照完成 */
    ret = p_this->p_controller_serv->p_servfuncs->get_photo_buf(p_this->p_controller_serv->p_cookie, p_buf);

    if (ret != AW_OK) {
        AW_MUTEX_UNLOCK(p_this->dev_lock);
        return ret;
    }

    /* 获取到一帧图片,标记为没有被释放  */
    __STATUS_SET(p_this->status, __PHOTO_SEAL_STATE);

    AW_MUTEX_UNLOCK(p_this->dev_lock);

    /* 重新打开CSI接口,改为拍照的分辨率，初始化OV7725为拍照的分辨率 , 拍照完成，重新初始化为预览模式  */
    return AW_OK;
}


/**
 * \brief 获取一帧预览模式的图片
 */
aw_local aw_err_t __camera_get_video_buf(void *p_cookie, uint32_t **p_buf)
{
    int ret = 0;
    __OV7725_DEV_DECL(p_this, p_cookie);

    if (p_this == NULL) {
        return -AW_ENXIO;
    }

    if (!__STATUS_IS(p_this->status, __VIDEO_MODE_STATE)||
            __STATUS_IS(p_this->status, __STANDBY_STATE)) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

    if (!__STATUS_IS(p_this->status, __VIDEO_RUNNING_STATE)) {
        /* 拍照模式  */
        p_this->cfg.resolution = p_this->video_resolution;
        p_this->cfg.current_mode = VIDEO;

        ret = __ov7725_resolutioin_set(p_cookie, p_this->cfg.current_mode);
        if (ret != AW_OK) {
            AW_MUTEX_UNLOCK(p_this->dev_lock);
            return ret;
        }
        ret = p_this->p_controller_serv->p_servfuncs->open(p_this->p_controller_serv->p_cookie, &p_this->cfg);
        if (ret != AW_OK) {
            AW_MUTEX_UNLOCK(p_this->dev_lock);
            return ret;
        }
        __ov7725_startup(p_this);
        __STATUS_SET(p_this->status, __VIDEO_RUNNING_STATE);
        __STATUS_CLR(p_this->status, __PHOTO_RUNNING_STATE);
    }

    ret = p_this->p_controller_serv->p_servfuncs->get_video_buf(p_this->p_controller_serv->p_cookie, p_buf);

    AW_MUTEX_UNLOCK(p_this->dev_lock);
    return ret;
}


/**
 * \brief 释放拍照模式的地址
 */
aw_local aw_err_t __camera_release_photo_buf(void *p_cookie, uint32_t addr)
{
    __OV7725_DEV_DECL(p_this, p_cookie);

    int ret = AW_OK;

    if (p_this == NULL) {
        return -AW_ENXIO;
    }

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


/**
 * \brief 释放预览模式的一帧地址
 */
aw_local aw_err_t __camera_relase_video_buf(void *p_cookie, uint32_t addr)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    int ret = AW_OK;

    if (p_this == NULL) {
        return -AW_ENXIO;
    }

    if (!__STATUS_IS(p_this->status, __VIDEO_MODE_STATE)||
            __STATUS_IS(p_this->status, __STANDBY_STATE)) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);
    ret = p_this->p_controller_serv->p_servfuncs->release_video_buf(p_this->p_controller_serv->p_cookie, addr);
    __STATUS_CLR(p_this->status, __PHOTO_SEAL_STATE);

    AW_MUTEX_UNLOCK(p_this->dev_lock);

    return ret;
}

/**
 * \brief 相机配制函数
 */
aw_local aw_err_t __camera_config_set(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option)
{
    struct awbl_camera_controller_service *p_serv = \
                                           __camera_controller_to_serv(p_cookie);
    __OV7725_DEV_DECL(p_this, p_cookie);
    int restart_flag = 0;
    int ret = AW_OK;
    uint32_t photo_resolution, video_resolution;

    if (p_this == NULL) {
        return -AW_ENXIO;
    }

    photo_resolution = p_this->photo_resolution;
    video_resolution = p_this->video_resolution;

    p_serv = __camera_controller_to_serv(p_cookie);

    if (p_serv == NULL) {
      return -AW_ENXIO;
    }

    *option &= AW_CAMERA_RESOLUTION|AW_CAMERA_EXPOSURE|AW_CAMERA_GAIN|
               AW_CAMERA_CONVERSION|AW_CAMERA_FORMAT;

    AW_MUTEX_LOCK(p_this->dev_lock, AW_SEM_WAIT_FOREVER);

    if (*option & AW_CAMERA_EXPOSURE) {
        ret = __ov7725_aec_set(p_this, p_cfg);
        if (ret != AW_OK) {
            *option &= (~AW_CAMERA_EXPOSURE);
        }
    }

    if (*option & AW_CAMERA_GAIN) {
        ret = __ov7725_awb_set(p_this, p_cfg);
        if (ret != AW_OK) {
            *option &= (~AW_CAMERA_EXPOSURE);
        }
    }

    if (*option & AW_CAMERA_CONVERSION) {
        ret = __ov7725_conversion_set(p_this, p_cfg);
        if (ret != AW_OK) {
            *option &= (~AW_CAMERA_EXPOSURE);
        }
    }

    if (*option & AW_CAMERA_FORMAT) {
        ret = __ov7725_set_pix_format(p_this, p_cfg->format);
        if (ret == AW_OK) {
            if (p_cfg->format == BAYER_RAW) {
                p_this->pix_byte = 1;
            } else {
                p_this->pix_byte = 2;
            }
        }

        restart_flag = 1;
    }

    if (*option & AW_CAMERA_RESOLUTION) {
        photo_resolution    = p_cfg->photo_resolution;
        video_resolution    = p_cfg->video_resolution;

        restart_flag = 1;
    }

    if (restart_flag) {
        __ov7725_restart(p_this, video_resolution, photo_resolution, p_this->pix_byte);
    }

    AW_MUTEX_UNLOCK(p_this->dev_lock);
    return AW_OK;
}

aw_local aw_err_t __camera_config_get(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option)
{
    __OV7725_DEV_DECL(p_this, p_cookie);
    int ret = AW_OK;
    uint8_t tmp;

    *option &= AW_CAMERA_RESOLUTION|AW_CAMERA_EXPOSURE|AW_CAMERA_GAIN|AW_CAMERA_CONVERSION;


    if (*option & AW_CAMERA_EXPOSURE) {
        p_cfg->exposure = 0;
        __sccb_rd_reg(p_this, OV7725_COM8_REG, &tmp);
        if (tmp & 0x01) {
            p_cfg->aec_enable = AW_CAMERA_OPT_ENABLE;
        } else {
            p_cfg->aec_enable = AW_CAMERA_OPT_DISABLE;
        }
        __sccb_rd_reg(p_this, OV7725_BRIGHT_REG, &p_cfg->brightness);
        __sccb_rd_reg(p_this, OV7725_AECH_REG, &tmp);
        p_cfg->exposure = tmp;
        p_cfg->exposure <<= 8;
        __sccb_rd_reg(p_this, OV7725_AEC_REG, &tmp);
        p_cfg->exposure |= tmp;
    }

    if (*option & AW_CAMERA_GAIN) {
        __sccb_rd_reg(p_this, OV7725_COM8_REG, &tmp);
        if (tmp & 0x02) {
            p_cfg->awb_enable = AW_CAMERA_OPT_ENABLE;
        } else {
            p_cfg->awb_enable = AW_CAMERA_OPT_DISABLE;
        }
        p_cfg->blue_gain = 0;
        p_cfg->red_gain = 0;
        p_cfg->green_gain = 0;

        __sccb_rd_reg(p_this, OV7725_BLUE_REG, &tmp);
        p_cfg->blue_gain = tmp;
        __sccb_rd_reg(p_this, OV7725_RED_REG, &tmp);
        p_cfg->red_gain = tmp;
        __sccb_rd_reg(p_this, OV7725_GREEN_REG, &tmp);
        p_cfg->green_gain = tmp;
    }

    if (*option & AW_CAMERA_CONVERSION) {
        __sccb_rd_reg(p_this, OV7725_COM3_REG, &tmp);

        p_cfg->conversion = 0;
        p_cfg->conversion |= (tmp & 0x40) ? MIRROR :0;
        p_cfg->conversion |= (tmp & 0x80) ? UPSIDE_DOWN :0;
    }

    if (*option & AW_CAMERA_FORMAT) {

    }

    if (*option & AW_CAMERA_RESOLUTION) {
        p_cfg->photo_resolution = p_this->photo_resolution;
        p_cfg->video_resolution = p_this->video_resolution;
    }

    return AW_OK;
}




/**
 * \brief OV7725服务函数表
 */
aw_local aw_const struct awbl_camera_servfuncs __g_camera_servfuncs = {

        /** \brief 打开摄像头  */
        __camera_open,

        /** \brief 关闭摄像头  */
        __camera_close,

        /** \brief 获取一帧照片buf  */
        __camera_get_photo_buf,

        /** \brief 获取一帧预览buf  */
        __camera_get_video_buf,

        /** \brief 释放一帧照片buf */
        __camera_release_photo_buf,

        /** \brief 释放一帧预览buf  */
        __camera_relase_video_buf,

        /** \brief 配制摄像机  */
        __camera_config_set,

        /** \brief 获取摄像机配制  */
        __camera_config_get,

};

/**
 * \brief 获取OV7725服务的函数列表
 *
 * awbl_ov7725serv_get的运行时实现
 * \param   p_dev   AWbus传下来表征当前设备
 * \param   p_arg   用于输出OV7725服务函数列表的指针
 * \return  无
 */
aw_local aw_err_t __ov7725_cameraserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __OV7725_DEV_DECL(p_this, p_dev);
    __OV7725_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_camera_service   *p_serv = &p_this->camera_serv;

    /* initialize the ov7725 service instance */
    p_serv->p_next      = NULL;
    p_serv->p_name       = p_devinfo->p_name;
    p_serv->p_servfuncs = &__g_camera_servfuncs;
    p_serv->p_cookie    = (void *)p_this;
    p_serv->flag        = AW_CAMERA_FLAG_ALIVE;
    p_serv->used_count  = 1;

    /* send back the service instance */
    *(struct awbl_camera_service **)p_arg = p_serv;

    return AW_OK;
}


/**
 * \brief OV7725驱动AWbus驱动函数
 */
aw_local aw_const struct awbl_drvfuncs __g_ov7725_drvfuncs = {
    __ov7725_inst_init1,
    __ov7725_inst_init2,
    __ov7725_inst_connect
};

/******************************************************************************/

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_cameraserv_get);


/**
 * \brief OV7725驱动设备专有函数
 */
aw_local aw_const struct awbl_dev_method __g_ov7725_dev_methods[] = {
    AWBL_METHOD(awbl_cameraserv_get, __ov7725_cameraserv_get),
    AWBL_METHOD_END
};


/**
 * \brief OV7725驱动注册信息
 */
aw_local aw_const awbl_plb_drvinfo_t __g_ov7725_drv_registration = {
    {
        AWBL_VER_1,                 /* awb_ver */
        AWBL_BUSID_PLB,             /* bus_id */
        AWBL_OV7725_NAME,           /* p_drvname */
        &__g_ov7725_drvfuncs,       /* p_busfuncs */
        &__g_ov7725_dev_methods[0], /* p_methods */
        NULL                        /* pfunc_drv_probe */
    }
};

/******************************************************************************/
void awbl_ov7725_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_ov7725_drv_registration);
}


/* end of file */
