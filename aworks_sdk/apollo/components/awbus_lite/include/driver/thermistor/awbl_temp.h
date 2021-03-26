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
 * \brief �¶ȴ��������
 * 
 * \internal
 * \par modification history
 * - 1.00 14-07-30  hbt, first implementation.
 * \endinternal
 */

#ifndef __AWBL_TEMP_H
#define __AWBL_TEMP_H

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_types.h"
#include "aw_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_temperature
 * @{
 */

/* �¶ȴ������豸ID */
#define TEMP_1      0
#define TEMP_2      1
#define TEMP_3      2
#define TEMP_4      3
#define TEMP_5      4
#define TEMP_6      5
#define TEMP_7      6
#define TEMP_8      7

/** \brief �¶ȴ�����������غ��� */
struct awbl_temp_servfuncs {

    /** \brief  ��ȡ������ */
    int (*pfunc_read) (void *p_cookie);
};

/** \brief  �¶ȴ�������ط��� */
struct awbl_temp_service {
    
    /** \brief ָ����һ������ */
    struct awbl_temp_service   *p_next;
    
    /** \brief ����ID���豸ID�����IDһһ��Ӧ */
    unsigned int                id;
    
    /** \brief �¶ȴ�����������غ��� */
    struct awbl_temp_servfuncs  *p_servfuncs;
};

/******************************************************************************/
void awbl_ntc_therm_drv_register (void);
aw_err_t awbl_temp_serv_add (struct awbl_temp_service *p_therm_dev);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_TEMP_H */

/* end of file */
