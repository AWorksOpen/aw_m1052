/**
 * File:   mask_view.h
 * Author: AWTK Develop Team
 * Brief:  蒙板裁剪控件
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


#ifndef TK_MASK_VIEW_H
#define TK_MASK_VIEW_H

#include "tkc/darray.h"
#include "base/bitmap.h"
#include "base/widget.h"
#include "base/canvas_offline.h"

BEGIN_C_DECLS
/**
 * @class mask_view_t
 * @parent widget_t
 * @annotation ["scriptable","design","widget"]
 * 蒙板裁剪控件
 * 
 * 该控件主要用于异形裁剪，通过子控件（mask\_view）来设置蒙板采样区和背景采样区，背景采样区会根据蒙板采样区的灰度来设置其透明通道的值来实现裁剪效果。
 * 详细用法请看 awtk-widget-mask-view/docs/mask_view控件用法.md。
 * 
 * 在xml中使用"mask\_view"标签创建mask\_view控件。如：
 *
 * ```xml
 * <!-- ui -->
 *  <mask_view x="c" y="m" w="120" h="96" fg_interval_time="1000" bg_interval_time="16" >
 *   <mask_view_item x="0" y="0" w="100%" h="100%" mask_view="true">
 *   </mask_view_item>
 *   <mask_view_item x="0" y="0" w="100%" h="100%" mask_view="false">
 *   </mask_view_item>
 * </mask_view>
 * ```
 *
 * 可用通过style来设置控件的显示风格，如字体的大小和颜色等等。如：
 *
 * ```xml
 * <!-- style -->
 * <mask_view>
 *   <style name="default">
 *     <normal />
 *   </style>
 * </mask_view>
 * ```
 */
typedef struct _mask_view_t {
  widget_t widget;

  /**
   * @property {int32_t} fg_interval_time
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 前景蒙板刷新间隔。
   */
  int32_t fg_interval_time;

  /**
   * @property {int32_t} bg_interval_time
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 背景刷新间隔。
   */
  int32_t bg_interval_time;

  /*private*/

  uint64_t last_fg_time;
  uint64_t last_bg_time;

  uint8_t* bg_alpha_data;
  bitmap_t online_bitmap;

  darray_t bg_rect_views;
  darray_t fg_mask_views;

  canvas_t* bg_canvas;
  canvas_t* fg_mask_canvas;

  rect_t last_fg_mask_drity_rect;

} mask_view_t;

/**
 * @method mask_view_create
 * @annotation ["constructor", "scriptable"]
 * 创建mask_view对象
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} mask_view对象。
 */
widget_t* mask_view_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method mask_view_cast
 * 转换为mask_view对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget mask_view对象。
 *
 * @return {widget_t*} mask_view对象。
 */
widget_t* mask_view_cast(widget_t* widget);


/**
 * @method mask_view_set_fg_interval_time
 * 设置前景蒙板刷新间隔。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {int32_t} fg_interval_time 前景蒙板刷新间隔。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t mask_view_set_fg_interval_time(widget_t* widget, int32_t fg_interval_time);

/**
 * @method mask_view_set_bg_interval_time
 * 设置背景刷新间隔。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {int32_t} bg_interval_time 背景刷新间隔。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t mask_view_set_bg_interval_time(widget_t* widget, int32_t bg_interval_time);

/**
 * @method mask_view_fg_interval
 * 刷新前景蒙板。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t mask_view_fg_interval(widget_t* widget);

/**
 * @method mask_view_bg_interval
 * 刷新背景。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t mask_view_bg_interval(widget_t* widget);

/**
 * @method mask_view_update_children_list
 * 更新子集的采样区的列表。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t mask_view_update_children_list(widget_t* widget);


#define MASK_VIEW_PROP_FG_INTERVAL_TIME "fg_interval_time"
#define MASK_VIEW_PROP_BG_INTERVAL_TIME "bg_interval_time"

#define WIDGET_TYPE_MASK_VIEW "mask_view"

#define MASK_VIEW(widget) ((mask_view_t*)(mask_view_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(mask_view);

END_C_DECLS

#endif /*TK_MASK_VIEW_H*/
