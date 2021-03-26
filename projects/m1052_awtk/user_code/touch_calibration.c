#include "aworks.h"
#include "aw_fb.h"
#include "aw_ts.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "string.h"
#include "aw_mem.h"
#include "aw_prj_params.h"

extern void aw_ts_cfg_init (void);

/* ��ɫ����*/
uint16_t color_group[5] = {0xFF80, 0x051D,0xE8C4,0xC618,0x2589};


/******************************************************************************/
/* ���㺯��*/
aw_err_t app_fbuf_pixel (int      x,
                         int      y,
                         uint8_t *screen_buf,
                         uint16_t color,
                         void*    p_fb)
{
    int                 idx;

    aw_fb_var_info_t var_info;
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &var_info);

    if ((x > var_info.res.x_res) || (y > var_info.res.y_res)) {
        return -AW_EINVAL;
    }

    idx = (y * var_info.res.x_res + x) * 2;
    memcpy(&screen_buf[idx], &color, sizeof(color));

    return AW_OK;
}

/* ���ߺ���*/
aw_err_t app_fbuf_line (int      x,
                        int      y,
                        uint8_t *screen_buf,
                        uint16_t color,
                        void*    p_fb)
{
    int  i;

    aw_fb_var_info_t var_info;
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &var_info);

    if ((x - 10 < 0) || (x + 10 >= var_info.res.x_res) ||
        (y - 10 < 0) || (y + 10 > var_info.res.y_res)) {
        return -AW_EINVAL;
    }

    for (i = 0; i < 20; i++) {

        app_fbuf_pixel( x - 10 + i, y,screen_buf, color, p_fb);
    }

    for (i = 0; i < 20; i++) {
        app_fbuf_pixel( x, y - 10 + i, screen_buf,color, p_fb);
    }

    return AW_OK;
}

/********************************************************************************/
/* У׼����*/
int ts_calibrate (uint8_t                 *screen_buf,
                  aw_ts_id                 id,
                  aw_ts_lib_calibration_t *p_cal,
                  void*                    p_fb)
{
    int                 i;
    struct aw_ts_state  sta;

    aw_fb_var_info_t var_info;
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &var_info);

    /* ��ʼ����㴥������ */
    p_cal->log[0].x = 60 - 1;
    p_cal->log[0].y = 60 - 1;
    p_cal->log[1].x = var_info.res.x_res  - 60 - 1;
    p_cal->log[1].y = 60 - 1;
    p_cal->log[2].x = var_info.res.x_res - 60 - 1;
    p_cal->log[2].y = var_info.res.y_res - 60 - 1;
    p_cal->log[3].x = 60 - 1;
    p_cal->log[3].y = var_info.res.y_res - 60 - 1;
    p_cal->log[4].x = var_info.res.x_res / 2 - 1;
    p_cal->log[4].y = var_info.res.y_res / 2 - 1;

    p_cal->cal_res_x = var_info.res.x_res;
    p_cal->cal_res_y = var_info.res.y_res;

    /* ��㴥�� */
    for (i = 0; i < 5; i++) {
        /* ������ͼ��*/
        app_fbuf_line (p_cal->log[i].x, p_cal->log[i].y, screen_buf, 0xFFFF, p_fb);

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
        /* ����ϸ���ͼ��*/
        app_fbuf_line (p_cal->log[i].x, p_cal->log[i].y, screen_buf, 0, p_fb);
    }
    return 0;
}


void ts_init (void* p_fb)
{
    aw_ts_id                sys_ts;
    aw_ts_lib_calibration_t cal;
    uint8_t                *p_buf = NULL;

    aw_fb_fix_info_t fix_info;
    aw_fb_var_info_t var_info;
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &fix_info);
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &var_info);

    /* ��ȡ�����豸 */
    sys_ts = aw_ts_serv_id_get(SYS_TS_ID, 0, 0);
    if (sys_ts == NULL) {
        return;
    }

    /* �������ͷ֡�ṹ��*/
    p_buf = (uint8_t *)fix_info.vram_addr;   /*emwin֡�����ַ*/
    /* ���ñ���Ϊ��ɫ*/
    memset(p_buf, 0, var_info.res.x_res * var_info.res.y_res *2);

    /* �ж��Ƿ��豸֧�ִ���У׼ */
    if (aw_ts_calc_flag_get(sys_ts)) {
        aw_ts_cfg_init ();
        /* ����ϵͳ�������� */
        if (aw_ts_calc_data_read(sys_ts) != AW_OK) {
            /* û����Ч���ݣ�����У׼ */
            do {
                if (ts_calibrate(p_buf, sys_ts, &cal, p_fb) < 0) {
                    //todo
                }
            } while (aw_ts_calibrate(sys_ts, &cal) != AW_OK);
            /* У׼�ɹ������津������ */
            aw_ts_calc_data_write(sys_ts);
        }
    } else {
        /* ����������4.3��������Ҫ����XYת�� */
        if(strcmp(SYS_TS_ID,"ft5x06")==0){
            aw_ts_set_orientation(sys_ts, AW_TS_SWAP_XY);
        }

    }
}

