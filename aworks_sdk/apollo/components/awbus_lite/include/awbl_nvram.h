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
 * \brief 非易失性存储器接口头文件
 *
 * 使用本模块需要包含以下头文件：
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

/** \brief 存储段，存储一条非易失数据 */
struct awbl_nvram_segment {
    char        *p_name;    /**< \brief 存储段名字 */
    
    /** \brief 存储段的单元号，用于区分多个名字相同的存储段 */
    int          unit;  
    
    uint32_t     seg_addr;  /**< \brief 存储段在存储器件中的起始地址  */
    uint32_t     seg_size;  /**< \brief 存储段的大小 */
};

/* \brief AWBL_METHOD_CALL(awbl_nvram_get) 的方法处理函数类型 */
typedef aw_err_t (*pfunc_awbl_nvram_get_t)(struct awbl_dev *p_dev,
                                           char            *p_name,
                                           int              unit,
                                           char            *p_buf,
                                           int              offset,
                                           int              len);

/* \brief AWBL_METHOD_CALL(awbl_nvram_get) 的方法处理函数类型 */
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
