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


def do_mk_dcd_file(dcd_cfg_file,dcd_file):
    sys_encoding = locale.getdefaultlocale()[1];
    exe_path = os.path.join(CUR_PY_PATH,'mk_dcd_file.exe');
    cmd_argv_list = [exe_path,];
    cmd_argv_list.append(dcd_cfg_file.encode(sys_encoding) );
    cmd_argv_list.append(dcd_file.encode(sys_encoding));
    
    p = subprocess.Popen(cmd_argv_list)
    p.wait()
    sys.stdout.flush();
    return  p.returncode;


###########################################################################
## Class Mk_dcd_file_dlg
###########################################################################

class Mk_dcd_file_dlg ( wx.Dialog ):

    def __init__( self, parent ):
        wx.Dialog.__init__ ( self, parent, id = wx.ID_ANY, title = u"dcd文件生成工具", pos = wx.DefaultPosition, size = wx.Size( 644,366 ), style = wx.DEFAULT_DIALOG_STYLE )

        self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )

        gbSizer1 = wx.GridBagSizer( 9, 4 )
        gbSizer1.SetFlexibleDirection( wx.BOTH )
        gbSizer1.SetNonFlexibleGrowMode( wx.FLEX_GROWMODE_SPECIFIED )

        self.m_button_browse_dcd_cfg_file = wx.Button( self, wx.ID_ANY, u"浏览...", wx.DefaultPosition, wx.DefaultSize, 0 )
        gbSizer1.Add( self.m_button_browse_dcd_cfg_file, wx.GBPosition( 1, 3 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_button_brow_save_dcd_file = wx.Button( self, wx.ID_ANY, u"浏览...", wx.DefaultPosition, wx.DefaultSize, 0 )
        gbSizer1.Add( self.m_button_brow_save_dcd_file, wx.GBPosition( 3, 3 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_staticText_sel_dcd_cfg_file = wx.StaticText( self, wx.ID_ANY, u"请选择dcd配置文件:", wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText_sel_dcd_cfg_file.Wrap( -1 )
        gbSizer1.Add( self.m_staticText_sel_dcd_cfg_file, wx.GBPosition( 0, 0 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_staticText_dcd_file = wx.StaticText( self, wx.ID_ANY, u"请选择要生成的dcd文件路径:", wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText_dcd_file.Wrap( -1 )
        gbSizer1.Add( self.m_staticText_dcd_file, wx.GBPosition( 2, 0 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_button_mk_dcd = wx.Button( self, wx.ID_ANY, u"生成", wx.DefaultPosition, wx.DefaultSize, 0 )
        gbSizer1.Add( self.m_button_mk_dcd, wx.GBPosition( 4, 0 ), wx.GBSpan( 1, 4 ), wx.ALL|wx.EXPAND, 5 )

        self.m_staticText_space_r0_c1 = wx.StaticText( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText_space_r0_c1.Wrap( -1 )
        self.m_staticText_space_r0_c1.SetMinSize( wx.Size( 250,-1 ) )

        gbSizer1.Add( self.m_staticText_space_r0_c1, wx.GBPosition( 0, 1 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_staticText_space_r2_c1 = wx.StaticText( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText_space_r2_c1.Wrap( -1 )
        gbSizer1.Add( self.m_staticText_space_r2_c1, wx.GBPosition( 2, 1 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_staticText_space_r0_c3 = wx.StaticText( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText_space_r0_c3.Wrap( -1 )
        gbSizer1.Add( self.m_staticText_space_r0_c3, wx.GBPosition( 0, 3 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_staticText_space_r0_c2 = wx.StaticText( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText_space_r0_c2.Wrap( -1 )
        gbSizer1.Add( self.m_staticText_space_r0_c2, wx.GBPosition( 0, 2 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_staticText_space_r2_c2 = wx.StaticText( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText_space_r2_c2.Wrap( -1 )
        gbSizer1.Add( self.m_staticText_space_r2_c2, wx.GBPosition( 2, 2 ), wx.GBSpan( 1, 1 ), wx.ALL, 5 )

        self.m_textCtrl_dcd_cfg_file = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_READONLY )
        gbSizer1.Add( self.m_textCtrl_dcd_cfg_file, wx.GBPosition( 1, 0 ), wx.GBSpan( 1, 2 ), wx.ALL|wx.EXPAND, 5 )

        self.m_textCtrl_dcd_file = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_READONLY )
        gbSizer1.Add( self.m_textCtrl_dcd_file, wx.GBPosition( 3, 0 ), wx.GBSpan( 1, 2 ), wx.ALL|wx.EXPAND, 5 )

        self.m_staticText_log = wx.StaticText( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0|wx.STATIC_BORDER )
        self.m_staticText_log.Wrap( -1 )
        gbSizer1.Add( self.m_staticText_log, wx.GBPosition( 5, 0 ), wx.GBSpan( 4, 4 ), wx.EXPAND|wx.ALL, 5 )


        self.SetSizer( gbSizer1 )
        self.Layout()

        self.Centre( wx.BOTH )

        # Connect Events
        self.m_button_browse_dcd_cfg_file.Bind( wx.EVT_BUTTON, self.OnButtonClickBrowseDCDCfgFile )
        self.m_button_brow_save_dcd_file.Bind( wx.EVT_BUTTON, self.OnButtonClickBrowseDCDFile )
        self.m_button_mk_dcd.Bind( wx.EVT_BUTTON, self.OnButtonClickMkDCDFile )

    def __del__( self ):
        pass


    # Virtual event handlers, overide them in your derived class
    def OnButtonClickBrowseDCDCfgFile( self, event ):
        wildcard = u"dcd配置文件(*.cfg)|*.cfg"
        dlg = wx.FileDialog(self,
                            message = u"选择一个dcd配置文件",
                            wildcard = wildcard,
                            style=wx.FD_OPEN|wx.FD_FILE_MUST_EXIST);
        if wx.ID_OK == dlg.ShowModal() :
            self.m_textCtrl_dcd_cfg_file.SetValue(dlg.GetPath()); 
        event.Skip()

    def OnButtonClickBrowseDCDFile( self, event ):
        wildcard = u"dcd文件(*.dcd)|*.dcd"
        dlg = wx.FileDialog(self,
                            message = u"保存DCD文件",
                            wildcard = wildcard,
                            style=wx.FD_SAVE|wx.FD_OVERWRITE_PROMPT );
        if wx.ID_OK == dlg.ShowModal() :
            self.m_textCtrl_dcd_file.SetValue(dlg.GetPath()); 
            
        event.Skip()

    def OnButtonClickMkDCDFile( self, event ):
        event.Skip();
        dcd_cfg_file_path = self.m_textCtrl_dcd_cfg_file.GetValue();
        dcd_file_path = self.m_textCtrl_dcd_file.GetValue();
        if 0 == len(dcd_file_path):
            wx.MessageBox(u'还没有选择要生成的dcd文件路径',u'错误',wx.OK|wx.ICON_ERROR,self);
            return;
        
        if 0 == len(dcd_cfg_file_path):
            wx.MessageBox(u'还没有选择dcd配置文件路径',u'错误',wx.OK|wx.ICON_ERROR,self);
            return;
        # 执行配置程序
        if 0 == do_mk_dcd_file(dcd_cfg_file_path,dcd_file_path):
            self.m_staticText_log.SetLabel('生成dcd文件成功');
        else:
            self.m_staticText_log.SetLabel('生成dcd文件失败');



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
    frame = Mk_dcd_file_dlg(None);
    frame.ShowModal();

