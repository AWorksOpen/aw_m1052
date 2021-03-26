/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief ini head file
 *
 * \internal
 * \par modification history:
 * - 1.00 16-10-31  imp, add some interface.
 * - 1.00 16-10-20  imp, refactoring code, use hash table.
 *                       add aw_ini_size() definition.
 * - 1.00 15-11-27  deo, first implementation
 * \endinternal
 */

#ifndef __AW_INI_LIB_H
#define __AW_INI_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_ini_lib
 * \copydoc aw_ini_lib.h
 * @{
 */


/**
 * \brief It provided a common method that does not make a instance 
 *        of aw_ini_lib.
 */
char *aw_ini_read_file_key (char            *p_file,
                            unsigned int     len,
                            const char      *section,
                            const char      *key);

/** @}  grp_aw_ini_lib */

#ifdef __cplusplus
}
#endif

#endif /* __AW_INI_LIB_H */

/* end of file */

