/**
 * File:   date_picker_register.h
 * Author: AWTK Develop Team
 * Brief:  日期选择控件。
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
 * 2020-07-12 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef TK_DATE_PICKER_REGISTER_H
#define TK_DATE_PICKER_REGISTER_H

#include "base/widget.h"

BEGIN_C_DECLS

/**
 * @method  date_picker_register
 * 注册控件。
 *
 * @annotation ["global"]
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t date_picker_register(void);

/**
 * @method  date_picker_supported_render_mode
 * 获取支持的渲染模式。
 *
 * @annotation ["global"]
 *
 * @return {const char*} 返回渲染模式。
 */
const char* date_picker_supported_render_mode(void);

END_C_DECLS

#endif /*TK_DATE_PICKER_REGISTER_H*/
