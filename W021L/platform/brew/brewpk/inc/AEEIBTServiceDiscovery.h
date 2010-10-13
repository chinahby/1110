#ifndef AEEIBTSERVICEDISCOVERY_H
#define AEEIBTSERVICEDISCOVERY_H
/*=============================================================================
FILE:         AEEIBTServiceDiscovery.h

SERVICES:     IBTServiceDiscovery Interfaces

DESCRIPTION:  IBTServiceDiscovery is the interface to the Service Discovery 
              module of the underlying Bluetooth driver.  This interface 
              provides methods for device discovery, service discovery, and
              service registration.

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEEBTSDDef.h"
#include "AEEISignal.h"

/*=============================================================================
 Interface ID
=============================================================================*/
#define AEEIID_IBTServiceDiscovery    	0x010570e0

/*=============================================================================
 BTSD Event types
=============================================================================*/
#define AEEBTSD_EV_NONE                       0
#define AEEBTSD_EV_DEV_SEARCH_STARTED         1
#define AEEBTSD_EV_DEV_SEARCH_FAILED          2
#define AEEBTSD_EV_DEV_SEARCH_RESPONSE        3
#define AEEBTSD_EV_DEV_SEARCH_COMPLETE        4
#define AEEBTSD_EV_SVC_SEARCH_STARTED         5
#define AEEBTSD_EV_SVC_SEARCH_FAILED          6
#define AEEBTSD_EV_SVC_SEARCH_COMPLETE        7
#define AEEBTSD_EV_NAME_REQ_STARTED           8
#define AEEBTSD_EV_NAME_REQ_FAILED            9
#define AEEBTSD_EV_NAME_REQ_COMPLETE         10
#define AEEBTSD_EV_SVC_REG_COMPLETE          11
#define AEEBTSD_EV_SVC_REG_FAILED            12
#define AEEBTSD_EV_SVC_DEREG_COMPLETE        13
#define AEEBTSD_EV_SVC_DEREG_FAILED          14
#define AEEBTSD_EV_DISC_MODE_SET_SUCCESS     15
#define AEEBTSD_EV_DISC_MODE_SET_FAILED      16

typedef uint32 BTSDEventType;

/*=============================================================================
 Interface Definition
=============================================================================*/
#define INHERIT_IBTServiceDiscovery(iname)   \
   INHERIT_IQI(iname);                       \
   int (*OnEventNotify)          (iname*                 po,               \
                                  ISignal*               piSignal);        \
   int (*GetEvent)               (iname*                 po,               \
                                  BTSDEventType*         pdEvent,          \
                                  BTResult*              pdResult);        \
   /* Device Search */                                                     \
   int (*StartDeviceSearch)      (iname*                 po,               \
                                  BTClassOfService       CoS2Srch4,        \
                                  int                    nMaxDevices,      \
                                  int                    nDuration);       \
   int (*StopDeviceSearch)       (iname*                 po);              \
   int (*GetNextResponse)        (iname*                 po,               \
                                  BTDeviceInfo*          pDevInfo);        \
   int (*StartNameRequest)       (iname*                 po,               \
                                  const BDAddress*       pBDAddr);         \
   int (*StopNameRequest)        (iname*                 po,               \
                                  const BDAddress*       pBDAddr);         \
   int (*GetName)                (iname*                 po,               \
                                  BTDeviceInfo*          pDevInfo);        \
   /* Service Registration */                                              \
   int (*RegisterService)        (iname*                 po,               \
                                  const char*            pszSvcRecXML,     \
                                  boolean                bConnectable);    \
   int (*GetMyRecordHandles)     (iname*                 po,               \
                                  uint32*                puHandle,         \
                                  int                    nNumHndls,        \
                                  int*                   pnValidHndls);    \
   int (*DeregisterService)      (iname*                 po,               \
                                  uint32                 uHandle);         \
   int (*GetLocalDBSummary)      (iname*                 po,               \
                                  BTServiceRecordSummary* pDBInfo,         \
                                  int                    nNumRecs,         \
                                  int*                   pnNumRemRecs);    \
   int (*ReadRecord)             (iname*                 po,               \
                                  uint32                 uHandle,          \
                                  char*                  pszSvcRecXML,     \
                                  int                    nLen,             \
                                  int*                   pnRequiredLen);   \
   int (*WriteRecord)            (iname*                 po,               \
                                  uint32                 uHandle,          \
                                  const char*            pszSvcRecXML);    \
   int (*GetDiscoverableMode)    (iname*                 po,               \
                                  BTSvcDiscoverableMode* pdMode);          \
   int (*SetDiscoverableMode)    (iname*                 po,               \
                                  BTSvcDiscoverableMode  dMode);           \
   /* Service Search APIs */                                               \
   int (*StartServiceSearch)     (iname*                 po,               \
                                  const BDAddress*       pBDAddr,          \
                                  const BTSvcSearchPattern*  pWhat2Search4);   \
   int (*StopServiceSearch)      (iname*                 po);              \
   int (*GetNumFoundRecs)        (iname*                 po,               \
                                  int*                   pnNumRecs);       \
   int (*GetSearchResult)        (iname*                 po,               \
                                  char*                  pszResultXML,     \
                                  int                    nSize,            \
                                  int*                   pnRequiredSize)


AEEINTERFACE_DEFINE(IBTServiceDiscovery);


static __inline uint32 IBTServiceDiscovery_AddRef (IBTServiceDiscovery* p)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->AddRef((p));
}

static __inline uint32 IBTServiceDiscovery_Release (IBTServiceDiscovery* p)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->Release((p));
}

static __inline int IBTServiceDiscovery_QueryInterface (IBTServiceDiscovery* p,
                                                        AEEIID iid,
                                                        void **ppo)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->QueryInterface((p),(iid),(ppo));
}

static __inline int IBTServiceDiscovery_OnEventNotify (IBTServiceDiscovery* p, 
                                                       ISignal* piSignal)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->OnEventNotify((p),(piSignal));
}

static __inline int IBTServiceDiscovery_GetEvent (IBTServiceDiscovery* p, 
                                                  BTSDEventType* pdEvent,
                                                  BTResult* pdR)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->GetEvent((p),(pdEvent),(pdR));
}

static __inline int IBTServiceDiscovery_StartDeviceSearch (
                       IBTServiceDiscovery* p,
                       BTClassOfService CoS,
                       int cnt,
                       int d)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->StartDeviceSearch(
                                                  (p),(CoS),(cnt),(d));
}

static __inline int IBTServiceDiscovery_StopDeviceSearch (IBTServiceDiscovery* p)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->StopDeviceSearch((p));
}

static __inline int IBTServiceDiscovery_GetNextResponse (IBTServiceDiscovery* p, 
                                                         BTDeviceInfo* pDevInfo)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->GetNextResponse((p),(pDevInfo));
}

static __inline int IBTServiceDiscovery_StartNameRequest (IBTServiceDiscovery* p,
                                                          const BDAddress* pA)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->StartNameRequest((p),(pA));
}

static __inline int IBTServiceDiscovery_StopNameRequest (IBTServiceDiscovery* p,
                                                         const BDAddress* pA)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->StopNameRequest((p),(pA));
}

static __inline int IBTServiceDiscovery_GetName (IBTServiceDiscovery* p,
                                                 BTDeviceInfo* pDevInfo)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->GetName((p),(pDevInfo));
}

static __inline int IBTServiceDiscovery_RegisterService (IBTServiceDiscovery* p, 
                                                         const char* psz,
                                                         boolean bC)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->RegisterService((p),(psz),(bC));
}

static __inline int IBTServiceDiscovery_GetMyRecordHandles (IBTServiceDiscovery* p,
                                                            uint32* puH,
                                                            int nH, int* pnH)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->GetMyRecordHandles((p),(puH),
                                                                   (nH),(pnH));
}

static __inline int IBTServiceDiscovery_DeregisterService (
                       IBTServiceDiscovery* p,
                       uint32 uH)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->DeregisterService((p),(uH));
}

static __inline int IBTServiceDiscovery_GetLocalDBSummary (
                       IBTServiceDiscovery* p, 
                       BTServiceRecordSummary* pRS,
                       int nR, int* pnR)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->GetLocalDBSummary((p),(pRS),
                                                                  (nR),(pnR));
}

static __inline int IBTServiceDiscovery_ReadRecord (IBTServiceDiscovery* p, 
                                                    uint32 uH,
                                                    char* psz,
                                                    int nL,
                                                    int* pnL)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->ReadRecord((p),(uH),(psz),
                                                           (nL),(pnL));
}

static __inline int IBTServiceDiscovery_WriteRecord (IBTServiceDiscovery* p, 
                                                     uint32 uH,
                                                     const char* psz)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->WriteRecord((p),(uH),(psz));
}

static __inline int IBTServiceDiscovery_GetDiscoverableMode (
                       IBTServiceDiscovery* p, 
                       BTSvcDiscoverableMode* pM)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->GetDiscoverableMode((p),(pM));
}

static __inline int IBTServiceDiscovery_SetDiscoverableMode (
                       IBTServiceDiscovery* p, 
                       BTSvcDiscoverableMode m)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->SetDiscoverableMode((p),(m));
}

static __inline int IBTServiceDiscovery_StartServiceSearch (
                       IBTServiceDiscovery* p,
                       const BDAddress* pAddr,
                       const BTSvcSearchPattern* pCriteria)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->StartServiceSearch((p),(pAddr),
                                                                   (pCriteria));
}

static __inline int IBTServiceDiscovery_StopServiceSearch (IBTServiceDiscovery* p)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->StopServiceSearch((p));
}

static __inline int IBTServiceDiscovery_GetNumFoundRecs (IBTServiceDiscovery* p, 
                                                         int* pN)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->GetNumFoundRecs((p),(pN));
}

static __inline int IBTServiceDiscovery_GetSearchResult (IBTServiceDiscovery* p, 
                                                         char* psz, int nSize, 
                                                         int* pN)
{
   return AEEGETPVTBL((p),IBTServiceDiscovery)->GetSearchResult((p),(psz),
                                                                (nSize),(pN));
}

/*=============================================================================
DATA STRUCTURE DOCUMENTATION
===============================================================================
BTSDEventType

Description:
   Possible events that can occur on SD object, retrieved via 
   IBTServiceDiscovery_GetEvent().
   
   #define AEEBTSD_EV_NONE                       0
   #define AEEBTSD_EV_DEV_SEARCH_STARTED         1
   #define AEEBTSD_EV_DEV_SEARCH_FAILED          2
   #define AEEBTSD_EV_DEV_SEARCH_RESPONSE        3
   #define AEEBTSD_EV_DEV_SEARCH_COMPLETE        4
   #define AEEBTSD_EV_SVC_SEARCH_STARTED         5
   #define AEEBTSD_EV_SVC_SEARCH_FAILED          6
   #define AEEBTSD_EV_SVC_SEARCH_COMPLETE        7
   #define AEEBTSD_EV_NAME_REQ_STARTED           8
   #define AEEBTSD_EV_NAME_REQ_FAILED            9
   #define AEEBTSD_EV_NAME_REQ_COMPLETE         10
   #define AEEBTSD_EV_SVC_REG_COMPLETE          11
   #define AEEBTSD_EV_SVC_REG_FAILED            12
   #define AEEBTSD_EV_DISC_MODE_SET_SUCCES      13
   #define AEEBTSD_EV_DISC_MODE_SET_FAILED      14
   
=============================================================================*/

/*=============================================================================
INTERFACE DOCUMENTATION
===============================================================================
IBTServiceDiscovery Interface

Description:
   This interface provides methods to control the Service Discovery module
   of the underlying Bluetooth driver.

===H2>
   Usage example:
===/H2>

   ===== To create an IBTServiceDiscovery object, see instruction in 
         class header file


   ===== To register signal
   // first create the signals; for example, use ISignalCBFactory_CreateSignal()
   // then register the signals:
   IBTServiceDiscovery_OnEventNotify (pMe->piBTSD, pMe->piSDEventSignal);


   ===== To search for up to 5 OBEX devices (OPP servers, ...) 
   ===== in the vicinity within 10 seconds
   pMe->pFoundDev = MALLOC (5*sizeof(BTDeviceInfo));
   IBTServiceDiscovery_StartDeviceSearch (pMe->piBTSD,
                                          AEEBT_COS_OBJECT_TRANSFER, 
                                          5,    // 5 responses max
                                          10);  // ten seconds
   // The search would generate DEV_SEARCH_RESPONSE event if any OBEX devices 
   // are found.  When 5 different OBEX devices are found, or 10 seconds later, 
   // whichever comes first, DEV_SEARCH_COMPLETE event would get generated


   ===== To stop the on-going device search
   IBTServiceDiscovery_StopDeviceSearch (pMe->piBTSD);
   // DEV_SEARCH_COMPLETE would get generated


   ===== To search remote device's service DB for all records having
   ===== UUID value of 0x0003 (RFCOMM protocol UUID) and extract only
   ===== server channel numbers
   BTSvcSearchPattern pattern;
   pattern.nNumUUID16s = 1;
   pattern.UUID16[0] = AEEBT_PROT_RFCOMM;
   pattern.nNumUUID128s = 0;
   pattern.nNumAttributes = 1;
   pattern.attribValue[0] = AEEBTSD_ATTRIBUTE_ID_PROTOCOL_DESCRIPTOR_LIST;
   IBTServiceDiscovery_StartServiceSearch (pMe->piBTSD, &remoteAddr, &pattern);
   // SVC_SEARCH_COMPLETE would get generated


   ===== To stop the on-going service search
   IBTServiceDiscovery_StopServiceSearch (pMe->piBTSD);
   // SVC_SEARCH_COMPLETE would get generated


   ===== To retrieve and handle events when Event signal is triggered:
   while (IBTServiceDiscovery_GetEvent (pMe->piBTSD, &dEvent,
                                        &dResult) != AEE_ENOMORE)
   {
      switch (dEvent)
      {
         case AEEBTSD_EV_DEV_SEARCH_RESPONSE:
            while (IBTServiceDiscovery_GetNextResponse (
                      pMe->piBTSD, pMe->pFoundDev[ i ]) != AEE_ENOMORE)
            {
               // if the search is over; it's possible now to request the
               // device name if necessary
               if (pMe->pFoundDev[ i ]->szName[0] == 0) // name not available
               {
                  IBTServiceDiscovery_StartNameRequest (
                     pMe->piBTSD, pMe->pFoundDev[ i ]->bdAddr);
                  // when remote device sends its name, NAME_REQ_DONE event gets 
                  // generated; if remote device isn't connectable, this request
                  // would not complete quickly; user may wish to cancel 
                  // this request:
                  IBTServiceDiscovery_StopNameRequest (
                     pMe->piBTSD, pMe->pFoundDev[ i ]->bdAddr);
               }
            }
            break;
         case AEEBTSD_EV_NAME_REQ_COMPLETE:
            IBTServiceDiscovery_GetName (pMe->piBTSD, pMe->pFoundDev[ i ]);
            break;
         case AEEBTSD_EV_SVC_SEARCH_COMPLETE:
            IBTServiceDiscovery_GetNumFoundRecs (pMe->piBTSD, &numFoundRecs);
            if (numFoundRecs > 0)
            {
               int nSize = 0;
               char* pszRecStr = NULL;
               IBTServiceDiscovery_GetSearchResult (pMe->piBTSD, NULL, 0, 
                                                    &nSize);
               pszRecStr = MALLOC (nSize);
               IBTServiceDiscovery_GetSearchResult (
                  pMe->piBTSD, pszRecStr, nSize, &nSize);
               // extract desired info from pszRecStr
            }
            break;
         case AEEBTSD_EV_SVC_REG_COMPLETE:
            // assuming MyRecHandles is large enough
            IBTServiceDiscovery_GetMyRecordHandles (pMe->piBTSD, 
                                                    NULL, 0, &nNumHandles);
            IBTServiceDiscovery_GetMyRecordHandles (pMe->piBTSD, 
                                                    &MyRecHandles,
                                                    nNumHandles,
                                                    &nNumHandles);
            break;
         // handle other events
      }
   }
   ISignalCtl_Enable (pMe->piSDEventSignalCtl); // re-enable signal


   ===== To register your proprietary OPP service which is 
   ===== implemented over SPP, first enable SPP server using
   ===== IBTSPPSRV_EnableService() and obtain the server channel number,
   ===== then build your service record in XLM
   uint32 uOPPRecHandle;
   char svcRecStr[] = 
      "<srec><attr id=\"0x0001\"><list>"           // Service Class ID list
      "<uuid16>0x1105</uuid16>"                    // OPP
      "<uuid16>0x1101</uuid16></list></attr>"      // SPP
      "<attr id=\"0x0004\"><list>"                 // Protocol Descriptor list
      "<list><uuid16>0x0100</uuid16></list>"       // L2CAP
      "<list><uuid16>0x0003</uuid16>"              // RFCOMM
      "<uint8>5</uint8></list></list></attr>"      // Server Channel Number
      "<attr id=\"0x0100\"><string>John's OPP Server</string></attr>" // svc name
      "<attr id=\"0x0009\"><list>"                 // Profile Descriptor list
      "<list><uuid16>0x1105</uuid16>"              // OPP profile
      "<uin16>0x0100</uint16></list></list><attr>  // profile version
      "</srec>";
   IBTServiceDiscovery_RegisterService (pMe->piBTSD, svcRecStr, TRUE);
   // see handling of SVC_REG_COMPLETE event above


   ===== To add the Supported Formats list to the OPP service record
   char str[] = 
      "<attr id=\"0x0303\"><list>"                 // OPP Supported Formats list
      "<uint8>0x01</uint8>"                        // VCard 2.1
      "<uint8>0x04</uint8></list></attr>";         // ICal 2.0
   IBTServiceDiscovery_WriteRecord (pMe->piBTSD, uOPPRecHandle, &str);


   ===== To change the service name attribute of the OPP service record
   char str[] = 
      "<attr id=\"0x0100\"><string>Smith's OPP Server</string></attr>";
   IBTServiceDiscovery_WriteRecord (pMe->piBTSD, uOPPRecHandle, &str);


   ===== To deregister the service
   IBTServiceDiscovery_DeregisterService (pMe->piBTSD, uOPPRecHandle);


   ===== To find out what services are provided by the local device
   int nRecCount = 0;
   BTServiceRecordSummary* pSvcRecSummary = NULL;
   IBTServiceDiscovery_GetLocalDBSummary (pMe->piBTSD, NULL, 0, &nRecCount);
   pSvcRecSummary = MALLOC (nRecCount * sizeof(BTServiceRecordSummary));
   IBTServiceDiscovery_GetLocalDBSummary (pMe->piBTSD, pSvcRecSummary,
                                          nRecCount, &nRecCount);


   ===== To find FTP service record(s) in local database
   int i = nRecCount;
   while (i-- > 0)
   {
      if (pSvcRecSummary[i]->profileUUID == AEEBT_SCU_OBEX_FILE_TRANSFER)
      {
         char* recStr = NULL;
         IBTServiceDiscovery_ReadRecord (pMe->piBTSD, uOPPRecHandle,
                                         NULL, 0, &nSize);
         recStr = MALLOC (nSize);
         IBTServiceDiscovery_ReadRecord (pMe->piBTSD, uOPPRecHandle,
                                         recStr, nSize, &nSize);
         // app must parse recStr to get the attributes

         FREE (recStr);
      }
   }


===============================================================================
IBTServiceDiscovery_AddRef

Description:
   Inherited from IQI_AddRef().

See Also:
   IBTServiceDiscovery_Release()
   
===============================================================================
IBTServiceDiscovery_Release

Description:
   Inherited from IQI_Release().

See Also:
   IBTServiceDiscovery_AddRef()
   
===============================================================================
IBTServiceDiscovery_QueryInterface

Description:
   Inherited from IQI_QueryInterface().

===============================================================================
IBTServiceDiscovery_OnEventNotify()
   
Description:
   This method registers a signal to be triggered when there is an event
   that occurred on the SD object.  Application should use 
   IBTServiceDiscovery_GetEvent() to retrieve the event information.
   
Prototype:
   int IBTServiceDiscovery_OnEventNotify (IBTServiceDiscovery*  po, 
                                          ISignal*              piSignal);

Parameters:
   po [i]               : the object
   piSignal [i]         : signal to be triggered when an event occurs

Return values:
   AEE_SUCCESS          : signal has been scheduled
   AEE_EBADPARM         : input pointers are invalid

See Also:
   IBTServiceDiscovery_GetEvent()
   
===============================================================================
IBTServiceDiscovery_GetEvent()

Description:
   This method retrieves SD events.  When the signal registered via 
   IBTServiceDiscovery_OnEventNotify() is triggered, his API should be 
   called repeatedly until AEE_ENOMORE is returned.

Prototype:
   int IBTServiceDiscovery_GetEvent (IBTServiceDiscovery*  po, 
                                     BTSDEventType*        pdEvent,
                                     BTResult*             pdResult);

Parameters:
   po [i]               : the object
   pdEvent [o]          : destination for the event code
   pdResult [o]         : destination for failure result

Return values:
   AEE_SUCCESS          : event info successfully retrieved
   AEE_EBADPARM         : input pointers are invalid
   AEE_ENOMORE          : no more events left

See Also:
   IBTServiceDiscovery_OnEventNotify()
   
===============================================================================
IBTServiceDiscovery_StartDeviceSearch

Description:
   Initiates a device search session.  If successful, DEV_SEARCH_RESPONSE event
   will be generated for each response received from remote devices, and
   DEV_SEARCH_COMPLETE event will be generated when nMaxDevices have responded
   to the search, or nDuration seconds have passed.  Application should call
   IBTServiceDiscovery_GetNextResponse() to retrieve the responses, one at a 
   time.  Application may wish to end the search before it completes on its
   own by calling IBTServiceDiscovery_StopDeviceSearch().

Prototype:
   int IBTServiceDiscovery_StartDeviceSearch (IBTServiceDiscovery*  po,
                                              BTClassOfService CoS2Search4,
                                              int nMaxDevices,
                                              int nDuration);

Parameters:
   po [i]               : the object
   CoS2Srch4 [i]        : class of device for which to search 
   nMaxDevices [i]      : max number of devices for which to search
   nDuration [i]        : search duration in seconds

Return Value:
   AEE_SUCCESS          : operation has been scheduled
   AEE_EBADPARM         : input parameters are invalid
   AEE_EALREADY         : the previous DeviceSearch operation hasn't completed
   AEE_EITEMBUSY        : another operation already in progress

See Also:
   IBTServiceDiscovery_StopDeviceSearch
   IBTServiceDiscovery_GetNextResponse

===============================================================================
IBTServiceDiscovery_StopDeviceSearch

Description:
   Stops an on-going device search.  If successful, DEV_SEARCH_COMPLETE event
   will be generated.  A call to IBTServiceDiscovery_GetNextResponse() may still
   be successful if any responses were received before the search ended.

Prototype:
   int IBTServiceDiscovery_StopDeviceSearch (IBTServiceDiscovery* po);

Parameters:
   po [i]               : the object

Return Value:
   AEE_SUCCESS          : operation has been scheduled
   AEE_EBADSTATE        : device search is not in progress
   AEE_EALREADY         : the previous StopDevSearch operation hasn't completed

See Also:
   IBTServiceDiscovery_StartDeviceSearch
   IBTServiceDiscovery_GetNextResponse

===============================================================================
IBTServiceDiscovery_GetNextResponse

Description:
   Retrieves the next device response.  Application should call this API
   until it returns AEE_ENOMORE.

Prototype:
   int IBTServiceDiscovery_GetNextResponse (IBTServiceDiscovery* po,
                                            BTDeviceInfo* pDevInfo);

Parameters:
   po [i]               : the object
   pDevInfo [o]         : destination for the next device response

Return Value:
   AEE_SUCCESS          : next response successfull retrieved
   AEE_EBADPARM         : input parameters are invalid
   AEE_ENOMORE          : no more responses

See Also:
   IBTServiceDiscovery_StartDeviceSearch
   IBTServiceDiscovery_StopDeviceSearch

===============================================================================
IBTServiceDiscovery_StartNameRequest

Description:
   Requests remote device for its name.  This causes a connection to be made
   to the given remote device, if a connection is not already present.

   If remote device is not currently connectable, the request may, in the worst
   case, take up to 30 seconds to complete in failure.  Application may wish
   to stop the request via IBTServiceDiscovery_StopNameRequest().

   When and if the remote device's name is received, NAME_REQ_COMPLETE event
   gets generated.  At this time, a call to IBTServiceDiscovery_GetName()
   should return the received name.

Prototype:
   int IBTServiceDiscovery_StartNameRequest (IBTServiceDiscovery* po,
                                             const BDAddress* pAddr);

Parameters:
   po [i]               : the object
   pAddr [i]            : address of remote device

Return Value:
   AEE_SUCCESS          : operation has been scheduled
   AEE_EBADPARM         : input parameters are invalid
   AEE_EITEMBUSY        : another operation already in progress

See Also:
   IBTServiceDiscovery_StopNameRequest
   IBTServiceDiscovery_GetName

===============================================================================
IBTServiceDiscovery_StopNameRequest

Description:
   Stop the on-going name request.  NAME_REQ_COMPLETE event will get generated.

Prototype:
   int IBTServiceDiscovery_StopNameRequest (IBTServiceDiscovery* po,
                                            const BDAddress* pAddr);

Parameters:
   po [i]               : the object
   pAddr [i]            : address of remote device 

Return Value:
   AEE_SUCCESS          : operation has been scheduled
   AEE_EBADPARM         : input parameters are invalid
   AEE_EBADSTATE        : name request is not in progress
   AEE_EALREADY         : the previous StopNameReq operation hasn't completed

See Also:
   IBTServiceDiscovery_StartNameRequest

===============================================================================
IBTServiceDiscovery_GetName

Description:
   Retrieve the remote device's name obtained via the most recent call to
   IBTServiceDiscovery_StartNameRequest().  This API should be called when
   application received NAME_REQ_COMPLETE.

Prototype:
   int IBTServiceDiscovery_GetName (IBTServiceDiscovery* po,
                                    BTDeviceInfo*        pDevInfo);


Parameters:
   po [i]               : the object
   pDevInfo->bdAddr [i] : address of remote device
   pDevInfo->szName [o] : destination for the name

Return Value:
   AEE_SUCCESS          : remote device's name successfully retrieved
   AEE_EBADPARM         : input parameters are invalid
   AEE_ENOSUCH          : remote device's name is not available

See Also:
   IBTServiceDiscovery_StartNameRequest

===============================================================================
IBTServiceDiscovery_RegisterService

Description:
   Adds an entry to the local Service Discovery database.  If successful,
   SERVICE_REGISTERED event will get generated and a new record handle added
   to this object's list of record handles.  Application can call 
   IBTServiceDiscovery_GetMyRecordHandles() to retrieve the handles of all 
   service records it has created.

   Applications must have AEEPRIVID_Service_Registration in order to use this
   API.

Prototype:
   int IBTServiceDiscovery_RegisterService (IBTServiceDiscovery* po,
                                            const char* pszSvcRecXML,
                                            boolean bConnectable);

Parameters:
   po [i]               : the object
   pszSvcRecXML [i]     : NULL-terminated XML string representing the 
                            service record
   bConnectable [i]     : TRUE if local device should be connectable

Return Value:
   AEE_SUCCESS          : operation has been scheduled
   AEE_EBADPARM         : input parameters are invalid
   AEE_EPRIVLEVEL       : insufficient privilege to perform operation
   AEE_EALREADY         : the previous RegService operation hasn't completed

See Also:
   IBTServiceDiscovery_DeregisterService
   IBTServiceDiscovery_GetMyRecordHandles

===============================================================================
IBTServiceDiscovery_GetMyRecordHandles

Description:
   Retrieves the handles of all service records created by this Service 
   Discovery object.


Prototype:
   int IBTServiceDiscovery_GetMyRecordHandles (
          IBTServiceDiscovery* po,
          uint32*              puHandles,
          int                  nNumHndls,
          int*                 pnNumValidHndls);

Parameters:
   po [i]               : the object
   puHandles [o]        : destination for the list of record handles
   nNumHndls [i]        : number of handles puHandles can hold
   pnNumValidHndls [o]  : number of valid handles 

Return Value:
   AEE_SUCCESS          : record handles retrieved
   AEE_EBADPARM         : input parameters are invalid
   AEE_EBUFFERTOOSMALL  : only the first nNumHndls valid handles copied and
                             number of valid handles returned in pnNumValidHndls

See Also:
   IBTServiceDiscovery_RegisterService

===============================================================================
IBTServiceDiscovery_DeregisterService

Description:
   Removes from the local Service Discovery database the entry represented
   by the given service record object.  If successful, SERVICE_DEREGISTERED 
   event will get generated and local Bluetooth radio may become unconnectable
   if this was the last entry in the database.

   If application does not explicitly deregister before exiting, service
   deregistration will be done when this Service Discovery object is freed.

   Applications must have AEEPRIVID_Service_Registration in order to use this
   API.

Prototype:
   int IBTServiceDiscovery_DeregisterService (IBTServiceDiscovery* po,
                                              uint32               uHandle);

Parameters:
   po [i]               : the object
   uHandle [i]          : handle of the service record to be removed

Return Value:
   AEE_SUCCESS          : operation has been scheduled
   AEE_EBADPARM         : input parameters are invalid
   AEE_EPRIVLEVEL       : insufficient privilege to perform operation
   AEE_EALREADY         : the previous DeregService operation hasn't completed

See Also:
   IBTServiceDiscovery_RegisterService

===============================================================================
IBTServiceDiscovery_GetLocalDBSummary

Description:
   Retrieves the summary information of the local Service Disovery DB.

Prototype:
   int IBTServiceDiscovery_GetLocalDBSummary (
          IBTServiceDiscovery*     po,
          BTServiceRecordSummary*  pSvcRecSummary,
          int                      nNumRecs,
          int*                     pnRemRecs);

Parameters:
   po [i]               : the object
   pSvcRecSummary [o]   : destination for the service record summary info
   nNumRecs [i]         : number of record buffers pDBInfo points to
   pnRemRecs [o]        : destination for number of records yet to be read

Return Value:
   AEE_SUCCESS          : DB summary successfull retrieved
   AEE_EBADPARM         : input parameters are invalid
   AEE_EBUFFERTOOSMALL  : destination buffer is too small for all data;
                            summary for first nNumRecs copied and
                            number of remaining recs gets returned via pnRemRecs

===============================================================================
IBTServiceDiscovery_ReadRecord

Description:
   Retrieves the service record in XML representation.

Prototype:
   int IBTServiceDiscovery_ReadRecord (IBTServiceDiscovery* po,
                                       uint32               uHandle,
                                       char*                pszRecXML,
                                       int                  nSize,
                                       int*                 pnRequiredSize);

Parameters:
   po [i]               : the object
   uHandle [i]          : record handle of the service record of interest
   pszRecXML [o]        : destination for NULL-terminated XML 
                            representation of record
   nSize [i]            : size of buffer pointed to by pszXML
   pnRequiredSize [o]   : destination for required size

Return Value:
   AEE_SUCCESS          : service record read
   AEE_EBADPARM         : input parameters are invalid
   AEE_ENOSUCH          : no service record of given handle
   AEE_EBUFFERTOOSMALL  : destination buffer is too small for all data;
                          no partial string returned

===============================================================================
IBTServiceDiscovery_WriteRecord

Description:
   Updates service record with data from given XML string.  String may contain
   the entire record, or just an attribute.

   Applications must have AEEPRIVID_Service_Registration in order to use this
   API.

Prototype:
   int IBTServiceDiscovery_WriteRecord (IBTServiceDiscovery* po,
                                        uint32               uHandle,
                                        const char*          pszXML);

Parameters:
   po [i]               : the object
   uHandle [i]          : record holder of the service record of interest
   pszXML [i]           : string representing service data to save in DB

Return Value:
   AEE_SUCCESS          : service record read
   AEE_EBADPARM         : input parameters are invalid
   AEE_ENOSUCH          : no service record of given handle
   AEE_ENOTOWNER        : not owner of this record

===============================================================================
IBTServiceDiscovery_GetDiscoverableMode

Description:
   Retrieves the current discoverable mode of the system.

Prototype:
   int IBTServiceDiscovery_GetDiscoverableMode (IBTServiceDiscovery* po,
                                                BTSvcDiscoverableMode* pdMode);

Parameters:
   po [i]               : the object
   pdMode [o]           : destination for the current discoverable mode

Return Value:
   AEE_SUCCESS          : max number of records retrieved
   AEE_EBADPARM         : input parameters are invalid

===============================================================================
IBTServiceDiscovery_SetDiscoverableMode

Description:
   Sets the service discoverable mode.  The result of this operation can be
   determined when either DISC_MODE_SET_SUCCESS or DISC_MODE_SET_FAILED
   event is received.

   When multiple applications call this API, the side effect would be
   implementation dependent.

Prototype:
   int IBTServiceDiscovery_SetDiscoverableMode (IBTServiceDiscovery*  po,
                                                BTSvcDiscoverableMode dMode);

Parameters:
   po [i]               : the object
   dMode [i]            : the discoverable mode to set

Return Value:
   AEE_SUCCESS          : operation has been scheduled
   AEE_EBADPARM         : input parameters are invalid

===============================================================================
IBTServiceDiscovery_StartServiceSearch

Description:
   Requests remote device for data in its Service Discovery DB.  The data of
   interest is conveyed in the given search pattern.

   If remote device is not currently connectable, the request may take a long
   time to complete.  Application may wish to stop the request via 
   IBTServiceDiscovery_StopServiceSearch().

   When and if remote device's service data is received, SVC_SEARCH_COMPLETE 
   event gets generated.  At this time, application should call
   IBTServiceDiscovery_GetNumFoundRecs() to find out how many records matching
   the search pattern have ben found.  IBTServiceDiscovery_GetSearchResult()
   should then be called to retrieve the string representing the search result.

   If the search can not get started, SVC_SEARCH_FAILED would get generated.  
   In this case SVC_SEARCH_COMPLETE should not be expected.

Prototype:
   int IBTServiceDiscovery_StartServiceSearch (
          IBTServiceDiscovery* po,
          const BDAddress* pAddr,
          const BTSvcSearchPattern* pPattern);

Parameters:
   po [i]               : the object
   pAddr [i]            : address of remote device
   pPattern [i]         : what to search for

Return Value:
   AEE_SUCCESS          : operation has been scheduled
   AEE_EBADPARM         : input parameters are invalid
   AEE_EITEMBUSY        : another operation already in progress
   AEE_EALREADY         : the previous StartSvcSrch operation hasn't completed

See Also:
   IBTServiceDiscovery_StopServiceSearch
   IBTServiceDiscovery_GetNumFoundRecs
   IBTServiceDiscovery_GetSearchResult

===============================================================================
IBTServiceDiscovery_StopServiceSearch

Description:
   Stop the on-going service search request.  SVC_SEARCH_COMPLETE event will 
   get generated.

Prototype:
   int IBTServiceDiscovery_StopServiceSearch (IBTServiceDiscovery* po);

Parameters:
   po [i]               : the object

Return Value:
   AEE_SUCCESS          : operation has been scheduled
   AEE_EBADSTATE        : name request is not in progress
   AEE_EALREADY         : the previous StopSvcSrch operation hasn't completed

See Also:
   IBTServiceDiscovery_StartServiceSearch

===============================================================================
IBTServiceDiscovery_GetNumFoundRecs

Description:
   Retrieves the number of records matching service search pattern.  This API 
   should be called when application received SVC_SEARCH_COMPLETE.

Prototype:
   int IBTServiceDiscovery_GetNumFoundRecs (IBTServiceDiscovery* po,
                                            int*                 pnNumRecs);

Parameters:
   po [i]               : the object
   pnNumRecs [o]        : destination for the number of records found

Return Value:
   AEE_SUCCESS          : number of records successfully retrieved
   AEE_EBADPARM         : input parameters are invalid
   AEE_EBADSTATE        : service search never started

See Also:
   IBTServiceDiscovery_StartServiceSearch

===============================================================================
IBTServiceDiscovery_GetSearchResult

Description:
   Retrieve the result from the most recent call to 
   IBTServiceDiscovery_StartServiceSearch().  This API should be called when
   application received SVC_SEARCH_COMPLETE.

Prototype:
   int IBTServiceDiscovery_GetSearchResult (IBTServiceDiscovery* po,
                                            char*                pszResultXML,
                                            int                  nSize,
                                            int*                 pnRequiredSize);

Parameters:
   po [i]               : the object
   pszResultXML [o]     : destination for the search result 
   nSize [i]            : size of buffer pointed to by pszResultXML
   pnRequiredSize [o]   : destination for the required size 

Return Value:
   AEE_SUCCESS          : search result successfully retrieved
   AEE_EBADPARM         : input parameters are invalid
   AEE_EBUFFERTOOSMALL  : destination buffer is too small for all data;
                             partial result not returned
   AEE_EBADSTATE        : service search never started

See Also:
   IBTServiceDiscovery_StartServiceSearch

==========================================================================*/

#endif // AEEIBTSERVICEDISCOVERY_H

