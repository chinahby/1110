/* =============================================================================
                                qtvsessionclock.cpp

DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Portions copyrighted by QUALCOMM Incorporated;
Copyright 2007 QUALCOMM Incorporated, All Rights Reserved
============================================================================= */

/* =============================================================================

                               EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/multimedia/qtv/player/avsync/main/latest/src/qtvsessionclock.cpp#16 $
$$DateTime: 2009/04/14 14:05:18 
$$Author: vvarghes $
$Change: 1112911 $

   when        who   what, where, why
   ----------  ---   ---------------------------------------------------------
   2009-04-10  sganji   Created.


============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qtvsessionclock.h"
#include "qtv_utils.h"


#define QTV_SESSION_CLOCK_DEFAULT_START_TIME 0
#define QTV_SESSION_CLOCK_DEFAULT_PAUSED_TIME 0
#define QTV_SESSION_CLOCK_DEFAULT_STOPPED_TIME (uint64(-1))

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

SessionClock::SessionClock(void)
{
  Init();
  rex_init_crit_sect(&m_SecssionClockCS);
}

SessionClock::~SessionClock(void)
{
  CS_Locker lock( m_SecssionClockCS );
}

void SessionClock::Init(void)
{
  m_nStartClockTime = QTV_SESSION_CLOCK_DEFAULT_START_TIME;
  m_nStartPTS = QTV_SESSION_CLOCK_DEFAULT_START_TIME;
  m_nStopPTS = QTV_SESSION_CLOCK_DEFAULT_STOPPED_TIME;
  m_nPausedPTS = QTV_SESSION_CLOCK_DEFAULT_PAUSED_TIME;
  m_nState = IDLE;
  m_nLastState = IDLE;
  m_pInputClockMethod = NULL;
  m_sDriftConstants.nDriftConstant = 0;
  m_sDriftConstants.nDriftRateConstant = 1;
  m_sDriftConstants.nLastClockTime = -1;
  m_sDriftConstants.nLastReferenceClockTime = -1;
  memset(&m_sFastPlaybackHistoryTable, 0, sizeof(nSessionClockFastPlaybackHistoryTableType));
  m_nSpeedNum = 1;
  m_nSpeedDen = 1;
}

/** Get the current state. */
nSessionClockStateType SessionClock::GetState(void) 
{ 
  return m_nState; 
}

bool SessionClock::IsClockAvailable(void) 
{ 
  return (m_pInputClockMethod != NULL); 
}

bool SessionClock::Start(pInputClockMethodType pInputClockMethod, uint64 nStartClockTime, uint64 nStartPTS, uint16 nSpeedNum, uint16 nSpeedDen)
{
   CS_Locker lock( m_SecssionClockCS );
   bool eRet = false;

   QTV_MSG_PRIO4(QTVDIAG_GENERAL,QTVDIAG_PRIO_MED,
         "SessionClock::Start. this: %x Clock method: %x, nStartClockTime: %d, nStartPTS: %d.\n", this,
          pInputClockMethod, nStartClockTime, nStartPTS);

   if( !pInputClockMethod )
   {
     return false;
   }

   switch(m_nState)
   {
   case IDLE:
   case RUNNING:
   case PAUSED:
     {
       m_pInputClockMethod = pInputClockMethod;
       m_nPausedPTS = QTV_SESSION_CLOCK_DEFAULT_PAUSED_TIME;
       m_nStopPTS = QTV_SESSION_CLOCK_DEFAULT_STOPPED_TIME;
       m_nSpeedNum = nSpeedNum;
       m_nSpeedDen = nSpeedDen;
       m_nLastState = m_nState;
       m_nState = RUNNING;
       (void)SetStartRef( nStartClockTime, nStartPTS );
       eRet = true;
       break;
     }

   default:
     {        
       break;
     }
   }
   return eRet;
}

bool SessionClock::Stop(uint64 nStoppedClockTime)
{
   CS_Locker lock( m_SecssionClockCS );
   bool eRet = false;
   uint64 nStartClockTime = 0;
   uint64 nStartPTS = 0;

   QTV_MSG_PRIO2(QTVDIAG_GENERAL,QTVDIAG_PRIO_MED,
                 "SessionClock::Stop. this: %x nStoppedClockTime: %d\n", this, nStoppedClockTime);

   switch(m_nState)
   {
   case IDLE:
     {
       eRet = true;
       break;
     }
   case RUNNING:
     {
       if( GetStartRef( &nStartClockTime, &nStartPTS ) )
       {
         uint64 nStopPTS = 0;

         if( nStoppedClockTime > nStartClockTime )
         {
           nStopPTS = nStartPTS + ((int64)(nStoppedClockTime - nStartClockTime)*m_nSpeedNum)/m_nSpeedDen;
         }
         else
         {
           nStopPTS = nStartPTS;
         }
         if( IsFastPlaybackHistoryAvailable())
         {
           /* Process the start ref with  fast playback history table */
           (void)ProcessClockStartRefWithFastPlaybackHistory( nStopPTS );

           /* re calculate the current PTS */
           if( GetStartRef( &nStartClockTime, &nStartPTS ))
           {
             if( nStoppedClockTime > nStartClockTime )
             {
               nStopPTS = nStartPTS + ((int64)(nStoppedClockTime - nStartClockTime)*m_nSpeedNum)/m_nSpeedDen;
             }
             else
             {
               nStopPTS = nStartPTS;
             }
           }
         }
         Init();
         m_nStopPTS = nStopPTS; 
         eRet = true;
       }
       break;
     }
   case PAUSED:
     {
       uint64 nStopPTS = m_nPausedPTS;
       Init();
       m_nStopPTS = nStopPTS; 
       eRet = true;
       break;
     }
   default:
     {        
       break;
     }
   }
   return eRet;
}

bool SessionClock::Pause(uint64 nPausedClockTime)
{
   CS_Locker lock( m_SecssionClockCS );
   bool eRet = false;
   uint64 nStartClockTime = 0;
   uint64 nStartPTS = 0;

   QTV_MSG_PRIO2(QTVDIAG_GENERAL,QTVDIAG_PRIO_MED,
                 "SessionClock::Pause. this: %x nPausedClockTime: %d\n",this, nPausedClockTime);

   switch(m_nState)
   {
   case IDLE:
   case RUNNING:
   case PAUSED:
     {
       if( GetStartRef( &nStartClockTime, &nStartPTS ) )
       {
         if( nPausedClockTime > nStartClockTime )
         {
           m_nPausedPTS = nStartPTS + ((int64)(nPausedClockTime - nStartClockTime)*m_nSpeedNum)/m_nSpeedDen;
         }
         else
         {
           m_nPausedPTS = nStartPTS;
         }
           if( IsFastPlaybackHistoryAvailable())
           {
             /* Process the start ref with  fast playback history table */
             (void)ProcessClockStartRefWithFastPlaybackHistory( m_nPausedPTS );

             /* re calculate the current PTS */
             if( GetStartRef( &nStartClockTime, &nStartPTS ))
             {
               if( nPausedClockTime > nStartClockTime )
               {
                 m_nPausedPTS = nStartPTS + ((int64)(nPausedClockTime - nStartClockTime)*m_nSpeedNum)/m_nSpeedDen;
               }
               else
               {
                 m_nPausedPTS = nStartPTS;
               }
             }
           }
         if( m_nState != PAUSED)
         {
           m_nLastState = m_nState;
           m_nState = PAUSED;
         }

         eRet = true;
       }
       break;
     }

   default:
     {        
       break;
     }
   }
   return eRet;
}

bool SessionClock::Resume(uint64 nStartClockTime , uint64 nStartPTS)
{
   CS_Locker lock( m_SecssionClockCS );
   bool eRet = false;

   QTV_MSG_PRIO3(QTVDIAG_GENERAL,QTVDIAG_PRIO_MED,
                 "SessionClock::Resume. this: %x nStartClockTime: %d, nStartPTS: %d\n", this, nStartClockTime, nStartPTS);

   switch(m_nState)
   {
   case IDLE:
   case RUNNING:
   case PAUSED:
     {
       m_nState = m_nLastState;
       eRet = SetStartRef( nStartClockTime, nStartPTS );
       break;
     }

   default:
     {        
       break;
     }
   }
   return eRet;
}

bool SessionClock::ReadCurrentClockTime(uint64* pCurrentClockTime)
{
   bool eRet = false;
   nSessionClockDriftConstantsType sDriftConstants;
   int64 nDrift = 0;
   uint64 nStartClockTime = 0;
   uint64 nStartPTS = 0;

   QTV_MSG_PRIO1(QTVDIAG_GENERAL,QTVDIAG_PRIO_MED,
                 "SessionClock::ReadCurrentClockTime. pCurrentClockTime: %x\n" , pCurrentClockTime );
   if(!pCurrentClockTime)
   {
     return false;
   }

   if(m_pInputClockMethod)
   {
     eRet = GetDriftConstants(&sDriftConstants);
     eRet = GetStartRef( &nStartClockTime, &nStartPTS );
     eRet = m_pInputClockMethod(pCurrentClockTime) ;

     if( eRet && (*pCurrentClockTime > nStartClockTime) )
     {
       uint64 nDriftCorrectedCurrentClockTime = 0;
       /* Calulate the drift model error = nDrftConstant + nDriftRateConstant* t  */
       nDriftCorrectedCurrentClockTime = nStartClockTime +sDriftConstants.nDriftConstant + (uint64)( sDriftConstants.nDriftRateConstant * ( *pCurrentClockTime - nStartClockTime ));
       /* calculate the drift and adjust clock */
       nDrift = (int64)(nDriftCorrectedCurrentClockTime - *pCurrentClockTime);
       *pCurrentClockTime = nDriftCorrectedCurrentClockTime;
     }
   }
   QTV_MSG_PRIO4(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,
                 "SessionClock::ReadCurrentClockTime.this: %x eRet: %d CurrentClockTime: %u Drift:%d \n" ,this, eRet, *pCurrentClockTime, nDrift );

   return eRet;
}

bool SessionClock::ReadCurrentPTS(uint64* pCurrentPTS, int64* pCurrentPTSOffset)
{
   CS_Locker lock( m_SecssionClockCS );
   bool eRet = false;
   uint64 nCurrentClockTime = 0;
   uint64 nStartClockTime = 0;
   uint64 nStartPTS = 0;

   QTV_MSG_PRIO2(QTVDIAG_GENERAL,QTVDIAG_PRIO_MED,
                 "SessionClock::ReadCurrentPTS. pCurrentPTS: %x, pCurrentPTSOffset: %x\n" , pCurrentPTS, pCurrentPTSOffset );
   if(!pCurrentPTS || !pCurrentPTSOffset)
   {
     return false;
   }


     switch(m_nState)
     {
       case IDLE:
       {
         if( m_nStopPTS < QTV_SESSION_CLOCK_DEFAULT_STOPPED_TIME)
         {
           *pCurrentPTS = m_nStopPTS;
           *pCurrentPTSOffset = 0;
           eRet = true;
         }
         break;
       }
       case RUNNING:
       {
         if( ReadCurrentClockTime( &nCurrentClockTime)&& GetStartRef( &nStartClockTime, &nStartPTS ) )
         {
           if( nCurrentClockTime > nStartClockTime )
           {
             *pCurrentPTS = nStartPTS + ((int64)(nCurrentClockTime - nStartClockTime)*m_nSpeedNum)/m_nSpeedDen;
             *pCurrentPTSOffset = 0;
           }
           else
           {
              *pCurrentPTS = nStartPTS;
              *pCurrentPTSOffset = ((int64)(nStartClockTime - nCurrentClockTime)*m_nSpeedNum)/m_nSpeedDen;
           }

           if( IsFastPlaybackHistoryAvailable())
           {
             /* Process the start ref with  fast playback history table */
             (void)ProcessClockStartRefWithFastPlaybackHistory( *pCurrentPTS );

             /* re calculate the current PTS */
             (void)GetStartRef( &nStartClockTime, &nStartPTS );

             if( nCurrentClockTime > nStartClockTime )
             {
               *pCurrentPTS = nStartPTS + ((int64)(nCurrentClockTime - nStartClockTime)*m_nSpeedNum)/m_nSpeedDen;
               *pCurrentPTSOffset = 0;
             }
             else
             {
              *pCurrentPTS = nStartPTS;
              *pCurrentPTSOffset = ((int64)(nStartClockTime - nCurrentClockTime)*m_nSpeedNum)/m_nSpeedDen;
             }
           }

           eRet = true;
         }
         break;
       }
       case PAUSED:
       {
         if( ReadCurrentClockTime( &nCurrentClockTime)&& GetStartRef( &nStartClockTime, &nStartPTS ) )
         {
           *pCurrentPTS = m_nPausedPTS;
           *pCurrentPTSOffset = (int64)(nStartClockTime + m_nPausedPTS - nStartPTS) - nCurrentClockTime;
           *pCurrentPTSOffset = (*pCurrentPTSOffset*m_nSpeedNum)/m_nSpeedDen;
           eRet = true;
         }
         break;
       }
       default:
       {
         break;
       }
     }
   QTV_MSG_PRIO4(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,
                 "SessionClock::ReadCurrentPTS Speed. this:%x eRet: %d  m_nSpeedNum: %u, m_nSpeedDen: %u\n" ,this, eRet, m_nSpeedNum, m_nSpeedDen);
   QTV_MSG_PRIO4(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,
                 "SessionClock::ReadCurrentPTS. this:%x eRet: %d  CurrentPTS: %u, CurrentPTSOffset: %d\n" ,this, eRet, *pCurrentPTS, *pCurrentPTSOffset);
   
   return eRet;
}

/** SetStartRef */
bool SessionClock::SetStartRef(uint64 nStartClockTime, uint64 nStartPTS)
{
   CS_Locker lock( m_SecssionClockCS );
   bool eRet = false;
   if( IsClockAvailable() && (m_nState == RUNNING) )
   {
     if( IsFastPlaybackHistoryAvailable())
     {
       /* Process the start ref with  fast playback history table */
       (void)ProcessClockStartRefWithFastPlaybackHistory( nStartPTS );
     }

     m_nStartClockTime = nStartClockTime;
     m_nStartPTS = nStartPTS;
     eRet = true;
   }
   QTV_MSG_PRIO3(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,
         "SessionClock::SetStartRef. this: %x nStartClockTime: %d, nStartPTS: %d\n", this,
          nStartClockTime, nStartPTS);
   return eRet; 
}

/** GetStartRef */
bool SessionClock::GetStartRef(uint64 *pStartClockTime, uint64 *pStartPTS)
{
   CS_Locker lock( m_SecssionClockCS );
   bool eRet = false;
   if( IsClockAvailable() && pStartClockTime && pStartPTS  )
   {     
     *pStartClockTime = m_nStartClockTime;
     *pStartPTS = m_nStartPTS;
     eRet = true;
     QTV_MSG_PRIO3(QTVDIAG_GENERAL,QTVDIAG_PRIO_LOW,
         "SessionClock::GetStartRef. this: %x nStartClockTime: %d, nStartPTS: %d\n", this,
          *pStartClockTime, *pStartPTS);
   }
   return eRet; 
}

/** SetDriftConstants */
bool SessionClock::SetDriftConstants(nSessionClockDriftConstantsType sDriftConstants)
{
   CS_Locker lock( m_SecssionClockCS );
   bool eRet = false;
   if( IsClockAvailable() )
   {
     m_sDriftConstants.nDriftConstant = sDriftConstants.nDriftConstant;
     m_sDriftConstants.nDriftRateConstant = sDriftConstants.nDriftRateConstant;
     m_sDriftConstants.nLastClockTime = sDriftConstants.nLastClockTime;
     m_sDriftConstants.nLastReferenceClockTime = sDriftConstants.nLastReferenceClockTime;
     eRet = true;
   }
   QTV_MSG_PRIO3(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,
         "SessionClock::SetDriftConstants. this: %x nDrftConstant: %d, nDriftRateConstant: %d\n", this,
          m_sDriftConstants.nDriftConstant, (m_sDriftConstants.nDriftRateConstant*1000000));
   return eRet; 
}

/** GetDriftConstants */
bool SessionClock::GetDriftConstants(nSessionClockDriftConstantsType *pDriftConstants)
{
   CS_Locker lock( m_SecssionClockCS );
   bool eRet = false;
   if( IsClockAvailable()&& pDriftConstants )
   {
     pDriftConstants->nDriftConstant = m_sDriftConstants.nDriftConstant;
     pDriftConstants->nDriftRateConstant = m_sDriftConstants.nDriftRateConstant;
     pDriftConstants->nLastClockTime = m_sDriftConstants.nLastClockTime;
     pDriftConstants->nLastReferenceClockTime = m_sDriftConstants.nLastReferenceClockTime;
     eRet = true;
   }
   QTV_MSG_PRIO3(QTVDIAG_GENERAL,QTVDIAG_PRIO_LOW,
         "SessionClock::GetDriftConstants. this: %x nDriftConstant: %d, nDriftRateConstant: %d\n", this,
          m_sDriftConstants.nDriftConstant, (m_sDriftConstants.nDriftRateConstant*1000000));
   return eRet; 

}

/** SetClockSpeed */
bool SessionClock::SetClockSpeed(uint16 nSpeedNum, uint16 nSpeedDen, uint64 nSpeedChangeStartPTS )
{
   CS_Locker lock( m_SecssionClockCS );
   bool eRet = false;
   bool eInsertEntry = true;
   if( IsClockAvailable() )
   {
     eRet = true;
     if( m_sFastPlaybackHistoryTable.nCurrentSize > 0 )
     {
       uint64 nSpeedChangePTSLocal = 0;
       uint16 nSpeedNumLocal = 1;
       uint16 nSpeedDenLocal = 1;
       uint16 nPreviousEntryIndex = 0;  // recent entry index
       if(m_sFastPlaybackHistoryTable.nWriteIndex == 0)
       {
         nPreviousEntryIndex = QTV_SESSION_CLOCK_FASTPLAYBACK_HISTORY_COUNT-1;
       }
       else
       {
         nPreviousEntryIndex = m_sFastPlaybackHistoryTable.nWriteIndex-1;
       }
       nSpeedChangePTSLocal = m_sFastPlaybackHistoryTable.nSpeedChangePTS[ nPreviousEntryIndex];
       nSpeedNumLocal = m_sFastPlaybackHistoryTable.nSpeedNum[nPreviousEntryIndex];
       nSpeedDenLocal = m_sFastPlaybackHistoryTable.nSpeedDen[nPreviousEntryIndex];
       if( (nSpeedDen==nSpeedNumLocal)&&(nSpeedDen==nSpeedDenLocal) )
       {
         /* current entry will not change the clock speed, so reject this entry */
         eInsertEntry = false; 
         QTV_MSG_PRIO5(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,"SessionClock::SetClockSpeed, no speed change, rejecting entry. this: %x  nSpeedNum: %d, nSpeedDen: %d, nSpeedChangeStartPTS: %d, nSpeedChangePTSLocal: %d\n",
                       this, nSpeedNum, nSpeedDen, nSpeedChangeStartPTS, nSpeedChangePTSLocal);
       }
     }

     if(eInsertEntry)
     {
       /* Insert the entry into fast playback history table */
       m_sFastPlaybackHistoryTable.nSpeedNum[ m_sFastPlaybackHistoryTable.nWriteIndex ]  = nSpeedNum;
       m_sFastPlaybackHistoryTable.nSpeedDen[ m_sFastPlaybackHistoryTable.nWriteIndex ] = nSpeedDen;
       m_sFastPlaybackHistoryTable.nSpeedChangePTS[ m_sFastPlaybackHistoryTable.nWriteIndex ] = nSpeedChangeStartPTS;
       m_sFastPlaybackHistoryTable.nWriteIndex ++;
       m_sFastPlaybackHistoryTable.nWriteIndex = m_sFastPlaybackHistoryTable.nWriteIndex % QTV_SESSION_CLOCK_FASTPLAYBACK_HISTORY_COUNT;
       m_sFastPlaybackHistoryTable.nCurrentSize ++;

       if( m_sFastPlaybackHistoryTable.nCurrentSize > QTV_SESSION_CLOCK_FASTPLAYBACK_HISTORY_COUNT )
       {
         /* overwrite oldest entry */
         m_sFastPlaybackHistoryTable.nReadIndex ++;
         m_sFastPlaybackHistoryTable.nReadIndex = m_sFastPlaybackHistoryTable.nReadIndex % QTV_SESSION_CLOCK_FASTPLAYBACK_HISTORY_COUNT;
         m_sFastPlaybackHistoryTable.nCurrentSize = QTV_SESSION_CLOCK_FASTPLAYBACK_HISTORY_COUNT;
         QTV_MSG_PRIO2(QTVDIAG_GENERAL,QTVDIAG_PRIO_FATAL,
         "SessionClock::SetClockSpeed. this: %x fastplayback history table is full, size : %d\n", this,
            m_sFastPlaybackHistoryTable.nCurrentSize);
        }
        QTV_MSG_PRIO4(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,
           "SessionClock::SetClockSpeed. this: %x nSpeedNum: %d, nSpeedDen: %d, nSpeedChangeStartPTS: %d\n", this,
            nSpeedNum, nSpeedDen, nSpeedChangeStartPTS);
      }

   }
   return eRet; 
}

/** Process the clock start ref with fastplaybacdj history table */
bool SessionClock::ProcessClockStartRefWithFastPlaybackHistory( uint64 nCurrentPTS )
{
   CS_Locker lock( m_SecssionClockCS );
   bool eRet = true;
   if( IsClockAvailable() && (m_nState == RUNNING) )
   {
     if( m_sFastPlaybackHistoryTable.nCurrentSize )
     {
       uint16 nItr = 0;
       uint16 nItrSize = m_sFastPlaybackHistoryTable.nCurrentSize;

       for( nItr = 0; nItr < nItrSize; nItr++  )
       {
         uint64 nSpeedChangePTSLocal = m_sFastPlaybackHistoryTable.nSpeedChangePTS[ m_sFastPlaybackHistoryTable.nReadIndex ];
         uint16 nSpeedNumLocal = m_sFastPlaybackHistoryTable.nSpeedNum[ m_sFastPlaybackHistoryTable.nReadIndex ];
         uint16 nSpeedDenLocal = m_sFastPlaybackHistoryTable.nSpeedDen[ m_sFastPlaybackHistoryTable.nReadIndex ];

         if( nSpeedChangePTSLocal <= nCurrentPTS )
         {
           /* Process start Ref */
           if( nSpeedChangePTSLocal > m_nStartPTS)
           {
             m_nStartClockTime = m_nStartClockTime+ ( (int64)(nSpeedChangePTSLocal - m_nStartPTS)*m_nSpeedDen)/m_nSpeedNum;
             m_nStartPTS = nSpeedChangePTSLocal;
           }
           m_nSpeedNum = nSpeedNumLocal;
           m_nSpeedDen = nSpeedDenLocal;

          QTV_MSG_PRIO4(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,
           "SessionClock::ProcessClockStartRefWithFastPlaybackHistory. this: %x nCurrentPTS: %d, m_nStartPTS: %d, m_nStartClockTime: %d\n", this,
            nCurrentPTS, m_nStartPTS, m_nStartClockTime);

           m_sFastPlaybackHistoryTable.nReadIndex ++;
           m_sFastPlaybackHistoryTable.nReadIndex = m_sFastPlaybackHistoryTable.nReadIndex % QTV_SESSION_CLOCK_FASTPLAYBACK_HISTORY_COUNT;
           if(m_sFastPlaybackHistoryTable.nCurrentSize)
           {
             m_sFastPlaybackHistoryTable.nCurrentSize --;
           }
         }
         else
         {
           /* Nothing to process more */
           break;
         }
       }
     }
   }
   QTV_MSG_PRIO4(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,
         "SessionClock::ProcessClockStartRefWithFastPlaybackHistory. this: %x nSpeedNum: %d, nSpeedDen: %d, nSpeedChangeStartPTS: %d\n", this,
          m_nSpeedNum, m_nSpeedDen, m_nStartPTS);
   return eRet; 
}

 /** IsFastPlayback history available  */
 bool SessionClock::IsFastPlaybackHistoryAvailable()
 {
   bool eRet = false;
   if( IsClockAvailable() && m_sFastPlaybackHistoryTable.nCurrentSize )
   {
     eRet = true;
   }

   return eRet;
 }
