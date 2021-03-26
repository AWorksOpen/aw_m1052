/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief Freescale iMX RT1050 LCD����������
 *
 * \internal
 * \par modification history:
 * - 1.01  19-05-06  cat, mod interface.
 * - 1.00  17-11-21  mex, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX1050_LCDIF_H
#define __AWBL_IMX1050_LCDIF_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_spinlock.h"
#include "driver/intensity/awbl_pwm_it.h"
#include "driver/display/awbl_imx1050_lcdif_regs.h"
#include "aw_fb.h"
#include "aw_clk.h"
#include "aw_sem.h"

#define AWBL_MXS_LCDIF_PANEL_SYSTEM      0
#define AWBL_MXS_LCDIF_PANEL_VSYNC       1
#define AWBL_MXS_LCDIF_PANEL_DOTCLK      2
#define AWBL_MXS_LCDIF_PANEL_DVI         3

#define AWBL_FB_HORIZONTAL_SCREEN        0    /* ���� */
#define AWBL_FB_VERTICAL_SCREEN          1    /* ���� */

/** \brief ��ȡ����豸�����ֺ͵�Ԫ�� */
typedef aw_err_t (* awbl_imx1050_lcdif_obtain_panel_t) (const char **pp_name, int *p_unit);

/** \brief LCDIF�豸��Ϣ */
typedef struct awbl_imx1050_lcdif_devinfo {

    const char          *default_panel_name; /**< \brief Ĭ������� */
    int                  default_panel_unit; /**< \brief Ĭ����嵥Ԫ�� */
    int                  default_pwm_num;    /**< \brief Ĭ�ϱ�����Ƶ�PWM�� */
    int                  gpio_backlight;     /**< \brief GPIO�ܽű������ */

    /** \brief ����ʹ���ĸ���� (ΪNULL��ʧ��ʱ��ʹ��Ĭ�����) */
    aw_err_t           (*pfunc_decide_panel) (const char **pp_name, int *p_unit);
    uint32_t            regbase;             /**< \brief LCDIF�Ĵ�������ַ */
    int                 inum;                /**< \brief LCD�жϺ� */
    const aw_clk_id_t  *dev_clks;            /**< \brief the clocks which the timer need */
    int                 dev_clk_count;       /**< \brief count of device clocks */
    aw_clk_id_t         ref_pixel_clk_id;    /**< \brief pixel clocks */

} awbl_imx1050_lcdif_devinfo_t;


struct awbl_imx1050_panel;

/** \brief LCDIF�豸 */
typedef struct awbl_imx1050_lcdif {

    awbl_dev_t dev;                          /** \brief �̳���AWBus */
    awbl_imx1050_lcdif_devinfo_t *p_devinfo; /** \brief LCDIF �豸��Ϣ */
    struct awbl_imx1050_panel    *p_panel;   /** \brief ʹ�õ���� */

    AW_SEMB_DECL(lcd_sem_sync);              /** \brief ����֡ͬ�����ź��� */

} awbl_imx1050_lcdif_t;

typedef struct awbl_imx1050_dotclk_timing {

    uint16_t    h_pulse_width;  /** \brief �������� */
    uint16_t    h_active;       /** \brief һ�е����ظ��� */
    uint16_t    h_bporch;       /** \brief ˮƽǰ�� */
    uint16_t    h_fporch;       /** \brief ˮƽ���� */
    uint16_t    v_pulse_width;  /** \brief ��ֱ������ */
    uint16_t    v_active;       /** \brief һ֡��������� */
    uint16_t    v_bporch;       /** \brief ��ֱǰ�� */
    uint16_t    v_fporch;       /** \brief ��ֱ���� */
    int         enable_present;

} awbl_imx1050_dotclk_timing_t;

/** \brief panel information */
typedef struct awbl_imx1050_panel_info {

    uint16_t x_res;          /** \brief ��Ļ��� */
    uint16_t y_res;          /** \brief ��Ļ�߶� */
    uint16_t bus_bpp;        /** \brief rgbӲ�������� */
    uint16_t word_lbpp;      /** \brief һ�����صĳ��ȣ�λΪ��λ,��rgb565���ݣ���ֵΪ16 */
    uint32_t dclk_f;         /** \brief ʱ��Ƶ�� */
    int32_t  lcd_type;       /** \brief ʱ��Դ���� */
    int32_t  bl_percent;     /** \brief Ĭ�ϵı������� */
    int32_t  dis_direction;  /** \brief ��Ļ���� */

} awbl_imx1050_panel_info_t;


struct awbl_imx1050_panel;

typedef struct awbl_imx1050_panel {

    awbl_imx1050_lcdif_t            *p_lcdif;
    awbl_imx1050_panel_info_t       *p_panel_info;
    void                            *p_cookie;

} awbl_imx1050_panel_t;

aw_err_t awbl_imx1050_lcdif_init(awbl_imx1050_lcdif_t *p_lcdif, uint8_t bus_bpp, uint8_t word_lbpp);
aw_err_t awbl_imx1050_lcdif_run(awbl_imx1050_lcdif_t *p_lcdif);
aw_err_t awbl_imx1050_lcdif_stop(awbl_imx1050_lcdif_t *p_lcdif);
aw_err_t awbl_imx1050_lcdif_pan_triple_vram_addr_set(awbl_imx1050_lcdif_t *p_lcdif, uint32_t addr);
aw_err_t awbl_imx1050_lcdif_pan_common_vram_addr_set(awbl_imx1050_lcdif_t *p_lcdif, uint32_t addr);
aw_err_t awbl_imx1050_lcdif_pan_try_vram_addr_set(awbl_imx1050_lcdif_t *p_lcdif, uint32_t addr);
aw_err_t awbl_imx1050_lcdif_pan_blank(awbl_imx1050_lcdif_t *p_lcdif, int blank);

aw_err_t awbl_imx1050_lcdif_dma_init(awbl_imx1050_lcdif_t *p_lcdif, uint32_t phys, size_t memsize);
aw_err_t awbl_imx1050_lcdif_dma_release(awbl_imx1050_lcdif_t *p_lcdif);

aw_err_t awbl_imx1050_lcdif_setup_dotclk_panel (awbl_imx1050_lcdif_t *p_lcdif, aw_fb_var_info_t *p_vinfo);
aw_err_t awbl_imx1050_lcdif_release_dotclk_panel (awbl_imx1050_lcdif_t *p_lcdif);

aw_err_t __imx_lcdif_common_buf_isr(void *p_dev);
aw_err_t __imx_lcdif_triple_buf_isr(void *p_dev);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_IMX1050_LCDIF_H */

/* end of file */

