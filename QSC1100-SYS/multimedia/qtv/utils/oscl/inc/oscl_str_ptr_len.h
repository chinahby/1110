#ifndef OSCL_STR_PTR_LEN_H_
#define OSCL_STR_PTR_LEN_H_
/* =======================================================================
                               oscl_str_ptr_len.h
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/oscl/main/latest/inc/oscl_str_ptr_len.h#7 $
$DateTime: 2008/05/08 14:04:55 $
$Change: 656428 $


========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
//#include <cstring>
#include <string.h>

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_types.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

// this is the XOR difference between any two ascii letters, 32
//
#define OSCL_ASCII_CASE_MAGIC_BIT 0x20

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
MACRO MYOBJ

ARGS 
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/* ======================================================================
CLASS 
  StrPtrLen

DESCRIPTION
// null-terminated pointer and string length, 
// encapsulated in one structure
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
typedef struct StrPtrLen
{
  protected:

    const char  * ptr;
    uint32        len;

    bool isLetter(const char c) const {
      return ((c >= 65 && c <= 90) ||
              (c >= 97 && c <= 122));
    }


  public:

    const char *
    getPtr() const
    {
      return ptr;
    }

    uint32
    getLen() const
    {
      return len;
    }

    void
    setPtrLen( const char * newPtr, uint32 newLen )
    {
      ptr = newPtr;
      len = newLen;
    }

    StrPtrLen( const char * newPtr )
      : ptr( newPtr )
    {
      len = strlen( newPtr );
    }

    StrPtrLen( const char * newPtr, uint32 newLen )
      : ptr( newPtr )
      , len( newLen )
    {
    }

    StrPtrLen()
      : ptr("")
      , len(0)
    {
    }

    StrPtrLen( const StrPtrLen & rhs )
      : ptr( rhs.ptr )
    , len( rhs.len )
    {
    }

    bool
    isCIEquivalentTo( const StrPtrLen & rhs ) const
    {
      if( len != rhs.len )
      {
        return false;
      }

      // since they are the same length just check for the prefix condition
      return isCIPrefixOf(rhs);
    }

    bool
    isCIPrefixOf( const StrPtrLen & rhs ) const
    {
      if( len > rhs.len )
      {
        return false;
      }

      for( uint32 ii=0; ii<len; ++ii )
      {
        if (ptr[ii] != rhs.ptr[ii]) {
          if (!isLetter(ptr[ii]) || 
              (     OSCL_ASCII_CASE_MAGIC_BIT 
                    !=  ((ptr[ii] ^ rhs.ptr[ii]) | OSCL_ASCII_CASE_MAGIC_BIT)))
            {
              return false;
            }
        }
      }

      return true;
    }



    bool
    operator==( const StrPtrLen & rhs ) const
    {
      if( len != rhs.len )
      {
        return false;
      }

      return( !strcmp( ptr, rhs.ptr ) );
    }

    bool
    operator!=( const StrPtrLen & rhs ) const
    {
      return !(*this == rhs);
    }
      
    StrPtrLen &
    operator=( const StrPtrLen & rhs )
    {
      this->ptr = rhs.ptr;
      this->len = rhs.len;
      return *this;
    }

    StrPtrLen &
    operator=( const char * rhs )
    {
      this->ptr = rhs;
      this->len = strlen( rhs );
      return *this;
    }


} StrPtrLen;

/* ======================================================================
CLASS 
  StrCSumPtrLen

DESCRIPTION
// same as StrPtrLen, but including checksum field and
// method to speed up querying
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
typedef struct StrCSumPtrLen
    : public StrPtrLen
{
  public:

    typedef int16 CheckSumType;

  protected:
    CheckSumType checkSum;

  public:

    void
    setPtrLen( const char * newPtr, uint32 newLen )
    {
      StrPtrLen::setPtrLen( newPtr, newLen );
      setCheckSum();
    }

    CheckSumType getCheckSum() const
    {
      return checkSum;
    }

    void    setCheckSum();

    StrCSumPtrLen()
      : checkSum( 0 )
    {
    }

    StrCSumPtrLen( const char * newPtr )
      : StrPtrLen( newPtr )
    {
      setCheckSum();
    }

    StrCSumPtrLen( const char * newPtr, uint32 newLen )
      : StrPtrLen( newPtr, newLen )
    {
      setCheckSum();
    }

    StrCSumPtrLen( const StrCSumPtrLen & rhs )
      : StrPtrLen( rhs )
      , checkSum( rhs.checkSum )
    {
    }

    StrCSumPtrLen( const StrPtrLen & rhs )
      : StrPtrLen( rhs )
    {
      setCheckSum();
    }


    bool
    isCIEquivalentTo( const StrCSumPtrLen & rhs ) const
    {
      if( getCheckSum() != rhs.getCheckSum() )
      {
        return false;
      }

#ifdef FEATURE_MP4_UNUSED_CODE
#error code not present
#else
      return  ((StrPtrLen*)this)->isCIEquivalentTo(rhs);
#endif /* FEATURE_MP4_UNUSED_CODE */
    }

    bool
    operator==( const StrCSumPtrLen & rhs ) const
    {
      if( getCheckSum() != rhs.getCheckSum() )
      {
        return false;
      }

#ifdef FEATURE_MP4_UNUSED_CODE
#error code not present
#else /* FEATURE_MP4_UNUSED_CODE */
      return ( (*((StrPtrLen*)this)) == (StrPtrLen)rhs );
#endif /* FEATURE_MP4_UNUSED_CODE */
    }

    bool
    operator!=( const StrCSumPtrLen & rhs ) const
    {
      return !(*this == rhs);
    }

    StrCSumPtrLen &
    operator=( const StrCSumPtrLen & rhs )
    {
      StrPtrLen::operator=( rhs );
      this->checkSum = rhs.checkSum;
      return *this;
    }

    StrCSumPtrLen &
    operator=( const StrPtrLen & rhs )
    {
      StrPtrLen::operator=( rhs );
      setCheckSum();
      return *this;
    }

    StrCSumPtrLen &
    operator=( const char * rhs )
    {
      StrPtrLen::operator=( rhs );
      setCheckSum();
      return *this;
    }

} StrCSumPtrLen;

#endif // OSCL_STR_PTR_LEN_H_

