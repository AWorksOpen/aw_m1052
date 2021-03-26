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
 * \brief ����ʧ�Դ洢���ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_nvram.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 12-10-30  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_NVRAM_H
#define __AWBL_NVRAM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_nvram.h"

/** \brief �洢�Σ��洢һ������ʧ���� */
struct awbl_nvram_segment {
    char        *p_name;    /**< \brief �洢������ */
    
    /** \brief �洢�εĵ�Ԫ�ţ��������ֶ��������ͬ�Ĵ洢�� */
    int          unit;  
    
    uint32_t     seg_addr;  /**< \brief �洢���ڴ洢�����е���ʼ��ַ  */
    uint32_t     seg_size;  /**< \brief �洢�εĴ�С */
};

/* \brief AWBL_METHOD_CALL(awbl_nvram_get) �ķ������������� */
typedef aw_err_t (*pfunc_awbl_nvram_get_t)(struct awbl_dev *p_dev,
                                           char            *p_name,
                                           int              unit,
                                           char            *p_buf,
                                           int              offset,
                                           int              len);

/* \brief AWBL_METHOD_CALL(awbl_nvram_get) �ķ������������� */
typedef aw_err_t (*pfunc_awbl_nvram_set_t)(struct awbl_dev *p_dev,
                                           char            *p_name,
                                           int              unit,
                                           char            *p_buf,
                                           int              offset,
                                           int              len);

#ifdef __cplusplus
}
#endif  /* __cplusplus 	*/

#endif  /* __AWBL_NVRAM_H */

/* end of file */
