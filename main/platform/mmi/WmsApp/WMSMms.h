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
#include "AEECallback.h"
#ifndef USES_RELEASE_VERSION
//#define MMS_TEST
#endif
/////////////////////////////////////////////////////////////
#define MAX_EMAILADD_LEN               64 
#define MMS_DELREP_STATUS_FLAG_NOREQ 0x0000
#define MMS_DELREP_STATUS_FLAG_NORCV 0x0001
#define MMS_DELREP_STATUS_FLAG_OK 0x0002
#define MMS_DELREP_STATUS_FLAG_NG 0x0004
#define MMS_DELREP_STATUS_FLAG_NOCHK 0x0008
#define MSG_MAX_PACKET_SIZE		(400*1024)//(400*1024)//(300*1024)

#define	MMS_STANDARD_PROGRESS 10
#define MMS_VALUE_USELESSNESS -1
#define MMS_MAX_RECEIVED_MMS_PDU_SIZE MSG_MAX_PACKET_SIZE
#define MMS_MAX_RECEIVED_MMS_CONTENT_SIZE MSG_MAX_PACKET_SIZE

#define MMS_MAX_SUBJECT_SIZE 40
#define MMS_MAX_TRANSACTION_ID_SIZE 128
#define MMS_MAX_MESSAGE_ID_SIZE 40
#define MMS_MAX_ADDRESS_SIZE	2700
#define	MMS_MAX_ADDRESS_COUNT	70
#define MMS_MAX_SINGLE_ADDRESS_SIZE MAX_EMAILADD_LEN*20//最多同时群发20个联系人
#define MMS_MAX_CONTENT_LOCATION	100
#define MMS_MAX_CONTENT_ID	100
#define MMS_MAX_CONTENT_TYPE	100
#define MMS_MAX_CONTENT_NAME	256
#define MMS_MAX_CONTENT_ENCODE	32
#define MMS_MAX_FILE_NAME_PATH      256
#define MMS_MAX_TEXT_SIZE 256
#define	MMSCORE_MAX_SERVER_URL_LENGTH	256
#define MMSCORE_MAX_PROXY_HOST_LENGTH	64
#define MMSCORE_MAX_PROXY_PORT_LENGTH	4
#define MMSCORE_MAX_USERBOX_NAME_LENGTH	64

#define MMS_MAX_MESSAGEID_CHARSIZE 	128
#define MMS_MAX_ADDRESS_CHARSIZE	90
#define MMS_MAX_TIME_SIZE  
#define MMS_DEFAULT_PROXYHOST		("10.0.0.200")
#define MMS_DEFAULT_PROXYPORT		(80)
#define OTHER_MIME_BASE "other/"
#define WMSMMS_FRAGMENTCOUNT 10

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

typedef enum
{
    WMS_PDU_STATUS_Expired,
    WMS_PDU_STATUS_Retrieved,
    WMS_PDU_STATUS_Rejected,
    WMS_PDU_STATUS_Deferred,
    WMS_PDU_STATUS_Unrecognised
    
}WMS_PDU_STATUS_TYPES;
typedef enum
{
    WMS_MMS_PDU_MSendReq,
    WMS_MMS_PDU_MSendConf,
    WMS_MMS_PDU_MNotificationInd,
    WMS_MMS_PDU_MNotifyrespInd,
    WMS_MMS_PDU_MRetrieveConf,
    WMS_MMS_PDU_MAcknowledgeInd,
    WMS_MMS_PDU_MDeliveryInd,
    WMS_MMS_PDU_MReadRecInd,
    
    WMS_MMS_PDU_WSPHTTPGETreq,
    WMS_MMS_PDU_WSPHTTPRESEND,
    WMS_MMS_PDU_DRAFT,
}WMS_MMS_PDU_MessageTypeValue;

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
}MMS_CORE_RET;

typedef enum
{
    MMS_RESPONSE_OK,
    
    // Errors:
    //Error-unspecified
    MMS_RESPONSE_UNSPECIFIED,

    //Error-service-denied
    MMS_RESPONSE_SERVICCEDENIED,

    //Error-message-format-corrupt
    MMS_RESPONSE_MSGFORMATCORRUPT,

    //Error-sending-address-unresolved
    MMS_RESPONSE_SENDINGADDRUNRESOLVED,

    //Error-message-not-found
    MMS_RESPONSE_MSGNOTFOUND,

    //Error-network-problem
    MMS_RESPONSE_NETWORK_PROBLEM,

    //Error-content-not-accepted
    MMS_RESPONSE_CONTENTNOTACCEPTED,

    //Error-unsupported-message
    MMS_RESPONSE_UNSUPPORTEDMSG
} MMS_RESPONSE_STATUS;

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

typedef enum _mms_from_field_ 
{
    ADDRESS_PRESENT,
    INSERT_ADDRESS
}MMS_FROM_FIELD;

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
	MMS_MESSAGE_TYPE_IMAGE,
	MMS_MESSAGE_TYPE_AUDIO,
	MMS_MESSAGE_TYPE_VIDEO,
	MMS_MESSAGE_TYPE_TEXT,
	MMS_MESSAGE_TYPE_UNKNOW
}MMS_MESSAGE_TYPE;

typedef enum _mms_message_image_type_
{
	MMS_MESSAGE_JPG,
	MMS_MESSAGE_PNG,
	MMS_MESSAGE_GIF,
	MMS_MESSAGE_WBMP,
	MMS_MESSAGE_UNKNOW
}MMS_MESSAGE_IMAGE_TYPE;

    
typedef struct _wsp_decoder_data_fragment_
{
	uint8 hContentType[MMS_MAX_CONTENT_TYPE];//类型
	uint8 hContentLocation[MMS_MAX_CONTENT_LOCATION];
	uint8 hContentID[MMS_MAX_CONTENT_ID];
	uint8 hContentName[MMS_MAX_CONTENT_NAME];
	uint8 hContentEnCode[MMS_MAX_CONTENT_ENCODE];
	uint8 *pContent;//彩信元素内容
	uint32 size;//大小
}WSP_DEC_DATA_FRAGMENT;

typedef struct _wsp_encoder_data_fragment_
{
	uint8 hContentType[MMS_MAX_CONTENT_TYPE];
	uint8 hContentLocation[MMS_MAX_CONTENT_LOCATION];
	uint8 hContentID[MMS_MAX_CONTENT_ID];
	uint8 hContentName[MMS_MAX_CONTENT_NAME];
	uint8 hContentEnCode[MMS_MAX_CONTENT_ENCODE];
	uint8 hContentFile[MMS_MAX_FILE_NAME_PATH];
	uint8 hContentText[MMS_MAX_TEXT_SIZE];
	uint8* pBuf;
	uint32 nBufLen;
	char* pType;
}WSP_ENCODE_DATA_FRAGMENT;

typedef struct _wsp_mms_encode_data_
{
	WSP_ENCODE_DATA_FRAGMENT fragment[WMSMMS_FRAGMENTCOUNT];
	int				 frag_num;
}WSP_MMS_ENCODE_DATA;

typedef struct _wsp_mms_data_
{
	WSP_DEC_DATA_FRAGMENT head_info;
	WSP_DEC_DATA_FRAGMENT fragment[WMSMMS_FRAGMENTCOUNT];
	int				 frag_num;
}WSP_MMS_DATA;

enum _mms_wsp_headers_names
{
	MMS_WSP_HEADER_CONTENT_TYPE,
	MMS_WSP_HEADER_CONTENT_ID,
	MMS_WSP_HEADER_CONTENT_LOCATION,
	MMS_WSP_HEADER_CONTENT_DISPOSITION,
	MMS_WSP_HEADER_NONE
};

enum _mms_wsp_headers_values
{
	MMS_WSP_HEADER_VALUE_MULTIPART_RELATED,
	MMS_WSP_HEADER_VALUE_MULTIPART_MIXED,
	MMS_WSP_HEADER_VALUE_TEXT_PLAIN,
/*	MMS_WSP_HEADER_VALUE_IMAGE_GIF,
	MMS_WSP_HEADER_VALUE_IMAGE_JPEG,
	MMS_WSP_HEADER_VALUE_IMAGE_PNG,
	MMS_WSP_HEADER_VALUE_IMAGE_WBMP,
	MMS_WSP_HEADER_VALUE_VCARD,
	MMS_WSP_HEADER_VALUE_VCALENDAR,*/
	MMS_WSP_HEADER_VALUE_APPLICATION_SMIL,
	MMS_WSP_HEADER_VALUE_KNOWN,
	MMS_WSP_HEADER_VALUE_UNKNOWN,
	MMS_WSP_HEADER_VALUE_NONE
};

/*
** 彩信内容
*/
typedef struct _wsp_decoder_data_message_received
{
	uint8 hFrom[MMS_MAX_SINGLE_ADDRESS_SIZE];
	uint8 hTo[MMS_MAX_SINGLE_ADDRESS_SIZE];
	uint8 hCc[MMS_MAX_SINGLE_ADDRESS_SIZE];
	uint8 hSubject[MMS_MAX_SUBJECT_SIZE];
	uint8 hMessageID[MMS_MAX_MESSAGEID_CHARSIZE];
	uint8 hTransactionID[MMS_MAX_TRANSACTION_ID_SIZE];
	uint8 hContentType[MMS_MAX_CONTENT_TYPE];
	WSP_MMS_DATA mms_data;
	boolean	bDelRep;
	boolean	bReadRep;
	uint8* hBody;
	int	iBodyLen;	
	int	iRetrieveStatus;
	int	iDate;
	int	iPriority;
	int	iMessageClass;
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
}MMS_WSP_DEC_NOTIFICATION_RECEIVED;

//
typedef struct _wsp_decoder_data_send_conf
{
	uint8 hMessageID[MMS_MAX_MESSAGEID_CHARSIZE];
	uint8 hTransactionID[MMS_MAX_TRANSACTION_ID_SIZE];
	int	iResponseStatus;	
}MMS_WSP_DEC_SEND_CONF;

// 通知反馈
typedef struct _wsp_encoder_data_notifty_resp
{
	uint8 hTransactionID[MMS_MAX_TRANSACTION_ID_SIZE];
	int iMMSVersion;
	int	iStatus;	
	boolean bReportAllowed;
}MMS_WSP_ENC_NOTIFY_RESP;

// 彩信接收确认
typedef struct _wsp_encoder_data_delivery_acknowledgement
{
	uint8 hTransactionID[MMS_MAX_TRANSACTION_ID_SIZE];
	int iMMSVersion;	
	boolean bReportAllowed;
}MMS_WSP_ENC_DELIVERY_ACKNOWLEDGEMENT;

typedef struct _wsp_encoder_data_read_report
{
	uint8 hTransactionID[MMS_MAX_TRANSACTION_ID_SIZE];
	uint8 hMessageID[MMS_MAX_MESSAGEID_CHARSIZE];
	uint8 hFrom[MMS_MAX_SINGLE_ADDRESS_SIZE];
	uint8 hTo[MMS_MAX_SINGLE_ADDRESS_SIZE];
	int iMMSVersion;	
	boolean bReadStutas;
	int	iMessageClass;
}MMS_WSP_ENC_READ_REPORT;


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

typedef struct _wsp_encoder_data_message_send
{
	uint8 hFrom[MMS_MAX_SINGLE_ADDRESS_SIZE];
	uint8 hTo[MMS_MAX_SINGLE_ADDRESS_SIZE];//这里的大小应该变成MAX_EMAILADD_LEN*最大允许群发的个数
	uint8 hCc[MMS_MAX_SINGLE_ADDRESS_SIZE];
	uint8 hBcc[MMS_MAX_SINGLE_ADDRESS_SIZE];
	uint8 hSubject[MMS_MAX_SUBJECT_SIZE];
	uint8 hTransactionID[MMS_MAX_TRANSACTION_ID_SIZE];
	uint8 hContentType[MMS_MAX_CONTENT_TYPE];
	WSP_MMS_ENCODE_DATA mms_data;
	int8	bDelRep;
	int8	bReadRep;
	uint8* hBody;
	int	iBodyLen;	
	int	iRetrieveStatus;
	int	iDate;
	int	iPriority;
	int	iMessageClass;
	int8 bSenderVisibility;
	int iMMSVersion;
	int iExpiry;
	int iDeliveryTime;
}MMS_WSP_MESSAGE_SEND;

typedef struct _wsp_encoder_data_send
{
    MMS_WSP_MESSAGE_SEND                *pMessage;
    MMS_WSP_ENC_NOTIFY_RESP             *pNotifyresp;
    MMS_WSP_ENC_DELIVERY_ACKNOWLEDGEMENT *pDeliveryacknowledgement;
    MMS_WSP_ENC_READ_REPORT             *pReadReport;
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


//#define MSG_MAX_PACKET_SIZE		(6*1024)//MUST bigger than max header length, because client must send whole header in singal packet


#define SOCKET_BUFFER_SIZE		(30*1024)
#define SENDDATAQUEUE_SIZE		(5)
#define NOQUEUESLOTAVAILABLE	(-1)
#define SOCKET_CONNECT_TIMER	(2*60000)// 2 min

typedef enum _WTP_PDU_ClientTransaction_
{
    WTP_PDU_NOT_ALLOWED,
    WTP_PDU_INVOKE,
    WTP_PDU_RESULT,
    WTP_PDU_ACK,
    WTP_SEGMENT_INVOKE,
    WTP_SEGMENT_RESULT,
    WTP_NEGATIVE_ACK
}WTP_PDU_ClientTransaction;

typedef struct tag_MSocket
{
    uint32 RecCount;
    uint8 RecBuffer[MSG_MAX_PACKET_SIZE];
    
    boolean				bConnected;
    ISocket*		    pISocket;
    INPort				Port;
    uint8*              pSendData;
    uint32              nDataLen;
    uint32				nBytesSent;

    uint32 NoDataCount;
    uint32 nODataLen;
    uint8* pOData;
    int nState;

    int16 wParam;
    uint32 dwParam;
    AEEDNSResult DNSResult;
    AEECallback DNSCallback;
    uint32 nError;
}MMSSocket;


/**
Called to create a new socket, according to the input type.
The newly-created socket object is returned as an out parameter

\param	pps		[out]   The newly created socket object is returned in this parameter.
\param	nType	[in]    Indicates the socket type. For the IM workshop this will always be  MG4SOCKTYPE_STREAM

\return ETrue if a new socket is created
*/
boolean WMS_MMSState(int nState,int16 wParam,uint32 dwParam);

//boolean  MMSSocketNew (MMSSocket **pps, uint16 nType);
//boolean  MMSSocketClose(MMSSocket **pps);
//boolean  MMSSocketConnect (MMSSocket *ps, char *pszServer, uint16 nPort);

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
//boolean  MMSSocketSend (MMSSocket *ps, const uint8 *pBuf, uint16 nLen);
//boolean  MMSSocketRecv (MMSSocket *ps, uint8 *pBuf, uint16 *pLen);
//////////////////////////////////////////////////////////////////////////////
int MMS_PDU_PutMessageClass(int in_MessageClass, int* out_MessageClass);
int MMS_PDU_PutDeliveryReport(int in_DelRep, boolean* out_DelRep);
int MMS_PDU_PutPriority(int in_priority, int* out_priority);
int MMS_PDU_PutDeliveryReportStatus(int in_Status, int* out_Status);
int WMS_MMS_SEND_TEST(uint8 *buffer, char* sendNumber);
int WMS_MMS_Decode_TEST(char *file);
boolean WMS_MMS_DeleteMMSALL(int nKind);
int WMS_MMS_SEND_PDU(WMS_MMS_PDU_MessageTypeValue type,uint8* hPDU,MMS_WSP_ENCODE_SEND* pData);
int WMS_MMS_PDU_Encode(MMS_WSP_ENCODE_SEND* encdata, uint8* hPDU, uint8 ePDUType);
int WMS_MMS_PDU_Decode(MMS_WSP_DEC_DATA* decdata,uint8* ptr, int datalen,uint8* ePDUType);
int WMS_MMS_WSP_DecodeMessage(uint8* pData, int iDataLen,  WSP_MMS_DATA* pContent);

//根据附件生成布局文件。
int WMS_MMS_CreateSMIL(uint8 *out_buf,int buf_size,WSP_MMS_ENCODE_DATA data);

boolean MMS_GetProxySettings(boolean *bUseProxy,char* hProxyHost, int* iProxyPort);
boolean MMS_SetProxySettings(boolean bUseProxy,char* hProxyHost, int iProxyPort);

#endif
