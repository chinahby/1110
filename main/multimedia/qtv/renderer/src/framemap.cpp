/// ===========================================================================
///
/// @file
/// @brief Implementation of FrameMap.
///
/// Copyright 2007 QUALCOMM Incorporated, All Rights Reserved.
///
/// ===========================================================================

// ============================================================================
//                              Edit History
//
// $Header: //source/qcom/qct/multimedia/qtv/renderer/main/latest/src/framemap.cpp#9 $
// $DateTime: 2009/11/30 03:18:44 $
// $Change: 1098040 $
//
// ============================================================================

// ============================================================================
//               Includes and Public Data Declarations
// ============================================================================

// ============================================================================
//
//                   INCLUDE FILES FOR MODULE
//
// ============================================================================
#include <qtvInternalDefs.h>
#include <FrameMap.h>
#include <qtv_msg.h>
#include <qtv_utils.h>

// ============================================================================
//
//                      DATA DECLARATIONS
//
// ============================================================================

// ----------------------------------------------------------------------------
// Constant / Define Declarations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Type Declarations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Global Constant Data Declarations
// ----------------------------------------------------------------------------
static const bool NOISY_DEBUG = false;

// ----------------------------------------------------------------------------
// Global Data Declarations
// ----------------------------------------------------------------------------

// ============================================================================
//                        Macro Definitions
// ============================================================================

// ============================================================================
//                        Class & Function Definitions
// ============================================================================

/// ---------------------------------------------------------------------------
///
/// MemoryLink constructor.
///
/// Makes sure the q_link inside the structure is properly initialized
/// on structure creation.
///
/// ---------------------------------------------------------------------------
FrameMap::MemoryLink::MemoryLink( void )
:  pFrame( 0 ),
   pBuf( 0 ),
   timestamp( -1 )
{
   q_link( this, &link );
}

/// ---------------------------------------------------------------------------
///
/// MemoryLink destructor.
///
/// ---------------------------------------------------------------------------
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
FrameMap::MemoryLink::~MemoryLink( void ) {}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/// ---------------------------------------------------------------------------
///
/// Sets the volatile contents of a MemoryLink.
///
/// Allows for object reuse without the need for construction and destruction.
/// Actually, since there's no targeted constructor, allows the ONLY
/// mechanism for updating the volatiles within the structure.
///
/// @param[in] pVF VDEC_FRAME pointer
/// @param[in] pVo void* buffer pointer
/// @param[in] ts frame timestamp
///
/// ---------------------------------------------------------------------------
void FrameMap::MemoryLink::Set( VDEC_FRAME* pVF,
                                void*       pVo,
                                int64       ts )
{
   pFrame    = pVF;
   pBuf      = pVo;
   timestamp = ts;
}

/// ---------------------------------------------------------------------------
///
/// FrameMap constructor.
///
/// Makes sure member data is properly initialized.
///
/// ---------------------------------------------------------------------------
FrameMap::FrameMap( void )
{
   q_init( &m_learnedFramesQ );
   QCUtils::InitCritSect( &m_cs );
}

/// ---------------------------------------------------------------------------
///
/// FrameMap destructor.
///
/// Frees all the memory held in the learned-frames queue.
///
/// ---------------------------------------------------------------------------
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
FrameMap::~FrameMap( void )
{
   CS_Locker lock( m_cs );
   MemoryLink* pL( ( MemoryLink* )q_get( &m_learnedFramesQ ) );

   while ( pL )
   {
      QTV_Delete( pL );
      pL = ( MemoryLink* )q_get( &m_learnedFramesQ );
   }

   QCUtils::DinitCritSect(&m_cs);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/// ---------------------------------------------------------------------------
///
/// FrameMap::Learn
///
/// Learns a frame/buffer pairing.  If the referenced buffer has been
/// previously used, then the old memory is replaced.
///
/// FrameInfoT information can be deduced from the VDEC_FRAME, so we
/// don't need to see both when we're remembering a frame.
///
/// Note that this is called 'learn', not 'insert'.  We're not copying
/// the VDEC_FRAME and the thing remains the property of the decoder.
/// There's absolutely no guarantee the frame will remain valid after
/// this call returns, so if we want any information from inside the
/// structure we must learn it now.
///
/// @param[in] f The VDEC_FRAME from which to learn the information.
///
/// ---------------------------------------------------------------------------
void FrameMap::Learn( VDEC_FRAME& f )
{
   CS_Locker lock( m_cs );
   // Search memory for a previous record of this frame
   MemoryLink* pL( ( MemoryLink* )q_check( &m_learnedFramesQ ) );

   while ( pL && !( pL->pBuf == f.pBuf ))
   {
      pL = ( MemoryLink* )q_next( &m_learnedFramesQ, &pL->link );
   }

   // No match?  Put a new memory in the list.
   if ( !pL )
   {
      pL = QTV_New( MemoryLink );
      if ( pL )
      {
        q_put( &m_learnedFramesQ, &pL->link );
      }
      // No need for else here, we'll squawk about OOM further down.
   }

   // Save frame info in the memory struct we found (or made).
   if ( pL )
   {
      pL->Set( &f, f.pBuf, f.timestamp );
      pL->pFrame->frameStatus = FRAME_WITH_RENDERER;
      QTV_MSG_PRIO4( QTVDIAG_DEBUG,
       QTVDIAG_PRIO_HIGH,
       "Frame learned: frame %x display buffer %x ts %d,m_learnedFramesQ.cnt=%d",
       pL->pFrame,
       pL->pBuf,
       pL->timestamp,
       m_learnedFramesQ.cnt);

   }
   else
   {
      QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL, "OOM" );
   }

   DumpAllLearnedFrames();
}

/// ---------------------------------------------------------------------------
///
/// Recalls a previously learned frame-buffer association.
///
/// Recall information about the last frame learned which matches information
/// in the given FrameInfoT.  The frame pointer may or may not be valid,
/// but it was the last one we saw which matched.
///
/// @param[in] fit The FrameInfoT we're trying to match.
///
/// ---------------------------------------------------------------------------
VDEC_FRAME* FrameMap::Recall( const QtvPlayer::FrameInfoT& fit )
{
   CS_Locker lock( m_cs );
   // Search memory for a previous record of this frame
   MemoryLink* pL( ( MemoryLink* )q_check( &m_learnedFramesQ ) );

   while ( pL && !( pL->pBuf == fit.RGBBuffer || pL->pBuf == fit.YUVBuffer ))
   {
      pL = ( MemoryLink* )q_next( &m_learnedFramesQ, &pL->link );
   }

   if ( pL && pL->pFrame->frameStatus == FRAME_WITH_UI)
   {
      QTV_MSG_PRIO4( QTVDIAG_DEBUG,
       QTVDIAG_PRIO_HIGH,
       "Frame recalled: frame %x display buffer %x ts %d,m_learnedFramesQ.cnt=%d",
       pL->pFrame,
       pL->pBuf,
       pL->timestamp,
       m_learnedFramesQ.cnt);
   }

   DumpAllLearnedFrames();

   return ( pL ? pL->pFrame : 0 );
}

/// ---------------------------------------------------------------------------
///
/// Forgets a previously learned frame.
///
/// If a decoder deletes a VDEC_FRAME object we must explicitly forget it,
/// as it's no longer valid to make references into deallocated memory.
/// We use a pointer here instead of a reference because we can't be sure
/// if the memory has already been freed.
///
/// @param[in] pF the frame to be forgotten.
///
/// ---------------------------------------------------------------------------
void FrameMap::Forget( const VDEC_FRAME* const pF )
{
   CS_Locker lock( m_cs );
   // Search memory for a previous record of this frame
   MemoryLink* pL( ( MemoryLink* )q_check( &m_learnedFramesQ ) );

   while ( pL && ( pL->pFrame != pF ))
   {
      pL = ( MemoryLink* )q_next( &m_learnedFramesQ, &pL->link );
   }

   if ( pL )
   {
      QTV_MSG_PRIO4( QTVDIAG_DEBUG,
       QTVDIAG_PRIO_HIGH,
       "Frame forgotten: frame %x display buffer %x ts %d,m_learnedFramesQ.cnt=%d",
       pL->pFrame,
       pL->pBuf,
       pL->timestamp,
       m_learnedFramesQ.cnt);
      q_delete( &m_learnedFramesQ, &pL->link );
      QTV_Delete( pL );
   }
   // else: No match?  Then it's already forgotten, eh?

   DumpAllLearnedFrames();
}

/// ---------------------------------------------------------------------------
///
/// Dumps the learned frame list via F3 messages to ease diagnostics.
///
/// ---------------------------------------------------------------------------
void FrameMap::DumpAllLearnedFrames( void )
{
   CS_Locker lock( m_cs );
   if ( NOISY_DEBUG )
   {
      QTV_MSG_PRIO( QTVDIAG_DEBUG,
                    QTVDIAG_PRIO_HIGH,
                    "BEGIN frame/buffer pairings BEGIN" );

      // Search memory for a previous record of this frame
      MemoryLink* pL( ( MemoryLink* )q_check( &m_learnedFramesQ ) );

      while ( pL )
      {
         QTV_MSG_PRIO3( QTVDIAG_DEBUG,
                        QTVDIAG_PRIO_HIGH,
                        "ts %d ( VDEC %08x == BUF %08x )",
                        pL->timestamp,
                        pL->pFrame,
                        pL->pBuf );
         pL = ( MemoryLink* )q_next( &m_learnedFramesQ, &pL->link );
      }

      QTV_MSG_PRIO( QTVDIAG_DEBUG,
                    QTVDIAG_PRIO_HIGH,
                    "END frame/buffer pairings END" );
   } // NOISY_DEBUG?
}
