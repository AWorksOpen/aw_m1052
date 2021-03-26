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
 * \brief Apollo CLK �ӿ�ͷ�ļ�
 *
 * \internal
 * \par modification history:
 * - 1.01 14-10-22  ehf, add some interface
 * - 1.00 14-05-14  zen, first implementation
 * \endinternal
 */

#ifndef __AW_CLK_H
#define __AW_CLK_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "apollo.h"
#include "aw_cpu_clk.h"


/**
 * \brief ʹ��ʱ��
 *
 * \param[in]   clk_id ʱ�� ID (��ƽ̨����)
 *
 * \retval      AW_OK       �ɹ�
 * \retval      -EINVAL     ��Ч����
 * \retval      -EIO        ʹ��ʧ��
 */
aw_err_t aw_clk_enable(aw_clk_id_t clk_id);

/**
 * \brief ����ʱ��
 *
 * \param[in]   clk_id ʱ�� ID (��ƽ̨����)
 *
 * \retval      AW_OK       �ɹ�
 * \retval      -EINVAL     ��Ч������ʱ�ӻ�Ƶ�ʲ����ڣ�
 * \retval      -EIO        ����ʧ��
 */
aw_err_t aw_clk_disable(aw_clk_id_t clk_id);

/**
 * \brief ��ȡʱ��Ƶ��
 *
 * \param[in]   clk_id ʱ�� ID (��ƽ̨����)
 *
 * \retval    0       ʱ��Ƶ��Ϊ0�����ȡʱ��Ƶ��ʧ��
 * \retval    ��ֵ    ʱ��Ƶ��
 */
aw_clk_rate_t aw_clk_rate_get(aw_clk_id_t clk_id);

/**
 * \brief ����ʱ��Ƶ��
 *
 * \param[in]   clk_id  ʱ�� ID (��ƽ̨����)
 * \param[in]   rate    ʱ��Ƶ��
 *
 * \retval    AW_OK       �ɹ�
 * \retval    -AW_EINVAL  ��Ч������ʱ�ӻ�Ƶ�ʲ����ڣ�
 */
aw_err_t aw_clk_rate_set(aw_clk_id_t clk_id, aw_clk_rate_t rate);

/**
 * \brief ��ȡʱ����·
 *
 * \param[in]   clk_id ʱ�� ID (��ƽ̨����)
 *
 * \retval    ��ֵ   Դʱ��ID������ʱ�ӷ�Χ����˵�������ڣ�
 */
aw_clk_id_t aw_clk_parent_get(aw_clk_id_t clk_id);

/**
 * \brief ����ʱ����·
 *
 * \param[in]   clk_id      ��ʱ�� ID      (��ƽ̨����)
 * \param[in]   clk_parent  ʱ��ԴID       (��ƽ̨����)
 *
 * \retval   AW_OK       �ɹ�
 * \retval   -AW_EINVAL  ��Ч���� ��ʱ�Ӳ����ڻ��ʱ�Ӽ䲻���ڿ����õ���·��ϵ��
 */
aw_err_t aw_clk_parent_set(aw_clk_id_t clk_id, aw_clk_id_t clk_parent);

/**
 * \brief ��ȡʱ�����ô���
 *
 * \param[in]   clk_id  ʱ�� ID (��ƽ̨����)
 *
 * \retval     �Ǹ�ֵ        ���ô���
 * \retval     -AW_EINVAL    ��Ч����
 */
int aw_clk_usecount_get(aw_clk_id_t clk_id);

#ifdef __cplusplus
}
#endif  /* __cplusplus     */

#endif  /* __AW_CLK_H */

/* end of file */

