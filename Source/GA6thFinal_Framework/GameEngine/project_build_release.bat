@echo off
chcp 65001

:: Visual Studio 환경 설정
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

:: 프로젝트 빌드
echo 프로젝트 빌드를 시작합니다.
msbuild "../GA6thFinal_Framework.sln" /t:ProjectVG /p:Configuration=Release /p:Platform=x64 
set BUILD_RESULT=%ERRORLEVEL%

if %BUILD_RESULT% neq 0 (
    echo 빌드 실패! ERRORLEVEL=%BUILD_RESULT%
) else (
	::스크립트 빌드
	echo 스크립트 빌드를 시작합니다.
	msbuild "../GameScripts.sln" /t:GameScripts /p:Configuration=Release /p:Platform=x64 
	set BUILD_RESULT=%ERRORLEVEL%
	
	if %BUILD_RESULT% neq 0 (
		echo 빌드 실패! ERRORLEVEL=%BUILD_RESULT%
	) else (
		echo 빌드 성공!
	)
)
pause
exit /b %BUILD_RESULT%
