/** 
 * 功能说明：
 *     1.设备与服务器通讯协议的客户端实现。
 *
 * 修改历史：
 *     1.2017-8-6 李先静<lixianjing@zlg.cn> 创建。
 */

#include "command.h"
#include "settings.h"
#include "mqtt_client.h"

#ifndef DEVICE_CLIENT_H
#define DEVICE_CLIENT_H

#define DEVICE_ID_LENGTH 16

BEGIN_C_DECLS

/**
 * 设备认证信息
 */
typedef struct _dev_meta_info_t {
  /*设备标识符*/
  char id[64];
  /*设备类型*/
  char type[64];
  /*设备密钥*/
  char secret[512];
}dev_meta_info_t;

/**
 * 客户端数据结构。
 */
typedef struct _client_t {
    /*设备所有者*/
    const char* owner;
    /*设备ID*/
    const char* id;
    /*设备类型*/
    const char* type;
    /*命令分发表。*/
    command_t* cmds;
    /*设置接口。*/
    settings_t* settings;
    /*MQTT client接口。*/
    mqtt_client_t* mqtt_client;
}client_t;

/**
 * 收到的消息类型。可以是回应或命令。
 */
typedef enum _resp_packet_type_t{
    PACKET_NONE = 0,
    PACKET_RESPONSE, /*回应*/
    PACKET_COMMAND,  /*命令*/
    PACKET_NR
}resp_packet_type_t;

/*透传数据头定义*/
typedef struct _raw_header_t {
  uint32_t magic:4; /*必须为0110*/
  uint32_t version:6; /*必须为000001*/
  uint32_t save:1; /*是否存保存数据，取值为0/1（不保存/保存）*/
  uint32_t reserved:21; /*保留位*/
} raw_header_t;

/*消息类型(类型int)*/
#define STR_KEY_MSG_TYPE     "mtype"

/*设备ID(类型string)*/
#define STR_KEY_DEVICE_ID    "devid"

/*子设备ID(类型string)*/
#define STR_KEY_SUBDEV_ID    "sub_devid"

/*设备类型(类型string)*/
#define STR_KEY_DEVICE_TYPE  "devtype"

/*子设备类型(类型string)*/
#define STR_KEY_SUBDEV_TYPE  "sub_devtype"

/*名称(类型string)。如命令的名称。*/
#define STR_KEY_NAME         "name"

/*命令的ID(类型int)。如果命令中带有CMDID，表示该命令需要回执。*/
#define STR_KEY_CMDID        "cmdid"

/*时间戳(类型int)*/
#define STR_KEY_TIME         "time"

/*版本(类型string)*/
#define STR_KEY_VERSION      "version"

/*网址(类型string)。如固件的网址。*/
#define STR_KEY_URL          "url"

/*消息(类型string)。如命令执行的详情。*/
#define STR_KEY_MSG          "msg"

/*结果(类型BOOL)。如命令执行的结果。*/
#define STR_KEY_RESULT       "result"

/*设备上线的主题，设备 --> 服务器*/
#define STR_TOPIC_ONLINE             "/d2s/%s/%s/%s/online"

/*设备下线的主题，设备 --> 服务器*/
#define STR_TOPIC_OFFLINE            "/d2s/%s/%s/%s/offline"

/*设备上报数据的主题，设备 --> 服务器*/
#define STR_TOPIC_REPORT_DATA        "/d2s/%s/%s/%s/data"

/*设备上报透传数据的主题，设备 --> 服务器*/
#define STR_TOPIC_REPORT_RAW         "/d2s/%s/%s/%s/raw"

/*设备上报错误的主题，设备 --> 服务器*/
#define STR_TOPIC_REPORT_ERROR       "/d2s/%s/%s/%s/error"

/*设备上报状态的主题，设备 --> 服务器*/
#define STR_TOPIC_REPORT_STATUS      "/d2s/%s/%s/%s/status"

/*设备上报警告的主题，设备 --> 服务器*/
#define STR_TOPIC_REPORT_WARNING     "/d2s/%s/%s/%s/warning"

/*设备上报日志的主题，设备 --> 服务器*/
#define STR_TOPIC_REPORT_LOG         "/d2s/%s/%s/%s/log"

/*设备上报命令执行结果的主题，设备 --> 服务器*/
#define STR_TOPIC_REPORT_RESULT      "/d2s/%s/%s/%s/result"

/*网关添加子设备，设备 --> 服务器*/
#define STR_TOPIC_BIND_SUBDEV    "/g2s/%s/%s/%s/bind"

/*网关删除子设备，设备 --> 服务器*/
#define STR_TOPIC_UNBIND_SUBDEV    "/g2s/%s/%s/%s/unbind"

/*网关转发子设备消息的主题，网关 --> 服务器*/
#define STR_TOPIC_FORWARD_MESSAGE    "/g2s/%s/%s/%s/%s"

/*internal定义了用来完成内部业务逻辑的主题*/
/*
refresh_token:
    GATEWAY/APPUSER类型的设备，当添加/删除子设备后，重新从auth获取新的token，然后发送该主题通知mqtt刷新token
    消息payload：token内容
*/
#define STR_TOPIC_REFRESH_TOKEN      "/internal/%s/%s/refresh_token"

/*服务器发送广播到所有的主题， 服务器 --> 设备*/
#define STR_TOPIC_BROADCAST          "/s2d/b/all"
#define STR_TOPIC_BROADCAST_PREFIX   "/s2d/b/"

/*服务器发送广播到指定类型(当前可以填入devtype)的设备的主题， 服务器 --> 设备*/
#define STR_TOPIC_BROADCAST_DEVTYPE  "/s2d/b/%s"

/*服务器发送广播到指定所有者的指定类型(当前可以填入devtype)的设备的主题， 服务器 --> 设备*/
#define STR_TOPIC_BROADCAST_OWNER_DEVTYPE  "/s2d/b/%s/%s"

/*服务器发送消息到指定设备的主题， 服务器 --> 设备*/
#define STR_TOPIC_DEVICE             "/s2d/d/%s/%s/%s"
#define STR_TOPIC_DEVICE_RAW         "/s2d/d/%s/%s/%s/raw"
#define STR_TOPIC_DEVICE_PREFIX      "/s2d/d"

/*服务器发送消息到指定网关的指定节点的主题， 服务器 --> 网关设备*/
#define STR_TOPIC_GATEWAY            "/s2g/d/%s/%s/%s/%s"
#define STR_TOPIC_GATEWAY_PREFIX     "/s2g/d/"



/*配置设备参数*/
#define STR_CMD_SET_CONFIG           "set_config"

/*时间同步命令*/
#define STR_CMD_SYNC_TIME            "sync_time"

/*通知设备有新固件，设备收到通知后，可提示用户有更新但不执行更新，也可后台自动更新固件*/
#define STR_CMD_NOTIFY_UPGRADE       "notify_upgrade"

/*通知设备更新固件，设备收到通知后，需立即更新固件*/
#define STR_CMD_EXEC_UPGRADE         "exec_upgrade"

/*透传命令*/
#define STR_CMD_PASS_THROUGH         "pass_through"

/*请求设备上报最新数据。*/
#define STR_CMD_REQ_REPORT           "req_report"


#define MAX_TOPIC_LENGTH             127
#define MAX_MESSAGE_LENGTH           256

/**
 * \brief 初始客户端。
 * \param[in] client   : 客户端对象。
 * \param[in] type     : 设备类型
 * \param[in] id       : 设备ID 
 *
 * \retval 客户端对象。
 */
client_t* client_init(client_t* client, const char* owner, const char* type, const char* id);

/**
 * \brief 指定命令分发表。
 * \param[in] client : 客户端对象。
 * \param[in] cmds   : 命令分发表。
 *
 * \retval 无。
 */
void client_set_cmds(client_t* client, command_t* cmds);

/**
 * \brief 指定设置接口。
 * \param[in] client   : 客户端对象。
 * \param[in] settings : 设置接口。
 *
 * \retval 无。
 */
void client_set_settings(client_t* client, settings_t* settings);

/**
 * \brief 指定MQTT接口。
 * \param[in] client      : 客户端对象。
 * \param[in] mqtt_client : MQTT Client。
 *
 * \retval 无。
 */
void client_set_mqtt_client(client_t* client, mqtt_client_t* mqtt_client);

/**
 * \brief 分发来自MQTT的消息，一般在MQTT的消息回调函数中调用。
 * \param[in] client : 客户端对象。
 * \param[in] buff   : 数据。
 * \param[in] size   : 数据长度。
 *
 * \retval 成功返回TRUE，失败返回FALSE。
 */
BOOL client_dispatch(client_t* client, const char* buff, size_t size);

/**
 * XXX: params需要预留不小于64字节的空间，下面的函数会追加设备ID和类型到params中。
 */

/**
 * \brief 上报数据。
 * \param[in] client : 客户端对象。
 * \param[in] params : 参数列表。
 *
 * \retval 成功返回TRUE，失败返回FALSE。
 */
BOOL client_report_data(client_t* client, params_t* params);

/**
 * \brief 上报透传数据。
 * \param[in] client : 客户端对象。
 * \param[in] buff :  数据。
 * \param[in] size :  数据长度。
 * \param[in] save :  标志服务器是否保存该数据。
 *
 * \retval 成功返回TRUE，失败返回FALSE。
 */
BOOL client_report_raw(client_t* client, const unsigned char* buff, size_t size, BOOL save);

/**
 * \brief 上报状态。
 * \param[in] client : 客户端对象。
 * \param[in] params : 参数列表。
 *
 * \retval 成功返回TRUE，失败返回FALSE。
 */
BOOL client_report_status(client_t* client, params_t* params);

/**
 * \brief 上报错误事件。
 * \param[in] client : 客户端对象。
 * \param[in] params : 参数列表。
 *
 * \retval 成功返回TRUE，失败返回FALSE。
 */
BOOL client_report_error(client_t* client, params_t* params);

/**
 * \brief 上报警告事件。
 * \param[in] client : 客户端对象。
 * \param[in] params : 参数列表。
 *
 * \retval 成功返回TRUE，失败返回FALSE。
 */
BOOL client_report_warning(client_t* client, params_t* params);

/**
 * \brief 上报日志事件。
 * \param[in] client : 客户端对象。
 * \param[in] params : 参数列表。
 *
 * \retval 成功返回TRUE，失败返回FALSE。
 */
BOOL client_report_log(client_t* client, params_t* params);

/**
 * \brief 设备上线。
 * \param[in] client : 客户端对象。
 * \param[in] params : 参数列表。可以包含设备本身的一些信息，如厂家和版本号。
 *
 * \retval 成功返回TRUE，失败返回FALSE。
 */
BOOL client_online(client_t* client, params_t* params);

/**
 * \brief 设备下线。
 * \param[in] client : 客户端对象。
 *
 * \retval 成功返回TRUE，失败返回FALSE。
 */
BOOL client_offline(client_t* client);

/**
 * \brief 上报命令执行的结果。
 * 注意：命令参数中带有cmdid时，才要求设备执行命令后，上报执行的结果。
 * \param[in] client : 客户端对象。
 * \param[in] cmdid : 命令ID。
 * \param[in] result : 执行结果。
 * \param[in] msg : 执行结果的详情(长度< 32)。
 *
 * \retval 注册过返回TRUE，否则返回FALSE。
 */
BOOL client_reply_cmd(client_t* client, int cmdid, BOOL result, const char* msg);

/**
 *  \brief 网关添加子设备
 *  \param[in] client : 客户端对象。
 *  \param[in] subdev_type : 子设备类型。
 *  \param[in] subdev_id : 子设备id。
 *  \param[in] params : 参数列表。
 * 
 *  \retval 成功返回TRUE，失败返回FALSE。
 */
BOOL client_bind_subdev(client_t* client, const char* subdev_type, const char* subdev_id, params_t* params);

/**
 *  \brief 网关删除子设备
 *  \param[in] client : 客户端对象。
 *  \param[in] subdev_type : 子设备类型。
 *  \param[in] subdev_id : 子设备id。
 *  \param[in] params : 参数列表。
 * 
 *  \retval 成功返回TRUE，失败返回FALSE。
 */
BOOL client_unbind_subdev(client_t* client, const char* subdev_type, const char* subdev_id);

/**
 *  \brief 网关转发子设备消息
 *  \param[in] client : 客户端对象。
 *  \param[in] topic : 要转发到哪个主题。
 *  \param[in] params : 参数列表。
 * 
 *  \retval 成功返回TRUE，失败返回FALSE。
 */
BOOL client_forward_message(client_t* client, const char* forward_topic, params_t* params);

END_C_DECLS

#endif/*DEVICE_CLIENT_H*/

