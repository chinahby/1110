#ifndef OEMAPPX5_H
#define OEMAPPX5_H
/*=====================================================================
FILE:  APPX5.h

SERVICES:  APPX5 start position location processing
DESCRIPTION: Example code for handling the MPC portion of the requirements
  for MS that support Location Services.

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   Allocate a APPX5 structure and call Appx5_Ctor.
   Initiate the position request by calling StartPositioningRequest.
   This is an asynchronous TCP/IP request and the caller is notified
   via an AEECallback method.
   If the request must be aborted, use CALLBACKCancel().
   Before deleting the object, call Appx5_Dtor to clean up.

        Copyright © 2000-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/

//=====================================================================
// INCLUDES AND VARIABLE DEFINITIONS
//=====================================================================

//---------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------

#include "AEENet.h"
#include "AEEPosDet.h"

//---------------------------------------------------------------------
// Global Constant Declarations
//---------------------------------------------------------------------

#define AX5_VERSION (0x1)     // current protocol version

// MS Positioning Capabilities Definitions
#define  AX5_POS_NONE                0
#define  AX5_POS_CELL_ID             1
#define  AX5_POS_MS_ASSISTED         2
#define  AX5_POS_MS_ASSISTED_AND_GPS 3
typedef byte Ax5PositioningType;

// Application Type Indicator
#define  AX5_APP_WAP             0
#define  AX5_APP_BREW            1
#define  AX5_APP_JAVA            2
#define  AX5_APP_INTERNAL        3
typedef byte Ax5AppType;

// Consent indicator definitions
#define  AX5_CONSENT_GIVEN       0
#define  AX5_CONSENT_DENIED      1
#define  AX5_CONSENT_TIMED_OUT   2
#define  AX5_CONSENT_MS_REFUSED  3
#define  AX5_CONSENT_NOT_NEEDED  4
typedef byte Ax5ConsentType;

// Extra Appx5-specific errors
#define  AX5_ERR_ABORTED         (AEEGPS_ERR_BASE-3) // the Appx5 was freed
#define  AX5_ERR_PROTOCOL_ERR    (AEEGPS_ERR_BASE-2)
#define  AX5_ERR_REQUEST_TIMEOUT (AEEGPS_ERR_BASE-1)

// Positioning Status Indicator Definitions
#define  AX5_STATUS_USE_GPSONE      0
#define  AX5_STATUS_USE_CACHED_POS  1
#define  AX5_STATUS_USE_CELL_POS    2
#define  AX5_STATUS_DENIED_ACK      3
#define  AX5_STATUS_NOT_AUTHORIZED  4
#define  AX5_STATUS_REQ_REFUSED     5

#define  IMSI_LEN 15

//---------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------

//
// StartPositioningProcessRequest input
//
typedef struct _SPPReq {
   byte                 ucMSPosCapability; // one of AX5_POS_ above
   byte                 ucAppType;         // one of AX5_APP_ above
   byte                 ucConsentInd;      // one of AX5_CONSENT_ above
   byte                 ucPQoS;            // number between 0 and 255
   uint32               uAppId;            // For BREW use AEEClassId
   uint32               uBaseLat;          // only 24 bits are sent
   uint32               uBaseLon;          // only 24 bits are sent
   uint32               uTime;             // GPS Time in Seconds since midnight Jan 5, 1980
   uint16               usSysID;            // SID
   uint16               usNetID;            // NID
   uint16               usBaseID;
   uint16               usURLLength;        // (in bytes) 0 for brew requests
   char                *pszURL;             // pointer to URL string
   char                 szIMSI[IMSI_LEN+1]; // mobile station id, as a 
                                            // null-terminated numerical 
                                            // ascii string
} AX5SPPRequest;

//
// StartPositioningProcessResponse
//
typedef struct _SPPRsp {
   int                  nErr;      // request error, might be any AEEError, plus the AX5_ERR_ values above
   int                  nStatus;   // if nErr == SUCCESS, one of AX5_STATUS above
   AEEGPSIPServer       gpsipPDE;  // Network address of PDE to use
   AEEGPSInfo           gpsiPos;   // Position
} AX5SPPResponse;

//---------------------------------------------------------------------
// Global Data Declarations
//---------------------------------------------------------------------

//=====================================================================
// FUNCTION DECLARATIONS AND INLINE FUNCTION DEFINITIONS
//=====================================================================
//---------------------------------------------------------------------
// APPX5 Interface
//---------------------------------------------------------------------


/* Stuff that can later be made more private */
// Estimate of the maximum possible message size
#define MAX_NET_BUF_SIZE  128

typedef struct _netBuf
{
   unsigned  uBit;    // number of bits used in active byte.  This is
                      // only used for the xxx_bits methods.
   byte     *ptr;     // pointer to last position+1 read/written in buffer

   byte      bBuf[MAX_NET_BUF_SIZE];
} NetBuf;

//Structure to contain module data.
typedef struct _Appx5
{
   IShell      *piShell;

   INAddr       mpcAddr;

   INPort       mpcPort;

   short        nState;     // my state

   ISocket     *piSock;     // ISocket interface ptr
   AEECallback *pcbList;    // list of active callbacks
   AEECallback  cbTimeout;  // callback used internally for timeouts, etc.
   AEECallback  cbWork;     // work callback

   NetBuf       nb;         // used to read, write, bit pack and unpack
   SockIOBlock  aIov[3];    // 3 tx bufs is all I need
   unsigned     uIoCount;   // keep track of how much of the SockIOBlock
                            // array's left, also keep track of bytes needed
                            // to read next
} Appx5;


void Appx5_CtorZ(Appx5 *pAppx5, IShell *piShell, 
                 INAddr mpcAddr, INPort mpcPort);
void Appx5_StartPositioningRequest(Appx5 * me, AEECallback *pcb,
                                   AX5SPPRequest *pReq, 
                                   AX5SPPResponse *pRsp);
void Appx5_Dtor(Appx5 *pAppx5);

/*=====================================================================
   INTERFACE DOCUMENTATION
=======================================================================

Interface Name: APPX5

Description:

Functions in this interface include:
APPx5_Ctor()
Appx5_Dtor()

=======================================================================

Appx5_Ctor()

Description:
  This function allocates and initializes the class-specific data.

Prototype:
    void Appx5_Ctor(Appx5 * pAppx5, IShell *piShell,
                   INAddr mpcAddr, INPort mpcPort);


Parameters:
    pAPPX5  : return pointer to the pointer to the APPX5 Interface object
    piShell : pointer to IShell
    mpcAddr : IP address of the MPC server (in network byte order)
    mpcPort : Socket number of the MPC server (in network byte order)

Return Value:
   None

Comments:
    None

Side Effects:
    None

See Also:
    Appx5_Dtor()

=======================================================================

Function:  Appx5_Dtor()

Description:
      This function releases all resources allocated by the Appx5 object and
      deletes the Appx5 object.

Prototype:
   void Appx5_Dtor(Appx5 * pAppx5)

Parameters:
   Appx5 : pointer to the class structure

Return Value:
   None

Comments:
  None

Side Effects: 
  None

See Also:
  None
=======================================================================
Function: Appx5_StartPositioningRequest()

Description:
   This function initiates a StartPositioning request to the MPC
   identified in the Appx5_Ctor call.  If this function is able to
   initiate the request, the user will be notified of the results via
   the AEECallback parameter.

   The memory 'req' and 'rsp' point to must remain valid for the entire
   duration of the operation (that is, until the completion callback is called, or
   until the operation is canceled.)  The result structure need not be
   initialized before the operation; Appx5 will assign its values.
   The AEECallback must be properly initialized.


Prototype:
void Appx5_StartPositioningRequest(Appx5 *me, AEECallback *pcb,
                  AX5SPPRequest *req, AX5SPPResponse *rsp )

Parameters:
   me : pointer to the AEEApplet structure
   pcb:  user-specified callback
   req: pointer to the information required to initiate the request
        The caller must fully populate this field before calling this method.
   rsp: Pointer to the allocated response structure

Return Value:
   None.  The status is returned in pRsp via the callback.

See Also:
None

=======================================================================*/

#endif    // OEMAPPX5_H
