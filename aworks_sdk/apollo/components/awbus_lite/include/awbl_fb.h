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
 * \brief AWBus FB 接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_fb.h"
 * \endcode
 *
 * 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 19-05-24  cat, first implementation
 * \endinternal
 */
 
#ifndef __AWBL_FB_H
#define __AWBL_FB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_fb.h"

struct awbl_fb_info;

/**
 * \brief 帧缓冲设备操作接口
 */
typedef struct awbl_fb_ops {

    /** \brief 初始化帧缓冲设备 */
    aw_err_t (*fb_init)             (struct awbl_fb_info *p_fb);

    /** \brief 开启帧缓冲设备 */
    aw_err_t (*fb_start)            (struct awbl_fb_info *p_fb);

    /** \brief 停止帧缓冲设备 */
    aw_err_t (*fb_stop)             (struct awbl_fb_info *p_fb);

    /** \brief 重启帧缓冲设备 */
    aw_err_t (*fb_reset)            (struct awbl_fb_info *p_fb);

    /** \breif 获取前置缓冲区地址 */
    uint32_t (*fb_get_online_buf)   (struct awbl_fb_info *p_fb);

    /** \brief 获取后置帧缓冲区地址 */
    uint32_t (*fb_get_offline_buf)  (struct awbl_fb_info *p_fb);

    /** \brief 交换帧缓冲区 */
    aw_err_t (*fb_swap_buf)         (struct awbl_fb_info *p_fb);

    /** \brief 尝试交换帧缓冲区 */
    aw_err_t (*fb_try_swap_buf)     (struct awbl_fb_info *p_fb);

    /** \brief 设置帧缓冲设备消影 */
    aw_err_t (*fb_blank)            (struct awbl_fb_info *p_fb, 
                                     aw_fb_black_t        blank);

    /** \brief 设置帧缓冲设备背光 */
    aw_err_t (*fb_backlight)        (struct awbl_fb_info *p_fb, int level);

    /** \brief 配置帧缓冲设备时序等信息 */
    aw_err_t (*fb_config)     (struct awbl_fb_info *p_fb);
	
    /** \brief 设置/获取帧缓冲设备信息 */
    aw_err_t (*fb_ioctl)            (struct awbl_fb_info *p_fb, 
                                     aw_fb_iocmd_t        cmd, 
                                     void                *p_arg);

}awbl_fb_ops_t;

/**
 * \brief 帧缓冲区状态
 */
typedef struct awbl_fb_status {

    int16_t                 online_buffer;  /**< \brief 前置缓冲区索引 */
    int16_t                 offline_buffer; /**< \brief 后置缓冲区索引 */

}awbl_fb_status_t;

/**
 * \brief 帧缓冲设备信息
 */
typedef struct awbl_fb_info {

    aw_fb_var_info_t        var_info;       /**< \brief 帧缓冲设备可变信息 */
    aw_fb_fix_info_t        fix_info;       /**< \brief 帧缓冲设备固定信息 */

    const awbl_fb_ops_t    *p_fb_ops;       /**< \brief 帧缓冲设备操作接口 */

    awbl_fb_status_t        fb_status;      /**< \brief 帧缓冲区状态 */

    void                   *p_cookie;

} awbl_fb_info_t;

#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_FB_H */

/* end of file */
