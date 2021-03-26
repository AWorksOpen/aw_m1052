set(LIB_SRC 
    src/zlg_iot_mqtt_client.c
    src/zlg_iot_mqtt_client_subscribe.c
    src/zlg_iot_mqtt_client_common_internal.c
    src/zlg_iot_mqtt_client_unsubscribe.c
    src/zlg_iot_mqtt_client_connect.c
    src/zlg_iot_mqtt_client_yield.c
    src/zlg_iot_mqtt_client_publish.c
    src/zlg_iot_http_client.c
    platform/linux/common/timer.c
    platform/linux/mbedtls/network_mbedtls_wrapper.c
    platform/linux/socket/network_socket_wrapper.c
)

ADD_DEFINITIONS(
    -DENABLE_IOT_INFO=true
    -DENABLE_IOT_ERROR=true
)

add_library(network ${LIB_SRC})

set(ECHO_CLIENT_SRC tools/echo_client.c)
add_executable(echoc ${ECHO_CLIENT_SRC})
target_link_libraries(echoc network ${SYS_LIBS}) 

set(MQTT_DEMO_SRC tools/mqtt_demo.c)
add_executable(mqtt_demo ${MQTT_DEMO_SRC})
target_link_libraries(mqtt_demo network ${SYS_LIBS}) 

set(HTTP_DOWNLOAD_SRC tools/http_download.c)
add_executable(http_download ${HTTP_DOWNLOAD_SRC})
target_link_libraries(http_download network ${SYS_LIBS}) 

set(HTTP_AUTH_SRC tools/http_auth.c)
add_executable(http_auth ${HTTP_AUTH_SRC})
target_link_libraries(http_auth network ${SYS_LIBS}) 
