/*==============================================================================
// 文件：WMSMMSTest.c
//        2011-08-13初始版本(Init Version)
//        版权所有(c) 2011 WaterWorld Tech. CO.,LTD.
//        
// 文件说明：这个文件主要实现和彩信相关的编解码用的外部接口
//        
// 作者：
// 创建日期：2011-08-13
// 当前版本：Init Version
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------   -----------------------------------------------
2011-08-13       wangliang            彩信测试程序
==============================================================================*/

#include "customer.h"
#include "OEMFeatures.h"
#include "OEMClassIds.h"


#include "AEEShell.h"
#include "AEE_OEM.h"
#include "AEEModGen.h"
#include "AEEModTable.h"
#include "AEEMenu.h"
#include "AEEStdLib.h"
#include "AEEAnnunciator.h"
#include "application.h"
#include "application_priv.h" 

#include "ContApp.h"

#include "Appscommon.h"
#include "AEENet.h"

#if !defined( AEE_SIMULATOR)
#include "AEERUIM.h" 
#include "OEMCFGI.h"
#endif //#if !defined( AEE_SIMULATOR)
#include "err.h"

#include "WMSMms.h"
#include "WMSMmsTest.h"
#include "WMSApp.h"
#include "Wms.h"
//int MMS_SocketTest(void);
#ifdef FEATURE_USES_MMS


#define SOCKET_STATE_IDLE            0   // socket not opened
#define SOCKET_STATE_CONNECTING      1   // waiting on connect
#define SOCKET_STATE_CONNECTED       2   // connected
#define SOCKET_STATE_WRITING         3   // waiting to complete write
#define SOCKET_STATE_READING         4   // waiting to complete read
	
//#define MSG_MAX_PACKET_SIZE		(6*1024)
//#define SOCKET_BUFFER_SIZE		(30*1024)
//#define SENDDATAQUEUE_SIZE		(5)
//#define NOQUEUESLOTAVAILABLE	(-1)
//#define SOCKET_CONNECT_TIMER	(2*60000)// 2 min
	
#define DBGPRINTF_EX_FORMAT    "*dbgprintf-%d* %s:%d"
#define SOCKET_DEBUG(arg) __DBGPRINTF(DBGPRINTF_EX_FORMAT,5,__FILE__,__LINE__),__DBGPRINTF arg 
	
	
#define IBASE_RELEASE_IPTR(pI) \
		 do \
		 { \
		   if (pI != NULL) \
		   { \
			 (void)IBASE_Release((IBase*)pI); \
			 pI = NULL; \
		   } \
		 } while (FALSE)
		 
	
typedef struct _SocketInfoTag_{
	ISocket 				*pISocket;		/* BREW ISocket 实例句柄. */
	char					 pAddress[64];
	INPort						wPort;
	AEEDNSResult			DNSResult;
	AEECallback 			DNSCallback;
	uint8					status;
	int32					RecCount;
	uint32					NoDataCount;
	uint8					RecBuffer[MSG_MAX_PACKET_SIZE];
	uint8					SendBuffer[MSG_MAX_PACKET_SIZE];
	uint32					nDataLen;
	uint32					nBytesSent;
    AECHAR                  sendNumber[MAX_PH_DIGITS+1];
    uint8                   SocketReadStatus;//发送彩信后返回的状态
    boolean                 SendFinsh;
}SocketInfoTag;

static SocketInfoTag pSocketInfoTag = {0};

//socket is readable,notify m4 client to receive
static void SocketReadableCB(void * pData)
{
	SocketInfoTag* pSocketInfo = (SocketInfoTag*)pData;
	int i;
	char buffer[100];
	MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB Enter!",0,0,0);
    
	if(pSocketInfo == NULL)
	{
		return;
	}
	else if(pSocketInfo->pISocket == NULL)
	{
		return;
	}
	pSocketInfoTag.SendFinsh = TRUE;
	pSocketInfo->RecCount = (uint16)ISOCKET_Read(pSocketInfo->pISocket, pSocketInfo->RecBuffer, MSG_MAX_PACKET_SIZE);
	MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB Enter pSocketInfo->RecCount = %d!",pSocketInfo->RecCount,0,0);

	if(pSocketInfo->RecCount > 0)
	{
		//reset the timeout timer
		MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB Enter!",0,0,0);

		for(i=0; i<pSocketInfo->RecCount; i=i+100)
		{
			MEMSET(buffer,0,100);
			MEMCPY(buffer,pSocketInfo->RecBuffer+i,100);
			SOCKET_DEBUG(("Data:%d %s",i,buffer));
		}

		pSocketInfo->NoDataCount = 0;

		if(pSocketInfo->RecCount > 0)//已经发送完成并收到了返回的数据
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

		    pIFile = IFILEMGR_OpenFile( pIFileMgr, "fs:/hsmm/pictures/send.txt", _OFM_READWRITE);
			if ( pIFile != NULL )
	        {
	            IFILE_Seek(pIFile, _SEEK_START, 0);
	            IFILE_Write( pIFile, pSocketInfo->RecBuffer, pSocketInfo->RecCount);

	            MSG_FATAL("IFILEMGR_OpenFile pSocketInfoTag.RecCount=%d",pSocketInfo->RecCount,0,0);
	            IFILE_Release( pIFile);
	            pIFile = NULL;
	            IFILEMGR_Release(pIFileMgr);
	            pIFileMgr = NULL;
	        }
	        else
	        {
				pIFile = IFILEMGR_OpenFile( pIFileMgr, "fs:/hsmm/pictures/send.txt", _OFM_CREATE);
				if ( pIFile != NULL )
		        {
		            IFILE_Write( pIFile, pSocketInfo->RecBuffer, pSocketInfo->RecCount);

		            MSG_FATAL("IFILEMGR_OpenFile pSocketInfoTag.RecCount=%d",pSocketInfo->RecCount,0,0);
		            IFILE_Release( pIFile);
		            pIFile = NULL;
		            IFILEMGR_Release(pIFileMgr);
		            pIFileMgr = NULL;
		        }
	        }
            if((STRISTR((char*)(pSocketInfo->RecBuffer), "HTTP/1.1 200") != NULL) ||
               (STRISTR((char*)(pSocketInfo->RecBuffer), "HTTP/1.1200") != NULL))
            {
                MSG_FATAL("SocketReadStatus == HTTP_CODE_OK",0,0,0);
                pSocketInfoTag.SocketReadStatus = HTTP_CODE_OK;
            }
            else
            {
                MSG_FATAL("SocketReadStatus == 0",0,0,0);
                pSocketInfoTag.SocketReadStatus = 0;
            }
		}
		
		//client may destroy socket in M4_SOCKETNOTIFICATION_RECEIVE synchronously
		//so we must register call-back first, otherwise the pointer may invaliable
		if(pSocketInfo != NULL && pSocketInfo->pISocket != NULL)
		{
			ISOCKET_Readable(pSocketInfo->pISocket, SocketReadableCB, pSocketInfo);
		}
        {
            IShell *pShell = AEE_GetShell();
            (void)ISHELL_PostEventEx(pShell,
                                     EVTFLG_ASYNC, 
                                     AEECLSID_WMSAPP, 
                                     EVT_MMS_MSG_SEND_FINSH,
                                     0, 
                                     0);
		}
		return;
	}
	else if(pSocketInfo->RecCount == AEE_NET_WOULDBLOCK)//发送完成但返回的数据有问题，表示发送不成功
	{
		pSocketInfo->RecCount = 0;
		pSocketInfo->NoDataCount = 0;
	}
	else if(pSocketInfo->RecCount == AEE_NET_ERROR)//发送完成但返回的数据有问题，表示发送不成功
	{
		int err = 0;
		
		err = ISOCKET_GetLastError(pSocketInfo->pISocket);
		MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB err = %d!",err,0,0);
		
		pSocketInfo->RecCount = 0;
		pSocketInfo->NoDataCount = 0;

		return;
	}
	else if(pSocketInfo->RecCount == 0)//发送完成但返回的数据有问题，表示发送不成功
	{
		pSocketInfo->NoDataCount++;

		MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB Enter pSocketInfo->NoDataCount = %d!",pSocketInfo->NoDataCount,0,0);

		//when the RecCount equal to zero three times continiously,
		//to ensure the connection is closed
		if(pSocketInfo->NoDataCount > 3)
		{
			return;
		}
	}

	//client may destroy socket in M4_SOCKETNOTIFICATION_RECEIVE synchronously
	if(pSocketInfo != NULL && pSocketInfo->pISocket != NULL)
	{
		MSG_FATAL("[MSG][DeviceSocket]: SocketReadableCB setSocketReadableCB!",0,0,0);
		ISOCKET_Readable(pSocketInfo->pISocket, SocketReadableCB, pSocketInfo);
	}
}


static void MMI_SocketSend(void *pUserData)
{
	SocketInfoTag* pSocketInfo = (SocketInfoTag*)pUserData;
	int32 bytes_written = 0;
	uint32 nLen = pSocketInfo->nDataLen - pSocketInfo->nBytesSent;
	
	MSG_FATAL("[MSG][DeviceSocket]: MMI_SocketSend Enter nLen = %d,pSocketInfo->nBytesSent=%d",nLen,pSocketInfo->nBytesSent,0);
	
	if(nLen > 0)
	{
		bytes_written = ISOCKET_Write(pSocketInfo->pISocket,
								pSocketInfo->SendBuffer + pSocketInfo->nBytesSent,
								nLen);
		MSG_FATAL("[MSG][DeviceSocket]: MMI_SocketSend Enter nLen = %d,bytes_written = %d",nLen,bytes_written,0);
	}
	
	if(bytes_written > 0)
	{
		//reset the timeout timer
		if((uint32)bytes_written < nLen)
		{
			pSocketInfo->nBytesSent += (uint32)bytes_written;
		}
		else if((uint32)bytes_written == nLen)
		{
			//client may destroy socket in M4_SOCKETNOTIFICATION_RECEIVE synchronously
			//so we must register call-back first, otherwise the pointer may invaliable
			MSG_FATAL("[MSG][DeviceSocket]: MMI_SocketSend Send-Queue Empty!",0,0,0);
			ISOCKET_Readable(pSocketInfo->pISocket, SocketReadableCB, pSocketInfo);
			return;
		}
	}
	else if(bytes_written == AEE_NET_ERROR)
	{
		int nError = 0;

		nError = ISOCKET_GetLastError(pSocketInfo->pISocket);
		MSG_FATAL("[MSG][DeviceSocket]: MMI_SocketSend nError = %d", nError,0,0);

		return;
	}
	else
	{
		//do nothing
	}

	if(pSocketInfo->SendBuffer != NULL)
	{
		ISOCKET_Writeable(pSocketInfo->pISocket, MMI_SocketSend, pSocketInfo);
	}
	else
	{
		MSG_FATAL("[MSG][DeviceSocket]: MMI_SocketSend Send-Queue Empty!",0,0,0);
	}
}

extern int WMS_MMS_SEND_TEST(uint8 *buffer, char* sendNumber);

static void ConnectError(void* pUser, int nError)
{
	SocketInfoTag *pSocketInfo = (SocketInfoTag*)pUser;
	int32 bytes_written = 0;
	int32 bytes_read = 0;
	IFile* pIFile = NULL;
    IFileMgr *pIFileMgr = NULL;
    int result = SUCCESS;
	int i;
#if 0
			
	switch(nError)
	{
		case AEE_NET_SUCCESS:
        {
            char sendNumber[MAX_PH_DIGITS+1] = {0};
            WSTRTOSTR(pSocketInfo->sendNumber, sendNumber, MAX_PH_DIGITS+1);
            DBGPRINTF("sendNumber=%s", sendNumber);
			pSocketInfo->nDataLen  = (uint32)WMS_MMS_SEND_TEST(pSocketInfo->SendBuffer, sendNumber);
			MSG_FATAL("[MSG][DeviceSocket]: WMS_MMS_SEND_TEST nDataLen=%d",pSocketInfo->nDataLen,0,0);
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
                //only for test ,
			    pIFile = IFILEMGR_OpenFile( pIFileMgr, "fs:/hsmm/pictures/send_test1.txt", _OFM_READWRITE);
				if ( pIFile != NULL )
		        {
		            IFILE_Seek(pIFile, _SEEK_START, 0);
		            IFILE_Write( pIFile, pSocketInfo->SendBuffer, pSocketInfo->nDataLen);

		            MSG_FATAL("IFILEMGR_OpenFile pSocketInfoTag.nDataLen=%d",pSocketInfo->nDataLen,0,0);
		            IFILE_Release( pIFile);
		            pIFile = NULL;
		            IFILEMGR_Release(pIFileMgr);
		            pIFileMgr = NULL;
		        }
		        else
		        {
					pIFile = IFILEMGR_OpenFile( pIFileMgr, "fs:/hsmm/pictures/send_test1.txt", _OFM_CREATE);
					if ( pIFile != NULL )
			        {
			            IFILE_Write( pIFile, pSocketInfo->SendBuffer, pSocketInfo->nDataLen);

			            MSG_FATAL("IFILEMGR_OpenFile pSocketInfoTag.nDataLen=%d",pSocketInfo->nDataLen,0,0);
			            IFILE_Release( pIFile);
			            pIFile = NULL;
			            IFILEMGR_Release(pIFileMgr);
			            pIFileMgr = NULL;
			        }
		        }
			}

			pSocketInfo->nBytesSent = (uint32)0;
			pSocketInfo->RecCount = (uint32)0;
			MSG_FATAL("nDataLen %d",pSocketInfo->nDataLen,0,0);
			ISOCKET_Writeable(pSocketInfo->pISocket, MMI_SocketSend, pSocketInfo);

			break;
		/* 若超时错误, 则 ... */
		}
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
#endif	
}

int MMS_SocketTest(AECHAR *sendNumber)
{
	int result = SUCCESS;
	INetMgr *pINetMgr = NULL;
	ISocket *pISocket = NULL;
	INAddr Addr;
	INPort wPort;
	IFile* pIFile = NULL;
    IFileMgr *pIFileMgr = NULL;
    FileInfo pInfo = {0};

    OEM_SetBAM_ADSAccount();
    
	result = ISHELL_CreateInstance(AEE_GetShell(),AEECLSID_NET, (void **)&pINetMgr);
	if ( result != SUCCESS || pINetMgr == NULL)
	{
		goto Exit;
	}

	result = INETMGR_SelectNetwork(pINetMgr, 0); 
	MSG_FATAL("INETMGR_SelectNetwork Return Status is %d", result,0,0);
			
	pISocket = INETMGR_OpenSocket(pINetMgr, AEE_SOCK_STREAM);
	if (pISocket == NULL)
	{
		result = INETMGR_GetLastError(pINetMgr);
		MSG_FATAL("MRS: Open socket error %x", result,0,0);
		goto Exit;
	}
  
	pSocketInfoTag.pISocket = pISocket;
	pSocketInfoTag.wPort = HTONS((INPort)80);
    
    MEMCPY(pSocketInfoTag.sendNumber, sendNumber, WSTRLEN(sendNumber)*2);
    DBGPRINTF("sendNumber1=%s, sendNumber2=%s", sendNumber, pSocketInfoTag.sendNumber);
	if (!INET_ATON("10.0.0.200",&Addr))
	{
		MSG_FATAL("INET_ATON failed!", 0,0,0);
	}	

	pSocketInfoTag.status = SOCKET_STATE_CONNECTING;
    pSocketInfoTag.SendFinsh = FALSE;
    pSocketInfoTag.SocketReadStatus = 0;
	ISOCKET_Connect(pISocket, Addr, pSocketInfoTag.wPort, ConnectError, (void*)&pSocketInfoTag);
Exit:

	IBASE_RELEASE_IPTR(pINetMgr);
	MSG_FATAL("MRS: Open socket result %x", result,0,0);
	
	return SUCCESS;
}

//获取发送彩信后返回的状态
uint8 MMS_GetSocketReadStatus()
{
    MSG_FATAL("pSocketInfoTag.SocketReadStatus=%d", pSocketInfoTag.SocketReadStatus,0,0);
    return pSocketInfoTag.SocketReadStatus;
}

boolean MMS_GetSocketSendIsFinsh()
{
    MSG_FATAL("pSocketInfoTag.SendFinsh=%d", pSocketInfoTag.SendFinsh,0,0);
    return pSocketInfoTag.SendFinsh;
}
#endif