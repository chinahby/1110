/** ----------------------------------------------------------------------------
 * IndexZoneApp.h
 * 
 * Copyright(c) 2012 zzg 
 * 
 * History:
 * 
 * when            who            what and why
 * -----------  -----------    -----------------------------
 * 2012_12_15   zzg             create
 * 
 * -----------------------------------------------------------------------------
 */

#ifndef INDEXZONEAPP_H
#define INDEXZONEAPP_H

/*==============================================================================                                 
                                相关数据类型定义                                 
==============================================================================*/


typedef struct _IIndexZoneApp  IIndexZoneApp;

 QINTERFACE(IIndexZoneApp)
{
    INHERIT_IApplet(IIndexZoneApp);      
};

#define IIndexZoneApp_AddRef(p)     				GET_PVTBL(p,IIndexZoneApp)->AddRef(p)
#define IIndexZoneApp_Release(p)    				GET_PVTBL(p,IIndexZoneApp)->Release(p)
#define IIndexZoneApp_HandleEvent(p,ec,wp,dw) 	GET_PVTBL(p,IIndexZoneApp)->HandleEvent(p, ec, wp, dw)


#endif
