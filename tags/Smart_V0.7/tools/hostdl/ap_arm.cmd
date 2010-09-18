@echo off
REM
REM

REM===========================================================================
REM   File to build ARMPRG for MSM.  Type "ap_arm" at command line
REM   and any arguments (like "clean" or "depend") 
REM
REM
REM                           EDIT HISTORY FOR FILE
REM  This section contains comments describing changes made to the module.
REM  Notice that changes are listed in reverse chronological order.
REM
REM  $Header: //depot/asic/msmshared/tools/hostdl/HOSTDL.14.00/common/ap_arm.cmd#1 $ $DateTime: 2007/03/05 17:31:24 $ $Author: rtilak $
REM  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/common/ap_arm.cmd#1 
REM   
REM when       who     what, where, why
REM --------   ---     -------------------------------------------------------
REM 07/21/05   drh     Pass all command line parameters
REM 04/28/02   drh     Removed use of other directory
REM 12/20/01   drh     New command file to make ARMPRG
REM
REM
REM  Copyright (c) 1998,1999, 2000, 2001 by QUALCOMM Incorporated. 
REM                All Rights Reserved.
REM===========================================================================


if exist buildflasharm.log del /f /q buildflasharm.old.log && move buildflasharm.log buildflasharm.old.log > nul

make -f ap_arm.mak %* 2>&1 | perl tee.pl buildflasharm.log

