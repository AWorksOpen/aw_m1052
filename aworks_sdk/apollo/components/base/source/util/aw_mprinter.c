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
 * \brief zytp drivers implementation
 *
 * \internal
 * \par Modification history
 * - 1.00 15-10-25  bob, first implementation.
 * \endinternal
 */


#include "aw_mprinter.h"
#include "aw_ioctl.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/aw_stropts.h"

/** \brief CAN Cancel print data in page mode ҳģʽ��ɾ����ӡ����*/
aw_err_t aw_mprinter_page_data_cancel(int     fd)
{
    if (fd < 0) {
        return -AW_EINVAL;
    }

    return aw_ioctl(fd, AW_MPPGCANDAT, NULL);
}

/** \briefESC % Select/cancel user-defined character set ѡ��/ȡ���û��Զ����ַ�*/
aw_err_t aw_mprinter_user_defch_select(int     fd)
{

    aw_mprinter_params_t params;
    uint8_t            data = 1;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSRUDCHDEFS, &params);
}

aw_err_t aw_mprinter_user_defch_cancel(int     fd)
{

    aw_mprinter_params_t params;
    uint8_t            data = 0;
    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSRUDCHDEFS, &params);
}
/** \briefESC & Define user-defined characters  �����û��Զ����ַ�*/
aw_err_t aw_mprinter_user_char_def(int      fd,
                                   uint8_t  ver_bytes,
                                   uint8_t  char_code_beg,
                                   uint8_t  char_code_end,
                                   uint8_t *p_code_buf)
{

    aw_mprinter_params_t params;
    uint8_t            data[3] = {ver_bytes,
                                  char_code_beg,
                                  char_code_end
                                 };

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         3,
                         p_code_buf,

                         ver_bytes *
                         (char_code_end - char_code_beg + 1) *
                         (char_code_end - char_code_beg + 1),

                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSUDCHDEF, &params);
}

/** \briefESC ? Cancel user-defined characters  ȡ���û��Զ����ַ�*/
aw_err_t aw_mprinter_user_char_canceldef(int     fd,
                                         uint8_t char_code)
{

    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &char_code,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPRUDCHDEF, &params);
}

/** \briefESC G Turn double-strike mode on/off  ����˫�ش�ӡģʽ�Ŀ�/��*/
aw_err_t aw_mprinter_dbl_strike_on(int     fd)
{

    aw_mprinter_params_t params;
    uint8_t            cfg = 1;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &cfg,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSRDBLSTKMD, &params);
}

aw_err_t aw_mprinter_dbl_strike_off(int     fd)
{

    aw_mprinter_params_t params;
    uint8_t            cfg = 0;
    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &cfg,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSRDBLSTKMD, &params);
}

/** \briefGS ( N Select character effects  ѡ���ַ�Ч��*/
aw_err_t aw_mprinter_char_effects_select(int                             fd,
                                         aw_mprinter_char_effect_type_t  type,
                                         uint8_t                         color)
{

    aw_mprinter_params_t params;
    uint8_t            data[5]  = {0x02, 0x00, type, color};
    uint8_t            data_len = 4;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    switch(type) {
    case AW_MPRINTER_EFFECT_CHAR_COLOR:

        break;
    case AW_MPRINTER_EFFECT_BACKGROUND_COLOR:
        break;
    case AW_MPRINTER_EFFECT_SHADING_ON:
        data[4] = data[3];
        data[3] = 0;
        data_len = 5;
        break;
    case AW_MPRINTER_EFFECT_SHADING_OFF:
        data[4] = data[3];
        data[3] = 0;
        data_len = 5;
        break;
    default :
        break;

    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         data_len,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSCHEFCT, &params);
}

/** \briefGS b Turn smoothing mode on/off   �趨/���ƽ��ģʽ*/
aw_err_t aw_mprinter_smoothing_mode_on(int     fd)
{

    aw_mprinter_params_t params;
    uint8_t            cfg = 1;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &cfg,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSRSMTHMD, &params);
}

aw_err_t aw_mprinter_smoothing_mode_off(int     fd)
{

    aw_mprinter_params_t params;
    uint8_t            cfg = 0;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &cfg,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSRSMTHMD, &params);
}

/** \brief ESC T Select print direction in page mode ҳ��ģʽ��ѡ���ӡ����*/
aw_err_t aw_mprinter_page_print_dir_set(int                     fd,
                                        aw_mprinter_pdir_type_t type)
{

    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                       (uint8_t *)(&type),
                        1,
                        NULL,
                        0,
                        NULL,
                        0,
                        0);

    return aw_ioctl(fd, AW_SMPPGMDIR, &params);
}
/** \brief ESC W Set print area in page mode ҳ��ģʽ��ѡ���ӡ����*/
aw_err_t aw_mprinter_page_print_area_set(int       fd,
                                         uint16_t  origin_x,
                                         uint16_t  origin_y,
                                         uint16_t  dx,
                                         uint16_t  dy)

{

    aw_mprinter_params_t params;
    uint8_t            data[8];

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = origin_x & 0xFF;
    data[1] = (origin_x >> 8) & 0xFF;
    data[2] = origin_y & 0xFF;
    data[3] = (origin_y >> 8) & 0xFF;
    data[4] = dx & 0xFF;
    data[5] = (dx >> 8) & 0xFF;
    data[6] = dy & 0xFF;
    data[7] = (dy >> 8) & 0xFF;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         8,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSPGMDPAR, &params);
}

aw_err_t aw_mprinter_relative_ppos_set (int           fd,
                                        uint16_t      post)
{
    aw_mprinter_params_t params;
    uint8_t            data[2];

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = post & 0xFF;
    data[1] = (post >> 8) & 0xFF;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSRLTPP, &params);
}

aw_err_t aw_mprinter_page_absver_ppost_set (int           fd,
                                            uint8_t       post)
{

    aw_mprinter_params_t params;
    uint8_t            data[2];

    if (fd < 0) {
        return -AW_EINVAL;
    }



    data[0] = post & 0xFF;
    data[1] = (post >> 8) & 0xFF;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSPGMDABSVPP, &params);
}

/** \brief ҳģʽ�����ô�ֱ���λ��*/
aw_err_t aw_mprinter_page_vrtcl_post_set(int      fd,
                                         uint16_t post)
{
    aw_mprinter_params_t params;
    uint8_t            data[2];

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = post & 0xFF;
    data[1] = (post >> 8) & 0xFF;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSPGMDRESVPP, &params);
}
/** \brief ��������ͼ*/

aw_err_t aw_mprinter_download_bmg_def(int      fd,
                                      uint8_t  x_bytes,
                                      uint8_t  y_bytes,
                                      uint8_t *p_image_buf)
{

    aw_mprinter_params_t params;
    uint8_t            data[2] = {x_bytes, y_bytes};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         p_image_buf,
                         x_bytes * y_bytes,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPDLDEFBMG, &params);
}

aw_err_t aw_mprinter_download_bmg_print(int                     fd,
                                        aw_mprinter_bmg_ptype_t type)
{

    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        (uint8_t *)(&type),
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPPTDEFBMG, &params);
}


aw_err_t aw_mprinter_prph_dev_stat_trans(int      fd,
                                         uint8_t *p_stat,
                                         uint16_t time_out)
{

    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         NULL,
                         0,
                         NULL,
                         0,
                         p_stat,
                         1,
                         time_out);

    return aw_ioctl(fd, AW_MPTNPRPHDSTAT, &params);
}
/** \brief  ����ֽ�Ŵ�����״̬*/
aw_err_t aw_mprinter_paper_sensor_stat_trans(int      fd,
                                             uint8_t *p_stat,
                                             uint16_t time_out)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         NULL,
                         0,
                         NULL,
                         0,
                         p_stat,
                         1,
                         time_out);

    return aw_ioctl(fd, AW_MPTNPSNSTAT, &params);
}

/** \brief�����иһ��֮�⣩*/
aw_err_t aw_mprinter_onepoint_cut(int     fd)
{

    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         NULL,
                         0,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCONENPNT, &params);
}

/** \brief�����и����֮�⣩*/
aw_err_t aw_mprinter_threepoint_cut_(int     fd)
{

    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         NULL,
                         0,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCTHRNPNT, &params);
}

/** \brief ѡ����Χ�豸*/
aw_err_t aw_mprinter_prph_dev_enable(int     fd)
{

    aw_mprinter_params_t params;
    uint8_t            data = 1;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSPRPHDEV, &params);
}
aw_err_t aw_mprinter_prph_dev_disable(int     fd)
{

    aw_mprinter_params_t params;
    uint8_t            data = 2;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSPRPHDEV, &params);
}


aw_err_t aw_mprinter_page_mode_select(int     fd)
{

    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         NULL,
                         0,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_SPGMD, &params);
}

aw_err_t aw_mprinter_stand_mode_select(int     fd)
{

    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         NULL,
                         0,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_SSTDMD, &params);
}


aw_err_t aw_mprinter_real_time_cmd_enable(int     fd,
                                          uint8_t cmd_id)
{

    aw_mprinter_params_t params;
    uint8_t            data[5] = {0x05, 0x00, 0x14, cmd_id ,0x01};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         5,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSRRLTCMD, &params);
}
aw_err_t aw_mprinter_real_time_cmd_disable(int         fd,
                                           uint8_t     cmd_id)
{
    aw_mprinter_params_t params;
    uint8_t            data[5] = {0x05, 0x00, 0x14, cmd_id ,0x00};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         5,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSRRLTCMD, &params);
}

aw_err_t aw_mprinter_print_ctrl_meth_select(int                   fd,
                                            aw_mprinter_ctrl_md_t md,
                                            uint8_t               dat)
{

    aw_mprinter_params_t params;
    uint8_t            data[2] = {md, dat};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSPCTLMD, &params);
}


aw_err_t aw_mprinter_hv_motion_unit_set(int      fd,
                                        uint8_t  x,
                                        uint8_t  y)
{

    aw_mprinter_params_t params;
    uint8_t            data[2] = {x, y};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_SHAVMTNT, &params);
}

aw_err_t aw_mainten_cnt_init_(int      fd,
                              uint16_t cnt)
{

    aw_mprinter_params_t params;
    uint8_t            data[3] = {0x00, cnt & 0xFF, (cnt >> 8) & 0xFF};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         3,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPINITMAINCNT, &params);
}

/** \brief ����ά���Ĵ���*/
aw_err_t aw_mprinter_mainten_cnt_trans (int      fd,
                                        uint16_t cnt)
{

    aw_mprinter_params_t params;
    uint8_t            data[3] = {0x00, cnt & 0xFF, (cnt >> 8) & 0xFF};
    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         3,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPTNMAINCNT, &params);
}


/** \brief �趨/�������ģʽ�»���*/
aw_err_t aw_mprinter_kanjichar_underline_on(int                           fd,
                                            aw_mprinter_underline_crude_t type)
{

    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                       (uint8_t *)(&type),
                        1,
                        NULL,
                        0,
                        NULL,
                        0,
                        0);

    return aw_ioctl(fd, AW_MPSRUNDERFKCH, &params);
}
aw_err_t aw_mprinter_kanjichar_underline_off(int     fd)
{

    aw_mprinter_params_t params;
    uint8_t            data = 48;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSRUNDERFKCH, &params);
}


/** \brief ���û�NV�ڴ�д��*/
aw_err_t aw_mprinter_nv_usr_mem_write(int       fd,
                                      uint32_t  addr,
                                      uint8_t  *p_nv_buf,
                                      size_t    len)
{

    aw_mprinter_params_t params;
    uint8_t            data[6] = {addr & 0xFF,
                                  (addr >> 8) & 0xFF,
                                  (addr >> 16) & 0xFF,
                                  (addr >> 24) & 0xFF,
                                  len & 0xFF,
                                  (len >> 8) & 0xFF,};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         6,
                         p_nv_buf,
                         len,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPWRNVUSRMEM, &params);
}
/** \brief ���û�NV�ڴ��ȡ*/
aw_err_t aw_mprinter_nv_usr_mem_nv_read(int       fd,
                                        uint32_t  addr,
                                        uint8_t  *p_nv_buf,
                                        size_t    len,
                                        uint16_t  time_out)
{

    aw_mprinter_params_t params;
    uint8_t            data[6] = {addr & 0xFF,
                                  (addr >> 8) & 0xFF,
                                  (addr >> 16) & 0xFF,
                                  (addr >> 24) & 0xFF,
                                  len & 0xFF,
                                  (len >> 8) & 0xFF,};
    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         6,
                         NULL,
                         0,
                         p_nv_buf,
                         len,
                         time_out);

    return aw_ioctl(fd, AW_MPRDNVUSRMEM, &params);
}

/** \brief ����ʵʱ�������ӡ��*/
aw_err_t aw_mprinter_real_time_req_send(int     fd,
                                        uint8_t type)
{

    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &type,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSDRLTREQTP, &params);
}

/** \brief ҳģʽ�´�ӡ����*/
aw_err_t aw_mprinter_page_print (int  fd)
{

    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         NULL,
                         0,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPPGMDPDAT, &params);
}

/** \brief ѡ���ִ�ӡģʽ*/
aw_err_t aw_mprinter_kanjichar_pmode_select(int     fd,
                                            uint8_t type)
{

    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &type,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPSRPMDKCH, &params);
}


/** \brief �����Ҳ��ַ����*/
aw_err_t aw_mprinter_right_spacing_set (int     fd,
                                        uint8_t space)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &space,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSRGCSP, &params);
}

/** \brief ���»���ģʽ����*/
aw_err_t aw_mprinter_underline_mode_set (int                          fd,
                                         aw_mprinter_underline_mode_t mode)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        (uint8_t *)(&mode),
                        1,
                        NULL,
                        0,
                        NULL,
                        0,
                        0);

    return aw_ioctl(fd, AW_MPCSRUNDL, &params);
}


/** \brief  ���������ӡ�Ŀ�/��*/
aw_err_t aw_mprinter_emprinthasized_on (int  fd)
{
    aw_mprinter_params_t params;
    uint8_t            data;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data = 1;

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSRBFP, &params);
}

aw_err_t aw_mprinter_emprinthasized_off (int  fd)
{
    aw_mprinter_params_t params;
    uint8_t            data;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data = 0;

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSRBFP, &params);
}

/** \brief  ѡ���ӡ����*/
aw_err_t aw_mprinter_character_font_set (int                     fd,
                                         aw_mprinter_char_font_t font)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        (uint8_t *)(&font),
                        1,
                        NULL,
                        0,
                        NULL,
                        0,
                        0);

    return aw_ioctl(fd, AW_MPCSPFONT, &params);
}

/** \brief ����/���˳ʱ��90����ת*/
aw_err_t aw_mprinter_clockwise90_rotat_on (int fd)
{
    aw_mprinter_params_t params;
    uint8_t            data = 1;
    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSRCLKWSRT, &params);
}

aw_err_t aw_mprinter_clockwise90_rotat_off (int  fd)
{
    aw_mprinter_params_t params;
    uint8_t            data = 0;
    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSRCLKWSRT, &params);
}



/** \brief ����/����ߵ���ӡģʽ*/
aw_err_t aw_mprinter_upside_down_on (int               fd)
{
    aw_mprinter_params_t params;
    uint8_t            data = 1;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSRRVSMD, &params);
}
aw_err_t aw_mprinter_upside_down_off (int               fd)
{
    aw_mprinter_params_t params;
    uint8_t            data = 0;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSRRVSMD, &params);
}

/** \brief ѡ���ַ���С*/
aw_err_t aw_mprinter_char_size_set (int                     fd,
                                    aw_mprinter_char_size_t size)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        (uint8_t *)(&size),
                        1,
                        NULL,
                        0,
                        NULL,
                        0,
                        0);

    return aw_ioctl(fd, AW_MPCSFNTSZ, &params);
}


/** \brief �趨/������״�ӡģʽ*/
aw_err_t aw_mprinter_whi_blk_reverse_on (int               fd)
{
    aw_mprinter_params_t params;
    uint8_t            data = 1;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSRANTWMD, &params);
}
aw_err_t aw_mprinter_whi_blk_reverse_off (int               fd)
{
    aw_mprinter_params_t params;
    uint8_t            data = 0;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSRANTWMD, &params);
}


/** \brief ʹ��/������尴ť*/
aw_err_t aw_mprinter_panel_buttons_enable (int               fd)
{
    aw_mprinter_params_t params;
    uint8_t            data = 0;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSRBLKKY, &params);
}
aw_err_t aw_mprinter_panel_buttons_disable (int               fd)
{
    aw_mprinter_params_t params;
    uint8_t            data = 1;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSRBLKKY, &params);
}

/** \brief ����ֽ�Ŵ�����ֹͣ��ӡ*/
aw_err_t aw_mprinter_paper_sensors_on (int               fd)
{
    aw_mprinter_params_t params;
    uint8_t            data = 2;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSSNRTSP, &params);
}

aw_err_t aw_mprinter_paper_sensors_off (int fd)
{
    aw_mprinter_params_t params;
    uint8_t            data = 0;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSSNRTSP, &params);
}

/** \brief  �趨���հ���*/
aw_err_t aw_mprinter_left_spacing_set (int           fd,
                                       uint16_t      space)
{
    aw_mprinter_params_t params;
    uint8_t            data[2];

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = space & 0xFF;
    data[1] = (space >> 8) & 0xFF;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSLFBLK, &params);
}

/** \brief ���ô�ӡλ��Ϊ��ӡ�����*/
aw_err_t aw_mprinter_ppos_begp_set (int                    fd,
                                    aw_mprinter_ppos_beg_t type)
{
    aw_mprinter_params_t params;
    uint8_t            data;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data = type;

    aw_mprinter_cfg_cmd(&params,
                        &data,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSPPTFLS, &params);
}

/** \brief ���ô�ӡ������*/
aw_err_t aw_mprinter_area_width_set (int            fd,
                                     uint16_t       width)
{
    aw_mprinter_params_t params;
    uint8_t            data[2];

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = width & 0xFF;
    data[1] = (width >> 8) & 0xFF;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSPSWD, &params);
}


aw_err_t aw_mprinter_print_and_cut (int                         fd,
                                    aw_mprinter_cut_pape_type_t type,
                                    uint8_t                     n)
{
    aw_mprinter_params_t params;
    uint8_t            data[2];

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = type;
    data[1] = n;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCFDCTPAP, &params);
}

/**
 * \brief ����PDF417��ά��������������
 */
aw_err_t aw_mprinter_PDF417_areavrow_set (int       fd,
                                          uint8_t   vrows)
{
    aw_mprinter_params_t params;
    uint8_t        data[5] = {0x03, 0x00, 0x30, 0x41, vrows};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         5,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCFDCTPAP, &params);
}


/**
 * \brief ����PDF417��ά������
 */
aw_err_t aw_mprinter_PDF417_areahrow_set (int       fd,
                                          uint8_t   hrows)
{
    aw_mprinter_params_t params;
    uint8_t            data[5] = {0x03, 0x00, 0x30, 0x42, hrows};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCFDCTPAP, &params);
}

/**
 * \brief ���ö�ά�뵥Ԫ��ȴ�С
 */
aw_err_t aw_mprinter_symbol_width_set (int                       fd,
                                       aw_mprinter_symbol_type_t symbol,
                                       uint8_t                   wdt)
{
    aw_mprinter_params_t params;
    uint8_t            data[5] = {0x03, 0x00, symbol, 0x43, wdt};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         5,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCFDCTPAP, &params);
}

/**
 * \brief ����PDF417��ά���и�
 */
aw_err_t aw_mprinter_PDF417_rowhigh_set (int       fd,
                                         uint8_t   high)
{
    aw_mprinter_params_t params;
    uint8_t            data[5] = {0x03, 0x00, 30, 0x44, high};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         5,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCFDCTPAP, &params);
}

/**
 * \brief ���ö�ά������ȼ�
 */
aw_err_t aw_mprinter_symbol_errcorlvl_set (int                       fd,
                                           aw_mprinter_symbol_type_t symbol,
                                           uint8_t                   lvl)
{
    aw_mprinter_params_t params;
    uint8_t            data[5] = {0x03, 0x00, symbol, 0x45, lvl};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCFDCTPAP, &params);
}

/**
 * \brief ����PDF417��ά��Ϊ��׼ģʽ����ά�룩
 */
aw_err_t aw_mprinter_PDF417_stdmd_set (int       fd)
{
    aw_mprinter_params_t params;
    uint8_t            data[5] = {0x03, 0x00, 0x30, 0x46, 0};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         5,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCFDCTPAP, &params);
}

/**
 * \brief ����PDF417��ά��Ϊ�ض�ģʽ����ά�룩
 */
aw_err_t aw_mprinter_PDF417_tncatemd_set (int       fd)
{
    aw_mprinter_params_t params;
    uint8_t            data[5] = {0x03, 0x00, 0x30, 0x46, 1};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         5,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCFDCTPAP, &params);
}

/**
 * \brief �������ݵ����棨��ά�룩
 */
aw_err_t aw_mprinter_symbol_data_write (int                         fd,
                                        aw_mprinter_symbol_type_t   symbol,
                                        uint8_t                    *p_code,
                                        size_t                      code_len)
{
    aw_mprinter_params_t params;
    uint8_t            data[5] = {(code_len + 3) & 0xFF,
                                 ((code_len + 3) >> 8) & 0xFF,
                                  symbol,
                                  0x50,
                                  0x30};

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         5,
                         p_code,
                         code_len,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCFDCTPAP, &params);
}

/**
 * \brief ��ӡ���뻺���еĶ�ά����
 */
aw_err_t aw_mprinter_symbol_data_print (int                       fd,
                                        aw_mprinter_symbol_type_t symbol)
{
    aw_mprinter_params_t params;

    uint8_t            data[5] = {0x03, 0x00, symbol, 0x51, 0x30};
    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         5,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCFDCTPAP, &params);
}


//FS ( L Select label and black mark control function(s) ��ǩֽ/�ڱ�ָֽ��
aw_err_t aw_mprinter_abel_and_black_mark_set (int       fd,
                                              uint8_t  *p_code,
                                              size_t    code_len)
{
    aw_mprinter_params_t params;
    uint8_t            data[2];

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = code_len & 0xFF;
    data[1] = (code_len >> 8) & 0xFF;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         p_code,
                         code_len,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSLBABLKP, &params);
}

/**
 * \brief ҳģʽ�ع��׼ģʽ
 */
aw_err_t aw_mprinter_page_stdmd_enter (int  fd)
{
    if (fd < 0) {
        return -AW_EINVAL;
    }

    return aw_ioctl(fd, AW_MPPGSTDMD, NULL);
}

/**
 * \brief ��ӡ����ֽ
 */
aw_err_t aw_mprinter_print_and_feed (int fd)
{
    if (fd < 0) {
        return -AW_EINVAL;
    }

    return aw_ioctl(fd, AW_MPCPRINT, NULL);
}

/**
 * \brief �س�����ӡλ�õ�����������ʼλ�ã�����ֽ��
 */
aw_err_t aw_mprinter_print_and_enter (int fd)
{
    if (fd < 0) {
        return -AW_EINVAL;
    }

    return aw_ioctl(fd, AW_MPCENTER, NULL);
}

/**
 * \brief ��ӡ����ֽn�㣨����ӡ����������ݴ�ӡ����ֽn�㣩
 */
aw_err_t aw_mprinter_print_and_ndotlines_feed (int            fd,
                                               uint8_t        ndotlines)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &ndotlines,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCPAFNP, &params);
}

/**
 * \brief ��ӡ����ֽn�㣨����ӡ��������ݴ�ӡ����ֽn�㣩
 */
aw_err_t aw_mprinter_print_and_ndotlines_back (int            fd,
                                               uint8_t        ndotlines)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &ndotlines,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCPABNP, &params);
}

/**
 * \brief ��ӡ����ֽn�У�����ӡ��������ݴ�ӡ����ֽn�У�
 */
aw_err_t aw_mprinter_print_and_nfontlines_feed (int      fd,
                                                uint8_t  nfontlines)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &nfontlines,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCPAFNFL, &params);
}

/**
 * \brief ��ӡ����ֽn��
 */
aw_err_t aw_mprinter_print_and_nfontlines_back (int            fd,
                                                uint8_t        nfontlines)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }
    aw_mprinter_cfg_cmd(&params,
                        &nfontlines,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCPAFBFL, &params);
}

/**
 * \brief �����м��Ϊn��
 */
aw_err_t aw_mprinter_print_line_spaceing_set (int            fd,
                                              uint8_t        ndotlines)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &ndotlines,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSLSNP, &params);
}

/**
 * \brief �����м��ΪĬ��ֵ
 */
aw_err_t aw_mprinter_print_line_spaceing_default_set (int fd)
{
    if (fd < 0) {
        return -AW_EINVAL;
    }

    return aw_ioctl(fd, AW_MPCSLSDF, NULL);
}



/**
 * \brief ���þ��Դ�ӡλ��
 */
aw_err_t aw_mprinter_absolute_print_set (int       fd,
                                         uint16_t  xnum)
{
    uint8_t            data[2];
    aw_mprinter_params_t params;

    data[0] = (xnum & 0xFF);
    data[1] = ((xnum >> 8) & 0xFF);

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSABSPP, &params);
}

/**
 * \brief �����ַ���ӡ��ʽ
 */
aw_err_t aw_mprinter_font_type_set (int             fd,
                                    uint8_t         font_type)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &font_type,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSCPT, &params);
}

/**
 * \brief ���ô�ӡ���뷽ʽ
 */
aw_err_t aw_mprinter_print_align_type_set (int                 fd,
                                           aw_mprinter_align_t align)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        (uint8_t *)(&align),
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSPALIT, &params);
}



/**
 * \brief ѡ����ģʽ
 */
aw_err_t aw_mprinter_chinese_mode_enter (int fd)
{
    if (fd < 0) {
        return -AW_EINVAL;
    }

    return aw_ioctl(fd, AW_MPCSCHNMD, NULL);
}

/**
 * \brief ȡ������ģʽ
 */
aw_err_t aw_mprinter_chinese_mode_exit (int fd)
{
    if (fd < 0) {
        return -AW_EINVAL;
    }

    return aw_ioctl(fd, AW_MPCRCHNMD, NULL);
}

/**
 * \brief ѡ������ַ���
 */
aw_err_t aw_mprinter_gb_char_type_set (int                       fd,
                                       aw_mprinter_inter_char_t  char_type)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        (uint8_t *)(&char_type),
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSINTLFNT, &params);
}

/**
 * \brief ѡ���ַ�����ҳ
 */
aw_err_t aw_mprinter_char_page_set (int                      fd,
                                    aw_mprinter_char_page_t  code_page)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        (uint8_t *)(&code_page),
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSFNTCDP, &params);
}

/**
 * \brief ͼ�δ�ֱȡģ�������
 */
int aw_mprinter_picture_printv (int                    fd,
                                aw_mprinter_dot_type_t type,
                                uint16_t               hsize,
                                uint8_t               *p_databuf)
{
    aw_mprinter_params_t params;
    uint8_t            data[3];
    uint32_t           cnt = (hsize * (type / 16 + 1));

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = type;
    data[1] = hsize & 0xFF;
    data[2] = (hsize >> 8) & 0xFF;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         3,
                         p_databuf,
                         cnt,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCGDMVP, &params);
}

/**
 * \brief ͼƬˮƽȡģ���ݴ�ӡ
 */
aw_err_t aw_mprinter_picture_printh (int                         fd,
                                     aw_mprinter_picture_type_t  type,
                                     uint16_t                    xsize,
                                     uint16_t                    ysize,
                                     uint8_t                    *p_data_buf)
{
    aw_mprinter_params_t params;
    uint8_t            data[5];
    uint32_t           cnt = xsize * ysize;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = type;
    data[1] = xsize & 0xFF;
    data[2] = (xsize >> 8) & 0xFF;
    data[3] = ysize & 0xFF;
    data[4] = (ysize >> 8) & 0xFF;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         5,
                         p_data_buf,
                         cnt,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCGLMDP, &params);
}

/**
 * \brief ��ӡNVλͼ
 */
aw_err_t aw_mprinter_nvpicture_print (int                         fd,
                                      uint8_t                     picture_id,
                                      aw_mprinter_picture_type_t  picture_type)
{
    aw_mprinter_params_t params;
    uint8_t            data[2];

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = picture_id;
    data[1] = picture_type;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCPNVIMG, &params);
}

/**
 * \brief ����NVλͼ
 */
aw_err_t aw_mprinter_nvpicture_download (int              fd,
                                         uint8_t          picture_num,
                                         uint8_t         *p_picture_buf,
                                         size_t           len)
{
    aw_mprinter_params_t params;
    uint8_t        data[1];

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = picture_num;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         1,
                         p_picture_buf,
                         len,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCDLNVIMG, &params);
}

/**
 * \brief ˮƽ�Ʊ�
 */
aw_err_t aw_mprinter_horizontal_tab (int fd)
{
    if (fd < 0) {
        return -AW_EINVAL;
    }

    return aw_ioctl(fd, AW_MPCLVLCTTAB, NULL);
}

/**
 * \brief ����ˮƽ�Ʊ�λ��
 */
aw_err_t aw_mprinter_horizontaltab_set (int            fd,
                                        uint8_t       *p_buf,
                                        size_t         data_num)
{
    aw_mprinter_params_t params;
    uint8_t            data[1];

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = 0;

    aw_mprinter_cfg_cmd(&params,
                         p_buf,
                         data_num,
                         data,
                         1,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCLVLCTTABP, &params);
}

/**
 * \brief ����һά����ɶ��ַ���HRI����ӡλ��
 */
aw_err_t aw_mprinter_barcode_hri_pos_set (int                    fd,
                                          aw_mprinter_hri_site_t hri_pos)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        (uint8_t *)(&hri_pos),
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSHRIPP, &params);
}

/**
 * \brief ����һά����߶�
 */
aw_err_t aw_mprinter_barcode_height_set (int            fd,
                                         uint8_t        height)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }
    aw_mprinter_cfg_cmd(&params,
                        &height,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSODCH, &params);
}

/**
 * \brief ����һά������
 */
aw_err_t aw_mprinter_barcode_width_set (int            fd,
                                        uint8_t        width)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &width,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSODCW, &params);
}

/**
 * \brief ����һά��������
 */
aw_err_t aw_mprinter_barcode_hri_font_set (int                          fd,
                                           aw_mprinter_hri_font_type_t  hri_font)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        (uint8_t *)(&hri_font),
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSODCF, &params);
}

/**
 * \brief ��ӡһά����
 */
aw_err_t aw_mprinter_barcode_print (int                       fd,
                                    aw_mprinter_barcode_sys_t barcode_sys,
                                    uint8_t                  *p_code_buf,
                                    size_t                    code_len)
{
    aw_mprinter_params_t params;
    uint8_t            data[2];

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = barcode_sys;
    data[1] = code_len;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         p_code_buf,
                         code_len,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCPODC, &params);
}

/**
 * \brief ��ѯ��ӡ��״̬��ʵʱ��
 */
aw_err_t aw_mprinter_printer_state_rt_get (int            fd,
                                           uint8_t       *p_state,
                                           uint16_t       time_out)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         NULL,
                         0,
                         NULL,
                         0,
                         p_state,
                         1,
                         time_out);

    return aw_ioctl(fd, AW_MPCRTQPS, &params);
}

/**
 * \brief ����/ȡ����ӡ��״̬�Զ�����
 */
aw_err_t aw_mprinter_printer_state_auto_set (int            fd,
                                             uint8_t        key)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        &key,
                         1,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSRPSAB, &params);
}

/**
 * \brief ����ֽ��״̬����ʵʱ��
 */
aw_err_t aw_mprinter_paper_stat_get (int                           fd,
                                     aw_mprinter_trans_stat_type_t type,
                                     uint8_t                      *p_state,
                                     uint16_t                      time_out)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        (uint8_t *)(&type),
                        1,
                        NULL,
                        0,
                        p_state,
                        1,
                        time_out);

    return aw_ioctl(fd, AW_MPCDLSTAT, &params);
}

/**
 * \brief ��ʼ����ӡ��
 */
aw_err_t aw_mprinter_init (int fd)
{
    if (fd < 0) {
        return -AW_EINVAL;
    }

    return aw_ioctl(fd, AW_MPCPINIT, NULL);
}


/**
 * \brief ���ӡ���棨ʵʱ��
 */
aw_err_t aw_mprinter_printer_buffer_clear (int fd)
{
    uint8_t            data[7] = {0x01, 0x03, 0x14, 0x01, 0x06, 0x02, 0x08};
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                         data,
                         7,
                         NULL,
                         0,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCRLCLPBUF, &params);
}

aw_err_t aw_mprinter_cmm_cfg_set (int                        fd,
                                  aw_mprinter_cmm_cfg_type_t cfg_type,
                                  uint32_t                   param)
{

    uint8_t            data[4], i;
    uint8_t            param_buf[6], tmprint;
    size_t             param_len;
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    if (cfg_type == AW_MPRINTER_CMM_CFG_BAUDRATE_SET) {
        for (i = 0; i < 8; i++) {  /* ����ó������ʵ�ÿ��ʮ����λ */
            param_buf[i] = (uint8_t)(param % 10);
            param /= 10;
            if (param_buf[i] != 0) {
                param_len = i + 1;    /* ������Чʮ����λ��             */
            }
        }

        for (i = 0; i < param_len - 1 - i; i++) {   /* �����ʮ����λ��ʼ���   */
            tmprint = param_buf[i];
            param_buf[i] = param_buf[param_len - 1 - i] | 0x30;
            param_buf[param_len - 1 - i] = tmprint | 0x30;
        }
    } else {
        param_len = 1;
        param_buf[0] = param & 0xFF;
    }

    data[0] = (param_len + 2) & 0XFF;
    data[1] = ((param_len + 2) >> 8) & 0XFF;
    data[2] = 11;
    data[3] = cfg_type;


    aw_mprinter_cfg_cmd(&params,
                         data,
                         4,
                         param_buf,
                         param_len,
                         NULL,
                         0,
                         0);

    return aw_ioctl(fd, AW_MPCSCMMPAP, &params);
}


/**
 * \brief ���ʹ�ӡ��ID
 */
aw_err_t aw_mprinter_id_get (int               fd,
                             aw_mprinter_id_t  id_type,
                             uint8_t          *p_id_val,
                             uint16_t          time_out)
{
    aw_mprinter_params_t params;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    aw_mprinter_cfg_cmd(&params,
                        (uint8_t *)(&id_type),
                        1,
                        NULL,
                        0,
                        p_id_val,
                        1,
                        time_out);

    return aw_ioctl(fd, AW_MPCDLPRTID, &params);
}

/**
 * \brief ����͹���ģʽ��ʵʱ��
 */
aw_err_t aw_mprinter_printer_low_power_mode_enter (int            fd,
                                                   uint16_t       time_out)
{
    aw_mprinter_params_t params;
    uint8_t            data[2];
    uint8_t            stat;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = 0x01;
    data[1] = 0x08;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         NULL,
                         0,
                        &stat,
                         1,
                         time_out);
    stat = aw_ioctl(fd, AW_MPCSRLOWPMD, &params);

    if (stat == 0x30) {
        return AW_OK;
    } else {
        return stat;
    }
}

/**
 * \brief �˳��͹���ģʽ��ʵʱ��
 */
aw_err_t aw_mprinter_printer_low_power_mode_exit (int            fd,
                                                  uint16_t       time_out)
{
    aw_mprinter_params_t params;
    uint8_t            data[2];
    uint8_t            stat;

    if (fd < 0) {
        return -AW_EINVAL;
    }

    data[0] = 0x02;
    data[1] = 0x08;

    aw_mprinter_cfg_cmd(&params,
                         data,
                         2,
                         NULL,
                         0,
                        &stat,
                         1,
                         time_out);

    stat = aw_ioctl(fd, AW_MPCSRLOWPMD, &params);

    if (stat == 0x31) {
        return AW_OK;
    } else {
        return stat;
    }
}

/**
 * \brief ���ô�ӡ��״̬�ص�����
 */
aw_err_t aw_mprinter_stat_callback_set (int                       fd,
                                        aw_mprinter_cbk_type_t    callback_type,
                                        aw_pfn_printer_stat_isr_t pfn_callback,
                                        void                     *p_callback_arg)
{
    aw_mprinter_stat_cbk_t callback;
    if (fd < 0) {
        return -AW_EINVAL;
    }

    callback.p_callback_arg = p_callback_arg;
    callback.pfn_callback   = pfn_callback;
    callback.stat_cbk_type  = callback_type;

    return aw_ioctl(fd, AW_MPCALLBACKSET, &callback);
}

/**
 * \brief ��չָ��
 */
aw_err_t aw_mprinter_features_expand (int                 fd,
                                aw_mprinter_params_t     *p_cmd)
{
    if (fd < 0 || NULL == p_cmd) {
        return -AW_EINVAL;
    }

    return aw_ioctl(fd, AW_MPEXPANDTYPE, p_cmd);
}

/**
 * \brief ��΢��д����
 */
ssize_t aw_mprinter_write(int fd, const void *p_buf, size_t nbytes)
{
    return aw_write(fd, p_buf, nbytes);
}

/**
 * \brief ��һ��΢���豸
 */
aw_err_t aw_mprinter_dev_open (const char *p_name)
{
    int fd = 0;
    fd = aw_open(p_name, O_WRONLY, 0777);

    if (fd < 0) {
        return -1;
    }

    return fd;
}

/**
 * \brief �ص�һ��΢���豸
 */
void aw_mprinter_dev_close (int fd)
{
    aw_close(fd);
}


/* end of file */
