#ifndef _OEMWMS_CACHEINFO_H
#define    _OEMWMS_CACHEINFO_H
 
/*==============================================================================
//           W M S   C A C A H E I N F O   M O D U L E
// 文件：
//        OEMWMS_CacheInfo.h
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        短信息 Cache Info 结构的定义、建立及维护。供 WMSApp 快速访问存储的短信
//        用。         
//
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/
#include "comdef.h"     // Get DMSS type declarations.
#ifndef WIN32
#include "wmsi.h"
#endif
#include "AEEStdDef.h"

/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
 

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/
#ifdef FEATURE_SUPPORT_ID
#define     LONGSMS_MAX_PACKAGES        10
#else
#define     LONGSMS_MAX_PACKAGES        5
#endif

/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/
// cache info 节点数据结构定义
typedef struct  _wms_cache_info_node
{
    struct _wms_cache_info_node *pPrev; // 上一节点指针
    struct _wms_cache_info_node *pNext; // 下一节点指针
    
    uint32      dwTime;                 // 消息时间戳
    uint32      message_id;             // 消息ID
#ifdef FEATURE_SMS_UDH
    uint16      msg_ref;                // 消息参考号
    uint8       total_sm;               // 消息数据包总数
    uint8       seq_num;                // 消息序号
    
    // 能够合并的长短信各数据包节点指针列表
    struct _wms_cache_info_node    **pItems; 
#endif    
    wms_priority_e_type             priority;   // 消息优先级        
    wms_privacy_e_type              privacy;    // 消息密级
    wms_bd_message_type_e_type      msg_type;   // 消息类型
    wms_message_tag_e_type          msg_tag;    // 消息标记
    
    wms_message_index_type          index;      // 消息在存储空间上的索引号
    char                            *pszNum;    // 始发或接收号码
    AECHAR                          *pwszTitle; // 消息标题
    wms_memory_store_e_type         mem_store;  // 消息存储位置
    int                             xuhao;      // 消息序号，用于消息列表菜单
} wms_cache_info_node;

// cache info 双向链表数据结构定义
typedef struct  _wms_cache_info_list
{
    wms_cache_info_node     *pHead;     // 头节点
    wms_cache_info_node     *pTail;     // 尾节点
    wms_cache_info_node     *pCur;      // 当前节点
    uint16                  nCount;     // 包含节点总数，用于空间统计
    uint16                  nNews;      // 未读或未发送成功或预约中消息条数，用于菜单列表
    uint16                  nAll;       // 消息总条数，用于菜单列表
    uint16                  nOnUIMS;    // 位于卡上的节点数, 对于预约短信，用此变量表示预约失败的分支数
    uint16                  nBranches;  // 分支数，用于显示消息条数, 对于预约短信，用此变量表示预约中的分支数
}wms_cache_info_list;


/*==============================================================================

                                 函数声明

==============================================================================*/


/*==============================================================================
函数: 
    wms_get_cacheinfolist
       
说明: 
    函数用来获取短消息各存储箱对应的 cache 链表指针。
       
参数: 
    ebox [in]: 短消息存储箱
    
返回值:
    成功: 短消息存储箱对应的 cache 链表指针。
    失败: NULL
       
备注:
     
==============================================================================*/
wms_cache_info_list * wms_get_cacheinfolist(wms_box_e_type ebox);

/*==============================================================================
函数: 
    OEMWMS_MsgInfoCacheCbExt
       
说明: 
    Callback function for Message Info Cache. The cached information is used to
    display the message lists
       
参数: 
    pMsg [in]: 消息 wms_client_message_s_type 结构指针。
    delcachetag [in]: only used while pMsg->msg_hdr.tag == WMS_TAG_NONE
    
返回值:
    none
       
备注:
    函数在插入节点前，先检索 cache info 链表若存在则相应节点则删除之。
    然后作为新节点加入。pMsg->msg_hdr.tag == WMS_TAG_NONE 表示要删除
    Cache 节点，此时要用到 delcachetag
    
==============================================================================*/
void OEMWMS_MsgInfoCacheCbExt(const wms_client_message_s_type  *pMsg,
                              wms_message_tag_e_type    delcachetag);

/*==============================================================================
函数: 
    wms_cacheinfolist_enumbegin
       
说明: 
    cache info 链表枚举初始化函数
       
参数: 
    ebox [in]: 短消息存储箱。
    
返回值:
    none
       
备注:
    
==============================================================================*/
void wms_cacheinfolist_enumbegin(wms_box_e_type ebox);

/*==============================================================================
函数: 
    wms_cacheinfolist_enumtoend
       
说明: 
    cache info 链表枚举到尾节点
       
参数: 
    ebox [in]: 短消息存储箱。
    
返回值:
    none
       
备注:
    
==============================================================================*/
void wms_cacheinfolist_enumtoend(wms_box_e_type ebox);

/*==============================================================================
函数: 
    wms_cacheinfolist_enumnext
       
说明: 
    cache info 链表枚举下一个节点的函数
       
参数: 
    ebox [in]: 短消息存储箱。
    
返回值:
    none
       
备注:
    第一次调用此函数前必须先调用 wms_cacheinfolist_enumbegin() 函数。
==============================================================================*/
wms_cache_info_node *wms_cacheinfolist_enumnext(wms_box_e_type ebox);

/*==============================================================================
函数: 
    wms_cacheinfolist_enumprev
       
说明: 
    cache info 链表枚举上一个节点的函数
       
参数: 
    ebox [in]: 短消息存储箱。
    
返回值:
    none
       
备注:
    第一次调用此函数前必须先调用 wms_cacheinfolist_enumbegin() 函数。
==============================================================================*/
wms_cache_info_node *wms_cacheinfolist_enumprev(wms_box_e_type ebox);

/*==============================================================================
函数: 
    wms_cacheinfolist_enumtoxuhao
       
说明: 
    cache info 链表枚举跳至指定序号的节点的函数
       
参数: 
    ebox [in]: 短消息存储箱。
    xuhao [in]: 指定序号。
    
返回值:
    none
       
备注:
   调用此函数前必须先调用 wms_cacheinfolist_updatexuhao() 函数。
==============================================================================*/
void wms_cacheinfolist_enumtoxuhao(wms_box_e_type ebox, int xuhao);

/*==============================================================================
函数: 
    wms_cacheinfolist_getcounts
       
说明: 
    函数返回链表节点数及其中新信息项数。
       
参数: 
    box [in]: 短消息存储箱。
    pNew[in]: 用于返回新消息数目的 buffer 指针。
    pOnUIM[in]: 用于返回存于卡上消息数目的 buffer 指针。
    pTotal[in]: 用于返回消息总数目的 buffer 指针。
    
返回值:
    链表节点数
       
备注:

==============================================================================*/
void wms_cacheinfolist_getcounts(wms_box_e_type box, 
                                 uint16 *pNew, 
                                 uint16 *pOnUIM,
                                 uint16 *pTotal);
                                 
/*==============================================================================
函数: 
    wms_cacheinfolist_getnode
       
说明: 
    函数返回符合条件的节点指针。
       
参数: 
    eBox [in]: 消息箱类型
    mem_store [in]: 消息存储位置
    index[in]: 消息在存储空间的索引号。
    
返回值:
    NULL 或 符合条件的节点指针。
       
备注:

==============================================================================*/
wms_cache_info_node *wms_cacheinfolist_getnode(wms_box_e_type  eBox,
                                               wms_memory_store_e_type mem_store, 
                                               uint16 index);

/*==============================================================================
函数: 
    wms_cacheinfolist_updatenodetag
       
说明: 
    函数返回符合条件的节点指针。
       
参数: 
    mem_store [in]: 消息存储位置
    index[in]: 消息在存储空间的索引号。
    newtag[in]: 消息新的标识

返回值:
    TRUE: 更改成功
    FALSE:更改失败
       
备注:

==============================================================================*/
boolean wms_cacheinfolist_updatenodetag(wms_memory_store_e_type mem_store, 
                                     uint16 index,
                                     wms_message_tag_e_type newtag);
                                     
/*==============================================================================
函数: 
    wms_get_cacheinfolist
       
说明: 
    函数用来更新链表主干线节点序号。
       
参数: 
    ebox [in]: 短消息存储箱
    
返回值:
    成功: 短消息存储箱对应的 cache 链表指针。
    失败: NULL
       
备注:
     
==============================================================================*/
void wms_cacheinfolist_updatexuhao(wms_box_e_type ebox);
#endif // _OEMWMS_CACHEINFO_H
