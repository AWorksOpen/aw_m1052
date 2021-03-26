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
 * \brief AWorks frame buffer implementation
 *
 * \internal
 * \par modification history:
 * - 2.00 19-04-29  cat, redesign frame interface
 * - 1.00 14-08-05  ops, add five interface
 * - 1.00 14-05-30  zen, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_fb.h"

#include <string.h>

/******************************************************************************/

AWBL_METHOD_IMPORT(awbl_fb_get);

void *aw_fb_open(const char *name, int unit)
{
    awbl_dev_t            *p_dev         = NULL;
    awbl_method_handler_t  pfunc_handler = NULL;
    awbl_fb_info_t        *p_fb_info     = NULL;

    /* find device by name */
    p_dev = awbl_dev_find_by_name(name, unit);
    if (p_dev == NULL) {
        return NULL;
    }

    /* find fb method on the device */
    pfunc_handler = awbl_dev_method_get(p_dev, AWBL_METHOD_CALL(awbl_fb_get));
    if (pfunc_handler == NULL) {
        return NULL;
    }

    /* get fb_info */
    pfunc_handler(p_dev, &p_fb_info);

    return (void *)p_fb_info;
}

aw_err_t aw_fb_init(void *p_fb)
{
    uint32_t panel_x_res, panel_y_res, vram_size, bpp_bits;
    awbl_fb_info_t *p_fb_info    = (awbl_fb_info_t *)p_fb;
    aw_fb_fix_info_t *p_fb_finfo = (aw_fb_fix_info_t *)&p_fb_info->fix_info;
    aw_fb_var_info_t *p_fb_vinfo = (aw_fb_var_info_t *)&p_fb_info->var_info;

    if (NULL == p_fb_info) {
        return -AW_ENXIO;
    }

    bpp_bits    = (p_fb_vinfo->bpp.word_lbpp + 7) / 8;
    panel_x_res =  p_fb_vinfo->res.x_res;
    panel_y_res =  p_fb_vinfo->res.y_res;

    p_fb_vinfo->buffer.buffer_size = panel_x_res * panel_y_res * bpp_bits;

    do {
        vram_size = p_fb_vinfo->buffer.buffer_size * p_fb_vinfo->buffer.buffer_num;

        if (vram_size <= p_fb_finfo->vram_max) {

            return p_fb_info->p_fb_ops->fb_init(p_fb_info);
        }

    } while (--(p_fb_vinfo->buffer.buffer_num));

    return -AW_ENOMEM;
}

aw_err_t aw_fb_start(void *p_fb)
{
    awbl_fb_info_t *p_fb_info = (awbl_fb_info_t *)p_fb;

    if (NULL == p_fb_info) {
        return -AW_ENXIO;
    }

    return p_fb_info->p_fb_ops->fb_start(p_fb_info);
}

aw_err_t aw_fb_stop(void *p_fb)
{
    awbl_fb_info_t *p_fb_info = (awbl_fb_info_t *)p_fb;

    if (NULL == p_fb_info) {
        return -AW_ENXIO;
    }

    return p_fb_info->p_fb_ops->fb_stop(p_fb_info);
}

aw_err_t aw_fb_reset(void *p_fb)
{
    awbl_fb_info_t *p_fb_info = (awbl_fb_info_t *)p_fb;

    if (NULL == p_fb_info) {
        return -AW_ENXIO;
    }

    return p_fb_info->p_fb_ops->fb_reset(p_fb_info);
}

uint32_t aw_fb_get_online_buf(void *p_fb)
{
    awbl_fb_info_t *p_fb_info = (awbl_fb_info_t *)p_fb;

    if (NULL == p_fb_info) {
        return -AW_ENXIO;
    }

    return p_fb_info->p_fb_ops->fb_get_online_buf(p_fb_info);
}

uint32_t aw_fb_get_offline_buf(void *p_fb)
{
    awbl_fb_info_t *p_fb_info = (awbl_fb_info_t *)p_fb;

    if (NULL == p_fb_info) {
        return -AW_ENXIO;
    }

    return p_fb_info->p_fb_ops->fb_get_offline_buf(p_fb_info);
}

aw_err_t aw_fb_swap_buf(void *p_fb)
{
    awbl_fb_info_t *p_fb_info = (awbl_fb_info_t *)p_fb;

    if (NULL == p_fb_info) {
        return -AW_ENXIO;
    }

    return p_fb_info->p_fb_ops->fb_swap_buf(p_fb_info);
}

aw_err_t aw_fb_try_swap_buf(void *p_fb)
{
    awbl_fb_info_t *p_fb_info = (awbl_fb_info_t *)p_fb;

    if (NULL == p_fb_info) {
        return -AW_ENXIO;
    }

    return p_fb_info->p_fb_ops->fb_try_swap_buf(p_fb_info);
}

aw_err_t aw_fb_blank(void *p_fb, aw_fb_black_t blank)
{
    awbl_fb_info_t *p_fb_info = (awbl_fb_info_t *)p_fb;

    if (NULL == p_fb_info) {
        return -AW_ENXIO;
    }

    return p_fb_info->p_fb_ops->fb_blank(p_fb_info, blank);
}

aw_err_t aw_fb_backlight (void *p_fb, int level)
{
    awbl_fb_info_t *p_fb_info = (awbl_fb_info_t *)p_fb;

    if (NULL == p_fb_info) {
        return -AW_ENXIO;
    }

    return p_fb_info->p_fb_ops->fb_backlight(p_fb_info, level);
}

aw_err_t aw_fb_config (void *p_fb, aw_fb_config_t conf, void *p_arg)
{
    awbl_fb_info_t *p_fb_info = (awbl_fb_info_t *)p_fb;

    if (NULL == p_fb_info) {
        return -AW_ENXIO;
    }

    switch (conf) {

    case AW_FB_CONF_DCLK_VINFO:
        memcpy(&(p_fb_info->var_info.dclk_f),
                (aw_fb_var_dclk_info_t *)p_arg,
                sizeof(aw_fb_var_dclk_info_t));
        break;

    case AW_FB_CONF_RES_VINFO:
        memcpy(&(p_fb_info->var_info.res),
                (aw_fb_var_res_info_t *)p_arg,
                sizeof(aw_fb_var_res_info_t));
        break;

    case AW_FB_CONF_BPP_VINFO:
        memcpy(&(p_fb_info->var_info.bpp),
                (aw_fb_var_bpp_info_t *)p_arg,
                sizeof(aw_fb_var_bpp_info_t));
        break;

    case AW_FB_CONF_HSYNC_VINFO:
        memcpy(&(p_fb_info->var_info.hsync),
                (aw_fb_var_hsync_info_t *)p_arg,
                sizeof(aw_fb_var_hsync_info_t));
        break;

    case AW_FB_CONF_VSYNC_VINFO:
        memcpy(&(p_fb_info->var_info.vsync),
                (aw_fb_var_vsync_info_t *)p_arg,
                sizeof(aw_fb_var_vsync_info_t));
        break;

    case AW_FB_CONF_BUFFER_VINFO:
        memcpy(&(p_fb_info->var_info.buffer),
                (aw_fb_var_buffer_info_t *)p_arg,
                sizeof(aw_fb_var_buffer_info_t));
        break;

//    case AW_FB_CONF_ORIENTATION_VINFO:
//        p_fb_info->var_info.orientation = *(uint32_t *)p_arg;
//        break;

    default:
        return -AW_EINVAL;
    }

    return p_fb_info->p_fb_ops->fb_config(p_fb_info);
}

aw_err_t aw_fb_ioctl(void *p_fb, aw_fb_iocmd_t cmd, void *p_arg)
{
    awbl_fb_info_t *p_fb_info = (awbl_fb_info_t *)p_fb;

    if (NULL == p_fb_info) {
        return -AW_ENXIO;
    }

    switch (cmd) {

    case AW_FB_CMD_GET_FINFO:
        *(aw_fb_fix_info_t *)p_arg = p_fb_info->fix_info;
        break;

    case AW_FB_CMD_GET_VINFO:
        *(aw_fb_var_info_t *)p_arg = p_fb_info->var_info;
        break;

    case AW_FB_CMD_SET_VINFO:
        p_fb_info->var_info = *(aw_fb_var_info_t *)p_arg;
        p_fb_info->p_fb_ops->fb_config(p_fb_info);
        p_fb_info->p_fb_ops->fb_reset(p_fb_info);
        break;

    default:
        return -AW_EINVAL;
    }

    return AW_OK;
}

/* end of file */
