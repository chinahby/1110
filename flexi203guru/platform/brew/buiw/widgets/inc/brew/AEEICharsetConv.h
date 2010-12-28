#ifndef AEEICHARSETCONV_H
#define AEEICHARSETCONV_H
/*=============================================================================
        Copyright 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "brew/AEEIQI.h"

#define AEEIID_ICharsetConv 0x01044104

#define INHERIT_ICharsetConv(iname) \
   INHERIT_IQI(iname); \
   int (*Initialize)(iname* p, const char* cpszFrom, const char* cpszTo, \
                     const unsigned int cMissing); \
   int (*Convert)(iname* p, \
                  unsigned char** ppSrcBuf,  int* pnSrcSize, \
                  unsigned char** ppDstBuf,  int* pnDstSize, \
                  int* pnNumCharsConverted)

AEEINTERFACE_DEFINE(ICharsetConv);

static __inline uint32 ICharsetConv_AddRef(ICharsetConv* me)
{
   return AEEGETPVTBL(me,ICharsetConv)->AddRef(me);
}

static __inline uint32 ICharsetConv_Release(ICharsetConv* me)
{
   return AEEGETPVTBL(me,ICharsetConv)->Release(me);
}

static __inline int ICharsetConv_QueryInterface(ICharsetConv* me, AEEIID id, 
                                                void** ppo)
{
   return AEEGETPVTBL(me,ICharsetConv)->QueryInterface(me, id, ppo);
}

static __inline int ICharsetConv_Initialize(ICharsetConv* me, 
                                            const char* cpszFrom, 
                                            const char* cpszTo,
                                            const unsigned int cMissing)
{
   return AEEGETPVTBL(me,ICharsetConv)->Initialize(me, cpszFrom, cpszTo, cMissing);
}

static __inline int ICharsetConv_CharsetConvert(ICharsetConv* me, 
                                                unsigned char** ppSrcBuf,  
                                                int* pnSrcSize, 
                                                unsigned char** ppDstBuf,  
                                                int* pnDstSize, 
                                                int* pnNumCharsConverted)
{
   return AEEGETPVTBL(me,ICharsetConv)->Convert(me, ppSrcBuf, 
                                                pnSrcSize,
                                                ppDstBuf, 
                                                pnDstSize,
                                                pnNumCharsConverted);
}

/*
======================================================================
INTERFACES DOCUMENTATION
=======================================================================

ICharsetConv Interface

  ICharsetConv is used to convert strings between character encodings.  Each 
  implementation should hanlde the conversion between different width
  of its own encoding, and to and from unicode.

  The user can query the registry for the implementation of a specific
  encoding, for example:
      IRegistry_GetHandler(AEEIID_ICHARSETCONV, "Shift_JIS")
  Should return the ICharsetConv implementation for the Shift_JIS byte 
  encoding.
      IRegistry_GetHandler(AEEIID_ICHARSETCONV, "UTF-16LE>Shift_JIS") 
  Should return the ICharsetConv implementation that supports 
  UTF-16LE conversion to Shift_JIS.
  
  see: http://www.iana.org/assignments/character-sets for names of 
  encodings and class documentation for lists of specific encodings
  each class supports.

=======================================================================

ICharsetConv_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 

See Also:
    ICharsetConv_Release()

=======================================================================

ICharsetConv_Release()

Description:
    This function is inherited from IQI_Release(). 

See Also:
    ICharsetConv_AddRef()

=======================================================================

ICharsetConv_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

=======================================================================

ICharsetConv_Initialize()

Description:
    This function is used to set the source and destination encoding,
    and set the default character used when a character is missing 
    in the destination encoding.

Prototype:

ICharsetConv_Initialize(ICharsetConv* me, const char* cpszFrom, 
                        const char* cpszTo, unsigned int cMissing)

Parameters:
   me [in]:  Pointer to the ICharsetConv interface
   cpszFrom [in]:  The source encoding.
   cpszTo  [in]:  The destination encoding.
   cMissing [in]: Unicode character to be used if there is no valid mapping
                  from the source encoding.

Return value:
   AEE_SUCCESS:  The initalization was successfull.
   AEE_EUNSUPPORTED:  The source and destination encoding and width
                      combination is not supported.
=======================================================================

ICharsetConv_CharsetConvert()

Description:
    This function converts the encoding in the input buffer and stores
    it in the output buffer.

Prototype:

ICharsetConv_CharsetConvert(ICharsetConv* me, 
                            char** ppSrcBuf,  int* nSrcSize, 
                            char** ppDstBuf,  int* nDstSize, 
                            int* pnNumCharsConverted);


Parameters:
   me [in]:  Pointer to the ICharsetConv interface
   ppSrcBuf [in/out]:  The input buffer.  On output this pointer will point 
                       right after the last consumed byte.
   pnSrcSize [in/out]: The input buffer size.  On output it will be the number
                       of unconsumed bytes left in the input buffer.
   ppDstBuf [in/out]:  The output buffer.  On output it will point right after
                       the last produced byte.
   pnDstSize [in/out]: The output buffer size.  On output it will be the 
                       number of bytes left in the destination buffer.
   pnNumCharsConverted [in/out]: number of characters converted.  This
                                 value is incremented for each 
                                 successfull converted character.

Return value:
   AEE_SUCCESS:  The entire input buffer was consumed.
   AEE_EBUFFERTOOSMALL:  pDest buffer is to small.  The input 
                         buffer will point at the first unconsumed
                         byte.
   AEE_EBADITEM:  The input byte does not belong to the source 
                  encoding.  The input buffer will point at the first
                  byte of the invalid character.  

   AEE_ENOSUCH:  The input byte belongs to the source encoding, but
                 no valid maping to the destination encoding exists.

   AEE_EINCOMPLETEITEM:  The input is incomplete at the end of 
                         the input buffer.  The input buffer will 
                         point at the first unconsumed byte.

=======================================================================
*/

#endif /* #ifndef AEEICHARSETCONV_H */

