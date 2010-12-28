@echo off
if "%1"=="clean" GOTO clean
make -f norprogimgtool.mak -e all
goto END
:clean
make -f norprogimgtool.mak -e %1
goto END
:END
