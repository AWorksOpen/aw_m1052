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
 * \brief AWorks 帧缓冲设备接口
 *
 * 使用本服务需要包含以下头文件
 * \code
 * #include "aw_fb.h"
 * \endcode
 *
 * \par 使用说明
 *  使用帧缓冲设备前, 请先使用 aw_fb_open() 打开一个帧缓冲设备，而后通过
 *  aw_fb_init() 对帧缓冲设备进行初始化。此时，帧缓冲设备已经创建完成，通
 *  过 aw_fb_start() 便可运行。帧缓冲设备相关固定信息可通过 aw_fb_ioctl() 
 *  获得，例如显存起始地址等（详见 aw_fb_fix_info_t ）。帧缓冲设备相关可变
 *  配置可通过 aw_fb_ioctl() 获取和修改，例如显示方向（详见 aw_fb_var_info_t）
 *  用户准备绘制时，先通过 aw_fb_get_online_buf() 或 aw_fb_get_offline_buf()
 *  获取到缓冲区地址,然后在获取的缓冲区上进行绘制，绘制完成后，多缓存环境下
 *  需要通过 aw_fb_swap_buf() 或aw_fb_try_swap_buf() 将后置缓冲区交换为前置
 *  缓冲区进行显示。
 *
 * \par 注意事项
 *  本服务的所有API均不支持在多任务中使用。
 *
 * \par 多缓冲基本原理说明
 *  在启用多个缓冲区的情况下，由显示控制器所使用的前置缓冲区用于在屏幕上显示图像，
 *  同时，一个或者多个后置缓冲区用于用户的绘制操作。绘制操作完成后，缓冲区进行交换，
 *  交换操作完成后，后置缓冲区成为可见的前置缓冲区。
 *  用户在使用时可通过 aw_fb_ioctl() （详见 aw_fb_fix_info_t ）获得当前的缓冲区数目。
 *
 * \par 三缓冲优势说明
 *  在软件绘制速度相对固定的环境下，三缓存和双缓存运行效果基本一致，
 *  而在软件绘制速度不稳定的环境下，三缓存会缓解由于绘制速度不稳定导致的显示帧数
 *  波动较大的情况。事实上，大部分的显示任务的软件绘制速度都是不稳定的，因此，为
 *  了实现最佳性能，在空间允许的情况下，建议使用三缓冲。
 *
 * \par 单缓存使用示例
 * \code
 * #include "aw_fb.h"
 *
 * int main() 
 * {
 *    uint32_t online_buffer;
 *
 *    //帧缓冲设备创建流程如下
 *    void *p_fb = aw_fb_open("imx1050_fb", 0);
 *    aw_fb_init(p_fb);
 *    aw_fb_start(p_fb);
 *    aw_fb_backlight(p_fb, 100);
 *
 *    //在使用单缓存的情况中，用户仅可以操作前置缓冲区。
 *    vram_addr = aw_fb_get_online_buf(p_fb);           //获取前置缓冲地址
 *    while(1) {
 *        ......                                        //用户在此显存上完成绘制
 *    }
 * }
 * \endcode
 *
 * \par 多缓存使用示例1(支持双缓存和三缓存)
 * \code
 * #include "aw_fb.h"
 *
 * int main()
 * {
 *    uint32_t offline_buffer;
 *
 *    //帧缓冲设备创建流程如下
 *    void *p_fb = aw_fb_open("imx1050_fb", 0);
 *    aw_fb_init(p_fb);
 *    aw_fb_start(p_fb);
 *    aw_fb_backlight(p_fb, 100);
 *
 *    //在使用多缓存的情况中，用户一般只需要操作后置缓冲区。
 *    while(1) {
 *        offline_buffer = aw_fb_get_offline_buf(p_fb); //获取后置缓冲地址
 *        ......                                        //用户在此缓存上完成绘制
 *        aw_fb_swap_buf(p_fb);                         //绘制完成后进行缓冲区交换
 *    }
 * }
 * \endcode
 *
 * \par 多缓存使用示例2（仅支持三缓存）
 * \code
 * #include "aw_fb.h"
 *
 * int main()
 * {
 *    uint32_t offline_buffer;
 *
 *    //帧缓冲设备创建流程如下
 *    void *p_fb = aw_fb_open("imx1050_fb", 0);
 *    aw_fb_init(p_fb);
 *    aw_fb_start(p_fb);
 *    aw_fb_backlight(p_fb, 100);
 *
 *    offline_buffer = aw_fb_get_offline_buf(p_fb); //获取后置缓冲地址
 *    while(1) {
 *        ......                                    //用户在此缓存上完成绘制
 *        if (AW_OK == aw_fb_try_swap_buf(p_fb)) {  //绘制完成后尝试进行缓冲区交换，
 *                                                  //如果尝试交换失败，用户需要继续
 *                                                  //在当前的后置缓冲区进行绘制。
 *                                                  //如果尝试交换成功，用户应获取后置
 *                                                  //缓冲区，并对其进行绘制操作。
 *            offline_buffer = aw_fb_get_offline_buf(p_fb); //获取后置缓冲地址
 *        }
 *    }
 * }
 * \endcode
 *
 * \par 帧缓冲设备获取帧缓冲信息示例
 * \code
 * #include "aw_fb.h"
 *
 * int main()
 * { 
 *    uint32_t             vram_addr;
 *    uint32_t             vram_max;
 *    uint32_t             buffer_size;
 *    uint32_t             buffer_num;
 *    uint16_t             x_res;
 *    uint16_t             y_res;
 *    aw_fb_fix_info_t     fix_info;
 *    aw_fb_var_info_t     var_info;
 *
 *    void *p_fb = aw_fb_open("imx1050_fb", 0);
 *    aw_fb_init(p_fb);
 *
 *    //通过 aw_fb_ioctl() 获取固定信息 
 *    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &fix_info);
 *    v_addr      = fix_info.vram_addr;
 *    vram_max    = fix_info.vram_max;
 *
 *    //通过 aw_fb_ioctl() 获取可变信息
 *    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &var_info);
 *
 *    buffer_size = var_info.buffer.buffer_size;
 *    buffer_num  = var_info.buffer.buffer_num;
 *    x_res       = var_info.res.x_res;
 *    y_res       = var_info.res.y_res;
 *
 *    ......
 * }
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 2.00 19-04-30  cat, redesign frame interface
 * - 1.02 14-10-17  ops, code optimization.
 * - 1.00 14-08-05  ops, add five interface
 * - 1.00 14-05-29  zen, first implementation
 * \endinternal
 */

#ifndef __AW_FB_H
#define __AW_FB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_fb
 * \copydoc aw_fb.h
 * @{
 */

#include <stdarg.h>
#include "apollo.h"

/**
 * \brief 屏幕消隐级别
 */
typedef enum aw_fb_blank_level {
    
    AW_FB_BLANK_UNBLANK = 0,            /**< \brief hsync: on,  vsync: on  */
    AW_FB_BLANK_NORMAL,                 /**< \brief hsync: on,  vsync: on  */
    AW_FB_BLANK_VSYNC_SUSPEND,          /**< \brief hsync: on,  vsync: off */
    AW_FB_BLANK_HSYNC_SUSPEND,          /**< \brief hsync: off, vsync: on  */
    AW_FB_BLANK_POWERDOWN               /**< \brief hsync: off, vsync: off */
    
}aw_fb_black_t;

/**
 * \brief io操作命令
 */
typedef enum aw_fb_iocmd {

    AW_FB_CMD_GET_FINFO = 1,            /**< \brief 获取frame buffer固定信息 */
    AW_FB_CMD_GET_VINFO,                /**< \brief 获取frame buffer可变信息 */
    AW_FB_CMD_SET_VINFO,                /**< \brief 修改frame buffer可变信息 */

}aw_fb_iocmd_t;

/**
 * \brief 配置操作
 */
typedef enum aw_fb_config {

    AW_FB_CONF_DCLK_VINFO = 1,           /**< \brief 配置像素时钟频率 */
    AW_FB_CONF_RES_VINFO,                /**< \brief 配置屏幕尺寸信息 */
    AW_FB_CONF_BPP_VINFO,                /**< \brief 配置bpp信息 */
    AW_FB_CONF_HSYNC_VINFO,              /**< \brief 配置水平同步信号信息 */
    AW_FB_CONF_VSYNC_VINFO,              /**< \brief 配置垂直同步信号信息 */
    AW_FB_CONF_BUFFER_VINFO,             /**< \brief 配置缓冲区信息 */
    AW_FB_CONF_ORIENTATION_VINFO,        /**< \brief 横屏或竖屏显示：0-横屏（默认），1-竖屏  */

}aw_fb_config_t;

/**
 * \brief R的数据，偏移和长度信息
 */
typedef struct aw_fb_var_red_info {

    uint8_t  data;
    uint16_t offset;
    uint16_t length;

}aw_fb_var_red_info_t;

/**
 * \brief G的数据，偏移和长度信息
 */
typedef struct aw_fb_var_green_info {

    uint8_t  data;
    uint16_t offset;
    uint16_t length;

}aw_fb_var_green_info_t;

/**
 * \brief B的数据，偏移和长度信息
 */
typedef struct aw_fb_var_blue_info {

    uint8_t  data;
    uint16_t offset;
    uint16_t length;

}aw_fb_var_blue_info_t;

/**
 * \brief 行同步信号信息
 */
typedef struct aw_fb_var_hsync_info {

    uint16_t    h_pulse_width;   /** \brief 行脉冲宽度 */
    uint16_t    h_active;        /** \brief 一行的像素个数 */
    uint16_t    h_bporch;        /** \brief 水平同步后沿 */
    uint16_t    h_fporch;        /** \brief 水平同步前沿 */

}aw_fb_var_hsync_info_t;

/**
 * \brief 场同步信号信息
 */
typedef struct aw_fb_var_vsync_info {

    uint16_t    v_pulse_width;   /** \brief 垂直脉冲宽度 */
    uint16_t    v_active;        /** \brief 一帧画面的行数 */
    uint16_t    v_bporch;        /** \brief 垂直同步后沿 */
    uint16_t    v_fporch;        /** \brief 垂直同步前沿 */

}aw_fb_var_vsync_info_t;

/**
 * \brief 缓冲区信息
 */
typedef struct aw_fb_var_buffer_info {

    uint32_t               buffer_num;   /**< \brief 缓存数目 */
    uint32_t               buffer_size;  /**< \brief 帧大小 */

}aw_fb_var_buffer_info_t;

/**
 * \brief bpp信息
 */
typedef struct aw_fb_var_bpp_info {

    uint16_t                word_lbpp;    /**< \brief 一个像素的长度，位为单位,如rgb565数据，该值为16 */
    uint16_t                bus_bpp;      /**< \brief rgb硬件总线数 */

}aw_fb_var_bpp_info_t;

/**
 * \brief 屏幕尺寸信息
 */
typedef struct aw_fb_var_res_info {

    uint16_t                x_res;        /**< \brief 屏幕x轴分辨率 */
    uint16_t                y_res;        /**< \brief 屏幕y轴分辨率 */

}aw_fb_var_res_info_t;

/**
 * \brief 时钟频率信息
 */
typedef struct aw_fb_var_dclk_info {

    uint32_t                typ_dclk_f;       /** \brief 典型时钟Hz */
//    uint32_t                max_dclk_f;   /** \brief 最大时钟Hz */
//    uint32_t                min_dclk_f;   /** \brief 最小时钟Hz */

}aw_fb_var_dclk_info_t;

/**
 * \brief 帧缓冲设备可变信息（用户可通过 aw_fb_ioctl() 获取、修改结构体成员信息）
 */
typedef struct aw_fb_var_info {

    aw_fb_var_dclk_info_t   dclk_f;       /** \brief 时钟频率信息 */

    aw_fb_var_res_info_t    res;          /** \brief 屏幕尺寸信息 */

    aw_fb_var_bpp_info_t    bpp;          /**< \brief bpp信息 */

    aw_fb_var_hsync_info_t  hsync;        /**< \brief 水平同步信号信息 */
    aw_fb_var_vsync_info_t  vsync;        /**< \brief 垂直同步信号信息 */

    aw_fb_var_buffer_info_t buffer;       /**< \brief 缓冲区信息 */

    aw_fb_var_red_info_t    red;          /**< \brief red */
    aw_fb_var_green_info_t  green;        /**< \brief green */
    aw_fb_var_blue_info_t   blue;         /**< \brief blue */

    uint32_t                orientation;  /** \brief 横屏或竖屏显示：0-横屏（默认），1-竖屏 */

}aw_fb_var_info_t;

/**
 * \brief 帧缓冲设备固定信息（用户可通过 aw_fb_ioctl() 
 *        获取（不可修改）结构体成员信息）
 */
typedef struct aw_fb_fix_info {

    uint32_t               vram_addr;       /**< \brief 显存起始地址 */
    uint32_t               vram_max;        /**< \brief 最大显存空间 */
    
}aw_fb_fix_info_t;

/**
 * \brief 打开帧缓冲设备
 *
 * \param[in]   name        设备名
 * \param[in]   unit        设备单元号
 *
 * \retval      NULL        失败，否则为帧设备
 */
void *aw_fb_open(const char *name, int unit);

/**
 * \brief 初始化帧缓冲设备
 *
 * \param[in]   info        帧缓冲设备信息
 *
 * \retval      AW_OK       初始化成功
 * \retval     -AW_ENXIO    p_fb_info 为 NULL
 * \retval     -AW_ENOMEM   显存空间不足
 */
aw_err_t aw_fb_init(void *p_fb);

/**
 * \brief 开启帧缓冲设备
 *
 * \param[in]   p_fb_info   帧缓冲设备信息
 *
 * \retval      AW_OK       运行成功
 * \retval     -AW_ENXIO    p_fb_info 为 NULL
 */
aw_err_t aw_fb_start(void *p_fb);

/**
 * \brief 停止帧缓冲设备   
 *
 * \param[in]   p_fb_info   帧缓冲设备信息
 *
 * \retval      AW_OK       停止成功
 * \retval     -AW_ENXIO    p_fb_info 为 NULL
 */
aw_err_t aw_fb_stop(void *p_fb);
/**
 * \brief 重启帧缓冲设备
 *
 * \param[in]   p_fb_info   帧缓冲设备信息
 *
 * \retval      AW_OK       重启成功
 * \retval     -AW_ENXIO    p_fb_info 为 NULL
 */
aw_err_t aw_fb_reset(void *p_fb);

/**
 * \brief 获取前置帧缓冲地址
 *
 * \param[in]   p_fb_info   帧缓冲设备信息
 *
 * \retval      NULL        失败，否则为帧缓冲地址
 * \retval     -AW_ENXIO    p_fb_info 为 NULL
 *
 * \note        单缓存环境下需要使用此API获取当前使用的帧缓存地址，
 *              多缓存环境下一般不会使用到此API。
 */
uint32_t aw_fb_get_online_buf(void *p_fb);

/**
 * \brief 获取后置帧缓冲地址
 *
 * \param[in]   p_fb_info   帧缓冲设备信息
 *
 * \retval      NULL        失败，否则为帧缓冲地址
 * \retval     -AW_ENXIO    p_fb_info 为 NULL
 *
 * \note        仅支持在多缓存环境下使用此API
 */
uint32_t aw_fb_get_offline_buf(void *p_fb);

/**
 * \brief 前、后置缓冲区交换（多缓存环境下推荐使用）
 *
 * \param[in]   p_fb_info   帧缓冲设备信息
 *
 * \retval      AW_OK       成功
 * \retval     -AW_ENXIO    p_fb_info 为 NULL
 * 
 * \note  双缓存和三缓存环境下均可使用此API
 * \note  此API包含帧同步的等待操作(等待LCD控制器把前置缓冲区刷新完成后交换缓冲区)。
 *        用户必须要等待此接口执行完成才能继续进行绘制操作。
 */
aw_err_t aw_fb_swap_buf(void *p_fb);

/**
 * \brief 前、后置缓冲区尝试交换
 *
 * \param[in]   p_fb_info   帧缓冲设备信息
 *
 * \retval      AW_OK       成功
 * \retval     -AW_ENXIO    p_fb_info 为 NULL
 * \retval     -AW_EPERM    不支持，仅在三缓存模式下可用
 * \retval     -AW_EAGAIN   尝试失败，表示LCD控制器正在刷新
 *
 * \note        仅支持在三缓存环境下使用此API
 * \note        若LCD控制器正在刷新前置缓冲区，则取消交换缓冲区，并返回 -AW_EAGAIN。
 *              用户的绘制操作不用等待LCD控制器刷新完前置缓冲区。
 */
aw_err_t aw_fb_try_swap_buf(void *p_fb);

/**
 * \brief 设置帧缓冲设备消隐
 *
 * \param[in]   p_fb_info   帧缓冲设备信息
 * \param[in]   blank       消隐级别(系统支持的有效级别在< aw_fb.h >中定义)
 *
 * \retval      AW_OK       成功
 * \retval     -AW_ENXIO    p_fb_info 为 NULL
 * \retval     -AW_EINVAL   参数无效
 */
aw_err_t aw_fb_blank(void *p_fb, aw_fb_black_t blank);

/**
 * \brief 设置帧缓冲设备背光
 *
 * \param[in]   p_fb_info   帧缓冲设备信息
 * \param[in]   level       背光级别(0 - 100)
 *                         (如果设置的背光级别不在合法的范围内, 将使用默认背光级别100.)
 *
 * \retval      AW_OK       设置成功
 * \retval     -AW_ENXIO    p_fb_info 为 NULL
 *
 * \note 有些硬件PWM不能满占空比输出，因此PWM控制背光时，建议背光级别设置为99.
 */
aw_err_t aw_fb_backlight(void *p_fb, int level);

/**
 * \brief 配置帧缓冲设备信息
 *
 * \param[in]   p_fb_info   帧缓冲设备信息
 * \param[in]   conf        aw_fb_config_t 中定义
 * \param[in]   p_arg       用于传参

 *
 * \retval      AW_OK       设置成功
 * \retval     -AW_ENXIO    p_fb_info 为 NULL
 *
 * \note 配置时序信息:水平同步时序和垂直同步时序.
 * \note 配置bpp信息:var_info.bpp.bus_bpp与var_info.bpp.word_lbpp分别为输入帧一个像素大小和屏幕总线宽度，
 *       当var_info.bpp.bus_bpp与var_info.bpp.word_lbpp不相等时，如果输入帧的每像素比显示器的位数少，如在
 *       16 bpp输入帧中进入24 bpp LCD，则eLCDIF将为每个像素填充每种颜色的MSB到相同颜色的LSB。如果输入帧的每像素位
 *       数多于显示器，例如，24 bpp输入帧到达16 bpp LCD，则eLCDIF将丢弃每个颜色通道的LSB以转换为较低的颜色深度。一
 *       般用法var_info.bpp.bus_bpp与var_info.bpp.word_lbpp相等，
 */
aw_err_t aw_fb_config(void *p_fb, aw_fb_config_t conf, void *p_arg);

/**
 * \brief 设置/获取帧缓冲设备信息
 *
 * \param[in]   p_fb_info   帧缓冲设备信息
 * \param[in]   cmd         aw_fb_iocmd_t 中定义
 * \param[in]   p_arg       用于传参或取值
 *
 * \retval      AW_OK
 * \retval     -AW_ENXIO    p_fb_info 为 NULL
 * \retval     -AW_EINVAL   参数无效
 */
aw_err_t aw_fb_ioctl(void *p_fb, aw_fb_iocmd_t cmd, void *p_arg);
/** @} */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_FB_H */

/* end of file */

