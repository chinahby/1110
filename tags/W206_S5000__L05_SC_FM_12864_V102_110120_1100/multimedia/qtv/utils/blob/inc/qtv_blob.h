#ifndef HEADER_H
#define HEADER_H
/************************************************************************* */
/**
 * @file
 * @brief Definition of the 'blob' utility class.
 * 
 * Copyright 2007 QUALCOMM Incorporated, All Rights Reserved.
 *
 ************************************************************************* */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/blob/main/latest/inc/qtv_blob.h#1 $
$DateTime: 2008/05/08 11:03:24 $
$Change: 656211 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* =======================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "comdef.h"
#include "rex.h"

/* =======================================================================

                        DATA DECLARATIONS

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_QTV_DEBUG_ONLY
  #define QTV_BLOB_UNIT_TESTS
#endif

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
**                        Macro Definitions
** ======================================================================= */

/* =======================================================================
**                        Class & Function Declarations
** ======================================================================= */

/************************************************************************* */
/**
 * Blobs are variable-sized chunks of binary data.
 *
 ************************************************************************* */
class qtv_blob_class
{
public:
  qtv_blob_class( void );
  qtv_blob_class( uint32 data_size );
  qtv_blob_class( const uint8* data_ptr, uint32 data_size );
  qtv_blob_class( const qtv_blob_class& );

  virtual ~qtv_blob_class( void );

  qtv_blob_class& operator=( const qtv_blob_class& );

  uint32 size( void ) const;
  uint8* data( void ) const;

  uint8 operator[]( int ) const;

private:
  struct memory_tag
  {
    rex_crit_sect_type cs;
    int                ref_cnt;
    uint8*             mem_ptr;
    uint32             mem_size;

    memory_tag( const uint8* ptr, uint32 size );
    ~memory_tag( void );
  };

  memory_tag* m_tag_ptr;

  void copy( const qtv_blob_class& );
  void release_tag( void );

#ifdef QTV_BLOB_UNIT_TESTS
private:
  static bool unit_tests_done;
  static void run_unit_tests( void );
#endif
};
#endif /* HEADER_H */

