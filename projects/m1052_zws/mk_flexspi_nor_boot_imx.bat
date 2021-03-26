@echo off

Rem 工程名(注意：用户需要手动设置工程名为自己的工程名，否则无法生成烧写文件)
Rem 例如SDK示例工程的名字为“img_rt1050_debug”
SET project=img_rt1050_debug

set burn_tools_common_dir=..\..\..\..\tools\SDK发布\04.烧写工具\生产烧写工具
set dcd_and_imx_dirs=%burn_tools_common_dir%\common_tools\dcd_and_imx
set copy_dst_dir=%burn_tools_common_dir%\QSPI Flash启动\M105x

echo.
echo 生成dcd文件...
%dcd_and_imx_dirs%\mk_dcd_file.exe debug_config\dcd.cfg debug_config\test.dcd
echo.
Rem echo 请选择一个需要生成flexspi_nor启动文件的构建类型1)Debug 2)Release 3)MinSizeRel:
choice /c 123 /m "请选择一个需要生成flexspi_nor启动文件的构建类型1)Debug 2)Release 3)MinSizeRel:"

set ret=%errorlevel%
if %ret% EQU 1 set TYPE=Debug
if %ret% EQU 2 set TYPE=Release
if %ret% EQU 3 set TYPE=MinSizeRel

echo 生成imx文件...
%dcd_and_imx_dirs%\mk_flexspi_nor_boot_imx.exe %TYPE%\%project%.srec debug_config\test.dcd %TYPE%\%project%.imx
copy %TYPE%\%project%_nopadding.imx "%copy_dst_dir%\%project%_nopadding.imx"

pause
