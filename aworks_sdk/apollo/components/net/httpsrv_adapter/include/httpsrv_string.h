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
 * \brief Compiler defines for lwip.
 *
 * \internal
 * \par History
 * - 130604, sdy, modify for anywhere platform.
 * - 110506, sdy, First implementation.
 * \endinternal
 */

#ifndef __HTTPSRV_STRING_H__
#define __HTTPSRV_STRING_H__

#include "aw_types.h"

#ifndef lwip_strnicmp
/* 
 * This can be #defined to strnicmp() or strncasecmp() 
 * depending on your platform 
 */
int   lwip_strnicmp(const char* str1, const char* str2, size_t len);
#endif
#ifndef lwip_stricmp
/* 
 * This can be #defined to stricmp() or strcasecmp() 
 * depending on your platform
 */
int   lwip_stricmp(const char* str1, const char* str2);
#endif


#endif /* __HTTPSRV_ARCH_H__ */

/* end of file */

