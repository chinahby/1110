#ifndef QTV_UTILS_H
#define QTV_UTILS_H
/* =======================================================================

                          qtv_utils.h

DESCRIPTION
  Definition of utility functions and classes used by QTV.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright 2005 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/common/main/latest/inc/qtv_utils.h#3 $
$DateTime: 2009/12/03 02:35:48 $
$Change: 1101863 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "rex.h"
#include "queue.h"
#include "qtv_msg.h"
#include "QtvPlayer.h"
#include "media.h"
#include "AVSync.h"

/* ==========================================================================

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
#define QTV_MEMORY_POOL_MAX_NAME_LEN 31

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Macro Definitions
** ======================================================================= */

/* =======================================================================
**                        Class Declarations
** ======================================================================= */

//----------------------------------------------------------------------------
// QTV_LockableQueue associates a q_type with a rex_crit_sect_type, forcing
// users to lock the CS before messing with the queue.
//
class QTV_LockableQueue
{
public:
  QTV_LockableQueue( void )
  {
    q_init( &m_q );
    rex_init_crit_sect( &m_cs );
  }

  virtual ~QTV_LockableQueue( void )
  {
    if ( q_cnt( &m_q ) > 0 )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "~QTV_LockableQueue with items queued" );
    }
    /* ------
       Commented out, since this code goes to all targets as common and 
       below REX API is not currently ported to all active targets and baselines.
    --------- */
    //rex_del_crit_sect( &m_cs );
  }

  q_type* Lock( void )
  {
    rex_enter_crit_sect( &m_cs );

    return &m_q;
  }

  void Unlock( q_type* &pQ )
  {
    pQ = NULL;
    rex_leave_crit_sect( &m_cs );
  }

  int GetCnt( void )
  {
    return q_cnt( &m_q );
  }

private:
  q_type             m_q;
  rex_crit_sect_type m_cs;

};

/*===========================================================================
** qtv_memory_pool_class
**
** This class allows us to obtain and release chunks of memory without going
** all the way out to the heap manager unless we need to.  Released memory
** is stored in a list and recycled.
*/
class qtv_memory_pool_class
{
public:
  qtv_memory_pool_class
  ( 
    const uint32 chunk_size, 
      /* memory allocated in chunks of this many bytes */
    const char* const pool_name_ptr = 0
      /* an optional name for the pool, used in diag debug messages */
  );

  ~qtv_memory_pool_class( void );

  /* Call get() to obtain a chunk of memory. */
  uint8* get( void );

  /* Call put() to release a previously..um...gotten chunk. */
  void put( uint8* const chunk_ptr );

  /* outstanding_chunk_count returns the number of chunks which have
  ** been passed out by the pool but not yet returned.
  */
  uint32 outstanding_chunk_count( void );

  /* Releases all idle heap memory held by the memory pool.  This is useful
  ** if you want to let go of heap without killing the pool.
  ** Don't laugh!  It happens.
  */
  void flush( void );

private:
  const uint32       m_chunk_size; 
    /* size of chunks, in bytes. */

  q_type             m_used_q;
    /* chunks which are used by clients */

  q_type             m_free_q;
    /* available chunks waiting to be recycled */

  rex_crit_sect_type m_cs;
    /* critical section for guarding queue access */

  char               m_pool_name[ QTV_MEMORY_POOL_MAX_NAME_LEN + 1 ];
    /* Name of this pool, or empty string if we're anonymous. */

  bool               m_object_is_alive;
    /* We set this false once the destructor starts to prevent clients
    ** from allocating chunks during or after destruction. */

  /* Release all heap memory held by the referenced chunk queue. */
  void flush( q_type &q );
};

/* ======================================================================
FUNCTION:
  WaitForFlag

DESCRIPTION:
  Waits (via polling) for a flag to be set.

PARAMETERS:
  bool &flag
    The flag we're waiting for.  We'll sleep and poll until the flag
    goes to true (or we hit timeout).

  unsigned long timeout_ms
    The timeout.  If we wait this long and the flag doesn't get set,
    we'll return false.  0 means 'wait forever'.

  unsigned long poll_interval_ms
    How often we'll check the flag, in ms.  Larger numbers will cause
    less CPU load, but will also introduce more jitter.

RETURN VALUE:
  bool
    true if the flag was set, false if we timed out.

SIDE EFFECTS:
  Can cause rapid task-switching if poll_interval_ms is too small.
===========================================================================*/
bool WaitForFlag( bool &flag, 
                  unsigned long timeout_ms       = 0, 
                  unsigned long poll_interval_ms = 2 );

/************************************************************************* */
//
// A teeny little utility class which makes it easy to make sure we remember
// to unlock criticial sections, since it happens automatically when the
// locker goes out of scope.
// 
/************************************************************************* */
class CS_Locker
{
public:
   CS_Locker( rex_crit_sect_type& cs )
   : m_cs( cs )
   {
      rex_enter_crit_sect( &m_cs );
   }

   ~CS_Locker( void )
   {
      rex_leave_crit_sect( &m_cs );
   }

private:
   rex_crit_sect_type& m_cs; //lint !e1725
}; //lint !e1712

/************************************************************************* */
/**
 * TRUE if the stream type describes a live broadcast stream.
 *
 ************************************************************************* */
inline bool is_live_broadcast( QtvPlayer::StreamTypeT t )
{
  return ( t == QtvPlayer::MEDIA_LIVE_STREAMING ||
           t == QtvPlayer::MEDIA_BUFFERED_LIVE_STREAMING );
}

/************************************************************************* */
/**
 * TRUE if this Media pointer points to a GenericBcastMedia object.
 * We infer this by source type, which is a little risky.
 *
 ************************************************************************* */
inline bool is_GenericBcastMedia( Media* media_ptr )
{
  return ( media_ptr &&
           ( media_ptr->GetSource() == Media::BCAST_FLO_SOURCE ||
             media_ptr->GetSource() == Media::BCAST_ISDB_SOURCE ) );
}

/************************************************************************* */
/**
 * Legacy (used to be macros) used to determine if a source is or isn't
 * a repositionable GBM.  These have been deprecated and should be removed
 * at earliest convenience.
 *
 * IsFileBcastMediaSource was originally written as !IsLiveBcastMediaSource,
 * which meant that despite the name it'd return true for any Media subclass
 * ( not just GBM ).  In order to keep things working, that crazy behavior
 * has been preserved.
 *
 ************************************************************************* */
// Check if we are playing in live mode
inline bool IsLiveBcastMediaSource( Media* media_ptr, AVSync *m_pAVSync )
{

  // Either If the media reports that the source is not repositionable 
  // or for TSB sources enquire the current status from the AVSync object
  return ( is_GenericBcastMedia( media_ptr ) && 
  	   (  !media_ptr->IsRepositioningAllowed() ||
  	       ( m_pAVSync && m_pAVSync->IsLive())
           )
         );
}

// Check if we are playing in buffered mode
inline bool IsFileBcastMediaSource( Media* media_ptr , AVSync *m_pAVSync ) 
{
  return !IsLiveBcastMediaSource( media_ptr,m_pAVSync );
}

// Check if media source is live
inline bool IsLiveBcastMediaSource( Media* media_ptr )
{
  return ( is_GenericBcastMedia( media_ptr ) && 
  	   media_ptr->IsLive()
         );
}

// Check if media source is not live
inline bool IsFileBcastMediaSource( Media* media_ptr  ) 
{
  return !IsLiveBcastMediaSource( media_ptr );
}

// ***************************************************************************
// Difference
//
// Calculate the difference between two unsigned values, with support
// for an arbitrary rollover point.
// ***************************************************************************
int64 Difference( const uint64& a, const uint64& b, const uint64& rolloverAfter );

// ***************************************************************************
// Sum
//
// Calculate the sum of two unsigned values, with support
// for an arbitrary rollover point.
// ***************************************************************************
uint64 Sum( const uint64& a, const uint64& b, const uint64& rolloverAfter );

#endif // QTV_UTILS_H
