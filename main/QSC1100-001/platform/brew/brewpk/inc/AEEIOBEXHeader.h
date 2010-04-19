#ifndef AEEIOBEXHEADER_H
#define AEEIOBEXHEADER_H
/*=============================================================================
FILE:         AEEIOBEXHeader.h

SERVICES:     IOBEXHeader Interface

DESCRIPTION:  IOBEXHeader is the interface that allows
              operations on a OBEX header List. 


===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

/*============================================================================
 Includes
============================================================================*/
#include "AEEIQI.h"

/*=============================================================================
 Interface ID
=============================================================================*/

#define AEEIID_IOBEXHeader   0x01058640

/*=============================================================================
   Definitions, Macros
==============================================================================*/

/*==============================================================================
 OBEX Header IDs defined in Sec 2.1 of IrDa OBEX v1.3 specification
===============================================================================*/
#define AEEOBEX_HDR_COUNT                       0xC0
#define AEEOBEX_HDR_NAME                        0x01
#define AEEOBEX_HDR_TYPE                        0x42
#define AEEOBEX_HDR_LENGTH                      0xC3
#define AEEOBEX_HDR_TIME                        0x44
#define AEEOBEX_HDR_DESCRIPTION                 0x05
#define AEEOBEX_HDR_TARGET                      0x46
#define AEEOBEX_HDR_HTTP                        0x47
#define AEEOBEX_HDR_BODY                        0x48
#define AEEOBEX_HDR_END_OF_BODY                 0x49
#define AEEOBEX_HDR_WHO                         0x4A
#define AEEOBEX_HDR_CONNECTION_ID               0xCB
#define AEEOBEX_HDR_APPLICATION_PARAMS          0x4C
#define AEEOBEX_HDR_AUTHENTICATION_CHALLENGE    0x4D
#define AEEOBEX_HDR_AUTHENTICATION_RESPONSE     0x4E
#define AEEOBEX_HDR_CREATOR_ID                  0xCF
#define AEEOBEX_HDR_OBJECT_CLASS                0x51


// A data type to refer to OBEX header IDs
typedef uint32 AEEOBEXHeaderID;

/*==============================================================================
 OBEX Header data Types defined in Sec 2.1 of IrDa OBEX v1.3 specification
===============================================================================*/

#define AEEOBEX_HDR_UNICODE                    0x00
#define AEEOBEX_HDR_BYTE_SEQ                   0x40
#define AEEOBEX_HDR_BYTE                       0x80
#define AEEOBEX_HDR_UINT32                     0xC0

// A data type to refer to OBEX header types
typedef uint32 AEEOBEXHeaderType;

/*=============================================================================
 OBEX Header Structure Definition
=============================================================================*/
typedef struct
{
   AEEOBEXHeaderID     headerId;
   AEEOBEXHeaderType   headerType;
   int                 nHeaderSize;
}  AEEOBEXHeader;


/*============================================================================
 OBEX Header Interface Definition
============================================================================*/

#define INHERIT_IOBEXHeader(iname) \
   INHERIT_IQI(iname);\
   int (*AddString)          (iname             *po,            \
                              AEEOBEXHeaderID   headerId,       \
                              const AECHAR      *pwSeq,         \
                              int               nwSeqLen);      \
                                                                \
   int (*AddU32)             (iname             *po,            \
                              AEEOBEXHeaderID   headerId,       \
                              uint32            dwData);        \
                                                                \
   int (*AddByteSeq)         (iname             *po,            \
                              AEEOBEXHeaderID   headerId,       \
                              const uint8       *pBuf,          \
                              int               nBufLen);       \
                                                                \
   int (*GetU32)             (iname             *po,            \
                              AEEOBEXHeaderID   headerId,       \
                              uint32            *pData);        \
                                                                \
   int (*GetByteSeq)         (iname             *po,            \
                              AEEOBEXHeaderID   headerId,       \
                              int               nOffSet,        \
                              uint8             *pBuf,          \
                              int               nBufLen,        \
                              int               *pnBufLenReq);  \
                                                                \
   int (*GetString)          (iname             *po,            \
                              AEEOBEXHeaderID   headerId,       \
                              AECHAR            *pwSeq,         \
                              int               nwSeqLen,       \
                              int               *pnwSeqLenReq); \
                                                                \
   int (*GetList)            (iname             *po,            \
                              AEEOBEXHeader     *pHdr,          \
                              int               nOffset,        \
                              int               nHdrLen,        \
                              int               *pnHdrLenReq);  \
                                                                \
   int (*GetU32AtIndex)      (iname             *po,            \
                              int               nIndex,         \
                              uint32            *pData);        \
                                                                \
   int (*GetByteSeqAtIndex)  (iname             *po,            \
                              int               nIndex,         \
                              int               nOffSet,        \
                              uint8             *pData,         \
                              int               nDataLen);      \
                                                                \
   int (*GetStringAtIndex)   (iname             *po,            \
                              int               nIndex,         \
                              AECHAR            *pwSeq,         \
                              int               nwSeqLen)
AEEINTERFACE_DEFINE(IOBEXHeader);


/*============================================================================
 Interface Accessor Functions
============================================================================*/

static __inline uint32 IOBEXHeader_AddRef( IOBEXHeader *p )
{
   return AEEGETPVTBL((p),IOBEXHeader)->AddRef( p );
}

static __inline uint32 IOBEXHeader_Release( IOBEXHeader *p )
{
   return AEEGETPVTBL((p),IOBEXHeader)->Release( p );
}

static __inline int IOBEXHeader_QueryInterface( IOBEXHeader *p,
                                                AEECLSID idReq, void **ppo )
{
   return AEEGETPVTBL((p),IOBEXHeader)->QueryInterface( p, idReq, ppo );
}

static __inline int IOBEXHeader_AddString(IOBEXHeader *p ,
                                 AEEOBEXHeaderID   headerId,
                                 const AECHAR      *pwSeq,
                                 int               nwSeqLen)
{
   return AEEGETPVTBL((p),IOBEXHeader)->AddString( p, headerId,
                                             pwSeq, nwSeqLen);
}

static __inline int IOBEXHeader_AddByteSeq(IOBEXHeader     *p,
                                           AEEOBEXHeaderID headerId,
                                           const uint8     *pSeq,
                                           int             nSeqLen)
{
   return AEEGETPVTBL((p),IOBEXHeader)->AddByteSeq(p, headerId, pSeq, nSeqLen);
}

static __inline int IOBEXHeader_AddU32(IOBEXHeader       *p,
                                       AEEOBEXHeaderID   headerId,
                                       uint32            data)
{
   return AEEGETPVTBL((p),IOBEXHeader)->AddU32( p, headerId,data );
}

static __inline int IOBEXHeader_AddByte(IOBEXHeader       *p,
                                        AEEOBEXHeaderID   headerId,
                                        uint8             data)
{
   uint32 loc = data;
   return AEEGETPVTBL((p),IOBEXHeader)->AddU32( p, headerId,loc );
}

static __inline int IOBEXHeader_GetList(IOBEXHeader         *p,
                                        AEEOBEXHeader       *pHdr,
                                        int                 nOffset,
                                        int                 nHdrLen,
                                        int                 *pnHdrLenReq)
{
   return AEEGETPVTBL((p),IOBEXHeader)->GetList( p, pHdr, nOffset, nHdrLen, pnHdrLenReq);
}
static __inline int IOBEXHeader_GetString(IOBEXHeader *p ,
                                 AEEOBEXHeaderID   headerId,
                                 AECHAR            *pwSeq,
                                 int               nwSeqLen,
                                 int               *pnwSeqLenReq)
{
   return AEEGETPVTBL((p),IOBEXHeader)->GetString( p, headerId,
                                             pwSeq, nwSeqLen,pnwSeqLenReq);
}


static __inline int IOBEXHeader_GetByteSeq(IOBEXHeader     *p,
                                           AEEOBEXHeaderID headerId,
                                           int             nOffSet,
                                           uint8           *pSeq,
                                           int             nSeqLen,
                                           int             *pnSeqLenReq)
{
   return AEEGETPVTBL((p),IOBEXHeader)->GetByteSeq(p, headerId, nOffSet,pSeq, nSeqLen,
                                                   pnSeqLenReq );
}

static __inline int IOBEXHeader_GetU32(IOBEXHeader       *p,
                                       AEEOBEXHeaderID   headerId,
                                       uint32            *pdwData)
{
   return AEEGETPVTBL((p),IOBEXHeader)->GetU32( p, headerId,pdwData );
}

static __inline int IOBEXHeader_GetByte(IOBEXHeader       *p,
                                        AEEOBEXHeaderID   headerId,
                                        uint8             *pData)
{  
   int result;
   uint32 loc = 0;
   result = AEEGETPVTBL((p),IOBEXHeader)->GetU32( p, headerId, &loc );
   *pData = (uint8)loc;
   return result;
}

static __inline int IOBEXHeader_GetU32AtIndex(IOBEXHeader   *p,
                                  int           nIndex,
                                  uint32        *pData)
{
   return AEEGETPVTBL((p),IOBEXHeader)->GetU32AtIndex( p, nIndex, pData);
}


static __inline int IOBEXHeader_GetByteAtIndex(IOBEXHeader   *p,
                                   int           nIndex,
                                   uint8         *pData)
{
   int result;
   uint32 loc = 0;
   result = AEEGETPVTBL((p),IOBEXHeader)->GetU32AtIndex( p, nIndex, &loc);
   *pData = (uint8)loc;
   return result;
}

static __inline int IOBEXHeader_GetByteSeqAtIndex(IOBEXHeader     *p,
                                                  int             nIndex,
                                                  int             nOffSet,
                                                  uint8           *pBuf,
                                                  int             nBufLen)
{
   return AEEGETPVTBL((p),IOBEXHeader)->GetByteSeqAtIndex( p, nIndex,nOffSet,pBuf,nBufLen);
}

static __inline int IOBEXHeader_GetStringAtIndex(IOBEXHeader     *p,
                                                 int             nIndex,
                                                 AECHAR          *pwSeq,
                                                 int             nwSeqLen)
{
   return AEEGETPVTBL((p),IOBEXHeader)->GetStringAtIndex( p, nIndex,pwSeq,nwSeqLen);
}

/*===============================================================================
   DATA STRUCTURE DOCUMENTATION
===============================================================================
AEEOBEXHeaderID

Definition
   typedef uint32 AEEOBEXHeaderID;

Description:
   OBEX Header IDs defined in Sec.2.1 of IrDa OBEX  specification v1.3

   AEEOBEX_HDR_COUNT                      : Count header, type is uint32
   AEEOBEX_HDR_NAME                       : Name Header, type is unicode (usc2) string
   AEEOBEX_HDR_TYPE                       : Type Header, type is byte sequence
   AEEOBEX_HDR_LENGTH                     : Length Header, uint32
   AEEOBEX_HDR_TIME                       : Time Header,type is byte sequence
   AEEOBEX_HDR_DESCRIPTION                : Description Header, type is unicode (usc2) string
   AEEOBEX_HDR_TARGET                     : Target Header,type is byte sequence
   AEEOBEX_HDR_HTTP                       : HTTP Header,type is byte sequence
   AEEOBEX_HDR_BODY                       : Body Header, type byte sequence
   AEEOBEX_HDR_END_OF_BODY                : End of Body Header, byte sequence
   AEEOBEX_HDR_WHO                        : Who Header, byte sequence
   AEEOBEX_HDR_CONNECTION_ID              : connection Id header,uint32 
   AEEOBEX_HDR_APPLICATION_PARAMS         : Application Parameters, byte sequence
   AEEOBEX_HDR_AUTHENTICATION_CHALLENGE   : Authentication challenge header, byte sequence
   AEEOBEX_HDR_AUTHENTICATION_RESPONSE    : Authentication Response Header, byte sequence
   AEEOBEX_HDR_CREATOR_ID                 : Creator ID , uint32
   AEEOBEX_HDR_OBJECT_CLASS               : Object class,byte sequence

===============================================================================
AEEOBEXHeaderType

Definition
   typedef uint32 AEEOBEXHeaderType;

Description: A data type to refer to OBEX header types

   AEEOBEX_HDR_UNICODE                   : Null terminated Unicode text ( USC2)
   AEEOBEX_HDR_BYTE_SEQ                  : byte sequence
   AEEOBEX_HDR_BYTE                      : single byte value
   AEEOBEX_HDR_UINT32                    : four byte value

=============================================================================

AEEOBEXHeader

Description
   Specifies a header in the list.This can be used by the application 
   while reading headers received from the remote device.IOBEXHeader_GetList()
   can be used for this purpose.

Definition

typedef struct
{
   AEEOBEXHeaderID     headerId;
   AEEOBEXHeaderType   headerType;
   int                 nHeaderSize;
}  AEEOBEXHeader;


Members:
   headerId         : Header ID , For E.g Name, Type 
   headerType       : Type of the header For E.g Unicode (usc2) string, byte sequence,
                      single byte or four byte value
   nHeaderSize      : size of header value.: For E.g if the 
                      headerId = AEEOBEX_HDR_NAME, then
                      headerType = AEEOBEX_HDR_UNICODE
                      nHeaderSize = 8, if name = "xyz.txt"
                      Note : nHeaderSize can be zero
                             nHeaderSize for a four byte header( uint32) and single byte
                             header is 1.

See Also :
   IOBEXHeader_GetList()


================================================================================
   INTERFACE DOCUMENTATION
===============================================================================

Interface Name: IOBEXHeader

Description: 

   This interface represents an OBEX header list. The header list can be sent
   optionally along with an OBEX command or OBEX response.
   When an application needs to send a list of headers
   to the remote device, it should create an instance of IOBEXHeader using 
   IOBEXCli_CreateHeaderList() for Clients or IOBEXSrvSession_CreateHeaderList() for server.
   Headers can be added to the list via IOBEXHeader_AddString(),IOBEXHeader_AddByteSeq()
   IOBEXHeader_AddU32(),IOBEXHeader_AddByte() APIs.This header list can 
   be sent along with the command for E.g. IOBEXClient_Put().The application 
   should call IOBEXHeader_Release() once the header has been sent with a command 
   or response.

   Headers received from the remote are encapsulated within the IOBEXHeader object.
   A pointer to this object may be returned to the application when the application
   calls IOBEXClient_GetEvent() or  IOBEXSrvSession_GetEvent().The headers
   in the list can be retrieved by calling IOBEXHeader_GetXXX()or
   IOBEXHeader_GetXXXAtIndex().
   After the application has parsed the headers, it should call IOBEXHeader_Release()
   to free the list.

===H2>
   Usage example:
===/H2>
===pre>
   ===== To create an OBEX header object

   IOBEXHeader *piHdr;
   int ret;
   ret = IOBEXCli_CreateHeaderList(pMe->pCli, &piHdr);

   if (AEE_SUCCESS == ret)
   {
      IOBEXHeader_AddByteSeq(piHdr, AEEOBEX_HDR_BODY, data, dataLen);
      IOBEXCli_Put(pMe->pCli, piHdr, FALSE);
      // done sending the header list
      IOBEXHeader_Release(piHdr);
   }

   ===== To fetch OBEX headers received from the remote

   ret = IOBEXCli_GetEvent(pMe->pCli, &event, &piHdr, &cmdStatus, &flags);

   if (piHdr != NULL)
   {
      // optionally get the list
      IOBEXHeader_GetList(piHdr, pHdrList, 0, hdrListLen, &reqHdrListLen);
      
      IOBEXHeader_GetByteSeq(piHdr, AEEOBEX_HDR_BODY, &buf, bufLen, &reqBufLen);
      
      // done with the received header list
      IOBEXHeader_Release(piHdr);
   }
   ===/pre>
==============================================================================

IOBEXHeader_AddRef()

Description:
   Inherited from IQI_AddRef().

See Also:
   IOBEXHeader_Release()

==============================================================================

IOBEXHeader_Release()

Description:
   Inherited from IQI_Release().

See Also:
   IOBEXHeader_AddRef()
   
==============================================================================

IOBEXHeader_QueryInterface()

Description:
   Inherited from IQI_QueryInterface().
   It can be used to
     -  Get a pointer to an available interface based on the input class ID
     -  Query an extended version of the IOBEXHeader-derived class

Prototype:
   int IOBEXClient_QueryInterface (IOBEXHeader* po, AEECLSID clsReq, 
                                   void** ppo);

Parameters:
   po [i]      : the object
   clsReq [i]  : Class ID of the interface to query.
   ppo [o]     : place holder for pointer to the interface.

Return Value:
   AEE_SUCCESS : on success, 
   Otherwise   : an error code.

Comments:
   On return of the API, if content of ppo is NULL, then the queried 
   interface is not available.

Side Effects:
   If an interface is retrieved, then this function increments its 
   reference count.

===============================================================================

Function: IOBEXHeader_AddString()

Description:
   An application can call this function to add to the list, an OBEX header
   which is of type unicode (usc2) string.The standard headers that fall under
   this category are AEEOBEX_HDR_NAME and AEEOBEX_HDR_DESCRIPTION.

Prototype:

   int IOBEXHeader_AddString(IOBEXHeader       *po,
                             AEEOBEXHeaderID   headerId,
                             const AECHAR      *pwSeq,
                             int               nwSeqLen)
Parameters
   po [i]         : pointer to the IOBEXHeader object
   headerId [i]   : OBEX header ID
   pwSeq [i]      : pointer to the unicode (usc2) string
   nwSeqLen [i]   : number of wide characters allocated for the 
                    string including the NULL terminator.For e.g , if 
                    string is "xyz.txt", nwSeqLen = 8

Return Value:
   AEE_SUCCESS    : the information was stored successfully
   AEE_EBADPARM   : bad parameters
   AEE_ENOMEMORY  : no memory to hold string

===============================================================================

Function: IOBEXHeader_AddByteSeq()

Description:
   An application can call this function to add to the list, an OBEX header
   which is a byte sequence.The standard headers that fall under
   this category are AEEOBEX_HDR_TIME,AEEOBEX_HDR_TARGET,AEEOBEX_HDR_HTTP
   AEEOBEX_HDR_BODY, AEEOBEX_HDR_END_OF_BODY, AEEOBEX_HDR_APPLICATION_PARAMS,
   AEEOBEX_HDR_OBJECT_CLASS.

Prototype:

   int IOBEXHeader_AddByteSeq(IOBEXHeader       *po,
                              AEEOBEXHeaderID   headerId,
                              const uint8       *pSeq,
                              int               nSeqLen)
Parameters
   po [i]         : pointer to the IOBEXHeader object
   headerId [i]   : OBEX header ID
   pSeq [i]       : pointer to the byte sequence
   nSeqLen [i]    : number of bytes in the sequence
                    
Return Value:
   AEE_SUCCESS    : the header was stored successfully
   AEE_EBADPARM   : bad parameters
   AEE_ENOMEMORY  : no memory to hold the sequence

===============================================================================

Function: IOBEXHeader_AddU32()

Description:
   An application can call this function to add to the list, an OBEX header
   which is a four byte value.The standard headers that fall under
   this category are AEEOBEX_HDR_COUNT,AEEOBEX_HDR_LENGTH,
   AEEOBEX_HDR_CREATOR_ID, AEEOBEX_HDR_CONNECTION_ID

Prototype:

   int IOBEXHeader_AddU32(IOBEXHeader       *po,
                          AEEOBEXHeaderID   headerId,
                          uint32            dwData)
Parameters
   po [i]         : pointer to the IOBEXHeader object
   headerId [i]   : OBEX header ID
   dwData [i]     : four byte header value

Return Value:
   AEE_SUCCESS    : the header was stored successfully
   AEE_EBADPARM   : bad parameters
   AEE_ENOMEMORY  : no memory to hold the header

===============================================================================

Function: IOBEXHeader_AddByte()

Description:
   An application can call this function to add to the list, an OBEX header
   which is a single byte value.

Prototype:

   int IOBEXHeader_AddByte(IOBEXHeader       *po,
                           AEEOBEXHeaderID   headerId,
                           uint8             data)
Parameters
   po [i]         : pointer to the IOBEXHeader object
   headerId [i]   : OBEX header ID
   data [i]       : single byte header value

Return Value:
   AEE_SUCCESS    : the header was stored successfully
   AEE_EBADPARM   : bad parameters
   AEE_ENOMEMORY  : no memory to hold the header

===============================================================================

Function: IOBEXHeader_GetList()

Description:
   An application can call this function to get an array of
   header IDs and their corresponding size.

Prototype:

   int IOBEXHeader_GetList(IOBEXHeader      *po,
                           AEEOBEXHeader    *pHdr,
                           int              nOffset,
                           int              nHdrLen,
                           int              *pnHdrLenReq)
Parameters
   po [i]              : pointer to the IOBEXHeader object
   pHdr [o]            : pointer to an array of AEEOBEXHeader structures
   nOffset [i]         : offset from the start of the list
                         For E.g nOffset = 0 and nHdrLen = 1,
                         will return information about the first
                         header in the list.
                         if nOffset = 0 and nHdrLen = 0, the API will return
                         error AEE_EBUFFERTOOSMALL and pnHdrLenReq will 
                         contain the number of headers in the list.
   nHdrLen [i]         : number of elements in pHdr
   pnHdrLenReq [o]     : if return value is AEE_EBUFFERTOOSMALL,
                         this location will contain the number of 
                         AEEOBEXHeader elements in pHdr required to
                         return all headers in the list. For E.g, if the 
                         header list contains 3 headers: Name, Length and
                         Body, then *pnHdrLenReq will be = 3 after the
                         function returns.
Return Value:
   AEE_SUCCESS         : the header list was returned successfully
                         pnHdrLenReq will contain the number of elements
                         copied.
   AEE_EBADPARM        : bad parameters
   AEE_ENOSUCH         : offset is invalid
   AEE_EBUFFERTOOSMALL : number of AEEOBEXHeader elements in the list 
                         is insufficient.pnHdrLenReq will contain the 
                         total number of elements in the list.

===============================================================================

Function: IOBEXHeader_GetString()

Description:
   An application can call this function to retrieve an OBEX header which is 
   a unicode (usc2) string.

Prototype:

   int IOBEXHeader_GetString(IOBEXHeader       *po,
                             AEEOBEXHeaderID   headerId,
                             AECHAR            *pwSeq,
                             int               nwSeqLen,
                             int               *pnwSeqLenReq)
Parameters
   po [i]              : pointer to the IOBEXHeader object
   headerId [i]        : header Id of the header being retrieved
   pwSeq [o]           : pointer to buffer to store the string
   nwSeqLen [i]        : length of the buffer allocated to 
                         retrieve the string.The length includes the NULL
                         terminator
   pnwSeqLenReq [o]    : if return value is AEE_EBUFFERTOOSMALL,
                         this location will contain the size of buffer
                         that is required to return the entire string.
                         For E.g. if the string is "xyz.txt", then *pnwSeqLenReq
                         will contain 8.

Return Value:
   AEE_SUCCESS         : the header was returned successfully
   AEE_EBADPARM        : bad parameters
   AEE_EBUFFERTOOSMALL : buffer allocated for the string is insufficient
   AEE_ENOSUCH         : no header found with matching header ID

===============================================================================

Function: IOBEXHeader_GetByteSeq()

Description:
   An application can call this function to retrieve an OBEX header which is 
   a byte sequence.

Prototype:

   int IOBEXHeader_GetByteSeq(IOBEXHeader     *po,
                              AEEOBEXHeaderID headerId,
                              int             nOffSet,
                              uint8           *pSeq,
                              int             nSeqLen,
                              int             *pnSeqLenReq)
Parameters
   po [i]              : pointer to the IOBEXHeader object
   headerId [i]        : header Id of the header being retrieved
   nOffSet [i]         : offset within the byte sequence 
   pwSeq [o]           : pointer to buffer to store the header
   nwSeqLen [i]        : length of the buffer allocated to 
                         retrieve the header
   pnwSeqLenReq [o]    : if return value is AEE_EBUFFERTOOSMALL,
                         this location will contain the size of buffer
                         that is required to return the entire header.

Return Value:
   AEE_SUCCESS         : the header was returned successfully
   AEE_EBADPARM        : bad parameters
   AEE_EBUFFERTOOSMALL : buffer allocated for the header is insufficient
   AEE_ENOSUCH         : no header found with matching header ID

===============================================================================

Function: IOBEXHeader_GetU32()

Description:
   An application can call this function to retrieve an OBEX header which is 
   a four byte value

Prototype:

   int IOBEXHeader_GetU32(IOBEXHeader       *po,
                          AEEOBEXHeaderID   headerId,
                          uint32            *pdwData)
Parameters
   po [i]              : pointer to the IOBEXHeader object
   headerId [i]        : header Id of the header being retrieved
   pdwData [o]         : pointer to location where the header is 
                         returned
Return Value:
   AEE_SUCCESS         : the header was returned successfully
   AEE_EBADPARM        : bad parameters
   AEE_ENOSUCH         : no header found with matching header ID

===============================================================================

Function: IOBEXHeader_GetByte()

Description:
   An application can call this function to retrieve an OBEX header which is 
   a single byte value

Prototype:

   int IOBEXHeader_GetByte(IOBEXHeader       *po,
                           AEEOBEXHeaderID   headerId,
                           uint8             *pData)
Parameters
   po [i]              : pointer to the IOBEXHeader object
   headerId [i]        : header Id of the header being retrieved
   pData [o]           : pointer to location where the header is 
                         returned
Return Value:
   AEE_SUCCESS         : the header was returned successfully
   AEE_EBADPARM        : bad parameters
   AEE_ENOSUCH         : no header found with matching header ID

===============================================================================

Function: IOBEXHeader_GetU32AtIndex()

Description:
   An application can call this function to retrieve an OBEX header which is 
   a four byte value and at a particular index in the list.

Prototype:

   int IOBEXHeader_GetU32AtIndex(IOBEXHeader   *po,
                                 int           nIndex,
                                 uint32        *pData)
Parameters
   po [i]              : pointer to the IOBEXHeader object
   nIndex [i]          : index of the header in the list
   pData [o]           : pointer to location where the header is 
                         returned
Return Value:
   AEE_SUCCESS         : the header was returned successfully
   AEE_EBADPARM        : bad parameters
   AEE_ENOSUCH         : a header which is a four byte value not
                         found at this index

See Also :
   IOBEXHeader_GetList()

===============================================================================

Function: IOBEXHeader_GetByteAtIndex()

Description:
   An application can call this function to retrieve an OBEX header which is 
   a single byte value and at a particular index in the list.

Prototype:

   int IOBEXHeader_GetByteAtIndex(IOBEXHeader   *po,
                                  int           nIndex,
                                  uint8         *pData
Parameters
   po [i]              : pointer to the IOBEXHeader object
   nIndex [i]          : index of the header in the list. 
   pData [o]           : pointer to location where the header is 
                         returned
Return Value:
   AEE_SUCCESS         : the header was returned successfully
   AEE_EBADPARM        : bad parameters
   AEE_ENOSUCH         : a header which is a single byte value not
                         found at this index
See Also :
   IOBEXHeader_GetList()

===============================================================================

Function: IOBEXHeader_GetByteSeqAtIndex()

Description:
   An application can call this function to retrieve an OBEX header at 
   a particular index. The header is of type byte sequence.

Prototype:

   int IOBEXHeader_GetByteSeqAtIndex(IOBEXHeader   *p,
                                     int           nIndex,
                                     int           nOffSet,
                                     uint8         *pBuf,
                                     int           nBufLen);
Parameters
   po [i]              : pointer to the IOBEXHeader object
   nIndex [i]          : index of the header in the list. 
   nOffSet [i]         : offset within the sequence from which to retrieve data

   pBuf [o]            : pointer to location where the header is 
                         returned
   nBufLen[i]          : length of the buffer

Return Value:
   AEE_SUCCESS         : the header was returned successfully
   AEE_EBADPARM        : bad parameters
   AEE_ENOSUCH         : a header which is a byte sequence not
                         found at this index
   AEE_EBUFFERTOOSMALL : buffer size insufficient

See Also :
   IOBEXHeader_GetList()

===============================================================================

Function: IOBEXHeader_GetByteSeqAtIndex()

Description:
   An application can call this function to retrieve an OBEX header at 
   a particular index. The header is of type unicode (usc2) string

Prototype:

   int IOBEXHeader_GetStringAtIndex(IOBEXHeader     *p,
                                    int             nIndex,
                                    AECHAR          *pwSeq,
                                    int             nwSeqLen)
Parameters
   po [i]              : pointer to the IOBEXHeader object
   nIndex [i]          : index of the header in the list. Zero based
   pwSeq [o]           : pointer to location where the header is 
                         returned
   nwSeqLen[i]         : length of the buffer including the null terminator

Return Value:
   AEE_SUCCESS         : the header was returned successfully
   AEE_EBADPARM        : bad parameters
   AEE_ENOSUCH         : a header which is a unicode (usc2) string not
                         found at this index
   AEE_EBUFFERTOOSMALL : buffer size insufficient

See Also :
   IOBEXHeader_GetList()

==============================================================================*/
#endif /*AEEOBEXHEADER_H */

