/*===========================================================================
                              OEMWSMSCust.c

  OEM REFERENCE SMS SUPPORT ROUTINES for CUSTOM DEVELOPMENT BY OEMs
  
  This file contains misc stub functions that OEMs can customize for
  SMS support.
  
  This file interfaces with WMS services of DMSS.
    
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
//#include "AEEStdLib.h"

#include "OEMSMS_priv.h"


void OEMSMSCust_LoopbackMsg ( SMSCacheEntry * pce )
{
      // Logic to be filled in by OEMs if you want to
      // use this as a hook to tap into loopback SMS messages
      // if your main messaging application is not written 
      // using ISMS.  See SMSCacheEntry for more information
      // about the fields of the message and you can call
      // your native app and deliver message using this function.
      //
      // Most probably you will be converting between the 
      // cache entry and your native messaging structure so this
      // file can be used to do the custom development without
      // cluttering OEMWSMS.c which has most of the reference
      // code provided by Qualcomm.
      SMSCacheEntry * pMyCacheEntry = pce;

	  // Dummy usage...
      (void) pMyCacheEntry;
	  (void) gSMSCriticalSection;

      //FARF(SMS, ("OEMSMSCust_LoopbackMsg"));
}
