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
 * \brief 温度传感器操作接口
 *
 * 使用本服务需要包含以下头文件:
 * \code
 * #include "aw_temp.h"
 * \endcode
 *
 * \par 使用示例
 * \code
 * #include "aw_temp.h"
 * 
 * int temp_val;
 *
 * temp_val = aw_temp_read(TEMP_1); // 读取温度传感器设备 TEMP_1 的温度值
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 14-07-30  hbt, first implementation.
 * \endinternal
 */

#ifndef __AW_TEMP_H
#define __AW_TEMP_H

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * \addtogroup grp_aw_if_temp
 * \copydoc aw_temp.h
 * @{
 */

/**
 * \brief 读取一个温度传感器设备的温度值
 *
 * 使用该接口可以方便的获取硬件外设上对应温度传感器的温度值。
 *
 * \param id        温度传感器设备的ID号。
 *
 * \retval >=0      该温度传感器设备的温度值，返回值为实际温度值的1000倍。
 * \retval -ENXIO   该温度传感器设备不存在。
 */
int aw_temp_read (unsigned int id);

/** @} grp_aw_if_temp */

#ifdef __cplusplus
}
#endif

#endif /* __AW_TEMP_H */

/* end of file */
