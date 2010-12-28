@echo off
REM
REM

REM===========================================================================
REM
REM                           EDIT HISTORY FOR FILE
REM  This section contains comments describing changes made to the module.
REM  Notice that changes are listed in reverse chronological order.
REM
REM  $Header: //depot/asic/msmshared/tools/jflash/JFLASH.03.00/jflash.cmd#1 $ $DateTime: 2007/03/02 18:20:12 $ $Author: rtilak $
REM   
REM when       who     what, where, why
REM --------   ---     -------------------------------------------------------
REM 09/01/03   drh     Created from jf29bds640.cmd
REM
REM
REM  Copyright (c) 1998-2003 by QUALCOMM Incorporated. 
REM                All Rights Reserved.
REM===========================================================================

if exist buildjflash.log del /f /q buildjflash.old.log && move buildjflash.log buildjflash.old.log > nul

make -f jflash.mak %1 2>&1 | perl qtee.pl buildjflash.log
