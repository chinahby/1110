/** ----------------------------------------------------------------------------
 * MobileTracker.h
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

#ifndef MOBILETRACKER_H
#define MOBILETRACKER_H
/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/
//Applet启动的不同状态
typedef enum _MobileTrackerServiceType 
{
    STARTMETHOD_PLAYER,       // ISHELL_StartApplet()
    STARTMETHOD_DATA          // ISHELL_StartApplet()
}MobileTrackerServiceType;

typedef struct _IMobileTracker  IMobileTracker;
 QINTERFACE(IMobileTracker)
{
    INHERIT_IApplet(IMobileTracker);

    // 外部进入MAINMENU的接口函数
    int (*MobileTrackerServiceType)(IMobileTracker *p, MobileTrackerServiceType eStype);    
};

#define IMOBILETRACKER_AddRef(p)     GET_PVTBL(p,IMobileTracker)->AddRef(p)
#define IMOBILETRACKER_Release(p)    GET_PVTBL(p,IMobileTracker)->Release(p)
#define IMOBILETRACKER_HandleEvent(p,ec,wp,dw) GET_PVTBL(p,IMobileTracker)->HandleEvent(p, ec, wp, dw)


// 外部快速进入到数据服务列表
#define IMOBILETRACKER_StartData(p)            GET_PVTBL(p,IMobileTracker)->MobileTrackerService(p, STARTMETHOD_DATA);



#endif
