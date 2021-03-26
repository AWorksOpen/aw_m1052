/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 摄像头例程(rgb565格式，通过frame buffer显示画面)
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 对应平台的串口宏
 *      - AW_COM_CONSOLE
 *      - AW_DEV_xxxx_CSI(xxxx代表具体硬件平台名称)
 *      - AW_DEV_OV7725
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 在awbl_hwconf_ov7725.h配置文件中修改像素格式为 RGB565;
 *   4. 使用大屏开启SCREEN_800_480设备宏，使用小屏开启SCREEN_480_272；
 *
 * - 实验现象：
 *   1. 串口打印调试信息。
 *   2. 显示屏上显示摄像头拍摄到的画面。
 *
 * - 备注：
 *   1. 如果工程在初始化时出错，请在配置文件 awbl_hwconf_ov7725.h 和 awbl_hwconf_xxxxx_csi.h
 *      (xxxx代表具体硬件平台名称)中查看此demo所使用GPIO是否存在引脚复用
 *   2. raw格式，一个像素是用1个字节表示;
 *   3. rgb565格式，一个像素是2个字节；
 *   4. rgb888格式，一个像素是3个字节表示；
 *   5. 在摄像头分辨率为640 x 480 、显示屏分辨率尺寸为480 x 272的情况下(即：摄像头分辨率  > 显示屏分辨率)，
 *      该例程在显示屏上显示的一帧画面只是摄像头一帧画面的一部分
 *
 * \par 源代码
 * \snippet demo_camera_rgb565.c src_camera_rgb565
 *
 * \internal
 * \par Modification History
 * - 2.00 19-11-23 wk, modification.
 * - 1.01 18-07-25 lqy, modification.
 * - 1.00 18-07-24 sls, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_camera_rgb565 摄像头(rgb565格式)
 * \copydoc demo_camera_rgb565.c
 */

/** [src_camera_rgb565] */

#include "Aworks.h"
#include "aw_vdebug.h"
#include "aw_camera.h"
#include "aw_system.h"
#include "aw_types.h"
#include <stdio.h>
#include "aw_task.h"
#include "aw_fb.h"
#include <string.h>
#include "aw_mem.h"
#include "aw_demo_config.h"

/* 摄像头名称 */
#define __CAMERA_NAME              DE_CAMERA_NAME

#define __TEST_DEV_FB              DE_FB  /* 帧缓冲设备 */

//#define SCREEN_160_120
#define SCREEN_480_272
//#define SCREEN_800_480

/**
 * \brief设置VIDEO和PHOTO的分辨率
 * 支持的分辨率有： 640 X 480 、320 X 240、160 X 120
 */

#if defined SCREEN_160_120
#define __VIDEO_RES                 AW_RESOLUTION_SET(160, 120)
#define __PHOTO_RES                 AW_RESOLUTION_SET(160, 120)
#elif defined SCREEN_480_272
#define __VIDEO_RES                 AW_RESOLUTION_SET(320, 240)
#define __PHOTO_RES                 AW_RESOLUTION_SET(320, 240)
#elif defined SCREEN_800_480
#define __VIDEO_RES                 AW_RESOLUTION_SET(640, 480)
#define __PHOTO_RES                 AW_RESOLUTION_SET(640, 480)
#endif

void __test_camera_rgb565_task (void)
{
    int                     i = 0;
    aw_err_t                ret = 0;
    void                   *fd = NULL;
    aw_tick_t               t1, t2;
    void                   *buf = NULL;
    void                   *p_fb;
    int                     frame_ms = 0;
    uint32_t                opt;
    size_t                  __g_x_res;      /* 显示屏x方向尺寸 */
    size_t                  __g_y_res;      /* 显示屏y方向尺寸 */
    struct aw_camera_cfg    cfg;
    aw_fb_fix_info_t        fix_screen_info;
    aw_fb_var_info_t        g_var_screen_info;

    /* 打开摄像头 */
    fd = aw_camera_open(__CAMERA_NAME);
    if (fd == NULL) {
        aw_kprintf("camera_open error! \n\r");
        return ;
    }

    /* 配置摄像头分辨率 */
    cfg.photo_resolution = __PHOTO_RES;
    cfg.video_resolution = __VIDEO_RES;

    /* 使能自动曝光并设置目标亮度值 */
    cfg.aec_enable = 1;
    cfg.brightness = 64;

    /* 使能自动增益 */
    cfg.awb_enable = 1;

    /* 使能垂直翻转与镜像翻转 */
    cfg.conversion = MIRROR|UPSIDE_DOWN;

    /* 动态设置摄像头输出格式 */
    cfg.format = RGB565;

    opt = AW_CAMERA_RESOLUTION | AW_CAMERA_EXPOSURE | AW_CAMERA_GAIN |\
          AW_CAMERA_CONVERSION | AW_CAMERA_FORMAT;

    ret = aw_camera_config_set(fd, &cfg, &opt);
    if (ret != AW_OK) {
        aw_kprintf("aw_camera_cfg error,code:%d\r\n",ret);
    }
    aw_kprintf("camera config:\n\rresolution set %s! \nwindow set %s! \nexposure set %s! \n"
               "gain set %s! \nformat set %s!\nconversion set %s! \n",
               (opt & AW_CAMERA_RESOLUTION)?"successful":"failed",
               (opt & AW_CAMERA_WINDOW)?"successful":"failed",
               (opt & AW_CAMERA_EXPOSURE)?"successful":"failed",
               (opt & AW_CAMERA_GAIN)?"successful":"failed",
               (opt & AW_CAMERA_FORMAT)?"successful":"failed",
               (opt & AW_CAMERA_CONVERSION)?"successful":"failed");


    /* 打开帧缓冲设备 */
    p_fb = aw_fb_open(__TEST_DEV_FB, 0);

    /* 初始化帧缓冲设备 */
    ret = aw_fb_init(p_fb);

    /* frame buffer 设备信息获取 */
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &fix_screen_info);
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);

    /* 打开背光并运行面板 */
    ret = aw_fb_backlight(p_fb, 99);

    /* frame buffer启动 */
    ret = aw_fb_start(p_fb);

    __g_x_res = g_var_screen_info.res.x_res;
    __g_y_res = g_var_screen_info.res.y_res;

    if (fix_screen_info.vram_addr == 0) {
        return ;
    }
    memset((void *)fix_screen_info.vram_addr, 0, \
            g_var_screen_info.buffer.buffer_size * \
            g_var_screen_info.buffer.buffer_num);

    opt = AW_CAMERA_EXPOSURE;
    while (1) {
        t1 = aw_sys_tick_get();

        /* 获取摄像头的一帧预览地址 */
        ret = aw_camera_get_video_buf(fd, &buf);

        if (ret == AW_OK) {

            /* 判断摄像头的分辨率，计算拷贝的尺寸 ，在LCD上显示图像 */
            if (__VIDEO_RES == AW_RESOLUTION_SET(160, 120)) {
                for (i = 0; i < ((__g_y_res >= 120) ? 120 : __g_y_res); i ++) {
                    memcpy((uint8_t*)fix_screen_info.vram_addr + __g_x_res * 2 * i, (uint8_t*)buf + 160 * 2 * i, 160  * 2);
                }
            } else if (__VIDEO_RES == AW_RESOLUTION_SET(320, 240)) {
                for (i = 0; i < 240; i ++) {
                    memcpy((uint8_t*)fix_screen_info.vram_addr + __g_x_res * 2 * i, (uint8_t*)buf + 320 * 2 * i , 320 * 2);
                }
            } else if (__VIDEO_RES == AW_RESOLUTION_SET(640, 480)) {

                for (i = 0; i < ((__g_y_res >= 480) ? 480 : __g_y_res); i ++) {
                    memcpy((uint8_t*)fix_screen_info.vram_addr + __g_x_res * 2 * i, (uint8_t*)buf + 640 * 2 * i, ((__g_x_res > 640) ? 640 : __g_x_res) * 2);
                }
            }

            /* 释放摄像头的一帧预览地址 */
            ret = aw_camera_release_video_buf(fd, buf);
            if (ret != AW_OK) {
                aw_kprintf("get relase video error!\r\n");
                continue ;
            }

        }

        /* 获取摄像头曝光值 */
        ret = aw_camera_config_get(fd, &cfg, &opt);
        if (opt & AW_CAMERA_EXPOSURE) {
            aw_kprintf("exposure:%d\r\n",cfg.exposure);
        } else {
            aw_kprintf("%s do not support inquired\r\n", __CAMERA_NAME);
        }

        /* 计算帧率 */
        t2 = aw_sys_tick_get();
        frame_ms = 1000 / aw_ticks_to_ms(t2 - t1);
        sprintf(buf, "%d f/s.\n", frame_ms);
        aw_kprintf("%s\r\n",buf);
    }
    return ;
}

AW_TASK_DECL_STATIC(__camera_rgb565_lcd_show, 1024 * 10 );

/**
 * /brief 摄像头(rgb565格式)例程入口函数
 */
void demo_camera_rgb565_entry (void)
{

    aw_kprintf("demo_camera_rgb565 test...\n");

    /* 初始化任务LCD显示任务 */
    AW_TASK_INIT(__camera_rgb565_lcd_show,      /* 任务实体 */
                 "__camera_lcd_show",           /* 任务名字 */
                 12,                            /* 任务优先级 */
                 1024 * 10 ,                    /* 任务堆栈大小 */
                 __test_camera_rgb565_task,     /* 任务入口函数 */
                 (void *)NULL);                 /* 任务入口参数 */

    /* 启动任务__camera_lcd_show */
    AW_TASK_STARTUP(__camera_rgb565_lcd_show);

}
/** [src_camera_rgb565] */

/* end of file */
