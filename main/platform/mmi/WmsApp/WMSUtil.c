/*==============================================================================
//           W M S   A P P L E T  M O D U L E
// 文件:
//        WMSUtil.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明:
//        短信息模块工具函数定义
//                 
==============================================================================*/

/*==============================================================================
修改历史记录:
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/



/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
#include "WmsApp_priv.h"
#include "WMSUtil.h"
#ifdef FEATURE_CONTACT_APP
#include "ContApp.h"
#endif
#include "CallApp.h"
#include "wms.h"
#ifdef FEATURE_CARRIER_TAIWAN_APBW
#include "wmsmsg.h"
#include "wmstscdma.h"
#endif
#include "recentcalls.h"
#include "mobile.h"

#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)
#include "AEECARDSESSION_NOTIFIER.BID"
#include "AEECARDSESSION.BID"
#include "AEECardSession.h"
#include "AEEDB.h"
#include "AEECardSessionNotifier.h"
#include "AEEDownload.h"
#include "OEMCardSessionNotifier.h"
#include "gsdi.h"
#endif

/*==============================================================================
                                 
                                 本地全局变量定义
                                 
==============================================================================*/
// 由回调函数使用的静态对象指针
static WmsApp * gpWMSApp = NULL;

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/
#define BCD2INT(x)  (((((x) & 0xf0) >> 4) * 10) + ((x) & 0x0f))


//Add By zzg 2010_08_03 for frenduo
#define FRENDUO_SMS_DESTNUM	 "551"				// "15323435013"
#define FRENDUO_SMS_BUFF     "status"
//Add End
/*==============================================================================

                                    函数声明

==============================================================================*/
#ifdef FEATURE_CONTACT_APP
// 用于从通信簿取号码的回调函数
static boolean GetNumCallback(sSelectFieldListNode* pNode);

// 用于从通信簿取联系人信息并插入到当前编辑的消息中的回调函数
static boolean InsertNumCallback(sSelectFieldListNode* pNode);
#endif

// 用于从通话历史记录取联系人信息并插入到当前编辑的消息中的回调函数
static boolean GetNumCallbackFromRecentCall(sSelectRecordListNode* pNode);

/*==============================================================================

                                    函数定义
                                 
==============================================================================*/
/*==============================================================================
函数: 
    OEMWMS_GetTimeFromWmsTimestamp
       
说明: 
    函数用来将短信时间戳格式转换为秒。
       
参数: 
    pTime [in]: wms_timestamp_s_type 结构数据指针
    
返回值:
    转换后的时间值。
       
备注:
    
==============================================================================*/
static uint32 OEMWMS_GetTimeFromWmsTimestamp(wms_timestamp_s_type *pTime)
{
#ifndef WIN32
    uint8 tmp;
    clk_julian_type jt;
    
    MEMSET(&jt, 0, sizeof(jt));
    
    tmp = BCD2INT(pTime->year);
    
    // If the year is above 95 it we assume it is in 20 th century(1900+ year
    // otherwise we assume it is in 21 century(2000+year)
    DBGPRINTF("OEMWMS_GetTimeFromWmsTimestamp tmp=%d", tmp);
    //jt.year = (tmp <= 95) ? (2000 + tmp) : (1900 + tmp);
    jt.year = (tmp < 80) ? (2000 + tmp) : (1900 + tmp);
    DBGPRINTF("OEMWMS_GetTimeFromWmsTimestamp jt.year=%d", jt.year);
    jt.month = BCD2INT(pTime->month);
    jt.day = BCD2INT(pTime->day);
    jt.hour = BCD2INT(pTime->hour);
    jt.minute = BCD2INT(pTime->minute);
    jt.second = BCD2INT(pTime->second);
    
    return clk_julian_to_secs(&jt);
#else
	return 0;
#endif
}


/*==============================================================================
函数: 
    WMSUtil_SecsToDateString

说明: 
    将以秒表示的时间转换为字符串形式。

参数: 
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    dwTime [in]: 以计秒的时间。
    pBuf [out]: 用于存放时间字符串的缓冲区。
    nSize [in]: pBuf 的空间大小(单位:字节)。

返回值: 
    none

备注: 

==============================================================================*/
void WMSUtil_SecsToDateString(WmsApp* pMe,uint32 dwTime, AECHAR *pBuf, int nSize)
{
    JulianType      jt;
    AECHAR          wstrFmt[32];
    uint16          wHour,len;
    AECHAR          *wstrBuf = NULL;
    byte            btTimeFmt = 0;
#ifdef FEATURE_TIME_DATA_SETTING
    byte            btDateFmt = 0;
#endif /* FEATURE_TIME_DATA_SETTING */    
    DBGPRINTF("WMSUtil_SecsToDateString Start");
	DBGPRINTF("***zzg WMSUtil_SecsToDateString Start dwTime=%d", dwTime);
	
    if ((NULL == pMe) || (NULL == pBuf) || (0 == nSize))
    {
        return;
    }
    
    wstrBuf = (AECHAR *) sys_malloc(128);
    if (NULL == wstrBuf)
    {
        MSG_ERROR("memory allocation failed !",0,0,0);
        return;
    }
    
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_TIME_FORMAT,
                           &btTimeFmt,
                           sizeof(btTimeFmt));
    DBGPRINTF("WMSUtil_SecsToDateString btTimeFmt=%d",btTimeFmt);
    GETJULIANDATE(dwTime, &jt);
    DBGPRINTF("WMSUtil_SecsToDateString jt.year=%d",jt.wYear);
    MEMSET(wstrFmt, 0, sizeof(wstrFmt));
    if (btTimeFmt == OEMNV_TIMEFORM_AMPM)
    {
        wHour = jt.wHour > 12 ? (jt.wHour - 12) : jt.wHour;
        if(jt.wHour == 0)
        {
            wHour = 12;
        }

        if(jt.wHour >= 12)
        {
            (void)STRTOWSTR("%02d:%02d PM", wstrFmt, sizeof(wstrFmt));
        }
        else
        {
            (void)STRTOWSTR("%02d:%02d AM", wstrFmt, sizeof(wstrFmt));
        }
    }
    else
    {
        (void)STRTOWSTR("%02d:%02d", wstrFmt, sizeof(wstrFmt));
        wHour = jt.wHour;
    }
    
    // 格式化时间字符串
    WSPRINTF(wstrBuf, 128, wstrFmt, wHour, jt.wMinute); 
             
    // 加空格
    len = WSTRLEN(wstrBuf);
    wstrBuf[len] = (AECHAR)' ';
    len++;
    wstrBuf[len] = 0;
    
    MEMSET(wstrFmt, 0, sizeof(wstrFmt));
    len = WSTRLEN(wstrBuf);
#ifdef FEATURE_TIME_DATA_SETTING
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_DATE_FORMAT,
                           &btDateFmt,
                           sizeof(btDateFmt));

    switch (btDateFmt)
    {
        case OEMNV_DATEFORM_MDY:
            (void)STRTOWSTR("%02d/%02d/%04d", wstrFmt, sizeof(wstrFmt));
            WSPRINTF(&wstrBuf[len], (128-len*sizeof(AECHAR)), 
                 wstrFmt, jt.wMonth, jt.wDay, jt.wYear); 
            break;
            
        case OEMNV_DATEFORM_DMY:
            (void)STRTOWSTR("%02d/%02d/%04d", wstrFmt, sizeof(wstrFmt));
            WSPRINTF(&wstrBuf[len], (128-len*sizeof(AECHAR)), 
                 wstrFmt, jt.wDay, jt.wMonth, jt.wYear); 
            break;
            
        case OEMNV_DATEFORM_YMD:
        default:
            (void)STRTOWSTR("%04d/%02d/%02d", wstrFmt, sizeof(wstrFmt));
            WSPRINTF(&wstrBuf[len], (128-len*sizeof(AECHAR)), 
                 wstrFmt, jt.wYear, jt.wMonth, jt.wDay); 
            break;
    }
    
#else
    (void)STRTOWSTR("%04d/%02d/%02d", wstrFmt, sizeof(wstrFmt));
    WSPRINTF(&wstrBuf[len], (128-len*sizeof(AECHAR)), 
         wstrFmt, jt.wYear, jt.wMonth, jt.wDay); 
#endif /* FEATURE_TIME_DATA_SETTING */
    
        
    len = WSTRLEN(wstrBuf);
    if (nSize > (len * sizeof(AECHAR)))
    {// 缓冲区空间足够
        MEMSET(pBuf, 0, nSize);
        (void) WSTRCPY(pBuf, wstrBuf);
    }
    
    SYS_FREEIF(wstrBuf);
}

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
boolean WMSUtil_HaveNoneASCIIChar ( AECHAR * pWstr, int * pPostion)
{
#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE
    // 对于以色列运营商固定使用 WMS_ENCODING_LATIN_HEBREW 编码
    return FALSE;
#else
    int i ,len;
    AECHAR *pWstrTep = pWstr;
    MSG_FATAL("WMSUtil_HaveNoneASCIIChar Start",0,0,0);
    if (NULL == pWstrTep)
    {
        return FALSE;
    }
    
    len = WSTRLEN(pWstrTep);
    MSG_FATAL("WMSUtil_HaveNoneASCIIChar 1 len=%d",len,0,0);
    if (len <= 0)
    {
        return FALSE;
    }
#if defined(FEATURE_SMS_GSM7BIT)
    for (i=0; ((i < len) && ((*pWstr) < UCS2_TO_GSM_TABLE_SIZE)); i++,pWstr++)
#else
    for (i=0; ((i < len) && ((*pWstr) < 128)); i++,pWstr++)
#endif    
    {
        //ERR("i='%c',=%d",*pWstr,*pWstr,0);//Azlun
        ;
    }
    MSG_FATAL("WMSUtil_HaveNoneASCIIChar i=%d",i,0,0);
    if (i < len)
    {
        if (NULL != pPostion)
        {
            *pPostion = i;
        }
        MSG_FATAL("WMSUtil_HaveNoneASCIIChar has NoneASCIIChar",0,0,0);
        return TRUE;
    }
    else
    {
        MSG_FATAL("WMSUtil_HaveNoneASCIIChar has't NoneASCIIChar",0,0,0);
        return FALSE;
    }
#endif /* FEATURE_CARRIER_ISRAEL_PELEPHONE */
}

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
                                   int *pnMaxcharsOneitem)
{
    int nItems = 1;// 将消息划分后得到的总项数
    int nMaxItemChars = 0;     // 每项最大字符数
    int nLen = 0;
    wms_user_data_encoding_e_type      encoding;
    int nMaxChars_UNICODE = MAX_UNICODMSG_PAYLOAD;
    int nMaxChars_ASCII = MAX_ASCIIMSG_PAYLOAD;
    
    if ((NULL == pWstr) || *pWstr == 0)
    {
        if (NULL != pencoding)
        {
            *pencoding = WMS_ENCODING_ASCII;
        }
        
        if (NULL != pnMaxcharsOneitem)
        {
            *pnMaxcharsOneitem = nMaxChars_ASCII;
        }
        return nItems;
    }
    
    nLen = WSTRLEN(pWstr);
    
    // 确定消息编码方式及需划分的条数
    if (WMSUtil_HaveNoneASCIIChar(pWstr, NULL))
    {
        encoding = WMS_ENCODING_UNICODE;
    }
    else
    {
#if defined (FEATURE_CARRIER_ANGOLA_MOVICEL)
        byte  mo_encoding=0;
        
        (void) OEM_GetConfig(CFGI_SMS_MO_ENCODING,
                             &mo_encoding,
                             sizeof(mo_encoding));
        if (mo_encoding == OEMNV_SMS_MO_ENCODING_7BIT )
        {
            encoding = WMS_ENCODING_ASCII;
        }
        else
        {
            encoding = WMS_ENCODING_LATIN;
        }
#elif defined(FEATURE_CARRIER_ISRAEL_PELEPHONE)        
        encoding = WMS_ENCODING_LATIN_HEBREW;
#else //FEATURE_CARRIER_ANGOLA_MOVICEL
        encoding = WMS_ENCODING_ASCII;
#endif //FEATURE_CARRIER_ANGOLA_MOVICEL
    }

    // 确定消息编码方式及需划分的条数
    if (WMS_ENCODING_UNICODE == encoding)
    {        
        if (nLen<=nMaxChars_UNICODE)
        {
            nMaxItemChars = nMaxChars_UNICODE;
        }
        else
        {
#ifdef FEATURE_SMS_UDH
            nMaxItemChars = (nMaxChars_UNICODE-3);
#else
            nMaxItemChars = nMaxChars_UNICODE;
#endif            
        }
    }
    else if (WMS_ENCODING_ASCII == encoding)
    {        
        if (nLen<=nMaxChars_ASCII)
        {
            nMaxItemChars = nMaxChars_ASCII;
        }
        else
        {
#ifdef FEATURE_SMS_UDH
            nMaxItemChars = (nMaxChars_ASCII-7);
#else
            nMaxItemChars = nMaxChars_ASCII;
#endif            
        }
    }
    else
    {        
#ifdef FEATURE_SMS_UDH
        if (nLen <= MAX_OCTETMSG_PAYLOAD)
        {
            nMaxItemChars = MAX_OCTETMSG_PAYLOAD;
        }
        else
        {
            nMaxItemChars = MAX_OCTETMSG_PAYLOAD-6;
        }
#else
        nMaxItemChars = MAX_OCTETMSG_PAYLOAD;
#endif            
    }
    
    nItems = nLen / nMaxItemChars;
    if (nLen % nMaxItemChars != 0)
    {
        nItems++;
    }
    
    if (NULL != pencoding)
    {
        *pencoding = encoding;
    }
    
    if (NULL != pnMaxcharsOneitem)
    {
        *pnMaxcharsOneitem = nMaxItemChars;
    }
    
    return nItems;
}

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
                            uint16 wBufLen)
{
#ifdef FEATURE_CONTACT_APP
    IContApp * pContApp = NULL;
    ContInfo  CInfo;
    
    if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
        AEECLSID_APP_CONTACT,
        (void**)&pContApp))
    {
        if (pContApp == NULL)
        {
            return;
        }
        
        MEMSET(&CInfo, 0, sizeof(ContInfo));
        CInfo.pName = pwszName;
        CInfo.wNameLen = wBufLen;
        
        (void)ICONTAPP_NumberLookup(pContApp, (AECHAR *)pwszNum, &CInfo);
                    
        (void)ICONTAPP_Release(pContApp);
    } 
#endif       
}

/*==============================================================================
函数: 
    WMSUtil_AVKType2AECHAR(AVKType key)

说明: 
    将按键输入代笔的字符转换成相应的宽字符。

参数: 
    key [in]: AVKType。

返回值: 
    AECHAR
       
备注: 

==============================================================================*/
AECHAR WMSUtil_AVKType2AECHAR(AVKType key)
{
    AECHAR  wChar;
    
    switch (key)
    {
        case AVK_STAR:
            wChar = (AECHAR)'*';
            break;

        case AVK_POUND:
            wChar = (AECHAR)'#';
            break;
            
        default:
            if (key < AVK_0 || key > AVK_9) 
            {
                wChar = (AECHAR)'\0';
            }
            else
            {
                wChar = (AECHAR)('0' + (char)(key - AVK_0));
            }
            break;
    }
 
    return wChar;
}

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
void WMSMessageStruct_Reset(WMSMessageStruct * pMsg) 
{
    if (NULL == pMsg) 
    {
        return;
    }
    
    // 先释放动态分配的 buffer
    if (NULL != pMsg->m_szMessage)
    {
        sys_free(pMsg->m_szMessage);
        pMsg->m_szMessage = NULL;
    }
    
    // 全部数据清零
    MEMSET(pMsg, 0, sizeof(WMSMessageStruct));
}
//add by yangdecai 2010-08-16
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
void WMSMessageStruct_Free(WmsApp * pMe)
{
	int i;
	for(i = 0;i<LONGSMS_MAX_PACKAGES;i++)
	{
		WMSMessageStruct_Reset(pMe->m_CurMsgNodesMS[i]);
	}
}

static void OEMWMS_ConvertFromUnicode(wms_client_bd_s_type *bd_data)
{
    uint8 i;
    uint16  *pTep;
    
    if (bd_data != NULL)
    {
        bd_data->user_data.data_len = bd_data->user_data.data_len/2;
        pTep = (uint16 *)bd_data->user_data.data;
        
        for (i=0; i<bd_data->user_data.data_len; i++,pTep++)
        {
            *pTep = NTOHS(*pTep);
        }
    }
}

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
                                 WMSMessageStruct *pOutMs)
{
    wms_client_ts_data_s_type   *cl_data = NULL;    // 需分配内存
    wms_client_bd_s_type        *info_data = NULL;  // 需分配内存
    uint8                       *buf = NULL;        // 需分配内存
    wms_cdma_user_data_s_type   *puserdata = NULL;
    int32                       nSize;
    int                         nlen = 0, i;
    
    if ((NULL == pClMsg) || (NULL == pOutMs))
    {
        return;
    }
    if (pClMsg->msg_hdr.message_mode != WMS_MESSAGE_MODE_CDMA)
    {
        return;
    }
    
    // 结构数据清零
    WMSMessageStruct_Reset(pOutMs);
    
    // 先分配函数要用到的 buffer
    cl_data = sys_malloc(sizeof(wms_client_ts_data_s_type));
    if (cl_data == NULL)
    {
        MSG_ERROR("memory allocation failed !",0,0,0);
        goto WmsApp_ConvertClientMsgToMSErr;
    }
    (void)MEMSET(cl_data, 0, sizeof(wms_client_ts_data_s_type));
    
    buf = sys_malloc(64);
    if (buf == NULL)
    {
        MSG_ERROR("memory allocation failed !",0,0,0);
        goto WmsApp_ConvertClientMsgToMSErr;
    }
    
    info_data = sys_malloc(sizeof(wms_client_bd_s_type));
    if (info_data == NULL)
    {
        MSG_ERROR("memory allocation failed !",0,0,0);
        goto WmsApp_ConvertClientMsgToMSErr;
    }
    (void)MEMSET(info_data, 0, sizeof(wms_client_bd_s_type));
    
    // 解析开始
    pOutMs->msg_tag = pClMsg->msg_hdr.tag;
    
    if (pOutMs->msg_tag == WMS_TAG_MO_TEMPLATE) 
    {// CDMA 模板消息或草稿消息
        wms_cdma_template_s_type *pClTemp = &(pClMsg->u.cdma_template);
        
        if ((pClTemp->mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
            (pClTemp->client_bd.mask & WMS_MASK_BD_USER_DATA))
        {
            // copy the user data into the info cache
            switch (pClTemp->client_bd.user_data.encoding)
            {
                case WMS_ENCODING_ASCII:
                case WMS_ENCODING_IA5:
                    puserdata = (wms_cdma_user_data_s_type*)&(pClTemp->client_bd.user_data);
                    nlen = wms_ts_unpack_ascii(puserdata,
                                pClTemp->client_bd.user_data.number_of_digits+1,
                                info_data->user_data.data);
                    info_data->user_data.data_len = nlen;
                    
                    nSize = sizeof(AECHAR)*(nlen + 1);
                    info_data->user_data.data[nlen] = 0;
                    pOutMs->m_szMessage = sys_malloc(nSize);
                    if (NULL == pOutMs->m_szMessage)
                    {
                        MSG_ERROR("memory allocation failed !",0,0,0);
                        goto WmsApp_ConvertClientMsgToMSErr;
                    }
                    (void)MEMSET(pOutMs->m_szMessage, 0, nSize);
                    
                    for (i=0; i<nlen; i++)
                    {
#if defined(FEATURE_SMS_GSM7BIT)
                        pOutMs->m_szMessage[i] = (AECHAR)GsmToUcs2[info_data->user_data.data[i]];
                        
                        if (pOutMs->m_szMessage[i] == 0)
                        {
                            pOutMs->m_szMessage[i] = (AECHAR)0x20;//' '
                        }
#else    
                        pOutMs->m_szMessage[i] = (AECHAR)info_data->user_data.data[i];
#endif            
                    }
                    pOutMs->m_szMessage[i] = 0;
                    break;
                    
                case WMS_ENCODING_UNICODE:
                    info_data->user_data.data_len = pClTemp->client_bd.user_data.data_len;
                    (void)MEMCPY(info_data->user_data.data,
                                 pClTemp->client_bd.user_data.data,
                                 info_data->user_data.data_len);
                    OEMWMS_ConvertFromUnicode(info_data);
                    
                    nlen = info_data->user_data.data_len;
                    nSize = sizeof(AECHAR)*(nlen + 1);
                    pOutMs->m_szMessage = sys_malloc(nSize);
                    if (NULL == pOutMs->m_szMessage)
                    {
                        MSG_ERROR("memory allocation failed !",0,0,0);
                        goto WmsApp_ConvertClientMsgToMSErr;
                    }
                    (void)MEMSET(pOutMs->m_szMessage, 0, nSize);
                    (void)WSTRNCOPYN (pOutMs->m_szMessage,
                                      (nlen+1),
                                      (AECHAR *)(info_data->user_data.data), 
                                      nlen);
                    break;
                    
                // just copy the user data for other encodings
                default:
                    nlen = pClTemp->client_bd.user_data.data_len;
                    
                    for (i=0; i<nlen; i++)
                    {
#if defined(FEATURE_SMS_GSM7BIT)
                        pOutMs->m_szMessage[i] = (AECHAR)pClTemp->client_bd.user_data.data[i];
                        
                        if (pOutMs->m_szMessage[i] == 0)
                        {
                            pOutMs->m_szMessage[i] = (AECHAR)0x20;//' '
                        }
#else    
                        pOutMs->m_szMessage[i] = (AECHAR)pClTemp->client_bd.user_data.data[i];
#endif            
                    }
                    pOutMs->m_szMessage[i] = 0; 
                    break;
            }
            pOutMs->m_epriority = pClTemp->client_bd.priority;
        }
        
        if (pClTemp->mask & WMS_CDMA_TEMPLATE_MASK_DEST_ADDR)
        {
            (void)MEMSET(buf, 0, 64);
            if (pClTemp->address.digit_mode == WMS_DIGIT_MODE_4_BIT)
            {
                (void)wms_ts_dtmf2ascii((byte)pClTemp->address.number_of_digits,
                                (byte*)pClTemp->address.digits, buf);
            }
            else
            {
                nlen = pClTemp->address.number_of_digits;
                (void)MEMCPY(buf, pClTemp->address.digits, nlen);
            }
            nlen = STRLEN((char *)buf);
            
            if (nlen>0)
            {
                nSize = sizeof(pOutMs->m_szNum);
                (void)MEMSET(pOutMs->m_szNum, 0, nSize);
                (void)STRTOWSTR((char *)buf, pOutMs->m_szNum, nSize);
            }
        }
    }
    else
    {// CDMA 消息
#ifdef FEATURE_SMS_UDH
        uint8 i=0;
#endif 

        // Initialized other to 0
        (void)MEMSET(&cl_data->u.cdma.other, 0, sizeof(wms_other_parm_s_type));

        // decode the raw bearer data
        if (wms_ts_decode(&pClMsg->u.cdma_message.raw_ts, cl_data) != WMS_OK_S)
        {
            MSG_ERROR("Decoding message failed!",0,0,0);
            goto WmsApp_ConvertClientMsgToMSErr;
        }
        
        // 消息数目(仅语音通知短信)
        if (cl_data->u.cdma.mask & WMS_MASK_BD_NUM_OF_MSGS)
        {
            pOutMs->m_NumMsgs = cl_data->u.cdma.num_messages;
        }
        
        // 短信中心时间戳
        if (cl_data->u.cdma.mask & WMS_MASK_BD_MC_TIME)
        {
            pOutMs->m_dwTime = OEMWMS_GetTimeFromWmsTimestamp(&(cl_data->u.cdma.mc_time));
        }
        
        // 优先级
        if (cl_data->u.cdma.mask & WMS_MASK_BD_PRIORITY)
        {
            pOutMs->m_epriority = cl_data->u.cdma.priority;
        }
        
        // 密级
        if (cl_data->u.cdma.mask & WMS_MASK_BD_PRIORITY)
        {
            pOutMs->m_eprivacy = cl_data->u.cdma.privacy;
        }
        
        // 语言指示
        if (cl_data->u.cdma.mask & WMS_MASK_BD_LANGUAGE)
        {
            pOutMs->language = cl_data->u.cdma.language;
        }
    
#ifdef FEATURE_SMS_UDH
        // 串接消息头
        for (i=0; i<cl_data->u.cdma.user_data.num_headers; i++)
        {
            if (cl_data->u.cdma.user_data.headers[i].header_id == WMS_UDH_CONCAT_8)
            {
                pOutMs->seq_num = cl_data->u.cdma.user_data.headers[i].u.concat_8.seq_num;
                pOutMs->total_sm = cl_data->u.cdma.user_data.headers[i].u.concat_8.total_sm;
                pOutMs->msg_ref = cl_data->u.cdma.user_data.headers[i].u.concat_8.msg_ref;
                break;
            }
            else if (cl_data->u.cdma.user_data.headers[i].header_id == WMS_UDH_CONCAT_16)
            {
                pOutMs->seq_num = cl_data->u.cdma.user_data.headers[i].u.concat_16.seq_num;
                pOutMs->total_sm = cl_data->u.cdma.user_data.headers[i].u.concat_16.total_sm;
                pOutMs->msg_ref = cl_data->u.cdma.user_data.headers[i].u.concat_16.msg_ref;
                break;
            }
        }
#endif
        
        // 先检查消息有无地址(MT:始发地址 MO:接收地址)
        if ((pClMsg->u.cdma_message.address.number_of_digits) > 0)
        {
            (void)MEMSET(buf, 0, 64);
            if (pClMsg->u.cdma_message.address.digit_mode == WMS_DIGIT_MODE_4_BIT)
            {
                (void)wms_ts_dtmf2ascii((byte)pClMsg->u.cdma_message.address.number_of_digits,
                                        (byte*)pClMsg->u.cdma_message.address.digits, buf);
            }
            else
            {
                nlen = pClMsg->u.cdma_message.address.number_of_digits;
                (void)MEMCPY(buf, pClMsg->u.cdma_message.address.digits, nlen);
            }
            nlen = STRLEN((char *)buf);
            
            if (nlen>0)
            {
                i = 0;
                nSize = sizeof(pOutMs->m_szNum);
                (void)MEMSET(pOutMs->m_szNum, 0, nSize);
                
                if ((nlen>3) &&
                    (pClMsg->msg_hdr.tag == WMS_TAG_MT_READ ||
                     pClMsg->msg_hdr.tag == WMS_TAG_MT_NOT_READ))
                {
                    // 去掉 "+86"
                    if (STRNCMP((char *)buf, "+86", 3) == 0)
                    {
                        i = 3;
                    }
                    else if (STRNCMP((char *)buf, "+62", 3) == 0)
                    {
                        i = 2;
                        buf[i] = '0';
                    }
                    // 去掉 "+91"
                    else if (STRNCMP((char *)buf, "+91", 3) == 0)
                    {
                        i = 3;
                    }
                    else if (STRNCMP((char *)buf, "+66", 3) == 0)
                    {
                        i = 2;
                        buf[i] = '0';
                    }
#ifdef FEATURE_CARRIER_TAIWAN_APBW
                    // 将"886"变成"0"
                    else if (STRNCMP((char *)buf, "886", 3) == 0)
                    {
                        i = 2;
                        buf[i] = '0';
                    }
#endif
#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
                    // 将"158"变成"0416"
                    // 将"199"变成"0419"
                    // 将"199"变成"0426" guoys modified for movilnet change 2008.10.28
                    else if ((STRNCMP((char *)buf, "158", 3) == 0) ||
                             (STRNCMP((char *)buf, "199", 3) == 0))
                    {
                        int j;
                        
                        buf[nlen+1] = 0;
                        for (j = nlen; j>3; j--)
                        {
                            buf[j] = buf[j-1];
                        }
                        
                        if (buf[2] == '8')
                        {
                            //buf[3] = '6';
                            buf[2] = '1';
                        }
                        else
                        {
                            //buf[3] = '9';
                            buf[2] = '2';
                        }
                        buf[0] = '0';
                        buf[1] = '4';
                        //buf[2] = '1';
                        buf[3] = '6';
                        nlen += 1;
                    }
#endif
                }
                
#ifdef FEATURE_CARRIER_TAIWAN_APBW
                if (pClMsg->msg_hdr.tag == WMS_TAG_MT_READ ||
                    pClMsg->msg_hdr.tag == WMS_TAG_MT_NOT_READ)
                {// 00000000777 去掉多余的 0
                    int j;
                    
                    for (j=i; j<nlen; j++)
                    {
                        if (buf[j] != (AECHAR)'0')
                        {
                            if (STRCMP("777", (char *)&buf[j]) == 0)
                            {
                                i=j;
                            }
                            break;
                        }
                    }
                }
#endif
                
                (void)STRTOWSTR((char *)&buf[i], pOutMs->m_szNum, nSize);
            }
        }
        
        if (cl_data->u.cdma.mask & WMS_MASK_BD_USER_DATA) 
        {
            pOutMs->m_epriority = cl_data->u.cdma.priority;
            
            // copy the user data 
            switch (cl_data->u.cdma.user_data.encoding)
            {
                case WMS_ENCODING_ASCII:
                case WMS_ENCODING_IA5:
                    puserdata = (wms_cdma_user_data_s_type*)&cl_data->u.cdma.user_data;
                    nlen = wms_ts_unpack_ascii(puserdata,
                                cl_data->u.cdma.user_data.number_of_digits+1,
                                info_data->user_data.data);
                    info_data->user_data.data_len = nlen;
                    
                    nlen = info_data->user_data.data_len;
                    nSize = sizeof(AECHAR)*(nlen + 1);
                    info_data->user_data.data[nlen] = 0;
                    pOutMs->m_szMessage = sys_malloc(nSize);
                    if (NULL == pOutMs->m_szMessage)
                    {
                        MSG_ERROR("memory allocation failed !",0,0,0);
                        goto WmsApp_ConvertClientMsgToMSErr;
                    }
                    (void)MEMSET(pOutMs->m_szMessage, 0, nSize);
                    
                    for (i=0; i<nlen; i++)
                    {
#if defined(FEATURE_SMS_GSM7BIT)
                        pOutMs->m_szMessage[i] = (AECHAR)GsmToUcs2[info_data->user_data.data[i]];
                        
                        if (pOutMs->m_szMessage[i] == 0)
                        {
                            pOutMs->m_szMessage[i] = (AECHAR)0x20;//' '
                        }
#else    
                        pOutMs->m_szMessage[i] = (AECHAR)info_data->user_data.data[i];
#endif            
                    }
                    pOutMs->m_szMessage[i] = 0;
                    break;
                    
                case WMS_ENCODING_UNICODE:
                    OEMWMS_ConvertFromUnicode(&cl_data->u.cdma);
                    nlen = cl_data->u.cdma.user_data.data_len;
                    
                    nSize = sizeof(AECHAR)*(nlen + 1);
                    pOutMs->m_szMessage = sys_malloc(nSize);
                    if (NULL == pOutMs->m_szMessage)
                    {
                        MSG_ERROR("memory allocation failed !",0,0,0);
                        goto WmsApp_ConvertClientMsgToMSErr;
                    }
                    (void)MEMSET(pOutMs->m_szMessage, 0, nSize);
                    (void)WSTRNCOPYN (pOutMs->m_szMessage,
                                      (nlen+1),
                                      (AECHAR *)(cl_data->u.cdma.user_data.data), 
                                      nlen);
                    break;
                    
                // just copy the user data for other encodings
                default:
                    nlen = cl_data->u.cdma.user_data.number_of_digits;//xg 2008 08 08modify msg
                    
                    nSize = sizeof(AECHAR)*(nlen + 1);
                    pOutMs->m_szMessage = sys_malloc(nSize);
                    if (NULL == pOutMs->m_szMessage)
                    {
                        MSG_ERROR("memory allocation failed !",0,0,0);
                        goto WmsApp_ConvertClientMsgToMSErr;
                    }
                    for (i=0; i<nlen; i++)
                    {
#ifdef FEATURE_LANG_HEBREW
                        pOutMs->m_szMessage[i] = (AECHAR)HebrewToUcs2[cl_data->u.cdma.user_data.data[i]];
                        
                        if (pOutMs->m_szMessage[i] == 0)
                        {
                            pOutMs->m_szMessage[i] = (AECHAR)0x20;//' '
                        }
#else
                        pOutMs->m_szMessage[i] = (AECHAR)cl_data->u.cdma.user_data.data[i];
#endif                        
                    }
                    pOutMs->m_szMessage[i] = 0; 
                    break;
            }
        }
        
        // Callback number
        if (cl_data->u.cdma.mask & WMS_MASK_BD_CALLBACK)
        {
            (void)MEMSET(buf, 0, 64);
            if (cl_data->u.cdma.callback.digit_mode == WMS_DIGIT_MODE_4_BIT)
            {
                (void)wms_ts_dtmf2ascii((byte)cl_data->u.cdma.callback.number_of_digits,
                                (byte*)cl_data->u.cdma.callback.digits, buf);
            }
            else
            {
                nlen = cl_data->u.cdma.callback.number_of_digits;
                (void)MEMCPY(buf, cl_data->u.cdma.callback.digits, nlen);
            }
            nlen = STRLEN((char *)buf);
            
            if (nlen>0)
            {
                i = 0;
                nSize = sizeof(pOutMs->m_szCallBkNum);
                (void)MEMSET(pOutMs->m_szCallBkNum, 0, nSize);
                
                if ((nlen>3) &&
                    (pClMsg->msg_hdr.tag == WMS_TAG_MT_READ ||
                     pClMsg->msg_hdr.tag == WMS_TAG_MT_NOT_READ))
                {
                    // 去掉 "+86"
                    if (STRNCMP((char *)buf, "+86", 3) == 0)
                    {
                        i = 3;
                    }
                    // 去掉 "+91"
                    else if (STRNCMP((char *)buf, "+91", 3) == 0)
                    {
                        i = 3;
                    }
                    else if (STRNCMP((char *)buf, "+62", 3) == 0)
                    {
                        i = 2;
                        buf[i] = '0';
                    }
                    else if (STRNCMP((char *)buf, "+66", 3) == 0)
                    {
                        i = 2;
                        buf[i] = '0';
                    }
#ifdef FEATURE_CARRIER_TAIWAN_APBW
                    // 将"886"变成"0"
                    else if (STRNCMP((char *)buf, "886", 3) == 0)
                    {
                        i = 2;
                        buf[i] = '0';
                    }
#endif
#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
                    // 将"158"变成"0416"
                    // 将"199"变成"0419"
                    // 将"199"变成"0426" guoys modified for movilnet change 2008.10.28
                    else if ((STRNCMP((char *)buf, "158", 3) == 0) ||
                             (STRNCMP((char *)buf, "199", 3) == 0))
                    {
                        int j;
                        
                        buf[nlen+1] = 0;
                        for (j = nlen; j>3; j--)
                        {
                            buf[j] = buf[j-1];
                        }
                        
                        if (buf[2] == '8')
                        {
                            //buf[3] = '6';
                            buf[2] = '1';
                        }
                        else
                        {
                            //buf[3] = '9';
                            buf[2] = '2';
                        }
                        buf[0] = '0';
                        buf[1] = '4';
                        //buf[2] = '1';
                        buf[3] = '6';
                        nlen += 1;
                    }
#endif
                }
                
#ifdef FEATURE_CARRIER_TAIWAN_APBW
                if (pClMsg->msg_hdr.tag == WMS_TAG_MT_READ ||
                    pClMsg->msg_hdr.tag == WMS_TAG_MT_NOT_READ)
                {// 00000000777 去掉多余的 0
                    int j;
                    
                    for (j=i; j<nlen; j++)
                    {
                        if (buf[j] != (AECHAR)'0')
                        {
                            if (STRCMP("777", (char *)&buf[j]) == 0)
                            {
                                i=j;
                            }
                            break;
                        }
                    }
                }
#endif
                
                (void)STRTOWSTR((char *)&buf[i], pOutMs->m_szCallBkNum, nSize);
            }
        }
    }
    
    // 程序正常退出也需执行后面的代码
WmsApp_ConvertClientMsgToMSErr:
    WMSAPPU_FREE(cl_data)
    WMSAPPU_FREE(buf)
    WMSAPPU_FREE(info_data)
}

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
void WMSExtApp_CallPhoneNumber(WmsApp * pMe, AECHAR *szVMNumber, boolean bIPCall)
{
    ICallApp  * pUICall = NULL;

    // 创建呼叫接口
    if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
        AEECLSID_DIALER,
        (void **)&pUICall))
    {
        if (pUICall != NULL)
        {
            // 调用呼叫接口函数进行呼叫
#ifdef FEATRUE_SET_IP_NUMBER
            if (bIPCall)
            {
                (void)ICallApp_Make_IP_Call_F(pUICall, szVMNumber);
            }
            else
#endif
            {
                (void)ICallApp_CallNumber(pUICall, szVMNumber);
            }
            
            // 释放呼叫接口
            (void)ICallApp_Release(pUICall);
        }
    }
}

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
void WMSExtApp_SaveExtractDetails(WmsApp *pMe, AECHAR  *wstrNum, AECHAR *wstrURL, AECHAR *wstrEmail)
{
 #ifdef FEATURE_CONTACT_APP
    IContApp * pContApp = NULL;
    uint16 wType = SAVE_NUMBER;

    if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
        AEECLSID_APP_CONTACT,
        (void**)&pContApp))
    {
        if (pContApp == NULL)
        {
            return;
        }
        if (NULL != wstrNum)
        {
            wType = SAVE_NUMBER;
            if(pMe->m_wPrevMenuSel == IDS_SAVETOCONTACT)
            {
                wType = ADD_FIELD;
            }
        }
        else if (NULL !=wstrEmail)
        {
            wType = SAVE_EMAIL;
        }
        else if (NULL !=wstrURL)
        {
            wType = SAVE_URL;
        }
        (void)ICONTAPP_SaveNumber(pContApp, wstrNum, wstrEmail, wstrURL,wType);
        (void)ICONTAPP_Release(pContApp);
    }
#endif
}

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
void WMSExtApp_GetAddresseeFromContactsApp(WmsApp *pMe, GetAddr_Type eType)
{
#ifdef FEATURE_CONTACT_APP
    IContApp * pContApp = NULL;
    
    if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
        AEECLSID_APP_CONTACT,
        (void**)&pContApp))
    {
        if (pContApp == NULL)
        {
            return;
        }
        
        gpWMSApp = pMe;
        
        switch (eType)
        {
            case GETADDR_INSERT:
                (void)ICONTAPP_SelectFieldEx(pContApp, (PFNSELECTEDCONTACTCB)InsertNumCallback, MULTI_SELECT_CONTENT);
                break;
                
            case GETADDR_ONEADD:
                (void)ICONTAPP_SelectFieldEx(pContApp, (PFNSELECTEDCONTACTCB)GetNumCallback, SINGLE_SELECT_NUMBER);
                break;
                
            case GETADDR_MULTIADD:
                (void)ICONTAPP_SelectFieldEx(pContApp, (PFNSELECTEDCONTACTCB)GetNumCallback, MULTI_SELECT_NUMBER);
                break;

            case GETADDR_GROUPlIST:
                (void)ICONTAPP_SelectFieldEx(pContApp, (PFNSELECTEDCONTACTCB)GetNumCallback, MULTI_SELECT_GROUPLIST);
                break;
            case GETADDR_GROUPLIST_ONEADD:
                (void)ICONTAPP_SelectFieldEx(pContApp, (PFNSELECTEDCONTACTCB)GetNumCallback, SINGLE_SELECT_GROUPLIST);
                break;
            
            default:
                break;
        }

        (void)ICONTAPP_Release(pContApp);
    } 
#endif       
}

#ifdef FEATURE_CONTACT_APP
/*==============================================================================
函数:
    GetNumCallback

说明:
    用于从通信簿取号码的回调函数。

参数:
    selRec [in]: 选定的记录。
    selItem [in]: 选定记录的字段。

返回值:
    TRUE: Contact applet 应退出。
    FALSE: Contact applet 不退出。

备注:
    函数会修改WMS对象的 m_pSendList 中的某个成员项。
        
==============================================================================*/
static boolean GetNumCallback(sSelectFieldListNode* pNode)
{
    int i;
    int32 nMaxItems = 1;
    CMultiSendItemInfo *pItem = NULL;
    sSelectFieldListNode* pTepNode = NULL;
    AECHAR  *wstrName = NULL;
#if defined(FEATURE_CARRIER_THAILAND_HUTCH)
    int nInserted = 0;
#endif
    
    if (NULL == pNode)
    {
        return TRUE;
    }
    
    if (gpWMSApp->m_eMakeAddListMode != MAKEADDLIST_NONE)
    {
        nMaxItems = MAXNUM_MULTISEND - IVector_Size(gpWMSApp->m_pSendList);
#if defined(FEATURE_CARRIER_THAILAND_HUTCH)
        nInserted = IVector_Size(gpWMSApp->m_pSendList);
#endif        
    }
    
    pTepNode = pNode;
    MSG_FATAL("GetNumCallback nMaxItems=%d", nMaxItems,0,0);
    for (i=0; ((i<nMaxItems) && (NULL != pTepNode)); )
    {
        if (pTepNode->SelectFieldInfo.wFieldMask & MASK_ADDRFIELD_NAME)
        {
            wstrName = WSTRDUP((AECHAR *)pTepNode->SelectFieldInfo.m_pAddNewName);
        }
        
        if ((pTepNode->SelectFieldInfo.wFieldMask & MASK_ADDRFIELD_PHONE_GENERIC) &&
            (NULL != pTepNode->SelectFieldInfo.m_pAddNewMobile) &&
            (WSTRLEN((AECHAR *)(pTepNode->SelectFieldInfo.m_pAddNewMobile))>0))
        {
            MSG_FATAL("MASK_ADDRFIELD_PHONE_GENERIC i=%d", i,0,0);
            if (gpWMSApp->m_currState == WMSST_SENDOPTS)
            {// Callback Number Enter:
                (void)WSTRCPY(gpWMSApp->m_msSend.m_szCallBkNum, (AECHAR *)(pTepNode->SelectFieldInfo.m_pAddNewMobile));
                goto GetNumCallback_Exit;
            }
            
            pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
            
            if (NULL == pItem)
            {// 缓冲区分配不成功
                goto GetNumCallback_Exit;
            }
            
            if (SUCCESS != IVector_AddElement(gpWMSApp->m_pSendList, pItem))
            {
                sys_free(pItem);
                goto GetNumCallback_Exit;
            }
            (void)WSTRCPY(pItem->m_szTo, (AECHAR *)(pTepNode->SelectFieldInfo.m_pAddNewMobile));
            MSG_FATAL("MASK_ADDRFIELD_PHONE_GENERIC1 i=%d", i,0,0);
            if((NULL != pTepNode->SelectFieldInfo.m_pAddNewEMAIL) &&
                (WSTRLEN((AECHAR *)(pTepNode->SelectFieldInfo.m_pAddNewEMAIL))>0))
            {
                (void)WSTRCPY(pItem->m_szEmail, (AECHAR *)(pTepNode->SelectFieldInfo.m_pAddNewEMAIL));
                DBGPRINTF("m_pAddNewEMAIL=%S",pItem->m_szEmail);
            }
            MSG_FATAL("MASK_ADDRFIELD_PHONE_GENERIC2 i=%d", i,0,0);
            if (NULL != wstrName)
            {
                (void)WSTRCPY(pItem->m_szName, wstrName);
            }
            i++;
            pItem = NULL;
            if (i>=nMaxItems)
            {
                goto GetNumCallback_Exit;
            }
        }
        
        if ((pTepNode->SelectFieldInfo.wFieldMask & MASK_ADDRFIELD_PHONE_HOME) &&
            (NULL != pTepNode->SelectFieldInfo.m_pAddNewHome) &&
            (WSTRLEN((AECHAR *)(pTepNode->SelectFieldInfo.m_pAddNewHome))>0))
        {
            if (gpWMSApp->m_currState == WMSST_SENDOPTS)
            {// Callback Number Enter:
                (void)WSTRCPY(gpWMSApp->m_msSend.m_szCallBkNum, (AECHAR *)(pTepNode->SelectFieldInfo.m_pAddNewHome));
                goto GetNumCallback_Exit;
            }
            
            pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
            
            if (NULL == pItem)
            {// 缓冲区分配不成功
                goto GetNumCallback_Exit;
            }
            
            if (SUCCESS != IVector_AddElement(gpWMSApp->m_pSendList, pItem))
            {
                sys_free(pItem);
                goto GetNumCallback_Exit;
            }
            (void)WSTRCPY(pItem->m_szTo, (AECHAR *)(pTepNode->SelectFieldInfo.m_pAddNewHome));
            if (NULL != wstrName)
            {
                (void)WSTRCPY(pItem->m_szName, wstrName);
            }
            i++;
            pItem = NULL;
            if (i>=nMaxItems)
            {
                goto GetNumCallback_Exit;
            }
        }
        
        if ((pTepNode->SelectFieldInfo.wFieldMask & MASK_ADDRFIELD_PHONE_WORK) &&
            (NULL != pTepNode->SelectFieldInfo.m_pAddNewOffice) &&
            (WSTRLEN((AECHAR *)(pTepNode->SelectFieldInfo.m_pAddNewOffice))>0))
        {
            if (gpWMSApp->m_currState == WMSST_SENDOPTS)
            {// Callback Number Enter:
                (void)WSTRCPY(gpWMSApp->m_msSend.m_szCallBkNum, (AECHAR *)(pTepNode->SelectFieldInfo.m_pAddNewOffice));
                goto GetNumCallback_Exit;
            }
            
            pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
            
            if (NULL == pItem)
            {// 缓冲区分配不成功
                goto GetNumCallback_Exit;
            }
            
            if (SUCCESS != IVector_AddElement(gpWMSApp->m_pSendList, pItem))
            {
                sys_free(pItem);
                goto GetNumCallback_Exit;
            }
            (void)WSTRCPY(pItem->m_szTo, (AECHAR *)(pTepNode->SelectFieldInfo.m_pAddNewOffice));
            if (NULL != wstrName)
            {
                (void)WSTRCPY(pItem->m_szName, wstrName);
            }
            i++;
            pItem = NULL;
            if (i>=nMaxItems)
            {
                goto GetNumCallback_Exit;
            }
        }
        
        if ((pTepNode->SelectFieldInfo.wFieldMask & MASK_ADDRFIELD_PHONE_FAX) &&
            (NULL != pTepNode->SelectFieldInfo.m_pAddNewFax) &&
            (WSTRLEN((AECHAR *)(pTepNode->SelectFieldInfo.m_pAddNewFax))>0))
        {
            if (gpWMSApp->m_currState == WMSST_SENDOPTS)
            {// Callback Number Enter:
                (void)WSTRCPY(gpWMSApp->m_msSend.m_szCallBkNum, (AECHAR *)(pTepNode->SelectFieldInfo.m_pAddNewFax));
                goto GetNumCallback_Exit;
            }
            
            pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
            
            if (NULL == pItem)
            {// 缓冲区分配不成功
                goto GetNumCallback_Exit;
            }
            
            if (SUCCESS != IVector_AddElement(gpWMSApp->m_pSendList, pItem))
            {
                sys_free(pItem);
                goto GetNumCallback_Exit;
            }
            (void)WSTRCPY(pItem->m_szTo, (AECHAR *)(pTepNode->SelectFieldInfo.m_pAddNewFax));
            if (NULL != wstrName)
            {
                (void)WSTRCPY(pItem->m_szName, wstrName);
            }
            i++;
            pItem = NULL;
            if (i>=nMaxItems)
            {
                goto GetNumCallback_Exit;
            }
        }
        
        pTepNode = pTepNode->pNextNode;
        if (wstrName != NULL)
        {
            SYS_FREEIF(wstrName);
        }        
    }
    
GetNumCallback_Exit:
    if (wstrName != NULL)
    {
        SYS_FREEIF(wstrName);
    }
    
    return TRUE;
}


/*==============================================================================
函数:
    InsertNumCallback

说明:
    
    用于从通信簿取号码并插入到当前编辑的消息中的回调函数。

参数:
    selRec [in]: 选定的记录。
    selItem [in]: 选定记录的字段。

返回值:
    TRUE:  Contact applet应退出。
    FALSE: Contact applet不退出。

备注:
    函数会修改WMS对象的 m_pSendList 中的某个成员项。
        
==============================================================================*/
static boolean InsertNumCallback(sSelectFieldListNode* pNode)
{
    AECHAR* wszTep;
    AECHAR* wszPack;
    AECHAR* szMessage = gpWMSApp->m_msSend.m_szMessage;
    int startPos = gpWMSApp->m_dwInsertPos; 
    int  nLen,nCanInsert,nLenPack;
    int  nMax = 0;
    int  nSize = 0;
    
    if ((NULL == pNode) || 
        (NULL == pNode->SelectFieldInfo.m_pAddNewFormatContent) ||
        (WSTRLEN(pNode->SelectFieldInfo.m_pAddNewFormatContent) == 0))
    {
        return TRUE;
    }
    wszPack = pNode->SelectFieldInfo.m_pAddNewFormatContent;
    
    if (startPos < 0)
    {
        startPos = 0;
    }
    
    nLenPack = WSTRLEN(wszPack);
    
    if (NULL == gpWMSApp->m_msSend.m_szMessage)
    {
        gpWMSApp->m_msSend.m_szMessage = (AECHAR *)sys_malloc((nLenPack+1)* sizeof(AECHAR));
        if (NULL != gpWMSApp->m_msSend.m_szMessage)
        {
            (void)WSTRCPY(gpWMSApp->m_msSend.m_szMessage, wszPack);
        }
        return TRUE;
    }
    nLen = WSTRLEN(szMessage);
    
    gpWMSApp->m_bTextFullAlert = TRUE;
    
    if (gpWMSApp->m_eInsertType == INSERT_EDITWMS)
    {
        nMax = gpWMSApp->m_nMOmaxChars;
    }
    else
    {
        nMax = MAX_TEMPLATECHARS;
    }

    // 计算当前最多允许插入的字符数
    if (nLen >= nMax)
    {// 不能插入
        nCanInsert = 0;
    }
    else
    {
        if (gpWMSApp->m_eInsertType == INSERT_EDITWMS)
        {
#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT        
            if (AEE_TM_THAI == gpWMSApp->m_CurInputMode || 
                AEE_TM_THAI_R == gpWMSApp->m_CurInputMode ||
                WMSUtil_HaveNoneASCIIChar(szMessage, NULL))
            {
                nCanInsert = MAX_UNICODMSG_PAYLOAD - nLen;
                
                if (nCanInsert < 1)
                {// 不能插入
                    nCanInsert = 0;
                }
                else if (nCanInsert > nLenPack)
                {
                    gpWMSApp->m_bTextFullAlert = FALSE;
                }
            }
            else
            {
                int nPos;
                
                if (nLen >= MAX_ASCIIMSG_PAYLOAD)
                {// 不能插入
                    nCanInsert = 0;
                }
                else if (WMSUtil_HaveNoneASCIIChar(wszPack, &nPos))
                {
                    if (nLen + nLenPack < MAX_UNICODMSG_PAYLOAD)
                    {// 可全部插入
                        nCanInsert = nLenPack;
                        gpWMSApp->m_bTextFullAlert = FALSE;
                    }
                    else if (nLen + nLenPack > MAX_UNICODMSG_PAYLOAD)
                    {// 只能部分插入
                        // 消息本身不含宽字符, 只能插入前面的非宽字符序列
                        nCanInsert = (MAX_ASCIIMSG_PAYLOAD - nLen) >= nPos ? 
                                    nPos : (MAX_ASCIIMSG_PAYLOAD - nLen);
                                        
                        if(nCanInsert+nLen < MAX_ASCIIMSG_PAYLOAD)
                        {
                            gpWMSApp->m_bTextFullAlert = FALSE;
                        }               
                    }
                    else
                    {
                        nCanInsert = nLenPack;
                    }
                }
                else
                {
                    nCanInsert = (MAX_ASCIIMSG_PAYLOAD - nLen) >= nLenPack ? 
                                nLenPack : (MAX_ASCIIMSG_PAYLOAD - nLen);
                                    
                    if (nCanInsert+nLen < MAX_ASCIIMSG_PAYLOAD)
                    {
                        gpWMSApp->m_bTextFullAlert = FALSE;
                    }               
                }
            }
#else
            nCanInsert = nMax - nLen;
            
            if (nCanInsert < 1)
            {// 不能插入
                nCanInsert = 0;
            }
            else if (nCanInsert > nLenPack)
            {
                gpWMSApp->m_bTextFullAlert = FALSE;
            }
#endif
        }
        else
        {
            nCanInsert = (nMax - nLen) >= nLenPack ? 
                        nLenPack : (nMax - nLen);
        }
                        
        if (nCanInsert+nLen < nMax)
        {
            gpWMSApp->m_bTextFullAlert = FALSE;
        }               
    }
    
    if (nCanInsert <= 0)
    {
        return TRUE;
    }
    MSG_FATAL("nCanInsert::::::::::::::%d",nCanInsert,0,0);
    nSize = (nLen + nCanInsert + 1) * sizeof(AECHAR);
    wszTep = (AECHAR *)sys_malloc(nSize);
    
    if (wszTep != NULL)
    {
        MEMSET(wszTep, 0, nSize);
        (void)WSTRNCOPYN(wszTep, (startPos+1), szMessage, startPos);
        (void)WSTRNCOPYN(&wszTep[startPos], (nCanInsert+1), wszPack, nCanInsert);
        (void)WSTRCAT(wszTep, &szMessage[startPos]);
        
        wszTep[nLen+nCanInsert] = 0;
        gpWMSApp->m_dwInsertPos = startPos + nCanInsert;
        SYS_FREEIF(szMessage);
        gpWMSApp->m_msSend.m_szMessage = wszTep;
    }
    
    return TRUE;
}
#endif       

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
void WmsApp_StorePrefs(WmsApp *pMe) 
{
    if (NULL == pMe) 
    {
        return;
    }

    (void)ISHELL_SetPrefs(pMe->m_pShell, 
            AEECLSID_WMSAPP, 
            WMS_CFG_VERSION,
            &pMe->m_cfg, 
            sizeof(WMSAPP_Config_Type));
}

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
void WmsApp_GetPrefs(WmsApp *pMe) 
{
    if (NULL == pMe) 
    {
        return;
    }

    (void)ISHELL_GetPrefs(pMe->m_pShell, 
            AEECLSID_WMSAPP, 
            WMS_CFG_VERSION,
            &pMe->m_cfg, 
            sizeof(WMSAPP_Config_Type));
}

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
uint8 WmsApp_GetMsgRef(WmsApp *pMe)
{
    uint8      msg_ref = 0;
    
    if (NULL == pMe) 
    {
        return 255;
    }
    
    WmsApp_GetPrefs(pMe);
    msg_ref = pMe->m_cfg.m_msg_ref;
    
    if (msg_ref == 255)
    {
        pMe->m_cfg.m_msg_ref = 1;
    }
    else
    {
        pMe->m_cfg.m_msg_ref++;
    }
    
    WmsApp_StorePrefs(pMe);
    
    return msg_ref;
}
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
uint16 WmsApp_GetMsgIDNum(WmsApp *pMe)
{
    uint16      m_msgid = 0;
    
    if (NULL == pMe) 
    {
        return 1;
    }
    
    WmsApp_GetPrefs(pMe);
    m_msgid = pMe->m_cfg.m_msgid;
    
    if (m_msgid == 0xFFFF)
    {
        pMe->m_cfg.m_msgid = 1;
    }
    else
    {
        pMe->m_cfg.m_msgid++;
    }
    
    WmsApp_StorePrefs(pMe);
    
    return m_msgid;
}

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
uint8 ConvertIntToBcd(uint16 i)
{
    uint8 bcd;
    
    bcd = ((int)(i/10)) << 4;
    bcd |= i % 10;
    
    return bcd;
}

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
void ConvertMStoMcTime(uint32 sec, wms_timestamp_s_type *pmc_time)
{
    uint8      year = 0;
    JulianType julian;
    
    if (NULL == pmc_time)
    {
        return;
    }
    GETJULIANDATE(sec, &julian);
    DBGPRINTF("ConvertMStoMcTime julian.Year=%d", julian.wYear);
    if (julian.wYear <= 1999)
    {
        //if (julian.wYear <= 1996)
        if (julian.wYear <= 1980)
        {
            //year = 96;
            year = 80;
        }
        else
        {
            year = julian.wYear - 1900;
        }
    }
    else
    {
        if (julian.wYear >= 2095)
        {
            year = 95;
        }
        else
        {
            year = julian.wYear - 2000;
        }
    }
    DBGPRINTF("ConvertMStoMcTime year=%d", year);
    pmc_time->year     = ConvertIntToBcd(year);
    pmc_time->month    = ConvertIntToBcd(julian.wMonth);
    pmc_time->day      = ConvertIntToBcd(julian.wDay);
    pmc_time->hour     = ConvertIntToBcd(julian.wHour);
    pmc_time->minute   = ConvertIntToBcd(julian.wMinute);
    pmc_time->second   = ConvertIntToBcd(julian.wSecond);
    pmc_time->timezone = 0;
    DBGPRINTF("ConvertMStoMcTime year=%d", year);
}

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
                                             boolean bOutTemplate)
{
    wms_client_message_s_type   *pCltMsg = NULL;
    wms_cdma_user_data_s_type   *pUserdata = NULL;
    wms_client_ts_data_s_type   *pCltTsData = NULL;
    int32 nSize = 0;
    
    if ((NULL == pMe) || (NULL == pMs) ||(NULL == pMs->m_szMessage))
    {
        return NULL;
    }
    
    // 先分配需要的 buffer
    nSize = sizeof(wms_cdma_user_data_s_type);
    pUserdata = (wms_cdma_user_data_s_type *)sys_malloc(nSize);
    if (NULL == pUserdata)
    {
        ERR("Malloc Failed!",0,0,0);
        return NULL;
    }
    MEMSET(pUserdata, 0, nSize);
    
    nSize = sizeof(wms_client_ts_data_s_type);
    pCltTsData = (wms_client_ts_data_s_type *)sys_malloc(nSize);
    if (NULL == pCltTsData)
    {
        ERR("sys_malloc Failed!",0,0,0);
        SYS_FREEIF(pCltTsData);
        return NULL;
    }
    MEMSET(pCltTsData, 0, nSize);
    
    nSize = sizeof(wms_client_message_s_type);
    pCltMsg = (wms_client_message_s_type *)sys_malloc(nSize);
    if (NULL == pCltMsg)
    {
        SYS_FREEIF(pUserdata);
        SYS_FREEIF(pCltTsData);
        ERR("sys_malloc Failed!",0,0,0);
        return NULL;
    }
    MEMSET(pCltMsg, 0, nSize);
    
    // 转换到 wms_cdma_user_data_s_type 结构
    pUserdata->encoding = WMS_ENCODING_UNICODE;
    {
        uint16  *pTep = (uint16 *)pUserdata->data;
        int j, nlen;
        
        nlen = WSTRLEN(pMs->m_szMessage);
        pUserdata->number_of_digits = nlen;
        pUserdata->data_len = nlen * sizeof(AECHAR);
        pUserdata->padding_bits = 0;
        
        for (j=0; j<nlen; j++,pTep++)
        {
            *pTep = HTONS(pMe->m_msSend.m_szMessage[j]);
        }
    }
    
    if (bOutTemplate)
    {
        // -------- 填充 wms_client_message_s_type 结构 --------
        pCltMsg->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
        pCltMsg->msg_hdr.tag = WMS_TAG_MO_TEMPLATE;
        pCltMsg->msg_hdr.mem_store = WMS_MEMORY_STORE_RUIM;
        
        // 消息索引在外部设置
        //pCltMsg->msg_hdr.index = 0;
        
        pCltMsg->u.cdma_template.teleservice = WMS_TELESERVICE_CMT_95;
        pCltMsg->u.cdma_template.mask = WMS_CDMA_TEMPLATE_MASK_BEARER_DATA;
        
        pCltMsg->u.cdma_template.client_bd.mask = WMS_MASK_BD_USER_DATA;
        nSize = sizeof(wms_cdma_user_data_s_type);
        MEMCPY(&pCltMsg->u.cdma_template.client_bd.user_data, pUserdata, nSize);
    }
    else
    {
        // -------- 填充 wms_client_ts_data_s_type 结构 --------
        pCltTsData->format = WMS_FORMAT_CDMA;
        pCltTsData->u.cdma.mask = 0;
    
        // 消息用户数据部分
        nSize = sizeof(wms_cdma_user_data_s_type);
        MEMCPY(&pCltTsData->u.cdma.user_data, pUserdata, nSize);
        pCltTsData->u.cdma.mask |= WMS_MASK_BD_USER_DATA;
        
        // 时间戳
        {
        	uint32	sec = 0;	

			/*
			byte	btTimeStamp = 0;
		    
			(void) ICONFIG_GetItem(pMe->m_pConfig,
		                           CFGI_SMS_TIMESTAMP,
		                           &btTimeStamp,
		                           sizeof(btTimeStamp));

			if (btTimeStamp == OEMNV_SMS_TIMESTAMP_ADJUST)
			{
				sec = GETUTCSECONDS();
				MSG_FATAL("***zzg GETUTCSECONDS 11 dwSecs=%d***", sec, 0, 0);
			}
			else
			{
				sec = GETTIMESECONDS();
				MSG_FATAL("***zzg GETTIMESECONDS 11 dwSecs=%d***", sec, 0, 0);
			}  
			*/

			sec = GETTIMESECONDS();
            
            ConvertMStoMcTime(sec, &pCltTsData->u.cdma.mc_time);
        }
        pCltTsData->u.cdma.mask |= WMS_MASK_BD_MC_TIME;
        
        // -------- 填充 wms_client_message_s_type 结构 --------
        pCltMsg->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
        pCltMsg->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;
        pCltMsg->msg_hdr.mem_store = WMS_MEMORY_STORE_NV_CDMA;
        // 消息索引在外部设置
        //pCltMsg->msg_hdr.index = 0;
        
        pCltMsg->u.cdma_message.is_mo = TRUE;
        pCltMsg->u.cdma_message.teleservice = WMS_TELESERVICE_CMT_95;
    
        // 接收地址
        pCltMsg->u.cdma_message.address.number_of_digits = 0;
        pCltMsg->u.cdma_message.address.digit_mode = WMS_DIGIT_MODE_4_BIT;
        pCltMsg->u.cdma_message.address.number_mode = WMS_NUMBER_MODE_NONE_DATA_NETWORK;
        pCltMsg->u.cdma_message.address.number_type = WMS_NUMBER_UNKNOWN;
        pCltMsg->u.cdma_message.address.number_plan = WMS_NUMBER_PLAN_TELEPHONY;
        
        pCltMsg->u.cdma_message.is_tl_ack_requested = TRUE;
        pCltMsg->u.cdma_message.is_service_present = FALSE;
    
        (void)IWMS_TsEncode(pMe->m_pwms, pCltTsData, &pCltMsg->u.cdma_message.raw_ts);
    }
    
    // 消息编码结束
    
    SYS_FREEIF(pCltTsData);
    SYS_FREEIF(pUserdata);
    
    return pCltMsg;
}

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
void WmsApp_BuildNumList(WmsApp *pMe, WMSMessageStruct *pmsMsg)
{
    AECHAR    *pItem = NULL;
    int  i,nLen, nStartPos, nChars;
    
    if ((NULL == pMe) || (NULL == pmsMsg))
    {
        return;
    }
    
    // 取 Originate Number
    nLen = WSTRLEN(pmsMsg->m_szNum);
    if (nLen > 0)
    {
        pItem = WSTRDUP(pmsMsg->m_szNum);
        
        if (NULL != pItem)
        {
            if (IVector_AddElement(pMe->m_pSaveNumList, pItem) != SUCCESS)
            {
                SYS_FREEIF(pItem);
                return;
            }
            
            // 注意pItem指向的地址不在这里释放
            pItem = NULL;
        }
    }
    
    // 取 CallBack Number
    nLen = WSTRLEN(pmsMsg->m_szCallBkNum);
    if (nLen > 0)
    {
        pItem = WSTRDUP(pmsMsg->m_szCallBkNum);
        
        if (NULL != pItem)
        {
            if (IVector_AddElement(pMe->m_pSaveNumList, pItem) != SUCCESS)
            {
                SYS_FREEIF(pItem);
                return;
            }
            
            // 注意pItem指向的地址不在这里释放
            pItem = NULL;
        }
    }
    
    nStartPos = -1;
    // 分析消息体，抽取号码
    if (NULL == pmsMsg->m_szMessage)
    {
        nLen = 0;
    }
    else
    {
        nLen = WSTRLEN(pmsMsg->m_szMessage);
    }
    for (i=0; i<nLen; i++)
    {
        if (pmsMsg->m_szMessage[i] >= (AECHAR)'0' &&
            pmsMsg->m_szMessage[i] <= (AECHAR)'9')
        {// 当前字符为数字
            if (nStartPos < 0)
            {
                nStartPos = i;
            }
            
            // 需要此判断，否则号码位于消息体尾部时将取不出
            if (i != nLen-1)
            {
                continue;
            }
            else
            {
                i++;
            }
        }
        
        if (nStartPos < 0)
        {
            continue;
        }
        nChars = i-nStartPos;
        if (nChars < 3 || nChars >= MAX_PH_DIGITS)
        { // 连续数字串长度小于3或不低于MAX_PH_DIGITS，此号码无效
            nStartPos = -1;
            continue;
        }
        
        // 抽取有效号码
        pItem = (AECHAR *)sys_malloc((nChars+1)*sizeof(AECHAR));
        if ((nStartPos > 0) && (pmsMsg->m_szMessage[nStartPos-1]==(AECHAR)'+'))
        {
            nStartPos -= 1;
            nChars++;
        }
        if (NULL != pItem)
        {
            (void)WSTRNCOPYN(pItem, (nChars+1), &(pmsMsg->m_szMessage[nStartPos]), nChars);
            
            if (IVector_AddElement(pMe->m_pSaveNumList, pItem) != SUCCESS)
            {
                SYS_FREEIF(pItem);
                return;
            }
            
            // 注意pItem指向的地址不在这里释放
            pItem = NULL;
        }
        
        // 重置起始标记
        nStartPos = -1;
    }
}

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
    
    与 WmsApp_BuildNumList 函数共用 m_pSaveNumList 链表。调用此函数前应将此链表
    清空。
==============================================================================*/
void WmsApp_BuildMailAddList(WmsApp *pMe, AECHAR *wstrText)
{
    AECHAR    *pItem = NULL;
    AECHAR    wchAt = (AECHAR)'@';
    AECHAR    *wstrAt = NULL;
    AECHAR    *wstrStartPos = NULL;
    AECHAR    *wstrEndPos = NULL;
    AECHAR    *wstrTep = NULL;
    AECHAR    wstrInvaild[]={0x22,0x28,0x29,0x2c,0x3a,0x3b,0x3c,0x3e,0x5b,0x5c,0x5d,0};
    int       nLen, nChars;
    
    if ((NULL == pMe) || (NULL == wstrText))
    {
        return;
    }
    
    nLen = WSTRLEN(wstrText);
    if (nLen == 0)
    {
        return;
    }
    
    wstrAt = WSTRCHR(wstrText, wchAt);
    wstrStartPos = wstrText;
    wstrEndPos = &wstrText[nLen-1];
    while (NULL != wstrAt)
    {
        // 确定地址前面部分
        wstrTep = wstrAt-1;
        
        for (; wstrTep>=wstrStartPos; wstrTep--)
        {
            if (((*wstrTep) <= 0x20) || 
                ((*wstrTep) >= 0x7F) || 
                (WSTRCHR(wstrInvaild, (*wstrTep)) != NULL))
            {// 出现无效字符
                break;
            }
        }
        
        if (wstrTep>=wstrStartPos)
        {
            wstrStartPos = wstrTep+1;
        }
        
        // 确定地址后面部分
        wstrTep = wstrAt+1;
        
        for (; wstrTep<=wstrEndPos; wstrTep++)
        {
            if (((*wstrTep) <= 0x20) || 
                ((*wstrTep) >= 0x7F) || 
                ((*wstrTep) == wchAt) || 
                (WSTRCHR(wstrInvaild, (*wstrTep)) != NULL))
            {// 出现无效字符
                break;
            }
        }
        
        if (wstrTep<=wstrEndPos)
        {
            wstrEndPos = wstrTep-1;
        }
        
        if ((wstrStartPos < wstrAt) && 
            (wstrEndPos > wstrAt) &&
            (wstrEndPos-wstrStartPos<=MAX_EMAILADD_LEN))
        {// 获取到有效电子邮件地址
            nChars = (wstrEndPos - wstrStartPos) + 1;
            
            pItem = (AECHAR *)sys_malloc(((nChars+1)*sizeof(AECHAR)));
            if (NULL != pItem)
            {
                (void)WSTRNCOPYN(pItem, (nChars+1), wstrStartPos, nChars);
                
                if (IVector_AddElement(pMe->m_pSaveNumList, pItem) != SUCCESS)
                {
                    SYS_FREEIF(pItem);
                    return;
                }
                
                //ERR("--%d", WSTRLEN(pItem),0,0);
                
                // 注意pItem指向的地址不在这里释放
                pItem = NULL;
            }
        }
        
        wstrStartPos = (wstrEndPos+1);
        if (wstrStartPos < &wstrText[nLen-1])
        {
            wstrEndPos = &wstrText[nLen-1];
            wstrAt = WSTRCHR(wstrStartPos, wchAt);
        }
        else
        {
            wstrAt = NULL;// 设置终止 while 循环的条件
        }
    }
}

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
void WmsApp_BuildUrlList(WmsApp *pMe, AECHAR *wstrText)
{
    AECHAR    *pItem = NULL;
    AECHAR    *wstrIn = NULL;
    AECHAR    wstrInvaild[]={0x22,0x3c,0x3e,0};
    int       nLen, nCopys, nStartPos, nEndPos;
    int       i;
    
    if ((NULL == pMe) || (NULL == wstrText))
    {
        return;
    }
    
    nLen = WSTRLEN(wstrText);
    if (nLen < 8)
    {
        return;
    }
    
    wstrIn = WSTRDUP(wstrText);
    if (NULL == wstrIn)
    {
        return;
    }
    WSTRLOWER(wstrIn);
    
    nEndPos = nLen-1;
    for (i=nLen-1; i>=6; i--)
    {
        if (wstrText[i] <= 0x20 || 
            wstrText[i] >= 0x7F || 
            WSTRCHR(wstrInvaild, wstrText[i]) != NULL)
        {
            nEndPos = i;
            continue;
        }
        
        if ((wstrIn[i] == (AECHAR)'/') &&
            (wstrIn[i-1] == (AECHAR)'/') &&
            (wstrIn[i-2] == (AECHAR)':') &&
            (wstrIn[i-3] == (AECHAR)'p') &&
            (wstrIn[i-4] == (AECHAR)'t') &&
            (wstrIn[i-5] == (AECHAR)'t') &&
            (wstrIn[i-6] == (AECHAR)'h'))
        {
            nStartPos = i-6;
        }
        else if (i<7)
        {
            break;
        }
        else if ((wstrIn[i] == (AECHAR)'/') &&
                 (wstrIn[i-1] == (AECHAR)'/') &&
                 (wstrIn[i-2] == (AECHAR)':') &&
                 (wstrIn[i-3] == (AECHAR)'s') &&
                 (wstrIn[i-4] == (AECHAR)'p') &&
                 (wstrIn[i-5] == (AECHAR)'t') &&
                 (wstrIn[i-6] == (AECHAR)'t') &&
                 (wstrIn[i-7] == (AECHAR)'h'))
        {
            nStartPos = i-7;
        }
        else
        {
            continue;
        }
        
        nCopys = nEndPos - nStartPos + 1;
        nCopys = (nCopys > MAX_URL_LEN) ? MAX_URL_LEN : nCopys;
            
        pItem = (AECHAR *)sys_malloc(((nCopys+1)*sizeof(AECHAR)));
        if (NULL != pItem)
        {
            (void)WSTRNCOPYN(pItem, (nCopys+1), &wstrText[nStartPos], nCopys);
            
            if (IVector_AddElement(pMe->m_pSaveNumList, pItem) != SUCCESS)
            {
                SYS_FREEIF(pItem);
            }
            else
            {
                ERR("--%d", WSTRLEN(pItem),0,0);
            }
            
            // 注意pItem指向的地址不在这里释放
            pItem = NULL;
        }
        
        nEndPos = nStartPos;
        i = nStartPos;
    }
    
    SYS_FREEIF(wstrIn);
}

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
int GetMonthDays(int nYear, int nMonth)
{
    if (nMonth == 2)
    {
        if ((((nYear)%4)==0) && (((nYear)%100) || (((nYear)%400)==0)))
        {
            return 29;
        }
        else
        {
            return 28;
        }
    }
    
    nMonth = (nMonth > 2) ? nMonth-3 : nMonth+9;
    
    return ((nMonth % 5) & 1) ? 30 : 31;
}

#ifdef FEATURE_CARRIER_TAIWAN_APBW
/*==============================================================================
函数: 
    WMSUtil_GetContactName

说明: 
    用于修正亚太宽频粉红色卡更新成功后提示字符串不完整的问题。

参数: 
    ruimdata [in/out]: 
        Layout of ruimdata
        0: tag/status 
        1: len 
        2+: data

返回值: 
    TRUE:  需调用者更新此消息
    FALSE: 调用者不需更新此消息

备注: 

==============================================================================*/
boolean ChkUpdate(uint8 *ruimdata)
{
    wms_msg_ruim_sms_s_type     *ruim_sms= NULL;
    wms_tl_message_type         *cdma_tl = NULL;
    wms_cdma_message_s_type     *cdma_cl = NULL;
    wms_client_ts_data_s_type   *cl_data = NULL;
    wms_client_bd_s_type        *info_data = NULL;
    wms_cdma_user_data_s_type   *pUserdata = NULL;
    boolean bRet = FALSE;
    int nlen;
    
    if (NULL == ruimdata)
    {
        return bRet;
    }
    
    ruim_sms = (wms_msg_ruim_sms_s_type *)sys_malloc(sizeof(wms_msg_ruim_sms_s_type));
    if (ruim_sms == NULL)
    {
        goto ChkUpdate_Exit;
    }
    cdma_tl = (wms_tl_message_type *)sys_malloc(sizeof(wms_tl_message_type));
    if (cdma_tl == NULL)
    {
        goto ChkUpdate_Exit;
    }
    cdma_cl = (wms_cdma_message_s_type *)sys_malloc(sizeof(wms_cdma_message_s_type));
    if (cdma_cl == NULL)
    {
        goto ChkUpdate_Exit;
    }
    info_data = (wms_client_bd_s_type *)sys_malloc(sizeof(wms_client_bd_s_type));
    if (info_data == NULL)
    {
        goto ChkUpdate_Exit;
    }
    cl_data = (wms_client_ts_data_s_type *)sys_malloc(sizeof(wms_client_ts_data_s_type));
    if (cl_data == NULL)
    {
        goto ChkUpdate_Exit;
    }
    
    ruim_sms->ota.format   = WMS_FORMAT_CDMA;
    ruim_sms->status       = ruimdata[0];
    ruim_sms->ota.data_len = ruimdata[1];
    MEMCPY((void*)ruim_sms->ota.data, ruimdata+2, ruim_sms->ota.data_len);
    if (wms_ts_decode_CDMA_tl(&ruim_sms->ota, cdma_tl, &cdma_cl->raw_ts) != WMS_OK_S)
    {
        goto ChkUpdate_Exit;
    }
    wms_ts_convert_tl2cl(cdma_tl, cdma_cl);
    
    if (wms_ts_decode(&cdma_cl->raw_ts, cl_data) != WMS_OK_S)
    {
        goto ChkUpdate_Exit;
    }
    
    if (cl_data->u.cdma.mask & WMS_MASK_BD_USER_DATA) 
    {
        pUserdata = (wms_cdma_user_data_s_type*)&cl_data->u.cdma.user_data;
        
        switch (cl_data->u.cdma.user_data.encoding)
        {
            case WMS_ENCODING_ASCII:
            case WMS_ENCODING_IA5:
                nlen = wms_ts_unpack_ascii(pUserdata,
                            pUserdata->number_of_digits+1,
                            info_data->user_data.data);
                info_data->user_data.data[nlen+1] = 0;

                if (STRSTR((char *)(info_data->user_data.data), "Succe") != NULL)
                {
                    bRet = TRUE;
                    (void)STRCPY((char *)info_data->user_data.data, "Update Successful");
                    pUserdata->data_len = STRLEN("Update Successful");
                    pUserdata->number_of_digits = wms_ts_pack_ascii((char *)(info_data->user_data.data),
                                                                pUserdata->data,
                                                                &pUserdata->data_len,
                                                                &pUserdata->padding_bits);
                }
                break;
                
            case WMS_ENCODING_UNICODE:
                OEMWMS_ConvertFromUnicode(&cl_data->u.cdma);
                info_data->user_data.data[0] = 0;
                (void)WSTRTOSTR((AECHAR *)(pUserdata->data),
                                (char *)info_data->user_data.data,
                                sizeof(info_data->user_data.data));
                
                if (STRSTR((char *)(info_data->user_data.data), "Succe") != NULL)
                {
                    uint16  *pTep = (uint16 *)pUserdata->data;
                    int i;
                    
                    bRet = TRUE;
                    (void)STRTOWSTR("Update Successful", 
                                    (AECHAR *)(pUserdata->data),
                                    sizeof(pUserdata->data));
                    
                    pUserdata->number_of_digits = STRLEN("Update Successful");
                    for (i=0; i<pUserdata->number_of_digits; i++, pTep++)
                    {
                        *pTep = HTONS(*pTep);
                    }
                    pUserdata->data_len = pUserdata->number_of_digits*sizeof(AECHAR);
                    pUserdata->padding_bits = 0;
                }
                break;
                
            default:
                pUserdata->data[pUserdata->data_len] = 0;
                if (STRSTR((char *)(pUserdata->data), "Succe") != NULL)
                {
                    bRet = TRUE;
                    
                    (void)STRCPY((char *)pUserdata->data, "Update Successful");
                    pUserdata->number_of_digits = STRLEN("Update Successful");
                    pUserdata->data_len = pUserdata->number_of_digits;
                    pUserdata->padding_bits = 0;
                }
                break;
        }
    }
    
    if (bRet)
    {
        if (wms_ts_encode(cl_data, &cdma_cl->raw_ts) == WMS_OK_S)
        {
            wms_ts_convert_cl2tl(cdma_cl, cdma_tl);
            
            if (wms_ts_encode_CDMA_tl(cdma_tl, &cdma_cl->raw_ts, &ruim_sms->ota) == WMS_OK_S)
            {
                ruimdata[1] = ruim_sms->ota.data_len;
                MEMCPY(ruimdata+2, (void*)ruim_sms->ota.data, ruim_sms->ota.data_len);
            }
            else
            {
                bRet = FALSE;
            }
        }
        else
        {
            bRet = FALSE;
        }
    }
    
ChkUpdate_Exit:    
    SYS_FREEIF(ruim_sms);
    SYS_FREEIF(cdma_tl);
    SYS_FREEIF(cdma_cl);
    SYS_FREEIF(info_data);
    SYS_FREEIF(cl_data);
    
    return bRet;
}
#endif

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
int WmsApp_SetMaxSize(ITextCtl *pIText)
{
    int nMaxchars = WMS_MSG_MAXCHARS;
    
    if (!pIText)
    {
        return 0;
    }
        
#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT        
    {
        AECHAR *pwstrText;
        int nLen = 0;
        int nInputMode;
        wms_user_data_encoding_e_type encoding = WMS_ENCODING_ASCII;    
        
        pwstrText = ITEXTCTL_GetTextPtr ( pIText );
        nInputMode = ITEXTCTL_GetInputMode ( pIText, NULL );
        if ( NULL != pwstrText )
        {
            nLen = WSTRLEN ( pwstrText );
        }
        if (WMSUtil_HaveNoneASCIIChar(pwstrText, NULL) )
        {
            encoding = WMS_ENCODING_UNICODE;
        }
        ERR("encoding (%d),nLen (%d),nInputMode (%d)",encoding,nLen,nInputMode);
        // Thai input mode, or Encoding is Unicode, then Mex size is 70
        if (encoding == WMS_ENCODING_ASCII && nLen>MAX_UNICODMSG_PAYLOAD)
        {// 对于长度超过 MAX_UNICODMSG_PAYLOAD 的文本，若编码为 WMS_ENCODING_ASCII
         // 则不予截断，但最大数为 MAX_ASCIIMSG_PAYLOAD ，多余部份仍截断，且需将输入
         // 法调至不能输入UNICODE 的输入法; 否则切断消息
            nMaxchars = MAX_ASCIIMSG_PAYLOAD;
        }
        else if (AEE_TM_THAI == nInputMode || AEE_TM_THAI_R == nInputMode || WMS_ENCODING_UNICODE == encoding) 
        {
            nMaxchars = MAX_UNICODMSG_PAYLOAD;
        }
        else
        {
            nMaxchars = MAX_ASCIIMSG_PAYLOAD;
        }
        ERR("WmsApp_SetMaxSize......... %d",nMaxchars,0,0);
    }
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT

#if defined(FEATURE_CARRIER_ANGOLA_MOVICEL)
    {
        byte  mo_encoding=0;
        
        (void) OEM_GetConfig(CFGI_SMS_MO_ENCODING,
                             &mo_encoding,
                             sizeof(mo_encoding));
        if (mo_encoding == OEMNV_SMS_MO_ENCODING_7BIT )
        {
            nMaxchars = MAX_ASCIIMSG_PAYLOAD;
        }
        else
        {
            nMaxchars = MAX_OCTETMSG_PAYLOAD;
        }
    }
#endif //FEATURE_CARRIER_ANGOLA_MOVICEL

    ITEXTCTL_SetMaxSize(pIText, nMaxchars);
    
    return nMaxchars;
}

#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE
// 插入字符(回车符)
#define   INSERT_AECHAR         0x000A 

// 步长字符数
#define   AECHARS_STEP          16 

// 插入字符时的条件字符(空格)
#define   CONDTION_AECHAR       0x0020 

extern void RbmpBiDiReOrder_WMS(AECHAR *wstrIn, 
                                AECHAR *wstrOut, 
                                int32 nInLen,
                                int32 ABaseLevel, 
                                int32 AReOrderOption);

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
void wmsutil_formatmessage(wms_client_message_s_type *pclt_msg)
{
    WMSMessageStruct *pMs = NULL;
    AECHAR           *wtrBuf = NULL;
    AECHAR           *wtrP = NULL;
    AECHAR           *wtrP1 = NULL;
    AECHAR           wstrTep[AECHARS_STEP+4] = {0,};
    AECHAR           wTep;
    int32            nSize = 0;
    int nLen, i, j, nBufLen, nTepLen, nPos, nBufMax;
    wms_client_ts_data_s_type *pclt_ts = NULL;
    wms_cdma_user_data_s_type *puserdata = NULL;
    int              nOpt; // 0-无操作 1-插入 2-替换 其余值-无定义
    
    if ((NULL == pclt_msg) || AECHARS_STEP < 10)
    {
        return;
    }
    
    pMs = (WMSMessageStruct *)sys_malloc(sizeof(WMSMessageStruct));
    if (NULL == pMs)
    {
        return;
    }
    
    WmsApp_ConvertClientMsgToMS(pclt_msg, pMs);
    if (NULL == pMs->m_szMessage)
    {
        SYS_FREEIF(pMs);
        return;
    }
    
    nLen = WSTRLEN(pMs->m_szMessage);
    if (nLen == 0)
    {
        goto FORMATMESSAGE_EXIT;
    }
    nBufLen = 3*(nLen+1)+3;
    nBufMax = nBufLen;
    nSize = nBufLen*sizeof(AECHAR);
    wtrBuf = (AECHAR *)sys_malloc(nSize);
    
    if (NULL == wtrBuf)
    {
        goto FORMATMESSAGE_EXIT;
    }
    MEMSET(wtrBuf, 0, nSize);
    
    pclt_ts = (wms_client_ts_data_s_type *)sys_malloc(sizeof(wms_client_ts_data_s_type));
    if (NULL == pclt_ts)
    {
        goto FORMATMESSAGE_EXIT;
    }
    MEMSET(pclt_ts, 0, sizeof(wms_client_ts_data_s_type));
    if (WMS_OK_S != wms_ts_decode(&pclt_msg->u.cdma_message.raw_ts, pclt_ts))
    {
        goto FORMATMESSAGE_EXIT;
    }
    
    puserdata = &pclt_ts->u.cdma.user_data;
    
    nPos = 0;
    j = 0;
    do
    {
        nOpt = 0;
        i = nPos + AECHARS_STEP;
        MEMSET(wstrTep, 0, sizeof(wstrTep));
        if (i < nLen)
        {
            wTep = pMs->m_szMessage[i];
            if (wTep != CONDTION_AECHAR)
            {// 非空格，回溯看能否找到条件字符
                wTep = pMs->m_szMessage[i];
                pMs->m_szMessage[i] = 0;
                wtrP = WSTRRCHR(&pMs->m_szMessage[nPos], CONDTION_AECHAR);
                pMs->m_szMessage[i] = wTep;
                
                nOpt = 1;// 插入
                if ((NULL != wtrP) && (wtrP != &pMs->m_szMessage[nPos]))
                {
                    for (wtrP1 = &pMs->m_szMessage[nPos]; wtrP1 < wtrP; wtrP1++)
                    {// 检查地址 wtrP 前是否全为 CONDTION_AECHAR
                        if (*wtrP1 != CONDTION_AECHAR)
                        {
                            break;
                        }
                    }
                    
                    if (wtrP1 != wtrP)
                    {
                        i = nPos + (wtrP - &pMs->m_szMessage[nPos]);
                        nOpt = 2;// 替换
                    }
                }
            }
            else
            {
                nOpt = 2;// 替换
            }

            wTep = pMs->m_szMessage[i];
            pMs->m_szMessage[i] = 0;
            (void)WSTRCPY(wstrTep, &pMs->m_szMessage[nPos]);
            pMs->m_szMessage[i] = wTep;
        }
        else
        {// 最后一部分不需要插入
            (void)WSTRCPY(wstrTep, &pMs->m_szMessage[nPos]);
            i = nLen;
        }
        
        nTepLen = WSTRLEN(wstrTep);
        if (nTepLen > 0)
        {
            RbmpBiDiReOrder_WMS(wstrTep, &wtrBuf[j], WSTRLEN(wstrTep), 1, 0);
            j = WSTRLEN(wtrBuf);
            
            if (nOpt != 0)
            {
                wtrBuf[j] = INSERT_AECHAR;
                j++;
            }
            nBufLen = nBufMax-j;
        }
        
        nPos = i+1;
    } while (nPos < nLen);
    
    // 重新打包消息
    puserdata->encoding = WMS_ENCODING_LATIN_HEBREW;
    {
        byte *pTep = (byte *)puserdata->data;
        
        puserdata->number_of_digits = j;
        puserdata->data_len = j;
        puserdata->padding_bits = 0;
        
        for (i=0; i<j; i++,pTep++)
        {
            if (wtrBuf[i] == 0x203E)
            {
                *pTep = 0xAF;
            }
            else if (wtrBuf[i] == 0x00D7)
            {
                *pTep = 0xAA;
            }
            else
            {
                wTep = Ucs2ToHebrew[wtrBuf[i]&0x00FF];
                
                *pTep = (wTep == NON_HEB ? 0x20 : (byte)wTep);
            }
        }
    }
    (void) wms_ts_encode(pclt_ts, &pclt_msg->u.cdma_message.raw_ts);
    
FORMATMESSAGE_EXIT:
    if (NULL != pMs)
    {
        SYS_FREEIF(pMs->m_szMessage);
        SYS_FREEIF(pMs);
    }
    SYS_FREEIF(pclt_ts);
    SYS_FREEIF(wtrBuf);
}

/*==============================================================================
函数:
    wmsutil_decodemessagefromview2logic

说明:
    将消息由可视顺序转为逻辑顺序。

参数:
    pclt_msg [in/out]: wms_client_message_s_type 型指针。

返回值:
    none
    
备注:
    本函数是函数 wmsutil_formatmessage 的逆操作。

==============================================================================*/
void wmsutil_decodemessagefromview2logic(wms_client_message_s_type *pclt_msg)
{
    WMSMessageStruct *pMs = NULL;
    AECHAR           *wtrBuf = NULL;
    AECHAR           *wtrP = NULL;
    AECHAR           wTep;
    int32            nSize = 0;
    int nLen, i, j, nBufLen, nPos, nBufMax;
    wms_client_ts_data_s_type *pclt_ts = NULL;
    wms_cdma_user_data_s_type *puserdata = NULL;
    
    if (NULL == pclt_msg)
    {
        return;
    }
    
    pMs = (WMSMessageStruct *)sys_malloc(sizeof(WMSMessageStruct));
    if (NULL == pMs)
    {
        return;
    }
    
    WmsApp_ConvertClientMsgToMS(pclt_msg, pMs);
    if (NULL == pMs->m_szMessage)
    {
        SYS_FREEIF(pMs);
        return;
    }
    
    nLen = WSTRLEN(pMs->m_szMessage);
    nBufLen = 3*(nLen+1)+3;
    nBufMax = nBufLen;
    nSize = nBufLen*sizeof(AECHAR);
    wtrBuf = (AECHAR *)sys_malloc(nSize);
    if (NULL == wtrBuf)
    {
        goto DECODEMESSAGEFROMVIEW2LOGIC_EXIT;
    }
    MEMSET(wtrBuf, 0, nSize);
    
    pclt_ts = (wms_client_ts_data_s_type *)sys_malloc(sizeof(wms_client_ts_data_s_type));
    if (NULL == pclt_ts)
    {
        goto DECODEMESSAGEFROMVIEW2LOGIC_EXIT;
    }
    MEMSET(pclt_ts, 0, sizeof(wms_client_ts_data_s_type));
    if (WMS_OK_S != wms_ts_decode(&pclt_msg->u.cdma_message.raw_ts, pclt_ts))
    {
        goto DECODEMESSAGEFROMVIEW2LOGIC_EXIT;
    }
    puserdata = &pclt_ts->u.cdma.user_data;
    
    nPos = 0;
    j = 0;
    do
    {
        wtrP = WSTRCHR(&pMs->m_szMessage[nPos], INSERT_AECHAR);
        
        if (wtrP == NULL)
        {
            RbmpBiDiReOrder_WMS(&pMs->m_szMessage[nPos], &wtrBuf[j], WSTRLEN(&pMs->m_szMessage[nPos]), 0, 1);
            nPos = nLen;
        }
        else
        {
            if (wtrP == &pMs->m_szMessage[nPos])
            {
                wtrBuf[j] = CONDTION_AECHAR;
                j++;
                nPos++;
                nBufLen--;
            }
            else
            {
                wTep = *wtrP;
                *wtrP = 0;
                RbmpBiDiReOrder_WMS(&pMs->m_szMessage[nPos], &wtrBuf[j], WSTRLEN(&pMs->m_szMessage[nPos]), 0, 1);
                j = WSTRLEN(wtrBuf);
                wtrBuf[j] = CONDTION_AECHAR;
                j++;
                wtrBuf[j] = 0;
                nBufLen = nBufMax - j;
                *wtrP = wTep;
                nPos =  (wtrP + 1) - pMs->m_szMessage;
            }
        }
    } while (nPos < nLen);
        
    
    // 重新打包消息
    puserdata->encoding = WMS_ENCODING_LATIN_HEBREW;
    {
        byte *pTep = (byte *)puserdata->data;
        
        j = WSTRLEN(wtrBuf);
        puserdata->number_of_digits = j;
        puserdata->data_len = j;
        puserdata->padding_bits = 0;
        
        for (i=0; i<j; i++,pTep++)
        {
            if (wtrBuf[i] == 0x203E)
            {
                *pTep = 0xAF;
            }
            else if (wtrBuf[i] == 0x00D7)
            {
                *pTep = 0xAA;
            }
            else
            {
                wTep = Ucs2ToHebrew[wtrBuf[i]&0x00FF];
                
                *pTep = (wTep == NON_HEB ? 0x20 : (byte)wTep);
            }
        }
    }
    (void) wms_ts_encode(pclt_ts, &pclt_msg->u.cdma_message.raw_ts);
    
DECODEMESSAGEFROMVIEW2LOGIC_EXIT:
    if (NULL != pMs)
    {
        SYS_FREEIF(pMs->m_szMessage);
        SYS_FREEIF(pMs);
    }
    SYS_FREEIF(pclt_ts);
    SYS_FREEIF(wtrBuf);
}
#endif
/*==============================================================================
函数:
    GetNumCallbackFromRecentCall

说明:
    用于从通话历史记录取号码的回调函数。

参数:
    pNode [in]: 选定记录的字段。

返回值:
    TRUE: recentcalls applet 应退出。
    FALSE: recentcalls applet 不退出。

备注:
    函数会修改WMS对象的 m_pSendList 中的某个成员项。
        
==============================================================================*/
static boolean GetNumCallbackFromRecentCall(sSelectRecordListNode* pNode)
{
    int32    nMaxItems = 0;
    CMultiSendItemInfo *pItem = NULL;
    sSelectRecordListNode *pSelectRecordNode = pNode;
    
    if(NULL == pNode)
    {
        return TRUE;
    }
    //if (gpWMSApp->m_eMakeAddListMode != MAKEADDLIST_NONE)
    //{
    nMaxItems = MAXNUM_MULTISEND - IVector_Size(gpWMSApp->m_pSendList);
    //}


    if(nMaxItems <= 0)
    {
        return TRUE;
    }

    while(NULL != pSelectRecordNode)
    {
        if(NULL != pSelectRecordNode->SelectRecord.number && (WSTRLEN((AECHAR *)pSelectRecordNode->SelectRecord.number)>0))
        {
            
            if (gpWMSApp->m_currState == WMSST_SENDOPTS)
            {// Callback Number Enter:
                (void)WSTRCPY(gpWMSApp->m_msSend.m_szCallBkNum, (AECHAR *)(pSelectRecordNode->SelectRecord.number));
                return TRUE;
            }/**/
            
            pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
            
            if (NULL == pItem)
            {// 缓冲区分配不成功
                return TRUE;
            }

            if (SUCCESS != IVector_AddElement(gpWMSApp->m_pSendList, pItem))
            {
                sys_free(pItem);
                return TRUE;
            }
            (void)WSTRCPY(pItem->m_szTo, (AECHAR *)(pSelectRecordNode->SelectRecord.number));
            if(WSTRLEN(pSelectRecordNode->SelectRecord.name)>0 && NULL != pSelectRecordNode->SelectRecord.name)
            {
                (void)WSTRCPY(pItem->m_szName, (AECHAR *)(pSelectRecordNode->SelectRecord.name));
            }
            else
            {
                (void)WSTRCPY(pItem->m_szName, (AECHAR *)(pSelectRecordNode->SelectRecord.number));
            }
            pItem = NULL;
        }
        
        nMaxItems--;
        if(nMaxItems <= 0)
        {
            break;
        }

        pSelectRecordNode = pSelectRecordNode->pNextNode;
    }

    return TRUE;

}
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
void WMSExtApp_GetNumFromRecentCalls(WmsApp *pMe, GetAddr_Type eType)
{
    IRecentCalls   *pRecentCall = NULL;

    if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
        AEECLSID_APP_RECENTCALL,
        (void**)&pRecentCall))
    {
        if (pRecentCall == NULL)
        {
            return;
        }
        
        gpWMSApp = pMe;
        switch(eType)
        {
            case GETADDR_MULTIADD:
                IRecentCalls_CallList(pRecentCall, GetNumCallbackFromRecentCall, SELECT_MULTI_NUMBER);
                break;

            case GETADDR_ONEADD:
                IRecentCalls_CallList(pRecentCall, GetNumCallbackFromRecentCall, SELECT_ONE_NUMBER);
                break;

            default:
                break;
        }
        IRecentCalls_Release(pRecentCall);

    } 
}

#ifdef FEATURE_POWERUP_REGISTER_CHINAUNICOM
/*==============================================================================
函数：
    ISO_13818_CRC32

说明：
    函数用于计算字符串的 CRC32 校验码。

参数：
       
返回值：
    TRUE: 初始化完成
    FALSE: 初始化未完成

备注:
       
==============================================================================*/
uint32 ISO_13818_CRC32(char *pdata, int nlen)
{
    uint32 *CRCTable=NULL;
    uint32 crc, i, j;
    uint32 poly = 0xEDB88320L;
    char *pTep;
    //uint8 p[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26};
    
    if ((NULL == pdata) || (nlen == 0))
    {
        return 0;
    }
    
    
    CRCTable = (uint32 *)sys_malloc(256*sizeof(uint32));
    if (NULL == CRCTable)
    {
        return 0;
    }
    
    /*
    // poly 生成算法，结果为 0xEDB88320L
    poly = 0L;
    for (i = 0; i < sizeof(p)/sizeof(uint8); i++)
    {
        poly |= 1L << (31 - p[i]);
    }
    */

    // 生成CRC32表
    for(i=0; i<256; i++)
    {
        crc = i;
        
        for (j=0; j<8; j++)
        {
            crc = (crc & 1) ? (poly ^ (crc >> 1)) : (crc >> 1);
        }
        
        CRCTable[i] = crc;
    }

    // 计算给定字节序列 CRC32
    pTep = pdata;
    crc = 0xFFFFFFFFL;
    i = nlen;
    while(i > 0)
    {
        crc = CRCTable[(crc ^ (*pTep)) & 0xFF] ^ (crc >> 8);
        pTep++;
        i--;
    }
    
    SYS_FREEIF(CRCTable);
    
    return crc^0xFFFFFFFFL;
}

/*==============================================================================
函数：
    GetRegisterInfo(char *szRegInfo, int nSize)

说明：
    本函数用于取得注册信息。

参数：
    szRegInfo [in]：指向用于返回注册信息的char指针。
    nSize [in]：    缓存区的大小。(单位：byte) 

返回值：
    >=0:    成功,数字为消息占用的字节数
    <0:    失败

备注：
    nSize 不得小于 140 
==============================================================================*/
int GetRegisterInfo(char *szRegInfo, int nSize)
{
    int  nlen;
    AEEMobileInfo     mi;
    uint32 dwCRC32;
    
    if (NULL == szRegInfo  || nSize<140)
    {
        return -1;
    }
        
    MEMSET(szRegInfo, 0, nSize);
    // 协议版本--使用ESN
    szRegInfo[0] = 1;
        
    //  命令类型--移动台自动注册消息
    szRegInfo[1] = 3;
    
    // 填充位
    szRegInfo[3] = 0;
    
    // 格式化数据：机型、手机ESN、IMSI号码、当前软件版本
    //OEM_GetConfig (CFGI_MOBILEINFO, &mi, sizeof(AEEMobileInfo));
    GetMobileInfo(&mi);
    SPRINTF(&szRegInfo[4], "<a1><b1>%s</b1><b2>%08x</b2><b3>%s</b3><b4>%s</b4></a1>",
            OEMNV_DEFAULT_BANNER, mi.dwESN, mi.szMobileID, ver_modelversion);
    
    nlen = STRLEN(&szRegInfo[4]);
    
    // 数据长度
    szRegInfo[2] = nlen;
    
    nlen = nlen+4;
    // 计算 CRC32 校验码
    
    dwCRC32 = ISO_13818_CRC32(szRegInfo, nlen);
    SPRINTF(&szRegInfo[nlen], "%08x", dwCRC32);
        
    return (nlen+8);
}

wms_client_message_s_type *GetRegisterMsg()
{
    char  *pBuf=NULL;
    int   nMsgSize = 0;
    int   nSize;
    wms_cdma_user_data_s_type    *pUserdata = NULL;
    wms_client_message_s_type    *pCltMsg = NULL;

    
    nSize = sizeof(char)*150;
    pBuf = (char *)sys_malloc(nSize);
    if (NULL == pBuf)
    {
        goto GETREGISTERMSG_EXIT;
    }
    nMsgSize = GetRegisterInfo(pBuf, 150);
    if (nMsgSize<0)
    {
        goto GETREGISTERMSG_EXIT;
    }
    
    nSize = sizeof(wms_cdma_user_data_s_type);
    pUserdata = (wms_cdma_user_data_s_type *)sys_malloc(nSize);
    if (NULL == pUserdata)
    {
        goto GETREGISTERMSG_EXIT;
    }
    MEMSET(pUserdata, 0, nSize);
    pUserdata->encoding = WMS_ENCODING_OCTET;
    pUserdata->number_of_digits = nMsgSize;
    pUserdata->data_len = nMsgSize;
    pUserdata->padding_bits = 0;
    MEMCPY(pUserdata->data, pBuf, nMsgSize);
    
    pCltMsg = GetMOClientMsg(REG_SERVERNUM, pUserdata, TRUE);
    
GETREGISTERMSG_EXIT:
    SYS_FREEIF(pBuf);
    SYS_FREEIF(pUserdata);
    
    return pCltMsg;
}
#endif

#ifdef FEATURE_SEAMLESS_SMS
static char mintable[10] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };
extern nv_stat_enum_type ui_get_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
);

int GetSeamlessSMSInfo(char *szInfoEx, int nSize)
{
    int len = 0;
    AEEMobileInfo     mi;
    JulianType        julian;
    nv_item_type      nvi;
    char* txt;         // destination text
    word temp;         // working buffer
    dword value;       // to store value read from nv
	int i=0;
	char szInfo[150] = {0};
	int count = 0;

	MSG_FATAL("GetSeamlessSMSInfo!!!",0,0,0);
    //if (NULL == szInfo)
    //{
    //    return -1;
    //}
    
    GetMobileInfo(&mi);
    GETJULIANDATE(0, &julian);
    
    // 格式化数据：品牌、机型、手机ESN、MDN、MIN、当前软件版本、时间
#ifdef FEATURE_VERSION_FLEXI203P
    STRCPY(szInfo, "IV;C-500Plus;");
#else
    STRCPY(szInfo, "IV;C500;");
#endif
    len = STRLEN(szInfo);
    SPRINTF(&szInfo[len],"%x", mi.dwESN);
    len += STRLEN(&szInfo[len]);
    szInfo[len]=';';
    len++;
        	
    //MDN不需要
    szInfo[len]=';';
    len++;
    
    // GetMIN
    // read MIN2
    txt = &szInfo[len];
    nvi.min2.nam = mi.nCurrNAM;
    if( ui_get_nv(NV_MIN2_I, &nvi) != NV_DONE_S)
    {
    	MSG_FATAL("Get NV_MIN2_I Failed!!!",0,0,0);
		return 0;
    }
    else
    {
    	MSG_FATAL("Get NV_MIN2_I : 0x%x",nvi.min2.min2[1],0,0);
    	value = nvi.min2.min2[1];
    }

    *txt++ = mintable[ (value/100) %10];
    value %= 100;
    *txt++ = mintable[ value/10 ];
    *txt++ = mintable[ value%10 ];

    // read MIN1
    nvi.min1.nam = mi.nCurrNAM;
	if( ui_get_nv(NV_MIN1_I, &nvi) != NV_DONE_S)
	{
		MSG_FATAL("Get NV_MIN1_I Failed!!!",0,0,0);
		return 0;
 	}
 	else
 	{
 		MSG_FATAL("Get NV_MIN1_I : 0x%x",nvi.min1.min1[1],0,0);
 		value = nvi.min1.min1[1];
    }

	MSG_FATAL("***zzg GetSeamlessSMSInfo  value=%d***", value, 0, 0);

    temp = (word) (value>>14 );
    *txt++ = mintable[ (temp/100) %10];
    temp %= 100;
    *txt++ = mintable[ temp/10 ];
    *txt++ = mintable[ temp%10 ];
    value &= 0x3FFFL;                /* get bottom 14 bits */
    /* next digit is top 4 bits */
    temp = (word) (( value >> 10 ) & 0xF );
    *txt++ = (char) ( ( ( temp == 10 ) ? 0 : temp ) + '0' );
    temp = (word) ( value & 0x3FF ); /* get bottom 10 bits */
    *txt++ = mintable[ (temp/100) %10];
    temp %= 100;
    *txt++ = mintable[ temp/10 ];
    *txt++ = mintable[ temp%10 ];
    *txt++ = (char)';';
    *txt = '\0';
    len += STRLEN(&szInfo[len]);

    SPRINTF(&szInfo[len], "%s;", ver_modelversion);
    len += STRLEN(&szInfo[len]);
    SPRINTF(&szInfo[len], "%04d%02d%02d%02d%02d", julian.wYear,julian.wMonth,julian.wDay,julian.wHour,julian.wMinute);
    len += STRLEN(&szInfo[len]);

	for(i=0; i<len; i++)
	{
		SPRINTF(szInfoEx+count,"%x",szInfo[i]);
		count = STRLEN(szInfoEx);
	}

	len = 2*len;
	MSG_FATAL("GetSeamlessSMSInfo End!!!",0,0,0);
    return len;
}

wms_client_message_s_type *GetSeamlessSMS()
{
    char  *pBuf=NULL;
    int   nMsgSize = 0;
    int   nSize;
    wms_cdma_user_data_s_type    *pUserdata = NULL;
    wms_client_message_s_type    *pCltMsg = NULL;


    MSG_FATAL("Send SeamLess SMS!",0,0,0);
    nSize = sizeof(char)*300;
    pBuf = (char *)sys_malloc(nSize);
    if (NULL == pBuf)
    {
        goto GETREGISTERMSG_EXIT;
    }
    nMsgSize = GetSeamlessSMSInfo(pBuf, 300);
    if (nMsgSize<=0)
    {
        goto GETREGISTERMSG_EXIT;
    }
    
    nSize = sizeof(wms_cdma_user_data_s_type);
    pUserdata = (wms_cdma_user_data_s_type *)sys_malloc(nSize);
    if (NULL == pUserdata)
    {
        goto GETREGISTERMSG_EXIT;
    }
    MEMSET(pUserdata, 0, nSize);
    pUserdata->encoding = WMS_ENCODING_OCTET;
    pUserdata->number_of_digits = nMsgSize;
    pUserdata->data_len = nMsgSize;
    pUserdata->padding_bits = 0;
    MEMCPY(pUserdata->data, pBuf, nMsgSize);
    
    pCltMsg = GetMOClientMsg(SEAMLESSSMS_SERVERNUM, pUserdata, FALSE);
    
GETREGISTERMSG_EXIT:
    SYS_FREEIF(pBuf);
    SYS_FREEIF(pUserdata);
    
    return pCltMsg;
}
#endif


#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)
void ICCID_ReadCb(ICardSession *po)
{
	
}

wms_client_message_s_type *GetMobileTrackerSMS()
{

	char  *pBuf=NULL;
    int   nMsgSize = 0;
    int   nSize;
	int nErr = AEE_SUCCESS;
	uint16 wDate[20] = {0};
	char strDate[20] = {0};
	char ICCID[20] = {"4105692345"};
	int len = 0;
	ICardSession*			m_pICardSession;
    wms_cdma_user_data_s_type    *pUserdata = NULL;
    wms_client_message_s_type    *pCltMsg = NULL;
	uint64 meid = 0;
	uint32 H32,L32;
	AECHAR	fmt_str[20];
	AECHAR szBuf[17]={0};
	char   strBuf[17]={0};
	int n = 0;
	IShell *pIShell = AEE_GetShell();
	AEECardSessionReadTpStatus	*m_pReadStatus = 0;
	AEECallback				m_cbRead;
	AEEMobileInfo     mi;
	GetMobileInfo(&mi);

	/*
	nErr = ISHELL_CreateInstance(pIShell, AEECLSID_CARDSESSION, (void **)&m_pICardSession);
	MSG_FATAL("0000000000000000000000000000",0,0,0);
	if(!nErr)
	{
		CALLBACK_Init(&m_cbRead, (PFNNOTIFY)ICCID_ReadCb, (void*)(&m_pICardSession));
	}
	m_pReadStatus->pReadData->nLen = 10;
	m_pReadStatus->pReadData->pData = (uint8*)ICCID;
	MSG_FATAL("1111111111111111111111111111",0,0,0);
	nErr = ICARDSESSION_ReadTransparent(
			m_pICardSession, 
			AEECARDSESSION_ICCID, 
			0,
			10,
			m_pReadStatus, 
			&m_cbRead
		);
		
		if (nErr != SUCCESS)
		{
			MSG_FATAL("ICARDSESSION_ReadTransparent failed",0,0,0);
		}
		else 
		{
			MSG_FATAL("ICARDSESSION_ReadTransparent SUCCESS",0,0,0);
		}
	*/
    nSize = sizeof(char)*120;
    pBuf = (char *)sys_malloc(nSize);
    if (NULL == pBuf)
    {
        goto GETREGISTERMSG_EXIT;
    }

	#ifdef FEATURE_VERSION_W515V3
    extern int OEM_ReadMEID(uint64 *meid);
	OEM_ReadMEID(&meid);
    #else
    tmc_get_stored_meid_me((qword *)&meid);
    #endif
	L32 = (uint32)meid;
    H32 = (uint32)(meid>>32);
	
	STRCPY(pBuf, "ARC8c");
	STRCAT(pBuf,"\n");
	
	STRCAT(pBuf, "MEID:");
	STRTOWSTR("%06X", fmt_str, sizeof(fmt_str));
	n = WSTRLEN(szBuf);
	MSG_FATAL("n========%d",n,0,0);
    WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            H32
            );
    n = WSTRLEN(szBuf);
    STRTOWSTR("%08X", fmt_str, sizeof(fmt_str));
    WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            L32
            );
	n = WSTRLEN(szBuf);
	MSG_FATAL("2222n========%d",n,0,0);
	WSTRTOSTR(szBuf,strBuf,sizeof(strBuf));
	STRCAT(pBuf,strBuf);
	STRCAT(pBuf,"\n");
	
	STRCAT(pBuf, "IMSI:");
	STRCAT(pBuf, mi.szMobileID);
	STRCAT(pBuf,"\n");
	
	STRCAT(pBuf, "ICCID:");
	STRCAT(pBuf,  ICCID);
    nMsgSize = STRLEN(pBuf);
	//nMsgSize = nMsgSize;
    if (nMsgSize<=0)
    {
        goto GETREGISTERMSG_EXIT;
    }
    
    nSize = sizeof(wms_cdma_user_data_s_type);
    pUserdata = (wms_cdma_user_data_s_type *)sys_malloc(nSize);
    if (NULL == pUserdata)
    {
        goto GETREGISTERMSG_EXIT;
    }
    MEMSET(pUserdata, 0, nSize);
    pUserdata->encoding = WMS_ENCODING_ASCII;
	pUserdata->data_len = nMsgSize;
    pUserdata->number_of_digits =  wms_ts_pack_ascii(pBuf,
                                                     pUserdata->data,
                                                     &pUserdata->data_len,
                                                     &pUserdata->padding_bits);

	MSG_FATAL("Send MobileTracker SMS!=%d",nMsgSize,0,0);
	
	OEM_GetConfig(CFGI_MOBILE_TRACKER_PHONENUMB, wDate, sizeof(wDate));
	WSTRTOSTR(wDate,strDate,sizeof(strDate));
    len = STRLEN(strDate);
	MSG_FATAL("Send MobileTracker num len====%d",len,0,0);
    pCltMsg = GetMOClientMsg(strDate, pUserdata, FALSE);
    
    
GETREGISTERMSG_EXIT:
    SYS_FREEIF(pBuf);
    SYS_FREEIF(pUserdata);
    if (m_pICardSession)
	{
		IBASE_Release((IBase *)m_pICardSession);
	}
	if (m_pReadStatus)
	{
		if (m_pReadStatus->pReadData)
		{			
			FREE(m_pReadStatus->pReadData);
		}
		FREE(m_pReadStatus);
	}

	m_pReadStatus = 0;
    return pCltMsg;
}

wms_client_message_s_type *GetSmsTrackerSms()
{
	char  *pBuf=NULL;
    int   nMsgSize = 0;
    int   nSize;
	int nErr = AEE_SUCCESS;
	uint16 wDate[20] = {0};
	char strDate[20] = {0};
	char strnumber[20] = {0};
	char strtempnumber[20] = {0};
	char ICCID[20] = {"4105692345"};
	int len = 0;
	ICardSession*			m_pICardSession;
    wms_cdma_user_data_s_type    *pUserdata = NULL;
    wms_client_message_s_type    *pCltMsg = NULL;
	uint64 meid = 0;
	uint32 H32,L32;
	AECHAR	fmt_str[20];
	AECHAR szBuf[17]={0};
	AECHAR szBuf2[3]={0};
	char   strBuf[17]={0};
	char   strBuf2[3]={0};
	int n = 0;
	IShell *pIShell = AEE_GetShell();
	AEECardSessionReadTpStatus	*m_pReadStatus = 0;
	AEECallback				m_cbRead;
	AEEMobileInfo     mi;
	char temp[5] = {0};
	GetMobileInfo(&mi);
	/*
	
	nErr = ISHELL_CreateInstance(pIShell, AEECLSID_CARDSESSION, (void **)&m_pICardSession);

	if(!nErr)
	{
		CALLBACK_Init(&m_cbRead, (PFNNOTIFY)ICCID_ReadCb, (void*)(&m_pICardSession));
	}
	
	nErr = ICARDSESSION_ReadTransparent(
			m_pICardSession, 
			AEECARDSESSION_ICCID, 
			0,
			10,
			m_pReadStatus, 
			&m_cbRead
		);
		
		if (nErr != SUCCESS)
		{
			MSG_FATAL("ICARDSESSION_ReadTransparent failed",0,0,0);
		}
		else 
		{
			MSG_FATAL("ICARDSESSION_ReadTransparent SUCCESS",0,0,0);
		}

	*/	
	
    nSize = sizeof(char)*140;
    pBuf = (char *)sys_malloc(nSize);
    if (NULL == pBuf)
    {
        goto GETREGISTERMSG_EXIT;
    }

	#ifdef FEATURE_VERSION_W515V3
    extern int OEM_ReadMEID(uint64 *meid);
	OEM_ReadMEID(&meid);
    #else
    tmc_get_stored_meid_me((qword *)&meid);
    #endif
	L32 = (uint32)meid;
    H32 = (uint32)(meid>>32);

	//W317A  销售统计
	#ifdef FEATURE_VERSION_W317A
	STRCPY(pBuf, "ARC8c MEID:");
	#else
	STRCPY(pBuf, "C260 MEID:");
	#endif
	STRTOWSTR("%06X", fmt_str, sizeof(fmt_str));
	n = WSTRLEN(szBuf);
	MSG_FATAL("n========%d",n,0,0);
    WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            H32
            );
    n = WSTRLEN(szBuf);
    STRTOWSTR("%08X", fmt_str, sizeof(fmt_str));
    WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            L32
            );
	n = WSTRLEN(szBuf);
	MSG_FATAL("2222n========%d",n,0,0);
	WSTRTOSTR(szBuf,strBuf,sizeof(strBuf));
	STRCAT(pBuf,strBuf);
	STRCAT(pBuf,"\n");
	
	STRCAT(pBuf, "IMSI:");
	STRCAT(pBuf, mi.szMobileID);
	STRCAT(pBuf,"\n");
	
	STRCAT(pBuf, "ICCID:");
	STRCAT(pBuf,ICCID);
	
	
    nMsgSize = STRLEN(pBuf);
	//nMsgSize = nMsgSize;
    if (nMsgSize<=0)
    {
        goto GETREGISTERMSG_EXIT;
    }
    
    nSize = sizeof(wms_cdma_user_data_s_type);
    pUserdata = (wms_cdma_user_data_s_type *)sys_malloc(nSize);
    if (NULL == pUserdata)
    {
        goto GETREGISTERMSG_EXIT;
    }
    MEMSET(pUserdata, 0, nSize);
    pUserdata->encoding = WMS_ENCODING_ASCII;
	pUserdata->data_len = nMsgSize;
    pUserdata->number_of_digits =  wms_ts_pack_ascii(pBuf,
                                                     pUserdata->data,
                                                     &pUserdata->data_len,
                                                     &pUserdata->padding_bits);
    OEM_GetConfig(CFGI_SMS_TRACKER_NUMBER, strnumber, sizeof(strnumber));
	if(STRCMP(strnumber,"+919211722715") ==0)
	{
		STRCPY(strnumber,"9211722715");
	}

	if(STRCMP(strnumber,"09211722715") ==0)
	{
		STRCPY(strnumber,"9211722715");
	}

	if(STRCMP(strnumber,"+919212230707") == 0)
	{
		STRCPY(strnumber,"9212230707");
	}
	STRNCPY(temp,strnumber,3);

	if(STRCMP(temp,"+91") == 0)
	{
		MSG_FATAL("000000000000",0,0,0);
		STRCPY(strtempnumber,strnumber+3);
		STRCPY(strnumber,strtempnumber);
	}
	
	DBGPRINTF("strnumber.............%s",strnumber);
	
    pCltMsg = GetMOClientMsg(strnumber, pUserdata, FALSE);
    
    
GETREGISTERMSG_EXIT:
    SYS_FREEIF(pBuf);
    SYS_FREEIF(pUserdata);
    if (m_pICardSession)
	{
		IBASE_Release((IBase *)m_pICardSession);
	}
	if (m_pReadStatus)
	{
		if (m_pReadStatus->pReadData)
		{			
			FREE(m_pReadStatus->pReadData);
		}
		FREE(m_pReadStatus);
	}

	m_pReadStatus = 0;
    return pCltMsg;
}
#endif


#ifdef FEATURE_VERSION_C337
wms_client_message_s_type *GetMIZONESMS()
{

	char  *pBuf=NULL;
    int   nMsgSize = 0;
    int   nSize;
	uint16 wDate[20] = {0};
	char strDate[64] = {0};
	int len = 0;
    wms_cdma_user_data_s_type    *pUserdata = NULL;
    wms_client_message_s_type    *pCltMsg = NULL;
	uint64 meid = 0;
	uint32 H32,L32;
	AECHAR	fmt_str[20];
	AECHAR szBuf[17]={0};
	char   strBuf[17]={0};
	int n = 0;
	AEEMobileInfo     mi;
	GetMobileInfo(&mi);
    nSize = sizeof(char)*120;
    pBuf = (char *)sys_malloc(nSize);
    if (NULL == pBuf)
    {
        goto GETREGISTERMSG_EXIT;
    }

	#ifdef FEATURE_VERSION_W515V3
    extern int OEM_ReadMEID(uint64 *meid);
	OEM_ReadMEID(&meid);
    #else
    tmc_get_stored_meid_me((qword *)&meid);
    #endif
	L32 = (uint32)meid;
    H32 = (uint32)(meid>>32);

    OEM_GetConfig(CFGI_MIZONE_SMSINFO, strDate, sizeof(strDate));
    //WSTRTOSTR(wDate,strDate,sizeof(strDate));
	//WSTRTOSTR(wDate,fmt_str,sizeof(fmt_str));
    
	STRCPY(pBuf, strDate);
	
    nMsgSize = STRLEN(pBuf);
	//nMsgSize = nMsgSize;
    if (nMsgSize<=0)
    {
        goto GETREGISTERMSG_EXIT;
    }
    
    nSize = sizeof(wms_cdma_user_data_s_type);
    pUserdata = (wms_cdma_user_data_s_type *)sys_malloc(nSize);
    if (NULL == pUserdata)
    {
        goto GETREGISTERMSG_EXIT;
    }
    MEMSET(pUserdata, 0, nSize);
    pUserdata->encoding = WMS_ENCODING_ASCII;
	pUserdata->data_len = nMsgSize;
    pUserdata->number_of_digits =  wms_ts_pack_ascii(pBuf,
                                                     pUserdata->data,
                                                     &pUserdata->data_len,
                                                     &pUserdata->padding_bits);

	MSG_FATAL("Send MobileTracker SMS!=%d",nMsgSize,0,0);
	
	OEM_GetConfig(CFGI_MIZONE_NUM, strDate, sizeof(strDate));
	//WSTRTOSTR(wDate,strDate,sizeof(strDate));
    len = STRLEN(strDate);
	MSG_FATAL("Send MobileTracker num len====%d",len,0,0);
    pCltMsg = GetMOClientMsg(strDate, pUserdata, FALSE);
    
    
GETREGISTERMSG_EXIT:
    SYS_FREEIF(pBuf);
    SYS_FREEIF(pUserdata);
    
    return pCltMsg;
}

wms_client_message_s_type *GetMiZoneRegisterMsg()
{
    char  *pBuf=NULL;
    int   nMsgSize = 0;
    int   nSize;
    char strDate[64] = {0};
    wms_cdma_user_data_s_type    *pUserdata = NULL;
    wms_client_message_s_type    *pCltMsg = NULL;

    
    nSize = sizeof(char)*150;
    pBuf = (char *)sys_malloc(nSize);
    if (NULL == pBuf)
    {
        goto GETREGISTERMSG_EXIT;
    }

    OEM_GetConfig(CFGI_MIZONE_SMSINFO, strDate, sizeof(strDate));
    STRCPY(pBuf, strDate);
    
    nMsgSize = STRLEN(pBuf);
    if (nMsgSize<0)
    {
        goto GETREGISTERMSG_EXIT;
    }
    
    nSize = sizeof(wms_cdma_user_data_s_type);
    pUserdata = (wms_cdma_user_data_s_type *)sys_malloc(nSize);
    if (NULL == pUserdata)
    {
        goto GETREGISTERMSG_EXIT;
    }
    MEMSET(pUserdata, 0, nSize);
    pUserdata->encoding = WMS_ENCODING_OCTET;
    pUserdata->number_of_digits = nMsgSize;
    pUserdata->data_len = nMsgSize;
    pUserdata->padding_bits = 0;
    MEMCPY(pUserdata->data, pBuf, nMsgSize);

    OEM_GetConfig(CFGI_MIZONE_NUM, strDate, sizeof(strDate));
    pCltMsg = GetMOClientMsg(strDate, pUserdata, TRUE);
    
GETREGISTERMSG_EXIT:
    SYS_FREEIF(pBuf);
    SYS_FREEIF(pUserdata);
    
    return pCltMsg;
}

#endif

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
wms_client_message_s_type *GetMOClientMsg(char *pszTonum, wms_cdma_user_data_s_type *pUserdata, boolean bTlAck)
{
    int32 nSize;
    wms_client_ts_data_s_type *pCltTsData = NULL;
    wms_client_message_s_type *pCltMsg = NULL;
    
    if ((NULL == pUserdata) ||
        (NULL == pszTonum) ||
        (STRLEN(pszTonum) < 3) ||
        (STRLEN(pszTonum) > MAX_PH_DIGITS))
    {
        return NULL;
    }
    
    nSize = sizeof(wms_client_ts_data_s_type);
    pCltTsData = (wms_client_ts_data_s_type *)sys_malloc(nSize);
    if (NULL == pCltTsData)
    {
        ERR("sys_malloc Failed!",0,0,0);
        return NULL;
    }
    MEMSET(pCltTsData, 0, nSize);
    
    nSize = sizeof(wms_client_message_s_type);
    pCltMsg = (wms_client_message_s_type *)sys_malloc(nSize);
    if (NULL == pCltMsg)
    {
        SYS_FREEIF(pCltTsData);
        ERR("sys_malloc Failed!",0,0,0);
        return NULL;
    }
    
    // -------- 填充 wms_client_ts_data_s_type 结构 --------
    pCltTsData->format = WMS_FORMAT_CDMA;
    pCltTsData->u.cdma.mask = 0;

    // 消息 ID 部分
    pCltTsData->u.cdma.message_id.id_number = 65530;
    pCltTsData->u.cdma.message_id.type = WMS_BD_TYPE_SUBMIT;
    pCltTsData->u.cdma.mask |= WMS_MASK_BD_MSG_ID;

    // 消息用户数据部分
    nSize = sizeof(wms_cdma_user_data_s_type);
    MEMCPY(&pCltTsData->u.cdma.user_data, pUserdata, nSize);
    pCltTsData->u.cdma.mask |= WMS_MASK_BD_USER_DATA;
    
    // 时间戳
    //ConvertMStoMcTime(GETTIMESECONDS(), &pCltTsData->u.cdma.mc_time);
    //pCltTsData->u.cdma.mask |= WMS_MASK_BD_MC_TIME;
    
    // 优先级: 
    pCltTsData->u.cdma.priority = WMS_PRIORITY_URGENT;
    pCltTsData->u.cdma.mask |= WMS_MASK_BD_PRIORITY;
    
    
    // 回复选项: 没有设置则不需要带这个设置
    //pCltTsData->u.cdma.reply_option.user_ack_requested = FALSE;
    //pCltTsData->u.cdma.reply_option.delivery_ack_requested = FALSE;
    //pCltTsData->u.cdma.reply_option.read_ack_requested = FALSE;
    //pCltTsData->u.cdma.mask |= WMS_MASK_BD_REPLY_OPTION;
    
    pCltTsData->u.cdma.num_messages = 1;
    
    // -------- 填充 wms_client_message_s_type 结构 --------
    pCltMsg->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
    pCltMsg->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;
    pCltMsg->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
    pCltMsg->msg_hdr.index = WMS_DUMMY_MESSAGE_INDEX;
    
    pCltMsg->u.cdma_message.is_mo = TRUE;
    pCltMsg->u.cdma_message.teleservice = WMS_TELESERVICE_CMT_95;
    
#ifndef WIN32
    // 接收地址
    if (pszTonum[0] == '+')
    {
        pCltMsg->u.cdma_message.address.number_of_digits = wms_ts_ascii2dtmf(&pszTonum[1], pCltMsg->u.cdma_message.address.digits);
    }
    else
    {
        pCltMsg->u.cdma_message.address.number_of_digits = wms_ts_ascii2dtmf(pszTonum, pCltMsg->u.cdma_message.address.digits);
    }
#endif
    pCltMsg->u.cdma_message.address.digit_mode = WMS_DIGIT_MODE_4_BIT;
    pCltMsg->u.cdma_message.address.number_mode = WMS_NUMBER_MODE_NONE_DATA_NETWORK;
    pCltMsg->u.cdma_message.address.number_type = WMS_NUMBER_UNKNOWN;
    pCltMsg->u.cdma_message.address.number_plan = WMS_NUMBER_PLAN_TELEPHONY;
    
    pCltMsg->u.cdma_message.is_tl_ack_requested = bTlAck;
    pCltMsg->u.cdma_message.is_service_present = FALSE;
#ifndef WIN32
    (void)wms_ts_encode(pCltTsData, &pCltMsg->u.cdma_message.raw_ts);
#endif   
    // 消息编码结束
    
    SYS_FREEIF(pCltTsData);
    
    return pCltMsg;
}

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
wms_client_message_s_type *CWmsApp_Getspecmsg(AECHAR *pwstrType)
{
    char chType = 0;
    
    if (NULL == pwstrType)
    {
        return NULL;
    }
    
    chType = (char) (*pwstrType);
    switch (chType)
    {
#ifdef FEATURE_POWERUP_REGISTER_CHINAUNICOM
        case POWERUP_REGISTER_CHINAUNICOM:
            return GetRegisterMsg();
#endif        
#ifdef FEATURE_SEAMLESS_SMS
        case POWERUP_REGISTER_SEAMLESSSMS:
            return GetSeamlessSMS();
#endif
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)
		case MOBILE_TRACKER_MSG:
			return GetMobileTrackerSMS();
		case SMS_TRACKER_MSG:
			return GetSmsTrackerSms();
#endif
#ifdef FEATURE_VERSION_C337
        case MIZONE_MSG:
            return GetMiZoneRegisterMsg();
#endif
				
        default:
            return NULL;
    }
}

//Add By zzg 2010_07_21
wms_client_message_s_type *CWmsApp_GetspecmsgEx(void)
{
    int   nMsgSize = 0;
    int   nSize;
    wms_cdma_user_data_s_type    *pUserdata = NULL;
    wms_client_message_s_type    *pCltMsg = NULL;
   
	nSize = sizeof(FRENDUO_SMS_BUFF);    
	nMsgSize = sizeof(FRENDUO_SMS_BUFF);
	
    if (nMsgSize<=0)
    {
        goto GETREGISTERMSG_EXIT;
    }
    
    nSize = sizeof(wms_cdma_user_data_s_type);
    pUserdata = (wms_cdma_user_data_s_type *)sys_malloc(nSize);
	
    if (NULL == pUserdata)
    {
        goto GETREGISTERMSG_EXIT;
    }
	
    MEMSET(pUserdata, 0, nSize);
    pUserdata->encoding = WMS_ENCODING_OCTET;
    pUserdata->number_of_digits = nMsgSize;
    pUserdata->data_len = nMsgSize;
    pUserdata->padding_bits = 0;
    MEMCPY(pUserdata->data, FRENDUO_SMS_BUFF, nMsgSize);
    
    pCltMsg = GetMOClientMsg(FRENDUO_SMS_DESTNUM, pUserdata, FALSE);
    
GETREGISTERMSG_EXIT:    
    SYS_FREEIF(pUserdata);
    
    return pCltMsg;    
}
//Add End

void WMSUtil_SendVMNSim(void)
{
    wms_tl_message_type             tl_msg; 
    wms_OTA_message_type            OTA_msg;
    JulianType                      currTime;

    MEMSET(&tl_msg,0,sizeof(tl_msg));
    
    tl_msg.mask                         = WMS_MASK_TL_TELESERVICE_ID
                                        | WMS_MASK_TL_ADDRESS
                                        | WMS_MASK_TL_BEARER_REPLY_OPTION
                                        | WMS_MASK_TL_BEARER_DATA;
    
    tl_msg.is_mo                        = FALSE;
    tl_msg.tl_message_type              = WMS_TL_TYPE_POINT_TO_POINT;

    // Teleservice
    tl_msg.teleservice                  = WMS_TELESERVICE_VMN_95;
    
    // Address
    tl_msg.address.digit_mode           = WMS_DIGIT_MODE_8_BIT;
    tl_msg.address.number_mode          = WMS_NUMBER_MODE_NONE_DATA_NETWORK;
    tl_msg.address.number_plan          = WMS_NUMBER_PLAN_UNKNOWN;
    tl_msg.address.number_type          = WMS_NUMBER_INTERNATIONAL;
    tl_msg.address.number_of_digits     = 4;
    MEMCPY(tl_msg.address.digits,"1500",tl_msg.address.number_of_digits);

    // Replay
    tl_msg.bearer_reply_seq_num         = 1;

    // Bearer Data
    tl_msg.cl_bd.mask                   = WMS_MASK_BD_MSG_ID
                                        | WMS_MASK_BD_USER_DATA
                                        | WMS_MASK_BD_MC_TIME
                                        | WMS_MASK_BD_PRIORITY
                                        | WMS_MASK_BD_NUM_OF_MSGS;

    // BD MSG ID
    tl_msg.cl_bd.message_id.type        = WMS_BD_TYPE_DELIVER;
    tl_msg.cl_bd.message_id.id_number   = 0;
    tl_msg.cl_bd.message_id.udh_present = FALSE;

    // BD USER DATA
    tl_msg.cl_bd.user_data.encoding     = WMS_ENCODING_ASCII;
    tl_msg.cl_bd.user_data.data_len     = 4;
    tl_msg.cl_bd.user_data.data[0]      = 0x24;
    tl_msg.cl_bd.user_data.data[1]      = 0x56;
    tl_msg.cl_bd.user_data.data[2]      = 0x20;
    tl_msg.cl_bd.user_data.data[3]      = 0x32;

    // MC TIME
    GETJULIANDATE(0,&currTime);
    tl_msg.cl_bd.mc_time.year           = (uint8)(currTime.wYear-2000);
    tl_msg.cl_bd.mc_time.month          = (uint8)(currTime.wMonth);
    tl_msg.cl_bd.mc_time.day            = (uint8)(currTime.wDay);
    tl_msg.cl_bd.mc_time.hour           = (uint8)(currTime.wHour);
    tl_msg.cl_bd.mc_time.minute         = (uint8)(currTime.wMinute);
    tl_msg.cl_bd.mc_time.second         = (uint8)(currTime.wSecond);

    // PRIORITY
    tl_msg.cl_bd.priority               = WMS_PRIORITY_NORMAL;

    // Number of MSGs
    tl_msg.cl_bd.num_messages           = 0x2;
    
    wms_ts_encode_CDMA_OTA(&tl_msg, &OTA_msg);
    
    wms_msg_cdma_mt_msg( WMS_FORMAT_CDMA,
                         OTA_msg.data_len,
                         OTA_msg.data,
                         WMS_BEARER_CDMA_1X,
                         TRUE, // ack_pending
                         WMS_DUMMY_TRANSACTION_ID // not used
                        );
}
