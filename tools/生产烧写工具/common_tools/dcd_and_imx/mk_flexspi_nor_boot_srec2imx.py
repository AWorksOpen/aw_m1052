#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import time
from array import array
import subprocess

#获取当前路径
if getattr(sys, 'frozen', False):
    CUR_PY_PATH = os.path.dirname(sys.executable)
else :
    CUR_PY_PATH = os.path.split(os.path.realpath(__file__))[0]

def print_auto_encoding(s):
    encode = sys.stdout.encoding
    if None == encode:
        encode = 'utf-8'

    print(s.encode(encode))
    sys.stdout.flush()

def error_exit():
    time.sleep(3)
    sys.exit(-1)

class SrecL :
    ''' analysis the line in srecfile '''
    def __init__(self, srecline, addr_len):
        self.chnum = int(srecline[-4:-2], base = 16)
        self.length = int(srecline[2:4], base = 16)
        
        self._aeidx = addr_len + 4
        self._deidx = 2*self.length + 2
        self._rowline = str(srecline[:-2])

        self.addr = int(srecline[4:self._aeidx], base = 16)
        self.data = [int(srecline[i:i+2], base = 16) for i in range(self._aeidx, self._deidx, 2)]

    def check_check(self):
        checknum = 0
        line = [str(hex(self.addr))[i:i+2] for i in range(2, self._aeidx - 2, 2)]

        for sum in (self.data + [int(l, base = 16) for l in line]):
            checknum += sum 

        return 0xFF - ((checknum + self.length) & 0XFF)

class SrecX:
    SrecDict = {'S1': 4, 'S2': 6, 'S3': 8, 'S7': 8, 'S8': 6, 'S9': 4}

    def __init__(self, srecfile):
        with open(srecfile, 'rb') as fp :
            self._srec = [SrecL(line, self.SrecDict[line[:2]]) 
                                for line in fp.readlines() if not line.startswith('S0')]
            self.start_addr = self._this_addr = self._srec[0].addr
            self._this_size = 0

    def mk_binary(self, filename):
        self._barray = array('B')
        
        for srecl in self._srec :
            #检测是否有空行，如果有则用0补充
            checksize = srecl.addr - self._this_addr
            if checksize != self._this_size:
                self._barray.extend(array('B', (0 for i in range(checksize - self._this_size))))

            if srecl.chnum != srecl.check_check():
                raise ValueError('!!srec file check failed at {}!!'.format(srecl))

            self._this_size = len(srecl.data)
            self._this_addr = srecl.addr

            #校验成功，转换为二进制行
            self._barray.extend(array('B', (data for data in srecl.data)))

        with open(filename.replace('.', '_nopadding.'), 'wb') as bp :
            self._barray.tofile(bp)


if __name__ == '__main__' :
    if len(sys.argv) == 3 :
        srec_filename, bin_filename  = sys.argv[1:]
    else :
        raise ValueError('!!param error!!\r\nrefer to: srec2bin.exe srec_filename bin_filename')

    #解析srec文件
    sfile = SrecX(srec_filename)

    #Norflash启动
    if 0x80000000 > sfile.start_addr >= 0x60000000:
        print_auto_encoding(u'firmware init load address is {}'.format(hex(sfile.start_addr)))
        try:
            print_auto_encoding(u'make Norflash bootable image...')
            sfile.mk_binary(bin_filename)

        except ValueError, error: 
            print_auto_encoding(str(error).decode('utf8'))
            error_exit()

        except KeyError, error:
            print_auto_encoding(u'!!srecline with wrong type {}!!'.format(error))
            error_exit()
        else:
            print_auto_encoding(u'IMX bootable image generated successfully') 

    #sd卡启动
    else:
        dcd_file_path = os.path.join(CUR_PY_PATH, '../../common_tools','dcd','m105x.dcd')
        mk_sd_boot_exe = os.path.join(CUR_PY_PATH, '../../common_tools','dcd_and_imx','mk_sd_boot_imx.exe')

        args = [mk_sd_boot_exe, srec_filename, dcd_file_path, bin_filename]
        ps = subprocess.Popen(args)
        ps.wait()
        if 0 != ps.returncode:
            sys.exit(-1)

