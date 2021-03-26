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
 * \brief STREAMS interface.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-08-22 orz, first implementation.
 * \endinternal
 */

#ifndef __IO_STROPTS_H /* { */
#define __IO_STROPTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_ios_stropts
 * @{
 */

#include "aworks.h"
#include "aw_ioctl.h"

#define I_FLUSH     AW_FIOFLUSH
#define I_NREAD     AW_FIONREAD


/**
 * \brief ����io��������
 *
 * \param[in]         filedes     �ļ�������
 * \param[in]         request     ����
 * \param[out]        argument    ����
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_ioctl (int filedes, int request, void *argument);

/** @} grp_ios_stropts */

#ifdef __cplusplus
}
#endif

#endif /* } __IO_STROPTS_H */

/* end of file */
