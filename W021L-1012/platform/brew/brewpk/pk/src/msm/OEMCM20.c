/*=============================================================================

FILE: OEMCM20.c

SERVICES: OEM Functions for Call Manager 2.0

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2003-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/

#include "OEMFeatures.h"
#ifndef AEE_STATIC
#define AEE_STATIC
#endif
#include "AEEStdLib.h"
#include "OEMTelephone.h"
#include "AEEModel.h"

/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/

struct _OEMTelephone{
   uint32               dwRefs;
   ModelListener        *pListeners;
};

static OEMTelephone *gpTelephone = NULL;   /* Single instance */
void OEMTelephone_Notify( AEETNotifyInfo *pData );


void OEMTelephone_Notify( AEETNotifyInfo *pNotify )
{
}

#define MODEL_CancelListener( model_type, pl )              \
{                                                           \
   model_type *pme = (model_type *)pl->pCancelData;         \
                                                            \
   if( pme->pListeners == pl ) {                            \
      pme->pListeners = pl->pNext;                          \
   }                                                        \
   else {                                                   \
      pl->pPrev->pNext = pl->pNext;                         \
   }                                                        \
                                                            \
   if( pl->pNext ) {                                        \
      pl->pNext->pPrev = pl->pPrev;                         \
   }                                                        \
                                                            \
   pl->pfnCancel     = 0;                                   \
   pl->pCancelData   = 0;                                   \
}

#define MODEL_AddListener( model_type, pmodel, pl )         \
{                                                           \
   if( 0 == pl ) {                                          \
      return;                                               \
   }                                                        \
                                                            \
   LISTENER_Cancel( pl );                                   \
                                                            \
   pl->pfnCancel     = model_type##_CancelListener;         \
   pl->pCancelData   = pme;                                 \
   pl->pPrev         = 0;                                   \
   pl->pNext         = pme->pListeners;                     \
   pme->pListeners   = pl;                                  \
                                                            \
   if( pl->pNext ) {                                        \
      pl->pNext->pPrev = pl;                                \
   }                                                        \
}

void OEMTelephone_CancelListener( ModelListener *pl )
{
   MODEL_CancelListener( OEMTelephone, pl )
}

void OEMTelephone_AddListener( OEMTelephone *pme, ModelListener *pl )
{
   MODEL_AddListener( OEMTelephone, pme, pl )
}

int OEMTelephone_Open( OEMTelephone **ppfn )
{
   OEMTelephone *pme = gpTelephone;
   
   if( pme ) {
      pme->dwRefs++;
      *ppfn = pme;
      return SUCCESS;
   }

   pme = MALLOC( sizeof(OEMTelephone) );
   if( !pme ) {
      return ENOMEMORY;
   }

   pme->dwRefs++;
   gpTelephone = *ppfn = pme;

   return SUCCESS;
}

int OEMTelephone_Close( OEMTelephone *pme )
{
   if( 0 == --pme->dwRefs ) {
      gpTelephone = NULL;

      FREE( pme );
   }
   return SUCCESS;
}

boolean OEMTelephone_IsModeCDMA( AEETSysMode theMode )
{
   return TRUE;
}

boolean OEMTelephone_IsModeGW( AEETSysMode theMode )
{
   return FALSE;
}

boolean OEMTelephone_IsCallTypeVoice( AEETCallType callType )
{
   return TRUE;
}

AEETOprtMode OEMTelephone_GetOperatingMode( OEMTelephone *pme )
{
   return AEET_OPRT_MODE_NONE;
}

AEETSysMode OEMTelephone_GetSystemMode( OEMTelephone *pme )
{
   return AEET_SYS_MODE_NONE;
}

int OEMTelephone_EndAllCalls( OEMTelephone *pme )
{
   return EUNSUPPORTED;
}

int OEMTelephone_Originate( OEMTelephone *pme, AEETCallType ct, IWebOpts *pOpts, int *pncd )
{
   return EUNSUPPORTED;
}

int OEMTelephone_SendFlash( OEMTelephone *pme, int cd, const char *pszNumber )
{
   return EUNSUPPORTED;
}

int OEMTelephone_Answer( OEMTelephone *pme, int cd )
{
   return EUNSUPPORTED;
}

int OEMTelephone_End( OEMTelephone *pme, int cd )
{
   return EUNSUPPORTED;
}

int OEMTelephone_Reject( OEMTelephone *pme, int cd )
{
   return EUNSUPPORTED;
}

int OEMTelephone_Transfer( OEMTelephone *pme, int cd1, int cd2 )
{
   return EUNSUPPORTED;
}

int OEMTelephone_Redirect( OEMTelephone *pme, int cd, const char *pszNubmer )
{
   return EUNSUPPORTED;
}

int OEMTelephone_Activate( OEMTelephone *pme, int cd )
{
   return EUNSUPPORTED;
}

int OEMTelephone_Hold( OEMTelephone *pme, int cd )
{
   return EUNSUPPORTED;
}

int OEMTelephone_GetCallInfo( OEMTelephone *pme, int cd, AEETCallInfo *pInfo, uint32 dwSize )
{
   return AEET_ERR_BADDESC;
}

int OEMTelephone_GetPhoneInfo( OEMTelephone *pme, AEETPhInfo *pInfo, uint32 dwSize )
{
   return EUNSUPPORTED;
}

int OEMTelephone_GetServingSystemInfo( OEMTelephone *pme, AEETSSInfo *pInfo, uint32 dwSize )
{
   return EUNSUPPORTED;
}

int OEMTelephone_SetOperatingMode( OEMTelephone *pme, AEETOprtMode mode )
{
   return EUNSUPPORTED;
}

int OEMTelephone_SetNamSel( OEMTelephone *pme, AEETNAM nam )
{
   return EUNSUPPORTED;
}

int OEMTelephone_SetRSSIDeltaThreshold( OEMTelephone *pme, byte thr )
{
   return EUNSUPPORTED;
}

int OEMTelephone_WakeupFromStandby( OEMTelephone *pme )
{
   return EUNSUPPORTED;
}

int OEMTelephone_SetSubscriptionStatus( OEMTelephone *pme, AEETSysMode mode, boolean status )
{
   return EUNSUPPORTED;
}

int OEMTelephone_BurstDTMF( OEMTelephone *pme, int cd, AEETDTMFOnLength on, AEETDTMFOffLength off, const char *pszDigits )
{
   return EUNSUPPORTED;
}

int OEMTelephone_StartDTMF( OEMTelephone *pme, int cd, char bDigit )
{
   return EUNSUPPORTED;
}

int OEMTelephone_StopDTMF( OEMTelephone *pme, int cd )
{
   return EUNSUPPORTED;
}

int OEMTelephone_Command( OEMTelephone *pme, uint32 dwCmd, AEECallback *pcb, void *pInBuffer, 
                          uint32 dwInBufSize, void *pOutBuffer, uint32 dwOutBufSize )
{
   return EUNSUPPORTED;
}

int OEMTelephone_SuppsCallOp( OEMTelephone *pme, const char *pszString )
{
   return EUNSUPPORTED;
}

int OEMTelephone_IsSuppsCallOp( OEMTelephone *pme, const char *pszString, uint32 *pnum )
{
   return EUNSUPPORTED;
}

int OEMTelephone_GetSuppsTrans( OEMTelephone *pme, AEETSuppsTransId tid, ISuppsTrans **ppTrans )
{
   return EUNSUPPORTED;
}

/* lint -restore */
