import subprocess
import sys
import os
import shutil

GCC_PREFIX = r"arm-none-eabi-"
GCC_PATH = r"E:\eclipse\eclipse_neon_2016q3_x86\GNU Tools ARM Embedded\2016q3\bin;"
ARMCC_PATH = r"C:\Keil_v5\ARM\ARMCC\bin;"
SCONS_RUN = r"scons.bat"

#for linux
#GCC_PREFIX = r"arm-none-eabi-"
#GCC_PATH = r"/opt/arm-none-eabi/bin:"
#ARMCC_PATH = r""
#SCONS_RUN = r"scons"


AWTK_GCC_LIB_NAME = 'libawtk_imx1050.a'
AWTK_ARMCC_LIB_NAME = 'awtk_imx1050.lib'
OUTPUT = 'lib'
AWTK_ROOT = 'output\awtk'

def run_scons_for_gcc():
    scons_gcc_cmd = []
    scons_gcc_cmd.append(SCONS_RUN)
    scons_gcc_cmd.append('COMPILER_TOOLS=gcc')
    scons_gcc_cmd.append('GCC_PREFIX=' + GCC_PREFIX)
    scons_gcc_cmd.append('AWTK_LIB_NAME=' + AWTK_GCC_LIB_NAME)
    
    print(scons_gcc_cmd)
    p_gcc = subprocess.Popen(scons_gcc_cmd)
    p_gcc.wait()

    if p_gcc.returncode == 0:
        shutil.move(AWTK_GCC_LIB_NAME,OUTPUT)
        scons_gcc_cmd.append('-c')
        p_clean = subprocess.Popen(scons_gcc_cmd)
        p_clean.wait()
    else:
        sys.exit(1)

def run_scons_for_armcc():
    scons_armcc_cmd = []
    scons_armcc_cmd.append(SCONS_RUN)
    scons_armcc_cmd.append('COMPILER_TOOLS=armcc')
    scons_armcc_cmd.append('AWTK_LIB_NAME=' + AWTK_ARMCC_LIB_NAME)
    
    p_armcc = subprocess.Popen(scons_armcc_cmd)
    p_armcc.wait()

    if p_armcc.returncode == 0:
        shutil.move(AWTK_ARMCC_LIB_NAME,OUTPUT)
        scons_armcc_cmd.append('-c')
        p_clean = subprocess.Popen(scons_armcc_cmd)
        p_clean.wait()
    else:
        sys.exit(1)

def __copytree(path_src, path_des, __ignore=None):
    if os.path.exists(path_des):
        for ent in os.listdir(path_src):
            s = os.path.join(path_src, ent)
            d = os.path.join(path_des, ent)
            if os.path.isdir(s):
                shutil.copytree(s, d, ignore=__ignore)
            else:
                shutil.copy2(s, d)
    else:
        shutil.copytree(path_src, path_des, ignore=__ignore)
    

def config_path():
    os.environ['PATH'] =GCC_PATH + os.environ['PATH']
    os.environ['PATH'] = ARMCC_PATH + os.environ['PATH']

        
def build_lib(type):        
    
    if os.path.exists(OUTPUT):
        shutil.rmtree(OUTPUT)
    os.mkdir(OUTPUT)
    
    if type == "gcc":
        #arm-none-eabi-gcc
        run_scons_for_gcc()
    elif type == "armcc":
        #ARMCC
        run_scons_for_armcc()
    elif type == "all":
        #arm-none-eabi-gcc
        run_scons_for_gcc()
        #ARMCC
        run_scons_for_armcc()
    else:
        print("#################################################")
        print("your lib type error...")
        print("#################################################")
    shutil.rmtree("output")

def create_sdk():
    __copytree("../awtk-sdk","../temp/release/awtk-sdk")
    __copytree("../awtk-port","../temp/release/awtk-port")
    __copytree("../scons_lib","../temp/release/scons_lib",__ignore = shutil.ignore_patterns('*.pyc','.sconsign.dblite'))
    shutil.copy2("../CHANGELOG.rst","../temp/release")
    shutil.copy2("../LICENSE","../temp/release")
    shutil.copy2("../README.md","../temp/release")

def copyfiles():
    if os.path.exists("output"):
        shutil.rmtree("output")
    os.mkdir("output")
    os.system("python copy_files.py")


if __name__ == "__main__":   
    if len(sys.argv) == 2:
        copyfiles()
        config_path()
        build_lib(sys.argv[1])
    else:
        print("input error")