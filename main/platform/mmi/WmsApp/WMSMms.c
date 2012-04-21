/*==============================================================================
// 文件：WMSMMS.c
//        2011-07-30初始版本(Init Version)
//        版权所有(c) 2011 WaterWorld Tech. CO.,LTD.
//        
// 文件说明：这个文件主要实现和彩信相关的编解码用的外部接口
//        
// 作者：
// 创建日期：2011-07-30
// 当前版本：Init Version
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------   -----------------------------------------------
2011-07-30       wangliang            初始版本
2011-08-03       wamgliang            增加数据接收和发送函数
2011-08-17       wangliang            增加编码和解码函数
==============================================================================*/

/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/

#ifndef AEE_SIMULATOR
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"  // Simulator Feature Definitions
#endif
#include "AEEEvent.h"
#include "OEMClassIDs.h"      // Applet Ids

#include "WMSApp.h"
#include "AEEHeap.h"          // AEE Memory Allocation Services
#include "AEEMenu.h"          // AEE Menu Services
#include "AEEFile.h"          // AEE File Services
#include "AEEText.h"          // AEE Text Services
#include "AEEStdLib.h"        // AEE StdLib Services

#include "AEEAlert.h"         // IAlert Services
#include "AEEAppGen.h"        // AEEApplet Declaration
// callback structure definitions.
#include "AEEVector.h"
#include "AEEAnnunciator.h"
#include "AEEBacklight.h"
#include "WMSApp_priv.h"
#include "AEEConfig.h"
#include "wms.h"
#include "AEEWMS.h"
#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "AEETelephone.h"
#include "AEETelDef.h"
#endif
#include "OEMWMS_CacheInfo.h"
#include "OEMHeap.h"
#include "wmsi.h"

#include "Appscommon.h"

#include "BREWVersion.h"
#include "oemcfgi.h"
#include "AEESound.h"
#include "AEE_OEM.h"
#include "AEERUIM.h"
#include "OEMCFGI.h"
#include "WMSMMS.h"
#include "AEEMimeTypes.h"

#ifdef FEATURE_USES_MMS
#define MG_MAX_FILE_NAME            128
/*
#define RELEASEIF(p) \
   if (p != NULL) { IBASE_Release((IBase*)(p)); (p) = NULL; }
*/   

#define DBGPRINTF_EX_FORMAT    "*dbgprintf-%d* %s:%d"
#define MMS_DEBUG(arg) __DBGPRINTF(DBGPRINTF_EX_FORMAT,5,__FILE__,__LINE__),__DBGPRINTF arg

#define WMS_MMS_PDU_FLAGS_EN(mainArg,otherArg) (mainArg | (otherArg << 16))
#define WMS_MMS_PDU_FLAGS_DE(flags,mainArg,otherArg) \
    { \
        mainArg = flags & 0x00FF; \
        otherArg = (flags >> 16); \
    }
    

static int MMS_WSP_Decode_UINTVAR(uint8* pData, int iDataLen, int* iUintvarLen);
static int MMS_WSP_GetExpiry(uint8* buf, int iDataLen, int curtime);
static int MMS_WSP_DecodeLongInteger(uint8* buf, int iDataLen, int* iValue);
static int MMS_WSP_GetValueLen(uint8* pData, int iDataLen, int* iDataOffset);
static int MMS_PDU_DecodeEncodedString(uint8* ptr, int datalen,	uint8 ePDUType,	uint8* handle);
static void MMI_SocketSend(void *pData);
static void SocketReadableCB(void* pSocketData);
static void SocketConnect_OnTimeout(void* pData);
static void ConnectError(void* pDdata, int nError);
static int MMS_Encode_header(uint8* mms_context,int nType,MMS_WSP_ENCODE_SEND* pData);
static int MMS_WSP_Encode_UINTVAR(uint8* buf, int val);
static int MMS_Encode_MsgBody(uint8* hPDU,WSP_MMS_ENCODE_DATA* pData);
static int MMS_EncodeSmilFile(uint8* encbuf,uint8 *smilFile, int len);
static int MMS_GetFileContent(uint8* encbuf,WSP_ENCODE_DATA_FRAGMENT frag);
static int MMS_WSP_DecodeContentTypeHeader(uint8* pData, int iDataLen, WSP_DEC_DATA_FRAGMENT* iMIMEHeaders, boolean* bIsMultipart,
								 int* charset, boolean add_boundary_param, int iHeadersLen, boolean in_encode_check_param, int *inout_depth);
static int MMS_WSP_DecodeContentTypeParams(uint8* pData, int iDataLen, WSP_DEC_DATA_FRAGMENT* iMIMEParams, int* charset);
static int MMS_WSP_Decode_MultipartData(uint8* pData, int iDataLen,int nParts, WSP_DEC_DATA_FRAGMENT* iMIMEParts,int *inout_depth);

static char* MMS_WSP_ContentTypeDB_MMS2Text(int ct);
static boolean is_us_ascii_string(char * in_s, int in_len);
static int MMS_WSP_ContentTypeDB_Text2MMS(uint8* ct, int ct_len, uint8* ctbyte);
static int slim_strncmp_nocase(char *in_s, char *in_t, int in_n);
static MMS_MESSAGE_TYPE MMS_GetMMSTypeByName(uint8 *hContentType);
const char *MMS_GetMimeType(const char *pszSrc);
static boolean MMS_STREQI(const char *s1, const char *s2);
static void MMSSocketState(MMSSocket *ps);
boolean  MMSSocketRecv (MMSSocket *ps, uint8 *pBuf, uint32 *pLen);
boolean  MMSSocketSend (MMSSocket *ps, const uint8 *pBuf, uint32 nLen);
boolean WMS_MMS_SaveMMS(char* phoneNumber,char *pBuffer,int DataLen,int nKind);
boolean WMS_MMS_DeleteMMS(uint32 index,int nKind);
uint8* WMS_MMS_PDUHeader_Encode(uint8* pBuf,int nKind,uint8* pValue,int32 nValue);
uint8* WMS_MMS_PDU_SendRequest(uint8* pBuff,MMS_WSP_ENCODE_SEND *pData);
uint8* WMS_MMS_PDU_NotifyResp(uint8* pBuff,MMS_WSP_ENCODE_SEND *pData);
uint8* WMS_MMS_PDU_ReadRecInd(uint8* pBuff,MMS_WSP_ENCODE_SEND *pData);
uint8* WMS_MMS_PDU_AcknowledgeInd(uint8* pBuff,MMS_WSP_ENCODE_SEND *pData);
uint8* WMS_MMS_BUFFERGet();
void WMS_MMS_BUFFERReset();
void WMS_MMS_BUFFERRelease();
char* STRTOPHONENUMBER(char* pDesStr,char* pScrStr);
uint8* WMS_MMS_ReadMMS(uint32 index,int nKind,uint32* pLen);
boolean WMS_MMS_Resend(int nIndex,int nKind);

void WMS_MMS_SetSocketState(boolean state);


#define SLIM_WSP_WELL_KNWON_VALULES_MIME_TEXT_PLAIN 0x03
#define SLIM_WSP_WELL_KNWON_VALULES_MIME_MULTIPART_MIXED 0x0c
#define SLIM_WSP_WELL_KNWON_VALULES_MIME_APPLICATION_VND_WAP_MULTIPART_MIXED 0x23
#define SLIM_WSP_WELL_KNWON_VALULES_MIME_APPLICATION_VND_WAP_MULTIPART_RELATED 0x33
#define SLIM_WSP_WELL_KNWON_VALULES_MIME_MULTIPART_ALTERNATIVE 0x0c
#define SLIM_WSP_WELL_KNWON_VALULES_MIME_APPLICATION_VND_WAP_MULTIPART_ALTERNATIVE 0x26

#define SLIM_CLIB_CTYPE_UPPER 0x00000001
#define SLIM_CLIB_CTYPE_LOWER 0x00000002
#define SLIM_CLIB_CTYPE_CNTRL_LOW 0x00000004
#define SLIM_CLIB_CTYPE_OCT 0x00000008
#define SLIM_CLIB_CTYPE_DIGIT 0x00000010
#define SLIM_CLIB_CTYPE_SPACE 0x00000020
#define SLIM_CLIB_CTYPE_TAB 0x00000040
#define SLIM_CLIB_CTYPE_HEX 0x00000080
#define SLIM_CLIB_CTYPE_PUNCT 0x00000100
#define SLIM_CLIB_CTYPE_ASCII_SP 0x00000200
#define SLIM_CLIB_CTYPE_EOL 0x00000400
#define SLIM_CLIB_CTYPE_NONASCII 0x00000800
#define SLIM_CLIB_CTYPE_HYPHEN 0x00001000
#define SLIM_CLIB_CTYPE_UNDER_SCORE 0x00002000
#define SLIM_CLIB_CTYPE_COMMA 0x00004000
#define SLIM_CLIB_CTYPE_PLUS 0x00008000
#define SLIM_CLIB_CTYPE_EQUAL 0x00010000
#define SLIM_CLIB_CTYPE_CSS_SEL_MOD 0x00020000
#define SLIM_CLIB_CTYPE_CSS_SEL_TERM 0x00040000
#define SLIM_CLIB_CTYPE_CSS_DECL_DELIM 0x00080000
#define SLIM_CLIB_CTYPE_CSS_PROPVAL_DELIM 0x00100000
#define SLIM_CLIB_CTYPE_CSS_ATTR_DELIM 0x00200000
#define SLIM_CLIB_CTYPE_FORM_NO_ENCODE_CHARS 0x00400000
#define SLIM_CLIB_CTYPE_RFC2396_MARK 0x00800000
#define SLIM_CLIB_CTYPE_RFC2396_RESERVED 0x01000000
#define SLIM_CLIB_CTYPE_RFC822_SPECIALS 0x02000000
#define SLIM_CLIB_CTYPE_XML_NAME_1ST_EXT 0x04000000
#define SLIM_CLIB_CTYPE_XML_NAME_EXT 0x08000000
#define SLIM_CLIB_CTYPE_XML_CLOSE_PAREN 0x10000000

#define slim_isctype(c,type) (cSlim_clib_ctype_table_mms[(c)] & (type))
#define slim_isnonascii(c) (slim_isctype((c),SLIM_CLIB_CTYPE_NONASCII))
#define SLIM_SS_FORWARD(s,len,n)	((s)+=(n),(len)-=(n))
#define SLIM_SS_INC(s,len)			((s)++,(len)--)
#define SLIM_SS_DEC(s,len)			((s)--,(len)++)
#define slim_tolower(c) ((int)cSlim_clib_tolower_table_mms[(c)])
#define slim_toupper(c) ((int)cSlim_clib_toupper_table_mms[(c)])
#if 0
#ifndef FEATURE_USES_MMS_TEST
//这是南美的短信中心http://mms.movilnet.com.ve
#define POST_TEST ("POST %s HTTP/1.1\r\nHost:http://mms.movilnet.com.ve\r\nAccept-Charset:utf-8\r\nContent-Length:%d\r\nAccept:*/*,application/vnd.wap.mms-message\r\nAccept-Language:en\r\nAccept-Encoding:gzip,deflate\r\nContent-Type:application/vnd.wap.mms-message\r\nUser-Agent: http://mms.movilnet.com.ve/phonemodel.xml\r\nx-wap-profile: \"http://mms.movilnet.com.ve/phonemodel.xml\"\r\nKeep-Alive:300\r\nConnection:Keep-Alive\r\n\r\n")
#else//这是中国电信的短信中心http://mmsc.vnet.mobi
#define POST_TEST ("POST %s HTTP/1.1\r\nHost:10.0.0.200:80\r\nAccept-Charset:utf-8\r\nContent-Length:%d\r\nAccept:*/*,application/vnd.wap.mms-message\r\nAccept-Language:en\r\nAccept-Encoding:gzip,deflate\r\nContent-Type:application/vnd.wap.mms-message\r\nUser-Agent: Nokia6235/1.0 (S190V0200.nep) UP.Browser/6.2.3.2 MMP/2.0\r\nx-wap-profile: \"http://nds1.nds.nokia.com/uaprof/N6235r200.xml\"\r\nKeep-Alive:300\r\nConnection:Keep-Alive\r\n\r\n")
#endif
#endif

#define POST_TEST ("POST %s HTTP/1.1\r\nHost:%s\r\nAccept-Charset:utf-8\r\nContent-Length:%d\r\nAccept:*/*,application/vnd.wap.mms-message\r\nAccept-Language:en\r\nAccept-Encoding:gzip,deflate\r\nContent-Type:application/vnd.wap.mms-message\r\nUser-Agent: http://mms.movilnet.com.ve/phonemodel.xml\r\nx-wap-profile: \"http://mms.movilnet.com.ve/phonemodel.xml\"\r\nKeep-Alive:300\r\nConnection:Keep-Alive\r\n\r\n")

boolean bSocketLock = FALSE;
boolean mms_space_enough = TRUE;
boolean bCloseSocket = FALSE;
IVector* pSocketParam= NULL;

void WMS_MMS_SetSocketState(boolean state)
{
    bCloseSocket = state;
}

boolean WMS_SOCKET_LOCK()
{
    if(bSocketLock)
        return FALSE;
        
    bSocketLock = TRUE;
}
boolean WMS_GETSOCKET_STATUS()
{
    return bSocketLock;
}
void WMS_SOCKET_UNLOK()
{
    bSocketLock = FALSE;
}

/*
** 彩信里面用到的数据类型
*/
// WSP - A  适用于WSP/WTP
const char* DB_Mms2Text[76] =
{
	"*/*",
	"text/*",
    "text/html",
    "text/plain",
    "text/x-hdml",
    "text/x-ttml",
    "text/x-vCalendar",
    "text/x-vCard",
    "text/vnd.wap.wml",
    "text/vnd.wap.wmlscript",
    "text/vnd.wap.wta-event",
    "multipart/*",
    "multipart/mixed",
    "multipart/form-data",
    "multipart/byterantes",
    "multipart/alternative",  //0x0f
    "application/*",
    "application/java-vm",
    "application/x-www-form-urlencoded",
    "application/x-hdmlc",
    "application/vnd.wap.wmlc",
    "application/vnd.wap.wmlscriptc",
    "application/vnd.wap.wta-eventc",
    "application/vnd.wap.uaprof",
    "application/vnd.wap.wtls-ca-certificate",
    "application/vnd.wap.wtls-user-certificate",
    "application/x-x509-ca-cert",
    "application/x-x509-user-cert",
    "image/*",
    "image/gif",  //29
    "image/jpeg",
    "image/tiff",   //0x1f
    "image/png",
    "image/vnd.wap.wbmp",
    "application/vnd.wap.multipart.*",
    "multipart/mixed",
/*    "application/vnd.wap.multipart.mixed",*/
    "application/vnd.wap.multipart.form-data",
    "application/vnd.wap.multipart.byteranges",
/*    "application/vnd.wap.multipart.alternative",*/
    "multipart/alternative",
    "application/xml",
    "text/xml",
    "application/vnd.wap.wbxml",
    "application/x-x968-cross-cert",
    "application/x-x968-ca-cert",
    "application/x-x968-user-cert",
    "text/vnd.wap.si",
    "application/vnd.wap.sic",
    "text/vnd.wap.sl",  //0x2f
    "application/vnd.wap.slc",
    "text/vnd.wap.co",
    "application/vnd.wap.coc",
    "multipart/related",
/*    "application/vnd.wap.multipart.related",*/
    "application/vnd.wap.sia",
    "text/vnd.wap.connectivity-xml",
    "application/vnd.wap.connectivity-wbxml",
    "application/pkcs7-mime",
    "application/vnd.wap.hashed-certificate",
    "application/vnd.wap.signed-certificate",
    "application/vnd.wap.cert-response",
    "application/xhtml+xml",
    "application/wml+xml",
    "text/css",
    "application/vnd.wap.mms-message",
    "application/vnd.wap.rollover-certificate",
    "application/vnd.wap.locc+wbxml",
    "application/vnd.wap.loc+xml",
    "application/vnd.syncml.dm+wbxml",
    "application/vnd.syncml.dm+xml",
    "application/vnd.syncml.notification",
    "application/vnd.wap.xhtml+xml",
    "application/vnd.wv.csp.cir",
    "application/vnd.oma.dd+xml",
    "application/vnd.oma.drm.message",
    "application/vnd.oma.drm.content",
    "application/vnd.oma.drm.rights+xml",
    "application/vnd.oma.drm.rights+wbxml",
};

// WSP - B
// 适用于HTTP 1.1
#if 0
const char* DB_Mms2Text[76] =
{
    "*/*",
    "text/*",// 0x01
    "text/html",// 0x02
    "text/plain",// 0x03
    "text/x-hdml",// 0x04
    "text/x-ttml",// 0x05
    "text/x-vCalendar",// 0x06
    "text/x-vCard",// 0x07
    "text/x-wap.wml",// 0x08
    "text/x-wap.wmlscript",// 0x09
    "text/x-wap.wta-event",// 0x0A
    "multipart/*"// 0x0B
    "multipart/mixed",// 0x0C
    "multipart/form-data",// 0x0D
    "multipart/byteranges",// 0x0E
    "multipart/alternative",// 0x0F
    "application/*",// 0x10
    "application/java-vm",// 0x11
    "application/x-www-form-urlencoded",// 0x12
    "application/x-hdmlc",// 0x13
    "application/x-wap.wmlc",// 0x14
    "application/x-wap.wmlscriptc",// 0x15
    "application/x-wap.wta-eventc",// 0x16
    "application/x-wap.uaprof",// 0x17
    "application/x-wap.wtls-ca-certificate",// 0x18
    "application/x-wap.wtls-user-certificate",// 0x19
    "application/x-x509-ca-cert",// 0x1A
    "application/x-x509-user-cert",// 0x1B
    "image/*",// 0x1C
    "image/gif",// 0x1D
    "image/jpeg",// 0x1E
    "image/tiff",// 0x1F
    "image/png",// 0x20
    "image/x-wap.wbmp",// 0x21
    "x-wap.multipart/*",// 0x22
    "x-wap.multipart/mixed",// 0x23
    "x-wap.multipart/form-data",// 0x24
    "x-wap.multipart/byteranges",// 0x25
    "x-wap.multipart/alternative",// 0x26
    //Unassigned 0x27-0x7F
}
#endif
typedef enum
{
    // TEXT
    TEXT_PLAIN,
    TEXT_HTML,
    TEXT_VCALENDAR,
    TEXT_VCARD,
    
    // IMAGE    
    IMAGE_JPEG,
    IMAGE_JPG,
    IMAGE_GIF,
    IMAGE_WBMP,
    IMAGE_PNG,

    // AUDIO
    AUDIO_AAC,
    AUDIO_AMR,
    AUDIO_IMELODY,
    AUDIO_MID,
    AUDIO_MIDI,
    AUDIO_MP3,
    AUDIO_MPEG3,
    AUDIO_MPEG,
    AUDIO_MPG,
    AUDIO_MP4,
    AUDIO_X_MID,
    AUDIO_X_MIDI,
    AUDIO_X_MP3,
    AUDIO_X_MPEG3,
    AUDIO_X_MPEG,
    AUDIO_X_MPG,
    AUDIO_3GPP,
    AUDIO_OGG,

    // VIDEO
    VIDEO_3GPP,
    VIDEO_3G2,
    VIDEO_H263,
    VIDEO_MP4,
    VIDEO_3GP,

    MIME_TYPE_COUT
} WSP_MIME_TYPE;

const static char* sWspMimeType[MIME_TYPE_COUT] =
{
    // TEXT
    "text/plain",
    "text/html",
    "text/x-vCalendar",
    "text/x-vCard",

    // IMAGE
    "image/jpeg",
    "image/jpg",
    "image/gif",
    "image/vnd.wap.wbmp",
    "image/png",

    // AUDIO
    "audio/aac",
    "audio/amr",
    "audio/imelody",
    "audio/mid",
    "audio/midi",
    "audio/mp3",
    "audio/mpeg3",
    "audio/mpeg",
    "audio/mpg",
    "audio/mp4",
    "audio/x-mid",
    "audio/x-midi",
    "audio/x-mp3",
    "audio/x-mpeg3",
    "audio/x-mpeg",
    "audio/x-mpg",
    "audio/3gpp",
    "application/ogg",

    // VIDEO
    "video/3gpp",
    "video/3gpp2",
    "video/h263",
    "video/mp4",   
    "video/3gp",
};

uint32 cSlim_clib_ctype_table_mms[256] = {
	0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 
	0x4, 0x64, 0x424, 0x24, 0x24, 0x424, 0x4, 0x4, 
	0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 
	0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 
	0x220, 0x900100, 0x2000100, 0x20100, 0x1000100, 0x100, 0x1000100, 0x800100, 
	0x2800100, 0x2800100, 0xc00100, 0x1008100, 0x3044100, 0x8c01100, 0xac20100, 0x1000100, 
	0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 
	0x90, 0x90, 0x7020100, 0x3180100, 0x2000100, 0x1210100, 0x12000100, 0x1000100, 
	0x3000100, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x1, 
	0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 
	0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 
	0x1, 0x1, 0x1, 0x2020100, 0x2000100, 0x2200100, 0x100, 0x4c02100, 
	0x100, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x2, 
	0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 
	0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 
	0x2, 0x2, 0x2, 0x40100, 0x200100, 0x180100, 0xa00100, 0x4, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
	0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 
};

uint8 cSlim_clib_toupper_table_mms[256] = {
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
	 16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
	 32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
	 48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
	 64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
	 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
	 96,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
	 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90, 123, 124, 125, 126, 127,
	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
	176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
};

uint8 cSlim_clib_tolower_table_mms[256] = {
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
	 16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
	 32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
	 48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
	 64,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,  91,  92,  93,  94,  95,
	 96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
	176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
};


//uint8 g_mms_buffer[300*1024];

//extern MMSData	g_mmsDataInfoList[MAX_MMS_STORED];
//extern MMSData	g_mmsDataInfoList[2];
extern uint8  g_mmsDataInfoMax;
//WSP_MMS_ENCODE_DATA mms_data = {0};

//uint8 buf[15*1024] = {0};
uint8* pBuf = NULL;// = MALLOC(150*1024);

static char* MMS_WSP_ContentTypeDB_MMS2Text(int ct)
{
	if ((ct >= 0) && (ct <= 0x4b))
	{
		return (char*)DB_Mms2Text[ct];
	}
	else
		return "application/octet-stream";
}

static int slim_strncmp_nocase(char *in_s, char *in_t, int in_n)
{
	int s;

	if (in_n <= 0)
		return 0;

	s = *in_s;
	while (in_n > 1 && slim_tolower(s) == slim_tolower(*in_t))
	{
		if (s != '\0')
		{
			in_s++;
			in_t++;
			in_n--;
			s = *in_s;
		}
		else
		{
			return 0;
		}
	}

	return (slim_tolower(s) - slim_tolower(*in_t));
}

static MMS_MESSAGE_TYPE MMS_GetMMSTypeByName(uint8 *hContentType)
{
    MMS_DEBUG(("[MMS_GetMMSTypeByName]: hContentType:%s",hContentType));
	if (slim_strncmp_nocase((char*)hContentType,"text",4) == 0)
	{
		return MMS_MESSAGE_TYPE_TEXT;
	}

	if (slim_strncmp_nocase((char*)hContentType,"image",5) == 0)
	{
		return MMS_MESSAGE_TYPE_IMAGE;
	}
	
	if ((slim_strncmp_nocase((char*)hContentType,"audio",5) == 0)
	    || (slim_strncmp_nocase((char*)hContentType,"snd",3) == 0))
	{
		return MMS_MESSAGE_TYPE_AUDIO;
	}

	if (slim_strncmp_nocase((char*)hContentType,"video",5) == 0)
	{
		return MMS_MESSAGE_TYPE_VIDEO;
	}

	return MMS_MESSAGE_TYPE_UNKNOW;
}

static int MMS_WSP_ContentTypeDB_Text2MMS(uint8* ct, int ct_len, uint8* ctbyte)
{
	int i,hv = MMS_WSP_HEADER_VALUE_UNKNOWN;
	
	for(i=0;i<76;i++)
	{
		if (slim_strncmp_nocase((char*)ct,(char*)DB_Mms2Text[i],ct_len) == 0)
		{
			hv = MMS_WSP_HEADER_VALUE_KNOWN;
			*ctbyte = i;
			break;
		}
	}
	
	if (hv == MMS_WSP_HEADER_VALUE_KNOWN)
	{
		switch(*ctbyte)
		{
		case 0x33:
			hv = MMS_WSP_HEADER_VALUE_MULTIPART_RELATED;
			break;
		case 0x0c:
		case 0x23:
			hv = MMS_WSP_HEADER_VALUE_MULTIPART_MIXED;
			break;
		}
	}
	
	return hv;
}

static boolean is_us_ascii_string(char * in_s, int in_len)
{
	int i;
	i = 0;
	
	while( i < in_len)
	{
		if(slim_isnonascii(*(in_s + i)))
		{
			return FALSE;
		}

		i++;
	}
	return TRUE;
}

static int MMS_WSP_Encode_UINTVAR(uint8* buf, int val)
{
	int i;

	for(i=0;i<5;i++)
	{
		buf[4-i] = (uint8)(val & 0x7f);
		val = val >> 7;
		if (i)
			buf[4-i] |= 0x80;
		if (!val)
		{
			return i+1;
		}
	}
	return i;
}

uint8* MMS_WTP_Encode_String(uint8* buf,char* pszStr)
{
    int strLen = 0;
    strLen = STRLEN(pszStr);
    
    if(strLen == 0x1f || strLen > 0x7F)
    {
        int nTempLen = strLen;
        int i = sizeof(int);
        uint8 val = 0;
        *buf = 0x1f;

        for(;i >= 0;i--)
        {
            val = nTempLen >> (7 * i);
            if(val)
            {
                buf++;
                *buf = val | 0x80;
                
            }
        }
        *buf &= 0x7F;
        buf++;

        STRNCPY((char*)buf,pszStr,strLen);
        buf += strLen;
        
    }
    else if(pszStr[0] < 0x1f)
    {
        *buf = strLen;
        buf++;
        STRNCPY((char*)buf,pszStr,strLen);
        buf += strLen;
    }
    else
    {
        STRNCPY((char*)buf,pszStr,strLen);
        buf += strLen;
    }
 
    return buf;
}
static void MMS_Encode_Address(uint8* pIn)
{
    uint8* pAddCur = pIn;
    
    boolean isNumber = TRUE;
    boolean isMail = FALSE;
    boolean isIpV4 = FALSE;
    boolean isIpV6 = FALSE;

    uint16 nCount = 0;
    uint16 nIndex = 0;

    if(pIn == NULL || STRLEN((char*)pIn) == 0)
        return ;
    DBGPRINTF("pAddCur=%s",pAddCur);    
    while(*pAddCur != '\0')
    {
        switch(*pAddCur)
        {
            case '.':
            {
                switch(nIndex)
                {
                    case 0:
                    {
                        if(nCount == 3)
                        {
                            isIpV4 = TRUE;
                        }
                        else
                        {
                            isIpV4 = FALSE;
                        }
                    }
                    break;
                    case 1:
                    case 2:
                    {
                        if(isIpV4 && nCount == 3)
                        {
                            isIpV4 = TRUE;
                           
                        }
                        else
                        {
                            isIpV4 = FALSE;
                        }
                    }
                    break;
                    default:
                    {
                       isIpV4 = FALSE;
                    }
                    break;
                }
                nIndex++;
                nCount = 0;
                pAddCur++;
                continue;
            }
            break;
            case ':':
            {
                switch(nIndex)
                {
                    case 0:
                    {
                        if(nCount == 4)
                        {
                            isIpV6 = TRUE;
                        }
                        else
                        {
                            isIpV6 = FALSE;
                        }
                    }
                    break;
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    {
                        if(isIpV6 && nCount == 4)
                        {
                            isIpV6 = TRUE; 
                        }
                        else
                        {
                            isIpV6 = FALSE;
                        }
                    }
                    break;
                    default:
                    {
                       isIpV6 = FALSE;
                    }
                    break;
                }
                nIndex++;
                nCount = 0;
                pAddCur++;
                continue;
            }
            break;
            case '@':
            {
                isMail = TRUE;
            }
            break;
            case '/':
            {
                if((STRLEN((char*)pIn) > (pAddCur  - pIn + 4))
                    && *(pAddCur + 1) == 'T'
                    && *(pAddCur + 2) == 'Y'
                    && *(pAddCur + 3) == 'P'
                    && *(pAddCur + 4) == 'E')
                {
                    return ;
                }
            }
            break;
            default:
            {                
                isNumber = (isNumber && (*pAddCur >= '0' && *pAddCur <= '9'))? TRUE : FALSE;
            }
            break;
        }
        
        nCount++;
        pAddCur++;        
    }

    
    if(isIpV6)
    {
        STRCAT((char*)pIn,"/TYPE=IPv6");
    }
    else if(isIpV4)
    {
        STRCAT((char*)pIn,"/TYPE=IPv4");
    }
    else if(isMail)
    {
        ;
    }
    if(isNumber)
    {
        STRCAT((char*)pIn,"/TYPE=PLMN");
    }
    else
    {
        ;
    }
}
static int MMS_Encode_header(uint8* mms_context,int nType,MMS_WSP_ENCODE_SEND* pData)
{
    uint8* pCurPos = (uint8*)mms_context;	

    if(pData == NULL)
        return 0;
    DBGPRINTF("MMS_Encode_header Start nType=%d",nType);
    switch(nType)
	{
	    case WMS_MMS_PDU_MSendReq:
	    {
	        //  以下必填
	        //  X-Mms-Message-Type
	        //  X-Mms-Transaction-ID
	        //  X-Mms-MMS-Version
	        //  From
	        //  To,Cc,Bcc 三选一
	        //  Content-Type
	        
	        JulianType juDateTime	= {0};
	        char szDataTime[30] = {0};

            if(pData->pMessage == NULL)
                return 0;

	        //X-Mms-Transaction-ID
	        {	
	        	/*					
				uint32  dwSecs; 				
				IConfig *pConfig = NULL;	
				byte	btTimeStamp = 0;
			    
				if (ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_CONFIG,(void **)&pConfig) != SUCCESS)
			    {
			    	MSG_FATAL("***zzg MMS_Encode_header Create AEECLSID_CONFIG faidled***", 0, 0, 0);
			        RELEASEIF(pConfig);
			        return 0;
			    }

				(void) ICONFIG_GetItem(pConfig,
			                           CFGI_SMS_TIMESTAMP,
			                           &btTimeStamp,
			                           sizeof(btTimeStamp));

				if (btTimeStamp == OEMNV_SMS_TIMESTAMP_ADJUST)
				{
					dwSecs = GETUTCSECONDS();
					MSG_FATAL("***zzg GETUTCSECONDS 9 dwSecs=%d***", dwSecs, 0, 0);
				}
				else
				{
					dwSecs = GETTIMESECONDS();
					MSG_FATAL("***zzg GETTIMESECONDS 9 dwSecs=%d***", dwSecs, 0, 0);
				}   	            

				RELEASEIF(pConfig);
				*/

				GetJulianDate(GETTIMESECONDS(), &juDateTime);		
				//GetJulianDate(dwSecs, &juDateTime);	
			}        	
			
        	SNPRINTF(szDataTime,
        	    sizeof(szDataTime) - 1,
        	    "%2.2d:%2.2d %2.2d/%2.2d/%4.4d",
        	    juDateTime.wHour,
        	    juDateTime.wMinute,
        	    juDateTime.wDay,
        	    juDateTime.wMonth,
        	    juDateTime.wYear);  
        	STRNCPY((char*)pData->pMessage->hTransactionID,szDataTime,STRLEN(szDataTime) + 1);
            MMS_DEBUG(("[MMS_Encode_header] id:%s",(char*)pData->pMessage->hTransactionID));
            
            //  X-Mms-MMS-Version
            pData->pMessage->iMMSVersion = 10;
            DBGPRINTF("pData->pMessage->hTo=%s",pData->pMessage->hTo);
            DBGPRINTF("pData->pMessage->hCc=%s",pData->pMessage->hCc);
            DBGPRINTF("pData->pMessage->hBcc=%s",pData->pMessage->hBcc);
            DBGPRINTF("pData->pMessage->hFrom=%s",pData->pMessage->hFrom);
            //如果是群发到手机时，需要在每一个hTo后面加"/TYPE=PLMN",邮箱则不用。再在WMS_MMS_PDU_SendRequest里循环调用
            //pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_To,pData->pMessage->hTo,0);则可实现邮件群发
        	MMS_Encode_Address(pData->pMessage->hTo);
        	MMS_Encode_Address(pData->pMessage->hCc);
        	MMS_Encode_Address(pData->pMessage->hBcc);
        	MMS_Encode_Address(pData->pMessage->hFrom);

	        pCurPos = WMS_MMS_PDU_SendRequest(pCurPos,pData);
	    }
	    break;
	    case WMS_MMS_PDU_MNotifyrespInd:
	    {
	        if(pData->pNotifyresp == NULL)
                return 0;
            //  X-Mms-MMS-Version
            pData->pNotifyresp->iMMSVersion = 10;
	        pCurPos = WMS_MMS_PDU_NotifyResp(pCurPos,pData);
	    }
	    break;
	    case WMS_MMS_PDU_MAcknowledgeInd:
	    {
	        if(pData->pDeliveryacknowledgement == NULL)
                return 0;

            //  X-Mms-MMS-Version
            pData->pDeliveryacknowledgement->iMMSVersion = 10;
            
	        pCurPos = WMS_MMS_PDU_AcknowledgeInd(pCurPos,pData);
	    }
	    break;
	    case WMS_MMS_PDU_MReadRecInd:
	    {
	        if(pData->pReadReport == NULL)
                return 0;

            //  X-Mms-MMS-Version
            pData->pReadReport->iMMSVersion = 10;

            pData->pReadReport->bReadStutas = TRUE;

            pData->pReadReport->iMessageClass = MMS_CLASS_AUTO;
            DBGPRINTF("pData->pReadReport->hTo%s",pData->pReadReport->hTo);
            MMS_Encode_Address(pData->pReadReport->hTo);
            
	        pCurPos = WMS_MMS_PDU_ReadRecInd(pCurPos,pData);
	    }
	}
	
    MMS_DEBUG(("[MMS_Encode_header] EXIT"));
	return (int)(pCurPos - mms_context);
}
#if 0
static int MMS_Encode_header(uint8* mms_context,uint8 *phonenum, uint8 *subject)
{
    JulianType juDateTime	= {0};
	char szDataTime[30] = {0};
    int addresslen = 0;
    int len = 0;
    int uintvar_len = 0;
    char *addresstype = "/TYPE=PLMN";
    uint8 uintvar[5];
    
    uint8* pCurPos = (uint8*)mms_context;	

	MMS_DEBUG(("[MMS]: MMS_Encode_header!!!!"));

	//X-Mms-Message-Type(0x8c): m-send-req(0x80)
	*pCurPos = 0x8c; pCurPos++;
	*pCurPos = 0x80; pCurPos++;

	//ID
	GetJulianDate(GETTIMESECONDS(), &juDateTime);
	SNPRINTF(szDataTime, sizeof(szDataTime) - 1, "%2.2d:%2.2d %2.2d/%2.2d/%4.4d", juDateTime.wHour, juDateTime.wMinute, juDateTime.wDay, juDateTime.wMonth, juDateTime.wYear);
	*pCurPos = 0x98; pCurPos++;
	STRNCPY((char*)pCurPos,szDataTime,STRLEN(szDataTime));
	pCurPos += STRLEN(szDataTime);
	*pCurPos = 0x00; pCurPos++;
	
	//X-Mms-MMS-Version(0x8d):v1.0(0x90)
	*pCurPos = 0x8d; pCurPos++;
	*pCurPos = 0x90; pCurPos++;	
	
	//from
	*pCurPos = 0x89; pCurPos++;
	*pCurPos = 0x01; pCurPos++;
	*pCurPos = 0x81; pCurPos++;                         

	/*
	** eg: 13332941841 + addresstype
	*/
	//To(0x97)
	addresslen = STRLEN((char*)phonenum) + STRLEN(addresstype);
	*pCurPos = 0x97; pCurPos++;
	STRNCPY((char*)pCurPos,(char*)phonenum,STRLEN((char*)phonenum));
	pCurPos += STRLEN((char*)phonenum);
	STRNCPY((char*)pCurPos,addresstype,STRLEN(addresstype));
	pCurPos += STRLEN(addresstype);
	*pCurPos = 0x00; pCurPos++;

#if 1
	//subject
	len = STRLEN((char*)subject);
	if ( len > 0)
	{
		*pCurPos = 0x96; pCurPos++;/*Subject*/
		if (len+1+1 > 30)
		{
			*pCurPos = 0x1F; pCurPos++;/*Charset - UTF-8*//*Charset - UTF-8*/
			uintvar_len = MMS_WSP_Encode_UINTVAR(uintvar,len+1+1);/* +1 = charset value , +1 - null termination*/
			MEMCPY((char*)pCurPos,(char*)&uintvar[5-uintvar_len],uintvar_len);
			pCurPos += uintvar_len;
		}
		else
		{
			uint8 l;
			l = len+1+1;
			*pCurPos = l; pCurPos++;
		}

		//*pCurPos = 0x0b; pCurPos++;/*Charset - UTF-8*/
		*pCurPos = 0xea; pCurPos++;
		MEMCPY((char*)pCurPos,(char*)subject,STRLEN((char*)subject));
		pCurPos += STRLEN((char*)subject);
		*pCurPos = 0x00; pCurPos++;
	}
#endif
	//后面为数据部分
	return (int)(pCurPos - (uint8*)mms_context);
}
#endif
static int MMS_Encode_MsgBody(uint8* hPDU,WSP_MMS_ENCODE_DATA* pData)
{
	uint8* pCurPos = hPDU;
    uint8 smil_buf[2000] = {0};
    uint32 head_len = 0;
    int len = 0;
    int i = 0;
	MMS_DEBUG(("[MMS]: MMS_Encode_MsgBody!!!!"));
	//Msg body
	// -------------- files -------------- //
	//file count, smil file is one
	*pCurPos = pData->frag_num + 1; pCurPos++;	//part number

    len = WMS_MMS_CreateSMIL(smil_buf,2000,*pData);	
	head_len = MMS_EncodeSmilFile(pCurPos,smil_buf,len);
	MMS_DEBUG(("[MMS] MMS_EncodeSmilFile head_len2 = %d",head_len));	
	pCurPos += head_len;

	for(i=0; i< WMSMMS_FRAGMENTCOUNT; i++)
	{
		head_len = MMS_GetFileContent(pCurPos,pData->fragment[i]);
		pCurPos += head_len;
	}
	return (int)(pCurPos-hPDU);
}

static int MMS_GetFileContent(uint8* encbuf,WSP_ENCODE_DATA_FRAGMENT frag)
{
	uint32 content_size = 0;
	IFile* pIFile = NULL;
    IFileMgr *pIFileMgr = NULL;
    FileInfo pInfo = {0};
    uint8* pCurPos = encbuf;
    int uintvar_len;
	int result = SUCCESS;
	int value_len, mediatype_value_len;
	int hn,hv,ct, param_len;
	uint8 *value = NULL, *mediatype_value = NULL, onebyte = 0, ctbyte = 0;
	uint8 uintvar[5];
	uint8 param_buf[256];
	uint8 subhead_buf[512];
	uint8 *temp_pos;
	int i = 0;
    MMS_MESSAGE_TYPE type;

        
        
    type = MMS_GetMMSTypeByName(frag.hContentType);

    if ( type != MMS_MESSAGE_TYPE_TEXT )
    {
        if(STRLEN((char*)frag.hContentFile) == 0)
        {
            MMS_DEBUG(("[MMS]: MMS_GetFileContent No File Path"));
            return 0;
        }
        result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
    	if (SUCCESS != result)
        {
    		MSG_FATAL("MRS: Open file error %x", result,0,0);
    		RELEASEIF(pIFileMgr);
    		return 0;
        }

        if (IFILEMGR_GetInfo(pIFileMgr,(char*)frag.hContentFile, &pInfo) == SUCCESS)
        {
        	content_size = (int)pInfo.dwSize;

        	MSG_FATAL("MRS: content_size:%d content_len: %d", content_size,0,0);
        }
        else
        {
            RELEASEIF(pIFileMgr);
            MSG_FATAL("MRS: Get content info failed", 0,0,0);
    		return 0;
        }
    }

    hn = MMS_WSP_HEADER_CONTENT_TYPE;

	value = frag.hContentType;
	value_len = STRLEN((char*)frag.hContentType);
	hv = MMS_WSP_ContentTypeDB_Text2MMS(value, value_len, &ctbyte);
	ct = hv;

	if (hv == MMS_WSP_HEADER_VALUE_UNKNOWN)
	{
		mediatype_value = value;
		mediatype_value_len = value_len;
	}

	//param encode
	temp_pos = param_buf;
	switch(ct)
	{
		case MMS_WSP_HEADER_VALUE_UNKNOWN:
		{
			STRNCPY((char*)temp_pos,(char*)mediatype_value,mediatype_value_len);
			temp_pos += mediatype_value_len;
			*temp_pos = 0x00; temp_pos++;
			
			param_len = STRLEN((char*)frag.hContentName);
			if ( param_len > 0 )
			{
				//name
				*temp_pos = 0x85; temp_pos++;
				STRNCPY((char*)temp_pos,(char*)frag.hContentName,param_len);
				temp_pos += param_len;
				*temp_pos = 0x00; temp_pos++;
			}

			param_len = (int)(temp_pos-param_buf);
			break;
		}
		

		case MMS_WSP_HEADER_VALUE_KNOWN:
		{
			onebyte = ctbyte+0x80;
			*temp_pos = onebyte; temp_pos++;

			//text/plain
			if ( onebyte == 0x83)
			{
				*temp_pos = 0x81; temp_pos++;
				*temp_pos = 0xea; temp_pos++;  //utf-8
			}
			
			param_len = STRLEN((char*)frag.hContentName);
			if ( param_len > 0 )
			{
				//name
				*temp_pos = 0x85; temp_pos++;
				STRNCPY((char*)temp_pos,(char*)frag.hContentName,param_len);
				temp_pos += param_len;
				*temp_pos = 0x00; temp_pos++;
			}

			param_len = (int)(temp_pos-param_buf);
			break;
		}
	}


	//sub head encode
	temp_pos = subhead_buf;

	*temp_pos = 0x1f; temp_pos++;
	uintvar_len = MMS_WSP_Encode_UINTVAR(uintvar,param_len);
	STRNCPY((char*)temp_pos,(char*)&uintvar[5-uintvar_len],uintvar_len);
	temp_pos += uintvar_len;
    MEMCPY((char*)temp_pos,(char*)param_buf,param_len);
	temp_pos += param_len;
    
	param_len = STRLEN((char*)frag.hContentID);
    MMS_DEBUG(("hContentID = %s",(char*)frag.hContentID));
	if ( param_len > 0 )
	{
		//content id
		*temp_pos = 0xc0; temp_pos++;
		*temp_pos = 0x22; temp_pos++;
		*temp_pos = 0x3c; temp_pos++;
		STRNCPY((char*)temp_pos,(char*)frag.hContentID,param_len);
		temp_pos += param_len;
		*temp_pos = 0x3e; temp_pos++;
		*temp_pos = 0x00; temp_pos++;
	}

	param_len = STRLEN((char*)frag.hContentLocation);
    MMS_DEBUG(("hContentLocation = %s",(char*)frag.hContentLocation));
	if ( param_len > 0 )
	{
		//hContentLocation
		*temp_pos = 0x8e; temp_pos++;
		STRNCPY((char*)temp_pos,(char*)frag.hContentLocation,param_len);
		temp_pos += param_len;
		*temp_pos = 0x00; temp_pos++;
	}
	param_len = (int)(temp_pos - subhead_buf);

	pCurPos = encbuf;
	//head length
	uintvar_len = MMS_WSP_Encode_UINTVAR(uintvar,param_len);
	STRNCPY((char*)pCurPos,(char*)&uintvar[5-uintvar_len],uintvar_len);
	pCurPos += uintvar_len;

	//text/plain
	if ( onebyte == 0x83)
	{
	    content_size = STRLEN((char*)frag.hContentText);
	}
	//content size
	uintvar_len = MMS_WSP_Encode_UINTVAR(uintvar,content_size);
	STRNCPY((char*)pCurPos,(char*)&uintvar[5-uintvar_len],uintvar_len);
	pCurPos += uintvar_len;

    for(i=0; i<param_len; i=i+3 )
    {
        MSG_FATAL("0x%x 0x%x 0x%x",subhead_buf[i],subhead_buf[i+1],subhead_buf[i+2]);
    }
    
	MEMCPY((char*)pCurPos,(char*)subhead_buf,param_len);
	pCurPos += param_len;

	//text/plain
	if ( onebyte == 0x83)
	{
		MEMCPY((char*)pCurPos,(char*)frag.hContentText,content_size);
		MSG_FATAL("[MMS_GetFileContent] Get Text Content:%d",content_size,0,0);
	}
	else
	{
        if ( type != MMS_MESSAGE_TYPE_TEXT )
        {
            if(frag.pBuf != NULL && frag.nBufLen > 0)
            {
                MEMCPY(pCurPos,frag.pBuf,frag.nBufLen);
                content_size = frag.nBufLen;
            }
            else
            {
        		pIFile = IFILEMGR_OpenFile( pIFileMgr, (char*)frag.hContentFile, _OFM_READ);
        		if ( pIFile != NULL )
        	    {
        	        uint32 nContentLen = 0;
        	        IFILE_Seek(pIFile, _SEEK_START, 0);
        	        nContentLen = IFILE_Read( pIFile, (char*)pCurPos, content_size);

        	        MSG_FATAL("IFILEMGR_OpenFile content_size:%d,nContentLen:%d",content_size,nContentLen,0);
        	    }
        	    RELEASEIF(pIFile);
    	    }
    	    
        }
	}
	pCurPos += content_size;
	
	RELEASEIF(pIFileMgr);
    return (uint32)(pCurPos-encbuf);
}

//一个SMIL文件的例子
//("<smil><head><layout><root-layout width=\"220px\" height=\"96px\" background-color=\"#FFFFFF\" /><region id=\"Image\" width=\"128px\" height=\"96px\" top=\"0px\" left=\"0px\" fit=\"meet\" /><region id=\"Text\" width=\"220px\" height=\"0px\" top=\"96px\" left=\"0px\" fit=\"meet\" /></layout></head><body><par dur=\"5000ms\" ><img src=\"1.jpg\" region=\"Image\" /><text src=\"text.txt\" region=\"Text\"/></par></body></smil>")
//根据附件生成布局文件
int WMS_MMS_CreateSMIL(uint8 *out_buf,int buf_size,WSP_MMS_ENCODE_DATA data)
{
	char *cur_pos = (char*)out_buf;
	int len;
	int i=0;
	MMS_MESSAGE_TYPE type;

	MMS_DEBUG(("[MMS]: WMS_MMS_CreateSMIL!!!!"));
	if ( out_buf == NULL )
	{
		return -1;
	}
#if 1
	len = STRLEN("<smil><head><layout><root-layout width=\"220px\" height=\"96px\" background-color=\"#FFFFFF\" />");
	STRNCPY(cur_pos,"<smil><head><layout><root-layout width=\"220px\" height=\"96px\" background-color=\"#FFFFFF\" />",len);
	cur_pos += len;

    if(STRLEN((char*)data.fragment[i].hContentText) != 0)
    {
        len = STRLEN("<region id=\"Text\" width=\"220px\" height=\"0px\" top=\"96px\" left=\"0px\" fit=\"meet\" /></layout></head>");
    	STRNCPY(cur_pos,"<region id=\"Text\" width=\"220px\" height=\"0px\" top=\"96px\" left=\"0px\" fit=\"meet\" /></layout></head>",len);
    	cur_pos += len;
    }
    
	for(i = 1; i < WMSMMS_FRAGMENTCOUNT; i++)
	{
	    if(STRLEN((char*)data.fragment[i].hContentFile) == 0)
	        continue;
	        
    	if ( !STRCMP(data.fragment[i].pType,IMAGE_MIME_BASE) )
    	{
    		len = STRLEN("<region id=\"Image\" width=\"120px\" height=\"77px\" top=\"0px\" left=\"0px\" fit=\"meet\" />");
    		STRNCPY(cur_pos,"<region id=\"Image\" width=\"120px\" height=\"77px\" top=\"0px\" left=\"0px\" fit=\"meet\" />",len);
    		cur_pos += len;
    	}

    	if ( !STRCMP(data.fragment[i].pType,VIDEO_MIME_BASE) )
    	{
    		len = STRLEN("<region id=\"video\" width=\"128px\" height=\"96px\" top=\"0px\" left=\"0px\"/>");
    		STRNCPY(cur_pos,"<region id=\"video\" width=\"128px\" height=\"96px\" top=\"0px\" left=\"0px\"/>",len);
    		cur_pos += len;
    	}
    }
	

	len = STRLEN("<body><par>");
	STRNCPY(cur_pos,"<body><par>",len);
	cur_pos += len;

	if(STRLEN((char*)data.fragment[0].hContentText) != 0)
	{
	    len = STRLEN("<text src=\"");
		STRNCPY(cur_pos,"<text src=\"",len);
		cur_pos += len;

		len = STRLEN((char*)data.fragment[0].hContentName);
		STRNCPY(cur_pos,(char*)data.fragment[0].hContentName,len);
		cur_pos += len;

		len = STRLEN("\" region=\"Text\"/>");
		STRNCPY(cur_pos,"\" region=\"Text\"/>",len);
		cur_pos += len;
	}
	
	for(i=1; i< WMSMMS_FRAGMENTCOUNT; i++)
	{
	    if(STRLEN((char*)data.fragment[i].hContentFile) == 0)
	        continue;
	        
		type = MMS_GetMMSTypeByName(data.fragment[i].hContentType);

		if ( type == MMS_MESSAGE_TYPE_IMAGE)
		{
			len = STRLEN("<img src=\"");
			STRNCPY(cur_pos,"<img src=\"",len);
			cur_pos += len;

			len = STRLEN((char*)data.fragment[i].hContentName);
			STRNCPY(cur_pos,(char*)data.fragment[i].hContentName,len);
			cur_pos += len;

			len = STRLEN("\" region=\"Image\" dur=\"5000ms\" />");
			STRNCPY(cur_pos,"\" region=\"Image\" dur=\"5000ms\" />",len);
			cur_pos += len;
		}
		else if ( type == MMS_MESSAGE_TYPE_AUDIO )
		{
			len = STRLEN("<audio src=\"");
			STRNCPY(cur_pos,"<audio src=\"",len);
			cur_pos += len;

			len = STRLEN((char*)data.fragment[i].hContentName);
			STRNCPY(cur_pos,(char*)data.fragment[i].hContentName,len);
			cur_pos += len;

			len = STRLEN("\" dur=\"5000ms\" />");
			STRNCPY(cur_pos,"\" dur=\"5000ms\" />",len);
			cur_pos += len;
		}
		else if ( type == MMS_MESSAGE_TYPE_VIDEO)
		{
			len = STRLEN("<video src=\"");
			STRNCPY(cur_pos,"<video src=\"",len);
			cur_pos += len;

			len = STRLEN((char*)data.fragment[i].hContentName);
			STRNCPY(cur_pos,(char*)data.fragment[i].hContentName,len);
			cur_pos += len;

			len = STRLEN("\" region=\"video\" dur=\"5000ms\" />");
			STRNCPY(cur_pos,"\" region=\"video\" dur=\"5000ms\" />",len);
			cur_pos += len;
		}
		else if ( type == MMS_MESSAGE_TYPE_TEXT)
		{
			len = STRLEN("<text src=\"");
			STRNCPY(cur_pos,"<text src=\"",len);
			cur_pos += len;

			len = STRLEN((char*)data.fragment[i].hContentName);
			STRNCPY(cur_pos,(char*)data.fragment[i].hContentName,len);
			cur_pos += len;

			len = STRLEN("\" region=\"Text\"/>");
			STRNCPY(cur_pos,"\" region=\"Text\"/>",len);
			cur_pos += len;
		}
	}

	len = STRLEN("</par></body></smil>");
	STRNCPY(cur_pos,"</par></body></smil>",len);
#endif
   // len = STRLEN("<smil><head><layout><root-layout width=\"220px\" height=\"96px\" background-color=\"#FFFFFF\" /><region id=\"Image\" width=\"128px\" height=\"96px\" top=\"0px\" left=\"0px\" fit=\"meet\" /><region id=\"Text\" width=\"220px\" height=\"0px\" top=\"96px\" left=\"0px\" fit=\"meet\" /></layout></head><body><par dur=\"5000ms\" ><img src=\"1.jpg\" region=\"Image\" /><text src=\"text.txt\" region=\"Text\"/></par></body></smil>");
    //STRNCPY(cur_pos, "<smil><head><layout><root-layout width=\"220px\" height=\"96px\" background-color=\"#FFFFFF\" /><region id=\"Image\" width=\"128px\" height=\"96px\" top=\"0px\" left=\"0px\" fit=\"meet\" /><region id=\"Text\" width=\"220px\" height=\"0px\" top=\"96px\" left=\"0px\" fit=\"meet\" /></layout></head><body><par dur=\"5000ms\" ><img src=\"1.jpg\" region=\"Image\" /><text src=\"text.txt\" region=\"Text\"/></par></body></smil>",len);
    cur_pos += len;

	return (int)(cur_pos-(char*)out_buf);
}

static int MMS_EncodeSmilFile(uint8* encbuf,uint8 *smilFile, int len)
{
	int content_size = 0;
    uint8* pCurPos = encbuf;
    uint8 uintvar[5];
    int uintvar_len;

	MMS_DEBUG(("[MMS]: MMS_EncodeSmilFile!!!!"));
	
	content_size = len;
	
	*pCurPos = 0x35; pCurPos++;
	uintvar_len = MMS_WSP_Encode_UINTVAR(uintvar,content_size);
	//content length
	STRNCPY((char*)pCurPos,(char*)&uintvar[5-uintvar_len],uintvar_len);
	pCurPos += uintvar_len;

	*pCurPos = 0x1e; pCurPos++;

	STRNCPY((char*)pCurPos,"application/smil",16);
	pCurPos += 16;
	*pCurPos = 0x00; pCurPos++;

	//name
	*pCurPos = 0x85; pCurPos++; 
	STRNCPY((char*)pCurPos,"ArSmil.smil",11);
	pCurPos += 11;
	*pCurPos = 0x00; pCurPos++;
	
	//add Content-ID
	*pCurPos = 0xc0; pCurPos++;
	*pCurPos = 0x22; pCurPos++;
	*pCurPos = 0x3c; pCurPos++;
	STRNCPY((char*)pCurPos,"AAAA",4);
	pCurPos += 4;
	*pCurPos = 0x3e; pCurPos++;
	*pCurPos = 0x00; pCurPos++;

	//add Content-Location
	*pCurPos = 0x8e; pCurPos++;
	STRNCPY((char*)pCurPos,"ArSmil.smil",11);
	pCurPos += 11;
	*pCurPos = 0x00; pCurPos++;

	STRNCPY((char*)pCurPos,(char*)smilFile,len);
	pCurPos += len;
	
    return (int)(pCurPos-encbuf);
}

typedef enum
{
    WMS_MMS_PDU_Bcc = 1,
    WMS_MMS_PDU_Cc,
    WMS_MMS_PDU_ContentLocation,
    WMS_MMS_PDU_ContentType,
    WMS_MMS_PDU_Date,

    // 6
    WMS_MMS_PDU_DeliveryReport,
    WMS_MMS_PDU_DeliveryTime,
    WMS_MMS_PDU_Expiry,
    WMS_MMS_PDU_From,
    WMS_MMS_PDU_MessageClass,

    // b
    WMS_MMS_PDU_MessageID,
    WMS_MMS_PDU_MessageType,
    WMS_MMS_PDU_MMSVersion,
    WMS_MMS_PDU_MessageSize,
    WMS_MMS_PDU_Priority,

    // 10
    WMS_MMS_PDU_ReadReply,
    WMS_MMS_PDU_ReportAllowed,
    WMS_MMS_PDU_ResponseStatus,
    WMS_MMS_PDU_ResponseText,
    WMS_MMS_PDU_SenderVisibility,

    // 15
    WMS_MMS_PDU_Status,
    WMS_MMS_PDU_Subject,
    WMS_MMS_PDU_To,
    WMS_MMS_PDU_TransactionId,

    WMS_MMS_PDU_ReadStatus = 0x1d,
} WMS_MMS_PDU_AssignedNumbers;

typedef enum
{
    WMS_MMS_PDU_AbsToken,
    WMS_MMS_PDU_RelToken
}WMS_MMS_PDU_TIME_FIELD;

int WMS_MMS_SEND_PDU(WMS_MMS_PDU_MessageTypeValue type,uint8* hPDU,MMS_WSP_ENCODE_SEND* pData)
{

}

uint8* WMS_MMS_PDU_SendRequest(uint8* pBuff,MMS_WSP_ENCODE_SEND *pData)
{
    uint8* pCurPos = pBuff;
    MSG_FATAL("WMS_MMS_PDU_SendRequest Start",0,0,0);
    //X-Mms-Message-Type
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_MessageType,NULL,WMS_MMS_PDU_MSendReq);

    // X-Mms-Transaction-ID
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_TransactionId,pData->pMessage->hTransactionID,0);
    
    // X-Mms-MMS-Version 
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_MMSVersion,NULL,0x10);
   
    // Date 
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_Date,NULL,pData->pMessage->iDate);
    
    // From 
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_FLAGS_EN(WMS_MMS_PDU_From,INSERT_ADDRESS),pData->pMessage->hFrom,0);
    {
        uint8 TempTo[MAX_EMAILADD_LEN+1];
        int len = STRLEN((char*)pData->pMessage->hTo);
        uint8 *to = pData->pMessage->hTo;
        int i, j;
        DBGPRINTF("to1=%s",to);
        if(STRSTR((char*)pData->pMessage->hTo, "PLMN") != NULL )
        {
            to += 4;//跳过前面的PLMN
            i += 4;            
            //处理发送到手机的情况
            DBGPRINTF("to2=%s",to);
            for(i=0, j=0; i < len; ++i)
            {
                DBGPRINTF("to=%s",to);
                if(*to == ',')
                {
                    TempTo[j] = '\0';
                    DBGPRINTF("len=%d, i=%d, j=%d, TempTo=%s",len, i, j, TempTo);
                    STRCAT((char*)TempTo,"/TYPE=PLMN");
                    //TempTo[j] = '\0';
                    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_To,TempTo,0);
                    DBGPRINTF("WMS_MMS_PDU_SendRequest TempTo=%s", TempTo);
                    MEMSET(TempTo, '\0', sizeof(TempTo));
                    j = 0;
                    to++;
                    continue;
                }
                TempTo[j++] = *to++;
                // To 
               // pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_To,pData->pMessage->hTo,0);
            }            
        }
        else
        {
            for(i=0, j=0; i < len; ++i)
            {
                if(*to == ',')
                {
                    DBGPRINTF("len=%d, i=%d, j=%d, TempTo=%s",len, i, j, TempTo);
                    TempTo[j] = '\0';
                    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_To,TempTo,0);
                    DBGPRINTF("WMS_MMS_PDU_SendRequest TempTo=%s", TempTo);
                    MEMSET(TempTo, '\0', sizeof(TempTo));
                    j = 0;
                    to++;
                    continue;
                }
                TempTo[j++] = *to++;
                // To 
               // pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_To,pData->pMessage->hTo,0);
            }
        }
    }
    // Cc  
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_Cc,pData->pMessage->hCc,0);

    // Bcc  
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_Bcc,pData->pMessage->hBcc,0);

    // Subject  
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_Subject,pData->pMessage->hSubject,0);
    
    // X-Mms -Message-Class  
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_MessageClass,NULL,pData->pMessage->iMessageClass);

    // X-Mms -Expiry  
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_FLAGS_EN(WMS_MMS_PDU_Expiry,WMS_MMS_PDU_RelToken),NULL,pData->pMessage->iExpiry);//MMS_INT_MAX
    
    // X-Mms -Delivery-Time  
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_FLAGS_EN(WMS_MMS_PDU_DeliveryTime,WMS_MMS_PDU_RelToken),NULL,pData->pMessage->iDeliveryTime);//0
    
    // X-Mms -Priority  
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_Priority,NULL,pData->pMessage->iPriority);
    
    // X-Mms -Sender-Visibility  
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_SenderVisibility,NULL,pData->pMessage->bSenderVisibility);
     
    // X-Mms -Delivery-Report  
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_DeliveryReport,NULL,pData->pMessage->bDelRep);
    
    // X-Mms -Read-Reply  
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_ReadReply,NULL,pData->pMessage->bReadRep);
    
    // Content-Type  
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_ContentType,pData->pMessage->hContentType,0);

    return pCurPos;
    
}

uint8* WMS_MMS_PDU_NotifyResp(uint8* pBuff,MMS_WSP_ENCODE_SEND *pData)
{
    uint8* pCurPos = pBuff;
        
    //X-Mms-Message-Type
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_MessageType,NULL,WMS_MMS_PDU_MNotifyrespInd);

    // X-Mms-Transaction-ID
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_TransactionId,pData->pNotifyresp->hTransactionID,0);
    
    // X-Mms-MMS-Version 
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_MMSVersion,NULL,pData->pNotifyresp->iMMSVersion);

    // X-Mms-Response-Status
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_ResponseStatus,NULL,pData->pNotifyresp->iStatus);

    // X-Mms-Report-Allowed
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_ReportAllowed,NULL,pData->pNotifyresp->bReportAllowed);

    return pCurPos;

}

uint8* WMS_MMS_PDU_AcknowledgeInd(uint8* pBuff,MMS_WSP_ENCODE_SEND *pData)
{
    uint8* pCurPos = pBuff;
        
    //X-Mms-Message-Type
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_MessageType,NULL,WMS_MMS_PDU_MAcknowledgeInd);

    // X-Mms-Transaction-ID
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_TransactionId,pData->pDeliveryacknowledgement->hTransactionID,0);
    
    // X-Mms-MMS-Version 
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_MMSVersion,NULL,pData->pDeliveryacknowledgement->iMMSVersion);

    // X-Mms-Report-Allowed
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_ReportAllowed,NULL,pData->pDeliveryacknowledgement->bReportAllowed);

    return pCurPos;
}
uint8* WMS_MMS_PDU_ReadRecInd(uint8* pBuff,MMS_WSP_ENCODE_SEND *pData)
{
    uint8* pCurPos = pBuff;
        
    //X-Mms-Message-Type
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_MessageType,NULL,WMS_MMS_PDU_MReadRecInd);

    // X-Mms-Transaction-ID
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_TransactionId,pData->pReadReport->hTransactionID,0);
    
    // X-Mms-MMS-Version 
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_MMSVersion,NULL,pData->pReadReport->iMMSVersion);

    // X-Mms-Message-Id 
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_MessageID,pData->pReadReport->hMessageID,0);
    
    // X-Mms-Report-Allowed
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_FLAGS_EN(WMS_MMS_PDU_From,INSERT_ADDRESS),pData->pReadReport->hFrom,0);
    
    // X-Mms-Report-Allowed
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_To,pData->pReadReport->hTo,0);
    
    // X-Mms-Report-Allowed
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_ReadStatus,NULL,pData->pReadReport->bReadStutas);

    // X-Mms-Report-Allowed
    pCurPos = WMS_MMS_PDUHeader_Encode(pCurPos,WMS_MMS_PDU_MessageClass,NULL,pData->pReadReport->iMessageClass);

    return pCurPos;
}

uint8* WMS_MMS_PDUHeader_Encode(uint8* pBuf,int nKind,uint8* pValue,int32 nValue)
{
    int nMainFlag;
    int nOtherFlag;
    int nValueLen = 0;

    MMS_DEBUG(("[WMS_MMS_PDUHeader_Encode] ENTER"));
    WMS_MMS_PDU_FLAGS_DE(nKind,nMainFlag,nOtherFlag);

    MMS_DEBUG(("[WMS_MMS_PDUHeader_Encode]: nMainFlag:%d",nMainFlag));
    
    if(( WMS_MMS_PDU_ContentType != nMainFlag)
        && (nValue == -1 || (pValue != NULL && STRLEN((char*)pValue) == 0)))
    {
        MMS_DEBUG(("[WMS_MMS_PDUHeader_Encode] NOT PACKAGE"));
        return pBuf;
    }    
    
    switch(nMainFlag)
    {
        // string
        case WMS_MMS_PDU_Bcc:
        case WMS_MMS_PDU_Cc:
        case WMS_MMS_PDU_MessageID:
        case WMS_MMS_PDU_ResponseText:
        case WMS_MMS_PDU_Subject:
        case WMS_MMS_PDU_To:
        case WMS_MMS_PDU_TransactionId:
        case WMS_MMS_PDU_ContentLocation:
        {
            if(pValue == NULL)
                return pBuf;
                
            *pBuf = (nMainFlag | 0x80);

            pBuf = MMS_WTP_Encode_String(++pBuf,(char*)pValue);
        }
        break;

        // long int
        case WMS_MMS_PDU_Date:
        case WMS_MMS_PDU_MessageSize:
        {
            int i = 0;
            *pBuf = (nMainFlag | 0x80);
            
            pBuf++;
            *pBuf = 0x04;
            
            if(nValue > 0x7FFFFF)
                nValue = 0x7FFFFFFF;
                
            pBuf[1] = nValue >> 24;
            pBuf[2] = nValue >> 16;
            pBuf[3] = nValue >> 8;
            pBuf[4] = nValue;
            pBuf += 4;
        }
        break;

        // special : long int | long int
        case WMS_MMS_PDU_DeliveryTime:
        case WMS_MMS_PDU_Expiry:
        {
            *pBuf = (nMainFlag | 0x80);

            pBuf++;
            *pBuf = 0x05;
            
            pBuf++;
            *pBuf = (nOtherFlag | 0x80);
       
            if(nValue > 0x7FFFFF)
                nValue = 0x7FFFFFFF;
                
            pBuf[1] = nValue >> 24;
            pBuf[2] = nValue >> 16;
            pBuf[3] = nValue >> 8;
            pBuf[4] = nValue;
            pBuf += 4;   
            
        }
        break;
        // special : long int | string
        case WMS_MMS_PDU_From:
        {
            if(nOtherFlag == ADDRESS_PRESENT)
            {
                if(pValue == NULL)
                    return pBuf;
                    
                *pBuf = (nMainFlag | 0x80);
                nValueLen = STRLEN((char*)pValue) + 1;
                
                pBuf ++;
                *pBuf = (nOtherFlag | 0x80);
                
                pBuf ++;
                STRNCPY((char*)pBuf,(char*)pValue,++nValueLen);
                pBuf += nValueLen;
            }
            else
            {
                *pBuf = (nMainFlag | 0x80);
                
                pBuf ++; 
                *pBuf = 0x01;

                pBuf ++;
                *pBuf = (nValue | 0x80);
            }
        }
        break;

        // short int
        case WMS_MMS_PDU_MMSVersion:
        {
            *pBuf = (nMainFlag | 0x80);
            pBuf ++;
            *pBuf = (nValue | 0x80);
        }
        break;
       
        // enum
        case WMS_MMS_PDU_MessageClass:
        case WMS_MMS_PDU_MessageType:
        case WMS_MMS_PDU_Priority:
        case WMS_MMS_PDU_ResponseStatus:
        case WMS_MMS_PDU_SenderVisibility:
        case WMS_MMS_PDU_Status:
        {
            *pBuf = (nMainFlag | 0x80);
            
            pBuf ++;
            *pBuf = (nValue | 0x80);
        }
        break;

        // Yes Or No
        case WMS_MMS_PDU_ReadStatus:
        case WMS_MMS_PDU_DeliveryReport: 
        case WMS_MMS_PDU_ReadReply:
        case WMS_MMS_PDU_ReportAllowed:
        {
            *pBuf = (nMainFlag | 0x80);
            
            pBuf ++;
            if(nValue)
            {
                *pBuf = 0x80;
            }
            else
            {
                *pBuf = 0x81;
            }
            
        }
        break;
        
        // special
        case WMS_MMS_PDU_ContentType:
        {
            *pBuf = (nMainFlag | 0x80);

            *pBuf = 0x84; pBuf++;
        	*pBuf = 0x1c; pBuf++;  //head length
        	*pBuf = 0xb3; pBuf++;  //application/vnd.wap.multipart.related类型，对应于附录A里面的0x33+0x80；

        	*pBuf = 0x89; pBuf++;  //Type参数，见WSP Table38
        	//application/smil
        	*pBuf = 0x61; pBuf++;
        	*pBuf = 0x70; pBuf++;
        	*pBuf = 0x70; pBuf++;
        	*pBuf = 0x6c; pBuf++;
        	*pBuf = 0x69; pBuf++;
        	*pBuf = 0x63; pBuf++;
        	*pBuf = 0x61; pBuf++;
        	*pBuf = 0x74; pBuf++;
        	*pBuf = 0x69; pBuf++;
        	*pBuf = 0x6F; pBuf++;
        	*pBuf = 0x6E; pBuf++;
        	*pBuf = 0x2F; pBuf++;
        	*pBuf = 0x73; pBuf++;
        	*pBuf = 0x6D; pBuf++;
        	*pBuf = 0x69; pBuf++;
        	*pBuf = 0x6C; pBuf++;
        	*pBuf = 0x00; pBuf++;

        	*pBuf = 0x8A; pBuf++;  //Start参数,见WSP协议Table 38. Well-Known Parameter Assignments,编码值加0x80
        	//<start>
        	*pBuf = 0x3C; pBuf++;
        	*pBuf = 0x73; pBuf++;
        	*pBuf = 0x74; pBuf++;
        	*pBuf = 0x61; pBuf++;
        	*pBuf = 0x72; pBuf++;
        	*pBuf = 0x74; pBuf++;
        	*pBuf = 0x3E; pBuf++;
        	*pBuf = 0x00;
        }
        break;
    }
    MMS_DEBUG(("[WMS_MMS_PDUHeader_Encode] EXIT"));
    return ++pBuf;
}

uint8* WMS_MMS_BUFFERGet()
{
    if(pBuf == NULL)
        pBuf = (uint8*)sys_malloc(MSG_MAX_PACKET_SIZE);
        
    return pBuf;
}

void WMS_MMS_BUFFERReset()
{
    MEMSET(WMS_MMS_BUFFERGet(),NULL,MSG_MAX_PACKET_SIZE);
}

void WMS_MMS_BUFFERRelease()
{
    if(pBuf != NULL)
    {
        sys_free(pBuf);
        pBuf = NULL;
    }
}

void WMS_MMS_MmsWspDecDataRelease(MMS_WSP_DEC_DATA** ppdata,Mms_pdu_types nDataType)
{
    MMS_WSP_DEC_DATA* pdata = *ppdata;
    if(nDataType == MMS_PDU_RETRIEVE_CONF)
    {
        int nFragNumCur = 0;
        
        while(nFragNumCur < pdata->message.mms_data.frag_num)
        {
            FREEIF(pdata->message.mms_data.fragment[nFragNumCur].pContent);
            nFragNumCur++;
        };
        FREEIF(pdata->message.hBody);        
    }
    //FREEIF(pdata);
}

boolean WMS_MMS_SaveMMS(char* phoneNumber,char *pBuffer,int DataLen,int nKind)
{
    IConfig *pConfig = NULL;
    IFile* pIFile = NULL;
    IFileMgr *pIFileMgr = NULL;
    
    int result = 0;
    int nMmsDataInfoType = 0;
    int nMmsCoutType = 0;
    char psz[15] = {0};
    char mmsDataFileName[AEE_MAX_FILE_NAME];
    MMSData	mmsDataInfoList[MAX_MMS_STORED];
    char sz[2] =   { '/', 0 };
    int nCharBegin = 'A';
    JulianType julian;
    
    MSG_FATAL("[WMS_MMS_SaveMMS] g_mmsDataInfoMax=%d, nKind=%d",g_mmsDataInfoMax,nKind,0);
    
    switch(nKind)
    {
        case MMS_OUTBOX:
        {
            nMmsDataInfoType = CFGI_MMSOUTDATA_INFO;
            nMmsCoutType = CFGI_MMS_OUTCOUNT;
            //nCharBegin = 'a';
        }
        break;
        
        case MMS_INBOX:
        {
            nMmsDataInfoType = CFGI_MMSINDATA_INFO;
            nMmsCoutType = CFGI_MMS_INCOUNT;
            //nCharBegin = 'A';
        }
        break;

        case MMS_DRAFTBOX:
        {
            nMmsDataInfoType = CFGI_MMSDRAFTDATA_INFO;
            nMmsCoutType = CFGI_MMS_DRAFTCOUNT;
            //nCharBegin = 'A';
        }
        break;   

        default:
            break;
    }
    if (result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_CONFIG,(void **)&pConfig) != SUCCESS)
    {
        goto Exit;
    }

    ICONFIG_GetItem(pConfig, 
        nMmsCoutType,
        &g_mmsDataInfoMax,
        sizeof(g_mmsDataInfoMax));  
    MSG_FATAL("[WMS_MMS_SaveMMS] 1 g_mmsDataInfoMax=%d",g_mmsDataInfoMax,0,0);
    ICONFIG_GetItem(pConfig,
                   nMmsDataInfoType,
                   (void*)mmsDataInfoList,
                   sizeof(mmsDataInfoList));  
    {//要把这个移到一进入到短信的地方就提示。
	    IFileMgr *pIFileMgr = NULL;
	    int result = 0;
        uint32 pdwTotal = 0;
        uint32 pdwFree = 0;
		result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
		if (SUCCESS != result)
	    {
			MSG_FATAL("WMS_MMS_SaveMMS: Open file error %x", result,0,0);
			return FALSE;
	    }            
		pdwFree = IFILEMGR_GetFreeSpace(pIFileMgr, &pdwTotal); 
        MSG_FATAL("WMS_MMS_SaveMMS pdwFree=%d, pdwTotal=%d",pdwFree, pdwTotal, 0);
        if((pdwFree < MSG_MAX_PACKET_SIZE))
        {
            WMS_MMS_DeleteMMSALL(MMS_DRAFTBOX);
        }
        pdwFree = IFILEMGR_GetFreeSpace(pIFileMgr, &pdwTotal); 
        if((pdwFree < MSG_MAX_PACKET_SIZE))
        {
            uint8 Max = 0;
            uint8 index = 0;
            ICONFIG_GetItem(pConfig, 
                MMS_OUTBOX,
                &Max,
                sizeof(Max)); 
            while((pdwFree < MSG_MAX_PACKET_SIZE) && (index++ < Max))
            {
                WMS_MMS_DeleteMMS(0,MMS_OUTBOX);
                pdwFree = IFILEMGR_GetFreeSpace(pIFileMgr, &pdwTotal); 
            }
            Max = 0; 
            index = 0;
            if((pdwFree < MSG_MAX_PACKET_SIZE))
            {
                ICONFIG_GetItem(pConfig, 
                    MMS_INBOX,
                    &Max,
                    sizeof(Max)); 
                while((pdwFree < MSG_MAX_PACKET_SIZE) && (index++ < Max))
                {
                    uint8 m = Max-1;
                    MMSData *pMmsDataInfoListTemp = NULL;
                    for(; m >= 0; --m)
                    {
                         pMmsDataInfoListTemp = &mmsDataInfoList[index];
                         if(pMmsDataInfoListTemp->MMSDataReaded)
                         {
                            //当有未读短信和已读短信时，要先删除已读短信
                             WMS_MMS_DeleteMMS(m,MMS_INBOX);
                             break;
                         }
                    }
                    if(m < 0)//全部为未读短信
                    {
                        WMS_MMS_DeleteMMS(0,MMS_INBOX);
                    }
                    pdwFree = IFILEMGR_GetFreeSpace(pIFileMgr, &pdwTotal); 
                }          
            }
        }
        MSG_FATAL("WMS_MMS_SaveMMS pdwFree=%d, pdwTotal=%d",pdwFree, pdwTotal, 0);
        IFILEMGR_Release(pIFileMgr);    
    }    
    if(g_mmsDataInfoMax >= MAX_MMS_STORED)
    {
        if(nKind == MMS_INBOX)
        {
            int index = g_mmsDataInfoMax-1;
            MMSData	*pMmsDataInfoListTemp = NULL;
            for(; index >= 0; --index)
            {
                 pMmsDataInfoListTemp = &mmsDataInfoList[index];
                 if(pMmsDataInfoListTemp->MMSDataReaded)
                 {
                    //当有未读短信和已读短信时，要先删除已读短信
                     WMS_MMS_DeleteMMS(index,nKind);
                     break;
                 }
            }
            if(index < 0)//全部为未读短信
            {
                WMS_MMS_DeleteMMS(0,nKind);
            }
            MSG_FATAL("[WMS_MMS_SaveMMS] 33333333 index=%d",index,0,0);
        }
        else
        {
            WMS_MMS_DeleteMMS(0,nKind);
        }
    }

    ICONFIG_GetItem(pConfig, 
        nMmsCoutType,
        &g_mmsDataInfoMax,
        sizeof(g_mmsDataInfoMax));  
    MSG_FATAL("[WMS_MMS_SaveMMS] 2 g_mmsDataInfoMax=%d",g_mmsDataInfoMax,0,0);    
    result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
    if (SUCCESS != result)
    {
    	MSG_FATAL("[WMS_MMS_SaveMMS] Open file error %x", result,0,0);
    	goto Exit;
    }
         
    ICONFIG_GetItem(pConfig,
                   nMmsDataInfoType,
                   (void*)mmsDataInfoList,
                   sizeof(mmsDataInfoList));

// Emulator filename and path
    MEMSET((void*)mmsDataFileName, 0, AEE_MAX_FILE_NAME);
    //psz[1] = g_mmsDataInfoMax+nCharBegin;
    if(nKind == MMS_OUTBOX)
    {
        psz[0]='O';
    }
    else if(nKind == MMS_INBOX)
    {
        psz[0]='I';
    }
    else if(nKind == MMS_DRAFTBOX)
    {
        psz[0]='D';
    }    
        
    GETJULIANDATE(GETTIMESECONDS(), &julian);
    
    SPRINTF(psz+STRLEN(psz), "%02d%02d%02d%02d", julian.wDay, julian.wHour, julian.wMinute, julian.wSecond);
    STRCPY(mmsDataFileName, "fs:/hsmm/mmsDataFile");

    if(SUCCESS != IFILEMGR_Test(pIFileMgr, mmsDataFileName))
    {
        MSG_FATAL("[WMS_MMS_SaveMMS] File not exist:%s",(char*)&mmsDataFileName,0,0);
        (void)IFILEMGR_MkDir(pIFileMgr, mmsDataFileName);
    }

// Load file name and remove exist file
    STRCAT(mmsDataFileName,sz);  
    STRCAT(mmsDataFileName, psz);
    DBGPRINTF("[WMS_MMS_SaveMMS] mmsDataFileName:%s",mmsDataFileName);  
    if(SUCCESS == IFILEMGR_Test(pIFileMgr, mmsDataFileName))
    {
        result = IFILEMGR_Remove(pIFileMgr, mmsDataFileName);
        MSG_FATAL("[WMS_MMS_SaveMMS] File exist:%s", (char*)&mmsDataFileName,0,0);
    }

// Write buffer into file
    pIFile = IFILEMGR_OpenFile(pIFileMgr, mmsDataFileName, _OFM_CREATE);

    if(NULL != pIFile)
    {
        uint8 nbit =  pBuffer[1];
        pBuffer[1] = 0x84;//为了将来进“已发送彩信箱”里时方便调用WMS_MMS_PDU_Decode来解码
        result = IFILE_Write(pIFile, (void*)pBuffer, DataLen);
        RELEASEIF(pIFile);
        RELEASEIF(pIFileMgr);
        pBuffer[1] = nbit;
        if(pBuffer == NULL)
        {
            MSG_FATAL("[WMS_MMS_SaveMMS] IFILE_Write pBuffer == NULL",0,0,0);            
        }
        MSG_FATAL("[WMS_MMS_SaveMMS] IFILE_Write result=%d, DataLen=%d",result,DataLen,0);
    }   

//  Save mms info
    DBGPRINTF("[WMS_MMS_SaveMMS] PhoneNumber:%s",phoneNumber);  
    if(STRISTR(phoneNumber, "PLMN") != NULL)
    {
        char temp[32];
        STRCPY(temp, phoneNumber+4);
        DBGPRINTF("temp=%s", temp);
        STRCPY(phoneNumber, temp);
    }
    if(STRCMP(phoneNumber,"Delivery") != 0)
    {
        STRTOPHONENUMBER(mmsDataInfoList[g_mmsDataInfoMax].phoneNumber,phoneNumber);
    }
    else
    {
        STRCPY(mmsDataInfoList[g_mmsDataInfoMax].phoneNumber, phoneNumber);
    }
    DBGPRINTF("[WMS_MMS_SaveMMS] PhoneNumber:%s",phoneNumber);  
	//STRCPY(mmsDataInfoList[g_mmsDataInfoMax].phoneNumber, phoneNumber);
    DBGPRINTF("[WMS_MMS_SaveMMS] PhoneNumber:0x%x:0x%x:0x%x",mmsDataFileName[0],mmsDataFileName[1],mmsDataFileName[2]);  
    STRCPY(mmsDataInfoList[g_mmsDataInfoMax].MMSDataFileName, mmsDataFileName);
    mmsDataInfoList[g_mmsDataInfoMax].MMSDatasize = DataLen;
    mmsDataInfoList[g_mmsDataInfoMax].MMSDataReaded = FALSE;
    
    DBGPRINTF("[WMS_MMS_SaveMMS] PhoneNumber=%s, length=%d",
        mmsDataInfoList[g_mmsDataInfoMax].phoneNumber,
        STRLEN(mmsDataInfoList[g_mmsDataInfoMax].phoneNumber));      
    DBGPRINTF("[WMS_MMS_SaveMMS] MMSDataFileName=%s",
        mmsDataInfoList[g_mmsDataInfoMax].MMSDataFileName);

    g_mmsDataInfoMax++;
    MSG_FATAL("[WMS_MMS_SaveMMS] 3 g_mmsDataInfoMax=%d",g_mmsDataInfoMax,0,0);
	ICONFIG_SetItem(pConfig, nMmsDataInfoType, (void*)&mmsDataInfoList, sizeof(mmsDataInfoList));        
    ICONFIG_SetItem(pConfig, nMmsCoutType, &g_mmsDataInfoMax, sizeof(g_mmsDataInfoMax));  

Exit:
    RELEASEIF(pIFile);
    RELEASEIF(pIFileMgr);
    RELEASEIF(pConfig);
    
    return (result == SUCCESS);
    
}

boolean WMS_MMS_DeleteMMSALL(int nKind)
{
    int i = 0;
    uint8 mmsCount = 0;
    IConfig *pConfig = NULL;
    int nMmsCoutType = 0;
    int nMmsDataInfoType = 0;
    int result = SUCCESS;
    IFileMgr *pIFileMgr = NULL;
    MMSData	mmsDataInfoList[MAX_MMS_STORED];
    MSG_FATAL("WMS_MMS_DeleteMMSALL Start nKind=%d",nKind,0,0);
    if (ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_CONFIG,(void **)&pConfig) != SUCCESS)
    {
        RELEASEIF(pConfig);
        return FALSE;
    }
    result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
    if (SUCCESS != result)
    {
    	MSG_FATAL("[WMS_MMS_DeleteMMS] Open file error %x", result,0,0);
    	return FALSE;
    }

    switch(nKind)
    {
        case MMS_OUTBOX:
        {
            nMmsCoutType = CFGI_MMS_OUTCOUNT;
            nMmsDataInfoType = CFGI_MMSOUTDATA_INFO;
        }
        break;

        case MMS_DRAFTBOX:
        {
            nMmsCoutType = CFGI_MMS_DRAFTCOUNT;
            nMmsDataInfoType = CFGI_MMSDRAFTDATA_INFO;
        }
        break;
        
        default:
        case MMS_INBOX:
        {
            nMmsCoutType = CFGI_MMS_INCOUNT;
            nMmsDataInfoType = CFGI_MMSINDATA_INFO;
        }
        break;
    }
    
    ICONFIG_GetItem(pConfig, 
        nMmsCoutType,
        &mmsCount,
        sizeof(mmsCount)); 

    ICONFIG_GetItem(pConfig,
       nMmsDataInfoType,
       (void*)mmsDataInfoList,
       sizeof(mmsDataInfoList));
    
    MSG_FATAL("g_mmsDataInfoMax=%d",mmsCount,0,0);   
    for(i = 0; i < mmsCount;i++)
    {
        if(SUCCESS == IFILEMGR_Test(pIFileMgr, mmsDataInfoList[i].MMSDataFileName))
        {
            if(SUCCESS != IFILEMGR_Remove(pIFileMgr,mmsDataInfoList[i].MMSDataFileName))   
            {
                result = IFILEMGR_GetLastError(pIFileMgr);
                MSG_FATAL("[WMS_MMS_DeleteMMSALL] find remove error 0x%x", result,0,0);               
            }
            DBGPRINTF("%d FileName=%s", i,mmsDataInfoList[i].MMSDataFileName);
        }
    }
    mmsCount = 0;
    MEMSET((void*)mmsDataInfoList,NULL,sizeof(MMSData));
    ICONFIG_SetItem(pConfig, nMmsDataInfoType, (void*)mmsDataInfoList, sizeof(mmsDataInfoList));        
    ICONFIG_SetItem(pConfig, nMmsCoutType, &mmsCount, sizeof(mmsCount));     
    RELEASEIF(pConfig);
    RELEASEIF(pIFileMgr);
    return TRUE;
}

boolean WMS_MMS_DeleteMMS(uint32 index,int nKind)
{
    IConfig *pConfig = NULL;
    IFileMgr *pIFileMgr = NULL;

    int i = 0;
    int result;
    MMSData	mmsDataInfoList[MAX_MMS_STORED];
    MMSData	*pMmsDataInfoListCur = NULL;
    MMSData	*pMmsDataInfoListNext = NULL;
    int nMmsDataInfoType = 0;
    int nMmsCoutType = 0;
    boolean isRemoveSuccess = FALSE;
    MSG_FATAL("WMS_MMS_DeleteMMS, Start, index=%d, nKind=%d",index,nKind,0);
    if (result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_CONFIG,(void **)&pConfig) != SUCCESS)
    {
        goto Exit;
    }

    result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
    if (SUCCESS != result)
    {
    	MSG_FATAL("[WMS_MMS_DeleteMMS] Open file error %x", result,0,0);
    	goto Exit;
    }
    MSG_FATAL("WMS_MMS_DeleteMMS, 1",0,0,0);
    switch(nKind)
    {
        case MMS_OUTBOX:
        {
            nMmsDataInfoType = CFGI_MMSOUTDATA_INFO;
            nMmsCoutType = CFGI_MMS_OUTCOUNT;
        }
        break;

        case MMS_DRAFTBOX:
        {
            nMmsDataInfoType = CFGI_MMSDRAFTDATA_INFO;
            nMmsCoutType = CFGI_MMS_DRAFTCOUNT;
        }
        break;
        
        default:
        case MMS_INBOX:
        {
            nMmsDataInfoType = CFGI_MMSINDATA_INFO;
            nMmsCoutType = CFGI_MMS_INCOUNT;
        }
        break;
    }
    MSG_FATAL("WMS_MMS_DeleteMMS, 2",0,0,0);
    ICONFIG_GetItem(pConfig, 
        nMmsCoutType,
        &g_mmsDataInfoMax,
        sizeof(g_mmsDataInfoMax));  
    MSG_FATAL("g_mmsDataInfoMax=%d",g_mmsDataInfoMax,0,0);
    ICONFIG_GetItem(pConfig,
       nMmsDataInfoType,
       (void*)mmsDataInfoList,
       sizeof(mmsDataInfoList));
                   
    if(g_mmsDataInfoMax <= 0)
    {
        result = EFAILED;
        goto Exit;
    }   
    MSG_FATAL("[WMS_MMS_DeleteMMS] g_mmsDataInfoMax:%d, index=%d", g_mmsDataInfoMax,index,0);
    
    pMmsDataInfoListCur = &mmsDataInfoList[index];
    if(pMmsDataInfoListCur != NULL)
    {
        DBGPRINTF("MMSDataFileName = %s", pMmsDataInfoListCur->MMSDataFileName);
    }
    if(SUCCESS == IFILEMGR_Test(pIFileMgr,
        pMmsDataInfoListCur->MMSDataFileName))
    {
        MSG_FATAL("[WMS_MMS_DeleteMMS] find file", 0,0,0);
        if(SUCCESS == IFILEMGR_Remove(pIFileMgr,
            pMmsDataInfoListCur->MMSDataFileName))
        {
            //isRemoveSuccess = TRUE;
            MSG_FATAL("[WMS_MMS_DeleteMMS] find remove success", 0,0,0);
        }
        else
        {
            result = IFILEMGR_GetLastError(pIFileMgr);
            MSG_FATAL("[WMS_MMS_DeleteMMS] find remove error 0x%x", result,0,0);
            //goto Exit;
        }
    }
    else
    {
        result = IFILEMGR_GetLastError(pIFileMgr);
        MSG_FATAL("[WMS_MMS_DeleteMMS] find file error 0x%x", result,0,0);
        //goto Exit;
    }
    
    isRemoveSuccess = TRUE;//不管删除成功与否，都要把第index个文件从DataInfoList列表中给移除
    for(i = index; (i < (g_mmsDataInfoMax - 1)) && (i < MAX_MMS_STORED-1); i++)
    {
        MSG_FATAL("[WMS_MMS_DeleteMMS] rename file index:%d", i,0,0);
        pMmsDataInfoListCur = &mmsDataInfoList[i];
        pMmsDataInfoListNext = &mmsDataInfoList[i + 1];
        if((pMmsDataInfoListCur != NULL) && (pMmsDataInfoListNext != NULL))
        {
            if(EFAILED == IFILEMGR_Test(pIFileMgr,pMmsDataInfoListCur->MMSDataFileName)
                && SUCCESS == IFILEMGR_Test(pIFileMgr,pMmsDataInfoListNext->MMSDataFileName))
            {
                DBGPRINTF("Cur FileName=%s",pMmsDataInfoListCur->MMSDataFileName);
                DBGPRINTF("Next FileName=%s",pMmsDataInfoListNext->MMSDataFileName);
                if(SUCCESS == IFILEMGR_Rename(pIFileMgr,pMmsDataInfoListNext->MMSDataFileName,pMmsDataInfoListCur->MMSDataFileName))
                {
                    STRCPY(pMmsDataInfoListCur->phoneNumber,pMmsDataInfoListNext->phoneNumber);
                    pMmsDataInfoListCur->MMSDatasize = pMmsDataInfoListNext->MMSDatasize;
                    pMmsDataInfoListCur->MMSDataReaded = pMmsDataInfoListNext->MMSDataReaded;
                    DBGPRINTF("[WMS_MMS_DeleteMMS] rename file phone number:%s", pMmsDataInfoListCur->phoneNumber);
                    MSG_FATAL("[WMS_MMS_DeleteMMS] rename file phone number len:%d", pMmsDataInfoListCur->MMSDatasize,0,0);
                    DBGPRINTF("[WMS_MMS_DeleteMMS] rename file phone name:%s", pMmsDataInfoListCur->MMSDataFileName);
                }
                else
                {
                    result = IFILEMGR_GetLastError(pIFileMgr);
                    MSG_FATAL("[WMS_MMS_DeleteMMS] rename file error:0x%x", result,0,0);
                    //goto Exit;
                    continue;
                }
            }
            else
            {
                result = IFILEMGR_GetLastError(pIFileMgr);
                MSG_FATAL("[WMS_MMS_DeleteMMS] rename file phone name:%s", pMmsDataInfoListCur->MMSDataFileName,0,0);
                MSG_FATAL("[WMS_MMS_DeleteMMS] rename file phone name:%s", pMmsDataInfoListNext->MMSDataFileName,0,0);
                MSG_FATAL("[WMS_MMS_DeleteMMS] Test file error:0x%x", result,0,0);
                //goto Exit;
                continue;
            }
        }
    };
    if(pMmsDataInfoListNext != NULL)
    {
        MEMSET((void*)pMmsDataInfoListNext,NULL,sizeof(MMSData));
    }

    if(isRemoveSuccess)
        g_mmsDataInfoMax--;
        
    ICONFIG_SetItem(pConfig, nMmsDataInfoType, (void*)mmsDataInfoList, sizeof(mmsDataInfoList));        
    ICONFIG_SetItem(pConfig, nMmsCoutType, &g_mmsDataInfoMax, sizeof(g_mmsDataInfoMax)); 
    
    
Exit:
    MSG_FATAL("WMS_MMS_DeleteMMS Exit",0,0,0);
    RELEASEIF(pIFileMgr);
    RELEASEIF(pConfig);
    
    return (result == SUCCESS);
}
uint8* WMS_MMS_ReadMMS(uint32 index,int nKind,uint32* pLen)
{
    IConfig *pConfig = NULL;
    IFileMgr *pIFileMgr = NULL;

    int i = 0;
    int result;
    MMSData	mmsDataInfoList[MAX_MMS_STORED];
    MMSData	*pMmsDataInfoListCur = NULL;
    MMSData	*pMmsDataInfoListNext = NULL;
    int nMmsDataInfoType = 0;
    int nMmsCoutType = 0;

    uint8* pBuf = WMS_MMS_BUFFERGet();
    
    WMS_MMS_BUFFERReset();
    
    if(index >= g_mmsDataInfoMax)
        return FALSE;
        
    if (result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_CONFIG,(void **)&pConfig) != SUCCESS)
    {
        goto Exit;
    }

    result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
    if (SUCCESS != result)
    {
    	MSG_FATAL("[WMS_MMS_SaveMMS] Open file error %x", result,0,0);
    	goto Exit;
    }

    switch(nKind)
    {
        case MMS_OUTBOX:
        {
            nMmsDataInfoType = CFGI_MMSOUTDATA_INFO;
            nMmsCoutType = CFGI_MMS_OUTCOUNT;
        }
        break;

        case MMS_DRAFTBOX:
        {
            nMmsDataInfoType = CFGI_MMSDRAFTDATA_INFO;
            nMmsCoutType = CFGI_MMS_DRAFTCOUNT;
        }
        break;
        
        default:
        case MMS_INBOX:
        {
            nMmsDataInfoType = CFGI_MMSINDATA_INFO;
            nMmsCoutType = CFGI_MMS_INCOUNT;
        }
        break;
    }
    
    ICONFIG_GetItem(pConfig, 
        nMmsCoutType,
        &g_mmsDataInfoMax,
        sizeof(g_mmsDataInfoMax));  

    ICONFIG_GetItem(pConfig,
       nMmsDataInfoType,
       (void*)mmsDataInfoList,
       sizeof(mmsDataInfoList));
                   
    if(g_mmsDataInfoMax <= 0 || index >= g_mmsDataInfoMax)
    {
        result = EFAILED;
        goto Exit;
    }   

    pMmsDataInfoListCur = &mmsDataInfoList[index];

    if(SUCCESS == IFILEMGR_Test(pIFileMgr,
        pMmsDataInfoListCur->MMSDataFileName))
    {
        IFile* pFile = NULL;
        pFile = IFILEMGR_OpenFile(pIFileMgr,pMmsDataInfoListCur->MMSDataFileName,_OFM_READ);
        if(pFile)
        {
            int nBitCount = 0;
            nBitCount = IFILE_Read(pFile,pBuf,pMmsDataInfoListCur->MMSDatasize);
            if(nBitCount == 0)
            {
                result = IFILEMGR_GetLastError(pIFileMgr);
                goto Exit;
            }
            *pLen = nBitCount;
        }
        RELEASEIF(pFile);
    }
    else
    {
        result = IFILEMGR_GetLastError(pIFileMgr);
        goto Exit;
    }
    
    
Exit:
    RELEASEIF(pIFileMgr);
    RELEASEIF(pConfig);
    
    return pBuf;
}

#if 0
int WMS_MMS_SEND_TEST(uint8 *buffer, char* sendNumber)
{
	
    uint8 smil_buf[2000] = {0};
	int head_len = 0;
	int size = 0;
	uint8 *pCurPos = WMS_MMS_BUFFERGet();
	int len;
	int i,index = mms_data.frag_num;
    IConfig *pConfig;
	IShell  *pShell = AEE_GetShell();
    char MMSImagepszPath[70];
    char MMSSoundpszPath[70];
    char MMSVideopszPath[70];
    MEMSET((void*)WMS_MMS_BUFFERGet(), 0, 150*1024);
	//head_len = MMS_Encode_header(pCurPos,(uint8*)"+8613714333583",(uint8*)"123456789");
	head_len = MMS_Encode_header(pCurPos,(uint8*)sendNumber,(uint8*)"123456789");
	MMS_DEBUG(("WMS_MMS_SEND_TEST head_len = %d, sendNumber len=%d, frag_num=%d",head_len, STRLEN(sendNumber), mms_data.frag_num));	
	pCurPos += head_len;

    //MEMSET((void*)&mms_data, 0, sizeof(mms_data));
#if 1	
    if (ISHELL_CreateInstance(pShell, AEECLSID_CONFIG,(void **)&pConfig) != SUCCESS)
    {
        return 0;
    }
    (void) ICONFIG_GetItem(pConfig,CFGI_MMS_OUTCOUNT,&g_mmsDataInfoMax,sizeof(g_mmsDataInfoMax));
    ICONFIG_GetItem(pConfig, CFGI_MMSIMAGE,MMSImagepszPath, sizeof(MMSImagepszPath));
    ICONFIG_GetItem(pConfig, CFGI_MMSSOUND,MMSSoundpszPath, sizeof(MMSSoundpszPath));
    ICONFIG_GetItem(pConfig, CFGI_MMSVIDEO,MMSVideopszPath, sizeof(MMSVideopszPath));
    if(STRLEN(MMSImagepszPath) != 0)
    {
        mms_data.bImage = TRUE;
        len = STRLEN(MMS_GetMimeType(MMSImagepszPath));
        DBGPRINTF("GetMimeType(MMSImagepszPath)=%s len=%d", MMS_GetMimeType(MMSImagepszPath), STRLEN(MMS_GetMimeType(MMSImagepszPath)));
    	MEMCPY((char*)mms_data.fragment[index].hContentType,MMS_GetMimeType(MMSImagepszPath),len);
    	len = STRLEN(MMSImagepszPath);
    	STRNCPY((char*)mms_data.fragment[index].hContentFile,MMSImagepszPath,len);
    	len = STRLEN(BASENAME(MMSImagepszPath));
    	MEMCPY((void*)mms_data.fragment[index].hContentLocation,(void*)BASENAME(MMSImagepszPath),len);
    	MEMCPY((void*)mms_data.fragment[index].hContentID,(void*)BASENAME(MMSImagepszPath),len);
    	MEMCPY((void*)mms_data.fragment[index].hContentName,(void*)BASENAME(MMSImagepszPath),len);   
        mms_data.frag_num++;
        ++index;
        DBGPRINTF("MMSImagepszPath=%s len=%d", BASENAME(MMSImagepszPath), STRLEN(MMSImagepszPath));
    }
    if(STRLEN(MMSSoundpszPath) != 0)
    {
        mms_data.bAudio = TRUE;
        len = STRLEN(MMS_GetMimeType(MMSSoundpszPath));
    	MEMCPY((char*)mms_data.fragment[index].hContentType,MMS_GetMimeType(MMSSoundpszPath),len);
        DBGPRINTF("GetMimeType(MMSSoundpszPath)=%s len=%d", MMS_GetMimeType(MMSSoundpszPath), STRLEN(MMS_GetMimeType(MMSSoundpszPath)));
    	len = STRLEN(MMSSoundpszPath);
    	STRNCPY((char*)mms_data.fragment[index].hContentFile,MMSSoundpszPath,len);
    	len = STRLEN(BASENAME(MMSSoundpszPath));
    	MEMCPY((void*)mms_data.fragment[index].hContentLocation,(void*)BASENAME(MMSSoundpszPath),len);
    	MEMCPY((void*)mms_data.fragment[index].hContentID,(void*)BASENAME(MMSSoundpszPath),len);
    	MEMCPY((void*)mms_data.fragment[index].hContentName,(void*)BASENAME(MMSSoundpszPath),len);   
        mms_data.frag_num++;
        ++index;
        DBGPRINTF("MMSSoundpszPath=%s len=%d", BASENAME(MMSSoundpszPath), STRLEN(MMSSoundpszPath));
        MMS_DEBUG(("hContentLocation=%s",(char*)mms_data.fragment[index-1].hContentLocation));
        MMS_DEBUG(("hContentID=%s",(char*)mms_data.fragment[index-1].hContentID));
        MMS_DEBUG(("hContentName=%s",(char*)mms_data.fragment[index-1].hContentName));
    }   
    if(STRLEN(MMSVideopszPath) != 0)
    {
        mms_data.bVideo = TRUE;
        len = STRLEN(MMS_GetMimeType(MMSVideopszPath));
    	MEMCPY((char*)mms_data.fragment[index].hContentType,MMS_GetMimeType(MMSVideopszPath),len);
    	len = STRLEN(MMSVideopszPath);
    	MEMCPY((char*)mms_data.fragment[index].hContentFile,MMSVideopszPath,len);
    	len = STRLEN(BASENAME(MMSVideopszPath));
    	MEMCPY((void*)mms_data.fragment[index].hContentLocation,(void*)BASENAME(MMSVideopszPath),len);
    	MEMCPY((void*)mms_data.fragment[index].hContentID,(void*)BASENAME(MMSVideopszPath),len);
    	MEMCPY((void*)mms_data.fragment[index].hContentName,(void*)BASENAME(MMSVideopszPath),len);   
        mms_data.frag_num++;
        ++index;
        DBGPRINTF("MMSVideopszPath=%s len=%d", BASENAME(MMSVideopszPath), STRLEN(MMSVideopszPath));  
    }   
 
#else    
	mms_data.frag_num = 1;
    mms_data.bImage = TRUE;

	len = STRLEN("image/jpeg");
	STRNCPY((char*)mms_data.fragment[0].hContentType,"image/jpeg",len);
	len = STRLEN("fs:/hsmm/pictures/1.jpg");
	STRNCPY((char*)mms_data.fragment[0].hContentFile,"fs:/hsmm/pictures/1.jpg",len);
	len = STRLEN("1.jpg");
	STRNCPY((char*)mms_data.fragment[0].hContentLocation,"1.jpg",len);
	len = STRLEN("1.jpg");
	STRNCPY((char*)mms_data.fragment[0].hContentID,"1.jpg",len);
	len = STRLEN("1.jpg");
	STRNCPY((char*)mms_data.fragment[0].hContentName,"1.jpg",len);

    //彩信里的消息文本
	len = STRLEN("123456789");
	STRNCPY((char*)mms_data.fragment[1].hContentText,"123456789",len);
	len = STRLEN("text/plain");
	STRNCPY((char*)mms_data.fragment[1].hContentType,"text/plain",len);
	len = STRLEN("1.txt");
	STRNCPY((char*)mms_data.fragment[1].hContentLocation,"1.txt",len);
	len = STRLEN("1.txt");
	STRNCPY((char*)mms_data.fragment[1].hContentID,"1.txt",len);
	len = STRLEN("1.txt");
	STRNCPY((char*)mms_data.fragment[1].hContentName,"1.txt",len);
#endif		     

	head_len = MMS_Encode_MsgBody(pCurPos,0,mms_data.frag_num+1);
	MMS_DEBUG(("[MMS] MMS_Encode_MsgBody head_len1 = %d",head_len));	
	pCurPos += head_len;

	len = WMS_MMS_CreateSMIL(smil_buf,2000,mms_data);	
	head_len = MMS_EncodeSmilFile(pCurPos,smil_buf,len);
	MMS_DEBUG(("[MMS] MMS_EncodeSmilFile head_len2 = %d",head_len));	
	pCurPos += head_len;

	for(i=0; i<mms_data.frag_num; i++)
	{
		head_len = MMS_GetFileContent(pCurPos,mms_data.fragment[i]);
		pCurPos += head_len;
	}

	size = (int)(pCurPos-WMS_MMS_BUFFERGet());

	MMS_DEBUG(("[MMS] MMS_SEND_TEST size = %d",size));
	SNPRINTF((char*)buffer,MSG_MAX_PACKET_SIZE,POST_TEST,size);

	MMS_DEBUG(("POST_TEST:%s",buffer));
	head_len = STRLEN((char*)buffer);
	MMS_DEBUG(("[MMS] POST_TEST head_len = %d",head_len));
	
	MEMCPY((void*)(buffer+head_len),(void*)WMS_MMS_BUFFERGet(),size);//将来直接保存buf就行了,再解析时用WMS_MMS_WSP_DecodeMessage解析就可以了

// Save mms
    WMS_MMS_SaveMMS(sendNumber,(char*)WMS_MMS_BUFFERGet(),size,MMS_OUTBOX);
//
	return (head_len+size);
}
#endif

int WMS_MMS_Decode_TEST(char *file)
{
	IFile* pIFile = NULL;
    IFileMgr *pIFileMgr = NULL;
    FileInfo pInfo = {0};
	int result = 0;
	int data_size;
	int decret;
	WSP_MMS_DATA *pContent = (WSP_MMS_DATA *)MALLOC(sizeof(WSP_MMS_DATA));

    MEMSET(pContent, NULL,sizeof(WSP_MMS_DATA));
    
    result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
	if (SUCCESS != result)
    {
		MSG_FATAL("[MMS]: Open file error %x", result,0,0);
		return -1;
    }

	if (IFILEMGR_GetInfo(pIFileMgr,file, &pInfo) == SUCCESS)
    {
		pIFile = IFILEMGR_OpenFile( pIFileMgr, file, _OFM_READ);
		if ( pIFile != NULL )
	    {
	        IFILE_Seek(pIFile, _SEEK_START, 1);
	        data_size = IFILE_Read(pIFile,WMS_MMS_BUFFERGet(), pInfo.dwSize);

	        MSG_FATAL("[MMS]: pInfo.dwSize=%d,data_size=%d",pInfo.dwSize,data_size,0);
	    }    
    }
    IFILE_Release( pIFile);
    pIFile = NULL;
    IFILEMGR_Release(pIFileMgr);
    pIFileMgr = NULL;

    MMS_DEBUG(("Decode testing start!"));
    MSG_FATAL("[MMS]: 0x%x 0x%x 0x%x",WMS_MMS_BUFFERGet()[0],WMS_MMS_BUFFERGet()[1],WMS_MMS_BUFFERGet()[2]);
	decret = WMS_MMS_WSP_DecodeMessage(WMS_MMS_BUFFERGet(), data_size,pContent);
	if (decret == MMC_OK)
	{
		MMS_DEBUG(("Decode testing success"));
	}

	return 0;
}

int WMS_MMS_PDU_Decode_Test(char *file)
{
    IFile* pIFile = NULL;
    IFileMgr *pIFileMgr = NULL;
    FileInfo pInfo = {0};
	int result = 0;
	int data_size;
	int decret;
	uint8 ePDUType = 0;
	
    MMS_WSP_DEC_DATA* pDecdata = (MMS_WSP_DEC_DATA*)MALLOC(sizeof(MMS_WSP_DEC_DATA));
    
    MEMSET(pDecdata, NULL,sizeof(MMS_WSP_DEC_DATA));
    
    result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
	if (SUCCESS != result)
    {
		MSG_FATAL("[MMS]: Open file error %x", result,0,0);
		return -1;
    }

	if (IFILEMGR_GetInfo(pIFileMgr,file, &pInfo) == SUCCESS)
    {
		pIFile = IFILEMGR_OpenFile( pIFileMgr, file, _OFM_READ);
		if ( pIFile != NULL )
	    {
	        IFILE_Seek(pIFile, _SEEK_START, 0);
	        data_size = IFILE_Read(pIFile,WMS_MMS_BUFFERGet(), pInfo.dwSize);

	        MSG_FATAL("[MMS]: pInfo.dwSize=%d,data_size=%d",pInfo.dwSize,data_size,0);
	    }    
    }
    IFILE_Release( pIFile);
    pIFile = NULL;
    IFILEMGR_Release(pIFileMgr);
    pIFileMgr = NULL;


    MMS_DEBUG(("Decode testing start! "));
    MSG_FATAL("[MMS]: 0x%x 0x%x 0x%x",WMS_MMS_BUFFERGet()[0],WMS_MMS_BUFFERGet()[1],WMS_MMS_BUFFERGet()[2]);
	decret = WMS_MMS_PDU_Decode(pDecdata,WMS_MMS_BUFFERGet(), data_size,&ePDUType);
	if (decret == MMC_OK)
	{
		MMS_DEBUG(("Decode testing success"));
	}

	return 0;
}

void WMS_MMS_DATA_Encode(WSP_MMS_ENCODE_DATA* pData)
{
    IConfig *pConfig = NULL;
	IShell  *pShell = AEE_GetShell();
    int len = 0;
    int index = 0;
    char* pFilePath = NULL;
    uint32 size = 0;
    char* pFileType = NULL;

    MMS_DEBUG(("[WMS_MMS_DATA_Encode] ENTER"));
    
    if(pData == NULL)
    {
        MMS_DEBUG(("[WMS_MMS_DATA_Encode] pData == NULL"));
        return ;
    }
#if 1    
    if (ISHELL_CreateInstance(pShell, AEECLSID_CONFIG,(void **)&pConfig) != SUCCESS)
    {
        return ;
    }
    
    ICONFIG_GetItem(pConfig,CFGI_MMS_OUTCOUNT,&g_mmsDataInfoMax,sizeof(g_mmsDataInfoMax));

//    pFilePath = (char*)(pData->fragment[1].hContentFile);

    pData->frag_num = 0;
    while(index < WMSMMS_FRAGMENTCOUNT)
    {
        if(index == 0)
        {
            MSG_FATAL("STRLEN((char*)pData->fragment[index].hContentText) != 0",0,0,0);
            len = STRLEN("text/plain");
            STRNCPY((char*)pData->fragment[0].hContentType,"text/plain",len);
            len = STRLEN("1.txt");
            STRNCPY((char*)pData->fragment[0].hContentLocation,"1.txt",len);
            len = STRLEN("1.txt");
            STRNCPY((char*)pData->fragment[0].hContentID,"1.txt",len);
            len = STRLEN("1.txt");
            STRNCPY((char*)pData->fragment[0].hContentName,"1.txt",len);  
            pData->frag_num ++;
        }
        else if(STRLEN(pFilePath = (char*)(pData->fragment[index].hContentFile)) != 0)
        {
            MMS_DEBUG(("[WMS_MMS_DATA_Encode] Find Resource"));
            MMS_DEBUG(("[WMS_MMS_DATA_Encode] File Path:%s",pFilePath));
            ISHELL_DetectType(AEE_GetShell(),NULL,&size,pFilePath,(const char**)&pFileType);
            pData->fragment[index].pType =  pFileType ? pFileType : OTHER_MIME_BASE;//IMAGE_MIME_BASE;

            DBGPRINTF("[WMS_MMS_DATA_Encode] File Type:%s",pData->fragment[index].pType);
            len = STRLEN(MMS_GetMimeType(pFilePath));
        	STRNCPY((char*)pData->fragment[index].hContentType,MMS_GetMimeType(pFilePath),len);

        	len = STRLEN(BASENAME(pFilePath));
        	MEMCPY((void*)pData->fragment[index].hContentLocation,(void*)BASENAME(pFilePath),len);
        	MEMCPY((void*)pData->fragment[index].hContentID,(void*)BASENAME(pFilePath),len);
        	MEMCPY((void*)pData->fragment[index].hContentName,(void*)BASENAME(pFilePath),len); 
        	pData->frag_num ++;
        }
        ++index;
    }; 
#else    
	pData->frag_num = 1;
    pData->bImage = TRUE;

	len = STRLEN("image/jpeg");
	STRNCPY((char*)pData->fragment[0].hContentType,"image/jpeg",len);
	len = STRLEN("fs:/hsmm/pictures/1.jpg");
	STRNCPY((char*)pData->fragment[0].hContentFile,"fs:/hsmm/pictures/1.jpg",len);
	len = STRLEN("1.jpg");
	STRNCPY((char*)pData->fragment[0].hContentLocation,"1.jpg",len);
	len = STRLEN("1.jpg");
	STRNCPY((char*)pData->fragment[0].hContentID,"1.jpg",len);
	len = STRLEN("1.jpg");
	STRNCPY((char*)pData->fragment[0].hContentName,"1.jpg",len);

    //彩信里的消息文本
	len = STRLEN("123456789");
	STRNCPY((char*)pData->fragment[1].hContentText,"123456789",len);
	len = STRLEN("text/plain");
	STRNCPY((char*)pData->fragment[1].hContentType,"text/plain",len);
	len = STRLEN("1.txt");
	STRNCPY((char*)pData->fragment[1].hContentLocation,"1.txt",len);
	len = STRLEN("1.txt");
	STRNCPY((char*)pData->fragment[1].hContentID,"1.txt",len);
	len = STRLEN("1.txt");
	STRNCPY((char*)pData->fragment[1].hContentName,"1.txt",len);
#endif
    MMS_DEBUG(("[WMS_MMS_DATA_Encode] EXIT"));
    RELEASEIF(pConfig);
}

int WMS_MMS_PDU_Encode(MMS_WSP_ENCODE_SEND* encdata, uint8* hPDU, uint8 ePDUType)
{
	uint32 head_len = 0;
	uint8 *pCurPos = WMS_MMS_BUFFERGet();
	int len;
	int i;
	uint32 size = 0;
    IConfig             *pConfig;
    char serverAddress[MAX_URL_LEN+1] = {0};
    IShell  *pShell = AEE_GetShell();
    if (ISHELL_CreateInstance(pShell, AEECLSID_CONFIG,(void **)&pConfig) != SUCCESS)
    {
        return ;
    }
    ICONFIG_GetItem(pConfig, CFGI_MMS_SERVER_ADDRESS, serverAddress, sizeof(serverAddress));
    (void)ICONFIG_Release(pConfig);
    pConfig = NULL;
    DBGPRINTF("serverAddress=%s", serverAddress);
	MMS_DEBUG(("[WMS_MMS_PDU_Encode] ENTER"));
	if ( hPDU == NULL || encdata == NULL )
	{
		return MMC_NOMEM;
	}
    MEMSET((void*)WMS_MMS_BUFFERGet(), 0, MSG_MAX_PACKET_SIZE);	  
    DBGPRINTF("WMS_MMS_PDU_Encode ePDUType=%d", ePDUType);
    // 只有收和发有body
	switch(ePDUType)
	{
	    case WMS_MMS_PDU_MSendReq:
	    {
	        if(encdata->pMessage == NULL)
	            return 0;
	        // 添加数据
            WMS_MMS_DATA_Encode(&(encdata->pMessage->mms_data));
	        // Encode
	        head_len = MMS_Encode_header(pCurPos,WMS_MMS_PDU_MSendReq,encdata);
	        pCurPos += head_len;

	        head_len = MMS_Encode_MsgBody(pCurPos,&encdata->pMessage->mms_data);
	        pCurPos += head_len;
        	MMS_DEBUG(("[MMS] MMS_Encode_MsgBody head_len1 = %d",head_len));	
            

            size = (int)(pCurPos-WMS_MMS_BUFFERGet());

            MMS_DEBUG(("[MMS] MMS_SEND_TEST size = %d",size));
            SNPRINTF((char*)hPDU,MSG_MAX_PACKET_SIZE,POST_TEST, serverAddress, serverAddress, size);

            DBGPRINTF(("POST_TEST:%s",(char*)hPDU));
            head_len = STRLEN((char*)hPDU);
            MMS_DEBUG(("[MMS] POST_TEST head_len = %d",head_len));

            MEMCPY((void*)(hPDU+head_len),(void*)WMS_MMS_BUFFERGet(),size);//将来直接保存buf就行了,再解析时用WMS_MMS_WSP_DecodeMessage解析就可以了

            // SaveMMS
            WMS_MMS_SaveMMS((char*)encdata->pMessage->hTo,(char*)WMS_MMS_BUFFERGet(),size,MMS_OUTBOX);
	    }
	    break;

        case WMS_MMS_PDU_DRAFT:
        {
            if(encdata->pMessage == NULL)
                return 0;
            // 添加数据
            WMS_MMS_DATA_Encode(&(encdata->pMessage->mms_data));
            // Encode
            head_len = MMS_Encode_header(pCurPos,WMS_MMS_PDU_MSendReq,encdata);
            pCurPos += head_len;
        
            head_len = MMS_Encode_MsgBody(pCurPos,&encdata->pMessage->mms_data);
            pCurPos += head_len;
            MMS_DEBUG(("[MMS] MMS_Encode_MsgBody head_len1 = %d",head_len));    
            
        
            size = (int)(pCurPos-WMS_MMS_BUFFERGet());
        
            MMS_DEBUG(("[MMS] MMS_SEND_TEST size = %d",size));
            SNPRINTF((char*)hPDU,MSG_MAX_PACKET_SIZE,POST_TEST, serverAddress, serverAddress, size);
        
            DBGPRINTF(("POST_TEST:%s",(char*)hPDU));
            head_len = STRLEN((char*)hPDU);
            MMS_DEBUG(("[MMS] POST_TEST head_len = %d",head_len));
        
            MEMCPY((void*)(hPDU+head_len),(void*)WMS_MMS_BUFFERGet(),size);//将来直接保存buf就行了,再解析时用WMS_MMS_WSP_DecodeMessage解析就可以了
        
            // SaveMMS
            WMS_MMS_SaveMMS((char*)encdata->pMessage->hTo,(char*)WMS_MMS_BUFFERGet(),size,MMS_DRAFTBOX);
        }
        break;

        case WMS_MMS_PDU_MDeliveryInd:
        {
            MMS_DEBUG(("[WMS_MMS_PDU_MDeliveryInd] "));
            if(encdata->pMessage == NULL)
                return 0;
            // 添加数据
            WMS_MMS_DATA_Encode(&(encdata->pMessage->mms_data));
            // Encode
            head_len = MMS_Encode_header(pCurPos,WMS_MMS_PDU_MSendReq,encdata);
            pCurPos += head_len;
        
            head_len = MMS_Encode_MsgBody(pCurPos,&encdata->pMessage->mms_data);
            pCurPos += head_len;
            MMS_DEBUG(("[MMS] MMS_Encode_MsgBody head_len1 = %d",head_len));    
            
        
            size = (int)(pCurPos-WMS_MMS_BUFFERGet());
        
            MMS_DEBUG(("[MMS] MMS_SEND_TEST size = %d",size));
            SNPRINTF((char*)hPDU,MSG_MAX_PACKET_SIZE,POST_TEST, serverAddress, serverAddress, size);
        
            DBGPRINTF(("POST_TEST:%s",(char*)hPDU));
            head_len = STRLEN((char*)hPDU);
            MMS_DEBUG(("[MMS] POST_TEST head_len = %d",head_len));
        
            MEMCPY((void*)(hPDU+head_len),(void*)WMS_MMS_BUFFERGet(),size);//将来直接保存buf就行了,再解析时用WMS_MMS_WSP_DecodeMessage解析就可以了
        
            // SaveMMS
            WMS_MMS_SaveMMS((char*)encdata->pMessage->hTo,(char*)WMS_MMS_BUFFERGet(),size,MMS_INBOX);
        }
        break;

	    case WMS_MMS_PDU_MNotifyrespInd:
	    {
	        if(NULL == encdata->pNotifyresp)
	            return 0;
	        head_len = MMS_Encode_header(pCurPos,WMS_MMS_PDU_MNotifyrespInd,encdata);
	        pCurPos += head_len;
	        size = head_len;

            MMS_DEBUG(("[MMS] MMS_SEND_TEST size = %d",size));
            SNPRINTF((char*)hPDU,MSG_MAX_PACKET_SIZE,POST_TEST, serverAddress, serverAddress, size);

            DBGPRINTF(("POST_TEST:%s",(char*)hPDU));
            head_len = STRLEN((char*)hPDU);
            MMS_DEBUG(("[MMS] POST_TEST head_len = %d",head_len));

            MEMCPY((void*)(hPDU+head_len),(void*)WMS_MMS_BUFFERGet(),size);
	    }
	    break;
	    case WMS_MMS_PDU_MAcknowledgeInd:
	    {
	        if(NULL == encdata->pDeliveryacknowledgement)
	            return 0;
	        head_len = MMS_Encode_header(pCurPos,WMS_MMS_PDU_MAcknowledgeInd,encdata);
	        pCurPos += head_len;
	        size = head_len;

            MMS_DEBUG(("[MMS] MMS_SEND_TEST size = %d",size));
            SNPRINTF((char*)hPDU,MSG_MAX_PACKET_SIZE,POST_TEST, serverAddress, serverAddress, size);

            DBGPRINTF(("POST_TEST:%s",(char*)hPDU));
            head_len = STRLEN((char*)hPDU);
            MMS_DEBUG(("[MMS] POST_TEST head_len = %d",head_len));

            MEMCPY((void*)(hPDU+head_len),(void*)WMS_MMS_BUFFERGet(),size);
	    }
	    break;
	    case WMS_MMS_PDU_MReadRecInd:
	    {
	        if(NULL == encdata->pReadReport)
	            return 0;
	        head_len = MMS_Encode_header(pCurPos,WMS_MMS_PDU_MReadRecInd,encdata);
	        pCurPos += head_len;
	        size = head_len;

            MMS_DEBUG(("[MMS] MMS_SEND_TEST size = %d",size));
            SNPRINTF((char*)hPDU,MSG_MAX_PACKET_SIZE,POST_TEST, serverAddress, serverAddress, size);

            DBGPRINTF(("POST_TEST:%s",(char*)hPDU));
            head_len = STRLEN((char*)hPDU);
            MMS_DEBUG(("[MMS] POST_TEST head_len = %d",head_len));

            MEMCPY((void*)(hPDU+head_len),(void*)WMS_MMS_BUFFERGet(),size);
	    }
	    break;
	    default:
	    {
	        return 0;
	    }
	    break;
	}

    MMS_DEBUG(("[WMS_MMS_PDU_Encode] EXIT"));
	return (head_len+size);
}

int WMS_MMS_WSP_DecodeMessage(uint8* pData, int iDataLen,  WSP_MMS_DATA* pContent)
{		
	int charset, dec = MMC_GENERIC, ret = MMC_GENERIC, contenttypelen;
	boolean bIsMultipart, completed = FALSE;
	int iDataOffset,consumed=0;
	
	contenttypelen = MMS_WSP_GetValueLen(pData, iDataLen, &iDataOffset);
	if (contenttypelen != MMS_DECODER_ERROR_VALUE)
	{
		int depth = 0;

		ret = MMS_WSP_DecodeContentTypeHeader(&pData[iDataOffset], contenttypelen, &pContent->head_info, &bIsMultipart, &charset, TRUE, 0, FALSE, &depth);

		MMS_DEBUG(("[MMS]: MMS_WSP_DecodeContentTypeHeader hContentType=%s",(char*)pContent->head_info.hContentType));
		MMS_DEBUG(("[MMS]: MMS_WSP_DecodeContentTypeHeader ret=%d bIsMultipart:%d",ret,bIsMultipart));
		
		if (ret != MMS_DECODER_ERROR_VALUE)
		{
			consumed = contenttypelen+iDataOffset;
			
			if (bIsMultipart == TRUE)
			{				
				pContent->frag_num = MMS_WSP_Decode_UINTVAR(&pData[consumed],iDataLen-consumed,&iDataOffset);
				MMS_DEBUG(("[MMS]: pContent->frag_num = %d",pContent->frag_num));
				if (pContent->frag_num != MMS_DECODER_ERROR_VALUE)
				{
					dec = MMS_WSP_Decode_MultipartData(&pData[consumed+iDataOffset],
						iDataLen-consumed-iDataOffset,pContent->frag_num,pContent->fragment, &depth);
					if (dec != MMC_GENERIC)
						completed = TRUE;
				}
			}
			else
			{
				dec = MMC_MIME_CONTENT_TYPE;
			}
		}
		else
		{
			dec = MMC_MIME_CONTENT_TYPE;
		}
	}
	else
	{
		dec = MMC_MIME_CONTENT_TYPE;
	}

	return 0;
}

static int MMS_WSP_Decode_MultipartData(uint8* pData, int iDataLen,int nParts, WSP_DEC_DATA_FRAGMENT* iMIMEParts,int *inout_depth)
{
	int i,iHeadersLen, iPartDataLen, iDataOffset,ret,consumed = 0;
	int iHeadersDataOffset, iDataDataOffset, contenttypelen, charset;
	boolean bIsMultipart;
	uint8* pbyte;
	int iInsDataLen, otherheaders;
	boolean bodycreated, modified = FALSE;
	int size = 0;
	int acc, iAccOffset, dec;
	int cur_part = -1;
	MMS_DEBUG(("[MMS]: MMS_WSP_Decode_MultipartData nParts=%d",nParts));
	
	for(i=0;(i < nParts)&&(consumed < iDataLen);i++)
	{
		cur_part++;
		
		iHeadersLen = MMS_WSP_Decode_UINTVAR(&pData[consumed], iDataLen-consumed,
			&iHeadersDataOffset);
		if (iHeadersLen == MMS_DECODER_ERROR_VALUE)
			goto leave_it;
		iPartDataLen = MMS_WSP_Decode_UINTVAR(&pData[iHeadersDataOffset+consumed],
			iDataLen-iHeadersDataOffset-consumed, &iDataDataOffset);
		if (iPartDataLen == MMS_DECODER_ERROR_VALUE)
			goto leave_it;

		contenttypelen = MMS_WSP_GetValueLen(&pData[iHeadersDataOffset+iDataDataOffset+consumed],
			iHeadersLen, &iDataOffset);
		if (contenttypelen == MMS_DECODER_ERROR_VALUE)
			goto skip_it;
		charset = 0;
		
		ret = MMS_WSP_DecodeContentTypeHeader(&pData[iHeadersDataOffset+iDataDataOffset+iDataOffset+consumed],
			contenttypelen, &iMIMEParts[cur_part], &bIsMultipart, &charset, TRUE, (iHeadersLen - iDataOffset), TRUE, inout_depth);
		if (ret == MMS_DECODER_ERROR_VALUE)
			goto skip_it;

		MMS_DEBUG(("[MMS]: bIsMultipart=%d,cur_part=%d",bIsMultipart,cur_part));
		MMS_DEBUG(("[MMS]: hContentType=%s",&iMIMEParts[cur_part].hContentType));
		MMS_DEBUG(("[MMS]: hContentName=%s",&iMIMEParts[cur_part].hContentName));
		MMS_DEBUG(("[MMS]: hContentID=%s",&iMIMEParts[cur_part].hContentID));
		MMS_DEBUG(("[MMS]: hContentLocation=%s",&iMIMEParts[cur_part].hContentLocation));
		MMS_DEBUG(("[MMS]: hContentEnCode=%s",&iMIMEParts[cur_part].hContentEnCode));
		
		if (bIsMultipart == TRUE)
		{
			consumed += (contenttypelen + iHeadersDataOffset + iDataDataOffset + iDataOffset);
			acc = MMS_WSP_Decode_UINTVAR(&pData[consumed], iDataLen - consumed, &iAccOffset);
            DBGPRINTF("acc==0x%x", acc);
			if (acc != MMS_DECODER_ERROR_VALUE)
			{
				dec = MMS_WSP_Decode_MultipartData(&pData[consumed+iAccOffset],iDataLen-consumed-iAccOffset, acc, &iMIMEParts[cur_part], inout_depth);
			}
			consumed += iPartDataLen;
			continue;
		}
		else
		{
			/* other headers */
			if (iHeadersLen > contenttypelen+iDataOffset)
			{
				otherheaders = contenttypelen+iDataOffset;
				ret = MMS_WSP_DecodeContentTypeParams(&pData[iHeadersDataOffset+iDataDataOffset+consumed+otherheaders],
					iHeadersLen-otherheaders, &iMIMEParts[cur_part],&charset);
				if (ret == MMS_DECODER_ERROR_VALUE)
					goto skip_it;
			}
			
			pbyte = &pData[iHeadersDataOffset+iDataDataOffset+iHeadersLen+consumed];
			iInsDataLen = iPartDataLen;
			size += iPartDataLen;
			if (size <= MMS_MAX_RECEIVED_MMS_CONTENT_SIZE)
			{
				bodycreated = FALSE;
				if (charset == 0x03e8)
				{
				}
				else
				{
					//bodycreated = TMIMEParts_NewBodySS(iMIMEParts,iPart,pbyte,iInsDataLen,SLIM_MIMECODEC_BINARY,TRUE);
					iMIMEParts[cur_part].size = iPartDataLen;
					iMIMEParts[cur_part].pContent = MALLOC(iPartDataLen+1);
                    iMIMEParts[cur_part].pContent[iPartDataLen] = '\0';
					MEMCPY(iMIMEParts[cur_part].pContent,pbyte,iPartDataLen);
				}

				if (!bodycreated)
					goto skip_it;
			}
			else
			{
				size -= iPartDataLen;
skip_it:
				modified = TRUE;
			}
			consumed += iHeadersDataOffset+iDataDataOffset+iHeadersLen+iPartDataLen;
			continue;
		}
leave_it:
		modified = TRUE;
		goto completed;
	}
completed:
/*	if (iPart == 0)
		return MMC_GENERIC;*/
	if (modified)
	{
		return MMC_MODIFIED;
	}
	return MMC_OK;
}

static int MMS_WSP_DecodeContentTypeParams(uint8* pData, int iDataLen, WSP_DEC_DATA_FRAGMENT* iMIMEParams, int* charset)
{
	/*
	 * 8.4.2.3 Parameter Values
	 * 8.4.2.4 Parameter
	 */

	char* paramname;
	char* paramvalue;
	int paramnamelen,paramvaluelen,iDataOffsetname, iDataOffsetvalue,consumed=0;
	boolean ok;
	int charset_type;
    DBGPRINTF("MMS_WSP_DecodeContentTypeParams Start");
	while (consumed < iDataLen)
	{
		paramnamelen = MMS_WSP_GetValueLen(&pData[consumed],iDataLen,&iDataOffsetname);
		if (paramnamelen == MMS_DECODER_ERROR_VALUE)
			return MMS_DECODER_ERROR_VALUE;
		paramname = (char*)&pData[consumed+iDataOffsetname];
		consumed += paramnamelen+iDataOffsetname;
        DBGPRINTF("MMS_WSP_DecodeContentTypeParams paramnamelen=%d", paramnamelen);
		ok = TRUE;
		if (paramnamelen == 1)/* case of Short-interger */
		{
			/*
			 * Short-integer = OCTET
			 * ; Integers in range 0-127 shall be encoded as a one octet value with the most significant bit set
			 * ; to one (1xxx xxxx) and with the value in the remaining least significant bits.
			 */
			uint8 short_integer;

			paramvaluelen = MMS_WSP_GetValueLen(&pData[consumed],
			iDataLen-consumed,&iDataOffsetvalue);
    		if (paramvaluelen == MMS_DECODER_ERROR_VALUE)
    			return MMS_DECODER_ERROR_VALUE;
    		paramvalue = (char*)&pData[consumed+iDataOffsetvalue];
    		consumed += paramvaluelen+iDataOffsetvalue;
		
			short_integer = paramname[0] & 0x7F;
            DBGPRINTF("MMS_WSP_DecodeContentTypeParams short_integer=0x%x", short_integer);
			switch(short_integer)
			{
				/* WAP-230-WSP-20010705-a, Approved Version 5 July 2001 
				 * Table 38. Well-Known Parameter Assignments
				 */
			case 0x0a: /* WAP230WSP20010705A_TABLE38_WELL_KNWON_PARAMETER_ASSINGMENTS_V1_2_Start*/
			case 0x19: /*WAP230WSP20010705A_TABLE38_WELL_KNWON_PARAMETER_ASSINGMENTS_V1_4_Start:*/
				paramname = "start";
				paramvaluelen--;/* ?? */
				break;
			case 0x03: /* WAP230WSP20010705A_TABLE38_WELL_KNWON_PARAMETER_ASSINGMENTS_V1_1_Type:*/
			case 0x09: /* WAP230WSP20010705A_TABLE38_WELL_KNWON_PARAMETER_ASSINGMENTS_V1_2_Type:*/
			{
				paramname = "type";
				paramvaluelen--;/* ?? */
				if (paramvaluelen > 0)
				{
					if (paramvalue[0] == '"')/* ?? Quoted? */
					{
						STRNCPY((char*)iMIMEParams->hContentType,(char*)&paramvalue[1],paramvaluelen-1);
					}
					else
					{
						STRNCPY((char*)iMIMEParams->hContentType,(char*)paramvalue,paramvaluelen);
					}
				}
				break;
			}
			case 0x01: /* WAP230WSP20010705A_TABLE38_WELL_KNWON_PARAMETER_ASSINGMENTS_V1_1_Charset:*/
				paramname = "charset";
				if (paramvaluelen == 1)
				{
					charset_type = paramvalue[0] - 0x80;
					switch(charset_type)
					{
							/*
							 * Table 42. Character Set Assignment Examples
							 * us-ascii 0x03 3
							 * utf-8 0x6A 106
							 * See also
							 * http://www.iana.org/assignments/character-sets
							 * ! We should prepare a rom table for this.
							 */
						case 0x03:
							paramvalue = "us-ascii";
							paramvaluelen = STRLEN(paramvalue);
							*charset = 0x03;
							if (paramvaluelen > 0)
							{
								if (paramvalue[0] == '"')/* ?? Quoted? */
								{
									STRNCPY((char*)iMIMEParams->hContentEnCode,(char*)&paramvalue[1],paramvaluelen-1);
								}
								else
								{
									STRNCPY((char*)iMIMEParams->hContentEnCode,(char*)paramvalue,paramvaluelen);
								}
							}
							break;
						case 0x6a:
                            DBGPRINTF("MMS_WSP_DecodeContentTypeParams 0x6a");
							paramvalue = "utf-8";
                            paramvaluelen = STRLEN(paramvalue);
                            DBGPRINTF("paramvaluelen=%d",paramvaluelen);
							*charset = 0x6a;
							if (paramvaluelen > 0)
							{
								if (paramvalue[0] == '"')/* ?? Quoted? */
								{
									MEMCPY((char*)iMIMEParams->hContentEnCode,(char*)&paramvalue[1],paramvaluelen-1);
								}
								else
								{
									MEMCPY((char*)iMIMEParams->hContentEnCode,(char*)paramvalue,paramvaluelen);
								}
							}
							break;
						default:
							/* Ugh! */
							ok = FALSE;
							break;
					}
				}
				else if (paramvaluelen == 2)
				{
					if ((paramvalue[0] == 0x03)&&(paramvalue[1] == 0xe8))
						/* iso-10646-ucs-2 0x03E8 1000 */
					{
						paramvalue = "utf-16";/* !? */
						paramvaluelen = STRLEN(paramvalue);
						*charset = 0x03e8; 
						if (paramvaluelen > 0)
						{
							if (paramvalue[0] == '"')/* ?? Quoted? */
							{
								STRNCPY((char*)iMIMEParams->hContentEnCode,(char*)&paramvalue[1],paramvaluelen-1);
							}
							else
							{
								STRNCPY((char*)iMIMEParams->hContentEnCode,(char*)paramvalue,paramvaluelen);
							}
						}
					}
					else
						/* Ugh! */
						ok = FALSE;
				}
				else
					ok = FALSE;
				break;
			case 0x05: /* WAP230WSP20010705A_TABLE38_WELL_KNWON_PARAMETER_ASSINGMENTS_V1_1_Name:*/
			case 0x17: /* WAP230WSP20010705A_TABLE38_WELL_KNWON_PARAMETER_ASSINGMENTS_V1_4_Name:*/
				paramname = "name";
				if(is_us_ascii_string(paramvalue, paramvaluelen))
				{
					paramvaluelen--;
				}
				else
				{					
					paramvalue = "non_ascii_name";
					paramvaluelen = STRLEN(paramvalue);
					consumed = iDataLen;/* consume all */
				}

				if (paramvaluelen > 0)
				{
					if (paramvalue[0] == '"')/* ?? Quoted? */
					{
						STRNCPY((char*)iMIMEParams->hContentName,(char*)&paramvalue[1],paramvaluelen-1);
					}
					else
					{
						STRNCPY((char*)iMIMEParams->hContentName,(char*)paramvalue,paramvaluelen);
					}
				}
						
				break;

			case 0x0e:		//content location
			{
				paramname = "content location";
				paramvaluelen--;/* ?? */
				if (paramvaluelen > 0)
				{
					if (paramvalue[0] == '"')/* ?? Quoted? */
					{
						STRNCPY((char*)iMIMEParams->hContentLocation,(char*)&paramvalue[1],paramvaluelen-1);
					}
					else
					{
						STRNCPY((char*)iMIMEParams->hContentLocation,(char*)paramvalue,paramvaluelen);
					}
				}
				break;
			}

			case 0x40:		//content ID
			{
				int i=0;
				paramname = "content ID";
				paramvaluelen--;/* ?? */
				if (paramvaluelen > 0)
				{
					if ( paramvalue[0] == 0x22 && paramvalue[1] == 0x3c)
					{
						//STRNCPY((char*)iMIMEParams->hContentID,(char*)paramvalue,paramvaluelen);
						for(i=0; i<(paramvaluelen-2); i++)
						{
							if ( paramvalue[i+2] == 0x3E)
							{
								break;
							}
							
							iMIMEParams->hContentID[i] = paramvalue[i+2];
						}
					}
				}
				break;
			}
			
			default:
				/* Ugh! */
				ok = FALSE;
				break;
			}
		}
		else
		{
			if (paramvaluelen > 0)
			{
				//do nothing
			}
			else
			{
				/* Ugh! negative legnth for param value*/ 
			}
		}
	}
	return consumed;
}

static int MMS_WSP_DecodeContentTypeHeader(uint8* pData, int iDataLen, WSP_DEC_DATA_FRAGMENT* iMIMEHeaders, boolean* bIsMultipart,
								 int* charset, boolean add_boundary_param, int iHeadersLen, boolean in_encode_check_param, int *inout_depth)
{
	int i = 0, iDataOffset, contenttype = -1,consumed=0;
	int contenttypevaluelen;
	char* headervaluect;

	*bIsMultipart = FALSE;

	if (iDataLen == 0)
		return MMS_DECODER_ERROR_VALUE;

	contenttypevaluelen = MMS_WSP_GetValueLen(pData, iDataLen, &iDataOffset);

	if (contenttypevaluelen == MMS_DECODER_ERROR_VALUE)
		return MMS_DECODER_ERROR_VALUE;

	if (iDataOffset == 0)
	{
		/* 8.4.1.2 Field values */
		/*
		 *  Value Interpretation of First Octet
		 *  0 - 30 This octet is followed by the indicated number (0 ?30) of data octets
		 *  31 This octet is followed by a uintvar, which indicates the number of data octets after it
		 *  32 - 127 The value is a text string, terminated by a zero octet (NUL character)
		 *  128 - 255 It is an encoded 7-bit value; this header has no more data
		 */
		if ((pData[i] >= 32) && (pData[i] <= 127))
		{
			for(;i<contenttypevaluelen;i++)
			{
				if (i >= iDataLen)
					return MMS_DECODER_ERROR_VALUE;
				if (pData[i] == 0)
				{
					i++;
					break;
				}
			}
		}
		else if (pData[i] >= 128)
		{
			contenttype = pData[i] - 128;
			i = 1;
		}
		else
		{
			/* Ugh! */
		}
	}
	
	if (iDataOffset > 0)
	{
		consumed += iDataOffset;
		contenttypevaluelen = MMS_WSP_GetValueLen(&pData[iDataOffset],
			iDataLen-consumed, &iDataOffset);
		if (contenttypevaluelen == MMS_DECODER_ERROR_VALUE)
			return MMS_DECODER_ERROR_VALUE;
		i = consumed;
		if (iDataOffset == 0)
		{
			if ((pData[i] >= 32) && (pData[i] <= 127))
			{
				for(;i<contenttypevaluelen;i++)
				{
					if (i >= iDataLen)
						return MMS_DECODER_ERROR_VALUE;
					if (pData[i] == 0)
					{
						i++;
						break;
					}
				}
			}
			else if (pData[i] >= 128)
			{
				if (i >= iDataLen)
					return MMS_DECODER_ERROR_VALUE;
				contenttype = pData[i] - 128;
				i++;
			}
			else
			{
				/* Ugh! */
			}
		}
	}

	if (contenttype == -1)
	{
		STRNCPY((char*)iMIMEHeaders->hContentType,(char*)&pData[iDataOffset+consumed],contenttypevaluelen-1);
	}

	consumed += i;

	if (contenttype != -1)
	{
		if (contenttype == SLIM_WSP_WELL_KNWON_VALULES_MIME_MULTIPART_MIXED)
			contenttype = SLIM_WSP_WELL_KNWON_VALULES_MIME_APPLICATION_VND_WAP_MULTIPART_MIXED;
		if (contenttype == SLIM_WSP_WELL_KNWON_VALULES_MIME_MULTIPART_ALTERNATIVE)
			contenttype = SLIM_WSP_WELL_KNWON_VALULES_MIME_APPLICATION_VND_WAP_MULTIPART_ALTERNATIVE;

		headervaluect = MMS_WSP_ContentTypeDB_MMS2Text(contenttype);

		STRNCPY((char*)iMIMEHeaders->hContentType,headervaluect,STRLEN(headervaluect));
	}

	consumed += MMS_WSP_DecodeContentTypeParams(&pData[consumed],iDataLen-consumed, iMIMEHeaders, charset);
	if (contenttype != SLIM_WSP_WELL_KNWON_VALULES_MIME_TEXT_PLAIN)
	{
		*charset = 0;
	}

	if (consumed == MMS_DECODER_ERROR_VALUE)
	{
		return MMS_DECODER_ERROR_VALUE;
	}

	if ((contenttype == SLIM_WSP_WELL_KNWON_VALULES_MIME_APPLICATION_VND_WAP_MULTIPART_RELATED) || (contenttype == SLIM_WSP_WELL_KNWON_VALULES_MIME_APPLICATION_VND_WAP_MULTIPART_MIXED) || (contenttype == SLIM_WSP_WELL_KNWON_VALULES_MIME_APPLICATION_VND_WAP_MULTIPART_ALTERNATIVE))
	{
		*bIsMultipart = TRUE;
	}

	return consumed;/*contenttypevaluelen;*/
}
void WMS_MMS_PDU_Decode_BuffClean(MMS_WSP_DEC_DATA* decdata,Mms_pdu_types type)
{
    switch(type)
    {
        case MMS_PDU_SEND_CONF:
        {
            MMS_WSP_DEC_MESSAGE_RECEIVED *message = &decdata->message;
            int i = 0;
            while(i < message->mms_data.frag_num && i < WMSMMS_FRAGMENTCOUNT)
            {
                FREEIF(message->mms_data.fragment[i].pContent);
                i++;
            };
            MEMSET(message,NULL,sizeof(MMS_WSP_DEC_MESSAGE_RECEIVED));
        }
        break;
        default:
        {
        }
        break;
    }
    
}
int WMS_MMS_PDU_Decode(MMS_WSP_DEC_DATA* decdata,uint8* ptr, int datalen,uint8 *ePDUType)
{
	int len,i=0,iDataOffset,j;
	uint8* pchar;
	int ret = MMC_OK;
    DBGPRINTF("WMS_MMS_PDU_Decode Start ePDUType=%d, datalen=%d",*ePDUType, datalen);
	if ( decdata == NULL)
	{
		return MMC_GENERIC;
	}
	
	MEMSET((void*)decdata,0,sizeof(MMS_WSP_DEC_DATA));

	if (datalen <= 2)
	{
	    DBGPRINTF("[WMS_MMS_PDU_Decode] datalen <= 2 ptr[i]:0x%x",ptr[i]);
		return MMC_GENERIC;
    }
	if (ptr[0] != 0x8c)
	{
	    DBGPRINTF("[WMS_MMS_PDU_Decode] ptr[0] != 0x8c ptr[i]:0x%x",ptr[0]);
		return MMC_GENERIC;
	}
	*ePDUType = 0;

	//目前只对彩信的通知进行处理
	switch(ptr[1])
	{
		case 0x81:
			*ePDUType = MMS_PDU_SEND_CONF;
			MEMSET((void*)decdata->sendconf.hMessageID,0,sizeof(decdata->notification.hContentLocation));
			MEMSET((void*)decdata->sendconf.hTransactionID,0,sizeof(decdata->notification.hContentLocation));
			decdata->sendconf.iResponseStatus = 0x80;
			break;
			
		case 0x82:
			*ePDUType = MMS_PDU_NOTIFICATION_IND;
			MEMSET((void*)decdata->notification.hContentLocation,0,sizeof(decdata->notification.hContentLocation));
			MEMSET((void*)decdata->notification.hFrom,0,sizeof(decdata->notification.hFrom));
			MEMSET((void*)decdata->notification.hSubject,0,sizeof(decdata->notification.hSubject));
			MEMSET((void*)decdata->notification.hTransactionID,0,sizeof(decdata->notification.hTransactionID));
			decdata->notification.iSize = -1;
			decdata->notification.iExpires = -1;
			decdata->notification.iDate = 0;
			decdata->notification.iPriority       = MMS_PRIORITY_NORMAL;
			decdata->notification.iMessageClass   = MMS_CLASS_PERSONAL;
			break;
			
		case 0x84:
			*ePDUType = MMS_PDU_RETRIEVE_CONF;
			*ePDUType = MMS_PDU_RETRIEVE_CONF;
			decdata->message.bDelRep = FALSE;
			decdata->message.bReadRep = FALSE;
			MEMSET((void*)decdata->message.hCc,0,sizeof(decdata->message.hCc));
			MEMSET((void*)decdata->message.hContentType,0,sizeof(decdata->message.hContentType));
			MEMSET((void*)decdata->message.hFrom,0,sizeof(decdata->message.hFrom));
			MEMSET((void*)decdata->message.hMessageID,0,sizeof(decdata->message.hMessageID));
			MEMSET((void*)decdata->message.hSubject,0,sizeof(decdata->message.hSubject));
			MEMSET((void*)decdata->message.hTransactionID,0,sizeof(decdata->message.hTransactionID));
			MEMSET((void*)decdata->message.hTo,0,sizeof(decdata->message.hTo));
			decdata->message.hBody = NULL;
			decdata->message.iBodyLen = 0;
			decdata->message.iRetrieveStatus = 0x80;
			decdata->message.iDate = 0;
			decdata->message.iPriority       = MMS_PRIORITY_NORMAL;
			decdata->message.iMessageClass   = MMS_CLASS_PERSONAL;
			break;
			
		case 0x86:
			*ePDUType = MMS_PDU_DELIVERY_IND;
			break;
			
		case 0x88:
			*ePDUType = MMS_PDU_READ_ORIG_IND;
			break;
			
		default:
			return MMC_PDU_TYPE;
			break;
	}
	
	if (*ePDUType == 0)
	{
	    DBGPRINTF("[WMS_MMS_PDU_Decode] *ePDUType == 0");
		return MMC_GENERIC;
	}
	else
	{
	    WMS_MMS_PDU_Decode_BuffClean(decdata,*ePDUType);
	}

	i = 2;
	while( i < datalen)
	{
	    DBGPRINTF("ptr[i] = 0x%x",ptr[i]);
		switch(ptr[i])
		{
			case 0x83:/* content-location */
			{
				i++;
				pchar = &ptr[i];
				len = i;
				for(;i<datalen;i++)
				{
					if (ptr[i] == 0)
						break;
				}
				
				len = i - len;
				if (*ePDUType == MMS_PDU_NOTIFICATION_IND)
				{
					if (len > MMS_MAX_CONTENT_LOCATION)
					{
						DBGPRINTF("len > MMS_MAX_CONTENT_LOCATION");
						return MMC_GENERIC;
					}
					
					STRNCPY((char*)decdata->notification.hContentLocation,(const char *)pchar,len);
					decdata->notification.hContentLocation[len] = 0;
					DBGPRINTF("MMS_PDU_Decode 0x83 content-location:%s",decdata->notification.hContentLocation);
				}
				break;
			}

			case 0x84:/* content-type */
			{
				i++;
				if (*ePDUType == MMS_PDU_RETRIEVE_CONF)
				{
					if (decdata->message.iRetrieveStatus == 0x80)
					{
						int decret;
						decret = WMS_MMS_WSP_DecodeMessage(&ptr[i], datalen-i,&decdata->message.mms_data);
						if (decret == MMC_OK)
							return ret;
						else
							return decret;
					}
					else
						return ret;
				}
				else
				{				    
					return MMC_OK;
				}
				break;
			}

			case 0x85:/* date */
			{
				i++;
				i += MMS_WSP_DecodeLongInteger(&ptr[i],datalen-i,&len);
				i--;
				switch(*ePDUType)
				{
					case MMS_PDU_RETRIEVE_CONF:
						decdata->message.iDate = len;
						break;
					case MMS_PDU_NOTIFICATION_IND:
						decdata->notification.iDate = len;
						break;
					case MMS_PDU_DELIVERY_IND:
						decdata->delrep.iDate = len;
						break;
				}
				break;
			}

			case 0x88:/* expires */
			{
				i++;
				len = MMS_WSP_GetValueLen(&ptr[i],datalen-i,&iDataOffset);
				if (len == MMS_DECODER_ERROR_VALUE)
				{
					DBGPRINTF("len == MMS_DECODER_ERROR_VALUE");
					return MMC_GENERIC;
				}

				switch(*ePDUType)
				{
					case MMS_PDU_NOTIFICATION_IND:
						decdata->notification.iExpires = MMS_WSP_GetExpiry(&ptr[i+iDataOffset],len,0);
						break;
				}
				
				i += (iDataOffset + len);
				i--;
				break;
			}
            case 0x80:
			case 0x89:/*from*/
			{
				i++;
				len = MMS_WSP_GetValueLen(&ptr[i],datalen-i,&iDataOffset);
				if (len == MMS_DECODER_ERROR_VALUE)
				{
					DBGPRINTF("len == MMS_DECODER_ERROR_VALUE");
					return MMC_GENERIC;
				}
				i += iDataOffset;
                if (*ePDUType == MMS_PDU_RETRIEVE_CONF)
				    STRNCPY((char*)decdata->message.hFrom,(char*)&ptr[i + 1],len - 1);//Address-present-token | Insert-address-token

                if (*ePDUType == MMS_PDU_NOTIFICATION_IND)//彩信通知短信里带的发件人的电话号码
				    STRNCPY((char*)decdata->notification.hFrom,(char*)&ptr[i + 1],len - 1);//Address-present-token | Insert-address-token
				    
				j = i;
				i += len;
				i--;
				break;
			}

			case 0x8A:/* X-Mms-Message-Class	*/
			{
				i++;
				if (*ePDUType == MMS_PDU_RETRIEVE_CONF) 
				{
				    //
					MMS_PDU_PutMessageClass(ptr[i], &decdata->message.iMessageClass);
				}
				else if (*ePDUType == MMS_PDU_NOTIFICATION_IND) 
				{
					MMS_PDU_PutMessageClass(ptr[i], &decdata->notification.iMessageClass);
				}
				
				break;
			}

			case 0x8b:/* messageid */
			{
			    uint8* pMessageID;
				i++;
				pchar = &ptr[i];
				len = i;
				for(;i<datalen;i++)
				{
					if (ptr[i] == 0)
						break;
				}
				len = i - len;
				if ((*ePDUType == MMS_PDU_RETRIEVE_CONF) 
					|| (*ePDUType == MMS_PDU_SEND_CONF)
					|| (*ePDUType == MMS_PDU_DELIVERY_IND)
					|| (*ePDUType == MMS_PDU_READ_ORIG_IND))
				{
					if(MMS_MAX_MESSAGEID_CHARSIZE < len)
					{
						DBGPRINTF("MMS_MAX_MESSAGEID_CHARSIZE < len");
						return MMC_GENERIC;
					}

					switch(*ePDUType)
					{
						case MMS_PDU_RETRIEVE_CONF:
						{
						    pMessageID = decdata->message.hMessageID;
						}
						break;
						case MMS_PDU_SEND_CONF:
						{
						    pMessageID = decdata->sendconf.hMessageID;
						}
						break;
						case MMS_PDU_DELIVERY_IND:
						{
						    pMessageID = decdata->delrep.hMessageID;
						}
						break;
						case MMS_PDU_READ_ORIG_IND:
						{
						
						    pMessageID = decdata->readrep.hMessageID;							
						}
						break;
							
						default:
							break;
					}
					STRNCPY((char*)pMessageID,(char*)pchar,len);
					pMessageID[len] = 0;
		
				}
				break;
			}

			case 0x8e:/* X-Mms-Message-Size	*/
			{
				i++;
				i += MMS_WSP_DecodeLongInteger(&ptr[i],datalen-i,&len);
				i--;
				switch(*ePDUType)
				{
					case MMS_PDU_NOTIFICATION_IND:
						decdata->notification.iSize = len;
						break;
				}
				break;
			}

			case 0x8F:/* X-Mms-Priority	*/
			{
				i++;
				if (*ePDUType == MMS_PDU_RETRIEVE_CONF) 
				{
					MMS_PDU_PutPriority(ptr[i], &decdata->message.iPriority);
				}
				else if (*ePDUType == MMS_PDU_NOTIFICATION_IND) 
				{
					MMS_PDU_PutPriority(ptr[i], &decdata->notification.iPriority);
				}
				break;
			}

			case 0x86:/*X-Mms-Delivery-Report*/
			{
				i++;
				if (*ePDUType == MMS_PDU_RETRIEVE_CONF) 
				{
					MMS_PDU_PutDeliveryReport(ptr[i], &decdata->message.bDelRep);
				}
// YY!:
/* ERROR!				
				else if (*ePDUType == MMS_PDU_NOTIFICATION_IND) 
				{
					MMS_PDU_PutDeliveryReport(ptr[i], &decdata->notification.iDeliveryReport);
				}
*/				
				break;
			}
            case 0x90:/* Read-Reply */
            {
                i++;
                if (*ePDUType == MMS_PDU_RETRIEVE_CONF) 
				{
					MMS_PDU_PutDeliveryReport(ptr[i], &decdata->message.bReadRep);
				}
            }
            break;
			case 0x92:/* response status */
			{
				i++;
				if (*ePDUType == MMS_PDU_SEND_CONF) 
				{
					decdata->sendconf.iResponseStatus = (int)ptr[i];
				}
				
				if(ptr[i] == 0x00)
				{
					i++;
				}
				break;
			}

			case 0x95:/* delrep status */
			{
				i++;
				if (*ePDUType == MMS_PDU_DELIVERY_IND) 
				{
					MMS_PDU_PutDeliveryReportStatus((int)ptr[i], &decdata->delrep.iStatus);
				}
				
				if(ptr[i] == 0x00)
				{
					i++;
				}
				break;
			}
            case 0x97: /* to */
            {
                i++;
				len = MMS_WSP_GetValueLen(&ptr[i],datalen-i,&iDataOffset);
				if (len == MMS_DECODER_ERROR_VALUE)
				{
					DBGPRINTF("len == MMS_DECODER_ERROR_VALUE");
					return MMC_GENERIC;
				}
				i += iDataOffset;
                if (*ePDUType == MMS_PDU_DELIVERY_IND)
                {
    				STRNCPY((char*)decdata->delrep.hTo,(char*)&ptr[i],len);
                }
                else if (*ePDUType == MMS_PDU_RETRIEVE_CONF)
                {
                    STRNCPY((char*)decdata->message.hTo,(char*)&ptr[i],len);
                }
                    
				j = i;
				i += len;
				i--;
				break;
            }
            break;
            
			case 0x99:/* retrieve status */
			{
				i++;
				if (*ePDUType == MMS_PDU_RETRIEVE_CONF) 
				{
					decdata->message.iRetrieveStatus = (int)ptr[i];
				}
				if(ptr[i] == 0x00)
				{
					i++;
				}
				break;
			}

			case 0x96:/*subject*/
			{							
				if ((*ePDUType == MMS_PDU_RETRIEVE_CONF) || (*ePDUType == MMS_PDU_NOTIFICATION_IND))
				{
					switch(*ePDUType)
					{
						case MMS_PDU_RETRIEVE_CONF:
							i++;
							len = MMS_PDU_DecodeEncodedString(&ptr[i],datalen-i,*ePDUType,(uint8*)&decdata->message.hSubject);
							if (len == MMS_DECODER_ERROR_VALUE)
							{
								DBGPRINTF("len == MMS_DECODER_ERROR_VALUE");
								return MMC_GENERIC;
							}
							i += len;
							i--;

							DBGPRINTF("MMS_PDU_Decode 0x96 subject:%s",decdata->notification.hSubject);
							break;
							
						case MMS_PDU_NOTIFICATION_IND:
							i++;
							len = MMS_PDU_DecodeEncodedString(&ptr[i],datalen-i,*ePDUType,(uint8*)&decdata->notification.hSubject);
							if (len == MMS_DECODER_ERROR_VALUE)
							{
								DBGPRINTF("len == MMS_DECODER_ERROR_VALUE");
								return MMC_GENERIC;
							}
							i += len;
							i--;

							DBGPRINTF("MMS_PDU_Decode 0x96 subject:%s",decdata->notification.hSubject);
							break;
						default:
							break;
					}				
				}
			}
			break;

			case 0x98:/* transactionid */
			{
				i++;
				pchar = &ptr[i];
				len = i;
				for(;i<datalen;i++)
				{
					if (ptr[i] == 0)
						break;
				}
				
				len = i - len;
				if ((*ePDUType == MMS_PDU_RETRIEVE_CONF) 
					|| (*ePDUType == MMS_PDU_NOTIFICATION_IND)
					|| (*ePDUType == MMS_PDU_SEND_CONF))
				{
					if (len > MMS_MAX_TRANSACTION_ID_SIZE)
						return MMC_TRANSACTION_ID;
					
					switch(*ePDUType)
					{
						case MMS_PDU_RETRIEVE_CONF:
							STRNCPY((char*)decdata->message.hTransactionID,(const char *)pchar,len);
							decdata->message.hTransactionID[len] = 0;
							break;
							
						case MMS_PDU_NOTIFICATION_IND:
							STRNCPY((char*)decdata->notification.hTransactionID,(const char *)pchar,len);
							decdata->notification.hTransactionID[len] = 0;
							break;
							
						case MMS_PDU_SEND_CONF:
							STRNCPY((char*)decdata->sendconf.hTransactionID,(const char *)pchar,len);
							decdata->sendconf.hTransactionID[len] = 0;
							break;
					}
				}
				break;
			}

			default:
			{
				i++;
				if( i == datalen )
				{
					break ;
				}
				len = MMS_WSP_GetValueLen(&ptr[i],datalen-i,&iDataOffset);
				if (len == MMS_DECODER_ERROR_VALUE)
				{
					DBGPRINTF("len == MMS_DECODER_ERROR_VALUE!!");
					return MMC_GENERIC;
				}
				i += (iDataOffset + len);
				i--;
				break;
			}
		}

		i++;
	}
	return MMC_OK;
}

int MMS_WSP_Decode_UINTVAR(uint8* pData, int iDataLen, int* iUintvarLen)
{
	int ret = 0,i=0;
	boolean completed = FALSE;
	*iUintvarLen = 0;
	for(;i<iDataLen;i++)
	{
		ret = ret << 7;
		ret += (pData[i] & 0x7f);
		(*iUintvarLen)++;
		if (pData[i] < 128)
		{
			completed = TRUE;
			break;
		}
	}
	if (completed == FALSE)
		return MMS_DECODER_ERROR_VALUE;
	else
		return ret;
}

int MMS_WSP_GetValueLen(uint8* pData, int iDataLen, int* iDataOffset)
{
	int ret, i = 0;
	boolean completed = FALSE;

	if (iDataLen == 0)
		return MMS_DECODER_ERROR_VALUE;
	if ((pData[i] >= 32) && (pData[i] <= 127))
	{
		*iDataOffset = 0;
		for(;i<iDataLen;i++)
		{
			if (pData[i] == 0)
			{
				i++;
				completed = TRUE;
				break;
			}
		}
	}
	else if (pData[i] == 31)
	{
		ret = MMS_WSP_Decode_UINTVAR(&pData[i+1], iDataLen-1, iDataOffset);
		(*iDataOffset)++;
		return ret;
	}
	else if (pData[i] >= 128)
	{
		*iDataOffset = 0;
		i = 1;
		completed = TRUE;
	}
	else 
	{
		*iDataOffset = 1;
		i += pData[i];
		completed = TRUE;
	}
	if (completed == FALSE)
		return MMS_DECODER_ERROR_VALUE;
	else
		return i;
}

int MMS_WSP_DecodeLongInteger(uint8* buf, int iDataLen, int* iValue)
{
	int iDataOffset,ret,i;
	ret = MMS_WSP_GetValueLen(buf,iDataLen, &iDataOffset);
	if (ret == MMS_DECODER_ERROR_VALUE)
		return ret;
	*iValue = 0;
	for(i=iDataOffset;i<ret+iDataOffset;i++)
	{
		if (i < iDataLen)
		{
			int prev_iValue = *iValue;
			if (*iValue > MMS_INT_MAX/256)
			{
				*iValue = MMS_INT_MAX;
			}
			else
			{
				*iValue = (*iValue) << 8;
				*iValue += buf[i];
			}
			if (*iValue < prev_iValue)
				*iValue = MMS_INT_MAX;
			if (*iValue < 0)
				*iValue = 0;
			if (*iValue > MMS_INT_MAX)
				*iValue = MMS_INT_MAX;
		}
		else
			return MMS_DECODER_ERROR_VALUE;
	}

	return ret+iDataOffset;
}

int MMS_WSP_GetExpiry(uint8* buf, int iDataLen, int curtime)
{
	int ret,val = 0;
	if (iDataLen == 0)
			return MMS_DECODER_ERROR_VALUE;
	if (iDataLen > 1)
	{
		ret = MMS_WSP_DecodeLongInteger(&buf[1],iDataLen-1,&val);
		if (ret == MMS_DECODER_ERROR_VALUE)
			return ret;
	}
	if (buf[0] == 128)
		ret = val;
	if (buf[0] == 129)
		ret = val+curtime;

	return ret;
}

int MMS_PDU_PutMessageClass(int in_MessageClass, int* out_MessageClass)
{
	switch( in_MessageClass )
	{
		case 128:
			*out_MessageClass = MMS_CLASS_PERSONAL;
			break;

		case 129:
			*out_MessageClass = MMS_CLASS_ADVERTISEMENT;
			break;

		case 130:
			*out_MessageClass = MMS_CLASS_IMFORMATIONAL;
			break;

		case 131:
			*out_MessageClass = MMS_CLASS_AUTO;
			break;
			
		default:
			*out_MessageClass = MMS_CLASS_PERSONAL;
			break;
	}
	return SUCCESS;
}

int MMS_PDU_PutPriority(int in_priority, int* out_priority)
{
	switch( in_priority )
	{
		case 128:
			*out_priority = MMS_PRIORITY_LOW;
			break;
		case 129:
			*out_priority = MMS_PRIORITY_NORMAL;
			break;
		case 130:
			*out_priority = MMS_PRIORITY_HIGH;
			break;
		default:
			*out_priority = MMS_PRIORITY_NORMAL;
			break;
	}

	return SUCCESS;
}

int MMS_PDU_PutDeliveryReport(int in_DelRep, boolean* out_DelRep)
{
	*out_DelRep = ((in_DelRep == 128) ? MMS_DELIVERYREP_YES:MMS_DELIVERYREP_NO);

	return SUCCESS;
}

int MMS_PDU_PutDeliveryReportStatus(int in_Status, int* out_Status)
{
	if (out_Status != NULL)
	{
		*out_Status = in_Status -127;
		return SUCCESS;
	}
	else
	{
		return EFAILED;
	}
/*
	Expired       = <Octet 128>:MMS_DELREP_EXPIRED = 0,
	Retrieved     = <Octet 129>:MMS_DELREP_RETRIEVED
	Rejected      = <Octet 130>:MMS_DELREP_REJECT
	Deferred      = <Octet 131>:MMS_DELREP_DEFFERED
	Unrecognised  = <Octet 132>:MMS_DELREP_UNRECOGNISED
	Indeterminate = <Octet 133>:MMS_DELREP_INDETERMINATE
	Forwarded     = <Octet 134>:MMS_DELREP_FORWARDED
	Unreachable   = <Octet 135>:MMS_DELREP_UNREACHABLE
*/
}

int MMS_PDU_DecodeEncodedString(uint8* ptr, int datalen,	uint8 ePDUType,	uint8* handle)
{
	int i = 0,j;
	boolean is_utf8 = FALSE;
	int charset_len = 0, charset = 0;
	uint8* pchar = NULL, *pcharset = NULL;
	int len = 0, iDataOffset, whole_len=0, ret;

	whole_len = MMS_WSP_GetValueLen(&ptr[i],datalen-i,&iDataOffset);
	MMS_DEBUG(("MMS_PDU_DecodeEncodedString whole_len:%d",whole_len));
	if (whole_len == MMS_DECODER_ERROR_VALUE)
		return MMS_DECODER_ERROR_VALUE;
		
	ret = whole_len+iDataOffset;
	i += iDataOffset;
	MMS_DEBUG(("MMS_PDU_DecodeEncodedString iDataOffset:%d",iDataOffset));
	if (iDataOffset)
	{
		charset_len = MMS_WSP_GetValueLen(&ptr[i],whole_len-i,&iDataOffset);
		if (charset_len == MMS_DECODER_ERROR_VALUE)
			return MMS_DECODER_ERROR_VALUE;
			
		i += iDataOffset;
		pchar = &ptr[i+charset_len];
		len = whole_len - charset_len - 1;
		pcharset = &ptr[i];
		if ((charset_len == 1) && (iDataOffset == 0))
		{
			charset = pcharset[0]-0x80;
			if (charset == 0xea)
			{
				is_utf8 = TRUE;
			}
		}
		else
		{
			for(j=0;j<charset_len;j++)
			{
				charset = charset << 8;
				charset += pcharset[j];
			}

			return ret;
		}
	}
	else
	{
		pchar = ptr;
		len = whole_len;
	}
	
	
	if (*pchar == 127)
	{
		if (len)
			len--;
		pchar = &pchar[1];
	}

	if (handle != NULL)
	{
		STRNCPY((char*)handle,(const char*)pchar,len);
		handle[len] = 0;
	}
	
	return ret;
}

boolean MMS_GetProxySettings(boolean *bUseProxy,char* hProxyHost, int* iProxyPort)
{
	IConfig             *m_pConfig;
	int 				nRet;
	CFG_MMsInfo         cfg_mmsinfo = {0};
	
	nRet = ISHELL_CreateInstance(AEE_GetShell(),AEECLSID_CONFIG, (void **)&m_pConfig);

	if ( nRet == SUCCESS )
	{
		(void)ICONFIG_GetItem(m_pConfig,CFGI_MMS_SETTING,&cfg_mmsinfo,sizeof(CFG_MMsInfo));

		*bUseProxy = cfg_mmsinfo.bUseProxy;
		*iProxyPort = cfg_mmsinfo.iProxyPort;
		MEMCPY(hProxyHost,cfg_mmsinfo.hProxyHost,sizeof(cfg_mmsinfo.hProxyHost));

		(void)ICONFIG_Release(m_pConfig);
        m_pConfig = NULL;
		return TRUE;
	}

	return FALSE;
}

boolean MMS_SetProxySettings(boolean bUseProxy,char* hProxyHost, int iProxyPort)
{
	IConfig             *m_pConfig;
	int 				nRet;
	CFG_MMsInfo         cfg_mmsinfo = {0};
	
	nRet = ISHELL_CreateInstance(AEE_GetShell(),AEECLSID_CONFIG, (void **)&m_pConfig);

	if ( nRet == SUCCESS )
	{

		cfg_mmsinfo.bUseProxy = bUseProxy;
		cfg_mmsinfo.iProxyPort = iProxyPort;
		MEMCPY(cfg_mmsinfo.hProxyHost,hProxyHost,sizeof(cfg_mmsinfo.hProxyHost));
		
		(void)ICONFIG_SetItem(m_pConfig,CFGI_MMS_SETTING,&cfg_mmsinfo,sizeof(CFG_MMsInfo));

		(void)ICONFIG_Release(m_pConfig);
        m_pConfig = NULL;
		return TRUE;
	}

	return FALSE;
}

boolean  MMSSocketNew (MMSSocket **pps, uint16 nType)
{
	ISocket* pISocket = NULL;
	INetMgr *pINetMgr = NULL;
	MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketNew Enter!"));
        
    OEM_SetMMS_ADSAccount();
    
	if(pINetMgr == NULL)
	{
		if(ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_NET, (void**)&pINetMgr) != SUCCESS)
			return FALSE;
	}
	INETMGR_SelectNetwork(pINetMgr,0);

	pISocket = INETMGR_OpenSocket(pINetMgr, nType);
	
	if(pISocket != NULL)
	{
		int result = 0;
		int OptionValue = 0;

		*pps = MALLOC(sizeof(MMSSocket));
		if(*pps == NULL)
		{
			return FALSE;
		}
        MEMSET(*pps,NULL,sizeof(MMSSocket));
		(*pps)->pISocket = pISocket;
        (*pps)->bConnected = FALSE;
        RELEASEIF(pINetMgr);
		MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketNew Success!"));
		return TRUE;
	}
    RELEASEIF(pINetMgr);
	MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketNew Failed!"));
	return FALSE;
}

boolean  MMSSocketClose (MMSSocket **pps)
{
	MMSSocket* pSocketInfo = *pps;
	MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketClose Enter!"));
	
	CALLBACK_Cancel(&pSocketInfo->DNSCallback);//cancel DNS callback
	//ISHELL_CancelTimer(AEE_GetShell(), (PFNNOTIFY)SocketConnect_OnTimeout, (void*)pSocketInfo);
	
	if(pSocketInfo->pISocket != NULL)
	{
		ISOCKET_Cancel(pSocketInfo->pISocket, NULL, NULL);
		ISOCKET_Close(pSocketInfo->pISocket);
		(void)IBASE_Release((IBase*)pSocketInfo->pISocket);
		pSocketInfo->pISocket = NULL;
	}
	FREEIF(pSocketInfo->pSendData);
	FREEIF(pSocketInfo);
	WMS_SOCKET_UNLOK();
	MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketClose Exit!"));
	return TRUE;
}


static void ConnectError(void* pDdata, int nError)
{
	MMSSocket* pUser = (MMSSocket*)pDdata;
	MMS_DEBUG(("[MSG][ConnectError]:ENTER"));
	if ( pUser == NULL )
	{
	    MMS_DEBUG(("[MSG][ConnectError]:pUser == NULL"));
		return;
	}
	
	switch(nError)
	{
		case AEE_NET_SUCCESS:
		{
			MSG_FATAL("ConnectError AEE_NET_SUCCESS",0,0,0);
			pUser->bConnected = TRUE;
			MMSSocketState(pUser);
			//ISOCKET_Readable(pUser->pISocket, SocketReadableCB, pUser);	
	    }
		break;		
		/* 若超时错误, 则 ... */
		case AEE_NET_ETIMEDOUT:
		{
		    MSG_FATAL("ConnectError AEE_NET_ETIMEDOUT",0,0,0);
		    ISHELL_PostEventEx(AEE_GetShell(),
                                         EVTFLG_ASYNC, 
                                         AEECLSID_WMSAPP, 
                                         EVT_MMS_MSG_SEND_FINSH,
                                         FALSE, 
                                         0);
            pUser->bConnected = FALSE;
            MMSSocketClose(&pUser);
		}
		break;

        case AEE_NET_ERROR:
		case AEE_NET_ECONNREFUSED:
		{
			MSG_FATAL("ConnectError AEE_NET_ECONNREFUSED",0,0,0);
			ISHELL_PostEventEx(AEE_GetShell(),
                                         EVTFLG_ASYNC, 
                                         AEECLSID_WMSAPP, 
                                         EVT_MMS_MSG_SEND_FINSH,
                                         FALSE, 
                                         0);
            pUser->bConnected = FALSE;
            MMSSocketClose(&pUser);
		}
		break; 
			
		default:
		{
		    IFileMgr *pIFileMgr = NULL;
		    int result = 0;
            uint32 pdwTotal = 0;
            uint32 pdwFree = 0;
			result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
			if (SUCCESS != result)
		    {
				MSG_FATAL("MRS: Open file error %x", result,0,0);
				return;
		    }            
		    MSG_FATAL("ConnectError error: 0x%x",nError,0,0);
    		pdwFree = IFILEMGR_GetFreeSpace(pIFileMgr, &pdwTotal);    
            MSG_FATAL("IFILEMGR_GetFreeSpace pdwFree=%d, pdwTotal=%d",pdwFree, pdwTotal, 0);
            IFILEMGR_Release(pIFileMgr);
		    ISHELL_PostEventEx(AEE_GetShell(),
                                         EVTFLG_ASYNC, 
                                         AEECLSID_WMSAPP, 
                                         EVT_MMS_MSG_SEND_FINSH,
                                         FALSE, 
                                         0);
            pUser->bConnected = FALSE;
            MMSSocketClose(&pUser);
		}
		break; 
	}
}


boolean  MMSSocketConnect (MMSSocket *ps, char *pszServer, uint16 nPort)
{
	INAddr mAddress = 0;
	INPort mPort = 0;
	MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketConnect Enter pszServer = %s,nPort = %d!",pszServer,nPort));

#ifdef MMS_TEST
    if(pszServer == NULL || !STRLEN(pszServer))
        pszServer = "10.0.0.200";
    if(nPort == 0)
        nPort = 80;
#endif
	if (!INET_ATON(pszServer,&mAddress))
	{
		MSG_FATAL("INET_ATON failed!", 0,0,0);
		return FALSE;
	}

    MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketConnect Enter 1"));
	ISOCKET_Connect(ps->pISocket, mAddress, HTONS((INPort)nPort), ConnectError, (void*)ps);
	MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketConnect Enter 2"));
	return TRUE;
}

boolean MMSSocket_MRetrieveConf(MMSSocket *ps)
{
    uint32 nDataLen = 0;
    char pTempChar[100] = {0};
    
    MMS_DEBUG(("[MSG][DeviceSocket]: MMSSocket_PDUResult Enter!"));
    
    if(NULL == ps->pISocket
        || !ps->bConnected)
    {
        return FALSE;
    }

    if(ps->RecCount)
    {
        if(ps->NoDataCount > 3)
        {   
            char* pCur = NULL;
            if(STRSTR((char*)ps->RecBuffer," 200 "))
            {
                ps->pOData = (uint8*)WMS_MMS_BUFFERGet();
                WMS_MMS_BUFFERReset();
                MMSSocketRecv(ps,ps->pOData,&ps->nODataLen);
            }
            else if(pCur = STRISTR((char*)ps->RecBuffer,"HTTP/1"))
            {
                STRNCPY(pTempChar,(char*)ps->RecBuffer,STRCHR(pCur + 10,' ') - pCur - 10);
                ps->nError = ATOI(pTempChar + 3);//HTTP/1.1 400 Bad Request
            }
            
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    ISOCKET_Readable(ps->pISocket,SocketReadableCB,ps);

    return FALSE;
}

boolean MMSSocket_HTTPGETReq(MMSSocket *ps,char* pAddress)
{
    int nDataLen = 0;
    char* pStrINVOKE = "GET %s HTTP/1.1\r\n\r\n";
    char chrFixedAddr[256] = {0};
    char* pFixedAddr = NULL;

    MMS_DEBUG(("[MSG][DeviceSocket]: MMSSocket_PDUInvoke Enter!"));
    
    if(NULL == ps->pISocket
        || !ps->bConnected)
    {
        return FALSE;
    }

    if(ps->nDataLen != 0)
    {
        if(ps->nBytesSent == ps->nDataLen)
        {
            FREEIF(ps->pSendData);
		    ps->nDataLen = 0;
		    ps->nBytesSent = 0;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    if(pAddress == NULL
        || STRLEN(pAddress) == 0)
        return FALSE;

    MMS_DEBUG(("[MSG][DeviceSocket]: MMSSocket_PDUInvoke pFixedAddr:%s",pAddress));    
    pFixedAddr = STRSTR(pAddress,"http:");
    if(pFixedAddr == NULL)
        pFixedAddr = pAddress;
        
    nDataLen = STRLEN(pStrINVOKE) - 2 + STRLEN(pFixedAddr);
    if(nDataLen > sizeof(chrFixedAddr))
    {
        MMS_DEBUG(("[MSG][DeviceSocket]: MMSSocket_PDUInvoke ERROR!"));
        return FALSE;
    }
    SPRINTF((char*)&chrFixedAddr,pStrINVOKE,pFixedAddr);
    
    MMS_DEBUG(("[MSG][DeviceSocket]: MMSSocket_PDUInvoke pFixedAddr:%s nDataLen:%d",pFixedAddr,nDataLen));
    MMSSocketSend(ps,(const uint8*)&chrFixedAddr,nDataLen);
    
    return FALSE;
}

boolean MMSSocket_FINISH_GETMSG(MMSSocket *ps)
{
    uint32 len = 0;
    char pTempChar[10] = {0};
    MMS_DEBUG(("[MSG][DeviceSocket]: MMSSocket_FINISH_GETMSG Enter!"));
    if(NULL == ps->pISocket
        || !ps->bConnected)
    {
        return FALSE;
    }

    MMS_DEBUG(("[MSG][DeviceSocket]: MMSSocket_FINISH_GETMSG ps->pOData=%s",ps->pOData));
    MSG_FATAL("[MSG][DeviceSocket]: MMSSocket_FINISH_GETMSG RecBuffer len=%d", STRLEN((char*)ps->RecBuffer),0,0);
    if(ps->RecCount!= 0 
        && STRLEN((char*)ps->RecBuffer))
    {
        if(STRISTR((char*)ps->RecBuffer," 200 "))
        {
            return TRUE;
        }
        else if(ps->NoDataCount != 0)
        {
            char* pCur = NULL;
            if(pCur = STRISTR((char*)ps->RecBuffer,"HTTP/1"))
            {
                STRNCPY(pTempChar,(char*)ps->RecBuffer,STRCHR(pCur + 10,' ') - pCur - 10);
                ps->nError = ATOI(pTempChar + 3);//HTTP/1.1 400 Bad Request
            }
            MEMSET(ps->RecBuffer,NULL,MSG_MAX_PACKET_SIZE);
            ps->RecCount = 0;
            
            return TRUE;
        }
    }
    if((ps->RecCount > 3) && (STRLEN((char*)ps->RecBuffer) == 0))
    {
        char* pCur = NULL;
        if(pCur = STRISTR((char*)ps->RecBuffer,"HTTP/1"))
        {
            STRNCPY(pTempChar,(char*)ps->RecBuffer,STRCHR(pCur + 10,' ') - pCur - 10);
            ps->nError = AEE_NET_ENETNONET;//HTTP/1.1 400 Bad Request
        }
        MEMSET(ps->RecBuffer,NULL,MSG_MAX_PACKET_SIZE);
        ps->RecCount = 0;
        
        return TRUE;        
    }

    ISOCKET_Readable(ps->pISocket,SocketReadableCB,ps);
    
}
static uint32 WMS_MMS_EncodePostHead(uint8* pBuf,uint8* pContentBuf,uint32 nContentLen)
{
    int nHeaderLen = 0;
    if(pBuf && pContentBuf && nContentLen != 0)
    {
        IConfig             *pConfig;
        char serverAddress[MAX_URL_LEN+1] = {0};
        IShell  *pShell = AEE_GetShell();
        if (ISHELL_CreateInstance(pShell, AEECLSID_CONFIG,(void **)&pConfig) != SUCCESS)
        {
            return ;
        }
        ICONFIG_GetItem(pConfig, CFGI_MMS_SERVER_ADDRESS, serverAddress, sizeof(serverAddress));
        (void)ICONFIG_Release(pConfig);
        pConfig = NULL;      
        
        nHeaderLen = SPRINTF((char*)pBuf,POST_TEST, serverAddress, serverAddress, nContentLen);
        DBGPRINTF(("POST_TEST:%s",(char*)pBuf));
        MEMCPY(pBuf + nHeaderLen,pContentBuf,nContentLen);
    }

    return nContentLen + nHeaderLen;
}
boolean WMS_MMS_Resend(int nIndex,int nKind)
{
    return WMS_MMSState(WMS_MMS_PDU_WSPHTTPRESEND,nKind,nIndex);
}
typedef struct
{
    int nState;
    int16 wParam;
    uint32 dwParam;
}WMS_MMS_SOCKET_PARAM;

void WMS_MMSWaitStack(void *pShell)
{
    if(IVector_IsEmpty(pSocketParam))
    {
        ISHELL_CancelTimer(pShell,&WMS_MMSWaitStack,pShell);
        return;
    }    
        
    if(!WMS_GETSOCKET_STATUS())
    {
        WMS_MMS_SOCKET_PARAM *pParam = NULL;
        pParam = IVector_ElementAt(pSocketParam,0);  
        WMS_MMSState(pParam->nState,pParam->wParam,pParam->dwParam);
        MMS_DEBUG(("[MSG][WMS_MMSState]: pParam->nState:%d,wParam:%d,dwParam:%d!"));
        IVector_RemoveElementAt(pSocketParam,0);
        FREEIF(pParam);
    }
    
    ISHELL_SetTimer(pShell,2000,&WMS_MMSWaitStack,pShell);
}
void WMS_MMS_RELEASE()
{    
    if(pSocketParam != NULL)
    {
        IVector_RemoveAllElements(pSocketParam);
        RELEASEIF(pSocketParam);
    }
    WMS_MMS_BUFFERRelease();
}

boolean WMS_MMSState(int nState,int16 wParam,uint32 dwParam)
{
    MMSSocket *pMMSSocket = NULL;
    boolean isLocked = FALSE;

    MSG_FATAL("[MSG][WMS_MMSState]: Enter!",0,0, 0);
    if(!WMS_SOCKET_LOCK())
    {
        WMS_MMS_SOCKET_PARAM* pParam = NULL;
        int i = 0;
        MMS_DEBUG(("[MSG][WMS_MMSState]: LOCK!"));
        if(pSocketParam == NULL)
        {
            ISHELL_CreateInstance(AEE_GetShell(),AEECLSID_VECTOR,(void**)&pSocketParam);
            IVector_Init(pSocketParam,20);
        }

        while(pParam = IVector_ElementAt(pSocketParam,i))
        {
            if((pParam->dwParam == dwParam) 
                && (pParam->nState == nState) 
                && (pParam->wParam == wParam))
            {
                return TRUE;
            }
            i++;
        }
        
        pParam = (WMS_MMS_SOCKET_PARAM*)MALLOC(sizeof(WMS_MMS_SOCKET_PARAM));
        pParam->dwParam = dwParam;
        pParam->nState = nState;
        pParam->wParam = wParam;
        bCloseSocket = FALSE; 
        IVector_AddElement(pSocketParam,pParam);
        ISHELL_SetTimer(AEE_GetShell(),2000,&WMS_MMSWaitStack,AEE_GetShell());
        return TRUE;
    }
    MMSSocketNew(&pMMSSocket,AEE_SOCK_STREAM);
    if(pMMSSocket == NULL)
        return FALSE;
       
    pMMSSocket->nState = nState;
    pMMSSocket->wParam = wParam;
    pMMSSocket->dwParam = dwParam;
    bCloseSocket = FALSE;     
    MMSSocketState(pMMSSocket);
    return TRUE;
}

static void MMSSocketState(MMSSocket *ps)
{

    MSG_FATAL("[MSG][DeviceSocket]: MMSSocketState Enter! bConnected=%d",ps->bConnected,0,0);
    
    if((!ps->bConnected) && (ps->nState != WMS_MMS_PDU_DRAFT) && (ps->nState != WMS_MMS_PDU_MDeliveryInd) )
    {
        IConfig             *pConfig;
        char Proxy[MAX_MMS_PROXY] = {0};
        char port[MAX_MMS_PORT] = {0};
        uint16 portID = 0;
        IShell  *pShell = AEE_GetShell();
        if (ISHELL_CreateInstance(pShell, AEECLSID_CONFIG,(void **)&pConfig) == SUCCESS)
        {
            ICONFIG_GetItem(pConfig, CFGI_MMS_PROXY, Proxy, sizeof(Proxy));
            ICONFIG_GetItem(pConfig, CFGI_MMS_PORT,  port,  sizeof(port));
            (void)ICONFIG_Release(pConfig);
            pConfig = NULL; 

            if((STRLEN(Proxy) != 0) && (STRLEN(port) != 0))
            {
                portID = (uint16)ATOI(port);
                DBGPRINTF("Proxy=%s, portID=%d",Proxy, portID);            
                MMSSocketConnect(ps,Proxy,portID);
            }
            else
            {
                //默认为南美的网关和端口号
                MMSSocketConnect(ps,"10.0.0.0",80);
            }
        }
        else
        {
                //默认为南美的网关和端口号
                MMSSocketConnect(ps,"10.0.0.0",80);            
        }
/*        
#ifdef FEATURE_USES_MMS_TEST        
        MMSSocketConnect(ps,"10.0.0.200",80);
#else
        MMSSocketConnect(ps,"10.0.0.0",80);
#endif*/
        return;
    }

    if((ISOCKET_GetLastError(ps->pISocket) != AEE_NET_SUCCESS
        && ISOCKET_GetLastError(ps->pISocket) != AEE_NET_EWOULDBLOCK))
    {
        MMS_DEBUG(("[MSG][DeviceSocket]: ISOCKET_GetLastError= 0x%x",ISOCKET_GetLastError(ps->pISocket)));
        MMSSocketClose(&ps);
        return ;
    }
    if(bCloseSocket)
    {
        MMS_DEBUG(("[MSG][DeviceSocket]:bCloseSocket = %d",bCloseSocket));
        MMSSocketClose(&ps);
        return ;
    }
    MMS_DEBUG(("[MSG][DeviceSocket]: ps->nState = %d",ps->nState));
    switch(ps->nState)
    {
        case WMS_MMS_PDU_MNotifyrespInd:
        {
            uint8* pBuf = NULL;
            uint32 nBufLen = 0;
            MMS_DEBUG(("[MSG][DeviceSocket]: nDataLen:%d",ps->nDataLen));
            if(0 != ps->nDataLen)
            {
                MMS_DEBUG(("[MSG][DeviceSocket]: nDataLen:%d",ps->nDataLen));
                if(ps->nBytesSent == ps->nDataLen)
                {
                    MMS_DEBUG(("[MSG][DeviceSocket]: nDataLen:%d",ps->nDataLen));
                    FREEIF(ps->pSendData);
        		    ps->nDataLen = 0;
        		    ps->nBytesSent = 0;
                    MMSSocketClose(&ps);
                    return;
                }
                break;
            }
            pBuf = (uint8*)MALLOC(SOCKET_BUFFER_SIZE);
            MMS_DEBUG(("[MSG][DeviceSocket]: pBuf:%d",pBuf));
            nBufLen = WMS_MMS_PDU_Encode((MMS_WSP_ENCODE_SEND*)ps->dwParam,pBuf,ps->nState);
            MMS_DEBUG(("[MSG][DeviceSocket]: nBufLen:%d",nBufLen));
            MMSSocketSend(ps,pBuf,nBufLen);
            FREEIF(((MMS_WSP_ENCODE_SEND*)ps->dwParam)->pNotifyresp);
            FREEIF(pBuf);
        }
        break;

        case WMS_MMS_PDU_DRAFT:
        {
            uint8* pBuf = NULL;
            uint32 nBufLen = 0;
            MSG_FATAL("MMSSocketState WMS_MMS_PDU_DRAFT",0,0,0);
            pBuf = (uint8*)MALLOC(MSG_MAX_PACKET_SIZE);
            nBufLen = WMS_MMS_PDU_Encode((MMS_WSP_ENCODE_SEND*)ps->dwParam,pBuf,WMS_MMS_PDU_DRAFT);
            MMSSocketClose(&ps);
            FREEIF(pBuf);
            return;
        }        

        case WMS_MMS_PDU_MDeliveryInd:
        {
            uint8* pBuf = NULL;
            uint32 nBufLen = 0;
            MSG_FATAL("MMSSocketState WMS_MMS_PDU_MDeliveryInd",0,0,0);
            pBuf = (uint8*)MALLOC(MSG_MAX_PACKET_SIZE);
            nBufLen = WMS_MMS_PDU_Encode((MMS_WSP_ENCODE_SEND*)ps->dwParam,pBuf,WMS_MMS_PDU_MDeliveryInd);
            MMSSocketClose(&ps);
            FREEIF(pBuf);
            return;
        }   

        case WMS_MMS_PDU_MReadRecInd:
        {
            uint8* pBuf = NULL;
            uint32 nBufLen = 0;
            if(ps->nDataLen != 0)
            {
                if(ps->nBytesSent == ps->nDataLen)
                {
                    FREEIF(ps->pSendData);
        		    ps->nDataLen = 0;
        		    ps->nBytesSent = 0;
                    MMSSocketClose(&ps);
                    return;
                }
                break;
            }
            pBuf = (uint8*)MALLOC(SOCKET_BUFFER_SIZE);
            nBufLen = WMS_MMS_PDU_Encode((MMS_WSP_ENCODE_SEND*)ps->dwParam,pBuf,ps->nState);
            MMSSocketSend(ps,pBuf,nBufLen);
            FREEIF(((MMS_WSP_ENCODE_SEND*)ps->dwParam)->pReadReport);
            FREEIF(pBuf);
        }
        break;
        case WMS_MMS_PDU_MAcknowledgeInd:
        {
            uint8* pBuf = NULL;
            uint32 nBufLen = 0;
            if(ps->nDataLen != 0)
            {
                if(ps->nBytesSent == ps->nDataLen)
                {
                    FREEIF(ps->pSendData);
        		    ps->nDataLen = 0;
        		    ps->nBytesSent = 0;
                    MMSSocketClose(&ps);
                    return;
                }
                break;
            }
            pBuf = (uint8*)MALLOC(SOCKET_BUFFER_SIZE);
            nBufLen = WMS_MMS_PDU_Encode((MMS_WSP_ENCODE_SEND*)ps->dwParam,pBuf,ps->nState);
            MMSSocketSend(ps,pBuf,nBufLen);
            FREEIF(((MMS_WSP_ENCODE_SEND*)ps->dwParam)->pDeliveryacknowledgement);
            FREEIF(pBuf);
        }
        break;
        
        case WMS_MMS_PDU_MSendConf:
        {
            if(MMSSocket_FINISH_GETMSG(ps))
            {

                MMS_DEBUG(("[MSG][DeviceSocket]: WMS_MMS_PDU_MSendConf:%d",ps->RecCount));
                if(ps->RecCount == 0)
                {
                    ISHELL_PostEventEx(AEE_GetShell(),
                                         EVTFLG_ASYNC, 
                                         AEECLSID_WMSAPP, 
                                         EVT_MMS_MSG_SEND_FINSH,
                                         ps->nError, 
                                         0);
                }
                else
                {
                    ISHELL_PostEventEx(
                        AEE_GetShell(),
                        EVTFLG_ASYNC,
                        AEECLSID_WMSAPP,
                        EVT_MMS_MSG_SEND_FINSH,
                        HTTP_CODE_OK,
                        0);
                }    
               MMSSocketClose(&ps);
               return ;
            }
        }
        break;

        case WMS_MMS_PDU_MSendReq:
        {
            uint8* pBuf = NULL;
            uint32 nBufLen = 0;

            if(ps->nDataLen != 0)
            {
                if(ps->nBytesSent == ps->nDataLen)
                {
                    FREEIF(ps->pSendData);
        		    ps->nDataLen = 0;
        		    ps->nBytesSent = 0;
                    ps->nState = WMS_MMS_PDU_MSendConf;
                }
                break;
            }
            pBuf = (uint8*)MALLOC(MSG_MAX_PACKET_SIZE);
            nBufLen = WMS_MMS_PDU_Encode((MMS_WSP_ENCODE_SEND*)ps->dwParam,pBuf,ps->nState);
            MMSSocketSend(ps,pBuf,nBufLen);
            FREEIF(((MMS_WSP_ENCODE_SEND*)ps->dwParam)->pMessage);
            FREEIF(pBuf);
        }
        break;
        case WMS_MMS_PDU_WSPHTTPRESEND:
        {
            uint8* pBuf = NULL;
            uint32 nBufLen = 0;
            uint8* pContentBuf = NULL;
            uint32 nContentLen = 0;

            if(ps->nDataLen != 0)
            {
                if(ps->nBytesSent == ps->nDataLen)
                {
                    FREEIF(ps->pSendData);
        		    ps->nDataLen = 0;
        		    ps->nBytesSent = 0;
                    ps->nState = WMS_MMS_PDU_MSendConf;
                }
                break;
            }
            pBuf = (uint8*)MALLOC(MSG_MAX_PACKET_SIZE);
            pContentBuf = WMS_MMS_ReadMMS(ps->dwParam,ps->wParam,&nContentLen);
            pContentBuf[1] = 0x80;
            nBufLen =  WMS_MMS_EncodePostHead(pBuf,pContentBuf,nContentLen);
            MMSSocketSend(ps,pBuf,nBufLen);
            FREEIF(pBuf);
        }
        break;
        
        case WMS_MMS_PDU_MRetrieveConf:
        {
            if(MMSSocket_MRetrieveConf(ps))
            {
                uint8* pData = (uint8*)STRSTR((const char*)ps->pOData,"\r\n\r\n") + 4;
                uint32 nDataLen = (ps->nODataLen - ((uint32)pData - (uint32)(ps->pOData)));
                MSG_FATAL("[MSG][DeviceSocket]: MMSSocketState nDataLen=%d",nDataLen, 0, 0);
                MMS_DEBUG(("[MSG][DeviceSocket]: MMSSocketState Enter! pData:%d,ps->pData:%d",pData,ps->pOData));
                if(pData != NULL && nDataLen > 0)
                {
                    pData -= 4;
                    MEMCPY((void*)pData,(void*)&nDataLen,4);
                    
                    ISHELL_PostEventEx(
                        AEE_GetShell(),
                        EVTFLG_ASYNC,
                        AEECLSID_WMSAPP,
                        EVT_MMS_PDUDECODE,
                        0,
                        (uint32)pData);
                 }
                 else
                 {
                    // 接收错误
                    ISHELL_PostEventEx(
                        AEE_GetShell(),
                        EVTFLG_ASYNC,
                        AEECLSID_WMSAPP,
                        EVT_MMS_PDUDECODE,
                        ps->nError,
                        0);
                 }

               MMSSocketClose(&ps);
               return;
            }
        }
        break;
        case WMS_MMS_PDU_WSPHTTPGETreq:
        {
            if(MMSSocket_HTTPGETReq(ps,(char*)ps->dwParam))
            {
                FREEIF((char*)ps->dwParam);
                ps->nState = WMS_MMS_PDU_MRetrieveConf;
            }
        }
        break;
        default:
        {
            MMSSocketClose(&ps);
            return;
        }
    }
    ISHELL_SetTimer(AEE_GetShell(),1000,(PFNNOTIFY)&MMSSocketState,ps);
}

boolean  MMSSocketRecv (MMSSocket *ps, uint8 *pBuf, uint32 *pLen)
{
	MMSSocket* pSocketInfo = ps;
	MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketRecv Enter!"));
	
	MEMCPY(pBuf, pSocketInfo->RecBuffer, pSocketInfo->RecCount);
	MEMSET(pSocketInfo->RecBuffer, 0, sizeof(pSocketInfo->RecBuffer));
	
	*pLen = pSocketInfo->RecCount;
	pSocketInfo->RecCount = 0;
	
	return TRUE;
}


/**
This function is used to send data over the socket. It is an asynchronous function call,
with the success or failure of the sending operation being notified via the socket callback.

\param	ps		[in]    The socket object
\param	pBuf	[in]    The buffer containing the data to send
\param	nLen	[in]    The length of the above data buffer
\param	pnID	[out]   A unique ID is returned. This ID is used in the socket callback
                        to determine whether the data has been successfully sent or not.

\return M4_ESuccess         If all the input arguments are valid. It does not indicate that

\return Whether the send request was accepted
*/
boolean  MMSSocketSend (MMSSocket *ps, const uint8 *pBuf, uint32 nLen)
{
	int ret = SUCCESS;
	
	MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketSend Enter nLen = %d, pBuf = %s",nLen, pBuf));

	if(nLen == 0)
	{
		return EFAILED;
	}

    ps->nDataLen = nLen;
    FREEIF(ps->pSendData);
	ps->pSendData = (uint8*)MALLOC(nLen);
	if ( ps->pSendData == NULL )
	{
		return ENOMEMORY;
	}
	MEMCPY(ps->pSendData,pBuf,nLen);  
    
	ISOCKET_Writeable(ps->pISocket, MMI_SocketSend, ps);
	
    MMS_DEBUG(("[MSG][MMSSocketSend]: End"));
	return ret;
}

static void MMI_SocketSend(void *pData)
{
	MMSSocket *pUserData = (MMSSocket*)pData;
	int32 bytes_written = 0;
	uint32 nLen = pUserData->nDataLen - pUserData->nBytesSent;
	
	MSG_FATAL("[MSG][DeviceSocket]: MMI_SocketSend Enter nLen = %d,pSocketInfo->nBytesSent=%d",nLen,pUserData->nBytesSent,0);
	
	if(nLen > 0)
	{
		bytes_written = ISOCKET_Write(pUserData->pISocket,
								pUserData->pSendData + pUserData->nBytesSent,
								nLen);
		MSG_FATAL("[MSG][DeviceSocket]: MMI_SocketSend Enter nLen = %d,bytes_written = %d",nLen,bytes_written,0);
	}
	
	if(bytes_written > 0)
	{
		//reset the timeout timer
		if((uint32)bytes_written < nLen)
		{
			pUserData->nBytesSent += (uint32)bytes_written;
		}
		else if((uint32)bytes_written == nLen)
		{
		    
			//client may destroy socket in M4_SOCKETNOTIFICATION_RECEIVE synchronously
			//so we must register call-back first, otherwise the pointer may invaliable
			pUserData->nBytesSent += (uint16)bytes_written;
			MSG_FATAL("[MSG][DeviceSocket]: MMI_SocketSend Send-Queue Empty!",0,0,0);
			//ISOCKET_Readable(pUserData->pISocket, SocketReadableCB, pSocketInfo);
			return;
		}
	}
	else if(bytes_written == AEE_NET_ERROR)
	{
		int nError = 0;

		nError = ISOCKET_GetLastError(pUserData->pISocket);
		MSG_FATAL("[MSG][DeviceSocket]: MMI_SocketSend nError = 0x%x", nError,0,0);

		return;
	}
	else
	{
		//do nothing
	}

	if(pUserData->pSendData!= NULL)
	{
		ISOCKET_Writeable(pUserData->pISocket, MMI_SocketSend, pUserData);
	}
	else
	{
		MSG_FATAL("[MSG][DeviceSocket]: MMI_SocketSend Send-Queue Empty!",0,0,0);
	}
}

static void SocketConnect_OnTimeout(void* pData)
{
	MMSSocket* pSocketInfo = (MMSSocket*)pData;
	MMS_DEBUG(("[MSG][DeviceSocket]: SocketConnect_OnTimeout!"));
}

static void SocketReadableCB(void *pSocketData)
{
	MMSSocket* pData = (MMSSocket*)pSocketData;
	int nBitGet;
	MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB Enter!",0,0,0);
    
	if(pData == NULL)
	{
		return;
	}
	else if(pData->pISocket == NULL)
	{
		return;
	}
	
	nBitGet = (uint16)ISOCKET_Read(pData->pISocket, pData->RecBuffer + pData->RecCount, MSG_MAX_PACKET_SIZE - pData->RecCount);
	MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB Enter pSocketInfo->RecCount = %d!",pData->RecCount,0,0);

	if(nBitGet > 0)
	{
		//reset the timeout timer
		MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB Enter!",0,0,0);

        pData->RecCount += nBitGet;
        
		pData->NoDataCount = 0;
#ifdef MMS_TEST 
		if(pData->RecCount > 0)
		{

			IFile* pIFile = NULL;
		    IFileMgr *pIFileMgr = NULL;
		    int result = 0;
    
			result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
			if (SUCCESS != result)
		    {
				MSG_FATAL("MRS: Open file error %x", result,0,0);
				return;
		    }

		    pIFile = IFILEMGR_OpenFile( pIFileMgr, "fs:/hsmm/pictures/out.txt", _OFM_READWRITE);
			if ( pIFile != NULL )
	        {
	            IFILE_Seek(pIFile, _SEEK_START, 0);
	            IFILE_Write( pIFile, pData->RecBuffer, pData->RecCount);

	            MSG_FATAL("IFILEMGR_OpenFile pSocketInfoTag.RecCount=%d",pData->RecCount,0,0);
	            IFILE_Release( pIFile);
	            pIFile = NULL;
	            IFILEMGR_Release(pIFileMgr);
	            pIFileMgr = NULL;
	        }
	        else
	        {
				pIFile = IFILEMGR_OpenFile( pIFileMgr, "fs:/hsmm/pictures/out.txt", _OFM_CREATE);
				if ( pIFile != NULL )
		        {
		            IFILE_Write( pIFile, pData->RecBuffer, pData->RecCount);

		            MSG_FATAL("IFILEMGR_OpenFile pSocketInfoTag.RecCount=%d",pData->RecCount,0,0);
		            IFILE_Release( pIFile);
		            pIFile = NULL;
		            IFILEMGR_Release(pIFileMgr);
		            pIFileMgr = NULL;
		        }
	        }
        }
	#endif	     
    
    //client may destroy socket in M4_SOCKETNOTIFICATION_RECEIVE synchronously
    //so we must register call-back first, otherwise the pointer may invaliable
		if(pData->RecCount >= MSG_MAX_PACKET_SIZE)
		{
		    MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB ERR: Length received > max length",0,0,0);
	    }
		else if(pData != NULL && pData->pISocket != NULL)
		{
			ISOCKET_Readable(pData->pISocket, SocketReadableCB, pData);
		}
		return;
	}
	else if(nBitGet == AEE_NET_WOULDBLOCK)
	{
		pData->RecCount = 0;
		pData->NoDataCount = 0;
	}
	else if(nBitGet == AEE_NET_ERROR)
	{
		int err = 0;
		
		err = ISOCKET_GetLastError(pData->pISocket);
		MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB err = %d!",err,0,0);
		
		pData->RecCount = 0;
		pData->NoDataCount = 0;

		return;
	}
	else if(nBitGet == 0)
	{
		pData->NoDataCount++;

		MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB Enter pSocketInfo->NoDataCount = %d!",pData->NoDataCount,0,0);

		//when the RecCount equal to zero three times continiously,
		//to ensure the connection is closed
		if(pData->NoDataCount > 3)
		{
		    MSG_FATAL("[MSG][DeviceSocket]: pData->NoDataCount > 3",0,0,0);
		    if(pData->DNSCallback.pfnNotify)
    	    {
                MSG_FATAL("[MSG][DeviceSocket]: pfnNotify != NULL",0,0,0);
    	        pData->DNSCallback.pfnNotify(pData->DNSCallback.pNotifyData);
    	        
    	        pData->DNSCallback.pNotifyData = NULL;
    	        pData->DNSCallback.pfnNotify = NULL;
    	    }
            else
            {
                MSG_FATAL("[MSG][DeviceSocket]: pfnNotify == NULL",0,0,0);
            }
			return;
		}
	}

	//client may destroy socket in M4_SOCKETNOTIFICATION_RECEIVE synchronously
	if(pData != NULL && pData->pISocket != NULL)
	{
		MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB setSocketReadableCB!",0,0,0);
		ISOCKET_Readable(pData->pISocket, SocketReadableCB, pData);
	}
}

boolean MMS_STREQI(const char *s1, const char *s2)
{
   return (s1 && s2 && STRICMP(s1,s2) == 0);
}

char* MMS_WSP_MineType2MormalMimeType(const char* pszSrc)
{
    if (pszSrc && *pszSrc) 
    {
        if(STRSTR(pszSrc,AUDIO_MIME_BASE))
        {
            if(STRSTR(pszSrc,sWspMimeType[AUDIO_AAC]))
            {
                return MT_AAC;
            }
            else if(STRSTR(pszSrc,sWspMimeType[AUDIO_AMR]))
            {
                return MT_AMR;
            }
            else if(STRSTR(pszSrc,sWspMimeType[AUDIO_MIDI])
                || STRSTR(pszSrc,sWspMimeType[AUDIO_MID])
                || STRSTR(pszSrc,sWspMimeType[AUDIO_X_MID])
                || STRSTR(pszSrc,sWspMimeType[AUDIO_X_MIDI]))
            {
                return MT_MIDI;
            }
            else if(STRSTR(pszSrc,sWspMimeType[AUDIO_X_MP3])
                || STRSTR(pszSrc,sWspMimeType[AUDIO_MP3]))
            {
                return MT_MP3;
            }
            else if(STRSTR(pszSrc,sWspMimeType[AUDIO_MPEG3])
                || STRSTR(pszSrc,sWspMimeType[AUDIO_X_MPEG3]))
            {
                return MT_VIDEO_MPEG4;
            }
            else if(STRSTR(pszSrc,sWspMimeType[AUDIO_MPEG])
                || STRSTR(pszSrc,sWspMimeType[AUDIO_X_MPEG]))
            {
                return MT_MP3;
            }
            else if(STRSTR(pszSrc,sWspMimeType[AUDIO_MPG])
                || STRSTR(pszSrc,sWspMimeType[AUDIO_X_MPG]))
            {
                return MT_MP3;
            }
            else if(STRSTR(pszSrc,sWspMimeType[AUDIO_3GPP])
                || STRSTR(pszSrc,sWspMimeType[AUDIO_MP4]))
            {
                return MT_VIDEO_MPEG4;
            }
        }
        else if(STRSTR(pszSrc,VIDEO_MIME_BASE))
        {
            if(STRSTR(pszSrc,sWspMimeType[VIDEO_MP4])
                || STRSTR(pszSrc,sWspMimeType[VIDEO_H263])
                || STRSTR(pszSrc,sWspMimeType[VIDEO_3GPP])
                || STRSTR(pszSrc,sWspMimeType[VIDEO_3G2])
                || STRSTR(pszSrc,sWspMimeType[VIDEO_3GP])
                )
            {
                return MT_VIDEO_MPEG4;
            }
        }
        else if(STRSTR(pszSrc,IMAGE_MIME_BASE))
        {
            if(STRSTR(pszSrc,sWspMimeType[IMAGE_JPEG]))
            {
                return MT_JPEG;
            }
            else if(STRSTR(pszSrc,sWspMimeType[IMAGE_JPG]))
            {
                return MT_JPG;
            }
            else if(STRSTR(pszSrc,sWspMimeType[IMAGE_PNG]))
            {
                return MT_PNG;
            }
            else if(STRSTR(pszSrc,sWspMimeType[IMAGE_GIF]))
            {
                return MT_GIF;
            }
            else if(STRSTR(pszSrc,sWspMimeType[IMAGE_WBMP]))
            {
                return MT_BMP;
            }
        }     
    }
    return NULL;
}
char* STRTOPHONENUMBER(char* pDesStr,char* pScrStr)
{
    char chrSpToken[] = {'+','*','w','p','#','\0'};
    int i = 0;

    if(pDesStr == pScrStr)
    {
        while(pDesStr[i])
        {
            if(pDesStr[i] < '0' || pDesStr[i] > '9')
            {
                if(!STRCHR(chrSpToken,(char)pDesStr[i]))
                {
                    break;
                }
            }
            i++;
        };
        pDesStr[i] = '\0';
    }
    else
    {
        while(*pScrStr)
        {
            if(*pScrStr < '0' || *pScrStr > '9')
            {
                if(!STRCHR(chrSpToken,*pScrStr))
                {
                    break;
                }
            }
            *pDesStr = *pScrStr;
            pScrStr++;
            pDesStr++;
        }
        *pDesStr = '\0';
    }
    return pDesStr;
}
const char *MMS_GetMimeType(const char *pszSrc)
{
   /* TODO - call the shell for help with some content */
   if (pszSrc && *pszSrc) {
      char *pext = STRRCHR(pszSrc, '.');
      if (pext) {
         if (MMS_STREQI(pext+1, "gif")) {
            return "image/gif";
         
         } else if (MMS_STREQI(pext+1, "jpeg") || MMS_STREQI(pext+1, "jpg") ) {
            return "image/jpeg";
         
         } else if (MMS_STREQI(pext+1, "bmp")) {
            return "image/vnd.wap.wbmp";
         
         } else if (MMS_STREQI(pext+1, "png")) {
            return "image/png";
         
         } else if (MMS_STREQI(pext+1, "bci")) {
            return "image/bci";
            
         } else if (MMS_STREQI(pext+1, "aac")) {
            return "audio/aac";
            
         }else if (MMS_STREQI(pext+1, "amr")) {
            return "audio/amr";
            
         }else if (MMS_STREQI(pext+1, "imelody")) {
            return "audio/imelody";
            
         }else if (MMS_STREQI(pext+1, "mid")) {
            return "audio/mid";
            
         } else if (MMS_STREQI(pext+1, "midi")) {
            return "audio/midi";
            
         } else if (MMS_STREQI(pext+1, "mp3")) {
            return "audio/mp3";
            
         } else if (MMS_STREQI(pext+1, "mpeg3")) {
            return "audio/mpeg3";
            
         } else if (MMS_STREQI(pext+1, "mpeg")) {
            return "audio/mpeg";
            
         } else if (MMS_STREQI(pext+1, "mpg")) {
            return "audio/mpg";
            
         } else if (MMS_STREQI(pext+1, "x-mid")) {
            return "audio/x-mid";
            
         } else if (MMS_STREQI(pext+1, "x-midi")) {
            return "audio/x-midi";
            
         } else if (MMS_STREQI(pext+1, "x-mp3")) {
            return "audio/x-mp3";
            
         } else if (MMS_STREQI(pext+1, "x-mpeg3")) {
            return "audio/x-mpeg3";
            
         } else if (MMS_STREQI(pext+1, "x-mpeg")) {
            return "audio/x-mpeg";
            
         } else if (MMS_STREQI(pext+1, "x-mpg")) {
            return "audio/x-mpg";
            
         } else if (MMS_STREQI(pext+1, "3gpp")) {
            return "video/3gpp";
            
         } else if (MMS_STREQI(pext+1, "3gpp2")) {
            return "video/3gpp2";
            
         } else if (MMS_STREQI(pext+1, "mp4")) {
            return "video/mp4";
            
         } else if (MMS_STREQI(pext+1, "3gp")) {
            return "video/3gp";
            
         } 
      }
   }
   return NULL;
}
#endif
