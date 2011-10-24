
#include "hx_queryinfo.h"
#include "AEEStdlib.h"
#include "err.h"

#define AEECLSID_HX_QUERYINFO 0x01000201

static uint32 IHXQueryInfo_AddRef(IHXQueryInfo* pIHXQueryInfo)
{   
    ERR_FATAL("IHXQueryInfo_AddRef invoke",0,0,0);
    if (pIHXQueryInfo == NULL)
    {
        ERR_FATAL("IHXQueryInfo_AddRef param error",0,0,0);    
        return EFAILED;
    }
    pIHXQueryInfo->nRefs++;

    return pIHXQueryInfo->nRefs;
}


static uint32 IHXQueryInfo_Release(IHXQueryInfo* pIHXQueryInfo)
{
    ERR_FATAL("IHXQueryInfo_Release invoke",0,0,0);    
    pIHXQueryInfo->nRefs--;
    if(pIHXQueryInfo->nRefs == 0)
    {
        FREE(pIHXQueryInfo);
		return SUCCESS;
    }
    return pIHXQueryInfo->nRefs;
}






static int IHXQueryInfo_QueryInterface(IHXQueryInfo* pIHXQueryInfo, AEECLSID ClsID, void** ppInterface)
{
    
	*ppInterface = NULL;

    if(ClsID == AEECLSID_QUERYINTERFACE || ClsID == AEECLSID_HX_QUERYINFO)
    {
    }

    return SUCCESS;
}


static int IHXQueryInfo_GetInfo(IHXQueryInfo* pIHXQueryInfo,IHXQUERYINFO_e eType,void *buf)
{
    switch(eType)
    {
        case IHXQUERYINFO_GET_MUSICSTATE_STATUS:
        {
            ERR_FATAL("IHXQueryInfo_GetInfo IHXQUERYINFO_GET_MUSICSTATE_STATUS",0,0,0);    
        }break;

        default:
            break;
    }

    return SUCCESS;
    
}



static const AEEVTBL(IHXQueryInfo) gIIHXQueryInfofuncs=
{
    IHXQueryInfo_AddRef,
    IHXQueryInfo_Release,
    IHXQueryInfo_QueryInterface,
    IHXQueryInfo_GetInfo
};


int IHXQueryInfo_New(IShell *pIShell,AEECLSID ClsID,void** ppInterface)
{
    int ret = 1;
    IHXQueryInfo *pMe;

    ERR_FATAL("IHXQueryInfo_New Enter",0,0,0);
    if (pIShell==NULL||ClsID!=AEECLSID_HX_QUERYINFO||ppInterface==NULL)
    {
        ERR_FATAL("IHXQueryInfo_New params ClsID = %d,failed",0,0,0);
        return EBADPARM;
    }
        
    pMe = (IHXQueryInfo*)MALLOC(sizeof(IHXQueryInfo));
    
    if (pMe != NULL)
    {         
        MEMSET(pMe,0x00,sizeof(IHXQueryInfo));
        pMe->pvt = &gIIHXQueryInfofuncs;                
        pMe->nRefs = 1;
        *ppInterface = (void *)pMe;
        ret = SUCCESS;
    }
    else
    {    
        ERR_FATAL("IHXQueryInfo_New MALLOC failed",0,0,0);
        ret= EFAILED;
    }
    ERR_FATAL("IHXQueryInfo_New Leave ret = %d",ret,0,0);
    return ret;
}





