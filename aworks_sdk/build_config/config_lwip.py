#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
from aw_build_config_base import aw_config_const
from config_build_common import build_common_config

aw_lwip_config = aw_config_const()

def get_config_object():
    global aw_lwip_config
    return aw_lwip_config

def make_aw_lwip_config():
    # 编译LWIP相关的库需要的头文件路径
    LWIP_INCLUDE = [build_common_config.AWORKS_ROOT + '/3rdparty/net/lwip_1.4.x/src/include']
    LWIP_INCLUDE.append(build_common_config.AWORKS_ROOT + '/3rdparty/net/lwip_1.4.x/src/include/ipv4')
    LWIP_INCLUDE.append(build_common_config.AWORKS_ROOT + '/3rdparty/net/lwip_1.4.x/src/include/netif')
    LWIP_INCLUDE.append(build_common_config.AWORKS_ROOT + '/3rdparty/net/lwip_1.4.x/src/netif/ppp')
    LWIP_INCLUDE.append(build_common_config.AWORKS_ROOT + '/components/net/lwip_1.4.x_adapter/include')
    LWIP_INCLUDE.append(build_common_config.PROJECT_CONFIG_ROOT )
    aw_lwip_config.LWIP_INCLUDE = tuple(LWIP_INCLUDE)