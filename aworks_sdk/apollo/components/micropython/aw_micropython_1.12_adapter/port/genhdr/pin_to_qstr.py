#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import re
import sys
# sys.path.append('config/genhdr/gen_qstr')

import gen_qstr

def pin_to_qstr():
    f_qstr = open('temp.txt','w')

    f_pin = open(r"imx1050_pin.h",'r')
    f_qstr.write('\r')
    f_pins = open('aw_machine_gpio.c','r')
    content  = f_pins.read()
    f_pins.close()
    for line in f_pin:
        searchOBJ = re.search(r'#define GPIO(.*?) .*',line,re.M|re.I)
        if searchOBJ:
            str = 'GPIO' + searchOBJ.group(1)
            qstr_computer = gen_qstr.make_bytes(1, 1, str)
            f_qstr.write(qstr_computer + '\r')
            #qstr_computer = '\r' + '  { MP_ROM_QSTR(MP_QSTR_PIO1_22), MP_ROM_INT(PIO1_22) },' + '\r'
            qstr_computer = '\r  { MP_ROM_QSTR(MP_QSTR_' + str + '), MP_ROM_INT(' + str + ') },\r'
            pos = content.find("}; /*GPIO dict table*/")
            if pos != -1:
                content = content[:pos] + qstr_computer + content[pos:] 
    f_pins = open('aw_machine_gpio.c','w')
    f_pins.write(content+'\r')
    f_pins.close()
    f_pin.close()
    f_qstr.close()

if __name__ == "__main__":
    pin_to_qstr()