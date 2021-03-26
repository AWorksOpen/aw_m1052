/**
 * File:   mask_view_register.h
 * Author: AWTK Develop Team
 * Brief:  前景为蒙板裁剪裁剪背景色的功能
 *
 * Copyright (c) 2020 - 2020 Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2020-07-22 Luo ZhiMing <luozhiming@zlg.cn> created
 *
 */

#ifndef TK_MASK_VIEW_REGISTER_H
#define TK_MASK_VIEW_REGISTER_H

#include "base/widget.h"

BEGIN_C_DECLS

/**
 * @method  mask_view_register
 * 注册控件。
 *
 * @annotation ["global"]
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t mask_view_register(void);

/**
 * @method  mask_view_supported_render_mode
 * 获取支持的渲染模式。
 *
 * @annotation ["global"]
 *
 * @return {const char*} 返回渲染模式。
 */
const char* mask_view_supported_render_mode(void);

END_C_DECLS

#endif /*TK_MASK_VIEW_REGISTER_H*/
