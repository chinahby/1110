#ifndef AEEIBCMCSDB_H
#define AEEIBCMCSDB_H
/*=============================================================================

FILE:          AEEIBCMCSDB.h

SERVICES:      IBCMCSDB Interface

DESCRIPTION:   Interface definitions and data structures
               for BREW BCMCS DB Interface

===============================================================================
        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEIQI.h"
#include "AEENetAddrTypes.h"
#include "AEEIxOpts.h"

// IBCMCSDB
// --------

#define AEEBCMCSDB_FLAG_NO_OVERWRITE 0x01


// BCMCS DB Record
// ---------------
// BCMCS DB Record is implemented using IxOpts.
// The following types are used for setting the options.

// IP Address
typedef IPAddr AEEBCMCSDBRecord_IpAddr;

// Port
typedef INPort AEEBCMCSDBRecord_Port;

// HDR Zone
typedef uint8 AEEBCMCSDBRecord_HdrZone[16];

// Simple Flow ID
typedef uint32 AEEBCMCSDBRecord_SimpleFlowId;

// Program ID
typedef struct  {
   uint32 val;
   uint8 len;
} AEEBCMCSDBRecord_ProgramId;

// Flow Discriminator
typedef struct  {
   uint8 val;
   uint8 len;
} AEEBCMCSDBRecord_FlowDiscrim;

// Composite Flow ID
typedef struct  {
   AEEBCMCSDBRecord_SimpleFlowId sSimpleFlowId;
   AEEBCMCSDBRecord_ProgramId sProgramId;
   AEEBCMCSDBRecord_FlowDiscrim sFlowDiscrim;
} AEEBCMCSDBRecord_CompositeFlowId;

// Framing
#define AEEBCMCSDB_FRAMING_SEGMENT           0
#define AEEBCMCSDB_FRAMING_HDLC              1

typedef uint32 AEEBCMCSDBRecord_Framing;

// Protocol
#define AEEBCMCSDB_PROTOCOL_PPP              0
#define AEEBCMCSDB_PROTOCOL_IPv4             1
#define AEEBCMCSDB_PROTOCOL_IPv6             2

typedef uint32 AEEBCMCSDBRecord_Protocol;

// CRC Length
#define AEEBCMCSDB_CRC_NONE                  0
#define AEEBCMCSDB_CRC_2BYTES                2

typedef uint8 AEEBCMCSDBRecord_CrcLen;


// BCMCS DB Record options IDs
// ---------------------------

// macro for selecting a unique XOPT_VARIABLE value, based on id.
#define AEEBCMCSDBOPT_VARIABLE_UNIQUE(id)  XOPT_VARIABLE_UNIQUE(id)

// complex types, such as struct and array

#define AEEBCMCSDBRECORDOPT_IpAddr          (AEEBCMCSDBOPT_VARIABLE_UNIQUE(0) \
                                             + sizeof(AEEBCMCSDBRecord_IpAddr))

#define AEEBCMCSDBRECORDOPT_HdrZone         (AEEBCMCSDBOPT_VARIABLE_UNIQUE(1) \
                                             + sizeof(AEEBCMCSDBRecord_HdrZone))

#define AEEBCMCSDBRECORDOPT_CompositeFlowId (AEEBCMCSDBOPT_VARIABLE_UNIQUE(2) \
                                             + sizeof(AEEBCMCSDBRecord_CompositeFlowId))


// simple types, such as integers and enum

#define AEEBCMCSDBRECORDOPT_Port            (XOPT_32BIT + 0)

#define AEEBCMCSDBRECORDOPT_SimpleFlowId    (XOPT_32BIT + 1)

#define AEEBCMCSDBRECORDOPT_Framing         (XOPT_32BIT + 2)

#define AEEBCMCSDBRECORDOPT_Protocol        (XOPT_32BIT + 3)

#define AEEBCMCSDBRECORDOPT_CrcLen          (XOPT_32BIT + 4)

#define AEEBCMCSDBRECORDOPT_END             XOPT_END

//-----------------------------------------------------------------
// Interface
//-----------------------------------------------------------------

// Interface ID
#define AEEIID_IBCMCSDB 0x01036173

//typedef struct IBCMCSDB IBCMCSDB;

#define INHERIT_IBCMCSDB(iname) \
   INHERIT_IQI(iname); \
   int (*Update)(iname* me, IxOpts* pBcmcsDbRecord, uint32 uUpdateMode); \
   \
   /* Extension Function to allow future enhancement to the interface without \
      breaking its binary compatibility. */ \
   int (*ExtFunc)(iname* me, AEECLSID id, void* pBuf, int nBufSize)

// declare the actual interface
AEEINTERFACE_DEFINE(IBCMCSDB);

//-----------------------------------------------------------------
// Methods
//-----------------------------------------------------------------

static __inline uint32 IBCMCSDB_AddRef(IBCMCSDB *me)
{
   return AEEGETPVTBL(me, IBCMCSDB)->AddRef(me);
}

static __inline uint32 IBCMCSDB_Release(IBCMCSDB *me)
{
   return AEEGETPVTBL(me, IBCMCSDB)->Release(me);
}

static __inline int IBCMCSDB_QueryInterface(IBCMCSDB *me, 
                                            AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me, IBCMCSDB)->QueryInterface(me, cls, ppo);
}

static __inline int IBCMCSDB_Update(IBCMCSDB* me, IxOpts* pBcmcsDbRecord, uint32 uUpdateMode)
{
   return AEEGETPVTBL(me, IBCMCSDB)->Update(me, pBcmcsDbRecord, uUpdateMode);
}

/*
===============================================================================
  DATA STRUCTURES DOCUMENTATION
===============================================================================
AEEBCMCSDBRecord_IpAddr

Description:
   This data type specifies a multicast IP address for a BCMCS flow

Definition:
   typedef IPAddr AEEBCMCSDBRecord_IpAddr;

Members:
   None.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   IPAddr

===============================================================================
AEEBCMCSDBRecord_Port

Description:
   This data type specifies application port for a BCMCS flow

Definition:
   typedef INPort AEEBCMCSDBRecord_Port;

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1.5

See Also:
   INPort

===============================================================================
AEEBCMCSDBRecord_HdrZone

Description:
   This data type specifies an HDR zone for a BCMCS flow.
   HDR zone is defined by the HDR Subnet.

Definition:
   typedef uint8 AEEBCMCSDBRecord_HdrZone[16];

Members:
   None

Comments:
   The value of all zeros is reserved for future use.

Version:
   Introduced BREW Client 3.1.5

See Also:
   None

===============================================================================
AEEBCMCSDBRecord_SimpleFlowId

Description:
   This data type specifies a simple flow ID for a BCMCS flow

Definition:
   typedef uint32 AEEBCMCSDBRecord_SimpleFlowId;

Members:
   None

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   None

===============================================================================
AEEBCMCSDBRecord_FlowDiscrim

Description:
   This data type specifies flow discriminator for a BCMCS flow

Definition:
   typedef struct  {
      uint8 val;
      uint8 len;
   } AEEBCMCSDBRecord_FlowDiscrim;

Members:
   val: value of the flow discriminator
   len: length of the flow discriminator in bits

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   None

===============================================================================
AEEBCMCSDBRecord_ProgramId

Description:
   This data type specifies program ID for a BCMCS flow

Definition:
   typedef struct  {
      uint32 val;
      uint8 len;
   } AEEBCMCSDBRecord_ProgramId;

Members:
   val: value of the program ID
   len: length of the program ID in bits

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   None

===============================================================================
AEEBCMCSDBRecord_CompositeFlowId

Description:
   This data type specifies a composite flow ID for a BCMCS flow.

Definition:
   typedef struct  {
      AEEBCMCSDBRecord_SimpleFlowId sSimpleFlowId;
      AEEBCMCSDBRecord_ProgramId sProgramId;
      AEEBCMCSDBRecord_FlowDiscrim sFlowDiscrim;
   } AEEBCMCSDBRecord_CompositeFlowId;

Members:
   sSimpleFlowId: Simple flow ID
   sProgramId: Program ID
   sFlowDiscrim: Flow discriminator

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   None

===============================================================================
AEEBCMCSDBRecord_Framing

Description:
   This data type specifies framing type for a BCMCS flow

Definition:
   #define AEEBCMCSDB_FRAMING_SEGMENT           0
   #define AEEBCMCSDB_FRAMING_HDLC              1

   typedef uint32 AEEBCMCSDBRecord_Framing;

Members:
   AEEBCMCSDB_FRAMING_SEGMENT: flow uses segment based framing
   AEEBCMCSDB_FRAMING_HDLC: flow uses segment HDLC framing

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   None

===============================================================================
AEEBCMCSDBRecord_Protocol

Description:
   This data type specifies the encapsulation protocol for a BCMCS flow

Definition:
   #define AEEBCMCSDB_PROTOCOL_PPP              0
   #define AEEBCMCSDB_PROTOCOL_IPv4             1
   #define AEEBCMCSDB_PROTOCOL_IPv6             2

   typedef uint32 AEEBCMCSDBRecord_Protocol;

Members:
   AEEBCMCSDB_PROTOCOL_PPP: encapsulation is PPP
   AEEBCMCSDB_PROTOCOL_IPv4: encapsulation is IPv4
   AEEBCMCSDB_PROTOCOL_IPv6: encapsulation is IPv6

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   None

===============================================================================
AEEBCMCSDBRecord_CrcLen

Description:
   This data type specifies the CRC length for a BCMCS flow

Definition:
   #define AEEBCMCSDB_CRC_NONE                  0
   #define AEEBCMCSDB_CRC_2BYTES                2

   typedef uint8 AEEBCMCSDBRecord_CrcLen;

Members:
   AEEBCMCSDB_CRC_NONE: No CRC is performed
   AEEBCMCSDB_CRC_2BYTES: CRC length is 2 bytes

Comments:
   None

Version:
   Introduced BREW Client 3.1.5

See Also:
   None

===============================================================================
*/

/*
===============================================================================
MACROS DOCUMENTATION
===============================================================================
BCMCS DB Record options

Description:
   These IxOpts options specify fields in the BCMCS DB Record.

Definition:
   #define AEEBCMCSDBOPT_VARIABLE_UNIQUE(id)   XOPT_VARIABLE_UNIQUE(id)
   
   #define AEEBCMCSDBRECORDOPT_IpAddr          (AEEBCMCSDBOPT_VARIABLE_UNIQUE(0) \
                                                + sizeof(AEEBCMCSDBRecord_IpAddr))

   #define AEEBCMCSDBRECORDOPT_HdrZone         (AEEBCMCSDBOPT_VARIABLE_UNIQUE(1) \
                                                + sizeof(AEEBCMCSDBRecord_HdrZone))

   #define AEEBCMCSDBRECORDOPT_CompositeFlowId (AEEBCMCSDBOPT_VARIABLE_UNIQUE(2) \
                                                + sizeof(AEEBCMCSDBRecord_CompositeFlowId))


   #define AEEBCMCSDBRECORDOPT_Port            (XOPT_32BIT + 0)

   #define AEEBCMCSDBRECORDOPT_SimpleFlowId    (XOPT_32BIT + 1)

   #define AEEBCMCSDBRECORDOPT_Framing         (XOPT_32BIT + 2)

   #define AEEBCMCSDBRECORDOPT_Protocol        (XOPT_32BIT + 3)

   #define AEEBCMCSDBRECORDOPT_CrcLen          (XOPT_32BIT + 4)

   #define AEEBCMCSDBRECORDOPT_END             XOPT_END

Members:
   AEEBCMCSDBRECORDOPT_IpAddr:
   Multicast IP address, a mandatory option. pVal contains a pointer to  
   AEEBCMCSDBRecord_IpAddr.

   AEEBCMCSDBRECORDOPT_HdrZone:
   HDR Zone is defined by the HDR Subnet, and it is a mandatory option.
   pVal contains a pointer to AEEBCMCSDBRecord_HdrZone.
                                
   AEEBCMCSDBRECORDOPT_CompositeFlowId:
   Composite Flow ID. pVal contains a pointer to AEEBCMCSDBRecord_CompositeFlowId.

   AEEBCMCSDBRECORDOPT_Port:
   Port, a mandatory option. pVal contains AEEBCMCSDBRecord_Port.

   AEEBCMCSDBRECORDOPT_SimpleFlowId:
   Simple Flow ID. pVal contains AEEBCMCSDBRecord_SimpleFlowId.

   AEEBCMCSDBRECORDOPT_Framing:
   Framing Type. pVal contains AEEBCMCSDBRecord_Framing.

   AEEBCMCSDBRECORDOPT_Protocol:
   Encapsulation Protocol. pVal contains AEEBCMCSDBRecord_Protocol.

   AEEBCMCSDBRECORDOPT_CrcLen:
   CRC Length. pVal contains AEEBCMCSDBRecord_CrcLen.

   AEEBCMCSDBRECORDOPT_END:
   Terminates an array of options. pVal is not used.


Comments:
   1. Some options are mandatory. Applications must specify these options,
      in order for IBCMCSDB_Update() to succeed.

   2. Applications must not specify an option more than once.

   3. Applications must specify either AEEBCMCSDBRECORDOPT_SimpleFlowId or
      AEEBCMCSDBRECORDOPT_CompositeFlowId, but not both.

Version:
   Introduced BREW Client 3.1.5

See Also:
   xOpt

===============================================================================
*/

/*
===============================================================================
  INTERFACES   DOCUMENTATION
===============================================================================

Interface Name: IBCMCSDB

This interface allows an application to add records to 
the device BCMCS database of flow mapping information, and then 
BCMCS client applications can also register to the new flows.

Upon handset reboot, all BCMCS DB updates are lost, and the BCMCS DB 
is populated by default values.

In order to use the BCMCS DB, an application should:
1. Create an IxOpts object, to hold the BCMCS DB Record.
2. Add all mandatory options to the BCMCS DB Record, 
   and possibly other options as the application wishes.
3. Create an IBCMCSDB object.
4. Update the BCMCS DB with your DB record, by calling IBCMCSDB_Update().

The following header files is required:~
   BREW 4.0 - AEEIBCMCSDB.h
   BREW 3.1 - AEEBCMCSDB.h


===============================================================================

IBCMCSDB_AddRef()

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IBCMCSDB_Release()

===============================================================================

IBCMCSDB_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
   IBCMCSDB_AddRef()

===============================================================================

IBCMCSDB_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None

===============================================================================
IBCMCSDB_Update()

Description:
   This function adds a record to the BCMCS DB of flow mapping information.

Prototype:
   int IBCMCSDB_Update(IBCMCSDB* pif, IxOpts* pBcmcsDbRecord, uint32 uUpdateMode);

Parameters:
   pif: Pointer to IBCMCSDB interface
   pBcmcsDbRecord: The BCMCS DB record to be added
   uUpdateMode: Mode of update. By default, a record will overwrite a
      matching record that already exists in the database. However, if 
      flag AEEBCMCSDB_FLAG_NO_OVERWRITE is specified, trying to add a 
      record that matches an existing record will result in an error.
      Matching is done by comparing IP Address, Port and HDR Zone fields.
      
Return Value:
   AEE_SUCCESS: BCMCS DB was updated successfully.
   AEE_EBADITEM: missing or invalid option in the BCMCS DB Record.
   AEE_NET_EBCMCSDB_FULL: BCMCS DB is full.
   AEE_NET_EBCMCSDB_EXIST: a matching DB record exists, and flag
                           AEEBCMCSDB_FLAG_NO_OVERWRITE was specified.


   Other error codes are also possible.
      
Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   BCMCS DB Record options

===============================================================================
*/

#endif // #ifndef AEEIBCMCSDB_H

