#include "customer.h"         // Handset Feature Definitions
#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"         // AEE StdLib Services
#include "AEEModGen.h"         // AEEMod Declaration
#include "AEEAppGen.h"         // AEEApplet Declaration
#include "OEMClassIDs.h"

#include "AEEMenu.h"           // AEE Menu Services
#include "AEEFile.h"           // AEE File Services
#include "AEEImage.h"
#include "AEEBitmap.h"
#include "AEEImageCtl.h"
#include "AEEText.h"           // AEE Text Services
#include "AEEMimeTypes.h"  
#ifndef AEE_SIMULATOR  
#include "AEEAlert.h"          // IAlert Services
#include "AEEGraphics.h"
#include "OEMCFGI.h"
#endif

//callback structure definitions.
#include "AEEAnnunciator.h"
#include "AEEBacklight.h"
#include "AEEConfig.h"
#include "AEECM.h"
#include "Appscommon.h"
#include "BREWVersion.h"
#include "Appscommon.h"
#include "AppComFunc.h"
#include "err.h"
#include "AEEAnnunciator.h"
#include "AEEMediaUtil.h"
#include "AEEMedia.h"
#include "db.h"
#include "WapBrowser.h"
#include "AEEStdLib.h"

#define EVT_NF3_ADD_BOOKMARK_OTA    (EVT_USER + 0x1010)
#define EVT_PUSH_DATA_IND                   (EVT_USER + 0x1020)

/* WAP Push definitions*/
/* Must same to defintion in netfront3app.h */
typedef struct
{
	uint16 sm_len;								/* wap push message length */
	uint8 *sm_data;		/* real info */				
}nf3_wap_push_info;

#define nf3_push_info nf3_wap_push_info

struct TBookmarkObj_{
   char* fTitle;
   char* fURL;
   boolean  fProtected;
   boolean  fRet;
   
};
typedef struct TBookmarkObj_ TBookmarkObj;


typedef struct otap_resource_tag
{
    char                        *pszURI;
    char                        *pszName;
    boolean                     bStartPage;
    struct otap_resource_tag    *pNext;
}otap_resource_t;

typedef struct otap_port_tag
{
    uint32          uPortNBR;
    char             *pszService;
}otap_port_t;

typedef struct otap_pxauth_tag
{
    char           *pszAuthType;
    char           *pszUserName;
    char           *pszPassword;
}otap_pxauth_t;

typedef struct otap_pxphysical_tag
{
    char            *pszProxyId;
    char            *pszPxAddr;
    char            *pszPxAddrType;
    char            *pszToNAPId;
    uint8           uProfileNum;
    otap_port_t     *pPort;
}otap_pxphysical_t;

typedef struct otap_pxlogical_tag
{
    char                *pszProxyId;
    char                *pszName;
    char                *pszStartPage;   //增加对首页的解析
    otap_port_t         *pPort;
    otap_pxauth_t       *pPxAuth;
    otap_pxphysical_t   *pPxphysical;
}otap_pxlogical_t;

typedef struct otap_browser_tag
{
    char                 *pszAppId;
    char                 *pszToNAPId;
    char                 *pszName;   //增加应用中NAME的处理20070530
    uint32               uProfileNum;
    char                 *pszToProxy;
    otap_pxlogical_t     *pProxy;
    otap_resource_t      *pBookmark;
}otap_browser_t;


//快速按件事件
#define EVT_APPISREADY   (EVT_USER+30) 
#define Brew_GetAppInstance() (GETAPPINSTANCE())
#define APP_ISHELL (((AEEApplet*)Brew_GetAppInstance())->m_pIShell)

typedef struct _WapBrowserAppMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} CWapBrowserAppMod;

// TVApp Applet对象结构体：
typedef struct _CWapBrowserApp
{
    DECLARE_VTBL(IWapBrowserApp)
    uint32               m_nRefs;          // 对象引用计数器
    IModule              *m_pModule;
    IDisplay             *m_pDisplay;
    IShell               *m_pShell;    
    AEERect               m_rc;
    
    // 当前活动对话框IDialog接口指针
    IDialog              *m_pActiveDlg;
    
    // 当前活动对话框ID
    uint16               m_pActiveDlgID;
    
    // 活动对话框关闭时的返回结果
     
    // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    boolean              m_bNotOverwriteDlgRet;
    
    
    // Applet是否处于挂起状态
    boolean              m_bSuspending;
    
   
} CWapBrowserApp;

