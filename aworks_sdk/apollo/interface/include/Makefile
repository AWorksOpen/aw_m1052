# -*- coding: gb2312; -*- 
## 
## 文   件   名: makefiletemplate.mk
##
## 创   建   人: liangyaozhan
##
## 版        本: 1.0
##
## 更  新 日  期: Sun Mar 25 00:30:37 HKT 2012
##
## 描        述: 
##              
#########################################################################################################


#auto search base directory
# shell cmd:
# d=`pwd`; while [ ! -d $d/makefiles ]; do d=`dirname $d` ; done ; echo $d
ifeq ($(PROJECT_BASE),)
PROJECT_BASE := $(shell d=`pwd`; while [ ! -d $$d/makefiles ]; do d=`dirname $$d` ; done ; echo $$d)
export PROJECT_BASE
endif
ifeq ($(PROJECT_IMG_BASE),)
PROJECT_IMG_BASE := $(shell d=`pwd`; while [ ! -f $$d/img.mk ] && [ ! -d $$d/makefiles ]  ; do d=`dirname $$d` ; done ; if [ -f $$d/img.mk ]; then echo "$$d"; else echo ""; fi)
export PROJECT_IMG_BASE
endif


#in case we are on a WIN32 host ...
PROJECT_BASE := $(subst \,/,$(PROJECT_BASE))

# 确保第一个目标是all
all:pre_building
# 消除默认的规则
Makefile:;

# 在定义目标文件之前需要包含一些定义，如：CPU
include $(PROJECT_BASE)/makefiles/config.mk

#编译到$(MODULE_NAME)的目标文件
COBJS   := $(patsubst %.c,%.o,$(wildcard *.c)) 
CPPOBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp)) 
SOBJS   := $(patsubst %.S,%.o,$(wildcard *.S))

# 本目录编译后打到的库名字，在安装Makefile时自动生成。
#<<tree.cfg

#指定其它编译参数,如包含目录，注意使用 "+=" 
CFLAGS +=

# 若编译当前目录时需要额外的include路径，则可把目录写上。直接写目录即可，不需在目录前加"-I".
#INC_PATH += /to/your/local/include/directory
INC_PATH += 


#在make clean时需要额外删除的文件.列在OBJS上的目标文件不需要再写在这里。
# 如yylex.c yylex.h
#EX_CLEAN_FILE += yylex.c yylex.h
EX_CLEAN_FILE += 

# 可指定子目录.若不定义此变量(需要把它注释)，则所有子目录都编译.
# 注意：需要在"sinclude $(PROJECT_BASE)/makefiles/rules.mk"之前定义此变量
#ALL_SUB_PATH = subdir1 subdir2 subdir3
# 当子目录的编译顺序重要时，可以在文件的后面(在"sinclude $(PROJECT_BASE)/makefiles/rules.mk"之后)
# 指定子目录的依赖关系，进而确定他们之间的编译顺序。
# 如： 当目录subdir1,subdir2,subdir3的编译顺序需求： subdir1 ==> subdir2 ==> subdir3
#     则在此文件加上依赖关系：
#         subdir3:subdir2
#         subdir2:subdir1
# 可以写在当前目录的dircfg.mk
# 

EXCLUDE_SUB_PATH :=

# 如果可能，则包含当前目录下的配置文件
-include ./dircfg.mk

#
# 包括模块(库)的生成规则
# 
sinclude $(PROJECT_BASE)/makefiles/rules.mk

# 在编译OBJS和子目录前，可以使用此目标完成一些工作，可以写在某个目录的dircfg.mk中
pre_building:


# 在编译OBJS和子目录后，可以使用此目标完成一些工作，可以写在某个目录的dircfg.mk中
post_building:

