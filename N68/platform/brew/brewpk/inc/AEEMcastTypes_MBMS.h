#ifndef AEEMCASTTYPES_MBMS_H
#define AEEMCASTTYPES_MBMS_H
/*=============================================================================

FILE:         AEEMcastTypes_MBMS.h

SERVICES:     None

DESCRIPTION:  MBMS mcast types and definitions

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEE64structs.h"

// MBMS info codes
#define AEE_MCAST_MBMS_INFO_CODE_SYSTEM_UNAVAILABLE         0x01061992


#define AEE_MCAST_MBMS_SERVICE_TYPE_STREAMING 1  // Streaming Service Type
#define AEE_MCAST_MBMS_SERVICE_TYPE_DOWNLOAD  2  // Download Service Type

typedef uint16 AEEMcast_MBMSServiceType;

#define AEE_MCAST_MBMS_SERVICE_METHOD_BROADCAST 1  // Broadcast Service Method
#define AEE_MCAST_MBMS_SERVICE_METHOD_MULTICAST 2  // Multicast Service Method

typedef uint16 AEEMcast_MBMSServiceMethod;

// MBMS join information
typedef struct AEEMcast_MBMSJoinInfo
{
   uint64struct                 tmgi;
   uint64struct                 session_start_time;
   uint64struct                 session_end_time;
   uint16                       priority;
   AEEMcast_MBMSServiceMethod   service_method;
   AEEMcast_MBMSServiceType     service_type;
   boolean                      selected_service;
   boolean                      service_security;
} AEEMcast_MBMSJoinInfo;

/*=====================================================================
DATA STRUCTURE DOCUMENTATION
======================================================================

AEEMcastInfoCode
 
Definition:

   #define AEE_MCAST_MBMS_INFO_CODE_SYSTEM_UNAVAILABLE         0x01061992

Members: 
   AEE_MCAST_MBMS_INFO_CODE_SYSTEM_UNAVAILABLE:
      In MBMS, failure in lower layers.

======================================================================

AEEMcast_MBMSServiceType

Description:
   This data type is used to describe the MBMS service type.

Definition:

   #define AEE_MCAST_MBMS_SERVICE_TYPE_STREAMING 1
   #define AEE_MCAST_MBMS_SERVICE_TYPE_DOWNLOAD  2

   typedef uint16 AEEMcast_MBMSServiceType;

Members: 
   AEE_MCAST_MBMS_SERVICE_TYPE_STREAMING: 
   Streaming Service Type.
   AEE_MCAST_MBMS_SERVICE_TYPE_DOWNLOAD: 
   Download Service Type.

Comments:
   None.

Version:
   Introduced BREW Client 4.0.2

See Also:
   None

======================================================================

AEEMcast_MBMSServiceMethod

Description:
   This data type is used to describe the MBMS service method.

Definition:

   #define AEE_MCAST_MBMS_SERVICE_METHOD_BROADCAST 1
   #define AEE_MCAST_MBMS_SERVICE_METHOD_MULTICAST 2 

   typedef uint16 AEEMcast_MBMSServiceMethod;

Members: 
   AEE_MCAST_MBMS_SERVICE_METHOD_BROADCAST: 
   Broadcast Service Method.
   AEE_MCAST_MBMS_SERVICE_METHOD_MULTICAST: 
   Multicast Service Method.

Comments:
   None.

Version:
   Introduced BREW Client 4.0.2

See Also:
   None

======================================================================

AEEMcast_MBMSJoinInfo

Description:
   This data type is used to deliver additional MBMS specific join.

Definition:

   typedef struct AEEMcast_MBMSJoinInfo
   {
      uint64struct                 tmgi;
      uint64struct                 session_start_time;
      uint64struct                 session_end_time;
      uint16                       priority;
      AEEMcast_MBMSServiceMethod   service_method;
      AEEMcast_MBMSServiceType     service_type;
      boolean                      selected_service;
      boolean                      service_security;
   } AEEMcast_MBMSJoinInfo;

Members: 
   tmgi: 
      TMGI value.
   session_start_time: 
      Session start time in SNTP format, if not specified use 0
   session_end_time:
      Session end time in SNTP format, if not specified use Max value (0xFFFF FFFF FFFF FFFF)
   priority:
      Session's priority, for example to arbitrate between services as only a 
      limited number can be supported by lower layers.
   service_method:
      Is requested service broadcast or multicast.
   service_type:
      Requested service's type, for example download service or streaming service.
   selected_service:
      Request special service which will have special privileges in RRC signaling.
   service_security:
      Request protected service. Usually all the multicast services are security protected.

Comments:
   None.

Version:
   Introduced BREW Client 4.0.2

See Also:
   None

======================================================================*/
#endif  // AEEMCASTTYPES_MBMS_H
