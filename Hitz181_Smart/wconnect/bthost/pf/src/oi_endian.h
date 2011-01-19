#ifndef _OI_ENDIAN_H
#define _OI_ENDIAN_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_endian.h#1 $ 

$Log: $
*
*    #3        30 May 2006            GS
* Added support for PBAP feature.
*

===========================================================================*/
/** @file  
 This file contains macros for dealing with byte-wise endianness (byte order
 in multibyte data items).
*/
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"

/* Write values in BIG endian order */

#ifndef OI_CODE
#define SetUINT64_BigEndian(dst,value)                                  \
     ((OI_UINT8*)(dst))[7] = (OI_UINT8) ((value) & 0x00000000000000ffll) ;        \
     ((OI_UINT8*)(dst))[6] = (OI_UINT8)(((value) & 0x000000000000ff00ll) >> 8) ;  \
     ((OI_UINT8*)(dst))[5] = (OI_UINT8)(((value) & 0x0000000000ff0000ll) >> 16) ; \
     ((OI_UINT8*)(dst))[4] = (OI_UINT8)(((value) & 0x00000000ff000000ll) >> 24) ; \
     ((OI_UINT8*)(dst))[3] = (OI_UINT8)(((value) & 0x000000ff00000000ll) >> 32) ; \
     ((OI_UINT8*)(dst))[2] = (OI_UINT8)(((value) & 0x0000ff0000000000ll) >> 40) ; \
     ((OI_UINT8*)(dst))[1] = (OI_UINT8)(((value) & 0x00ff000000000000ll) >> 48) ; \
     ((OI_UINT8*)(dst))[0] = (OI_UINT8)(((value) & 0xff00000000000000ull) >> 56) ;
#endif /* OI_CODE */

#define SetUINT32_BigEndian(dst,value)                                  \
     ((OI_UINT8*)(dst))[3] = (OI_UINT8) ((value) & 0x000000ff) ;        \
     ((OI_UINT8*)(dst))[2] = (OI_UINT8)(((value) & 0x0000ff00) >> 8) ;  \
     ((OI_UINT8*)(dst))[1] = (OI_UINT8)(((value) & 0x00ff0000) >> 16) ; \
     ((OI_UINT8*)(dst))[0] = (OI_UINT8)(((value) & 0xff000000) >> 24) ;

#define SetUINT24_BigEndian(dst, value)                                 \
     ((OI_UINT8*)(dst))[2] = (OI_UINT8) ((value) & 0x000000ff) ;        \
     ((OI_UINT8*)(dst))[1] = (OI_UINT8)(((value) & 0x0000ff00) >> 8) ;  \
     ((OI_UINT8*)(dst))[0] = (OI_UINT8)(((value) & 0x00ff0000) >> 16) ;

#define SetUINT16_BigEndian(dst,value)                                  \
     ((OI_UINT8*)(dst))[1] = (OI_UINT8) ((value) & 0x00ff) ;            \
     ((OI_UINT8*)(dst))[0] = (OI_UINT8)(((value) & 0xff00) >> 8) ;

#define SetUINT8_BigEndian(dst,value)                   \
     ((OI_UINT8*)(dst))[0] = (OI_UINT8) ((value)) ;

/* Write values in LITTLE endian order */

#ifndef OI_CODE
#define SetUINT64_LittleEndian(dst,value)                               \
     ((OI_UINT8*)(dst))[0] = (OI_UINT8) ((value) & 0x00000000000000ffll) ;        \
     ((OI_UINT8*)(dst))[1] = (OI_UINT8)(((value) & 0x000000000000ff00ll) >> 8) ;  \
     ((OI_UINT8*)(dst))[2] = (OI_UINT8)(((value) & 0x0000000000ff0000ll) >> 16) ; \
     ((OI_UINT8*)(dst))[3] = (OI_UINT8)(((value) & 0x00000000ff000000ll) >> 24) ; \
     ((OI_UINT8*)(dst))[4] = (OI_UINT8)(((value) & 0x000000ff00000000ll) >> 32) ; \
     ((OI_UINT8*)(dst))[5] = (OI_UINT8)(((value) & 0x0000ff0000000000ll) >> 40) ; \
     ((OI_UINT8*)(dst))[6] = (OI_UINT8)(((value) & 0x00ff000000000000ll) >> 48) ; \
     ((OI_UINT8*)(dst))[7] = (OI_UINT8)(((value) & 0xff00000000000000ull) >> 56) ;
#endif /* OI_CODE */

#define SetUINT32_LittleEndian(dst,value)                               \
     ((OI_UINT8*)(dst))[0] = (OI_UINT8) ((value) & 0x000000ff) ;        \
     ((OI_UINT8*)(dst))[1] = (OI_UINT8)(((value) & 0x0000ff00) >> 8) ;  \
     ((OI_UINT8*)(dst))[2] = (OI_UINT8)(((value) & 0x00ff0000) >> 16) ; \
     ((OI_UINT8*)(dst))[3] = (OI_UINT8)(((value) & 0xff000000) >> 24) ;

#define SetUINT24_LittleEndian(dst, value)                              \
     ((OI_UINT8*)(dst))[0] = (OI_UINT8) ((value) & 0x000000ff) ;        \
     ((OI_UINT8*)(dst))[1] = (OI_UINT8)(((value) & 0x0000ff00) >> 8) ;  \
     ((OI_UINT8*)(dst))[2] = (OI_UINT8)(((value) & 0x00ff0000) >> 16) ;

#define SetUINT16_LittleEndian(dst,value)                               \
     ((OI_UINT8*)(dst))[0] = (OI_UINT8) ((value) & 0x00ff) ;            \
     ((OI_UINT8*)(dst))[1] = (OI_UINT8)(((value) & 0xff00) >> 8) ;

#define SetUINT8_LittleEndian(dst,value)                \
     ((OI_UINT8*)(dst))[0] = (OI_UINT8) ((value)) ;



/* Read values in BIG endian order */

#ifndef OI_CODE
#define GetUINT64_BigEndian(src)            \
    ((UINT64) (                          \
        ((UINT64)(((OI_UINT8 *)(src))[7])      )  |   \
        ((UINT64)(((OI_UINT8 *)(src))[6]) << 8 )  |   \
        ((UINT64)(((OI_UINT8 *)(src))[5]) << 16)  |   \
        ((UINT64)(((OI_UINT8 *)(src))[4]) << 24)  |   \
        ((UINT64)(((OI_UINT8 *)(src))[3]) << 32)  |   \
        ((UINT64)(((OI_UINT8 *)(src))[2]) << 40)  |   \
        ((UINT64)(((OI_UINT8 *)(src))[1]) << 48)  |   \
        ((UINT64)(((OI_UINT8 *)(src))[0]) << 56) ))
#endif

#define GetUINT32_BigEndian(src)            \
    ((OI_UINT32) (                          \
        ((OI_UINT32)(((OI_UINT8 *)(src))[3])      )  |   \
        ((OI_UINT32)(((OI_UINT8 *)(src))[2]) << 8 )  |   \
        ((OI_UINT32)(((OI_UINT8 *)(src))[1]) << 16)  |   \
        ((OI_UINT32)(((OI_UINT8 *)(src))[0]) << 24) ))

#define GetUINT24_BigEndian(src)            \
    ((OI_UINT32) (                          \
        ((OI_UINT32)(((OI_UINT8 *)(src))[2])      )  |   \
        ((OI_UINT32)(((OI_UINT8 *)(src))[1]) << 8 )  |   \
        ((OI_UINT32)(((OI_UINT8 *)(src))[0]) << 16) ))

#define GetUINT16_BigEndian(src)            \
    ((OI_UINT16) (                          \
        ((OI_UINT16)(((OI_UINT8 *)(src))[1])      )  |   \
        ((OI_UINT16)(((OI_UINT8 *)(src))[0]) << 8 ) ))

#define GetUINT8_BigEndian(src)             \
    (* ((OI_UINT8 *)(src)) )
             
/* Read values in LITTLE endian order */

#ifndef OI_CODE
#define GetUINT64_LittleEndian(src)            \
    ((UINT64) (                          \
        ((UINT64)(((OI_UINT8 *)(src))[0])      )  |   \
        ((UINT64)(((OI_UINT8 *)(src))[1]) << 8 )  |   \
        ((UINT64)(((OI_UINT8 *)(src))[2]) << 16)  |   \
        ((UINT64)(((OI_UINT8 *)(src))[3]) << 24)  |   \
        ((UINT64)(((OI_UINT8 *)(src))[4]) << 32)  |   \
        ((UINT64)(((OI_UINT8 *)(src))[5]) << 40)  |   \
        ((UINT64)(((OI_UINT8 *)(src))[6]) << 48)  |   \
        ((UINT64)(((OI_UINT8 *)(src))[7]) << 56) ))
#endif

#define GetUINT32_LittleEndian(src)         \
    ((OI_UINT32) (                          \
        ((OI_UINT32)(((OI_UINT8 *)(src))[0])      )  |   \
        ((OI_UINT32)(((OI_UINT8 *)(src))[1]) << 8 )  |   \
        ((OI_UINT32)(((OI_UINT8 *)(src))[2]) << 16)  |   \
        ((OI_UINT32)(((OI_UINT8 *)(src))[3]) << 24) ))

#define GetUINT24_LittleEndian(src)         \
    ((OI_UINT32) (                          \
        ((OI_UINT32)(((OI_UINT8 *)(src))[0])      )  |   \
        ((OI_UINT32)(((OI_UINT8 *)(src))[1]) << 8 )  |   \
        ((OI_UINT32)(((OI_UINT8 *)(src))[2]) << 16) ))

#define GetUINT16_LittleEndian(src)         \
    ((OI_UINT16) (                          \
        ((OI_UINT16)(((OI_UINT8 *)(src))[0])      )  |   \
        ((OI_UINT16)(((OI_UINT8 *)(src))[1]) << 8 ) ))

#define GetUINT8_LittleEndian(src)          \
    (* ((OI_UINT8 *)(src)) )


/*
 * In-place endian conversion on an array of UINT16
 */


/* Convert array from CPU byte order to BigEndian */

void SetUINT16Array_BigEndian(OI_UINT16 *data,
                              OI_UINT16 len);

/* Convert array from BigEndian to CPU byte order */

void GetUINT16Array_BigEndian(OI_UINT16 *data,
                              OI_UINT16 len);

/*
 * BD_ADDR is an array of bytes - we have defined its native type as being a BIG
 * ENDIAN array of bytes.
 */

#define GetBDADDR_BigEndian(dest, src)      \
        (dest)[0] = (src)[0] ;              \
        (dest)[1] = (src)[1] ;              \
        (dest)[2] = (src)[2] ;              \
        (dest)[3] = (src)[3] ;              \
        (dest)[4] = (src)[4] ;              \
        (dest)[5] = (src)[5] ;

#define GetBDADDR_LittleEndian(dest, src)   \
        (dest)[0] = (src)[5] ;              \
        (dest)[1] = (src)[4] ;              \
        (dest)[2] = (src)[3] ;              \
        (dest)[3] = (src)[2] ;              \
        (dest)[4] = (src)[1] ;              \
        (dest)[5] = (src)[0] ;

#define SetBDADDR_BigEndian(dest, src)      \
        (dest)[0] = (src)[0] ;              \
        (dest)[1] = (src)[1] ;              \
        (dest)[2] = (src)[2] ;              \
        (dest)[3] = (src)[3] ;              \
        (dest)[4] = (src)[4] ;              \
        (dest)[5] = (src)[5] ;

#define SetBDADDR_LittleEndian(dest, src)   \
        (dest)[0] = (src)[5] ;              \
        (dest)[1] = (src)[4] ;              \
        (dest)[2] = (src)[3] ;              \
        (dest)[3] = (src)[2] ;              \
        (dest)[4] = (src)[1] ;              \
        (dest)[5] = (src)[0] ;
/*
 * Natively, OI_UUID128 is a 32-bit integer followed by the remaining 96 bits as
 * an array of bytes in BIG ENDIAN order.
 */

#define GetUUID128_BigEndian(dest, src)     \
        (dest).ms32bits = GetUINT32_BigEndian(src)  \
        (dest).base[0]  = (src)[4];         \
        (dest).base[1]  = (src)[5];         \
        (dest).base[2]  = (src)[6];         \
        (dest).base[3]  = (src)[7];         \
        (dest).base[4]  = (src)[8];         \
        (dest).base[5]  = (src)[9];         \
        (dest).base[6]  = (src)[10];        \
        (dest).base[7]  = (src)[11];        \
        (dest).base[8]  = (src)[12];        \
        (dest).base[9]  = (src)[13];        \
        (dest).base[10] = (src)[14];        \
        (dest).base[11] = (src)[15];        \

#define GetUUID128_LittleEndian(dest, src)  \
        (dest).ms32bits = GetUINT32_LittleEndian(src + (OI_BT_UUID128_SIZE - sizeof(OI_UINT32)))  \
        (dest).base[0]  = (src)[11];        \
        (dest).base[1]  = (src)[10];        \
        (dest).base[2]  = (src)[9];         \
        (dest).base[3]  = (src)[8];         \
        (dest).base[4]  = (src)[7];         \
        (dest).base[5]  = (src)[6];         \
        (dest).base[6]  = (src)[5];         \
        (dest).base[7]  = (src)[4];         \
        (dest).base[8]  = (src)[3];         \
        (dest).base[9]  = (src)[2];         \
        (dest).base[10] = (src)[1];         \
        (dest).base[11] = (src)[0];         \

#define SetUUID128_BigEndian(dest, src)     \
        SetUINT32_BigEndian(dest, (src).ms32bits)  \
        (dest)[4]  = (src).base[0];         \
        (dest)[5]  = (src).base[1];         \
        (dest)[6]  = (src).base[2];         \
        (dest)[7]  = (src).base[3];         \
        (dest)[8]  = (src).base[4];         \
        (dest)[9]  = (src).base[5];         \
        (dest)[10] = (src).base[6];        \
        (dest)[11] = (src).base[7];        \
        (dest)[12] = (src).base[8];        \
        (dest)[13] = (src).base[9];        \
        (dest)[14] = (src).base[10];        \
        (dest)[15] = (src).base[11];        \

#define SetUUID128_LittleEndian(dest, src)  \
        SetUINT32_LittleEndian(dest + (OI_BT_UUID128_SIZE - sizeof(OI_UINT32)), src.ms32bits)  \
        (dest)[0]  = (src).base[11];        \
        (dest)[1]  = (src).base[10];        \
        (dest)[2]  = (src).base[9];         \
        (dest)[3]  = (src).base[8];         \
        (dest)[4]  = (src).base[7];         \
        (dest)[5]  = (src).base[6];         \
        (dest)[6]  = (src).base[5];         \
        (dest)[7]  = (src).base[4];         \
        (dest)[8]  = (src).base[3];         \
        (dest)[9]  = (src).base[2];         \
        (dest)[10] = (src).base[1];         \
        (dest)[11] = (src).base[0];         \

/*****************************************************************************/
#endif /* _OI_ENDIAN_H */
