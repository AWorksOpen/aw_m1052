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
 * \brief Apollo psp application initialzation entry file.
 *
 * \internal
 * \par History
 * - 1.00 13-01-10  zen, first implementation.
 * \endinternal
 */

#ifdef AW_IMG_PRJ_BUILD

/*******************************************************************************
  headers
*******************************************************************************/

#include "apollo.h"

/*******************************************************************************
  import
*******************************************************************************/
aw_import int aw_prj_usr_app_init(void);

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief user application initialization entry
 */
int aw_psp_usr_app_init (void)
{

    /* add psp application initialization code here */

    return aw_prj_usr_app_init();
}

#endif /* AW_IMG_PRJ_BUILD */

/* end of file */
