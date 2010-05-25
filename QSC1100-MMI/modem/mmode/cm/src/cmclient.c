/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             C A L L   M A N A G E R   C L I E N T   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Client Interface Block

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmclient_list_init() must be call to initialize the client list object
  before any other cmclient_list_xxx() function declared in this module is
  being called.


Copyright (c) 1991 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmclient.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/09   mp      Updated SUPS event notification in SUSPEND state
04/02/09   sv      Updated MBMS event notification for apps power collapse
11/26/08   jd      Fix compilation warnings
11/14/08   jd      Fix for Lint Errors
09/11/08   rn      bcmcs_info_ptr checks to remove lint warning
08/28/08   aj      client_ptr NULL checks in event_ntfy APIs
07/28/08   rm      Fixing compiler warnings
07/01/08   rm      Mainlining of MBMS changes
06/26/08   ak      Fix for compiler warnings
06/11/08   sv      Updated bit_is_any_bit_on to report correct result
                   Updated power collapse buffered events notification
                   to send the events, instead of queuing a command
05/27/08   sv      Add new power collapse bitmask, to buffer any events
                   during Power Collapse State
03/31/08   ks      Add new CM client critical section
03/26/08   ks      Avoid negative value used as an index of cmclient_map_arr array
06/14/07   pk/cl   Added support for power suspend
04/03/07   sk      Made changes to asynchronously free the client object and
                   and remove it from the client list.
03/29/07   ic      In cm_client_init() replaced INRANGE() with BETWEEN()
03/12/07   rp      Fixing compilation issue with nas_power_collapse_bit_arr
03/09/07   sk      RAM usage reduction.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/18/07   rp      Added support for WMS<->CM<->NAS interface.
11/07/06   sk      Memory reduction effort.
07/17/06   ic      Print ID and type of the client to whom CM sends an event.
                   For consistency, moved setting of requesting_client_id from
                   cmclient_list_call_event_ntfy() to cm_client_call_event_ntfy().
                   Added setting of requesting_client_id for BCMCS and NAS
                   events
03/15/06   sk      Decoupled client type and client id.
                   Added support for multiple client ids per client type.
                   Client objects allocated from TMC when all static buffers
                   are used up.
03/13/06   sk      Added two new client types:
                   - CM_CLIENT_TYPE_GSDI
                   - CM_CLIENT_TYPE_BREW_APPS
03/09/06   pk      Added support for POWER COLLAPSE
02/02/06   pk      Corrected the client id for CM_CLIENT_TYPE_PBM
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ka      Adding new client type CM_CLIENT_TYPE_PBM.
01/06/06   ic      Lint cleanup  
12/10/05   ic      Lint cleanup
12/01/05   ic      In cmclient_get_client_ptr() do not search for client 
                   structure for commands queued with CM_CLIENT_ID_ANONYMOUS
                   and not flag an error when such client id is passed into 
                   the function 
11/18/05   ic      Added CM_CLIENT_TYPE_OEMTELEPHONE, clients to check for  
                   compile flag CM_API_CLIENT_TYPE_OEMTELEPHONE
11/07/05   ic      Added CM_CLIENT_TYPE_MFLO, clients to check for compile 
                   flag CM_API_CLIENT_TYPE_MFLO
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
09/14/05   pk      Removed CMCLIENT_POOL_SIZE definition
09/13/05   pk      Added CM_CLIENT_TYPE_DS_BCMCS client type
02/09/10   dk      Added RM_INTERFACE_CLIENT type.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
12/05/04   ic      Added cmclient_get_client_ptr_by_type()
12/03/04   ws      Support for Remote WMS.
11/15/04   ic      Lint cleanup with CM_DEBUG on
08/12/04   ws      Remove cm20 support.
08/02/04   ic      CM Lint clean up - cmclient.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/21/04   sj      Added support for umts to 1x handover.
07/20/04   ic      Fixed ADS 1.2 compiler warning
07/14/04   ws      Add client id in event generations.
07/12/04   ic      Removed assertions from cmclient_get_client_ptr() so I can
                   test in CUTE whether CM reports appropriate error for
                   invalid client id.
06/08/04   ank     Mainlined FEATURE_GPSONE_DBM.
06/04/04   ka      Fixed Lint errors
06/02/04   ws      Added camera as a client type.
03/18/04   sj      Reset the init_mask when queues are initialized.
03/15/04   ka      Merged changes from 6250 branch
                   Added checks to prevent double client init.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
11/20/03   ic      Added support for:
                   CM Commands:
                   - CM_PD_CMD_FIX_START
                   - CM_PD_CMD_FIX_END
                   CM Events:
                   - CM_PD_EVENT_FIX_START
                   - CM_PD_EVENT_FIX_END
                   Log events:
                   - EVENT_CM_BLOCK_HDR_ORIG_DURING_GPS
                   - EVENT_CM_ALLOW_HDR_ORIG_DURING_GPS
09/30/03   vt      Modified to support common cm.h across all targets.
05/15/03   ic      Removed FEATURE_MONITOR / FEATURE_JCDMA_MONITOR
04/28/03   AT      Lint cleanup.
04/11/03   prk     Removed references to FEATURE_SUPS.
02/27/03   at      Added support for CM_CLIENT_TYPE_GSTK.
02/19/03   ATM     Added DIAG client
01/29/03   AT      Added support for CAT_APP type
01/21/03   ws      Updated copyright information for 2003
11/14/02   vt      Merged changes from GW mainline.
11/13/02   ic      More changes to monitor object interface - added
                   CM_MONIT_CMD_CLIENT_ACT command
11/13/02   ic      Changes to monitor object interface.
10/30/02   vt      Fixed compiler errors.
10/22/02   ic      Fixed compile error under FEATURE_MONITOR
                   (a field changed from client to client_id)
09/12/02   kar     Fixed compile error
09/10/02   ws      Added initial cm20 backward compatibility.
08/30/02   vt      Made sms, pd, monit, & sups registration functions return
                   error status if proper fearures are not defined.
08/06/02   HQ      Added FEATURE_CDSMS.
05/05/02   vt      Fixed compile problem with new mm cm.h
04/22/02   RI      Added ATCOP as CM client.
04/17/02   prk     Removed call to cm_cmd_client_act in function cm_client_act.
                   Client now needs to call cm_ph_cmd_get_ph_info to get ph info.
02/13/02   ic      Compile errors fixes related to the fact that call_id field
                   was removed from cm_inband_cmd_s_type (not needed, we use
                   call_id field in cm_inband_cmd__info_s_type instead).
                   Removed const modifier from cm_inband_cmd_s_type  *inband_cmd_ptr
                   function parameter (ripple effect of removing call_id from
                   cm_inband_cmd_s_type structure)
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                   Added parameter in call to cm_cmd_client_act due to
                   changes in serving system group (FEATURE_NEWSS).
10/15/01   RI      Added support for Supplementary Services ( GSM/WCDMA ).
08/23/01   HD      Lint clean up.
08/21/01   ic      Main-lined FEATURE_NEWCM
04/27/01   HD      Added changes for WCDMA/GSM mode.
03/27/01   RC      Remove CM_ASSERT(FALSE) form cmclient_get_client_ptr.
03/13/01   VT      Added 3 more floating clients.
02/28/01   HQ      Fixed complier warnings.
02/26/01   sko     Added monitor call back entry initialization.
02/23/01   HQ      Fixed problem of PD event registration.
01/29/01   HQ      Properly feature-ized UASMS, GPSONE_DBM and NEWCM.
01/23/01   ych     Merged JCDMA features.
07/27/00   PJ      Added support for PD commands/events. (FEATURE_GPSONE_DBM).
08/23/00   vt      Added new CM interface changes.
08/31/99   SH      Modified codes to featurize UASMS
08/06/99   SH      Added SMS support
07/27/99   SH      Removed using sel_ptr in the q_link_type to access client data structure
06/30/99   SH      Modified cm_client_init to allocate memory for client
06/21/99   SH      Added return value to cm_client_act and cm_client_deact
05/31/99   SH      Added CM 2.0 features
01/07/99   RC      Changes per ARM compiler.
11/24/98   RC      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "queue.h"     /* Interface to queue services */
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */
#include "rex.h"       /* Interface to rex services */
#include "cmclient.h"  /* Interface to this file */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmph.h"      /* Interface to the Phone object */
#include "cmsms.h"     /* Interface to SMS object */
#include "cmxdbm.h"    /* Interface to DBM object */
#include "cmwsups.h"   /* Interface to SUPS object */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                             FREE CMCLIENT QUEUE
---------------------------------------------------------------------------*/


#ifdef FEATURE_CM_LOW_MEM_TARGET
#define CM_CLIENT_IDS_MAX ((int) CM_CLIENT_TYPE_END_LEGACY )
/* CM_CLIENT_TYPE_END_LEGACY is used to stay within the memory constraints of the
** low memory targets. It can be changed in the future to reflect the
** actual numver of clients. Also, if we run out of the array, we go
** to the TMC heap.
*/
#define CM_CLIENT_ARR_MAX   CM_CLIENT_TYPE_END_LEGACY 
static cm_client_s_type  cmclient_arr[CM_CLIENT_ARR_MAX];
#else
#define CM_CLIENT_IDS_MAX ((int) CM_CLIENT_TYPE_MAX + 40)
static cm_client_s_type  cmclient_arr[ (int) CM_CLIENT_TYPE_MAX + 2 ];
#endif
    /* RAM pool of cmclient objects*/

static q_type            cmclient_free_q;
    /* Queue to manage free cmclient objects  */

/*---------------------------------------------------------------------------
                             CLIENT ID TO PTR MAP LIST
---------------------------------------------------------------------------*/

typedef struct cm_client_map_s {

    cm_client_type_e_type client_type;
        /* Client Type */

    cm_client_id_type     client_id;
        /* Client Id */

    cm_client_s_type      *client_ptr;
        /* Pointer to client object */

} cm_client_map_s_type;

/* The client id is nothing but the index of the array. It is included in
** the table to accomodate a more sophisticated scheme in the future.
*/

static cm_client_map_s_type cmclient_map_arr[ CM_CLIENT_IDS_MAX ];

/*---------------------------------------------------------------------------
                                 CLIENT LIST
---------------------------------------------------------------------------*/

/* Client list object (singular). The client list
** object is responsible for maintaining a list of
** CM clients and for forwarding CM events to them.
*/
static struct {

    q_type  queue;
        /* Q item to manage the client list */

} client_list;


/* Macro to get a pointer to the client list object.
**
** NOTE! This macro should only be used within the
** client list object scope.
*/
#define CLIENT_LIST_PTR() ( &client_list )


/* Macro to get a pointer the queue element within
** the client list object. NOTE! This macro should
** only be used within the client list object scope.
*/
#define CLIENT_LIST_Q_PTR() ( &(CLIENT_LIST_PTR()->queue) )

/* Macro to check if the power collapse notify preference of a client equals
** CM_PWR_COLLAPSE_NOTIFY_NULL. For targets without power collapse feature,
** this evaluates to FALSE (so as to not affect any || conditions ).
*/
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else
#define IS_PWR_COLLAPSE_NTFY_PREF_NULL(x) (/*lint -e{506,774,831} */ FALSE)
#endif




/*===========================================================================
                          FUNCTION PROTOTYPES
===========================================================================*/

static void cmclient_list_add( cm_client_s_type *client_ptr );

/*===========================================================================

FUNCTION cmclient_map_init

DESCRIPTION
  Initialize the client map array (cmclient_map_arr).

  This function must be called before the cmclient_map_arr can be used.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_map_init( void )
{
  unsigned int i;

  CM_FUNC_START( "cmclient_map_init(), free client maps=%d",
                 ARR_SIZE(cmclient_map_arr), 0, 0 );

  for(i = 0; i < ARR_SIZE(cmclient_map_arr); i++)
  {
    cmclient_map_arr[i].client_type = CM_CLIENT_TYPE_NONE;
    cmclient_map_arr[i].client_id   = CM_CLIENT_ID_ANONYMOUS;
    cmclient_map_arr[i].client_ptr  = NULL;
  }

}

/*===========================================================================

FUNCTION cmclient_free_q_init

DESCRIPTION
  Initialize the client free queue.

  This function must be called before the cmclinet free queue is used.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_free_q_init( void )
{
  unsigned int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmclient_free_q_init(), free cmclient=%d",
                 ARR_SIZE(cmclient_arr), 0, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the free cmclient queue.
  */
  (void)q_init( &cmclient_free_q );

  /* Link and place each of the cmclient buffer
  ** into the free command queue.
  */
  for ( i=0; i < ARR_SIZE(cmclient_arr); i++ )
  {
    (void) q_link( &cmclient_arr[i], &cmclient_arr[i].link );
    q_put( &cmclient_free_q, &cmclient_arr[i].link );
    cmclient_arr[i].init_mask = (byte)(~CM_INIT_MASK);
  }

}

/*===========================================================================

FUNCTION cmclient_get_client_buf

DESCRIPTION
  Allocate a client object.

  This function must be called after cmclient_free_q_init is called.

DEPENDENCIES
  none

RETURN VALUE
  Address to allocated object, or NULL if out of memory.

SIDE EFFECTS
  none

===========================================================================*/
static cm_client_s_type* cmclient_get_client_buf( void )
{
  cm_client_s_type *client_ptr;

  /* Try to get a buffer from the statically allocated array.
  */
  client_ptr = (cm_client_s_type *) q_get(&cmclient_free_q);

  /* Get a buffer from TMC heap if the statically allocated array is full.
  */
  if(client_ptr == NULL)
  {
    client_ptr = mem_malloc(&tmc_heap, sizeof(cm_client_s_type));
  }

  return client_ptr;
}

/*===========================================================================

FUNCTION cmclient_free_client_buf

DESCRIPTION
  Free a client object.

  This function must be called after cmclient_free_q_init is called.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_free_client_buf( cm_client_s_type *client_ptr )
{
  CM_ASSERT( client_ptr != NULL );

  if( client_ptr >= &cmclient_arr[0] &&
      client_ptr <= &cmclient_arr[ARR_SIZE(cmclient_arr)-1]
    )
  {
    q_put( &cmclient_free_q, &(client_ptr->link) );
  }
  else
  {
    mem_free( &(tmc_heap), client_ptr);
  }
}

/*---------------------------------------------------------------------------
                               BIT ARRAY
---------------------------------------------------------------------------*/

/*lint -save -e702 -e734 */

/*===========================================================================


FUNCTION bit_arr_set_bits

DESCRIPTION
  Takes a pointer to a bit array (bit_arr_ptr), a starting bit position
  (start_bit) and an ending bit position (end_bit) and set to ON or OFF
  all the bits between the start_bit and the end_bit, including the start
  and the end bit positions.

  Note that bits are ordered from left to right; that is, bit 0 is
  the most left bit of the bit array.

DEPENDENCIES
  Specified bit array must have already been initialized with
  bit_arr_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif // FEATURE_APPS_POWER_COLLAPSE

/*===========================================================================


FUNCTION bit_arr_set_bits

DESCRIPTION
  Takes a pointer to a bit array (bit_arr_ptr), a starting bit position
  (start_bit) and an ending bit position (end_bit) and set to ON or OFF
  all the bits between the start_bit and the end_bit, including the start
  and the end bit positions.

  Note that bits are ordered from left to right; that is, bit 0 is
  the most left bit of the bit array.

DEPENDENCIES
  Specified bit array must have already been initialized with
  bit_arr_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void bit_arr_set_bits(

    bit_arr_s_type  *bit_arr_ptr,
        /* pointer to a bit array item */

    boolean         is_set_bits_on,
        /* if TRUE set bits to ON, else set bits to OFF */

    int             start_bit,
        /* start from this bit, including */

    int             end_bit
        /* end at this bit, including */
)
/*lint -esym(818, bit_arr_ptr) 
** Pointer parameter 'bit_arr_ptr' (line 357) could be declared as pointing to const
** ... yeah right
*/
{
  int i;

  /* Two mask tables to isolate any contiguous stream of bits
  ** with in a byte.
  **
  ** E.g. to isolate bits 2-5 do start_mask_tbl[2] & end_mask_tbl[5]
  ** which is equal to (0011:1111 & 1111:1100) which is equal to 0011:1100.
  */
  static const byte start_mask_tbl[] =
                          { 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01 };

  static const byte end_mask_tbl[] =
                          { 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Mask to mask 3 LSB's - i.e. 3 most right bits
  */
  #define LSB3_MASK  (0x07)

  /* Get the start and end byte bases.
  */
  const byte start_base = start_bit >> 3;
  const byte end_base   = end_bit   >> 3;

  /* Get the start and end bits offsets.
  */
  const byte start_offset = start_bit & LSB3_MASK;
  const byte end_offset   = end_bit   & LSB3_MASK;

  /* Get the start and end masks.
  */
  const byte start_mask = start_mask_tbl[start_offset];
  const byte end_mask   = end_mask_tbl  [end_offset];

  byte *arr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check some assertions.
  */
  CM_ASSERT( bit_arr_ptr != NULL );
  CM_ASSERT( bit_arr_ptr->bit_buf != NULL );
  CM_ASSERT( 0         <= start_bit  );
  CM_ASSERT( start_bit <= end_bit);
  CM_ASSERT( end_bit   < bit_arr_ptr->bit_size );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the start and bits are within allowable range.
  */
  if( ! (0         <= start_bit             &&
         start_bit <= end_bit               &&
         end_bit   <  bit_arr_ptr->bit_size) )
  {
    ERR_FATAL("Bits out of range, s_bit=%d, e_bit=%d, size=%d",
               start_bit, end_bit, bit_arr_ptr->bit_size );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point a byte pointer at the actual bit buffer, so we do not
  ** need to make any assumption of the bit buffer type.
  */
  arr_ptr = (byte*) bit_arr_ptr->bit_buf;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we need to set bits to ON, do the following:
  */
  if( is_set_bits_on )
  {
    /* If stream of bits to set spans over more than one byte,
    ** (e.g. bits 6-12), do the following:
    */
    if( start_base < end_base )
    {
      arr_ptr[start_base] |= start_mask;
      for( i=start_base+1; i<end_base; i++ ) arr_ptr[i] = 0xFF;
      arr_ptr[end_base]   |= end_mask;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else, stream of bits to set contained with in a single byte
    ** (e.g. 2-6) - do the following:
    */
    else
    {
      arr_ptr[start_base] |= ( start_mask & end_mask );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (we need to set bits to OFF), do the following:
  */
  else
  {
    /* If stream of bits to set spans over more than one byte,
    ** (e.g. bits 6-12), do the following:
    */
    if( start_base < end_base )
    {
      arr_ptr[start_base] &= ~start_mask;
      for( i=start_base+1; i<end_base; i++ ) arr_ptr[i] = 0x00;
      arr_ptr[end_base]   &= ~end_mask;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else, stream of bits to set contained with in a single byte
    ** (e.g. 2-6) - do the following:
    */
    else
    {
      arr_ptr[start_base] &= ~( start_mask & end_mask );
    }
  }
}
/*lint +esym(818, bit_arr_ptr) */

/*===========================================================================

FUNCTION bit_arr_is_bit_on

DESCRIPTION
  Takes a pointer to a bit array (bit_arr_ptr) and a bit position
  (bit) and check whether the bit in this position is ON.

  Note that bits are ordered from left to right; that is, bit 0 is
  the most left bit of the bit array.

DEPENDENCIES
  Specified bit array must have already been initialized with
  bit_arr_init().

RETURN VALUE
  TRUE if the bit in the specified position is ON, else return FALSE.

SIDE EFFECTS
  none

===========================================================================*/
static boolean bit_arr_is_bit_on(

    const bit_arr_s_type    *bit_arr_ptr,
        /* pointer to a bit array item */

    int                     bit
        /* indicating which bit to check */
)
{

  /* Mask table to isolate any specific bit with in a byte.
  **
  ** E.g. to isolate bit 2 do mask_tbl[2] which is equal to 0010:0000
  */
  static const byte bit_mask_tbl[] =
                          { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

  /* Get the byte base.
  */
  const byte base = bit >> 3;

  /* Get the bit offset
  */
  const byte offset = bit & LSB3_MASK;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( bit_arr_ptr != NULL );
  CM_ASSERT( bit_arr_ptr->bit_buf != NULL );
  CM_ASSERT( bit >= 0 && bit < bit_arr_ptr->bit_size );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the specified bit is within allowable range.
  */
  if( bit < 0 || bit >= bit_arr_ptr->bit_size )
  {
    ERR_FATAL("Out of range bit=%d",bit,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else
  {
    /* Point a byte pointer at the actual bit buffer, so we do not
    ** need to make any assumption of the bit buffer type.
    */
    byte *arr_ptr = (byte*) bit_arr_ptr->bit_buf;

    /* Return TRUE if specified bit is ON.
    */
    if( arr_ptr[ base ] & bit_mask_tbl[ offset ] )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the bit is OFF -- return FALSE.
  */
  return FALSE;

}
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */

/*===========================================================================

FUNCTION bit_arr_init

DESCRIPTION
  Initializing a bit array object.

  This function must be called on a bit array object before
  it is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void bit_arr_init(

    bit_arr_s_type  *bit_arr_ptr,
        /* pointer to a bit array to be initialized */

    byte            *bit_buf_ptr,
        /* pointer to a bit buffer */

    int             bit_size
        /* bit size of bit buffer */
)
{

  CM_ASSERT( bit_arr_ptr != NULL );
  CM_ASSERT( bit_buf_ptr != NULL );
  CM_ASSERT( bit_size    > 0 );
  CM_FUNC_START( "bit_arr_init(), bit_size=%d", bit_size,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set bit_buf to point at supplied bit buffer,
  ** and set bit_size to supplied buffer size.
  */
  bit_arr_ptr->bit_buf  = bit_buf_ptr;
  bit_arr_ptr->bit_size = bit_size;

  /* Set all bits off.
  */
  bit_arr_set_bits( bit_arr_ptr, /* for this bit array */
                    FALSE,       /* set bits to OFF */
                    0,           /* start from bit 0 */
                    bit_size-1); /* end with the last bit */
}



#ifdef CM_DEBUG
#error code not present
#endif /* CM_DEBUG */



/*---------------------------------------------------------------------------
                                  CM CLIENT
---------------------------------------------------------------------------*/

/*lint -save -e574 -e737 */

/*===========================================================================

FUNCTION cm_client_init

DESCRIPTION
  Initializing a client object.

  This function can be called more than once with the same client type. Each
  call will result in an ADDITIONAL client id being allocated for the client
  type. It's upto the caller to keep track of the several client ids that
  they own and use them appropriately.

  Note that in order to get notified of CM events a client still needs
  to do the following:
  1. Register its callback functions and specific events with each functional group
  2. Activate its client object with cm_client_act()

DEPENDENCIES
  none

RETURN VALUE
  request staus

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_client_init(
    cm_client_type_e_type           client_type,
            /* Client type */
    cm_client_id_type               *client_id_ptr
            /* Client ID pointer*/
)
{   
  int index = CM_CLIENT_ID_ANONYMOUS;

  cm_client_s_type  *client_ptr = NULL;

  /*----------------------------------------------------------------------*/

  CM_FUNC_START( "cm_client_init()", 0,0,0 );

  CM_ASSERT(INRANGE(client_type, CM_CLIENT_TYPE_NONE, CM_CLIENT_TYPE_MAX));
  CM_ASSERT( client_type != CM_CLIENT_TYPE_END_LEGACY );

  if ( client_id_ptr == NULL )
  {
      return CM_CLIENT_ERR_CLIENT_ID_PTR;
  }

  /* The same client type can now be used to register several times. Each
  ** registration will result in an additional client id being allocated for
  ** the client type.
  */

  /* If the client type is one of the legacy ones, try to get the client id
  ** from those that are reserved for the legacy client types.
  */
  rex_enter_crit_sect(cmclient_crit_sect_ptr());
  if(BETWEEN(client_type, CM_CLIENT_TYPE_NONE, CM_CLIENT_TYPE_END_LEGACY))
  {
    if ( cmclient_map_arr[client_type].client_ptr == NULL )
    {
      index = (int) client_type;
    }
  }
  
  #ifndef FEATURE_CM_LOW_MEM_TARGET
  /* If the previous attempt at finding a client id fails, search for client
  ** ids from those that are not reserved for the legacy client types.
  */
  if(index == CM_CLIENT_ID_ANONYMOUS)
  {
    int i;
    for (i = (int) CM_CLIENT_TYPE_END_LEGACY; 
        i < ARR_SIZE(cmclient_map_arr);
        i++)
    {
        /* If a free slot is found in the client map array.
        */
        if ( cmclient_map_arr[i].client_ptr == NULL )
        {
            index = i;
            break;
        }
    }
  }
  #endif
  
  rex_leave_crit_sect(cmclient_crit_sect_ptr());
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(index == CM_CLIENT_ID_ANONYMOUS)
  {
    return CM_CLIENT_ERR_CLIENT_INIT_FAIL;
  }

  client_ptr = cmclient_get_client_buf();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(client_ptr == NULL)
  {
      return  CM_CLIENT_ERR_CLIENT_INIT_FAIL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the client map.
  */
  cmclient_map_arr[index].client_ptr  = client_ptr;
  cmclient_map_arr[index].client_type = client_type;
  cmclient_map_arr[index].client_id   = index;
  *client_id_ptr = index;

  /* Mark this object as initialized.
  */
  client_ptr->init_mask = CM_INIT_MASK;


  /* Initially client is inactive.
  */
  client_ptr->is_active = FALSE;


  /* Initialize the link element*/

  (void) q_link( client_ptr, &client_ptr->link );


  /* Set client ID to the address of this client object
  */
  client_ptr->client = client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize call bit array.
  */
  bit_arr_init( &client_ptr->call_bit_arr,       /* call bit array struct */
                client_ptr->call_bit_buf,                   /* bit buffer */
                (8 * ARR_SIZE(client_ptr->call_bit_buf)) ); /* bit size   */

  /* Initialize phone bit array.
  */
  bit_arr_init( &client_ptr->ph_bit_arr,        /* phone bit array struct */
                client_ptr->ph_bit_buf,                     /* bit buffer */
                (8 * ARR_SIZE(client_ptr->ph_bit_buf)) );   /* bit size   */

  /* Initialize serving system bit array.
  */
  bit_arr_init( &client_ptr->ss_bit_arr, /*serving system bit array struct*/
                client_ptr->ss_bit_buf,                     /* bit buffer */
                (8 * ARR_SIZE(client_ptr->ss_bit_buf)) );   /* bit size   */

  /* Initialize inband bit array.
  */
  bit_arr_init( &client_ptr->inband_bit_arr, /*inband bit array struct*/
                client_ptr->inband_bit_buf,                     /* bit buffer */
                (8 * ARR_SIZE(client_ptr->inband_bit_buf)) );   /* bit size   */

 /* Initialize SMS bit array.
  */
  bit_arr_init( &client_ptr->sms_bit_arr, /*sms bit array struct*/
                client_ptr->sms_bit_buf,                     /* bit buffer */
                (8 * ARR_SIZE(client_ptr->sms_bit_buf)) );   /* bit size   */   

  #ifdef FEATURE_GPSONE
#error code not present
  #endif /* FEATURE_GPSONE */

  #ifdef FEATURE_DBM
  /* Initialize the DBM event bit array
   */
  bit_arr_init( &client_ptr->dbm_event_bit_arr, /* DBM event bit array struct */
                client_ptr->dbm_event_bit_buf,               /* bit buffer */
                (8 * ARR_SIZE(client_ptr->dbm_event_bit_buf)) ); /* bit size   */

  /* Initialize the DBM class bit array
   */
  bit_arr_init( &client_ptr->dbm_class_bit_arr, /* DBM class bit array struct */
                client_ptr->dbm_class_bit_buf,               /* bit buffer */
                (8 * ARR_SIZE(client_ptr->dbm_class_bit_buf)) ); /* bit size   */

  #endif /* FEATURE_DBM */

  #if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA))
#error code not present
  #endif /* FEATURE_GSM || FEATURE_WCDMA */

  #if ((defined FEATURE_BCMCS) || (defined FEATURE_MODEM_MBMS))
  /* Initialize BCMCS bit array.
  */
  bit_arr_init( &client_ptr->bcmcs_bit_arr,         /* BCMCS bit array struct */
                client_ptr->bcmcs_bit_buf,                     /* bit buffer */
                (8 * ARR_SIZE(client_ptr->bcmcs_bit_buf)) );   /* bit size   */

  #endif /* FEATURE_BCMCS || MBMS */

  #if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA))
#error code not present
  #endif   /* FEATURE_GSM || FEATURE_WCDMA */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
  #endif /* FEATURE_APPS_POWER_COLLAPSE */
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initially set all callback function pointers
  ** to point to NULL
  */
  client_ptr->call_event_func             = NULL;
  client_ptr->ph_event_func               = NULL;
  client_ptr->ss_event_func               = NULL;
  client_ptr->inband_event_func           = NULL;
  client_ptr->sms_event_func              = NULL;
  client_ptr->call_info_list_func         = NULL;

  #if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA))
#error code not present
  #endif

  #if (defined FEATURE_BCMCS || defined FEATURE_MODEM_MBMS)
  client_ptr->bcmcs_event_func            = NULL;
  #endif

  client_ptr->call_cmd_err_func           = NULL;
  client_ptr->ph_cmd_err_func             = NULL;
  client_ptr->ss_cmd_err_func             = NULL;
  client_ptr->call_info_list_cmd_err_func = NULL;
  client_ptr->inband_cmd_err_func         = NULL;
  #if (defined FEATURE_BCMCS || defined FEATURE_MODEM_MBMS)
  client_ptr->bcmcs_cmd_err_func          = NULL;
  #endif
  #ifdef FEATURE_CDSMS
  client_ptr->sms_cmd_err_func            = NULL;
  #endif
  #ifdef FEATURE_GPSONE
#error code not present
  #endif /* FEATURE_GPSONE */

  #ifdef FEATURE_DBM
  client_ptr->dbm_cmd_err_func            = NULL;
  client_ptr->dbm_event_func              = NULL;
  #endif /* FEATURE_DBM */

  #if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA))
#error code not present
  #endif

  /* Initialize the power collapse variables
  */
  client_ptr->client_processor            = CM_PROCESSOR_MODEM;

  /* After all initialization is done, put the client object in the client
  ** list.
  */
  cmclient_list_add( client_ptr );

  return CM_CLIENT_OK;

}

/*===========================================================================

FUNCTION cm_client_release

DESCRIPTION
  Release a client object.

  This function should be called if a client doesn't need CM anymore.
  This call will release the client object. That object can be reused for
  another client. If the client needs to use CM again, it should call
  cm_client_init function to establish connection.

DEPENDENCIES
  none

RETURN VALUE
  request staus

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_client_release(

    cm_client_id_type               client_id
        /* Client ID pointer*/
)
{
    int i;

    /*---------------------------------------------------------------------*/

    CM_FUNC_START( "cm_client_release()", 0,0,0 );

    /*
    Acertain if the client object is already released. If yes, return
    failure. Otherwise, put it on the queue.
    */

    for ( i = 0; i < ARR_SIZE( cmclient_map_arr ); i++ )
    {
        if ( cmclient_map_arr[i].client_id == client_id )
        {
            if (cmclient_map_arr[i].client_ptr == NULL)
            {
                return CM_CLIENT_ERR_CLIENT_ID_PTR;
            }
            else
            {
                CM_MSG_HIGH( "cm_client_release(): id=%d, type=%d",
                             cmclient_map_arr[i].client_id,
                             cmclient_map_arr[i].client_type, 0 );

                /* Set up the client object for asynch freeing.
                */
                cmph_cmd_client_release(cmclient_map_arr[i].client_ptr);
                rex_enter_crit_sect(cmclient_crit_sect_ptr());
                cmclient_map_arr[i].client_ptr  = NULL;
                cmclient_map_arr[i].client_type = CM_CLIENT_TYPE_NONE;
                cmclient_map_arr[i].client_id   = CM_CLIENT_ID_ANONYMOUS;
                rex_leave_crit_sect(cmclient_crit_sect_ptr());
                return CM_CLIENT_OK;
            }
        }
    }

    return  CM_CLIENT_ERR_CLIENT_ID_PTR;

}



/*===========================================================================

FUNCTION cmclient_get_client_ptr

DESCRIPTION

  Given client id, return a pointer to client object

DEPENDENCIES
  None

RETURN VALUE
  A pointer to client object or NULL if not found

SIDE EFFECTS
  None

===========================================================================*/
cm_client_s_type* cmclient_get_client_ptr(

    cm_client_id_type client_id
    /* Client Id */
)
{
    int i;

    /* Do not look for client structure for commands queued with
    ** CM_CLIENT_ID_ANONYMOUS, as by definition, there is none
    */ 
    if(client_id == CM_CLIENT_ID_ANONYMOUS)
    {
      CM_MSG_MED("CM_CLIENT_ID_ANONYMOUS, no client struct",0,0,0);
      return NULL; 
    }

    /*---------------------------------------------------------------------*/

    for ( i = 0; i < ARR_SIZE( cmclient_map_arr ); i++ )
    {
        if ( client_id == cmclient_map_arr[i].client_id )
        {
            return cmclient_map_arr[i].client_ptr;
        }
    }

    CM_ERR("Bad client id - %d", client_id,0,0);
    return NULL;
}

/*===========================================================================

FUNCTION cmclient_get_client_ptr_by_type

DESCRIPTION

  Given client type, return a pointer to client object

DEPENDENCIES
  None

RETURN VALUE
  A pointer to client object or NULL if not found

SIDE EFFECTS
  None

===========================================================================*/
cm_client_s_type* cmclient_get_client_ptr_by_type(

    cm_client_type_e_type client_type
    /* Client type */
)
{
    int i;

    /*---------------------------------------------------------------------*/

    for ( i = 0; i < ARR_SIZE( cmclient_map_arr ); i++ )
    {
        if ( client_type == cmclient_map_arr[i].client_type )
        {
            return cmclient_map_arr[i].client_ptr;
        }
    }

    CM_ERR("Bad client type - %d", client_type,0,0);
    return NULL;
}


/*===========================================================================

FUNCTION cmclient_get_client_id

DESCRIPTION
  Returns client id that corresponds to a client object

DEPENDENCIES
  None

RETURN VALUE
  client id

SIDE EFFECTS
  None

===========================================================================*/
cm_client_id_type cmclient_get_client_id(

    const cm_client_s_type *client_ptr
    /* Client object pointer */
)
{
    int i;

    /*---------------------------------------------------------------------*/

    for ( i = 0; i < ARR_SIZE( cmclient_map_arr ); i++ )
    {
        if ( client_ptr == cmclient_map_arr[i].client_ptr )
        {
            return cmclient_map_arr[i].client_id;
        }
    }

    /* This catches bogus client_ptr */
    CM_ASSERT(FALSE);
    return CM_CLIENT_ID_ANONYMOUS;
}



/*===========================================================================

FUNCTION cmclient_get_client_type

DESCRIPTION
  Returns client id that corresponds to a client object

DEPENDENCIES
  None

RETURN VALUE
  client id

SIDE EFFECTS
  None

===========================================================================*/
cm_client_type_e_type cmclient_get_client_type(

    cm_client_id_type client_id
    /* Client object pointer */
)
/*lint -save -e539 */
{
  if(client_id == CM_CLIENT_ID_ANONYMOUS)
  {
    #ifdef CM_DEBUG
#error code not present
    #endif
    return CM_CLIENT_TYPE_NONE;
  }
 
  if(client_id < ARR_SIZE(cmclient_map_arr) &&
     cmclient_map_arr[client_id].client_ptr != NULL &&
     cmclient_map_arr[client_id].client_id  == client_id)
  {
    return cmclient_map_arr[client_id].client_type;
  }
    
  /* This catches bogus client_id */
  CM_ASSERT(FALSE);
  return CM_CLIENT_TYPE_MAX;
}
/*lint -restore */

/*===========================================================================

FUNCTION cm_client_act

DESCRIPTION
  Activate the client object, so that the client will be notified
  of call manager events (for which it is registered).

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  Add the client object to the client list.

===========================================================================*/
cm_client_status_e_type cm_client_act(

    cm_client_id_type          client
    /* Client ID */
)
{

  cm_client_s_type          *client_ptr;

  client_ptr = cmclient_get_client_ptr( client );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != CM_INIT_MASK) )
  {
      return CM_CLIENT_ERR_CLIENT_ID;
  }

  CM_FUNC_START( "cm_client_act() client_id=%p",
                client,0,0 );


  /* If client is NOT already active,
  ** add the client to the client list
  ** and indicate that client is now active,
  */
  if( !client_ptr->is_active )
  {
    client_ptr->is_active = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, we are trying to activate a client that is already active.
  ** This is not a problem except that it might indicate client confusion.
  */
  else
  {
    CM_MSG_HIGH( "Client already active! client_id=%p",client_ptr,0,0 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform CM that the client activated itself.
  */

  /* need to add some error handling if
     there is no buffer to notify phone
     object regarding this client activation SH */

  cm_ph_cmd_client_act( NULL, NULL, client );

  return CM_CLIENT_OK;

}



/*===========================================================================

FUNCTION cm_client_deact

DESCRIPTION
  Deactivate a client object, so that the client will NOT be notified
  of call manager events (for which it is registered).

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  Remove the client object from the client list.

===========================================================================*/
cm_client_status_e_type cm_client_deact(

    cm_client_id_type          client
    /* Client ID */
)
{

  cm_client_s_type          *client_ptr;
  client_ptr = cmclient_get_client_ptr( client );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != CM_INIT_MASK) )
  {
      return CM_CLIENT_ERR_CLIENT_ID;
  }



  CM_FUNC_START( "cm_client_deact() client_id=%p",
                client,0,0 );


  /* If client is active,
  ** delete the client from the client list
  ** and indicate that client is NOT active,
  */
  if( client_ptr->is_active )
  {
    client_ptr->is_active = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, we are trying to deactivate a client that is already inactive.
  ** This is not a problem except that it might indicate client confusion.
  */
  else
  {
    CM_MSG_HIGH( "Client already inactive! client_id=%p",client_ptr,0,0 );
  }


  return CM_CLIENT_OK;


}

/*===========================================================================

FUNCTION cm_client_call_event_ntfy

DESCRIPTION
  Notify a client of a specified call event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_call_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_call_event_e_type       call_event,
        /* notify client of this call event */

    cm_mm_call_info_s_type     *call_info_ptr
        /* pointer to call state information struct */
)
/*lint -esym(818, client_ptr) 
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH("client_ptr NULL",0,0,0);
    return;
  }
  CM_ASSERT( BETWEEN(call_event, CM_CALL_EVENT_NONE, CM_CALL_EVENT_MAX) );
  CM_ASSERT( call_info_ptr != NULL );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status()  == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             || 
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->call_power_collpase_notify_pref) ) )
  {
    if( client_ptr->call_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->call_bit_arr, (int)call_event) )
    {
      call_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED("call evt %d ct %d cid id %d",
                  call_event,
                  cmclient_get_client_type(call_info_ptr->requesting_client_id),
                  call_info_ptr->requesting_client_id);
      client_ptr->call_event_func( call_event, call_info_ptr );
    }

    return;
  }
  #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
  #endif
}/* cm_client_call_event_ntfy */
/*lint +esym(818, client_ptr) */ 


/*===========================================================================

FUNCTION cm_client_ph_event_ntfy

DESCRIPTION
  Notify a client of a specified phone event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_ph_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_ph_event_e_type         ph_event,
        /* notify client of this phone event */

    cm_ph_info_s_type          *ph_info_ptr
        /* pointer to phone state information struct */
)
/*lint -esym(818, client_ptr) 
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH("client_ptr NULL",0,0,0);
    return;
  }
  CM_ASSERT( BETWEEN(ph_event, CM_PH_EVENT_NONE, CM_PH_EVENT_MAX) );
  CM_ASSERT( ph_info_ptr != NULL );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             || 
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->ph_power_collpase_notify_pref) ) )
  {
    if( client_ptr->ph_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->ph_bit_arr, (int)ph_event) )
    {
      ph_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED("ph evt %d ct %d cid id %d",
                  ph_event,
                  cmclient_get_client_type(ph_info_ptr->requesting_client_id),
                  ph_info_ptr->requesting_client_id);
      client_ptr->ph_event_func( ph_event, ph_info_ptr );
    }
    return;
  }
  #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
  #endif
} /* cm_client_ph_event_ntfy */
/*lint +esym(818, client_ptr) */ 

#if ((defined FEATURE_BCMCS) || (defined FEATURE_MODEM_MBMS))
/*===========================================================================

FUNCTION cm_client_bcmcs_event_ntfy

DESCRIPTION
  Notify a client of a specified BCMCS event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_bcmcs_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_bcmcs_event_e_type      bcmcs_event,
        /* notify client of this BCMCS event */

    cm_bcmcs_info_s_type       *bcmcs_info_ptr
        /* pointer to BCMCS information struct */
)
/*lint -esym(715,client_ptr) */
/*lint -esym(715,bcmcs_event) */
/*lint -esym(715,bcmcs_info_ptr) */
/*lint -esym(818, bcmcs_info_ptr) */
/*lint -esym(818, client_ptr) */ 
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH("client_ptr NULL",0,0,0);
    return;
  }
  CM_ASSERT( BETWEEN(bcmcs_event, CM_BCMCS_EVENT_NONE, CM_BCMCS_EVENT_MAX) );

  if (bcmcs_info_ptr == NULL )
  {
    CM_MSG_HIGH("bcmcs_info_ptr NULL",0,0,0);
    return;
  }

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             || 
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->bcmcs_power_collpase_notify_pref) ) )
  {
    if( client_ptr->bcmcs_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->bcmcs_bit_arr, (int)bcmcs_event) )
    {
      bcmcs_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED("bcmcs evt %d ct %d cid id %d",
                  bcmcs_event,
                  cmclient_get_client_type(bcmcs_info_ptr->requesting_client_id),
                  bcmcs_info_ptr->requesting_client_id);
      client_ptr->bcmcs_event_func( bcmcs_event, bcmcs_info_ptr );
    }
    return;
  }
  #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
  #endif /* FEATURE_APPS_POWER_COLLAPSE */
 
} /* cm_client_bcmcs_event_ntfy */
/*lint +esym(715,client_ptr) */
/*lint +esym(818, client_ptr) */ 
/*lint +esym(715,bcmcs_event) */
/*lint +esym(715,bcmcs_info_ptr) */
/*lint +esym(818, bcmcs_info_ptr) */ 
#endif /* FEATURE_BCMCS || MBMS */

/*===========================================================================

FUNCTION cm_client_ss_event_ntfy

DESCRIPTION
  Notify a client of a specified serving system event if the client is
  registered for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_ss_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_ss_event_e_type         ss_event,
        /* notify client of this serving system event */

    cm_mm_ss_info_s_type       *ss_info_ptr
        /* pointer to serving system information struct */
)
/*lint -esym(818, client_ptr) 
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH("client_ptr NULL",0,0,0);
    return;
  }
  CM_ASSERT( BETWEEN(ss_event, CM_SS_EVENT_NONE, CM_SS_EVENT_MAX) );
  CM_ASSERT( ss_info_ptr != NULL );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             || 
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->ss_power_collpase_notify_pref) ) )
  {
    if( client_ptr->ss_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->ss_bit_arr, (int)ss_event) )
    {
      ss_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED("ss evt %d ct %d cid id %d",
                  ss_event,
                  cmclient_get_client_type(ss_info_ptr->requesting_client_id),
                  ss_info_ptr->requesting_client_id);
      client_ptr->ss_event_func( ss_event, ss_info_ptr );
    }
    return;
  }

  #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
  #endif
} /* cm_client_ss_event_ntfy */
/*lint +esym(818, client_ptr) */ 

/*===========================================================================

FUNCTION cm_client_inband_event_ntfy

DESCRIPTION
  Notify a client of a specified inband event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_inband_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_inband_event_e_type      inband_event,
        /* notify client of this inband event */

    cm_inband_info_s_type      *inband_info_ptr
        /* pointer to inband information struct */
)
/*lint -esym(818, client_ptr) 
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH("client_ptr NULL",0,0,0);
    return;
  }
  CM_ASSERT( BETWEEN(inband_event, CM_INBAND_EVENT_NONE, CM_INBAND_EVENT_MAX) );
  CM_ASSERT( inband_info_ptr != NULL );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             || 
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->inband_power_collpase_notify_pref) ) )
  {
    if( client_ptr->inband_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->inband_bit_arr, (int)inband_event) )
    {
      inband_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED("ib evt %d ct %d cid id %d",
                  inband_event,
                  cmclient_get_client_type(inband_info_ptr->requesting_client_id),
                  inband_info_ptr->requesting_client_id);
      client_ptr->inband_event_func( inband_event, inband_info_ptr );
    }
    return;
  }
  #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
  #endif
} /* cm_client_inband_event_ntfy */
/*lint +esym(818, client_ptr) */ 


/*===========================================================================

FUNCTION cm_client_sms_event_ntfy

DESCRIPTION
  Notify a client of a specified sms event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_sms_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_sms_event_e_type        sms_event,
        /* notify client of this sms event */

    cm_sms_info_s_type         *sms_info_ptr
        /* pointer to sms information struct */
)
/*lint -esym(818, client_ptr) 
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH("client_ptr NULL",0,0,0);
    return;
  }
  CM_ASSERT( BETWEEN(sms_event, CM_SMS_EVENT_NONE, CM_SMS_EVENT_MAX) );
  CM_ASSERT( sms_info_ptr != NULL );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             || 
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->sms_power_collpase_notify_pref) ) )
  {
    if( client_ptr->sms_event_func != NULL &&
          bit_arr_is_bit_on(&client_ptr->sms_bit_arr, (int)sms_event) )
    {
      sms_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED("sms evt %d ct %d cid id %d",
                  sms_event,
                  cmclient_get_client_type(sms_info_ptr->requesting_client_id),
                  sms_info_ptr->requesting_client_id);
      client_ptr->sms_event_func( sms_event, sms_info_ptr );
    }
    return;
  }
  #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
  #endif

} /* cm_client_sms_event_ntfy */
/*lint +esym(818, client_ptr) */ 

#ifdef FEATURE_GPSONE
#error code not present
#endif


#ifdef FEATURE_DBM
/*===========================================================================

FUNCTION cm_client_dbm_event_ntfy

DESCRIPTION
  Notify a client of a specified dbm event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_dbm_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_dbm_event_e_type         dbm_event,
        /* notify client of this dbm event */

    cm_dbm_info_s_type         *dbm_info_ptr
        /* pointer to dbm information struct */
)
/*lint -esym(818, client_ptr) 
** Pointer parameter 'client_ptr' could be declared as pointing to const
** ... yeah right
*/
{
  int dbm_class = (int)CM_DBM_CLASS_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (client_ptr == NULL)
  {
    CM_MSG_HIGH("client_ptr NULL",0,0,0);
    return;
  }
  CM_ASSERT( BETWEEN(dbm_event, CM_DBM_EVENT_NONE, CM_DBM_EVENT_MAX) );
  CM_ASSERT( dbm_info_ptr != NULL );
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (dbm_info_ptr->burst_type)
  {
    case CAI_POSITION_DET:
      dbm_class = (int)CM_DBM_CLASS_PD;
      break;

    case CAI_SHORT_DATA_BURST:
      dbm_class = (int)CM_DBM_CLASS_SDB;
      break;

    default:
      CM_ERR("Unexpected burst type %d", dbm_info_ptr->burst_type, 0, 0);
      return;
  }

  /* If the apps processor is in Power collapse state, or if this is modem client
  ** check if the client
  ** is registered for this event, if the client is registered for this event
  ** call the call back function
  */

  if( cmph_dem_get_apps_pwr_status() == CM_APPS_PWR_STATUS_PWR_RESTORE ||
      client_ptr->client_processor    == CM_PROCESSOR_MODEM             || 
      ( cmph_dem_get_apps_pwr_status()  != CM_APPS_PWR_STATUS_PWR_SUSPEND &&
      IS_PWR_COLLAPSE_NTFY_PREF_NULL(client_ptr->dbm_power_collpase_notify_pref) ) )
  {
    if( client_ptr->dbm_event_func != NULL &&
        bit_arr_is_bit_on(&client_ptr->dbm_event_bit_arr, (int)dbm_event) &&
        bit_arr_is_bit_on(&client_ptr->dbm_class_bit_arr, (int)dbm_class))
    {
      dbm_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
      CM_MSG_MED("dbm evt %d ct %d cid id %d",
                  dbm_event,
                  cmclient_get_client_type(dbm_info_ptr->requesting_client_id),
                  dbm_info_ptr->requesting_client_id);
      client_ptr->dbm_event_func( dbm_event, dbm_info_ptr );
    }
    return;
  }
  #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
  #endif
}/* cm_client_dbm_event_ntfy */
/*lint +esym(818, client_ptr) */ 

#endif


#if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
#endif

#if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA))
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */



/*===========================================================================

FUNCTION cm_client_call_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified call command error code iff the client is
  registered to be notified of OTHER clients call command errors and the
  client is NOT the one originally sending this call command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_call_cmd_err_ntfy(

    const cm_client_s_type      *client_ptr,
        /* pointer to a client struct */

    const cm_call_cmd_s_type    *call_cmd_ptr,
        /* pointer to call command struct */

    cm_call_cmd_err_e_type      call_cmd_err
        /* call command error code */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( call_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN(call_cmd_err, CM_CALL_CMD_ERR_NONE, CM_CALL_CMD_ERR_MAX) );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr->client != call_cmd_ptr->client_ptr )
  {
    if( client_ptr->call_cmd_err_func != NULL )
    {
      client_ptr->call_cmd_err_func( call_cmd_ptr->info.call_id,
                                     call_cmd_ptr->client_id,
                                     call_cmd_ptr->cmd,
                                     call_cmd_err
                                    );
    }
  }
}


/*===========================================================================

FUNCTION cm_client_ph_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified phone command error code iff the client is
  registered to be notified of OTHER clients phone command errors and the
  client is NOT the one originally sending this phone command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_ph_cmd_err_ntfy(

    const cm_client_s_type    *client_ptr,
        /* pointer to a client struct */

    const cm_ph_cmd_s_type    *ph_cmd_ptr,
        /* pointer to phone command struct */

    cm_ph_cmd_err_e_type      ph_cmd_err
        /* phone command error code */
)
{
  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( ph_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN((int)ph_cmd_err, (int)CM_PH_CMD_ERR_NONE, (int)CM_PH_CMD_ERR_MAX) );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr->client != ph_cmd_ptr->client_ptr )
  {
    if( client_ptr->ph_cmd_err_func != NULL )
    {
      client_ptr->ph_cmd_err_func( ph_cmd_ptr->client_id,
                                   ph_cmd_ptr->cmd,
                                   ph_cmd_err );
    }
  }
}
  #if ((defined FEATURE_BCMCS) || (defined FEATURE_MODEM_MBMS))
/*===========================================================================

FUNCTION cm_client_bcmcs_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified phone command error code iff the client is
  registered to be notified of OTHER clients phone command errors and the
  client is NOT the one originally sending this phone command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_bcmcs_cmd_err_ntfy(

    const cm_client_s_type    *client_ptr,
        /* pointer to a client struct */

    const cm_bcmcs_cmd_s_type    *bcmcs_cmd_ptr,
        /* pointer to phone command struct */

    cm_bcmcs_cmd_err_e_type      bcmcs_cmd_err
        /* phone command error code */
)
{
  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( bcmcs_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN((int)bcmcs_cmd_err, (int)CM_BCMCS_CMD_ERR_NONE, (int)CM_BCMCS_CMD_ERR_MAX) );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr->client != bcmcs_cmd_ptr->client_ptr )
  {
    if( client_ptr->bcmcs_cmd_err_func != NULL )
    {
      client_ptr->bcmcs_cmd_err_func( bcmcs_cmd_ptr->client_id,
                                   bcmcs_cmd_ptr->cmd,
                                   bcmcs_cmd_err );
    }
  }
}
#endif /* FEATURE_BCMCS  || MBMS */

#if defined (FEATURE_MODEM_MBMS)
#error code not present
#endif

/*===========================================================================

FUNCTION cm_client_ss_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified serving system command error code iff the client is
  registered to be notified of OTHER clients serving system command errors and the
  client is NOT the one originally sending this serving system command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_ss_cmd_err_ntfy(

    const cm_client_s_type      *client_ptr,
        /* pointer to a client struct */

    const cm_ss_cmd_s_type      *ss_cmd_ptr,
        /* pointer to ss command struct */

    cm_ss_cmd_err_e_type        ss_cmd_err
        /* call command error code */
)
{
  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( ss_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN(ss_cmd_err, CM_SS_CMD_ERR_NONE, CM_SS_CMD_ERR_MAX) );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr->client != ss_cmd_ptr->client_ptr )
  {
    if( client_ptr->ss_cmd_err_func != NULL )
    {
      client_ptr->ss_cmd_err_func( ss_cmd_ptr->client_id,
                                   ss_cmd_ptr->cmd,
                                   ss_cmd_err );
    }
  }
}



/*===========================================================================

FUNCTION cm_client_inband_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified inband command error code iff the client is
  registered to be notified of OTHER clients inband command errors and the
  client is NOT the one originally sending this inband command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_inband_cmd_err_ntfy(

    const cm_client_s_type      *client_ptr,
        /* pointer to a client struct */

    const cm_inband_cmd_s_type  *inband_cmd_ptr,
        /* pointer to inband command struct */

    cm_inband_cmd_err_e_type    inband_cmd_err
        /* phone command error code */
)
{
  const cm_inband_cmd_info_s_type *cmd_info_ptr  = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( inband_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN((int)inband_cmd_err, 
                     (int)CM_INBAND_CMD_ERR_NONE, 
                     (int)CM_INBAND_CMD_ERR_MAX) );
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr  = CMD_INFO_PTR( inband_cmd_ptr );


  if( client_ptr->client != inband_cmd_ptr->client_ptr )
  {
    if( client_ptr->inband_cmd_err_func != NULL )
    {
      client_ptr->inband_cmd_err_func( cmd_info_ptr->call_id,
                                       inband_cmd_ptr->client_id,
                                       inband_cmd_ptr->cmd,
                                       inband_cmd_err
                                       );
    }
  }
}


/*===========================================================================

FUNCTION cm_client_sms_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified SMS command error code iff the client is
  registered to be notified of OTHER clients SMS command errors and the
  client is NOT the one originally sending this SMS command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_sms_cmd_err_ntfy(

    const cm_client_s_type      *client_ptr,
        /* pointer to a client struct */

    const cm_sms_cmd_s_type     *sms_cmd_ptr,
        /* pointer to sms command struct */

    cm_sms_cmd_err_e_type       sms_cmd_err
        /* phone command error code */
)
{
  CM_ASSERT( client_ptr    != NULL );
  CM_ASSERT( sms_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN((int)sms_cmd_err, (int)CM_SMS_CMD_ERR_NONE, (int)CM_SMS_CMD_ERR_MAX) );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( client_ptr->client != sms_cmd_ptr->client_ptr )
  {
    if( client_ptr->sms_cmd_err_func != NULL )
    {
      client_ptr->sms_cmd_err_func( sms_cmd_ptr->client_id,
                                       sms_cmd_ptr->cmd,
                                       sms_cmd_err );
    }
  }
}


#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */


#ifdef FEATURE_DBM
/*===========================================================================

FUNCTION cm_client_dbm_cmd_err_ntfy

DESCRIPTION
  Notify a client of a specified DBM command error code iff the client is
  registered to be notified of OTHER clients DBM command errors and the
  client is NOT the one originally sending this DBM command.

  Note that the client originally sending this command is notified of
  the command error via its command supplied callback function.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void cm_client_dbm_cmd_err_ntfy(

    const cm_client_s_type      *client_ptr,
        /* pointer to a client struct */

    const cm_dbm_cmd_s_type     *dbm_cmd_ptr,
        /* pointer to sms command struct */

    cm_dbm_cmd_err_e_type       dbm_cmd_err
        /* phone command error code */
)
{
  CM_ASSERT( client_ptr   != NULL );
  CM_ASSERT( dbm_cmd_ptr  != NULL );
  CM_ASSERT( BETWEEN((int)dbm_cmd_err, (int)CM_DBM_CMD_ERR_NONE, (int)CM_DBM_CMD_ERR_MAX));

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( client_ptr->client != dbm_cmd_ptr->client_ptr )
  {
    if( client_ptr->dbm_cmd_err_func != NULL )
    {
      client_ptr->dbm_cmd_err_func( dbm_cmd_ptr->client_id,
                                    dbm_cmd_ptr->cmd,
                                    dbm_cmd_err );
    }
  }
}
#endif /* FEATURE_DBM */

#if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA))
#error code not present
#endif


#ifdef CM_DEBUG
#error code not present
#endif /* CM_DEBUG */


/*---------------------------------------------------------------------------
                                CLIENT LIST
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION cmclient_list_init

DESCRIPTION
  Initializes the client list.

  This function must be called before using the client list is used in any
  way, place or form.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmclient_list_init( void )
{

  CM_FUNC_START( "cmclient_list_init()",0,0,0 );

  /* Initialize client queue.
  */
  (void) q_init( CLIENT_LIST_Q_PTR() );
}




/*===========================================================================

FUNCTION cmclient_list_next

DESCRIPTION
  Return a pointer to the next active client in the client list.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  A pointer to the next client in the client list.
  If the end of the client list is reached, a NULL is returned.

SIDE EFFECTS
  None.

===========================================================================*/
static cm_client_s_type *cmclient_list_next( cm_client_s_type *client_ptr )
{

  CM_ASSERT( client_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get pointer to next client
  ** (i.e. the link with in the client).
  */
  do
  {
    client_ptr = (cm_client_s_type *)q_next( CLIENT_LIST_Q_PTR(), &client_ptr->link );
  }while( client_ptr!= NULL && !client_ptr->is_active );

  return client_ptr;

}


/*===========================================================================

FUNCTION cmclient_list_first

DESCRIPTION
  Return a pointer to first active client in the client list.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  Return a pointer to first client in the client list.
  If client list is empty a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cm_client_s_type *cmclient_list_first( void )
{
  cm_client_s_type *next_ptr;

  CM_FUNC_START( "cmclient_list_first()",0,0,0 );

  next_ptr = (cm_client_s_type*) q_check(CLIENT_LIST_Q_PTR());

  if( !next_ptr->is_active )
  {
    next_ptr = cmclient_list_next(next_ptr);
  }
  return( next_ptr );
}





/*===========================================================================

FUNCTION cmclient_list_add

DESCRIPTION
  Add a client to the client list.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmclient_list_add( cm_client_s_type *client_ptr )
{
  CM_FUNC_START( "cmclient_list_add()",0,0,0 );
  CM_ASSERT( client_ptr != NULL );

  /* Make sure this client is NOT already in the client list.
  */
  CM_ASSERT( client_ptr->link.next_ptr == NULL );
#ifndef FEATURE_Q_SINGLE_LINK
  CM_ASSERT( client_ptr->link.prev_ptr == NULL );
#endif

#ifndef FEATURE_Q_NO_SELF_QPTR
  CM_ASSERT( client_ptr->link.q_ptr == NULL );
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  q_put( CLIENT_LIST_Q_PTR(), &client_ptr->link );
}




/*===========================================================================

FUNCTION cmclient_list_del

DESCRIPTION
  Delete a client from the client list.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_del( cm_client_s_type *client_ptr )
{

  CM_ASSERT( client_ptr != NULL );

  /* Make sure this client is indeed in the client list.
  */
  CM_ASSERT( client_ptr->link.next_ptr != NULL );
#ifndef FEATURE_Q_SINGLE_LINK
  CM_ASSERT( client_ptr->link.prev_ptr != NULL );
#endif

#ifndef FEATURE_Q_NO_SELF_QPTR
  CM_ASSERT( client_ptr->link.q_ptr != NULL );
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Delete the client from the client list.
  */

#ifdef FEATURE_Q_NO_SELF_QPTR
  q_delete(CLIENT_LIST_Q_PTR(), (q_link_type *) &client_ptr->link );
#else
  q_delete( (q_link_type *) &client_ptr->link );

#endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the client's link item to make sure its next and prev
  ** pointers are no longer pointing at other clients.
  */
  (void) q_link( client_ptr, &client_ptr->link );

}



/*===========================================================================

FUNCTION cmclient_call_info_list_ntfy

DESCRIPTION
  Notify a client of call info list iff the client is registered
  for this.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_call_info_list_ntfy (

    const cm_client_s_type       *client_ptr,

    const cm_call_state_info_list_s_type    *call_info_list_ptr
        /* pointer to call state information struct */
)
{

  CM_ASSERT( client_ptr != NULL );
  CM_ASSERT( call_info_list_ptr != NULL );

  /* Make sure this object was properly initialized.
  */
  CM_ASSERT( client_ptr->init_mask == CM_INIT_MASK );

  /* If callback function exists and bit for receiving call info list
  *  is present, then call the callback.
  */
  if( client_ptr->call_info_list_func != NULL)
  {
    client_ptr->call_info_list_func (call_info_list_ptr);
  }

}


/*===========================================================================

FUNCTION cmclient_list_call_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified call event.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_call_event_ntfy(

    cm_call_event_e_type           call_event,
        /* notify clients of this call event */

    cm_mm_call_info_s_type   *call_info_ptr
        /* pointer to call state information struct */
)
{
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(call_event, CM_CALL_EVENT_NONE, CM_CALL_EVENT_MAX) );
  CM_ASSERT( call_info_ptr != NULL );
  /*
  ** 571: Suspicious cast
  ** Cast call_event as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_CALL,
    (word) call_event, (const void *) call_info_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse the client list and notify the
  ** individual client objects of the event.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    cm_client_call_event_ntfy( client_ptr, call_event, call_info_ptr );
  }
}





/*===========================================================================

FUNCTION cmclient_list_ph_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified phone event.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_ph_event_ntfy(

    cm_ph_event_e_type         ph_event,
        /* notify clients of this phone event */

    cm_ph_info_s_type    *ph_info_ptr
        /* pointer to phone state information struct */
)
{
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(ph_event, CM_PH_EVENT_NONE, CM_PH_EVENT_MAX) );
  CM_ASSERT( ph_info_ptr != NULL );
  /*
  ** 571: Suspicious cast
  ** Cast ph_event as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_PH,
    (word) ph_event, (const void *) ph_info_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    cm_client_ph_event_ntfy( client_ptr, ph_event, ph_info_ptr );
  }
}

#if ((defined FEATURE_BCMCS) || (defined FEATURE_MODEM_MBMS))
/*===========================================================================

FUNCTION cmclient_list_bcmcs_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified BCMCS event.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_bcmcs_event_ntfy(

    cm_bcmcs_event_e_type         bcmcs_event,
        /* notify clients of this BCMCS event */

    cm_bcmcs_info_s_type         *bcmcs_info_ptr
        /* pointer to BCMCS information struct */
)
{
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(bcmcs_event, CM_BCMCS_EVENT_NONE, CM_BCMCS_EVENT_MAX) );

  if (bcmcs_info_ptr == NULL )
  {
    CM_MSG_HIGH("bcmcs_info_ptr NULL",0,0,0);
    return;
  }
  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_BCMCS,
    (word) bcmcs_event, (const void *) bcmcs_info_ptr );
  /*lint -restore */ 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    bcmcs_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
    cm_client_bcmcs_event_ntfy( client_ptr, bcmcs_event, bcmcs_info_ptr );
  }
}
#endif /* FEATURE_BCMCS || MBMS */


/*===========================================================================

FUNCTION cmclient_list_ss_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified serving system
  event.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_ss_event_ntfy(

    cm_ss_event_e_type            ss_event,
        /* notify clients of this serving system event */

    cm_mm_ss_info_s_type    *ss_info_ptr
        /* pointer to serving system state information struct */
)
{
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(ss_event, CM_SS_EVENT_NONE, CM_SS_EVENT_MAX) );
  CM_ASSERT( ss_info_ptr != NULL );
  /*
  ** 571: Suspicious cast
  ** Cast ss_event as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_SS,
    (word) ss_event, (const void *) ss_info_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    ss_info_ptr->requesting_client_id = cmclient_get_client_id( client_ptr );
    cm_client_ss_event_ntfy( client_ptr, ss_event, ss_info_ptr );
  }
}


/*===========================================================================

FUNCTION cmclient_list_inband_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified inband event.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_inband_event_ntfy(

    cm_inband_event_e_type         inband_event,
        /* notify clients of this inband event */

    cm_inband_info_s_type    *inband_info_ptr
        /* pointer to inband information struct */
)
{
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(inband_event, CM_INBAND_EVENT_NONE, CM_INBAND_EVENT_MAX) );
  CM_ASSERT( inband_info_ptr != NULL );
  /*
  ** 571: Suspicious cast
  ** Cast inband_event as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_INBAND,
    (word) inband_event, (const void *) inband_info_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    cm_client_inband_event_ntfy( client_ptr, inband_event, inband_info_ptr );
  }
}


/*===========================================================================

FUNCTION cmclient_list_sms_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified sms event.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_sms_event_ntfy(

    cm_sms_event_e_type         sms_event,
        /* notify clients of this sms event */

    cm_sms_info_s_type    *sms_info_ptr
        /* pointer to sms information struct */
)
{
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(sms_event, CM_SMS_EVENT_NONE, CM_SMS_EVENT_MAX) );
  CM_ASSERT( sms_info_ptr != NULL );
  /*
  ** 571: Suspicious cast
  ** Cast sms_event as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_EVENT_FUNC( CM_CMD_TYPE_SMS,
    (word) sms_event, (const void *) sms_info_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    cm_client_sms_event_ntfy( client_ptr, sms_event, sms_info_ptr );
  }
}


#ifdef FEATURE_GPSONE
#error code not present
#endif

#ifdef FEATURE_DBM
/*===========================================================================

FUNCTION cmclient_list_dbm_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified dbm event.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_dbm_event_ntfy(
    cm_dbm_event_e_type         dbm_event,
        /* notify clients of this dbm event */

    cm_dbm_info_s_type    *dbm_info_ptr
        /* pointer to dbm information struct */
)
{
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( BETWEEN(dbm_event, CM_DBM_EVENT_NONE, CM_DBM_EVENT_MAX) );
  CM_ASSERT( dbm_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client objects of the event.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    cm_client_dbm_event_ntfy(client_ptr, dbm_event, dbm_info_ptr);
  }
}
#endif

#if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
#endif

#if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA))
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */


/*===========================================================================

FUNCTION cmclient_list_call_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified call command
  error.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_call_cmd_err_ntfy(

    cm_call_cmd_err_e_type      call_cmd_err,
        /* call command error code */

    const cm_call_cmd_s_type    *call_cmd_ptr
        /* pointer to call command struct */

)
{
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(call_cmd_err, CM_CALL_CMD_ERR_NONE, CM_CALL_CMD_ERR_MAX) );
  /*
  ** 571: Suspicious cast
  ** Cast call_cmd_err as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_CALL, (word) call_cmd_err,
    (const void *) call_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    cm_client_call_cmd_err_ntfy( client_ptr,
                                 call_cmd_ptr,
                                 call_cmd_err );
  }
}




/*===========================================================================

FUNCTION cmclient_list_ph_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified phone command
  error.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_ph_cmd_err_ntfy(

    cm_ph_cmd_err_e_type      ph_cmd_err,
        /* phone command error code */

    const cm_ph_cmd_s_type    *ph_cmd_ptr
        /* pointer to ph command struct */

)
{
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ph_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(ph_cmd_err, CM_PH_CMD_ERR_NONE, CM_PH_CMD_ERR_MAX) );
  /*
  ** 571: Suspicious cast
  ** Cast ph_cmd_err as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_PH, (word) ph_cmd_err,
    (const void *) ph_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    cm_client_ph_cmd_err_ntfy( client_ptr,
                               ph_cmd_ptr,
                               ph_cmd_err );
  }
}
  #if ((defined FEATURE_BCMCS) || (defined FEATURE_MODEM_MBMS))
/*===========================================================================

FUNCTION cmclient_list_bcmcs_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified BCMCS command
  error.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmclient_list_bcmcs_cmd_err_ntfy(

    cm_bcmcs_cmd_err_e_type      bcmcs_cmd_err,
        /* phone command error code */

    const cm_bcmcs_cmd_s_type    *bcmcs_cmd_ptr
        /* pointer to BCMCS command struct */

)
{
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( bcmcs_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(bcmcs_cmd_err, CM_BCMCS_CMD_ERR_NONE, CM_BCMCS_CMD_ERR_MAX) );
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_BCMCS, (word) bcmcs_cmd_err,
    (const void *) bcmcs_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    cm_client_bcmcs_cmd_err_ntfy( client_ptr,
                               bcmcs_cmd_ptr,
                               bcmcs_cmd_err );
  }
}
#endif /* FEATURE_BCMCS || MBMS */
/*===========================================================================

FUNCTION cmclient_list_mbms_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified MBMS command
  error.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmclient_list_mbms_cmd_err_ntfy(

    cm_mbms_cmd_err_e_type        mbms_cmd_err,
        /* phone command error code */

    const cm_mbms_cmd_s_type    *mbms_cmd_ptr
        /* pointer to MBMS command struct */

)
/*lint -esym(715,mbms_cmd_err) */
/*lint -esym(715,mbms_cmd_ptr) */
/*lint -esym(714,cmclient_list_mbms_cmd_err_ntfy) */
/*lint -esym(765,cmclient_list_mbms_cmd_err_ntfy)
** Can't be static as it would result in compiler warnings
*/
{
  #if (defined FEATURE_MODEM_MBMS)
#error code not present
  #else
  CM_MSG_HIGH("FEATURE_MODEM_MBMS not defined",0,0,0);
  #endif /* defined (FEATURE_MODEM_MBMS) */
}
/*lint +esym(715,mbms_cmd_err) */
/*lint +esym(715,mbms_cmd_ptr) */


/*===========================================================================

FUNCTION cmss_list_ss_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified serving system command
  error.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_ss_cmd_err_ntfy(

    cm_ss_cmd_err_e_type      ss_cmd_err,
        /* call command error code */

    const cm_ss_cmd_s_type    *ss_cmd_ptr
        /* pointer to ss command struct */

)
{
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ss_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(ss_cmd_err, CM_SS_CMD_ERR_NONE, CM_SS_CMD_ERR_MAX) );
  /*
  ** 571: Suspicious cast
  ** Cast ss_cmd_err as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_SS, (word) ss_cmd_err,
    (const void *) ss_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    cm_client_ss_cmd_err_ntfy( client_ptr,
                               ss_cmd_ptr,
                               ss_cmd_err );
  }
}


/*===========================================================================

FUNCTION cmclient_list_inband_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified inband command
  error.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_inband_cmd_err_ntfy(

    cm_inband_cmd_err_e_type      inband_cmd_err,
        /* phone command error code */

    const cm_inband_cmd_s_type    *inband_cmd_ptr
        /* pointer to inband command struct */

)
{
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( inband_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(inband_cmd_err, CM_INBAND_CMD_ERR_NONE, CM_INBAND_CMD_ERR_MAX) );
  /*
  ** 571: Suspicious cast
  ** Cast inband_cmd_err as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_INBAND, (word) inband_cmd_err,
    (const void *) inband_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    cm_client_inband_cmd_err_ntfy( client_ptr,
                                   inband_cmd_ptr,
                                   inband_cmd_err );
  }
}


/*===========================================================================

FUNCTION cmclient_list_sms_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified SMS command
  error.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_sms_cmd_err_ntfy(

    cm_sms_cmd_err_e_type      sms_cmd_err,
        /* phone command error code */

    const cm_sms_cmd_s_type    *sms_cmd_ptr
        /* pointer to sms command struct */

)
{
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(sms_cmd_err, CM_SMS_CMD_ERR_NONE, CM_SMS_CMD_ERR_MAX) );
  /*
  ** 571: Suspicious cast
  ** Cast sms_cmd_err as word so we can use generic function to print it
  */
  /*lint -save -e571 */
  CMDBG_PRINT_CM_CMDERR_FUNC( CM_CMD_TYPE_SMS, (word) sms_cmd_err,
    (const void *) sms_cmd_ptr );
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    cm_client_sms_cmd_err_ntfy( client_ptr,
                                sms_cmd_ptr,
                                sms_cmd_err );
  }
}

#ifdef FEATURE_GPSONE
#error code not present
#endif



#ifdef FEATURE_DBM
/*===========================================================================

FUNCTION cmclient_list_dbm_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified DBM command
  error.

DEPENDENCIES
  Client list must have already been initialized with cmclient_list_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmclient_list_dbm_cmd_err_ntfy(
    cm_dbm_cmd_err_e_type      dbm_cmd_err,
        /* phone command error code */

    const cm_dbm_cmd_s_type    *dbm_cmd_ptr
        /* pointer to sms command struct */
)
{
  cm_client_s_type *client_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( dbm_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN(dbm_cmd_err, CM_DBM_CMD_ERR_NONE, CM_DBM_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Traverse client list and notify the
  ** individual client object of the error.
  */
  for( client_ptr = cmclient_list_first();
       client_ptr != NULL;
       client_ptr = cmclient_list_next(client_ptr) )
  {
    cm_client_dbm_cmd_err_ntfy( client_ptr,
                                dbm_cmd_ptr,
                                dbm_cmd_err);
  }
}
#endif /* FEATURE_DBM */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif

/*===========================================================================

FUNCTION cm_mm_client_call_reg

DESCRIPTION
  This function allows a client to register the following call-related
  callbacks and events with CM:

   1. Callback function to receive registered call events
   2. Register and de-register call events
   3. Callback function to receive other clients' call-command errors.

   If a client registers a callback function to receive call event
   notification, it also needs to register one or more call events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of call events, a client needs to call on this API twice- once for each
   range of call events.

  If a client registers a callback function to receive other clients'
  call-command errors, it will receive notifications on all possible
  call-command errors caused by other clients. A client cannot register to
  receive only a subset of call-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' call-command errors, it can set
  call_cmd_err_func = NULL. However, if the call_event_func is set to NULL,
  all registered call events are also de-registered and the event_reg_type,
  from_call_event, and to_call_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_call_reg(

    cm_client_id_type           client,
        /* client id */

    cm_mm_call_event_f_type     *call_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_client_event_reg_e_type  event_reg_type,

    cm_call_event_e_type        from_call_event,
        /* register from this event (including) */

    cm_call_event_e_type        to_call_event,
          /* to this event (including) */

    cm_mm_call_cmd_err_f_type   *call_cmd_err_func
      /* pointer to a callback function to notify the client of call
      ** command errors of OTHER clients */
)
{
  boolean reg_flag;
  struct cm_client_s  *client_ptr;

  client_ptr = cmclient_get_client_ptr( client );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != CM_INIT_MASK) )
  {
      return CM_CLIENT_ERR_CLIENT_ID;
  }

  if( call_event_func == NULL )
  {

     /* deregister all of client's call events */
     bit_arr_init( &client_ptr->call_bit_arr,       /* call bit array struct */
                   client_ptr->call_bit_buf,                   /* bit buffer */
                   (8 * ARR_SIZE(client_ptr->call_bit_buf)) ); /* bit size   */

     /* update call event and other clients' command error callback functions */

     client_ptr->call_event_func = call_event_func;
     client_ptr->call_cmd_err_func = call_cmd_err_func;

     return CM_CLIENT_OK;

  }


  if (!BETWEEN(from_call_event, CM_CALL_EVENT_NONE, CM_CALL_EVENT_MAX))
  {
      return CM_CLIENT_ERR_FROM_EVENT;
  }

  if (!BETWEEN(to_call_event, CM_CALL_EVENT_NONE, CM_CALL_EVENT_MAX))
  {
    return CM_CLIENT_ERR_TO_EVENT;
  }

  if ( from_call_event > to_call_event )
  {
     return CM_CLIENT_ERR_FROM_EVENT;
  }

  switch(event_reg_type)
  {
    case CM_CLIENT_EVENT_REG:
      reg_flag=TRUE;
      break;

    case CM_CLIENT_EVENT_DEREG:
      reg_flag=FALSE;
      break;

    default:
      return CM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  bit_arr_set_bits( &client_ptr->call_bit_arr,       /* for this bit array */
                    reg_flag,                        /* set bits to ON/off */
                    (int)from_call_event,            /* start from this */
                    (int)to_call_event);             /* end with this bit */


  /* update call event and other clients' command error callback functions */

  client_ptr->call_event_func = call_event_func;
  client_ptr->call_cmd_err_func = call_cmd_err_func;

  return CM_CLIENT_OK;

}

/*===========================================================================

FUNCTION cm_mm_client_call_pwr_collapse_reg

DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored 
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the 
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use 
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_call_event and to_call_event, and event_reg_type are used only 
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If 
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of call events, a client needs to call on this API twice- once for each
  range of call events. In case of conflicting pwr_collapse_type, the last
  one will be honored

  
DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_call_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_mm_call_event_f_type      *call_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Registration type */

    cm_call_event_e_type          from_call_event,
        /* register from this event (including) */

    cm_call_event_e_type          to_call_event
        /* to this event (including) */
)
/*lint -esym(715,client) */
/*lint -esym(715,call_event_func) */
/*lint -esym(715,proc_id) */
/*lint -esym(715,pwr_collapse_type) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_call_event) */
/*lint -esym(715,to_call_event) */
{
  #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif
 
} /* cm_mm_client_call_pwr_collapse_reg */
/*lint +esym(715,client) */
/*lint +esym(715,call_event_func) */
/*lint +esym(715,proc_id) */
/*lint +esym(715,pwr_collapse_type) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_call_event) */
/*lint +esym(715,to_call_event) */



/*===========================================================================

FUNCTION cm_client_ph_reg

DESCRIPTION
  This function allows a client to register the following phone-related
  callbacks and events with CM:

   1. Callback function to receive registered phone events
   2. Register and de-register phone events
   3. Callback function to receive other clients' phone-command errors.

   If a client registers a callback function to receive phone event
   notification, it also needs to register one or more phone events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of call events, a client needs to call on this API twice- once for each
   range of call events.

  If a client registers a callback function to receive other clients'
  phone-command errors, it will receive notifications on all possible
  phone-command errors caused by other clients. A client cannot register to
  receive only a subset of phone-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' phone-command errors, it can set
  ph_cmd_err_func = NULL. However, if the ph_event_func is set to NULL,
  all registered phone events are also de-registered and the event_reg_type,
  from_ph_event, and to_ph_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_client_ph_reg(

    cm_client_id_type           client,
        /* client id */

    cm_ph_event_f_type          *ph_event_func,
        /* Pointer to a callback function to notify the client of phone
        ** events */

    cm_client_event_reg_e_type  event_reg_type,

    cm_ph_event_e_type          from_ph_event,
        /* register from this event (including) */

    cm_ph_event_e_type          to_ph_event,
          /* to this event (including) */

    cm_ph_cmd_err_f_type        *ph_cmd_err_func
      /* pointer to a callback function to notify the client of phone
      ** command errors of OTHER clients */
)
{
  boolean reg_flag;
  struct cm_client_s  *client_ptr;

  client_ptr = cmclient_get_client_ptr( client );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != CM_INIT_MASK) )
  {
      return CM_CLIENT_ERR_CLIENT_ID;
  }

  if( ph_event_func == NULL )
  {

     /* deregister all of client's phone events */

     bit_arr_init( &client_ptr->ph_bit_arr,        /* phone bit array struct */
                   client_ptr->ph_bit_buf,                     /* bit buffer */
                   (8 * ARR_SIZE(client_ptr->ph_bit_buf)) );   /* bit size   */

     /* update phone event and other clients' phone command error callback functions */

     client_ptr->ph_event_func   = ph_event_func;
     client_ptr->ph_cmd_err_func = ph_cmd_err_func;

     return CM_CLIENT_OK;

  }


  if (!BETWEEN(from_ph_event, CM_PH_EVENT_NONE, CM_PH_EVENT_MAX))
  {
      return CM_CLIENT_ERR_FROM_EVENT;
  }

  if (!BETWEEN(to_ph_event, CM_PH_EVENT_NONE, CM_PH_EVENT_MAX))
  {
    return CM_CLIENT_ERR_TO_EVENT;
  }

  if ( from_ph_event > to_ph_event )
  {
    return CM_CLIENT_ERR_FROM_EVENT;
  }

  switch(event_reg_type)
  {
    case CM_CLIENT_EVENT_REG:
      reg_flag=TRUE;
      break;

    case CM_CLIENT_EVENT_DEREG:
      reg_flag=FALSE;
      break;

    default:
      return CM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  bit_arr_set_bits( &client_ptr->ph_bit_arr,       /* for this bit array */
                    reg_flag,                        /* set bits to ON/off */
                    (int)from_ph_event,            /* start from this */
                    (int)to_ph_event);             /* end with this bit */


  /* update call event and other clients' command error callback functions */

  client_ptr->ph_event_func   = ph_event_func;
  client_ptr->ph_cmd_err_func = ph_cmd_err_func;

  return CM_CLIENT_OK;

}


/*===========================================================================

FUNCTION cm_mm_client_ph_pwr_collapse_reg

DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored 
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the 
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use 
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_ph_event and to_ph_event, and event_reg_type are used only 
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If 
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of ph events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored

  
DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_ph_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_ph_event_f_type          *ph_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Registration type */

    cm_ph_event_e_type            from_ph_event,
        /* register from this event (including) */

    cm_ph_event_e_type            to_ph_event
        /* to this event (including) */
)
/*lint -esym(715,client) */
/*lint -esym(715,ph_event_func) */
/*lint -esym(715,proc_id) */
/*lint -esym(715,pwr_collapse_type) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_ph_event) */
/*lint -esym(715,to_ph_event) */
{
  #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif
 
} /* cm_mm_client_ph_pwr_collapse_reg */
/*lint +esym(715,client) */
/*lint +esym(715,ph_event_func) */
/*lint +esym(715,proc_id) */
/*lint +esym(715,pwr_collapse_type) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_ph_event) */
/*lint +esym(715,to_ph_event) */



/*===========================================================================

FUNCTION cm_mm_client_ss_reg

DESCRIPTION
  This function allows a client to register the following serving system-related
  callbacks and events with CM:

   1. Callback function to receive registered serving system events
   2. Register and de-register serving system events
   3. Callback function to receive other clients' serving system-command errors.

   If a client registers a callback function to receive serving system event
   notification, it also needs to register one or more serving system events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of serving system events, a client needs to call on this API twice- once for each
   range of serving system events.

  If a client registers a callback function to receive other clients'
  serving system-command errors, it will receive notifications on all possible
  serving system-command errors caused by other clients. A client cannot register to
  receive only a subset of serving system-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' serving system-command errors, it can set
  ss_cmd_err_func = NULL. However, if the ss_event_func is set to NULL,
  all registered serving system events are also de-registered and the event_reg_type,
  from_ss_event, and to_ss_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_ss_reg(

    cm_client_id_type           client,
        /* client id */

    cm_mm_ss_event_f_type       *ss_event_func,
        /* Pointer to a callback function to notify the client of serving system
           ** events */

    cm_client_event_reg_e_type  event_reg_type,

    cm_ss_event_e_type          from_ss_event,
        /* register from this event (including) */

    cm_ss_event_e_type          to_ss_event,
          /* to this event (including) */

    cm_ss_cmd_err_f_type        *ss_cmd_err_func
      /* pointer to a callback function to notify the client of serving system
      ** command errors of OTHER clients */
)
{
  boolean reg_flag;
  struct cm_client_s  *client_ptr;

  client_ptr = cmclient_get_client_ptr( client );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != CM_INIT_MASK) )
  {
      return CM_CLIENT_ERR_CLIENT_ID;
  }

  if( ss_event_func == NULL )
  {

     /* deregister all of client's serving system events */

     bit_arr_init( &client_ptr->ss_bit_arr, /*serving system bit array struct*/
                   client_ptr->ss_bit_buf,                     /* bit buffer */
                   (8 * ARR_SIZE(client_ptr->ss_bit_buf)) );   /* bit size   */

     /* update phone event and other clients' phone command error callback functions */

     client_ptr->ss_event_func   = ss_event_func;
     client_ptr->ss_cmd_err_func = ss_cmd_err_func;

     return CM_CLIENT_OK;

  }


  if (!BETWEEN(from_ss_event, CM_SS_EVENT_NONE, CM_SS_EVENT_MAX))
  {
      return CM_CLIENT_ERR_FROM_EVENT;
  }

  if (!BETWEEN(to_ss_event, CM_SS_EVENT_NONE, CM_SS_EVENT_MAX))
  {
    return CM_CLIENT_ERR_TO_EVENT;
  }

  if ( from_ss_event > to_ss_event )
  {
     return CM_CLIENT_ERR_FROM_EVENT;
  }

  switch(event_reg_type)
  {
    case CM_CLIENT_EVENT_REG:
      reg_flag=TRUE;
      break;

    case CM_CLIENT_EVENT_DEREG:
      reg_flag=FALSE;
      break;

    default:
      return CM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  bit_arr_set_bits( &client_ptr->ss_bit_arr,       /* for this bit array */
                    reg_flag,                        /* set bits to ON/off */
                    (int)from_ss_event,            /* start from this */
                    (int)to_ss_event);             /* end with this bit */


  /* update call event and other clients' command error callback functions */

  client_ptr->ss_event_func   = ss_event_func;
  client_ptr->ss_cmd_err_func = ss_cmd_err_func;

  return CM_CLIENT_OK;

}

/*===========================================================================

FUNCTION cm_mm_client_ss_pwr_collapse_reg

DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored 
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the 
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use 
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_ss_event and to_ss_event, and event_reg_type are used only 
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If 
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of ss events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored

  
DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_ss_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_mm_ss_event_f_type        *ss_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Registration type */

    cm_ss_event_e_type            from_ss_event,
        /* register from this event (including) */

    cm_ss_event_e_type            to_ss_event
        /* to this event (including) */
)
/*lint -esym(715,client) */
/*lint -esym(715,ss_event_func) */
/*lint -esym(715,proc_id) */
/*lint -esym(715,pwr_collapse_type) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_ss_event) */
/*lint -esym(715,to_ss_event) */
{
  #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif
 
} /* cm_mm_client_ss_pwr_collapse_reg */
/*lint +esym(715,client) */
/*lint +esym(715,ss_event_func) */
/*lint +esym(715,proc_id) */
/*lint +esym(715,pwr_collapse_type) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_ss_event) */
/*lint +esym(715,to_ss_event) */


/*===========================================================================

FUNCTION cm_mm_client_inband_reg

DESCRIPTION
  This function allows a client to register the following inband-related
  callbacks and events with CM:

   1. Callback function to receive registered inband events
   2. Register and de-register inband events
   3. Callback function to receive other clients' inband-command errors.

   If a client registers a callback function to receive inband event
   notification, it also needs to register one or more inband events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of inband events, a client needs to call on this API twice- once for each
   range of inband events.

  If a client registers a callback function to receive other clients'
  inband-command errors, it will receive notifications on all possible
  inband-command errors caused by other clients. A client cannot register to
  receive only a subset of inband-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' inband-command errors, it can set
  inband_cmd_err_func = NULL. However, if the inband_event_func is set to NULL,
  all registered inband events are also de-registered and the event_reg_type,
  from_inbnad_event, and to_inband_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/

cm_client_status_e_type cm_mm_client_inband_reg(

    cm_client_id_type               client,
        /* client id */

    cm_inband_event_f_type          *inband_event_func,
        /* Pointer to a callback function to notify the client of inband
        ** events */

    cm_client_event_reg_e_type      event_reg_type,

    cm_inband_event_e_type          from_inband_event,
        /* register from this event (including) */

    cm_inband_event_e_type          to_inband_event,
          /* to this event (including) */

    cm_mm_inband_cmd_err_f_type     *inband_cmd_err_func
      /* pointer to a callback function to notify the client of inband
      ** command errors of OTHER clients */
)
{
  boolean reg_flag = FALSE;
  struct cm_client_s  *client_ptr;

  client_ptr = cmclient_get_client_ptr( client );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != CM_INIT_MASK) )
  {
     return CM_CLIENT_ERR_CLIENT_ID;
  }

  if(inband_event_func == NULL)
  {

     /* deregister all of client's inband events */

     bit_arr_init( &client_ptr->inband_bit_arr, /*inband bit array struct*/
                   client_ptr->inband_bit_buf,                     /* bit buffer */
                   (8 * ARR_SIZE(client_ptr->inband_bit_buf)) );   /* bit size   */

     /* update inband event and other clients' inband command error callback functions */

     client_ptr->inband_event_func   = inband_event_func;
     client_ptr->inband_cmd_err_func = inband_cmd_err_func;

     return CM_CLIENT_OK;

  }


  if (!BETWEEN(from_inband_event, CM_INBAND_EVENT_NONE, CM_INBAND_EVENT_MAX))
  {
      return CM_CLIENT_ERR_FROM_EVENT;
  }

  if (!BETWEEN(to_inband_event, CM_INBAND_EVENT_NONE, CM_INBAND_EVENT_MAX))
  {
    return CM_CLIENT_ERR_TO_EVENT;
  }

  if ( from_inband_event > to_inband_event )
  {
     return CM_CLIENT_ERR_FROM_EVENT;
  }

  switch(event_reg_type)
  {
    case CM_CLIENT_EVENT_REG:
      reg_flag=TRUE;
      break;

    case CM_CLIENT_EVENT_DEREG:
      reg_flag=FALSE;
      break;

    default:
      return CM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  bit_arr_set_bits( &client_ptr->inband_bit_arr,       /* for this bit array */
                    reg_flag,                          /* set bits to ON/off */
                    (int)from_inband_event,            /* start from this */
                    (int)to_inband_event);             /* end with this bit */


  /* update inband event and other clients' command error callback functions */

  client_ptr->inband_event_func   = inband_event_func;
  client_ptr->inband_cmd_err_func = inband_cmd_err_func;

  return CM_CLIENT_OK;

} 

/*===========================================================================

FUNCTION cm_mm_client_inband_pwr_collapse_reg

DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored 
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the 
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use 
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_inband_event and to_inband_event, and event_reg_type are used only 
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If 
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of inband events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored

  
DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_inband_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_inband_event_f_type       *inband_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Registration type */

    cm_inband_event_e_type        from_inband_event,
        /* register from this event (including) */

    cm_inband_event_e_type        to_inband_event
        /* to this event (including) */
)
/*lint -esym(715,client) */
/*lint -esym(715,inband_event_func) */
/*lint -esym(715,proc_id) */
/*lint -esym(715,pwr_collapse_type) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_inband_event) */
/*lint -esym(715,to_inband_event) */
{
  #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif
 
} /* cm_mm_client_inband_pwr_collapse_reg */
/*lint +esym(715,client) */
/*lint +esym(715,inband_event_func) */
/*lint +esym(715,proc_id) */
/*lint +esym(715,pwr_collapse_type) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_inband_event) */
/*lint +esym(715,to_inband_event) */


/*===========================================================================

FUNCTION cm_client_sms_reg

DESCRIPTION
  This function allows a client to register the following sms-related
  callbacks and events with CM:

   1. Callback function to receive registered sms events
   2. Register and de-register sms events
   3. Callback function to receive other clients'sms-command errors.

   If a client registers a callback function to receive inband event
   notification, it also needs to register one or more inband events it is
   interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of sms events, a client needs to call on this API twice- once for each
   range of sms events.

  If a client registers a callback function to receive other clients'
  sms-command errors, it will receive notifications on all possible
  sms-command errors caused by other clients. A client cannot register to
  receive only a subset of sms-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' sms-command errors, it can set
  sms_cmd_err_func = NULL. However, if the sms_event_func is set to NULL,
  all registered sms events are also de-registered and the event_reg_type,
  from_sms_event, and to_sms_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_client_sms_reg(

    cm_client_id_type               client,
        /* Requesting client */

    cm_sms_event_f_type             *sms_event_func,
        /* Pointer to a callback function to notify the client of sms
        ** events */

    cm_client_event_reg_e_type      event_reg_type,

    cm_sms_event_e_type             from_sms_event,
        /* register from this event (including) */

    cm_sms_event_e_type             to_sms_event,
          /* to this event (including) */


    cm_sms_cmd_err_f_type           *sms_cmd_err_func
      /* pointer to a callback function to notify the client of sms
      ** command errors of OTHER clients */

)
/*lint -esym(715,client) */
/*lint -esym(715,sms_event_func) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_sms_event) */
/*lint -esym(715,to_sms_event) */
/*lint -esym(715,sms_cmd_err_func) */
{
  #ifdef FEATURE_CDSMS
  boolean reg_flag;
  struct cm_client_s  *client_ptr;

  client_ptr = cmclient_get_client_ptr( client );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != CM_INIT_MASK) )
  {
     return CM_CLIENT_ERR_CLIENT_ID;
  }

  if(sms_event_func == NULL)
  {

     /* deregister all of client's inband events */

     bit_arr_init( &client_ptr->sms_bit_arr, /*sms bit array struct*/
                   client_ptr->sms_bit_buf,                     /* bit buffer */
                   (8 * ARR_SIZE(client_ptr->sms_bit_buf)) );   /* bit size   */

     /* update sms event and other clients' sms command error callback functions */

     client_ptr->sms_event_func   = sms_event_func;

     client_ptr->sms_cmd_err_func = sms_cmd_err_func;

     return CM_CLIENT_OK;

  }


  if (!BETWEEN(from_sms_event, CM_SMS_EVENT_NONE, CM_SMS_EVENT_MAX))
  {
      return CM_CLIENT_ERR_FROM_EVENT;
  }

  if (!BETWEEN(to_sms_event, CM_SMS_EVENT_NONE, CM_SMS_EVENT_MAX))
  {
    return CM_CLIENT_ERR_TO_EVENT;
  }

  if ( from_sms_event > to_sms_event )
  {
     return CM_CLIENT_ERR_FROM_EVENT;
  }

  switch(event_reg_type)
  {
    case CM_CLIENT_EVENT_REG:
      reg_flag=TRUE;
      break;

    case CM_CLIENT_EVENT_DEREG:
      reg_flag=FALSE;
      break;

    default:
      return CM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  bit_arr_set_bits( &client_ptr->sms_bit_arr,       /* for this bit array */
                    reg_flag,                        /* set bits to ON/off */
                    (int)from_sms_event,            /* start from this */
                    (int)to_sms_event);             /* end with this bit */


  /* update SMS event and other clients' command error callback functions */

  client_ptr->sms_event_func   = sms_event_func;

  client_ptr->sms_cmd_err_func = sms_cmd_err_func;

  return CM_CLIENT_OK;
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif /* FEATURE_CDSMS */

}
/*lint +esym(715,client) */
/*lint +esym(715,sms_event_func) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_sms_event) */
/*lint +esym(715,to_sms_event) */
/*lint +esym(715,sms_cmd_err_func) */

/*===========================================================================

FUNCTION cm_mm_client_sms_pwr_collapse_reg

DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored 
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the 
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use 
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_sms_event and to_sms_event, and event_reg_type are used only 
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If 
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of sms events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored

  
DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_sms_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_sms_event_f_type          *sms_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Normal, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Registration type */

    cm_sms_event_e_type           from_sms_event,
        /* register from this event (including) */

    cm_sms_event_e_type           to_sms_event
        /* to this event (including) */
)
  /*lint -esym(715,client)*/
  /*lint -esym(715,proc_id)*/
  /*lint -esym(715,pwr_collapse_type)*/
  /*lint -esym(715,event_reg_type)*/
  /*lint -esym(715,from_sms_event)*/
  /*lint -esym(715,to_sms_event)*/
  /*lint -esym(715,sms_event_func) */

{
  #if defined(FEATURE_CDSMS) && defined(FEATURE_APPS_POWER_COLLAPSE)
#error code not present
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif
 
} /* cm_mm_client_sms_pwr_collapse_reg */
/*lint +esym(715,client)*/
/*lint +esym(715,proc_id)*/
/*lint +esym(715,pwr_collapse_type)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_sms_event)*/
/*lint +esym(715,to_sms_event)*/
/*lint +esym(715,sms_event_func)*/



/*===========================================================================

FUNCTION cm_mm_client_call_info_list_reg

DESCRIPTION
  This function allows a client to register the following call-related
  callbacks with CM:

   1. Callback function to receive call information list.
   2. Register and de-register function for call information list
   3. Callback function to receive other clients'  errors.


  If a client registers a callback function to receive other clients'
  call-command errors, it will receive notifications on all possible
  call-command errors caused by other clients. A client cannot register to
  receive only a subset of call-command errors. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' call-command errors, it can set
  call_info_list_cmd_err_func = NULL.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_call_info_list_reg(

    cm_client_id_type           client,
        /* client id */

    cm_call_info_list_f_type     *call_info_list_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_client_event_reg_e_type  event_reg_type,

    cm_mm_call_cmd_err_f_type   *call_info_list_cmd_err_func
      /* pointer to a callback function to notify the client of call
      ** command errors of OTHER clients */
)
/*lint -esym(715,client) */
/*lint -esym(715,call_info_list_func) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,call_info_list_cmd_err_func) */
{
  struct cm_client_s  *client_ptr;
  client_ptr = cmclient_get_client_ptr (client);

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != CM_INIT_MASK) )
  {
      return CM_CLIENT_ERR_CLIENT_ID;
  }

  /* update call event and other clients' command error callback functions */
  client_ptr->call_info_list_func = call_info_list_func;
  client_ptr->call_info_list_cmd_err_func = call_info_list_cmd_err_func;

  return CM_CLIENT_OK;
}
/*lint +esym(715,client) */
/*lint +esym(715,call_info_list_func) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,call_info_list_cmd_err_func) */
/*===========================================================================

FUNCTION cm_client_pd_reg

DESCRIPTION
  This function is to be used by CM clients interested in registering for
  Position Determination call manager events.

DEPENDENCIES
  cm_client_init must have been called previously.

RETURN VALUE
  results of the registration.

SIDE EFFECTS
  None

===========================================================================*/

cm_client_status_e_type cm_client_pd_reg(

    cm_client_id_type             client,
        /* Requesting client */

    cm_pd_event_f_type           *pd_event_func,
        /* Pointer to a callback function to notify the client of pd
        ** events */

    cm_client_event_reg_e_type    event_reg_type,
        /* Is this a registration or de-reg */

    cm_pd_event_e_type            from_pd_event,
        /* register from this event (including) */

    cm_pd_event_e_type            to_pd_event,
        /* to this event (including) */


    cm_pd_cmd_err_f_type         *pd_cmd_err_func
      /* pointer to a callback function to notify the client of pd
      ** command errors of OTHER clients */

)
/*lint -esym(715,client) */
/*lint -esym(715,pd_event_func) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_pd_event) */
/*lint -esym(715,to_pd_event) */
/*lint -esym(715,pd_cmd_err_func) */
{
  #ifdef FEATURE_GPSONE
#error code not present
  #else /* FEATURE_GPSONE */
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif /* else of FEATURE_GPSONE */

}
/*lint +esym(715,client) */
/*lint +esym(715,pd_event_func) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_pd_event) */
/*lint +esym(715,to_pd_event) */
/*lint +esym(715,pd_cmd_err_func) */

/*===========================================================================

FUNCTION cm_mm_client_pd_pwr_collapse_reg

DESCRIPTION
  DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored 
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the 
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use 
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_pd_event and to_pd_event, and event_reg_type are used only 
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If 
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of pd events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored

DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_pd_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_pd_event_f_type           *pd_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Is this a registration or de-reg */

    cm_pd_event_e_type            from_pd_event,
        /* register from this event (including) */

    cm_pd_event_e_type            to_pd_event
          /* to this event (including) */
)
/*lint -esym(715,client)*/
/*lint -esym(715,proc_id)*/
/*lint -esym(715,pwr_collapse_type)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_pd_event)*/
/*lint -esym(715,to_pd_event)*/
/*lint -esym(715,pd_event_func)*/

{
  #if defined(FEATURE_GPSONE) && defined(FEATURE_APPS_POWER_COLLAPSE)
#error code not present
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_pd_pwr_collapse_dereg */
/*lint +esym(715,client)*/
/*lint +esym(715,proc_id)*/
/*lint +esym(715,pwr_collapse_type)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_pd_event)*/
/*lint +esym(715,to_pd_event)*/
/*lint +esym(715,pd_event_func)*/


/*===========================================================================

FUNCTION cm_client_dbm_event_reg

DESCRIPTION
  This function is to be used by CM clients interested in registering for
  DBM call manager events.

DEPENDENCIES
  cm_client_init must have been called previously.

RETURN VALUE
  results of the registration.

SIDE EFFECTS
  None

===========================================================================*/

cm_client_status_e_type cm_client_dbm_event_reg(

    cm_client_id_type               client,
        /* Requesting client */

    cm_dbm_event_f_type             *dbm_event_func,
        /* Pointer to a callback function to notify the client of dbm
        ** events */

    cm_client_event_reg_e_type      event_reg_type,
        /* Is this a registration or de-reg */

    cm_dbm_event_e_type             from_dbm_event,
        /* register from this event (including) */

    cm_dbm_event_e_type             to_dbm_event,
        /* to this event (including) */


    cm_dbm_cmd_err_f_type           *dbm_cmd_err_func
      /* pointer to a callback function to notify the client of dbm
      ** command errors of OTHER clients */

)
/*lint -esym(715,client) */
/*lint -esym(715,dbm_event_func) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_dbm_event) */
/*lint -esym(715,to_dbm_event) */
/*lint -esym(715,dbm_cmd_err_func) */
{
  #ifdef FEATURE_DBM
  boolean reg_flag;
  struct cm_client_s  *client_ptr;

  client_ptr = cmclient_get_client_ptr( client );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != CM_INIT_MASK) )
  {
     return CM_CLIENT_ERR_CLIENT_ID;
  }

  if(dbm_event_func == NULL)
  {
     /* deregister all of client's events */

     bit_arr_init( &client_ptr->dbm_event_bit_arr,               /* bit array struct*/
                   client_ptr->dbm_event_bit_buf,                     /* bit buffer */
                   (8 * ARR_SIZE(client_ptr->dbm_event_bit_buf)) );   /* bit size   */

     client_ptr->dbm_event_func   = dbm_event_func;
     client_ptr->dbm_cmd_err_func = dbm_cmd_err_func;

     return CM_CLIENT_OK;
  }


  if (!BETWEEN(from_dbm_event, CM_DBM_EVENT_NONE, CM_DBM_EVENT_MAX))
  {
      return CM_CLIENT_ERR_FROM_EVENT;
  }

  if (!BETWEEN(to_dbm_event, CM_DBM_EVENT_NONE, CM_DBM_EVENT_MAX))
  {
    return CM_CLIENT_ERR_TO_EVENT;
  }

  if ( from_dbm_event > to_dbm_event )
  {
     return CM_CLIENT_ERR_FROM_EVENT;
  }

  switch(event_reg_type)
  {
     case CM_CLIENT_EVENT_REG:
             reg_flag=TRUE;
             break;

     case CM_CLIENT_EVENT_DEREG:
             reg_flag=FALSE;
             break;

     default:
             return CM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  bit_arr_set_bits( &client_ptr->dbm_event_bit_arr,       /* for this bit array */
                    reg_flag,                      /* set bits to ON/off */
                    (int)from_dbm_event,            /* start from this */
                    (int)to_dbm_event);             /* end with this bit */


  /* update SMS event and other clients' command error callback functions */

  client_ptr->dbm_event_func   = dbm_event_func;
  client_ptr->dbm_cmd_err_func = dbm_cmd_err_func;

  return CM_CLIENT_OK;
  #else /* FEATURE_DBM */
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif /* else of FEATURE_DBM */

}
/*lint +esym(715,client) */
/*lint +esym(715,dbm_event_func) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_dbm_event) */
/*lint +esym(715,to_dbm_event) */
/*lint +esym(715,dbm_cmd_err_func) */


/*===========================================================================

FUNCTION cm_client_dbm_class_reg

DESCRIPTION
  This function is to be used by CM clients interested in registering for
  DBM classes like PD, SDB etc..

DEPENDENCIES
  cm_client_init must have been called previously.

RETURN VALUE
  results of the registration.

SIDE EFFECTS
  None

===========================================================================*/

cm_client_status_e_type cm_client_dbm_class_reg(

    cm_client_id_type               client,
        /* Requesting client */

    cm_client_event_reg_e_type      event_reg_type,
        /* Is this a registration or de-reg */

    cm_dbm_class_e_type             from_dbm_class,
        /* register from this class (including) */

    cm_dbm_class_e_type             to_dbm_class
        /* to this class (including) */

)
/*lint -esym(715,client) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_dbm_class) */
/*lint -esym(715,to_dbm_class) */
{
  #ifdef FEATURE_DBM
  boolean reg_flag;
  struct cm_client_s  *client_ptr;

  client_ptr = cmclient_get_client_ptr( client );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != CM_INIT_MASK) )
  {
     return CM_CLIENT_ERR_CLIENT_ID;
  }


  if (!BETWEEN(from_dbm_class, CM_DBM_CLASS_NONE, CM_DBM_CLASS_MAX))
  {
      return CM_CLIENT_ERR_FROM_EVENT;
  }

  if (!BETWEEN(to_dbm_class, CM_DBM_CLASS_NONE, CM_DBM_CLASS_MAX))
  {
    return CM_CLIENT_ERR_TO_EVENT;
  }

  if ( from_dbm_class > to_dbm_class )
  {
     return CM_CLIENT_ERR_FROM_EVENT;
  }

  switch(event_reg_type)
  {
     case CM_CLIENT_EVENT_REG:
             reg_flag=TRUE;
             break;

     case CM_CLIENT_EVENT_DEREG:
             reg_flag=FALSE;
             break;

     default:
             return CM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  bit_arr_set_bits( &client_ptr->dbm_class_bit_arr, /* for this bit array */
                    reg_flag,                      /* set bits to ON/off */
                    (int)from_dbm_class,            /* start from this */
                    (int)to_dbm_class);             /* end with this bit */


  return CM_CLIENT_OK;
  #else /* FEATURE_DBM */
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif /* else of FEATURE_DBM */
}
/*lint +esym(715,client) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_dbm_class) */
/*lint +esym(715,to_dbm_class) */


/*===========================================================================

FUNCTION cm_mm_client_dbm_pwr_collapse_reg

DESCRIPTION
  DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored 
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the 
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use 
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_dbm_event and to_dbm_event, and event_reg_type are used only 
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If 
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of dbm events, a client needs to call on this API twice- once for each
  range of dbm events. In case of conflicting pwr_collapse_type, the last
  one will be honored

DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_dbm_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_dbm_event_f_type          *dbm_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Is this a registration or de-reg */

    cm_dbm_event_e_type           from_dbm_event,
        /* register from this event (including) */

    cm_dbm_event_e_type           to_dbm_event
          /* to this event (including) */
)
/*lint -esym(715,client)*/
/*lint -esym(715,proc_id)*/
/*lint -esym(715,pwr_collapse_type)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_dbm_event)*/
/*lint -esym(715,to_dbm_event)*/
/*lint -esym(715,dbm_event_func)*/

{
  #if defined(FEATURE_DBM) && defined(FEATURE_APPS_POWER_COLLAPSE)
#error code not present
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_dbm_pwr_collapse_reg */
/*lint +esym(715,client)*/
/*lint +esym(715,proc_id)*/
/*lint +esym(715,pwr_collapse_type)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_dbm_event)*/
/*lint +esym(715,to_dbm_event)*/
/*lint +esym(715,dbm_event_func)*/


/*===========================================================================

FUNCTION cm_client_sups_reg

DESCRIPTION
  This function is to be used by CM clients interested in registering for
  SUPS call manager events.

DEPENDENCIES
  cm_client_init must have been called previously.

RETURN VALUE
  results of the registration.

SIDE EFFECTS
  None

===========================================================================*/

cm_client_status_e_type cm_client_sups_reg(

    cm_client_id_type             client,
        /* Requesting client */

    cm_sups_event_f_type         *sups_event_func,
        /* Pointer to a callback function to notify the client of sups
        ** events */

    cm_client_event_reg_e_type    event_reg_type,
        /* Is this a registration or de-reg */

    cm_sups_event_e_type          from_event,
        /* register from this event (including) */

    cm_sups_event_e_type          to_event,
        /* to this event (including) */


    cm_sups_cmd_err_f_type       *sups_cmd_err_func
      /* pointer to a callback function to notify the client of pd
      ** command errors of OTHER clients */

)
/*lint -esym(715,client) */
/*lint -esym(715,sups_event_func) */
/*lint -esym(715,event_reg_type) */
/*lint -esym(715,from_event) */
/*lint -esym(715,to_event) */
/*lint -esym(715,sups_cmd_err_func) */
{
  #if ( defined (FEATURE_WCDMA) || defined (FEATURE_GSM) )
#error code not present
  #else /* ( defined (FEATURE_WCDMA) || defined (FEATURE_GSM) ) */
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif /* else of ( defined (FEATURE_WCDMA) || defined (FEATURE_GSM) ) */
}

/*lint +esym(715,client) */
/*lint +esym(715,sups_event_func) */
/*lint +esym(715,event_reg_type) */
/*lint +esym(715,from_event) */
/*lint +esym(715,to_event) */
/*lint +esym(715,sups_cmd_err_func) */


/*===========================================================================

FUNCTION cm_mm_client_sups_pwr_collapse_reg

DESCRIPTION
  DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored 
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the 
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use 
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_sups_event and to_sups_event, and event_reg_type are used only 
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If 
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of sups events, a client needs to call on this API twice- once for each
  range of sups events. In case of conflicting pwr_collapse_type, the last
  one will be honored

DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_sups_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_sups_event_f_type         *sups_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Is this a registration or de-reg */

    cm_sups_event_e_type          from_sups_event,
        /* register from this event (including) */

    cm_sups_event_e_type          to_sups_event
          /* to this event (including) */
)
/*lint -esym(715,client)*/
/*lint -esym(715,proc_id)*/
/*lint -esym(715,pwr_collapse_type)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_sups_event)*/
/*lint -esym(715,to_sups_event)*/
/*lint -esym(715,sups_event_func)*/

{
  #if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) \
      && defined(FEATURE_APPS_POWER_COLLAPSE)
#error code not present
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_sups_pwr_collapse_reg */
/*lint +esym(715,client)*/
/*lint +esym(715,proc_id)*/
/*lint +esym(715,pwr_collapse_type)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_sups_event)*/
/*lint +esym(715,to_sups_event)*/
/*lint +esym(715,sups_event_func)*/


#ifdef CM_DEBUG
#error code not present
#endif /* CM_DEBUG */


/*===========================================================================
FUNCTION cm_client_mbms_reg

DESCRIPTION


DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/

cm_client_status_e_type cm_client_mbms_reg(

    cm_client_id_type              client,
        /* client id
        */

    cm_mbms_event_f_type         *mbms_event_func,
        /* Pointer to a callback function to notify the client
        ** of MBMS events
        */

    cm_client_event_reg_e_type    event_reg_type,
        /* Registering or deregistering?
        */

    cm_mbms_event_e_type          from_mbms_event,
        /* register from this event (including)
        */

    cm_mbms_event_e_type          to_mbms_event,
          /* to this event (including)
          */

    cm_mbms_cmd_err_f_type        *mbms_cmd_err_func
      /* pointer to a callback function to notify the client of MBMS
      ** command errors of OTHER clients
      */
)
/*lint -esym(715,client)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_mbms_event)*/
/*lint -esym(715,to_mbms_event)*/
/*lint -esym(715,mbms_event_func)*/
/*lint -esym(715,mbms_cmd_err_func)*/
{
  #ifdef FEATURE_MODEM_MBMS
#error code not present
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif /* #if defined (FEATURE_MODEM_MBMS) */

}



/*lint +esym(715,client)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_mbms_event)*/
/*lint +esym(715,to_mbms_event)*/
/*lint +esym(715,mbms_event_func)*/
/*lint +esym(715,mbms_cmd_err_func)*/

/*===========================================================================
FUNCTION cm_client_bcmcs_reg

DESCRIPTION


DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_client_bcmcs_reg(

    cm_client_id_type           client,
        /* client id */

    cm_bcmcs_event_f_type          *bcmcs_event_func,
        /* Pointer to a callback function to notify the client of phone
        ** events */

    cm_client_event_reg_e_type  event_reg_type,

    cm_bcmcs_event_e_type          from_bcmcs_event,
        /* register from this event (including) */

    cm_bcmcs_event_e_type          to_bcmcs_event,
          /* to this event (including) */

    cm_bcmcs_cmd_err_f_type        *bcmcs_cmd_err_func
      /* pointer to a callback function to notify the client of phone
      ** command errors of OTHER clients */
)
/*lint -esym(715,client)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_bcmcs_event)*/
/*lint -esym(715,to_bcmcs_event)*/
/*lint -esym(715,bcmcs_event_func)*/
/*lint -esym(715,bcmcs_cmd_err_func)*/
{
  #if ((defined (FEATURE_BCMCS)) || (defined (FEATURE_MODEM_MBMS)))
  boolean reg_flag;
  struct cm_client_s  *client_ptr;

  client_ptr = cmclient_get_client_ptr( client );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != CM_INIT_MASK) )
  {
      return CM_CLIENT_ERR_CLIENT_ID;
  }

  if( bcmcs_event_func == NULL )
  {

     /* deregister all of client's BCMCS events */

     bit_arr_init( &client_ptr->bcmcs_bit_arr,        /* phone bit array struct */
                   client_ptr->bcmcs_bit_buf,                     /* bit buffer */
                   (8 * ARR_SIZE(client_ptr->bcmcs_bit_buf)) );   /* bit size   */

     /* update phone event and other clients' phone command error callback functions */

     client_ptr->bcmcs_event_func   = bcmcs_event_func;
     client_ptr->bcmcs_cmd_err_func = bcmcs_cmd_err_func;

     return CM_CLIENT_OK;

  }


  if (!BETWEEN(from_bcmcs_event, CM_BCMCS_EVENT_NONE, CM_BCMCS_EVENT_MAX))
  {
      return CM_CLIENT_ERR_FROM_EVENT;
  }

  if (!BETWEEN(to_bcmcs_event, CM_BCMCS_EVENT_NONE, CM_BCMCS_EVENT_MAX))
  {
    return CM_CLIENT_ERR_TO_EVENT;
  }

  if ( from_bcmcs_event > to_bcmcs_event )
  {
    return CM_CLIENT_ERR_FROM_EVENT;
  }

  switch(event_reg_type)
  {
    case CM_CLIENT_EVENT_REG:
      reg_flag=TRUE;
      break;

    case CM_CLIENT_EVENT_DEREG:
      reg_flag=FALSE;
      break;

    default:
      return CM_CLIENT_ERR_EVENT_REG_TYPE;
  }

  bit_arr_set_bits( &client_ptr->bcmcs_bit_arr,  /* for this bit array */
                    reg_flag,                 /* set bits to ON/off */
                    (int)from_bcmcs_event,       /* start from this */
                    (int)to_bcmcs_event);        /* end with this bit */


  /* update BCMCS event and other clients' command error callback functions */

  client_ptr->bcmcs_event_func   = bcmcs_event_func;
  client_ptr->bcmcs_cmd_err_func = bcmcs_cmd_err_func;

  return CM_CLIENT_OK;
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif /* FEATURE_BCMCS || MBMS */
}
/*lint +esym(715,client)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_bcmcs_event)*/
/*lint +esym(715,to_bcmcs_event)*/
/*lint +esym(715,bcmcs_event_func)*/
/*lint +esym(715,bcmcs_cmd_err_func)*/


/*===========================================================================

FUNCTION cm_mm_client_bcmcs_pwr_collapse_reg

DESCRIPTION
  DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored 
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the 
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use 
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_bcmcs_event and to_bcmcs_event, and event_reg_type are used only 
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If 
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of bcmcs events, a client needs to call on this API twice- once for each
  range of bcmcs events. In case of conflicting pwr_collapse_type, the last
  one will be honored

DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_bcmcs_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_bcmcs_event_f_type        *bcmcs_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Full, default, custom */
  
    cm_client_event_reg_e_type    event_reg_type,
        /* Is this a registration or de-reg */

    cm_bcmcs_event_e_type         from_bcmcs_event,
        /* register from this event (including) */

    cm_bcmcs_event_e_type         to_bcmcs_event
          /* to this event (including) */
)
/*lint -esym(715,client)*/
/*lint -esym(715,proc_id)*/
/*lint -esym(715,pwr_collapse_type)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_bcmcs_event)*/
/*lint -esym(715,to_bcmcs_event)*/
/*lint -esym(715,bcmcs_event_func)*/

{
  #if defined(FEATURE_BCMCS) && defined(FEATURE_APPS_POWER_COLLAPSE)
#error code not present
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_bcmcs_pwr_collapse_reg */
/*lint +esym(715,client)*/
/*lint +esym(715,proc_id)*/
/*lint +esym(715,pwr_collapse_type)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_bcmcs_event)*/
/*lint +esym(715,to_bcmcs_event)*/
/*lint +esym(715,bcmcs_event_func)*/

 
/*===========================================================================
FUNCTION cm_client_nas_reg

DESCRIPTION

  This function allows a client to register the following NAS
  callbacks and events with CM:

   1. Callback function to receive registered NAS events
   2. Register for and de-register from receiving NAS events
   3. Callback function to receive other clients' NAS command errors.

   If a client registers a callback function to receive NAS event
   notification, it also needs to register one or more NAS events
   it is interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of NAS events, a client needs to call on this API twice -
   once for each range of BCMCS events.

   If a client registers a callback function to receive other clients'
   NAS command errors, it will receive notifications on all possible
   NAS command errors caused by other clients. A client cannot
   register to receive only a subset of NAS command errors.
   The NULL function pointer can also be used for the callback function
   if a client is not interested in receiving that callback.
   For example, if a client is not interested in
   receiving other clients' NAS command errors, it can set
   nas_cmd_err_func = NULL. However, if the nas_event_func is set to NULL,
   all registered NAS events are also de-registered and the
   event_reg_type, from_nas_event, and to_nas_event parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  Request status

SIDE EFFECTS
  None

===========================================================================*/
cm_client_status_e_type cm_client_nas_reg(

    cm_client_id_type              client,
        /* client id
        */

    cm_nas_event_f_type         *nas_event_func,
        /* Pointer to a callback function to notify the client
        ** of NAS events
        */

    cm_client_event_reg_e_type     event_reg_type,
        /* Registering or deregistering?
        */

    cm_nas_event_e_type          from_nas_event,
        /* register from this event (including)
        */

    cm_nas_event_e_type          to_nas_event,
          /* to this event (including)
          */

    cm_nas_cmd_err_f_type        *nas_cmd_err_func
      /* pointer to a callback function to notify the client of NAS
      ** command errors of OTHER clients
      */
)
/*lint -esym(715,client)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_nas_event)*/
/*lint -esym(715,to_nas_event)*/
/*lint -esym(715,nas_event_func)*/
/*lint -esym(715,nas_cmd_err_func)*/
{

  #if (defined FEATURE_WMS_CM_NAS_INTERFACES) && ((defined (FEATURE_GSM) || defined FEATURE_WCDMA))
#error code not present
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif    /* FEATURE_WMS_CM_NAS_INTERFACES && FEATURE_GSM || FEATURE_WCDMA */

}  /* cm_client_nas_reg */
/*lint +esym(715,client)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_nas_event)*/
/*lint +esym(715,to_nas_event)*/
/*lint +esym(715,nas_event_func)*/
/*lint +esym(715,nas_cmd_err_func)*/


/*===========================================================================

FUNCTION cm_mm_client_nas_pwr_collapse_reg

DESCRIPTION
  DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored 
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the 
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use 
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_nas_event and to_nas_event, and event_reg_type are used only 
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If 
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of nas events, a client needs to call on this API twice- once for each
  range of nas events. In case of conflicting pwr_collapse_type, the last
  one will be honored

DEPENDENCIES
  Client must have already been initialized with cm_client_init().


RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_client_status_e_type cm_mm_client_nas_pwr_collapse_reg(

    cm_client_id_type             client,
        /* client id */

    cm_nas_event_f_type          *nas_event_func,
        /* Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /* The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /* The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /* Is this a registration or de-reg */

    cm_nas_event_e_type           from_nas_event,
        /* register from this event (including) */

    cm_nas_event_e_type           to_nas_event
          /* to this event (including) */
)
/*lint -esym(715,client)*/
/*lint -esym(715,proc_id)*/
/*lint -esym(715,pwr_collapse_type)*/
/*lint -esym(715,event_reg_type)*/
/*lint -esym(715,from_nas_event)*/
/*lint -esym(715,to_nas_event)*/
/*lint -esym(715,nas_event_func)*/

{
  #if ( defined (FEATURE_WMS_CM_NAS_INTERFACES) \
      && ( defined (FEATURE_GSM) || defined (FEATURE_WCDMA) )) \
      && defined(FEATURE_APPS_POWER_COLLAPSE)
#error code not present
  #else
  return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE;
  #endif

} /* cm_mm_client_nas_pwr_collapse_reg */
/*lint +esym(715,client)*/
/*lint +esym(715,proc_id)*/
/*lint +esym(715,pwr_collapse_type)*/
/*lint +esym(715,event_reg_type)*/
/*lint +esym(715,from_nas_event)*/
/*lint +esym(715,to_nas_event)*/
/*lint +esym(715,nas_event_func)*/

 
/*===========================================================================

FUNCTION cmclient_get_client_id_for_client_type

DESCRIPTION
  Returns client id that corresponds to a client type

DEPENDENCIES
  None

RETURN VALUE
  client id

SIDE EFFECTS
  None

===========================================================================*/
cm_client_id_type cmclient_get_client_id_for_client_type
(

    cm_client_type_e_type client_type
      /* Client type */
)
{
    unsigned int i;

    for ( i = 0; i < ARR_SIZE( cmclient_map_arr ); i++ )
    {
        if (cmclient_map_arr[i].client_type == client_type)
        {
            return cmclient_map_arr[i].client_id;
        }
    }

    /* This catches bogus client_type */
    CM_ASSERT(FALSE);
    return CM_CLIENT_ID_ANONYMOUS;
}


/*=========================================================================

FUNCTION 
  cmclient_crit_sect_ptr

DESCRIPTION

  Return pointer to CM client critical section. 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
rex_crit_sect_type* cmclient_crit_sect_ptr(void)
{
  static rex_crit_sect_type cmclient_crit_sect; 
  /* The one and only CM critical section */

  return &cmclient_crit_sect; 
}

/*===========================================================================

FUNCTION 
  cmclient_init_crit_sect

DESCRIPTION
  Initialize CM critical section. 
  This function must be called before any other part of CM is initialized 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_init_crit_sect(void)
{
  rex_init_crit_sect(cmclient_crit_sect_ptr());
}


