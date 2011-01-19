/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         T A S K   M A I N   C O N T R O L L E R   N V   I N T E R F A C E

GENERAL DESCRIPTION

  This file makes up the NV-Interface component of the TMC.
  
  The TMC NV-Interface component encapsulates all the functionality that is
  associated with reading and writing from/to NV.    

EXTERNALIZED FUNCTIONS

  TMC NV-Interface
    tmcnv_init                          Initialize TMC NV-Interface.
    tmcnv_read                          Read a item from nv.
    tmcnv_write                         Write a item to nv.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

  tmcnv_init() must be call to initialize this component before any other
  function declared in this component is called.
 
Copyright (c) 2002 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header:   

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/02   jqi     Initial Release

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE                     

===========================================================================*/

#include "rex.h"                                        /* REX header file */
#include "nv.h"                                          /* NV header file */

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* Type for holding the information that is associated with the TMC
** NV-Interface component.
*/
typedef struct {
  
  /* Rex signal to be used by TMC for waiting on NV to completes a read/write
  ** operation.
  */
  rex_sigs_type             nv_sig;
  
  
  /* Rex signal to be used by TMC for watchdog reporting while TMC waits on NV
  ** to complete a read/write operation.
  */
  rex_sigs_type             dog_sig;
  
                                   
  /* Pointer to a watchdog reporting function to be called by TMC when the
  ** watchdog sig is set while TMC waits on NV to complete a read/write
  ** operation.
  */
  void                      (*dog_rpt_func_ptr) (void);
  
} tmcnv_s_type;


/* TMC-Interface item.
*/
tmcnv_s_type  tmcnv;


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
================================== TMCNV INIT ===============================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION tmcnv_init

DESCRIPTION
  Initialize the TMC NV-Interface component.

  This function must be called before the NV interface block
  is being used in any way, read or write.

DEPENDENCIES
  This function must be called before any other tmcnv_xxx() function is
  ever called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void                             tmcnv_init(

        rex_sigs_type             nv_sig,
            /* Rex signal to be used by TMC for waiting on NV to completes a
            ** read/write operation.
            */
        
        rex_sigs_type             dog_sig,
            /* Rex signal to be used by TMC for watchdog reporting while TMC
            ** waits on NV to complete a read/write operation.
            */
        
        void                      (*dog_rpt_func_ptr) (void)
            /* Pointer to a watchdog reporting function to be called by TMC
            ** when the watchdog sig is set while TMC waits on NV to complete
            ** a read/write operation.
            */
)            
{
  tmcnv_s_type         *nv_ptr = &tmcnv;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  /* Initialize the TMC NV-Interface sigs and callback function.
  */
  nv_ptr->nv_sig              = nv_sig;
  nv_ptr->dog_sig             = dog_sig;
  nv_ptr->dog_rpt_func_ptr    = dog_rpt_func_ptr;
  
} /* tmcnv_init */


/* <EJECT> */
/*===========================================================================

FUNCTION tmcnv_wait

DESCRIPTION
  This function suspends tmc task from executing until NV completes
  the read/write operation.

DEPENDENCIES
  tmcnv_init has been called.
  
RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void    tmcnv_wait( void )
{
  tmcnv_s_type     *nv_ptr       =   &tmcnv;
  rex_sigs_type    received_sigs;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop while waiting for the NV signal to be set.
  */
  do
  {
    /* Wait on the NV and watchdog signals.
    */
    received_sigs = rex_wait( (rex_sigs_type) (nv_ptr->nv_sig | nv_ptr->dog_sig));

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the watchdog signal is set, call on the watchdog reporting function
    ** that was provided by the tmc_task.
    */
    if( received_sigs & nv_ptr->dog_sig )
    { 
      nv_ptr->dog_rpt_func_ptr();
    }

  } while( ! (received_sigs & nv_ptr->nv_sig) );

} /* tmcnv_wait */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
================================== TMCNV READ ===============================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION tmcnv_read

DESCRIPTION
  Read a item from nv.
  
  Note that the NV read is done in a synchronous fashion. That is, this
  function only returns after the NV task completes the read operation.
  During this time, it kicks the watch dog periodically.

DEPENDENCIES
  tmcnv_init has been called to initialize the TMC NV-Interface.

RETURN VALUE
  nv_stat_enum_type - nv read status is returned.

SIDE EFFECTS
  None
  
===========================================================================*/
nv_stat_enum_type                tmcnv_read (
        
       nv_items_enum_type         nv_item,
            /* NV item to read.
            */

        nv_item_type              *data_ptr
            /* Pointer to a data buffer where to store the data that is
            ** associated with the NV item.
            */
)
{
  tmcnv_s_type         *nv_ptr   = &tmcnv;
  nv_cmd_type          tmc_nv_cmd;

  /* Prepare the NV read command buffer.
  */
  tmc_nv_cmd.item       = nv_item;           /* item to read */
  tmc_nv_cmd.cmd        = NV_READ_F;         /* NV operation */
  tmc_nv_cmd.data_ptr   = data_ptr;          /* buffer to store read data  */
  tmc_nv_cmd.tcb_ptr    = rex_self();        /* Notify this task when done */
  tmc_nv_cmd.sigs       = nv_ptr->nv_sig; /* Signal with this sig when done */
  tmc_nv_cmd.done_q_ptr = NULL;              /* Return cmd to NO Q when done */

  /* Read to NV by:
  **
  ** 1. Clearing the NV signal.
  ** 2. Queuing the read command to the NV task.
  ** 3. Waiting for the NV task to finish servicing the read command.
  */
  (void) rex_clr_sigs( rex_self(), nv_ptr->nv_sig );
  nv_cmd( &tmc_nv_cmd );                
  tmcnv_wait();
  (void) rex_clr_sigs( rex_self(), nv_ptr->nv_sig );

  return tmc_nv_cmd.status;

} /* tmcnv_read */

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
================================== TMCNV WRITE ==============================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION tmcnv_write

DESCRIPTION
  Write a item from nv.
  
  Note that the NV read is done in a synchronous fashion. That is, this
  function only returns after the NV task completes the write operation.
  During this time, it kicks the watch dog periodically.

DEPENDENCIES
  tmcnv_init has been called to initialize the TMC NV-Interface.

RETURN VALUE
  nv_stat_enum_type - nv write status is returned.

SIDE EFFECTS
  None
  
===========================================================================*/
nv_stat_enum_type                tmcnv_write (
        
       nv_items_enum_type         nv_item,
            /* NV item to write.
            */

        nv_item_type              *data_ptr
            /* Pointer to a data buffer where to store the data that is
            ** associated with the NV item.
            */
)
{
  tmcnv_s_type         *nv_ptr   = &tmcnv;
  nv_cmd_type          tmc_nv_cmd;

  /* Prepare the NV write command buffer.
  */
  tmc_nv_cmd.item       = nv_item;           /* item to write */
  tmc_nv_cmd.cmd        = NV_WRITE_F;         /* NV operation */
  tmc_nv_cmd.data_ptr   = data_ptr;          /* buffer to store write data  */
  tmc_nv_cmd.tcb_ptr    = rex_self();        /* Notify this task when done */
  tmc_nv_cmd.sigs       = nv_ptr->nv_sig; /* Signal with this sig when done */
  tmc_nv_cmd.done_q_ptr = NULL;              /* Return cmd to NO Q when done */

  /* Write to NV by:
  **
  ** 1. Clearing the NV signal.
  ** 2. Queuing the write command to the NV task.
  ** 3. Waiting for the NV task to finish servicing the write command.
  */
  (void) rex_clr_sigs( rex_self(), nv_ptr->nv_sig );
  nv_cmd( &tmc_nv_cmd );                
  tmcnv_wait();
  (void) rex_clr_sigs( rex_self(), nv_ptr->nv_sig );

  return tmc_nv_cmd.status;

} /* tmcnv_write */

