#ifndef QTV_VIDEO_DISPLAY_ANALYZER_H
#define QTV_VIDEO_DISPLAY_ANALYZER_H
/*=============================================================================
        Copyright 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================
DESCRIPTION:  QTV_VideoDisplayAnalyzers listen to timing information broadcast
by the renderer and can suggest adjustments to rendering procedures in order
to maintain optimal visual quality.

                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/videodelayanalyzer/main/latest/inc/qtv_videodelayanalyzer.h#8 $
$DateTime: 2009/01/02 14:03:15 $
$Change: 813226 $
=============================================================================*/
/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include <list>

#include "qtvInternalDefs.h"
#include "QTV_Allocator.h"
#include "video_renderer_broadcast.h"

//#include "Mpeg4Player.h"
class Mpeg4Player;

/*
===============================================================================
DATA STRUCTURES DOCUMENTATION
===============================================================================

===============================================================================
*/

/*
===============================================================================
MACROS DOCUMENTATION
===============================================================================

===============================================================================
*/

/*
===============================================================================
CLASS DOCUMENTATION
===============================================================================


===============================================================================
*/
class QTV_VideoDelayAnalyzer : private QTV_BroadcastReceiver
{
public:
   // CorrectionType structs are used to make suggestions on how rendering
   // delays should be modified to produce better a/v sync results.
   // if 'drop' is true, dropping frames is suggested.  If drop is false,
   // it is suggested that the given offset be added to the frame's sleep time.
   struct CorrectionType
   {
      int32 offset;
      bool  drop;
   };

   // 'N' is the input used to calculate the smoothing factor alpha used
   // in the exponential moving average calculations.  The most recent N
   // samples account for approximately 86% of the weight of the result.
   // For an alpha of 0.1 (common), n = 19.
   QTV_VideoDelayAnalyzer( int32 n = 19
                           #ifdef FEATURE_QTV_AVI
                           , Mpeg4Player* pPlayer=NULL
                           #endif 
                           );
   virtual ~QTV_VideoDelayAnalyzer( void );

   CorrectionType GetCorrection( void ) const;

private:
   typedef QTV_BroadcastTransmitter::InstallationKey PartialKeyType;
   typedef QTV_BroadcastTransmitter* BroadcasterIdType;

   typedef STD::pair< BroadcasterIdType, PartialKeyType > KeyType;
   typedef STD::list< KeyType, QTV_Allocator< KeyType > > ListType;

   // -------------------------------------------------------------------------
   // Broadcast reception "antenna"
   virtual void Receive( const QTV_Broadcast&, void* );

   // Number of decimal points of data to preserve in the EMA calculation.
   static const int32 FIXED_POINT_OFFSET;

   ListType m_receiverKeys;

   // All these values are stored offset by the fixed-point offset.
   // Real results can be obtained with a division.
   int32 m_n;     // n and alpha are discussed above.
   int32 m_ema;   // exponential moving average.

   bool m_bConsecutiveFrameDrop;
   int m_nConsecutiveFrameDrop;
#ifdef FEATURE_QTV_AVI
   Mpeg4Player *m_pPlayer;
#endif
};


#endif // QTV_VIDEO_DISPLAY_ANALYZER_H
