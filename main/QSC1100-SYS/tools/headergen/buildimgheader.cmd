@echo off

if exist image_header.exe rm image_header.exe
if "%1"=="" goto USAGE

make -e SECBOOT=yes SRCROOT=../.. DEFAULT_CFG_DATA_DIR=%1 -f image_header.mak
echo ------------------------------------------------------------
echo image_header.exe is Generated.
echo ------------------------------------------------------------
goto end

:USAGE
echo ------------------------------------------------------------
echo Usage   --  buildimgheader.cmd PARTITION_TABLE_DIR
echo Example --  buildimgheader.cmd MCCBA
echo ------------------------------------------------------------

:end
