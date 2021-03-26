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
 * \brief AWorks1.0 PSP start up entry file.
 *
 * \internal
 * \par History
 * - 1.00 13-01-10  zen, first implementation.
 * \endinternal
 */

#ifdef AW_IMG_PRJ_BUILD

/*
 *  For aworks1.0:
 *
 *------------------------------------------------------------------------------
 *  Image Project:
 *
 *  xxx.c                                                          aw_main()
 *                                                                    /|\
 *  aw_prj_usr_app_init.c:                                aw_prj_usr_app_init()
 *                                                                     .
 *  aw_prj_config.c: aw_prj_early_init()    aw_prj_task_level_init()  /|\
 *                           .                        .                |
 *                          /|\                      /|\               |
 *                           |                        |                |
 *                           |                        |                |
 *---------------------------|-----------------------------------------|--------
 *  AWorks PSP               |                        |                |
 *                           |                        |                |
 *  aw_psp_startup.c:        |                        |    aw_psp_usr_app_init()
 *                           |                        |                .
 *  aw_psp_startup.c:   aw_psp_early_init() aw_psp_task_level_init()  /|\
 *                           .                        .                |
 *                          /|\                      /|\               |
 *                           |                        |                |
 *                           |                        |                |
 *---------------------------|------------------------|----------------|--------
 *  Aworks1.0                |                        |                |
 *                           |                        |                |
 *  aworks_startup.c: aworks_startup()----> aworks_task_level_init() --|
 *                    .         |                         |
 *                   /|\        |                         |
 *                    |         |                         |
 *                    |        \|/                       \|/
 *                    |         .                         .
 *  BSP:              |  aw_bsp_early_init()      aw_bsp_task_level_init()
 *                    |
 *                    |
 *                __main()
 *                   /|\
 *                    |
 *--------------------|---------------------------------------------------------
 *  hw reset  --------|
 */
#include "aw_common.h"

/*******************************************************************************
  import
*******************************************************************************/
aw_import void aw_prj_early_init(void);
aw_import void aw_prj_task_level_init(void);

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief PSP early initialization entry
 */
void aw_psp_early_init (void)
{
    /* add PSP early initialization code here */

    aw_prj_early_init();
}

/**
 * \brief PSP task level initialization entry
 */
void aw_psp_task_level_init (void)
{
    /* add PSP task level initialization code  here */

    aw_prj_task_level_init();
}

#endif /* AW_IMG_PRJ_BUILD */

/* end of file */

