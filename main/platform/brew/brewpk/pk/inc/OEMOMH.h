#ifndef __OEMOMH__
#define __OEMOMH__

#include "OEMFeatures.h"

#ifdef FEATURE_OEMOMH
#include "AEEConfig.h"
#include "AEECardSession.h"
#include "AEEDB.h"
#include "AEECardSessionNotifier.h"
#include "AEEDownload.h"
#include "OEMCardSessionNotifier.h"

#if 0
/* The BREW Elementary File on the RUIM card */
struct tagDevice 
{ 
    IConfig *m_pIConfig; 
    OEMProvisionInfo pInfo; 
}; 
typedef struct tagDevice DEVICE; 

void Get_config(DEVICE *pMe); 
void Config_Data_Cb(void* po); 
void Config_Len_Cb(void* po); 
#endif

extern int OEMOMH_Init(void);
extern void OEMOMH_Cleanup(void);
extern void OEMOMH_Notify(uint32 dwMask, void *pNotifyData, uint32 sClsId);
extern void OEMOMH_Notify2();
extern int OEMOMH_GetConfig(AEEConfigItem i, void * pBuff, int nSize);

	
#else

#define OEMOMH_Init()
#define OEMOMH_Cleanup()
#define OEMOMH_Notify(dwMask, pNotifyData, sClsId)
#define OEMOMH_GetConfig(item, pBuff, nSize)

#endif

#endif
