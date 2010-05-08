#include "customer.h"
#ifdef  FEATURE_CAMERA_SVCS_DIAG
#ifndef CAMERA_SVCS_DIAG_H
#define CAMERA_SVCS_DIAG_H

/*****************************************************************************

  C A M E R A   S V C S   D I A G

  (camera_svcs_diag.h)

  defines the C-side of RPC (remote procedure calls)
  involving Perl on a PC connected over a serial line
  to AMSS C code running on a phone.

  This is a total rewrite of an earlier extension
  to ftm_camera.{c,h}, which plug into DIAG services
  (the phone-side RPC mechanism) differently than
  we do here.

     Copyright (c) 2005 by QUALCOMM INCORPORATED.
                   All Rights Reserved.

                EDIT HISTORY FOR FILE
          (in reverse chronological order)

  $Header: //source/qcom/qct/multimedia/camera/6k/diag/main/latest/inc/camera_svcs_diag.h#1 $
  $DateTime: 2008/05/09 16:40:03 $ 
  $Author: karenp $
 
  This #1 was //depot/asic/msm6550/services/camera/camera_svcs_diag.h#13 .
  This was under msmshared version 7 until June 28 2005.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/26/2007 vma     Replaced custcamera.h with customer.h
11/10/2005 jj      Stuck down in subsidiary directory by request.
07/22/2005 jj      Rewriting comments to QCT standards.
07/13/2005 jj      Simpler..only exposes the single function required for
                   other AMSS code's visibility.  Functions themselves to
                   be available via RPC are themselves registered by the
                   single function below.
07/07/2005 jj      Deriving from services/diag/toolsdiag.h per
                   Saritha Alluri's tip.
05/17/2005 jj      moving from FTM (75,11,15) to (75,22) Qcamera subsystem ID.

*****************************************************************************/


void  camera_svcs_diag_init ( void );


#endif  //  CAMERA_SVCS_DIAG_H
#endif  //  FEATURE_CAMERA_SVCS_DIAG
