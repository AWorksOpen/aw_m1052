# -*- coding:utf-8 -*-
# Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd..
# All rights reserved.

class ComponentX(object):

    def get_src_filter(self, env):
        filters = super(ComponentX, self).get_src_filter(env)
        toolchain_family = env['AXIO_TOOLCHAIN_FAMILY']
        
        filters += ["+<../extra_src/*.c>", "+<../extra_src/net_sts/net_sts.c>"]
        
        if env.get('AWBL_GPRS_SERVICE'):
            filters += ["+<../extra_src/net_sts/4g_sts.c>"]

        if env.get('AWBL_WIFI_SERVICE'):
            filters += ["+<../extra_src/net_sts/wifi_sts.c>"]

        if env.get('AWBL_ENET_SERVICE'):
            filters += ["+<../extra_src/net_sts/eth_sts.c>"]
        print filters
        return filters