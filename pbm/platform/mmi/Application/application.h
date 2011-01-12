/** ----------------------------------------------------------------------------
 * Application.h
 * 
 * Copyright(c) 2007 Anylook
 * 
 * 
 * History:
 * 
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2008.12.29            4 X 3 large icon style menu * list menu
 * 
 * -----------------------------------------------------------------------------
 */

#ifndef APPLICATION_H
#define APPLICATION_H
/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/
typedef struct _IApplication  IApplication;
 QINTERFACE(IApplication)
{
    INHERIT_IApplet(IApplication);
};

#define IApplication_AddRef(p)     GET_PVTBL(p,IApplication)->AddRef(p)
#define IApplication_Release(p)    GET_PVTBL(p,IApplication)->Release(p)
#define IApplication_HandleEvent(p,ec,wp,dw) GET_PVTBL(p,IApplication)->HandleEvent(p, ec, wp, dw)
#endif

