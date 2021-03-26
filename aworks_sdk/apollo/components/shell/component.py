# -*- coding:utf-8 -*-
# Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd..
# All rights reserved.

from axio.ext.framework.aworks.compat import map

class ComponentX(object):
    
    def get_src_filter(self, env):
        sources = []

        if env.get('CONFIG_AW_SHELL_RTT'):
            sources.append('aw_shell_serial.c')
        elif env.get('CONFIG_AW_SHELL_SERIAL'):
            sources.append('aw_shell_segger_rtt.c')

        filters = ['+<*>']
        filters += map(lambda x: '-<{}>'.format(x), sources)

        return filters + self.get_buildset(env).get('src_filter', [])

    # def get_dependencies(self, env=None):
    #     deps = super(ComponentX, self).get_dependencies(env)

    #     if env.get('CONFIG_AW_SHELL_RTT') == 'y':
    #         deps = deps + ["segger_rtt"]

    #     return deps