#include "aworks.h"

extern void app_enet_init (void);
extern void webui_init (uint16_t port);

void app_enet_webserver_main(void){

    app_enet_init();

    webui_init(80);

    return ;
}
