@echo off
REM ======================================================
rem Sets path for RVDS221 Patch Level 593, GNU and PERL

set CRMPERL=C:\CRMApps\apps\Perl\bin


call setupecloud
set EMAKE_ROOT=../../../../..

set ARMTOOLS=RVCT221

set ROOT_DRIVE=C:
set APPS_DIR=Apps
set APPS_PATH=%ROOT_DRIVE%\%APPS_DIR%

set ARMROOT=%APPS_PATH%\RVDS221
set UTILROOT=%ROOT_DRIVE%\utils

set GNUPATH=%UTILROOT%\cygwin\bin
set PERLPATH=C:\perl\bin
set ARMPATH=%ARMROOT%\RVCT\Programs\2.2\593\win_32-pentium

rem
rem ARM Include and Path Variables
rem
set ARMLIB=%ARMROOT%\RVCT\Data\2.2\503\lib
set ARMINCLUDE=%ARMROOT%\RVCT\Data\2.2\503\include\windows
set ARMINC=%ARMROOT%\RVCT\Data\2.2\503\include\windows

set ARMBIN=%ARMROOT%\RVCT\Programs\2.2\593\win_32-pentium
set ARMBIN_ARMSD=%ARMROOT%\RDI\armsd\1.3.1\66\win_32-pentium\armsd.exe
set ARMBIN_AXD=%ARMROOT%\RDI\AXD\1.3.1\98\win_32-pentium\axd.exe
set ARMBIN_PRJ2XML=%ARMROOT%\IDEs\CodeWarrior\RVPlugins\1.0\155\win_32-pentium\converters\RVD\prj2xml.exe
set ARMCONF=%ARMROOT%\RDI\armperip\1.3\50
set ARMDLL=%ARMROOT%\RDI\rdimsvr\1.3.1\91\win_32-pentium;%ARMROOT%\RDI\Targets\Remote_A\1.3.1\98\win_32-pentium
set ARMHOME=%ARMROOT%

rem
rem Path for RVCT, GNU, Perl tools
rem
set path=%ECMAKE%;%PERLPATH%;%CRMPERL%;%GNUPATH%;%ARMPATH%;%UTILROOT%;%path%
REM ======================================================
