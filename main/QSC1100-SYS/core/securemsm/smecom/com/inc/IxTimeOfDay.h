#ifndef IXTIMEOFDAY_H
#define IXTIMEOFDAY_H
/*============================================================================
IxTimeOfDay

ABSTRACT
   The IxTimeOfDay interface definitions.

DESCRIPTION
   This service will provide basic file operations to EFS.


Copyright (c) 2005 - 2006  by QUALCOMM, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxTimeOfDay.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
12/16/06    rv     Fixed compiler warnings
10/30/06    rv     Init() takes an IPC signal
04/03/06    ws     Separate C related interfaces
03/16/06    ws     Add Julian functions 
11/11/05    ws     Initial Revision

=========================================================================== */

#include "comdef.h"

typedef struct
{
   uint16                 year;           /* Year [1980 .. 2100]                     */
   uint16                 month;          /* Month of year [1..12]                   */
   uint16                 day;            /* Day of month [1..31]                    */
   uint16                 hour;           /* Hour of day [0..23]                     */
   uint16                 minute;         /* Minute of hour [0..59]                  */
   uint16                 second;         /* Second of minute [0..59]                */
   uint16                 day_of_week;    /* Day of the week [0..6] [Monday..Sunday] */
} IxTimeOfDay_JulianTimeT;

//Definitions for "C" files
#ifndef __cplusplus
typedef struct
{
   uint32 dummy;
} IxTimeOfDay;

typedef struct
{
   uint32 dummy;
} IxCallback;

void                      Call_IxTimeOfDay_Init( IxTimeOfDay *obj, uint32 RecvIpcSigId );
uint64                    Call_IxTimeOfDay_GetLocalTime( IxTimeOfDay *obj );
void                      Call_IxTimeOfDay_SetLocalTimeOffset( IxTimeOfDay *obj, IxCallback *pcb, int32 offset );
int32                     Call_IxTimeOfDay_GetLocalTimeOffset( IxTimeOfDay *obj );
uint64                    Call_IxTimeOfDay_GetUserTime( IxTimeOfDay *obj );
void                      Call_IxTimeOfDay_SetUserTime( IxTimeOfDay *obj, IxCallback *pcb, uint32 new_user_time );
uint64                    Call_IxTimeOfDay_GetSysTime( IxTimeOfDay *obj );
void                      Call_IxTimeOfDay_SetSysTime( IxTimeOfDay *obj, IxCallback *pcb, uint32 new_sys_time );
IxTimeOfDay_JulianTimeT   Call_IxTimeOfDay_GetJulianTime( IxTimeOfDay *obj );
uint64                    Call_IxTimeOfDay_JulianToSeconds( IxTimeOfDay *obj, IxTimeOfDay_JulianTimeT *julianPtr );
IxTimeOfDay_JulianTimeT   Call_IxTimeOfDay_SecondsToJulian( IxTimeOfDay *obj, uint64 secs );

IxTimeOfDay*  Call_IxTimeOfDay_New( void );
void          Call_IxTimeOfDay_Delete( IxTimeOfDay * obj );

#else //__cplusplus
// Definitions for "C++" files

#include "IxBase.h"
#include "IxCallbackDef.h"

class IxTimeOfDay : public IxBase {
public:

   virtual unsigned long AddRef( void );
   virtual unsigned long Release( void );
   /* Gets the data representing the object */
   virtual void GetState( uint8** stateDataPtr, uint32* stateDataLenPtr ){(void)stateDataPtr; (void)stateDataLenPtr;};
   /* Sets the data for the object */
   virtual void SetState( uint8* stateDataPtr, uint32 stateDataLen ){(void)stateDataPtr; (void)stateDataLen;};

   virtual void                      Init( uint32 RecvIpcSigId );
   virtual uint64                    GetLocalTime( void );
   virtual void                      SetLocalTimeOffset( IxCallback *pcb, int32 offset );
   virtual int32                     GetLocalTimeOffset( void );
   virtual uint64                    GetUserTime( void );
   virtual void                      SetUserTime( IxCallback *pcb, uint32 new_user_time );
   virtual uint64                    GetSysTime( void );
   virtual void                      SetSysTime( IxCallback *pcb, uint32 new_sys_time );
   virtual IxTimeOfDay_JulianTimeT   GetJulianTime( void );
   virtual uint64                    JulianToSeconds( IxTimeOfDay_JulianTimeT *julianPtr );
   virtual IxTimeOfDay_JulianTimeT   SecondsToJulian( uint64 secs );

   // Constructor
   IxTimeOfDay( void );

private:

   static uint32 m_nRefs;

   static uint32 serverNodeId;
   static uint32 ipcMemPoolId;

};
#endif //_cplusplus

#endif //IXTIMEOFDAY_H

