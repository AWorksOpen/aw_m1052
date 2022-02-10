﻿
/*This file is generated by code generator*/

#include "room_settings.h"
#include "mvvm/base/view_model.h"

#ifndef TK_ROOM_SETTINGS_VIEW_MODEL_H
#define TK_ROOM_SETTINGS_VIEW_MODEL_H

BEGIN_C_DECLS
/**
 * @class room_settings_view_model_t
 *
 * view model of room_settings
 *
 */
typedef struct _room_settings_view_model_t {
  view_model_t view_model;

  /*model object*/
  room_settings_t* aroom_settings;
} room_settings_view_model_t;

/**
 * @method room_settings_view_model_create
 * 创建room_settings view model对象。
 *
 * @annotation ["constructor"]
 * @param {navigator_request_t*} req 请求参数。
 *
 * @return {view_model_t} 返回view_model_t对象。
 */
view_model_t* room_settings_view_model_create(navigator_request_t* req);

/**
 * @method room_settings_view_model_create_with
 * 创建room_settings view model对象。
 *
 * @annotation ["constructor"]
 * @param {room_settings_t*}  aroom_settings room_settings对象。
 *
 * @return {view_model_t} 返回view_model_t对象。
 */
view_model_t* room_settings_view_model_create_with(room_settings_t* aroom_settings);

/**
 * @method room_settings_view_model_attach
 * 关联到room_settings对象。
 *
 * @param {view_model_t*} view_model view_model对象。
 * @param {room_settings_t*} room_settings room_settings对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t room_settings_view_model_attach(view_model_t* vm, room_settings_t* aroom_settings);

END_C_DECLS

#endif /*TK_ROOM_SETTINGS_VIEW_MODEL_H*/