/*======================================================
FILE:  AEEPhone.c

SERVICES:  AEE Phone Interface

GENERAL DESCRIPTION:
   AEE Phone Interfaces

PUBLIC CLASSES AND STATIC FUNCTIONS:
   List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

        Copyright © 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdlib.h>
#include "customer.h"
#include "cm.h"
#include "AEEStdLib.h"
#include "AEETAPI.h"
#include "OEMHeap.h"
#include "OEMFeatures.h"
#include "OEMConfig.h"
#include "msm/AEEPhone.h"
#include "msm/OEMPhone.h"
#include "OEMTAPI.h"

#if defined(FEATURE_BREW_TAPI)

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

typedef struct _AEEPhone   AEEPhone;

struct _AEEPhone
{
   const IPhoneVtbl              *pvt;

   uint32                        m_nRefs;
   OEMPhone                      *pOEMPhone;
   void                          *pTapi;

   cm_ph_info_s_type             PhoneInfo;
   CM_CALL_INFO_S_TYPE           CallInfo;
   CM_SS_INFO_S_TYPE             SSInfo;
};

//
// Method declarations...
//

int                                  AEEPhone_New(IShell *piShell, AEECLSID clsid, void **ppif);
static void                          AEEPhone_Delete( AEEPhone *pme );
static uint32                        AEEPhone_AddRef(IPhone * po) ;
static uint32                        AEEPhone_Release(IPhone * po) ;
static void                          AEEPhone_SetMask(IPhone * po, uint32 * pdwMask) ;
static const cm_ph_info_s_type *     AEEPhone_GetPhoneInfo(IPhone * po) ;
static const CM_CALL_INFO_S_TYPE *   AEEPhone_GetCallInfo(IPhone * po) ;
static const CM_SS_INFO_S_TYPE *     AEEPhone_GetSystemInfo(IPhone * po) ;
static int                           AEEPhone_GetMinWStr(IPhone * po, int nNAM, AECHAR * pDest, int nSize) ;
static int                           AEEPhone_GetMinStr(IPhone * po, int nNAM, char * pszDest, int nSize) ;
static int                           AEEPhone_SetRSSIThreshold(IPhone * po, uint8 nDelta) ;
static int                           AEEPhone_MakeCall(IPhone *po, cm_call_type_e_type cType, cm_srv_type_e_type nServ, 
                                                       word nServOpt, const byte * pNum, byte nNumLen, 
                                                       cm_digit_mode_e_type nDigitMode, const byte * pAlpha, byte nAlphaLen, 
                                                       CM_ACTIVATE_CODE_TYPE nActivateCode) ;
static int                           AEEPhone_AnswerCall(IPhone * po,cm_call_type_e_type ct) ;
static int                           AEEPhone_AnswerVoice(IPhone * po, cm_answer_voice_e_type nAnswer, dword dwDuration) ;
static int                           AEEPhone_EndCall(IPhone * po) ;
static int                           AEEPhone_Flash(IPhone * po,const byte* pNum, byte nNumLen) ;
static int                           AEEPhone_SetPrefMode(IPhone * po, cm_mode_pref_e_type nMode, 
                                                            CM_MODE_TERM_E_TYPE nModeTerm, uint32 dwDuration);
static int                           AEEPhone_SetNAM(IPhone * po, cm_nam_e_type nNam) ;
static int                           AEEPhone_SetOprtMode(IPhone * po, CM_OPRT_MODE_E_TYPE nMode) ;

static int                           AEEPhone_BurstDTMF(IPhone * po, uint8 nOn, uint8 nOff, const uint8 * pDigits, uint8 nLen) ;
static int                           AEEPhone_StartDTMF(IPhone * po, uint8 nDigit) ;
static int                           AEEPhone_StopDTMF(IPhone * po, uint16 nDuration) ;
static int                           AEEPhone_OnCallStatus(IPhone * po,PFNNOTIFY pfn,void * pUser, uint32 dwDelay,uint16 wFlags) ;
static boolean                       AEEPhone_IsVoiceCall(IPhone * po) ;
static boolean                       AEEPhone_IsDataSupported(IPhone * po) ;

static AEEPhone *   gpPhone = NULL;

static const VTBL(IPhone)   gAEEPhoneFuncs = { AEEPhone_AddRef,
                                               AEEPhone_Release,
                                               AEEPhone_SetMask,
                                               AEEPhone_GetPhoneInfo,
                                               AEEPhone_GetCallInfo,
                                               AEEPhone_GetSystemInfo,
                                               AEEPhone_GetMinWStr,
                                               AEEPhone_GetMinStr,
                                               AEEPhone_SetRSSIThreshold,
                                               AEEPhone_MakeCall,
                                               AEEPhone_AnswerCall,
                                               AEEPhone_AnswerVoice,
                                               AEEPhone_EndCall,
                                               AEEPhone_Flash,
                                               AEEPhone_SetPrefMode,
                                               AEEPhone_SetNAM,
                                               AEEPhone_SetOprtMode,
                                               AEEPhone_BurstDTMF,
                                               AEEPhone_StartDTMF,
                                               AEEPhone_StopDTMF,
                                               AEEPhone_OnCallStatus,
                                               AEEPhone_IsVoiceCall,
                                               AEEPhone_IsDataSupported };


/*===========================================================================
//
//
//   PHONE INTERFACE
//
//
===========================================================================*/
/*===========================================================================

===========================================================================*/
int AEEPhone_New(IShell *piShell, AEECLSID clsid, void **ppif)
{
   int nErr = SUCCESS;
   *ppif = 0;

   if (gpPhone) {

      ++gpPhone->m_nRefs;
      *ppif = gpPhone;

   } else {

      gpPhone = (AEEPhone *)sys_malloc( sizeof(AEEPhone) );

      if( (AEEPhone *)0 == gpPhone ) {
         
         return ENOMEMORY;
      }

      ZEROAT( gpPhone );

      if( SUCCESS == (nErr = OEMPhone_New( &gpPhone->pOEMPhone ))
          && SUCCESS == (nErr = ISHELL_CreateInstance( piShell, AEECLSID_TAPI, &gpPhone->pTapi )) ) {

         gpPhone->pvt = &gAEEPhoneFuncs;
         gpPhone->m_nRefs = 1;
         *ppif = gpPhone;
      }
      else {

         AEEPhone_Delete( gpPhone );
      }
   }

   return nErr;
}

/*===========================================================================

===========================================================================*/
static void AEEPhone_Delete( AEEPhone *pme )
{
   if( pme->pOEMPhone ) {
      
      OEMPhone_Release( pme->pOEMPhone );
   }
   if( pme->pTapi ) {

      ITAPI_Release( pme->pTapi );
   }

   if( pme == gpPhone ) {

      gpPhone = NULL;
   }

   sys_free( pme );
}

/*===========================================================================

===========================================================================*/
static uint32 AEEPhone_AddRef(IPhone * po)
{
   return ++((AEEPhone *)po)->m_nRefs;
}

/*===========================================================================

===========================================================================*/
static uint32 AEEPhone_Release(IPhone * po)
{
   AEEPhone *  pme = (AEEPhone *)po;

   if(pme->m_nRefs){

      if(--pme->m_nRefs)
         return pme->m_nRefs;

      AEEPhone_Delete( pme );
   }
   return 0;
}

/*===========================================================================

===========================================================================*/
static void AEEPhone_SetMask(IPhone * po, uint32 * pdwMask)
{
   AEEPhone * pme = (AEEPhone *)po;
   uint32   dwNotify = 0;

   if( pdwMask ) {
      
      /* pdwMask is an array of bit masks, merge them in */
      while(*pdwMask != 0){

         dwNotify |= GET_NOTIFIER_MASK(*pdwMask);
         pdwMask++;
      }
      
      OEMPhone_SetMask( pme->pOEMPhone,  dwNotify );
   }
}


/*===========================================================================

===========================================================================*/
static const cm_ph_info_s_type * AEEPhone_GetPhoneInfo(IPhone * po )
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_GetPhoneInfo2( pme->pOEMPhone, &pme->PhoneInfo ) 
   ? &pme->PhoneInfo : (const cm_ph_info_s_type *)0;
}

/*===========================================================================

===========================================================================*/
static const CM_CALL_INFO_S_TYPE * AEEPhone_GetCallInfo(IPhone * po)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_GetCallInfo2( pme->pOEMPhone, &pme->CallInfo ) 
   ? &pme->CallInfo : (const CM_CALL_INFO_S_TYPE *)0;
}

/*===========================================================================

===========================================================================*/
static const CM_SS_INFO_S_TYPE * AEEPhone_GetSystemInfo(IPhone * po)
{
   AEEPhone *pme = (AEEPhone *)po;

   return  OEMPhone_GetSystemInfo2( pme->pOEMPhone, &pme->SSInfo ) 
   ? &pme->SSInfo : (const CM_SS_INFO_S_TYPE *)0;
}

/*===========================================================================

===========================================================================*/
static int AEEPhone_GetMinWStr(IPhone * po, int nNAM, AECHAR * pDest, int nSize)
{
   AEEMobileInfo  mi;

   if( (AECHAR *)0 == pDest || 0 == nSize ) return 0;

   OEM_GetConfig(CFGI_MOBILEINFO, &mi, sizeof(AEEMobileInfo));
   STRTOWSTR(mi.szMobileID, pDest, nSize);

   return WSTRLEN(pDest);
}

/*===========================================================================

===========================================================================*/
static int AEEPhone_GetMinStr(IPhone * po, int nNAM, char * pszDest, int nSize)
{
   AEEMobileInfo  mi;

   if( (char *)0 == pszDest || 0 == nSize ) return 0;

   OEM_GetConfig(CFGI_MOBILEINFO, &mi, sizeof(AEEMobileInfo));
   STRNCPY(pszDest,mi.szMobileID, nSize);
   pszDest[nSize - 1] = 0;
   return STRLEN(pszDest);
}

/*===========================================================================

===========================================================================*/
static int AEEPhone_SetRSSIThreshold(IPhone * po, uint8 nDelta)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_SetRSSIThreshold( pme->pOEMPhone, nDelta );
}

/*===========================================================================

===========================================================================*/
static int AEEPhone_MakeCall(IPhone *po,cm_call_type_e_type cType,
                                        cm_srv_type_e_type nServ,
                                        word nServOpt,
                                        const byte * pNum,
                                        byte nNumLen,
                                        cm_digit_mode_e_type nDigitMode,
                                        const byte * pAlpha,
                                        byte nAlphaLen,
                                        CM_ACTIVATE_CODE_TYPE nActivateCode)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_MakeCall( pme->pOEMPhone, cType, nServ, nServOpt, pNum, nNumLen, nDigitMode, pAlpha,
                             nAlphaLen, nActivateCode );
}

/*===========================================================================

===========================================================================*/
static int AEEPhone_AnswerCall(IPhone * po,cm_call_type_e_type ct)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_AnswerCall( pme->pOEMPhone, ct );
}

/*===========================================================================

===========================================================================*/
static int AEEPhone_AnswerVoice(IPhone * po, cm_answer_voice_e_type nAnswer, dword dwDuration)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_AnswerVoice( pme->pOEMPhone, nAnswer, dwDuration );
}

/*===========================================================================

===========================================================================*/
static int AEEPhone_EndCall(IPhone * po)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_EndCall( pme->pOEMPhone );
}

/*===========================================================================
            
===========================================================================*/
static int AEEPhone_Flash(IPhone * po,const byte* pNum, byte nNumLen)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_Flash( pme->pOEMPhone, pNum, nNumLen );
}

/*===========================================================================

===========================================================================*/
static int AEEPhone_SetPrefMode(IPhone * po, cm_mode_pref_e_type nMode,
                                CM_MODE_TERM_E_TYPE nModeTerm,uint32 dwDuration)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_SetPrefMode( pme->pOEMPhone, nMode, nModeTerm, dwDuration );
}

/*===========================================================================

===========================================================================*/
static int AEEPhone_SetNAM(IPhone * po, cm_nam_e_type nNam)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_SetNAM( pme->pOEMPhone, nNam );
}

/*===========================================================================

===========================================================================*/
int AEEPhone_SetOprtMode(IPhone * po, CM_OPRT_MODE_E_TYPE nMode)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_SetOprtMode( pme->pOEMPhone, nMode );
}

/*===========================================================================

===========================================================================*/
static int AEEPhone_BurstDTMF(IPhone * po, uint8 nOn, uint8 nOff, const uint8 * pDigits, uint8 nLen)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_BurstDTMF( pme->pOEMPhone, nOn, nOff, pDigits, nLen );
}

/*===========================================================================

===========================================================================*/
static int AEEPhone_StartDTMF(IPhone * po, uint8 nDigit)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_StartDTMF( pme->pOEMPhone, nDigit );
}

/*===========================================================================

===========================================================================*/
static int AEEPhone_StopDTMF(IPhone * po, uint16 nDuration)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_StopDTMF( pme->pOEMPhone, nDuration );
}

/*===========================================================================

===========================================================================*/
static int AEEPhone_OnCallStatus(IPhone * po,PFNNOTIFY pfn,void * pUser, 
                                 uint32 dwDelay, uint16 wFlags)
{
   AEEPhone *pme = (AEEPhone *)po;

   /* Use ITAPI_OnCallStatus instead. */
   return ITAPI_OnCallStatus( pme->pTapi, pfn, pUser, dwDelay, wFlags );
}

/*===========================================================================

===========================================================================*/
static boolean AEEPhone_IsVoiceCall(IPhone * po)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_IsVoiceCall( pme->pOEMPhone );
}

/*===========================================================================

===========================================================================*/
static boolean AEEPhone_IsDataSupported(IPhone * po)
{
   AEEPhone *pme = (AEEPhone *)po;

   return OEMPhone_IsDataSupported( pme->pOEMPhone );
}

#endif /* if defined(FEATURE_BREW_TAPI) */
