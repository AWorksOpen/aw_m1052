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
 * \brief ͨ��shell�������LCD��ʾ���ʹ�����
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *      - ��Ӧƽ̨�Ĵ��ں�
 *      - ��Ӧ��ʾ���ĺ�
 *      - ��Ӧ�������ĺ�
 *
 * - ʵ������
 *   1. ����".test"�������
 *   2. ִ�� "lcd -c"��ѡ����Ļ������ɫ
 *   3. ִ�� "lcd -v"  ����Ļ����佥��ɫ����ֱ�ݶȣ�
 *   4. ִ�� "lcd -h"  ����Ļ����佥��ɫ��ˮƽ�ݶȣ�
 *   5. ִ�� "lcd -j"  ��ĻУ׼
 *   6. ִ�� "lcd -b +���⼶�� ",������Ļ���⣬���⼶��Ϊ��0-100������Խ������Խ�ߣ������ڴ˷�Χ�İ�100��
 *
 * - ��ע��
 *   1. ��ɫ����ֱ������16�������������0xǰ׺��һЩ���õ���ɫҲ��ֱ��������ɫ��
 *
 * \par Դ����
 * \snippet app_buzzer.c app_buzzer
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_lcd] */
#include "aworks.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
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
#define __TFT_NAME       DE_TS_NAME              /*���败����*/
#else
#define __TFT_NAME       DE_CAP_TS_NAME              /*���ݴ�����*/
#endif

#define __TFB_NAME       DE_FB                       /*��ʾ������*/

aw_local uint16_t        __g_pen_color;             /* ������ɫ */
aw_local size_t          __g_x_res;                 /* x����ߴ� */
aw_local size_t          __g_y_res;                 /* y����ߴ� */
aw_local uint16_t        *__gp_frame_buffer;        /* frame buffer��ַ */


///////////////////////////////////////////////////////////////
/* ���û�����ɫ */
aw_local void __set_pen_color (uint8_t r, uint8_t g, uint8_t b)
{
    __g_pen_color = (r << 11) | (g << 6) | b;
}

/**
 * \brief ����һ����
 */
aw_local void __draw_point (int x, int y)
{
    __gp_frame_buffer[y * __g_x_res + x] = __g_pen_color;
}

/* ������������ */
aw_local void swap(int *p1, int *p2)
{
    int t;

    t = *p1;
    *p1 = *p2;
    *p2 = t;
}

/**
 * \brief ����һ������
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
 * \brief ����һ������
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


/* ���ƹ�꣨���׹�꣩ */
aw_local void __draw_cursor (int x, int y, int size)
{
    __set_pen_color(0x1f, 0x3f, 0x1f);
    __draw_h_line(x - size, x + size, y);
    __draw_v_line(x, y - size, y + size);
}

/* ����Ĳ�����Ҫ�ͻ��ƹ��ʱ�����һģһ�� */
aw_local void __clear_cursor (int x, int y, int size)
{
    __set_pen_color(0, 0, 0);
    __draw_h_line(x - size, x + size, y);
    __draw_v_line(x, y - size, y + size);
}


/* ����У׼ */
aw_local int __app_ts_calibrate (aw_ts_id                 id,
                               aw_ts_lib_calibration_t *p_cal)
{
    int                 i;
    struct aw_ts_state  sta;

    /* ��ʼ���������� */
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

    /* ��㴥�� */
    for (i = 0; i < 5; i++) {
        /* ��ʾ��� */

        if (i != 0) {
            __clear_cursor(p_cal->log[i - 1].x, p_cal->log[i - 1].y, 5);
        }
        __draw_cursor(p_cal->log[i].x, p_cal->log[i].y, 5);


        while (1) {
            /* �ȴ���ȡ�������λ�ô������� */
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

    /* ��ȡ�����豸 */
    sys_ts = aw_ts_serv_id_get(__TFT_NAME, 0, 0);

    if (sys_ts == NULL) {
        aw_kprintf("get touch server failed\r\n");
        while (1);
    }

    /* �ж��Ƿ��豸֧�ִ���У׼ */
//    if (aw_ts_calc_flag_get(sys_ts)) {
//        /* ����ϵͳ�������� */
//        if (aw_ts_calc_data_read(sys_ts) != AW_OK) {
//            /* û����Ч���ݣ�����У׼ */
            do {
                __app_ts_calibrate(sys_ts, &cal);
            } while (aw_ts_calibrate(sys_ts, &cal) != AW_OK);
            /* У׼�ɹ������津������ */
            aw_ts_calc_data_write(sys_ts);
//        }
//    } else {
//        /* ��������Ҫ����XYת�� */
//        aw_ts_set_orientation(sys_ts, AW_TS_SWAP_XY);
//    }

    return sys_ts;
}

static aw_fb_fix_info_t           fix_screen_info;
static aw_fb_var_info_t           var_screen_info;

aw_local aw_ts_id __g_lcd_init(void)
{


    void                       *p_fb = NULL;

    /* ��frame buffer */
    p_fb = aw_fb_open(__TFB_NAME, 0);
    if (p_fb == NULL) {
        aw_kprintf("FB open failed.\r\n");
        return p_fb;
    }

    /*frame buffer ��ʼ��*/
    aw_fb_init(p_fb);

    /*frame buffer �豸��Ϣ��ȡ*/
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_VINFO,&var_screen_info);
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_FINFO,&fix_screen_info);

    /* �򿪱��Ⲣ������� */
    aw_fb_backlight(p_fb, 100);

    /*frame buffer�豸��Ϣ����*/
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

     p_fb = __g_lcd_init();    //��Ļ��ʼ��
     if(p_fb == NULL){
         return -AW_EAGAIN;
     }

     while (1) {
        if (aw_shell_args_get(p_user, "-j", 0) == AW_OK) {
            /* ��ĻУ׼ */
             __touch_handel();
        }
        else if (aw_shell_args_get(p_user, "-b", 1) == AW_OK) {
            /* ��frame buffer */
            bk_bright = aw_shell_long_arg(p_user, 1);
            aw_fb_backlight(p_fb, bk_bright);    /*�������*/
            memset((void *)fix_screen_info.vram_addr,
                   0xFF,
                   var_screen_info.buffer.buffer_size
                   * var_screen_info.buffer.buffer_num);
        }else if (aw_shell_args_get(p_user, "-c", 0) == AW_OK) {
            for(x_i=0; x_i<__g_x_res; x_i++) {
                __set_pen_color(0x1f, 0x00, 0x00); /* ���� */
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
                __set_pen_color(0, 0, x_i);       /* ���� */
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
