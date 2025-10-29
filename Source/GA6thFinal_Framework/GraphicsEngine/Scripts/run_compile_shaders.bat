@echo off
REM This batch file executes the PowerShell script to update shader enums.

REM Get the directory where this batch file is located.
set SCRIPT_DIR=%~dp0

REM Define the path to the PowerShell script.
set PS_SCRIPT_PATH=%SCRIPT_DIR%compile_shaders.ps1

REM Execute the PowerShell script, bypassing the execution policy for this run only.
echo Running PowerShell script...
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%PS_SCRIPT_PATH%"

echo.
echo Script execution finished.
pause
