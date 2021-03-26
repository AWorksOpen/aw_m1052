@echo off

echo. 
echo I.MX RT1050 SPI FLASH烧写程序

Rem 自动获取后缀为.srec的固件
set file_path=%~dp0

for /f "delims=" %%a in ('dir /b /a-d /o-d "%file_path%\*.srec"') do (set ndata=%%a)
set firmware_name=%ndata:~0,-5%

echo 生成imx文件...
..\..\common_tools\dcd_and_imx\mk_flexspi_nor_boot_srec2imx.exe %firmware_name%.srec %firmware_name%.imx

echo. 
echo 烧录文件名为：%firmware_name%_nopadding.imx

echo. 
echo 请输入SD卡盘符：
set /p diskpath=
REM set diskpath=E
echo 注意:
echo     文件会被烧写在 %diskpath% 盘

@echo 是否需要格式化 %diskpath% 盘?(Y/N):

set /p sel=

if "%sel%" == "y" set sel=Y
if "%sel%" == "Y" (
    echo 你选择了格式化 %diskpath% 盘
    echo **** 重建分区表 ****
    ..\..\common_tools\bin\repart 40960 %diskpath%


    echo **** 格式化分区 ****
    ..\..\common_tools\bin\fat32format -c8 %diskpath%

    echo " "
) else (
    echo 你选择了不格式化 %diskpath% 盘
)

echo **** 烧写AWorks系统到保留扇区 ****
rem  1.烧写“烧写固件”到SD卡， 第2个扇区开始
..\..\common_tools\bin\cfimager.exe -raw -offset 0x400 -f m105x_flash_burn.imx -d %diskpath%

rem 2.烧写“nor启动固件”到SD卡，第2048个扇区开始
..\..\common_tools\bin\cfimager.exe -raw -offset 0x100000 -f %firmware_name%_nopadding.imx -d %diskpath%
pause
