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
 * \brief configuration file for internal use
 *
 * This file can't be compiled into the library, but link it when build image.
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-24  zen, first implementation
 * \endinternal
 */

#ifdef AW_IMG_PRJ_BUILD

#ifdef AXIO_AWORKS
#include "aw_buslite_autoconf.h"
#endif
/*******************************************************************************
  headers
*******************************************************************************/

#include "apollo.h"

#include "awbus_lite.h"

/* todo: check the configuration parameter */

/*******************************************************************************
  imports
*******************************************************************************/
aw_import void awbus_lite_init2 (void   **pp_drvinfos,
                                 size_t   drvinfo_count,
                                 void   **p_bustype_infos,
                                 size_t   bustype_info_count);

/*******************************************************************************
  locals
*******************************************************************************/
#ifdef AXIO_AWORKS
/** \brief allocate the memory for driver informations statically */
aw_local void *__g_drvinfos[CONFIG_AW_CFG_AWBUS_LITE_DRIVER_MAX];

/** \brief allocate the memory for bustype informations statically */
aw_local void *__g_bustype_infos[CONFIG_AW_CFG_AWBUS_LITE_BUSTYPE_MAX];

#else
/** \brief allocate the memory for driver informations statically */
aw_local void *__g_drvinfos[AW_CFG_AWBUS_LITE_DRIVER_MAX];

/** \brief allocate the memory for bustype informations statically */
aw_local void *__g_bustype_infos[AW_CFG_AWBUS_LITE_BUSTYPE_MAX];
#endif
/*******************************************************************************
  implementation
*******************************************************************************/

/******************************************************************************/
void awbus_lite_init (void)
{
    awbus_lite_init2(&__g_drvinfos[0],
                     AW_NELEMENTS(__g_drvinfos),
                     &__g_bustype_infos[0],
                     AW_NELEMENTS(__g_bustype_infos));
}

#endif /* AW_IMG_PRJ_BUILD */

/* end of file */
