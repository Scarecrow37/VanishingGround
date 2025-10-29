@echo off
     REM This batch file executes the PowerShell script to compile all shaders including RT shaders.

     REM Get the directory where this batch file is located.
     set SCRIPT_DIR=%~dp0

     REM Define the path to the PowerShell script.
     set PS_SCRIPT_PATH=%SCRIPT_DIR%compile_shaders.ps1

     REM Define the path to the RT Shader compiler batch
     set RT_SHADER_BAT=%SCRIPT_DIR%compile_rt_shader.bat

     echo ================================================================
     echo Starting Shader Compilation Process
     echo ================================================================
     echo.

     REM Execute the PowerShell script for regular shaders
     echo [1/2] Compiling regular shaders (VS, PS, CS, GS, HS, DS)...
     echo ================================================================
     powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%PS_SCRIPT_PATH%"

     if errorlevel 1 (
         echo.
         echo [ERROR] Regular shader compilation failed!
         pause
         exit /b 1
     )

     echo.
     echo ================================================================
     echo [1/2] Regular shaders completed!
     echo ================================================================
     echo.

     REM Execute RT Shader compilation
     echo [2/2] Compiling Ray Tracing shaders...
     echo ================================================================
     call "%RT_SHADER_BAT%"

     if errorlevel 1 (
         echo.
         echo [ERROR] RT shader compilation failed!
         pause
         exit /b 1
     )

     echo.
     echo ================================================================
     echo [2/2] RT shaders completed!
     echo ================================================================
     echo.
     echo ================================================================
     echo All shader compilation completed successfully!
     echo   - Regular shaders: compiled with FXC
     echo   - RT shaders: compiled with DXC
     echo ================================================================
     pause
