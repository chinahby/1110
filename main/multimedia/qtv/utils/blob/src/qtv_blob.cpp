/************************************************************************* */
/**
 * @file
 * @brief Implementation of qtv_blob_class.
 * 
 * Copyright 2007 QUALCOMM Incorporated, All Rights Reserved.
 *
 ************************************************************************* */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/blob/main/latest/src/qtv_blob.cpp#2 $
$DateTime: 2009/11/30 03:18:44 $
$Change: 1098040 $

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
#include "qtv_blob.h"
#include "qcutils.h"
#include "qtvsystem.h"
#include "qtv_msg.h"
#include "assert.h"

/* =======================================================================

                        DATA DECLARATIONS

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
#ifdef QTV_BLOB_UNIT_TESTS
bool qtv_blob_class::unit_tests_done = false;
#define RUN_UNIT_TESTS() \
  if ( !unit_tests_done ) \
  { \
    unit_tests_done = true; \
    run_unit_tests(); \
  }
#else /* QTV_BLOB_UNIT_TESTS */
#define RUN_UNIT_TESTS()
#endif /* QTV_BLOB_UNIT_TESTS */

/* =======================================================================
**                        Macro Definitions
** ======================================================================= */

/* =======================================================================
**                        Class & Function Declarations
** ======================================================================= */

/************************************************************************* */
/**
 * Creates a memory tag.
 *
 * Memory is copied whenever a tag is constructed.
 * 
 ************************************************************************* */
qtv_blob_class::memory_tag::memory_tag( const uint8* ptr, uint32 size )
: ref_cnt( 1 ),
  mem_ptr( 0 ),
  mem_size( 0 )
{
  QCUtils::InitCritSect( &cs );
  if ( size > 0 )
  {
    mem_ptr = ( uint8* )QTV_Malloc( size );
    if ( mem_ptr )
    {
      mem_size = size;
      if ( ptr )
      {
        memcpy( mem_ptr, ptr, size );
      }
      else
      {
        memset( mem_ptr, 0, size );
      }
    }
  }
}

/************************************************************************* */
/**
 * Destroys a memory tag.
 *
 ************************************************************************* */
qtv_blob_class::memory_tag::~memory_tag( void )
{
  if ( mem_ptr )
  {
    #ifdef QTV_BLOB_UNIT_TESTS
    memset( mem_ptr, 0, mem_size );
    #endif
    QTV_Free( mem_ptr );
  }
  QCUtils::DinitCritSect(&cs);
}

/************************************************************************* */
/**
 * Creates an empty blob.
 * 
 ************************************************************************* */
qtv_blob_class::qtv_blob_class( void )
: m_tag_ptr( 0 )
{
  RUN_UNIT_TESTS();
}

/************************************************************************* */
/**
 * Creates a pre-sized blob.
 * 
 * @param[in] data_size The number of bytes of input memory.
 * @sa ~qtv_blob_class
 *
 ************************************************************************* */
qtv_blob_class::qtv_blob_class( uint32 data_size )
: m_tag_ptr( QTV_New_Args( qtv_blob_class::memory_tag, ( 0, data_size ) ) )
{
  RUN_UNIT_TESTS();
}

/************************************************************************* */
/**
 * Creates a blob copy of some source data.
 * 
 * When the blob is created, it copies the provided data - this allows
 * the blob to control the lifetime of that data and allows callers to 
 * delete or reuse the input memory after the blob has been constructed.
 *
 * @param[in] data_ptr The input memory.
 * @param[in] data_size The number of bytes of input memory.
 * @sa ~qtv_blob_class
 *
 ************************************************************************* */
qtv_blob_class::qtv_blob_class( const uint8* data_ptr, uint32 data_size )
: m_tag_ptr( QTV_New_Args( qtv_blob_class::memory_tag, ( data_ptr, data_size ) ) )
{
  RUN_UNIT_TESTS();
}

/************************************************************************* */
/**
 * Creates a blob copy of another blob.
 *
 * @param[in] other The blob to be copied.
 * @sa ~qtv_blob_class
 ************************************************************************* */
qtv_blob_class::qtv_blob_class( const qtv_blob_class& other )
: m_tag_ptr( 0 )
{
  RUN_UNIT_TESTS();

  copy( other );
}

/************************************************************************* */
/**
 * Destructor.
 * 
 * @sa qtv_blob_class()
 *
 ************************************************************************* */
qtv_blob_class::~qtv_blob_class( void )
{
  release_tag();
}

/************************************************************************* */
/**
 * Copies an existing blob.
 * 
 * @param[in] other The blob to be copied.
 *
 ************************************************************************* */
qtv_blob_class& qtv_blob_class::operator=( const qtv_blob_class& other )
{
  if ( this != &other )
  {
    copy( other );
  }
  return *this;
}

/************************************************************************* */
/**
 * Returns the size, in bytes, of the data being held in the blob.
 * 
 * @sa data()
 *
 ************************************************************************* */
uint32 qtv_blob_class::size( void ) const
{
  uint32 size( 0 );

  if ( m_tag_ptr )
  {
    size = m_tag_ptr->mem_size;
  }

  return size;
}

/************************************************************************* */
/**
 * Returns a pointer to the blob's data.
 *
 * This pointer is CONST for a reason!  Clients shouldn't be messing
 * with data held in a blob - that memory belongs to the blob.
 *
 * It's also dicey to ask for a memory pointer, as there's no way to know
 * when the memory pointed to may be deallocated.  If you call data(),
 * make sure you stop using the pointer before the blob is destroyed.
 * 
 * @sa size()
 *
 ************************************************************************* */
uint8* qtv_blob_class::data( void ) const
{
  uint8* data_ptr( 0 );

  if ( m_tag_ptr )
  {
    data_ptr = m_tag_ptr->mem_ptr;
  }

  return data_ptr;
}

/************************************************************************* */
/**
 * Copies data into the blob.
 *
 * @param[in] data_ptr The input memory.
 * @param[in] size The number of bytes of input memory.
 *
 ************************************************************************* */
void qtv_blob_class::copy( const qtv_blob_class& other )
{
  release_tag();

  if ( other.m_tag_ptr )
  {
    QCUtils::EnterCritSect( &other.m_tag_ptr->cs );
    m_tag_ptr = other.m_tag_ptr;
    ++( m_tag_ptr->ref_cnt );
    QCUtils::LeaveCritSect( &other.m_tag_ptr->cs );
  }
}

/************************************************************************* */
/**
 * Returns a single byte from within the blob.
 *
 * @param[in] index the byte index.
 * @return the indicated byte.
 *
 ************************************************************************* */
uint8 qtv_blob_class::operator[]( int index ) const
{
  if ( m_tag_ptr && data() && ( index < size() ))
  {
    return data()[ index ];
  }
  else
  {
    /** Ideally we'd throw an exception here but that's a no-no in this
     *  environment so instead we squawk and return 0. */
    QTV_MSG_PRIO( QTVDIAG_GENERAL,
                  QTVDIAG_PRIO_FATAL,
                  "blob accessed out of bounds" );
    return 0;
  }
}

/************************************************************************* */
/**
 * Releases a memory tag.  If we're the last to release it, frees the memory.
 *
 * @param[in] tag_ptr The tag.
 *
 ************************************************************************* */
void qtv_blob_class::release_tag( void )
{
  bool delete_tag( false );

  if ( m_tag_ptr )
  {
    QCUtils::EnterCritSect( &m_tag_ptr->cs );
    --( m_tag_ptr->ref_cnt );
    if ( m_tag_ptr->ref_cnt < 1 )
    {
      delete_tag = true;
    }
    QCUtils::LeaveCritSect( &m_tag_ptr->cs );

    if ( delete_tag )
    {
      QTV_Delete( m_tag_ptr );
    }
    m_tag_ptr = 0;
  }
}

#ifdef QTV_BLOB_UNIT_TESTS
/************************************************************************* */
/**
 * Unit test the qtv_blob!
 *
 ************************************************************************* */
void qtv_blob_class::run_unit_tests( void )
{
  static unsigned char data[]   = { 1,2,3,4,5,6,7,8,9,0xa,0xb,0xc };
  static unsigned char zeroes[] = { 0,0,0,0 };

  qtv_blob_class *b1_ptr( new qtv_blob_class( data, 4 ) );
  qtv_blob_class *b2_ptr( new qtv_blob_class( data + 4, 4 ) );
  qtv_blob_class *b3_ptr( 0 );

  /* Check for proper copy & bounds */
  ASSERT( ( *b1_ptr )[ 0 ] == 1 && 
          ( *b1_ptr )[ 1 ] == 2 && 
          ( *b1_ptr )[ 2 ] == 3 && 
          ( *b1_ptr )[ 3 ] == 4 && 
          ( *b1_ptr )[ 4 ] == 0 );

  b3_ptr = new qtv_blob_class( *b1_ptr );
  /* Copy construction. b1 = b3 */
  ASSERT( b3_ptr->size() == b1_ptr->size() && 
          b3_ptr->data() == b1_ptr->data() );
  ASSERT( b3_ptr->m_tag_ptr == b1_ptr->m_tag_ptr );
  ASSERT( b3_ptr->m_tag_ptr->ref_cnt == 2 );

  /* Assignment.  Move b1 from b3 to b2 */
  ( *b1_ptr ) = ( *b2_ptr );
  ASSERT( b1_ptr->m_tag_ptr == b2_ptr->m_tag_ptr );
  ASSERT( b1_ptr->m_tag_ptr->ref_cnt == 2 );
  ASSERT( b3_ptr->m_tag_ptr->ref_cnt == 1 );

  /* Reference counting.  Deleting b3 will dealloc the tag */
  const uint8 *mem_ptr = b3_ptr->data();
  ASSERT( memcmp( mem_ptr, data, 4 ) == 0 );
  delete b3_ptr;
  ASSERT( memcmp( mem_ptr, zeroes, 4 ) == 0 );
  b3_ptr = 0;

  /* Clean up! */
  mem_ptr = b2_ptr->data();
  ASSERT( memcmp( mem_ptr, data + 4, 4 ) == 0 );
  delete b2_ptr; b2_ptr = 0;
  ASSERT( memcmp( mem_ptr, data + 4, 4 ) == 0 );
  ASSERT( b1_ptr->m_tag_ptr->ref_cnt == 1 );
  delete b1_ptr; b1_ptr = 0;
  ASSERT( memcmp( mem_ptr, zeroes, 4 ) == 0 );
}
#endif /* QTV_BLOB_UNIT_TESTS */

