
#ifndef SECASN1_H
#define SECASN1_H

#ifdef FEATURE_SEC_X509
/*==========================================================================

                A S W   S E C U R I T Y   S E R V I C E S
           
         S S L   A S N . 1   I N T E R F A C E   M O D U L E  

GENERAL DESCRIPTION
  This software module contains the source code for the asn.1 DER parsing
  functions

  Here a format of a simple ASN.1 document
  
  SEQUENCE {
    name          OCTECT STRING
    version       [0] INTEGER DEFAULT (0)
    valid         BOOLEAN
    storage       Storage
  }
  
  Storeage ::== SEQUENCE {
    time          UTC TIME
    data          BIT STRING
  }  
  
  So below is the code that would parse an ASN.1 document in this format
  
  #DEFINE OPTIONAL_TAG 0xA0      // Optional tag for the verision 
  
  uint8 *data_ptr;               // ASN.1 document pointer
  secasn1_data_type seq;         // Sequence data holder  
  secasn1_data_type seq2;        // Inner sequence data holder
  secasn1_data_type tmp;         // Temporary data holder
  secasn1_err_type  err;         // Error Code

  // Fields of the document
  secasn1_data_type name;
  uint8 version;
  uint32 time;
  boolean valid;
  secasn1_bit_string_type data;

  // Start the document
  if ( secasn1_start( data_ptr, &seq ) != E_ASN1_SUCCESS )
  {
    // Handle parsing error
  }

  // Parse the name field
  if ( secasn1_next_field( &seq, &name, SECASN1_OCTET_STRING_TYPE ) 
    != E_ASN1_SUCCESS )
  {
    // Handle parsing error
  }
  
  // Attempt to parse the verision number
  err = secasn1_next_field( &seq, &tmp, OPTIONAL_TAG );
  if ( err == E_ASN1_SUCCESS )
  {
    // Found a version number 
    if ( secasn1_next_field( &seq, &tmp, SECASN1_INTEGER_TYPE ) 
       != E_ASN1_SUCCESS ) 
    {
      // Handle parsing error
    }      
      
    verison = (uint8)*tmp->data;
  }  
  else if ( err = E_ASN1_INVALID_TAG ) 
  {
    // No version is specified use the default
    version = 0;
  }
  else {
    // Handle parsing error
  }

  // Parse the valid statement     
  if ( secasn1_next_field( &seq, &tmp, SECASN1_BIT_STRING_TYPE ) 
    != E_ASN1_SUCCESS )
  {
    // Handle parsing error
  }
  
  if ( secasn1_decode_boolean( &tmp, &valid ) != E_ASN1_SUCCESS ) 
  {
    // Handle parsing error
  }
  
  // Now open the sequence for the storage item
  if ( secasn1_next_field( &seq, &seq2, SECASN1_SEQUENCE_TYPE ) 
    != E_ASN1_SUCCESS )
  {
    // Handle parsing error
  }

  // Parse the time statement
  if ( secasn1_next_field( &seq2, &tmp, SECASN1_UTC_TYPE ) 
    != E_ASN1_SUCCESS )
  {
    // Handle parsing error
  }
  
  if ( secasn1_decode_time( &tmp, &time, TRUE ) != E_ASN1_SUCCESS )
  {
    // Handle parsing error
  }
  
  // Parse the data statement
  if ( secasn1_next_field( &seq2, &tmp, SECASN1_BIT_STRING_TYPE ) 
    != E_ASN1_SUCCESS )
  {
    // Handle parsing error
  }
  
  if ( secasn1_decode_bit_string( &tmp, &data, TRUE ) != E_ASN1_SUCCESS )
  {
    // Handle parsing error
  }
  
  // Close the inner sequence
  if ( secasn1_close_sequence( &seq, &seq2 ) != E_ASN1_SUCCESS )
  {
    // Handle parsing error
  }
  
  // Close out the document
  if ( secasn1_end( &seq ) != E_ASN1_SUCCESS )
  {
    // Hanldle parsing error
  }
  
  // And we are finished
  

EXTERNALIZED FUNCTIONS

  secasn1_start()                Starts an ASN.1 document for parsing

  secasn1_end()                  Closes an ASN.1 document
  
  secasn1_close_sequence()       Closes a sequence in an ASN.1 document
  
  secasn1_next_field()           Parses the next field in an ASN.1 document

  secasn1_decode_bit_string()    Decodes a bit string from a generic 
                                 ASN.1 field
  
  secasn1_decode_boolean()       Decodes a boolean value from a generic 
                                 ASN.1 field
  
  secasn1_decode_time()          Decode a time value from a generic
                                 ASN.1 field

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
==========================================================================*/
/*==========================================================================
          EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/secasn1.h#1 $
  $DateTime: 2007/06/26 13:17:46 $ $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/06   lx      Add UTF8String support
01/24/03   Jay     Initial version

==========================================================================*/


/*==========================================================================

             Include Files for Module 

==========================================================================*/
#include "comdef.h"
/*--------------------------------------------------------------------------
 ASN.1 parsing for Distinguished Encoding Rules (DER)
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                ASN1 Constants and definitions
--------------------------------------------------------------------------*/
#define SECASN1_NO_TYPE_CHECK         (0x00)
#define SECASN1_BOOLEAN_TYPE          (0x01)
#define SECASN1_INTEGER_TYPE          (0x02)
#define SECASN1_BIT_STRING_TYPE       (0x03)
#define SECASN1_OCTET_STRING_TYPE     (0x04)
#define SECASN1_NULL_TYPE             (0x05)
#define SECASN1_OID_TYPE              (0x06)
#define SECASN1_UTF8_STRING_TYPE      (0x0c)
#define SECASN1_SEQUENCE_TYPE         (0x10)
#define SECASN1_SET_TYPE              (0x11)
#define SECASN1_PRINTABLE_STRING_TYPE (0x13)
#define SECASN1_TELETEX_STRING_TYPE   (0x14)
#define SECASN1_UTC_TYPE              (0x17)

/*--------------------------------------------------------------------------
                Modular Constants and definitions
--------------------------------------------------------------------------*/

/* ASN.1 Error Codes */
typedef enum
{
  E_ASN1_SUCCESS = 0,
  E_ASN1_INVALID_TAG,
  E_ASN1_NO_DATA,
  E_ASN1_INVALID_DATA,
  E_ASN1_INVALID_ARG
} secasn1_err_type;

/* ASN.1 data holder */
typedef struct
{
  uint8 *data;
  uint16 len;
} secasn1_data_type;

/* ASN.1 bit string data holder */
typedef struct
{
  uint8 *data;
  uint16 len;
  uint8 unused;
} secasn1_bit_string_type;

/*--------------------------------------------------------------------------
                Function Declarations 
--------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION SECASN1_START

DESCRIPTION
  Starts an ASN.1 DER encoding by creating an initial container for the
  next_field() function

DEPENDENCIES
  None
  
PARAMETERS
  data    - pointer to the data
  ret_ptr - pointer to the returned data to start the ASN.1 block

RETURN VALUE
  E_ASN1_SUCCESS - if the sequence is properly started
  E_ASN1_INVALID_ARG - if the pointer arguments have a NULL value
  E_ASN1_INVALID_TAG - if it is not a sequence tag at the beginning

SIDE EFFECTS
  None  
===========================================================================*/
extern secasn1_err_type secasn1_start
(
  uint8 *data,
  secasn1_data_type *ret_ptr
);

/*===========================================================================

FUNCTION SECASN1_END

DESCRIPTION
  closes and verifies an ASN.1 DER encoding, by checking that no data
  has been left unprocessed at the end of the stream

DEPENDENCIES
  None
  
PARAMETERS
  data_ptr - pointer to the end of the data holder

RETURN VALUE
  E_ASN1_SUCCESS - if the document has been closed successfully
  E_ASN1_INVALID_ARG - if the pointer argument has a NULL value
  E_ASN1_INVALID_DATA - all the data was not processed
  
SIDE EFFECTS
  None  
===========================================================================*/
extern secasn1_err_type secasn1_end
(
  secasn1_data_type *data_ptr
);

/*===========================================================================

FUNCTION SECASN1_CLOSE_SEQUENCE

DESCRIPTION
  This function verifies that all the data in the inner sequence has
  been processed.  The outer sequence is included in the case that 
  a sequence has an undefined length so the "0x00, 0x00" end marker
  can be removed.
  
DEPENDENCIES
  secasn1_next_field() to start a sequence value has been called
  
PARAMETERS
  outer - pointer to the outer sequence
  inner - pointer to the inner sequence

RETURN VALUE
  E_ASN1_SUCCESS - if the sequence was successful closed
  E_ASN1_INVALID_ARG - if a pointer argument has a NULL value
  E_ASN1_INVALID_DATA - otherwise
  
SIDE EFFECTS
  None  
===========================================================================*/
extern secasn1_err_type secasn1_close_sequence
(
  secasn1_data_type *outer,
  secasn1_data_type *inner
);

/*===========================================================================

FUNCTION SECASN1_NEXT_FIELD

DESCRIPTION
  This function parses the next field of data in the ASN.1 DER encoding.
  The function points the secasn1_data_type at the data contained within
  the field.  ret_ptr must be allocated before it is passed into the
  function or the error E_ASN1_INVALID_ARG will be returned.  If 
  verify_tag_id != 0, then the function verifies the tag_id against the
  current fields tags.  If the tag_ids do not match an error code of 
  E_ASN1_INVALID_TAG is returned.

DEPENDENCIES
  None
  
PARAMETERS
  data_ptr - pointer to the data to be parsed
  ret_ptr  - where the inner field data is returned
  verify_tag_id - tag value to check the tag against

RETURN VALUE
  If successful, E_ASN1_SUCCESS will be returned and ret_ptr will be updated
  to contain the data in the field.  If an error occurs during the parsing
  the appropriate error will be returned.  On an error the data_ptr is 
  returned to the initial position
  
SIDE EFFECTS
  None  
===========================================================================*/
extern secasn1_err_type secasn1_next_field
(
  secasn1_data_type *data_ptr,
  secasn1_data_type *ret_ptr,
  uint8 verify_tag_id
);

/*===========================================================================

FUNCTION SECASN1_DECODE_BIT_STRING

DESCRIPTION
  This function takes a data pointer and decodes it to a bit_string pointer
  In the ASN.1 DER encoding, the first byte of a byte string indicates, how
  many unused bytes are at the end of the string.  Assumes that the bit_ptr
  has been pre-allocated.

DEPENDENCIES
  None
  
PARAMETERS
  data_ptr - pointer to the bit string field
  bit_ptr  - where the decoded bit string will be returned


RETURN VALUE
  If successful, E_ASN1_SUCCESS will be returned and bit_ptr will be updated
  to contain the bit string.  If an error occurs during the conversion
  the appropriate error will be returned.

SIDE EFFECTS
  None  
===========================================================================*/
extern secasn1_err_type secasn1_decode_bit_string
(
  secasn1_data_type *data_ptr,
  secasn1_bit_string_type *bit_ptr
);

/*===========================================================================

FUNCTION SECASN1_DECODE_BOOLEAN

DESCRIPTION
  This function takes a data pointer and decodes it to a boolean pointer
  In the ASN.1 DER encoding, the boolean data should only be a single byte,
  with 0x00 meaning FALSE and anything else being true

DEPENDENCIES
  None
  
PARAMETERS
  data_ptr - pointer to the boolean field
  b_ptr - pointer where the boolean value will be returned

RETURN VALUE
  If successful, E_ASN1_SUCCESS will be returned and the boolean pointer will
  will be set.  If the data_ptr contains more than a single byte
  E_ASN1_INVALID_DATA will be returned
  
SIDE EFFECTS
  None  
===========================================================================*/
extern secasn1_err_type secasn1_decode_boolean
(
  secasn1_data_type *data_ptr,
  boolean *b_val
);

/*===========================================================================

FUNCTION SECASN1_DECODE_TIME

DESCRIPTION
  This function takes a data pointer representing time and decodes it to 
  the number of seconds since CDMA epoch time of Jan 6th, 1980.  If the time 
  occurs before this date, the time is adjusted to 0.

DEPENDENCIES
  None
  
PARAMETERS
  data_ptr - pointer to the time data field
  time     - pointer where the time in seconds is returned
  utc_time - whether the time is in UTC time format

RETURN VALUE
  If successful, E_ASN1_SUCCESS will be returned and the time pointer will
  will be set.  E_ASN1_INVALID_DATA is returned if an invalid format is 
  encountered.
  
SIDE EFFECTS
  None  
===========================================================================*/
extern secasn1_err_type secasn1_decode_time
(
  secasn1_data_type *data_ptr,
  uint32  *time,
  boolean utc_time
);

#endif /* FEATURE_SEC_X509 */

#endif /* SECASN1_H */
