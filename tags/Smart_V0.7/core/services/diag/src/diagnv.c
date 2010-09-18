/*==========================================================================
 
 General Description
   This modules contain implemnetation of diag_nv_read & diag_nv_write.
   
 Copyright (c) 2003 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

  $Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diagnv.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/05   as      Fixed lint errors.
04/04/05   as      Modified MEID code to use DIAG_MEID_ENABLE
03/30/05   as      Replaced FEATURE_NV_MEID_ENABLE with
                   FEATURE_DIAG_MEID_ENABLE.
03/10/05   ht      Added code for NV_MEID_I.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
09/05/02   lad     Integrated into first MSMSHARED_DIAG release.
11/11/01   as      Created file.
                   
===========================================================================*/

#include "comdef.h"
#include "customer.h"

//#include "diagcmd.h"
#include "diagdiag.h"
#include "diagi.h"
#include "diagnv.h"
#include "msg.h"

#include "nv.h"
#include "task.h"
   

static nv_cmd_type ncmd;  /* Command buffer for NV interface */

#ifndef FEATURE_DIAG_COUPLED_ARCHITECTURE
static
#endif
#if defined (FEATURE_DIAG_CACHE) || defined (FEATURE_DIAG_COUPLED_ARCHITECTURE)
nv_item_type nitem; /* NV Item buffer for NV interface */
#endif

/*===========================================================================

FUNCTION DIAGNV_CMD_INIT

DESCRIPTION
  Initializes the NV command parameter block used to communicate
  with the NV task.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
 
===========================================================================*/
void diagnv_cmd_init (void) {

  static int ncmd_is_inited = FALSE;

  if( !ncmd_is_inited ) {

    ncmd_is_inited = TRUE;  /* Do this only once */
                  
    (void) q_link( &ncmd, &ncmd.link );

    ncmd.tcb_ptr = &diag_tcb;    /* Always signal diag            */
    ncmd.sigs = DIAG_NV_CMD_SIG;  /* Always reply through this sig */
    ncmd.done_q_ptr = NULL;       /* Not using links               */
  }

} /* diagnv_cmd_init() */



/*===========================================================================

FUNCTION DIAG_NV_READ

DESCRIPTION
  This procedure encapsulates an NV read operation for the diag task.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diag_nv_read ( 
  nv_items_enum_type item,              /* Which NV item to read */
  PACKED void        *data_ptr          /* Where to put the data */
)

{
  diagnv_cmd_init();

  ncmd.cmd = NV_READ_F;
  
  if (data_ptr == NULL) {  
    return NV_BADPARM_S;  /* Error */
  }

  if (ncmd.tcb_ptr != &diag_tcb) {
    return NV_BADPARM_S;  /* Error */
  }
  ncmd.data_ptr = (nv_item_type *) data_ptr;
  ncmd.item = item;

  (void) rex_clr_sigs( &diag_tcb, DIAG_NV_CMD_SIG );
  
  nv_cmd( &ncmd);

  (void) rex_wait( DIAG_NV_CMD_SIG );
  return ( ncmd.status );
} /* diag_nv_read */ 



/*===========================================================================

FUNCTION DIAG_NV_WRITE
DESCRIPTION
  This procedure encapsulates an NV write operation for the diag task.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diag_nv_write ( 
  nv_items_enum_type item,
  PACKED void *data_ptr
)

{
  diagnv_cmd_init();

  ncmd.cmd = NV_WRITE_F;

  if( data_ptr == NULL) {  
    return NV_BADPARM_S;  /* Error */
  }

  if(ncmd.tcb_ptr != &diag_tcb ) {
    return NV_BADPARM_S;  /* Error */
  }

  ncmd.data_ptr = (nv_item_type *) data_ptr;
  ncmd.item = item;

  (void) rex_clr_sigs( &diag_tcb, DIAG_NV_CMD_SIG );
  nv_cmd( &ncmd);

  (void) rex_wait( DIAG_NV_CMD_SIG );
  return ( ncmd.status );
} /* diag_nv_write */ 



/*=========================================================================*/
nv_stat_enum_type diagnv_peek (nv_peek_type *peek)
{
  if (!peek)
  {  
    return NV_BADPARM_S;  /* Error */
  }
  else
  {
    diagnv_cmd_init ();

    ncmd.cmd = NV_PEEK_F;

    ncmd.data_ptr = (nv_item_type *) peek;

    (void) rex_clr_sigs (&diag_tcb, DIAG_NV_CMD_SIG);
    
    nv_cmd (&ncmd);

    (void) rex_wait (DIAG_NV_CMD_SIG);
    return (ncmd.status);
  }
} /* diagnv_peek */ 



/*=========================================================================*/
nv_stat_enum_type diagnv_poke (nv_poke_type *poke)
{
  if (!poke)
  {  
    return NV_BADPARM_S;  /* Error */
  }
  else
  {
    diagnv_cmd_init ();

    ncmd.cmd = NV_POKE_F;

    ncmd.data_ptr = (nv_item_type *) poke;

    (void) rex_clr_sigs (&diag_tcb, DIAG_NV_CMD_SIG);
    
    nv_cmd (&ncmd);

    (void) rex_wait (DIAG_NV_CMD_SIG);
    return (ncmd.status);
  }
} /* diagnv_poke */ 


/*--------------------------------------------------------------------------
  Regional (to diag) buffers for nv information that is read at initializa-
  tion from NV.
--------------------------------------------------------------------------*/
#ifdef FEATURE_DIAG_CACHE

#ifndef FEATURE_DIAG_COUPLED_ARCHITECTURE
static
#endif
  diag_nv_cache_type  diagnv;  /* Cached version of NV data */
#endif

#ifdef FEATURE_DIAG_CACHE
/*===========================================================================*/
void diagnv_refresh_cache (void)
{
#ifdef FEATURE_IS2000
    byte          i, j;
#endif


  MSG_LOW("Refreshing DIAG NV cache.", 0L, 0L, 0L);

  if ( diag_nv_read ( NV_ESN_I, &nitem ) != NV_DONE_S ) {
    MSG_HIGH("Failed to read ESN", 0, 0, 0);
  } else {
    diagnv.esn = nitem.esn.esn;
  }

#ifdef FEATURE_IS2000

  if (diag_nv_read (NV_CURR_NAM_I, &nitem) != NV_DONE_S)
  {
    MSG_HIGH("Failed to read CURR_NAM", 0, 0, 0);
  } else
  {
    diagnv.current_nam = nitem.curr_nam;
  }

  for (i = 0; i < NV_MAX_NAMS; i++ )
  {
    nitem.min1.nam = i;

    if (diag_nv_read (NV_MIN1_I, &nitem) != NV_DONE_S)
    {
      MSG_HIGH ("Failed to read MIN1 for NAM%d", i, 0, 0);
    } else
    {
      for (j = 0; j < NV_MAX_MINS; j++)
      {
        diagnv.min1[i][j] = nitem.min1.min1[j];
      }
    }

    nitem.min2.nam = i;

    if (diag_nv_read (NV_MIN2_I, &nitem) != NV_DONE_S)
    {
      MSG_HIGH ("Failed to read MIN2 for NAM%d", i, 0, 0);
    } else
    {
      for (j = 0; j < NV_MAX_MINS; j++)
      {
        diagnv.min2[i][j] = nitem.min2.min2[j];
      }
    }
  }
#endif /* FEATURE_IS2000 */

} /* diag_nv_refresh_cache */

#endif /* FEATURE_DIAG_CACHE */


/*===========================================================================*/
dword diagnv_get_esn (void)
{
#ifdef FEATURE_IS2000
  static dword esn = 0;

  if (!esn)
  {
    /* No ESN or not initialized.  Read it from NV. */
    if (diag_nv_read (NV_ESN_I, (void *) &esn) != NV_DONE_S)
    {
      esn = 0;
    }
  }
  
  return esn;
#else
  return 0xDEADD00DU; /* Dummy ESN value */
#endif
}


#ifdef DIAG_MEID_ENABLE
/*===========================================================================

FUNCTION DIAGNV_GET_MEID

DESCRIPTION
  This function retrieves the Mobile Equipment IDentifier (MEID) from NV.
  
DEPENDENCIES
  None.

RETURN VALUE
  NV Status

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diagnv_get_meid (qword meid)
{

  nv_item_type  nv_item;  /* NV Item buffer for NV interface */

  nv_stat_enum_type nvstatus;  /* NV Status */

  nvstatus = diag_nv_read (NV_MEID_I, &nv_item);

  if (nvstatus == NV_DONE_S)
  {
    /* read successful, copy the meid */
    qw_equ(meid, nv_item.meid);
  }
  else
  {
    /* read was not successful */
    MSG_HIGH("MEID READ Failed code = %d", nvstatus,0,0);
  }
  return nvstatus;

}
#endif /* DIAG_MEID_ENABLE */

