# -*- coding:utf-8 -*-
# Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd..
# All rights reserved.

from axio.ext.framework.aworks import AWorksComponent as ComponentBase

class ComponentAwPspRtk(ComponentBase):

    def prepare_config(self, env):

        ComponentBase.prepare_config(self, env)

        env.AppendUnique(AXIO_AWORKS_PSP_DIRS=[self.get_dir()],
                         AXIO_AW_TASK_PROVIDER_DIRS=[self.get_dir()])




