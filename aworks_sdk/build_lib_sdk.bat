call scons.bat CPU_CORE_NAME=cortex-m4 BUILD_TYPE=Release -j4
set errno=%ERRORLEVEL%
if %errno% neq 0 (exit /b %errno%)
call scons.bat CPU_CORE_NAME=cortex-m4 BUILD_TYPE=Debug -j4
set errno=%ERRORLEVEL%
if %errno% neq 0 (exit /b %errno%)
call scons.bat CPU_CORE_NAME=cortex-m4 BUILD_TYPE=MinSizeRel -j4
set errno=%ERRORLEVEL%
if %errno% neq 0 (exit /b %errno%)
call scons.bat CPU_CORE_NAME=cortex-m4 BUILD_TYPE=RelWithDebInfo -j4
set errno=%ERRORLEVEL%
if %errno% neq 0 (exit /b %errno%)

call scons.bat CPU_CORE_NAME=cortex-m0plus BUILD_TYPE=Release -j4
set errno=%ERRORLEVEL%
if %errno% neq 0 (exit /b %errno%)
call scons.bat CPU_CORE_NAME=cortex-m0plus BUILD_TYPE=Debug -j4
set errno=%ERRORLEVEL%
if %errno% neq 0 (exit /b %errno%)
call scons.bat CPU_CORE_NAME=cortex-m0plus BUILD_TYPE=MinSizeRel -j4
set errno=%ERRORLEVEL%
if %errno% neq 0 (exit /b %errno%)
call scons.bat CPU_CORE_NAME=cortex-m0plus BUILD_TYPE=RelWithDebInfo -j4
set errno=%ERRORLEVEL%
if %errno% neq 0 (exit /b %errno%)