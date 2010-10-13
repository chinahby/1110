@echo off

rem window默认字符编码GBK,用于解决某些系统字符编码为UTF-8出现乱码
set OUTPUT_CHARSET=GBK
rem 设置环境变量
rem PATH=..\..\Tools\xMake;..\..\..\Tools\xMake;..\..\platform\brew\brewpk\brewtools\ResourceEditor;%PATH%

rem b n进行网络联编
IF "%~1"=="n" goto INCREDI_BUILD
rem b c清除文件
IF "%~1"=="c" goto CLEAN
rem b 普通编译
:NORMAL_BUILD
w021 %~1
goto EXIT

:INCREDI_BUILD
rem make使用的线程由这个bat引入
call w021 preobjs
call XGConsole /command="w021 genobjs -j20" /openmonitor /profile="XGConsole.xml"
rem 删除由s文件生成的o文件
call \bin\sh RmEmptyFile.sh W001
call w021
goto EXIT



:CLEAN
w021 clean
goto EXIT

:EXIT
rem echo msgbox "编译结束,退出！">msg.vbs
rem msg.vbs
rem del msg.vbs
rem IF NOT "%*"=="" goto END
msg %username% /TIME:60000 编译结束,退出！
goto END


:END
rem shutdown -s -f
rem rundll32.exe powrprof.dll,SetSuspendState 0,1,0



