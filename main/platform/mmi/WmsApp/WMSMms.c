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

#include "OEMClassIds.h"      // Applet Ids

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

#ifdef FEATURE_USES_MMS

#define DBGPRINTF_EX_FORMAT    "*dbgprintf-%d* %s:%d"
#define MMS_DEBUG(arg) __DBGPRINTF(DBGPRINTF_EX_FORMAT,5,__FILE__,__LINE__),__DBGPRINTF arg

static int MMS_WSP_Decode_UINTVAR(uint8* pData, int iDataLen, int* iUintvarLen);
static int MMS_WSP_GetExpiry(uint8* buf, int iDataLen, int curtime);
static int MMS_WSP_DecodeLongInteger(uint8* buf, int iDataLen, int* iValue);
static int MMS_WSP_GetValueLen(uint8* pData, int iDataLen, int* iDataOffset);
static int MMS_PDU_DecodeEncodedString(uint8* ptr, int datalen,	uint8 ePDUType,	uint8* handle);
static void MMI_SocketSend(void *pData);
static void SocketReadableCB(void* pSocketData);
static void SocketConnect_OnTimeout(void* pData);
static void ConnectError(void* pDdata, int nError);
static int MMS_Encode_header(uint8* mms_context,uint8 *phonenum, uint8 *subject);
static int MMS_WSP_Encode_UINTVAR(uint8* buf, int val);
static int MMS_Encode_MsgBody(uint8* hPDU, int hLen, boolean isText);
static int MMS_CreateSmilFile(uint8* encbuf,MMS_MESSAGE_TYPE type);
static int MMS_GetFileContent(uint8* encbuf,char* FileName,MMS_MESSAGE_TYPE type);
static int MMS_EncodeText(uint8* encbuf,uint8 *text,int len);
static int MMS_WSP_DecodeContentTypeHeader(uint8* pData, int iDataLen, WSP_DEC_DATA_FRAGMENT* iMIMEHeaders, boolean* bIsMultipart,
								 int* charset, boolean add_boundary_param, int iHeadersLen, boolean in_encode_check_param, int *inout_depth);
static int MMS_WSP_DecodeContentTypeParams(uint8* pData, int iDataLen, WSP_DEC_DATA_FRAGMENT* iMIMEParams, int* charset);
static int MMS_WSP_Decode_MultipartData(uint8* pData, int iDataLen,int nParts, WSP_DEC_DATA_FRAGMENT* iMIMEParts,int *inout_depth);

static char* MMS_WSP_ContentTypeDB_MMS2Text(int ct);
static boolean is_us_ascii_string(char * in_s, int in_len);

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

#define slim_isctype(c,type) (cSlim_clib_ctype_table[(c)] & (type))
#define slim_isnonascii(c) (slim_isctype((c),SLIM_CLIB_CTYPE_NONASCII))

#define POST_TEST ("POST http://mmsc.vnet.mobi HTTP/1.1\r\nHost:10.0.0.200:80\r\nAccept-Charset:utf-8\r\nContent-Length:%d\r\nAccept:*/*,application/vnd.wap.mms-message\r\nAccept-Language:en\r\nAccept-Encoding:gzip,deflate\r\nContent-Type:application/vnd.wap.mms-message\r\nUser-Agent: Nokia6235/1.0 (S190V0200.nep) UP.Browser/6.2.3.2 MMP/2.0\r\nx-wap-profile: \"http://nds1.nds.nokia.com/uaprof/N6235r200.xml\"\r\nKeep-Alive:300\r\nConnection:Keep-Alive\r\n\r\n")
/*
** 彩信里面用到的数据类型
*/
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
    "image/gif",
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

uint32 cSlim_clib_ctype_table[256] = {
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

uint8 g_mms_buffer[300*1024];

static char* MMS_WSP_ContentTypeDB_MMS2Text(int ct)
{
	if ((ct >= 0) && (ct <= 0x4b))
	{
		return (char*)DB_Mms2Text[ct];
	}
	else
		return "application/octet-stream";
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

static int MMS_Encode_header(uint8* mms_context,uint8 *phonenum, uint8 *subject)
{
	int i=0;
    JulianType juDateTime	= {0};
	char szDataTime[30] = {0};
    int addresslen = 0;
    int len = 0;
    int uintvar_len = 0;
    char *addresstype = "/TYPE=PLMN";
    uint8 uintvar[5];
    
    uint8* pCurPos = (uint8*)mms_context;	
	

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


	//subject
	len = STRLEN((char*)subject);
	if ( len > 0)
	{
		*pCurPos = 0x96; pCurPos++;/*Subject*/
		if (len+1+1 > 30)
		{
			*pCurPos = 0x1F; pCurPos++;/*Charset - UTF-8*//*Charset - UTF-8*/
			uintvar_len = MMS_WSP_Encode_UINTVAR(uintvar,len+1+1);/* +1 = charset value , +1 - null termination*/
			STRNCPY((char*)pCurPos,(char*)&uintvar[5-uintvar_len],uintvar_len);
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
		STRNCPY((char*)pCurPos,(char*)subject,STRLEN((char*)subject));
		pCurPos += STRLEN((char*)subject);
		*pCurPos = 0x00; pCurPos++;
	}

	//后面为数据部分
	return (int)(pCurPos - (uint8*)mms_context);
}

static int MMS_Encode_MsgBody(uint8* hPDU, int hLen, boolean isText)
{
	int head_length = 0;
	int file_length = 0;
	int sub_head_length = 0;
	int content_size = 0;
	
	uint8* pCurPos = hPDU;

	//Msg body
	*pCurPos = 0x84; pCurPos++;
	*pCurPos = 0x1c; pCurPos++;  //head length
	*pCurPos = 0xb3; pCurPos++;  //application/vnd.wap.multipart.related类型，对应于附录A里面的0x33+0x80；

	*pCurPos = 0x89; pCurPos++;  //Type参数，见WSP Table38
	//application/smil
	*pCurPos = 0x61; pCurPos++;
	*pCurPos = 0x70; pCurPos++;
	*pCurPos = 0x70; pCurPos++;
	*pCurPos = 0x6c; pCurPos++;
	*pCurPos = 0x69; pCurPos++;
	*pCurPos = 0x63; pCurPos++;
	*pCurPos = 0x61; pCurPos++;
	*pCurPos = 0x74; pCurPos++;
	*pCurPos = 0x69; pCurPos++;
	*pCurPos = 0x6F; pCurPos++;
	*pCurPos = 0x6E; pCurPos++;
	*pCurPos = 0x2F; pCurPos++;
	*pCurPos = 0x73; pCurPos++;
	*pCurPos = 0x6D; pCurPos++;
	*pCurPos = 0x69; pCurPos++;
	*pCurPos = 0x6C; pCurPos++;
	*pCurPos = 0x00; pCurPos++;

	*pCurPos = 0x8A; pCurPos++;  //Start参数,见WSP协议Table 38. Well-Known Parameter Assignments,编码值加0x80
	//<start>
	*pCurPos = 0x3C; pCurPos++;
	*pCurPos = 0x73; pCurPos++;
	*pCurPos = 0x74; pCurPos++;
	*pCurPos = 0x61; pCurPos++;
	*pCurPos = 0x72; pCurPos++;
	*pCurPos = 0x74; pCurPos++;
	*pCurPos = 0x3E; pCurPos++;
	*pCurPos = 0x00; pCurPos++;

	// -------------- files -------------- //
	//file count, smil file is one
	if ( isText )
	{
		*pCurPos = 0x03; pCurPos++;	//part number
	}
	else
	{
		*pCurPos = 0x02; pCurPos++;	//part number
	}	

	return (int)(pCurPos-hPDU);
}

static int MMS_EncodeText(uint8* encbuf,uint8 *text,int len)
{
	int head_length = 0;
	int file_length = 0;
	int sub_head_length = 0;
	int content_size = 0;
	int uintvar_len;
	uint8 m_buf[128] = {0};
	uint8 uintvar[5];
	uint8* pCurPos = m_buf;
	
	
	if ( encbuf == NULL || text == NULL )
	{
		return -1;
	}

	//file use "text.txt"
	content_size = len;
	file_length = 8;
	sub_head_length = 8+5;
	*pCurPos = sub_head_length; pCurPos++;

	//UTF-8
	*pCurPos = 0x83; pCurPos++;
	*pCurPos = 0x85; pCurPos++;

	//FILE NAME
	STRNCPY((char*)pCurPos,"text.txt",8);
	pCurPos += 8;
	*pCurPos = 0x00; pCurPos++;

	*pCurPos = 0x81; pCurPos++;
	*pCurPos = 0xea; pCurPos++;

	//add Content-ID
	*pCurPos = 0xc0; pCurPos++;
	*pCurPos = 0x22; pCurPos++;
	*pCurPos = 0x3c; pCurPos++;
	STRNCPY((char*)pCurPos,"text.txt",8);
	pCurPos += 8;
	*pCurPos = 0x3e; pCurPos++;
	*pCurPos = 0x00; pCurPos++;

	//add Content-Location
	*pCurPos = 0x8e; pCurPos++;
	STRNCPY((char*)pCurPos,"text.txt",8);
	pCurPos += 8;
	*pCurPos = 0x00; pCurPos++;

	head_length = (uint8)(pCurPos-m_buf);

	pCurPos = encbuf;

	//head length
	*pCurPos = head_length; pCurPos++;
	uintvar_len = MMS_WSP_Encode_UINTVAR(uintvar,content_size);

	//content length
	STRNCPY((char*)pCurPos,(char*)&uintvar[5-uintvar_len],uintvar_len);
	pCurPos += uintvar_len;

	//sub head
	STRNCPY((char*)pCurPos,(char*)m_buf,head_length);
	pCurPos += head_length;

	//content
	STRNCPY((char*)pCurPos,(char*)text,len);
	pCurPos += len;

	return (int)(pCurPos-encbuf);
}

static int MMS_GetFileContent(uint8* encbuf,char* FileName,MMS_MESSAGE_TYPE type)
{
	int head_length = 0;
	int file_length = 0;
	int sub_head_length = 0;
	int content_size = 0;
	IFile* pIFile = NULL;
    IFileMgr *pIFileMgr = NULL;
    FileInfo pInfo = {0};
    uint8* pCurPos = encbuf;
    uint8 uintvar[5];
    int uintvar_len;
	int result = SUCCESS;
	int16 readcount = 0;
	
    result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
	if (SUCCESS != result)
    {
		MSG_FATAL("MRS: Open file error %x", result,0,0);
		return -1;
    }

    if (IFILEMGR_GetInfo(pIFileMgr,FileName, &pInfo) == SUCCESS)
    {
    	content_size = (int)pInfo.dwSize;
    	//IFILEMGR_Release(pIFileMgr);
        //pIFileMgr = NULL;
    }
    
	if ( type == MMS_MESSAGE_JPG )
	{
		*pCurPos = 0x1a; pCurPos++;
		uintvar_len = MMS_WSP_Encode_UINTVAR(uintvar,content_size);
		//content length
		STRNCPY((char*)pCurPos,(char*)&uintvar[5-uintvar_len],uintvar_len);
		pCurPos += uintvar_len;

		*pCurPos = 0x08; pCurPos++;
		*pCurPos = 0x9e; pCurPos++;

		//name
		*pCurPos = 0x85; pCurPos++; 
		STRNCPY((char*)pCurPos,"1.jpg",5);
		pCurPos += 5;
		*pCurPos = 0x00; pCurPos++;
		
		//add Content-ID
		*pCurPos = 0xc0; pCurPos++;
		*pCurPos = 0x22; pCurPos++;
		*pCurPos = 0x3c; pCurPos++;
		STRNCPY((char*)pCurPos,"1.jpg",5);
		pCurPos += 5;
		*pCurPos = 0x3e; pCurPos++;
		*pCurPos = 0x00; pCurPos++;

		//add Content-Location
		*pCurPos = 0x8e; pCurPos++;
		STRNCPY((char*)pCurPos,"1.jpg",5);
		pCurPos += 5;
		*pCurPos = 0x00; pCurPos++;

		pIFile = IFILEMGR_OpenFile( pIFileMgr, FileName, _OFM_READ );
		if ( pIFile != NULL )
        {
            IFILE_Seek(pIFile, _SEEK_START, 0);
            readcount = IFILE_Read( pIFile, pCurPos, pInfo.dwSize);
            pCurPos += readcount;
            
            MSG_FATAL("MRS: Open file pInfo.dwSize=%d,pSocketInfoTag.RecCount=%d", pInfo.dwSize,readcount,0);
            IFILE_Release( pIFile);
            pIFile = NULL;
            IFILEMGR_Release(pIFileMgr);
            pIFileMgr = NULL;
        }

        return (int)(pCurPos-encbuf);
	}
	else if ( type == MMS_MESSAGE_GIF )
	{
		*pCurPos = 0x1a; pCurPos++;
		uintvar_len = MMS_WSP_Encode_UINTVAR(uintvar,content_size);
		//content length
		STRNCPY((char*)pCurPos,(char*)&uintvar[5-uintvar_len],uintvar_len);
		pCurPos += uintvar_len;

		*pCurPos = 0x08; pCurPos++;
		*pCurPos = 0x9d; pCurPos++;

		//name
		*pCurPos = 0x85; pCurPos++; 
		STRNCPY((char*)pCurPos,"1.gif",5);
		pCurPos += 5;
		*pCurPos = 0x00; pCurPos++;
		
		//add Content-ID
		*pCurPos = 0xc0; pCurPos++;
		*pCurPos = 0x22; pCurPos++;
		*pCurPos = 0x3c; pCurPos++;
		STRNCPY((char*)pCurPos,"1.gif",5);
		pCurPos += 5;
		*pCurPos = 0x3e; pCurPos++;
		*pCurPos = 0x00; pCurPos++;

		//add Content-Location
		*pCurPos = 0x8e; pCurPos++;
		STRNCPY((char*)pCurPos,"1.gif",5);
		pCurPos += 5;
		*pCurPos = 0x00; pCurPos++;

		pIFile = IFILEMGR_OpenFile( pIFileMgr, FileName, _OFM_READ );
		if ( pIFile != NULL )
        {
            IFILE_Seek(pIFile, _SEEK_START, 0);
            readcount = IFILE_Read( pIFile, pCurPos, pInfo.dwSize);
            pCurPos += readcount;
            
            MSG_FATAL("MRS: Open file pInfo.dwSize=%d,pSocketInfoTag.RecCount=%d", pInfo.dwSize,readcount,0);
            IFILE_Release( pIFile);
            pIFile = NULL;
            IFILEMGR_Release(pIFileMgr);
            pIFileMgr = NULL;
        }

        return (int)(pCurPos-encbuf);
	}
	else if ( type == MMS_MESSAGE_PNG )
	{
		*pCurPos = 0x1a; pCurPos++;
		uintvar_len = MMS_WSP_Encode_UINTVAR(uintvar,content_size);
		//content length
		STRNCPY((char*)pCurPos,(char*)&uintvar[5-uintvar_len],uintvar_len);
		pCurPos += uintvar_len;

		*pCurPos = 0x08; pCurPos++;
		*pCurPos = 0xa0; pCurPos++;

		//name
		*pCurPos = 0x85; pCurPos++; 
		STRNCPY((char*)pCurPos,"1.png",5);
		pCurPos += 5;
		*pCurPos = 0x00; pCurPos++;
		
		//add Content-ID
		*pCurPos = 0xc0; pCurPos++;
		*pCurPos = 0x22; pCurPos++;
		*pCurPos = 0x3c; pCurPos++;
		STRNCPY((char*)pCurPos,"1.png",5);
		pCurPos += 5;
		*pCurPos = 0x3e; pCurPos++;
		*pCurPos = 0x00; pCurPos++;

		//add Content-Location
		*pCurPos = 0x8e; pCurPos++;
		STRNCPY((char*)pCurPos,"1.png",5);
		pCurPos += 5;
		*pCurPos = 0x00; pCurPos++;

		pIFile = IFILEMGR_OpenFile( pIFileMgr, FileName, _OFM_READ );
		if ( pIFile != NULL )
        {
            IFILE_Seek(pIFile, _SEEK_START, 0);
            readcount = IFILE_Read( pIFile, pCurPos, pInfo.dwSize);
            pCurPos += readcount;
            
            MSG_FATAL("MRS: Open file pInfo.dwSize=%d,pSocketInfoTag.RecCount=%d", pInfo.dwSize,readcount,0);
            IFILE_Release( pIFile);
            pIFile = NULL;
            IFILEMGR_Release(pIFileMgr);
            pIFileMgr = NULL;
        }

        return (int)(pCurPos-encbuf);
	}
	else if ( type == MMS_MESSAGE_BMP )
	{
		*pCurPos = 0x1a; pCurPos++;
		uintvar_len = MMS_WSP_Encode_UINTVAR(uintvar,content_size);
		//content length
		STRNCPY((char*)pCurPos,(char*)&uintvar[5-uintvar_len],uintvar_len);
		pCurPos += uintvar_len;

		*pCurPos = 0x08; pCurPos++;
		*pCurPos = 0xA1; pCurPos++;

		//name
		*pCurPos = 0x85; pCurPos++; 
		STRNCPY((char*)pCurPos,"1.bmp",5);
		pCurPos += 5;
		*pCurPos = 0x00; pCurPos++;
		
		//add Content-ID
		*pCurPos = 0xc0; pCurPos++;
		*pCurPos = 0x22; pCurPos++;
		*pCurPos = 0x3c; pCurPos++;
		STRNCPY((char*)pCurPos,"1.bmp",5);
		pCurPos += 5;
		*pCurPos = 0x3e; pCurPos++;
		*pCurPos = 0x00; pCurPos++;

		//add Content-Location
		*pCurPos = 0x8e; pCurPos++;
		STRNCPY((char*)pCurPos,"1.bmp",5);
		pCurPos += 5;
		*pCurPos = 0x00; pCurPos++;

		pIFile = IFILEMGR_OpenFile( pIFileMgr, FileName, _OFM_READ );
		if ( pIFile != NULL )
        {
            IFILE_Seek(pIFile, _SEEK_START, 0);
            readcount = IFILE_Read( pIFile, pCurPos, pInfo.dwSize);
            pCurPos += readcount;
            
            MSG_FATAL("MRS: Open file pInfo.dwSize=%d,pSocketInfoTag.RecCount=%d", pInfo.dwSize,readcount,0);
            IFILE_Release( pIFile);
            pIFile = NULL;
            IFILEMGR_Release(pIFileMgr);
            pIFileMgr = NULL;
        }

        return (int)(pCurPos-encbuf);
	}
	else if ( type == MMS_MESSAGE_3GP)
	{
		*pCurPos = 0x1a; pCurPos++;
		uintvar_len = MMS_WSP_Encode_UINTVAR(uintvar,content_size);
		//content length
		STRNCPY((char*)pCurPos,(char*)&uintvar[5-uintvar_len],uintvar_len);
		pCurPos += uintvar_len;

		*pCurPos = 0x12; pCurPos++;

		STRNCPY((char*)pCurPos,"video/3gpp",10);
		pCurPos += 10;
		*pCurPos = 0x00; pCurPos++;

		//name
		*pCurPos = 0x85; pCurPos++; 
		STRNCPY((char*)pCurPos,"1.3gp",5);
		pCurPos += 5;
		*pCurPos = 0x00; pCurPos++;
		
		//add Content-ID
		*pCurPos = 0xc0; pCurPos++;
		*pCurPos = 0x22; pCurPos++;
		*pCurPos = 0x3c; pCurPos++;
		STRNCPY((char*)pCurPos,"1.3gp",5);
		pCurPos += 5;
		*pCurPos = 0x3e; pCurPos++;
		*pCurPos = 0x00; pCurPos++;

		//add Content-Location
		*pCurPos = 0x8e; pCurPos++;
		STRNCPY((char*)pCurPos,"1.3gp",5);
		pCurPos += 5;
		*pCurPos = 0x00; pCurPos++;

		pIFile = IFILEMGR_OpenFile( pIFileMgr, FileName, _OFM_READ );
		if ( pIFile != NULL )
        {
            IFILE_Seek(pIFile, _SEEK_START, 0);
            readcount = IFILE_Read( pIFile, pCurPos, pInfo.dwSize);
            pCurPos += readcount;
            
            MSG_FATAL("MRS: Open file pInfo.dwSize=%d,pSocketInfoTag.RecCount=%d", pInfo.dwSize,readcount,0);
            IFILE_Release( pIFile);
            pIFile = NULL;
            IFILEMGR_Release(pIFileMgr);
            pIFileMgr = NULL;
        }

        return (int)(pCurPos-encbuf);
	}
	else if ( type == MMS_MESSAGE_MID )
	{
		*pCurPos = 0x1a; pCurPos++;
		uintvar_len = MMS_WSP_Encode_UINTVAR(uintvar,content_size);
		//content length
		STRNCPY((char*)pCurPos,(char*)&uintvar[5-uintvar_len],uintvar_len);
		pCurPos += uintvar_len;

		*pCurPos = 0x12; pCurPos++;

		STRNCPY((char*)pCurPos,"audio/midi",10);
		pCurPos += 10;
		*pCurPos = 0x00; pCurPos++;

		//name
		*pCurPos = 0x85; pCurPos++; 
		STRNCPY((char*)pCurPos,"1.mid",5);
		pCurPos += 5;
		*pCurPos = 0x00; pCurPos++;
		
		//add Content-ID
		*pCurPos = 0xc0; pCurPos++;
		*pCurPos = 0x22; pCurPos++;
		*pCurPos = 0x3c; pCurPos++;
		STRNCPY((char*)pCurPos,"1.mid",5);
		pCurPos += 5;
		*pCurPos = 0x3e; pCurPos++;
		*pCurPos = 0x00; pCurPos++;

		//add Content-Location
		*pCurPos = 0x8e; pCurPos++;
		STRNCPY((char*)pCurPos,"1.mid",5);
		pCurPos += 5;
		*pCurPos = 0x00; pCurPos++;

		pIFile = IFILEMGR_OpenFile( pIFileMgr, FileName, _OFM_READ );
		if ( pIFile != NULL )
        {
            IFILE_Seek(pIFile, _SEEK_START, 0);
            readcount = IFILE_Read( pIFile, pCurPos, pInfo.dwSize);
            pCurPos += readcount;
            
            MSG_FATAL("MRS: Open file pInfo.dwSize=%d,pSocketInfoTag.RecCount=%d", pInfo.dwSize,readcount,0);
            IFILE_Release( pIFile);
            pIFile = NULL;
            IFILEMGR_Release(pIFileMgr);
            pIFileMgr = NULL;
        }

        return (int)(pCurPos-encbuf);
	}
}

#define MMS_JPG_SMIL_FILE ("<smil><head><layout><root-layout width=\"220px\" height=\"96px\" background-color=\"#FFFFFF\" /><region id=\"Image\" width=\"128px\" height=\"96px\" top=\"0px\" left=\"0px\" fit=\"meet\" /><region id=\"Text\" width=\"220px\" height=\"0px\" top=\"96px\" left=\"0px\" fit=\"meet\" /></layout></head><body><par dur=\"5000ms\" ><img src=\"1.jpg\" region=\"Image\" /><text src=\"text.txt\" region=\"Text\"/></par></body></smil>")
#define MMS_PNG_SMIL_FILE ("<smil><head><layout><root-layout width=\"220px\" height=\"96px\" background-color=\"#FFFFFF\" /><region id=\"Image\" width=\"128px\" height=\"96px\" top=\"0px\" left=\"0px\" fit=\"meet\" /><region id=\"Text\" width=\"220px\" height=\"0px\" top=\"96px\" left=\"0px\" fit=\"meet\" /></layout></head><body><par dur=\"5000ms\" ><img src=\"1.png\" region=\"Image\" /><text src=\"text.txt\" region=\"Text\"/></par></body></smil>")
#define MMS_BMP_SMIL_FILE ("<smil><head><layout><root-layout width=\"220px\" height=\"96px\" background-color=\"#FFFFFF\" /><region id=\"Image\" width=\"128px\" height=\"96px\" top=\"0px\" left=\"0px\" fit=\"meet\" /><region id=\"Text\" width=\"220px\" height=\"0px\" top=\"96px\" left=\"0px\" fit=\"meet\" /></layout></head><body><par dur=\"5000ms\" ><img src=\"1.bmp\" region=\"Image\" /><text src=\"text.txt\" region=\"Text\"/></par></body></smil>")
#define MMS_GIF_SMIL_FILE ("<smil><head><layout><root-layout width=\"220px\" height=\"96px\" background-color=\"#FFFFFF\" /><region id=\"Image\" width=\"128px\" height=\"96px\" top=\"0px\" left=\"0px\" fit=\"meet\" /><region id=\"Text\" width=\"220px\" height=\"0px\" top=\"96px\" left=\"0px\" fit=\"meet\" /></layout></head><body><par dur=\"5000ms\" ><img src=\"1.gif\" region=\"Image\" /><text src=\"text.txt\" region=\"Text\"/></par></body></smil>")
#define MMS_MID_SMIL_FILE ("<smil><head><layout><root-layout width=\"220px\" height=\"96px\" /><region id=\"audio\"/><region id=\"Text\" width=\"220px\" height=\"0px\" top=\"96px\" left=\"0px\" fit=\"meet\" /></layout></head><body><par dur=\"5000ms\" ><audio src=\"1.mid\" region=\"audio\" /><text src=\"text.txt\" region=\"Text\"/></par></body></smil>")
#define MMS_3GP_SMIL_FILE ("<smil><head><layout><root-layout width=\"220px\" height=\"96px\" /><region id=\"video\" width=\"128px\" height=\"96px\" top=\"0px\" left=\"0px\"/><region id=\"Text\" width=\"220px\" height=\"0px\" top=\"96px\" left=\"0px\" fit=\"meet\" /></layout></head><body><par dur=\"5000ms\" ><video src=\"1.3gp\" region=\"video\" /><text src=\"text.txt\" region=\"Text\"/></par></body></smil>")

static int MMS_CreateSmilFile(uint8* encbuf,MMS_MESSAGE_TYPE type)
{
	int head_length = 0;
	int file_length = 0;
	int sub_head_length = 0;
	int content_size = 0;
    uint8* pCurPos = encbuf;
    uint8 uintvar[5];
    int uintvar_len;

	if ( type == MMS_MESSAGE_PNG )
	{
		content_size = STRLEN(MMS_PNG_SMIL_FILE);
	}
	else if ( type == MMS_MESSAGE_JPG )
	{
		content_size = STRLEN(MMS_JPG_SMIL_FILE);
	}
	else if ( type == MMS_MESSAGE_GIF )
	{
		content_size = STRLEN(MMS_GIF_SMIL_FILE);
	}
	else if ( type == MMS_MESSAGE_BMP )
	{
		content_size = STRLEN(MMS_BMP_SMIL_FILE);
	}
	else if ( type == MMS_MESSAGE_3GP )
	{
		content_size = STRLEN(MMS_3GP_SMIL_FILE);
	}
	else if ( type == MMS_MESSAGE_MID )
	{
		content_size = STRLEN(MMS_MID_SMIL_FILE);
	}
	else
	{
		return -1;
	}
	
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

	if ( type == MMS_MESSAGE_PNG )
	{
		STRNCPY((char*)pCurPos,MMS_PNG_SMIL_FILE,STRLEN(MMS_PNG_SMIL_FILE));
		pCurPos += STRLEN(MMS_PNG_SMIL_FILE);
	}
	else if ( type == MMS_MESSAGE_JPG )
	{
		STRNCPY((char*)pCurPos,MMS_JPG_SMIL_FILE,STRLEN(MMS_JPG_SMIL_FILE));
		pCurPos += STRLEN(MMS_JPG_SMIL_FILE);
	}
	else if ( type == MMS_MESSAGE_GIF )
	{
		STRNCPY((char*)pCurPos,MMS_GIF_SMIL_FILE,STRLEN(MMS_GIF_SMIL_FILE));
		pCurPos += STRLEN(MMS_GIF_SMIL_FILE);
	}
	else if ( type == MMS_MESSAGE_BMP )
	{
		STRNCPY((char*)pCurPos,MMS_BMP_SMIL_FILE,STRLEN(MMS_BMP_SMIL_FILE));
		pCurPos += STRLEN(MMS_BMP_SMIL_FILE);
	}
	else if ( type == MMS_MESSAGE_3GP )
	{
		STRNCPY((char*)pCurPos,MMS_3GP_SMIL_FILE,STRLEN(MMS_3GP_SMIL_FILE));
		pCurPos += STRLEN(MMS_3GP_SMIL_FILE);
	}
	else if ( type == MMS_MESSAGE_MID )
	{
		STRNCPY((char*)pCurPos,MMS_MID_SMIL_FILE,STRLEN(MMS_MID_SMIL_FILE));
		pCurPos += STRLEN(MMS_MID_SMIL_FILE);
	}
	
    return (int)(pCurPos-encbuf);
}

int MMS_SEND_PDU(HTTP_METHOD_TYPE type,uint8* hPDU, int hLen)
{
	
}

int MMS_SEND_TEST(uint8 *buffer)
{
	uint8 buf[5*1024] = {0};
	int head_len = 0;
	int size = 0;
	uint8 *pCurPos = buf;

	head_len = MMS_Encode_header(pCurPos,(uint8*)"+8615986838642",(uint8*)"123456789");

	MMS_DEBUG(("[MMS] MMS_Encode_header head_len = %d",head_len));
	
	pCurPos += head_len;

	head_len = MMS_Encode_MsgBody(pCurPos,0,FALSE);

	MMS_DEBUG(("[MMS] MMS_Encode_MsgBody head_len = %d",head_len));
	
	pCurPos += head_len;
	
	head_len =	MMS_CreateSmilFile(pCurPos,MMS_MESSAGE_JPG);

	MMS_DEBUG(("[MMS] MMS_CreateSmilFile head_len = %d",head_len));
	
	pCurPos += head_len;
	
	head_len = MMS_GetFileContent(pCurPos,"fs:/hsmm/pictures/1.jpg",MMS_MESSAGE_JPG);

	MMS_DEBUG(("[MMS] MMS_GetFileContent head_len = %d",head_len));
	
	pCurPos += head_len;

	size = (int)(pCurPos-buf);

	MMS_DEBUG(("[MMS] MMS_SEND_TEST size = %d",size));
	SNPRINTF((char*)buffer,MSG_MAX_PACKET_SIZE,POST_TEST,size);

	MMS_DEBUG(("POST_TEST:%s",buffer));
	head_len = STRLEN((char*)buffer);
	MMS_DEBUG(("[MMS] POST_TEST head_len = %d",head_len));
	
	MEMCPY((char*)(buffer+head_len),buf,size);

	return (head_len+size);
}

int MMS_Decode_TEST(char *file)
{
	IFile* pIFile = NULL;
    IFileMgr *pIFileMgr = NULL;
    FileInfo pInfo = {0};
	int result = 0;
	int data_size;
	int decret;
	WSP_MMS_DATA pContent = {0};
	
    result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
	if (SUCCESS != result)
    {
		MSG_FATAL("[MMS]: Open file error %x", result,0,0);
		return;
    }

	if (IFILEMGR_GetInfo(pIFileMgr,file, &pInfo) == SUCCESS)
    {
		pIFile = IFILEMGR_OpenFile( pIFileMgr, file, _OFM_READ);
		if ( pIFile != NULL )
	    {
	        IFILE_Seek(pIFile, _SEEK_START, 1);
	        data_size = IFILE_Read(pIFile, g_mms_buffer, pInfo.dwSize);

	        MSG_FATAL("[MMS]: pInfo.dwSize=%d,data_size=%d",pInfo.dwSize,data_size,0);
	        IFILE_Release( pIFile);
	        pIFile = NULL;
	        IFILEMGR_Release(pIFileMgr);
	        pIFileMgr = NULL;
	    }    
    }


    MMS_DEBUG(("Decode testing start!"));
    MSG_FATAL("[MMS]: 0x%x 0x%x 0x%x",g_mms_buffer[0],g_mms_buffer[1],g_mms_buffer[2]);
	decret = MMS_WSP_DecodeMessage(g_mms_buffer, data_size,&pContent);
	if (decret == MMC_OK)
	{
		MMS_DEBUG(("Decode testing success"));
	}

	return 0;
}

int MMS_PDU_Encode(MMS_WSP_ENCODE_SEND* encdata, uint8* hPDU, int* hLen, uint8 ePDUType)
{
	int head_len = 0;
	IFile* pIFile = NULL;
    IFileMgr *pIFileMgr = NULL;
    FileInfo pInfo = {0};
	uint8 *pCurPos = hPDU;
	
	if ( hPDU == NULL || hLen == NULL || encdata == NULL )
	{
		return MMC_NOMEM;
	}


	head_len = MMS_Encode_header(pCurPos,encdata->hTo,encdata->hSubject);

	pCurPos += head_len;

	head_len =	MMS_CreateSmilFile(pCurPos,encdata->bMsgType);

	pCurPos += head_len;
	
	head_len = MMS_GetFileContent(pCurPos,encdata->FilePath,encdata->bMsgType);

	pCurPos += head_len;
	
	return (int)(pCurPos - hPDU);

	
}

int MMS_WSP_DecodeMessage(uint8* pData, int iDataLen,  WSP_MMS_DATA* pContent)
{		
	int charset, iPart, dec = MMC_GENERIC, ret = MMC_GENERIC, contenttypelen;
	boolean bIsMultipart, completed = FALSE;
	uint8 content_type = 0;
	uint8 file_count = 0;
	uint8* pCurPos = NULL;
	int head_length = 0;
	int sub_head_length = 0;
	int content_size = 0;
	int i = 0, iDataOffset, contenttype = -1,consumed=0;
	
	contenttypelen = MMS_WSP_GetValueLen(pData, iDataLen, &iDataOffset);
	if (contenttypelen != MMS_DECODER_ERROR_VALUE)
	{
		int depth = 0;

		ret = MMS_WSP_DecodeContentTypeHeader(&pData[iDataOffset], contenttypelen, &pContent->head_info, &bIsMultipart, &charset, TRUE, 0, FALSE, &depth);

		MMS_DEBUG(("[MMS]: MMS_WSP_DecodeContentTypeHeader hContentType=%s",(char*)pContent->head_info.hContentType));
		
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

no_mem:
	return MMC_NOMEM;
}

static int MMS_WSP_Decode_MultipartData(uint8* pData, int iDataLen,int nParts, WSP_DEC_DATA_FRAGMENT* iMIMEParts,int *inout_depth)
{
	int i,iHeadersLen, iPartDataLen, iDataOffset,ret,iPart=0, consumed = 0;
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
		MMS_DEBUG(("[MMS]: hContentType=%s",iMIMEParts[cur_part].hContentType));
		MMS_DEBUG(("[MMS]: hContentName=%s",iMIMEParts[cur_part].hContentName));
		MMS_DEBUG(("[MMS]: hContentID=%s",iMIMEParts[cur_part].hContentID));
		MMS_DEBUG(("[MMS]: hContentLocation=%s",iMIMEParts[cur_part].hContentLocation));
		
		if (bIsMultipart == TRUE)
		{
			consumed += (contenttypelen + iHeadersDataOffset + iDataDataOffset + iDataOffset);
			acc = MMS_WSP_Decode_UINTVAR(&pData[consumed], iDataLen - consumed, &iAccOffset);
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
	char* filename_str = NULL;

	while (consumed < iDataLen)
	{
		paramnamelen = MMS_WSP_GetValueLen(&pData[consumed],iDataLen,&iDataOffsetname);
		if (paramnamelen == MMS_DECODER_ERROR_VALUE)
			return MMS_DECODER_ERROR_VALUE;
		paramname = (char*)&pData[consumed+iDataOffsetname];
		consumed += paramnamelen+iDataOffsetname;
		paramvaluelen = MMS_WSP_GetValueLen(&pData[consumed],
			iDataLen-consumed,&iDataOffsetvalue);
		if (paramvaluelen == MMS_DECODER_ERROR_VALUE)
			return MMS_DECODER_ERROR_VALUE;
		paramvalue = (char*)&pData[consumed+iDataOffsetvalue];
		consumed += paramvaluelen+iDataOffsetvalue;

		ok = TRUE;
		if (paramnamelen == 1)/* case of Short-interger */
		{
			/*
			 * Short-integer = OCTET
			 * ; Integers in range 0-127 shall be encoded as a one octet value with the most significant bit set
			 * ; to one (1xxx xxxx) and with the value in the remaining least significant bits.
			 */
			uint8 short_integer;
			short_integer = paramname[0] & 0x7F;
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
							paramvalue = "utf-8";
							paramvaluelen = STRLEN(paramvalue);
							*charset = 0x6a;
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
				paramname = "content ID";
				paramvaluelen--;/* ?? */
				if (paramvaluelen > 0)
				{
					if (paramvalue[0] == '"')/* ?? Quoted? */
					{
						STRNCPY((char*)iMIMEParams->hContentID,(char*)&paramvalue[1],paramvaluelen-1);
					}
					else
					{
						STRNCPY((char*)iMIMEParams->hContentID,(char*)paramvalue,paramvaluelen);
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
	char* headernameContentType = "Content-Type";
	char* headertransfername = "Content-Transfer-Encoding";
	char* headertransferbin = "Binary";
/*
	char* headervalueMultipartRelated = "multipart/related";
	char* headervalueMultipartMixed = "multipart/mixed";
	char* headervalueImageGIF = "image/gif";
	char* headervalueImageJPEG = "image/jpeg";
	char* headervalueImagePNG = "image/png";
	char* headervalueImageWBMP = "image/vnd.wap.wbmp";
	char* headervalueTextPlain = "text/plain";
	char* headervalueUnknown = "application/octet-stream";
*/
	char* param1name = "boundary";
	char* param1value = "abecadlo_bnd";
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
int MMS_PDU_Decode(MMS_WSP_DEC_DATA* decdata,uint8* ptr, int datalen,uint8 *ePDUType)
{
	int len,i=0,iDataOffset,j;
	uint8* pchar;
	int ret = MMC_OK;

	
	if ( decdata == NULL)
	{
		return MMC_GENERIC;
	}
	
	MEMSET((void*)decdata,0,sizeof(MMS_WSP_DEC_DATA));

	if (datalen <= 2)
		return MMC_GENERIC;
	if (ptr[0] != 0x8c)
		return MMC_GENERIC;
	*ePDUType = 0;

	//目前只对彩信的通知进行处理
	switch(ptr[1])
	{
		case 0x81:
			*ePDUType = MMS_PDU_SEND_CONF;
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
			decdata->notification.iDeliveryReport = MMS_DELREP_STATUS_FLAG_NOREQ;
			decdata->notification.iDeliveryReport = 0;
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
			decdata->message.iDeliveryReport = MMS_DELREP_STATUS_FLAG_NOREQ;
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
		return MMC_GENERIC;

	i = 2;
	while( i < datalen)
	{
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
						MMS_DEBUG(("len > MMS_MAX_CONTENT_LOCATION"));
						return MMC_GENERIC;
					}
					
					STRNCPY((char*)decdata->notification.hContentLocation,(const char *)pchar,len);
					decdata->notification.hContentLocation[len] = 0;
					MMS_DEBUG(("MMS_PDU_Decode 0x83 content-location:%s",decdata->notification.hContentLocation));
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
						decret = MMS_WSP_DecodeMessage(&ptr[i], datalen-i,&decdata->message.mms_data);
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
					MMS_DEBUG(("len == MMS_DECODER_ERROR_VALUE"));
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

			case 0x89:/*from*/
			{
				i++;
				len = MMS_WSP_GetValueLen(&ptr[i],datalen-i,&iDataOffset);
				if (len == MMS_DECODER_ERROR_VALUE)
				{
					MMS_DEBUG(("len == MMS_DECODER_ERROR_VALUE"));
					return MMC_GENERIC;
				}

				i += iDataOffset;
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
				}
				else if (*ePDUType == MMS_PDU_NOTIFICATION_IND) 
				{
					MMS_PDU_PutMessageClass(ptr[i], &decdata->notification.iMessageClass);
				}
				
				break;
			}

			case 0x8b:/* messageid */
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
					|| (*ePDUType == MMS_PDU_SEND_CONF)
					|| (*ePDUType == MMS_PDU_DELIVERY_IND)
					|| (*ePDUType == MMS_PDU_READ_ORIG_IND))
				{
					if(MMS_MAX_MESSAGEID_CHARSIZE < len)
					{
						MMS_DEBUG(("MMS_MAX_MESSAGEID_CHARSIZE < len"));
						return MMC_GENERIC;
					}

					switch(*ePDUType)
					{
						case MMS_PDU_RETRIEVE_CONF:
						case MMS_PDU_SEND_CONF:
						case MMS_PDU_DELIVERY_IND:
						case MMS_PDU_READ_ORIG_IND:
							//STRNCPY(mms_notif_ind->hTransactionID,pchar,len);
							//mms_notif_ind->hTransactionID[len] = 0;
							break;
							
						default:
							break;
					}
				
					
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
					MMS_PDU_PutDeliveryReport(ptr[i], &decdata->message.iDeliveryReport);
				}
				else if (*ePDUType == MMS_PDU_NOTIFICATION_IND) 
				{
					MMS_PDU_PutDeliveryReport(ptr[i], &decdata->notification.iDeliveryReport);
				}
				break;
			}

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
							len = MMS_PDU_DecodeEncodedString(&ptr[i],datalen-i,*ePDUType,decdata->message.hSubject);
							if (len == MMS_DECODER_ERROR_VALUE)
							{
								MMS_DEBUG(("len == MMS_DECODER_ERROR_VALUE"));
								return MMC_GENERIC;
							}
							i += len;
							i--;

							MMS_DEBUG(("MMS_PDU_Decode 0x96 subject:%s",decdata->notification.hSubject));
							break;
							
						case MMS_PDU_NOTIFICATION_IND:
							i++;
							len = MMS_PDU_DecodeEncodedString(&ptr[i],datalen-i,*ePDUType,decdata->notification.hSubject);
							if (len == MMS_DECODER_ERROR_VALUE)
							{
								MMS_DEBUG(("len == MMS_DECODER_ERROR_VALUE"));
								return MMC_GENERIC;
							}
							i += len;
							i--;

							MMS_DEBUG(("MMS_PDU_Decode 0x96 subject:%s",decdata->notification.hSubject));
							break;
						default:
							break;
					}				
				}
			}

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
					MMS_DEBUG(("len == MMS_DECODER_ERROR_VALUE!!"));
					return MMC_GENERIC;
				}
				i += (iDataOffset + len);
				i--;
				break;
			}
		}

		i++;
	}
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

int MMS_PDU_PutDeliveryReport(int in_DelRep, int* out_DelRep)
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
	boolean notmail = FALSE, is_utf8 = FALSE;
	int charset_len = 0, charset = 0;
	uint8* pchar = NULL, *pcharset = NULL;
	int len = 0, iDataOffset, whole_len=0, ret;
	
	whole_len = MMS_WSP_GetValueLen(&ptr[i],datalen-i,&iDataOffset);
	if (whole_len == MMS_DECODER_ERROR_VALUE)
		return MMS_DECODER_ERROR_VALUE;
		
	ret = whole_len+iDataOffset;
	i += iDataOffset;
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
		pcharset[len] = 0;
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

	if(pINetMgr == NULL)
	{
		if(ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_NET, (void**)&pINetMgr) != SUCCESS)
			return FALSE;
	}
	
	pISocket = INETMGR_OpenSocket(pINetMgr, AEE_SOCK_STREAM);

	if(pISocket != NULL)
	{
		int result = 0;
		int OptionValue = 0;
		
		*pps = MALLOC(sizeof(MMSSocket));
		if(*pps == NULL)
		{
			return FALSE;
		}

		OptionValue = SOCKET_BUFFER_SIZE;
		result = ISOCKET_SetOpt(pISocket,
						AEE_SOL_SOCKET,
						AEE_SO_RCVBUF,
						&OptionValue,
						sizeof(OptionValue));
		if(result != SUCCESS)
		{
			result = ISOCKET_GetLastError(pISocket);
			MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketNew SetOpt1 = %d!",result));
			result = 0;
		}

		OptionValue = SOCKET_BUFFER_SIZE;
		result = ISOCKET_SetOpt(pISocket,
						AEE_SOL_SOCKET,
						AEE_SO_SNDBUF,
						&OptionValue,
						sizeof(OptionValue));
						
		if(result != SUCCESS)
		{
			result = ISOCKET_GetLastError(pISocket);
			MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketNew SetOpt2 = %d!",result));
			result = 0;
		}

		(*pps)->pISocket = pISocket;

		MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketNew Success!"));
		return TRUE;
	}

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
	
	return TRUE;
}


static void ConnectError(void* pDdata, int nError)
{
	MMSSocket* pUser = (MMSSocket*)pDdata;
	if ( pUser == NULL )
	{
		return;
	}
	
	switch(nError)
	{
		case AEE_NET_SUCCESS:
			MSG_FATAL("ConnectError AEE_NET_SUCCESS",0,0,0);
			ISOCKET_Readable(pUser->pISocket, SocketReadableCB, pUser);			
			break;
		/* 若超时错误, 则 ... */
		case AEE_NET_ETIMEDOUT:
			MSG_FATAL("ConnectError AEE_NET_ETIMEDOUT",0,0,0);
			break;

		case AEE_NET_ECONNREFUSED:
			MSG_FATAL("ConnectError AEE_NET_ECONNREFUSED",0,0,0);
			break;
			
		default:
			MSG_FATAL("ConnectError error: 0x%x",nError,0,0);
			break; 
	}
}


boolean  MMSSocketConnect (MMSSocket *ps, char *pszServer, uint16 nPort)
{
	INAddr mAddress = 0;
	INPort mPort = 0;
	MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketConnect Enter pszServer = %s,nPort = %d!",pszServer,nPort));

#ifdef MMS_TEST
	if (!INET_ATON("10.0.0.200",&mAddress))
	{
		MSG_FATAL("INET_ATON failed!", 0,0,0);
		return FALSE;
	}
#endif

	ISOCKET_Connect(ps->pISocket, mAddress, HTONS((INPort)nPort), ConnectError, (void*)ps);
	return TRUE;
}

boolean  MMSSocketRecv (MMSSocket *ps, uint8 *pBuf, uint16 *pLen)
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
boolean  MMSSocketSend (MMSSocket *ps, const uint8 *pBuf, uint16 nLen)
{
	int ret = SUCCESS;
	
	MMS_DEBUG(("[MSG][DeviceSocket]: DeviceSocketSend Enter nLen = %d, pBuf = %s",nLen, pBuf));

	if(nLen == 0)
	{
		return EFAILED;
	}

	ps->pSendData = (uint8*)MALLOC(nLen);
	if ( ps->pSendData == NULL )
	{
		return ENOMEMORY;
	}
	MEMCPY(ps->pSendData,pBuf,nLen);

	ISOCKET_Writeable(ps->pISocket, MMI_SocketSend, ps);

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
			pUserData->nBytesSent += (uint16)bytes_written;
		}
		else if((uint32)bytes_written == nLen)
		{
			//client may destroy socket in M4_SOCKETNOTIFICATION_RECEIVE synchronously
			//so we must register call-back first, otherwise the pointer may invaliable
			MSG_FATAL("[MSG][DeviceSocket]: MMI_SocketSend Send-Queue Empty!",0,0,0);
			//ISOCKET_Readable(pUserData->pISocket, SocketReadableCB, pSocketInfo);
			return;
		}
	}
	else if(bytes_written == AEE_NET_ERROR)
	{
		int nError = 0;

		nError = ISOCKET_GetLastError(pUserData->pISocket);
		MSG_FATAL("[MSG][DeviceSocket]: MMI_SocketSend nError = %d", nError,0,0);

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
	int i;
	MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB Enter!",0,0,0);
    
	if(pData == NULL)
	{
		return;
	}
	else if(pData->pISocket == NULL)
	{
		return;
	}
	
	pData->RecCount = (uint16)ISOCKET_Read(pData->pISocket, pData->RecBuffer, MSG_MAX_PACKET_SIZE);
	MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB Enter pSocketInfo->RecCount = %d!",pData->RecCount,0,0);

	if(pData->RecCount > 0)
	{
		//reset the timeout timer
		MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB Enter!",0,0,0);

		pData->NoDataCount = 0;

		if(pData->RecCount > 0)
		{
		#ifdef MMS_TEST	
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
		if(pData != NULL && pData->pISocket != NULL)
		{
			ISOCKET_Readable(pData->pISocket, SocketReadableCB, pData);
		}

		return;
	}
	else if(pData->RecCount == AEE_NET_WOULDBLOCK)
	{
		pData->RecCount = 0;
		pData->NoDataCount = 0;
	}
	else if(pData->RecCount == AEE_NET_ERROR)
	{
		int err = 0;
		
		err = ISOCKET_GetLastError(pData->pISocket);
		MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB err = %d!",err,0,0);
		
		pData->RecCount = 0;
		pData->NoDataCount = 0;

		return;
	}
	else if(pData->RecCount == 0)
	{
		pData->NoDataCount++;

		MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB Enter pSocketInfo->NoDataCount = %d!",pData->NoDataCount,0,0);

		//when the RecCount equal to zero three times continiously,
		//to ensure the connection is closed
		if(pData->NoDataCount > 3)
		{
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

#endif
