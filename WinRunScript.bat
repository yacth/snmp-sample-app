@echo off
echo =================================================================
echo Batch Script to Run snmp-sample-app with user parameters
echo =================================================================
echo This script prompts the user for necessary input parameters 
echo and executes the snmp-sample-app using xmake through MSYS2.
echo =================================================================
echo The command uses the following parameters:
echo [IP_ADDRESS] [IP_PORT] [INPUT_OID] [OUTPUT_OID] [COMMAND]
echo .
echo Default values: 
echo localhost 1601 1.3.6.1.2.1.1.5.0 1.3.6.1.2.1.1.1.0 "Test command"
echo .
echo =================================================================

REM Prompt for IP address
set /p IP_ADDRESS=Enter Semsuron IP address (press Enter to default):
if "%IP_ADDRESS%"=="" set IP_ADDRESS=localhost

REM Prompt for IP port
set /p IP_PORT=Enter Sensuron Port (press Enter to use default):
if "%IP_PORT%"=="" set IP_PORT=1601

REM Prompt for input OID
set /p INPUT_OID=Enter Received data OID (press Enter to use default):
if "%INPUT_OID%"=="" set INPUT_OID=1.3.6.1.2.1.1.5.0

REM Prompt for output OID
set /p OUTPUT_OID=Enter Sent data OID (press Enter to use default): 
if "%OUTPUT_OID%"=="" set OUTPUT_OID=1.3.6.1.2.1.1.1.0

REM Prompt for command (optional)
set /p COMMAND=Enter Command: 
if "%COMMAND%"=="" set COMMAND=Test command

REM Define the path to MSYS2 shell
set MSYS2_PATH=C:\msys64\usr\bin\bash.exe

REM Run the executable with arguments
%MSYS2_PATH% xmake run Sensuron %IP_ADDRESS% %IP_PORT% %INPUT_OID% %OUTPUT_OID% "%COMMAND%"

REM Check if the execution was successful
if %ERRORLEVEL% neq 0 (
    echo Error occurred while running the application.
) else (
    echo Application ran successfully.
)

pause
