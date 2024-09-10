@echo off
echo Select build option:
echo 1. Build Visual Studio project
echo 2. Build CLI app
set /p choice=Enter your choice (1 or 2):

echo.

if "%choice%"=="1" (
    echo Building Visual Studio project...
    xmake project -k vsxmake2022 -m "debug;release"
    start vsxmake2022/snmp-sample-app.sln
) else if "%choice%"=="2" (
    echo Building CLI app with xmake...
    xmake
) else (
    echo Invalid choice. Please run the script again and select a valid option.
)

pause
