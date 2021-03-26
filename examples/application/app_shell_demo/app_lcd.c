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
 * \brief 通过shell命令控制LCD显示屏和触摸屏
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *      - 对应平台的串口宏
 *      - 对应显示屏的宏
 *      - 对应触摸屏的宏
 *
 * - 实验现象：
 *   1. 输入".test"进入命令集
 *   2. 执行 "lcd -c"，选在屏幕上填充红色
 *   3. 执行 "lcd -v"  在屏幕上填充渐变色（垂直梯度）
 *   4. 执行 "lcd -h"  在屏幕上填充渐变色（水平梯度）
 *   5. 执行 "lcd -j"  屏幕校准
 *   6. 执行 "lcd -b +背光级别 ",调整屏幕背光，背光级别为（0-100，数字越大亮度越高），不在此范围的按100算
 *
 * - 备注：
 *   1. 颜色可以直接输入16进制数，必须加0x前缀，一些常用的颜色也可直接输入颜色。
 *
 * \par 源代码
 * \snippet app_buzzer.c app_buzzer
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_lcd] */
#include "aworks.h"                     /* 此头文件必须被首先包含 */
#include "aw_shell.h"
#include "aw_vdebug.h"
#include "aw_ts.h"
#include "app_config.h"
#include "stdio.h"
#include "string.h"
#include "aw_serial.h"
#include "aw_delay.h"
#include "aw_demo_config.h"
#include "aw_fb.h"

#ifdef APP_LCD

#ifndef CAP_TEST
#define __TFT_NAME       DE_TS_NAME              /*电阻触摸屏*/
#else
#define __TFT_NAME       DE_CAP_TS_NAME              /*电容触摸屏*/
#endif

#define __TFB_NAME       DE_FB                       /*显示屏名称*/

aw_local uint16_t        __g_pen_color;             /* 画笔颜色 */
aw_local size_t          __g_x_res;                 /* x方向尺寸 */
aw_local size_t          __g_y_res;                 /* y方向尺寸 */
aw_local uint16_t        *__gp_frame_buffer;        /* frame buffer地址 */


///////////////////////////////////////////////////////////////
/* 设置画笔颜色 */
aw_local void __set_pen_color (uint8_t r, uint8_t g, uint8_t b)
{
    __g_pen_color = (r << 11) | (g << 6) | b;
}

/**
 * \brief 绘制一个点
 */
aw_local void __draw_point (int x, int y)
{
    __gp_frame_buffer[y * __g_x_res + x] = __g_pen_color;
}

/* 交换两个变量 */
aw_local void swap(int *p1, int *p2)
{
    int t;

    t = *p1;
    *p1 = *p2;
    *p2 = t;
}

/**
 * \brief 绘制一条竖线
 */
aw_local void __draw_v_line (int x, int y1, int y2)
{
    int i;
    if (y1 > y2) {
        swap(&y1, &y2);
    }
    for (i = y1; i <= y2; i++) {
        __gp_frame_buffer[i * __g_x_res + x] = __g_pen_color;
    }
}

/**
 * \brief 绘制一条横线
 */
aw_local void __draw_h_line (int x1, int x2, int y)
{
    int i;
    if (x1 > x2) {
        swap(&x1, &x2);
    }
    for (i = x1; i <= x2; i++) {
        __gp_frame_buffer[y * __g_x_res + i] = __g_pen_color;
    }
}


/* 绘制光标（简易光标） */
aw_local void __draw_cursor (int x, int y, int size)
{
    __set_pen_color(0x1f, 0x3f, 0x1f);
    __draw_h_line(x - size, x + size, y);
    __draw_v_line(x, y - size, y + size);
}

/* 传入的参数需要和绘制光标时传入的一模一样 */
aw_local void __clear_cursor (int x, int y, int size)
{
    __set_pen_color(0, 0, 0);
    __draw_h_line(x - size, x + size, y);
    __draw_v_line(x, y - size, y + size);
}


/* 触摸校准 */
aw_local int __app_ts_calibrate (aw_ts_id                 id,
                               aw_ts_lib_calibration_t *p_cal)
{
    int                 i;
    struct aw_ts_state  sta;

    /* 初始化触摸数据 */
    p_cal->log[0].x = 60 - 1;
    p_cal->log[0].y = 60 - 1;
    p_cal->log[1].x = __g_x_res - 60 - 1;
    p_cal->log[1].y = 60 - 1;
    p_cal->log[2].x = __g_x_res - 60 - 1;
    p_cal->log[2].y = __g_y_res - 60 - 1;
    p_cal->log[3].x = 60 - 1;
    p_cal->log[3].y = __g_y_res - 60 - 1;
    p_cal->log[4].x = __g_x_res / 2 - 1;
    p_cal->log[4].y = __g_y_res / 2 - 1;

    p_cal->cal_res_x = __g_x_res;
    p_cal->cal_res_y = __g_y_res;

    /* 五点触摸 */
    for (i = 0; i < 5; i++) {
        /* 显示光标 */

        if (i != 0) {
            __clear_cursor(p_cal->log[i - 1].x, p_cal->log[i - 1].y, 5);
        }
        __draw_cursor(p_cal->log[i].x, p_cal->log[i].y, 5);


        while (1) {
            /* 等待获取光标所在位置触摸数据 */
            if ((aw_ts_get_phys(id, &sta, 1) > 0) &&
                (sta.pressed == AW_TRUE)) {
                p_cal->phy[i].x = sta.x;
                p_cal->phy[i].y = sta.y;

                aw_kprintf("\n x=%d, y=%d \r\n", (uint32_t)sta.x, (uint32_t)sta.y);
                while(1) {
                    aw_mdelay(500);
                    if ((aw_ts_get_phys(id, &sta, 1) == AW_OK) &&
                        (sta.pressed == AW_FALSE)) {
                        break;
                    }
                }
                break;
            }
            aw_mdelay(10);
        }
    }

    __clear_cursor(p_cal->log[i - 1].x, p_cal->log[i - 1].y, 5);

    return 0;
}

aw_local aw_ts_id __touch_handel (void)
{
    aw_ts_id                sys_ts;
    aw_ts_lib_calibration_t cal;

    /* 获取触摸设备 */
    sys_ts = aw_ts_serv_id_get(__TFT_NAME, 0, 0);

    if (sys_ts == NULL) {
        aw_kprintf("get touch server failed\r\n");
        while (1);
    }

    /* 判断是否设备支持触摸校准 */
//    if (aw_ts_calc_flag_get(sys_ts)) {
//        /* 加载系统触摸数据 */
//        if (aw_ts_calc_data_read(sys_ts) != AW_OK) {
//            /* 没有有效数据，进行校准 */
            do {
                __app_ts_calibrate(sys_ts, &cal);
            } while (aw_ts_calibrate(sys_ts, &cal) != AW_OK);
            /* 校准成功，保存触摸数据 */
            aw_ts_calc_data_write(sys_ts);
//        }
//    } else {
//        /* 电容屏需要进行XY转换 */
//        aw_ts_set_orientation(sys_ts, AW_TS_SWAP_XY);
//    }

    return sys_ts;
}

static aw_fb_fix_info_t           fix_screen_info;
static aw_fb_var_info_t           var_screen_info;

aw_local aw_ts_id __g_lcd_init(void)
{


    void                       *p_fb = NULL;

    /* 打开frame buffer */
    p_fb = aw_fb_open(__TFB_NAME, 0);
    if (p_fb == NULL) {
        aw_kprintf("FB open failed.\r\n");
        return p_fb;
    }

    /*frame buffer 初始化*/
    aw_fb_init(p_fb);

    /*frame buffer 设备信息获取*/
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_VINFO,&var_screen_info);
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_FINFO,&fix_screen_info);

    /* 打开背光并运行面板 */
    aw_fb_backlight(p_fb, 100);

    /*frame buffer设备信息开启*/
    aw_fb_start(p_fb);

    __g_x_res = var_screen_info.res.x_res;
    __g_y_res = var_screen_info.res.y_res;


    __gp_frame_buffer = (uint16_t *)fix_screen_info.vram_addr;

    memset((void *)fix_screen_info.vram_addr, 0, var_screen_info.buffer.buffer_size * var_screen_info.buffer.buffer_num);


    return p_fb;
}


int app_lcd(int argc, char **argv, struct aw_shell_user *p_user)
{

     int                        x_i,y_j;
     int                        ret = 0;
     int                        bk_bright = 0;
     void                       *p_fb = NULL;

     p_fb = __g_lcd_init();    //屏幕初始化
     if(p_fb == NULL){
         return -AW_EAGAIN;
     }

     while (1) {
        if (aw_shell_args_get(p_user, "-j", 0) == AW_OK) {
            /* 屏幕校准 */
             __touch_handel();
        }
        else if (aw_shell_args_get(p_user, "-b", 1) == AW_OK) {
            /* 打开frame buffer */
            bk_bright = aw_shell_long_arg(p_user, 1);
            aw_fb_backlight(p_fb, bk_bright);    /*背光调整*/
            memset((void *)fix_screen_info.vram_addr,
                   0xFF,
                   var_screen_info.buffer.buffer_size
                   * var_screen_info.buffer.buffer_num);
        }else if (aw_shell_args_get(p_user, "-c", 0) == AW_OK) {
            for(x_i=0; x_i<__g_x_res; x_i++) {
                __set_pen_color(0x1f, 0x00, 0x00); /* 绘制 */
                for(y_j = 0; y_j < __g_y_res; y_j++) {
                    __draw_point(x_i, y_j);
                }
            }
        } else if (aw_shell_args_get(p_user, "-v", 0) == AW_OK) {
            for(y_j = 0; y_j < __g_y_res; y_j++) {
                __set_pen_color(0, y_j, 0);
                for(x_i=0; x_i<__g_x_res; x_i++){
                    __draw_point(x_i, y_j);
                }
            }

        } else if (aw_shell_args_get(p_user, "-h", 0) == AW_OK) {
            for(x_i=0; x_i<__g_x_res; x_i++) {
                __set_pen_color(0, 0, x_i);       /* 绘制 */
                for(y_j = 0; y_j < __g_y_res; y_j++) {
                    __draw_point(x_i, y_j);
                }
             }
        }
        else {
            break;
        }
    }
    return ret;
}




#endif /*APP_LCD*/
