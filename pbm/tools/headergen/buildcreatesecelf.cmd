@echo off
if "%1"=="" GOTO BAILOUT
if "%1"=="createsecelf" GOTO buildsecelf
if "%1"=="clean" GOTO clean
if "%1"=="all" GOTO all
goto BAILOUT
:clean
make -f createsecelf.mak -e %1
goto END
:all
make -f createsecelf.mak -e %1
goto END
:buildsecelf
make -f createsecelf.mak -e %1
goto END
:BAILOUT
@echo on
@echo USAGE: build all/createsecelf/clean
:END