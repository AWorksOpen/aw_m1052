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
 * \brief 通用DAC接口
 *
 * 使用本服务需要包含头文件 
 * \code 
 * #include aw_dac.h
 * \endcode
 *
 * \par 简单示例
 * \code
 *  #include aw_dac.h
 *
 *  int dac_bits = aw_dac_get_bits(0);        // 读取DAC通道0的转换位数
 *  int dac_vref = aw_dac_vref_get(0);        // 读取DAC通道0的参考电压(单位：mV)
 *
 *  aw_dac_val_t dac_val；
 *
 *  dac_val = 1200 * (1 << dac_bits) / dac_vref;
 *
 *  // 设置DAC通道0的输出电压为 1.2V (未使能，还未实际输出)
 *  aw_dac_val_set(0, dac_val);               
 *  // 使能输出，输出电压为1.2V
 *  aw_dac_enable(0);                         
 *
 *  dac_val = 1400 * (1 << dac_bits) / dac_vref; 
 *  // 设置DAC通道0的输出电压为 1.4V (前面已使能，实际输出电压1.4V)
 *  aw_dac_val_set(0, dac_val);               
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 15-04-02 tee, first implementation
 * \endinternal
 */

#ifndef __AW_DAC_H
#define __AW_DAC_H

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * \addtogroup grp_aw_if_dac
 * \copydoc aw_dac.h
 * @{
 */

#include "apollo.h"
#include "aw_psp_dac.h"
#include "aw_list.h"

/**
 * \brief DAC通道类型定义
 */
typedef aw_psp_dac_channel_t    aw_dac_channel_t;

/**
 * \brief DAC值类型定义
 */
typedef aw_psp_dac_val_t        aw_dac_val_t;


/**
 * \brief 获取一个DAC通道的位数
 *
 * \param[in] ch  DAC通道号
 *
 * \retval >0          该DAC通道的有效位数
 * \retval -AW_ENXIO   DAC通道通道号不存在
 * \retval -AW_EFAULT  操作错误
 */
int aw_dac_bits_get(aw_dac_channel_t ch);

/**
 * \brief 获取一个DAC通道的参考电压(单位：mV)
 *
 * \param[in] ch  DAC通道号
 *
 * \retval >0          该DAC通道对应的参考电压
 * \retval -AW_ENXIO   DAC通道通道号不存在
 * \retval -AW_EFAULT  操作错误
 */
int aw_dac_vref_get(aw_dac_channel_t ch);

/**
 * \brief 设置一个DAC通道的数字量值
 *
 * \param[in] ch  DAC通道号
 * \param[in] val 待设置的数字量值
 *
 * \retval   AW_OK      操作成功
 * \retval  -AW_ENXIO   DAC通道号不存在
 * \retval  -AW_EFAULT  操作错误
 * \retval  -AW_EINVAL  无效参数，待设置的数字量值过大
 */
aw_err_t aw_dac_val_set(aw_dac_channel_t ch, aw_dac_val_t val);
                         
/**
 * \brief 使能一个DAC通道输出模拟量
 *
 * \param[in] ch  DAC通道号
 *
 * \retval   AW_OK      操作成功
 * \retval  -AW_EFAULT  操作错误
 * \retval  -AW_ENXIO   DAC通道号不存在
 *
 * \note 关于该使能函数和aw_adc_val_set()函数的调用先后关系，说明如下：
 *       如果先调用该使能函数，则DAC使能后输出电压为0，可以通过后续调用
 *       aw_dac_val_set()设置为需要的值。如果先调用了用aw_dac_val_set()
 *       函数设置了电压值，则使能后即为相应的电压输出值。
 */
aw_err_t aw_dac_enable(aw_dac_channel_t ch);

/**
 * \brief 禁能一个DAC通道输出模拟量
 *
 * \param[in] ch  DAC通道号
 *
 * \retval   AW_OK      操作成功
 * \retval  -AW_EFAULT  操作错误
 * \retval  -AW_ENXIO   DAC通道号不存在
 */
aw_err_t aw_dac_disable(aw_dac_channel_t ch);
                        
                        
/**
 * \brief 设置一个DAC通道的数字量值
 *
 * \param[in] ch  DAC通道号
 * \param[in] mv  待设置的数字量值
 *
 * \retval   AW_OK      操作成功
 * \retval  -AW_ENXIO   DAC通道号不存在
 * \retval  -AW_EFAULT  操作错误
 * \retval  -AW_EINVAL  无效参数，待设置的数字量值过大
 */
aw_err_t aw_dac_mv_set(aw_dac_channel_t ch, int mv);
                       
/** @} grp_aw_if_dac */

#ifdef __cplusplus
}
#endif

#endif  /* __AW_dac_H */

/* end of file */
