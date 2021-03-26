#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import time
import subprocess
import shutil
import re
import struct


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


class DCD_CFG_PARSE:
    def __init__(self,dcd_type,dcd_sub_type):
        self._dcd_type = dcd_type;
        self._dcd_sub_type = dcd_sub_type;

def parse_dcd_cfg_line_write(dcd_cfg_line):
    if len(dcd_cfg_line) != 4:
        error_exit(u'无效的write行:' + str(dcd_cfg_line));
    s = dcd_cfg_line[1];
    if s.startswith('0x') or s.startswith('0X'):
        write_addr = int(s,16);
    else :
        write_addr = int(s,10);
    s = dcd_cfg_line[2];
    if s.startswith('0x') or s.startswith('0X'):
        write_val_msk = int(s,16);
    else :
        write_val_msk = int(s,10);
    s = dcd_cfg_line[3];
    valid_write_types = ('write_value','clear_bitmask','set_bitmask');
    if s not in valid_write_types:
        error_exit(u'无效的write行:' + str(dcd_cfg_line));

    cfg_parse = DCD_CFG_PARSE('write',s)
    cfg_parse.write_addr = write_addr;
    cfg_parse.write_val_msk = write_val_msk;
    return cfg_parse

def parse_dcd_cfg_line_nop(dcd_cfg_line):
    if len(dcd_cfg_line) != 1:
        error_exit(u'无效的nop行:' + dcd_cfg_line[0]);
    cfg_parse = DCD_CFG_PARSE('nop','nop');
    return cfg_parse

def parse_dcd_cfg_line_check(dcd_cfg_line):
    if len(dcd_cfg_line) != 4:
        error_exit(u'无效的check行:' + str(dcd_cfg_line));
    s = dcd_cfg_line[1];
    if s.startswith('0x') or s.startswith('0X'):
        check_addr = int(s,16);
    else :
        check_addr = int(s,10);
    s = dcd_cfg_line[2];
    if s.startswith('0x') or s.startswith('0X'):
        check_mask = int(s,16);
    else :
        check_mask = int(s,10);
    s = dcd_cfg_line[3];
    valid_check_types = ('all_bits_clear','all_bits_set','any_bit_clear','any_bit_set');
    if s not in valid_check_types:
        error_exit(u'无效的check行:' + str(dcd_cfg_line));

    cfg_parse = DCD_CFG_PARSE('check',s)
    cfg_parse.check_addr = check_addr;
    cfg_parse.check_mask = check_mask;
    return cfg_parse

def parse_dcd_cfg_line(dcd_cfg_line):
    data = []
    if dcd_cfg_line[0] == 'write':
        data = parse_dcd_cfg_line_write(dcd_cfg_line);
    elif dcd_cfg_line[0] == 'check':
        data = parse_dcd_cfg_line_check(dcd_cfg_line);
    elif dcd_cfg_line[0] == 'nop':
        data =  parse_dcd_cfg_line_nop(dcd_cfg_line);
    else :
        error_exit(u'无效的行:' + dcd_cfg_line[0]);
    return data;

def process_dcd_cfg_merge_write(dcd_cfg_merge):
    dcd_sub_type = dcd_cfg_merge._dcd_sub_type;
    valid_write_types = ('write_value','clear_bitmask','set_bitmask');
    write_count = len(dcd_cfg_merge.write_addr);
    count = 8 * write_count + 4;

    write_data = [0xCC,];
    write_data.append(count / 256);
    write_data.append(count % 256);
    data = 0x4;
    write_types_mask = (0x00,0x08,0x18);
    data |= write_types_mask[valid_write_types.index(dcd_sub_type)];
    write_data.append(data);

    j = 0;
    while j < write_count:
        #print j
        write_addr = dcd_cfg_merge.write_addr[j];
        # 填入地址
        i = 0;
        while i < 4:
            data = (write_addr >> (8 * (3-i) ) ) & 0xFF;
            write_data.append(data);
            i = i+1;
        write_val_msk = dcd_cfg_merge.write_val_msk[j];
        # 填入数据
        i = 0;
        while i < 4:
            data = (write_val_msk >> (8 * (3-i) ) ) & 0xFF;
            write_data.append(data);
            i = i+1;

        j = j + 1;
    return write_data

def process_dcd_cfg_merge_check(dcd_cfg_merge):
    dcd_sub_type = dcd_cfg_merge._dcd_sub_type;
    valid_check_types = ('all_bits_clear','all_bits_set','any_bit_clear','any_bit_set');

    write_data = [0xCF,0x00,12];
    data = 0x4;
    check_types_mask = (0x00,0x10,0x08,0x18);
    data |= check_types_mask[valid_check_types.index(dcd_sub_type)];
    write_data.append(data);

    write_addr = dcd_cfg_merge.check_addr;
    # 填入地址
    i = 0;
    while i < 4:
        data = (write_addr >> (8 * (3-i) ) ) & 0xFF;
        write_data.append(data);
        i = i+1;
    write_val_msk = dcd_cfg_merge.check_mask;
    # 填入数据
    i = 0;
    while i < 4:
        data = (write_val_msk >> (8 * (3-i) ) ) & 0xFF;
        write_data.append(data);
        i = i+1;
    return write_data

def process_dcd_cfg_merge_nop(dcd_cfg_merge):
    write_data = [0xc0,0x00,0x04,0x00]
    return write_data


def process_dcd_cfg_merge(dcd_cfg_merge):
    data = []
    if dcd_cfg_merge._dcd_type == 'write':
        data =  process_dcd_cfg_merge_write(dcd_cfg_merge);
    elif dcd_cfg_merge._dcd_type == 'nop':
        data =  process_dcd_cfg_merge_nop(dcd_cfg_merge);
    elif dcd_cfg_merge._dcd_type == 'check':
        data =  process_dcd_cfg_merge_check(dcd_cfg_merge);
    else :
        error_exit(u'无效的行:' + dcd_cfg_merge);
    return data;

if __name__ == '__main__':
    #获取当前路径
    if getattr(sys, 'frozen', False):
        CUR_PY_PATH = os.path.dirname(sys.executable)
    else :
        CUR_PY_PATH = os.path.split(os.path.realpath(__file__))[0];

    TOOLS_ROOT_DIR = os.path.join(CUR_PY_PATH,'..','Flashloader_RT1050_1.0','Tools');
    TOOLS_ROOT_DIR = os.path.realpath(TOOLS_ROOT_DIR);

    #print_auto_encoding('本工具用于由配置文件生成dcd文件');
    #检查参数
    from sys import argv
    argc_len = len(sys.argv)
    if argc_len == 3 :
        argv1, DCD_CFG_FILE_PATH, DCD_FILE_PATH = argv
    else :
        error_exit(u'param error!\r\nrefer to: mk_dcd_file.exe  dcd_cfg_filename  dcd_output_filename') #打印参数不正确

    OLD_CUR_DIR = os.getcwd();
    #os.chdir(CUR_PY_PATH);

    dcd_file = open(DCD_FILE_PATH,'wb');
    dcd_cfg_file = open(DCD_CFG_FILE_PATH,'rb');
    dcd_cfgs = dcd_cfg_file.readlines();
    dcd_cfg_file.close();
    all_dcd_data = [];
    all_dcd_parse = [];

    for dcd_cfg_line in dcd_cfgs:
        # 以#号开头的是注释
        if dcd_cfg_line[0] == '#' :
            continue;
        dcd_cfg_line = dcd_cfg_line.split();
        if len(dcd_cfg_line) == 0:
            continue;
        dcd_cfg_line_data = parse_dcd_cfg_line(dcd_cfg_line);
        all_dcd_parse.append(dcd_cfg_line_data);

    all_dcd_merge = [];
    for dcd_parse in all_dcd_parse:
        #print all_dcd_merge
        if 0 != len(all_dcd_merge):
            dcd_merge = all_dcd_merge[len(all_dcd_merge) - 1];
            if (dcd_merge._dcd_type == "write" and dcd_merge._dcd_sub_type == dcd_parse._dcd_sub_type ) :
                dcd_merge.write_addr.append(dcd_parse.write_addr);
                dcd_merge.write_val_msk.append(dcd_parse.write_val_msk);
                continue;
        if dcd_parse._dcd_type == 'write':
            dcd_merge = DCD_CFG_PARSE('write',dcd_parse._dcd_sub_type);
            dcd_merge.write_addr = [dcd_parse.write_addr,];
            dcd_merge.write_val_msk = [dcd_parse.write_val_msk,];
        else :
            dcd_merge = dcd_parse;
        all_dcd_merge.append(dcd_merge);

    for dcd_merge in all_dcd_merge:
        data = process_dcd_cfg_merge(dcd_merge);
        #print data
        all_dcd_data.extend(data);

    dcd_header = [0xd2,];


    all_dcd_len = len(all_dcd_data) + 4;
    dcd_header.append(all_dcd_len/256);
    dcd_header.append(all_dcd_len%256);
    dcd_header.append(0x41);
    dcd_header.extend(all_dcd_data);
    all_dcd_data = dcd_header;

    for temp in all_dcd_data:
        temp = struct.pack('B',temp);
        dcd_file.write(temp);

    dcd_file.close();
    os.chdir(OLD_CUR_DIR);