#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import time
import subprocess
import shutil
import re
import struct
import wx
import wx
import wx.xrc
import locale

def print_auto_encoding(s):
    encode = sys.stdout.encoding;
    if None == encode:
        encode = 'utf-8'
    print (s.decode('utf-8').encode(encode) );
    sys.stdout.flush();

def error_exit(msg):
    print_auto_encoding(msg);
    time.sleep(3);
    sys.exit(-1);
    
    
BOOT_MODE_SD_CARD=0;
BOOT_MODE_FLEX_SPI_NOR_FLASH = 1;


def do_mk_imx_file(firmare_file,dcd_file,imx_file,boot_mode):
    sys_encoding = locale.getdefaultlocale()[1];
    
    if BOOT_MODE_SD_CARD == boot_mode:
        exe_path = os.path.join(CUR_PY_PATH,'mk_sd_boot_imx.exe');
    elif BOOT_MODE_FLEX_SPI_NOR_FLASH == boot_mode:
        exe_path = os.path.join(CUR_PY_PATH,'mk_flexspi_nor_boot_imx.exe');
    else :
        return -1;
    cmd_argv_list = [exe_path,];
    
    cmd_argv_list.append(firmare_file.encode(sys_encoding) );
    cmd_argv_list.append(dcd_file.encode(sys_encoding));
    cmd_argv_list.append(imx_file.encode(sys_encoding) );

    p = subprocess.Popen(cmd_argv_list)
    p.wait()
    sys.stdout.flush();
    return  p.returncode;


###########################################################################
## Class MkBootImxDlg
###########################################################################

class MkBootImxDlg ( wx.Dialog ):

    def __init__( self, parent ):
        wx.Dialog.__init__ ( self, parent, id = wx.ID_ANY, title = u"imx1050 imx文件生成器", pos = wx.DefaultPosition, size = wx.Size( 699,399 ), style = wx.DEFAULT_DIALOG_STYLE )

        self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )

        gbSizer1 = wx.GridBagSizer( 0, 0 )
        gbSizer1.SetFlexibleDirection( wx.BOTH )
        gbSizer1.SetNonFlexibleGrowMode( wx.FLEX_GROWMODE_SPECIFIED )

        self.m_staticText_select_firmware = wx.StaticText( self, wx.ID_ANY, u"请选择用作启动的固件:", wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText_select_firmware.Wrap( -1 )
        gbSizer1.Add( self.m_staticText_select_firmware, wx.GBPosition( 0, 0 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_textCtrl_firmware_path = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_READONLY )
        gbSizer1.Add( self.m_textCtrl_firmware_path, wx.GBPosition( 1, 0 ), wx.GBSpan( 1, 2 ), wx.ALL|wx.EXPAND, 5 )

        self.m_staticText_reserve_space = wx.StaticText( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText_reserve_space.Wrap( -1 )
        self.m_staticText_reserve_space.SetMinSize( wx.Size( 350,-1 ) )

        gbSizer1.Add( self.m_staticText_reserve_space, wx.GBPosition( 0, 1 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_staticText_select_dcd_file = wx.StaticText( self, wx.ID_ANY, u"请选择需要的dcd文件:", wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText_select_dcd_file.Wrap( -1 )
        gbSizer1.Add( self.m_staticText_select_dcd_file, wx.GBPosition( 2, 0 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_textCtrl_dcd_file_path = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_READONLY )
        gbSizer1.Add( self.m_textCtrl_dcd_file_path, wx.GBPosition( 3, 0 ), wx.GBSpan( 1, 2 ), wx.ALL|wx.EXPAND, 5 )

        self.m_textCtrl_boot_file_save_path = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_READONLY )
        gbSizer1.Add( self.m_textCtrl_boot_file_save_path, wx.GBPosition( 5, 0 ), wx.GBSpan( 1, 2 ), wx.ALL|wx.EXPAND, 5 )

        self.m_button_browse_dcd_file = wx.Button( self, wx.ID_ANY, u"浏览...", wx.DefaultPosition, wx.DefaultSize, 0 )
        gbSizer1.Add( self.m_button_browse_dcd_file, wx.GBPosition( 3, 2 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_button_browse_boot_file = wx.Button( self, wx.ID_ANY, u"浏览...", wx.DefaultPosition, wx.DefaultSize, 0 )
        gbSizer1.Add( self.m_button_browse_boot_file, wx.GBPosition( 5, 2 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_button_do = wx.Button( self, wx.ID_ANY, u"生成", wx.DefaultPosition, wx.DefaultSize, 0 )
        gbSizer1.Add( self.m_button_do, wx.GBPosition( 7, 0 ), wx.GBSpan( 1, 3 ), wx.ALL|wx.EXPAND, 5 )

        m_choice_boot_deviceChoices = [ u"sd卡", u"FlexSPI NOR Flash" ]
        self.m_choice_boot_device = wx.Choice( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, m_choice_boot_deviceChoices, 0 )
        self.m_choice_boot_device.SetSelection( 0 )
        gbSizer1.Add( self.m_choice_boot_device, wx.GBPosition( 6, 1 ), wx.GBSpan( 1, 2 ), wx.ALL, 5 )

        self.m_staticText_select_boot_mode = wx.StaticText( self, wx.ID_ANY, u"请选择启动方式:", wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText_select_boot_mode.Wrap( -1 )
        gbSizer1.Add( self.m_staticText_select_boot_mode, wx.GBPosition( 6, 0 ), wx.GBSpan( 1, 1 ), wx.ALL|wx.EXPAND, 5 )

        self.m_textCtrl_log = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_READONLY )
        gbSizer1.Add( self.m_textCtrl_log, wx.GBPosition( 8, 0 ), wx.GBSpan( 1, 3 ), wx.ALL|wx.EXPAND, 5 )

        self.m_button_browse_firmare = wx.Button( self, wx.ID_ANY, u"浏览...", wx.DefaultPosition, wx.DefaultSize, 0 )
        gbSizer1.Add( self.m_button_browse_firmare, wx.GBPosition( 1, 2 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_staticText_select_boot_file_save_path = wx.StaticText( self, wx.ID_ANY, u"请选择启动文件保存路径:", wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText_select_boot_file_save_path.Wrap( -1 )
        gbSizer1.Add( self.m_staticText_select_boot_file_save_path, wx.GBPosition( 4, 0 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )


        self.SetSizer( gbSizer1 )
        self.Layout()

        self.Centre( wx.BOTH )

        # Connect Events
        self.m_button_browse_dcd_file.Bind( wx.EVT_BUTTON, self.OnBrowseDCDFile )
        self.m_button_browse_boot_file.Bind( wx.EVT_BUTTON, self.OnBrowseSavePath )
        self.m_button_do.Bind( wx.EVT_BUTTON, self.OnClickDo )
        self.m_button_browse_firmare.Bind( wx.EVT_BUTTON, self.OnBrowseFirmare )

    def __del__( self ):
        pass


    # Virtual event handlers, overide them in your derived class
    def OnBrowseDCDFile( self, event ):
        event.Skip();
        wildcard = u"dcd文件(*.dcd)|*.dcd"
        dlg = wx.FileDialog(self,
                            message = u"选择DCD文件",
                            wildcard = wildcard,
                            style=wx.FD_OPEN|wx.FD_FILE_MUST_EXIST );
        if wx.ID_OK == dlg.ShowModal() :
            self.m_textCtrl_dcd_file_path.SetValue(dlg.GetPath()); 

    def OnBrowseSavePath( self, event ):
        wildcard = u"IMX文件(*.imx)|*.imx"
        dlg = wx.FileDialog(self,
                            message = u"保存imx文件",
                            wildcard = wildcard,
                            style=wx.FD_SAVE|wx.FD_OVERWRITE_PROMPT);
        if wx.ID_OK == dlg.ShowModal() :
            self.m_textCtrl_boot_file_save_path.SetValue(dlg.GetPath()); 
        event.Skip()
        event.Skip()

    def OnClickDo( self, event ):
        event.Skip();
        firmware_path = self.m_textCtrl_firmware_path.GetValue();
        if 0 == len(firmware_path):
            wx.MessageBox(u'还没有选择固件',u'错误',wx.OK|wx.ICON_ERROR,self);
            return;
        
        dcd_file_path = self.m_textCtrl_dcd_file_path.GetValue();
        if 0 == len(dcd_file_path):
            wx.MessageBox(u'还没有选择dcd文件',u'错误',wx.OK|wx.ICON_ERROR,self);
            return;
            
        imx_file_path = self.m_textCtrl_boot_file_save_path.GetValue();
        if 0 == len(imx_file_path):
            wx.MessageBox(u'还没有选择imx文件的保存路径',u'错误',wx.OK|wx.ICON_ERROR,self);
            return;
        boot_mode = self.m_choice_boot_device.GetCurrentSelection();
        if 0 == do_mk_imx_file(firmware_path,dcd_file_path,imx_file_path,boot_mode):
            self.m_textCtrl_log.SetValue(u'生成imx文件成功');
        else :
            self.m_textCtrl_log.SetValue(u'生成imx文件失败');
    def OnBrowseFirmare( self, event ):
        event.Skip();
        wildcard = u"固件(*.srec)|*.srec"
        dlg = wx.FileDialog(self,
                            message = u"选择固件",
                            wildcard = wildcard,
                            style=wx.FD_OPEN|wx.FD_FILE_MUST_EXIST );
        if wx.ID_OK == dlg.ShowModal() :
            self.m_textCtrl_firmware_path.SetValue(dlg.GetPath()); 



if __name__ == '__main__':
    global CUR_PY_PATH
    #获取当前路径
    if getattr(sys, 'frozen', False):
        CUR_PY_PATH = os.path.dirname(sys.executable)
    else :
        CUR_PY_PATH = os.path.split(os.path.realpath(__file__))[0];

    TOOLS_ROOT_DIR = os.path.join(CUR_PY_PATH,'..','Flashloader_RT1050_1.0','Tools');
    TOOLS_ROOT_DIR = os.path.realpath(TOOLS_ROOT_DIR);

    app = wx.App(False);
    frame = MkBootImxDlg(None);
    frame.ShowModal();

