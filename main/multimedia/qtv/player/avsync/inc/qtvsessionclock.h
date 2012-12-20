#ifndef QTVSESSIONCLOCK_H
#define QTVSESSIONCLOCK_H
/*============================================================================
  @file qtvsessionclock.h

               Copyright (c) 2007 QUALCOMM Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/multimedia/qtv/player/avsync/main/latest/inc/qtvsessionclock.h#1 
  $$DateTime: 2009/04/14 14:05:18 
  $$Author: sganji $

   when        who   what, where, why
   ----------  ---   ---------------------------------------------------------
   2009-04-10  sganji   Created.


============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "customer.h"
#include "qtvInternalDefs.h"
#include "zrex.h"

/* Constants */

#define QTV_SESSION_CLOCK_FASTPLAYBACK_HISTORY_COUNT 20


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum {
  IDLE = 0,
  RUNNING,
  PAUSED
} nSessionClockStateType;

typedef struct nSessionClockDriftConstantsTypeTag {
   int64 nDriftConstant;
   double nDriftRateConstant;          // for error = nDrftConstant + nDriftRateConstant* t  model  
   int64 nLastClockTime;
   int64 nLastReferenceClockTime;
} nSessionClockDriftConstantsType;

typedef struct nSessionClockFastPlaybackHistoryTableTypeTag {
   uint64 nSpeedChangePTS[QTV_SESSION_CLOCK_FASTPLAYBACK_HISTORY_COUNT];
   uint16 nSpeedNum[QTV_SESSION_CLOCK_FASTPLAYBACK_HISTORY_COUNT];
   uint16 nSpeedDen[QTV_SESSION_CLOCK_FASTPLAYBACK_HISTORY_COUNT];
   uint16 nReadIndex;
   uint16 nWriteIndex;
   uint16 nCurrentSize;
} nSessionClockFastPlaybackHistoryTableType;

typedef bool (*pInputClockMethodType) (uint64* pCurrentTime);

/*----------------------------------------------------------------------------
 * Class Definitions
 * -------------------------------------------------------------------------*/

class SessionClock {

private:

   SessionClock(const SessionClock &);              // Copy constructor.
   SessionClock &operator =(const SessionClock &);  // Copy assignment.
   uint64 m_nStartClockTime;                        /* in micro seconds*/
   uint64 m_nStartPTS;                              /* in micro seconds*/
   uint64 m_nStopPTS;                              /* in micro seconds*/
   uint64 m_nPausedPTS;                            /* in micro seconds*/
   nSessionClockStateType m_nState;
   nSessionClockStateType m_nLastState;
   pInputClockMethodType m_pInputClockMethod;
   rex_crit_sect_type m_SecssionClockCS;
   nSessionClockDriftConstantsType m_sDriftConstants;
   nSessionClockFastPlaybackHistoryTableType m_sFastPlaybackHistoryTable;
   uint32 m_nSpeedNum;
   uint32 m_nSpeedDen;

public:

   SessionClock(void);                   // constructor
   virtual ~SessionClock(void);          // Destructor.


   /** Get the current state. */
   nSessionClockStateType GetState(void);

   bool IsClockAvailable(void);

   /** Init */
   void Init(void);

   /** Start */
   bool Start(pInputClockMethodType pInputClockMethod, uint64 nStartClockTime, uint64 nStartPTS, uint16 nSpeedNum, uint16 nSpeedDen);

   /** Stop */
   bool Stop(uint64 nStoppedClockTime);

   /** Pause */
   bool Pause(uint64 nPausedClockTime);

   /** Resume   */
   bool Resume(uint64 nStartClockTime , uint64 nStartPTS);

   /** Read Current Clock Time   */
   bool ReadCurrentClockTime(uint64* pCurrentClockTime);

   /** Read Current PTS   */
   bool ReadCurrentPTS(uint64* pCurrentPTS, int64* pCurrentPTSOffset);

   /** SetStartRef */
   bool SetStartRef(uint64 nStartClockTime, uint64 nStartPTS);

   /** GetStartRef */
   bool GetStartRef(uint64 *pStartClockTime, uint64 *pStartPTS);

   /** SetDriftConstants */
   bool SetDriftConstants(nSessionClockDriftConstantsType sDriftConstants);

   /** GetDriftConstants */
   bool GetDriftConstants(nSessionClockDriftConstantsType *pDriftConstants);

   /** Set Clock Speed */
   bool SetClockSpeed(uint16 nSpeedNum, uint16 nSpeedDen, uint64 nSpeedChangeStartPTS );

   /** Process the clock start ref with fastplaybacdj history table */
   bool ProcessClockStartRefWithFastPlaybackHistory ( uint64 nCurrentPTS );

   /** IsFastPlayback history available  */
   bool IsFastPlaybackHistoryAvailable();
};


#endif /* QTVSESSIONCLOCK_H */
