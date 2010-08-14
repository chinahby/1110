/*=============================================================================

FILE: OEMIMM.c

SERVICES: OEM Functions Input Method Manager

GENERAL DESCRIPTION:
            Input method manager implementation

        Copyright 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/


#include "OEMFeatures.h"
#include "AEE_OEM.h"
#include "OEMIMM.h"
#include "AEEDisp.h"
#include "AEEStdLib.h"

//#define PROFILE_IMM_BUFFERS

#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
#include "zi8api.h"
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT

#ifdef FEATURE_ZICORP_EZITEXT // {
// Shift states for eZiText entry
// Only affects characters you handle in OEM_WStrUpper/OEM_WStrLower...
// nShiftState is the member below that uses these.
// It would tell whether to:
// good, Good, or GOOD when composing
#define EZISHIFT_SMALL     (0)
#define EZISHIFT_MIXED     (1)
#define EZISHIFT_CAPS      (2)
#endif // } FEATURE_ZICORP_EZITEXT

#define SIZE_OF_ELEMENT_BUF      (32)
#define SIZE_OF_CANDIDATE_BUF    (256)
#define SIZE_OF_WORD_BUF         (256)

#define DEFAULT_MAXCANDIDATE     (64)
#define DEFAULT_CHOICEBUFF_SIZE  (32*sizeof(AECHAR))

typedef struct _CIMMgr CIMMgr;

#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
OBJECT(ZiGPData){
   ZI8GETPARAM    ziGetParam;

   ZI8WCHAR       ziElementBuffer[SIZE_OF_ELEMENT_BUF];
   ZI8WCHAR       ziElementBackup[SIZE_OF_ELEMENT_BUF];
   ZI8WCHAR       ziCandidates[SIZE_OF_CANDIDATE_BUF];
   ZI8WCHAR       ziWordBuffer[SIZE_OF_WORD_BUF];
};
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT

struct _CIMMgr{
   AEEVTBL(IIMMgr) *    pvt;              // My functions!

   uint32               nRefs;            // My references
   AEECLSID             clsMe;            // My class Id
   IShell *             pIShell;          // Shell ptr
   AEETextInputMode     tmCurrent;        // Operating mode
   int                  nModeIdx;         // Enumerator
   AECHAR *             pszwWordChoice;   // Buffer to hold final choices
#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
   ZiGPData             ziCtx;            // Context for Zi
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT
   boolean              bInit;            // Initialised?
   uint16               nMaxCandidates;   // Number of maximum candidates as specified to SetOpt
   uint16               nChoiceSize;      // Size of the chosen words and assoc. words
   uint16               wContextOpt;      // Which context it is in
   uint16               wKeyOpt;          // How to match Keys
#ifdef FEATURE_ZICORP_EZITEXT // {
   uint32               nShiftState;      // Handle Word shifting
#endif // } FEATURE_ZICORP_EZITEXT
};

static const AEETextInputMode timModes[] =
{
   AEE_TM_NONE // 1 base this
#ifdef FEATURE_ZICORP_PINYIN // {
   , AEE_TM_PINYIN
#endif // } FEATURE_ZICORP_PINYIN
#ifdef FEATURE_ZICORP_STROKE // {
   , AEE_TM_STROKE
#endif // } FEATURE_ZICORP_STROKE
#ifdef FEATURE_ZICORP_EZITEXT // {
   , AEE_TM_EZTEXT
#endif // } FEATURE_ZICORP_EZITEXT
};

int                     OEMIMMgr_New(IShell * pIShell, AEECLSID clsID, void ** ppobj);

static uint32           OEMIMMgr_AddRef(IIMMgr * po);
static uint32           OEMIMMgr_Release(IIMMgr * po);
static int              OEMIMMgr_QueryInterface(IIMMgr * po, AEECLSID clsID, void ** ppobj);
static boolean          OEMIMMgr_HandleEvent(IIMMgr * po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static void             OEMIMMgr_Reset(IIMMgr * po);
static int              OEMIMMgr_Initialise(IIMMgr * po);
static int              OEMIMMgr_EnumModeInit(IIMMgr * po);
static int              OEMIMMgr_EnumNextMode(IIMMgr * po, AEETextInputMode * pmode);
static int              OEMIMMgr_SetInputMode(IIMMgr * po, AEETextInputMode mode);
static int              OEMIMMgr_GetInputMode(IIMMgr * po, AEETextInputMode * pmode);
static int              OEMIMMgr_SetOpt(IIMMgr * po, uint16 wOption, void * pData, uint32 nSize);
static int              OEMIMMgr_GetOpt(IIMMgr * po, uint16 wOption, void * pData, uint32 nSize);
static int              OEMIMMgr_GetCompositionCandidates(IIMMgr *po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars);
static int              OEMIMMgr_GetCompleteCandidates(IIMMgr *po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars);
static boolean          OEMIMMgr_IsCompleteCandidate(IIMMgr * po, const AECHAR * pBuff);
static int              OEMIMMgr_GetData(IIMMgr * po, AECHAR * pBuff, uint16 * pwBuffSize);
static int              OEMIMMgr_SetData(IIMMgr * po, const AECHAR * pBuff);
static int              OEMIMMgr_GetAssociatedCandidates(IIMMgr *po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars);
static int              OEMIMMgr_GetCompositionCount(IIMMgr * po, int * pnElements);

#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
static uint16           IMMgr_GetZi8Context(uint16 wContext);
static uint16           IMMgr_GetZi8KeyOpt(uint16 wKeyOpt);
#ifdef FEATURE_ZICORP_PINYIN // {
static uint16           IMMgr_PYNMapKeytoElement(AVKType key);
#endif // } FEATURE_ZICORP_SPINYIN
#ifdef FEATURE_ZICORP_STROKE // {
static uint16           IMMgr_STKMapKeytoElement(AVKType key);
#endif // } FEATURE_ZICORP_STROKE
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT
#ifdef FEATURE_ZICORP_EZITEXT // {
static uint16           IMMgr_EZIMapKeytoElement(AVKType key);
#endif // } FEATURE_ZICORP_EZITEXT

static const AEEVTBL(IIMMgr) gpvtIIMMgr = {  OEMIMMgr_AddRef,
                                             OEMIMMgr_Release,
                                             OEMIMMgr_QueryInterface,
                                             OEMIMMgr_HandleEvent,
                                             OEMIMMgr_Reset,
                                             OEMIMMgr_Initialise,
                                             OEMIMMgr_EnumModeInit,
                                             OEMIMMgr_EnumNextMode,
                                             OEMIMMgr_SetInputMode,
                                             OEMIMMgr_GetInputMode,
                                             OEMIMMgr_SetOpt,
                                             OEMIMMgr_GetOpt,
                                             OEMIMMgr_GetCompositionCandidates,
                                             OEMIMMgr_GetCompleteCandidates,
                                             OEMIMMgr_IsCompleteCandidate,
                                             OEMIMMgr_GetData,
                                             OEMIMMgr_SetData,
                                             OEMIMMgr_GetAssociatedCandidates,
                                             OEMIMMgr_GetCompositionCount };


int OEMIMMgr_New(IShell * pIShell, AEECLSID clsID, void ** ppobj)
{
   CIMMgr *          pme = NULL;

   *ppobj = NULL;

   switch( clsID ){
   case AEECLSID_OEMIMMGR:
      break;
   default:
      return EUNSUPPORTED;
   }

   
   if( (pme = (CIMMgr *)AEE_NewClassEx((IBaseVtbl*)&gpvtIIMMgr, sizeof(CIMMgr), FALSE)) == NULL ){
      return ENOMEMORY;
   }

   ISHELL_AddRef(pIShell);
   pme->pIShell         = pIShell;
   pme->clsMe           = clsID;
   pme->nRefs           = 1;
   pme->nChoiceSize     = DEFAULT_CHOICEBUFF_SIZE;
   pme->pszwWordChoice  = (AECHAR *)MALLOC(pme->nChoiceSize);

   OEMIMMgr_Reset((IIMMgr *)pme);

   *ppobj = (void *)pme;

   return AEE_SUCCESS;
}

static uint32 OEMIMMgr_AddRef(IIMMgr * po)
{
   return ++((CIMMgr *)po)->nRefs;
}
static uint32 OEMIMMgr_Release(IIMMgr * po)
{
   CIMMgr * pme = (CIMMgr *)po;

   if( --pme->nRefs ){
      return pme->nRefs;
   }
   if( pme->pIShell ){
      ISHELL_Release( pme->pIShell );
      pme->pIShell = NULL;
   }
   if( pme->pszwWordChoice ){
      FREE( pme->pszwWordChoice );
      pme->pszwWordChoice  = NULL;
   }
   FREE( (void *)pme );

   return 0;
}
static int OEMIMMgr_QueryInterface(IIMMgr * po, AEECLSID clsID, void ** ppobj)
{
   switch( clsID ){
   case AEECLSID_OEMIMMGR:
   case AEECLSID_QUERYINTERFACE:
      *ppobj = po;
      OEMIMMgr_AddRef(po);
      return AEE_SUCCESS;
   default:
      return EUNSUPPORTED;
   }
   return EFAILED;
}
static boolean OEMIMMgr_HandleEvent(IIMMgr * po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   CIMMgr * pme = (CIMMgr *)po;
#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
   ZiGPData *        pZi   = &pme->ziCtx;
   PZI8GETPARAM      pziGP = &pZi->ziGetParam;
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT

   if( !pme->bInit ){
      return FALSE;
   }

   if( eCode == EVT_KEY && ((wParam >= AVK_0 && wParam <= AVK_9) || wParam == AVK_CLR) ){
      switch( pme->tmCurrent ){
#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
#ifdef FEATURE_ZICORP_PINYIN // {
      case AEE_TM_PINYIN:
#endif // } FEATURE_ZICORP_PINYIN
#ifdef FEATURE_ZICORP_STROKE // {
      case AEE_TM_STROKE:
#endif // } FEATURE_ZICORP_STROKE
#ifdef FEATURE_ZICORP_EZITEXT // {
      case AEE_TM_EZTEXT:
#endif // } FEATURE_ZICORP_EZITEXT
         {
            int   i,
                  nMatches;

#ifdef FEATURE_ZICORP_EZITEXT // {
            // Handle the shift modifier
            if( pme->tmCurrent == AEE_TM_EZTEXT && wParam == (uint16)AVK_STAR ){
               if( pme->nShiftState == EZISHIFT_CAPS ){
                  pme->nShiftState = EZISHIFT_SMALL;
               }else{
                  pme->nShiftState++;
               }
               return TRUE;
            }
#endif // } FEATURE_ZICORP_EZITEXT

            // Count the typed elements
            for( i = 0; i < SIZE_OF_ELEMENT_BUF; i++ ){
               if( !pziGP->pElements[i] ){
                  break;
               }
            }
            // If they are really clearing a char, we handle it
            // If not, it isn't handled.
            if( wParam == AVK_CLR && i <= 1 ){
               if( i ){
                  pziGP->pElements[0] = 0;
                  return TRUE;
               }else{
                  return FALSE;
               }
            }

            pziGP->GetMode          = ZI8_GETMODE_DEFAULT;
            pziGP->GetOptions       = (ZI8UCHAR)IMMgr_GetZi8KeyOpt(pme->wKeyOpt);
            pziGP->MaxCandidates    = 1;
            pziGP->FirstCandidate   = 0;

            if( wParam == AVK_CLR ){
               // Erase the last editing character
               // -1 is guarded above, unless the choice
               // is made to allow the dialog to linger behind
               // even though there are no editing characters.
               pziGP->pElements[--i]   = 0;
            }else{
               if( (i+1) < SIZE_OF_ELEMENT_BUF ){
                  // We start with 0, they start with 1, so special case it.
                  if( pme->tmCurrent == AEE_TM_PINYIN ){
#ifdef FEATURE_ZICORP_PINYIN // {
                     pziGP->pElements[i++]   = IMMgr_PYNMapKeytoElement((AVKType)wParam);
#endif // } FEATURE_ZICORP_PINYIN
                  }else if( pme->tmCurrent == AEE_TM_STROKE ){
#ifdef FEATURE_ZICORP_STROKE // {
                     pziGP->pElements[i++]   = IMMgr_STKMapKeytoElement((AVKType)wParam);
#endif // } FEATURE_ZICORP_STROKE
                  }else if( pme->tmCurrent == AEE_TM_EZTEXT ){
#ifdef FEATURE_ZICORP_EZITEXT // {
                     pziGP->pElements[i++]   = IMMgr_EZIMapKeytoElement((AVKType)wParam);
#endif // } FEATURE_ZICORP_EZITEXT

                  }
                  // Key not handled!
                  if( !pziGP->pElements[i-1] ){
                     return FALSE;
                  }
               }
            }
            pziGP->ElementCount     = i;

            nMatches = Zi8GetCandidatesCount(pziGP);
            if( !nMatches ){
               // If no matches for this additional character
               // remove it.
               pziGP->pElements[i-1] = 0;
               return FALSE;
            }
            return TRUE;
         }
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT
      case AEE_TM_KANJI:
      case AEE_TM_HANKAKU_KANA:
      case AEE_TM_HANGUL:
      case AEE_TM_T9:
      default:
         return FALSE;
      }

   }
   return FALSE;
}
static void OEMIMMgr_Reset(IIMMgr * po)
{
   CIMMgr *          pme = (CIMMgr *)po;
#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
   ZiGPData *        pZi   = &pme->ziCtx;
   PZI8GETPARAM      pziGP = &pZi->ziGetParam;
   
   MEMSET((void *)pziGP, 0, sizeof(ZI8GETPARAM));
   MEMSET((void *)pZi->ziElementBuffer, 0, sizeof(pZi->ziElementBuffer));
   MEMSET((void *)pZi->ziElementBackup, 0, sizeof(pZi->ziElementBackup));
   MEMSET((void *)pZi->ziWordBuffer, 0, sizeof(pZi->ziWordBuffer));
   MEMSET((void *)pZi->ziCandidates, 0, sizeof(pZi->ziCandidates));
   pziGP->Language      = ZI8_LANG_ZH;
   // If you're system allows configuration of 
   // Taiwan or Hong Kong lookup mode, you should look
   // up the current setting and apply it here.
   pziGP->SubLanguage   = ZI8_SUBLANG_ZH;
   /*
   pziGP->SubLanguage   = ZI8_SUBLANG_ZH_TW;
   pziGP->SubLanguage   = ZI8_SUBLANG_ZH_HK;
   pziGP->SubLanguage   = ZI8_SUBLANG_ZH_ALL;
   */

   pziGP->pElements     = pZi->ziElementBuffer;
   pziGP->pCurrentWord  = pZi->ziWordBuffer;
   pziGP->pCandidates   = pZi->ziCandidates;
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT
   pme->wContextOpt     = IMMCTX_EMAIL;
   pme->wKeyOpt         = IMMKEY_MATCHONE;
   pme->bInit           = FALSE;
   pme->nMaxCandidates  = DEFAULT_MAXCANDIDATE;
   pme->tmCurrent       = AEE_TM_NONE;
#ifdef FEATURE_ZICORP_EZITEXT // {
   pme->nShiftState     = EZISHIFT_SMALL;
#endif // } FEATURE_ZICORP_EZITEXT


   OEMIMMgr_SetData(po, NULL);
}
static int OEMIMMgr_Initialise(IIMMgr * po)
{
   CIMMgr *          pme = (CIMMgr *)po;
#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
   if( !Zi8Initialize() ){
      return EFAILED;
   }
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT

   pme->bInit  = TRUE;

   return AEE_SUCCESS;
}
static int OEMIMMgr_EnumModeInit(IIMMgr * po)
{
   CIMMgr *          pme = (CIMMgr *)po;

   pme->nModeIdx = 0;
   return AEE_SUCCESS;
}
static int OEMIMMgr_EnumNextMode(IIMMgr * po, AEETextInputMode * pmode)
{
   CIMMgr *          pme = (CIMMgr *)po;

   if( !pmode ){
      return EBADPARM;
   }
   if( ARRAY_SIZE(timModes) == 1 ){
      return EFAILED;
   }
   if( ++pme->nModeIdx >= ARRAY_SIZE(timModes) ){
      return EFAILED;
   }
   *pmode = timModes[pme->nModeIdx];

   return AEE_SUCCESS;
}

// For AEE_TM_EZTEXT mode this function uses the Current device language. 
// AEE_TM_EZTEXT does not specify a language, so current dwLang of OEM_GetDeviceInfo() is used.
// 1. Ensure the language is supported by your Zi license and libraries
// 2. #define the appropriate feature in OEMFeatures.h to enable that language here
//    If it does not exist as a feature, just add the language desired and map to
//    the correct ZI8_LANG_* setting
// Note: Adding languages here WILL NOT add a new menu item "Multitap, Numbers, LanguageX".
// It also WILL NOT handle setting the language's AEE_TM_* mode to enter this mode
// See the sTextModes in OEMText.c structure for enabling separate menu items and supporting
// rapid as well as tapping for that language.
static int OEMIMMgr_SetInputMode(IIMMgr * po, AEETextInputMode mode)
{
   CIMMgr *          pme = (CIMMgr *)po;
#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
   ZiGPData *        pZi   = &pme->ziCtx;
   PZI8GETPARAM      pziGP = &pZi->ziGetParam;
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT

   if( !pme->bInit ){
      return EBADSTATE;
   }
   if( mode == pme->tmCurrent ){
      return AEE_SUCCESS;
   }
#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT)
   MEMSET((void *)pziGP, 0, sizeof(ZI8GETPARAM));
   MEMSET((void *)pZi->ziElementBuffer, 0, sizeof(pZi->ziElementBuffer));
   MEMSET((void *)pZi->ziElementBackup, 0, sizeof(pZi->ziElementBackup));
   MEMSET((void *)pZi->ziWordBuffer, 0, sizeof(pZi->ziWordBuffer));
   MEMSET((void *)pZi->ziCandidates, 0, sizeof(pZi->ziCandidates));
   pziGP->pElements     = pZi->ziElementBuffer;
   pziGP->pCurrentWord  = pZi->ziWordBuffer;
   pziGP->pCandidates   = pZi->ziCandidates;
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT
   switch( mode ){
   case AEE_TM_PINYIN:
#ifdef FEATURE_ZICORP_PINYIN // {
      pziGP->Language      = ZI8_LANG_ZH;
      // If you're system allows configuration of 
      // Taiwan or Hong Kong lookup mode, you should look
      // up the current setting and apply it here.
      pziGP->SubLanguage   = ZI8_SUBLANG_ZH;
      /*
      pziGP->SubLanguage   = ZI8_SUBLANG_ZH_TW;
      pziGP->SubLanguage   = ZI8_SUBLANG_ZH_HK;
      pziGP->SubLanguage   = ZI8_SUBLANG_ZH_ALL;
      */
      pme->tmCurrent = mode;
      break;
#else // } FEATURE_ZICORP_PINYIN {
      return EUNSUPPORTED;
#endif // } !FEATURE_ZICORP_PINYIN
   case AEE_TM_STROKE:
#ifdef FEATURE_ZICORP_STROKE // {
      pziGP->Language      = ZI8_LANG_ZH;
      // If you're system allows configuration of 
      // Taiwan or Hong Kong lookup mode, you should look
      // up the current setting and apply it here.
      pziGP->SubLanguage   = ZI8_SUBLANG_ZH;
      /*
      pziGP->SubLanguage   = ZI8_SUBLANG_ZH_TW;
      pziGP->SubLanguage   = ZI8_SUBLANG_ZH_HK;
      pziGP->SubLanguage   = ZI8_SUBLANG_ZH_ALL;
      */
      pme->tmCurrent = mode;
      break;
#else // } FEATURE_ZICORP_STROKE {
      return EUNSUPPORTED;
#endif // } !FEATURE_ZICORP_STROKE
#ifdef FEATURE_ZICORP_EZITEXT // {
   case AEE_TM_EZTEXT:
      {
         AEEDeviceInfo di;

         ISHELL_GetDeviceInfo(pme->pIShell, &di);

         switch( di.dwLang ){
#ifdef FEATURE_ZICORP_EZI_TH // {
         case LNG_THAI:
            pziGP->Language      = ZI8_LANG_TH;
            break;
#endif // } FEATURE_ZICORP_EZI_TH
#ifdef FEATURE_ZICORP_EZI_IT // {
         case LNG_ITALIAN:
            pziGP->Language      = ZI8_LANG_IT;
            break;
#endif // } FEATURE_ZICORP_EZI_IT
#ifdef FEATURE_ZICORP_EZI_FR // {
         case LNG_FRENCH:
            pziGP->Language      = ZI8_LANG_FR;
            break;
#endif // } FEATURE_ZICORP_EZI_FR
#ifdef FEATURE_ZICORP_EZI_ES // {
         case LNG_SPANISH:
            pziGP->Language      = ZI8_LANG_ES;
            break;
#endif // } FEATURE_ZICORP_EZI_ES
#ifdef FEATURE_ZICORP_EZI_PR // {
         case LNG_PORTUGUESE:
            pziGP->Language      = ZI8_LANG_PT;
            pziGP->SubLanguage   = ZI8_SUBLANG_PR_PR;
            break;
#endif // } FEATURE_ZICORP_EZI_PR
#ifdef FEATURE_ZICORP_EZI_BP //{
         case LNG_PORTUGUESE_BRAZIL:
            pziGP->Language      = ZI8_LANG_PT;
            pziGP->SubLanguage   = ZI8_SUBLANG_PR_BR;
            break;
#endif // } FEATURE_ZICORP_EZI_BP
#ifdef FEATURE_ZICORP_EZI_EN // {
         case LNG_ENGLISH:
#endif // } FEATURE_ZICORP_EZI_EN
         default:
            pziGP->Language      = ZI8_LANG_EN;
            break;
         }
         pme->tmCurrent = mode;
         pme->wKeyOpt   = ZI8_GETOPTION_DEFAULT;
      }
      break;
#endif // } FEATURE_ZICORP_EZITEXT
   case AEE_TM_KANJI:
   case AEE_TM_HANKAKU_KANA:
   case AEE_TM_HANGUL:
   case AEE_TM_T9:
   default:
      return EUNSUPPORTED;
   }
   return AEE_SUCCESS;
}
static int OEMIMMgr_GetInputMode(IIMMgr * po, AEETextInputMode * pmode)
{
   CIMMgr *          pme = (CIMMgr *)po;

   if( !pmode ){
      return EBADPARM;
   }
   if( pme->tmCurrent == AEE_TM_NONE ){
      return EBADSTATE;
   }

   *pmode = pme->tmCurrent;
   return AEE_SUCCESS;
}
static int OEMIMMgr_SetOpt(IIMMgr * po, uint16 wOption, void * pData, uint32 nSize)
{
   CIMMgr *          pme = (CIMMgr *)po;
   if( !pData ){
      return EBADPARM;
   }
   switch( wOption ){
   case IMGRO_MAXCANDIDATE:
      {
         uint16 * pOpt = (uint16 *)pData;
         if( nSize != sizeof(uint16) ){
            return EBADPARM;
         }
         if( !*pOpt ){
            return EBADITEM;
         }
         pme->nMaxCandidates = *pOpt;
      }
      break;
   case IMGRO_SYSTEMCONTEXT:
      {
         uint16 * pOpt  = (uint16 *)pData;
         if( nSize != sizeof(uint16) ){
            return EBADPARM;
         }
         // If not all options can be supported
         // Do not do this straight = setting
         // and ensure that the original is not modified
         // unless everything asked for may be set.
         if( *pOpt & ~(IMMCTX_ALL) ){
            return EUNSUPPORTED;
         }
         pme->wContextOpt  = *pOpt;
      }
      break;
   case IMGRO_KEYOPTIONS:
      {
         uint16 * pOpt  = (uint16 *)pData;
         if( nSize != sizeof(uint16) ){
            return EBADPARM;
         }
         if( *pOpt > IMMKEY_LAST ){
            return EUNSUPPORTED;
         }
         pme->wKeyOpt   = *pOpt;
      }
      break;
   default:
      return EUNSUPPORTED;
   }
   return AEE_SUCCESS;
}
static int OEMIMMgr_GetOpt(IIMMgr * po, uint16 wOption, void * pData, uint32 nSize)
{
   CIMMgr *          pme = (CIMMgr *)po;
   if( !pData ){
      return EBADPARM;
   }
   switch( wOption ){
   case IMGRO_MAXCANDIDATE:
      {
         uint16 * pOpt = (uint16 *)pData;
         if( nSize != sizeof(uint16) ){
            return EBADPARM;
         }
         *pOpt = pme->nMaxCandidates;
      }
      break;
   case IMGRO_SYSTEMCONTEXT:
      {
         uint16 * pOpt  = (uint16 *)pData;
         if( nSize != sizeof(uint16) ){
            return EBADPARM;
         }
         *pOpt = pme->wContextOpt;
      }
      break;
   case IMGRO_KEYOPTIONS:
      {
         uint16 * pOpt  = (uint16 *)pData;
         if( nSize != sizeof(uint16) ){
            return EBADPARM;
         }
         *pOpt = pme->wKeyOpt;
      }
      break;
   default:
      return EUNSUPPORTED;
   }
   return AEE_SUCCESS;
}
static int OEMIMMgr_GetCompositionCandidates(IIMMgr *po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars)
{
   CIMMgr * pme = (CIMMgr *)po;
#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
   int               nCandidateReq = *pnNumCandidates;
   ZiGPData *        pZi   = &pme->ziCtx;
   PZI8GETPARAM      pziGP = &pZi->ziGetParam;
#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT

   if( pBuff && nBuffSize ){
      MEMSET((void *)pBuff, 0, nBuffSize);
   }

   switch( pme->tmCurrent ){
   case AEE_TM_PINYIN:
#ifdef FEATURE_ZICORP_PINYIN // {
      {
         int      nBuffIter   = 0,
                  nCandIter   = 0,
                  nCandidate  = 0,
                  nMatches    = 0,
                  nAECharBuf  = nBuffSize>>1;
         boolean  bBreak      = FALSE;
#ifdef PROFILE_IMM_BUFFERS // {
         uint32 nProfile   = GETTIMEMS();

         DBGPRINTF("---------------------");
         DBGPRINTF("CompoCandidates");
         DBGPRINTF("%u\tInit", nProfile);
#endif // } PROFILE_IMM_BUFFERS

         pziGP->Context          = (ZI8UCHAR)IMMgr_GetZi8Context(pme->wContextOpt);

         pziGP->GetMode          = ZI8_GETMODE_1KEYPRESS_PINYIN;
         pziGP->GetOptions       = (ZI8UCHAR)IMMgr_GetZi8KeyOpt(pme->wKeyOpt);
         pziGP->MaxCandidates    = MIN(SIZE_OF_CANDIDATE_BUF>>3, pme->nMaxCandidates);
         pziGP->FirstCandidate   = nFirst;

         if( nCandidateReq ){
            pziGP->MaxCandidates = MIN(pziGP->MaxCandidates, nCandidateReq);
         }
         for( nBuffIter = 0; nBuffIter < SIZE_OF_ELEMENT_BUF; nBuffIter++ ){
            if( !pziGP->pElements[nBuffIter] ){
               break;
            }
         }
         if( !nBuffIter ){
            return EBADSTATE;
         }
         pziGP->ElementCount     = nBuffIter;

#ifdef PROFILE_IMM_BUFFERS // {
         DBGPRINTF("%d\tPreProcess", GETTIMEMS() - nProfile);
         nProfile   = GETTIMEMS();
#endif // } PROFILE_IMM_BUFFERS
         // Optimisation: 
         // GetCount is quicker than filling the buffer and
         // counting the rest. If there is no buffer to fill and 
         // no buffer size to calculate, just return the info
         if( ((!pBuff || !nBuffSize) && !pnMaxChars) ){
            nMatches = Zi8GetCandidatesCount(pziGP);
            *pnNumCandidates  = MIN(nMatches, pme->nMaxCandidates);
            if( nCandidateReq ){
               *pnNumCandidates  = MIN(*pnNumCandidates, nCandidateReq);
            }
#ifdef PROFILE_IMM_BUFFERS // {
            DBGPRINTF("%d\tGotCandidatesCount", GETTIMEMS() - nProfile);
            nProfile   = GETTIMEMS();
            DBGPRINTF("%u\tDone Compl", nProfile);
            DBGPRINTF("---------------------");
#endif // } PROFILE_IMM_BUFFERS
            return AEE_SUCCESS;
         }else{
            nMatches = Zi8GetCandidates(pziGP);
         }
#ifdef PROFILE_IMM_BUFFERS // {
         DBGPRINTF("%d\tGotCandidates", GETTIMEMS() - nProfile);
         nProfile   = GETTIMEMS();
#endif // } PROFILE_IMM_BUFFERS
         if( !nMatches ){
            pziGP->ElementCount--;
            pziGP->pElements[pziGP->ElementCount] = 0;
            nMatches = Zi8GetCandidates(pziGP);
         }
         if( !nMatches ){
            *pnNumCandidates  = 0;
            if( pnMaxChars ){
               *pnMaxChars       = 0;
            }
            return AEE_SUCCESS;
         }

         nBuffIter = 0;
#ifdef PROFILE_IMM_BUFFERS // {
         DBGPRINTF("%d\tPre-Process Results", GETTIMEMS() - nProfile);
         nProfile   = GETTIMEMS();
#endif // } PROFILE_IMM_BUFFERS
         *pnNumCandidates = 0;
         while( (nCandidate < nMatches 
            && (!nCandidateReq || nCandidate < nCandidateReq)
            && (nBuffIter < nAECharBuf || !nAECharBuf || pnMaxChars))
            && (nCandidate < (int)pme->nMaxCandidates) ){
            for( nCandIter = 0; nCandIter < SIZE_OF_CANDIDATE_BUF; nCandIter++, nBuffIter++ ){
               if( pziGP->pCandidates[nCandIter] ){
                  if( nBuffIter < nAECharBuf ){
                     pBuff[nBuffIter] = (AECHAR)((pziGP->pCandidates[nCandIter] - ZI8_CODE_PINYIN_A) + 'A');
                  }
               }else{
                  nCandidate++;
                  if( nBuffIter < nAECharBuf ){
                     *pnNumCandidates = nCandidate;
                  }
                  if( nCandidate == nMatches || (nCandidateReq && nCandidate == nCandidateReq) || nCandidate == (int)pme->nMaxCandidates ){
                     bBreak   = TRUE;
                     break;
                  }
               }
            }
            if( !bBreak ){
               pziGP->FirstCandidate   = nFirst + nCandidate;
               nMatches = nCandidate + Zi8GetCandidates(pziGP);
            }
         }
#ifdef PROFILE_IMM_BUFFERS // {
         DBGPRINTF("%d\tProcess Results", GETTIMEMS() - nProfile);
         nProfile   = GETTIMEMS();
#endif // } PROFILE_IMM_BUFFERS
         if( pBuff && nAECharBuf ){
            while( pBuff[--nAECharBuf] != 0 ){
               pBuff[nAECharBuf] = 0;
            }
         }else{
            *pnNumCandidates = nCandidate;
         }
         if( pnMaxChars ){
            *pnMaxChars = nBuffIter+1; // for NULL
         }
#ifdef PROFILE_IMM_BUFFERS // {
         nProfile   = GETTIMEMS();
         DBGPRINTF("%u\tDone Compo", nProfile);
         DBGPRINTF("---------------------");
#endif // } PROFILE_IMM_BUFFERS
         break;
      }
#else // } FEATURE_ZICORP_PINYIN {
      return EBADSTATE;
#endif // } !FEATURE_ZICORP_PINYIN
   case AEE_TM_STROKE:
#ifdef FEATURE_ZICORP_STROKE // {
      {
         int   nBuffIter   = 0,
               nCandIter   = nFirst,
               nCandidate  = 0,
               nMatches    = 0,
               nAECharBuf  = nBuffSize>>1,
               i           = 0;

         for( nBuffIter = 0; nBuffIter < SIZE_OF_ELEMENT_BUF; nBuffIter++ ){
            if( !pziGP->pElements[nBuffIter] ){
               break;
            }
         }

         nMatches = ((nBuffIter - nFirst) ? 1: 0);
         if( nMatches <= 0 ){
            *pnNumCandidates  = 0;
            if( pnMaxChars ){
               *pnMaxChars       = 0;
            }
            return AEE_SUCCESS;
         }

         *pnNumCandidates = 0;
         while( (nCandidate < nMatches 
            && (!nCandidateReq || nCandidate < nCandidateReq)
            && (i < nAECharBuf || !nAECharBuf || pnMaxChars))
            && (nCandidate < (int)pme->nMaxCandidates) ){

            nCandidate++;
            for( nCandIter = 0; nCandIter < SIZE_OF_CANDIDATE_BUF && pziGP->pElements[nCandIter]; nCandIter++ ){
               if( i < (nAECharBuf-1) ){
                  pBuff[i] = (AECHAR)(pziGP->pElements[nCandIter]);
               }
               i++;
            }
            if( i < nAECharBuf ){
               *pnNumCandidates = nCandidate;
            }
            if( nCandidate == nMatches || (nCandidateReq && nCandidate == nCandidateReq) || (nCandidate == (int)pme->nMaxCandidates) ){
               break;
            }
         }
         if( !nAECharBuf ){
            *pnNumCandidates = nCandidate;
         }
         if( pnMaxChars ){
            *pnMaxChars = i+1;//for NULL
         }

         break;
      }
#else // } FEATURE_ZICORP_STROKE {
      return EBADSTATE;
#endif // } !FEATURE_ZICORP_STROKE
#ifdef FEATURE_ZICORP_EZITEXT // {
   case AEE_TM_EZTEXT:
      {
         int      nBuffIter   = 0,
                  nCandIter   = 0,
                  nCandidate  = 0,
                  nMatches    = pme->nMaxCandidates,
                  nAECharBuf  = nBuffSize>>1;
         AECHAR * pBuffWalker = pBuff;

         pziGP->Context          = (ZI8UCHAR)IMMgr_GetZi8Context(pme->wContextOpt);
         pziGP->GetMode          = ZI8_GETMODE_DEFAULT;
         pziGP->GetOptions       = (ZI8UCHAR)IMMgr_GetZi8KeyOpt(pme->wKeyOpt);
         pziGP->MaxCandidates    = MIN(SIZE_OF_CANDIDATE_BUF>>3, pme->nMaxCandidates);
         pziGP->FirstCandidate   = nFirst;

         if( nCandidateReq ){
            pziGP->MaxCandidates = MIN(pziGP->MaxCandidates, nCandidateReq);
         }
         for( nBuffIter = 0; nBuffIter < SIZE_OF_ELEMENT_BUF; nBuffIter++ ){
            if( !pziGP->pElements[nBuffIter] ){
               break;
            }
         }
         if( !nBuffIter ){
            return EBADSTATE;
         }
         pziGP->ElementCount     = nBuffIter;

         nMatches = Zi8GetCandidates(pziGP);
         if( !pziGP->count ){
            pziGP->ElementCount--;
            pziGP->pElements[pziGP->ElementCount] = 0;
            nMatches = Zi8GetCandidates(pziGP);
         }
         if( !pziGP->count ){
            *pnNumCandidates  = 0;
            if( pnMaxChars ){
               *pnMaxChars       = 0;
            }
            return AEE_SUCCESS;
         }
         nMatches = pziGP->count;
         // Optimisation: 
         // If there is no buffer to fill and 
         // no buffer size to calculate, just return the info
         if( ((!pBuff || !nBuffSize) && !pnMaxChars) ){
            *pnNumCandidates  = MIN(nMatches, pme->nMaxCandidates);
            if( nCandidateReq ){
               *pnNumCandidates  = MIN(nMatches, nCandidateReq);
            }
            return AEE_SUCCESS;
         }
         nBuffIter = 0;
         *pnNumCandidates = 0;
         while( (nCandidate < nMatches 
            && (!nCandidateReq || nCandidate < nCandidateReq)
            && (nBuffIter < nAECharBuf || !nAECharBuf || pnMaxChars))
            && (nCandidate < (int)pme->nMaxCandidates) ){

            for( nCandIter = 0; nCandIter < nMatches; nCandIter++, nBuffIter++ ){
               AECHAR tmpBuf[64];

               Zi8GetAlphaWordW(pziGP->Language, pziGP->pCandidates[nCandIter], tmpBuf, sizeof(tmpBuf));
               switch( pme->nShiftState ){
               case EZISHIFT_SMALL:
                  {
                     // Do nothing, Zi8GetAlphaWordW gave lower case
                  }
                  break;
               case EZISHIFT_MIXED:
                  {
                     AECHAR szwFirst[2];
                     
                     MEMSET((void *)szwFirst, 0, sizeof(szwFirst));
                     *szwFirst = *tmpBuf;
                     WSTRUPPER(szwFirst);
                     *tmpBuf = *szwFirst;
                  }
                  break;
               case EZISHIFT_CAPS:
                  {
                     WSTRUPPER(tmpBuf);
                  }
                  break;
               }

               nBuffIter   += (WSTRLEN(tmpBuf)+1);
               if( nBuffIter <= nAECharBuf ){
                  WSTRCPY(pBuffWalker, tmpBuf);
                  pBuffWalker += (WSTRLEN(tmpBuf)+1);
               }
               nCandidate++;
               if( nBuffIter <= nAECharBuf ){
                  *pnNumCandidates = nCandidate;
               }
               if( nCandidate == nMatches || (nCandidateReq && nCandidate == nCandidateReq) || nCandidate == (int)pme->nMaxCandidates ){
                  break;
               }
            }
         }
         if( pBuff && nAECharBuf ){
            while( pBuff[--nAECharBuf] != 0 ){
               pBuff[nAECharBuf] = 0;
            }
         }else{
            *pnNumCandidates = nCandidate;
         }
         if( pnMaxChars ){
            *pnMaxChars = nBuffIter; // NULL accounted for above.
         }
      }
      break;
#else // } FEATURE_ZICORP_EZITEXT {
      return EBADSTATE;
#endif // } !FEATURE_ZICORP_EZITEXT
   case AEE_TM_KANJI:
   case AEE_TM_HANKAKU_KANA:
   case AEE_TM_HANGUL:
   case AEE_TM_T9:
   default:
      return EBADSTATE;
   }
   return AEE_SUCCESS;
}
static int OEMIMMgr_GetCompleteCandidates(IIMMgr *po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars)
{
   CIMMgr * pme = (CIMMgr *)po;
#ifdef FEATURE_ZICORP_CHINESE// {
   int               nCandidateReq = *pnNumCandidates;
   ZiGPData *        pZi   = &pme->ziCtx;
   PZI8GETPARAM      pziGP = &pZi->ziGetParam;
#endif // } FEATURE_ZICORP_CHINESE

   switch( pme->tmCurrent ){
   case AEE_TM_PINYIN:
#ifdef FEATURE_ZICORP_PINYIN // {
      {
         int      i,
                  nMatches,
                  nAECharBuf  = nBuffSize>>1,
                  nCandidate  = 0,
                  nCandIter   = 0,
                  nBuffIter   = 0;
         boolean  bBreak      = FALSE;
#ifdef PROFILE_IMM_BUFFERS // {
         uint32 nProfile   = GETTIMEMS();

         DBGPRINTF("---------------------");
         DBGPRINTF("ComplCandidates");
         DBGPRINTF("%u\tInit", nProfile);
#endif // } PROFILE_IMM_BUFFERS

         MEMCPY((void *)pZi->ziElementBackup, (void *)pZi->ziElementBuffer, sizeof(pZi->ziElementBuffer));
         MEMSET((void *)pZi->ziElementBuffer, 0, sizeof(pZi->ziElementBuffer));
         pziGP->Context          = (ZI8UCHAR)IMMgr_GetZi8Context(pme->wContextOpt);
         pziGP->GetMode          = ZI8_GETMODE_PINYIN;
         pziGP->GetOptions       = ZI8_GETOPTION_DEFAULT;
         pziGP->ElementCount     = (ZI8UCHAR) WSTRLEN(pBuff);
         pziGP->MaxCandidates    = MIN(SIZE_OF_CANDIDATE_BUF>>1,pme->nMaxCandidates);
         pziGP->FirstCandidate   = nFirst;

         if( nCandidateReq ){
            pziGP->MaxCandidates = MIN(pziGP->MaxCandidates, nCandidateReq);
         }
         for( i = 0; i < pziGP->ElementCount; i++ ){
            pziGP->pElements[i] = (ZI8WCHAR)(ZI8_CODE_PINYIN_A + (pBuff[i] - (AECHAR)'A'));
         }
#ifdef PROFILE_IMM_BUFFERS // {
         DBGPRINTF("%d\tPreProcess", GETTIMEMS() - nProfile);
         nProfile   = GETTIMEMS();
#endif // } PROFILE_IMM_BUFFERS
         // Optimisation: 
         // GetCount is quicker than filling the buffer and
         // counting the rest. If there is no buffer to fill and 
         // no buffer size to calculate, just return the info
         if( ((!pBuff || !nBuffSize) && !pnMaxChars) ){
            nMatches = Zi8GetCandidatesCount(pziGP);
            *pnNumCandidates  = MIN(nMatches, pme->nMaxCandidates);
            if( nCandidateReq ){
               *pnNumCandidates  = MIN(*pnNumCandidates, nCandidateReq);
            }
#ifdef PROFILE_IMM_BUFFERS // {
            DBGPRINTF("%d\tGotCandidatesCount", GETTIMEMS() - nProfile);
            nProfile   = GETTIMEMS();
            DBGPRINTF("%u\tDone Compl", nProfile);
            DBGPRINTF("---------------------");
#endif // } PROFILE_IMM_BUFFERS
            MEMCPY((void *)pZi->ziElementBuffer, (void *)pZi->ziElementBackup, sizeof(pZi->ziElementBuffer));
            return AEE_SUCCESS;
         }else{
            nMatches = Zi8GetCandidates(pziGP);
         }
#ifdef PROFILE_IMM_BUFFERS // {
         DBGPRINTF("%d\tGotCandidates", GETTIMEMS() - nProfile);
         nProfile   = GETTIMEMS();
#endif // } PROFILE_IMM_BUFFERS
         if( nBuffSize ){
            MEMSET((void *)pBuff, 0, nBuffSize);
         }
         if( !nMatches ){
            *pnNumCandidates = nMatches;
            if( pnMaxChars ){
               *pnMaxChars = 0;
            }
            MEMCPY((void *)pZi->ziElementBuffer, (void *)pZi->ziElementBackup, sizeof(pZi->ziElementBuffer));
            return AEE_SUCCESS;
         }

#ifdef PROFILE_IMM_BUFFERS // {
         DBGPRINTF("%d\tPre-Process Results", GETTIMEMS() - nProfile);
         nProfile   = GETTIMEMS();
#endif // } PROFILE_IMM_BUFFERS
         while( (nCandidate < nMatches 
            && (!nCandidateReq || nCandidate < nCandidateReq)
            && (nBuffIter < nAECharBuf || !nAECharBuf || pnMaxChars))
            && (nCandidate < (int)pme->nMaxCandidates) ){

            for( nCandIter = 0; nCandIter < SIZE_OF_CANDIDATE_BUF && pziGP->pCandidates[nCandIter]; nCandIter++ ){
               if( nBuffIter < nAECharBuf-1 ){
                  pBuff[nBuffIter] = (AECHAR)(pziGP->pCandidates[nCandIter]);
                  pBuff[nBuffIter+1] = (AECHAR)0;
               }
               nBuffIter += 2;
               nCandidate++;
               if( nBuffIter <= nAECharBuf ){
                  *pnNumCandidates = nCandidate;
               }
               if( nCandidate == nMatches || (nCandidateReq && nCandidate == nCandidateReq) || (nCandidate == (int)pme->nMaxCandidates) ){
                  bBreak   = TRUE;
                  break;
               }
            }
            if( !bBreak ){
               pziGP->FirstCandidate   = nFirst + nCandidate;
               nMatches = nCandidate + Zi8GetCandidates(pziGP);
            }
         }
#ifdef PROFILE_IMM_BUFFERS // {
         DBGPRINTF("%d\tProcess Results", GETTIMEMS() - nProfile);
         nProfile   = GETTIMEMS();
#endif // } PROFILE_IMM_BUFFERS
         if( !nAECharBuf ){
            *pnNumCandidates = nCandidate;
         }
         if( pnMaxChars ){
            *pnMaxChars = nBuffIter;// NULL taken care of above!
         }

         MEMCPY((void *)pZi->ziElementBuffer, (void *)pZi->ziElementBackup, sizeof(pZi->ziElementBuffer));
#ifdef PROFILE_IMM_BUFFERS // {
         nProfile   = GETTIMEMS();
         DBGPRINTF("%u\tDone Compl", nProfile);
         DBGPRINTF("---------------------");
#endif // } PROFILE_IMM_BUFFERS
         break;
      }
#else // } FEATURE_ZICORP_PINYIN {
      return EBADSTATE;
#endif // } !FEATURE_ZICORP_PINYIN
   case AEE_TM_STROKE:
#ifdef FEATURE_ZICORP_STROKE // {
      {
         int      nBuffIter   = 0,
                  nCandIter   = 0,
                  nCandidate  = 0,
                  nMatches    = 0,
                  nAECharBuf  = nBuffSize>>1;
         boolean  bBreak      = FALSE;

         MEMCPY((void *)pZi->ziElementBackup, (void *)pZi->ziElementBuffer, sizeof(pZi->ziElementBuffer));
         MEMSET((void *)pZi->ziElementBuffer, 0, sizeof(pZi->ziElementBuffer));
         pziGP->Context          = (ZI8UCHAR)IMMgr_GetZi8Context(pme->wContextOpt);

         pziGP->GetMode          = ZI8_GETMODE_STROKES;
         pziGP->GetOptions       = ZI8_GETOPTION_ALLCOMPONENTS;
         pziGP->ElementCount     = (ZI8UCHAR) WSTRLEN(pBuff);
         pziGP->MaxCandidates    = MIN(SIZE_OF_CANDIDATE_BUF>>1, pme->nMaxCandidates);
         pziGP->FirstCandidate   = nFirst;

         if( nCandidateReq ){
            pziGP->MaxCandidates = MIN(pziGP->MaxCandidates, nCandidateReq);
         }
         for( nBuffIter = 0; nBuffIter < pziGP->ElementCount; nBuffIter++ ){
            pziGP->pElements[nBuffIter] = (ZI8WCHAR)pBuff[nBuffIter];
         }
         if( !nBuffIter ){
            MEMCPY((void *)pZi->ziElementBuffer, (void *)pZi->ziElementBackup, sizeof(pZi->ziElementBuffer));
            return EBADSTATE;
         }
         // Optimisation: 
         // GetCount is quicker than filling the buffer and
         // counting the rest. If there is no buffer to fill and 
         // no buffer size to calculate, just return the info
         if( ((!pBuff || !nBuffSize) && !pnMaxChars) ){
            nMatches = Zi8GetCandidatesCount(pziGP);
            *pnNumCandidates  = MIN(nMatches, pme->nMaxCandidates);
            if( nCandidateReq ){
               *pnNumCandidates  = MIN(*pnNumCandidates, nCandidateReq);
            }
            MEMCPY((void *)pZi->ziElementBuffer, (void *)pZi->ziElementBackup, sizeof(pZi->ziElementBuffer));
            return AEE_SUCCESS;
         }else{
            nMatches = Zi8GetCandidates(pziGP);
         }

         if( nBuffSize ){
            MEMSET((void *)pBuff, 0, nBuffSize);
         }
         if( !nMatches ){
            *pnNumCandidates  = 0;
            if( pnMaxChars ){
               *pnMaxChars       = 0;
            }
            MEMCPY((void *)pZi->ziElementBuffer, (void *)pZi->ziElementBackup, sizeof(pZi->ziElementBuffer));
            return AEE_SUCCESS;
         }

         nBuffIter   = 0;
         while( (nCandidate < nMatches 
            && (!nCandidateReq || nCandidate < nCandidateReq)
            && (nBuffIter < nAECharBuf || !nAECharBuf || pnMaxChars))
            && (nCandidate < (int)pme->nMaxCandidates) ){

            for( nCandIter = 0; nCandIter < SIZE_OF_CANDIDATE_BUF && pziGP->pCandidates[nCandIter]; nCandIter++ ){
               if( nBuffIter < nAECharBuf-1 ){
                  pBuff[nBuffIter] = (AECHAR)(pziGP->pCandidates[nCandIter]);
                  pBuff[nBuffIter+1] = (AECHAR)0;
               }
               nBuffIter += 2;
               nCandidate++;
               if( nBuffIter <= nAECharBuf ){
                  *pnNumCandidates = nCandidate;
               }
               if( nCandidate == nMatches || (nCandidateReq && nCandidate == nCandidateReq) || (nCandidate == (int)pme->nMaxCandidates) ){
                  bBreak   = TRUE;
                  break;
               }
            }
            if( !bBreak ){
               pziGP->FirstCandidate   = nFirst + nCandidate;
               nMatches = nCandidate + Zi8GetCandidates(pziGP);
            }
         }
         if( !nAECharBuf ){
            *pnNumCandidates = nCandidate;
         }
         if( pnMaxChars ){
            *pnMaxChars = nBuffIter+1;// NULL taken care of above!
         }

         MEMCPY((void *)pZi->ziElementBuffer, (void *)pZi->ziElementBackup, sizeof(pZi->ziElementBuffer));
         break;
      }
#else // } FEATURE_ZICORP_STROKE {
      return EBADSTATE;
#endif // } !FEATURE_ZICORP_STROKE
   case AEE_TM_KANJI:
   case AEE_TM_HANKAKU_KANA:
   case AEE_TM_HANGUL:
   case AEE_TM_EZTEXT:
   case AEE_TM_T9:
   default:
      return EBADSTATE;
   }
   return AEE_SUCCESS;
}
static boolean OEMIMMgr_IsCompleteCandidate(IIMMgr * po, const AECHAR * pBuff)
{
   CIMMgr * pme = (CIMMgr *)po;

   if( !pBuff || !*pBuff || !pme->bInit ){
      return FALSE;
   }
#ifdef FEATURE_ZICORP_CHINESE // {
   return (boolean)((Zi8IsComponent((ZI8WCHAR)*pBuff) == ZI8TRUE) ? FALSE : TRUE);
#endif // } FEATURE_ZICORP_CHINESE
   return TRUE;
}
static int OEMIMMgr_GetData(IIMMgr * po, AECHAR * pBuff, uint16 * pwBuffSize)
{
   CIMMgr * pme = (CIMMgr *)po;

   if( !pwBuffSize ){
      return EBADPARM;
   }
   if( !pme->pszwWordChoice || !*pme->pszwWordChoice ){
      *pwBuffSize = 0;
      return EBADSTATE;
   }
   if( !pBuff || !*pwBuffSize ){
      *pwBuffSize = (WSTRLEN(pme->pszwWordChoice)+1)*sizeof(AECHAR);
      return AEE_SUCCESS;
   }

   WSTRLCPY(pBuff, pme->pszwWordChoice, (*pwBuffSize)/sizeof(AECHAR));
   *pwBuffSize = (WSTRLEN(pme->pszwWordChoice)+1)*sizeof(AECHAR);

   return AEE_SUCCESS;
}
static int OEMIMMgr_SetData(IIMMgr * po, const AECHAR * pBuff)
{
   CIMMgr * pme   = (CIMMgr *)po;
   uint16   nLen;
   AECHAR * pRealloc;

   if( !pBuff || !*pBuff ){
      MEMSET((void *)pme->pszwWordChoice, 0, pme->nChoiceSize);
      return AEE_SUCCESS;
   }
   if( (nLen = WSTRLEN(pBuff)) >= 0xFFFF ){
      return EBADPARM;
   }
   if( nLen >= (uint16)(pme->nChoiceSize>>1) 
      || !pme->pszwWordChoice ){
      nLen = ((nLen+1)*sizeof(AECHAR));
      pRealloc = (AECHAR *)REALLOC(pme->pszwWordChoice, nLen);
      if( pRealloc ){
         pme->pszwWordChoice  = pRealloc;
         pme->nChoiceSize     = nLen;
      }else{
         return ENOMEMORY;
      }
   }
   WSTRLCPY(pme->pszwWordChoice, pBuff, pme->nChoiceSize);
   return AEE_SUCCESS;
}
static int OEMIMMgr_GetAssociatedCandidates(IIMMgr *po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars)
{
#ifdef FEATURE_ZICORP_CHINESE // {
   CIMMgr * pme            = (CIMMgr *)po;
   int      nCandidateReq  = *pnNumCandidates;
   int      nMatches    = 0,
            nAECharBuf  = nBuffSize>>1,
            nCandidate  = 0,
            nCandIter   = 0,
            nBuffIter   = 0;
   boolean  bBreak      = FALSE;
   ZiGPData *        pZi   = &pme->ziCtx;
   PZI8GETPARAM      pziGP = &pZi->ziGetParam;
#endif // } FEATURE_ZICORP_CHINESE

#ifdef FEATURE_ZICORP_CHINESE // {
   // Validate Mode
   if( pme->tmCurrent != AEE_TM_PINYIN && pme->tmCurrent != AEE_TM_STROKE ){
      return EBADSTATE;
   }
   // Play with Zi parameters, note that word association doesn't seem to care 
   // about stroke or pinyins
   pziGP->Context          = (ZI8UCHAR)IMMgr_GetZi8Context(pme->wContextOpt);
   pziGP->GetMode          = ZI8_GETMODE_PINYIN;
   pziGP->GetOptions       = ZI8_GETOPTION_DEFAULT;
   pziGP->ElementCount     = (ZI8UCHAR) 0;
   pziGP->MaxCandidates    = MIN(SIZE_OF_CANDIDATE_BUF>>1,pme->nMaxCandidates);
   pziGP->FirstCandidate   = nFirst;

   WSTRLCPY((AECHAR *)pZi->ziWordBuffer, pBuff, SIZE_OF_WORD_BUF);
   pziGP->WordCharacterCount = WSTRLEN((AECHAR *)pZi->ziWordBuffer);

   // Use smaller of maximum they've initialised to and what they're asking now
   if( nCandidateReq ){
      pziGP->MaxCandidates = MIN(pziGP->MaxCandidates, nCandidateReq);
   }
   // Get the data from Zi
   Zi8GetCandidates(pziGP);
   nMatches = pziGP->wordCandidates;

   // Optimisation: 
   // Often this function will be polled for count, so
   // If there is no buffer to fill and 
   // no buffer size to calculate, just return the info
   if( ((!pBuff || !nBuffSize) && !pnMaxChars) ){
      *pnNumCandidates  = MIN(nMatches, pme->nMaxCandidates);
      if( nCandidateReq ){
         *pnNumCandidates  = MIN(*pnNumCandidates, nCandidateReq);
      }
      return AEE_SUCCESS;
   }

   if( nBuffSize ){
      MEMSET((void *)pBuff, 0, nBuffSize);
   }
   // Give all information back to caller as requested
   if( !nMatches ){
      *pnNumCandidates = nMatches;
      if( pnMaxChars ){
         *pnMaxChars = 0;
      }
      return AEE_SUCCESS;
   }

   while( (nCandidate < nMatches 
      && (!nCandidateReq || nCandidate < nCandidateReq)
      && (nBuffIter < nAECharBuf || !nAECharBuf || pnMaxChars))
      && (nCandidate < (int)pme->nMaxCandidates) ){

      for( nCandIter = 0; nCandIter < SIZE_OF_CANDIDATE_BUF && pziGP->pCandidates[nCandIter]; nCandIter++ ){
         if( nBuffIter < nAECharBuf-1 ){
            pBuff[nBuffIter] = (AECHAR)(pziGP->pCandidates[nCandIter]);
            pBuff[nBuffIter+1] = (AECHAR)0;
         }
         nBuffIter += 2;
         nCandidate++;
         if( nBuffIter <= nAECharBuf ){
            *pnNumCandidates = nCandidate;
         }
         if( nCandidate == nMatches || (nCandidateReq && nCandidate == nCandidateReq) || (nCandidate == (int)pme->nMaxCandidates) ){
            bBreak   = TRUE;
            break;
         }
      }
      if( !bBreak ){
         pziGP->FirstCandidate   = nFirst + nCandidate;
         nMatches = nCandidate + Zi8GetCandidates(pziGP);
      }
   }
   if( !nAECharBuf ){
      *pnNumCandidates = nCandidate;
   }
   if( pnMaxChars ){
      *pnMaxChars = nBuffIter;// NULL handled above
   }
#endif // } FEATURE_ZICORP_CHINESE
   return AEE_SUCCESS;
}
static int OEMIMMgr_GetCompositionCount(IIMMgr * po, int * pnElements)
{
#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
   CIMMgr * pme            = (CIMMgr *)po;
   int      i;

   if( !pnElements ){
      return EBADPARM;
   }
   // Count the typed elements
   for( i = 0; i < SIZE_OF_ELEMENT_BUF; i++ ){
      if( !pme->ziCtx.ziGetParam.pElements[i] ){
         break;
      }
   }

   *pnElements = i;

   return AEE_SUCCESS;
#else // }{
   return EFAILED;
#endif // } 
}


#if defined(FEATURE_ZICORP_CHINESE) || defined(FEATURE_ZICORP_EZITEXT) // {
static uint16 IMMgr_GetZi8Context(uint16 wContext)
{
   uint16 wReturn = 0;

   if( wContext & IMMCTX_NODUPE ){
      wReturn |= ZI8_GETCONTEXT_NO_DUPLICATE;
   }
   if( wContext & IMMCTX_COMMON ){
      wReturn |= ZI8_GETCONTEXT_NO_RADICALS;
   }
   if( wContext & IMMCTX_PUNCTUATION ){
      wReturn |= ZI8_GETCONTEXT_CHINESE_PUNCT;
   }
   if( wContext & IMMCTX_GIVENNAME ){
      wReturn |= ZI8_GETCONTEXT_GIVENNAME;
   }
   if( wContext & IMMCTX_SURNAME ){
      wReturn |= ZI8_GETCONTEXT_SURNAME;
   }
   if( wContext & IMMCTX_NAME ){
      wReturn |= ZI8_GETCONTEXT_SURNAME; // go with this one
   }
   if( wContext & IMMCTX_EMAIL ){
      wReturn |= ZI8_GETCONTEXT_SMS;
   }
   return wReturn;
}
static uint16 IMMgr_GetZi8KeyOpt(uint16 wKeyOpt)
{
   switch( wKeyOpt ){
   case IMMKEY_MATCHONE:
      return ZI8_GETOPTION_GET_SPELLING2;
   case IMMKEY_MATCHMANY:
      return ZI8_GETOPTION_GET_SPELLING;
   case IMMKEY_DEFAULT:
   default:
      return ZI8_GETOPTION_DEFAULT;
   }
   return 0;
}
#ifdef FEATURE_ZICORP_PINYIN // {
static uint16 IMMgr_PYNMapKeytoElement(AVKType key)
{
   if( key >= AVK_0 && key <= AVK_9 ){
      return ((key == AVK_0) ? ZI8_ONEKEY_KEY0 : (key-AVK_1)+ZI8_ONEKEY_KEY1);
   }
   return 0;
}
#endif // } FEATURE_ZICORP_PINYIN
#ifdef FEATURE_ZICORP_STROKE // {
static uint16 IMMgr_STKMapKeytoElement(AVKType key)
{
   if( key >= AVK_1 && key <= AVK_9 ){
      switch( key ){
      case AVK_1:
         return (uint16)ZI8_CODE_OVER;
      case AVK_2:
         return (uint16)ZI8_CODE_DOWN;
      case AVK_3:
         return (uint16)ZI8_CODE_LEFT;
      case AVK_4:
         return (uint16)ZI8_CODE_DOT;
      case AVK_5:
         return (uint16)ZI8_CODE_OVER_DOWN;
      case AVK_6:
         return (uint16)ZI8_CODE_CURVED_HOOK;
      case AVK_7:
         return (uint16)ZI8_CODE_DOWN_OVER;
      case AVK_9:
         return (uint16)ZI8_CODE_OVER_DOWN_OVER;
      case AVK_8:
      default:
         return (uint16)ZI8_CODE_WILDCARD;
      }
   }
   return 0;
}
#endif // } FEATURE_ZICORP_STROKE
   #ifdef FEATURE_ZICORP_EZITEXT // {
static uint16 IMMgr_EZIMapKeytoElement(AVKType key)
{
   if( key >= AVK_2 && key <= AVK_9 ){
      return (uint16)(ZI8_CODE_LATIN_KEY2 + (ZI8WCHAR) key - (ZI8WCHAR) AVK_2);
   }
   switch( key ){
   case AVK_1:
      return (uint16)ZI8_CODE_LATIN_PUNCT;
   default:
      break;
   }
   return 0;
}
#endif // } FEATURE_ZICORP_EZITEXT

#endif // } FEATURE_ZICORP_CHINESE || FEATURE_ZICORP_EZITEXT
