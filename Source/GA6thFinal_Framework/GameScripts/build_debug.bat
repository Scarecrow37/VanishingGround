@echo off
chcp 65001

cd /d "%~dp0"

set BASE_PATH=..\GameScripts\bin\Debug\
set "PDB_PATH=%BASE_PATH%GameScripts.pdb"
set "DLL_PATH=%BASE_PATH%GameScripts.dll"

set "NEW_PDB_NAME=Prev_GameScripts.pdb"
set "NEW_DLL_NAME=Prev_GameScripts.dll"

set "NEW_PDB_PATH=%BASE_PATH%%NEW_PDB_NAME%"
set "NEW_DLL_PATH=%BASE_PATH%%NEW_DLL_NAME%"

@echo PBD_PATH : %PDB_PATH%
@echo DLL_PATH : %DLL_PATH%

@echo NEW_PDB_PATH : %NEW_PDB_PATH%
@echo NEW_DLL_PATH : %NEW_DLL_PATH%

echo PDB 파일 삭제: %NEW_PDB_PATH%
if exist "%NEW_PDB_PATH%" (
    del /f /q "%NEW_PDB_PATH%"
    echo 삭제 완료
) else (
    echo 이전 PDB 파일이 존재하지 않습니다
)

echo DLL 파일 삭제: %NEW_DLL_PATH%
if exist "%NEW_DLL_PATH%" (
    del /f /q "%NEW_DLL_PATH%"
    echo 삭제 완료
) else (
    echo 이전 DLL 파일이 존재하지 않습니다
)

if exist "%PDB_PATH%" (
	ren "%PDB_PATH%" "%NEW_PDB_NAME%"
)


if exist "%DLL_PATH%" (
	ren "%DLL_PATH%" "%NEW_DLL_NAME%"
)

:: Visual Studio 환경 설정
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

:: 솔루션 파일 열고 빌드
msbuild "../GameScripts.sln" /t:GameScripts /p:Configuration=Debug /p:Platform=x64 

set BUILD_RESULT=%ERRORLEVEL%

if %BUILD_RESULT% neq 0 (
    echo 빌드 실패! ERRORLEVEL=%BUILD_RESULT%
	pause
) else (
    echo 빌드 성공!
)
exit /b %BUILD_RESULT%
