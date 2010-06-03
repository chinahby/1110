
#ifndef AEEALERT_H
#define AEEALERT_H

/*============================================================================
FILE:  AEEAlert.h

SERVICES:  AEE Alert Interface

GENERAL DESCRIPTION:
        Base level definitions, typedefs, etc. for AEE SuppSvc/

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE


when       who     what, where, why
-------    ---     ----------------------------------------------------------
10/14/04   SUN     Added new Alert Type
09/30/04   GAT     Added capability to get media ringer delay
07/01/04   RAG     Added capability for apps to delay media ringers
04/05/04   BPW     Added new alert types
12/16/03   BPW     Added base station signaling support
12/02/03   BPW     Linted
11/26/03   sun     Initial release.

=============================================================================*/
#include "AEE.h"
#include "AEEComdef.h"
#include "ALERT.BID"
#include "ALERT_NOTIFIER.BID"


/* Alerting
*/
typedef enum
{
 AEEALERT_STATE_ON =1,
 AEEALERT_STATE_OFF,
 AEEALERT_STATE_MAX = 0x10000000
}AEEALERT_STATE;

#define AEEALERT_CALLTYPE_VOICE   0
#define AEEALERT_CALLTYPE_CSDATA  1
#define AEEALERT_CALLTYPE_PSDATA  2
#define AEEALERT_CALLTYPE_VIDEO   3
#define AEEALERT_CALLTYPE_FAX     4
#define AEEALERT_CALLTYPE_TEST    5

typedef enum{
  AEEALERT_ALERT_NONE = 0,
  AEEALERT_ALERT_NORMAL,
  AEEALERT_ALERT_CALLWAITING,
  AEEALERT_ALERT_DIALTONE,
  AEEALERT_ALERT_RINGBACK,
  AEEALERT_ALERT_BUSY,
  AEEALERT_ALERT_ANSWER,
  AEEALERT_ALERT_INTERCEPT,
  AEEALERT_ALERT_ABBR_INTERCEPT,
  AEEALERT_ALERT_REORDER,
  AEEALERT_ALERT_ABBR_REORDER,
  AEEALERT_ALERT_ABBR_ALERT,
  AEEALERT_ALERT_FADE_TONE,
  AEEALERT_ALERT_SPECIAL_INFO,
  AEEALERT_ALERT_CONFIRM,
  AEEALERT_ALERT_CALLWAIT_SIGNAL,
  AEEALERT_ALERT_PIP,
  AEEALERT_ALERT_ISDN_NORMAL,
  AEEALERT_ALERT_ISDN_INTERGROUP,
  AEEALERT_ALERT_ISDN_SPECIAL,
  AEEALERT_ALERT_ISDN_PING,
  AEEALERT_ALERT_IS54B_LONG_M, 
  AEEALERT_ALERT_IS54B_LONG_H, 
  AEEALERT_ALERT_IS54B_LONG_L,
  AEEALERT_ALERT_IS54B_SS_M, 
  AEEALERT_ALERT_IS54B_SS_H, 
  AEEALERT_ALERT_IS54B_SS_L,
  AEEALERT_ALERT_IS54B_SSL_M, 
  AEEALERT_ALERT_IS54B_SSL_H, 
  AEEALERT_ALERT_IS54B_SSL_L,
  AEEALERT_ALERT_IS54B_SS2_M, 
  AEEALERT_ALERT_IS54B_SS2_H, 
  AEEALERT_ALERT_IS54B_SS2_L,
  AEEALERT_ALERT_IS54B_SLS_M, 
  AEEALERT_ALERT_IS54B_SLS_H, 
  AEEALERT_ALERT_IS54B_SLS_L,
  AEEALERT_ALERT_IS54B_SSSS_M, 
  AEEALERT_ALERT_IS54B_SSSS_H, 
  AEEALERT_ALERT_IS54B_SSSS_L,
  AEEALERT_ALERT_IS54B_PBX_LONG_M, 
  AEEALERT_ALERT_IS54B_PBX_LONG_H,                                 
  AEEALERT_ALERT_IS54B_PBX_LONG_L,
  AEEALERT_ALERT_IS54B_PBX_SS_M, 
  AEEALERT_ALERT_IS54B_PBX_SS_H,                               
  AEEALERT_ALERT_IS54B_PBX_SS_L,
  AEEALERT_ALERT_IS54B_PBX_SSL_M, 
  AEEALERT_ALERT_IS54B_PBX_SSL_H,                                
  AEEALERT_ALERT_IS54B_PBX_SSL_L,
  AEEALERT_ALERT_IS54B_PBX_SLS_M, 
  AEEALERT_ALERT_IS54B_PBX_SLS_H,                                 
  AEEALERT_ALERT_IS54B_PBX_SLS_L,
  AEEALERT_ALERT_IS54B_PBX_SSSS_M, 
  AEEALERT_ALERT_IS54B_PBX_SSSS_H,
  AEEALERT_ALERT_IS54B_PBX_SSSS_L,
  AEEALERT_ALERT_IS53A_PPPP_M, 
  AEEALERT_ALERT_IS53A_PPPP_H,                                
  AEEALERT_ALERT_IS53A_PPPP_L,
  AEEALERT_ALERT_MAX = 0x10000000
}AEEALERTType;


#define NMASK_ALERT_ONOFF         0x0001
#define NMASK_ALERT_MUTED         0x0002
#define NMASK_ALERT_ANSWER        0x0004

typedef struct IALERT IALERT;
typedef struct IALERT_NOTIFIER IALERT_NOTIFIER;

AEEINTERFACE(IALERT_NOTIFIER)
{
  INHERIT_INotifier(IALERT_NOTIFIER);
};


AEEINTERFACE(IALERT)
{
  INHERIT_IQueryInterface(IALERT);

  int (*StartAlerting)(IALERT *po, uint8 call_type,
                       AECHAR *phone_number, AEEALERTType alert_type);
  int (*StopAlerting)(IALERT *po);
  int (*SetMuteCtl)(IALERT *po, boolean mute_on);
  int (*SetRect)(IALERT *po, AEERect*  tRect);
  int (*GetRect)(IALERT *po, AEERect *pRect);
  int (*GetInfo)(IALERT *po, uint8 *call_type,
                 AECHAR *phone_number, uint16 phoneNumberBufSize, AEEALERTType *alert_type);
  int (*SetMediaRingerDelay)(IALERT *po, int32 nMediaRingerDelay);
  int (*GetMediaRingerDelay)(IALERT *po, int32 *pnMediaRingerDelay);
};

#define IALERT_AddRef(p)          AEEGETPVTBL(p,IALERT)->AddRef(p)
#define IALERT_Release(p)         AEEGETPVTBL(p,IALERT)->Release(p)
#define IALERT_QueryInterface(p, clsid, pp)                             \
        AEEGETPVTBL(p,IALERT)->QueryInterface(p,clsid,pp)

#define IALERT_StartAlerting(po, call_type, phone_number, alert_type)   \
        AEEGETPVTBL(po,IALERT)->StartAlerting(po, call_type,            \
                                              phone_number, alert_type)
#define IALERT_StopAlerting(po)                                         \
        AEEGETPVTBL(po,IALERT)->StopAlerting(po)
#define IALERT_SetMuteCtl(po, mute_on)                                  \
        AEEGETPVTBL(po,IALERT)->SetMuteCtl(po, mute_on)
#define IALERT_SetRect(po, tRect)                                       \
        AEEGETPVTBL(po,IALERT)->SetRect(IALERT *po, AEERect tRect)
#define IALERT_GetRect(po, pRect)                                       \
        AEEGETPVTBL(po,IALERT)->GetRect(IALERT *po, AEERect *pRect)
#define IALERT_GetInfo(po, call_type, phone_number, phoneNumberBufSize, alert_type)         \
        AEEGETPVTBL(po,IALERT)->GetInfo(po, call_type, phone_number, phoneNumberBufSize, alert_type)
#define IALERT_SetMediaRingerDelay(po, delay) \
	    AEEGETPVTBL(po,IALERT)->SetMediaRingerDelay(po, delay)
#define IALERT_GetMediaRingerDelay(po, pDelay) \
	    AEEGETPVTBL(po,IALERT)->GetMediaRingerDelay(po, pDelay)

#endif //#ifndef AEEALERT_H
