/*
 * webui.h
 *
 *  Created on: 2017年12月14日
 *      Author: chenminghao
 */

#ifndef USER_CODE_NETRWORK_APP_WEBUI_H_
#define USER_CODE_NETRWORK_APP_WEBUI_H_

#define HTTPAUTH_STORAGE_TAG        "TAG"
#define HTTPAUTH_DEFAULT_TOKEN      "admin"

/******************************************************************************/

#define HTTP_AUTH_VAL_TO_REAL(x)    ((uint8_t *) x + (sizeof HTTPAUTH_STORAGE_TAG - 1))

/******************************************************************************/

void webui_init (uint16_t port);

/******************************************************************************/

#endif /* USER_CODE_NETRWORK_APP_WEBUI_H_ */
