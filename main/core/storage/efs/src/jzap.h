/*
  ========================================================================
  jzap.h:  ZAP JTAG Services external header file.
  ========================================================================

  This include file provides definitions external to the ZAP messaging.
  This file must be included by users of the ZAP services.

  Copyright (C) 2005-2006, 2008  QUALCOMM, Incorporated.
  All Rights Reserved.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/jzap.h#3 $
  ----------------------------------------------------------------------

                           Edit History

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/19/08    sh     Babylon references must be condition on feature
11/06/08    sh     Replaced JZAP calls with Babylon
03/28/08    eb     Removed all declarations. jzap.h now only a stub.
05/08/06    sh     My mistake.. (void)0 works fine.
05/07/06    sh     RVCT did not like (void)0.  Replaced with do-while(0)
02/16/06    sh     Added (void) to macro stubs to silence compiler warnings
10/27/05   dlb     Clean up header.
10/12/05    sh     Added to Aegis and EFS.

*/

#ifndef _JZAP_H_
#define _JZAP_H_

#include "fs_babylon.h"

#ifdef FEATURE_EFS_BABYLON

/* Defined in fs_util.c */
void jzap_bab_printf (const char *format, ...);

/* These are constant strings, so they can be handled directly */
#define ZAP(x)                  EFS_BAB_PRINT (EFS_BAB_ZAP, (x))
#define ZAPP(x)                 EFS_BAB_PRINT (EFS_BAB_ZAP, (x))
#define ZAPN(x)                 EFS_BAB_PRINT (EFS_BAB_ZAP, (x))
#define ZPRINTF                 jzap_bab_printf
#define ZMSG                    jzap_bab_printf

#else /* No Babylon to use */

#define ZAP(x)                  (void)0
#define ZAPP(x)                 (void)0
#define ZAPN(x)                 (void)0
#define ZPRINTF(...)            (void)0
#define ZMSG(...)               (void)0

#endif

/* These aren't ever supported */
#define ZAP_DISPLAY_DATA(a,b)   (void)0
#define ZMSG_3HEX(str,a1,a2,a3) (void)0
#define ZPOLL                   (void)0
#define ZDRAIN                  (void)0
#define ZTASK                   (void)0
#define ZAPNL                   (void)0
#define ZAPTICKS                (void)0
#define ZAP_TX(a,b,c,d)         (void)0

#endif /* _JZAP_H_ */
