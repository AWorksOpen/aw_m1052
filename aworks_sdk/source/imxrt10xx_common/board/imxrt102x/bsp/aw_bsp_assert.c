/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.embedcontrol.com/
*******************************************************************************/

/**
 * \file
 * \brief ���Եļ�ʵ��
 *
 * ʹ�ñ�������Ҫ��������ͷ�ļ���
 * \code
 * #include "aw_assert.h"
 * \endcode
 *
 * // �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history
 * - 1.00 12-10-23  orz, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_vdebug.h"

/**
 * \addtogroup grp_aw_if_assert
 * \copydetails aw_assert.h
 * @{
 */

void aw_assert_msg (const char *msg)
{
	volatile int exit = 0;

	(void)aw_kprintf(msg);

	while (0 == exit) {
	}
}

/** @}  aw_grp_if_assert */

/* end of file */
