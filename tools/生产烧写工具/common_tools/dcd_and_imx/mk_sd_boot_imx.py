#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import time
import subprocess
import shutil
import re

ELF_PROJECT_NAME = 'img_rt1050_debug'

def print_auto_encoding(s):
    encode = sys.stdout.encoding;
    if None == encode:
        encode = 'utf-8'
    if not isinstance (s,unicode) :
        print_auto_encoding(u'print_auto_encoding 参数不是unicode');
        raise ValueError;
    print (s.encode(encode) );
    sys.stdout.flush();

def error_exit(msg):
    print_auto_encoding(msg);
    time.sleep(3);
    sys.exit(-1);


def copy_imx_to_sdtool_dirs(SRC_FILE,DST_FILE):
    shutil.copyfile(SRC_FILE,DST_FILE);

def elf_to_imx(SREC_FILE_NAME,IMX_FILE_NAME,BD_FILE_NAME):
    ELF_TO_SB_EXE_FILE = os.path.join(IMX_TOOLS_ROOT_DIR,'elftosb','win','elftosb.exe');
    print_auto_encoding(u'执行"' + ELF_TO_SB_EXE_FILE.decode(sys.getfilesystemencoding() ) + u'"...');
    ARGS = [ELF_TO_SB_EXE_FILE,'-f','imx','-V','-c',BD_FILE_NAME,'-o',IMX_FILE_NAME ,SREC_FILE_NAME];
    ps = subprocess.Popen(ARGS);
    ps.wait()
    if 0 != ps.returncode:
        error_exit(u'生成"' + IMX_FILE_NAME.decode(sys.getfilesystemencoding())  + '"错误');

def int_to_hex_string(val,min_length = 0):
    if (val < 0) :
        error_exit(u"val < 0");
    
    sz = '';
    asc_hex_array = ('0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F');
    while(0 != val):
        t = val % 16;
        val = val / 16;
        sz = asc_hex_array[t] + sz;
    
    if 0 == len(sz):
        sz = '0';
    
    if 0 != min_length:
        while(min_length > len(sz) ):
            sz = '0' + sz;
    return sz

def get_srec_start_address(srec_file_name):
    cal_count = 0;
    srec_file = open(srec_file_name,'rb');
    srec_line = srec_file.readline();
    if srec_line.startswith('S0'):
        #error_exit(u'不支持的SREC文件头');
        srec_line = srec_file.readline();
    
    if srec_line.startswith('S3'):
        cal_count = 8;
    elif srec_line.startswith('S2'):
        cal_count = 6;
    elif srec_line.startswith('S1'):
        cal_count = 4;
    else:
        error_exit(u'不支持的SREC文件行');
    i = 4;
    cal_end = i + cal_count;
    data = 0;
    asc_hex_array = ('0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F');
    while(i<cal_end):
        c = srec_line[i];
        c = asc_hex_array.index(c);
        i = i + 1;
        data = data * 16;
        data = data | c;
    srec_file.close();
    return data

def mk_imx_bd_file(bd_file_path,srec_start_address,dcd_file_path = ''):
    ivtOffset = 0x400
    dcd_offset_of_ivt = 0x40;
    dcd_file_size = 0x0;
    
    if 0 != len(dcd_file_path):
        dcd_file_size = os.path.getsize(dcd_file_path);

    
    imx_bd_file = open(bd_file_path,'wb');

    imx_bd_file.write('options {' + '\r\n');
    imx_bd_file.write('    flags = 0x00;' + '\r\n');
    
    reserve_size = ivtOffset + dcd_offset_of_ivt + dcd_file_size;
    start_address = srec_start_address - reserve_size;
    if (start_address < 0):
        error_exit(u'计算的imx文件起始地址小于0，这是不允许的');
    # 向下1k地址对齐
    start_address = start_address - start_address % 1024;
    
    start_address = '0x' + int_to_hex_string(start_address);
    start_address = '    startAddress = ' + start_address + ';'
    imx_bd_file.write(start_address + '\r\n');
    
    imx_bd_file.write('    ivtOffset = 0x400;' + '\r\n');
    imx_bd_file.write('    initialLoadSize = 0x2000;' + '\r\n');
    
    if 0 != len(dcd_file_path):
        dcd_line = '    DCDFilePath = "'
        dcd_line += dcd_file_path;
        dcd_line += '";'
        imx_bd_file.write(dcd_line + '\r\n');
    
    
    imx_bd_file.write('}' + '\r\n');
    imx_bd_file.write('' + '\r\n');
    imx_bd_file.write('sources {' + '\r\n');
    imx_bd_file.write('    elfFile = extern(0);' + '\r\n');
    imx_bd_file.write('}' + '\r\n');
    imx_bd_file.write('' + '\r\n');
    imx_bd_file.write('section (0)' + '\r\n');
    imx_bd_file.write('{' + '\r\n');
    imx_bd_file.write('}' + '\r\n');
    imx_bd_file.write('' + '\r\n');

    imx_bd_file.close();
    

if __name__ == '__main__':
    #获取当前路径
    if getattr(sys, 'frozen', False):
        CUR_PY_PATH = os.path.dirname(sys.executable)
    else :
        CUR_PY_PATH = os.path.split(os.path.realpath(__file__))[0];

    IMX_TOOLS_ROOT_DIR = os.path.join(CUR_PY_PATH,'..','Flashloader_RT1050_1.0','Tools');
    IMX_TOOLS_ROOT_DIR = os.path.realpath(IMX_TOOLS_ROOT_DIR);
    SD_TOOLS_ROOT_DIR = os.path.join(CUR_PY_PATH,'..','sd_card_burn');
    SD_TOOLS_ROOT_DIR = os.path.realpath(SD_TOOLS_ROOT_DIR);

    print_auto_encoding(u'本工具主要用于生成SD卡启动文件');
    #检查参数
    from sys import argv
    argc_len = len(sys.argv)
    if argc_len == 4 :
        #SREC_FILE_NAME: srec文件
        #dcd_file_path : dcd文件
        #IMX_FILE_NAME : .imx输出文件名
        argv1, SREC_FILE_NAME, dcd_file_path, IMX_FILE_NAME = argv
    else :
        error_exit(u'param error!\r\nrefer to: mk_sd_boot_imx.exe srec_filename  dcd_filename imx_filename') #打印参数不正确

    OLD_CUR_DIR = os.getcwd();
    #os.chdir(CUR_PY_PATH);

    BD_FILE_NAME = os.path.join("elftoimx.bd");
    
    srec_start_address = get_srec_start_address(SREC_FILE_NAME);
    output_str = u'srec_start_address = 0x' + hex(srec_start_address);
    print_auto_encoding(output_str);
    mk_imx_bd_file(BD_FILE_NAME,srec_start_address,dcd_file_path);
    elf_to_imx(SREC_FILE_NAME,IMX_FILE_NAME,BD_FILE_NAME);

    #os.remove("elftoimx.bd");
    os.chdir(OLD_CUR_DIR);