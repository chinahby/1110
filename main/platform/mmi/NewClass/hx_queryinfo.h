#ifndef __HX_QUERYINFO_H__
#define __HX_QUERYINFO_H__
#include "AEE.h"
#include "AEEComdef.h"
#include "AEEInterface.h"

typedef enum 
{
    IHXQUERYINFO_GET_MUSICSTATE_STATUS
}IHXQUERYINFO_e;

typedef struct IHXQueryInfo IHXQueryInfo;

#define INHERIT_IHXQueryInfo(iname)\
        INHERIT_IQueryInterface(iname);\
        int (*IHXQueryInfo_GetInfo)(iname* p##iname,IHXQUERYINFO_e eType,void *buf)


AEEINTERFACE(IHXQueryInfo)
{
    INHERIT_IHXQueryInfo(IHXQueryInfo);
};

struct IHXQueryInfo
{
    const AEEVTBL(IHXQueryInfo) *pvt;
    int nRefs;
    void* pData;
};



#endif













