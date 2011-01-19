/*============================================================================
FILE: QCamDiag.h
                     
GENERAL DESCRIPTION
    Automation test shared function prototies and definitions.  

    Copyright (c) 2004 - 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
 ============================================================================*/
 
 /*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qcamcommon/main/latest/inc/QcamDiag.h#1 $ $DateTime: 2008/05/12 10:09:09 $ $Author: karenp $

 when      who    what, where, why
--------  -----  ----------------------------------------------------------
 11/09/07  NC     Added support to FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE.
 09/05/07  NC     Fixed some compiling issues with feature condition change.
 08/17/07  KV     Added New Event EVT_QCAMERA_DIAG_SELECT_MULTISHOT and 
                  added new Qcamera_MultishotStatusResType Structure.
 04/02/07  NC     Removed compiler warning due to IDIAG_OTHER_INT_ERROR out of int range. 
 11/15/06  SG     IDIAG Support for QMobicaT
 09/07/06  NC     Added new test events and definitions.
 02/08/06  NC     Added new error code for abnormal test termination. 
 10/24/05  NC     More lint errors clean up.
 10/13/05  NC     Added Qcamera_GenericReqType.
 09/28/05  NC     Added new struct for MemUsage info
 07/29/05  NC     Added new evt for Add Frame and Shutter Sound
 07/20/05  NC     Added new view type QALBUM_DIAG_ADV_EDIT_VIEW.
 06/20/05  NC     Define new event and error code for Advanced edit.
 04/29/05  NC     Added new Album contexts to Qcamera_DiagContextType struct.
 04/15/05  NC     Added EVT_QCAMERA_DIAG_USER_KEY for simulating key event.
 04/13/05  NC     Added new error ids.
 02/16/05  NC     Added #ifdef FEATURE_APP_QCAMERA condition.
 12/01/04  NC     Init file for DIAG helper functions
 ============================================================================*/
#ifndef QCAMDIAG_H
#define QCAMDIAG_H

#ifndef AEE_SIMULATOR
#include "comdef.h"
#endif

#ifdef FEATURE_APP_QCAMERA
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
#endif //#ifdef FEATURE_APP_QCAMERA
#endif //#ifndef QCAMDIAG_H
