/**
 * File:   date_picker.h
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

#ifndef TK_DATE_PICKER_H
#define TK_DATE_PICKER_H

#include "base/widget.h"

BEGIN_C_DECLS
/**
 * @class date_picker_t
 * @parent widget_t
 * @annotation ["scriptable","design","widget"]
 * 日期选择控件。
 * date\_picker\_t是[widget\_t](widget_t.md)的子类控件，widget\_t的函数均适用于date\_picker\_t控件。
 * 
 * date\_picker 需要几个特殊名称的子控件，它们的名称和功能如下： 
 * 
 * * prev_year 切换到前一年的按钮。
 * * prev_month 切换到前一月的按钮。
 * * year_month 显示当前年月的文本。
 * * next_month 切换到后一月的按钮。
 * * next_year 切换到后一年的按钮。
 * * days 存放"天"的容器控件，里面放42个按钮。
 * * today "今天"按钮。
 * 
 * 子控件的大小和风格可以自行设定，遵循命名规则即可。
 * 
 * 在xml中使用"date\_picker"标签创建日期选择控件。如：
 *
 * ```xml
 * <!-- ui -->
 * <date_picker x="c" y="m" w="200" h="200">
 *   <view name="title" x="0" y="0" w="100%" h="30">
 *     <button name="prev_year" x="0" y="m" w="30" h="26" text="<<"/>
 *     <button name="prev_month" x="35" y="m" w="30" h="26" text="<"/>
 *     <label name="year_month" x="c" y="m" w="50%" h="100%" text="2020/7"/>
 *     <button name="next_month" x="r:35" y="m" w="30" h="26" text=">"/>
 *     <button name="next_year" x="r" y="m" w="30" h="26" text=">>"/>
 *   </view>
 *
 *   <view name="week" x="0" y="32" w="100%" h="30" children_layout="default(r=1,c=7,m=2,s=2)">
 *     <label name="sun" tr_text="Su"/>
 *     <label name="mon" tr_text="Mo"/>
 *     <label name="tue" tr_text="Tu"/>
 *     <label name="wed" tr_text="We"/>
 *     <label name="thu" tr_text="Th"/>
 *     <label name="fri" tr_text="Fr"/>
 *     <label name="sat" tr_text="Sa"/>
 *   </view>
 *
 *   <view name="days" x="0" y="64" w="100%" h="-100" children_layout="default(r=6,c=7,m=2,s=2)">
 *     <button text="0"/>
 *     <button text="1"/>
 *     <button text="2"/>
 *     <button text="3"/>
 *     <button text="4"/>
 *     <button text="5"/>
 *     <button text="6"/>
 *      
 *     <button text="0"/>
 *     <button text="1"/>
 *     <button text="2"/>
 *     <button text="3"/>
 *     <button text="4"/>
 *     <button text="5"/>
 *     <button text="6"/>
 *     
 *     <button text="0"/>
 *     <button text="1"/>
 *     <button text="2"/>
 *     <button text="3"/>
 *     <button text="4"/>
 *     <button text="5"/>
 *     <button text="6"/>
 *
 *     <button text="0"/>
 *     <button text="1"/>
 *     <button text="2"/>
 *     <button text="3"/>
 *     <button text="4"/>
 *     <button text="5"/>
 *     <button text="6"/>
 *
 *     <button text="0"/>
 *     <button text="1"/>
 *     <button text="2"/>
 *     <button text="3"/>
 *     <button text="4"/>
 *     <button text="5"/>
 *     <button text="6"/>
 *
 *     <button text="0"/>
 *     <button text="1"/>
 *     <button text="2"/>
 *     <button text="3"/>
 *     <button text="4"/>
 *     <button text="5"/>
 *     <button text="6"/>
 *   </view>
 *   <button name="today" x="c" y="b:2" w="60" h="30" tr_text="Today"/>
 * </date_picker>
 * ```
 *
 * 可用通过style来设置控件的显示风格，如字体的大小和颜色等等。如：
 *
 * ```xml
 * <!-- style -->
 * <button>
 *   <style name="day">
 *     <normal text_color="#222222"/>
 *     <pressed text_color="#ffffff" bg_color="#1971dd" border_color="#1971dd"/>
 *     <over text_color="#ffffff" bg_color="#338fff" border_color="#338fff"/>
 *     <focused text_color="#444444" bg_color="#f4f4f4" border_color="#338fff" />
 *     <disable text_color="gray"/>
 *   </style>
 *   <style name="day_current">
 *     <normal text_color="#444444" bg_color="#338fff"/>
 *     <pressed text_color="#ffffff" bg_color="#1971dd" border_color="#1971dd"/>
 *     <over text_color="#ffffff" bg_color="#338fff" border_color="#338fff"/>
 *     <focused text_color="#444444" bg_color="#f4f4f4" border_color="#338fff" />
 *     <disable text_color="#444444" />
 *   </style>
 * </button>
 * ```
 */
typedef struct _date_picker_t {
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
} date_picker_t;

/**
 * @event {event_t} EVT_VALUE_CHANGED
 * 值改变事件。
 */

/**
 * @method date_picker_create
 * @annotation ["constructor", "scriptable"]
 * 创建date_picker对象
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} date_picker对象。
 */
widget_t* date_picker_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method date_picker_cast
 * 转换为date_picker对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget date_picker对象。
 *
 * @return {widget_t*} date_picker对象。
 */
widget_t* date_picker_cast(widget_t* widget);

/**
 * @method date_picker_set_year
 * 设置 年。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} year 年。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t date_picker_set_year(widget_t* widget, uint32_t year);

/**
 * @method date_picker_set_month
 * 设置 月。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} month 月。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t date_picker_set_month(widget_t* widget, uint32_t month);

/**
 * @method date_picker_set_day
 * 设置 日。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} day 日。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t date_picker_set_day(widget_t* widget, uint32_t day);

#define DATE_PICKER_PROP_YEAR "year"
#define DATE_PICKER_PROP_MONTH "month"
#define DATE_PICKER_PROP_DAY "day"

#define WIDGET_TYPE_DATE_PICKER "date_picker"

#define DATE_PICKER(widget) ((date_picker_t*)(date_picker_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(date_picker);

END_C_DECLS

#endif /*TK_DATE_PICKER_H*/
