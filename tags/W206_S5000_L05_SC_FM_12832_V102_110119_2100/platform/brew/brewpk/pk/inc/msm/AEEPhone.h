/*======================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

This file is covered by the licensing terms for CDMA 
ASIC licensees.  Under terms of those licensing agreements, 
this file MUST NOT be transmitted in whole or in part 
to any party who has not been licensed for use of the
QUALCOMM CDMA technologies. 

FILE:  AEEPhone.h

SERVICES:  Extended AEE Phone/TAPI Services

GENERAL DESCRIPTION:
	Base level definitions, typedefs, etc. for AEE Phone Services

        Copyright © 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#ifndef AEEPHONE_H
#define AEEPHONE_H
#include "OEMFeatures.h"

#include "cm.h"
#include "AEE.h"

/*
** SOME TYPE DEFINITIONS THAT HAPPEN TO HAVE CHANGED FOR MULTIMODE ARCH
*/
#ifdef FEATURE_MULTIMODE_ARCH
typedef cm_mm_call_info_s_type    CM_CALL_INFO_S_TYPE;
typedef cm_mm_ss_info_s_type      CM_SS_INFO_S_TYPE;
typedef cm_activate_code_e_type   CM_ACTIVATE_CODE_TYPE;
typedef cm_pref_term_e_type       CM_MODE_TERM_E_TYPE; 
typedef sys_oprt_mode_e_type      CM_OPRT_MODE_E_TYPE;
#else
typedef cm_call_info_s_type       CM_CALL_INFO_S_TYPE;
typedef cm_ss_info_s_type         CM_SS_INFO_S_TYPE;
typedef cm_activate_code_type     CM_ACTIVATE_CODE_TYPE;
typedef cm_mode_term_e_type       CM_MODE_TERM_E_TYPE;
typedef cm_oprt_mode_e_type       CM_OPRT_MODE_E_TYPE;
#endif

typedef struct _IPhone  IPhone;

// Notification Masks...

#define NMASK_PH_PHONE_INFO   0x0100      // AEENotify->pData = cm_ph_info_s_type *
#define NMASK_PH_CALL_INFO    0x0200      // AEENotify->pData = cm_mm_call_info_s_type/cm_call_info_s_type *
#define NMASK_PH_SS_INFO      0x0400      // AEENotify->pData = cm_mm_ss_info_s_type/cm_ss_info_s_type *
#define NMASK_PH_RAW_SMS      0x0800      // AEENotify->pData = wms_client_message_s_type/uasms_client_message_type *

   
// Interface

QINTERFACE(IPhone)
{
   DECLARE_IBASE(IPhone)
   DECLARE_INOTIFIER(IPhone)

   const cm_ph_info_s_type *        (*GetPhoneInfo)(IPhone * po);
   const CM_CALL_INFO_S_TYPE *      (*GetCallInfo)(IPhone * po);
   const CM_SS_INFO_S_TYPE *        (*GetSystemInfo)(IPhone * po);
   int                              (*GetMinWStr)(IPhone * po, int nNAM, AECHAR * pDest, int nSize);
   int                              (*GetMinStr)(IPhone * po, int nNAM, char * pszDest, int nSize);
   int                              (*SetRSSIThreshold)(IPhone * po, uint8 nDelta);
   int                              (*MakeCall)(IPhone *po,
                                                cm_call_type_e_type cType,
                                                cm_srv_type_e_type nServ,
                                                word nServOpt,
                                                const byte * pNum,
                                                byte nNumLen,
                                                cm_digit_mode_e_type nDigitMode,
                                                const byte * pAlpha,
                                                byte nAlphaLen,
                                                CM_ACTIVATE_CODE_TYPE nActivateCode);

   int                              (*AnswerCall)(IPhone * po,cm_call_type_e_type ct);
   int                              (*AnswerVoice)(IPhone * po, cm_answer_voice_e_type nAnswer, dword dwDuration);
   int                              (*EndCall)(IPhone * po);
   int                              (*Flash)(IPhone * po,const byte* pNum, byte nNumLen);
   int                              (*SetPrefMode)(IPhone * po,
                                                   cm_mode_pref_e_type nMode,
                                                   CM_MODE_TERM_E_TYPE nModeTerm,
                                                   uint32 dwDuration);
   int                           (*SetNAM)(IPhone * po, cm_nam_e_type nNam);
   int                           (*SetOprtMode)(IPhone * po,
                                                CM_OPRT_MODE_E_TYPE nMode);
   int                           (*BurstDTMF)(IPhone * po, uint8 nOn, uint8 nOff, const uint8 * pDigits, uint8 nLen);
   int                           (*StartDTMF)(IPhone * po, uint8 nDigit);
   int                           (*StopDTMF)(IPhone * po, uint16 nDuration);
   int                           (*OnCallStatus)(IPhone * po,PFNNOTIFY pfn,void * pUser,uint32 dwDelay,uint16 wFlags);
   boolean                       (*IsVoiceCall)(IPhone * po);
   boolean                       (*IsDataSupported)(IPhone * po);
};

#define IPHONE_AddRef(p)                                 GET_PVTBL(p,IPhone)->AddRef(p)
#define IPHONE_Release(p)                                GET_PVTBL(p,IPhone)->Release(p)
#define IPHONE_GetPhoneInfo(p)                           GET_PVTBL(p,IPhone)->GetPhoneInfo(p)
#define IPHONE_GetCallInfo(p)                            GET_PVTBL(p,IPhone)->GetCallInfo(p)
#define IPHONE_GetSystemInfo(p)                          GET_PVTBL(p,IPhone)->GetSystemInfo(p)
#define IPHONE_GetMinWStr(p,nn,pd,n)                     GET_PVTBL(p,IPhone)->GetMinWStr(p,nn,pd,n)
#define IPHONE_GetMinStr(p,nn,pd,n)                      GET_PVTBL(p,IPhone)->GetMinStr(p,nn,pd,n)
#define IPHONE_SetRSSIThreshold(p,n)                     GET_PVTBL(p,IPhone)->SetRSSIThreshold(p,n)
#define IPHONE_MakeCall(p,ct,ns,no,pn,nn,nd,pa,na,nac)   GET_PVTBL(p,IPhone)->MakeCall(p,ct,ns,no,pn,nn,nd,pa,na,nac)
#define IPHONE_AnswerCall(p,ct)                          GET_PVTBL(p,IPhone)->AnswerCall(p,ct)
#define IPHONE_AnswerVoice(p,na,dw)                      GET_PVTBL(p,IPhone)->AnswerVoice(p,na,dw)
#define IPHONE_EndCall(p)                                GET_PVTBL(p,IPhone)->EndCall(p)
#define IPHONE_Flash(p,pn,n)                             GET_PVTBL(p,IPhone)->Flash(p,pn,n)
#define IPHONE_SetPrefMode(p,n,nm,dw)                    GET_PVTBL(p,IPhone)->SetPrefMode(p,n,nm,dw)
#define IPHONE_SetNAM(p,n)                               GET_PVTBL(p,IPhone)->SetNAM(p,n)
#define IPHONE_SetOprtMode(p,n)                          GET_PVTBL(p,IPhone)->SetOprtMode(p,n)
#define IPHONE_BurstDTMF(p,nOn,nOff,pd,n)                GET_PVTBL(p,IPhone)->BurstDTMF(p,nOn,nOff,pd,n)
#define IPHONE_StartDTMF(p,n)                            GET_PVTBL(p,IPhone)->StartDTMF(p,n)
#define IPHONE_StopDTMF(p,n)                             GET_PVTBL(p,IPhone)->StopDTMF(p,n)
#define IPHONE_OnCallStatus(p,pfn,pu,dwd,bs)             GET_PVTBL(p,IPhone)->OnCallStatus(p,pfn,pu,dwd,bs)
#define IPHONE_OnCallEnd(p,pfn,pu,dwd,bs)                GET_PVTBL(p,IPhone)->OnCallStatus(p,pfn,pu,dwd,bs)
#define IPHONE_IsVoiceCall(p)                            GET_PVTBL(p,IPhone)->IsVoiceCall(p)
#define IPHONE_IsDataSupported(p)                        GET_PVTBL(p,IPhone)->IsDataSupported(p)

#endif    // AEEPHONE_H

