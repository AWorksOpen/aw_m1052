# -*- coding:utf-8 -*-
# Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd..
# All rights reserved.

import os

from axio import util, exception


class ComponentX(object):

    def prepare_env(self, env=None):
        super(ComponentX, self).prepare_env(env)

    def get_src_filter(self, env):
        sources = []

        if env.get('CONFIG_AW_PSP_WIN32') == 'y':
            sources.append('tick_lib.c')

        filters = ['+<*>']
        filters += map(lambda x: '-<core/{}>'.format(x), sources)
        return filters + self.get_buildset(env).get('src_filter', [])

    def get_buildset(self, env):

        data = self.parse_buildset(env, {})

        arch_dir = os.path.join(self.get_dir(), 'source/arch', '$AW_MAIN_NAME', '$AW_CORE_NAME')
        
        if env.subst('$AW_CORE_NAME') == 'cortex-a7':
            core_name = 'armv7'
        else: 
            core_name = '$AW_CORE_NAME'
        
        if os.path.isdir(env.subst(arch_dir)):
            cpu_core_path = os.path.join('$AW_MAIN_NAME', core_name)
            cpu_core_toolchain_path = os.path.join('$AW_MAIN_NAME', core_name, '$AW_TOOLCHAIN_NAME')

            data['src_filter'] += ["-<arch/*>", 
                                   env.subst("+<arch/" + cpu_core_path + "/*.c>"), 
                                   env.subst("+<arch/" + cpu_core_path + "/*.S>"), 
                                   env.subst("+<arch/" + cpu_core_path + "/*.s>"), 
                                   env.subst("+<arch/" + cpu_core_toolchain_path + "/*.c>"),
                                   env.subst("+<arch/" + cpu_core_toolchain_path + "/*.S>"),
                                   env.subst("+<arch/" + cpu_core_toolchain_path + "/*.s>")]                    
        else:
            raise exception.BuildError('RTK not support arch {}-{}.'.format(env['AW_MAIN_NAME'], env['AW_CORE_NAME']))

        self.append_buildset(env, data)
        return super(ComponentX, self).get_buildset(env)

