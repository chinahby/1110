#ifndef DIAG_H
#define DIAG_H
/*==========================================================================

                     Diagnostic Task Header File

Description
  Global Data declarations of the diag_task.

Copyright (c) 1991-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                         Edit History

      $Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diag.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/05   sl      Added support for SPC_UNLOCK_TTL to unlock/lock the sp_state
10/17/03   ph      For Rel A builds, redirect to new MC subsystem command.
09/23/03    gr     Function prototypes related to changes that enable more
                   efficient detection of the condition where diag is out of
                   heap space.
08/20/01   jal     Changes to support more Diag packets.  Support for powerup
                   online/offline state, service programming lock state.
04/06/01   lad     Cosmetic changes.
02/23/01   lad     Rearchitected the diagnostics subsystem to be a service 
                   rather than a module.  All coupling to targets has been
                   moved to target-specific implementations.  This file now
                   contains an API.  No other information or coupling remains
                   except for featurized legacy code that impacts existing 
                   targets.
                   Old history comments have been removed since most of them
                   are no longer applicable.

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "qw.h"

/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
/* Diagnostics version (protocol revision).
*/
/* DIAG_DIAGVER                                                            
   Diagnostics version, used to ensure compatibility of the DM and the DMSS.
   1   Original
   2   Changes to status packet and verno packet. Removed verstr packet
   3   for release to factory. Sends RF mode in status packet
   4   Adds release directory to verno packet and dipswitch packets
   5   Many changes in DM.
   6   Added Vocoder PCM and PKT loopback, and Analog IS-55 tests
   7   New FER data in tagraph packet
   8   Streaming protocol enhancements and event reporting.
*/
#ifdef FEATURE_DIAG_NON_STREAMING
#define DIAG_DIAGVER 7
#else
#define DIAG_DIAGVER 8
#endif

/* ---------------------------------------------------------------------------
** Security states.  Some packets are restricted based on security state.
** -------------------------------------------------------------------------*/
#define DIAG_SEC_UNLOCKED 0x00	/* Packet accepted if security is unlocked */
#define DIAG_SEC_LOCKED   0x01	/* Packet accepted if security is locked   */
#define DIAG_SEC_ANY      0xFF	/* Any security */

typedef uint8 diag_security_type;

/* ---------------------------------------------------------------------------
** Service programming states.  Some packets are restricted until a valid
**  Service Programming Code is entered.
** -------------------------------------------------------------------------*/
typedef uint8 diag_sp_type;

#define DIAG_SPC_UNLOCKED 0x00	/* SPC has been entered, valid service   */
			       /* programming code has been entered.    */
#define DIAG_SPC_LOCKED   0x01	/* SP is locked, no valid service        */
			       /* programming code has been entered yet */

#ifdef FEATURE_DIAG_SPC_TTL   /* DIAG_SPC_COUNT is an OEM specific count */
#define DIAG_SPC_UNLOCK_COUNT 10 /* DIAG_SPC_COUNT is the max TTL for an SPC */
#endif

/* This structure is sent to an event listener when an event is processed.
   The implementation relies on the format of this structure.  It is 
   dangerous to change this format of this structure. */
typedef struct
{
  unsigned int event_id;	/* event ID */

  qword ts;			/* 8-byte CDMA time stamp. */

  uint8 length;		/* length of the payload */
  /* Payload of size 'length': */
  uint8 payload[255];		/* payload, if length > 0 */

}
diag_event_type;

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD diag_event_type.payload VARRAY 255 LENGTH diag_event_type.length */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* -------------------------------------------------------------------------
** Function Definitions
** ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================

FUNCTION DIAG_TASK

DESCRIPTION
  This function contains the diagnostic task.  This task supports commun-
  ication with the Diagnostic Monitor, or other external diagnostic devices.
  Each packet received from the DM is processed and replied to, allowing the
  DM to perform diagnostic functions (peek and poke, commanded tests),
  configuration functions (NV item read and write, service programming),
  calibration functions (power adjustment table tweak), and remote handset
  emulation.

  This procedure does not return.

===========================================================================*/
  void diag_task (uint32 param);

/*===========================================================================

FUNCTION DIAG_GET_SECURITY_STATE

DESCRIPTION
  This function returns the current security state of the diagnostics service.
  Some packets may be restricted by security state.  The security state is
  unlocked by sending the Security Password.


===========================================================================*/
  diag_security_type diag_get_security_state (void);

/*===========================================================================

FUNCTION DIAG_GET_SP_STATE

DESCRIPTION
  This function returns the current service programming state of the
  diagnostics service.  Some packets may be restricted by SP state.
  The SP state is unlocked by sending the Service Programming Code.


===========================================================================*/
  diag_sp_type diag_get_sp_state (void);

/*===========================================================================

FUNCTION DIAG_SUSPEND_TASK

DESCRIPTION
  This procedure suspends the DIAG task for 'duration' milliseconds.  This
  routine has no impact on the flow of diagnostic data other than 
  suspending the DIAG task.
   
===========================================================================*/
  void diag_suspend (unsigned int duration);

/*===========================================================================

FUNCTION DIAG_PAUSE

DESCRIPTION
  This procedure suspends the caller's context of the DIAG task for 
  'duration' milliseconds.  
  
  This routine, unlike diag_suspend(), allows the DIAG task to continue
  funcitoning.  DIAG will continue to transmit data while pausing.
  
  If 'enable_rx_processing' is TRUE, the DIAG task will process and dispatch
  inbound packets as well as servicing TX data.  If FALSE, TX only is 
  serviced.
     
===========================================================================*/
  void diag_pause (unsigned int duration, boolean enable_rx_processing);

/*===========================================================================

FUNCTION DIAG_TIMEOUT

DESCRIPTION
  This procedure makes the diag task time out in the event of a detected 
  security breach.  This security measure is intended to prevent automated
  password and SPC hacking.
  
  The diag task flush all outbound data, timeout for 10 seconds, then flush
  the RX queue.
 
===========================================================================*/
  void diag_timeout (void);

/* ==================================================================
FUNCTION DIAG_MALLOC
DESCRIPTION
   Dynamically allocates a specified number of bytes and returns a
   pointer to this chunk of memory.
===================================================================== */
  void *diag_malloc (unsigned long num_bytes);

/* ==================================================================
FUNCTION DIAG_FREE
DESCRIPTION
   Frees a chunk of memory allocated using rex_malloc.
===================================================================== */
  void diag_free (void *mem_ptr);

/* ==================================================================
FUNCTION DIAG_ALLOC_CHECK
DESCRIPTION
   Checks if memory is available in the diag heap. The caller can
   specify an array of allocations to be made, and the function
   returns 0 if one or more of the requested allocations cannot be
   satisfied.
   NOTE: This function uses cached information about the heap to do
   the allocation check. If the cache is dirty, the function returns
   1 without updating the cache. So the user must not assume that all
   allocations will succeed even if this function returns 1.
===================================================================== */
  int diag_alloc_check (unsigned int num_allocs, unsigned int *alloc_sizes);

/* ==================================================================
FUNCTION DIAG_BLOCK
DESCRIPTION
   Blocks the DIAG task until diag_release() is called.  The DIAG 
   task will be suspended completely until diag_release() is called.
===================================================================== */
  void diag_block (void);

/* ==================================================================
FUNCTION DIAG_RELEASE
DESCRIPTION
   Blocks the DIAG task until diag_release() is called.  The DIAG 
   task will be suspended completely until diag_release() is called.
===================================================================== */
  void diag_release (void);

/*===========================================================================

FUNCTION TYPE DIAG_IDLE_PROCESSING_FTYPE

DESCRIPTION
  A function type that, if registered, is called when the DIAG task has 
  idle processing time.

===========================================================================*/
  typedef void (*diag_idle_processing_ftype) (void *param);

/*===========================================================================

FUNCTION DIAG_IDLE_PROCESSING_REGISTER

DESCRIPTION
  This procedure registers a function pointer to be called when the DIAG 
  task has idle processing time.
  
  If multiple entries of the same entry are registered, the funciton will
  be called as many times as it is registered.
  
RETURN VALUE
  A boolean indicating success of the registration.  If no resources 
  are available, FALSE is returned.

===========================================================================*/
  boolean diag_idle_processing_register (diag_idle_processing_ftype fp,
					 void *param);

/*===========================================================================

FUNCTION DIAG_IDLE_PROCESSING_UNREGISTER

DESCRIPTION
  This procedure unregisters the specified function pointer.  The function
  will no longer be called during idle DIAG task time.
  
  This routine matches both 'fp' and 'param'.  'param' is not dereferenced.
  
  If multiple entries exist, this routine will only unregister the first
  entry found.

RETURN VALUE
  A boolean indicating success of the registration.  FALSE indicates the
  specified funciton/param pair are not registered.

===========================================================================*/
  boolean diag_idle_processing_unregister (diag_idle_processing_ftype fp,
					   void *param);

/*===========================================================================

FUNCTION DIAG_TX_NOTIFY

DESCRIPTION
  This routine notifies DIAG that data is ready to be sent without causing
  a context switch immediately.
  
  This eliminates concern that INTLOCK's may be violated from a context 
  switch.
  
RETURN VALUE
  None.

===========================================================================*/
  void diag_tx_notify (void);

/*===========================================================================

FUNCTION TYPE DIAG_EVENT_LISTENER

DESCRIPTION
  This function type is registered via diag_add_event_listener() or
  diag_add_event_range_listener() and is called by the DIAG task when an 
  event ID matching the registered value(s) is generated by DMSS/AMSS.
  
  This routine must not hold the memory very long.  If it is held too long
  it may hinder the performance of diagnostic services.  Memory is owned
  by the DIAG task and is freed when this function returns.
  
  The sequence number is incremented each time a new log is serviced.  This
  is useful to detect duplicated when more than one listener for the same
  type is used.
  
RETURN VALUE
  None.

===========================================================================*/
  typedef void (*diag_event_listener) (uint32 seq_num, 
                                       const diag_event_type * event, 
                                       void *param);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM event POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION DIAG_ADD_EVENT_LISTENER

DESCRIPTION
  This routine registers a function to be called by the DIAG task when the
  specified event is generated by DMSS/AMSS.
  
  'param' will be passed unmodified to the registered function when called.
  
  Duplicate entries are allowed.  
    
RETURN VALUE
  TRUE - listener was successfully added to the listener table.
  FALSE - Listener was not added due to lack of resources.

===========================================================================*/
  boolean diag_add_event_listener (const unsigned int event_id,
				   const diag_event_listener listener,
				   void *param);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION diag_add_event_listener
     CLNTNAME diag_add_event_listener_on_modem */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION DIAG_REMOVE_EVENT_LISTENER

DESCRIPTION
  This routine unregisters a function from the event listener table.
  'listener' will no longer be called when the event ID is 
    
  'event_id', 'listener', and 'param' are used to match a registered 
  listener.
  
  If duplicate entries exist, only one is removed.

RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.

===========================================================================*/
  boolean diag_remove_event_listener (const unsigned int event_id,
				      const diag_event_listener listener,
				      void *param);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION diag_remove_event_listener
     CLNTNAME diag_remove_event_listener_on_modem */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION TYPE DIAG_LOG_LISTENER

DESCRIPTION
  This function type is registered via diag_add_log_listener() or
  diag_listen_for_log_range() and is called by the DIAG task when an 
  log code matching the register value(s) is called.
  
  This routine must not hold the memory very long.  If it is held too long
  it may hinder the performance of diagnostic services.  Memory is owned
  by the DIAG task and is freed when this function returns.
  
  The sequence number is incremented each time a new log is serviced.  This
  is useful to detect duplicated when more than one listener for the same
  type is used.
  
RETURN VALUE
  None.

===========================================================================*/
  typedef void (*diag_log_listener) (uint32 seq_num, const byte *log, 
                                     unsigned int length, void *param);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM log VARRAY LENGTH length */
#endif /* FEATURE_HTORPC_METACOMMENTS */



/*===========================================================================

FUNCTION DIAG_ADD_LOG_LISTENER

DESCRIPTION
  This routine registers a function to be called when the specified log
  is generated by DMSS/AMSS.
  
  'param' will be passed unmodified to the registered function when called.
  
  Duplicate entries are allowed.  
    
RETURN VALUE
  TRUE - listener was successfully added to the listener table.
  FALSE - Listener was not added due to lack of resources.

===========================================================================*/
  boolean diag_add_log_listener (const unsigned int log_code,
				 const diag_log_listener listener,
				 void *param);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION diag_add_log_listener
     CLNTNAME diag_add_log_listener_on_modem */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION DIAG_REMOVE_LOG_LISTENER

DESCRIPTION
  This routine unregisters a function from the log listener table.
  'listener' will no longer be called when the log code is 
    
  'log_code', 'listener', and 'param' are used to match a registered 
  listener.
  
  If duplicate entries exist, only one is removed.

RETURN VALUE
  Boolean indicating the success of the operation.  If FALSE, the entry was
  not found in the listener table.

===========================================================================*/
  boolean diag_remove_log_listener (const unsigned int log_code,
				    const diag_log_listener listener,
				    void *param);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION diag_remove_log_listener
     CLNTNAME diag_remove_log_listener_on_modem */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION TYPE DIAG_CMD_RSP

DESCRIPTION
  This function type is provided by the caller when sending a request packet
  using diag_cmd_req().  After the request is processed, this function is
  called (if specified) with a pointer to the response packet and the handle
  returned from the corresponding diag_cmd_req() call.
  
  Memory is owned by the DIAG task and is freed when this function returns.
  
  'param' is the unmodified value from the corresponding diag_cmd_req() call.
  
RETURN VALUE
  None.

===========================================================================*/
  typedef void (*diag_cmd_rsp) (const byte *rsp, unsigned int length, 
                                void *param);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM rsp VARRAY LENGTH length */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION DIAG_CMD_REQ

DESCRIPTION
  This routine injects the given request packet to the diagnostics service.
  The callback function, if specified, will be called with the response packet
  and 'param' (unmodified).
        
RETURN VALUE
  Boolean indicating success of enquing the command.

===========================================================================*/
  boolean diag_cmd_req (const byte *req, unsigned int length, 
                        const diag_cmd_rsp rsp_cb, void *param);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION diag_cmd_req
     CLNTNAME diag_cmd_req_on_modem */
/*~ PARAM req VARRAY LENGTH length */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#if defined(FEATURE_DIAG_COUPLED_ARCHITECTURE)
/* -------------------------------------------------------------------------
** The rest of this file contains entry points and definitions needed by
** IS95/IS2000/HDR targets.
** ------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------
** Definitions of Analog and CDMA operating states of the phone
** ------------------------------------------------------------------------- */
#define  DIAG_NO_SVC_S       0x00

/* (A)nalog States
*/
#define  DIAG_A_INIT_S       0x01
#define  DIAG_A_IDLE_S       0x02
#define  DIAG_A_VC_INIT_S    0x03
#define  DIAG_A_WFO_S        0x04
#define  DIAG_A_WFA_S        0x05
#define  DIAG_A_CONV_S       0x06
#define  DIAG_A_RELEASE_S    0x07
#define  DIAG_A_SA_S         0x08

/* CDMA (D)igital States 
*/
#define  DIAG_D_INIT_S           0x81
#define  DIAG_D_IDLE_S           0x82
#define  DIAG_D_VC_INIT_S        0x83
#define  DIAG_D_WFO_S            0x84
#define  DIAG_D_WFA_S            0x85
#define  DIAG_D_CONV_S           0x86
#define  DIAG_D_RELEASE_S        0x87
#define  DIAG_D_OVHD_S           0x88
#define  DIAG_D_ORIG_S           0x89
#define  DIAG_D_PAGE_RES_S       0x8a
#define  DIAG_D_ORD_RES_S        0x8b
#define  DIAG_D_REG_S            0x8c
#define  DIAG_D_MSG_S            0x8d
#define  DIAG_D_USE_SUBSYS_CMD_S 0x8e

#ifdef FEATURE_DIAG_COUPLED_ARCHITECTURE
#ifdef FEATURE_HDR
#error code not present
#endif

/* Sigs that are used by clients in IS95/IS2000/HDR targets.  These are 
   copied from diagi.h. */
#define DIAG_NV_CMD_SIG           0x00000004

//TODO: Remove all references to this from DMSS.
#define DIAG_OFFLINE_COMPLETE_SIG 0	/* This signal has been deprecated */

#define DIAG_COMM_OPEN_SIG        0x00000800	/* Signal that SIO is availabe
						   to be opened for use.       */

#define DIAG_SIO_OPEN_SIG DIAG_COMM_OPEN_SIG

#endif


/*===========================================================================

FUNCTION DIAG_INC_EVENT_CNT

DESCRIPTION
  This function increments a count of events that took place in the phone
  which might imply that the phone had changed enough that other kinds of
  status might want to be requested from diag by external devices.

===========================================================================*/
  void diag_inc_event_cnt (void);

#endif				/* FEATURE_DIAG_COUPLED_ARCHITECTURE */

#if defined(FEATURE_DIAG_DS_SIO_COUPLING)
#include "sio.h"
/*===========================================================================

FUNCTION DIAG_RECORD_DM_BAUDRATE

DESCRIPTION
  This procedure is called when a bad SPC is processed.  The Diag task will
  time out for several seconds.
 
===========================================================================*/
  void diag_record_dm_baudrate (sio_bitrate_type bitrate);
#endif				/* !defined(FEATURE_DIAG_DS_SIO_COUPLING) */

#if defined(FEATURE_DIAG_UI_BAUD)
#include "sio.h"
/*===========================================================================

FUNCTION DIAG_CHANGE_DEFAULT_SIO_BITRATE

DESCRIPTION
  This function changed the default DIAG bitrate as well as change the 
  current operating bitrate.

===========================================================================*/
  void diag_change_default_sio_bitrate (sio_bitrate_type new_baud);
#endif				/* FEATURE_DIAG_UI_BAUD */

#if defined (FEATURE_OTASP) || defined (FEATURE_UIM_RUIM)
/*===========================================================================

FUNCTION DIAG_OTASP_UPDATE

DESCRIPTION
  This procedure tells diag to refresh its status cache from NV.

===========================================================================*/
  void diag_otasp_update (void);
#endif				/* defined (FEATURE_OTASP) || defined (FEATURE_UIM_RUIM) */

#ifdef __cplusplus
}
#endif

#endif				/* DIAG_H  */

