#include <string>
#include "client.h"
#include "gtest/gtest.h"

using std::string;

/////////////////////////////////////////

#define CMD_NAME_START   "start"
#define CMD_NAME_STOP    "stop"
#define CMD_NAME_UPGRADE "upgrade"

static string g_cmd;
static string gnameargs;

static string buff_to_strings(const char* b, size_t size) {
    string str;
    char* buff = (char*)calloc(1, size+1);
    if(*b == 0x00) {
        b++;
        size--;
    }

    memcpy(buff, b, size);
    for(size_t i = 0; i < size; i++) {
        if(buff[i] == '\0') {
           buff[i] = ':';
        }
    }
    str = buff;
    free(buff);

    return str;
}

static string params_to_strings(params_t* params) {
    return buff_to_strings(params->buff, params->size);
}

static BOOL start_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    (void)cmd;
    (void)client;
    g_cmd = CMD_NAME_START;
    gnameargs = params_to_strings(params);

    return TRUE;
}

static BOOL stop_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    (void)cmd;
    (void)client;

    g_cmd = CMD_NAME_STOP;
    gnameargs = params_to_strings(params);

    return TRUE;
}

static BOOL upgrade_exec(command_t* cmd, struct _client_t* client, params_t* params) {
    (void)cmd;
    (void)client;

    g_cmd = CMD_NAME_UPGRADE;
    gnameargs = params_to_strings(params);

    return TRUE;
}

static command_t s_cmds[] = {
    {CMD_NAME_START,   start_exec},
    {CMD_NAME_STOP,    stop_exec},
    {CMD_NAME_UPGRADE, upgrade_exec},
    {NULL,    NULL}
};

//////////////////////////////////

string s_publish_args;
static BOOL dummy_mqtt_client_publish(mqtt_client_t* mqtt, int qos, int retain, 
        const char* topic, const char* buff, size_t size) {
    (void)mqtt;
    (void)qos;
    (void) retain;

    s_publish_args = string(topic) + ":" + buff_to_strings(buff, size) ;

	return TRUE; 
}

static mqtt_client_t s_mqtt_client;

mqtt_client_t* mqtt_client_get_instance() {
    s_mqtt_client.publish = dummy_mqtt_client_publish;

    return &(s_mqtt_client);
}

/////////////////////////////////////////
static settings_t settings;

static BOOL dummy_settings_set(settings_t* settings, const char* key, const char* value) {
    (void)key;
    (void)value;
    (void)settings;
    return FALSE;
}

static const char* dummy_settings_get(settings_t* settings, const char* key) {
    (void)key;
    (void)settings;
    return NULL;
}

settings_t* settings_get_instance() {
    settings.get = dummy_settings_get;
    settings.set = dummy_settings_set;

    return &settings;
}
/////////////////////////////////////////

static void test_cmd(client_t* c, const char* cmd, const string& expected, const char* key, const char* value) {
    params_t params;
    char buff[128];

    request_init(&params, buff, sizeof(buff));
    params_append_int(&params, STR_KEY_MSG_TYPE, PACKET_COMMAND);
    params_append_string(&params, STR_KEY_NAME, cmd);
    if(key && value) {
        params_append_string(&params, key,value);
    }

    ASSERT_EQ(TRUE, client_dispatch(c, params.buff, params.size));
    ASSERT_EQ(g_cmd, cmd);
    ASSERT_EQ(gnameargs, expected);
}

/*static void test_response(client_t* c, const char* key, const char* value) {
    params_t params;
    char buff[128];

    request_init(&params, buff, sizeof(buff));
    params_append_int(&params, STR_KEY_MSG_TYPE, PACKET_RESPONSE);
    params_append_string(&params, STR_KEY_NAME, STR_TOPIC_ADD_SUBDEV);
    params_append_string(&params, key, value);

    ASSERT_EQ(TRUE, client_dispatch(c, params.buff, params.size));
}*/

static void dummy_client_init(client_t* c) {
    client_init(c, "owner", "type", "id");
    client_set_cmds(c, (command_t*)&s_cmds);
    client_set_settings(c, settings_get_instance());
    client_set_mqtt_client(c, mqtt_client_get_instance());
}

TEST(ClientTest, cmd) {
    client_t c;
    dummy_client_init(&c);

    test_cmd(&c, CMD_NAME_START, "mtype:2:name:start:", NULL, NULL); 
    test_cmd(&c, CMD_NAME_STOP, "mtype:2:name:stop:", NULL, NULL); 
    test_cmd(&c, CMD_NAME_UPGRADE, "mtype:2:name:upgrade:url:http://www.zlg.cn:",
            STR_KEY_URL, "http://www.zlg.cn"); 
}

/*TEST(ClientTest, response) {
    client_t c;
    dummy_client_init(&c);

    test_response(&c, STR_KEY_TIME, "12314");
}*/

TEST(ClientTest, report) {
    client_t c;
    char buff[128];
    params_t params;
    dummy_client_init(&c);

    request_init(&params, buff, sizeof(buff));

    ASSERT_EQ(TRUE, client_online(&c, &params));
    ASSERT_EQ(s_publish_args, "/d2s/owner/type/id/online:");

    ASSERT_EQ(TRUE, client_offline(&c));
    ASSERT_EQ(s_publish_args, "/d2s/owner/type/id/offline:");
    
    request_init(&params, buff, sizeof(buff));
    params_append_string(&params, "voltage", "1.0");
    params_append_string(&params, "voltage1", "2.0");
    params_append_string(&params, "voltage2", "3.0");

    ASSERT_EQ(TRUE, client_report_data(&c, &params));
    ASSERT_EQ(s_publish_args, "/d2s/owner/type/id/data:voltage:1.0:voltage1:2.0:voltage2:3.0:");
    
    request_init(&params, buff, sizeof(buff));
    params_append_string(&params, "version", "1.0");
    ASSERT_EQ(TRUE, client_report_status(&c, &params));
    ASSERT_EQ(s_publish_args, "/d2s/owner/type/id/status:version:1.0:");

    request_init(&params, buff, sizeof(buff));
    params_append_string(&params, STR_KEY_NAME, "v1low");
    ASSERT_EQ(TRUE, client_report_warning(&c, &params)); 
    ASSERT_EQ(s_publish_args, "/d2s/owner/type/id/warning:name:v1low:");
    
    request_init(&params, buff, sizeof(buff));
    params_append_string(&params, STR_KEY_NAME, "v1high");
    ASSERT_EQ(TRUE, client_report_error(&c, &params));
    ASSERT_EQ(s_publish_args, "/d2s/owner/type/id/error:name:v1high:");
   
    ASSERT_EQ(TRUE, client_reply_cmd(&c, 1234, TRUE, "success"));
    ASSERT_EQ(s_publish_args, "/d2s/owner/type/id/result:cmdid:1234:result:1:msg:success:");

    request_init(&params, buff, sizeof(buff));
    params_append_string(&params, "clientip", "::ffff:192.168.13.2");
    ASSERT_EQ(TRUE, client_bind_subdev(&c, "type2", "id2", &params));
    ASSERT_EQ(s_publish_args, "/g2s/owner/type/id/bind:clientip:::ffff:192.168.13.2:sub_devid:id2:sub_devtype:type2:");

    ASSERT_EQ(TRUE, client_unbind_subdev(&c, "type2", "id2"));
    ASSERT_EQ(s_publish_args, "/g2s/owner/type/id/bind:sub_devid:id2:sub_devtype:type2:");

    request_init(&params, buff, sizeof(buff));
    params_append_string(&params, STR_KEY_NAME, "v1high");
    ASSERT_EQ(TRUE, client_forward_message(&c, "/d2s/owner/type2/id2/error", &params));
    ASSERT_EQ(s_publish_args, "/g2s/owner/type/id/d2s/owner/type2/id2/error:name:v1high:");
}

