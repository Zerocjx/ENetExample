@echo off

REM 准备：检测VC是否安装
where vswhere >nul 2>nul
if errorlevel 1 (
    echo [错误] 没有找到 vswhere.exe
    goto Ended
)

where cl >nul 2>nul
if errorlevel 1 (
    set getInstallCmd=vswhere -legacy -latest -property installationPath
    for /f "usebackq tokens=*" %%i in (`%getInstallCmd%`) do (
    set InstallDir=%%i
    )
    set vcVars="%InstallDir%\VC\Auxiliary\Build\vcvars64.bat"
    call %vcVars%
)

:Ended
