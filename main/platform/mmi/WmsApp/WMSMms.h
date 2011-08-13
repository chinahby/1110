#ifndef _WMSMMS_H_
#define _WMSMMS_H_
/*==============================================================================
// 文件：WMSMMS.h
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
2011-08-04       wangliang            增加数据发送和接受接口，已经设置和获取代理的接口
==============================================================================*/

/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
#include "AEE.h"              // Standard AEE Declarations
#include "AEEShell.h"         // AEE Shell Services
#include "AEENet.h"

#define MMS_TEST
/////////////////////////////////////////////////////////////

#define MMS_DELREP_STATUS_FLAG_NOREQ 0x0000
#define MMS_DELREP_STATUS_FLAG_NORCV 0x0001
#define MMS_DELREP_STATUS_FLAG_OK 0x0002
#define MMS_DELREP_STATUS_FLAG_NG 0x0004
#define MMS_DELREP_STATUS_FLAG_NOCHK 0x0008

#define	MMS_STANDARD_PROGRESS 10

#define MMS_MAX_RECEIVED_MMS_PDU_SIZE (300*1024)
#define MMS_MAX_RECEIVED_MMS_CONTENT_SIZE (300*1024)

#define MMS_MAX_SUBJECT_SIZE 40
#define MMS_MAX_TRANSACTION_ID_SIZE 128
#define MMS_MAX_MESSAGE_ID_SIZE 40
#define MMS_MAX_ADDRESS_SIZE	2700
#define	MMS_MAX_ADDRESS_COUNT	70
#define MMS_MAX_SINGLE_ADDRESS_SIZE 90
#define MMS_MAX_CONTENT_LOCATION	100
#define MMS_MAX_FILE_NAME_PATH      256
#define MMS_MAX_TEXT_SIZE 256
#define	MMSCORE_MAX_SERVER_URL_LENGTH	256
#define MMSCORE_MAX_PROXY_HOST_LENGTH	64
#define MMSCORE_MAX_PROXY_PORT_LENGTH	4
#define MMSCORE_MAX_USERBOX_NAME_LENGTH	64

#define MMS_MAX_MESSAGEID_CHARSIZE 	128
#define MMS_MAX_ADDRESS_CHARSIZE	90

#define MMS_DEFAULT_PROXYHOST		("10.0.0.200")
#define MMS_DEFAULT_PROXYPORT		(80)

/* constant for http status code */
typedef enum HTTP_RET_CODE{
	HTTP_CODE_Continue							= 100,
	HTTP_CODE_Switching_Protocols				= 101,
	HTTP_CODE_OK								= 200,
	HTTP_CODE_Created							= 201,
	HTTP_CODE_Accepted							= 202,
	HTTP_CODE_Non_Authoritative_Information		= 203,
	HTTP_CODE_No_Content						= 204,
	HTTP_CODE_Reset_Content						= 205,
	HTTP_CODE_Partial_Content					= 206,
	HTTP_CODE_Multiple_Choices					= 300,
	HTTP_CODE_Moved_Permanently					= 301,
	HTTP_CODE_Found								= 302,
	HTTP_CODE_See_Other							= 303,
	HTTP_CODE_Not_Modified						= 304,
	HTTP_CODE_Use_Proxy							= 305,
	HTTP_CODE_Temporary_Redirect				= 307,
	HTTP_CODE_Bad_Request						= 400,
	HTTP_CODE_Unauthorized						= 401,
	HTTP_CODE_Payment_Required					= 402,
	HTTP_CODE_Forbidden							= 403,
	HTTP_CODE_Not_Found							= 404,
	HTTP_CODE_Method_Not_Allowed				= 405,
	HTTP_CODE_Not_Acceptable					= 406,
	HTTP_CODE_Proxy_Authentication_Required		= 407,
	HTTP_CODE_Request_Time_out					= 408,
	HTTP_CODE_Conflict							= 409,
	HTTP_CODE_Gone								= 410,
	HTTP_CODE_Length_Required					= 411,
	HTTP_CODE_Precondition_Failed				= 412,
	HTTP_CODE_Request_Entity_Too_Large			= 413,
	HTTP_CODE_Request_URI_Too_Large				= 414,
	HTTP_CODE_Unsupported_Media_Type			= 415,
	HTTP_CODE_Requested_range_not_satisfiable	= 416,
	HTTP_CODE_Expectation_Failed				= 417,
	HTTP_CODE_Internal_Server_Error				= 500,
	HTTP_CODE_Not_Implemented					= 501,
	HTTP_CODE_Bad_Gateway						= 502,
	HTTP_CODE_Service_Unavailable				= 503,
	HTTP_CODE_Gateway_Time_out					= 504,
	HTTP_CODE_HTTP_Version_not_supported		= 505
}HTTP_RET_CODE;

/*
** 彩信请求和通知类型
*/
typedef enum _mms_pdu_types
{
	MMS_PDU_SEND_REQ,
	MMS_PDU_NOTIFY_RESP_IND_DEFERRED,
	MMS_PDU_NOTIFY_RESP_IND_IMMEDIATE,
	MMS_PDU_NOTIFY_RESP_IND_REJECT,
	MMS_PDU_ACKNOWLEDGE_IND,
	MMS_PDU_READ_REC_IND,
	MMS_PDU_SEND_CONF,
	MMS_PDU_NOTIFICATION_IND,
	MMS_PDU_RETRIEVE_CONF,
	MMS_PDU_DELIVERY_IND,
	MMS_PDU_READ_ORIG_IND
}Mms_pdu_types;

typedef enum _mms_core_ans_
{
	MMC_OK =0, 
	MMC_PROT,

	MMC_NOMEM,
	MMC_GENERIC,
	MMC_WOULDBLOCK,
	MMC_SEND_REQUEST,
	MMC_RECEIVE_RESPONSE,
	MMC_STATUS_CODE,
	MMC_HTTP_CONTENT_TYPE,
	MMC_MIME_CONTENT_TYPE,
	MMC_PDU_LIMIT,
	MMC_PDU_TYPE,
	MMC_NO_URL,
	MMC_MODIFIED,
	MMC_DISKFULL,
	MMC_RESPONSE_STATUS_PERMANENT,
	MMC_RESPONSE_STATUS_TRANSIENT,
	MMC_MAX_REACHED,
	MMC_TRANSACTION_ID,
	MMC_REJECT,
	MMC_SAME_MSG,
	MMC_CANCEL,
	MMC_CORE_ANSs
};

typedef enum _mms_priorty_
{
	MMS_PRIORITY_HIGH = 0,
	MMS_PRIORITY_NORMAL,
	MMS_PRIORITY_LOW
}MMS_PRIORTY;

typedef enum _mms_message_class_
{
	MMS_CLASS_PERSONAL = 0,
	MMS_CLASS_ADVERTISEMENT,
	MMS_CLASS_IMFORMATIONAL,
	MMS_CLASS_AUTO
}MMS_MESSAGECLASS;

typedef enum _mms_deliveryRep_
{
	MMS_DELIVERYREP_NO = 0,
	MMS_DELIVERYREP_YES 
}MMS_DELIVERY_REP;

typedef enum _setting_validity_
{
	PREF_VALIDITY_12HOURS = 12,
	PREF_VALIDITY_DAY = 24,
	PREF_VALIDITY_WEEK = 168,
	PREF_VALIDITY_MONTH = 720,
	PREF_VALIDITY_MAXIMUM = 999
}MMS_VALIDITY;

typedef enum _mms_message_type_
{
	MMS_MESSAGE_PNG,
	MMS_MESSAGE_JPG,
	MMS_MESSAGE_GIF,
	MMS_MESSAGE_BMP,
	MMS_MESSAGE_3GP,
	MMS_MESSAGE_MID,
}MMS_MESSAGE_TYPE;
/*
** 收到的彩信内容
*/
typedef struct _wsp_decoder_data_message_received
{
	uint8 hFrom[MMS_MAX_SINGLE_ADDRESS_SIZE];
	uint8 hTo[MMS_MAX_SINGLE_ADDRESS_SIZE];
	uint8 hCc[MMS_MAX_SINGLE_ADDRESS_SIZE];
	uint8 hSubject[MMS_MAX_SUBJECT_SIZE];
	uint8 hMessageID[MMS_MAX_MESSAGEID_CHARSIZE];
	uint8 hTransactionID[MMS_MAX_TRANSACTION_ID_SIZE];
	uint8 hContentType[40];
	boolean	bDelRep;
	boolean	bReadRep;
	uint8* hBody;
	int	iBodyLen;	
	int	iRetrieveStatus;
	int	iDate;
	int	iPriority;
	int	iMessageClass;
	int	iDeliveryReport;
}MMS_WSP_DEC_MESSAGE_RECEIVED;

/*
** 收到的彩信通知
*/
typedef struct _wsp_decoder_data_notification_received
{
	uint8 hFrom[MMS_MAX_SINGLE_ADDRESS_SIZE];
	uint8 hSubject[MMS_MAX_SUBJECT_SIZE];
	uint8 hTransactionID[MMS_MAX_TRANSACTION_ID_SIZE];
	uint8 hContentLocation[MMS_MAX_CONTENT_LOCATION];
	int	iSize;
	int	iDate;
	int	iExpires;
	int	iPriority;
	int	iMessageClass;
	int	iDeliveryReport;
}MMS_WSP_DEC_NOTIFICATION_RECEIVED;

//
typedef struct _wsp_decoder_data_send_conf
{
	uint8 hMessageID[MMS_MAX_MESSAGEID_CHARSIZE];
	uint8 hTransactionID[MMS_MAX_TRANSACTION_ID_SIZE];
	int	iResponseStatus;	
}MMS_WSP_DEC_SEND_CONF;

typedef struct _wsp_decoder_data_readrep_received
{
	uint8 hFrom[MMS_MAX_SINGLE_ADDRESS_SIZE];
	uint8 hMessageID[MMS_MAX_MESSAGEID_CHARSIZE];
	int	iReadStatus;
	int	iDate;
}MMS_WSP_DEC_READREP_RECEIVED;

typedef struct _wsp_decoder_data_delrep_received
{
	uint8 hTo[MMS_MAX_SINGLE_ADDRESS_SIZE];
	uint8 hMessageID[MMS_MAX_MESSAGEID_CHARSIZE];
	int	iStatus;
	int	iDate;
}MMS_WSP_DEC_DELREP_RECEIVED;

typedef struct _wsp_encoder_data_sent
{
	uint8 hTo[MMS_MAX_SINGLE_ADDRESS_SIZE];
	uint8 hSubject[MMS_MAX_SUBJECT_SIZE];
	uint8 hCc[MMS_MAX_SINGLE_ADDRESS_SIZE];
	char FilePath[MMS_MAX_FILE_NAME_PATH];
	uint8 Text[MMS_MAX_TEXT_SIZE];
	MMS_MESSAGE_TYPE  bMsgType;
	boolean bIsText;
}MMS_WSP_ENCODE_SEND;

typedef union _wsp_decoder_data_
{
	MMS_WSP_DEC_MESSAGE_RECEIVED		message;
	MMS_WSP_DEC_NOTIFICATION_RECEIVED	notification;
	MMS_WSP_DEC_DELREP_RECEIVED			delrep;
	MMS_WSP_DEC_READREP_RECEIVED		readrep;
	MMS_WSP_DEC_SEND_CONF				sendconf;
}MMS_WSP_DEC_DATA;


typedef enum _HTTP_METHOD_TYPE_
{
	HTTP_METHOD_OPTIONS = 0,
	HTTP_METHOD_GET,
	HTTP_METHOD_HEAD,
	HTTP_METHOD_POST,
	HTTP_METHOD_PUT,
	HTTP_METHOD_DELETE,
	HTTP_METHOD_TRACE,
	HTTP_METHOD_CONNECT,
	HTTP_METHODS
}HTTP_METHOD_TYPE;

#define MMS_DECODER_ERROR_VALUE	0xFFFF
#define MMS_INT_MAX		0X7FFFFFFF
///////////////////////////////////////////////////////////////////////////////////////


#define MSG_MAX_PACKET_SIZE		(6*1024)//MUST bigger than max header length, because client must send whole header in singal packet
#define SOCKET_BUFFER_SIZE		(30*1024)
#define SENDDATAQUEUE_SIZE		(5)
#define NOQUEUESLOTAVAILABLE	(-1)
#define SOCKET_CONNECT_TIMER	(2*60000)// 2 min

typedef struct tag_MSocket
{
	int16 RecCount;
	uint8 RecBuffer[MSG_MAX_PACKET_SIZE];

	boolean				bConnected;
	ISocket*		    pISocket;
	INPort				Port;
	uint8				*pSendData;
	uint16				nDataLen;
	uint16				nBytesSent;

	uint16 NoDataCount;

	AEEDNSResult DNSResult;
	AEECallback DNSCallback;
}MMSSocket;

/**
Called to create a new socket, according to the input type.
The newly-created socket object is returned as an out parameter

\param	pps		[out]   The newly created socket object is returned in this parameter.
\param	nType	[in]    Indicates the socket type. For the IM workshop this will always be  MG4SOCKTYPE_STREAM

\return ETrue if a new socket is created
*/
boolean  MMSSocketNew(MMSSocket **pps, uint16 nType);
boolean  MMSSocketClose(MMSSocket **pps);
boolean  MMSSocketConnect (MMSSocket *ps, char *pszServer, uint16 nPort);

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
boolean  MMSSocketSend (MMSSocket *ps, const uint8 *pBuf, uint16 nLen);
boolean  MMSSocketRecv (MMSSocket *ps, uint8 *pBuf, uint16 *pLen);
//////////////////////////////////////////////////////////////////////////////
int MMS_PDU_PutMessageClass(int in_MessageClass, int* out_MessageClass);
int MMS_PDU_PutDeliveryReport(int in_DelRep, int* out_DelRep);
int MMS_PDU_PutPriority(int in_priority, int* out_priority);
int MMS_PDU_PutDeliveryReportStatus(int in_Status, int* out_Status);
int MMS_SEND_TEST(uint8 *buffer);
int MMS_SEND_PDU(HTTP_METHOD_TYPE type,uint8* hPDU, int hLen);
int MMS_PDU_Encode(MMS_WSP_ENCODE_SEND* encdata,uint8* hPDU, int* hLen, uint8 ePDUType);
int MMS_PDU_Decode(MMS_WSP_DEC_DATA* decdata,uint8* ptr, int datalen,uint8* ePDUType);
int MMS_WSP_DecodeMessage(uint8* pData, int iDataLen,  uint8* hContentType, uint8* hBody,int* iBodyLen);

boolean MMS_GetProxySettings(boolean *bUseProxy,char* hProxyHost, int* iProxyPort);
boolean MMS_SetProxySettings(boolean bUseProxy,char* hProxyHost, int iProxyPort);
#endif