#ifndef QTV_TASK_IF_H
#define QTV_TASK_IF_H
/* =======================================================================

                          qtv_task_if.h

DESCRIPTION
  Definition of types and classes to support the message/IPC-based qtv 
  common-task primitive class.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/task/main/latest/inc/qtv_task_if.h#9 $
$DateTime: 2008/09/24 02:10:11 $
$Change: 749684 $

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
#include "qtvsystem.h"
#include "qtv_utils.h"
#include "queue.h"
#include "qtv_task_if_task.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* Enable this if you want an F3 diag message every time qtv_task_if-
** based tasks have a thought.  It's a lot of messages. */
// #define QTV_TASK_IF_MSG_QUEUE_DEBUG

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* qtv_msg_struct
**
** qtv_msg_sinks communicate with one another via qtv_msg_structs.
** Fields are defined as follows:
**
** payload: the message payload.  It is up to the
** sender and receiver of the message to agree on the format of data in this
** area.  If more room is needed than is provided by the payload, a pointer
** to an external location can be placed in the payload - it's up to the
** entities on both ends of the message to agree on 
** allocation/ownership/deallocation rules in those cases. */

struct qtv_msg_struct
{
  enum
  {
    PAYLOAD_BYTES = 128
  };

  uint8 payload[ PAYLOAD_BYTES ];

  /* We provide a specialized new/delete set which uses object pools
  ** to minimize the number of heap calls.  This means you want to use
  ** these functions to control these things...not the QTV macros! */
  static qtv_msg_struct* malloc( void )
  {
    return QTV_New( qtv_msg_struct );
  }

  static void free( qtv_msg_struct* ptr )
  {
     if (ptr != NULL)
     {
       QTV_Delete( ptr );
     }
  }
};

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Macro Definitions
** ======================================================================= */

/* =======================================================================
**                        Class & Function Declarations
** ======================================================================= */

/* qtv_msg_sink_class
**
** Anything capable of receiving events (as defined in this header) must
** support the event-sink interface.  Tasks or subsystems wishing
** to communicate with msg_sink objects via their IPC mechanism can 
** implement their own event sinks to support their end of the interaction. 
**
** Message sinks use reference-counted handles to allow dispatch mechanisms
** to refer to them with no risk of using stale memory.  This mechanism
** is 'automatic': subclasses of msg_sink do not need to do anything
** to benefit from it. */

class qtv_msg_sink_class;

class qtv_msg_sink_handle_class
{
public:
  qtv_msg_sink_handle_class( void );
  ~qtv_msg_sink_handle_class( void );

  qtv_msg_sink_handle_class* add_ref( void );
  void release( void );

  void set_sink( qtv_msg_sink_class* );
  bool dispatch_to_sink( qtv_msg_struct* const msg_ptr );
  
private:
  rex_crit_sect_type  m_ref_cnt_cs_;
  uint32              m_ref_cnt_;

  rex_crit_sect_type  m_sink_ptr_cs_;
  qtv_msg_sink_class* m_sink_ptr_;
};

class qtv_msg_sink_class
{
public:
  /* dispatch feeds a message into the message sink.  Once dispatch is called,
  ** the sink has accepted ownership of the message and will delete it 
  ** when the time is right. */

  virtual void dispatch( qtv_msg_struct* const msg_ptr ) = 0;

  qtv_msg_sink_class( void );
  virtual ~qtv_msg_sink_class( void );

  /* get_handle does an implicit add_ref on the handle */
  qtv_msg_sink_handle_class* get_handle( void );

private:
  rex_crit_sect_type         m_handle_cs_;
  qtv_msg_sink_handle_class* m_handle_ptr_;
};

/* qtv_task_if_class
**
** Objects of this class receive and dispatch qtv_msg_structs on their own REX
** tasks.
*/

class qtv_task_if_class
{
public:
  typedef uint32 dispatch_id_type;

  qtv_task_if_class( rex_tcb_type*        const task_tcb_ptr,
                     qtv_task_info_struct&      task_info );
  virtual ~qtv_task_if_class( void );

  /* This function can be thought of as an 'indirect dispatch'.  Clients
  ** give the task a sink and a message and let the task dispatch the message
  ** to the sink. Optionally, callers can provide a delay when they want to set
  ** a timer to trigger the dispatch at some time in the future.*/

  dispatch_id_type dispatch( qtv_msg_sink_class* const recipient_ptr,
                             qtv_msg_struct* const msg_ptr,
                             const uint32 delay_ms = 0 );

  /* Request a dispatch which you no longer want delivered?  Use this member
  ** to cancel it.  Best effort will be made to cancel the dispatch, but
  ** if delivery has already begun it can't be halted. */

  void cancel_dispatch( dispatch_id_type dispatch_id );

  /* --- end of public API --- */

  /* These typedefs should really be private, as they're not for
  ** external consumption.  Unfortunately, making them private prevents
  ** them from referencing one another, which sorta ruins it. */

  struct parcel_struct
  {
    qtv_task_if_class*         sender_obj_ptr;
    qtv_msg_sink_handle_class* recipient_ptr;
    qtv_msg_struct*            msg_ptr;
    uint32                     delay_ms;

    void fill( qtv_task_if_class*  const sender_obj_ptr,
               qtv_msg_sink_class* const recipient_ptr,
               qtv_msg_struct*     const msg_ptr,
               uint32              delay_ms );

    void dispatch( void );
  };

  struct link_struct
  {
    q_link_type       link;

    enum
    {
      DISPATCH,
      CANCELLATION
    }
    type;

    union
    {
      parcel_struct    parcel;
      dispatch_id_type dispatch_id;
    };

    /* We provide a specialized new/delete set which uses object pools
    ** to minimize the number of heap calls.  This means you want to use
    ** these functions to control these things...not the QTV macros! */
    static link_struct* malloc( void )
    {
      return QTV_New( link_struct );
    }

    static void free( link_struct* ptr )
    {
       if (ptr != NULL)
       {
          QTV_Delete( ptr );
       }
    }
  };

private:
  rex_tcb_type* const   m_task_tcb_ptr_;
  qtv_task_info_struct& m_task_info_;
};

#endif /* QTV_TASK_IF_H */

