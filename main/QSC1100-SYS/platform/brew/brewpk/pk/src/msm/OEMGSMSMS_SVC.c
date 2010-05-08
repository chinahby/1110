/*===========================================================================

                             OEMGSMSMS_SVC.c     
                

GENERAL DESCRIPTION
   This file contains functions used by GSM1SMSSVC to access the SMS cache.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   BREW must be initialized.

Copyright © 2003-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* Then include OEM headers.*/
#include "OEMFeatures.h"

/* The whole file depends on the following include.*/                         
#if defined(FEATURE_GSM1x)

#include "OEMGSMSMS_SVC.h"
//#include "OEMCriticalSection.h"

/* Then include AEE headers.*/
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "AEEStdLib.h"


/*===========================================================================

                      CONSTANTS and ENUMs

===========================================================================*/

#ifdef GSMSMS_UNITTEST
    #define STATIC 
    #define FARF_ERROR    0
    #define FARF_HIGH     0
    #define FARF_MED      0
    #define FARF_LOW      0
    #define FARF_UNITTEST 0
#else
    #define STATIC static
    #define FARF_ERROR    0
    #define FARF_HIGH     0
    #define FARF_MED      0
    #define FARF_LOW      0
    #define FARF_UNITTEST 0
#endif

#ifdef FARF
    #undef FARF
#endif /* #ifdef FARF */

#if defined(AEE_SIMULATOR)
    #define FARF_ALWAYS  1
#else
    #define FARF_ALWAYS  0
#endif

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p 

#define ASSERT_FARF(x) if (!(x)) { FARF(ALWAYS,("assertion %s failed, "__FILE__":%d", #x, __LINE__)); }

/*===========================================================================

                      CONSTANTS

===========================================================================*/

#define MAX_SMS_CACHE  5

/*===========================================================================

                      TYPES 

===========================================================================*/

enum {
   CES_FREE        = 0,
   CES_PARSING     = 1,
   CES_READY       = 2,
   CES_DISPATCHING = 3
};

/*===========================================================================

                      EXTERNAL FUNCTIONS

===========================================================================*/

extern void GSMSMS_ProcessCacheNotification(void);

/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/

typedef struct
{
   uint16   wStatus;
   byte     bLen;
   GSMSMSMessage clientMsg;
} GSMSMSCacheEntry;


/*===========================================================================

                      PUBLIC METHOD DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PROTECTED METHOD DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PRIVATE METHOD DECLARATIONS

===========================================================================*/

/*===========================================================================

                      LOCAL/STATIC DATA

===========================================================================*/

static GSMSMSCacheEntry   gSMSCache[MAX_SMS_CACHE] = {0};

static boolean            gbInitialized = FALSE;

//static OEMCriticalSection gGSMSMSCriticalSection;


/*===========================================================================

                     Public Methods

===========================================================================*/
void OEMGSMSMS_SVC_Init()
{
   int i;

   // Init the cache to invalid values...
   for (i = 0; i < MAX_SMS_CACHE; i++)
   {
      gSMSCache[i].wStatus = CES_FREE;
   }
   gbInitialized = TRUE;
}


/*=============================================================================
FUNCTION: OEMGSMSMS_SVC_GetSMSMessage 

DESCRIPTION:
     Get an SMS message from the cache.
        
PARAMETERS:
  message : pointer to a buffer that will receive the extracted message buffer
  len     : pointer to the size in bytes of the extracted message buffer 

RETURN VALUE:
  TRUE if a message was extracted, FALSE if there are no more SMS message

SIDE EFFECTS:
    None
=============================================================================*/
boolean OEMGSMSMS_SVC_GetSMSMessage(GSMSMSMessage* message, byte* length)
{
    GSMSMSCacheEntry        * pce;
    int                       i;

    if ( (!gbInitialized) || (message == NULL) || (length == NULL) )
    {
       return FALSE;
    }

    //OEMCriticalSection_Enter(&gGSMSMSCriticalSection);

    for (pce = gSMSCache, i = 0; i < MAX_SMS_CACHE; i++, pce++)
    {
        if (pce->wStatus == CES_READY){
            pce->wStatus = CES_DISPATCHING;
            break;
        }
    }

    //OEMCriticalSection_Leave(&gGSMSMSCriticalSection);

    if (i < MAX_SMS_CACHE)
    {
        // copy data from cache
        COPY_TO_USER(length, &pce->bLen, sizeof(*length));        
        COPY_TO_USER((void *)message, pce->clientMsg, pce->bLen);

        pce->wStatus = CES_FREE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*=============================================================================
FUNCTION: OEMGSMSMS_SVC_ProcessMessage 

DESCRIPTION:
     Process an incoming GSM1x message. This is the entry point from IPhone.
     This method is only called from the WMS thread context.
        
PARAMETERS:
  bearerData             : IS-637 message in uasms format
  len                    : message length

RETURN VALUE:
  none

SIDE EFFECTS:
    None
=============================================================================*/
void OEMGSMSMS_SVC_ProcessMessage(const byte *bearerData, const uint16 len)
{
    uint16               i;
    GSMSMSCacheEntry   * pce;

    if (!gbInitialized) return;

    FARF(UNITTEST, ("Called OEMGSMSMS_SVC_ProcessMessage"));

    if (len > GSM1xMESSAGE_SIGNALING_MAX) 
    {
       FARF(HIGH, ("IGSMSMS: Msg of wrong size passed!"));
       return;
    }
    
    //OEMCriticalSection_Enter(&gGSMSMSCriticalSection);

    for(pce = gSMSCache, i = 0; i < MAX_SMS_CACHE; i++, pce++)
    {
       if(pce->wStatus == CES_FREE)
       {
          pce->wStatus = CES_PARSING;
          break;
       }
    }

    //OEMCriticalSection_Leave(&gGSMSMSCriticalSection);

    if (i >= MAX_SMS_CACHE)
    {
       FARF(HIGH, ("SMS: No Cache available"));

       // TODO Send a RP_ERROR message indicating MS is BUSY
       return;
    }

    FARF(LOW,("SMS (Decoding)"));
    FARF(LOW,("SMS Len: %d ", len));

    MEMSET((void *)pce,0,sizeof(*pce));
    COPY_FROM_USER(pce->clientMsg, bearerData, len);

#if GSM1xMESSAGE_SIGNALING_MAX > 255
    #error wrong integral conversion below
#endif 

    pce->bLen   = (uint8)len;

    pce->wStatus = CES_READY;
    GSMSMS_ProcessCacheNotification();
}


#endif /* FEATURE_GSM1x*/

