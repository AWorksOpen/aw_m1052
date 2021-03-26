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
 * \brief ����ͷ����(raw��ʽ��ͨ��frame buffer��ʾ����)
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ��Ӧƽ̨�Ĵ��ں�
 *      - AW_COM_CONSOLE
 *      - AW_DEV_xxxx_CSI(xxxx�������Ӳ��ƽ̨����)
 *      - AW_DEV_OV7725
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ��awbl_hwconf_ov7725.h�����ļ����޸����ظ�ʽΪ BAYER_RAW;
 *   4. ʹ�ô�������SCREEN_800_480�豸�꣬ʹ��С������SCREEN_480_272��
 *
 * - ʵ������
 *   1. ���ڴ�ӡ������Ϣ��
 *   2. ��ʾ������ʾ����ͷ���㵽�Ļ��档
 *
 * - ��ע��
 *   1. ��������ڳ�ʼ��ʱ�������������ļ� awbl_hwconf_ov7725.h �� awbl_hwconf_xxxxx_csi.h
 *      (xxxx�������Ӳ��ƽ̨����)�в鿴��demo��ʹ��GPIO�Ƿ�������Ÿ���
 *   2. raw��ʽ��һ����������1���ֽڱ�ʾ;
 *   3. rgb565��ʽ��һ��������2���ֽڣ�
 *   4. rgb888��ʽ��һ��������3���ֽڱ�ʾ��
 *   5. ������ͷ�ֱ���Ϊ640 x 480 ����ʾ���ֱ��ʳߴ�Ϊ480 x 272�������(��������ͷ�ֱ���  > ��ʾ���ֱ���)��
 *      ����������ʾ������ʾ��һ֡����ֻ������ͷһ֡�����һ����
 *
 * \par Դ����
 * \snippet demo_camera_raw.c src_camera_raw
 *
 * \internal
 * \par Modification History
 * - 1.01 18-07-25 lqy, modification.
 * - 1.00 18-07-24 sls, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_camera_raw ����ͷ(raw��ʽ)
 * \copydoc demo_camera_raw.c
 */

/** [src_camera_raw] */

#include "aworks.h"
#include "aw_fb.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "raw2rgb.h"
#include "aw_camera.h"
#include "aw_system.h"
#include "aw_types.h"
#include <stdio.h>
#include <string.h>
#include "aw_task.h"
#include "aw_demo_config.h"

/* ����ͷ���� */
#define __CAMERA_NAME   DE_CAMERA_NAME

#define __TEST_DEV_FB   DE_FB

#define SCREEN_480_272
//#define SCREEN_800_480

/**
 * /brief����ͷ��ֱ����ؿ�� �� �߶�
 * ֧�ֵķֱ����У� 640 X 480 ��320 X 240
 */
#if defined SCREEN_480_272
#define __RESOLUTION_WIDTH          320
#define __RESOLUTION_HEIGHT         240
#else defined SCREEN_800_480
#define __RESOLUTION_WIDTH          640
#define __RESOLUTION_HEIGHT         480
#endif

/** \brief ����VIDEO��PHOTO�ķֱ��� */
#define __VIDEO_RES     AW_RESOLUTION_SET(__RESOLUTION_WIDTH, __RESOLUTION_HEIGHT)
#define __PHOTO_RES     AW_RESOLUTION_SET(__RESOLUTION_WIDTH, __RESOLUTION_HEIGHT)

void __test_camera_raw_task( void )
{
    int                     ret = 0;
    void                   *p_fb;
    aw_fb_fix_info_t        fix_screen_info;
    aw_fb_var_info_t        g_var_screen_info;
    struct aw_camera_cfg    cfg;
    void                   *buf = NULL;
    void                   *fd  = NULL;
    aw_tick_t               t1, t2;
    int                     frame_ms = 0;
    int                     i = 0;
    uint32_t                tmp = 0, pix = 0;
    uint8_t                *p_rgb888 = NULL;
    uint8_t                *p_rgb565 = NULL;
    uint8_t                *p_rgb888_temp ;
    uint8_t                *p_rgb565_temp ;
    uint32_t                opt;
    size_t                  __g_x_res;     /* ��ʾ��x����ߴ� */
    size_t                  __g_y_res;     /* ��ʾ��y����ߴ� */

    /* rgb888 ÿ������3���ֽ� */
    p_rgb888 = (uint8_t *)aw_mem_alloc(__RESOLUTION_WIDTH * __RESOLUTION_HEIGHT * 3);
    memset(p_rgb888, 0 , __RESOLUTION_WIDTH * __RESOLUTION_HEIGHT * 3);

    /* rgb565 ÿ������2���ֽ� */
    p_rgb565 = (uint8_t *)aw_mem_alloc(__RESOLUTION_WIDTH * __RESOLUTION_HEIGHT * 2 );
    memset(p_rgb565, 0 , __RESOLUTION_WIDTH * __RESOLUTION_HEIGHT * 2);

    /* ������ͷ */
    fd = aw_camera_open(__CAMERA_NAME);
    if (fd == NULL) {
        aw_kprintf("camera_open error! \n\r");
    }

    /* ��������ͷ�ֱ��� */
    cfg.photo_resolution = __PHOTO_RES;
    cfg.video_resolution = __VIDEO_RES;

    /* ʹ���Զ��عⲢ����Ŀ������ֵ */
    cfg.aec_enable = 1;
    cfg.brightness = 64;

    /* ʹ���Զ����� */
    cfg.awb_enable = 0;
    cfg.red_gain   = 84;
    cfg.blue_gain  = 72;
    cfg.green_gain = 64;

    /* ʹ�ܴ�ֱ��ת�뾵��ת */
    cfg.conversion = MIRROR|UPSIDE_DOWN;

    /* ��̬��������ͷ�����ʽ */
    cfg.format = BAYER_RAW;

    opt = AW_CAMERA_RESOLUTION | AW_CAMERA_EXPOSURE | AW_CAMERA_GAIN | \
          AW_CAMERA_CONVERSION | AW_CAMERA_FORMAT;

    ret = aw_camera_config_set(fd, &cfg, &opt);
        if(ret != AW_OK){
            aw_kprintf("aw_camera_cfg error,code:%d\r\n",ret);
            return ;
        }

    /* ��֡�����豸 */
    p_fb = aw_fb_open(__TEST_DEV_FB, 0);
    if (p_fb == NULL) {
        aw_kprintf("FB open failed.\r\n");
        return;
    }

    /* ��ʼ��֡�����豸 */
    aw_fb_init(p_fb);

    /* frame buffer �豸��Ϣ��ȡ */
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &fix_screen_info);
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);

    /* �򿪱��Ⲣ������� */
    aw_fb_backlight(p_fb, 100);

    aw_fb_start(p_fb);

    __g_x_res = g_var_screen_info.res.x_res;
    __g_y_res = g_var_screen_info.res.y_res;


    while (1) {
        t1 = aw_sys_tick_get();

        p_rgb888_temp = p_rgb888;
        p_rgb565_temp = p_rgb565;

        /* ��ȡ����ͷ��һ֡Ԥ����ַ */
        ret = aw_camera_get_video_buf(fd, &buf);
        if (ret != AW_OK) {
            aw_kprintf("get video error!\r\n");
            continue;
        }

        /* RAW��ʽתRGB888 */
        bayer_bilinear((unsigned char *)buf,
                       p_rgb888,
                       __RESOLUTION_WIDTH,
                       __RESOLUTION_HEIGHT,
                       COLOR_FILTER_RGGB);

        /* RGB888 ת RGB565 */
        pix =  __RESOLUTION_WIDTH * __RESOLUTION_HEIGHT;
        for (i = 0; i < pix; i++) {

            tmp = ((p_rgb888_temp[0] << 8) & 0xF800) |
                  ((p_rgb888_temp[1] << 3) & 0X7E0)  |
                  ((p_rgb888_temp[2] >> 3));

            p_rgb888_temp += 3;
            *(uint16_t*)p_rgb565_temp = (uint16_t)tmp;
            p_rgb565_temp += 2;
        }

        /* �ж���ʾ���ĳߴ磬���㿽���ĳߴ� ����LCD����ʾͼ�� */
            if (__VIDEO_RES == AW_RESOLUTION_SET(320, 240)) {
                for (i = 0; i < 240; i ++) {
                    memcpy((uint8_t*)fix_screen_info.vram_addr + __g_x_res * 2 * i,
                           (uint8_t*)p_rgb565 + 320 * 2 * i,
                           320 * 2);
                }
            } else if (__VIDEO_RES == AW_RESOLUTION_SET(640, 480)) {
                for (i = 0; i < ((__g_y_res >= 480) ? 480 : __g_y_res); i ++) {
                    memcpy((uint8_t*)fix_screen_info.vram_addr + __g_x_res * 2 * i,
                           (uint8_t*)p_rgb565 + 640 * 2 * i,
                           ((__g_x_res > 640) ? 640 : __g_x_res) * 2);
                }
            }

        /* �ͷ�����ͷ��һ֡Ԥ����ַ */
        ret = aw_camera_release_video_buf(fd, buf);
        if (ret != AW_OK) {
            aw_kprintf("get relase video error!\r\n");
            continue;
        }

        /* ��ȡ����ͷ�ع�ֵ */
        ret = aw_camera_config_get(fd, &cfg, &opt);
        if (opt & AW_CAMERA_EXPOSURE) {
            aw_kprintf("exposure:%d\r\n", cfg.exposure);
        } else {
            aw_kprintf("%s do not support inquired\r\n", __CAMERA_NAME);
        }

        /* ����֡�� */
        t2 = aw_sys_tick_get();
        frame_ms = 1000 / aw_ticks_to_ms(t2 - t1);
        sprintf(buf, "%d f/s.\n", frame_ms);
        aw_kprintf("%s\r\n", buf);
    }
}

/**
 * /brief ����ͷ(raw��ʽ)������ں���
 */
void demo_camera_raw_entry (void)
{

    AW_TASK_DECL_STATIC(__camera_raw_lcd_show, 1024 * 20 );

    /* ��ʼ������LCD��ʾ���� */
    AW_TASK_INIT(__camera_raw_lcd_show,     /* ����ʵ�� */
                 "__camera_lcd_show",       /* �������� */
                 12,                        /* �������ȼ� */
                 1024 * 10 ,                /* �����ջ��С */
                 __test_camera_raw_task,    /* ������ں��� */
                 (void *)NULL);             /* ������ڲ��� */

    /* ��������__camera_lcd_show */
    AW_TASK_STARTUP(__camera_raw_lcd_show);

}

/** [src_camera_raw] */

/* end of file */
