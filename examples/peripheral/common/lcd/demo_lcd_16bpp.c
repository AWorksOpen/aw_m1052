/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief frame buffer例程
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能：
 *      - AW_COM_CONSOLE
 *      - 对应平台的串口宏
 *      - AW_COM_SHELL_SERIAL
 *      - 使用的显示屏的宏、触摸外设的宏
 *   2. 打开shell调试终端，设置波特率为115200-8-N-1
 *   3. 在LCD配置文件中配置相关默认参数，如__HW800480_BUS_BPP 16，__HW800480_WORD_LBPP 16，
 *      则表示16位像素数据在16线数据总线的宽度的LCD显示
 *   4. 如果LCD缓冲区是静态区段,则需要注意LCD该段的大小，不能低于一帧数据大小，
 *      如16bpp,800*480分辨率，一帧数据大小为800*480*2=768000
 *
 * - 实验现象：
 *   1. 先进行屏幕校准；
 *   2. 屏幕中间从上至下显示红绿蓝黄紫青白7条条纹；
 *   3. 左边屏幕中间显示一条绿色的横线，右边显示一条红色的竖线；
 *   4. 屏幕左上角显示“AWorks”
 *   5. 触摸屏幕将在屏幕上画线。
 *   6. 屏幕左下角出现(x,y)触摸坐标值
 *   7. 用户通过shell终端,键入命令:
 *      键入"LCD_DCLK 48000000"命令，会配置LCD时钟频率
 *      键入"LCD_BPP  16 16"命令，会配置LCD位像素深度（一个像素的大小）和 数据总线的宽度
 *      键入"LCD_HSYNC 1 800 1000 46"命令，会配置LCD水平同步信号信息
 *      键入"LCD_VSYNC 1 480 22 23"命令，会配置LCD垂直同步信号信息
 *      键入"LCD_RES   800 480"命令，会配置屏幕分辨率，使用该命令修改分辨率，如果LCD缓冲区是静态区段，可能会
 *          导致内存溢出，请谨慎使用，确保分给LCD静态区段的大小足够。
 *          如果分辨率改变，则HSYNC与VSYNC参数也要做相应改变，如键入"LCD_RES 480 272"，则需键入
 *          "LCD_HSYNC 1 480 4 15"，"LCD_VSYNC 1 272 114 13"
 *      键入"LCD_RESET"命令，会复位LCD
 *   8. 用户配置好参数之后，需键入"LCD_RESET"命令复位LCD，观看调参后的显示效果
 *   9. 该例程支持16位像素数据在24线数据总线的宽度的LCD显示，只需键入"LCD_BPP  16 24"命令和"LCD_RESET"命令即可
 *
 * \par 源代码
 * \snippet demo_lcd_16bpp.c src_lcd_16bpp
 *
 * \internal
 * \par Modification history
 * - 1.00 18-07-16  sdq, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lcd 显示屏例程
 * \copydoc demo_lcd_16bpp.c
 */

/** [src_lcd_16bpp] */
#include "aworks.h"
#include "aw_ts.h"
#include "aw_fb.h"
#include <string.h>
#include <stdio.h>
#include "aw_vdebug.h"
#include "aw_shell.h"
#include "aw_delay.h"
#include "aw_demo_config.h"

#define __TS_NAME    DE_TS_NAME     /* 触摸屏名字 */

#define __FB_NAME    DE_FB          /* 显示屏名称 */

#define __CONTINUOUS      1         /* 连续 */
#define __DISCONTINUOUS   0         /* 不连续 */

/* 字模 */
aw_local uint8_t __g_my_words[] =
{0x00,0x00,0x00,0x18,0x3C,0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00,0x00,0x00,0x00,   /* "A" */
0x00,0x00,0x00,0x63,0x63,0x63,0x6B,0x6B,0x6B,0x36,0x36,0x36,0x00,0x00,0x00,0x00,    /* "W" */
0x00,0x00,0x00,0x00,0x00,0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00,    /* "o" */
0x00,0x00,0x00,0x00,0x00,0x66,0x6E,0x70,0x60,0x60,0x60,0x60,0x00,0x00,0x00,0x00,    /* "r" */
0x00,0x00,0x00,0x60,0x60,0x66,0x66,0x6C,0x78,0x6C,0x66,0x66,0x00,0x00,0x00,0x00,    /* "k" */
0x00,0x00,0x00,0x00,0x00,0x3E,0x60,0x60,0x3C,0x06,0x06,0x7C,0x00,0x00,0x00,0x00};   /* "s" */

aw_local uint8_t __g_num[13][16] = {
{0x00,0x00,0x00,0x1E,0x33,0x37,0x37,0x33,0x3B,0x3B,0x33,0x1E,0x00,0x00,0x00,0x00},  /* "0" */
{0x00,0x00,0x00,0x0C,0x1C,0x7C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x00,0x00,0x00,0x00},  /* "1" */
{0x00,0x00,0x00,0x3C,0x66,0x66,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00,0x00,0x00,0x00},  /* "2" */
{0x00,0x00,0x00,0x3C,0x66,0x66,0x06,0x1C,0x06,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},  /* "3" */
{0x00,0x00,0x00,0x30,0x30,0x36,0x36,0x36,0x66,0x7F,0x06,0x06,0x00,0x00,0x00,0x00},  /* "4" */
{0x00,0x00,0x00,0x7E,0x60,0x60,0x60,0x7C,0x06,0x06,0x0C,0x78,0x00,0x00,0x00,0x00},  /* "5" */
{0x00,0x00,0x00,0x1C,0x18,0x30,0x7C,0x66,0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},  /* "6" */
{0x00,0x00,0x00,0x7E,0x06,0x0C,0x0C,0x18,0x18,0x30,0x30,0x30,0x00,0x00,0x00,0x00},  /* "7" */
{0x00,0x00,0x00,0x3C,0x66,0x66,0x76,0x3C,0x6E,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},  /* "8" */
{0x00,0x00,0x00,0x3C,0x66,0x66,0x66,0x66,0x3E,0x0C,0x18,0x38,0x00,0x00,0x00,0x00},  /* "9" */
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x0C,0x18,0x00,0x00},  /* "," */
{0x00,0x00,0x00,0x0C,0x18,0x18,0x30,0x30,0x30,0x30,0x30,0x18,0x18,0x0C,0x00,0x00},  /* "(" */
{0x00,0x00,0x00,0x30,0x18,0x18,0x0C,0x0C,0x0C,0x0C,0x0C,0x18,0x18,0x30,0x00,0x00},  /* ")" */
};

aw_local uint16_t __g_pen_color;        /* 画笔颜色 */
aw_local size_t   __g_x_res;            /* x方向尺寸 */
aw_local size_t   __g_y_res;            /* y方向尺寸 */
aw_local uint16_t *__gp_frame_buffer;   /* frame buffer地址 */
aw_local void     * g_p_fb;
aw_fb_fix_info_t  g_fix_screen_info;
aw_fb_var_info_t  g_var_screen_info;

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

/**
 * \brief 绘制一个圆
 */
aw_local void __draw_circle (int x0, int y0, int pen_size)
{
    int x = 0;
    int y = 0;
    int x_min = 0;
    int y_min = 0;
    int x_max = 0;
    int y_max = 0;
    int r = 0;

    if (pen_size == 1) {
        __draw_point (x0, y0);
    } else if (pen_size < 1) {
        return;
    } else {
        r = pen_size/2;
    }

    x_min = x0 - r;
    x_max = x0 + r;
    y_min = y0 - r;
    y_max = y0 + r;

    for(y = y_min; y < y_max; y++) {
        for(x = x_min; x < x_max; x++) {
            if((x - x0)*(x - x0) + (y - y0)*(y - y0) <= r*r) {
                __gp_frame_buffer[y * __g_x_res + x] = __g_pen_color;
            }
        }
    }

}

/**
 * \brief 交换两个变量
 */
aw_local void swap(int *p1, int *p2)
{
    int t;

    t = *p1;
    *p1 = *p2;
    *p2 = t;
}

/**
 * \brief 从左下角到右上角画线
 */
aw_local void __draw_compensation1 (int x1, int y1, int x2, int y2, int size)
{
    int dx, dy, e;

    dx = x2 - x1;
    dy = y2 - y1;

    if (dx < 0) {
        dx =- dx;
    }

    if (dy < 0) {
        dy =- dy;
    }

    if (dx >= dy) {
        e = dy - dx/2;
        while (x1 <= x2) {
            __draw_circle(x1, y1, size);
            if (e > 0) {                          /* 从左下角到右上角，0 <= 度数 <= 45 */
                y1 += 1;
                e  -= dx;
            }
            x1 += 1;
            e  += dy;
        }
    } else {
        e = dx - dy/2;
        while (y1 <= y2) {
            __draw_circle(x1, y1, size);
            if(e > 0) {                          /* 从左下角到右上角，45 < 度数 <= 90 */
                x1 += 1;
                e  -= dy;
            }
            y1 += 1;
            e  += dx;

        }
    }

}

/**
 * \brief 从左上角到右下角画线
 */
aw_local void __draw_compensation2 (int x1, int y1, int x2, int y2, int size)
{
    int dx, dy, e;

    dx = x2 - x1;
    dy = y2 - y1;

    if (dx < 0) {
        dx =- dx;
    }

    if (dy < 0) {
        dy =- dy;
    }

    if (dx >= dy) {
        e = dy - dx/2;
        while (x1 <= x2) {
            __draw_circle(x1, y1, size);
            if (e > 0) {                         /* 从左上角到右下角，135 <= 度数 < 180 */
                y1 -= 1;
                e  -= dx;
            }
            x1 += 1;
            e  += dy;
        }
    } else {
        e = dx - dy/2;
        while (y1 >= y2) {
            __draw_circle(x1, y1, size);
            if (e > 0) {                         /* 从左上角到右下角，90 <= 度数 < 135 */
                x1 += 1;
                e  -= dy;
            }
            y1 -= 1;
            e  += dx;
        }
    }

}

/**
 * \brief 从右下角到左上角画线
 */
aw_local void __draw_compensation3 (int x1, int y1, int x2, int y2, int size)
{
    int dx, dy, e;

    dx = x2 - x1;
    dy = y2 - y1;

    if (dx < 0) {
        dx =- dx;
    }

    if (dy < 0) {
        dy =- dy;
    }

    if (dx >= dy) {
        e = dy - dx/2;
        while (x1 >= x2) {
            __draw_circle(x1, y1, size);
            if (e > 0) {                          /* 从右下角到左上角，135 <= 度数 <= 180 */
                y1 += 1;
                e  -= dx;
            }
            x1 -= 1;
            e  += dy;
        }
    } else {
        e = dx - dy/2;
        while (y1 <= y2) {
            __draw_circle(x1, y1, size);
            if (e > 0) {                         /* 从右下角到左上角，90 < 度数 < 135 */
                x1 -= 1;
                e  -= dy;
            }
            y1 += 1;
            e  += dx;
        }
    }

}

/**
 * \brief 从右上角到左下角画线
 */
aw_local void __draw_compensation4 (int x1, int y1, int x2, int y2, int size)
{
    int dx, dy, e;

    dx = x2 - x1;
    dy = y2 - y1;

    if (dx < 0) {
        dx =- dx;
    }

    if (dy < 0) {
        dy =- dy;
    }

    if (dx >= dy) {
        e = dy - dx/2;
        while (x1 >= x2) {
            __draw_circle(x1, y1, size);
            if (e > 0) {                        /* 从右上角到左下角，0 < 度数 <= 45 */
                y1 -= 1;
                e  -= dx;
            }
            x1 -= 1;
            e  += dy;
        }
    } else {
        e = dx - dy/2;
        while (y1 >= y2) {
            __draw_circle(x1, y1, size);
            if (e > 0) {                       /* 从右上角到左下角，45 < 度数 < 90 */
                x1 -= 1;
                e  -= dy;
            }
            y1 -= 1;
            e  += dx;
        }
    }

}

/**
 * \brief 绘制一条连续的线
 */
aw_local void __draw_line (int x1, int y1, int x2, int y2, int size)
{
    int dx, dy;

    dx = x2 - x1;
    dy = y2 - y1;
    if (dx >= 0) {
        if (dy >= 0) {
            __draw_compensation1 (x1, y1, x2, y2, size);
        } else {
            __draw_compensation2 (x1, y1, x2, y2, size);
        }
    } else {
        if (dy >= 0) {
            __draw_compensation3 (x1, y1, x2, y2, size);
        } else {
            __draw_compensation4 (x1, y1, x2, y2, size);
        }
    }

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

/**
 * \brief 绘制一个矩形
 */
aw_local void __draw_rect (int x1, int x2, int y1, int y2)
{
    int i;

    if (x1 > x2) {
        swap(&x1, &x2);
    }
    if (y1 > y2) {
        swap(&y1, &y2);
    }

    for (i = y1; i <= y2; i++) {
        __draw_h_line(x1, x2, i);
    }
}

/* 绘制文字 */
aw_local void __draw_words(int x, int y, uint8_t *p_words, int words_len)
{
    int i, j, k;
    for (i = 0; i < words_len / 16; i++) {
        for (j = 0; j < 16; j++) {
            for (k = 7; k >= 0; k--) {
                if ((p_words[i * 16 + j] >> k) & 1) {
                    __draw_point(x + (8 * i) + (7 - k), y + j);
                }
            }
        }
    }
}

/* 绘制x,y值 */
aw_local void __draw_xy(int x, int y, int x_num, int y_num)
{
    int i;
    char buf[10] = {0};
    uint8_t xy_num[144] = {0};
    static int len = 0;

    snprintf(buf, sizeof(buf), "(%d,%d)", x_num, y_num);

    /* 擦除原xy值 */
    __set_pen_color(0x00, 0x00, 0x00);
    __draw_rect(x, x+8*len, y, y+20);
    __set_pen_color(0xff, 0xff, 0xff);

    len = strlen(buf);

    for(i=0; i<len; i++) {
        switch(buf[i]) {
        case '0': memcpy(xy_num+i*16, __g_num[0],  16);break;
        case '1': memcpy(xy_num+i*16, __g_num[1],  16);break;
        case '2': memcpy(xy_num+i*16, __g_num[2],  16);break;
        case '3': memcpy(xy_num+i*16, __g_num[3],  16);break;
        case '4': memcpy(xy_num+i*16, __g_num[4],  16);break;
        case '5': memcpy(xy_num+i*16, __g_num[5],  16);break;
        case '6': memcpy(xy_num+i*16, __g_num[6],  16);break;
        case '7': memcpy(xy_num+i*16, __g_num[7],  16);break;
        case '8': memcpy(xy_num+i*16, __g_num[8],  16);break;
        case '9': memcpy(xy_num+i*16, __g_num[9],  16);break;
        case ',': memcpy(xy_num+i*16, __g_num[10], 16);break;
        case '(': memcpy(xy_num+i*16, __g_num[11], 16);break;
        case ')': memcpy(xy_num+i*16, __g_num[12], 16);break;
        default : break;
        }
    }

    __draw_words(x, y, xy_num, len*16);
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
            __clear_cursor(p_cal->log[i - 1].x, p_cal->log[i - 1].y, 10);
        }
        __draw_cursor(p_cal->log[i].x, p_cal->log[i].y, 10);


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

    __clear_cursor(p_cal->log[i - 1].x, p_cal->log[i - 1].y, 10);

    return 0;
}

aw_local aw_ts_id __touch_handel (void)
{
    aw_ts_id                sys_ts;
    aw_ts_lib_calibration_t cal;

    /* 获取触摸设备 */
    sys_ts = aw_ts_serv_id_get(__TS_NAME, 0, 0);

    if (sys_ts == NULL) {
        aw_kprintf("get touch server failed\r\n");
        while (1);
    }

    /* 判断设备是否支持触摸校准 */
    if (aw_ts_calc_flag_get(sys_ts)) {

        /* 加载系统触摸数据 */
        if (aw_ts_calc_data_read(sys_ts) != AW_OK) {

            /* 没有有效数据，进行校准 */
            do {
                __app_ts_calibrate(sys_ts, &cal);
            } while (aw_ts_calibrate(sys_ts, &cal) != AW_OK);

            /* 校准成功，保存触摸数据 */
            aw_ts_calc_data_write(sys_ts);
        }
    } else {

        /* 电容屏需要进行XY转换 */
        aw_ts_set_orientation(sys_ts, AW_TS_SWAP_XY);
    }

    return sys_ts;
}

aw_local void __draw_lcd(void)
{
    int i, ylen;
    uint8_t color[][3] = {{0x1f, 0x00, 0x00},   /* 红色 */
                          {0x00, 0x3f, 0x00},   /* 绿色 */
                          {0x00, 0x00, 0x1f},   /* 蓝色 */
                          {0x1f, 0x3f, 0x00},   /* 黄色 */
                          {0x1f, 0x00, 0x1f},   /* 紫色 */
                          {0x00, 0x3f, 0x1f},   /* 青色 */
                          {0x1f, 0x3f, 0x1f}};  /* 白色 */

    __g_x_res = g_var_screen_info.res.x_res;
    __g_y_res = g_var_screen_info.res.y_res;

    __gp_frame_buffer = (uint16_t *)g_fix_screen_info.vram_addr;

    memset((void *)g_fix_screen_info.vram_addr, 0, g_var_screen_info.buffer.buffer_size * g_var_screen_info.buffer.buffer_num);

    __set_pen_color(0x1f, 0, 0);    /* 绘制一条竖线 */
    __draw_v_line(g_var_screen_info.res.x_res / 8 * 7, 0, g_var_screen_info.res.y_res);
    __set_pen_color(0, 0x3f, 0);    /* 绘制一条横线 */
    __draw_h_line(0, g_var_screen_info.res.x_res / 4, g_var_screen_info.res.y_res / 2);

    /* 绘制彩条 */
    ylen = g_var_screen_info.res.y_res / AW_NELEMENTS(color);
    for (i = 0; i < AW_NELEMENTS(color); i++) {
        __set_pen_color(color[i][0], color[i][1], color[i][2]);
        __draw_rect(g_var_screen_info.res.x_res / 4,
                g_var_screen_info.res.x_res / 4 * 3,
                    ylen * i,
                    ylen * (i + 1));
    }
    /* 绘制文字 */
    __draw_words(0, 0, __g_my_words, sizeof(__g_my_words));

}

/**
 * \brief shell配置LCD时钟频率命令入口
 */
aw_local int __shell_lcd_dclk (int                   argc,
                               char                 *argv[],
                               struct aw_shell_user *p_shell_user)
{
    if (!(argc == 1)) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        aw_shell_printf(p_shell_user, "param:\r\n"
                                       "argv[0]:lcd clock frequency\r\n");
        aw_shell_printf(p_shell_user, "For Example:LCD_DCLK 48000000\r\n");
        return -AW_E2BIG;
    }

    /* 配置时钟频率 */
    g_var_screen_info.dclk_f.typ_dclk_f = atoi(argv[0]); /** \brief 典型时钟Hz */
    aw_fb_config(g_p_fb, AW_FB_CONF_DCLK_VINFO, &g_var_screen_info.dclk_f);

    /* frame buffer 设备信息获取 */
    aw_fb_ioctl(g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    return AW_OK;
}

/**
 * \brief shell配置LCD位像素深度命令入口
 */
aw_local int __shell_lcd_bpp (int                   argc,
                              char                 *argv[],
                              struct aw_shell_user *p_shell_user)
{
    if (!(argc == 2)) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        aw_shell_printf(p_shell_user, "param:\r\n"
                                      "argv[0]:lcd word length bpp \r\n"
                                      "argv[1]:lcd bus bpp\r\n");
        aw_shell_printf(p_shell_user, "For Example:LCD_BPP 16 16\r\n");
        return -AW_E2BIG;
    }

    /* 配置bpp */
    g_var_screen_info.bpp.word_lbpp     = atoi(argv[0]);
    g_var_screen_info.bpp.bus_bpp       = atoi(argv[1]);
    aw_fb_config(g_p_fb, AW_FB_CONF_BPP_VINFO, &g_var_screen_info.bpp);

    /* frame buffer 设备信息获取 */
    aw_fb_ioctl(g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    return AW_OK;
}

/**
 * \brief shell配置LCD水平同步信息命令入口
 */
aw_local int __shell_lcd_hsync (int                   argc,
                                char                 *argv[],
                                struct aw_shell_user *p_shell_user)
{
    if (!(argc == 4)) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        aw_shell_printf(p_shell_user, "param:\r\n"
                                      "argv[0]:lcd horizontal sync pulse width \r\n"
                                      "argv[1]:lcd horizontal active\r\n"
                                      "argv[2]:lcd horizontal front porch \r\n"
                                      "argv[3]:lcd horizontal back porch\r\n");
        aw_shell_printf(p_shell_user, "For Example:LCD_HSYNC 1 800 1000 46\r\n");
        return -AW_E2BIG;
    }

    /* 配置行同步信息 */
    g_var_screen_info.hsync.h_pulse_width = atoi(argv[0]);      /** \brief 行脉冲宽度 */
    g_var_screen_info.hsync.h_active      = atoi(argv[1]);      /** \brief 一行的像素个数 */
    g_var_screen_info.hsync.h_fporch      = atoi(argv[2]);      /** \brief 水平前沿 */
    g_var_screen_info.hsync.h_bporch      = atoi(argv[3]);      /** \brief 水平后沿 */
    aw_fb_config(g_p_fb, AW_FB_CONF_HSYNC_VINFO, &g_var_screen_info.hsync);

    /* frame buffer 设备信息获取 */
    aw_fb_ioctl(g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);


    return AW_OK;
}

/**
 * \brief shell配置LCD垂直同步信息命令入口
 */
aw_local int __shell_lcd_vsync (int                   argc,
                                char                 *argv[],
                                struct aw_shell_user *p_shell_user)
{
    if (!(argc == 4)) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        aw_shell_printf(p_shell_user, "param:\r\n"
                                      "argv[0]:lcd vertical sync pulse width \r\n"
                                      "argv[1]:lcd vertical active\r\n"
                                      "argv[2]:lcd vertical front porch \r\n"
                                      "argv[3]:lcd vertical back porch\r\n");
        aw_shell_printf(p_shell_user, "For Example:LCD_VSYNC 1 480 22 23\r\n");
        return -AW_E2BIG;
    }

    /* 配置垂直同步信息 */
    g_var_screen_info.vsync.v_pulse_width = atoi(argv[0]);      /** \brief 垂直脉冲宽度 */
    g_var_screen_info.vsync.v_active      = atoi(argv[1]);      /** \brief 一帧画面的行数 */
    g_var_screen_info.vsync.v_fporch      = atoi(argv[2]);      /** \brief 垂直前沿 */
    g_var_screen_info.vsync.v_bporch      = atoi(argv[3]);      /** \brief 垂直后沿 */
    aw_fb_config(g_p_fb, AW_FB_CONF_VSYNC_VINFO, &g_var_screen_info.vsync);

    /* frame buffer 设备信息获取 */
    aw_fb_ioctl(g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    return AW_OK;
}

/**
 * \brief shell配置LCD分辨率命令入口
 */
aw_local int __shell_lcd_res (int                   argc,
                              char                 *argv[],
                              struct aw_shell_user *p_shell_user)
{
    if (!(argc == 2)) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        aw_shell_printf(p_shell_user, "param:\r\n"
                                      "argv[0]:lcd resolution x\r\n"
                                      "argv[1]:lcd resolution y\r\n");
        aw_shell_printf(p_shell_user, "For Example:LCD_RES 800 480\r\n");
        return -AW_E2BIG;
    }

        /* 配置屏幕分辨率 */
    g_var_screen_info.res.x_res         = atoi(argv[0]);
    g_var_screen_info.res.y_res         = atoi(argv[1]);
    aw_fb_config(g_p_fb, AW_FB_CONF_RES_VINFO, &g_var_screen_info.res);

    /* frame buffer 设备信息获取 */
    aw_fb_ioctl(g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    return AW_OK;
}

/**
 * \brief shell重启LCD命令入口
 */
aw_local int __shell_lcd_reset (int                   argc,
                                char                 *argv[],
                                struct aw_shell_user *p_shell_user)
{
    if (argc > 0) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        return -AW_E2BIG;
    }

    /* 重启帧缓冲设备 */
    aw_fb_reset(g_p_fb);

    __draw_lcd();

    return AW_OK;
}

/**
 * \brief shell lcd控制命令表
 */
aw_local const aw_shell_cmd_item_t __g_my_lcd_cmds[] = {
    {__shell_lcd_dclk,   "LCD_DCLK",    "<lcd clock frequency>"},            /* 时钟频率 */
    {__shell_lcd_bpp,    "LCD_BPP",     "<lcd bit pixel depth>"},            /* 位像素深度 */
    {__shell_lcd_hsync,  "LCD_HSYNC",   "<lcd horizontal synchronization>"}, /* 水平同步 */
    {__shell_lcd_vsync,  "LCD_VSYNC",   "<lcd vertical synchronization>"},   /* 垂直同步 */
    {__shell_lcd_res,    "LCD_RES",     "<lcd resolution>"},                 /* 分辨率 */
    {__shell_lcd_reset,  "LCD_RESET",   "<lcd reset>"},                      /* 重启LCD */

};

/**
 * \brief shell任务入口
 * \param[in] p_arg : 任务参数
 * \return 无
 */
aw_local void __lcd_task (void *p_arg)
{
    static aw_shell_cmd_list_t  cmd_list;
    struct aw_ts_state          ts;
    aw_ts_id                    sys_ts;
    uint8_t                     state   = 0;
    uint32_t                    x0      = 0;
    uint32_t                    y0      = 0;

    /* 打开frame buffer */
    g_p_fb = aw_fb_open(__FB_NAME, 0);
    if (g_p_fb == NULL) {
        aw_kprintf("FB open failed.\r\n");
        return;
    }

    /* frame buffer 初始化 */
    aw_fb_init(g_p_fb);

    /* frame buffer 设备信息获取 */
    aw_fb_ioctl(g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    /* 打开背光并运行面板 */
    aw_fb_backlight(g_p_fb, 100);

    /* frame buffer设备信息开启 */
    aw_fb_start(g_p_fb);

    __g_x_res = g_var_screen_info.res.x_res;
    __g_y_res = g_var_screen_info.res.y_res;

    __gp_frame_buffer = (uint16_t *)g_fix_screen_info.vram_addr;
    memset((void *)g_fix_screen_info.vram_addr,
           0,
           g_var_screen_info.buffer.buffer_size
           * g_var_screen_info.buffer.buffer_num);

    /* 触屏校准 */
    sys_ts = __touch_handel();

    __draw_lcd();

    /* 添加自定义shell命令 */
    AW_SHELL_REGISTER_CMDS(&cmd_list, __g_my_lcd_cmds);

    aw_shell_system("aworks");  /* 执行aworks命令 */

    /* 触摸画线 */
    while (1) {

        /* 持续获取触摸状态 */
        if (aw_ts_exec(sys_ts, &ts, 1) > 0) {
            if (ts.x < __g_x_res && ts.y < __g_y_res) {

                __set_pen_color(0x1f, 0x3f, 0x1f); /* 设置画笔颜色 */

                if(state == __DISCONTINUOUS) {
                    __draw_point (ts.x, ts.y);     /* 画第一次触摸的点 */
                    state = __CONTINUOUS;
                } else {
                    __draw_line(x0, y0, ts.x, ts.y, 2);
                }

                x0 = ts.x; /* 保存上次触摸的横向值 */
                y0 = ts.y; /* 保存上次触摸的纵向值 */

                __draw_xy(0, __g_y_res-20, ts.x, ts.y);
            }

        } else {
            state = __DISCONTINUOUS;
        }

        aw_mdelay(5);
    }
}

/******************************************************************************/
void demo_lcd_16bpp_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Lcd demo",
                         12,
                         4096,
                         __lcd_task,
                         NULL);
    if (tsk != NULL) {
        aw_kprintf("lcd shell task create ok.\r\n");
        if (aw_task_startup(tsk) == AW_OK) {
            aw_kprintf("lcd shell task startup ok.\r\n");
        } else {
            aw_kprintf("lcd shell task startup fail.\r\n");
        }
    } else {
        aw_kprintf("lcd shell task create fail.\r\n");
    }

    return;
}

/** [src_lcd_16bpp] */

/* end of file */
