/*===========================================================================

FILE: OEMAppx5.c

SERVICES: Sample class to demonstrate Appendix 5 Mobile originated position
  location.

DESCRIPTION
  This file contains the implementation of a simple class which implements
  the Appendix 5 Mobile Originated Position Location
  StartPositioningProcessRequest protocol.

PUBLIC CLASSES:
   Appx5

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   Allocate a Appx5 structure and call Appx5_Ctor.
   Initiate the position request by calling StartPositioningRequest.
   This is an asynchronous TCP/IP request and the caller is notified
   via an AEECallback method.
   If the request must be aborted, call CALLBACK_Cancel().
   Before deleting the object, call Appx5_Dtor to clean up.

         Copyright © 2000-2003 QUALCOMM Incorporated.
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/


/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#if defined(AEE_STATIC)
#include "OEMFeatures.h"
#endif  /* #if defined(AEE_STATIC) */


#include "AEE_OEM.h"
#include "AEE_OEMComdef.h"
#include "OEMAppx5.h"
#include "AEEStdLib.h"  // AEE StdLib Services


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/

typedef struct _MSIDRecord {
   // information required for MS_ID_RECORD type IMSI_M as described in
   // CL93-V2246-1 Rev B
   uint16   IMSI_MCC;      // 10 bits used (3 digits)
   uint8    IMSI_11_12;    //  7 bits used (2 digits)
   uint16   IMSI_S2;       // 10 bits used (3 digits)
   uint32   IMSI_S1;       // 24 bits used (7 digits)
                           //    encoded as (Top 3 digits)    << 14
                           //             + (thousands digit) << 10
                           //             + (last 3 digits)
} AX5MSIDRecord;


//-------------------------------------------------------------------
// AEECallback Usage
// pNext - pointer to CALLBACK structure of next event
// pmc   - pointer to reponse data for user
// pCancelData - pointer to Appx5 object
// pReserved - pointer to request data from user
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// StartPositioningProcessRequest message structure
//  Field                       Length (bits)
//  ------                      -------------
//  Version                     8
//  Length                      16
//  MS Positioning Capability   8
//  Application Type            8
//  Consent Indicator           8
//  PQos                        8
//  Application Identifier      32
//  SID                         16
//  NID                         16
//  BASE_ID                     16
//  BASE_LAT                    24
//  BASE_LONG                   24
//  Time Stamp record
//    WEEKS                     16
//    Seconds                   24
//  URL Record (containing the following fields)
//    URL_LEN                   16
//    URL                       8*RUL_LEN
//  MS_ID_RECORD (containing the following fields)
//    Reserved                  1  == 0B
//    MS_ID_TYPE                3  == 010B
//    MS_ID_LEN                 4  == 0111B (length of following structure)
//    MS_ID (containing the following fields)
//      IMSI_CLASS              1  == 0B
//      IMSI_CLASS_0_TYPE       2  == 11B
//      RESERVED                2  == 00B
//      IMSI_MCC                10 Per IS-95B
//      IMSI_11_12              7  Per IS-95B
//      IMSI_S2                 10 Per IS-95B
//      IMSI_S1                 24 Per IS-95B
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// StartPositioningProcessResponse message structure
//  Field                       Length (bits)
//  ------                      -------------
//  Version                     8
//  Length                      16
//  Position Status Indicator   8
//  PDE IP Address              32
//  PDE Port Number             16
//  Position Included flag      1    indicates when the following fields exists:
//   Time Stamp                 40
//   LAT                        25
//   LONG                       26
//   LON_UNCRTNTY_ANG           4
//   LON_UNCRTNTY_A             5
//   LON_UNCRTNTY_P             5
//   VELOCITY_INCL              1    indicates when the following fields exist:
//    VELOCITY_HOR              9
//    HEADING                   10
//    VELOCITY_VER              8
//   HEIGHT_INCL                1    indicates when the following fields exist
//    HEIGHT                    14
//    LOC_UNCRTNTY_V            5
//  PADDING (RESERVED)          0-7
//-------------------------------------------------------------------

/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/

// Helper Functions
static void Appx5_Work(void *p);
static void Appx5_Connected(void *cxt, int err);
static void Appx5_DoWrite(Appx5 *me);
static void Appx5_DoRead(Appx5 *me);
static void Appx5_Timeout(void *cxt);
static void Appx5_Cancel(AEECallback *pcb);
static void Appx5_Return(Appx5 *me, int nErr);
static int  Appx5_DecodeResponse(Appx5 *me);
static int Appx5_ComposeRequest(Appx5 *me);
static void Appx5_NetReset(Appx5 *me);

// routines to stream values in/out of buffer in network order
static void NetBuf_Reset( NetBuf *buf );
static void NetBuf_WriteUint16( NetBuf *buf, uint16 u);
static void NetBuf_WriteUint24( NetBuf *buf, uint32 u);
static void NetBuf_WriteUint32( NetBuf *buf, uint32 u);
static void NetBuf_WriteByte( NetBuf *buf, byte b);
//static void NetBuf_WriteBytes( NetBuf *buf, byte *b, uint16 cnt);
static void NetBuf_WriteBits( NetBuf *buf, uint32 u, uint32 len);

static uint16 NetBuf_ReadUint16( NetBuf *buf);
//static uint32 NetBuf_ReadUint24( NetBuf *buf);
static uint32 NetBuf_ReadUint32( NetBuf *buf);
static byte   NetBuf_ReadByte( NetBuf *buf);
//static void   NetBuf_ReadBytes( NetBuf *buf, byte *b, uint16 cnt);
static uint32 NetBuf_ReadBits( NetBuf *buf, uint32 len);

static int32 SignExtend(uint32 u, uint32 signbit);
static void StringToMSID(char *pszSrc, AX5MSIDRecord *pIMSI);


/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/
#define APPX5_TIMEOUT      (10*1000)   // timeout in milliseconds waiting for
                                       // a response to StartPositioningProcesssRequest
#define SECS_PER_WEEK      (7*24*60*60)   // seconds in a week
/*-------------------------------------------------------------------
            Global Data Definitions
-------------------------------------------------------------------*/

// App specific constants
// keep track of state for current request
#define AX5_STATE_IDLE            0   // socket not opened
#define AX5_STATE_CONNECTING      1   // waiting on connect
#define AX5_STATE_CONNECTED       2   // connected
#define AX5_STATE_WRITING         3   // waiting to complete write
#define AX5_STATE_READING         4   // waiting to complete read

/*-------------------------------------------------------------------
            Other Handy Definitions
-------------------------------------------------------------------*/

#define RELEASEIF(x) if ((x)) { IBASE_Release((IBase*)(x)); x = 0; }

#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#if defined(AEE_SIMULATOR) && defined(_M_IX86)
#define BREAKPOINT() _asm { int 3 };
#else /* #if defined(AEE_SIMULATOR) && defined(_M_IX86) */
#define BREAKPOINT()
#endif /* #if defined(AEE_SIMULATOR) && defined(_M_IX86) */

#define FARF_ALWAYS  1

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p

/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/


/*===============================================================================
                     FUNCTION DEFINITIONS
============================================================================= */



/*===========================================================================

Function:  Appx5_CtorZ()

Description:
      This function initializes the class specific data.

Prototype:
    void Appx5_CtorZ(Appx5 *pAppx5, IShell *piShell,
                    INAddr mpcAddr, INPort mpcPort);

Parameters:
    pAppx5 : Pointer to Appx5 Interface object
    piShell : Pointer to IShell
    mpcAddr : IP address of MPC server (in network byte order)
    mpcPort : Socket number of MPC server (in network byte order)

Return Value:
   None

Comments:  None

Side Effects: None

 See Also:
    Appx5_Dtor()
==============================================================================*/
// Future enhancement - change this method to take a network name
// and use DNS lookup
void Appx5_CtorZ(Appx5 *me, IShell *piShell, INAddr mpcAddr, INPort mpcPort)
{
   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   me->mpcAddr = mpcAddr;
   me->mpcPort = mpcPort;

   CALLBACK_Init(&me->cbTimeout, Appx5_Timeout, me);
   CALLBACK_Init(&me->cbWork, Appx5_Work, me);

   Appx5_NetReset(me);
}

/*===========================================================================

Function:  Appx5_Dtor()

Description:
      This function releases all resources allocated by Appx5 object and
      must be called before deleting it.
      All pending requests are canceled

Prototype:
   void Appx5_Dtor(Appx5 *me)

Parameters:
   me : Pointer to the class structure.

Return Value:
   None

Comments:  None

Side Effects: None

==============================================================================*/
void Appx5_Dtor(Appx5 *me)
{
   while ((AEECallback *)0 != me->pcbList) {
      Appx5_Return(me, AX5_ERR_ABORTED);
   }

   RELEASEIF(me->piSock);
   RELEASEIF(me->piShell);

   ZEROAT(me);
}


/*===========================================================================

FUNCTION Appx5_StartPositioningRequest()

DESCRIPTION
   This function initiates a StartPositioning request to the MPC
   identified in the Appx5_Ctor call.  If this function is able to
   initiate the request, the user will be notified of the results via
   the AEECallback parameter.

   The memory 'req' and 'rsp' point to must remain valid for the entire
   duration of the operation (i.e until the completion callback is called, or
   until the operation is canceled.)  The result structure need not be
   initialized before the operation; Appx5 will assign its values.
   The AEECallback must be properly initialized.

Prototype:
void AX5StartPositioningRequest(Appx5 * me, AEECallback *cb,
                  AX5SPPRequest *req, AX5SPPResponse *pRsp )

PARAMETERS:
   me: Pointer to the AEEApplet structure
   pcb:  User specified callback
   req: Pointer to the information required to initiate the request.
        The caller must fully populate this field before calling this method
   pRsp: Pointer to allocated response structure.

Return Value:
   None.  Status is returned in pRsp via the callback

===========================================================================*/
void Appx5_StartPositioningRequest(Appx5 *me, AEECallback *pcb,
                                   AX5SPPRequest *pReq,
                                   AX5SPPResponse *pRsp)
{
   // cancel any previous user of callback
   CALLBACK_Cancel(pcb);

   pcb->pCancelData = me;
   pcb->pfnCancel = Appx5_Cancel;
   pcb->pmc = pRsp;
   pcb->pCancelData = me;
   pcb->pReserved = pReq;
   pcb->pNext = 0;

   // Add request to end of pending list
   {
      AEECallback **lfp;

      for (lfp = &me->pcbList;(AEECallback *)0 != *lfp;lfp = &(*lfp)->pNext)
         {}
      *lfp = pcb;
   }

   ISHELL_Resume(me->piShell,&me->cbWork);
}

static int Appx5_NetSetup(Appx5 *me)
{
   INetMgr *piNet = 0;
   int      nErr;

#if defined(AEE_STATIC)
   {
      ACONTEXT *pac = AEE_EnterAppContext(0);
#endif /* #if defined(AEE_STATIC) */

   // get INetMgr
   nErr = ISHELL_CreateInstance(me->piShell,AEECLSID_NET,
                                (void**)&piNet);

   if (SUCCESS == nErr) {

      me->piSock = INETMGR_OpenSocket(piNet, AEE_SOCK_STREAM);


      if ((ISocket *)0 == me->piSock) {
         nErr = INETMGR_GetLastError(piNet);
      }
   }

#if defined(AEE_STATIC)
   AEE_LeaveAppContext(pac);
   }
#endif /* #if defined(AEE_STATIC) */

   RELEASEIF(piNet);

   return nErr;
}

static int Appx5_ComposeRequest(Appx5 *me)
{
   AX5SPPRequest *pReq = (AX5SPPRequest *)me->pcbList->pReserved;
   NetBuf *pnb = &me->nb;
   uint16  usMsgLen;
   AX5MSIDRecord MSID;

   // base msg len + url length + imsi length
   usMsgLen = 30 + pReq->usURLLength + 8;

   NetBuf_Reset(pnb);
   NetBuf_WriteByte  (pnb, AX5_VERSION);
   NetBuf_WriteUint16(pnb, usMsgLen );
   NetBuf_WriteByte  (pnb, pReq->ucMSPosCapability);
   NetBuf_WriteByte  (pnb, pReq->ucAppType);
   NetBuf_WriteByte  (pnb, pReq->ucConsentInd);
   NetBuf_WriteByte  (pnb, pReq->ucPQoS);
   NetBuf_WriteUint32(pnb, pReq->uAppId);
   NetBuf_WriteUint16(pnb, pReq->usSysID);
   NetBuf_WriteUint16(pnb, pReq->usNetID);
   NetBuf_WriteUint16(pnb, pReq->usBaseID);
   NetBuf_WriteUint24(pnb, pReq->uBaseLat);
   NetBuf_WriteUint24(pnb, pReq->uBaseLon);
   NetBuf_WriteUint16(pnb, (uint16) (pReq->uTime / SECS_PER_WEEK) );
   NetBuf_WriteUint24(pnb, pReq->uTime % SECS_PER_WEEK );
   NetBuf_WriteUint16(pnb, pReq->usURLLength);

   // ok, that's everything for the 1st buffer
   me->aIov[0].wLen     = pnb->ptr - pnb->bBuf;
   me->aIov[0].pbBuffer = pnb->bBuf;
   me->aIov[1].wLen     = pReq->usURLLength;
   me->aIov[1].pbBuffer = (byte *)pReq->pszURL;
   me->aIov[2].wLen     = (4+4+5+10+7+10+24)/8; // sum of IMSI bit fields / 8
   me->aIov[2].pbBuffer = pnb->ptr;
   me->uIoCount         = 3; // now holds length of iovec array

   // write Mobile station id record...
   StringToMSID(pReq->szIMSI, &MSID);
   NetBuf_WriteBits(pnb, 2, 4);    // Reserved + MS_ID_TYPE
   NetBuf_WriteBits(pnb, 7, 4);    // MS_ID_LEN (does not include MS_ID_TYPE and MS_ID_LEN)
   NetBuf_WriteBits(pnb, 0xC, 5);  // IMSI_CLASS + IMSI_CLASS_0_TYPE + RESERVED
   NetBuf_WriteBits(pnb, MSID.IMSI_MCC, 10);
   NetBuf_WriteBits(pnb, MSID.IMSI_11_12, 7);
   NetBuf_WriteBits(pnb, MSID.IMSI_S2, 10);
   NetBuf_WriteBits(pnb, MSID.IMSI_S1, 24);

   if (usMsgLen != (pnb->ptr - pnb->bBuf) + pReq->usURLLength) {
      FARF(ALWAYS,("marg!"));
      return EFAILED;
      // report error -
      // need to update constants!
   }

   return SUCCESS;
}


/*===========================================================================

FUNCTION Appx5_Work()

DESCRIPTION

   This function transitions through all of the states of a request.
   It assumes me->pCB points to the current request.  If a request is
   not active, it will initiate it.

PARAMETERS:
   me: Pointer to the Appx5 structure.

RETURNS
   AEE_SUCCESS
===========================================================================*/
static void Appx5_Work(void *p)
{
   Appx5       *me = (Appx5 *)p;
   int          nErr;
   AEECallback *pcb = me->pcbList;

   // nothing outstanding, cleanup
   if ((AEECallback *)0 == pcb) {
      Appx5_NetReset(me);
      return;
   }

   switch (me->nState) {
   case AX5_STATE_IDLE:
      nErr = Appx5_NetSetup(me);

      if (SUCCESS != nErr) { // bail everybody
         while ((AEECallback *)0 != me->pcbList) {
            Appx5_Return(me, nErr);
         }
         break;
      }
      me->nState = AX5_STATE_CONNECTING;
      // fallthrough

   case AX5_STATE_CONNECTING:
      // Connect the socket.
      ISOCKET_Connect(me->piSock,
                      me->mpcAddr,
                      me->mpcPort,
                      Appx5_Connected,
                      me);
      break;

   case AX5_STATE_CONNECTED:
      nErr = Appx5_ComposeRequest(me);
      if (SUCCESS != nErr) {
         Appx5_Return(me,nErr);
      }
      me->nState = AX5_STATE_WRITING;
      // fallthrough

   case AX5_STATE_WRITING:
      Appx5_DoWrite(me);
      break;

   case AX5_STATE_READING:
      Appx5_DoRead(me);
      break;

   default: // never get here
      FARF(ALWAYS,("invalid Appx5 state!"));
      BREAKPOINT();
      break;
   }
}

/*===========================================================================

FUNCTION Appx5_Connected()

DESCRIPTION

   This function is the callback that accepts the result code from Connect().

   It takes a 'void *' so it can be passed to Connect() without a cast.
   (Safer to cast a simple structure pointer than to cast a function pointer.)
   When the connect is successful, send the positioning request.

PARAMETERS:
   cxt: Pointer to the Appx5 structure.
   err: connect status

===========================================================================*/
static void Appx5_Connected(void *cxt, int nErr)
{
   Appx5 *me = (Appx5 *)cxt;

   if (SUCCESS != nErr && AEE_NET_EISCONN != nErr) {

      // connect failed - report to all outstanding requests &
      // flush requests
      while ((AEECallback *)0 != me->pcbList) {
         Appx5_Return(me, nErr);
      }

      Appx5_NetReset(me);
      return;
   }

   me->nState = AX5_STATE_CONNECTED;

   ISHELL_Resume(me->piShell,&me->cbWork);
}

static unsigned SockIOBlock_Advance(SockIOBlock *piov, unsigned cntBlocks,
                                    int cbAdvance)
{
   unsigned ndx;

   for (ndx = 0; ndx < cntBlocks; ++ndx) {
      if (cbAdvance < piov[ndx].wLen) {
         /* Skip initial portion of this block and stop */
         piov[ndx].pbBuffer += cbAdvance;
         piov[ndx].wLen -= cbAdvance;
         break;
      } else {
         /* Skip block entirely */
         cbAdvance -= piov[ndx].wLen;
      }
   }

   /*  move remaining blocks to start of piov[] */

   MEMMOVE(piov, piov + ndx, (cntBlocks - ndx) * sizeof(SockIOBlock));

   return cntBlocks - ndx;
}

/*===========================================================================

FUNCTION Appx5_DoWrite()

DESCRIPTION

   This is the function that writes to the socket. It will be called after a
   successful connect, and if the socket is not ready, it will schedule
   itself to be called again.

PARAMETERS:
   me: Pointer to the Appx5 structure.

===========================================================================*/
static void Appx5_DoWrite(Appx5 *me)
{
   ISocket *piSock = me->piSock;
   int nRv;             // Return value from Write()

   // Issue a write command on the socket
   nRv = ISOCKET_WriteV(piSock, me->aIov, (uint16) me->uIoCount);

   // Write() return values fall into four categories of interest:
   //
   //   1. AEE_NET_WOULDBLOCK: This means, data cannot be queued on the socket yet
   //   2. AEE_NET_ERROR: An eror occurred
   //   3. 0 (zero):  No more data (the peer has closed the connection).
   //   3. >0 : The actual number of bytes written
   //

   if (0 < nRv) {

      me->uIoCount = SockIOBlock_Advance(me->aIov,me->uIoCount,nRv);

   } else if (AEE_NET_ERROR == nRv) {

      nRv = ISOCKET_GetLastError(piSock);

      Appx5_Return(me, nRv); /* report issue to client */

      Appx5_NetReset(me);  /* net cleanup */

      ISHELL_Resume(me->piShell,&me->cbWork); /* do next guy */

      return;
   }

   // nRv == AEE_NET_WOULDBLOCK  or nRv == 0

   if (0 == me->uIoCount) { // done writing

      ISHELL_SetTimerEx(me->piShell, APPX5_TIMEOUT, &me->cbTimeout);

      /* reset for read */
      NetBuf_Reset(&me->nb);

      me->nState = AX5_STATE_READING;

      me->uIoCount = 3; // read at least version and size for message

      ISOCKET_Readable(me->piSock, Appx5_Work, me);

   } else {

      ISOCKET_Writeable(me->piSock, Appx5_Work, me);

   }

}


/*===========================================================================

FUNCTION Appx5_DoRead()

DESCRIPTION

   This is the function that reads from the socket. It is called after a
   successful write, and if it can't read the entire message, it may schedule
   itself to be called again.

PARAMETERS:
   me: Pointer to the Appx5 structure.

===========================================================================*/
static void Appx5_DoRead(Appx5 *me)
{
   ISocket *piSock = me->piSock;
   int      nRv;              // Return value from Read()
   NetBuf   *pnb;

   // Issue a read command on the socket.
   pnb = &me->nb;

   nRv = ISOCKET_Read(piSock, pnb->ptr, me->uIoCount);

   // Read() return values fall into four categories of interest:
   //
   //   1. AEE_NET_WOULDBLOCK: This means, data is not yet available on the socket.
   //   2. AEE_NET_ERROR: An eror occurred
   //   3. 0 (zero):  No more data (the peer has closed the connection).
   //   3. >0 : The actual number of bytes read
   //

   if (AEE_NET_WOULDBLOCK == nRv) {
      // WOULDBLOCK => there is no more data available at the moment
      //
      // In this case, re-register the callback function using ISOCKET_Readable()
      // and return from this function.  The callback will be called by the system
      // at a later time.

      ISOCKET_Readable(piSock, Appx5_Work, me);
      return;
   }

   if (0 < nRv) { // data has been read
      int nRead;

      CALLBACK_Cancel(&me->cbTimeout);

      // Some data has been read from the socket. Verify that it's all
      // arrived and translate it if it has
      pnb->ptr += nRv;
      nRead = pnb->ptr - pnb->bBuf;

      nRv = SUCCESS;

      if (nRead >= 3) {
         // assume version 1 format
         // determine how many more bytes to read, based on message length
         me->uIoCount = ((pnb->bBuf[1]<<8) + pnb->bBuf[2]) - nRead;

         // assure rest of response will fit
         if (me->uIoCount + nRead > sizeof(pnb->bBuf)) {
            nRv = AX5_ERR_PROTOCOL_ERR;
         }
      }
      else {
         // finish reading header first
         me->uIoCount = 3 - nRead;
      }

      if (SUCCESS == nRv) {
         // more to come...
         if (me->uIoCount > 0) {
            ISOCKET_Readable(piSock, Appx5_Work, me);
            return;
         }

         nRv = Appx5_DecodeResponse(me);
      }

   } else { // nRv == 0 or error
      nRv = ISOCKET_GetLastError(piSock);

      if (SUCCESS == nRv) { // stream closed == protocol error
         nRv = AX5_ERR_PROTOCOL_ERR;
      }
   }

   Appx5_Return(me, nRv);

   Appx5_NetReset(me); /* net cleanup */

   ISHELL_Resume(me->piShell,&me->cbWork); /* do next guy */

}

/*===========================================================================

FUNCTION Appx5_DecodeResponse()

DESCRIPTION
   Decode received message.

INPUTS
   me: Pointer to the Appx5 structure

===========================================================================*/
static int Appx5_DecodeResponse(Appx5 *me)
{
   byte   version;
   uint32 bPosValid;
   NetBuf *pnb = &me->nb;
   AX5SPPResponse *pResp = (AX5SPPResponse *)me->pcbList->pmc;

   // @@@ TODO - check that all data is available for decode..

   // read from beginning of netbuf
   NetBuf_Reset(pnb);

   version = NetBuf_ReadByte(pnb);

   if (AX5_VERSION != version) {
      // unknown protocol
      return AX5_ERR_PROTOCOL_ERR;
   }

   // length is next, but we don't use it here
   (void)NetBuf_ReadUint16(pnb);

   pResp->nStatus       = NetBuf_ReadByte(pnb);
   pResp->gpsipPDE.addr = NetBuf_ReadUint32(pnb);
   pResp->gpsipPDE.port = NetBuf_ReadUint16(pnb);
   bPosValid            = NetBuf_ReadBits(pnb, 1);
   pResp->gpsiPos.fValid = 0;

   if (bPosValid) {
      pResp->gpsiPos.fValid = (AEEGPS_VALID_LAT  |
                               AEEGPS_VALID_LON  |
                               AEEGPS_VALID_HUNC |
                               AEEGPS_VALID_AUNC |
                               AEEGPS_VALID_PUNC);

      pResp->gpsiPos.dwTimeStamp = NetBuf_ReadBits(pnb,16)*SECS_PER_WEEK;
      pResp->gpsiPos.dwTimeStamp += NetBuf_ReadBits(pnb,24);
      pResp->gpsiPos.status = AEEGPS_ERR_NO_ERR;
      pResp->gpsiPos.dwLat  = SignExtend(NetBuf_ReadBits(pnb, 25), 1 << 24);
      pResp->gpsiPos.dwLon  = SignExtend(NetBuf_ReadBits(pnb, 26), 1 << 25);
      pResp->gpsiPos.bHorUncAngle = (byte) NetBuf_ReadBits(pnb, 4);
      pResp->gpsiPos.bHorUnc      = (byte) NetBuf_ReadBits(pnb, 5);
      pResp->gpsiPos.bHorUncPerp  = (byte) NetBuf_ReadBits(pnb, 5);
      // Check if velocity included
      if (NetBuf_ReadBits(pnb, 1)) {
         pResp->gpsiPos.wVelocityHor = (uint16) NetBuf_ReadBits(pnb, 9);
         pResp->gpsiPos.wHeading     = (uint16) NetBuf_ReadBits(pnb, 10);
         pResp->gpsiPos.bVelocityVer = (int8)(NetBuf_ReadBits(pnb, 8));
         pResp->gpsiPos.fValid      |= (AEEGPS_VALID_HVEL |
                                        AEEGPS_VALID_VVEL |
                                        AEEGPS_VALID_HEAD);

      }
      // check if height included
      if (NetBuf_ReadBits(pnb, 1)) {
         pResp->gpsiPos.wAltitude =
            (int16) SignExtend(NetBuf_ReadBits(pnb, 14), 1 << 13);
         pResp->gpsiPos.wVerUnc   = (uint16) NetBuf_ReadBits(pnb, 5);
         pResp->gpsiPos.fValid   |= AEEGPS_VALID_ALT;
      }
   }

   return SUCCESS;
}

/*===========================================================================

FUNCTION Appx5_Return()

DESCRIPTION
   Return error status to client via callback mechanism.

INPUTS
   me: Pointer to the Appx5 structure
   nErr: Status to return to client

===========================================================================*/
static void Appx5_Return(Appx5 *me, int nErr)
{
   AEECallback *pcb = me->pcbList;

   if ((AEECallback *)0 != pcb) {
      // Clean up and return status via callback.
      ((AX5SPPResponse *)pcb->pmc)->nErr = nErr;
      ISHELL_Resume(me->piShell, pcb);
   }
}

/*===========================================================================

FUNCTION Appx5_NetReset()

DESCRIPTION
   Reset Appx5 including cleaning up socket for next request.

INPUTS
   me: Pointer to the Appx5 structure

===========================================================================*/
static void Appx5_NetReset(Appx5 *me)
{
   // cancel any outstanding timer
   CALLBACK_Cancel(&me->cbTimeout);
   NetBuf_Reset(&me->nb);

   // release the socket
   RELEASEIF(me->piSock);

   me->nState = AX5_STATE_IDLE;
}

/*===========================================================================

FUNCTION Appx5_Timeout()

DESCRIPTION
   Handle timeout if server doesn't respond within the timeout period.

===========================================================================*/
static void Appx5_Timeout(void *cxt)
{
   Appx5 *me = cxt;

   Appx5_NetReset(me);

   Appx5_Return(me, AX5_ERR_REQUEST_TIMEOUT);

   ISHELL_Resume(me->piShell,&me->cbWork); /* do next guy */
}


/*===========================================================================

FUNCTION Appx5_CBCancel()

DESCRIPTION
   PFNCBCALLBACK method for Appx5.  This method is invoked via
   CALLBACK_Cancel().

===========================================================================*/
static void Appx5_Cancel(AEECallback *pcb)
{
   Appx5       *me = (Appx5 *)pcb->pCancelData;
   AEECallback **lfp;

   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pcb->pfnCancel != Appx5_Cancel)
      return;
   
   // Note: this is the only place where an AEECALLBACK is removed
   // from the Appx5 list!

   // traverse list and delete if found
   for (lfp = &me->pcbList; pcb != *lfp; lfp = &(*lfp)->pNext);

   *lfp = pcb->pNext;

   // is it the current operation being aborted?
   if (pcb->pNext == me->pcbList) {
      // if the request was in progress, whack it
      if (me->nState != AX5_STATE_CONNECTING) {
         Appx5_NetReset(me);
      }
   }

   pcb->pNext = 0;
   // indicate callback no longer active
   pcb->pfnCancel = 0;

   ISHELL_Resume(me->piShell,&me->cbWork); /* do next guy */
}


// read/write functions to buffer.

static void NetBuf_Reset( NetBuf *buf )
{
   buf->uBit = 0;
   buf->ptr = buf->bBuf;
}


static void NetBuf_WriteUint16( NetBuf *buf, uint16 u)
{
   uint16 n = HTONS(u);
   byte *v = (byte *) &n;
   *buf->ptr++ = v[0];
   *buf->ptr++ = v[1];
}

static void NetBuf_WriteUint24( NetBuf *buf, uint32 u)
{
   uint32 n = HTONL(u);
   byte *v = (byte *) &n;
   *buf->ptr++ = v[1];
   *buf->ptr++ = v[2];
   *buf->ptr++ = v[3];
}

static void NetBuf_WriteUint32( NetBuf *buf, uint32 u)
{
   uint32 n = HTONL(u);
   byte *v = (byte *) &n;
   *buf->ptr++ = v[0];
   *buf->ptr++ = v[1];
   *buf->ptr++ = v[2];
   *buf->ptr++ = v[3];
}


static void NetBuf_WriteByte( NetBuf *buf, byte b)
{
   *buf->ptr++ = b;
}

//static void NetBuf_WriteBytes( NetBuf *buf, byte *b, uint16 cnt)
//{
//   uint16 i;
//   for (i=0;i<cnt; i++) {
//      *buf->ptr++ = b[i];
//   }
//}


// create a mask containing x bits starting at offset start
// (bit 0 = MSB)
//  for example, B_MASK(1, 4) = 01111000B
// shifts must be of unsigned values (don't want replicate sign bit)
#define B_MASK(start, bits)   ((byte)~(0xff >> (bits)) >> (start))

// write a bit aligned field up to 32 bits long into the network buffer
static void NetBuf_WriteBits( NetBuf *buf, uint32 u, uint32 len)
{
   uint32 first_bits = 8 - buf->uBit; // Maximum number of bits to fill in 1st byte
   uint32 first;                 // bits stuffed into 1st byte of dest
   byte mask;

   if( len < first_bits) {
      first_bits = len;
   }

   mask = B_MASK(buf->uBit, first_bits);

   // first is the source bits which go into the first byte
   first = (u >> (len-first_bits)) << (8-(first_bits+buf->uBit));
   *buf->ptr = (*buf->ptr & ~mask) | (byte)(first & mask);

   len -= first_bits;
   if (len == 0) {  // done - take quick exit
      buf->uBit = (byte) (buf->uBit + first_bits) % 8;
      if (buf->uBit == 0) {
         buf->ptr++;
      }
      return;
   }
   buf->ptr++;
   // now take care of the middle bits
   while (len >= 8) {
      len -= 8;
      *buf->ptr++ = (byte) (u >> len);
   }

   // put remaining bits in final byte.
   if (len > 0) {
      mask = B_MASK(0,len);
      *buf->ptr = (*buf->ptr & ~mask) | (byte)((u << (8-len)) & mask);
   }
   buf->uBit = (byte) len;
}

static uint16 NetBuf_ReadUint16( NetBuf *buf)
{
   uint16 u = 0;
   byte *v = (byte *) &u;
   v[0] = *buf->ptr++;
   v[1] = *buf->ptr++;
   return NTOHS(u);
}

//static uint32 NetBuf_ReadUint24( NetBuf *buf)
//{
//   uint32 u;
//   byte *v = (byte *) &u;
//   v[0] = 0;            // zero fill (hopefully it doesn't need to be sign extended)
//   v[1] = *buf->ptr++;
//   v[2] = *buf->ptr++;
//   v[3] = *buf->ptr++;
//   return NTOHL(u);
//}

static uint32 NetBuf_ReadUint32( NetBuf *buf)
{
   uint32 u = 0;
   byte *v = (byte *) &u;
   v[0] = *buf->ptr++;
   v[1] = *buf->ptr++;
   v[2] = *buf->ptr++;
   v[3] = *buf->ptr++;
   return NTOHL(u);
}


static byte NetBuf_ReadByte( NetBuf *buf)
{
   return *buf->ptr++;
}

//static void NetBuf_ReadBytes( NetBuf *buf, byte *b, uint16 cnt)
//{
//   uint16 i;
//   for (i=0;i<cnt; i++) {
//      b[i] = *buf->ptr++;
//   }
//}


// read a bit aligned field up to 32 bits long from the network buffer
static uint32 NetBuf_ReadBits( NetBuf *buf, uint32 len)
{
   uint32 first_bits = 8 - buf->uBit; // Maximum number of bits to take from 1st byte
   uint32 result;                 // local holder for results
   byte mask;

   if( len < first_bits) {
      first_bits = len;
   }
   mask = B_MASK(8-first_bits, first_bits);

   result = (*buf->ptr >> (8-(first_bits+buf->uBit))) & mask;

   len -= first_bits;

   if (len == 0) {  // done - take quick exit
      buf->uBit = (buf->uBit + (byte) first_bits) % 8;
      if (buf->uBit == 0)
         buf->ptr++;
      return result;
   }
   buf->ptr++;
   // now take care of the middle bits
   while (len >= 8) {
      len -= 8;
      result = (result << 8) | *buf->ptr++;
   }

   // put remaining bits in final byte.
   if (len > 0) {
      mask = B_MASK(0,len);
      result = (result << len) |  ((*buf->ptr & mask) >> (8-len));
   }
   buf->uBit = (byte)len;
   return result;
}

static int32 SignExtend(uint32 u, uint32 signbit)
{
   // extend sign bit if negative
   if (u & signbit)
      return ~(signbit - 1) | u;
   else
      return u;
}

/*===========================================================================

FUNCTION StringToMCC()

DESCRIPTION
   Convert numeric string to MCC digits.
   No range checking is done on inputs.

SIDE EFFECTS
   Original string is modified.


===========================================================================*/
// See conversion algorithm in IS-95
static const word s2min[10] = {
   9, 0, 1, 2, 3, 4, 5, 6, 7, 8
};

static word StringToMCC(char *pStr)
{
   word mcc;
   mcc = s2min[pStr[0] - '0']*100 +
         s2min[pStr[1] - '0']*10  +
         s2min[pStr[2] - '0'];
   return mcc;
}

static word StringToMNC(char *pStr)
{
   word mcc;
   mcc = s2min[pStr[0] - '0']*10 +
         s2min[pStr[1] - '0'];
   return mcc;
}

static void StringToMSID(char *pszSrc, AX5MSIDRecord *pIMSI)
{
   char pszMSID[IMSI_LEN];
   int iStrLen;

   MEMSET(pszMSID,'0',IMSI_LEN);
   iStrLen = STRLEN(pszSrc);

   if (iStrLen > IMSI_LEN)
      iStrLen = IMSI_LEN;

   // Copy IMSI, zero filled, right justified
   STRNCPY(pszMSID+(IMSI_LEN-iStrLen), pszSrc,iStrLen);
   pIMSI->IMSI_MCC = StringToMCC(pszMSID);
   pIMSI->IMSI_11_12 = (byte) StringToMNC(&pszMSID[3]);
   pIMSI->IMSI_S2 =  StringToMCC(&pszMSID[5]);
   pIMSI->IMSI_S1 = (StringToMCC(&pszMSID[8]) << 14) +
                    ((pszMSID[11] - '0' + ((pszMSID[11]=='0')?10:0)) << 10) +
                    StringToMCC(&pszMSID[12]);
}

/*========================End of File ================================================*/

