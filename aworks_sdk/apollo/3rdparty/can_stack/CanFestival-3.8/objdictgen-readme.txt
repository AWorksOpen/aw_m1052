对象词典生成方式

1. objdictgen/tools 安装 Python2.7 及 wxPython3.0-win64-3.0.2.0-py27

2. 双击运行 objdictgen/objdictedit.py

3. 来导入EDS文件或者导入od工程文件或者新建工程

4. 编辑节点信息
       名称: g_canfestival_inst0（该信息用户可修改，但修改后aw_canfestival_cfg.c中的__gp_canfestival_inst_data需要修改为生成的aw_canfestival_data_t）
       默认字符串：10（该信息用户可修改）
       描述：AWorks（该信息用户可修改）

5. 编辑自己所需的对象词典


6. 点击建立词典生成.c文件，替换AWorks中原有的对象词典即可

(可参考objdictgen/examples下的xgate-cop10-mst.od, aw_canfestival_inst0.c为生成的文件)