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

#ifndef __AW_LUA_H
#define __AW_LUA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief 向 aw_shell 注册 Lua 解释器的入口。
 */
extern void  aw_lua_shell_init (void);


/**
 * \brief 向 aw_shell 注册 Luac 编译器的入口。
 */
extern void aw_luac_shell_init (void);

#ifdef __cplusplus
}
#endif

#endif /* __AW_LUA_H */

/* end of file */
