/** 
 * 功能说明：
 *     1.用户演示固件下载。
 *
 * 修改历史：
 *     1.2017-11-24 李先静<lixianjing@zlg.cn> 创建。
 */

#include "zlg_iot_http_client.h"

static void on_header(zlg_iot_http_client_t* c) {
    printf("status=%d content_length=%zu content_type=%s\n", c->status, c->content_length, c->content_type);
}

static void on_body(zlg_iot_http_client_t* c) {
    FILE* fp = (FILE*)c->ctx;
    fwrite(c->body, c->size, 1, fp);
    printf("offset=%zu size=%zu content_length=%zu", c->offset, c->size, c->content_length);
}

int main(int argc, char* argv[]) {
    FILE* fp = NULL;
    zlg_iot_http_client_t c;
    const char* filename = NULL;

    if(argc < 5) {
        printf("Usage: host port urlpath filename\n");
        return 0;
    }

    memset(&c, 0x00, sizeof(c));
    c.hostname = argv[1];
    c.port = atoi(argv[2]);
    c.path = argv[3];
    c.on_header = on_header;
    c.on_body = on_body;

    filename = argv[4];
    fp = fopen(filename, "wb+");
    
    c.ctx = fp;
    zlg_iot_https_get(&c);

    fclose(fp);

    return 0;
}
