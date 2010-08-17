#ifndef TRMLOG_H
#define TRMLOG_H
/*===========================================================================

                   T R M    L O G   H E A D E R    F I L E

DESCRIPTION
   This file contains log types used internally by the TRM.
   It should NOT be included by any source file outside the TRM!

    Copyright (c) 2005 - 2008
                    by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/trm/main/latest/src/trmlog.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/08   adw     Added FEATURE_MCS_TRM to featurize TRM for ULC.
06/16/05   ajn     Re-implemented logging as on "ostream" look-a-like.
05/26/05   ajn     Code review comment changes
05/06/05   ajn     Added official LOG_TRANSCEIVER_RESOURCE_MGR_C log code.
02/23/05   ajn     TRMLogClient now takes client id
02/22/05   ajn     Implemented logging structures using inheritance.
02/18/05   ajn     Added more log structures
02/11/05   ajn     Initial revision.

===========================================================================*/

#include "customer.h"

#ifdef FEATURE_MCS_TRM

extern "C"
{
  #include "trm.h"
}

#include "trmi.h"

#include <stddef.h>


/*===========================================================================

                              HEADER CONSTANTS

  Numeric values for the identifiers in this section should not be changed.

===========================================================================*/


/*-------------------------------------------------------------------------
  Version IDs 
-------------------------------------------------------------------------*/

#define TRMLOG_VERSION_1              1
#define TRMLOG_VERSION_2              2
#define TRMLOG_VERSION_3              3



/*-------------------------------------------------------------------------
  Packet Type IDs 
-------------------------------------------------------------------------*/

typedef enum
{                 
  TRMLOG_ID_GRANT                   = 0,
  TRMLOG_ID_RESERVE_AT              = 1,
  TRMLOG_ID_REQUEST_N_NOTIFY        = 2,
  TRMLOG_ID_REQUEST                 = 3,
  TRMLOG_ID_CANCEL_REQUEST          = 4,
  TRMLOG_ID_RELEASE                 = 5,
  TRMLOG_ID_RETAIN_LOCK             = 6,
  TRMLOG_ID_CHANGE_PRIORITY         = 7,
  TRMLOG_ID_EXTEND_DURATION         = 8,
  TRMLOG_ID_CHANGE_DURATION         = 9,
  TRMLOG_ID_EXCHANGE                = 10,
  TRMLOG_ID_UNLOCK_NOTIFY           = 11
}
trmlog_id_type;



/*============================================================================

Class: TRMLog

Description:
  Allocates a buffer from diag's log pool, using TRM's log code, and
  accumulates data into the buffer.  Automatically commits the log buffer
  when the TRMLog object goes out of scope.

  If logging of TRM packets is not enabled, or if there is no availiable 
  memory for the buffer, logging requests using this object are silently
  discarded.
  
============================================================================*/

class TRMLog
{
  /*------------------------------------------------------------------------
    Private data members
  ------------------------------------------------------------------------*/
  private:

    /* Pointer to log buffer - if one can be allocated */
    uint8 * packet;

    /* Size of the log buffer */
    uint32  size;

    /* Amount of log buffer that has been used */
    uint32  used;


  /*------------------------------------------------------------------------
    No implicit copy constructor or assignment operator, please!
  ------------------------------------------------------------------------*/
  private:

    TRMLog( const TRMLog & );         /* Not implementated */
    void operator= (const TRMLog& );  /* Not implementated */


  /*------------------------------------------------------------------------
    Private Member functions
  ------------------------------------------------------------------------*/
  private:

    /* Allocate memory from diag's log buffer */
    void allocate( uint32 length );


  /*------------------------------------------------------------------------
    Public Member functions
  ------------------------------------------------------------------------*/
  public:

    /* Constructor.  Begin a log packet with the given log id and version */
    TRMLog( trmlog_id_type id, uint8 ver=TRMLOG_VERSION_1, uint32 length=0 );

    /* Constructor.  length=0 --> uses default length */
    TRMLog( uint32 length=0 );

    /* Destructor.  Will commit the log packet, if necessary. */
    ~TRMLog();

    /* Commit the log packet */
    void commit();

    /* Allocate memory inside the log packet */
    void *alloc( uint32 length );

    /* Allocate memory inside the log packet, and copy in the given data */
    void log( const void *data, uint32 length );

    /* Returns "true" if the log packet can accept more data */
    operator int () const { return size > used; }


  /*------------------------------------------------------------------------
    "ostream" style logging functions.
  ------------------------------------------------------------------------*/
  public:

    TRMLog& operator << (uint8  u8);
    TRMLog& operator << (uint16 u16);
    TRMLog& operator << (uint32 u32);

    TRMLog& operator << (int8   i8);
    TRMLog& operator << (int16  i16);
    TRMLog& operator << (int32  i32);

    TRMLog& operator << (trm_client_enum_t client_id);
    TRMLog& operator << (const trm_client_state &state);

  /*------------------------------------------------------------------------
    Additional stream output operators can be declared outside the class,
    via: "TRMLog& operator << (TRMLog& log, sometype& t);"
  ------------------------------------------------------------------------*/
};

#endif /* FEATURE_MCS_TRM */

#endif /* TRMLOG_H */

