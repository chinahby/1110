#ifndef _WMSUTIL_H
#define _WMSUTIL_H
/*==============================================================================
// 文件：
//        WMSUtil.h
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        短信息助手函数相关声明。
//
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/



/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/


/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/
#define WMSAPPU_FREE(ptr) {FREEIF(ptr); ptr = NULL;}
#define WMSAPPU_SYSFREE(ptr) if (ptr) { sys_free((void*)ptr); (ptr) = 0; }


/*==============================================================================

                                 函数声明

==============================================================================*/
/*==============================================================================
函数: 
    WMSUtil_SecsToDateString

说明: 
    将以秒表示的时间转换为字符串形式。

参数: 
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    dwTime [in]: 以计秒的时间。
    pBuf [out]: 用于存放时间字符串的缓冲区。
    nSize [in]: pBuf 的空间大小(单位：字节)。

返回值: 
    none

备注: 

==============================================================================*/
void WMSUtil_SecsToDateString(WmsApp* pMe,uint32 dwTime, AECHAR *pBuf, int nSize); 

/*==============================================================================
函数: 
    WMSUtil_HaveNoneASCIIChar

说明: 
    判断宽字符串中是否含有非ASCII字符。

参数: 
    pWstr [in]: 指向宽字符串。
    nPostion [out]: 非ASCII字符第一次出现的位置。

返回值: 
    若含有非ASCII字符，返回TRUE；否则，FALSE。

备注: 

==============================================================================*/
boolean WMSUtil_HaveNoneASCIIChar (AECHAR * pWstr, int * pPostion);

/*==============================================================================
函数: 
    WMSUtil_GetContactName

说明: 
    根据电话号码从电话本中查询人名。

参数: 
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    pwszNum [in]: 号码字符串。
    pwszName [in/out]: 用于返回人名的字符串Buf。
    wBufLen [in]: pwszName 最多容纳的字符数(AECHAR)

返回值: 
    none

备注: 

==============================================================================*/
void WMSUtil_GetContactName(WmsApp* pMe,  
                            const AECHAR *pwszNum, 
                            AECHAR *pwszName,
                            uint16 wBufLen);

/*==============================================================================
函数：
    WMSUtil_AVKType2AECHAR(AVKType key)

说明：
    将按键输入代笔的字符转换成相应的宽字符。

参数：
    key [in]：AVKType。

返回值：
    AECHAR
       
备注：

==============================================================================*/
AECHAR WMSUtil_AVKType2AECHAR(AVKType key);
/*==============================================================================
函数:
    WMSMessageStruct_Free

说明:
    函数重新设置消息结构各成员变量为缺省值。

参数:
    pMsg [in/out]: WMSMessageStruct_Free 数据结构指针。

返回值:
    none

备注:
        
==============================================================================*/
void WMSMessageStruct_Free(WmsApp * pMe);

/*==============================================================================
函数:
    WMSMessageStruct_Reset

说明:
    函数重新设置消息结构各成员变量为缺省值。

参数:
    pMsg [in/out]: WMSMessageStruct 数据结构指针。

返回值:
    none

备注:
        
==============================================================================*/
void WMSMessageStruct_Reset(WMSMessageStruct * pMsg);

/*==============================================================================
函数:
    WmsApp_ConvertClientMsgToMS

说明:
    函数将 wms_client_message_s_type 消息解析到内部结构 WMSMessageStruct 。

参数:
    pClMsg [in]: wms_client_message_s_type 数据结构指针。
    pOutMs [in/out]: WMSMessageStruct 数据结构指针。

返回值:
    none

备注:
        
==============================================================================*/
void WmsApp_ConvertClientMsgToMS(wms_client_message_s_type *pClMsg,
                                 WMSMessageStruct *pOutMs);


/*==============================================================================
函数:
    WMSExtApp_CallPhoneNumber

说明:
    本函数用于发起对指定号码的呼叫。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    szVMNumber [in/out]: 拨出号码字符串。
    bIPCall [in]: TRUE - IP Call; FALSE - Normal Call

返回值:
    none

备注:

==============================================================================*/
void WMSExtApp_CallPhoneNumber(WmsApp * pMe, AECHAR *szVMNumber, boolean bIPCall);

/*==============================================================================
函数:
    WMSExtApp_SaveExtractDetails

说明:
    通过Contact Applet将电话号码保存到通信簿数据库。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    wstrNum [in]: 待保存的号码字符串。
    wstrURL [in]: 待保存的 URL 字符串。
    wstrEmail [in]: 待保存的 e-mail 地址字符串。

返回值:
    none

备注:
    调用Contact Applet，WMS applet被挂起。

==============================================================================*/
void WMSExtApp_SaveExtractDetails(WmsApp *pMe, AECHAR  *wstrNum, AECHAR *wstrURL, AECHAR *wstrEmail);

/*==============================================================================
函数:
    WMSExtApp_GetAddresseeFromContactsApp

说明:
    本函数用于从通信簿数取号码。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eType [in]: 从电话本取号码时操作类型

返回值:
    无。

备注:
    调用Contact Applet，WMS applet被挂起。
        
==============================================================================*/
void WMSExtApp_GetAddresseeFromContactsApp(WmsApp *pMe, GetAddr_Type eType);

/*==============================================================================
函数:
    WmsApp_StorePrefs
       
说明:
    保存当前 Applet 配置数据。
       
参数:
    pMe [in]:指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    none
       
备注:
       
==============================================================================*/
void WmsApp_StorePrefs(WmsApp *pMe);

/*==============================================================================
函数:
    WmsApp_GetPrefs
       
说明:
    获取 Applet 配置数据。
       
参数:
    pMe [in]:指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    none
       
备注:
       
==============================================================================*/
void WmsApp_GetPrefs(WmsApp *pMe);

#ifdef FEATURE_SMS_UDH
/*==============================================================================
函数:
    WmsApp_GetMsgRef
       
说明:
    获取消息参考号。
       
参数:
    pMe [in]:指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    none
       
备注:
       
==============================================================================*/
uint8 WmsApp_GetMsgRef(WmsApp *pMe);
#endif

/*==============================================================================
函数:
    WmsApp_GetMsgIDNum
       
说明:
    获取消息 ID 号。
       
参数:
    pMe [in]:指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。
       
返回值:
    none
       
备注:
       
==============================================================================*/
uint16 WmsApp_GetMsgIDNum(WmsApp *pMe);

/*==============================================================================
函数:
    ConvertIntToBcd
       
说明:
    转换整数到 BCD 数字
       
参数:
    i [in]: 待转换整数。
       
返回值:
    转换后 BCD 数字
       
备注:
       
==============================================================================*/
uint8 ConvertIntToBcd(uint16 i);

/*==============================================================================
函数:
    ConvertMStoMcTime
       
说明:
    将以秒为单位的时间转换为 wms_timestamp_s_type 格式。
       
参数:
    sec [in]: 时间(单位: 秒)
    pmc_time [in]:指向 wms_timestamp_s_type 对象结构的指针。
       
返回值:
    none
       
备注:
       
==============================================================================*/
void ConvertMStoMcTime(uint32 sec, wms_timestamp_s_type *pmc_time);

/*==============================================================================
函数:
    WmsApp_PackPhrase

说明:
    函数根据 WMSMessageStruct 结构消息创建 wms_client_message_s_type 结构模板消息。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    pMs [in]: WMSMessageStruct 数据结构指针。
    bOutTemplate [in]: TRUE--输出 cdma_template FALSE--输出 cdma_message

返回值:
    none

备注:
    返回数据 buffer 系动态分配，由调用者负责释放。

==============================================================================*/
wms_client_message_s_type *WmsApp_PackPhrase(WmsApp *pMe, 
                                             WMSMessageStruct *pMs,
                                             boolean bOutTemplate);

/*==============================================================================
函数:
    WmsApp_BuildNumList

说明:
    分析消息，建立号码链表。

参数:
    pMe [in]: 指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。
    pmsMsg [in]:指向 WMSMessageStruct 结构的指针。

返回值:
    none

备注:
    本函数假定传入的消息结构保存的是接收到的消息。
==============================================================================*/
void WmsApp_BuildNumList(WmsApp *pMe, WMSMessageStruct *pmsMsg);

/*==============================================================================
函数:
    WmsApp_BuildMailAddList

说明:
    分析给定字符串，建立电子邮件地址链表。

参数:
    pMe [in]: 指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。
    wstrText [in]: 待分析字符串指针。

返回值:
    none

备注:
    邮件地址必须是只含除 "(),:;<>[\] 之外的且值大于0x20、小于0x7F的ASCII 字符,
    邮件地址最大长度为64个字符。
    见联通<<多媒体邮件终端功能规范>>
==============================================================================*/
void WmsApp_BuildMailAddList(WmsApp *pMe, AECHAR *wstrText);

/*==============================================================================
函数:
    WmsApp_BuildUrlList

说明:
    分析给定字符串，建立URL链表。

参数:
    pMe [in]: 指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。
    wstrText [in]: 待分析字符串指针。

返回值:
    none

备注:
    URL必须是只含除 "<> 之外的且值大于0x20、小于0x7F的ASCII 字符, 且最大长度低于
    256个字符。
    见联通<<多媒体邮件终端功能规范>>
==============================================================================*/
void WmsApp_BuildUrlList(WmsApp *pMe, AECHAR *wstrText);

/*==============================================================================
函数:
    GetMonthDays

说明:
    函数返回指定年指定月所含的天数。

参数:
    nYear [in]: 年。
    nMonth [in]: 月。

返回值:
    none

备注:

==============================================================================*/
int GetMonthDays(int nYear, int nMonth);

/*==============================================================================
函数: 
    WMSUtil_CalculateMessagesCount

说明: 
    判断发送给定字符串需要的短信条数。

参数: 
    pWstr [in]: 指向宽字符串。
    pencoding [in/out]: 用于返回当前使用的编码方式。
    pnMaxcharsOneitem [in/out]: 用于返回单条短信最多容纳的字符数的整数指针。

返回值: 
    发送给定字符串需要的短信条数。

备注: 

==============================================================================*/
int WMSUtil_CalculateMessagesCount(AECHAR *pWstr, 
                                   int *pencoding,
                                   int *pnMaxcharsOneitem);

/*==============================================================================
函数:
    WmsApp_SetMaxSize

说明:
    设置最大输入容量。

参数:
    pIText, 当前的text control。

返回值:
    当前文本控件允许输入的最大字符数。
备注:

==============================================================================*/
int WmsApp_SetMaxSize(ITextCtl *pIText);

#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE
/*==============================================================================
函数:
    wmsutil_formatmessage

说明:
    应以色列要求格式化发出的短信文本。

参数:
    pclt_msg [in/out]: wms_client_message_s_type 型指针。

返回值:
    none
    
备注:

==============================================================================*/
void wmsutil_formatmessage(wms_client_message_s_type *pclt_msg);
#endif

/*==============================================================================
函数:
    WMSExtApp_GetNumFromRecentCalls

说明:
    本函数用于从最近通话记录取号码。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    无。

备注:
    调用RecentCalls Applet，WMS applet被挂起。
        
==============================================================================*/
void WMSExtApp_GetNumFromRecentCalls(WmsApp *pMe, GetAddr_Type eType);

/*==============================================================================
函数:
    GetMOClientMsg

说明:
    函数根据 pszTonum 、pUserdata 结构消息创建 wms_client_message_s_type 结
    构 MO 消息。（消息Tag为未发送）。

参数:
    pszTonum [in]: 接收消息的号码地址。
    pUserdata [in]: 已编码用户数据。

返回值:
    none

备注:
    返回数据 buffer 系动态分配，由调用者负责释放。

==============================================================================*/
wms_client_message_s_type *GetMOClientMsg(char *pszTonum, wms_cdma_user_data_s_type *pUserdata, boolean bTlAck);




/*==============================================================================
函数:
    CWmsApp_Getspecmsg

说明:
    本函数用于根据特定的消息类型返回相应的 wms_client_message_s_type 型 MO 消息。

参数:
    pwstrType [in]: 区分获取类型。

返回值:
    无。

备注:
    返回Buf系动态分配，调用者负责释放。
        
==============================================================================*/
wms_client_message_s_type *CWmsApp_Getspecmsg(AECHAR *pwstrType);

//Add By zzg 2010_07_21
wms_client_message_s_type *CWmsApp_GetspecmsgEx(void);
//Add End

#endif // _WMSUTIL_H
