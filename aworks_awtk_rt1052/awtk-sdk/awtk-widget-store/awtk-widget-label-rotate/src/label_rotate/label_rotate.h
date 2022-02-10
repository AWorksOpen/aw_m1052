/**
 * File:   label_rotate.h
 * Author: AWTK Develop Team
 * Brief:  可旋转 label 控件
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
 * 2020-06-16 Luo ZhiMing <luozhiming@zlg.cn> created
 *
 */

#ifndef TK_LABEL_ROTATE_H
#define TK_LABEL_ROTATE_H

#include "base/widget.h"

BEGIN_C_DECLS

/**
* 文本选择方向枚举类型
**/
typedef enum _label_rotate_orientation_t {
  LABEL_ROTATE_ORIENTATION_0 = 0x0,
  LABEL_ROTATE_ORIENTATION_90,
  LABEL_ROTATE_ORIENTATION_180,
  LABEL_ROTATE_ORIENTATION_270,

  LABEL_ROTATE_ORIENTATION_COUNT = LABEL_ROTATE_ORIENTATION_270 + 1,
} label_rotate_orientation_t;

static const char* LABEL_ROTATE_DIRECTION_STRING_LIST[] = {"orientation_0", "orientation_90",
                                                           "orientation_180", "orientation_270"};

/**
 * @class label_rotate_t
 * @parent widget_t
 * @annotation ["scriptable","design","widget"]
 *
 * 可旋转的文本控件，仅可显示一行文本。
 *
 * 在xml中使用"label_\rotate"标签创建文本控件。如：
 *
 * ```xml
 * <!-- ui -->
 * <label_rotate text="label_rotate"/>
 * ```
 *
 * 在c代码中使用函数label_\rotate\_create创建文本控件。如：
 *
 * ```c
 *  widget_t* label_rotate = label_rotate_create(win, 10, 10, 128, 30);
 *  widget_set_text(label_rotate, L"hello awtk!");
 * ```
 *
 * > 创建之后，需要用widget\_set\_text或widget\_set\_text\_utf8设置文本内容。
 *
 * 可用通过style来设置控件的显示风格，如字体的大小和颜色等等。如：
 *
 * ```xml
 * <!-- style -->
 * <label_rotate>
 *   <style name="default">
 *     <normal text_color="#444444"/>
 *     <disable text_color="#44444466"/>
 *   </style>
 * </label_rotate>
 * ```
 */
typedef struct _label_rotate_t {
  widget_t widget;

  /**
   * @property {int32_t} length
   * @annotation ["set_prop","get_prop","readable","persitent","scriptable"]
   * 该功能暂时没有效果
   * 显示字符的个数(小余0时全部显示)。
   * 主要用于动态改变显示字符的个数，来实现类似[拨号中...]的动画效果。
   */
  int32_t length;

  /**
  * @property {label_rotate_orientation_t} orientation
  * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
  * 用于文本选择方向，可选方向有 0、90、180、270。
  */
  label_rotate_orientation_t orientation;

} label_rotate_t;

/**
 * @method label_rotate_create
 * @annotation ["constructor", "scriptable"]
 * 创建label_rotate对象
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* label_rotate_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method label_rotate_set_length
 * 设置显示字符的个数(小余0时全部显示)。（该功能暂时没有效果）
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {int32_t}  length 最大可显示字符个数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t label_rotate_set_length(widget_t* widget, int32_t length);

/**
* @method label_rotate_set_orientation
* 设置文本选择方向。
* @annotation ["scriptable"]
* @param {widget_t*} widget 控件对象。
* @param {label_rotate_orientation_t}  orientation 旋转方向。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t label_rotate_set_orientation(widget_t* widget, label_rotate_orientation_t orientation);

/**
 * @method label_rotate_cast
 * 转换为label_rotate对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget label_rotate对象。
 *
 * @return {widget_t*} label_rotate对象。
 */
widget_t* label_rotate_cast(widget_t* widget);

#define LABEL_ROTATE(widget) ((label_rotate_t*)(label_rotate_cast(WIDGET(widget))))

#define WIDGET_TYPE_LABEL_ROTATE "label_rotate"

#define WIDGET_PROP_ORIENTATION "orientation"

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(label_rotate_t);

END_C_DECLS

#endif /*TK_LABEL_ROTATE_H*/
