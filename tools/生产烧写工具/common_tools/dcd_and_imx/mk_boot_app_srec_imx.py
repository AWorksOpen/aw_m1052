#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import time
import struct
import argparse
from array import array

HW_VERSION = '1.00'
FW_VERSION = '1.00'

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
            self._this_addr = self._srec[0].addr
            self._this_size = 0
            
    #特定位置，替代数据
    def __bin_replace(self, addr, str):
        tmp = self._barray[:addr]
        tmp.extend(array('B', (ord(data) for data in str)))
        tmp.extend(self._barray[addr + len(str):])
        self._barray = tmp
        
    @property
    def start_addr(self):
        return hex(self._srec[0].addr)

    #srec转imx
    def mk_binary(self, app_link):
        self._barray = array('B')
        self._app_link = app_link
        
        for srecl in self._srec :
            #检测是否有空行，如果有则用0补充
            checksize = srecl.addr - self._this_addr
            if checksize < 0:
                continue
            if checksize > self._this_size:
                #非XIP模块固件，存储基址与boot相对应，由参数提供
                if ((self._this_addr >= 0x60002000) and 
                    (self._this_addr <  0x60800000) and
                    (srecl.addr      >  0x80000000)):
                    checksize = app_link - self._this_addr
                #最后一次有空行，合并的应用固件链接地址
                #self._app_link = srecl.addr
                self._barray.extend(array('B', (0 for i in range(checksize - self._this_size))))

            if srecl.chnum != srecl.check_check():
                raise ValueError('!!srec file check failed at {}!!'.format(srecl))

            #校验成功，转换为二进制行
            if checksize >= self._this_size:
                self._barray.extend(array('B', (data for data in srecl.data)))
            else:
                self._barray.extend(array('B', (data for data in srecl.data[self._this_size - checksize:])))
        
            self._this_size = len(srecl.data) 
            self._this_addr = srecl.addr
            
        #合并的应用固件入口地址
        self._app_entry = srecl.addr
        

    #更新合并后的长度信息，保存所有数据到imx文件
    def mk_binfile(self, filename):
        #替换长度信息
        self.__bin_replace(0x1024, struct.pack('I', len(self._barray)))
        with open(filename, 'wb') as bp :
            self._barray.tofile(bp)
        
    #制作应用固件头信息，Boot检测有效性通过
    def mk_app_head(self, args):
        head  = struct.pack('I', 0x55cccc55)
        hlen  = struct.pack('H', 76)
        stat  = struct.pack('H', 2)
        #4bytes(升级次数)
        #从链接地址开始，有效固件信息大小
        size    = len(self._barray[self._app_link - 0x60000000:])
        fw_size = struct.pack('I', size)
        entry   = struct.pack('I', self._app_entry)
        create_time   = time.time() + 28800
        create_time   = struct.pack('I', create_time)
        #4bytes(CRC校验码)

        #计算校验码
        sum = head + hlen + stat + ''.zfill(4) + args.hv + \
              args.fv + fw_size + entry + create_time + \
              args.data + ''.zfill(4)

        chksum = 0
        for i in range(0, len(sum), 4):
            chksum += struct.unpack('I', sum[i:i+4])[0]
        chk = struct.pack('I', ~chksum & 0xFFFFFFFF)
        
        #应用固件头信息，为了通过boot的有效性检查
        app_head = head + hlen + stat + ''.zfill(4) + \
                   args.hv + args.fv + fw_size + entry + \
                   create_time + args.data + ''.zfill(4) + chk

        #替换掉本来的无效信息，默认info区域在链接地址前4k
        info_addr = self._app_link - 0x1000 - 0x60000000
        self.__bin_replace(info_addr, app_head)

def args_handles(args):
    args.addr   = int(args.addr, 16)

    #补齐头属性长度
    args.hv   = args.hv.ljust(4, '\0')
    args.fv   = args.fv.ljust(4, '\0')
    args.data = args.data.ljust(36, '\0')  

    if (len(args.hv) > 4):
        raise ValueError('!!hardware version too loog !!')

    if (len(args.fv) > 4):
        raise ValueError('!!firmware version too loog !!')

    if (len(args.data) > 36):
        raise ValueError('!!custom data too loog !!')  

#app_link为应用固件在flash上的存储地址(与boot对应)：
#   flash上运行时，填固件链接地址即可。
#   sdram上运行时，为boot中的存储基址。
if __name__ == '__main__' :
    parser = argparse.ArgumentParser()

    parser.add_argument('boot',     help='input boot srec file name')
    parser.add_argument('app',      help='input app srec file name')
    parser.add_argument('o',        help='output imx flle name')
    parser.add_argument('addr',     help='link addr')

    parser.add_argument('--hv',     help='hardware version',  default=HW_VERSION)
    parser.add_argument('--fv',     help='firemware version', default=FW_VERSION)
    parser.add_argument('--data',   help='custom data', default='')

    args = parser.parse_args()  

    try:
        #头属性，参数处理
        args_handles(args)
            
        #解析boot srec文件，生成临时的imx文件，仅用于获取大小，判断应用链接地址是否合法
        sfile = SrecX(args.boot)
        sfile.mk_binary(0)
        sfile.mk_binfile('tmp.imx') 
        size = os.path.getsize('tmp.imx')
        #删除临时的imx文件
        os.system('del tmp.imx')
        
        #应用链接地址合法性判断
        #应用链接地址前4K的区域为应用固件头信息
        if ((args.addr - 0x1000 - 0x60000000) < size) or \
            (args.addr > 0x60800000):
            raise ValueError('!!param error!!\r\nrefer to: app_link fail')
        
        #合并srec文件
        data = 0;
        with open(args.boot, 'rb') as fd:
            data = fd.read()
            fd.close()
        with open(args.app, 'rb') as fd:
            data += fd.read()
            fd.close()   
        with open('tmp.srec', 'wb') as fd:
            fd.write(data)
            fd.close()   
            
        #解析srec文件
        sfile = SrecX('tmp.srec')
        print_auto_encoding(u'firmware init load address is {}'.format(sfile.start_addr))

        #删除临时合并的srec文件
        os.system('del tmp.srec')
        
        print_auto_encoding(u'make bootable image...')
        #生成二进制数组
        sfile.mk_binary(args.addr)
        #添加应用头信息
        sfile.mk_app_head(args)
        #生成二进制文件
        sfile.mk_binfile(args.o)
        
    except ValueError, error: 
        print_auto_encoding(str(error).decode('utf8'))
        error_exit()

    except KeyError, error:
        print_auto_encoding(u'!!srecline with wrong type {}!!'.format(error))
        error_exit()
    
    else:
        #转换成功
        print_auto_encoding(u'IMX bootable image generated successfully')