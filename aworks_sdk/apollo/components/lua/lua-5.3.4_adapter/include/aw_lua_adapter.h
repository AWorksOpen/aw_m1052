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
 * \brief lua entrance function.
 *
 * \internal
 * \par modification history:
 * - 180306, imp, first implemetation
 * \endinternal
 */

#ifndef __AW_LUA_ADAPTER_H
#define __AW_LUA_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

int lua_main (int argc, char **argv);

int luac_main(int argc, char* argv[]);

#ifdef __cplusplus
}
#endif

#endif /* __AW_LUA_ADAPTER_H */

/* end of file */
