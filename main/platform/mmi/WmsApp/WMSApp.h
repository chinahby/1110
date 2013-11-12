#ifndef WMSAPP_H
#define WMSAPP_H
//*==============================================================================
//           W M S   A P P L E T  M O D U L E
// 文件:
//        WMSApp.h
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明: 
//        
//==============================================================================*/

/*==============================================================================
修改历史记录: 
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/


/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
#include "AEE.h"              // Standard AEE Declarations
#include "AEEShell.h"         // AEE Shell Services

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/
// 一个短信数据包以 OCTET 编码所允许的最大字符数
#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE
// 剔出插入的回车符
#define MAX_OCTETMSG_PAYLOAD    119
#else
#define MAX_OCTETMSG_PAYLOAD    140
#endif

// 一个短信数据包以 ASCII 编码所允许的最大字符数
#define MAX_ASCIIMSG_PAYLOAD    (MAX_OCTETMSG_PAYLOAD*8/7) 

// 一个短信数据包以 UNICODE 编码所允许的最大字符数
#define MAX_UNICODMSG_PAYLOAD   (MAX_OCTETMSG_PAYLOAD/2)


// 对于发送长短信，最大支持发送文本的长度
#if defined(FEATURE_CARRIER_VENEZUELA_MOVILNET)
#define  WMS_MSG_MAXCHARS       MAX_ASCIIMSG_PAYLOAD
#elif defined(FEATURE_CARRIER_THAILAND_HUTCH) || defined(FEATURE_CARRIER_THAILAND_CAT)
#define  WMS_MSG_MAXCHARS       MAX_ASCIIMSG_PAYLOAD
#elif defined(FEATURE_CARRIER_ISRAEL_PELEPHONE)
#define  WMS_MSG_MAXCHARS       MAX_OCTETMSG_PAYLOAD
#else
#ifdef FEATURE_SUPPORT_ID
#define  WMS_MSG_MAXCHARS       1537
#elif defined(FEATURE_VERSION_C316) //Add by pyuangui 20121226
#define  WMS_MSG_MAXCHARS       918
#else
#define  WMS_MSG_MAXCHARS       1024
#endif
#endif

// 对于模板短信，最大支持文本的长度
#define  WMS_TEMPMSG_MAXCHARS   MAX_UNICODMSG_PAYLOAD

#define MAX_PH_DIGITS                  WMS_ADDRESS_MAX       // Maximum phone number digits

#define MAX_TITLE_LEN                  WMS_ADDRESS_MAX       // Maximum title AECHARs

#define MAX_EMAILADD_LEN               64       // 电子邮件地址最大允许长度
#define MAX_URL_LEN                    256      // URL最大允许长度

#define WMSAPP_RELATIVE_VALIDITY_IMMEDIATE          246

// 每条用户定制消息模板最大允许字符数
#define  MAX_TEMPLATECHARS      70

//预约失败短信最大条数
#define  MAX_RSVFAILD        10

#ifdef FEATURE_POWERUP_REGISTER_CHINAUNICOM
// 接收注册信息的服务器号码
#define REG_SERVERNUM  "10659401"

#define  POWERUP_REGISTER_CHINAUNICOM   'R'
#endif
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)||defined(FEATURE_SALESTRACKER) || defined(FEATURE_VERSION_IC241A_MMX)
#define  MOBILE_TRACKER_MSG             'M'
#define SMS_TRACKER_MSG                 'T'
#define SMS_TRACKER_MSG_TWO             'Q'
#define SMS_TRACKER_TOW                 'Y'
#define SMS_TRACKER_THREE               'Z'
#endif

//#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
#define  MIZONE_MSG             'Z'
//#endif

#if defined(FEATURE_VERSION_K212_20D)||defined(FEATURE_VERSION_K212)
#define REG_SERVERNUM       "10659401" //"18126010560"
#define REGHOPE_MSG         'Z'
#endif

#ifdef FEATURE_SEAMLESS_SMS
// 接收注册信息的服务器号码
#define SEAMLESSSMS_SERVERNUM  "02170277582"

#define  POWERUP_REGISTER_SEAMLESSSMS   'S'
#endif
/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/
//Applet启动的不同状态
typedef enum _WMSServiceType 
{
    STARTMETHOD_POWERONSTART,       // ISHELL_StartApplet()
    STARTMETHOD_WMSNORMAL,          // ISHELL_StartApplet()
    STARTMETHOD_SENDTEXTMESSAGE,    // Send message with known address
    STARTMETHOD_WRITENEWMESSAGE,    // write a new message with no address
    STARTMETHOD_SENDSPECMESSAGE,    // Send spec message
    STARTMETHOD_SHOWINBOXLIST,      // Show sms received list
    STARTMETHOD_VIEWVMAIL,          // View Voice mail
    STARTMETHOD_REFRESH             // Refresh UIM SMS
}WMSServiceType;

typedef struct _IWmsApp IWmsApp;

QINTERFACE(IWmsApp)
{
    INHERIT_IApplet(IWmsApp);
    
    // 外部进入短信服务接口函数
    int (*MessageService)(IWmsApp *p, WMSServiceType eStype, AECHAR *toAddrStr, AECHAR *pwstrText);
    int (*GetLeftSendListNodeNum)(IWmsApp *p, int *leftNum);
    int (*DeleteAllNvCdmaSms)(IWmsApp *p);
};

extern boolean gbWmsVMailNtf;
extern boolean gbWmsSMSNtf;
extern boolean gbWmsLastNtfIsSMS;

/*==============================================================================

                                 函数声明

==============================================================================*/
/*==============================================================================
函数: 
    IWmsApp_AddRef

说明: 
    用于更新引用计数的Applet函数。

参数: 
    p [in/out]: IWmsApp接口对象指针。

返回值: 
    当前对 WMS Applet的引用数

备注:

==============================================================================*/
#define IWmsApp_AddRef(p)                GET_PVTBL(p,IWmsApp)->AddRef(p)

/*==============================================================================
函数: 
    IWmsApp_Release

说明: 
    BREW用于释放资源的Applet函数。

参数: 
    p [in/out]: IWmsApp接口对象指针。

返回值: 
    当前对 WMS Applet的引用数

备注:

==============================================================================*/
#define IWmsApp_Release(p)               GET_PVTBL(p,IWmsApp)->Release(p)

/*==============================================================================
函数: 
    IWmsApp_HandleEvent

说明: 
    由BREW调用的SMS Applet事件处理主函数。

参数: 
    p [in/out]: IWmsApp接口对象指针。
    ec [in]:  事件代码
    wp [in]:  事件特定数据
    dw [in]: 事件特定数据

返回值: 
    TRUE: 事件得到处理。
    FALSE: 没有处理事件。

备注:

==============================================================================*/
#define IWmsApp_HandleEvent(p,ec,wp,dw) GET_PVTBL(p,IWmsApp)->HandleEvent(p,  \
    ec,  wp, dw)

/*==============================================================================
接口: 
    IWMSApp_PoweronStart

说明: 
    提供给外部Applet用来启动 WmsApp 开机初始化 Applet 结构数据的助手函数。

参数: 
    pi [in]: 指向IWmsApp接口t对象的指针。

返回值: 
    SUCCESS: Applet启动成功。
    EFAILED: Applet启动失败。

备注:
    该函数供外部程序调用，内部不得调用。
        
==============================================================================*/
#define IWMSApp_PoweronStart(p)  \
    GET_PVTBL(p,IWmsApp)->MessageService(p, STARTMETHOD_POWERONSTART, NULL, NULL);


/*==============================================================================
接口: 
    IWMSApp_SendTextMessage

说明: 
    提供给外部Applet用来发送文本消息至给定号码的助手函数。

参数: 
    pi [in]: 指向IWmsApp接口t对象的指针。
    s [in]: (以null结尾的宽字符串)。消息发往目的地。

返回值: 
    SUCCESS: Applet启动成功。
    EFAILED: Applet启动失败。

备注:
    该函数供外部程序调用，内部不得调用。
        
==============================================================================*/
#define IWMSApp_SendTextMessage(p,s)  \
    GET_PVTBL(p,IWmsApp)->MessageService(p, STARTMETHOD_SENDTEXTMESSAGE, s, NULL)
    
/*==============================================================================
接口: 
    IWmsApp_SendTextMessageExt

说明: 
    提供给外部Applet用来发送给定文本消息至给定号码的助手函数。

参数: 
    pi [in]: 指向IWmsApp接口t对象的指针。
    s [in]: (以null结尾的宽字符串)。消息发往目的地。
    t [in]: (以null结尾的宽字符串)。消息文本。

返回值: 
    SUCCESS: Applet启动成功。
    EFAILED: Applet启动失败。

备注:
    该函数供外部程序调用，内部不得调用。
        
==============================================================================*/
#define IWmsApp_SendTextMessageExt(p,s,t)  \
    GET_PVTBL(p,IWmsApp)->MessageService(p, STARTMETHOD_SENDTEXTMESSAGE, s, t)
    
/*==============================================================================
接口: 
    IWmsApp_SendSpecMessage

说明: 
    提供给外部Applet用来启动发送特殊消息的助手函数。特殊消息的内容、封装及发送
    在短信内实现。外部应用调用该接口启动该函数。

参数: 
    pi [in]: 指向IWmsApp接口对象的指针。
    t [in]:  特殊消息类型。

返回值: 
    SUCCESS: Applet启动成功。
    EFAILED: Applet启动失败。

备注:
    该函数供外部程序调用，内部不得调用。
    特殊消息类型：
        
==============================================================================*/
#define IWmsApp_SendSpecMessage(p,t)  \
    GET_PVTBL(p,IWmsApp)->MessageService(p, STARTMETHOD_SENDSPECMESSAGE, NULL, t)




//Add By zzg 2010_07_20
#define IWmsApp_SendSpecMessageEx(p)  \
	GET_PVTBL(p,IWmsApp)->MessageService(p, STARTMETHOD_SENDSPECMESSAGE, NULL, NULL)



/*==============================================================================
接口: 
    IWmsApp_WriteNewMessage

说明: 
    提供给外部Applet用来发送文本消息至不确定号码的助手函数。

参数: 
    pi [in]: 指向IWmsApp接口t对象的指针。

返回值: 
    SUCCESS: Applet启动成功。
    EFAILED: Applet启动失败。

备注:
    该函数供外部程序调用，内部不得调用。
        
==============================================================================*/
#define IWmsApp_WriteNewMessage(p)  \
    GET_PVTBL(p,IWmsApp)->MessageService(p, STARTMETHOD_WRITENEWMESSAGE, NULL, NULL)

/*==============================================================================
接口: 
    IWmsApp_ShowMessageList

说明: 
    提供给外部Applet用来查看新短信的助手函数。

参数: 
    pi [in]: 指向IWmsApp接口t对象的指针。

返回值: 
    SUCCESS: Applet启动成功。
    EFAILED: Applet启动失败。

备注:
    该函数供BREW调用，用户不得调用。
        
==============================================================================*/
#define IWmsApp_ShowMessageList(p)  \
    GET_PVTBL(p,IWmsApp)->MessageService(p, STARTMETHOD_SHOWINBOXLIST, NULL, NULL)
    
/*==============================================================================
接口: 
    IWmsApp_ViewVMail

说明: 
    提供给外部Applet用来查看语音短信的助手函数。

参数: 
    pi [in]: 指向IWmsApp接口t对象的指针。

返回值: 
    SUCCESS: Applet启动成功。
    EFAILED: Applet启动失败。

备注:
    该函数供BREW调用，用户不得调用。
        
==============================================================================*/
#define IWmsApp_ViewVMail(p)  \
    GET_PVTBL(p,IWmsApp)->MessageService(p, STARTMETHOD_VIEWVMAIL, NULL, NULL)
    
/*==============================================================================
接口: 
    IWmsApp_RefreshRUIMSMS

说明: 
    提供给外部Applet用来通知短信启动更新UIM上消息的助手函数。

参数: 
    pi [in]: 指向IWmsApp接口t对象的指针。

返回值: 
    SUCCESS: Applet启动成功。
    EFAILED: Applet启动失败。

备注:
    该函数供BREW调用，用户不得调用。
        
==============================================================================*/
#define IWmsApp_RefreshRUIMSMS(p)  \
    GET_PVTBL(p,IWmsApp)->MessageService(p, STARTMETHOD_REFRESH, NULL, NULL)


/*==============================================================================
接口: 
    IWmsApp_GetLeftSendListNodeNum

说明: 
    提供给外部Applet用来获取sendList最多还能添加号码的个数

参数: 
    pi [in]: 指向IWmsApp接口t对象的指针。

返回值: 
    SUCCESS: Applet启动成功。
    EFAILED: Applet启动失败。

备注:
        
==============================================================================*/
#define IWmsApp_GetLeftSendListNodeNum(p, l)  \
    GET_PVTBL(p,IWmsApp)->GetLeftSendListNodeNum(p, l)

/*==============================================================================
接口: 
    IWmsApp_DeleteAllNvcdmaSms

说明: 
    提供给外部Applet用来获取sendList最多还能添加号码的个数

参数: 
    pi [in]: 指向IWmsApp接口t对象的指针。

返回值: 
    SUCCESS: 成功。
    EFAILED: 失败。

备注:
        
==============================================================================*/
#define IWmsApp_DeleteAllNvCdmaSms(p)  \
    GET_PVTBL(p,IWmsApp)->DeleteAllNvCdmaSms(p)


#endif // end WMSAPP_H

