/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/inc/oi_bip_spec.h#1 $ 

$Log: $
*
*    #6        18 Oct 2006            PR
* Merged OI Code v3.0.14rev1revQ
*
*    #5        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
*    #4        19 Oct 2004            JH
* Added nbReturnedHandles to OI_BIP_GET_IMAGES_LIST_PARAMS response.
*
*    #3        23 Jul 2004            JH
* Updated to version 3.0.12 revision 1 baseline
*
===========================================================================*/

/** 
@file 

Basic Imaging Profile definitions
  
Each BIP function (except GET_STATUS) has an associated parameter
block defined in this file. Although the parameter blocks differ for
the various functions, they are substantially similar. A parameter
block has a 'request' sub-struct, a 'response' sub-struct, or both. If
a function involves the transfer of body data, one of these
sub-structs will have 'data' and 'final' fields, depending on the
direction of the data transfer.
 
A BIP client implementation is responsible for setting the values in
the request field. A server implementation sets the values in the
response field.  If body data is present in a request or response, the
'data' field points to an OI_OBEX_BYTESEQ describing the data, and
'final' field indicates whether this is the final data packet.
 
If body data entails a function with more than one request-response
round-trip then the 'data' field (if present) should be the only
request field present after the first request. Similarly, the 'data'
field (if present) should be the only response field present before
the final response.

 */

/** \addtogroup BIP BIP APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#ifndef _OI_BIP_SPEC_H
#define _OI_BIP_SPEC_H

#include "oi_bt_spec.h"
#include "oi_stddefs.h"
#include "oi_obexspec.h"

#define OI_BIP_VERSION 0x0100



#define OI_BIP_SDP_CAPABILITY_GENERIC_IMAGING       BIT0 /**< BIP SDP constant for generic imaging capability.
                                                              BIP specification version 1.0 final, section 6.1.1. */
#define OI_BIP_SDP_CAPABILITY_CAPTURING             BIT1 /**< BIP SDP constant for capturing capability.
                                                              BIP specification version 1.0 final, section 6.1.1. */
#define OI_BIP_SDP_CAPABILITY_PRINTING              BIT2 /**< BIP SDP constant for printing capability.
                                                              BIP specification version 1.0 final, section 6.1.1. */
#define OI_BIP_SDP_CAPABILITY_DISPLAYING            BIT3 /**< BIP SDP constant for displaying capability.
                                                              BIP specification version 1.0 final, section 6.1.1. */

#define OI_BIP_SDP_FEATURE_IMAGE_PUSH               BIT0 /**< BIP SDP constant for ImagePush feature. 
                                                              BIP specification version 1.0 final, section 6.1.1. */
#define OI_BIP_SDP_FEATURE_IMAGE_PUSH_STORE         BIT1 /**< BIP SDP constant for ImagePush-Store feature. 
                                                              BIP specification version 1.0 final, section 6.1.1. */
#define OI_BIP_SDP_FEATURE_IMAGE_PUSH_PRINT         BIT2 /**< BIP SDP constant for ImagePush-Print feature. 
                                                              BIP specification version 1.0 final, section 6.1.1. */
#define OI_BIP_SDP_FEATURE_IMAGE_PUSH_DISPLAY       BIT3 /**< BIP SDP constant for ImagePush-Display feature. 
                                                              BIP specification version 1.0 final, section 6.1.1. */
#define OI_BIP_SDP_FEATURE_IMAGE_PULL               BIT4 /**< BIP SDP constant for ImagePull feature. 
                                                              BIP specification version 1.0 final, section 6.1.1. */
#define OI_BIP_SDP_FEATURE_ADVANCED_IMAGE_PRINTING  BIT5 /**< BIP SDP constant for AdvancedImagePrinting feature. 
                                                              BIP specification version 1.0 final, section 6.1.1. */
#define OI_BIP_SDP_FEATURE_AUTOMATIC_ARCHIVE        BIT6 /**< BIP SDP constant for AutomaticArchive feature. 
                                                              BIP specification version 1.0 final, section 6.1.1. */
#define OI_BIP_SDP_FEATURE_REMOTE_CAMERA            BIT7 /**< BIP SDP constant for RemoteCamera feature. 
                                                              BIP specification version 1.0 final, section 6.1.1. */
#define OI_BIP_SDP_FEATURE_REMOTE_DISPLAY           BIT8 /**< BIP SDP constant for RemoteDisplay feature. 
                                                              BIP specification version 1.0 final, section 6.1.1. */

#define OI_BIP_SDP_FUNCTION_GET_CAPABILITIES        BIT0 /**< GetCapabilities function.
                                                              BIP specification version 1.0 final, section 4.5.1. */
#define OI_BIP_SDP_FUNCTION_PUT_IMAGE               BIT1 /**< PutImage function.
                                                              BIP specification version 1.0 final, section 4.5.2. */
#define OI_BIP_SDP_FUNCTION_PUT_LINKED_ATTACHMENT   BIT2 /**< PutLinkedAttachment function.
                                                              BIP specification version 1.0 final, section 4.5.4. */
#define OI_BIP_SDP_FUNCTION_PUT_LINKED_THUMBNAIL    BIT3 /**< PutLinkedThumbnail function.
                                                              BIP specification version 1.0 final, section 4.5.3. */
#define OI_BIP_SDP_FUNCTION_REMOTE_DISPLAY          BIT4 /**< RemoteDisplay function.
                                                              BIP specification version 1.0 final, section 4.5.5. */
#define OI_BIP_SDP_FUNCTION_GET_IMAGES_LIST         BIT5 /**< GetImagesList function.
                                                              BIP specification version 1.0 final, section 4.5.6. */
#define OI_BIP_SDP_FUNCTION_GET_IMAGE_PROPERTIES    BIT6 /**< GetImageProperties function.
                                                              BIP specification version 1.0 final, section 4.5.7. */
#define OI_BIP_SDP_FUNCTION_GET_IMAGE               BIT7 /**< GetImage function.
                                                              BIP specification version 1.0 final, section 4.5.8. */
#define OI_BIP_SDP_FUNCTION_GET_LINKED_THUMBNAIL    BIT8 /**< GetLinkedThumbnail function.
                                                              BIP specification version 1.0 final, section 4.5.9. */
#define OI_BIP_SDP_FUNCTION_GET_LINKED_ATTACHMENT   BIT9 /**< GetLinkedAttachment function.
                                                              BIP specification version 1.0 final, section 4.5.10. */
#define OI_BIP_SDP_FUNCTION_DELETE_IMAGE            BIT10 /**< DeleteImage function.
                                                               BIP specification version 1.0 final, section 4.5.11. */
#define OI_BIP_SDP_FUNCTION_START_PRINT             BIT11 /**< StartPrint function.
                                                               BIP specification version 1.0 final, section 4.5.12. */
#define OI_BIP_SDP_FUNCTION_GET_PARTIAL_IMAGE       BIT12 /**< GetPartialImage function.
                                                               BIP specification version 1.0 final, section 4.5.13. */
#define OI_BIP_SDP_FUNCTION_START_ARCHIVE           BIT13 /**< StartArchive function.
                                                               BIP specification version 1.0 final, section 4.5.14. */
#define OI_BIP_SDP_FUNCTION_GET_MONITORING_IMAGE    BIT14 /**< GetMonitoringImage function.
                                                               BIP specification version 1.0 final, section 4.5.16. */
    /* RESERVED BIT15, */
#define OI_BIP_SDP_FUNCTION_GET_STATUS              BIT16 /**< GetStatus function.
                                                               BIP specification version 1.0 final, section 4.5.15. */

/** 
A type of state parameter used to indicate the phase of 
a BIP response in many callback functions in oi_bip_srv.h.
*/
typedef enum {
    OI_BIP_REQUEST_INITIAL,
    OI_BIP_REQUEST_CONTINUE,
    OI_BIP_REQUEST_CLEANUP
} OI_BIP_REQUEST_STATE;

/** 
RemoteDisplay screen control commands.
See Bluetooth BIP specification version 1.0 final, section 4.5.5. 
*/
typedef enum {
    OI_BIP_REMOTE_DISPLAY_NEXT_IMAGE = 1,
    OI_BIP_REMOTE_DISPLAY_PREVIOUS_IMAGE = 2,
    OI_BIP_REMOTE_DISPLAY_SELECT_IMAGE = 3,
    OI_BIP_REMOTE_DISPLAY_CURRENT_IMAGE = 4
} OI_BIP_REMOTE_DISPLAY_COMMAND;


/* BIP operation parameter structures */

/** Parameter block for the GetCapabilities function OI_BIPCLI_GetCapabilities().
See Bluetooth BIP specification version 1.0 final, section 4.5.1. */
typedef struct {
    struct {
        OI_OBEX_BYTESEQ *data;
        OI_BOOL final;
    } response;
} OI_BIP_GET_CAPABILITIES_PARAMS;


/** Parameter block for the GetMonitoringImage function OI_BIPCLI_GetMonitoringImage().
See Bluetooth BIP specification version 1.0 final, section 4.5.16. */
typedef struct {
    struct {
        OI_BOOL storeImage;
    } request;

    struct {
        OI_OBEX_UNICODE *imageHandle;
        OI_OBEX_BYTESEQ *data;
        OI_BOOL final;
    } response;
} OI_BIP_GET_MONITORING_IMAGE_PARAMS;


/** Parameter block for the PutImage function OI_BIPCLI_PutImage().
See Bluetooth BIP specification version 1.0 final, section 4.5.2. */
typedef struct {
    struct {
        OI_OBEX_UNICODE *imageName;
        OI_OBEX_BYTESEQ *imageDescriptor;
        OI_OBEX_BYTESEQ *data;
        OI_BOOL final;
    } request;

    struct {
        OI_OBEX_UNICODE *imageHandle;
    } response;
} OI_BIP_PUT_IMAGE_PARAMS;


/** Parameter block for the PutLinkedAttachment function OI_BIPCLI_PutLinkedAttachment().
See Bluetooth BIP specification version 1.0 final, section 4.5.4. */
typedef struct {
    struct {
        OI_OBEX_UNICODE *imageHandle;
        OI_OBEX_BYTESEQ *attachmentDescriptor;
        OI_OBEX_BYTESEQ *data;
        OI_BOOL final;
    } request;
} OI_BIP_PUT_LINKED_ATTACHMENT_PARAMS;


/** Parameter block for the PutLinkedThumbnail function OI_BIPCLI_PutLinkedThumbnail().
See Bluetooth BIP specification version 1.0 final, section 4.5.3. */
typedef struct {
    struct {
        OI_OBEX_UNICODE *imageHandle;
        OI_OBEX_BYTESEQ *data;
        OI_BOOL final;
    } request;
} OI_BIP_PUT_LINKED_THUMBNAIL_PARAMS;


/** Parameter block for the GetImagesList function OI_BIPCLI_GetImagesList().
See Bluetooth BIP specification version 1.0 final, section 4.5.6. */
typedef struct {
    struct {
        OI_UINT16 handleCount;
        OI_UINT16 handleOffset;
        OI_UINT8 latest;
        OI_OBEX_BYTESEQ *handlesDescriptor;
    } request;

    struct {
        OI_UINT16 nbReturnedHandles;
        OI_OBEX_BYTESEQ *imageHandlesDescriptor;
        OI_OBEX_BYTESEQ *data;
        OI_BOOL final;
    } response;
} OI_BIP_GET_IMAGES_LIST_PARAMS;


/** Parameter block for the GetImage function OI_BIPCLI_GetImage().
See Bluetooth BIP specification version 1.0 final, section 4.5.8. */
typedef struct {
    struct {
        OI_OBEX_UNICODE *imageHandle;
        OI_OBEX_BYTESEQ *imageDescriptor;
    } request;

    struct {
        OI_UINT32 imageFileSize;
        OI_OBEX_BYTESEQ *data;
        OI_BOOL final;
    } response;
} OI_BIP_GET_IMAGE_PARAMS;


/** Parameter block for the GetLinkedThumbnail function OI_BIPCLI_GetLinkedThumbnail().
See Bluetooth BIP specification version 1.0 final, section 4.5.9. */
typedef struct {
    struct {
        OI_OBEX_UNICODE *imageHandle;
    } request;

    struct {
        OI_OBEX_BYTESEQ *data;
        OI_BOOL final;
    } response;
} OI_BIP_GET_LINKED_THUMBNAIL_PARAMS;


/** Parameter block for the GetLinkedAttachment function OI_BIPCLI_GetLinkedAttachment().
See Bluetooth BIP specification version 1.0 final, section 4.5.10. */
typedef struct {
    struct {
        OI_OBEX_UNICODE *imageHandle;
        OI_OBEX_UNICODE *attachmentName;
    } request;

    struct {
        OI_OBEX_BYTESEQ *data;
        OI_BOOL final;
    } response;
} OI_BIP_GET_LINKED_ATTACHMENT_PARAMS;


/** Parameter block for the StartPrint function OI_BIPCLI_StartPrint().
See Bluetooth BIP specification version 1.0 final, section 4.5.12. */
typedef struct {
    struct {
        OI_UUID128 secondaryServiceId;
        OI_OBEX_BYTESEQ *data;
        OI_BOOL final;
    } request;
} OI_BIP_START_PRINT_PARAMS;


/** Parameter block for the StartArchive function OI_BIPCLI_StartArchive().
See Bluetooth BIP specification version 1.0 final, section 4.5.14. */
typedef struct {
    struct {
        OI_UUID128 secondaryServiceId;
    } request;
} OI_BIP_START_ARCHIVE_PARAMS;


/** Parameter block for the DeleteImage function OI_BIPCLI_DeleteImage().
See Bluetooth BIP specification version 1.0 final, section 4.5.11. */
typedef struct {
    struct {
        OI_OBEX_UNICODE *imageHandle;
    } request;
} OI_BIP_DELETE_IMAGE_PARAMS;


/** Parameter block for the GetPartialImage function OI_BIPCLI_GetPartialImage().
See Bluetooth BIP specification version 1.0 final, section 4.5.13. */
typedef struct {
    struct {
        OI_OBEX_UNICODE *imageName;
        OI_UINT32 partialLength;
        OI_UINT32 partialOffset;
    } request;

    struct {
        OI_UINT32 partialLength;
        OI_UINT32 totalFileSize;
        OI_BOOL endFlag;
        OI_OBEX_BYTESEQ *data;
        OI_BOOL final;
    } response;
} OI_BIP_GET_PARTIAL_IMAGE_PARAMS;


/** Parameter block for the GetImageProperties function OI_BIPCLI_GetImageProperties().
See Bluetooth BIP specification version 1.0 final, section 4.5.7. */
typedef struct {
    struct {
        OI_OBEX_UNICODE *imageHandle;
    } request;

    struct {
        OI_OBEX_BYTESEQ *data;
        OI_BOOL final;
    } response;
} OI_BIP_GET_IMAGE_PROPERTIES_PARAMS;


/** Parameter block for the RemoteDisplay function OI_BIPCLI_RemoteDisplay().
See Bluetooth BIP specification version 1.0 final, section 4.5.5. */
typedef struct {
    struct {
        OI_OBEX_UNICODE *imageHandle;
        OI_UINT8 displayCommand;
    } request;

    struct {
        OI_OBEX_UNICODE *imageHandle;
    } response;
} OI_BIP_REMOTE_DISPLAY_PARAMS;


/** A union of all of the BIP parameter blocks. */
typedef union {
    OI_BIP_GET_CAPABILITIES_PARAMS      getCapabilities;
    OI_BIP_GET_MONITORING_IMAGE_PARAMS  getMonitoringImage;
    OI_BIP_PUT_IMAGE_PARAMS             putImage;
    OI_BIP_PUT_LINKED_ATTACHMENT_PARAMS putLinkedAttachment;
    OI_BIP_PUT_LINKED_THUMBNAIL_PARAMS  putLinkedThumbnail;
    OI_BIP_GET_IMAGES_LIST_PARAMS       getImagesList;
    OI_BIP_GET_IMAGE_PARAMS             getImage;
    OI_BIP_GET_LINKED_THUMBNAIL_PARAMS  getLinkedThumbnail;
    OI_BIP_GET_LINKED_ATTACHMENT_PARAMS getLinkedAttachment;
    OI_BIP_START_PRINT_PARAMS           startPrint;
    OI_BIP_START_ARCHIVE_PARAMS         startArchive;
    OI_BIP_DELETE_IMAGE_PARAMS          deleteImage;
    OI_BIP_GET_PARTIAL_IMAGE_PARAMS     getPartialImage;
    OI_BIP_GET_IMAGE_PROPERTIES_PARAMS  getImageProperties;
    OI_BIP_REMOTE_DISPLAY_PARAMS        remoteDisplay;
} OI_BIP_PARAM_UNION;


/* *************** Primary target UUIDs *******************/

extern const OI_OBEX_BYTESEQ OI_BIP_ImagePushObexTargetUUID;
extern const OI_OBEX_BYTESEQ OI_BIP_ImagePullObexTargetUUID;
extern const OI_OBEX_BYTESEQ OI_BIP_AdvancedImagePrintingObexTargetUUID;
extern const OI_OBEX_BYTESEQ OI_BIP_AutomaticArchiveObexTargetUUID;
extern const OI_OBEX_BYTESEQ OI_BIP_RemoteCameraObexTargetUUID;
extern const OI_OBEX_BYTESEQ OI_BIP_RemoteDisplayObexTargetUUID;

#define OI_BIP_IMAGE_PUSH_OBEX_TARGET_UUID \
    { 0xE3, 0x3D, 0x95, 0x45, 0x83, 0x74, 0x4A, 0xD7, 0x9E, 0xC5, 0xC1, 0x6B, 0xE3, 0x1E, 0xDE, 0x8E }

#define OI_BIP_IMAGE_PULL_OBEX_TARGET_UUID \
    { 0x8E, 0xE9, 0xB3, 0xD0, 0x46, 0x08, 0x11, 0xD5, 0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E }

#define OI_BIP_ADVANCED_IMAGE_PRINTING_OBEX_TARGET_UUID \
    { 0x92, 0x35, 0x33, 0x50, 0x46, 0x08, 0x11, 0xD5, 0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E }

#define OI_BIP_AUTOMATIC_ARCHIVE_OBEX_TARGET_UUID \
    { 0x94, 0x01, 0x26, 0xC0, 0x46, 0x08, 0x11, 0xD5, 0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E }

#define OI_BIP_REMOTE_CAMERA_OBEX_TARGET_UUID \
    { 0x94, 0x7E, 0x74, 0x20, 0x46, 0x08, 0x11, 0xD5, 0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E }

#define OI_BIP_REMOTE_DISPLAY_OBEX_TARGET_UUID \
    { 0x94, 0xC7, 0xCD, 0x20, 0x46, 0x08, 0x11, 0xD5, 0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E }

/* *************** Secondary target UUIDs *******************/
    
extern const OI_OBEX_BYTESEQ OI_BIP_ReferencedObjectsObexTargetUUID;
extern const OI_OBEX_BYTESEQ OI_BIP_ArchivedObjectsObexTargetUUID;
    
#define OI_BIP_REFERENCED_OBJECTS_OBEX_TARGET_UUID \
    { 0x8E, 0x61, 0xF9, 0x5D, 0x1A, 0x79, 0x11, 0xD4, 0x8E, 0xA4, 0x00, 0x80, 0x5F, 0x9B, 0x98, 0x34 }

#define OI_BIP_ARCHIVED_OBJECTS_OBEX_TARGET_UUID \
    { 0x8E, 0x61, 0xF9, 0x5E, 0x1A, 0x79, 0x11, 0xD4, 0x8E, 0xA4, 0x00, 0x80, 0x5F, 0x9B, 0x98, 0x34 }



/**@}*/

#endif /* _OI_BIP_SPEC_H */


