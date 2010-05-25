#ifndef _ESURFING_APP_PROT_C_
#define _ESURFING_APP_PROT_C_

#include "EsurfingAppProt.h"

int Esurfing_SendSMS(CEsurfingMenu *pMe, AECHAR *pNumber, AECHAR *pText )
{
    IWmsApp *pSMSApp;

    //ASSERT(pMe != NULL);
    
    if ((pNumber == NULL) && (NULL == pText))
    {
        return EFAILED;
    }
    
    if (SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                          AEECLSID_WMSAPP,
                                          (void**)&pSMSApp))
    {
        return EFAILED;
    }

    if (SUCCESS != IWmsApp_SendTextMessageExt(pSMSApp, pNumber, pText))
    {
        (void)IWmsApp_Release(pSMSApp);
        pSMSApp = NULL;
        return EFAILED;
    }    
    
    (void)IWmsApp_Release(pSMSApp);
    pSMSApp = NULL;
    return SUCCESS;    

}

int Esurfing_MakeCall(CEsurfingMenu *pMe,AECHAR *pNumber)
{
    //ASSERT(pMe != NULL); 
   
    if( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                           AEECLSID_DIALER,
                                           (void **)&pMe->m_pCallApp))
    {
        return EFAILED;
    }
    
    ICallApp_CallNumber(pMe->m_pCallApp,pNumber);
    
    if (pMe->m_pCallApp != NULL) 
    {
        ICallApp_Release(pMe->m_pCallApp);
        pMe->m_pCallApp = NULL;
    }
    
    return SUCCESS;    
}

void Esurfing_MakeCallBestTone(CEsurfingMenu *pMe)
{
    char BtoneNum[] = "118114";
    AECHAR AEBtoneNum[8] = {0};
    STRTOWSTR(BtoneNum,AEBtoneNum,sizeof(AEBtoneNum));
    Esurfing_MakeCall(pMe,AEBtoneNum);

}

void Esurfing_MakeCalliMuisc(CEsurfingMenu *pMe)
{
    char iMusicNum[] = "118101";
    AECHAR AEiMusicNum[8] = {0};
    STRTOWSTR(iMusicNum,AEiMusicNum,sizeof(AEiMusicNum));
    Esurfing_MakeCall(pMe,AEiMusicNum);

}

void Esurfing_OpenWapToBestTone(CEsurfingMenu *pMe)
{
    STRCPY( pMe->m_URL, "http://wap.118114.cn");
    ISHELL_PostURL(pMe->m_pShell,pMe->m_URL);                 
}

void Esurfing_OpenWapToiTv(CEsurfingMenu *pMe)
{ 
    STRCPY( pMe->m_URL, "http://streaming.vnet.mobi");
    ISHELL_PostURL(pMe->m_pShell,pMe->m_URL);             
}

void Esurfing_OpenWapToVNet(CEsurfingMenu *pMe)
{
    STRCPY( pMe->m_URL, "http://streaming.vnet.mobi");
    ISHELL_PostURL(pMe->m_pShell,pMe->m_URL);    

}
#endif

