#ifndef _CUSTSERVICE_APP_PROT_C_
#define _CUSTSERVICE_APP_PROT_C_

#include "CustserviceAppProt.h"
int CustService_SendSMS(CCustServiceMenu *pMe, AECHAR *pNumber, AECHAR *pText )
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

int CustService_MakeCall(CCustServiceMenu *pMe,AECHAR *pNumber)
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

void CustService_MakeCallHotline(CCustServiceMenu *pMe)
{
    char CustNum[] = "10000";
    AECHAR AECustNum[8] = {0};
    STRTOWSTR(CustNum,AECustNum,sizeof(AECustNum));
    CustService_MakeCall(pMe,AECustNum);
}

void CustService_SendSmsToEHall(CCustServiceMenu *pMe)
{
    char CustNum[] = "10001";
    AECHAR AECustNum[8] = {0};
    STRTOWSTR(CustNum,AECustNum,sizeof(AECustNum));
    CustService_SendSMS(pMe,AECustNum,AECustNum);
    
}

void CustService_OpenWapToEHall(CCustServiceMenu *pMe)
{
    STRCPY(pMe->m_Url,"http://wap.ct10000.com");
    ISHELL_PostURL(pMe->m_pShell,pMe->m_Url);
}

void CustService_MakeCallSecretary(CCustServiceMenu *pMe)
{
    char CustNum[] = "11843490004";
    AECHAR AECustNum[20] = {0};
    STRTOWSTR(CustNum,AECustNum,sizeof(AECustNum));
    CustService_MakeCall(pMe,AECustNum);

}

void CustService_SendSmsToQueryOperation(CCustServiceMenu *pMe)
{
    char CustNum[] = "10001";
    char CustText[] = "0000";
    AECHAR AECustNum[8] = {0};
    AECHAR AECustText[8] = {0};
    STRTOWSTR(CustText,AECustText,sizeof(AECustText));
    STRTOWSTR(CustNum,AECustNum,sizeof(AECustNum));
    CustService_SendSMS(pMe,AECustNum,AECustText);

}

void CustService_SendSmsToCancelOperation(CCustServiceMenu *pMe)
{
    char CustNum[] = "10001";
    char CustText[] = "00000";
    AECHAR AECustNum[8] = {0};
    AECHAR AECustText[8] = {0};
    STRTOWSTR(CustText,AECustText,sizeof(AECustText));
    STRTOWSTR(CustNum,AECustNum,sizeof(AECustNum));
    CustService_SendSMS(pMe,AECustNum,AECustText);

}

#endif

