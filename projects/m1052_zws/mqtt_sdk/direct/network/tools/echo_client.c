/** 
 * 功能说明：
 *     1.用户演示socket实现的Network.
 *
 * 修改历史：
 *     1.2017-11-24 李先静<lixianjing@zlg.cn> 创建。
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <network_interface.h>

int main(int argc, char* argv[]) {
    int port = 0;
    char* host = NULL;
    size_t ret = 0;
    Timer  t = {0};
    unsigned char buff[1024];
    const char* data = "hello\n";
    Network* net = calloc(1, sizeof(Network));

    if(argc < 3) {
        printf("Usage: host port\n");
        return 0;
    }

    host = argv[1];
    port = atoi(argv[2]);

    init_timer_delta(&t, 5, 0);

    iot_socket_init(net, host, port, NULL, 1000);
    net->connect(net, NULL);
    net->write(net, (unsigned char*)data, strlen(data), &t, &ret);
    printf("wrirte ret=%zu\n", ret);
    net->read(net, buff, sizeof(buff), &t, &ret);
    printf("read ret=%zu %s\n", ret, buff);
    net->disconnect(net);
    free(net);

    printf("quit\n");
    return 0;
}

