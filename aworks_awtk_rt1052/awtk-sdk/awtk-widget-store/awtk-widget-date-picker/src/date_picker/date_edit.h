/**
 * File:   date_edit.h
 * Author: AWTK Develop Team
 * Brief:  日期编辑控件。
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

#ifndef TK_DATE_EDIT_H
#define TK_DATE_EDIT_H

#include "base/widget.h"

BEGIN_C_DECLS
/**
 * @class date_edit_t
 * @parent widget_t
 * @annotation ["scriptable","design","widget"]
 * 日期编辑控件。
 * 
 * date\_edit\_t是[widget\_t](widget_t.md)的子类控件，widget\_t的函数均适用于date\_edit\_t控件。
 * 
 * date\_edit 需要两个特殊名称的子控件，它们的名称和功能如下： 
 * 
 * * date 用于显示和编辑日期，通常用edit控件，并指定input type为"date"
 * * pick 用于点击后弹出日期选择控件。通常用button控件。点击pick按钮后，会打开名为"date_picker"的弹出窗口。
 * 
 * 子控件的大小和风格可以自行设定，遵循命名规则即可。
 * 
 * 在xml中使用"date\_edit"标签创建日期编辑控件。如：
 *
 * ```xml
 * <!-- ui -->
 * <date_edit x="10" y="10" w="148" h="30" year="2020" month="12" day="31">
 *   <edit name="date" x="0" y="m" w="100%" h="100%" input_type="date" />
 *   <button name="pick" style="pick" x="r:2" y="m" w="26" h="26" text="..." />
 * </date_edit>
 * ```
 *
 * 可用通过style来设置控件的显示风格，如字体的大小和颜色等等。如：
 *
 * ```xml
 * <!-- style -->
 * <button>
 *   <style name="pick">
 *     <normal text_color="#222222"/>
 *     <pressed text_color="#ffffff" bg_color="#1971dd" border_color="#1971dd"/>
 *     <over text_color="#ffffff" bg_color="#338fff" border_color="#338fff"/>
 *     <focused text_color="#444444" bg_color="#f4f4f4" border_color="#338fff" />
 *     <disable text_color="gray"/>
 *   </style>
 * </button>
 * ```
 *
 */
typedef struct _date_edit_t {
  widget_t widget;

  /**
   * @property {uint32_t} year
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 年。
   */
  uint32_t year;

  /**
   * @property {uint32_t} month
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 月。
   */
  uint32_t month;

  /**
   * @property {uint32_t} day
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 日。
   */
  uint32_t day;

  /*private*/
  bool_t inited;
} date_edit_t;

/**
 * @event {event_t} EVT_VALUE_CHANGED
 * 值改变事件。
 */

/**
 * @method date_edit_create
 * @annotation ["constructor", "scriptable"]
 * 创建date_edit对象
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} date_edit对象。
 */
widget_t* date_edit_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method date_edit_cast
 * 转换为date_edit对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget date_edit对象。
 *
 * @return {widget_t*} date_edit对象。
 */
widget_t* date_edit_cast(widget_t* widget);

/**
 * @method date_edit_set_year
 * 设置 年。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} year 年。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t date_edit_set_year(widget_t* widget, uint32_t year);

/**
 * @method date_edit_set_month
 * 设置 月。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} month 月。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t date_edit_set_month(widget_t* widget, uint32_t month);

/**
 * @method date_edit_set_day
 * 设置 日。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} day 日。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t date_edit_set_day(widget_t* widget, uint32_t day);

#define DATE_EDIT_PROP_YEAR "year"
#define DATE_EDIT_PROP_MONTH "month"
#define DATE_EDIT_PROP_DAY "day"

#define DATE_EDIT_FORMAT "%d/%d/%d"
#define DATE_EDIT_CHILD_DATE "date"
#define DATE_EDIT_CHILD_PICK "pick"

#define WIDGET_TYPE_DATE_EDIT "date_edit"

#define DATE_EDIT(widget) ((date_edit_t*)(date_edit_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(date_edit);

END_C_DECLS

#endif /*TK_DATE_EDIT_H*/
