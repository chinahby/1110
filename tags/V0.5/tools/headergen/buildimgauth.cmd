@echo off
if "%1"=="" GOTO BAILOUT
if "%1"=="clean" GOTO clean
if "%1"=="all" GOTO build_image_auth
goto BAILOUT
:clean
make -f image_auth.mak -e %1
goto END
:all
make -f image_auth.mak -e image_auth
goto END
:build_image_auth
make -f image_auth.mak image_auth
goto END
:BAILOUT
@echo on
@echo USAGE: build all/image_auth/clean
:END
