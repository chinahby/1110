@echo off
if "%1"=="" GOTO BAILOUT
if "%1"=="norimgtool" GOTO build_norimgtool
if "%1"=="clean" GOTO clean
if "%1"=="all" GOTO all
goto BAILOUT
:clean
make -f norimgtool.mak -e %1
goto END
:all
make -f norimgtool.mak -e norimgtool
goto END
:build_norimgtool
make -f norimgtool.mak -e %1
goto END
:BAILOUT
@echo on
@echo USAGE: build all/norimgtool/clean
:END
