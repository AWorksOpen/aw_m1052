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
 * \brief AWorks ֡�����豸�ӿ�
 *
 * ʹ�ñ�������Ҫ��������ͷ�ļ�
 * \code
 * #include "aw_fb.h"
 * \endcode
 *
 * \par ʹ��˵��
 *  ʹ��֡�����豸ǰ, ����ʹ�� aw_fb_open() ��һ��֡�����豸������ͨ��
 *  aw_fb_init() ��֡�����豸���г�ʼ������ʱ��֡�����豸�Ѿ�������ɣ�ͨ
 *  �� aw_fb_start() ������С�֡�����豸��ع̶���Ϣ��ͨ�� aw_fb_ioctl() 
 *  ��ã������Դ���ʼ��ַ�ȣ���� aw_fb_fix_info_t ����֡�����豸��ؿɱ�
 *  ���ÿ�ͨ�� aw_fb_ioctl() ��ȡ���޸ģ�������ʾ������� aw_fb_var_info_t��
 *  �û�׼������ʱ����ͨ�� aw_fb_get_online_buf() �� aw_fb_get_offline_buf()
 *  ��ȡ����������ַ,Ȼ���ڻ�ȡ�Ļ������Ͻ��л��ƣ�������ɺ󣬶໺�滷����
 *  ��Ҫͨ�� aw_fb_swap_buf() ��aw_fb_try_swap_buf() �����û���������Ϊǰ��
 *  ������������ʾ��
 *
 * \par ע������
 *  �����������API����֧���ڶ�������ʹ�á�
 *
 * \par �໺�����ԭ��˵��
 *  �����ö��������������£�����ʾ��������ʹ�õ�ǰ�û�������������Ļ����ʾͼ��
 *  ͬʱ��һ�����߶�����û����������û��Ļ��Ʋ��������Ʋ�����ɺ󣬻��������н�����
 *  ����������ɺ󣬺��û�������Ϊ�ɼ���ǰ�û�������
 *  �û���ʹ��ʱ��ͨ�� aw_fb_ioctl() ����� aw_fb_fix_info_t ����õ�ǰ�Ļ�������Ŀ��
 *
 * \par ����������˵��
 *  ����������ٶ���Թ̶��Ļ����£��������˫��������Ч������һ�£�
 *  ������������ٶȲ��ȶ��Ļ����£�������Ỻ�����ڻ����ٶȲ��ȶ����µ���ʾ֡��
 *  �����ϴ���������ʵ�ϣ��󲿷ֵ���ʾ�������������ٶȶ��ǲ��ȶ��ģ���ˣ�Ϊ
 *  ��ʵ��������ܣ��ڿռ����������£�����ʹ�������塣
 *
 * \par ������ʹ��ʾ��
 * \code
 * #include "aw_fb.h"
 *
 * int main() 
 * {
 *    uint32_t online_buffer;
 *
 *    //֡�����豸������������
 *    void *p_fb = aw_fb_open("imx1050_fb", 0);
 *    aw_fb_init(p_fb);
 *    aw_fb_start(p_fb);
 *    aw_fb_backlight(p_fb, 100);
 *
 *    //��ʹ�õ����������У��û������Բ���ǰ�û�������
 *    vram_addr = aw_fb_get_online_buf(p_fb);           //��ȡǰ�û����ַ
 *    while(1) {
 *        ......                                        //�û��ڴ��Դ�����ɻ���
 *    }
 * }
 * \endcode
 *
 * \par �໺��ʹ��ʾ��1(֧��˫�����������)
 * \code
 * #include "aw_fb.h"
 *
 * int main()
 * {
 *    uint32_t offline_buffer;
 *
 *    //֡�����豸������������
 *    void *p_fb = aw_fb_open("imx1050_fb", 0);
 *    aw_fb_init(p_fb);
 *    aw_fb_start(p_fb);
 *    aw_fb_backlight(p_fb, 100);
 *
 *    //��ʹ�ö໺�������У��û�һ��ֻ��Ҫ�������û�������
 *    while(1) {
 *        offline_buffer = aw_fb_get_offline_buf(p_fb); //��ȡ���û����ַ
 *        ......                                        //�û��ڴ˻�������ɻ���
 *        aw_fb_swap_buf(p_fb);                         //������ɺ���л���������
 *    }
 * }
 * \endcode
 *
 * \par �໺��ʹ��ʾ��2����֧�������棩
 * \code
 * #include "aw_fb.h"
 *
 * int main()
 * {
 *    uint32_t offline_buffer;
 *
 *    //֡�����豸������������
 *    void *p_fb = aw_fb_open("imx1050_fb", 0);
 *    aw_fb_init(p_fb);
 *    aw_fb_start(p_fb);
 *    aw_fb_backlight(p_fb, 100);
 *
 *    offline_buffer = aw_fb_get_offline_buf(p_fb); //��ȡ���û����ַ
 *    while(1) {
 *        ......                                    //�û��ڴ˻�������ɻ���
 *        if (AW_OK == aw_fb_try_swap_buf(p_fb)) {  //������ɺ��Խ��л�����������
 *                                                  //������Խ���ʧ�ܣ��û���Ҫ����
 *                                                  //�ڵ�ǰ�ĺ��û��������л��ơ�
 *                                                  //������Խ����ɹ����û�Ӧ��ȡ����
 *                                                  //����������������л��Ʋ�����
 *            offline_buffer = aw_fb_get_offline_buf(p_fb); //��ȡ���û����ַ
 *        }
 *    }
 * }
 * \endcode
 *
 * \par ֡�����豸��ȡ֡������Ϣʾ��
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
 *    //ͨ�� aw_fb_ioctl() ��ȡ�̶���Ϣ 
 *    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &fix_info);
 *    v_addr      = fix_info.vram_addr;
 *    vram_max    = fix_info.vram_max;
 *
 *    //ͨ�� aw_fb_ioctl() ��ȡ�ɱ���Ϣ
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
 * \brief ��Ļ��������
 */
typedef enum aw_fb_blank_level {
    
    AW_FB_BLANK_UNBLANK = 0,            /**< \brief hsync: on,  vsync: on  */
    AW_FB_BLANK_NORMAL,                 /**< \brief hsync: on,  vsync: on  */
    AW_FB_BLANK_VSYNC_SUSPEND,          /**< \brief hsync: on,  vsync: off */
    AW_FB_BLANK_HSYNC_SUSPEND,          /**< \brief hsync: off, vsync: on  */
    AW_FB_BLANK_POWERDOWN               /**< \brief hsync: off, vsync: off */
    
}aw_fb_black_t;

/**
 * \brief io��������
 */
typedef enum aw_fb_iocmd {

    AW_FB_CMD_GET_FINFO = 1,            /**< \brief ��ȡframe buffer�̶���Ϣ */
    AW_FB_CMD_GET_VINFO,                /**< \brief ��ȡframe buffer�ɱ���Ϣ */
    AW_FB_CMD_SET_VINFO,                /**< \brief �޸�frame buffer�ɱ���Ϣ */

}aw_fb_iocmd_t;

/**
 * \brief ���ò���
 */
typedef enum aw_fb_config {

    AW_FB_CONF_DCLK_VINFO = 1,           /**< \brief ��������ʱ��Ƶ�� */
    AW_FB_CONF_RES_VINFO,                /**< \brief ������Ļ�ߴ���Ϣ */
    AW_FB_CONF_BPP_VINFO,                /**< \brief ����bpp��Ϣ */
    AW_FB_CONF_HSYNC_VINFO,              /**< \brief ����ˮƽͬ���ź���Ϣ */
    AW_FB_CONF_VSYNC_VINFO,              /**< \brief ���ô�ֱͬ���ź���Ϣ */
    AW_FB_CONF_BUFFER_VINFO,             /**< \brief ���û�������Ϣ */
    AW_FB_CONF_ORIENTATION_VINFO,        /**< \brief ������������ʾ��0-������Ĭ�ϣ���1-����  */

}aw_fb_config_t;

/**
 * \brief R�����ݣ�ƫ�ƺͳ�����Ϣ
 */
typedef struct aw_fb_var_red_info {

    uint8_t  data;
    uint16_t offset;
    uint16_t length;

}aw_fb_var_red_info_t;

/**
 * \brief G�����ݣ�ƫ�ƺͳ�����Ϣ
 */
typedef struct aw_fb_var_green_info {

    uint8_t  data;
    uint16_t offset;
    uint16_t length;

}aw_fb_var_green_info_t;

/**
 * \brief B�����ݣ�ƫ�ƺͳ�����Ϣ
 */
typedef struct aw_fb_var_blue_info {

    uint8_t  data;
    uint16_t offset;
    uint16_t length;

}aw_fb_var_blue_info_t;

/**
 * \brief ��ͬ���ź���Ϣ
 */
typedef struct aw_fb_var_hsync_info {

    uint16_t    h_pulse_width;   /** \brief �������� */
    uint16_t    h_active;        /** \brief һ�е����ظ��� */
    uint16_t    h_bporch;        /** \brief ˮƽͬ������ */
    uint16_t    h_fporch;        /** \brief ˮƽͬ��ǰ�� */

}aw_fb_var_hsync_info_t;

/**
 * \brief ��ͬ���ź���Ϣ
 */
typedef struct aw_fb_var_vsync_info {

    uint16_t    v_pulse_width;   /** \brief ��ֱ������ */
    uint16_t    v_active;        /** \brief һ֡��������� */
    uint16_t    v_bporch;        /** \brief ��ֱͬ������ */
    uint16_t    v_fporch;        /** \brief ��ֱͬ��ǰ�� */

}aw_fb_var_vsync_info_t;

/**
 * \brief ��������Ϣ
 */
typedef struct aw_fb_var_buffer_info {

    uint32_t               buffer_num;   /**< \brief ������Ŀ */
    uint32_t               buffer_size;  /**< \brief ֡��С */

}aw_fb_var_buffer_info_t;

/**
 * \brief bpp��Ϣ
 */
typedef struct aw_fb_var_bpp_info {

    uint16_t                word_lbpp;    /**< \brief һ�����صĳ��ȣ�λΪ��λ,��rgb565���ݣ���ֵΪ16 */
    uint16_t                bus_bpp;      /**< \brief rgbӲ�������� */

}aw_fb_var_bpp_info_t;

/**
 * \brief ��Ļ�ߴ���Ϣ
 */
typedef struct aw_fb_var_res_info {

    uint16_t                x_res;        /**< \brief ��Ļx��ֱ��� */
    uint16_t                y_res;        /**< \brief ��Ļy��ֱ��� */

}aw_fb_var_res_info_t;

/**
 * \brief ʱ��Ƶ����Ϣ
 */
typedef struct aw_fb_var_dclk_info {

    uint32_t                typ_dclk_f;       /** \brief ����ʱ��Hz */
//    uint32_t                max_dclk_f;   /** \brief ���ʱ��Hz */
//    uint32_t                min_dclk_f;   /** \brief ��Сʱ��Hz */

}aw_fb_var_dclk_info_t;

/**
 * \brief ֡�����豸�ɱ���Ϣ���û���ͨ�� aw_fb_ioctl() ��ȡ���޸Ľṹ���Ա��Ϣ��
 */
typedef struct aw_fb_var_info {

    aw_fb_var_dclk_info_t   dclk_f;       /** \brief ʱ��Ƶ����Ϣ */

    aw_fb_var_res_info_t    res;          /** \brief ��Ļ�ߴ���Ϣ */

    aw_fb_var_bpp_info_t    bpp;          /**< \brief bpp��Ϣ */

    aw_fb_var_hsync_info_t  hsync;        /**< \brief ˮƽͬ���ź���Ϣ */
    aw_fb_var_vsync_info_t  vsync;        /**< \brief ��ֱͬ���ź���Ϣ */

    aw_fb_var_buffer_info_t buffer;       /**< \brief ��������Ϣ */

    aw_fb_var_red_info_t    red;          /**< \brief red */
    aw_fb_var_green_info_t  green;        /**< \brief green */
    aw_fb_var_blue_info_t   blue;         /**< \brief blue */

    uint32_t                orientation;  /** \brief ������������ʾ��0-������Ĭ�ϣ���1-���� */

}aw_fb_var_info_t;

/**
 * \brief ֡�����豸�̶���Ϣ���û���ͨ�� aw_fb_ioctl() 
 *        ��ȡ�������޸ģ��ṹ���Ա��Ϣ��
 */
typedef struct aw_fb_fix_info {

    uint32_t               vram_addr;       /**< \brief �Դ���ʼ��ַ */
    uint32_t               vram_max;        /**< \brief ����Դ�ռ� */
    
}aw_fb_fix_info_t;

/**
 * \brief ��֡�����豸
 *
 * \param[in]   name        �豸��
 * \param[in]   unit        �豸��Ԫ��
 *
 * \retval      NULL        ʧ�ܣ�����Ϊ֡�豸
 */
void *aw_fb_open(const char *name, int unit);

/**
 * \brief ��ʼ��֡�����豸
 *
 * \param[in]   info        ֡�����豸��Ϣ
 *
 * \retval      AW_OK       ��ʼ���ɹ�
 * \retval     -AW_ENXIO    p_fb_info Ϊ NULL
 * \retval     -AW_ENOMEM   �Դ�ռ䲻��
 */
aw_err_t aw_fb_init(void *p_fb);

/**
 * \brief ����֡�����豸
 *
 * \param[in]   p_fb_info   ֡�����豸��Ϣ
 *
 * \retval      AW_OK       ���гɹ�
 * \retval     -AW_ENXIO    p_fb_info Ϊ NULL
 */
aw_err_t aw_fb_start(void *p_fb);

/**
 * \brief ֹͣ֡�����豸   
 *
 * \param[in]   p_fb_info   ֡�����豸��Ϣ
 *
 * \retval      AW_OK       ֹͣ�ɹ�
 * \retval     -AW_ENXIO    p_fb_info Ϊ NULL
 */
aw_err_t aw_fb_stop(void *p_fb);
/**
 * \brief ����֡�����豸
 *
 * \param[in]   p_fb_info   ֡�����豸��Ϣ
 *
 * \retval      AW_OK       �����ɹ�
 * \retval     -AW_ENXIO    p_fb_info Ϊ NULL
 */
aw_err_t aw_fb_reset(void *p_fb);

/**
 * \brief ��ȡǰ��֡�����ַ
 *
 * \param[in]   p_fb_info   ֡�����豸��Ϣ
 *
 * \retval      NULL        ʧ�ܣ�����Ϊ֡�����ַ
 * \retval     -AW_ENXIO    p_fb_info Ϊ NULL
 *
 * \note        �����滷������Ҫʹ�ô�API��ȡ��ǰʹ�õ�֡�����ַ��
 *              �໺�滷����һ�㲻��ʹ�õ���API��
 */
uint32_t aw_fb_get_online_buf(void *p_fb);

/**
 * \brief ��ȡ����֡�����ַ
 *
 * \param[in]   p_fb_info   ֡�����豸��Ϣ
 *
 * \retval      NULL        ʧ�ܣ�����Ϊ֡�����ַ
 * \retval     -AW_ENXIO    p_fb_info Ϊ NULL
 *
 * \note        ��֧���ڶ໺�滷����ʹ�ô�API
 */
uint32_t aw_fb_get_offline_buf(void *p_fb);

/**
 * \brief ǰ�����û������������໺�滷�����Ƽ�ʹ�ã�
 *
 * \param[in]   p_fb_info   ֡�����豸��Ϣ
 *
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_ENXIO    p_fb_info Ϊ NULL
 * 
 * \note  ˫����������滷���¾���ʹ�ô�API
 * \note  ��API����֡ͬ���ĵȴ�����(�ȴ�LCD��������ǰ�û�����ˢ����ɺ󽻻�������)��
 *        �û�����Ҫ�ȴ��˽ӿ�ִ����ɲ��ܼ������л��Ʋ�����
 */
aw_err_t aw_fb_swap_buf(void *p_fb);

/**
 * \brief ǰ�����û��������Խ���
 *
 * \param[in]   p_fb_info   ֡�����豸��Ϣ
 *
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_ENXIO    p_fb_info Ϊ NULL
 * \retval     -AW_EPERM    ��֧�֣�����������ģʽ�¿���
 * \retval     -AW_EAGAIN   ����ʧ�ܣ���ʾLCD����������ˢ��
 *
 * \note        ��֧���������滷����ʹ�ô�API
 * \note        ��LCD����������ˢ��ǰ�û���������ȡ�������������������� -AW_EAGAIN��
 *              �û��Ļ��Ʋ������õȴ�LCD������ˢ����ǰ�û�������
 */
aw_err_t aw_fb_try_swap_buf(void *p_fb);

/**
 * \brief ����֡�����豸����
 *
 * \param[in]   p_fb_info   ֡�����豸��Ϣ
 * \param[in]   blank       ��������(ϵͳ֧�ֵ���Ч������< aw_fb.h >�ж���)
 *
 * \retval      AW_OK       �ɹ�
 * \retval     -AW_ENXIO    p_fb_info Ϊ NULL
 * \retval     -AW_EINVAL   ������Ч
 */
aw_err_t aw_fb_blank(void *p_fb, aw_fb_black_t blank);

/**
 * \brief ����֡�����豸����
 *
 * \param[in]   p_fb_info   ֡�����豸��Ϣ
 * \param[in]   level       ���⼶��(0 - 100)
 *                         (������õı��⼶���ںϷ��ķ�Χ��, ��ʹ��Ĭ�ϱ��⼶��100.)
 *
 * \retval      AW_OK       ���óɹ�
 * \retval     -AW_ENXIO    p_fb_info Ϊ NULL
 *
 * \note ��ЩӲ��PWM������ռ�ձ���������PWM���Ʊ���ʱ�����鱳�⼶������Ϊ99.
 */
aw_err_t aw_fb_backlight(void *p_fb, int level);

/**
 * \brief ����֡�����豸��Ϣ
 *
 * \param[in]   p_fb_info   ֡�����豸��Ϣ
 * \param[in]   conf        aw_fb_config_t �ж���
 * \param[in]   p_arg       ���ڴ���

 *
 * \retval      AW_OK       ���óɹ�
 * \retval     -AW_ENXIO    p_fb_info Ϊ NULL
 *
 * \note ����ʱ����Ϣ:ˮƽͬ��ʱ��ʹ�ֱͬ��ʱ��.
 * \note ����bpp��Ϣ:var_info.bpp.bus_bpp��var_info.bpp.word_lbpp�ֱ�Ϊ����֡һ�����ش�С����Ļ���߿�ȣ�
 *       ��var_info.bpp.bus_bpp��var_info.bpp.word_lbpp�����ʱ���������֡��ÿ���ر���ʾ����λ���٣�����
 *       16 bpp����֡�н���24 bpp LCD����eLCDIF��Ϊÿ���������ÿ����ɫ��MSB����ͬ��ɫ��LSB���������֡��ÿ����λ
 *       ��������ʾ�������磬24 bpp����֡����16 bpp LCD����eLCDIF������ÿ����ɫͨ����LSB��ת��Ϊ�ϵ͵���ɫ��ȡ�һ
 *       ���÷�var_info.bpp.bus_bpp��var_info.bpp.word_lbpp��ȣ�
 */
aw_err_t aw_fb_config(void *p_fb, aw_fb_config_t conf, void *p_arg);

/**
 * \brief ����/��ȡ֡�����豸��Ϣ
 *
 * \param[in]   p_fb_info   ֡�����豸��Ϣ
 * \param[in]   cmd         aw_fb_iocmd_t �ж���
 * \param[in]   p_arg       ���ڴ��λ�ȡֵ
 *
 * \retval      AW_OK
 * \retval     -AW_ENXIO    p_fb_info Ϊ NULL
 * \retval     -AW_EINVAL   ������Ч
 */
aw_err_t aw_fb_ioctl(void *p_fb, aw_fb_iocmd_t cmd, void *p_arg);
/** @} */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_FB_H */

/* end of file */

