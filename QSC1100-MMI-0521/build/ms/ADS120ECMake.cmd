@echo off
REM ======================================================
rem
rem Root of ARM ADS 1.2, GNUWIN32, and Perl
rem

call setupecloud
set CRMPERL=C:\CRMApps\apps\Perl\bin

set MAKEMODE=win32
set ROOT_DRIVE=C:
set APPS_DIR=Apps
set APPS_PATH=%ROOT_DRIVE%\%APPS_DIR%

set ARMROOT=%APPS_PATH%\Ads120
set PERLROOT=C:\Perl
set UTILROOT=C:\Utils
set MAKEROOT=%UTILROOT%\Cygwin

rem
rem ARM Include, Path and License Variables
rem
set LM_LICENSE_FILE=%ARMROOT%\LICENSES
set ARMCONF=%ARMROOT%\BIN
set ARMDLL=%ARMROOT%\BIN
set ARMHOME=%ARMROOT%
set ARM12HOME=%ARMROOT%
set ARMLIB=%ARMROOT%\Lib
set ARMINCLUDE=%ARMROOT%\Include
set ARMINC=%ARMROOT%\Include

rem
rem Path for ADS 1.2, GNUWIN32, Perl tools
rem
set path=%ECMAKE%;%PERLROOT%\Bin;%CRMPERL%;%MAKEROOT%\Bin;%ARMROOT%\Bin;%path%


REM ======================================================




