#ifndef _AEE_BT_EXT_BIP_H_
#define _AEE_BT_EXT_BIP_H_

/*===========================================================================
FILE:      AEEBTExtBIP.h

SERVICES:  BlueTooth Basic Imaging Profile Interface

GENERAL DESCRIPTION: BREW interface to BT BIP

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTExtBIP

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEFile.h"

// BREW generated ID
#define AEECLSID_BLUETOOTH_BIP           0x010220ba

// event types returned via HandleEvent()
enum
{
  /* for servers */
  AEEBT_BIP_EVT_REG_DONE,       // registration process done
  AEEBT_BIP_EVT_DEREG_DONE,     // deregistration process done
  AEEBT_BIP_EVT_CONN_REQ,       // a client wants to connect

  AEEBT_BIP_EVT_GET_CAPABILITIES_REQ_INITIAL,
  AEEBT_BIP_EVT_GET_CAPABILITIES_REQ_CLEANUP,
  AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_REQ_INITIAL,
  AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_REQ_CLEANUP,
  AEEBT_BIP_EVT_GET_IMAGE_LIST_REQ_INITIAL,
  AEEBT_BIP_EVT_GET_IMAGE_LIST_REQ_CLEANUP,
  AEEBT_BIP_EVT_GET_IMAGE_REQ_INITIAL,
  AEEBT_BIP_EVT_GET_IMAGE_REQ_CLEANUP,
  AEEBT_BIP_EVT_GET_MONITORING_IMAGE_REQ_INITIAL,
  AEEBT_BIP_EVT_GET_MONITORING_IMAGE_REQ_CLEANUP,
  AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_REQ_INITIAL,
  AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_REQ_CLEANUP,
  AEEBT_BIP_EVT_GET_ATTACHMENT_REQ_INITIAL,
  AEEBT_BIP_EVT_GET_ATTACHMENT_REQ_CLEANUP,
  AEEBT_BIP_EVT_GET_THUMBNAIL_REQ_INITIAL,
  AEEBT_BIP_EVT_GET_THUMBNAIL_REQ_CLEANUP,

  AEEBT_BIP_EVT_PUT_IMAGE_REQ_INITIAL,
  AEEBT_BIP_EVT_PUT_IMAGE_REQ_CLEANUP,
  AEEBT_BIP_EVT_PUT_ATTACHMENT_REQ_INITIAL,
  AEEBT_BIP_EVT_PUT_ATTACHMENT_REQ_CLEANUP,
  AEEBT_BIP_EVT_PUT_THUMBNAIL_REQ_INITIAL,
  AEEBT_BIP_EVT_PUT_THUMBNAIL_REQ_CLEANUP,

  AEEBT_BIP_EVT_DELETE_IMAGE_REQ_INITIAL,
  AEEBT_BIP_EVT_DELETE_IMAGE_REQ_CLEANUP,
  AEEBT_BIP_EVT_REMOTE_DISPLAY_REQ_INITIAL,
  AEEBT_BIP_EVT_REMOTE_DISPLAY_REQ_CLEANUP,
  AEEBT_BIP_EVT_START_ARCHIVE_REQ_INITIAL,
  AEEBT_BIP_EVT_START_ARCHIVE_REQ_CLEANUP,

  /* for clients */
  AEEBT_BIP_EVT_OP_ABORTED,      // current operation aborted

  AEEBT_BIP_EVT_GET_CAPABILITIES_DONE,
  AEEBT_BIP_EVT_GET_IMAGE_LIST_DONE,
  AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_DONE,
  AEEBT_BIP_EVT_GET_IMAGE_DONE,
  AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_DONE,
  AEEBT_BIP_EVT_GET_ATTACHMENT_DONE,
  AEEBT_BIP_EVT_GET_THUMBNAIL_DONE,
  AEEBT_BIP_EVT_GET_MONITORING_IMAGE_DONE,
  AEEBT_BIP_EVT_GET_STATUS_DONE,

  AEEBT_BIP_EVT_PUT_IMAGE_DONE,       
  AEEBT_BIP_EVT_PUT_ATTACHMENT_DONE,  
  AEEBT_BIP_EVT_PUT_THUMBNAIL_DONE,  

  AEEBT_BIP_EVT_DELETE_IMAGE_DONE,
  AEEBT_BIP_EVT_REMOTE_DISPLAY_DONE,
  AEEBT_BIP_EVT_START_ARCHIVE_DONE,
  AEEBT_BIP_EVT_START_PRINT_DONE,

  /* common to servers and clients */
  AEEBT_BIP_EVT_CONNECTED, 
  AEEBT_BIP_EVT_CONN_FAILED, 
  AEEBT_BIP_EVT_DISCONNECTED,
  AEEBT_BIP_EVT_AUTH_REQ
};

// error codes for AEEBT_BIP_EVT_*
// DEPRECATED - DO NOT ADD ERROR CODES TO THIS ENUMERATION!
enum
{
  // DEPRECATED
  AEEBT_BIP_ERR_NONE          = AEEBT_ERR_NONE,            // no worry
  AEEBT_BIP_ERR_BAD_PARAM     = AEEBT_ERR_BAD_PARAM,       // for any function
  AEEBT_BIP_ERR_RESOURCE      = AEEBT_ERR_RESOURCE,        // lack of resource (memory, ...)
  AEEBT_BIP_ERR_DB_FULL       = AEEBT_ERR_SD_DB_FULL,      // SDDB is full
  AEEBT_BIP_ERR_REC_EXISTS    = AEEBT_ERR_SD_REC_EXISTS,   // server record already in SDDB
  AEEBT_BIP_ERR_REC_NOT_FOUND = AEEBT_ERR_SD_REC_NOT_FOUND,// server record not found in SDDB
  AEEBT_BIP_ERR_FAILED        = AEEBT_ERR_UNKNOWN          // general failure
  // DEPRECATED
};

// RemoteDisplay API display commands
enum
{
  AEEBT_BIP_REMOTE_DISPLAY_NEXT_IMAGE_CMD         = 0x01,
  AEEBT_BIP_REMOTE_DISPLAY_PREVIOUS_IMAGE_CMD, // = 0x02
  AEEBT_BIP_REMOTE_DISPLAY_SELECT_IMAGE_CMD,   // = 0x03
  AEEBT_BIP_REMOTE_DISPLAY_CURRENT_IMAGE_CMD   // = 0x04
};

typedef uint8  AEEBTBIPRDcmd ;

// Primary and secondary BIP server types
enum
{
  // Primary OBEX targets
  AEEBT_BIP_TARGET_IMAGE_PUSH,
  AEEBT_BIP_TARGET_IMAGE_PULL,
  AEEBT_BIP_TARGET_IMAGE_PRINTING,
  AEEBT_BIP_TARGET_AUTOMATIC_ARCHIVE,
  AEEBT_BIP_TARGET_REMOTE_CAMERA,
  AEEBT_BIP_TARGET_REMOTE_DISPLAY,

  // Secondary OBEX targets
  AEEBT_BIP_TARGET_ARCHIVED_OBJECTS_SERVER,
  AEEBT_BIP_TARGET_REFERENCED_OBJECTS_SERVER
};

typedef uint8  AEEBTBIPServerType;

// Imaging Service types
enum
{
  AEEBT_BIP_IMAGING_RESPONDER,
  AEEBT_BIP_ARCHIVED_OBJECTS,
  AEEBT_BIP_REFERENCED_OBJECTS
};

typedef uint8  AEEBTBIPServiceType;

// Imaging capabilities flags (based on Table 6-1 in BIP spec)
enum 
{
  AEEBT_BIP_CAPABILITIES_GENERIC_IMAGING     = 0x01,
  AEEBT_BIP_CAPABILTIIES_CAPTURING           = 0x02,
  AEEBT_BIP_CAPABILITIES_PRINTING            = 0x04,
  AEEBT_BIP_CAPABILITIES_DISPLAYING          = 0x08
};

// Imaging features flags (based on Table 6-1 in BIP spec)
enum
{
  AEEBT_BIP_FEATURES_IMAGE_PUSH              = 0x0001,
  AEEBT_BIP_FEATURES_IMAGE_PUSH_STORE        = 0x0002,
  AEEBT_BIP_FEATURES_IMAGE_PUSH_PRINT        = 0x0004,
  AEEBT_BIP_FEATURES_IMAGE_PUSH_DISPLAY      = 0x0008,
  AEEBT_BIP_FEATURES_IMAGE_PULL              = 0x0010,
  AEEBT_BIP_FEATURES_ADVANCED_IMAGE_PRINTING = 0x0020,
  AEEBT_BIP_FEATURES_AUTOMATIC_ARCHIVE       = 0x0040,
  AEEBT_BIP_FEATURES_REMOTE_CAMERA           = 0x0080,
  AEEBT_BIP_FEATURES_REMOTE_DISPLAY          = 0x0100
};

// Imaging functions flags (based on Table 6-1,6-2,6-3 in BIP spec)
enum
{
  AEEBT_BIP_GET_CAPABILITIES                 = 0x00000001,
  AEEBT_BIP_PUT_IMAGE                        = 0x00000002,
  AEEBT_BIP_PUT_LINKED_ATTACHMENT            = 0x00000004,
  AEEBT_BIP_PUT_LINKED_THUMBNAIL             = 0x00000008,
  AEEBT_BIP_REMOTE_DISPLAY                   = 0x00000010,
  AEEBT_BIP_GET_IMAGE_LIST                   = 0x00000020,
  AEEBT_BIP_GET_IMAGE_PROPERTIES             = 0x00000040,
  AEEBT_BIP_GET_IMAGE                        = 0x00000080,
  AEEBT_BIP_GET_LINKED_THUMBNAIL             = 0x00000100,
  AEEBT_BIP_GET_LINKED_ATTACHMENT            = 0x00000200,
  AEEBT_BIP_DELETE_IMAGE                     = 0x00000400,
  AEEBT_BIP_START_PRINT                      = 0x00000800,
  AEEBT_BIP_GET_PARTIAL_IMAGE                = 0x00001000,
  AEEBT_BIP_START_ARCHIVE                    = 0x00002000,
  AEEBT_BIP_GET_MONITORING_IMAGE             = 0x00004000,
  AEEBT_BIP_GET_STATUS                       = 0x00008000
};

typedef uint8  AEEBTBIPsupCapabilities;
typedef uint16 AEEBTBIPsupFeatures;
typedef uint32 AEEBTBIPsupFunctions;

typedef struct _IBTExtBIP IBTExtBIP;

AEEINTERFACE(IBTExtBIP)
{
  INHERIT_IQueryInterface(IBTExtBIP);

  /* for server */
  int (*RegisterImagingResponder)    (IBTExtBIP*              po,
                                      const char*             pSrvName,
                                      AEEBTBIPsupCapabilities supCapabilities,
                                      AEEBTBIPsupFeatures     supFeatures,
                                      AEEBTBIPsupFunctions    supFunctions,
                                      uint64                  imagingCapacity,
                                      boolean                 auth);
  int (*RegisterArchivedObjects)     (IBTExtBIP*              po,
                                      const char*             pSrvName,
                                      AEEBTBIPSDUUID128type*  pServiceID,
                                      AEEBTBIPsupFunctions    supFunctions,
                                      boolean                 auth);
  int (*RegisterReferencedObjects)   (IBTExtBIP*              po,
                                      const char*             pSrvName,
                                      AEEBTBIPSDUUID128type*  pServiceID,
                                      AEEBTBIPsupFunctions    supFunctions,
                                      boolean                 auth);

  int (*Deregister)                  (IBTExtBIP*              po);
  int (*AcceptConnection)            (IBTExtBIP*              po);

  /* for client and server */
  int (*Authenticate)                (IBTExtBIP*              po,
                                      const AECHAR*           pPIN);

  /* for client */
  int (*Connect)                     (IBTExtBIP*              po, 
                                      const AEEBTBDAddr*      pBDAddr, 
                                      uint8                   uChannelNumber,
                                      AEEBTBIPSDUUID128type*  pTarget);
  int (*Disconnect)                  (IBTExtBIP*              po);
  int (*Abort)                       (IBTExtBIP*              po);

  int (*GetCapabilities)             (IBTExtBIP*              po,
                                      char*                   pXMLdata,
                                      uint16                  XMLdataMaxSize);
  int (*PutImage)                    (IBTExtBIP*              po,
                                      const char*             pXMLdata,
                                      const char*             pFileName);
  int (*PutLinkedAttachment)         (IBTExtBIP*              po,
                                      const AECHAR*           pImageHandle,
                                      const char*             pXMLdata,
                                      const char*             pFileName);
  int (*PutLinkedThumbnail)          (IBTExtBIP*              po,
                                      const AECHAR*           pImageHandle,
                                      const char*             pFileName);

  int (*GetImageProperties)          (IBTExtBIP*              po,
                                      const AECHAR*           pImageHandle,
                                      char*                   pXMLdata,
                                      uint16                  XMLdataMaxSize);
  int (*GetImageList)                (IBTExtBIP*              po,
                                      const char*             pXMLdata,
                                      uint16                  numRetHandles,
                                      uint16                  startOffset,
                                      boolean                 latest,
                                      char*                   pXMLdata2,
                                      uint16                  XMLdataMaxSize2);
  int (*GetImage)                    (IBTExtBIP*              po,
                                      const AECHAR*           pImageHandle,
                                      const char*             pXMLdata,
                                      const char*             pFileName);
  int (*GetPartialImage)             (IBTExtBIP*              po,
                                      const AECHAR*           pFileName,
                                      uint32                  partialLength,
                                      uint32                  partialOffset);
  int (*GetLinkedAttachment)         (IBTExtBIP*              po,
                                      const AECHAR*           pImageHandle,
                                      const AECHAR*           pAttachmentName);
  int (*GetLinkedThumbnail)          (IBTExtBIP*              po,
                                      const AECHAR*           pImageHandle,
                                      const char*             pFileName);

  int (*DeleteImage)                 (IBTExtBIP*              po,
                                      const AECHAR*           pImageHandle);

  int (*GetMonitoringImage)          (IBTExtBIP*              po,
                                      boolean                 storeFlag,
                                      const char*             pFileName);

  int (*GetStatus)                   (IBTExtBIP*              po );

  int (*RemoteDisplay)               (IBTExtBIP*              po,
                                      const AECHAR*           pImageHandle,
                                      AEEBTBIPRDcmd           displayCommand);

  int (*StartArchive)                (IBTExtBIP*              po,
                                      AEEBTBIPSDUUID128type*  pServiceID);

  int (*StartPrint)                  (IBTExtBIP*              po,
                                      AEEBTBIPSDUUID128type*  pServiceID,
                                      const char*             pDPOFfileName);

  /* for server */
  int (*GetCapabilitiesResponse)     (IBTExtBIP*              po, 
                                      const char*             pXMLdata);
  int (*PutImageResponse)            (IBTExtBIP*              po,
                                      const AECHAR*           pImageHandle,
                                      boolean                 bGetThumbnail);
  int (*PutLinkedAttachmentResponse) (IBTExtBIP*              po,
                                      const char*             pFileName);
  int (*PutLinkedThumbnailResponse)  (IBTExtBIP*              po,
                                      const char*             pFileName);
  int (*GetImagePropertiesResponse)  (IBTExtBIP*              po,
                                      const char*             pXMLdata);
  int (*GetImageListResponse)        (IBTExtBIP*              po,
                                      const char*             pXMLdata,
                                      const char*             pXMLdata2,
                                      uint16                  numImageHandles);
  int (*GetImageResponse)            (IBTExtBIP*              po,
                                      const char*             pFileName);
  int (*GetMonitoringImageResponse)  (IBTExtBIP*              po,
                                      const AECHAR*           pImageHandle,
                                      const char*             pFileName);
  int (*GetPartialImageResponse)     (IBTExtBIP*              po,
                                      const char*             pFileName,
                                      uint32                  partialLength,
                                      uint32                  partialOffset);
  int (*GetLinkedAttachmentResponse) (IBTExtBIP*              po,
                                      const char*             pFileName);
  int (*GetLinkedThumbnailResponse)  (IBTExtBIP*              po,
                                      const char*             pFileName);
  int (*DeleteImageResponse)         (IBTExtBIP*              po,
                                      boolean                 bSuccess);
  int (*RemoteDisplayResponse)       (IBTExtBIP*              po,
                                      const AECHAR*           pImageHandle,
                                      boolean                 bSuccess);
  int (*StartArchiveResponse)        (IBTExtBIP*              po,
                                      boolean                 bSuccess);
};

#define IBTEXTBIP_AddRef(p)                               \
        AEEGETPVTBL((p),IBTExtBIP)->AddRef((p))

#define IBTEXTBIP_Release(p)                              \
        AEEGETPVTBL((p),IBTExtBIP)->Release((p))

#define IBTEXTBIP_QueryInterface(p,i,ppo)                 \
        AEEGETPVTBL((p),IBTExtBIP)->QueryInterface((p),(i),(ppo))

#define IBTEXTBIP_RegisterImagingResponder(p,pn,c1,f1,f2,c2,a)                        \
        AEEGETPVTBL((p),IBTExtBIP)->RegisterImagingResponder((p),(pn),(c1),(f1),(f2),(c2),(a))
        
#define IBTEXTBIP_RegisterArchivedObjects(p,pn,pt,f,a)                        \
        AEEGETPVTBL((p),IBTExtBIP)->RegisterArchivedObjects((p),(pn),(pt),(f),(a))
        
#define IBTEXTBIP_RegisterReferencedObjects(p,pn,pt,f,a)                        \
        AEEGETPVTBL((p),IBTExtBIP)->RegisterReferencedObjects((p),(pn),(pt),(f),(a))

#define IBTEXTBIP_Deregister(p)                           \
        AEEGETPVTBL((p),IBTExtBIP)->Deregister((p))

#define IBTEXTBIP_AcceptConnection(p)            \
        AEEGETPVTBL((p),IBTExtBIP)->AcceptConnection((p))
        
#define IBTEXTBIP_Authenticate(p,pp)            \
        AEEGETPVTBL((p),IBTExtBIP)->Authenticate((p),(pp))

#define IBTEXTBIP_Connect(p,pa,c,pt)                         \
        AEEGETPVTBL((p),IBTExtBIP)->Connect((p),(pa),(c),(pt))

#define IBTEXTBIP_Disconnect(p)                           \
        AEEGETPVTBL((p),IBTExtBIP)->Disconnect((p))
  
#define IBTEXTBIP_Abort(p)                                \
        AEEGETPVTBL((p),IBTExtBIP)->Abort((p))
    
#define IBTEXTBIP_GetCapabilities(p,pc,s)                  \
        AEEGETPVTBL((p),IBTExtBIP)->GetCapabilities((p),(pc),(s))
       
#define IBTEXTBIP_PutImage(p,ph,pf)                      \
        AEEGETPVTBL((p),IBTExtBIP)->PutImage((p),(ph),(pf))
        
#define IBTEXTBIP_PutLinkedAttachment(p,ph,pd,pf)         \
        AEEGETPVTBL((p),IBTExtBIP)->PutLinkedAttachment((p),(ph),(pd),(pf))
        
#define IBTEXTBIP_PutLinkedThumbnail(p,ph,pf)                      \
        AEEGETPVTBL((p),IBTExtBIP)->PutLinkedThumbnail((p),(ph),(pf))
        
#define IBTEXTBIP_GetImageProperties(p,ph,pc,s)                  \
        AEEGETPVTBL((p),IBTExtBIP)->GetImageProperties((p),(ph),(pc),(s))
        
#define IBTEXTBIP_GetImageList(p,pd,n,o,l,pc,s)                  \
        AEEGETPVTBL((p),IBTExtBIP)->GetImageList((p),(pd),(n),(o),(l),(pc),(s))
        
#define IBTEXTBIP_GetImage(p,h,d,pf)                  \
        AEEGETPVTBL((p),IBTExtBIP)->GetImage((p),(h),(d),(pf))
        
#define IBTEXTBIP_GetPartialImage(p,pf,l,o)                  \
        AEEGETPVTBL((p),IBTExtBIP)->GetPartialImage((p),(pf),(l),(o))
        
#define IBTEXTBIP_GetLinkedAttachment(p,ph,pa)                  \
        AEEGETPVTBL((p),IBTExtBIP)->GetLinkedAttachment((p),(ph),(pa))
        
#define IBTEXTBIP_GetLinkedThumbnail(p,ph,pf)                  \
        AEEGETPVTBL((p),IBTExtBIP)->GetLinkedThumbnail((p),(ph),(pf))
        
#define IBTEXTBIP_DeleteImage(p,ph)                  \
        AEEGETPVTBL((p),IBTExtBIP)->DeleteImage((p),(ph))
        
#define IBTEXTBIP_GetMonitoringImage(p,s,pf)                  \
        AEEGETPVTBL((p),IBTExtBIP)->GetMonitoringImage((p),(s),(pf))
        
#define IBTEXTBIP_GetStatus(p)                  \
        AEEGETPVTBL((p),IBTExtBIP)->GetStatus((p))
        
#define IBTEXTBIP_RemoteDisplay(p,ph,c)                  \
        AEEGETPVTBL((p),IBTExtBIP)->RemoteDisplay((p),(ph),(c))
        
#define IBTEXTBIP_StartArchive(p,ps)                  \
        AEEGETPVTBL((p),IBTExtBIP)->StartArchive((p),(ps))
        
#define IBTEXTBIP_StartPrint(p,ps,pd)                  \
        AEEGETPVTBL((p),IBTExtBIP)->StartPrint((p),(ps),(pd))
        
#define IBTEXTBIP_GetCapabilitiesResponse(p,pc)               \
        AEEGETPVTBL((p),IBTExtBIP)->GetCapabilitiesResponse((p),(pc))
        
#define IBTEXTBIP_PutImageResponse(p,ph,b)        \
        AEEGETPVTBL((p),IBTExtBIP)->PutImageResponse((p),(ph),(b))
        
#define IBTEXTBIP_PutLinkedAttachmentResponse(p,pf)        \
        AEEGETPVTBL((p),IBTExtBIP)->PutLinkedAttachmentResponse((p),(pf))
        
#define IBTEXTBIP_PutLinkedThumbnailResponse(p,pf)        \
        AEEGETPVTBL((p),IBTExtBIP)->PutLinkedThumbnailResponse((p),(pf))
        
#define IBTEXTBIP_GetImagePropertiesResponse(p,pd)        \
        AEEGETPVTBL((p),IBTExtBIP)->GetImagePropertiesResponse((p),(pd))

#define IBTEXTBIP_GetImageListResponse(p,pd1,pd2,n)        \
        AEEGETPVTBL((p),IBTExtBIP)->GetImageListResponse((p),(pd1),(pd2),(n))
        
#define IBTEXTBIP_GetImageResponse(p,pf)        \
        AEEGETPVTBL((p),IBTExtBIP)->GetImageResponse((p),(pf))
        
#define IBTEXTBIP_GetMonitoringImageResponse(p,ph,pf)        \
        AEEGETPVTBL((p),IBTExtBIP)->GetMonitoringImageResponse((p),(ph),(pf))
        
#define IBTEXTBIP_GetPartialImageResponse(p,pf,l,o)        \
        AEEGETPVTBL((p),IBTExtBIP)->GetPartialImageResponse((p),(pf),(l),(o))
        
#define IBTEXTBIP_GetLinkedAttachmentResponse(p,pf)        \
        AEEGETPVTBL((p),IBTExtBIP)->GetLinkedAttachmentResponse((p),(pf))
        
#define IBTEXTBIP_GetLinkedThumbnailResponse(p,pf)        \
        AEEGETPVTBL((p),IBTExtBIP)->GetLinkedThumbnailResponse((p),(pf))
        
#define IBTEXTBIP_DeleteImageResponse(p,b)        \
        AEEGETPVTBL((p),IBTExtBIP)->DeleteImageResponse((p),(b))
        
#define IBTEXTBIP_RemoteDisplayResponse(p,ph,b)        \
        AEEGETPVTBL((p),IBTExtBIP)->RemoteDisplayResponse((p),(ph),(b))
        
#define IBTEXTBIP_StartArchiveResponse(p,b)        \
        AEEGETPVTBL((p),IBTExtBIP)->StartArchiveResponse((p),(b))

/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTExtBIP

Description: BlueTooth Basic Imaging Profile

  Consult the Bluetooth ISOD for description of BIP.
  
  The use of IBTExtBIP consists of sending commands and receiving events.  
  These events are delivered to IBTExtBIP clients as notifications, hence
  the clients must register for BIP notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_BIP.  See AEEBTExt.h for
  more information on IBTExtNotifier.
  
  If a command returns SUCCESS, then an event is guaranteed to be generated 
  in response at a later time.  However, not all events are associated with 
  a command.  Some events are asyncronously generated such as a pin request
  sent by remote device.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_BIP_EVT_* events when the notification mask
  is NMASK_BT_BIP.  The pData field of the AEENotify is a pointer to 
  NotificationData.
  
  Event                                                  pData->
  -----                                                  -------------
  
  // Server Events
  AEEBT_BIP_EVT_CONN_REQ                                 bdAddr 
  AEEBT_BIP_EVT_REG_DONE                                 uError
  AEEBT_BIP_EVT_DEREG_DONE                               uError
  
  AEEBT_BIP_EVT_GET_CAPABILITIES_REQ_INITIAL             BipObject
  AEEBT_BIP_EVT_GET_CAPABILITIES_REQ_CLEANUP             BipObject
  AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_REQ_INITIAL         BipObject
  AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_REQ_CLEANUP         BipObject
  AEEBT_BIP_EVT_GET_IMAGE_LIST_REQ_INITIAL               BipObject
  AEEBT_BIP_EVT_GET_IMAGE_LIST_REQ_CLEANUP               BipObject
  AEEBT_BIP_EVT_GET_IMAGE_REQ_INITIAL                    BipObject
  AEEBT_BIP_EVT_GET_IMAGE_REQ_CLEANUP                    BipObject
  AEEBT_BIP_EVT_GET_MONITORING_IMAGE_REQ_INITIAL         BipObject
  AEEBT_BIP_EVT_GET_MONITORING_IMAGE_REQ_CLEANUP         BipObject
  AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_REQ_INITIAL            BipObject
  AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_REQ_CLEANUP            BipObject
  AEEBT_BIP_EVT_GET_ATTACHMENT_REQ_INITIAL               BipObject
  AEEBT_BIP_EVT_GET_ATTACHMENT_REQ_CLEANUP               BipObject
  AEEBT_BIP_EVT_GET_THUMBNAIL_REQ_INITIAL                BipObject
  AEEBT_BIP_EVT_GET_THUMBNAIL_REQ_CLEANUP                BipObject

  AEEBT_BIP_EVT_PUT_IMAGE_REQ_INITIAL                    BipObject
  AEEBT_BIP_EVT_PUT_IMAGE_REQ_CLEANUP                    BipObject
  AEEBT_BIP_EVT_PUT_ATTACHMENT_REQ_INITIAL               BipObject
  AEEBT_BIP_EVT_PUT_ATTACHMENT_REQ_CLEANUP               BipObject
  AEEBT_BIP_EVT_PUT_THUMBNAIL_REQ_INITIAL                BipObject
  AEEBT_BIP_EVT_PUT_THUMBNAIL_REQ_CLEANUP                BipObject

  AEEBT_BIP_EVT_DELETE_IMAGE_REQ_INITIAL                 BipObject
  AEEBT_BIP_EVT_DELETE_IMAGE_REQ_CLEANUP                 BipObject
  AEEBT_BIP_EVT_REMOTE_DISPLAY_REQ_INITIAL               BipObject
  AEEBT_BIP_EVT_REMOTE_DISPLAY_REQ_CLEANUP               BipObject
  AEEBT_BIP_EVT_START_ARCHIVE_REQ_INITIAL                BipObject
  AEEBT_BIP_EVT_START_ARCHIVE_REQ_CLEANUP                BipObject
  
  // Client Events
  AEEBT_BIP_EVT_GET_CAPABILITIES_DONE                    uError
  AEEBT_BIP_EVT_GET_IMAGE_LIST_DONE                      uError
  AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_DONE                uError
  AEEBT_BIP_EVT_GET_IMAGE_DONE                           uError
  AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_DONE                   uError
  AEEBT_BIP_EVT_GET_ATTACHMENT_DONE                      uError
  AEEBT_BIP_EVT_GET_THUMBNAIL_DONE                       uError
  AEEBT_BIP_EVT_GET_MONITORING_IMAGE_DONE                uError
  AEEBT_BIP_EVT_GET_STATUS_DONE                          uError

  AEEBT_BIP_EVT_PUT_IMAGE_DONE                           uError
  AEEBT_BIP_EVT_PUT_ATTACHMENT_DONE                      uError
  AEEBT_BIP_EVT_PUT_THUMBNAIL_DONE                       uError

  AEEBT_BIP_EVT_DELETE_IMAGE_DONE                        uError
  AEEBT_BIP_EVT_REMOTE_DISPLAY_DONE                      uError
  AEEBT_BIP_EVT_START_ARCHIVE_DONE                       uError
  AEEBT_BIP_EVT_START_PRINT_DONE                         uError

  AEEBT_BIP_EVT_CONNECTED                                bdAddr
  AEEBT_BIP_EVT_CONN_FAILED                              bdAddr
  AEEBT_BIP_EVT_DISCONNECTED                             bdAddr

Interfaces:

  IBTEXTBIP provides several additional interfaces; please see the
  corresponding header file for complete documentation.
  
  IBASE                        AEE.h
    IBTEXTBIP_AddRef()
    IBTEXTBIP_Release()
  IQUERYINTERFACE              AEE.h
    IBTEXTBIP_QueryInterface()
    
=======================================================================

Function:  IBTEXTBIP_RegisterImagingResponder()

Description:
  Server application uses this command to initialize the BIP server and 
  registers it in the service discovery database (SDDB.) as
  Imaging Responder.

Prototype:
  int IBTEXTBIP_RegisterImagingResponder(
                          IBTExtBIP*              po,
                          const char*             pSrvName,
                          AEEBTBIPsupCapabilities supCapabilities,
                          AEEBTBIPsupFeatures     supFeatures,
                          AEEBTBIPsupFunctions    supFunctions,
                          uint64                  imagingCapacity,
                          boolean                 auth);

Parameters:
  po                    - pointer to the IBTExtBIP object
  pSrvName              - service name to store in SDDB; must not be NULL
  supCapabilities       - Imaging Capabilites flag 
  supFeatures           - Imaging Features flag 
  supFunctions          - Imaging Functions flag
  imagingCapacity       - Max. memory available for image storage in bytes
  auth                  - Whether to enable authentication

Return value:
  SUCCESS   - the request to register with SDDB was queued and that
              AEEBT_BIP_EVT_REG_DONE event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or unrecognized supported formats

Event:
  AEEBT_BIP_EVT_REG_DONE with accompanying error code of one of the
  following values:
    AEEBT_BIP_ERR_NONE
    AEEBT_BIP_ERR_REC_EXISTS

=======================================================================

Function:  IBTEXTBIP_RegisterArchivedObjects()

Description:
  Server application uses this command to initialize the BIP server and 
  registers it in the service discovery database (SDDB.) as
  Archived Objects server.

Prototype:                          
  int IBTEXTBIP_RegisterArchivedObjects(
                          IBTExtBIP*             po,
                          const char*            pSrvName,
                          AEEBTBIPSDUUID128type* pServiceID,
                          AEEBTBIPsupFunctions   supportedFunctions,
                          boolean                auth);

Parameters:
  po                    - pointer to the IBTExtBIP object
  pSrvName              - service name to store in SDDB; must not be NULL
  pServiceID            - 128-bit service UUID
  supportedFunctions    - Imaging Functions flag
  auth                  - Whether to enable authentication

Return value:
  SUCCESS   - the request to register with SDDB was queued and that
              AEEBT_BIP_EVT_REG_DONE event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or unrecognized supported formats

Event:
  AEEBT_BIP_EVT_REG_DONE with accompanying error code of one of the
  following values:
    AEEBT_BIP_ERR_NONE
    AEEBT_BIP_ERR_REC_EXISTS
=======================================================================

Function:  IBTEXTBIP_RegisterReferencedObjects()

Description:
  Server application uses this command to initialize the BIP server and 
  registers it in the service discovery database (SDDB.) as
  Referenced Objects server.

Prototype:                          
  int IBTEXTBIP_RegisterReferencedObjects(
                          IBTExtBIP*             po,
                          const char*            pSrvName,
                          AEEBTBIPSDUUID128type* pServiceID,
                          AEEBTBIPsupFunctions   supportedFunctions,
                          boolean                auth);

Parameters:
  po                    - pointer to the IBTExtBIP object
  pSrvName              - service name to store in SDDB; must not be NULL
  PServiceID            - 128-bit service UUID
  supportedFunctions    - Imaging Functions flag
  auth                  - Whether to enable authentication

Return value:
  SUCCESS   - the request to register with SDDB was queued and that
              AEEBT_BIP_EVT_REG_DONE event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or unrecognized supported formats

Event:
  AEEBT_BIP_EVT_REG_DONE with accompanying error code of one of the
  following values:
    AEEBT_BIP_ERR_NONE
    AEEBT_BIP_ERR_REC_EXISTS
=======================================================================

Function:  IBTEXTBIP_Deregister()

Description: 
  Terminates the Basic Imaging Profile server (could be Imaging Responder,
  Referenced Objects or Archived Objects server) and deregisters it from the SDDB.

Prototype:
  int IBTEXTBIP_Deregister( IBTExtBIP* po );

Parameters:
  po                    - pointer to the IBTExtBIP object

Return value:
  SUCCESS   - the request to deregister with SDDB was queued and that
              AEEBT_BIP_EVT_DEREG_DONE event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory

Event:
  AEEBT_BIP_EVT_DEREG_DONE with accompanying error code of one of the
  following values:
    AEEBT_BIP_ERR_NONE
    AEEBT_BIP_ERR_REC_NOT_FOUND

=======================================================================

Function:  IBTEXTBIP_AcceptConnection()

Description: 
  The server application must issue this command to accept a 
  connection request from a client. 

Prototype:
  int IBTEXTBIP_AcceptConnection( IBTExtBIP* po );

Parameters:
  po                    - pointer to the IBTExtBIP object

Return value:
  SUCCESS   - the request to accept the connection request was queued
              and that AEEBT_BIP_EVT_CONNECTED event will be generated at a 
              later time (if the server accepted the connection.)
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers

Event:
  AEEBT_BIP_EVT_CONNECTED with the accompanying error code AEEBT_BIP_ERR_NONE.
  
=======================================================================

Function:  IBTEXTBIP_Authenticate()

Description:
  Client and Server application can use this command to authenticate
  the connection being brought up.
  
  Client and Server application should issue this command in response to 
  event AEEBT_BIP_EVT_AUTH_REQ.

Prototype:
  int IBTEXTBIP_Authenticate(
                          IBTExtBIP*             po,
                          const AECHAR*          pPIN);

Parameters:

  po              - pointer to the IBTExtBIP object
  pPIN            - PIN number

Return value:
  SUCCESS   - the request to authenticate was queued.  On the client, 
              AEEBT_BIP_EVT_CONNECTED or AEEBT_BIP_EVT_CONN_FAILED event will 
              be generated at a later time.  On server, the event 
              AEEBT_BIP_EVT_CONN_REQ will be generated if authentication
              is successful
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers

Event:
  For server, if the authentication passes, the event AEEBT_BIP_EVT_CONN_REQ 
  will be generated again with bUnauthorized set to FALSE.  Server can then
  accept or reject the connection request.
  
  For client, depending on whether server accepts the connection request, 
  either the event AEEBT_BIP_EVT_CONNECTED or AEEBT_BIP_EVT_CONN_FAILED 
  will be generated.

=======================================================================

Function:  IBTEXTBIP_Connect()

Description:
  Called by client application to connect to remote BIP server.

Prototype:

  int IBTEXTBIP_Connect(
                          IBTExtBIP*             po,
                          const AEEBTBDAddr*     pBDAddr, 
                          uint8                  uChannelNumber,
                          AEEBTBIPSDUUID128type* pTarget);

Parameters:

  po              - pointer to the IBTExtBIP object
  pBDAddr         - BD address of BIP server
  uChannelNumber  - the server channel number to use to establish connection

Return value:
  SUCCESS   - the request to connect to BIP server was queued and that
              AEEBT_BIP_EVT_CONNECTED or AEEBT_BIP_EVT_CONN_FAILED event will 
              be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or invalid channel number

Event:
  AEEBT_BIP_EVT_CONNECTED or
  AEEBT_BIP_EVT_CONN_FAILED

=======================================================================

Function:  IBTEXTBIP_Disconnect()

Description:
  Client can call this interface to terminate the OBEX connection
  with the server.

Prototype:
  int IBTEXTBIP_Disconnect( IBTExtBIP* po );

Parameters:
  po          - pointer to the IBTExtBIP object

Return value:
  SUCCESS   - the request to disconnect from remote device was queued and that
              AEEBT_BIP_EVT_DISCONNECTED event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  AEEBT_BIP_EVT_DISCONNECTED with the one of the error codes:
    AEEBT_BIP_ERR_NONE
    AEEBT_BIP_ERR_NO_CONN

=======================================================================

Function:  IBTEXTBIP_Abort()

Description:
  Client application can issue this command to abort the current operation.

Prototype:
  int IBTEXTBIP_Abort( IBTExtBIP* po );

Parameters:
  po              - pointer to the IBTExtBIP object

Return value:
  SUCCESS   - the current operation has been aborted.
  EIDLE     - no operation in progress
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTBIP_GetCapabilities()

Description:
  Client issues this command to get the Imaging Capabilities of the
  Server.

Prototype:
  int IBTEXTBIP_GetCapabilities( 
                          IBTExtBIP*     po,
                          char*          pXMLdata,
                          uint16         XMLdataMaxSize);

Parameters:
  po              - pointer to the IBTExtBIP object
  pXMLdata        - pointer to user supplied buffer in which the
                    imaging capability of the server (XML string) will be 
                    returned.  Will be null terminated.
  XMLdataMaxSize  - Size of pXMLdata buffer 

Return value:
  SUCCESS   - the request to get the capabilities object has been queued
              and that AEEBT_BIP_EVT_GET_CAPABILTIES_DONE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_BIP_EVT_GET_CAPABILITIES_DONE

=======================================================================

Function:  IBTEXTBIP_PutImage()

Description:
  Client issues this command to push an image to the server.  Although
  not mandatory, it is recommended that the client first obtain the
  imaging-capabilities object of the server (using GetCapabilities API)
  so that the images are sent to the server in a format that the server 
  supports.

Prototype:
  int IBTEXTBIP_PutImage( 
                          IBTExtBIP*     po,
                          const char*    pXMLData,
                          const char*    pFileName);

Parameters:

  po              - pointer to the IBTExtBIP object
  pXMLdata        - image descriptor (XML string).  Must be null terminated.
                    e.g, a client invoking PutImage function with a JPEG
                    encoded image with size 1280*960 pixels and a size of
                    500000 bytes would construct the following image
                    descriptor:
                    
                    < image-descriptor version="1.0" >
                    < image encoding="JPEG" pixel="1280*960" size="500000"/>
                    < /image-descriptor >

  pFileName       - image filename

Return value:
  SUCCESS   - the request to push the image file has been queued
              and that AEEBT_BIP_EVT_PUT_IMAGE_DONE event will be generated 
              at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_PUT_IMAGE_DONE
  
=======================================================================

Function:  IBTEXTBIP_PutLinkedAttachment()

Description:
  Client issues this command to push the attachment associated
  with an image to the server.  Usually, called after PutImage.

Prototype:
  int IBTEXTBIP_PutLinkedAttachment( 
                          IBTExtBIP*     po,
                          const AECHAR*  pImageHandle,
                          const char*    pXMLdata,
                          const char*    pFileName);

Parameters:

  po              - pointer to the IBTExtBIP object
  pImageHandle    - image handle string (7 wide-chars + 1 null)
  pXMLdata        - attachment descriptor (XML string).  Must be null 
                    terminated.
                    The following is a sample attachment descriptor 
                    XML string:
                    
                    < attachment-descriptor version="1.0" >
                    < attachment name="DSCF0001.txt" 
                      content-type="text/plain" size="5000" />
                    < /attachment-descriptor >

  pFileName       - attachment filename

Return value:
  SUCCESS   - the request to push the attachment file has been queued
              and that AEEBT_BIP_EVT_PUT_LINKED_ATTACHMENT_DONE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_PUT_LINKED_ATTACHMENT_DONE
  
=======================================================================

Function:  IBTEXTBIP_PutLinkedThumbnail()

Description:
  Client issues this command to push the thumbnail version of an image.  
  Usually, called after PutImage if the server requests the thumbnail
  version of the image in response to PutImage.

Prototype:
  int IBTEXTBIP_PutLinkedThumbnail( 
                          IBTExtBIP*     po,
                          const AECHAR*  pImageHandle,
                          const char*    pFileName);

Parameters:

  po              - pointer to the IBTExtBIP object
  pImageHandle    - image handle string (7 wide-chars + 1 null).  
                    The image handle that was assigned to the newly
                    received image by the server in response to PutImage
                    request.
  pFileName       - name of thumbnail image file

Return value:
  SUCCESS   - the request to push the image thumbnail has been queued
              and that AEEBT_BIP_EVT_PUT_LINKED_THUMBNAIL_DONE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_PUT_LINKED_THUMBNAIL_DONE

=======================================================================

Function:  IBTEXTBIP_GetImageProperties()

Description:
  Client issues this command to get the description of an image's
  characteristics (in XML format).
  

Prototype:
  int IBTEXTBIP_GetImageProperties( 
                          IBTExtBIP*     po,
                          const AECHAR*  pImageHandle,
                          char*          pXMLdata,
                          uint16         XMLdataMaxSize);

Parameters:

  po              - pointer to the IBTExtBIP object
  pImageHandle    - image handle string (7 wide-chars + 1 null)
  pXMLdata        - pointer to user supplied buffer in which the
                    imaging properites of the server (XML string) is 
                    returned.  Will be null terminated.
  XMLdataMaxSize  - Size of pXMLdata buffer 

Return value:
  SUCCESS   - the request to get the image properties object has been queued
              and that AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_DONE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_DONE
 
=======================================================================

Function:  IBTEXTBIP_GetImageList()

Description:
  Client issues this command to get server's images-listing object
  (in XML format).
  

Prototype:
  int IBTEXTBIP_GetImageList( 
                          IBTExtBIP*     po,
                          const char*    pXMLdata
                          uint16         numReturnedHandles,
                          uint16         startOffset,
                          boolean        latest,
                          char*          pXMLdata2,
                          uint16         XMLdataMaxSize2);

Parameters:

  po                 - pointer to the IBTExtBIP object
  pXMLdata           - image handles descriptor in XML format.  Must be null
                       terminated.  
                       Specifies the filtering mask to apply to the 
                       images-listing object. For example, a client that wants
                       to retrieve images created on the 1st of January 2000
                       would construct the following image handles descriptor
                       XML string:
                       
                       <image-handles-descriptor version="1.0">
                       <filtering-parameters created="20000101T000000Z-20000101T235959Z"/>
                       </image-handles-descriptor>
                       
                       If no filtering is required, leave this string empty. 
                       
  numReturnedHandles - max number of image handles to be returned in the 
                       images-listing object.  If the client does not want to
                       limit the number of images returned in the 
                       images-listing object, it should set the value to 
                       maximum, i.e 65535. If the client wants to learn the 
                       number of images that would be included in the 
                       images-listing object but doesn't want the actual list,
                       it should set the value to 0.
  startOffset        - zero-based offset from the beginning of the images-listing
                       object.  This parameter can be used to retrieve the
                       images-listing object in pieces.  e.g, a client could
                       send a GetImageList request with numReturnedHandles set
                       to 10 and the startOffset set to 10 to retieve the list
                       of first 10 images, followed by GetImageList with
                       numReturnedHandes set to 10 and startOffset set to 10
                       to get the next 10 images, etc.  
                       It is recommended that the application developer use
                       this functionality if the buffer space is limited
                       and it is anticipated that the server could return a very 
                       large image-listing XML string.
  latest             - Whether to return only the listing of latest captured
                       images. 
                       Setting to TRUE indicates that the images-listings
                       object shall include only locally captured images sorted
                       chronologically by descending capture time.  The size of the
                       list is implementation dependent.  If latest is set to TRUE,
                       the value of startOffset should be 0 (according to the spec).
                       Setting latest to FALSE results in an unrestricted unordered
                       images-listing object.
  pXMLdata2          - pointer to user supplied buffer in which the
                       images list of the server (XML string) is 
                       returned.  Will be null terminated.
  XMLdataMaxSize2    - Size of pXMLdata2 buffer 

Return value:
  SUCCESS   - the request to get the image list object has been queued
              and that AEEBT_BIP_EVT_GET_IMAGE_LIST_DONE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_IMAGE_LIST_DONE
  
=======================================================================

Function:  IBTEXTBIP_GetImage()

Description:
  Client issues this command to get the image corresponding to a
  image handle from the server. 
  
Prototype:
  int IBTEXTBIP_GetImage( 
                          IBTExtBIP*     po,
                          const AECHAR*  pImageHandle,
                          const char*    pXMLdata
                          const char*    pFileName);

Parameters:

  po              - pointer to the IBTExtBIP object
  pImageHandle    - image handle string (7 wide-chars + 1 null)
  pXMLdata        - image descriptor XML string. i.e, image format that the 
                    server should use.  Must be null terminated.  If the
                    client does not provide a descriptor to use, the
                    server will return the native version of the image. 
                    This is useful if the client doesn't need to negotiate
                    image encodings and sizes.
  pFileName       - name of target image file on the client

Return value:
  SUCCESS   - the request to get the image has been queued
              and that AEEBT_BIP_EVT_GET_IMAGE_DONE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_IMAGE_DONE
  
=======================================================================

Function:  IBTEXTBIP_GetPartialImage()

Description:
  GetPartialImage is a modified version of the GetImage function that
  can be used to retrieve either an entire image file or part of an 
  image file.
  
Prototype:
  int IBTEXTBIP_GetPartialImage( 
                          IBTExtBIP*     po,
                          const AECHAR*  pFileName,
                          uint32         partialLength,
                          uint32         partialOffset);

Parameters:

  po              - pointer to the IBTExtBIP object
  pFileName       - name of the image file
  partialLength   - length of the partial file to be returned in the
                    response.
  partialOffset   - first byte of the file from which the partial file
                    starts.  
                    
  Set partialLength to 0x00000000 and partialOffset to 0xFFFFFFFF to
  retrieve the entire image.

Return value:
  SUCCESS   - the request to get the partial image has been queued
              and that AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_DONE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_DONE
  
=======================================================================

Function:  IBTEXTBIP_GetLinkedAttachment()

Description:
  Client issues this command to get the attachment associated with
  an image that it retrieved earlier using GetImage interface.
  
Prototype:
  int IBTEXTBIP_GetLinkedAttachment( 
                          IBTExtBIP*     po,
                          const AECHAR*  pImageHandle,
                          const AECHAR*  pAttachmentName);

Parameters:

  po              - pointer to the IBTExtBIP object
  pImageHandle    - image handle string (7 wide-chars + 1 null)
  pAttachmentName - name of the attachment file 

Return value:
  SUCCESS   - the request to get the partial image has been queued
              and that AEEBT_BIP_EVT_GET_LINKED_ATTACHMENT_DONE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_LINKED_ATTACHMENT_DONE
  
=======================================================================

Function:  IBTEXTBIP_GetLinkedThumbnail()

Description:
  Client issues this command to get the thumbnail version of
  an image that it retrieved earlier using GetImage interface.
  
Prototype:
  int IBTEXTBIP_GetLinkedThumbnail( 
                          IBTExtBIP*     po,
                          const AECHAR*  pImageHandle,
                          const char*    pFileName);

Parameters:

  po              - pointer to the IBTExtBIP object
  pImageHandle    - image handle string (7 wide-chars + 1 null)
  pFileName       - name of target attachment file

Return value:
  SUCCESS   - the request to get the thubnail version of an image has been
              queuedand that AEEBT_BIP_EVT_GET_LINKED_THUMBNAIL_DONE event
              will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_LINKED_THUMBNAIL_DONE

=======================================================================

Function:  IBTEXTBIP_DeleteImage()

Description:
  Client issues this command to delete the image given by the image handle.
  
Prototype:
  int IBTEXTBIP_DeleteImage( 
                          IBTExtBIP*     po,
                          const AECHAR*  pImageHandle);

Parameters:

  po              - pointer to the IBTExtBIP object
  pImageHandle    - image handle string (7 wide-chars + 1 null)

Return value:
  SUCCESS   - the request to delete the image file has been queued
              and that AEEBT_BIP_EVT_DELETE_IMAGE_DONE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_DELETE_IMAGE_DONE
  
=======================================================================

Function:  IBTEXTBIP_GetMonitoringImage()

Description:
  Client issues this command to get the monitoring images from an
  object exchange server with capturing capability (e.g, camera)
  
Prototype:
  int IBTEXTBIP_GetMonitoringImage( 
                          IBTExtBIP*     po,
                          boolean        storeFlag,
                          const char*    pFileName);

Parameters:

  po              - pointer to the IBTExtBIP object
  storeFlag       - whether to capture the image on the server
                    (i.e, remote camera)
  pFileName       - name of target image file

Return value:
  SUCCESS   - the request to get the monitored image has been queued
              and that AEEBT_BIP_EVT_GET_MONITORING_IMAGE_DONE event
              will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_MONITORING_IMAGE_DONE
  
=======================================================================

Function:  IBTEXTBIP_GetStatus()

Description:
  Issued by a primary OBEX client to monitor a secondary OBEX connection. 
  
Prototype:
  int IBTEXTBIP_GetStatus( 
                          IBTExtBIP*     po);

Parameters:

  po              - pointer to the IBTExtBIP object

Return value:
  SUCCESS   - the request to get the status of a secondary OBEX connection
              has been queued and that AEEBT_BIP_EVT_GET_STATUS_DONE event
              will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_STATUS_DONE
  
=======================================================================

Function:  IBTEXTBIP_RemoteDisplay()

Description:
  Issued by a client to pilot the screen of an OBEX server with display
  capability.
  
Prototype:
  int IBTEXTBIP_RemoteDisplay( 
                          IBTExtBIP*     po,
                          const AECHAR*  pImageHandle,
                          AEEBTBIPRDcmd  displayCommand);

Parameters:

  po              - pointer to the IBTExtBIP object
  pImageHandle    - image handle string
  displayCommand  - whether to display next, previous, current or selected
                    image (as given by image handle).  If SelectImage value
                    is used, the image handle string must be present and 
                    must be set to the handle of the image to display.  The
                    image handle string must be empty for NextImage, PreviousImage
                    and CurrentImage.

Return value:
  SUCCESS   - the request to display the image as given by the display command
              has been queued and that AEEBT_BIP_EVT_REMOTE_DISPLAY_DONE event
              will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_REMOTE_DISPLAY_DONE
  
=======================================================================

Function:  IBTEXTBIP_StartArchive()

Description:
  Issued by a client to trigger an imaging server to start draining the
  client of its image files (some of or all of the image files)
  
Prototype:
  int IBTEXTBIP_StartArchive( 
                          IBTExtBIP*     po,
                          AEEBTBIPSDUUID128type* pServiceID);

Parameters:

  po              - pointer to the IBTExtBIP object
  pServiceID      - 128-bit ServiceID used by the server to determine
                    which Automatic Archive service record on the client
                    to read.

Return value:
  SUCCESS   - the request to archive the image files on the client 
              has been queued and that AEEBT_BIP_EVT_START_ARCHIVE_DONE event
              will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_START_ARCHIVE_DONE

=======================================================================

Function:  IBTEXTBIP_StartPrint()

Description:
  Issued by a client to trigger an object exchange server with print
  capability to execute a print job.  The print job is described in the
  printer-control object sent by the server.
  
Prototype:
  int IBTEXTBIP_StartPrint( 
                          IBTExtBIP*             po,
                          AEEBTBIPSDUUID128type* pServiceID,
                          const char*            pDPOFfileName);

Parameters:

  po              - pointer to the IBTExtBIP object
  pServiceID      - 128-bit ServiceID used by the server to determine
                    which Referenced Object service record on the client
                    to read.
  pDPOFfileName   - Name of the printer-control object file. 
  
                    The printer-control object is a text description
                    of a print job based on the DPOF 1.1 standard.  The
                    following figure illustrates an example file system
                    structure and its associated DPOF file:
                    
                    Root --- DCIM --- 100ABCDE --- ABCD0001.JPG --> "1000001"
                          |        |            |- ABCD0002.JPG --> "1000002"
                          |        |            |- ABCD0003.JPG --> "1000003"
                          |        |            |- ABCD0004.JPG --> "1000004"
                          |        |            |- ABCD0005.JPG --> "1000005"
                          |        |            |- ABCD0006.JPG --> "1000006"
                          |        |
                          |        |- 102_BLUE --- BLUE0001.JPG --> "1020001"
                          |
                          |- MISC --- AUTRINT.MRK --> DPOF File
                    
                    [HDR] 
                    GEN REV = 01.10 
                    GEN CRT = "Bluetooth camera" -01.00 
                    GEN DTM = 2001:01:01:12:00:00 
                    
                    [JOB] 
                    PRT PID = 001 
                    PRT TYP = STD
                    PTR QTY = 001 
                    IMG FMT = EXIF2 -J 
                    <IMG SRC = "../DCIM/100ABCDE/ABCD0001.JPG"> 
                    CFG DSC = "100-0001" -ATR FID 
                    
                    [JOB] 
                    PRT PID = 002 
                    PRT TYP = STD 
                    PRT QTY = 002 
                    IMG FMT = EXIF2 -J 
                    <IMG SRC = "../DCIM/100ABCDE/ABCD0002.JPG"> 
                    CFG DSC = "2000.12.24" -ATR DTM 
                    CFG DSC = "100-0002" -ATR FID 
                    
                    [JOB] 
                    PRT PID = 003 
                    PRT TYP = STD 
                    PRT QTY = 001 
                    IMG FMT = EXIF2 -J 
                    <IMG SRC = "../DCIM/100ABCDE/ABCD0003.JPG"> 
                    CFG DSC = "2000.12.25" -ATR DTM 
                    CFG DSC = "100-0003" -ATR FID 
                    
                    [JOB] 
                    PRT PID = 004 
                    PRT TYP = STD 
                    PRT QTY = 003 
                    IMG FMT = EXIF2 -J <IMG SRC = "../DCIM/102_BLUE/BLUE0001.JPG"> 
                    CFG DSC = "102-0001" -ATR FID 
                    
                    [JOB] 
                    PRT PID = 100 
                    PRT TYP = IDX 
                    PRT QTY = 001 
                    IMG FMT = EXIF2 -J IMG SRC = "../DCIM/100ABCDE/ABCD0001.JPG" 
                    IMG SRC = "../DCIM/100ABCDE/ABCD0002.JPG" 
                    IMG SRC = "../DCIM/100ABCDE/ABCD0003.JPG" 
                    IMG SRC = "../DCIM/102_BLUE/BLUE0001.JPG"

Return value:
  SUCCESS   - the request to start a print job by the client 
              has been queued and that AEEBT_BIP_EVT_START_PRINT_DONE event
              will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_START_PRINT_DONE
  
=======================================================================

Function:  IBTEXTBIP_GetCapabilitiesResponse()

Description:
  Issued by the server in response to GetCapabilities request from
  the client as given by AEEBT_BIP_EVT_GET_CAPABILITIES_REQ_INITIAL
  event.  
  
Prototype:
  int IBTEXTBIP_GetCapabilitiesResponse( 
                          IBTExtBIP*     po,
                          const char*    pXMLdata);

Parameters:

  po              - pointer to the IBTExtBIP object
  pXMLdata        - buffer containing imaging capabilities XML string.
                    Must be null terminated.  
                    
                    The following is a sample imaging-capabilities
                    object XML string:
                    
                    <imaging-capabilities version="1.0">
                    <preferred-format encoding="JPEG" pixel=1280*960 />
                    <image-formats encoding="JPEG" pixel="160*120" maxsize="5000" />
                    <image-formats encoding="JPEG" pixel="320*240" /> 
                    <image-formats encoding="JPEG" pixel="640*480" /> 
                    <image-formats encoding="JPEG" pixel="1280*960" /> 
                    <attachment-formats content-type="audio/basic" /> 
                    <filtering-parameters created="1" modified="1" /> 
                    </imaging-capabilities> 
                    
                    Note that preferred-format, image-formats and 
                    attachment-formats elements are applicable only in
                    Image Push or Remote Display scenario (where the
                    client has to learn which formats the server supports)
                    or an Advanced Image Printing scenario (where the
                    client prepares the images in a format the server, i.e
                    printer, understands).  The filtering-parameters
                    element is used only when the client uses an imaging
                    feature that includes the GetImageList function.

Return value:
  SUCCESS   - the response command has been successfully sent.  
              AEEBT_BIP_EVT_GET_CAPABILITIES_REQ_CLEANUP event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_CAPABILITIES_REQ_INITIAL
  AEEBT_BIP_EVT_GET_CAPABILITIES_REQ_CLEANUP
  
=======================================================================

Function:  IBTEXTBIP_PutImageResponse()

Description:
  Issued by the server in response to PutImage request from
  the client as given by AEEBT_BIP_EVT_PUT_IMAGE_REQ_INITIAL
  event.  
  
Prototype:
  int IBTEXTBIP_PutImageResponse( 
                          IBTExtBIP*     po,
                          const AECHAR*  pImageHandle,
                          boolean        bGetThumbnail);

Parameters:

  po              - pointer to the IBTExtBIP object
  pImageHandle    - Image Handle string
  bGetThumbnail   - Whether to request the thumbnail version of the image.
                    TRUE => get thumbnail version following the image transfer.
                    FALSE => don't get thumbnail version for the image.

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_BIP_EVT_PUT_IMAGE_REQ_CLEANUP event will be generated
              at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_PUT_IMAGE_REQ_INITIAL
  AEEBT_BIP_EVT_PUT_IMAGE_REQ_CLEANUP
  
=======================================================================

Function:  IBTEXTBIP_PutLinkedAttachmentResponse()

Description:
  Issued by the server in response to PutLinkedAttachment request from
  the client as given by AEEBT_BIP_EVT_PUT_LINKED_ATTACHMENT_REQ_INITIAL
  event.
  
Prototype:
  int IBTEXTBIP_PutLinkedAttachmentResponse( 
                          IBTExtBIP*     po,
                          const char*    pFileName);

Parameters:

  po              - pointer to the IBTExtBIP object
  pFileName       - attachment filename

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_BIP_EVT_PUT_LINKED_ATTACHMENT_REQ_CLEANUP event will be
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_PUT_LINKED_ATTACHMENT_REQ_INITIAL
  AEEBT_BIP_EVT_PUT_LINKED_ATTACHMENT_REQ_CLEANUP
  
=======================================================================

Function:  IBTEXTBIP_PutLinkedThumbnailResponse()

Description:
  Issued by the server in response to PutLinkedThumbnail request from
  the client as given by AEEBT_BIP_EVT_PUT_LINKED_THUMBNAIL_REQ_INITIAL
  event.
  
Prototype:
  int IBTEXTBIP_PutLinkedThumbnailResponse( 
                          IBTExtBIP*     po,
                          const char*    pFileName);

Parameters:

  po              - pointer to the IBTExtBIP object
  pFileName       - thumbnail image filename

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_BIP_EVT_PUT_LINKED_THUMBNAIL_REQ_CLEANUP event will
              be generated at a later time
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_PUT_LINKED_THUMBNAIL_REQ_INITIAL
  AEEBT_BIP_EVT_PUT_LINKED_THUMBNAIL_REQ_CLEANUP
  
=======================================================================

Function:  IBTEXTBIP_GetImagePropertiesResponse()

Description:
  Issued by the server in response to GetImageProperties request from
  the client as given by AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_REQ_INITIAL
  event.
  
Prototype:
  int IBTEXTBIP_GetImagePropertiesResponse( 
                          IBTExtBIP*     po,
                          const char*    pXMLdata);

Parameters:

  po              - pointer to the IBTExtBIP object
  pXMLdata        - image properties XML string.  Must be null terminated.
  
                    The following example image-properties object shows
                    that Image 1000001 is available in JPEG and GIF encodings.
                    In JPEG, it can be delivered in 1280*1024, 640*480 or
                    160*120 sizes.  In GIF, it can be delievered in resolutions
                    between 80*60 and 640*480.  The native image format is
                    JPEG 1280*1024 and its size is 1Mbyte.  Two attachments
                    are available, one text and one audio; their sizes are
                    5Kbytes (5120 bytes) and 100Kbytes (102400 bytes).
                    
                    <image-properties version="1.0" handle="1000001">
                    <native encoding="JPEG" pixel="1280*1024" size="1048576"/>
                    <variant encoding="JPEG" pixel="640*480" />
                    <variant encoding="JPEG" pixel="160*120" />
                    <variant encoding="GIF" pixel="80*60-640*480"/>
                    <attachment content-type="text/plain" 
                     name="ABCD0001.txt" size="5120"/>
                    <attachment content-type="audio/basic" 
                     name="ABCD0001.wav" size="102400"/>
                    </image-properties>

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_REQ_CLEANUP will be
              generate at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_REQ_INITIAL
  AEEBT_BIP_EVT_GET_IMAGE_PROPERTIES_REQ_CLEANUP
  
=======================================================================

Function:  IBTEXTBIP_GetImageListResponse()

Description:
  Issued by the server in response to GetImageList request from
  the client as given by AEEBT_BIP_EVT_GET_IMAGE_LIST_REQ_INITIAL
  event.
  
Prototype:
  int IBTEXTBIP_GetImageListResponse( 
                          IBTExtBIP*     po,
                          const char*    pXMLdata,
                          const char*    pXMLdata2,
                          uint16         numImageHandles);

Parameters:

  po              - pointer to the IBTExtBIP object
  pXMLdata        - images-list object XML string.  Must be null terminated.
                    The following is a sample images-list object XML string:
                   
                    <images-listing version="1.0" >
                    <image handle="1000001" created="20000801T060000Z" />
                    <image handle="1000003" created="20000801T060115Z" 
                     modified="20000808T071500Z" />
                    <image handle="1000004" created="20000801T060137Z" />
                    </images-listing>
                    
                    It shows that the source device (server) has 3 images.
                    Image 1000001 was created on the 1st August 2000
                    at 6:00 UTC.  Image 1000003 was created on the 1st of
                    August 2000 at 6:01 UTC and modified on the 8th August
                    at 07:15:00 UTC.  Image 1000004 was created on 1st August
                    2000 at 6:01:37 UTC.
               
  pXMLdata2       - image-handles descriptor filter string.  Must be null 
                    terminated.  It indicates the filtering parameters
                    that were applied when building the images-listing
                    object.  If all the filtering parameters indicated
                    in the GetImageList request were applied, the 
                    image-handles descriptor in the response should be
                    identical to the one in the request.  Otherwise,
                    it is left to the implementor.  For instance, a 
                    server implementation that does not support UTC
                    notation used by the client in the image-handles
                    descriptor of the request, might return values for
                    the created or modified filtering parameters in local
                    time format.
  
  numImageHandles - number of image handles.  If numRetHandles in 
                    client's GetImageList API is set to 0, that means, the
                    client wants to learn about the number of images but not 
                    the actual images that would be included in the 
                    images-listing object.

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_BIP_EVT_GET_IMAGE_LIST_REQ_CLEANUP event will be
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_IMAGE_LIST_REQ_INITIAL
  AEEBT_BIP_EVT_GET_IMAGE_LIST_REQ_CLEANUP
  
=======================================================================

Function:  IBTEXTBIP_GetImageResponse()

Description:
  Issued by the server in response to GetImage request from
  the client as given by AEEBT_BIP_EVT_GET_IMAGE_REQ_INITIAL event.
  
Prototype:
  int IBTEXTBIP_GetImageResponse( 
                          IBTExtBIP*     po,
                          const char*    pFileName);

Parameters:

  po              - pointer to the IBTExtBIP object
  pFileName       - image filename

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_BIP_EVT_GET_IMAGE_REQ_CLEANUP event will be
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_IMAGE_REQ_INITIAL
  AEEBT_BIP_EVT_GET_IMAGE_REQ_CLEANUP
  
=======================================================================

Function:  IBTEXTBIP_GetMonitoringImageResponse()

Description:
  Issued by the server in response to GetMonitoringImage request from
  the client as given by AEEBT_BIP_EVT_GET_MONITORING_IMAGE_REQ_INITIAL
  event.
  
Prototype:
  int IBTEXTBIP_GetMonitoringImageResponse( 
                          IBTExtBIP*     po,
                          const AECHAR*  pImageHandle,
                          const char*    pFileName);

Parameters:

  po              - pointer to the IBTExtBIP object
  pImageHandle    - image handle string
  pFileName       - target image filename

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_BIP_EVT_GET_MONITORING_IMAGE_REQ_CLEANUP event will
              be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_MONITORING_IMAGE_REQ_INITIAL
  AEEBT_BIP_EVT_GET_MONITORING_IMAGE_REQ_CLEANUP
  
=======================================================================

Function:  IBTEXTBIP_GetPartialImageResponse()

Description:
  Issued by the server in response to GetPartialImage request from
  the client as given by AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_REQ_INITIAL event.
  
Prototype:
  int IBTEXTBIP_GetPartialImageResponse( 
                          IBTExtBIP*     po,
                          const char*    pFileName,
                          uint32         partialLength,
                          uint32         partialOffset);

Parameters:

  po              - pointer to the IBTExtBIP object
  pFileName       - name of image file
  partialLength   - length of the partial file to be returned in the response.
                    See GetPartialImage API for more information.
  partialOffset   - first byte of the file from which the partial file starts.
                    See GetPartialImage API for more information.

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_REQ_CLEANUP event will be
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_REQ_INITIAL
  AEEBT_BIP_EVT_GET_PARTIAL_IMAGE_REQ_CLEANUP
  
=======================================================================

Function:  IBTEXTBIP_GetLinkedAttachmentResponse()

Description:
  Issued by the server in response to GetLinkedAttachment request from
  the client as given by AEEBT_BIP_EVT_GET_ATTACHMENT_REQ_INITIAL event.
  
Prototype:
  int IBTEXTBIP_GetLinkedAttachmentResponse( 
                          IBTExtBIP*     po,
                          const char*    pFileName);

Parameters:

  po              - pointer to the IBTExtBIP object
  pFileName       - name of attachment file

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_BIP_EVT_GET_ATTACHMENT_REQ_CLEANUP event will be
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_ATTACHMENT_REQ_INITIAL
  AEEBT_BIP_EVT_GET_ATTACHMENT_REQ_CLEANUP
  
=======================================================================

Function:  IBTEXTBIP_GetLinkedThumbnailResponse()

Description:
  Issued by the server in response to GetLinkedThumbnail request from
  the client as given by AEEBT_BIP_EVT_GET_THUMBNAIL_REQ_INITIAL event.
  
Prototype:
  int IBTEXTBIP_GetLinkedThumbnailResponse( 
                          IBTExtBIP*     po,
                          const char*    pFileName);

Parameters:

  po              - pointer to the IBTExtBIP object
  pFileName       - name of thumbnail image file

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_BIP_EVT_GET_THUMBNAIL_REQ_CLEANUP event will be
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_GET_THUMBNAIL_REQ_INITIAL
  AEEBT_BIP_EVT_GET_THUMBNAIL_REQ_CLEANUP
  
=======================================================================

Function:  IBTEXTBIP_DeleteImageResponse()

Description:
  Issued by the server in response to DeleteImage request from
  the client as given by AEEBT_BIP_EVT_DELETE_IMAGE_REQ_INITIAL event.
  
Prototype:
  int IBTEXTBIP_GetLinkedThumbnailResponse( 
                          IBTExtBIP*     po,
                          boolean        bSuccess);

Parameters:

  po              - pointer to the IBTExtBIP object
  bSuccess        - whether the operation was successful

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_BIP_EVT_DELETE_IMAGE_REQ_CLEANUP event will be
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_DELETE_IMAGE_REQ_INITIAL
  AEEBT_BIP_EVT_DELETE_IMAGE_REQ_CLEANUP
  
=======================================================================

Function:  IBTEXTBIP_RemoteDisplayResponse()

Description:
  Issued by the server in response to RemoteDisplay request from
  the client as given by AEEBT_BIP_EVT_REMOTE_DISPLAY_REQ_INITIAL event.
  
Prototype:
  int IBTEXTBIP_RemoteDisplayResponse( 
                          IBTExtBIP*     po,
                          const AECHAR*  pImageHandle,
                          boolean        bSuccess);

Parameters:

  po              - pointer to the IBTExtBIP object
  pImageHandle    - image handle string
  bSuccess        - whether the operation was successful

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_BIP_EVT_REMOTE_DISPLAY_REQ_CLEANUP event will be
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_REMOTE_DISPLAY_REQ_INITIAL
  AEEBT_BIP_EVT_REMOTE_DISPLAY_REQ_CLEANUP
  
=======================================================================

Function:  IBTEXTBIP_StartArchiveResponse()

Description:
  Issued by the server in response to StartArchive request from
  the client as given by AEEBT_BIP_EVT_START_ARCHIVE_REQ_INITIAL event.
  
Prototype:
  int IBTEXTBIP_StartArchiveResponse( 
                          IBTExtBIP*     po,
                          boolean        bSuccess);

Parameters:

  po              - pointer to the IBTExtBIP object
  bSuccess        - whether the operation was successful

Return value:
  SUCCESS   - the response command has been successfully sent.
              AEEBT_BIP_EVT_START_ARCHIVE_REQ_CLEANUP event will be
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_BIP_EVT_START_ARCHIVE_REQ_INITIAL
  AEEBT_BIP_EVT_START_ARCHIVE_REQ_CLEANUP

=======================================================================*/

#endif /* _AEE_BT_EXT_BIP_H_ */
