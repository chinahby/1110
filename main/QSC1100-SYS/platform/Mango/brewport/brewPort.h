/**============================================================================
 * $Rev: 8609 $
 * $Date: 2009/04/08 $
 * $Author: pramodk $
 * 
 * The contents of this file are governed by a license granted by Mango 
 * Technologies Pvt. Ltd. You may not use this file except in compliance with
 * that license.
 * 
 * COPYRIGHT (c) 2008,2009 Mango Technologies Pvt. Ltd.
 * All Rights Reserved.
 *=============================================================================
 */
#ifndef _brewport_h_
#define _brewport_h_

#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEStdlib.h"
#include "AEESms.h"
#include "AEEFile.h"
#include "AEETelephone.h"
#include "AEEDeviceNotifier.h"
#include "AEEError.h"


#include "brewPort.brh"
#include "brewPort.bid"


typedef struct _brewPort {
   AEEApplet      a ;         // First element of this structure must be AEEApplet
   AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
} brewPort;


#endif /* _brewport_h_ */
