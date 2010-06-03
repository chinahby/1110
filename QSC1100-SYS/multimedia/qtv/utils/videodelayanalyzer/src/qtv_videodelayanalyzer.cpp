/*=============================================================================
                          QTV_VideoDelayAnalyzer.cpp

DESCRIPTION
  Implementation of the QTV_VideoDelayAnalyzer.

        Copyright 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/* ===========================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/videodelayanalyzer/main/latest/src/qtv_videodelayanalyzer.cpp#9 $
$DateTime: 2009/01/02 14:03:15 $
$Change: 813226 $

=============================================================================*/

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "assert.h"
#include "qtv_msg.h"
#include "QTV_VideoDelayAnalyzer.h"
#include "Mpeg4Player.h"

//=============================================================================
//   Type definitions
//=============================================================================

//=============================================================================
//   Constant definitions
//=============================================================================
const int32 QTV_VideoDelayAnalyzer::FIXED_POINT_OFFSET = 1000;
   // powers of ten make hand-debugging easier - for production, powers of two
   // would make things faster.

//=============================================================================
// QTV_VideoDelayAnalyzer
//
// Constructs a delay analyzer.  Once construction is complete, the analyzer
// is hooked to all desired broadcast sources.  Should broadcast hookups fail,
// default behavior, the EMA will remain unchanged at 0, causing recommentations
// which reverse the sign but not the amplitude of rendering offsets.  Since
// average offsets are, over the long term, always a small positive (needing
// a small negative) correction, this 'incomplete' behavior, while incorrect,
// will still reduce the error integral over no correction at all.
//
// N is discussed in the header file, and used to create the smoothing factor,
// alpha.  See http://en.wikipedia.org/wiki/Exponential_moving_average
//=============================================================================
QTV_VideoDelayAnalyzer::QTV_VideoDelayAnalyzer( int32 n
                                                #ifdef FEATURE_QTV_AVI
                                                , Mpeg4Player* pPlayer
                                                #endif
                                                )
:  m_n( n ),
   m_ema( 0 ),
   m_bConsecutiveFrameDrop(false),
   m_nConsecutiveFrameDrop(0)
   #ifdef FEATURE_QTV_AVI
   ,
   m_pPlayer(pPlayer)
   #endif
{
   static const QTV_Broadcast::Type aBroadcastTypes[] =
   {
      QTV_Broadcast::VIDEO_FRAME_RENDERED,
      QTV_Broadcast::VIDEO_FRAME_DROPPED,
   };
   static const int nBroadcastTypes =
      sizeof(aBroadcastTypes) / sizeof(aBroadcastTypes[0]);

   QTV_BroadcastTransmitter* pTx( QTV_BroadcastTransmitter::Locate( "QTV" ));
   if ( pTx )
   {
      KeyType k;
      k.first = pTx;

      for( int i = 0; i < nBroadcastTypes; ++i )
      {
         k.second = pTx->InstallReceiver( aBroadcastTypes[i], this, 0 );

         if ( k.second )
         {
            m_receiverKeys.push_back( k );
         }
      }
   }
}

//=============================================================================
// ~QTV_VideoDelayAnalyzer
//
// Disconnects the analyzer from all broadcast sources.
//=============================================================================
QTV_VideoDelayAnalyzer::~QTV_VideoDelayAnalyzer( void )
{
   KeyType k;

   while ( !m_receiverKeys.empty() )
   {
      k = ( KeyType )m_receiverKeys.front();
      m_receiverKeys.pop_front();

      ( k.first )->UninstallReceiver( k.second );
   }
   ASSERT( m_receiverKeys.size() == 0 );
}

//=============================================================================
// Receive
//
// Receives renderer broadcasts and updates internal statistics based on
// received data.
//=============================================================================
void QTV_VideoDelayAnalyzer::Receive( const QTV_Broadcast& message, void* )
{
   QTV_VideoRendererBroadcast* pB ;

   if(message.m_type == QTV_Broadcast::VIDEO_FRAME_RENDERED)
   {
     pB = ( ( QTV_VideoRendererBroadcast* )&message );

     // Moving Average analysis
     int32 s = ( pB->m_timeliness * FIXED_POINT_OFFSET );
     m_ema = m_ema + ((( s - m_ema ) * 2 ) / ( m_n + 1 ));
     CorrectionType c = GetCorrection();
     QTV_MSG_PRIO3( QTVDIAG_VIDEO_TASK,
                  QTVDIAG_PRIO_DEBUG,
                  "%d,%d,%d",
                  s/FIXED_POINT_OFFSET, m_ema/FIXED_POINT_OFFSET, s + c.offset );

     // Reset FrameDrop parameters
     m_bConsecutiveFrameDrop = false;
     m_nConsecutiveFrameDrop = 0;
   }
   else if(message.m_type == QTV_Broadcast::VIDEO_FRAME_DROPPED)
   {
     if(m_bConsecutiveFrameDrop == true)
     {
       m_nConsecutiveFrameDrop++;
     }
     else
     {
       m_bConsecutiveFrameDrop = true;
     }
    #ifdef FEATURE_QTV_AVI
     if(m_nConsecutiveFrameDrop > (m_pPlayer->GetQTVConfigItem(QtvConfig::QTVCONFIG_MAX_FRAME_DROPS_TO_IFRAME)))
    #else
     if(m_nConsecutiveFrameDrop > 4)
    #endif
     {
       #ifdef FEATURE_QTV_AVI
       QTV_EXCESSIVE_FRAME_DROPS_type *pEvent = 
         QCCreateMessage(QTV_EXCESSIVE_FRAME_DROPS, m_pPlayer); /*lint !e641 */

       m_pPlayer->PostMessage(pEvent);
       #endif
       QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK,
                  QTVDIAG_PRIO_DEBUG,
                  "Dropped %d consecutive frames",m_nConsecutiveFrameDrop);     
        m_nConsecutiveFrameDrop = 0;
        m_bConsecutiveFrameDrop = false;
        
     }                                     
   }  
}

//=============================================================================
// Receive
//
// Receives renderer broadcasts and updates internal statistics based on
// received data.
//=============================================================================
QTV_VideoDelayAnalyzer::CorrectionType QTV_VideoDelayAnalyzer::GetCorrection( void ) const
{
   CorrectionType c;

   c.offset = -( m_ema / FIXED_POINT_OFFSET );
   c.drop   = false;

   return c;
}

