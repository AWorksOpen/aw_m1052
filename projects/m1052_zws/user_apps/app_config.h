/*
 * app_config.h
 *
 *  Created on: 2019��8��10��
 *      Author: wengyedong
 */

#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

#include "aw_list.h"

/**
 * @\brief ������Ϣ��nvram�е�ƫ�Ƶ�ַ
 */
#define NVRAM_NETINFO_ADDR         0
#define NVRAM_DEVINFO_ADDR         512

/**
 * @\brief wifi����״̬
 */
#define WIFI_DISCONNECT         0
#define WIFI_CONNECTED          1

/**
 * @\brief zws_dev������Ϣ��־
 */
#define INVALID                 0
#define VALID                   1
#define CHANGED                 2
#define DONE                    3

/**
 * @ \brief �������豸������Ϣ
 */
typedef struct sensor_cfginfo {
    /* Input params */
    const char  *name;
    uint32_t     start_chn;
    uint32_t     chn_nums;
}sensor_cfginfo_t;

/**
 * @ \brief ������������Ϣ
 */
typedef struct sensor_datinfo {
    /* Output params */
    uint32_t     *type;
    uint32_t     *unit;
    double       *value;
}sensor_datinfo_t;

/**
 * @ \brief �������豸��Ϣ
 */
typedef struct sensor_devinfo {
    struct aw_list_head node;
    struct sensor_cfginfo *cfg;
    struct sensor_datinfo *data;
}sensor_devinfo_t;


/**
 * @ \brief ������Ϣ
 */
typedef struct net_info {
    char ssid[64];
    char pswd[64];
    uint32_t crc;
    const char *wifi_name;
    char connect_status;
}net_info_t;


/**
 * @ \brief zws���豸������Ϣ
 */
typedef struct zws_dev_info {
    char type[64 + 1];
    char id[256 + 1];
    char key[48];
    uint32_t crc;
}zws_dev_info_t;

/**
 * @ \brief ��Ӧ����Ϣ
 */
typedef struct app_sys_config {
    char                    cfg_sta;
    struct aw_list_head     sensor_head;
    struct net_info         *net;
    struct zws_dev_info     *zws_devinfo;
}app_sys_config_t;

/**
 * @ \brief appӦ����Ϣ
 */
typedef struct app {
    void                    *p_zws;
    struct app_sys_config   *p_cfg;
}app_t;


#endif /* __APP_CONFIG_H */
