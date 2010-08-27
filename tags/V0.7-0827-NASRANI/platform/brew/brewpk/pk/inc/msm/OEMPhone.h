/*======================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

This file is covered by the licensing terms for CDMA 
ASIC licensees.  Under terms of those licensing agreements, 
this file MUST NOT be transmitted in whole or in part 
to any party who has not been licensed for use of the
QUALCOMM CDMA technologies. 

FILE:  OEMPhone.h

SERVICES:  Extended Phone/TAPI Services.

GENERAL DESCRIPTION:
	Base level definitions, typedefs, etc. for AEE Phone Services

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#ifndef OEMPHONE_H
#define OEMPHONE_H

#include "cm.h"
#include "AEE.h"
#include "msm/AEEPhone.h"

typedef struct _OEMPhone  OEMPhone;

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif /* __cplusplus */

EXTERN AEE_EXPORTS int OEMPhone_New( OEMPhone **ppif );
EXTERN AEE_EXPORTS uint32 OEMPhone_AddRef( OEMPhone *po );
EXTERN AEE_EXPORTS uint32 OEMPhone_Release( OEMPhone *po );
EXTERN AEE_EXPORTS void OEMPhone_SetMask( OEMPhone *po, uint32 dwMask );
EXTERN AEE_EXPORTS int OEMPhone_AddListener( OEMPhone *pme, AEECallback *pcb );
EXTERN AEE_EXPORTS const cm_ph_info_s_type * OEMPhone_GetPhoneInfo(OEMPhone * po);
EXTERN AEE_EXPORTS const CM_CALL_INFO_S_TYPE *OEMPhone_GetCallInfo(OEMPhone * po);
EXTERN AEE_EXPORTS const CM_SS_INFO_S_TYPE *OEMPhone_GetSystemInfo(OEMPhone * po);
EXTERN AEE_EXPORTS boolean OEMPhone_GetPhoneInfo2(OEMPhone * po, cm_ph_info_s_type * );
EXTERN AEE_EXPORTS boolean OEMPhone_GetCallInfo2(OEMPhone * po, CM_CALL_INFO_S_TYPE *);
EXTERN AEE_EXPORTS boolean OEMPhone_GetSystemInfo2(OEMPhone * po, CM_SS_INFO_S_TYPE *);
EXTERN AEE_EXPORTS int OEMPhone_GetMinWStr(OEMPhone * po, int nNAM, AECHAR * pDest, int nSize);
EXTERN AEE_EXPORTS int OEMPhone_GetMinStr(OEMPhone * po, int nNAM, char * pszDest, int nSize);
EXTERN AEE_EXPORTS int OEMPhone_SetRSSIThreshold(OEMPhone * po, uint8 nDelta);
EXTERN AEE_EXPORTS int OEMPhone_MakeCall(OEMPhone *po,
                                             cm_call_type_e_type cType,
                                             cm_srv_type_e_type nServ,
                                             word nServOpt,
                                             const byte * pNum,
                                             byte nNumLen,
                                             cm_digit_mode_e_type nDigitMode,
                                             const byte * pAlpha,
                                             byte nAlphaLen,
                                             CM_ACTIVATE_CODE_TYPE nActivateCode);

EXTERN AEE_EXPORTS int OEMPhone_AnswerCall(OEMPhone * po,cm_call_type_e_type ct);
EXTERN AEE_EXPORTS int OEMPhone_AnswerVoice(OEMPhone * po, cm_answer_voice_e_type nAnswer, dword dwDuration);
EXTERN AEE_EXPORTS int OEMPhone_EndCall(OEMPhone * po);
EXTERN AEE_EXPORTS int OEMPhone_Flash(OEMPhone * po,const byte* pNum, byte nNumLen);
EXTERN AEE_EXPORTS int OEMPhone_SetPrefMode(OEMPhone * po,
                                                cm_mode_pref_e_type nMode,
                                                CM_MODE_TERM_E_TYPE nModeTerm,
                                                uint32 dwDuration);
//#ifndef FEATURE_SD20
//EXTERN int OEMPhone_SetSysMode(OEMPhone * po,cm_sys_pref_e_type nPrefSys);
//#endif /* FEATURE_SD20 */
EXTERN AEE_EXPORTS int OEMPhone_SetNAM(OEMPhone * po, cm_nam_e_type nNam);
EXTERN AEE_EXPORTS int OEMPhone_SetOprtMode(OEMPhone * po,
                                             CM_OPRT_MODE_E_TYPE nMode);
EXTERN AEE_EXPORTS int OEMPhone_BurstDTMF(OEMPhone * po, uint8 nOn, uint8 nOff, const uint8 * pDigits, uint8 nLen);
EXTERN AEE_EXPORTS int OEMPhone_StartDTMF(OEMPhone * po, uint8 nDigit);
EXTERN AEE_EXPORTS int OEMPhone_StopDTMF(OEMPhone * po, uint16 nDuration);
EXTERN AEE_EXPORTS boolean OEMPhone_IsVoiceCall(OEMPhone * po);
EXTERN AEE_EXPORTS boolean OEMPhone_IsDataSupported(OEMPhone * po);
EXTERN AEE_EXPORTS boolean OEMPhone_IsOffline( OEMPhone *pme );
EXTERN AEE_EXPORTS boolean OEMPhone_InConversation(OEMPhone * po);

#endif    // OEMPHONE_H

