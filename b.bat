@echo off
REM setlocal
chcp 65001

set CompilerExe=cl.exe
call .working/initvc.cmd

set buildDir=.Build
if not exist %buildDir% (
    mkdir %buildDir%
)

set scriptDir=%~dp0

set buildTarget=%1
if "%buildTarget%" == "" (
    set buildSource=../client.cpp
) else (
    set buildSource=../server.cpp
)

pushd %buildDir%
%CompilerExe% %buildSource% /EHsc /nologo /std:c++17 -I%scriptDir%enet-1.3.15/include /link /LIBPATH:%scriptDir%enet-1.3.15 ^
    enet64.lib gdi32.lib user32.lib Shell32.lib kernel32.lib ws2_32.lib winmm.lib
popd

REM endlocal
