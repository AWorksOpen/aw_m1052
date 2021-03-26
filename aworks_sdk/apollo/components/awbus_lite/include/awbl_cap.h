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
 * \brief Freescale i.MX283 cap外设
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_cap.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-10-14  tee, first implementation
 * \endinternal
 */

#ifndef __AWBL_CAP_H
#define __AWBL_CAP_H

#include "aw_errno.h"
#include "aw_cap.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void (*cap_callback_t) (void *p_arg,unsigned int count); 
         
/**
 * \brief AWBus cap 服务实例的相关信息
 */
struct awbl_cap_servinfo {

    /** \brief 支持的最小cap通道id号 */
    int cap_min_id;

    /** \brief 支持的最大cap通道id号 */
    int cap_max_id;
};

/**
 * \brief AWBus cap 服务函数
 */
struct awbl_cap_servopts {

    /** \brief 配置cap有效触发边沿和回调函数 */
    aw_err_t (*pfunc_cap_config) (void           *p_cookie,
                                  int             cap_id,
                                  unsigned int    flags,
                                  cap_callback_t  p_func_callback,
                                  void           *p_arg);

    /** \brief 使能cap */
    aw_err_t (*pfunc_cap_enable) (void *p_cookie,
                                  int   cap_id);

    /** \brief 禁能cap */
    aw_err_t (*pfunc_cap_disable) (void *p_cookie,
                                   int   cap_id);
    /** \brief 复位cap */
    aw_err_t (*pfunc_cap_reset) (void *p_cookie,
                                 int   cap_id);
    
    aw_err_t (*pfunc_cap_count_to_time)(void         *p_cookie,
                                        int           cap_id,
                                        unsigned int  count1,     /* 第一次捕获得到的值 */
                                        unsigned int  count2,     /* 第二次捕获得到的值 */
                                        unsigned int *p_time_ns);
};

/**
 * \brief AWBus cap 服务实例
 */
struct awbl_cap_service {

    /** \brief 指向下一个 cap 服务 */
    struct awbl_cap_service *p_next;

    /** \brief CAP 服务的相关信息 */
    const struct awbl_cap_servinfo *p_servinfo;

    /** \brief CAP 服务的相关函数 */
    const struct awbl_cap_servopts *p_servfuncs;

    /**
     * \brief cap 服务自定义的数据
     *
     * 此参数在cap接口模块搜索服务时由驱动设置，在调用服务函数时作为第一个参数。
     */
    void *p_cookie;
};

/**
 * \brief AWBus cap 接口模块初始化
 *
 * \attention 本函数应当在 awbl_dev_init2() 之后调用
 */
void awbl_cap_init(void);


#ifdef __cplusplus
}
#endif

#endif
