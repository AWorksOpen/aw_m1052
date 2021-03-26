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
 * \brief �¶ȴ����������ӿ�
 *
 * ʹ�ñ�������Ҫ��������ͷ�ļ�:
 * \code
 * #include "aw_temp.h"
 * \endcode
 *
 * \par ʹ��ʾ��
 * \code
 * #include "aw_temp.h"
 * 
 * int temp_val;
 *
 * temp_val = aw_temp_read(TEMP_1); // ��ȡ�¶ȴ������豸 TEMP_1 ���¶�ֵ
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
 * \brief ��ȡһ���¶ȴ������豸���¶�ֵ
 *
 * ʹ�øýӿڿ��Է���Ļ�ȡӲ�������϶�Ӧ�¶ȴ��������¶�ֵ��
 *
 * \param id        �¶ȴ������豸��ID�š�
 *
 * \retval >=0      ���¶ȴ������豸���¶�ֵ������ֵΪʵ���¶�ֵ��1000����
 * \retval -ENXIO   ���¶ȴ������豸�����ڡ�
 */
int aw_temp_read (unsigned int id);

/** @} grp_aw_if_temp */

#ifdef __cplusplus
}
#endif

#endif /* __AW_TEMP_H */

/* end of file */
