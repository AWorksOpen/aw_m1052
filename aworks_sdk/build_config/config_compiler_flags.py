#!/usr/bin/python
# -*- coding: utf-8 -*-

# 本文件主要用于生产编译整个AWorks工程所需要的编译器标志参数

def make_compiler_flags(build_common_config,ARGUMENTS):
    BUILD_TYPE = ARGUMENTS.get('BUILD_TYPE')

    COMPILER_TOOLS = ARGUMENTS.get('COMPILER_TOOLS')

    # 判断为哪一个工程构建
    __compiler_tools =  ('gcc', 'armcc')
    if COMPILER_TOOLS not in __compiler_tools:
        print ('Error compiler tools, we use default ' + __compiler_tools[0]);
        COMPILER_TOOLS = __compiler_tools[0];

    # 配置CPU属性
    build_common_config.AW_CPU_MAIN_NAME = 'arm'
    # 配置CPU生产型号名称
    build_common_config.AW_CPU_VENDOR_NAME = 'imx1050'
    # 设置CPU核心型号
    build_common_config.AW_CPU_CORE_NAME = 'cortex-m7'
    # 设置arm处理器thumb模式
    build_common_config.AW_CPU_ARM_THUMB_MODE = 'thumb'
    build_common_config.ARM_FLOAT_ABI = 'hard'
    build_common_config.ARM_FPU_TYPE = 'fpv5-d16'
    ##########################################################################
 
    # 为eclipse工程构建    
    if COMPILER_TOOLS == 'gcc' :

        # 首先检查构建类型是否合法
        __permit_build_type=('Debug','Release','MinSizeRel','RelWithDebInfo')
        __build_type_debug_opt_flags = ('-O0','-O3','-Os','-O2')
        if BUILD_TYPE not in __permit_build_type:
            print ('Error build type,we use default ' + __permit_build_type[0]);
            BUILD_TYPE = __permit_build_type[0];

        # 配置编译器
        build_common_config.AW_COMPILER_TOOLS='arm-none-eabi-gcc'
        # 设置编译时需要全局定义的宏

        build_common_config.AW_CPPDEFINES = ['AW_DEBUG','AW_VDEBUG','AW_VDEBUG_INFO','AW_VDEBUG_WARN','AW_VDEBUG_ERROR','AW_CORTEX_M7','HAS_AWTK_CONFIG']
        build_common_config.AW_BUILD_TYPE = BUILD_TYPE


        # 推导CPU标志
        CPU_FLAGS =  ' '
        CPU_FLAGS += ' -mcpu='+build_common_config.AW_CPU_CORE_NAME;
        CPU_FLAGS += ' -m' + build_common_config.AW_CPU_ARM_THUMB_MODE;
        CPU_FLAGS += ' -mthumb-interwork ';
        CPU_FLAGS += ' -mfloat-abi=' + build_common_config.ARM_FLOAT_ABI;
        if build_common_config.ARM_FLOAT_ABI != 'soft':
            CPU_FLAGS += ' -mfpu=' + build_common_config.ARM_FPU_TYPE;
        CPU_FLAGS += ' -mno-unaligned-access';

        # 推导警告，优化，调试，其它标志
        WARN_FLAGS='-Wall  -Wno-unused-variable -Wno-unused-function '
        OPT_FLAGS = __build_type_debug_opt_flags[__permit_build_type.index(BUILD_TYPE)];
        OPT_FLAGS += ' -ffunction-sections -fdata-sections ';
        DEBUG_FLAGS = ' -g3 -gdwarf-2 '
        OTHER_FLAGS = ' -fmessage-length=0 -MMD -MP '
        # 生成C,C++,汇编的编译标志参数
        TEMP = ' '
        build_common_config.AW_CFLAGS = TEMP.join((CPU_FLAGS,DEBUG_FLAGS,OPT_FLAGS,OTHER_FLAGS))
        build_common_config.AW_ASFLAGS= build_common_config.AW_CFLAGS + ' $_CPPDEFFLAGS '
        build_common_config.AW_CXXFLAGS = build_common_config.AW_CFLAGS + ' -std=c++11 '

        # 添加链接参数
        TEMP = CPU_FLAGS + DEBUG_FLAGS + OPT_FLAGS + WARN_FLAGS;
        TEMP += ' -Xlinker --gc-sections -Wl,-Map,"${TARGET}.map"  -nostartfiles -T "${LINK_SCRIPT}" '
        build_common_config.AW_LINKFLAGS = TEMP

        ##################################################################################################################

    # 为keil工程构建
    else :
        # 首先检查构建类型是否合法
        __permit_build_type=('Debug','Release','MinSizeRel','RelWithDebInfo')
        __build_type_debug_opt_flags = ('-O0','-O3 -Otime','-O3 -Ospace','-O2')
        if BUILD_TYPE not in __permit_build_type:
            print ('Error build type,we use default ' + __permit_build_type[0]);
            BUILD_TYPE = __permit_build_type[0];

        # 配置编译器
        build_common_config.AW_COMPILER_TOOLS='armcc'

        # 设置编译时需要全局定义的宏
        build_common_config.AW_CPPDEFINES = ['AW_DEBUG','AW_VDEBUG','AW_VDEBUG_INFO','AW_VDEBUG_WARN','AW_VDEBUG_ERROR','AW_CORTEX_M7','HAS_AWTK_CONFIG']
        build_common_config.AW_BUILD_TYPE = BUILD_TYPE

        # 如果是armcc编译器，还需要定义一些指定的宏修改
       #build_common_config.AW_CPPDEFINES.append('__GNUC__')
        build_common_config.AW_CPPDEFINES.append('__EVAL')
        
        #build_common_config.AW_CPPDEFINES.append('CONFIG_CLK_ENABLE_SAVE_CONTEXT')
        #build_common_config.AW_CPPDEFINES.append('_BLKSIZE_T_DECLARED')
        #build_common_config.AW_CPPDEFINES.append('_BLKCNT_T_DECLARED')

        # 推导CPU标志
        CPU_FLAGS =  ' '
        if build_common_config.AW_CPU_CORE_NAME == 'cortex-m7':
            if build_common_config.ARM_FLOAT_ABI != 'soft':
                CPU_FLAGS += ' --cpu=' + 'Cortex-M7 ';                           #armcc通过CPU名字生成指定符合指令集的代码
                if build_common_config.ARM_FPU_TYPE == 'fpv5-d16' :
                    CPU_FLAGS += ' --fpu=' + 'fpv5_d16 ';                       #设置处理器的FUP TYPE
            else:
                CPU_FLAGS += ' --cpu=' + 'Cortex-M7 '
                CPU_FLAGS += ' --fpu=' + 'softvfp ';                            #设置处理器的FUP TYPE
        if build_common_config.AW_CPU_ARM_THUMB_MODE == 'thumb':
            CPU_FLAGS += ' --thumb ';                                               #设置处理器thumb模式
        CPU_FLAGS += ' --apcs=interwork ';                                      #设定处理器允许CPU ARM指令及THUMB指令一起使用
        CPU_FLAGS += ' --unaligned_access';                                     #设定处理器可以生成地址非对齐的数据

        # 推导警告，优化，调试，其它标志
        WARN_FLAGS=' '                                                                    #armcc默认开启所有警告
        OPT_FLAGS = __build_type_debug_opt_flags[__permit_build_type.index(BUILD_TYPE)];
        OPT_FLAGS += ' --split_sections ';                                                #生成的函数在自己的段中
        DEBUG_FLAGS = ' -g --debug '
        OTHER_FLAGS =  ' --omf_browse "${TARGET}.crf" --depend "${TARGET}.d" -J"A:\"'
        # 生成C,C++,汇编的编译标志参数
        TEMP = ' '
        build_common_config.AW_CFLAGS = TEMP.join((' --gnu ', CPU_FLAGS, DEBUG_FLAGS, OPT_FLAGS, OTHER_FLAGS)) + ' --c99'
        build_common_config.AW_ASFLAGS = CPU_FLAGS + DEBUG_FLAGS + ' --list "${TARGET}.lst" ' + ' --cpreproc  '
        build_common_config.AW_CXXFLAGS = TEMP.join((' --gnu ', CPU_FLAGS, DEBUG_FLAGS, OPT_FLAGS, OTHER_FLAGS)) + ' --no_exceptions' + ' --no_rtti' + ' --cpp'
        ##################################################################################################################








