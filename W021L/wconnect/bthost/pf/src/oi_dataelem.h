#ifndef _OI_DATAELEM_H
#define _OI_DATAELEM_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_dataelem.h#1 $ 

$Log: $
*
*    #4        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
===========================================================================*/
/**
 * @file  
 * 
 * This file contains definitions and functions for working with Data Elements
 * as defined in the SDP specification.
 * 
 * The Service Discovery Protocol (SDP) specification is Part E of the
 * Specification of the Bluetooth System: Core. Data Elements are described in
 * Section 3 of that document.
 *
 * Additional information is available in the @ref SDP_docpage section of the
 * BLUEmagic 3.0 SDK documentation.
 * 
 */

/** \addtogroup Marshaller Marshaller APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_status.h"
#include "oi_bt_spec.h"

/**
 * This enumeration is for Data Element type descriptors as defined in Section
 * 3.2 of the SDP specification.
 *
 * The Service Discovery Protocol uses the Data Element header field to describe
 * the type and length of the data in the Data Element's data field. The Data
 * Element header is 1, 2, 3, or 5 bytes in length.
 * 
 * The first byte of the Data Element header is the "Data Element descriptor".
 * The Data Element descriptor is composed of a 5-bit Data Element type
 * descriptor (which indicates the type of the data in the Data Element's data
 * field) followed by a 3-bit Data Element size descriptor index (which
 * indicates the size of the Data Element's data field). The three bits of the
 * size descriptor index are the least significant three bits of the one-byte
 * Data Element descriptor.
 *
 * Data Element types #OI_DATAELEM_REF, #OI_DATAELEM_BDADDR,
 * #OI_DATAELEM_RAWBYTES, #OI_DATAELEM_STATUS, and #OI_DATAELEM_TEXT16 are
 * specific and internal to implementations of the BLUEmagic 3.0 protocol stack. 
 */

enum OI_DATAELEM_TYPES {
    OI_DATAELEM_NULL      =  0,  /**< null data */
    OI_DATAELEM_UINT      =  1,  /**< unsigned integer data */
    OI_DATAELEM_SINT      =  2,  /**< signed integer data */
    OI_DATAELEM_UUID      =  3,  /**< UUID (universally unique identifier) data */
    OI_DATAELEM_TEXT      =  4,  /**< text string data */
    OI_DATAELEM_BOOL      =  5,  /**< Boolean data */
    OI_DATAELEM_SEQ       =  6,  /**< a sequence of Data Elements */
    OI_DATAELEM_ALT       =  7,  /**< Data Element alternative (a sequence of Data Elements from which one Data Element is to be selected) */
    OI_DATAELEM_URL       =  8,  /**< URL (uniform resource locator) data */
    
    /*
     * Extended data element types used internally by BLUEmagic 3.0
     */

    OI_DATAELEM_REF       = 16,  /**< used for dynamic attributes in a service record */
    OI_DATAELEM_BDADDR    = 17,  /**< Bluetooth address data (used by BHAPI) */
    OI_DATAELEM_RAWBYTES  = 18,  /**< raw byte data (used by BHAPI)*/
    OI_DATAELEM_STATUS    = 19,  /**< error status value (used by BHAPI) */
    OI_DATAELEM_TEXT16    = 20   /**< wide (16 bit) text string type (used by BHAPI) */
};


/**
 * This enumeration is for Data Element size descriptor indices, which are 3-bit
 * indices indicating Data Element size as defined in Section 3.3 of the
 * Service Discovery Protocol specification.
 */

enum OI_DATAELEM_SIZES {
    OI_DATAELEM8         =   0, /**< Data Element data field is one byte long; unless Data Element type is null, in which case data field is zero bytes. */
    OI_DATAELEM16        =   1, /**< Data Element data field is two bytes long. */
    OI_DATAELEM32        =   2, /**< Data Element data field is four bytes long. */
    OI_DATAELEM64        =   3, /**< Data Element data field is eight bytes long. */
    OI_DATAELEM128       =   4, /**< Data Element data field is sixteen bytes long. */
    OI_DATAELEM_VAR8     =   5, /**< Data Element data field length is given by the following one-byte value. */
    OI_DATAELEM_VAR16    =   6, /**< Data Element data field length is given by the following two-byte value. */
    OI_DATAELEM_VAR32    =   7  /**< Data Element data field length is given by the following four-byte value. */
};

/**
 * @name Data Element utility macros
 * @{
 */


/**
 * This macro extracts the Data Element type descriptor (top 5 bits) from the
 * network representation of a Data Element descriptor.
 */

#define OI_DATAELEM_TYPE(e)            ((OI_UINT8) (((e) & 0xF8) >> 3))

/**
 * This macro extracts the Data Element size descriptor index (bottom 3 bits)
 * from the network representation of a Data Element descriptor.
 */

#define OI_DATAELEM_SIZE_INDEX(e)      ((OI_UINT8) ((e) & 0x07))

/**
 * This macro composes the network representation of a Data Element descriptor
 * from a Data Element type descriptor and a Data Element size descriptor.
 */

#define OI_DATAELEM_DESCRIPTOR(t, s)   ((OI_UINT8) (((t) << 3) | (s)))


/**
 * the size of a VAR8 Data Element header
 */

#define OI_DATAELEM_VAR8_SIZE     (sizeof(OI_UINT8) + sizeof(OI_UINT8))

/**
 * the size of a VAR16 Data Element header
 */

#define OI_DATAELEM_VAR16_SIZE    (sizeof(OI_UINT8) + sizeof(OI_UINT16))

/**
 * the size of a VAR32 Data Element header
 */

#define OI_DATAELEM_VAR32_SIZE    (sizeof(OI_UINT8) + sizeof(OI_UINT32))


/**
 * This macro tests whether a Data Element is one of the Data Element list types.
 */

#define OI_DATAELEM_IS_LIST(e) \
    (((e)->ElemType == OI_DATAELEM_SEQ) || ((e)->ElemType == OI_DATAELEM_ALT))

/**
 * This macro tests whether a Data Element is one of the Data Element text types.
 */

#define OI_DATAELEM_IS_TEXT(e) \
    (((e)->ElemType == OI_DATAELEM_TEXT) || ((e)->ElemType == OI_DATAELEM_URL))


/**@}*/

/**
 * The choice of unsigned integer for this type of union prevents sign extension
 * on integer conversion in static initializers.
 */
#ifndef OI_CODE
typedef void* OI_ELEMENT_UNION;
#endif

typedef struct _OI_DATAELEM OI_DATAELEM;    /**< Data Elements are stored as structures. */


/**
 * This union represents all possible types for the data in a Data Element's data field.
 */

typedef union {
    OI_ELEMENT_UNION ElementUnion; /**< Data Element union */
    OI_BOOL      Boolean;       /**< Boolean type */
    OI_INT32     SInt;          /**< signed 32-bit integer used for 8-bit, 16-bit, and 32-bit integers */
    OI_UINT32    UInt;          /**< unsigned 32-bit integer  used for 8-bit, 16-bit, and 32-bit unsigned integers*/
    OI_INT64     *SInt64;       /**< pointer to a signed 64-bit integer */
    OI_UINT64    *UInt64;       /**< pointer to an unsigned 64-bit integer */
    OI_INT128    *SInt128;      /**< pointer to a signed 128-bit integer */
    OI_UINT128   *UInt128;      /**< pointer to an unsigned 128-bit integer */
    OI_CHAR      *Text;         /**< pointer to text as a string of characters */
    OI_CHAR      *URL;          /**< pointer to a URL as a string of characters */
    OI_UUID32    ShortUUID;     /**< 32-bit representation of a UUID used for 16-bit and 32-bit UUIDs */
    OI_UUID128   *LongUUID;     /**< pointer to a 128-bit UUID */
    OI_DATAELEM  *ElemSeq;      /**< pointer to a sequence of Data Elements */
    OI_DATAELEM  *ElemAlt;      /**< pointer to a sequence of Data Elements from which one is to be selected */
    OI_BD_ADDR   *BDAddr;       /**< pointer to a 48-bit Bluetooth device address */
    OI_BYTE      *RawBytes;     /**< pointer to raw bytes */
    OI_STATUS    Status;        /**< a status code */
    OI_DATAELEM  *ElemRef;      /**< pointer to a Data Element of type used for dynamic attributes in a service record */
    OI_CHAR16    *Text16;       /**< pointer to text as a string of double-byte characters */
    OI_DATAELEM  const *__Elem; /**< !!! for Internal use only */
    OI_CHAR      const *__Text;
    OI_CHAR16    const *__Text16;
    OI_BYTE      const *__RawBytes;
} OI_DATAELEM_UNION;


/**
 * This structure defines the in-memory representation of a Data Element.
 */
struct _OI_DATAELEM {
    /**
     * This is one of the OI_DATAELEM_TYPES values.
     */
    OI_UINT8 ElemType;
    /**
     * For primitive types, this is the length of the Data Element data field in
     * bytes. For lists (OI_DATAELEM_ALT, OI_DATAELEM_SEQ) this is the number of
     * Data Elements in the list. For OI_DATAELEM_TEXT16 it is the number or
     * double-byte (OI_CHAR16) characters in the string.
     */
    OI_UINT16 Size; 
    /**
     * The Data Element's data field value is of type OI_DATAELEM_UNION, which
     * is a union of all possible Data Element types. Data field values that are
     * larger than four bytes are referenced via a pointer.
     */
    OI_DATAELEM_UNION Value;
};



/**
 * @note
 * The OI_ELEMENT_VAL macro accomodates different behavior of compilers needed
 * because the third item in each of the Data Element initialization macros is a
 * union and compilers differ in their brace requirements for union
 * initialization. GCC requires braces; some earlier Hitachi compileres require
 * no braces; and Microsoft Visual C/C++ compiler accepts unions either way.
 * The requirement of no braces is considered here to be the special case.
 */

#ifdef HITACHI_UNIONS_BROKEN
#error code not present
#else
#define OI_ELEMENT_VAL(v)    { ((OI_ELEMENT_UNION) (v)) }     
#endif

/**
 * @name Macros for static initialization of Data Elements
 * 
 * These macros are for constructing statically initialized Data Elements.
 *
 * When statically initializing a union, only the first item in the union is
 * initialized. Therefore, all initialization values have been cast to
 * OI_ELEMENT_UNION defined as an unsigned 32-bit integer so that sign extension
 * does not occur by default. As described above, OI_ELEMENT_VAL() is a macro
 * that deals with platform-specific union behavior and casts data items to the
 * union OI_ELEMENT_UNION.
 *
 * @note that OI_ELEMENT_STRING and OI_ELEMENT_URL are expecting a null
 * terminated string e.g. OI_ELEMENT_STRING("This is a string") and they
 * assume (and remove) the terminating null. Use OI_ELEMENT_TEXT if the data
 * is to be initialized exactly as specified.
 *
 * @{
 */

#define OI_ELEMENT_INT8(i)    { OI_DATAELEM_SINT, sizeof(OI_INT8),          OI_ELEMENT_VAL((OI_INT32)(i)) }             /**< This macro statically initializes an 8-bit signed integer Data Element. */ 
#define OI_ELEMENT_INT16(i)   { OI_DATAELEM_SINT, sizeof(OI_INT16),         OI_ELEMENT_VAL((OI_INT32)(i)) }             /**< This macro statically initializes a 16-bit signed integer Data Element. */ 
#define OI_ELEMENT_INT32(i)   { OI_DATAELEM_SINT, sizeof(OI_INT32),         OI_ELEMENT_VAL((OI_INT32)(i)) }             /**< This macro statically initializes a 32-bit signed integer Data Element. */ 
#define OI_ELEMENT_UINT8(i)   { OI_DATAELEM_UINT, sizeof(OI_UINT8),         OI_ELEMENT_VAL((OI_UINT32)(i)) }            /**< This macro statically initializes an 8-bit unsigned integer Data Element. */ 
#define OI_ELEMENT_UINT16(i)  { OI_DATAELEM_UINT, sizeof(OI_UINT16),        OI_ELEMENT_VAL((OI_UINT32)(i)) }            /**< This macro statically initializes a 16-bit unsigned integer Data Element. */
#define OI_ELEMENT_UINT32(i)  { OI_DATAELEM_UINT, sizeof(OI_UINT32),        OI_ELEMENT_VAL((OI_UINT32)(i)) }            /**< This macro statically initializes a 32-bit unsigned integer Data Element. */
#define OI_ELEMENT_BOOL(b)    { OI_DATAELEM_BOOL, sizeof(OI_UINT8),         OI_ELEMENT_VAL((OI_UINT32)(b)) }            /**< This macro statically initializes a boolean Data Element. */

#define OI_ELEMENT_UINT64(i)  { OI_DATAELEM_UINT, sizeof(OI_UINT64),        OI_ELEMENT_VAL(i) }                         /**< This macro statically initializes a pointer to a 64-bit unsigned integer Data Element */
#define OI_ELEMENT_UINT128(i) { OI_DATAELEM_UINT, sizeof(OI_UINT128),       OI_ELEMENT_VAL(i) }                         /**< This macro statically initializes a pointer to a 128-bit unsigned integer Data Element */
#define OI_ELEMENT_INT64(i)   { OI_DATAELEM_SINT, sizeof(OI_INT64),         OI_ELEMENT_VAL(i) }                         /**< This macro statically initializes a pointer to a 64-bit signed integer Data Element */
#define OI_ELEMENT_INT128(i)  { OI_DATAELEM_SINT, sizeof(OI_INT128),        OI_ELEMENT_VAL(i) }                         /**< This macro statically initializes a pointer to a 128-bit signed integer Data Element */

#define OI_ELEMENT_UUID16(u)  { OI_DATAELEM_UUID, sizeof(OI_UUID16),        OI_ELEMENT_VAL(u) }                         /**< This macro statically initializes a 16-bit UUID Data Element. */ 
#define OI_ELEMENT_UUID32(u)  { OI_DATAELEM_UUID, sizeof(OI_UUID32),        OI_ELEMENT_VAL(u) }                         /**< This macro statically initializes a 32-bit UUID Data Element. */ 
#define OI_ELEMENT_UUID128(u) { OI_DATAELEM_UUID, sizeof(OI_UUID128),       OI_ELEMENT_VAL(u) }                         /**< This macro statically initializes a pointer to a 128-bit UUID Data Element. */ 

#define OI_ELEMENT_NULL       { OI_DATAELEM_NULL, 0,                        OI_ELEMENT_VAL(0) }                         /**< This macro statically initializes a null Data Element. */ 

#define OI_ELEMENT_STRING(t)  { OI_DATAELEM_TEXT, (OI_UINT16) (OI_ARRAYSIZE(t) - 1), OI_ELEMENT_VAL(((OI_CHAR*) (t))) } /**< This macro statically initializes a pointer to a text string Data Element (removes trailing 0) */          
#define OI_ELEMENT_URL(u)     { OI_DATAELEM_URL,  (OI_UINT16) (OI_ARRAYSIZE(u) - 1), OI_ELEMENT_VAL(((OI_CHAR*) (u))) } /**< This macro statically initializes a pointer to a URL text string Data Element (removes trailing 0) */
#define OI_ELEMENT_TEXT(t)    { OI_DATAELEM_TEXT, (OI_UINT16) OI_ARRAYSIZE(t),       OI_ELEMENT_VAL(((OI_CHAR*) (t))) } /**< This macro statically initializes a pointer to a TEXT Data Element */          
#define OI_ELEMENT_SEQ(s)     { OI_DATAELEM_SEQ,  (OI_UINT16) OI_ARRAYSIZE(s),       OI_ELEMENT_VAL((&(s)[0])) }        /**< This macro statically initializes a pointer to a sequence of Data Elements */
#define OI_ELEMENT_ALT(s)     { OI_DATAELEM_ALT,  (OI_UINT16) OI_ARRAYSIZE(s),       OI_ELEMENT_VAL((&(s)[0])) }        /**< This macro statically initializes a pointer to a sequence of alternative Data Elements */


#define OI_ELEMENT_REF(e)     { OI_DATAELEM_REF,  0,                        OI_ELEMENT_VAL(&(e)) }                      /**< This macro statically initializes a pointer to a Data Element (used for dynamic attributes in a service record) */

/** @}*/

/**
 * @name Macros for run-time assignment of Data Elements
 * 
 * These macros are for run-time assignment of Data Elements.
 *
 * @{
 */

/**
 * This macro sets a null Data Element.
 *
 * @param elem   the null Data Element to be set
 */

#define OI_SET_NULL_ELEMENT(elem) \
{ \
    (elem).ElemType = OI_DATAELEM_NULL; \
}

/**
 * This macro sets a Data Element of signed integer type.
 *
 * @param elem   the Data Element of signed integer type to be set
 *
 * @param value  a signed integer value, based on which the Data Element's size and value are set
 */

#define OI_SET_INT_ELEMENT(elem, value) \
{ \
    (elem).ElemType = OI_DATAELEM_SINT; \
    (elem).Size = sizeof(value); \
    (elem).Value.SInt = (OI_INT32) (value); \
}

/**
 * This macro sets a Data Element of unsigned integer type.
 *
 * @param elem    the Data Element of unsigned integer type to be set
 *
 * @param value   an unsigned integer value, based on which the Data Element's size and value are set
 */

#define OI_SET_UINT_ELEMENT(elem, value) \
{ \
    (elem).ElemType = OI_DATAELEM_UINT; \
    (elem).Size = sizeof(value); \
    (elem).Value.UInt = (OI_UINT32) (value); \
}


/**
 * This macro sets a Data Element of UUID16 type. 
 *
 * Note that UUID16 values are stored as 32 bits.
 *
 * @param elem    the Data Element to be set
 *
 * @param value   a UUID16 value.
 */

#define OI_SET_UUID16_ELEMENT(elem, value) \
{ \
    (elem).ElemType = OI_DATAELEM_UUID; \
    (elem).Size = sizeof(OI_UUID16); \
    (elem).Value.ShortUUID = (OI_UUID32) (value); \
}


/**
 * This macro sets a Data Element of UUID32 type.
 *
 * @param elem    the Data Element to be set
 *
 * @param value   a UUID32 value.
 */

#define OI_SET_UUID32_ELEMENT(elem, value) \
{ \
    (elem).ElemType = OI_DATAELEM_UUID; \
    (elem).Size = sizeof(OI_UUID32); \
    (elem).Value.ShortUUID = (OI_UUID32) (value); \
}

/**
 * This macro sets a Data Element of UUID128 type.
 *
 * @param elem    the Data Element to be set
 *
 * @param value   a UUID128 l-value.
 */

#define OI_SET_UUID128_ELEMENT(elem, value) \
{ \
    (elem).ElemType = OI_DATAELEM_UUID; \
    (elem).Size = OI_BT_UUID128_SIZE; \
    (elem).Value.LongUUID = (value); \
}


/**
 * This macro sets a Data Element of status code type.
 *
 * @param elem   the Data Element of status code type to be set
 *
 * @param value  the status code value
 */

#define OI_SET_STATUS_ELEMENT(elem, value) \
{ \
    (elem).ElemType = OI_DATAELEM_STATUS; \
    (elem).Size = sizeof(OI_UINT16); \
    (elem).Value.Status = (value); \
}

/**
 * This macro sets a Data Element of Boolean (BOOL) type.
 *
 * @param elem   the Data Element of Boolean type to be set
 *
 * @param value  the Boolean (TRUE/FALSE) value
 */

#define OI_SET_BOOL_ELEMENT(elem, value) \
{ \
    (elem).ElemType = OI_DATAELEM_BOOL; \
    (elem).Size = sizeof(OI_UINT8); \
    (elem).Value.Boolean = (OI_UINT8) (value); \
}

/**
 * This macro sets a Data Element of list type.
 *
 * @param elem   the Data Element of list type to be set
 *
 * @param type   one of the list Data Element types (OI_DATAELEM_SEQ, OI_DATAELEM_ALT)
 *
 * @param list   pointer to the first Data Element in the body of the list
 *
 * @param size   the number of elements in the body of the list
 */

#define OI_SET_LIST_ELEMENT(elem, type, list, size) \
{ \
    (elem).ElemType = type; \
    (elem).Size = size; \
    (elem).Value.__Elem = &(list); \
}


/**
 * This macro sets a Data Element of list type that has no data elements.
 *
 * @param elem   the Data Element of list type to be set
 *
 * @param type   one of the list Data Element types (OI_DATAELEM_SEQ, OI_DATAELEM_ALT)
 */

#define OI_SET_EMPTY_LIST_ELEMENT(elem, type) \
{ \
    (elem).ElemType = type; \
    (elem).Size = 0; \
    (elem).Value.__Elem = NULL; \
}

/**
 * This macro sets a Data Element of sequence type by applying the OI_SET_LIST_ELEMENT macro.
 *
 * @param elem   the Data Element of sequence type to be set
 *
 * @param list   the first Data Element in the body of the list
 */

#define OI_SET_SEQ_ELEMENT(elem, list) OI_SET_LIST_ELEMENT(elem, OI_DATAELEM_SEQ, list[0], OI_ARRAYSIZE(list))

/**
 * This macro sets a Data Element of alternative sequence type by applying the OI_SET_LIST_ELEMENT macro.
 *
 * @param elem   the Data Element of alternative sequence type to be set
 *
 * @param list   the first Data Element in the body of the list
 */


#define OI_SET_ALT_ELEMENT(elem, list) OI_SET_LIST_ELEMENT(elem, OI_DATAELEM_ALT, list[0], OI_ARRAYSIZE(list))


/**
 * This macro sets a Data Element of text string type.
 *
 * @param elem   the Data Element of text string type to be set
 * 
 * @param addr   a pointer to the text string
 *
 * @param len    the length of the text string
 */
#define OI_SET_TEXT_ELEMENT(elem, addr, len) \
{ \
    (elem).ElemType = OI_DATAELEM_TEXT; \
    (elem).Size = (len); \
    (elem).Value.__Text = (addr); \
}


/**
 * This macro sets a Data Element of double-byte text string type.
 *
 * @param elem   the Data Element of text string type to be set
 * 
 * @param addr   a pointer to the text string
 *
 * @param len    the number of characters in the text string
 */
#define OI_SET_TEXT16_ELEMENT(elem, addr, len) \
{ \
    (elem).ElemType = OI_DATAELEM_TEXT16; \
    (elem).Size = (len); \
    (elem).Value.__Text16 = (addr); \
}

/**
 * This macro sets a Data Element of BDADDR (Bluetooth device address) type. Bluetooth device addresses
 * are 48-bit quantities and therefore must be passed by reference.
 *
 * @param elem   the Data Element of BDADDR type to be set
 *
 * @param addr   a pointer to the BDADDR value
 */

#define OI_SET_BDADDR_ELEMENT(elem, addr) \
{ \
    (elem).ElemType = OI_DATAELEM_BDADDR; \
    (elem).Size = OI_BD_ADDR_BYTE_SIZE; \
    (elem).Value.BDAddr = (addr); \
}

/**
 * This macro sets a raw bytes Data Element
 *
 * @param elem   the Data Element to be set
 * 
 * @param addr   a pointer to a buffer
 *
 * @param len    the length of buffer
 */
#define OI_SET_RAWBYTES_ELEMENT(elem, addr, len) \
{ \
    (elem).ElemType = OI_DATAELEM_RAWBYTES; \
    (elem).Size = (len); \
    (elem).Value.__RawBytes = (addr); \
}

/**
 * This macro sets a 64-bit unsigned integer Data Element
 *
 * @param elem  the Data Element to be set
 * 
 * @param value a OI_UINT64 struct
 */
#define OI_SET_UINT64_ELEMENT(elem, value) \
{ \
    (elem).ElemType = OI_DATAELEM_UINT; \
    (elem).Size = sizeof(OI_UINT64); \
    (elem).Value.UInt64 = &(value); \
}

/**
 * This macro sets a 128-bit unsigned integer Data Element
 *
 * @param elem  the Data Element to be set
 * 
 * @param value a OI_UINT128 struct
 */
#define OI_SET_UINT128_ELEMENT(elem, value) \
{ \
    (elem).ElemType = OI_DATAELEM_UINT; \
    (elem).Size = sizeof(OI_UINT128); \
    (elem).Value.UInt128 = &(value); \
}


/** @}*/

/**
 * This function determines whether two UUID Data Element values represent the same UUID.
 *
 * UUIDs are 128-bit values, but the SDP specification defines 16-, and 32-bit
 * shorthand formats and a standard way of converting from the short forms to
 * the full 128-bit UUID.
 *
 * This function handles all permutations of 16-, 32-, and 128-bit representations.
 *
 * @param A            UUID to be compared
 *
 * @param B            UUID to be compared
 *
 * @returns TRUE if A and B match, FALSE if 
  - A and B are not UUID data or
  - A and B do not match.
 */

OI_BOOL OI_DATAELEM_SameUUID(const OI_DATAELEM *A,
                             const OI_DATAELEM *B);


/**
 * Returns the UUID32 representation of a generic UUID. If there is no 32 bit
 * representation returns 0.
 *
 * @param U    A data element UUID.
 *
 * @returns    A 32 bit UUID.
 */

OI_UUID32 OI_DATAELEM_ConvertToUUID32(const OI_DATAELEM *U);

/**
 * Returns the UUID128 representation of a generic UUID. 
 *
 * @param uuid128    The 128 bit uuid representation of the uuid described by
 *                   the data element.
 *
 * @param U          A data element UUID.
 *
 *
 * @returns          OI_OK if the data element was a UUID
 */

OI_STATUS OI_DATAELEM_ConvertToUUID128(OI_UUID128 *uuid128,
                                       const OI_DATAELEM *U);


/** function for comparing a 32-bit UUID to a 128-bit UUID */
OI_BOOL OI_ShortUUID_eq_LongUUID(OI_UUID32 shortUUID,
                                 OI_UUID128 *longUUID);

#endif /* _OI_DATAELEM_H */

/**@}*/
