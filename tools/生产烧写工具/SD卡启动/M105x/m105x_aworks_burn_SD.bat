@echo off
echo. 
echo I.MX RT1050 SD��д����

Rem �Զ���ȡ��׺Ϊ.srec�Ĺ̼�
set file_path=%~dp0

for /f "delims=" %%a in ('dir /b /a-d /o-d "%file_path%\*.srec"') do (set ndata=%%a)
set firmware_name=%ndata:~0,-5%

echo ����imx�ļ�...
..\..\common_tools\dcd_and_imx\mk_sd_boot_imx.exe %firmware_name%.srec ..\..\common_tools\dcd\m105x.dcd %firmware_name%.imx

echo.
echo ��¼�ļ���Ϊ��%firmware_name%_nopadding.imx

echo. 
echo ������SD���̷���
set /p diskpath=
REM set diskpath=E
echo ע��:
echo     �ļ��ᱻ��д�� %diskpath% ��

@echo �Ƿ���Ҫ��ʽ�� %diskpath% ��?(Y/N):

set /p sel=

if "%sel%" == "y" set sel=Y
if "%sel%" == "Y" (
    echo ��ѡ���˸�ʽ�� %diskpath% ��
    echo **** �ؽ������� ****
    ..\..\common_tools\bin\repart 20480 %diskpath%


    echo **** ��ʽ������ ****
    ..\..\common_tools\bin\fat32format -c8 %diskpath%

    echo " "
) else (
    echo ��ѡ���˲���ʽ�� %diskpath% ��
)

echo **** ��дAWorksϵͳ���������� ****
..\..\common_tools\bin\cfimager.exe -raw -offset 0x400 -f %firmware_name%_nopadding.imx -d %diskpath%
pause