@echo off

Rem ������(ע�⣺�û���Ҫ�ֶ����ù�����Ϊ�Լ��Ĺ������������޷�������д�ļ�)
Rem ����SDKʾ�����̵�����Ϊ��img_rt1050_debug��
SET project=img_rt1050_debug

set burn_tools_common_dir=..\..\..\..\tools\SDK����\04.��д����\������д����
set dcd_and_imx_dirs=%burn_tools_common_dir%\common_tools\dcd_and_imx
set copy_dst_dir=%burn_tools_common_dir%\QSPI Flash����\M105x

echo.
echo ����dcd�ļ�...
%dcd_and_imx_dirs%\mk_dcd_file.exe debug_config\dcd.cfg debug_config\test.dcd
echo.
Rem echo ��ѡ��һ����Ҫ����flexspi_nor�����ļ��Ĺ�������1)Debug 2)Release 3)MinSizeRel:
choice /c 123 /m "��ѡ��һ����Ҫ����flexspi_nor�����ļ��Ĺ�������1)Debug 2)Release 3)MinSizeRel:"

set ret=%errorlevel%
if %ret% EQU 1 set TYPE=Debug
if %ret% EQU 2 set TYPE=Release
if %ret% EQU 3 set TYPE=MinSizeRel

echo ����imx�ļ�...
%dcd_and_imx_dirs%\mk_flexspi_nor_boot_imx.exe %TYPE%\%project%.srec debug_config\test.dcd %TYPE%\%project%.imx
copy %TYPE%\%project%_nopadding.imx "%copy_dst_dir%\%project%_nopadding.imx"

pause
