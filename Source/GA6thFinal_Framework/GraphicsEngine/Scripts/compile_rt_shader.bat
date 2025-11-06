@echo off
     setlocal

     set SCRIPT_DIR=%~dp0
     set EXE_PATH=%SCRIPT_DIR%CompileRTShader.exe
     set OBJ_PATH=%SCRIPT_DIR%CompileRTShader.obj

     REM Check if executable exists
     if not exist "%EXE_PATH%" (
         echo CompileRTShader.exe not found. Building...

         call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

         cd /d "%SCRIPT_DIR%"
         cl.exe /EHsc /std:c++17 /O2 CompileRTShader.cpp /Fe:CompileRTShader.exe

         if errorlevel 1 (
             echo Failed to compile CompileRTShader.exe
             REM Clean up on failure
             if exist "%OBJ_PATH%" del "%OBJ_PATH%"
             pause
             exit /b 1
         )

         REM Clean up .obj file immediately after successful build
         if exist "%OBJ_PATH%" del "%OBJ_PATH%"

         echo Build successful!
         echo.
     )

     REM Run the compiler
     echo Running RT Shader compiler...
     "%EXE_PATH%" release

     if errorlevel 1 (
         echo RT Shader compilation failed!
         pause
         exit /b 1
     )

     echo.
     echo RT Shader compilation completed successfully!
     pause