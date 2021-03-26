#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import config_compiler_flags
from aw_build_config_base import aw_config_const

# 本文件模拟了一个build_common_config，主要用于在sdk中编译已经改变配置了的开源代码，例如lwip
build_common_config = aw_config_const()

def make_build_common_config(ARGUMENTS):
    global build_common_config;

    # 首先配置编译器参数
    config_compiler_flags.make_compiler_flags(build_common_config,ARGUMENTS);

    # 获取配置编译器参数中的某些分析结果
    CPU_CORE_NAME = build_common_config.AW_CPU_CORE_NAME;
    BUILD_TYPE = build_common_config.AW_BUILD_TYPE;

    #设置工程相关的变量,
    CUR_PY_PATH = os.path.split(os.path.realpath(__file__))[0] + os.path.sep
    PROJECT_ROOT = os.path.join(CUR_PY_PATH,'..')
    PROJECT_ROOT = os.path.abspath(PROJECT_ROOT) + os.path.sep

    AWORKS_BUILD_DIR_NAME=os.path.join('build',build_common_config.AW_BUILD_TYPE)

    SDK_ROOT = PROJECT_ROOT + os.sep
    PROJECT_BUILD_ROOT = PROJECT_ROOT + AWORKS_BUILD_DIR_NAME + os.path.sep + CPU_CORE_NAME + os.path.sep
    build_common_config.PROJECT_ROOT = PROJECT_ROOT
    build_common_config.PROJECT_BUILD_ROOT = PROJECT_BUILD_ROOT

    AW_SDK_LIB_INSTALL_PATH = os.path.join(SDK_ROOT,"lib",CPU_CORE_NAME,build_common_config.AW_BUILD_TYPE)
    build_common_config.AW_SDK_LIB_INSTALL_PATH = AW_SDK_LIB_INSTALL_PATH;

    build_common_config.PROJECT_CONFIG_ROOT = os.path.join(SDK_ROOT,'include','config')


    # 确定AWorks的通用头文件搜索路径
    build_common_config.AWORKS_DIR_NAME = 'apollo';
    AWORKS_ROOT = os.path.join(SDK_ROOT,build_common_config.AWORKS_DIR_NAME) + os.path.sep;
    build_common_config.AWORKS_ROOT = AWORKS_ROOT;

    AW_COMMON_INCLUDE = [SDK_ROOT + 'include']
    AW_COMMON_INCLUDE.append( AWORKS_ROOT+'interface/include')
    AW_COMMON_INCLUDE.append( AWORKS_ROOT+'interface/posix' )
    AW_COMMON_INCLUDE.append( AWORKS_ROOT+'psp/rtk/include' )
    AW_COMMON_INCLUDE.append( AWORKS_ROOT+'components/awbus_lite/include' )
    AW_COMMON_INCLUDE.append( AWORKS_ROOT+'rtk/include' )
    AW_COMMON_INCLUDE.append( AWORKS_ROOT+'cpu/nxp/lpc17xx/include' )
    build_common_config.AW_COMMON_INCLUDE = tuple(AW_COMMON_INCLUDE);


