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
 * \brief ftl config
 *
 * \internal
 * \par modification history:
 * - 1.00 2017-4-15   vih, first implementation.
 * \endinternal
 */

#ifndef __AW_FTL_CONFIG_H
#define __AW_FTL_CONFIG_H

/**
 * \addtogroup grp_aw_if_ftl_config
 * \copydoc aw_ftl_config.h
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** \brief ��־���������Զ�����Ϊ-1  */
#define AW_FTL_CONFIG_NLOGBLKS             -1

/** \brief ����������  */
#define AW_FTL_CONFIG_NRSBLKS               4

/** \brief д����֮���Զ��������ݽ��м��Ĺ��ܣ�Ϊ1������0�ر�  */
#define AW_FTL_CONFIG_WRITE_VERIFY          1

/** \brief ��黵��ʱ���Ƿ���д������ݵĿ��أ�1Ϊ����0Ϊ��  */
#define AW_FTL_CONFIG_BAD_CHECK_LEVEL_HIGH  1


/** @}  grp_aw_if_ftl_config */

#ifdef __cplusplus
}
#endif

#endif /* __AW_FTL_CONFIG_H */



/* end of file */
