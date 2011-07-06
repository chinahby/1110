#ifndef QTV_FRAME_MAP_H
#define QTV_FRAME_MAP_H
/// ===========================================================================
///
/// @file
/// @brief Defines the FrameMap class
///
/// Copyright 2007 QUALCOMM Incorporated, All Rights Reserved.
///
/// ===========================================================================

// ============================================================================
//                              Edit History
//
// $Header: //source/qcom/qct/multimedia/qtv/renderer/main/latest/inc/framemap.h#2 $
// $DateTime: 2009/06/18 08:14:34 $
// $Change: 943958 $
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
#include <vdecoder_types.h>
#include <QtvPlayer.h>

extern "C"
{
   #include <queue.h>
   #include <rex.h>
}

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
enum frame_current_status
{
   FRAME_WITH_RENDERER = 0,
   FRAME_WITH_UI = 1
};

// ----------------------------------------------------------------------------
// Global Data Declarations
// ----------------------------------------------------------------------------

// ============================================================================
//                        Macro Definitions
// ============================================================================

// ============================================================================
//                        Class & Function Declarations
// ============================================================================

/// ---------------------------------------------------------------------------
///
/// FrameMaps are used to associate VDEC_FRAMEs with corresponding
/// FrameInfoT structures.
///
/// This is particularly useful during release-buffer calls, where a
/// FrameInfoT must be turned back into a VDEC_FRAME for passing to the lower
/// levels of the system.
/// ---------------------------------------------------------------------------
class FrameMap
{
public:
   FrameMap( void );
   ~FrameMap( void );

   /// FrameInfoT information can be deduced from the VDEC_FRAME, so we
   /// don't need to see both when we're remembering a frame.
   ///
   /// Note that this is called 'learn', not 'insert'.  We're not copying
   /// the VDEC_FRAME and the thing remains the property of the decoder.
   /// There's absolutely no guarantee the frame will remain valid after
   /// this call returns, so if we want any information from inside the
   /// structure we must learn it now.
   void Learn( VDEC_FRAME& f );

   /// Recall information about the last frame learned which matches information
   /// in the given FrameInfoT.  The frame pointer may or may not be valid,
   /// but it was the last one we saw which matched.
   VDEC_FRAME* Recall( const QtvPlayer::FrameInfoT& );

   /// If a decoder deletes a VDEC_FRAME object we must explicitly forget it,
   /// as it's no longer valid to make references into deallocated memory.
   /// We use a pointer here instead of a reference because we can't be sure
   /// if the memory has already been freed.
   void Forget( const VDEC_FRAME* const pF );

private:
   struct MemoryLink
   {
      q_link_type link;
      VDEC_FRAME* pFrame;
      void*       pBuf;
      int64       timestamp;

      explicit MemoryLink( void );
      ~MemoryLink( void );
      void Set( VDEC_FRAME*, void*, int64 );

   private:
      // Implicit copies are not allowed.
      MemoryLink( const MemoryLink& );
      MemoryLink& operator=( const MemoryLink& );
   };

   q_type m_learnedFramesQ;
   rex_crit_sect_type m_cs;

   void DumpAllLearnedFrames( void );
};
#endif // QTV_FRAME_MAP_H

