#ifndef _ESURFING_APP_PROT_H_
#define _ESURFING_APP_PROT_H_

#include "Esurfing_priv.h"


extern int Esurfing_SendSMS(CEsurfingMenu *pMe, AECHAR *pNumber, AECHAR *pText );
extern int Esurfing_MakeCall(CEsurfingMenu *pMe,AECHAR *pNumber);
extern void Esurfing_MakeCallBestTone(CEsurfingMenu *pMe);
extern void Esurfing_MakeCalliMuisc(CEsurfingMenu *pMe);
extern void Esurfing_OpenWapToBestTone(CEsurfingMenu *pMe);
extern void Esurfing_OpenWapToiTv(CEsurfingMenu *pMe);
extern void Esurfing_OpenWapToVNet(CEsurfingMenu *pMe);
#endif

