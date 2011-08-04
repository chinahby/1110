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


/*
** 彩信里面用到的数据类型
*/
#if 0
const char* db_mms2text[76] =
{
	"*/*",
	"text/*",
    "text/html",
    "text/plain",
    "text/x-hdml",
    "text/x-ttml"),
    "text/x-vCalendar"),
    "text/x-vCard"),
    "text/vnd.wap.wml"),
    ("text/vnd.wap.wmlscript"),
    ("text/vnd.wap.wta-event"),
    ("multipart/*"),
    ("multipart/mixed"),
    ("multipart/form-data"),
    ("multipart/byterantes"),
    ("multipart/alternative"),  //0x0f
    ("application/*"),
    ("application/java-vm"),
    ("application/x-www-form-urlencoded"),
    ("application/x-hdmlc"),
    ("application/vnd.wap.wmlc"),
    ("application/vnd.wap.wmlscriptc"),
    ("application/vnd.wap.wta-eventc"),
    ("application/vnd.wap.uaprof"),
    ("application/vnd.wap.wtls-ca-certificate"),
    ("application/vnd.wap.wtls-user-certificate"),
    ("application/x-x509-ca-cert"),
    ("application/x-x509-user-cert"),
    ("image/*"),
    ("image/gif"),
    ("image/jpeg"),
    ("image/tiff"),   //0x1f
    ("image/png"),
    ("image/vnd.wap.wbmp"),
    ("application/vnd.wap.multipart.*"),
    ("multipart/mixed"),
/*    ("application/vnd.wap.multipart.mixed"),*/
    ("application/vnd.wap.multipart.form-data"),
    ("application/vnd.wap.multipart.byteranges"),
/*    ("application/vnd.wap.multipart.alternative"),*/
    ("multipart/alternative"),
    ("application/xml"),
    ("text/xml"),
    ("application/vnd.wap.wbxml"),
    ("application/x-x968-cross-cert"),
    ("application/x-x968-ca-cert"),
    ("application/x-x968-user-cert"),
    ("text/vnd.wap.si"),
    ("application/vnd.wap.sic"),
    ("text/vnd.wap.sl"),  //0x2f
    ("application/vnd.wap.slc"),
    ("text/vnd.wap.co"),
    ("application/vnd.wap.coc"),
    ("multipart/related"),
/*    ("application/vnd.wap.multipart.related"),*/
    ("application/vnd.wap.sia"),
    ("text/vnd.wap.connectivity-xml"),
    ("application/vnd.wap.connectivity-wbxml"),
    ("application/pkcs7-mime"),
    ("application/vnd.wap.hashed-certificate"),
    ("application/vnd.wap.signed-certificate"),
    ("application/vnd.wap.cert-response"),
    ("application/xhtml+xml"),
    ("application/wml+xml"),
    ("text/css"),
    ("application/vnd.wap.mms-message"),
    ("application/vnd.wap.rollover-certificate"),
    ("application/vnd.wap.locc+wbxml"),
    ("application/vnd.wap.loc+xml"),
    ("application/vnd.syncml.dm+wbxml"),
    ("application/vnd.syncml.dm+xml"),
    ("application/vnd.syncml.notification"),
    ("application/vnd.wap.xhtml+xml"),
    ("application/vnd.wv.csp.cir"),
    ("application/vnd.oma.dd+xml"),
    ("application/vnd.oma.drm.message"),
    ("application/vnd.oma.drm.content"),
    ("application/vnd.oma.drm.rights+xml"),
    ("application/vnd.oma.drm.rights+wbxml"),
};
#endif

int MMS_PDU_Encode(MMS_WSP_DEC_DATA* decdata, char* hPDU, int* hLen, uint8 ePDUType)
{
	return 0;
}

int MMS_PDU_Decode(MMS_WSP_DEC_DATA* decdata,uint8* ptr, int datalen,uint8 *ePDUType)
{
	int len,i=0,iDataOffset,j;
	uint8* pchar;

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
				return MMC_OK;
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
