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
 * \brief AWorks MII操作接口
 *
 * 使用本服务需要包含头文件 aw_mii.h
 *
 * \internal
 * \par modification history:
 *
 * - 1.00 19-05-24  hsg, first implementation
 * \endinternal
 */

#ifndef __AW_MII_H
#define __AW_MII_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief 以太网模式
 */
#define     AW_ETH_MODE_AUTONEG         0x1
#define     AW_ETH_MODE_1000M_FULL      0x2
#define     AW_ETH_MODE_1000M_HALF      0x3
#define     AW_ETH_MODE_100M_FULL       0x4
#define     AW_ETH_MODE_100M_HALF       0x5
#define     AW_ETH_MODE_10M_FULL        0x6
#define     AW_ETH_MODE_10M_HALF        0x7


/**
 * \brief 设置以太网的速度和双工模式
 * 
 * \param[in]   p_name  网络名
 * \param[in]   mode    需要设置的模式，参见"以太网模式"
 * 
 * \return  AW_OK 成功
 * 
 * \par 简单示例
 * \code
 * #include "aw_mii.h"
 * 
 * aw_err_t ret;
 * ret = aw_eth_mode_set("eth0", AW_ETH_MODE_10M_FULL); //将网络0设置为10M全双工 
 * if (ret != AW_OK) {
 *     aw_kprintf("err\r\n");
 *  }
 * \endcode
 */
aw_err_t aw_eth_mode_set (char *p_name, uint32_t mode);


#ifdef __cplusplus
}
#endif

#endif /* __AW_MII_H */

/* end of file */
