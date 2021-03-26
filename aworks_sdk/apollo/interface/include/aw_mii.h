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
 * \brief AWorks MII�����ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_mii.h
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
 * \brief ��̫��ģʽ
 */
#define     AW_ETH_MODE_AUTONEG         0x1
#define     AW_ETH_MODE_1000M_FULL      0x2
#define     AW_ETH_MODE_1000M_HALF      0x3
#define     AW_ETH_MODE_100M_FULL       0x4
#define     AW_ETH_MODE_100M_HALF       0x5
#define     AW_ETH_MODE_10M_FULL        0x6
#define     AW_ETH_MODE_10M_HALF        0x7


/**
 * \brief ������̫�����ٶȺ�˫��ģʽ
 * 
 * \param[in]   p_name  ������
 * \param[in]   mode    ��Ҫ���õ�ģʽ���μ�"��̫��ģʽ"
 * 
 * \return  AW_OK �ɹ�
 * 
 * \par ��ʾ��
 * \code
 * #include "aw_mii.h"
 * 
 * aw_err_t ret;
 * ret = aw_eth_mode_set("eth0", AW_ETH_MODE_10M_FULL); //������0����Ϊ10Mȫ˫�� 
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
