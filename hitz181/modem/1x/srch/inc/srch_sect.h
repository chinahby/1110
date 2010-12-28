#ifndef SRCH_SECT_H
#define SRCH_SECT_H
/*====*====*====*====*====*====*====*========*====*====*====*====*====*====*==*

        S E A R C H   T A S K  --  S E R C H   S E C T O R

GENERAL DESCRIPTION
  This module contains the module that maintains and operates all known
  and unknown sectors as well as creates and manages lists containing these
  sectors.

EXTERNALIZED FUNCTIONS
  srch_sect_init
  srch_sect_reset
  srch_sect_loaded
  srch_sect_update_n
  srch_sect_update_a
  srch_sect_idle_assign
  srch_sect_tc_reset_n
  srch_sect_tc_drop_oldest_c
  srch_sect_tc_update_a
  srch_sect_add
  srch_sect_delete
  srch_sect_find
  srch_sect_modified
  srch_sect_build_all
  srch_sect_a
  srch_sect_set_ref_sect
  srch_sect_delete_nset
  srch_sect_r_add
  srch_sect_r_delete
  srch_sect_r_delete_all
  srch_sect_r_promote
  srch_sect_group_enable
  srch_sect_group_disable
  srch_sect_group_lock
  srch_sect_group_unlock
  srch_sect_group_build
  srch_sect_list_enable
  srch_sect_list_disable
  srch_sect_list_locked
  srch_sect_list_lock
  srch_sect_list_unlock
  srch_sect_list_build
  srch_sect_list_member
  srch_sect_list_get
  srch_sect_list_size
  srch_sect_list_probe
  srch_sect_list_find
  srch_sect_list_is_dirty
  srch_sect_elist_add
  srch_sect_elist_remove
  srch_sect_elist_remove_all
  srch_sect_elist_get_next
  srch_sect_elist_count
  srch_sect_srch4_register
  srch_sect_srch4_deregister
  srch_sect_srch4_active
  srch_sect_srch4_append
  srch_sect_srch4_append_r
  srch_sect_srch4_start
  srch_sect_srch4_abort
  srch_sect_srch4_list_strength_sort
  srch_sect_age
  srch_sect_same_config_as_active
  srch_sect_send_packet
  srch_sect_build_pilot_set_mask

INITIALIZATION AND SEQUENCING REQUIREMENTS
  srch_sect_init must be called before any function of this module is called.

      Copyright (c) 2004 - 2009 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_sect.h#3 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
02/12/09   bb      Add band class and channel information to 
                   srch_sect_pilot_info_image_type structure
06/20/08   mca     Merge from //depot
04/08/08   sst     Add parameter to update aset values in srch_sect_tc_update_a
05/25/07   tjc     Remove hard_handoff flag from Aset update
05/01/07   tjc     Support band class diversity switching
04/27/07   tjc     Add a delete all remainders function
04/27/07   tjc     Support old non-RIF traffic set maintenance for HHO
04/25/07   tjc     Restore NSET parameters when sector is dropped from A/CSET
04/18/07   tjc     Clean up of traffic 1.5
12/15/06   tjc     Modified traffic to use the sector module
07/11/06   tjc     Implemented searcher peak module
07/03/06   awj     Moved pilot_rec_type to sect parms, removed parm translation
06/21/06   tjc     Moved Assign command processing to the sector module
04/05/06   kwo     Added support for srch_sect_nset_valid()
02/02/06   awj     Added sect snapshot interface, srch_sect_get_pilot_image()
01/16/06   awj     Lint cleanup
07/26/05   awj     Moved function pointer type definition to typedef section.
05/31/05   rng     Merged in from SHDR Sandbox.
04/01/05   kwo     Added type for abort callback function
01/10/05   kwo     Added srch_sect_in_list()
                   Added srch_sect_srch4_active_list_id
                   Removed TD and aging from remainder searching
12/27/04   kwo     Added srch_sect_list_is_dirty()
12/15/04   bt      Added srch_sect_group_build and introduced
                   SRCH_SECT_FIRST_ILIST.
12/08/04   jcm     Allow user to set remainder parameters
12/07/04   sst     Changed srch4_1x_add_task() and srch_sect_srch4_append/_r()
                   to remove offset and add receive diversity field parameters
12/02/04   bt      Substitute oldest_neighbor with last_seen_neighbor.
11/18/04   bt      Added oldest same config list
11/18/04   bt      Added oldest same config list
10/28/04   kwo     Added oldest list based on srch time
                   Added Srch4 queue strength sort
09/29/04   sfm     Changed include of srch_fing.h to srch_fing_driver.h
09/13/04   sst     Integrated with ACQ2.0 dev, featurized as FEATURE_ACQ2
09/10/04   jcm     Add srch_sect_add_force(), srch_sect_list_delete_all(),
                   srch_sect_elist_delete_all()
09/10/04   jcm     Convert sector module to be PN,Band,Chan generic
09/08/04   jcm     Rename exported genlog types to include srch_
09/03/04   jcm     Modified genlog to log multiple packet simultaneously
08/27/04   dmw     Added support for genlog logging.
08/06/04   jcm     Add srch_sect_build_pilot_set_mask() for logging purposes
08/03/04   jcm     Rename explicit lists from SECT_LIST to SECT_ELIST
07/28/04   ddh     Cleaned up RVCT compiler warnings.
07/17/04   jcm     Externalize function srch_sect_same_config_as_active()
06/24/04   jcm     Assign values to enums
06/16/04   jcm     Function name cleanup
06/15/04   jcm     Aset update slew is signed
06/02/04   jcm     Add Cheap handoff neighbor list
06/02/04   jcm     Convert to using srchi_t types header file
05/28/04   jcm     implementation, first cut
=============================================================================*/


/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"

#include "srchi.h"
#include "srchi_t.h"
#include "srch_genlog.h"
#include "srch_rx_t.h"
#include "srch4trans.h"


/*=============================================================================
             LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
=============================================================================*/
/*-----------------------------------------------------------------------------
      Typedefs
-----------------------------------------------------------------------------*/
typedef enum
{
  SRCH_SECT_FIRST_LIST         =  0,
  SRCH_SECT_ELIST_A            =  0,  /* First explicit list */
  SRCH_SECT_ELIST_FING         =  1,
  SRCH_SECT_ELIST_SRCH4_1      =  2,
  SRCH_SECT_ELIST_SRCH4_2      =  3,
  SRCH_SECT_ELIST_C            =  4,
  SRCH_SECT_LAST_ELIST         =  SRCH_SECT_ELIST_C,
  SRCH_SECT_FIRST_ILIST        =  7, /* First implicit list */
  SRCH_SECT_LIST_N             =  SRCH_SECT_FIRST_ILIST,
  SRCH_SECT_LIST_REACQ         =  8,
  SRCH_SECT_LIST_PRIO          =  9,
  SRCH_SECT_LIST_SFREQ         = 10,
  SRCH_SECT_LIST_TC_N          = SRCH_SECT_LIST_SFREQ,
  SRCH_SECT_LIST_OFREQ         = 11,
  SRCH_SECT_LIST_TC_CF_N       = SRCH_SECT_LIST_OFREQ,
  SRCH_SECT_LIST_STRONG        = 12,
  SRCH_SECT_LIST_OLD_AGE       = 13,
  SRCH_SECT_LIST_CHEAP         = 14,
  SRCH_SECT_LIST_OLD_TIME      = 15,
  SRCH_SECT_LIST_STRONG_NS     = 16,
  SRCH_SECT_LIST_LAST_SEEN_SC_STRONG   = 17,
  SRCH_SECT_LIST_TC_CF_SRCH_N  = 18,

  SRCH_SECT_NUM_LISTS

} srch_sect_list_name_type;

typedef enum
{
  SRCH_SECT_GROUP_COMMON =  0,
  SRCH_SECT_GROUP_ZZ     =  1,
  SRCH_SECT_GROUP_NS     =  2,
  SRCH_SECT_GROUP_TC     =  3,
  SRCH_SECT_GROUP_MAX
} srch_sect_group_type;

/* The following list criteria must be reported using srch_sect_modified()
   if one or more of the criteria is modified.  For example, if any sector
   frequency information is changed then you must call:
   srch_sect_modified( SRCH_SECT_CRITERIA_FREQ );
*/
enum
{
  SRCH_SECT_CRITERIA_NONE = 0x0,
  SRCH_SECT_CRITERIA_SRCH_RESULTS = 0x1,  /* Search results (Energy, etc... */
  SRCH_SECT_CRITERIA_FREQ         = 0x2,  /* Sector Frequency information */
};

/* Sector list is an array of pointers to sector structures */
typedef srch_sect_struct_type*const* srch_sect_list_type;

typedef struct
{
  uint16 chips;
  uint8  nc_int;
  uint8  gain;
  uint32 win_size;
  uint32 lecio_threshold;
  uint32 filter_max_len;

} srch_sect_r_parms_type;

typedef struct
{
  uint16 pilot_inc;             /* Pilot increment for remaining sets */
  uint16 ref_index;             /* Reference index */
  uint8  active_cnt;            /* Count of pilots in active set */
  uint8  cand_cnt;              /* Count of pilots in candidate set */
  uint8  neigh_cnt;             /* Count of pilots in neighbor set */
  uint8  srch_state;            /* Searcher state */

  struct
  {
    uint16 pn;
    uint16 strength;
    srch_rx_band_type band;
    srch_rx_chan_type chan;
  } pilot_info[SRCH_MAX_SETS];

  #ifdef FEATURE_IS95B_MDR
  struct
  {
    /* Number of supplemental channels */
    uint8 num_sup_chans;
    /* Specifies which walsh code is being used, including fundamental */
    uint16 code[ 1 + SRCH_SUP_CHANNEL_MAX ];
  } walsh_info[SRCH_ASET_MAX];
  #endif /* FEATURE_IS95B_MDR */

} srch_sect_pilot_info_image_type;

/* function pointer to abort callback */
typedef void (*srch_sect_srch4_abort_type)(srch_sect_list_name_type, void*);

/*-----------------------------------------------------------------------------
      Constants and Macros
-----------------------------------------------------------------------------*/
#define SRCH_SECT_LIST_A_MAX_SIZE      SRCH_ASET_MAX
#define SRCH_SECT_LIST_FING_MAX_SIZE   HW_MAX_FING_CNT
#define SRCH_SECT_LIST_SRCH4_MAX_SIZE  SRCH4_MAX_NUM_TASKS
#define SRCH_SECT_LIST_C_MAX_SIZE      SRCH_CSET_MAX

/* Constant to indicate that the position at which the sector is added is
   not important */
#define SRCH_SECT_ELIST_ADD_LAST       (~0)

/* Constant to return the first item of an explicit list in the get next func */
#define SRCH_SECT_ELIST_GET_FIRST      (0)

/*-----------------------------------------------------------------------------
      Variables
-----------------------------------------------------------------------------*/


/*=============================================================================
              FUNCTION DEFINITIONS FOR MODULE
=============================================================================*/
/*-----------------------------------------------------------------------------
      Exported Functions
-----------------------------------------------------------------------------*/

/*=============================================================================

FUNCTION       SRCH_SECT_INIT
DESCRIPTION    Initializes the sector module and only needs to be called once.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_init(void);


/*=============================================================================

FUNCTION       SRCH_SECT_RESET
DESCRIPTION    Re-initializes the sector module state.  This means

               - Reset all linked lists
               - Clearing sector lists
               - Resetting the active srch4 sector list to first list
               - Deactivating all implicit lists

DEPENDENCIES   The user must cleanup all search4 activities before calling
               this function.  This includes calling
               srch_sect_srch4_deregister() which will cleanup the srch4
               lists.

RETURN VALUE   None

SIDE EFFECTS   Resetting the sector module in addition to resetting the modules
               state will also deregister any search4 queues requested
               previously.

=============================================================================*/
extern void srch_sect_reset(void);


/*=============================================================================

FUNCTION       SRCH_SECT_LOADED
DESCRIPTION    Queries whether sector module is loaded.

DEPENDENCIES   None

RETURN VALUE   TRUE if sector module is currently loaded with sectors.

SIDE EFFECTS   None

=============================================================================*/
extern boolean srch_sect_loaded(void);


/*=============================================================================

FUNCTION       SRCH_SECT_NSET_VALID
DESCRIPTION    Queries whether sector module to see if the nset is valid

DEPENDENCIES   None

RETURN VALUE   TRUE if neighbor list is valid; otherwise, FALSE

SIDE EFFECTS   None

=============================================================================*/
extern boolean srch_sect_nset_valid(void);


/*=============================================================================

FUNCTION       SRCH_SECT_UPDATE_N
DESCRIPTION    Processes a neighbor set update by adding new sectors,
               updating information of any existing sectors, marking for
               deletion any sector not in the command but used in a list,
               and deleting any sectors not in the command or any lists.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_update_n
(
  srch_nset_cmd_type* cmd_ptr  /* Pointer to the Neighbor Set Update Cmd */
);


/*=============================================================================

FUNCTION       SRCH_SECT_UPDATE_A
DESCRIPTION    Processes a active set update by adding the active sector
               if not already known by the sector module and clearing active
               list and adding the sector to it.

DEPENDENCIES   None

RETURN VALUE   Slew required for new active

SIDE EFFECTS   None

=============================================================================*/
extern int32 srch_sect_update_a
(
  srch_pc_aset_cmd_type* cmd_ptr  /* Pointer to the Active Set Update Cmd */
);


/*===========================================================================

FUNCTION       SRCH_SECT_TC_RESET_N
DESCRIPTION    This function is called to reset the neighbor sect upon demotion
               from the active/candidate sets

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_sect_tc_reset_n
(
  srch_sect_struct_type *sector,  /* Pointer to sector being demoted to NSET */
  srch_set_enum_type set          /* The set where this sector came from */
);


/*===========================================================================

FUNCTION       SRCH_SECT_TC_DROP_OLDEST_C
DESCRIPTION    This function is called to handle full Candidate Sets
               according to IS-95 CAI standards. The oldest candidate pilot is
               selected and deleted unless there is more than one oldest pilot,
               in which case the weakest oldest pilot is deleted
               (See CAI 6.6.6.2.6). Deleted Candidate pilots are added to the
               Neighbor list as required by the CAI standard.

DEPENDENCIES   None

RETURN VALUE   Boolean indicating whether or not we dropped a pilot.

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_sect_tc_drop_oldest_c
(
  srch_sect_struct_type *sector  /* Sector to place into the Candidate list */
);


/*=============================================================================

FUNCTION       SRCH_SECT_TC_UPDATE_A
DESCRIPTION    Freshen stale actives including:
               - update fields of existing actives
               - promote pilots from neighbor status
               - create and add previously-unknown pilots

DEPENDENCIES   None

RETURN VALUE   TRUE if a pilot appeared more than once in aset_info
               FALSE otherwise

SIDE EFFECTS   None

=============================================================================*/
boolean srch_sect_tc_update_a
(
  srch_tc_aset_info_type *info,  /* Pointer to the Aset Info Type */
  uint16 std_path_thresh,        /* Standard path threshold */
  boolean fpc_enabled,           /* Whether FPC is enabled or not */
  boolean hho_check,             /* RIF-HHO in progress */
  boolean on_sf                  /* Flag to indicate if on the Serving Freq */
);

/*=============================================================================

FUNCTION       SRCH_SECT_TC_DELETE_CFNSET
DESCRIPTION    Deletes the candidate frequency neighbor set.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
void srch_sect_tc_delete_cfnset(void);

/*=============================================================================

FUNCTION       SRCH_SECT_IDLE_ASSIGN
DESCRIPTION    Processes an assign command

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
void srch_sect_idle_assign
(
  srch_pc_assign_cmd_type* cmd_ptr  /* Pointer to the Assign Cmd */
);


/*=============================================================================

FUNCTION       SRCH_SECT_ADD
DESCRIPTION    Adds a sector to the known pilot linked list.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern srch_sect_struct_type* srch_sect_add
(
  srch_sect_struct_type* sect  /* Sector structure to add */
);


/*=============================================================================

FUNCTION       SRCH_SECT_ADD_FORCE
DESCRIPTION    Adds a sector to the known pilot linked list without checking
               if this sector is a duplicate.  A duplicate is a sector
               with the same PN, band, chan as a sector that already exists
               in the known pilot linked list.

DEPENDENCIES   None

RETURN VALUE   Pointer to the sector module sector added

SIDE EFFECTS   None

=============================================================================*/
srch_sect_struct_type* srch_sect_add_force
(
  srch_sect_struct_type* sector  /* Sector structure to add */
);


/*=============================================================================

FUNCTION       SRCH_SECT_DELETE
DESCRIPTION    Deletes a sector from the known pilot linked list.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_delete
(
  srch_sect_struct_type* sector  /* Sector to delete */
);


/*=============================================================================

FUNCTION       SRCH_SECT_FIND
DESCRIPTION    Searches the known pilot linked list for a specific sector.

DEPENDENCIES   None

RETURN VALUE   Pointer to the sector if found, NULL otherwise.

SIDE EFFECTS   None

=============================================================================*/
extern srch_sect_struct_type* srch_sect_find
(
  srch_pilot_type pilot,   /* Pilot PN Offset */
  srch_rx_band_type band,  /* Band */
  srch_rx_chan_type chan   /* Channel */
);


/*=============================================================================

FUNCTION       SRCH_SECT_MODIFIED
DESCRIPTION    Notifies the sector module that certain sector fields were
               modified.  Each sector structure field may affect one or more
               list criteria and all criteria affected must be reported.
               Failure to do so may result in a stale sector list.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_modified
(
  uint8 criteria  /* Sector criteria modified */
);


/*=============================================================================

FUNCTION       SRCH_SECT_BUILD_ALL
DESCRIPTION    Build all sector lists

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_build_all(void);


/*=============================================================================

FUNCTION       SRCH_SECT_A
DESCRIPTION    Retrieves the active sector.

DEPENDENCIES   None

RETURN VALUE   Returns pointer to the active sector

SIDE EFFECTS   None

=============================================================================*/
extern srch_sect_struct_type*  srch_sect_a(void);


/*=============================================================================

FUNCTION       SRCH_SECT_SET_REF
DESCRIPTION    Sets the reference sector.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_set_ref
(
  srch_sect_struct_type *sector
);


/*=============================================================================

FUNCTION       SRCH_SECT_DELETE_NSET
DESCRIPTION    Deletes the neighbor set.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
void srch_sect_delete_nset(void);

/*=============================================================================

FUNCTION       SRCH_SECT_R_ADD
DESCRIPTION    Creates a remainder and links it into the remainder linked
               list.

DEPENDENCIES   None

RETURN VALUE   Returns pointer to added remainder sector, or NULL if failed.

SIDE EFFECTS   None

=============================================================================*/
extern srch_sect_struct_type* srch_sect_r_add(void);


/*=============================================================================

FUNCTION       SRCH_SECT_R_DELETE
DESCRIPTION    Deletes a remainder node from the remainder linked list.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_r_delete
(
  srch_sect_struct_type* sector  /* Remainder sector to delete */
);


/*=============================================================================

FUNCTION       SRCH_SECT_R_DELETE_ALL
DESCRIPTION    Deletes all remainder nodes from the remainder linked list.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_r_delete_all(void);


/*=============================================================================

FUNCTION       SRCH_SECT_R_PROMOTE
DESCRIPTION    Promotes the remainder sector to the neighbor list and
               deletes the remainder from the remainder linked list.

DEPENDENCIES   None

RETURN VALUE   Pointer to promoted sector or NULL if promotion failed.

SIDE EFFECTS   None

=============================================================================*/
extern srch_sect_struct_type* srch_sect_r_promote
(
  srch_sect_struct_type* sector  /* remainder sector to promote */
);

/*=============================================================================

FUNCTION       SRCH_SECT_R_SET_PARMS
DESCRIPTION    Modifies the remainder sector parameters used when creating
               remainders.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_r_set_parms
(
  srch_sect_r_parms_type* parms  /* remainder parameter values to set */
);

/*=============================================================================

FUNCTION       SRCH_SECT_GROUP_ENABLE
DESCRIPTION    Enables a group of implicit sector lists.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_group_enable
(
  srch_sect_group_type group  /* Group lists to enable */
);


/*=============================================================================

FUNCTION       SRCH_SECT_GROUP_DISABLE
DESCRIPTION    Disables a group of implicit sector lists.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_group_disable
(
  srch_sect_group_type group  /* Group lists to disable */
);


/*=============================================================================

FUNCTION       SRCH_SECT_GROUP_LOCK
DESCRIPTION    Locks the specified list group

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_group_lock
(
  srch_sect_group_type group  /* Group to lock */
);


/*=============================================================================

FUNCTION       SRCH_SECT_GROUP_UNLOCK
DESCRIPTION    Unlocks the specified sector list

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_group_unlock
(
  srch_sect_group_type group  /* Group to unlock */
);


/*=============================================================================

FUNCTION       SRCH_SECT_LIST_ENABLE
DESCRIPTION    Enables the specified implicit sector list.  Enabled
               implicit sector lists are actively managed by the sector module.
               If an implicit sector list is enabled but not used and not
               built it will hold nodes in the known pilot linked list even
               though they should be deleted.  This can cause the known pilot
               linked list to overflow.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_list_enable
(
  srch_sect_list_name_type list  /* Sector list to enable */
);


/*=============================================================================

FUNCTION       SRCH_SECT_LIST_DISABLE
DESCRIPTION    Disables the specified implicit sector list.  Disabled
               implicit sector lists are cleared and are not managed by
               the sector module.  If an implicit sector list is enabled but
               not used and not built it will hold nodes in the known pilot
               linked list even though they should be deleted.  This can cause
               the known pilot linked list to overflow.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_list_disable
(
  srch_sect_list_name_type list  /* Sector list to disable */
);

/*=============================================================================

FUNCTION       SRCH_SECT_LIST_LOCKED
DESCRIPTION    Queries whether the specified sector list is locked.

DEPENDENCIES   None

RETURN VALUE   Number of locks still outstanding.

SIDE EFFECTS   None

=============================================================================*/
extern uint32 srch_sect_list_locked
(
  srch_sect_list_name_type list  /* Sector list whose lock status to query */
);


/*=============================================================================

FUNCTION       SRCH_SECT_LIST_LOCK
DESCRIPTION    Locks the specified sector list

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_list_lock
(
  srch_sect_list_name_type list  /* Sector list to lock */
);


/*=============================================================================

FUNCTION       SRCH_SECT_LIST_UNLOCK
DESCRIPTION    Unlocks the specified sector list

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_list_unlock
(
  srch_sect_list_name_type list  /* Sector list to unlock */
);


/*=============================================================================

FUNCTION       SRCH_SECT_LIST_BUILD
DESCRIPTION    Build the specified sector list.  A locked sector list cannot
               be built and the attempt to do so results in an error.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_list_build
(
  srch_sect_list_name_type list  /* Sector list to build */
);


/*=============================================================================

FUNCTION       SRCH_SECT_LIST_IS_DIRTY
DESCRIPTION    Return the dirty bit status of a list

DEPENDENCIES   None

RETURN VALUE   TRUE if list is dirty and needs to be rebuilt; otherwise, FALSE

SIDE EFFECTS   None

=============================================================================*/
extern boolean srch_sect_list_is_dirty
(
  srch_sect_list_name_type list  /* Sector list to build */
);

/*=============================================================================

FUNCTION       SRCH_SECT_GROUP_BUILD
DESCRIPTION    Builds a group of implicit sector lists.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_group_build
(
  srch_sect_group_type group  /* Group to unlock */
);


/*=============================================================================

FUNCTION       SRCH_SECT_LIST_MEMBER
DESCRIPTION    Checks if the sector is contained in the specified list.

DEPENDENCIES   None

RETURN VALUE   TRUE if a member.

SIDE EFFECTS   None

=============================================================================*/
extern boolean srch_sect_list_member
(
  srch_sect_struct_type *sect,   /* Sector to query */
  srch_sect_list_name_type list  /* Sector list to query */
);


/*=============================================================================

FUNCTION       SRCH_SECT_LIST_GET
DESCRIPTION    Returns a pointer to the requested sector list along with the
               size of the sector list.  The user may request to lock this
               sector list in which case this list is locked from being built
               until unlocked.  The user is responsible for unlocking sector
               lists when done modifying sector information.  The user is
               responsible for notifying the sector module whenever sector
               information is modified, see the function srch_sect_modified().

DEPENDENCIES   None

RETURN VALUE   Sector lists returned by this function are arrays of pointers
               to sector structures.  The user is free to modify the sector
               structure pointed to by this list but after doing so the user
               must notify the sector module of which list criteria has been
               effected.

SIDE EFFECTS   None

=============================================================================*/
extern srch_sect_list_type srch_sect_list_get
(
  srch_sect_list_name_type list,  /* List to return */
  uint32* size,                   /* Filled with number of sect is list */
  boolean lock_list               /* Lock list or not */
);


/*=============================================================================

FUNCTION       SRCH_SECT_LIST_SIZE
DESCRIPTION    Returns the number of elements in a sector list.  Explicit
               sector lists are a fixed size and may contain empty elements.
               Srch4 and implicit sector lists are variable sized but are bounded.

DEPENDENCIES   None

RETURN VALUE   Number of sectors in the specified sector list

SIDE EFFECTS   None

=============================================================================*/
extern uint32 srch_sect_list_size
(
  srch_sect_list_name_type list  /* Sector list to query */
);


/*=============================================================================

FUNCTION       SRCH_SECT_LIST_PROBE
DESCRIPTION    Returns a pointer to the sector in the sector list at the
               position specified.


DEPENDENCIES   None

RETURN VALUE   Pointer to sector specified, or NULL if empty.

SIDE EFFECTS   None

=============================================================================*/
extern srch_sect_struct_type* srch_sect_list_probe
(
  srch_sect_list_name_type list,  /* Sector list to query sector in */
  uint32 index                    /* Position in sector list */
);

/*=============================================================================

FUNCTION       SRCH_SECT_LIST_DELETE_ALL
DESCRIPTION    Deletes all the sectors contained in the list

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
void srch_sect_list_delete_all
(
  srch_sect_list_name_type list   /* Sector list to delete sectors from */
);

/*=============================================================================

FUNCTION       SRCH_SECT_IN_LIST
DESCRIPTION    Returns TRUE if a sector is in a specified list

DEPENDENCIES   None

RETURN VALUE   TRUE if sector is in the list; otherwise, FALSE

SIDE EFFECTS   None

=============================================================================*/
extern boolean srch_sect_in_list
(
  srch_sect_struct_type     *sector,
  srch_sect_list_name_type  id
);

/*=============================================================================

FUNCTION       SRCH_SECT_LIST_FIND
DESCRIPTION    Searches for a specific sector in the sector list.

DEPENDENCIES   None

RETURN VALUE   TRUE if found in the list, FALSE otherwise.

SIDE EFFECTS   None

=============================================================================*/
extern boolean srch_sect_list_find
(
  srch_sect_list_name_type list,    /* Sector list to search */
  srch_sect_struct_type*   sector   /* Sector to find */
);


/*=============================================================================

FUNCTION       SRCH_SECT_ELIST_ADD
DESCRIPTION    Add a sector to the specified position in an explicit sector
               list.  Rules apply when adding sectors to certain sector lists
               (adding to active sector list removes the sector from neighbor
               sector list).  See explicit sector list definitions for details.

DEPENDENCIES   None

RETURN VALUE   TRUE if addition was successful, FALSE otherwise.

SIDE EFFECTS   None

=============================================================================*/
extern boolean srch_sect_elist_add
(
  srch_sect_list_name_type list,    /* List to add sector to */
  uint32                   index,   /* Position in list */
  srch_sect_struct_type*   sector   /* Sector to add */
);


/*=============================================================================

FUNCTION       SRCH_SECT_ELIST_REMOVE
DESCRIPTION    Remove a sector from the specified position in an explicit
               sector list. Rules apply when removing sectors from specific
               sector lists (Removing from active sector list adds the sector
               to neighbor sector list). See explicit sector list definitions
               for details.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_elist_remove
(
  srch_sect_list_name_type list,  /* Sector list to remove sector from */
  uint32 start,                   /* First position in sector list to remove*/
  uint32 num                      /* Number of sectors to remove */
);


/*=============================================================================

FUNCTION       SRCH_SECT_ELIST_REMOVE_ALL
DESCRIPTION    Removes all sectors from an explicit sector list. Rules
               apply when removing sectors from certain sector lists
               (Removing from active sector list adds sector to neighbor sector
               list).  See explicit sector list definitions for details.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_elist_remove_all
(
  srch_sect_list_name_type list  /* Sector list to clear */
);

/*=============================================================================

FUNCTION       SRCH_SECT_ELIST_DELETE_ALL
DESCRIPTION    Deletes all the sectors contained in the list

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
void srch_sect_elist_delete_all
(
  srch_sect_list_name_type list    /* Sector list to delete sectors from */
);

/*=============================================================================

FUNCTION       SRCH_SECT_ELIST_GET_NEXT
DESCRIPTION    Gets the next sector in the list (INCLUSIVE)

DEPENDENCIES   None

RETURN VALUE   Index value of the next sector, size if at the end

SIDE EFFECTS   None

=============================================================================*/
uint32 srch_sect_elist_get_next
(
  srch_sect_list_name_type list,    /* Sector list to search */
  uint32 start_idx                  /* Index to start search from */
);

/*=============================================================================

FUNCTION       SRCH_SECT_ELIST_COUNT
DESCRIPTION    Returns the count of non-NULL items in the current explicit list.
               This is different than srch_sect_elist_size, since that returns
               the maximum size of the list for explicit lists.

DEPENDENCIES   None

RETURN VALUE   Number of sectors in the specified explicit sector list

SIDE EFFECTS   None

=============================================================================*/
uint32 srch_sect_elist_count
(
  srch_sect_list_name_type list    /* Sector list to get count of */
);

/*=============================================================================

FUNCTION       SRCH_SECT_SRCH4_REGISTER
DESCRIPTION    This function registers 1x with searcher 4. Once registered
               1x will "own" a queue, and it's callbacks will be set.

DEPENDENCIES   None

RETURN VALUE   Srch4 driver registration status.

SIDE EFFECTS   None

=============================================================================*/
extern int32 srch_sect_srch4_register
(
  srch4_priority_type priority,  /* Search priority */
  int16  freq_offset,            /* freq offset ( 2.34375 Hz/LSB ) */
  void (*dump_cb_func)(srch_sect_list_name_type, void*),
                                 /* Pointer to a callback function */
                                 /* to be called upon status dump  */
  void (*lost_cb_func)(srch_sect_list_name_type, void*),
                                 /* Pointer to a callback function */
                                 /* to be called upon lost dump    */
  void (*reg_cb_func)(void *),   /* Pointer to a callback function  */
                                 /* to be called upon successful    */
                                 /* assignment to a queue   */
  void  *reg_cb_data             /* Register callback data  */
);


/*=============================================================================

FUNCTION       SRCH_SECT_SRCH4_DEREGISTER
DESCRIPTION    This function deregisters 1x from it's assigned queue.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_srch4_deregister(void);


/*=============================================================================

FUNCTION       SRCH_SECT_SRCH4_ACTIVE
DESCRIPTION    This function checks the status of the assigned queue.

DEPENDENCIES   None

RETURN VALUE   Returns TRUE if assigned queue is active.

SIDE EFFECTS   None

=============================================================================*/
extern boolean srch_sect_srch4_active(void);


/*=============================================================================

FUNCTION       SRCH_SECT_SRCH4_APPEND
DESCRIPTION    Appends one or more sectors from the specified sector list to
               the srch4 explicit sector list.  If the sector list cannot accept
               all the sectors requested the extras are ignored.  The parameters
               transmit_diversity, and age_searches correspond to searcher4
               setting per sector and all sectors added per call use the
               same settings.

DEPENDENCIES   None

RETURN VALUE   Number of sectors appended.

SIDE EFFECTS   None

=============================================================================*/
extern uint32 srch_sect_srch4_append
(
  srch_sect_list_name_type list,  /* Source sector list */
  uint32 start,                   /* Beginning index into source sector list */
  uint32 num,                     /* Number of sectors to search */
  uint32 max_tasks,               /* Maximum number of srch4 tasks allowed */
  boolean rd,                     /* Flag to indicate use of RD  */
  boolean td,                     /* Flag to indicate use of TD  */
  boolean age_searches            /* Flag to indicate srch aging */
);


/*=============================================================================

FUNCTION       SRCH_SECT_SRCH4_APPEND_R
DESCRIPTION    Appends one or more remainder sectors to the srch4 explicit
               sector list.  If the sector list cannot accept all the sectors
               requested the extras are ignored.  The parameters
               transmit_diversity, and age_searches correspond to searcher4
               setting per sector and all sectors added per call use the same
               settings.  When called, remainder sectors will be generated by
               the sector module.  The sector structure will use the template
               and a PN offset will be generated.  For each remainder sector
               generated the PN offset will walk around the PN circle avoiding
               pilot offsets contained in the active or neighbor sector list.

               The srch4 list is an appending list which means that its size
               grows as more sectors are added.  When the search is complete
               any remainder sectors found to have no energy will be removed
               from the srch4 list and the srch4 list will be collapsed.

DEPENDENCIES   None

RETURN VALUE   Number of sectors appended.

SIDE EFFECTS   None

=============================================================================*/
extern uint32 srch_sect_srch4_append_r
(
  uint32 num_r,                /* Number of remainders to add */
  uint32 max_tasks,            /* Maximum number of srch4 tasks allowed */
  boolean rd                   /* Flag to indicate use of RD  */
);


/*=============================================================================

FUNCTION       SRCH_SECT_SRCH4_START
DESCRIPTION    Starts a searcher4 search using sectors in one of the srch4
               explicit sector lists.  The freq_offset parameter corresponds to
               a searcher 4 parameter.  On completion of the search the
               searcher4 driver will call the sector module which will update
               the search results in the sector structures and then call back
               the user callbacks.

               Two srch4 sector lists are implemented but only one is active
               at a time.  Upon starting a search, the active list is
               considered inactive, locked, and sent to the searcher
               driver.  The other srch4 sector list is now active and
               available for modifications (append, clear, etc...).  Only
               one srch4 sector list may be searched at a time and therefore
               start will fail if a second search is started before the first
               is done.

DEPENDENCIES   None

RETURN VALUE   TRUE if searched started, FALSE if not.

SIDE EFFECTS   None

=============================================================================*/
extern boolean srch_sect_srch4_start
(
  void* (cb_data)      /* Callback data */
);


/*=============================================================================

FUNCTION       SRCH_SECT_SRCH4_ABORT
DESCRIPTION    Aborts the search if the search is active.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_srch4_abort
(
  srch_sect_srch4_abort_type abort_cb_func,
                    /* pointer to a callback function
                       to be called upon abort dump */
  void *(cb_data),  /* callback data */
  boolean sync,     /* synchronous abort flag */
  boolean part_res  /* partial results flag  */
);

/*=============================================================================

FUNCTION       SRCH_SECT_SRCH4_LIST_STRENGTH_SORT
DESCRIPTION    Sorts a search 4 list by strength

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   searched sector's biased filtered energy is updated

=============================================================================*/
void srch_sect_srch4_list_strength_sort
(
  srch_sect_list_name_type list_id
);

/*=============================================================================

FUNCTION       SRCH_SECT_SRCH4_SCTIVE_LIST_ID
DESCRIPTION    Returns the ID of the active search 4 list

DEPENDENCIES   None

RETURN VALUE   Number of sectors appended.

SIDE EFFECTS   None

=============================================================================*/
extern srch_sect_list_name_type srch_sect_srch4_active_list_id (void);

/*=============================================================================

FUNCTION       SRCH_SECT_AGE
DESCRIPTION    Marks a new time and sets the time that the sector was
               searched using OTA parameters.  This should be called whenever a
               search is performed in idle using OTA parameters.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_age
(
  srch_sect_struct_type* sector,  /* Sector age */
  boolean mark_new                /* Mark a new time if TRUE otherwise use
                                     last marked time */
);

/*===========================================================================

FUNCTION       SRCH_SECT_SAME_CONFIG_AS_ACTIVE
DESCRIPTION    This function checks whether a pilot has the "same"
               configuration as the active pilot.

DEPENDENCIES   None

RETURN VALUE   TRUE if pilot has "same" config, FALSE otherwise

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_sect_same_config_as_active
(
  srch_sect_struct_type *sector /* sector to be compared to the active */
);

/*=============================================================================

FUNCTION       SRCH_SECT_SEND_PACKET
DESCRIPTION    Dumps a sector module parameters to a log packet

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_send_packet(void);

/*=============================================================================

FUNCTION       SRCH_SECT_BUILD_PILOT_SET_MASK
DESCRIPTION    Builds the Pilot Set Membership Mask used when logging the
               Searcher 1 subpacket (Version 1).

DEPENDENCIES   None

RETURN VALUE   32 bit Pilot Set Membership Mask.  The first five bits of the
               mask are as defined in the Logging ICD as follows:

                 Bit  Field
                 0    Active Set
                 1    Candidate Set
                 2    Neighbor Set
                 3    Remainder Set
                 4    PreCandidate Set

                 The bits from bit 5 through bit 31 contain the sector's
                 list member mask, so for example, bit 5 of the Pilot Set
                 Membership Mask is equal to bit 0 of the sector list mask.

SIDE EFFECTS   None

=============================================================================*/
extern uint32 srch_sect_build_pilot_set_mask
(
  srch_sect_struct_type* sector  /* Sector to build the mask for */
);

/*=============================================================================

FUNCTION       SRCH_SECT_BUILD_ACTIVE_INFO_SUB
DESCRIPTION    Builds an Active Info Sub Packet Version 1.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_build_active_info_sub
(
  srch_genlog_packet_id_type  id,         /* Id of packet to commit sub */
  sub_commit_func_type        commit_fp   /* F(x) to commit the subpacket  */
);

/*=============================================================================

FUNCTION       SRCH_SECT_BUILD_SUPPORTED_CHAN_MASK
DESCRIPTION    Builds a supported channel mask

DEPENDENCIES   None

RETURN VALUE   uint32 containing the mask

SIDE EFFECTS   None

=============================================================================*/
uint32 srch_sect_build_supported_chan_mask
(
  srch_sect_struct_type *sector
);

/*=============================================================================

FUNCTION       SECT_SECT_GET_PILOT_IMAGE

DESCRIPTION    Fills a pilot set snapshot.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_sect_get_pilot_image
(
  srch_sect_pilot_info_image_type  *pilot_set_info
);

#endif /* SRCH_SECT_H */
