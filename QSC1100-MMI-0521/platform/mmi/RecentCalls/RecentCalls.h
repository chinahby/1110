#ifndef RECENTCALL_H
#define RECENTCALL_H

/*==============================================================================
// 文件：
//        recentcalls.h
//        2007-11-18 陈喜玲草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook
//        
// 文件说明：
//        
// 作者：2007-11-18
// 创建日期：陈喜玲
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
07-11-18         陈喜玲         初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/


/*==============================================================================
                                 宏定义和常数
==============================================================================*/


/*==============================================================================
                                 类型定义
==============================================================================*/
//#include "aeecalllist.h"
#include "CallInfo.h"
typedef struct _Record{
    AECHAR *number;        // number
    AECHAR *name;           // name
    //boolean  is_number;      // number only or not
    callPIType pi;                // PI
    uint16 category;          // call type
    uint16 counter;            // call counter
    uint32 time_stamp;     // years to seconds
    uint32 duration;          // call duration
    uint16 wIndex;            //Index
}Record;

typedef struct _sSelectRecordListNode sSelectRecordListNode ;

struct _sSelectRecordListNode
{
    sSelectRecordListNode* pPreNode;
    sSelectRecordListNode* pNextNode;
    Record                        SelectRecord;
};

//#include "recentcalls_priv.h"
typedef boolean (*PFNSELECTEDRECENTCALLSCB)(sSelectRecordListNode* pSelectRecordListRoot);

typedef enum _SelectRecordType{
    SELECT_ONE_NUMBER,
    SELECT_MULTI_NUMBER
}SelectRecordType;
/*==============================================================================
                                 类接口定义
==============================================================================*/
typedef struct _IRecentCalls  IRecentCalls;
QINTERFACE(IRecentCalls)
{
    INHERIT_IApplet(IRecentCalls);
    void (*SendList)(IRecentCalls *p);
    void (*MissedList)(IRecentCalls *p);
    void (*CallList)(IRecentCalls *p, PFNSELECTEDRECENTCALLSCB cb, SelectRecordType t); //modfied by chengxiao 2009.02.19
};


/*==============================================================================
                                 类接口函数定义
==============================================================================*/
#define IRecentCalls_AddRef(p)                GET_PVTBL(p,IRecentCalls)->AddRef(p)

#define IRecentCalls_Release(p)                GET_PVTBL(p,IRecentCalls)->Release(p)

#define IRecentCalls_HandleEvent(p,ec,wp,dw) \
                                    GET_PVTBL(p,IRecentCalls)->HandleEvent(p, ec, wp, dw)

#define IRecentCalls_SendList(p)    GET_PVTBL(p,IRecentCalls)->SendList(p)

#define IRecentCalls_MissedList(p)  GET_PVTBL(p,IRecentCalls)->MissedList(p)

#define IRecentCalls_CallList(p,cb,t) GET_PVTBL(p,IRecentCalls)->CallList(p,cb,t) //modfied by chengxiao 2009.02.19
#endif
//RECENTCALL_H

