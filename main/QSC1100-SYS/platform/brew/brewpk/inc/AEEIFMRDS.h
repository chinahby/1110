#ifndef AEEIFMRDS_H
#define AEEIFMRDS_H
/*==============================================================================

FILE:
   AEEIFMRDS.h

SERVICES:
   FM Radio RDS/RBDS related Services

GENERAL DESCRIPTION:
  Base level definitions, typedefs, etc. for AEEIFMRDS

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IFMRDS

INITIALIZATION AND SEQUENCING REQUIREMENTS:

   Copyright © 2006-2008 QUALCOMM Incorporated.
                  All Rights Reserved.
               QUALCOMM Proprietary/GTDR

==============================================================================*/
#include "AEEIQI.h"
#include "AEECallback.h"

#define AEEIID_IFMRDS          0x010486E2

#define AEEFMRDS_RAW_GROUPED_DATA     1
#define AEEFMRDS_PARSED_DATA          2
#define AEEFMRDS_RAW_DATA             3

typedef uint32 AEEFMRDSDataStream;


/* groups in RDS/RBDS */
#define AEEFMRDS_INFO_0A         1
#define AEEFMRDS_INFO_0B         2
#define AEEFMRDS_INFO_1A         3
#define AEEFMRDS_INFO_1B         4
#define AEEFMRDS_INFO_2A         5
#define AEEFMRDS_INFO_2B         6
#define AEEFMRDS_INFO_3A         7
#define AEEFMRDS_INFO_3B         8
#define AEEFMRDS_INFO_4A         9
#define AEEFMRDS_INFO_4B         10
#define AEEFMRDS_INFO_5A         11
#define AEEFMRDS_INFO_5B         12
#define AEEFMRDS_INFO_6A         13
#define AEEFMRDS_INFO_6B         14
#define AEEFMRDS_INFO_7A         15
#define AEEFMRDS_INFO_7B         16
#define AEEFMRDS_INFO_8A         17
#define AEEFMRDS_INFO_8B         18
#define AEEFMRDS_INFO_9A         19
#define AEEFMRDS_INFO_9B         20
#define AEEFMRDS_INFO_10A        21
#define AEEFMRDS_INFO_10B        22
#define AEEFMRDS_INFO_11A        23
#define AEEFMRDS_INFO_11B        24
#define AEEFMRDS_INFO_12A        25
#define AEEFMRDS_INFO_12B        26
#define AEEFMRDS_INFO_13A        27
#define AEEFMRDS_INFO_13B        28
#define AEEFMRDS_INFO_14A        29
#define AEEFMRDS_INFO_14B        30
#define AEEFMRDS_INFO_15A        31
#define AEEFMRDS_INFO_15B        32

//define a type for information fields.
typedef uint32 AEEFMRDSInfo;


/* Program Item Number Codes, group 1A/1B */
typedef struct {

   uint16 nDayItemCode;
   uint16 nHourItemCode;
   uint16 nMinuteItemCode;

}AEEPINCode;

/* Clock-Date Time, code group 4A */
typedef struct {

   uint32   dwMJDCode;
   uint16   nHourCode;
   uint16   nMinuteCode;
   int16    nLocalOffsetTimeCode;

}AEEClockTDCode;

/* Alternate Frequency codes */
typedef struct {

   int   nBufLen;
   int   nBufLenReqd;
   int*  pAFCodePairs;

}AEEAFCodePairs;

/* Application Id code, group 3A */
typedef struct {

   int      nAGTypeCode;
   uint16   nMessage;
   uint16   nAID;

}AEEAICode;

/* data as exposed to users when asked for raw data */
typedef struct {

   uint16   nBlockA;
   uint16   nBlockB;
   uint16   nBlockC;
   uint16   nBlockD;

}AEEFMRDSGroup;

typedef struct {

   uint16            nPICode;
   uint8             nPTYCode;
   boolean           bTPCode;
   char              aPSName[8];
   char              aPTYName[8];
   boolean           bTACode;
   boolean           bABTextFalg;
   uint8             nDICode;
   boolean           bMSCode;
   char              aRTMessage[64];
   uint8             nECCode;
   AEEPINCode        sPinCode;
   AEEAFCodePairs    sAFCodePairs;
   AEEClockTDCode    sClkTDCode;
   AEEAICode         sAICode;

}AEEFMRDSParsedInfo;

#define INHERIT_IFMRDS(iname)                               \
   INHERIT_IQI(iname);                                      \
   int (*GetInfoData)(iname         *pif,                   \
                      AEEFMRDSInfo  nId,                    \
                      int           *pnSequence,            \
                      void          *pBuf,                  \
                      uint32        nBufSize,               \
                      uint32        *pnBufSizeReq);         \
   int (*GetNextData)(iname           *pif,                 \
                      int             *pnSequence,          \
                      AEEFMRDSGroup   *pFMRDSGroupsBuf,     \
                      int             nFMRDSGroupsBufSize,  \
                      int             *pnFMRDSBufSizeReq);  \
   int (*GetParsedData)(iname                *pif,          \
                        AEEFMRDSInfo         nId,           \
                        AEEFMRDSParsedInfo   *pRDSInfo);    \
   void (*InfoReadable)(iname                *pif,          \
                        AEECallback          *pcb,          \
                        AEEFMRDSInfo         nId,           \
                        AEEFMRDSDataStream   nType)

AEEINTERFACE_DEFINE(IFMRDS);

static __inline uint32 IFMRDS_AddRef(IFMRDS *pif)
{
   return AEEGETPVTBL(pif,IFMRDS)->AddRef(pif);
}

static __inline uint32 IFMRDS_Release(IFMRDS *pif)
{
   return AEEGETPVTBL(pif, IFMRDS)->Release(pif);
}

static __inline int IFMRDS_QueryInterface(IFMRDS    *pif,
                                          AEEIID    id,
                                          void      **ppo)
{
   return AEEGETPVTBL(pif, IFMRDS)->QueryInterface(pif, id, ppo);
}

static __inline int IFMRDS_GetInfoData(IFMRDS         *pif,
                                       AEEFMRDSInfo   nId,
                                       int            *pnSequence,
                                       void           *pBuf,
                                       uint32         nBufSize,
                                       uint32         *pnBufSizeReq)
{
   return AEEGETPVTBL(pif,IFMRDS)->GetInfoData(pif,
                                               nId,
                                               pnSequence,
                                               pBuf,
                                               nBufSize,
                                               pnBufSizeReq);
}

static __inline int IFMRDS_GetNextData(IFMRDS          *pif,
                                       int             *pnSequence,
                                       AEEFMRDSGroup   *pFMRDSGroupsBuf,
                                       int             nFMRDSGroupsBufSize,
                                       int             *pnFMRDSBufSizeReq)

{
   return AEEGETPVTBL(pif,IFMRDS)->GetNextData(pif,
                                               pnSequence,
                                               pFMRDSGroupsBuf,
                                               nFMRDSGroupsBufSize,
                                               pnFMRDSBufSizeReq);
}

static __inline int IFMRDS_GetParsedData(IFMRDS              *pif,
                                         AEEFMRDSInfo        nId,
                                         AEEFMRDSParsedInfo  *pRDSInfo)
{
   return AEEGETPVTBL(pif,IFMRDS)->GetParsedData(pif,nId,pRDSInfo);
}


static __inline void IFMRDS_InfoReadable(IFMRDS             *pif,
                                         AEECallback        *pcb,
                                         AEEFMRDSInfo       nId,
                                         AEEFMRDSDataStream nType)
{
   AEEGETPVTBL(pif,IFMRDS)->InfoReadable(pif,pcb,nId,nType);
}


/*==========================================================================
   DATA STRUCTURE DOCUMENTATION
============================================================================

AEEFMRDSDataStream

Description:
   Indicates what type of different data streams that the user can query for

Definition:
   #define AEEFMRDS_RAW_GROUPED_DATA     1
   #define AEEFMRDS_PARSED_DATA          2
   #define AEEFMRDS_RAW_DATA             3

   typedef uint32 AEEFMRDSDataStream;

Members:

   AEEFMRDS_RAW_GROUPED_DATA  :  If the user registers for this type, the
                                 Callback would be fired only when we have
                                 raw RDS data for that particular FMRDSInfo
                                 Id

   AEEFMRDS_PARSED_DATA       :  If the user registers with this id the user
                                 would be notified only when we have parsed
                                 data for that FMRDS info id.

   AEEFMRDS_RAW_DATA          :  If we register using this, user would be
                                 called to give the raw data stream received
                                 from the lower layers.

Comments:
   None

See Also:
   None

============================================================================

AEEFMRDSInfo

Description:
   Gives the different types of RDS data that are supported.

Definition:
   #define AEEFMRDS_INFO_0A         1
   #define AEEFMRDS_INFO_0B         2
   #define AEEFMRDS_INFO_1A         3
   #define AEEFMRDS_INFO_1B         4
   #define AEEFMRDS_INFO_2A         5
   #define AEEFMRDS_INFO_2B         6
   #define AEEFMRDS_INFO_3A         7
   #define AEEFMRDS_INFO_3B         8
   #define AEEFMRDS_INFO_4A         9
   #define AEEFMRDS_INFO_4B         10
   #define AEEFMRDS_INFO_5A         11
   #define AEEFMRDS_INFO_5B         12
   #define AEEFMRDS_INFO_6A         13
   #define AEEFMRDS_INFO_6B         14
   #define AEEFMRDS_INFO_7A         15
   #define AEEFMRDS_INFO_7B         16
   #define AEEFMRDS_INFO_8A         17
   #define AEEFMRDS_INFO_8B         18
   #define AEEFMRDS_INFO_9A         19
   #define AEEFMRDS_INFO_9B         20
   #define AEEFMRDS_INFO_10A        21
   #define AEEFMRDS_INFO_10B        22
   #define AEEFMRDS_INFO_11A        23
   #define AEEFMRDS_INFO_11B        24
   #define AEEFMRDS_INFO_12A        25
   #define AEEFMRDS_INFO_12B        26
   #define AEEFMRDS_INFO_13A        27
   #define AEEFMRDS_INFO_13B        28
   #define AEEFMRDS_INFO_14A        29
   #define AEEFMRDS_INFO_14B        30
   #define AEEFMRDS_INFO_15A        31
   #define AEEFMRDS_INFO_15B        32

   typedef uint32 AEEFMRDSInfo;


Members:

  The id's from AEEFMRDS_INFO_0A to AEEFMRDS_INFO_15B correspond to the
  various groups 0A...15B respectively of RDS/RBDS standard.

Comments:
   None

See Also:
   None

============================================================================
AEEPINCode

Description:
   Contains Program item number code. It is the scheduled broadcast start
   time and day of month as published by the broadcaster.

Definition:
   typedef struct {

      uint16 nDayItemCode;
      uint16 nHourItemCode;
      uint16 nMinuteItemCode;

   }AEEPINCode;

Members:
   nDayItemCode    : specifies day of month.
   nHourItemCode   : Specifies hours part in start time.
   nMinuteItemCode : Specifies Minutes part in start time.

Comments:
   None

See Also:
   None

============================================================================

AEEClockTDCode

Description:
   Contains Clock-time and date. The transmitted clock-time and data will be
   accurately set to UTC plus local offset time. ClockTDCode structure has
   4 items. Modified Julian Day code, Hour time code, minute time code,
   Local offset time code. Hour time code and minute time code is nothing
   but UTC.

Definition:
   typedef struct {
      
      uint32         dwMJDCode;
      uint16         nHourCode;
      uint16         nMinuteCode;
      int16          nLocalOffsetTimeCode;

   }AEEClockTDCode;


Members:
   dwMJDCode            : Modified Julian Day code. Represented by 17bits.
   nHourCode            : Hour time code. Represented by 5 bits.
   nMinuteCode          : Minute time code. Represented by 6 bits.
   nLocalOffsetTimeCode : Local offset time. Represented by 6 bits. Will be
                          added to UTC to get Local time.
Comments:
   None

See Also:
   None

============================================================================

AEEAFCodePairs

Description:
   Contains Alternate Frequency Code pairs.

Definition:
   typedef struct {

      int   nBufLen;
      int   nBufLenReqd;
      int*  pAFCodePairs;

}AEEAFCodePairs;

Members:
   nBufLen       : specifies the length of pAFCodePairs buffer in bytes.
   nBufLenReqd   : Indicates the total buffer size
   pAFCodePairs  : Alternate frequencies list. If any element in
                   pAFCodePairs is <=15, it means that 15 alternate freq-
                   uencies follows the list.
Comments:
   None

See Also:
   None

============================================================================
AEEAICode

Description:
   Contains Application identification for open data. We don't maintain any
   queue for AI data.

Definition:
   typedef struct {

      int      nAGTypeCode;
      uint16   nMessage;
      uint16   nAID;

   }AEEAICode;

Members:
   nAGTypeCode  : Application group type code, least significant 5 bits
                  of the Block - B (application group type code)
   nMessage     : Message bits (Block - C)
   nAID         : Application identification (Block - D)

Comments:
   None

See Also:
   None

============================================================================

AEEFMRDSGroup

Description:
   When the user requests for raw data blocks the user gets blocks of data
   in this format

Definition:

   typedef struct {

      uint16   nBlockA;
      uint16   nBlockB;
      uint16   nBlockC;
      uint16   nBlockD;


   }AEEFMRDSGroup;

Members:
   nBlockA   :  Block data comprising of data for block A
   nBlockB   :  Block B data
   nBlockC   :  Block C data in a given group
   nBlockD   :  Block D data in the given group

Comments:
   The data in the blocks have their most significant bit transmitted first
   Thus the last bit in the block has a weight of 2^0

   Apps would receive only blocks that are good

See Also:
   None

============================================================================

AEEFMRDSParsedInfo

Description:
   Contains RDS message info. Each RDSInfo structure contains items
   correspond directly to fields as described in the RDS messaging format
   specification (RFC1035).

Definition:
   typedef struct {

      uint16            nPICode;
      uint8             nPTYCode;
      boolean           bTPCode;
      char              aPSName[8];
      char              aPTYName[8];
      boolean           bTACode;
      boolean           bABTextFalg;
      uint8             nDICode;
      boolean           bMSCode;
      char              aRTMessage[64];
      uint8             nECCode;
      AEEPINCode        sPinCode;
      AEEAFCodePairs    sAFCodePairs;
      AEEClockTDCode    sClkTDCode;
      AEEAICode         sAICode;

}AEEFMRDSParsedInfo;

Members:
   nPICode        : specifies Program identification code.

   nPTYCode       : Program Type codes(5 bits. ex: News, sports, Music.

   bTPCode        : Traffic Program Code, gives details about traffic
                    announcements.

   aPSName[8]     : Program service name. Used to inform the listener what
                    program service is being broadcast by the station.

   aPTYName[8]    : Program type name. Allows further description of the
                    current program type.

   bTACode        : Traffic announcement code. used combinedly with TPCode to
                    mention whether program carries traffic announcements or
                    not.

   bABTextFalg    : Indicates A/B text flag

   nDICode        : Decoder Identification code. Used to indicate different
                    operating modes.

   bMSCode        : Music/Speech code.

   aRTMessage[64] : Radiotext message. Radiotext is transmitted as 8-bit
                    characters.

   nECCode        : Extended country code.

   sPinCode       : Program item number code. It is the scheduled broadcast
                    start time and day of month.

   sAFCodePairs   : Alternate frequencies list. If any element in
                    pAFCodePairs is <=15, it means that 15 alternate freq-
                    uencies follows the list. nBufLen specifies

   sClkTDCode     : Holds Clock date & time code

   sAICode        : Application identification code.

Comments:
   We don't parse slow labelling codes used for paging, TMC identification,
   language codes, Identification of EWS channel except Extended Country code
   We don't parse data corresponding to transparent data channels,radio paging
   EWS, Enhanced radio paging or ODA & Enhanced other networks information.

See Also:
   None

============================================================================
      INTERFACE DOCUMENTATION
============================================================================
IFMRDS

The IFMRDS is a simple way to read the RDS(Radio Data System) data
transmitted in digital form by conventional FM radio broadcasts. The RDS
system standardizes several types of information transmitted.


The IFMRDS implementation is a very simple interface, just passes on
the data received from lower layers. Whenever there is data from lower layers
it just calls all registered apps, and the apps can retrieve all the data
by calling the proper API's.


RDS data is transmitted in the form of groups by the base station.
Each group that is transmitted by the base station is as shown below.
Where in each group consists of 4 blocks of each 26 bits. Each block
comprises of an information word and a checkword. Where as the information
word is contained in 16 bits and checkword in the remaining 10 bits.


===pre>
 |----------------|----------|---- | - | - |-----|-----|----------|----------------|----------|---------------|----------|
 | PI Code        |CheckWord |Group|   |   |     |     |CheckWord |                | CheckWord|               | CheckWord|
 |                |   +      |type |B0 |TP | PTY |     |   +      |                |     +    |               |     +    |
 |________________|_Offset A_|code |___|___|_____|_____|_Offset B_|________________|_Offset C`|_______________|_Offset D_|

===/pre>



  Where,

   1. Group type code = 4 bits
   2. B0 = version code = 1 bit
   3. PI code = Program Identification code = 16 bits
   4. TP = Traffic Program Identification code = 1 bit
   5. PTY = Program Type code = 5 bits
   6. Checkword + offset "N" = 10 bits added to provide error protection
      and block and group synchronization information.

We also the give the data to the apps in the same format except that we
remove the checksum bits and give the remaining data as it is.

All information words, binary numbers or binary address values
have their most significant bit (m.s.b.) transmitted first. Thus the last
bit transmitted in a binary number or address has weight 2^0.

The way apps can listen for events is they can just attach a call back
to the IFMRDS_InfoReadable by passing the AEEFMRDSInfo id in which they are
interested for and that CB would be fired when ever we have data to be
consumed.

============================================================================

IFMRDS_AddRef

Description:
   This function increments the reference count of the IFMRDS Interface
   object. This allows the object to be shared by multiple callers. The
   object is freed when the reference count reaches 0 (zero).

Prototype:
   uint32 IFMRDS_AddRef(IFMRDS * pif);

Parameters:
   pif : Pointer to the IFMRDS Interface object.

Return Value:
   Incremented reference count for the object.

Comments:
   A valid object returns a positive reference count.

See Also:
   IFMRDS_Release

============================================================================

IFMRDS_Release

Description:
   This function decrements the reference count of the IFMRDS Interface
   object. If the reference count reaches 0 (zero), the object associated
   with the IFMRDS Interface is destroyed.

Prototype:
   uint32 IFMRDS_Release(IFMRDS *pif);

Parameters:
   pif : Pointer to the IFMRDS interface object.

Return Value:
   Decremented reference count for the object.

   0 (zero), If the object has been freed and is no longer valid.

Comments:
   None

See Also:
   IFMRDS_AddRef

============================================================================

IFMRDS_QueryInterface

Description:
   This function asks an object for another API contract from the object in
   question.

Prototype:
   int IFMRDS_QueryInterface(IFMRDS   *pif,
                             AEEIID   idReq,
                             void     **ppo);

Parameters:
   pif     :  Pointer to the IFMRDS interface object.
   idReq   :  Requested ClassID exposed by the object.
   ppo     :  Returned object. Filled by this function.

Return Value:
   SUCCESS, interface found.

   ENOMEMORY, insufficient memory.

   ECLASSNOTSUPPORT, requested interface is unsupported.

Comments:
   If *ppo is an interface pointer, then the pointer in *ppo is set to the
   new interface (with refcount incremented), or NULL if the ClassID is not
   supported by the object.

   If *ppo is a data structure pointer, then *ppo is set to the internal
   data represented by the classID or set to NULL if classID is not
   supported by the object.

See Also:
   None

============================================================================

IFMRDS_GetInfoData

Description:
   This function tries to retrieve the data requested for FMRDSInfo id when
   the app is notified through IFMRDS_InfoReadable CB.

   The apps can also directly poll to get the info data, in that case there
   may be cases in which there's no data available yet for that FMRDSInfo.


Prototype:
   int IFMRDS_GetInfoData(
        IFMRDS        *pif,
        AEEFMRDSInfo  nId,
        int           *pnSequence,
        void          *pBuf,
        uint32        nBufSize,
        uint32        *pnBufSizeReq);

Parameters:

   pif         :  Pointer to the IFMRDS Interface object.
   nId         :  The RDSSource data id for which data has to be retrieved.
   pnSequence  :  This will be incremented each time we receive new block of
                  data. Using this sequence number apps can come to know
                  whether they have retrieved this chunk of data already.
   pBuf        :  Contains the corresponding information.
   nBufSize    :  The size of the pBuf in multiple of sizeof(AEEFMRDSGroup).
   pnBufSizeReq:  Specifies the size that would be required to get the buffer
                  will be a multiple of sizeof(AEEFMRDSGroup).

Return Value:
   SUCCESS         : If successful.
   ENOTYPE         : If no data is available for that Id.
   EBADPARM        : If either pif, pnSequence or pnFMRDSBufSizeReq is NULL
   EBUFFERTOOSMALL : If the provided buffer is too small to fill with data

Comments:
   If the pBuf size is not sufficient to hold the curent data,
   upon return pnBufSizeReq is filled with the required size & pBuf
   is partially filled according to nBufSize.

===pre>
   Usage:

   int nBufSize = 0, nBufSizeReq = 0, nSequence = 0;
   void *pBuf = NULL;

   nResult = IFMRDS_GetInfoData(pIFMRDS,
                                AEEFMRDS_INFO_0A,
                                &nSequence,
                                NULL,
                                0,
                                &nBufSizeReq);
  if(SUCCESS != nResult){
      // Do Error handling
   }

   pBuf = MALLOC(nBufSizeReq * sizeof(AEEFMRDSGroup));
   if(NULL == pBuf){
      nResult = ENOMEMORY;
      //Do Error handling
   }
   
   nBufSize = nBufSizeReq;
   nResult = IFMRDS_GetInfoData(pIFMRDS,
                                AEEFMRDS_INFO_0A,
                                &nSequence,
                                pBuf,
                                nBufSize,
                                &nBufSizeReq);
   if(SUCCESS != nResult){
      //Do Error handling
   }

===/pre>

See Also:
   None

=============================================================================

IFMRDS_GetNextData

Description:
   This function is used to retrieve the raw data. The data would be
   returned in groups in AEEFMRDSGroup

Prototype:
   int IFMRDS_GetNextData(IFMRDS          *pif,
                          int             *pnSequence,
                          AEEFMRDSGroup   *pFMRDSGroupsBuf,
                          int             nFMRDSGroupsBufSize,
                          int             *pnFMRDSBufSizeReq);

Parameters:

   pif                  :  Pointer to the IFMRDS Interface object.
   pnSequence           :  This will be incremented each time we receive
                           new block of data. Using this sequence number
                           apps can come to know whether they have
                           retrieved this chunk of data already.
   pFMRDSGroupsBuf      :  Contains the raw data blocks
   nFMRDSGroupsBufSize  :  The size of the pFMRDSGroupsBuf in multiple of
                           sizeof(AEEFMRDSGroup).
   pnFMRDSBufSizeReq    :  Specifies the size that would be required to get the
                           buffer will be a multiple of sizeof(AEEFMRDSGroup).

Return Value:
   SUCCESS         : If successful.
   ENOTYPE         : If no data is available for that type.
   EBUFFERTOOSMALL : If the provided buffer is too small to fill with data
   EBADPARM        : If either pnSequence or pnFMRDSBufSizeReq is NULL

Comments:
   If the pFMRDSGroupsBuf size is not sufficient to hold the curent data,
   upon return pnBufSizeReq is filled with the required size & pFMRDSGroupsBuf
   is partially filled according to nFMRDSGroupsBufSize.

===pre>
   Usage:

   int nFMRDSGroupsBufSize = 0, nFMRDSBufSizeReq = 0, nSequence = 0;
   void *pFMRDSGroupsBuf = NULL;

   nResult = IFMRDS_GetNextData(pIFMRDS,
                                &nSequence,
                                NULL,
                                0,
                                &nFMRDSBufSizeReq);
   if(SUCCESS != nResult){
      // Do Error handling
   }

   pFMRDSGroupsBuf = MALLOC(nFMRDSBufSizeReq * sizeof(AEEFMRDSGroup));
   if(NULL == pFMRDSGroupsBuf){
      nResult = ENOMEMORY;
      //Do Error handling
   }
   
   nFMRDSGroupsBufSize = nFMRDSBufSizeReq;
   nResult = IFMRDS_GetNextData(pIFMRDS,
                                &nSequence,
                                pFMRDSGroupsBuf,
                                nFMRDSGroupsBufSize,
                                &nFMRDSBufSizeReq);
   if(SUCCESS != nResult){
      //Do Error handling
   }

===/pre>


See Also:
   None

=============================================================================
IFMRDS_GetParsedData

Description:

Prototype:
   int IFMRDS_GetParsedData(IFMRDS              *pif,
                            AEEFMRDSInfo        nId,
                            AEEFMRDSParsedInfo  *pRDSInfo);

Parameters:

   pif      :  Pointer to the IFMRDS Interface object.
   nId      :  The RDSSource data id for which parsed data has to be retrieved
   pRDSInfo :  Pointer to RDSInfo structure.

Return Value:
   SUCCESS  : If successful.
   ENOTYPE  : If no data is available for that Id.
   EBADPARM : If pRDSInfo is NULL

Comments:

   Though the passed in RDSInfo structure may contain all the members for
   all groups, but upon return the user should look only for data that is
   valid for that group.

   E.g., The structure may contain a ptr to RT buffer, if the user
   has queried for type OA elements then he is expected to look for only
   Program Service (PS) Name, in the structure apart from the common fields
   like PI code, PT code, and TP id code.

See Also:
   None

=============================================================================

IFMRDS_InfoReadable

Description:
   This function schedules a callback to be called when it has any RDS data
   indicated by nId ready for the app to consume.

Prototype:
   void IFMRDS_InfoReadable(IFMRDS              *pif,
                            AEECallback         *pcb,
                            AEEFMRDSInfo        nId
                            AEEFMRDSDataStream  nType);

Parameters:
   pif       :  Pointer to the IFMRDS Interface object.
   pcb       :  Where to call when the IFMRDS may be readable.
   nId       :  The FMRDSInfo id in which the apps are interested.
   nType     :  Indicates in what type of data stream the user is interested

Return Value:
   None.

Comments:
   The nId argument is ignored when nType is AEEFMRDS_RAW_DATA.

See Also:
   None

============================================================================
 
*/

#endif //AEEIFMRDS_H

