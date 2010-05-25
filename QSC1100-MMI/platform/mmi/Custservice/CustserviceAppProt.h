#ifndef _MORSE_APP_PROT_H_
#define _MORSE_APP_PROT_H_

#include "Custservice_priv.h"


extern int CustService_SendSMS(CCustServiceMenu *pMe, AECHAR *pNumber, AECHAR *pText );
extern int CustService_MakeCall(CCustServiceMenu *pMe,AECHAR *pNumber);
extern void CustService_MakeCallHotline(CCustServiceMenu *pMe);
extern void CustService_OpenWapToEHall(CCustServiceMenu *pMe);
extern void CustService_SendSmsToEHall(CCustServiceMenu *pMe);
extern void CustService_MakeCallSecretary(CCustServiceMenu *pMe);
extern void CustService_SendSmsToQueryOperation(CCustServiceMenu *pMe);
extern void CustService_SendSmsToCancelOperation(CCustServiceMenu *pMe);
#endif

