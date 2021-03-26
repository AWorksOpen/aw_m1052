#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import subprocess
from subprocess import Popen,PIPE
import os
import re
import glob
import shutil
import tempfile
import time
import importlib
import fnmatch
import pdb
import inspect

# 获取当前路径
scons_common_dir = os.path.join(u'.',u'apollo',u'scons_common');


if sys.version_info > (3,6):
    pass;
elif sys.version_info > (2,7) and sys.version_info < (3,0):
    pass;
else :
    print('unsupported python version');
    sys.out.flush();
    sys.exit(-1);
    pass;

def include_file(filename):
    file = open(filename,u'rb');
    str_lines = file.read();
    file.close();
    x = 1
    _locals = {};
    file_byte_code = compile(str_lines,filename,'exec');

    exec(file_byte_code,globals(),_locals);

    
    _globals = globals();
    while len(_locals) > 0:
        temp_item = _locals.popitem();
        if inspect.isfunction(temp_item[1] ) or inspect.ismodule(temp_item[1] ):
            _globals[temp_item[0] ] = temp_item[1];
    pass;

# 导入python3和python2兼容支持
include_file(os.path.join(scons_common_dir,u'python2_python3_port.py') )




# 导入通用工具包
include_file(os.path.join(scons_common_dir,u'build_aworks_common_tools.py') );
# 导入arm-non-eabi-gcc工具包
include_file(os.path.join(scons_common_dir,u'arm-none-eabi-gcc_tools.py') );
include_file(os.path.join(scons_common_dir,u'armcc_tools.py') );
