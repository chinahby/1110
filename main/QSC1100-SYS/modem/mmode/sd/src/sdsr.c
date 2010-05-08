/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S Y S T E M   D E T E R M I N A T I O N

                    S Y S T E M   R E C O R D

GENERAL DESCRIPTION
  This file makes up the System Record component of the SD.

  The System Record component encapsulates all the knowledge that SD has of
  systems. This knowledge comes from two major sources; programmable
  knowledge (mostly in the form of a PRL and a SID/NID list that are
  programmed into NV) and an accumulated knowledge that is being collected
  during phone operation.



                 2nd Level System Record Decomposition
                 -------------------------------------


  +---------------------------------------------------------------------+
  |                           SS-Front-end                              |
  +---------------------------------------------------------------------+
                                                        |
                                                        |
  +---------+     +=====================================|===============+
  |         |     |                    System Record    |               |
  |         |     |                                    \|/              |
  |         |     |   +---------------------------------------------+   |
  |         |--------->              Interface - sdsr.h             |   |
  |         |     |   |                  (sdsr_xxx)                 |   |
  |         |     |   +---------------------------------------------+   |
  |         |     |             |                         |             |
  |         |     |            \|/                       \|/            |
  | SS-Core |     |   +-------------------+     +-------------------+   |
  |         |     |   |                   |     |                   |   |
  |         |     |   |   System Record   |     |    System Record  |   |
  |         |     |   |      Table        |     |        Lists      |   |
  |         |     |   |                   |     |                   |   |
  |         |     |   |   (sdsr_tbl_xxx)  |     |   (sdsr_list_xxx) |   |
  |         |     |   |                   |     |                   |   |
  |         |     |   |                   |     |                   |   |
  |         |     |   +-------------------+     +-------------------+   |
  |         |     |                                                     |
  +---------+     +=====================================================+


  System Record Table:
    In the center of the database stands the system record table. The system
    record table contains information on all the systems we have tried to
    acquire at one time or another.

    During power-up, the system record table is populated with systems from
    the MRU table and the PRL's acquisition table. Then during operation, the
    system record table is continuously being updated as SD gathers more
    information about systems.

  System Record Lists:
    In addition to the system record table the database also maintains
    several system record lists and items. A system record list is simply an
    ordered subset of the system record table; or in other words, it is a
    list of references to system record table entries. A system record item
    is a system record list of length one.

    Note that a system record list shall not contain duplicate elements; that
    is, no two elements on the same list shall reference the same system
    record table entry.

    In addition each system record list is of the same length as the system
    record table; or put differently, each system record list is capable of
    referencing all the systems that are stored in the system record table.


EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/sd/rel/08H1/src/sdsr.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
10/02/08   aj      Bandclass support for BC17, 18, 19
09/25/08   sv      Memory reduction changes
09/15/08   pk      Added support for WLAN UOoS
08/20/08   st      Resolve Klockwork defects
06/12/08   aj      Klocwork fixes
03/17/08   vs      Added sdsr_list_sorted_is_same, to sort and compare lists.
09/17/07   pk      Added support for non-Hybrid UOOS
08/29/07   sk      Fixed error in MOST_PREF classification of a system to
                   ignore systems that shall not be acquired due to user pref
                   or target capability.
06/06/07   pk      UOOS enhancements...
05/08/07   pk      Automatic mode UOOS fixes
12/01/06   pk      Added support for Automatic mode for CDMA + GSM UOOS 
                   builds
11/15/06   pk      Added support for HYBR_GW
11/13/06   ic      Lint fixes
09/22/06   rp      Added BC 15 , BC 16 support 
09/08/06   sk      Added WLAN tech mask to sdsr_s_type.
07/13/06   jqi     Added the system lost reason for HDR system lost event.
07/07/06   sk      Added support for creating automatic mode counter part of
                   the last GW system that the MS stays on.
05/22/06   jqi     Made use of the chan list for HDR oh info.
05/17/06   jqi     Added ss information in sdsr table to differenciate the 
                   avoidance over different SD instances.
05/04/06   jqi     Lint fixes.
04/28/06   sk      Added new list PWR_SAVE_LST.
04/17/05   jqi     Added EGSRDM support.
04/09/06   jqi     Increased Register Failure Counter to 6 in over reach.
                   Made "1x Reselection Threshold" as a configurable NV item.
                   Mainline FEATURE_SILENT_REDIAL_20.
                   Connection Deny reason(network busy), avoid the channel
                   60s.
                   No TCA and NoRTC ACK, 3 failures avoid 60s or encountering 
                   ACQ_FAIL, whichever come first.
                   Scan all channels during overreach wake up.
                   Do not cancel avoidance when entering power save mode.
                   Added null pointer check in sdsr_list_is_fade.
                   Mainline FEATURE_SILENT_REDIAL_20
03/27/06   jqi     Kept PLMN id of manual list intact when network selection 
                   mode is auto.
03/17/06   pk      Corrected sdsr_list_del_list_cnt
03/9/06    jqi     DO system reselection optimization(Sys resl at DO MO call
                   end.)
                   Multichannel avoidance.
                   Reset the DO failure counter when Subnet ID is changed.
                   Do not change sd state if only srv domain is changed.
03/05/06   jqi     Merged WCDMA BC3,4,8 and 9 support.
02/08/06   pk      Replaced SD_ERR with SD_MSG_ERROR
01/09/06   sk      Made changes to treat custom home system as home system.
                   Fixed sdsr_comp to return 0 when there is no difference.
                   Added sdsr_comp2() for use in list sorting.
12/02/05   jqi     Lint 8 fixes.
11/07/05   jqi     Removed F3 message in sdsr_list_intersect().
11/02/05   sk      Fixed all Lint and compiler warnings.
10/18/05   jqi     Increased the system record table size to 200.
10/14/05   jqi     Added BC14 support.
09/28/05   pk      Modified list integrity check in sdsr_list_del_pos()
09/10/05   pk      Added sdsr_list_del_list_cnt()
08/05/05   jqi     Added support for Best preferred acquisition for DO.
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
07/18/05   ka      Network profile id is no more used to compare similarity
                   of two WLAN system.
07/11/05   jqi     Added avoidance timer for access failures and fades.
06/26/05   ic      Lint and compiler warnings cleanup
05/05/05   sj      Fixed issue with SR add not updating the profileid/
                   bss_type.
03/28/05   RC      Fix printing in sdsr_print.
01/30/05   sj      Added support for WLAN.
01/27/05   ka      Added support for Band class 10, 11, 12
01/10/05   dk      Added sdsr_is_same_gw_lac() function.
12/04/04   jqi     Power save mode optimization for GSM/UMTS targets.
08/26/04   jqi     Lint fix.
08/20/04   sj      Added support for Auto A/B.
07/29/04   jqi     Quadband optimizatioin - eliminate scheduling.
07/14/04   jqi     Update GW last user system upon acquired limited system.
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
06/07/04   jqi     More quad-band enhancement for automatic mode.
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
04/19/04   jqi     Fixed lint -for error-level : error.
12/12/03   RC      Added support for FEATURE_INT_ROAMING
11/14/03   JQ      Fixes for IS683D support.
11/06/03   RC      Optimized the side of sdsr_s_type.
10/31/03   JQ      Added support for latest IS-683D format changes.
09/22/03   SJ      Updated sdsr_init to include sys parameter for init.
08/18/03   RC      Fixed issues with IS-683D implementation.
07/15/03   JQ      Added Extended PRL support.
06/09/03   RC      Added a len parameter to sdsr_is_same_is856_sid().
05/27/03   RC      Added to sdsr_list_store_and_app2().
03/27/03   RC      For non 1x targets use a small table size to save on RAM.
03/07/03   SJ      Changed MSG level for few debug messages.
01/20/03   jqi     Added suport for band class 6.
01/16/03   SJ      Changed use of DB_ROAM* to SYS_ROAM_*.
11/07/02   RC      Added support for new hybrid-HDR power-save algorithm.
10/15/02   RC      Added prl_assn filed to sdsr_tbl_storexxx() functions.
06/19/02   jqi     Eliminated compiler warning.
05/17/02   RC      Added support for IS-683C, PN and data association fields.
03/08/02   hxw     Added GPS mode support for MSBased gpsOne.
03/14/02   RC      Optimized sdsr_lists_hybr_pref_switch().
02/06/02   RC      Eliminated compiler warnings.
01/23/02   RC      Added support for Rel. 3.0 CDMA-HDR hybrid operation.
12/12/01   RC      Added support for CDMA-HDR hybrid operation.
12/07/01   jqi     Added support for band class 5.
10/30/01   RC      Added support for IS-683C collcated systems.
07/20/01   RC      Added support for IS-683C.
05/23/01   RC      Eliminated compiler warning.
03/27/01   RC      Changes to support tri-mode targets.
02/26/01   RC      Changes to support dual-mode targets.
01/03/01   RC      Changes per 4th week of I&T.
11/27/00   RC      Eliminated sched_start_time.
11/17/00   RC      Initial release.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "sdsr.h"     /* External interface to SDSR.C (System Record)*/
#include "sd.h"       /* External interface to sd.c */
#include "sdi.h"      /* Internal interface to sd.c */
#include "sdss.h"     /* External interface to sdss.c */
#include "sdssscr.h"  /* External interface to sdssscr.c (SS-Script) */
#include "sdprl.h"    /* External interface to sdprl.c */
#include "prl.h"      /* External interface to prl.c */
#include "sddbg.h"    /* SD debug macros */

#include "comdef.h"   /* Definition for basic types and macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */
#include "sys.h"      /* Declarations for common types. */
#include "clk.h"




/* <EJECT> */
/*===========================================================================
  TO DO

===========================================================================*/



/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
=============================== SYSTEM RECORD ===============================
=============================================================================
=============================================================================
===========================================================================*/

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW) 
/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_init_plmn_sys

DESCRIPTION
  Set plmn fields with default value.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static  void                       sdsr_init_plmn_sys(

        sd_plmn_s_type             *plmn_ptr
        /* Pointer to sid union type (PLMN)
        */
)
{
  SD_ASSERT(plmn_ptr != NULL);

  sys_plmn_undefine_plmn_id( &plmn_ptr->plmn_id );


} /*sdsr_init_plmn_sys*/
#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_init

DESCRIPTION
  Initializes a system record.

  NOTE! this function must be called on a sdsr_s_type object before any other
  sdsr_xxx is called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdsr_init(

        sdsr_s_type               *sr_ptr,
            /* Pointer to a system record which to initialize.
            */
        const sd_sys_s_type       *sys_ptr
            /* Mode/Band/Channel of the record.
            */
)
{
  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT( sys_ptr!= NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the mode/band/channel of the record.
  */
  sr_ptr->sys = *sys_ptr;

  /* Initialize the expected PRL's system table index to 0.
  */
  sr_ptr->idx_expected      = SD_WILDCARD_SYS_IDX;

  /* Initialize the PRL and roaming designation to none.
  */
  sr_ptr->prl               = (byte) SDSS_PRL_MAX;
  sr_ptr->roam              = (sys_roam_status_e_type) SD_ROAM_MAX;

  /* Initialize all PRL association indicators to OFF.
  */
  SDSR_PRL_ASSN_OFF( sr_ptr->prl_assn, SDSR_PRL_ASSN_ALL );

  /* Initialize the SS-Event to none.
  */
  sr_ptr->event             = (word) SDSS_EVT_MAX;
  sr_ptr->event_time        = 0;

  /* Initialize the schedule to none.
  */
  sr_ptr->sched             = (word) SDSSSCR_SCHED_MAX;
  //sr_ptr->sched_start_time  = 0;

  /* Initialize the acquisition counter to 0.
  */
  sr_ptr->new_acq_cnt       = 0;

  /* Initialize the measurement value to -255.
  */
  sr_ptr->meas_val          = -255;

  ///* Initialize the bad points counter to 0.
  //*/
  //sr_ptr->bad_point_cnt     = 0;

  sr_ptr->nw_profile_id       = SYS_NW_PROFILE_ID_INVALID;

  sr_ptr->num_fade            = 0;

  sr_ptr->uptime_fade         = 0;

  /* Initialize the status bit mask to NONE.
  */
  sr_ptr->status_mask = SDSR_STATUS_MASK_NONE;

  /* Initialize the avoidance of which the system selection item is set to 
  ** MAX.
  */
  sr_ptr->sched_ss = SD_SS_MAX;

  /* Update the initial service capability for this system.
  */
  switch( sr_ptr->sys.mode )
  {
    case SD_MODE_CDMA:
      sr_ptr->srv_cap = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
      sr_ptr->sid.is95.sid      = SD_WILDCARD_SID;
      sr_ptr->sid.is95.nid      = SD_WILDCARD_NID;

      sr_ptr->sid.is95.true_sid       = SD_WILDCARD_SID;
      sr_ptr->sid.is95.true_nid       = SD_WILDCARD_NID;
      sr_ptr->sid.is95.mcc            = SD_MCC_NONE;
      sr_ptr->sid.is95.imsi_11_12     = SD_IMSI_11_12_NONE;

      sr_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_AUTO;
      sr_ptr->sid_expected.prm.is95.sid      = SD_WILDCARD_SID;
      sr_ptr->sid_expected.prm.is95.nid      = SD_WILDCARD_NID;
      break;

    case SD_MODE_AMPS:
      sr_ptr->srv_cap = SYS_SRV_CAPABILITY_CS_SERVICE_ONLY;
      sr_ptr->sid.is95.sid      = SD_WILDCARD_SID;
      sr_ptr->sid.is95.nid      = SD_WILDCARD_NID;

      sr_ptr->sid.is95.true_sid       = SD_WILDCARD_SID;
      sr_ptr->sid.is95.true_nid       = SD_WILDCARD_NID;
      sr_ptr->sid.is95.mcc            = SD_MCC_NONE;
      sr_ptr->sid.is95.imsi_11_12     = SD_IMSI_11_12_NONE;

      sr_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_AUTO;
      sr_ptr->sid_expected.prm.is95.sid      = SD_WILDCARD_SID;
      sr_ptr->sid_expected.prm.is95.nid      = SD_WILDCARD_NID;
      break;

    case SD_MODE_HDR:
      sr_ptr->srv_cap = SYS_SRV_CAPABILITY_PS_SERVICE_ONLY;
      break;

    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW) 
      sr_ptr->srv_cap = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
      /* Initialize the plmn to undefined value.
      */
      sdsr_init_plmn_sys( &sr_ptr->sid.plmn );

      /* Initialize the expected plmn id to undefined value.
      ** Set other plmn fields to default value.
      */
      sr_ptr->sid_expected.prm.plmn.lac = PRL_WILDCARD_LAC;

      sr_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_AUTO;
      sdsr_init_plmn_sys( &sr_ptr->sid_expected.prm.plmn );

      /* Initialize the prl to MOST_PREF.
      */
      sr_ptr->prl               = (byte) SDSS_PRL_MOST_PREF;
      #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */
      break;

    case SD_MODE_GPS:
      break;

    case SD_MODE_WLAN:
      #if defined(FEATURE_WLAN)
#error code not present
      #endif /* (FEATURE_WLAN) */
      break;


    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_MAX:
    default:
       SD_ASSERT( FALSE );
       break;

  } /* switch( sr_ptr->sys.mode ) */

} /* sdsr_init */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_sid_encode_mcc_imsi_11_12

DESCRIPTION
  Incode the MCC/IMSI_11_12 into the SID/NID fields.

DEPENDENCIES
  The MCC and IMSI_11_12 should be in their OTA format - see IS-2000.5.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_sid_encode_mcc_imsi_11_12(

        sdsr_s_type               *sr_ptr
            /* Pointer to a system record into which to encide the
            ** MCC/IMSI_11_12.
            */
)
{
  byte  conv_arr[]        = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
  word  prl_mcc;
  byte  prl_imsi_11_12;
  word  mcc;
  byte  imsi_11_12;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mcc               = sr_ptr->sid.is95.mcc;
  imsi_11_12        = sr_ptr->sid.is95.imsi_11_12;

  SD_ASSERT( mcc < 1000 )
  SD_ASSERT( imsi_11_12 < 100 )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert MCC from OTA format to PRL format.
  */
  prl_mcc =  conv_arr[ mcc%10 ];
  mcc /= 10;
  prl_mcc += conv_arr[ mcc%10 ] * 10;
  mcc /= 10;
  prl_mcc += conv_arr[ mcc%10 ] * 100;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert IMSI_11_12 from OTA format to PRL format.
  */
  prl_imsi_11_12 =  conv_arr[ imsi_11_12%10 ];
  imsi_11_12 /= 10;
  prl_imsi_11_12 += (byte) (conv_arr[ imsi_11_12%10 ] * 10);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the MSB bit of the MCC is 0, set sid to PRL_MCC_SID_0. Else, set SID
  ** to PRL_MCC_SID_1.
  */
  if( prl_mcc < 512 ) {
    sr_ptr->sid.is95.sid = PRL_MCC_SID_0;
  }
  else {
    sr_ptr->sid.is95.sid = PRL_MCC_SID_1;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Incode the 9 LSB of the MCC + the 7 bits of the IMSI_11_12 into the NID.
  */
  sr_ptr->sid.is95.nid = (word)( (prl_mcc<<7) | prl_imsi_11_12  );

} /* sdsr_sid_encode_mcc_imsi_11_12 */



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_sid_restore_true

DESCRIPTION
  Restore the true SID/NID values into the SID/NID fields. This function undo
  what sdsr_sid_encode_mcc_imsi_11_12() does.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_sid_restore_true(

        sdsr_s_type               *sr_ptr
            /* Pointer to a system record for which to restore the true
            ** SID/NID.
            */
)
{
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ptr->sid.is95.sid = sr_ptr->sid.is95.true_sid;
  sr_ptr->sid.is95.nid = sr_ptr->sid.is95.true_nid;

} /* sdsr_sid_restore_true */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_hdr_sid_print

DESCRIPTION
  Print HDR Sector ID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_hdr_sid_print(

        const byte                sid_ptr[16]
            /* Pointer to array of 16 byte Sector ID.
            *//*lint -esym(715, sid_ptr) */ /* not used */
)
{
  SD_ASSERT( sid_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_MED( "Sector ID=%x:%x:%x:",
    BYTES_TO_WORD(sid_ptr[0], sid_ptr[1]),
    BYTES_TO_WORD(sid_ptr[2], sid_ptr[3]),
    BYTES_TO_WORD(sid_ptr[4], sid_ptr[5]));

  SD_MSG_MED( "          %x:%x:%x:",
    BYTES_TO_WORD(sid_ptr[6], sid_ptr[7]),
    BYTES_TO_WORD(sid_ptr[8], sid_ptr[9]),
    BYTES_TO_WORD(sid_ptr[10], sid_ptr[11]));

  SD_MSG_MED( "          %x:%x",
    BYTES_TO_WORD(sid_ptr[12], sid_ptr[13]),
    BYTES_TO_WORD(sid_ptr[14], sid_ptr[15]),0);

} /* sdsr_hdr_sid_print */ /*lint +esym(715, sid_ptr) */



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_print

DESCRIPTION
  Print the content of a system record.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_print(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record for which to print its content.
            */

        boolean                   is_print_full
            /* Indicate whether the complete system record information
            ** should be printed.
            */
)
{
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( sr_ptr->sys.mode)
  {
    case SD_MODE_CDMA: 
      SD_MSG_MED( "  mode=CDMA,  chan=%-4d, band=0x%x 0x%x",sr_ptr->sys.chan, 
                                                QWORD_HIGH(sr_ptr->sys.band), 
                                                QWORD_LOW(sr_ptr->sys.band) );
      break;
    case SD_MODE_HDR:
      SD_MSG_MED( "  mode=HDR,   chan=%-4d, band=0x%x 0x%x",sr_ptr->sys.chan, 
                                                QWORD_HIGH(sr_ptr->sys.band), 
                                                QWORD_LOW(sr_ptr->sys.band) );
      break;
    case SD_MODE_GSM:
      SD_MSG_MED( "  mode=GSM,   chan=%-4d, band=0x%x 0x%x",sr_ptr->sys.chan, 
                                                QWORD_HIGH(sr_ptr->sys.band), 
                                                QWORD_LOW(sr_ptr->sys.band) );
      break;
    case SD_MODE_WCDMA:
      SD_MSG_MED( "  mode=WCDMA, chan=%-4d, band=0x%x 0x%x",sr_ptr->sys.chan, 
                                                QWORD_HIGH(sr_ptr->sys.band), 
                                                QWORD_LOW(sr_ptr->sys.band) );
      break;
    case SD_MODE_GW:
      SD_MSG_MED( "  mode=GW,    chan=%-4d, band=0x%x 0x%x",sr_ptr->sys.chan, 
                                                QWORD_HIGH(sr_ptr->sys.band), 
                                                QWORD_LOW(sr_ptr->sys.band) );
      break;
    case SD_MODE_GPS:
      SD_MSG_MED( "  mode=GPS,   chan=%-4d, band=0x%x 0x%x",sr_ptr->sys.chan, 
                                                QWORD_HIGH(sr_ptr->sys.band), 
                                                QWORD_LOW(sr_ptr->sys.band) );
      break;
    case SD_MODE_WLAN:
      SD_MSG_MED( "  mode=WLAN,  chan=%-4d, band=0x%x 0x%x",sr_ptr->sys.chan, 
                                                QWORD_HIGH(sr_ptr->sys.band), 
                                                QWORD_LOW(sr_ptr->sys.band) );
      break;
    case SD_MODE_AMPS:
      SD_MSG_MED( "  mode=AMPS,  chan=%-4d, band=0x%x 0x%x",sr_ptr->sys.chan, 
                                                QWORD_HIGH(sr_ptr->sys.band), 
                                                QWORD_LOW(sr_ptr->sys.band) );
      break;

    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_MAX:
    default:
      SD_MSG_MED( "  Sys mode=%d, band=0x%x 0x%x",
                  sr_ptr->sys.mode, 
                  QWORD_HIGH(sr_ptr->sys.band),  
                  QWORD_LOW(sr_ptr->sys.band) );
      SD_MSG_MED( "  chan=%d",sr_ptr->sys.chan, 0, 0 );
    
  } /* switch case */
  //SD_MSG_HIGH( "Event time=%d",sr_ptr->event_time,0,0 );

  if( ! is_print_full )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (sr_ptr->sys.mode)
  {
    case SD_MODE_HDR:
      sdsr_hdr_sid_print( sr_ptr->sid.is856 );

      SD_MSG_MED( "prl=%d, roam=%d, sched=%d",
                    sr_ptr->prl, sr_ptr->roam, sr_ptr->sched );

      SD_MSG_MED( "idx_exp=%d", sr_ptr->idx_expected,0,0 );
      break;
    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW) 
      SD_MSG_MED( "plmn=%d, %d, %d",
                  sr_ptr->sid_expected.prm.plmn.plmn_id.identity[0],
                  sr_ptr->sid_expected.prm.plmn.plmn_id.identity[1],
                  sr_ptr->sid_expected.prm.plmn.plmn_id.identity[2] );

      SD_MSG_MED( "sel_mode=%d,",
                   sr_ptr->sid_expected.net_select_mode, 0, 0);
    #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */
      break;

    case SD_MODE_WLAN:
      SD_MSG_MED(" net_sel %d,",
                 sr_ptr->sid_expected.net_select_mode,
                 0, 0
                );
      break;

    case SD_MODE_CDMA:
    case SD_MODE_AMPS:
    case SD_MODE_GPS:
    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_MAX:
    default:
      SD_MSG_MED( "SID=%d, prl=%d, roam=%d",
                  sr_ptr->sid.is95.sid, sr_ptr->prl, sr_ptr->roam );

      SD_MSG_MED( "Expected SID=%d, idx_exp=%d, sched=%d",
                   sr_ptr->sid_expected.prm.is95.sid, sr_ptr->sched, 0 );
      break;
  } /* switch */

} /* sdsr_print */



#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW) 
/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_plmn_is_same_sys

DESCRIPTION
  Compare two plmn system records and return TRUE if they reference the same
  system.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the two system records reference the same system. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdsr_plmn_is_same_sys(

        const sd_sid_plmn_wlan_s_type *plmn_a_ptr,
            /* A plmn system which to compare.
            */

        const sd_sid_plmn_wlan_s_type *plmn_b_ptr
            /* A plmn system which to compare.
            */
)
{
  SD_ASSERT( plmn_a_ptr != NULL );
  SD_ASSERT( plmn_b_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( plmn_a_ptr->net_select_mode == plmn_b_ptr->net_select_mode )
  {
    switch( plmn_a_ptr->net_select_mode )
    {
      case SD_NET_SELECT_MODE_LIMITED:
      case SD_NET_SELECT_MODE_HPLMN:
      case SD_NET_SELECT_MODE_MANUAL_SYS:
        return TRUE;

      case SD_NET_SELECT_MODE_AUTO:
      case SD_NET_SELECT_MODE_PRL_SYS:
        return sys_plmn_match( plmn_a_ptr->prm.plmn.plmn_id, plmn_b_ptr->prm.plmn.plmn_id );

      case SD_NET_SELECT_MODE_MAX:
      case SD_NET_SELECT_MODE_MANUAL_BSSID:
      case SD_NET_SELECT_MODE_PRIV_SYS:
      default:
        SD_MSG_LOW( "network selection mode error %d",
                    plmn_a_ptr->net_select_mode, 0, 0 );
        return FALSE;

    } /* switch */

  } /* if */

  return FALSE;
} /* sdsr_plmn_is_same_sys */
#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */

#if defined(FEATURE_WLAN)
#error code not present
#endif /* (FEATURE_WLAN) */

/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_is_same_sys

DESCRIPTION
  Compare two system records and return TRUE if they reference the same
  system.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the two system records reference the same system. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdsr_is_same_sys(

        const sdsr_s_type         *sr_a_ptr,
            /* Pointer to a system record which to compare.
            */

        const sdsr_s_type         *sr_b_ptr
            /* Pointer to a system record which to compare.
            */
)
{
  boolean status = FALSE;
  SD_ASSERT( sr_a_ptr != NULL );
  SD_ASSERT( sr_b_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If both system records don't have the same mode, band, and channel, return
  ** TRUE. Else, return FALSE.
  */
  if( sr_a_ptr->sys.mode != sr_b_ptr->sys.mode ||
      sr_a_ptr->sys.band != sr_b_ptr->sys.band ||
      sr_a_ptr->sys.chan != sr_b_ptr->sys.chan )
  {

    /* System records don't match.
    */
    return FALSE;


  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Additional comparison.
  */
  switch ( sr_a_ptr->sys.mode )
  {
    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW) 
    /* If system record mode is GSM/WCDMA system, need check PLMN and LAC fields.
    ** at this time PLMN and LAC are unknown.
    */
      status = sdsr_plmn_is_same_sys( &sr_a_ptr->sid_expected,
                                      &sr_b_ptr->sid_expected
                                    );
      #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
      break;


    case SD_MODE_WLAN:     
    #if defined(FEATURE_WLAN)
#error code not present
      #endif /* (FEATURE_WLAN) */
      break;

    case SD_MODE_AMPS:
    case SD_MODE_CDMA:
    case SD_MODE_HDR:
    case SD_MODE_GPS:
    case SD_MODE_INACT:
    case SD_MODE_NONE:
    case SD_MODE_MAX:
    default:
      status = TRUE;
      break;

  } /* switch ( sr_a_ptr->sys.mode ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return status;

} /* sdsr_is_same_sys */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_is_same_is856_sid

DESCRIPTION
  Check whether a system record as an ID that is matching the specified
  IS-856 Sector ID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system ID match the specified IS-856 Sector ID. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdsr_is_same_is856_sid(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record to be checked for SID matching.
            */

        const byte                sid_ptr[16],
            /* Pointer to array of 16 byte Sector ID.
            */

        int                       len
            /* Length to match.
            */
)
{
  /* Mask table to isolate MSB bits within a byte.
  */
  static const  byte mask_tbl[] = { 0x00, 0x80, 0xC0, 0xE0,
                                    0xF0, 0xF8, 0xFC, 0xFE };
  byte          mask;
  unsigned int           i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != 0 );
  SD_ASSERT( sid_ptr != 0 );
  SD_ASSERT( len <= 128 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system is not an HDR system, return FALSE.
  */
  if( sr_ptr->sys.mode != SD_MODE_HDR )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i=0;
       i < (unsigned)(len / 8) &&
       i < ARR_SIZE(sr_ptr->sid.is856);
       i++ )
  {
    if( sr_ptr->sid.is856[i] != sid_ptr[i] )
    {
      return FALSE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check the reminder, as needed.
  */
  if( i < ARR_SIZE(sr_ptr->sid.is856) )
  {
    mask = mask_tbl[ len & 0x0007 ];

    if( (sr_ptr->sid.is856[i] & mask) !=
        (sid_ptr[i] & mask) )
    {
      return FALSE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the two SIDs match - return TRUE.
  */
  return TRUE;

} /* sdsr_is_same_is856_sid */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_is_same_sid

DESCRIPTION
  Check whether the system IDs of two system records are equal.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system IDs are equal. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdsr_is_same_sid(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr1_ptr.
            */
)
{
  unsigned int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr1_ptr != 0 );
  SD_ASSERT( sr2_ptr != 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the two systems are of different modes, return FALSE.
  */
  if( sr1_ptr->sys.mode != sr2_ptr->sys.mode )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Compare the two SIDs base on the system's mode.
  */
  switch( sr1_ptr->sys.mode )
  {
    case SD_MODE_AMPS:
    case SD_MODE_CDMA:

      if( sr1_ptr->sid.is95.sid != sr2_ptr->sid.is95.sid ||
          sr1_ptr->sid.is95.nid != sr2_ptr->sid.is95.nid )
      {
        return FALSE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_HDR:

      for( i=0; i < ARR_SIZE(sr1_ptr->sid.is856); i++ )
      {
        if( sr1_ptr->sid.is856[i] != sr2_ptr->sid.is856[i] )
        {
          return FALSE;
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
      return sys_plmn_match( sr1_ptr->sid.plmn.plmn_id,
                             sr2_ptr->sid.plmn.plmn_id );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_WLAN:
    #if defined(FEATURE_WLAN)
      if ( sr1_ptr->sid_expected.net_select_mode !=
                                     sr2_ptr->sid_expected.net_select_mode )
      {
        return FALSE;
      }

      if ( sr1_ptr->sid_expected.net_select_mode ==
                                            SD_NET_SELECT_MODE_MANUAL_BSSID)
      {
        /* Compare the BSSids.
        */
        return sys_is_wlan_bssid_match( &sr1_ptr->sid_expected.prm.wlan.bssid,
                                        &sr2_ptr->sid_expected.prm.wlan.bssid
                                      );
      }

      /* Compare the SSIDs.
      */
      return sys_is_wlan_ssid_match( &sr1_ptr->sid_expected.prm.wlan.ssid,
                                     &sr2_ptr->sid_expected.prm.wlan.ssid
                                   );

     #else
     return FALSE;
     #endif /* (FEATURE_WLAN) */


   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_GPS:
    case SD_MODE_MAX:
    default:
      return FALSE;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the two SIDs match - return TRUE.
  */
  return TRUE;

} /* sdsr_is_same_sid */

/*===========================================================================

FUNCTION sdsr_is_same_gw_lac

DESCRIPTION
  Check whether the LAC of two GW system IDs of two system records are equal.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the LACs are equal. FALSE otherwise.
  This also return TRUE if we are comparing non-GW systems.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdsr_is_same_gw_lac(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr1_ptr.
            */
)
{

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
  #else
  SYS_ARG_NOT_USED(sr1_ptr);
  SYS_ARG_NOT_USED(sr2_ptr);
  #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

}





/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_is_same

DESCRIPTION
  Compare two system records with respect to a specified equality category.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the two system records are the same with respect to the specified
  equality category. FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdsr_is_same(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to a system record which to compare.
            */

        const sdsr_s_type         *sr2_ptr,
            /* Pointer to a system record which to compare.
            */

        sdsr_equal_e_type         equal_cat
            /* Category according to which to check the lists for equality.
            */
)
{
  SD_ASSERT( sr1_ptr != NULL );
  SD_ASSERT( sr2_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( equal_cat, SDSR_EQUAL_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the two system records are the same with respect to the specified
  ** equality category, return TRUE.
  */
  switch( equal_cat )
  {
    case SDSR_EQUAL_MODE:
      if( sr1_ptr->sys.mode == sr2_ptr->sys.mode )
      {
        return TRUE;
      }
      break;

    case SDSR_EQUAL_BAND:
      if( sr1_ptr->sys.band == sr2_ptr->sys.band )
      {
        return TRUE;
      }
      break;

    case SDSR_EQUAL_CHAN:
      if( sr1_ptr->sys.chan == sr2_ptr->sys.chan )
      {
        return TRUE;
      }
      break;

    case SDSR_EQUAL_SID_NID:
      if( sdsr_is_same_sid(sr1_ptr, sr2_ptr) )
      {
        return TRUE;
      }
      break;

    case SDSR_EQUAL_MAX:
    default:
      SD_CASE_ERR( "equal_cat",0,0,0 );
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the two system records are not the same with respect to
  ** the specified equality category - return FALSE.
  */
  return FALSE;

} /* sdsr_is_same */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_comp

DESCRIPTION
  Compare two system records according to a specified category.

DEPENDENCIES
  None.

RETURN VALUE
  Positive integer if the 1st system record is of higher priority than the
  2nd system record according to the specified comparison category.

  -1 if the 1st system record is of lower priority than the
  2nd system record according to the specified comparison category.

  -2 if either system record is NULL

  0 if 1st system record is of equal priority to the 2nd system record
  according to the specified comparison category.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  int                       sdsr_comp(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to a system record to be compared against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type         *sr2_ptr,
            /* Pointer to a system record to be compared against the system
            ** record that is being pointed to by sr1_ptr.
            */

        sdsr_comp_e_type          comp_cat
            /* Comparison category according to which system records should
            ** be compared.
            */
)
{
  sd_ss_mode_pref_e_type  mode_pref;
  sd_ss_band_pref_e_type  band_pref = SD_SS_BAND_PREF_NONE;

  int                     pos1;
  int                     pos2;

  sd_blksys_e_type        cell_blksys;
  sd_blksys_e_type        sr1_blksys;
  sd_blksys_e_type        sr2_blksys;

  boolean                 sr1_is_home_side;
  boolean                 sr2_is_home_side;

  sd_blksys_e_type        cdma_home_blksys;
  sd_blksys_e_type        amps_home_blksys;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sr1_ptr == NULL || sr2_ptr == NULL)
  {
    SD_MSG_ERROR(" sr_ptr null",0,0,0);
    return -2;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( comp_cat )
  {
    case SDSR_COMP_RXPWR:
    case SDSR_COMP_PILOT:

      /* Compare according to Rx-power/Pilot Strength measurements.
      */
      if( sr1_ptr->meas_val != sr2_ptr->meas_val )
      {
        return( sr1_ptr->meas_val - sr2_ptr->meas_val );
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_COMP_AMPS:
    case SDSR_COMP_CDMA:
    case SDSR_COMP_HDR:
    case SDSR_COMP_DIGITAL:
    case SDSR_COMP_GW:

      /* Compare according to specified mode.
      */
      switch (comp_cat)
      {
        case SDSR_COMP_AMPS:     mode_pref = SD_SS_MODE_PREF_AMPS;
          break;

        case SDSR_COMP_CDMA:     mode_pref = SD_SS_MODE_PREF_CDMA;
          break;

        case SDSR_COMP_HDR:      mode_pref = SD_SS_MODE_PREF_HDR;
          break;

        case SDSR_COMP_DIGITAL:  mode_pref = SD_SS_MODE_PREF_DIGITAL;
          break;

        default:                 mode_pref = SD_SS_MODE_PREF_GW;
          break;
      }


      if( sdss_sr_is_mode_pref(sr1_ptr, mode_pref) &&
          !sdss_sr_is_mode_pref(sr2_ptr, mode_pref) )
      {
        return 1;
      }

      if( !sdss_sr_is_mode_pref(sr1_ptr, mode_pref) &&
          sdss_sr_is_mode_pref(sr2_ptr, mode_pref) )
      {
        return -1;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_COMP_BC0:
    case SDSR_COMP_BC1:
    case SDSR_COMP_BC4:
    case SDSR_COMP_BC5:
    case SDSR_COMP_BC6:
    case SDSR_COMP_BC10:
    case SDSR_COMP_BC11:
    case SDSR_COMP_BC12:
    case SDSR_COMP_BC14:
    case SDSR_COMP_BC15:
    case SDSR_COMP_BC16:
    case SDSR_COMP_BC17:
    case SDSR_COMP_BC18:
    case SDSR_COMP_BC19:


      /* Compare according to specified band.
      */
      switch (comp_cat)
      {
        case SDSR_COMP_BC0:              band_pref = SD_SS_BAND_PREF_BC0;
          break;

        case SDSR_COMP_BC1:               band_pref = SD_SS_BAND_PREF_BC1;
          break;

        case SDSR_COMP_BC4:               band_pref = SD_SS_BAND_PREF_BC4;
          break;

        case SDSR_COMP_BC5:               band_pref = SD_SS_BAND_PREF_BC5;
          break;

        case SDSR_COMP_BC6:               band_pref = SD_SS_BAND_PREF_BC6;
          break;

        case SDSR_COMP_BC10:              band_pref = SD_SS_BAND_PREF_BC10;
          break;

        case SDSR_COMP_BC11:              band_pref = SD_SS_BAND_PREF_BC11;
          break;

        case SDSR_COMP_BC12:              band_pref = SD_SS_BAND_PREF_BC12;
          break;

        case SDSR_COMP_BC14:              band_pref = SD_SS_BAND_PREF_BC14;
          break;

        case SDSR_COMP_BC15:              band_pref = SD_SS_BAND_PREF_BC15;
          break;

        case SDSR_COMP_BC16:              band_pref = SD_SS_BAND_PREF_BC16;
          break;

        case SDSR_COMP_BC17:              band_pref = SD_SS_BAND_PREF_BC17;
          break;

        case SDSR_COMP_BC18:              band_pref = SD_SS_BAND_PREF_BC18;
          break;

        default:                          band_pref = SD_SS_BAND_PREF_BC19;
          break;
      }

      if( sr1_ptr->sys.band == band_pref &&
          sr2_ptr->sys.band != band_pref )
      {
        return 1;
      }

      if( sr1_ptr->sys.band != band_pref &&
          sr2_ptr->sys.band == band_pref )
      {
        return -1;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_COMP_ROAM:

      if( sdprl_roam_ind_map_custom_home_to_home(sr1_ptr->roam) < 
                    sdprl_roam_ind_map_custom_home_to_home(sr2_ptr->roam) )
      {
        return 1;
      }
      else if( sdprl_roam_ind_map_custom_home_to_home(sr2_ptr->roam) < 
                    sdprl_roam_ind_map_custom_home_to_home(sr1_ptr->roam) )
      {
        return -1;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_COMP_PRL_ACQ:

      pos1 = sdsr_list_find( SDSR_PRL_LST, sr1_ptr );
      pos2 = sdsr_list_find( SDSR_PRL_LST, sr2_ptr );

      if( pos1 == SDSR_POS_NULL ) {
        pos1 = 10000;
      }

      if( pos2 == SDSR_POS_NULL ) {
        pos2 = 10000;
      }

      if( pos1 < pos2 ) {
        return 1;
      }

      if( pos2 < pos1 ) {
        return -1;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_COMP_CELL_A:
    case SDSR_COMP_CELL_B:

      /* Compare according to specified cellular system.
      */
      if( comp_cat == SDSR_COMP_CELL_A )
      {
        cell_blksys = SD_BLKSYS_CELL_A;
      }
      else
      {
        cell_blksys = SD_BLKSYS_CELL_B;
      }

      sr1_blksys = sdprl_map_chan_to_blksys( sr1_ptr->sys.mode,
                 sdss_map_band_pref_to_band(sr1_ptr->sys.band),
                                             sr1_ptr->sys.chan );

      sr2_blksys = sdprl_map_chan_to_blksys( sr2_ptr->sys.mode,
                 sdss_map_band_pref_to_band(sr2_ptr->sys.band),
                                             sr2_ptr->sys.chan );

      if( sr1_blksys == cell_blksys &&
          sr2_blksys != cell_blksys )
      {
        return 1;
      }

      if( sr1_blksys != cell_blksys &&
          sr2_blksys == cell_blksys )
      {
        return -1;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_COMP_CELL_HOME:

      /* Compare according to home cellular system side.
      */

      /* Get the home cellular system for each mode.
      */
      cdma_home_blksys = sdprl_amps_bc0_sys_to_blksys(
                            sdprl_get_home_side(SD_NAM_MAX, SD_MODE_CDMA) );

      amps_home_blksys = sdprl_amps_bc0_sys_to_blksys(
                            sdprl_get_home_side(SD_NAM_MAX, SD_MODE_AMPS) );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Get the cellular system of each system record.
      */
      sr1_blksys = sdprl_map_chan_to_blksys( sr1_ptr->sys.mode,
                 sdss_map_band_pref_to_band(sr1_ptr->sys.band),
                                               sr1_ptr->sys.chan );

      sr2_blksys = sdprl_map_chan_to_blksys( sr2_ptr->sys.mode,
                 sdss_map_band_pref_to_band(sr2_ptr->sys.band),
                                               sr2_ptr->sys.chan );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Decide whether the 1st system record is home side based on the
      ** system's mode and home side for this mode.
      */
      if( ((sr1_ptr->sys.mode == SD_MODE_CDMA ||
            sr1_ptr->sys.mode == SD_MODE_HDR) &&
           sr1_blksys == cdma_home_blksys)
                        ||
          (sr1_ptr->sys.mode == SD_MODE_AMPS &&
           sr1_blksys == amps_home_blksys) )
      {
        sr1_is_home_side = TRUE;
      }
      else
      {
        sr1_is_home_side = FALSE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Decide whether the 2nd system record is home side based on the
      ** system's mode and home side for this mode.
      */
      if( ((sr2_ptr->sys.mode == SD_MODE_CDMA ||
            sr2_ptr->sys.mode == SD_MODE_HDR) &&
           sr2_blksys == cdma_home_blksys)
                        ||
          (sr2_ptr->sys.mode == SD_MODE_AMPS &&
           sr2_blksys == amps_home_blksys) )
      {
        sr2_is_home_side = TRUE;
      }
      else
      {
        sr2_is_home_side = FALSE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Compare the two system record according to whether they are of home
      ** cellular system side.
      */
      if( sr1_is_home_side &&
          !sr2_is_home_side )
      {
        return 1;
      }

      if( !sr1_is_home_side &&
          sr2_is_home_side )
      {
        return -1;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_COMP_MAX:
    default:
      SD_CASE_ERR( "comp_cat",0,0,0 );
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* No difference between both the systems records in the comparison
  ** category.
  */
  return 0;

} /* sdsr_comp */



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_comp2

DESCRIPTION
  Compare two system records according to a specified category.

DEPENDENCIES
  None.

RETURN VALUE
  Positive integer if the 1st system record is of higher priority than the
  2nd system record according to the specified comparison category.

  -1 if the 1st system record is of lower priority than the
  2nd system record according to the specified comparison category.

  If the system records are the same according to the specified comparison
  category, the system records are compared by their list positions.
  
  0 ONLY if 1st system record and the 2nd system record have the same list
  positions (which technically can happen only if both the pointers are
  pointing to the same system record).

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  int                       sdsr_comp2(

        const sdsr_s_type               *sr1_ptr,
            /* Pointer to a system record to be compared against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type               *sr2_ptr,
            /* Pointer to a system record to be compared against the system
            ** record that is being pointed to by sr1_ptr.
            */

        sdsr_comp_e_type          comp_cat
            /* Comparison category according to which system records should
            ** be compared.
            */
)
{
  int diff = sdsr_comp(sr1_ptr, sr2_ptr, comp_cat);

  if( diff == 0 )
  {
    /* If we got here, the system two records are indifferent according to
    ** the sorting criteria, so decide according to their list positions.
    */
    return( sr2_ptr->list_pos - sr1_ptr->list_pos );
  }

  return diff;
}



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================ SYSTEM RECORD TABLE ============================
=============================================================================
=============================================================================
===========================================================================*/


/* The system record table records relevant information that is known about
** all the systems that were tried acquisition at one time or another.
**
** During power-up, this system record table is populated with systems from
** the MRU table and the PRL's acquisition table. Then during operation the
** table is continuously being updated as SD gathers more information about
** systems.
**
** If during operation more unique systems that can be fit into the system
** record table are encountered, the following logic determines which system
** shall be retained in the table:
**
**   1. Systems from the PRL's acquisition tables shall never be deleted
**      from the table
**
**   2. Additional systems shall be retained on a most recently attempted
**      acquisition basis; that is, the system that was not attempted
**      acquisition for the longest time is deleted from the table first.
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for a system record table.
*/
typedef struct {

  /* Array for holding the system records.
  */
  sdsr_s_type     arr[ PRL_TBL_MAX ];

  /* Number of system records that are stored in the sr_arr.
  */
  int             num;

} sdsr_tbl_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* The system record table.
*/
static sdsr_tbl_s_type   sdsr_tbl;




/* <EJECT> */
/*===========================================================================
=============================================================================
============================== TABLE OPERATORS ==============================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdsr_tbl_size

DESCRIPTION
  Return the size of the system record table.

DEPENDENCIES
  None.

RETURN VALUE
  The size of the system record table.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  int                   sdsr_tbl_size( void )
{
  //SD_FUNC_START( "sdsr_tbl_init",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( ARR_SIZE(sdsr_tbl.arr) );

} /* sdsr_tbl_size */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_get_ptr

DESCRIPTION
  Return a pointer to the system record that is referenced by the sr_ref
  parameter.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Pointer to the requested system record, NULL if no such record exists.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdsr_s_type               *sdsr_tbl_get_ptr(

        sdsr_ref_type             sr_ref
            /* Reference to a system record for which to get a pointer.
            */
)
{
  SD_ASSERT( INRANGE(sr_ref, 0, ARR_SIZE(sdsr_tbl.arr)) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( sdsr_tbl.num > PRL_TBL_MAX)
  {
    return NULL;
  }   

  if( sr_ref < sdsr_tbl.num )
  {
    return &sdsr_tbl.arr[ sr_ref ];
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return NULL;

} /* sdsr_tbl_get_ptr */



#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW) \
    || ( defined(SD_DEBUG) && TG==T_PC &&!defined(_lint) )
/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_get

DESCRIPTION
  Get (copy) the information from the system record that is referenced by the
  sr_ref parameter into the system record buffer that is pointed to by the
  sr_ptr parameter.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  TRUE if sr_ref resolved to an existing system record. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdsr_tbl_get(

        sdsr_ref_type             sr_ref,
            /* Reference to a system record from which to get the
            ** information
            */

        sdsr_s_type               *sr_ptr
            /* Pointer to a buffer where to copy system record information.
            */
)
{
  sdsr_s_type   *tbl_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( INRANGE(sr_ref, 0, ARR_SIZE(sdsr_tbl.arr)) );
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tbl_ptr = sdsr_tbl_get_ptr( sr_ref );

  if( tbl_ptr != NULL )
  {
    *sr_ptr = *tbl_ptr;
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdsr_tbl_get */
#endif



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_print

DESCRIPTION
  Print the content of a system record table.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_tbl_print( void )
{
  byte           i;
  sdsr_s_type   *trav_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH( "System Record Table Print, count=%d", sdsr_tbl.num,0,0 );
  for( i=0; (trav_ptr=sdsr_tbl_get_ptr(i)) != NULL; i++ )
  {
    SD_MSG_HIGH( "Entry=%d",i,0,0 );
    sdsr_print( trav_ptr, FALSE );
  }

} /* dsdsr_tbl_print */




#if 0
///* <EJECT> */
///*===========================================================================
//
//FUNCTION sdsr_tbl_find
//
//DESCRIPTION
//  Given the input parameters, sys_mode, sys_band and channel, this function
//  find and return a reference to the system record that has the same
//  sys_mode, sys_band and channel.
//
//DEPENDENCIES
//  sdsr_tbl_init() must have already been called to initialize the system
//  record table.
//
//RETURN VALUE
//  Reference to the matching system record if such a system record is found.
//  SDSR_REF_NULL otherwise.
//
//SIDE EFFECTS
//  None.
//
//===========================================================================*/
//static  sdsr_ref_type             sdsr_tbl_find(
//
//        sd_mode_e_type            mode,
//            /* The system's mode.
//            */
//
//        sd_band_e_type            band,
//            /* The system's band.
//            */
//
//        word                      chan
//            /* The system's channel/AMPS system.
//            */
//)
//{
//  SD_FUNC_NOT_IMP( sdsr_tbl_find );
//
//} /* sdsr_tbl_find */
#endif




/* <EJECT> */
/*===========================================================================
=============================================================================
============================= TABLE MANIPULATORS ============================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_init

DESCRIPTION
  Initializes the system record table.

  NOTE! this function must be called before any other sdsr_tbl_xxx is called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                  sdsr_tbl_init( void )
{
  SD_FUNC_START( "sdsr_tbl_init",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_tbl.num = 0;

}




#if 0
///* <EJECT> */
///*===========================================================================
//
//FUNCTION sdsr_tbl_put
//
//DESCRIPTION
//  Update the information of the system record that is referenced by the
//  sr_ref parameter per the information of the system record that is pointed
//  to by the sr_ptr parameter.
//
//DEPENDENCIES
//  sdsr_tbl_init() must have already been called to initialize the system
//  record table.
//
//RETURN VALUE
//  None.
//  //TRUE if sr_ref resolved to an existing system record. FALSE otherwise.
//
//SIDE EFFECTS
//  None.
//
//===========================================================================*/
//static  void                      sdsr_tbl_put(
//
//        sdsr_ref_type             sr_ref,
//            /* Reference to a system record to be updated.
//            */
//
//        sdsr_s_type               *sr_ptr
//            /* Pointer to a system record buffer containing the updated
//            ** information.
//            */
//)
//{
//  sdsr_s_type   *tbl_ptr;
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  SD_ASSERT( INRANGE(sr_ref, 0, ARR_SIZE(sdsr_tbl.arr)-1) );
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  tbl_ptr = sdsr_tbl_get_ptr( sr_ref );
//
//  if( tbl_ptr != NULL )
//  {
//    *tbl_ptr = *sr_ptr;
//  }
//  else
//  {
//    SD_ERR_FATAL( "",0,0,0 );
//  }
//
//} /* sdsr_tbl_put */
#endif




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store

DESCRIPTION
  Store the system record that is pointed to by the sr_ptr parameter in the
  system record table.

  If a matching table entry is already found in the system record table,
  return a reference to this system record entry.

  If a matching table entry is not found and the system record table is not
  full to capacity, a new entry is added to the end of the system record
  table.

  If a matching table entry is not found and the system record table is full
  to capacity, the new system record replaces an existing table entry as per
  the following criteria:

    1. Systems from the PRL's acquisition tables are never deleted from the
       system record table.

    2. Additional systems retained on a most recently used/attempted
       acquisition basis; that is, the system that was least recently
       used/attempted acquisition is deleted from the table first.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the newly added system record.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdsr_ref_type             sdsr_tbl_store(

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record buffer containing the system to be
            ** added to the system record table.
            */
)
{
  sdsr_ref_type   i;
  sdsr_s_type     *tbl_ptr;
  sdsr_s_type     *lru_ptr;
  sdsr_ref_type   lru_idx;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If a matching table entry is already found in the system record table,
  ** update this entry as per the information of the system record that is
  ** pointed to by the sr_ptr parameter.
  */
  for( i=0; (tbl_ptr=sdsr_tbl_get_ptr(i)) != NULL; i++ )
  {
    if( sdsr_is_same_sys(tbl_ptr, sr_ptr) )
    {
      //SD_MSG_LOW( "Matching entry=%d found in sr_tbl", i, 0, 0 );
      break;
    }
  }

  if( tbl_ptr != NULL )
  {
    return i;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, a matching table entry is not found. If the system
  ** record table is not full to capacity, add a new entry to the end of the
  ** system record table.
  */
  if( sdsr_tbl.num < (int) ARR_SIZE(sdsr_tbl.arr) )
  {
    SD_MSG_LOW( "Adding new entry=%d to sr_tbl", sdsr_tbl.num, 0, 0 );

    tbl_ptr = &sdsr_tbl.arr[ sdsr_tbl.num ];
    sdsr_init( tbl_ptr, &sr_ptr->sys );


    sdsr_tbl.num++;

    /* Insert the new entry at the end of system records that represent the
    ** entire system record table.
    */
    sdsr_list_insert_ref( SDSR_TBL_LST,
                          SDSR_POS_NULL,
          (sdsr_ref_type) (sdsr_tbl.num - 1),
                          TRUE );

    return (sdsr_ref_type) (sdsr_tbl.num - 1);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, a matching table entry is not found and the system
  ** record table is full to capacity. In such a case the new system record
  ** replaces an existing table entry as per the following criteria:
  **
  **  1. Systems from the PRL's acquisition tables are never deleted from the
  **     system record table.
  **
  **  2. Additional systems retained on a most recently used/attempted
  **     acquisition basis; that is, the system that was least recently
  **     used/attempted acquisition is deleted from the table first.
  */
  lru_ptr = sdsr_tbl_get_ptr( 0 );
  lru_idx = 0;

  if(lru_ptr == NULL) 
  {
    SD_MSG_ERROR(" lru_ptr NULL ",0,0,0);
    return lru_idx;
  }
  for( i=1; (tbl_ptr=sdsr_tbl_get_ptr(i)) != NULL; i++ )
  {
    if( sdsr_list_find(SDSR_PRL_LST, tbl_ptr) == SDSR_POS_NULL &&
        tbl_ptr->event_time < lru_ptr->event_time )
    {
      lru_ptr = tbl_ptr;
      lru_idx = i;
    }
  }

  SD_ERR( "Overwriting entry=%d of sr_tbl", lru_idx, 0, 0 );

  sdsr_init( lru_ptr, &sr_ptr->sys );


  return lru_idx;

} /* sdsr_tbl_store */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store2

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  If a matching table entry is already found in the system record table,
  return a reference to this system record entry.

  If a matching table entry is not found and the system record table is not
  full to capacity, a new entry is added to the end of the system record
  table.

  If a matching table entry is not found and the system record table is full
  to capacity, the new system record replaces an existing table entry as per
  the following criteria:

    1. Systems from the PRL's acquisition tables are never deleted from the
       system record table.

    2. Additional systems retained on a most recently used/attempted
       acquisition basis; that is, the system that was least recently
       used/attempted acquisition is deleted from the table first.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the newly added system record.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdsr_ref_type             sdsr_tbl_store2(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* The system's band.
            */

        sd_chan_type              chan
            /* The system's channel.
            */
)
{
  sdsr_s_type sr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = band_pref;
  sr.sys.chan = chan;

  return sdsr_tbl_store( &sr );

} /* sdsr_tbl_store2 */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store3

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's SID and NID per the input SID and NID.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdsr_ref_type             sdsr_tbl_store3(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        word                      sid,
            /* The system's SID.
            */

        word                      nid
            /* The system's NID.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = sdss_map_band_to_band_pref(band);
  sr.sys.chan = chan;

  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID/NID of the stored system per the input SID and NID.
  */
  sr_ptr        = sdsr_tbl_get_ptr(sr_ref);
  if(sr_ptr == NULL)
  {
    SD_MSG_HIGH("sr_ptr NULL",0,0,0);
    return sr_ref;
  }

  sr_ptr->sid.is95.sid  = sid;
  sr_ptr->sid.is95.nid  = nid;

  sr_ptr->sid.is95.true_sid = sid;
  sr_ptr->sid.is95.true_nid = nid;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store3 */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store4

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's expected SID and NID per the input expected SID
  and NID.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdsr_ref_type             sdsr_tbl_store4(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        word                      sid_expected,
            /* The system's expected SID.
            */

        word                      nid_expected,
            /* The system's expected NID.
            */

        boolean                   is_delete_tmsi
            /* Indicate if MS is required to delete the assigned TMSI.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = sdss_map_band_to_band_pref(band);
  sr.sys.chan = chan;

  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID/NID of the stored system per the input SID and NID.
  */
  sr_ptr        = sdsr_tbl_get_ptr(sr_ref);
  if (sr_ptr == NULL )
  {
    SD_MSG_ERROR(" sr_ptr NULL" ,0,0,0);
    return sr_ref;
  }
  sr_ptr->sid_expected.prm.is95.sid   = sid_expected;
  sr_ptr->sid_expected.prm.is95.nid   = nid_expected;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update delete tmsi bit mask.
  */
  SDSR_STATUS_MASK_SET(sr_ptr->status_mask, SDSR_STATUS_MASK_DEL_TMSI, is_delete_tmsi);


  return sr_ref;

} /* sdsr_tbl_store4 */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store5

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's SID, NID, PRL-designation and roaming indication
  fields per the corresponding input parameters.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdsr_ref_type             sdsr_tbl_store5(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        word                      sid,
            /* The system's SID.
            */

        word                      nid,
            /* The system's NID.
            */

        byte                      prl,
            /* The system's PRL designation (as per sdss_prl_e_type).
            */

        sys_roam_status_e_type    roam,
            /* The system's roaming indication (as per sd_roam_e_type).
            */

        sd_sr_prl_assn_e_type     prl_assn
            /* The system's PRL association.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl, SDSS_PRL_MAX );
  SD_ASSERT( roam < SYS_ROAM_STATUS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl_assn, SDSR_PRL_ASSN_ALL+1 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = sdss_map_band_to_band_pref(band);
  sr.sys.chan = chan;

  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID, NID, PRL-designation and roaming indication fields of
  ** the stored system per the corresponding input parameters.
  */
  sr_ptr        = sdsr_tbl_get_ptr(sr_ref);
  if( sr_ptr == NULL )
  {
    SD_MSG_HIGH("sr_ptr NULL",0,0,0);
    return sr_ref;
  }
  sr_ptr->sid.is95.sid  = sid;
  sr_ptr->sid.is95.nid  = nid;

  sr_ptr->sid.is95.true_sid = sid;
  sr_ptr->sid.is95.true_nid = nid;

  sr_ptr->prl           = prl;
  sr_ptr->roam          = roam;
  sr_ptr->prl_assn      = prl_assn;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store5 */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store5_is856

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's IS-856 SID, PRL-designation and roaming
  indication fields per the corresponding input parameters.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdsr_ref_type             sdsr_tbl_store5_is856(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        const byte                sid_ptr[16],
            /* Pointer to array of 16 byte Sector ID.
            */

        byte                      prl,
            /* The system's PRL designation (as per sdss_prl_e_type).
            */

        sys_roam_status_e_type    roam,
            /* The system's roaming indication (as per sd_roam_e_type).
            */

        sd_sr_prl_assn_e_type     prl_assn
            /* The system's PRL association.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl, SDSS_PRL_MAX );
  SD_ASSERT( roam < SYS_ROAM_STATUS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl_assn, SDSR_PRL_ASSN_MAX );
  SD_ASSERT( sid_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = sdss_map_band_to_band_pref(band);
  sr.sys.chan = chan;

  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID, PRL-designation and roaming indication fields of the
  ** stored system per the corresponding input parameters.
  */
  sr_ptr        = sdsr_tbl_get_ptr(sr_ref);
  if( sr_ptr == NULL )
  {
    SD_MSG_LOW("sr_ptr NULL",0,0,0);
    return sr_ref;    
  }

  memcpy( sr_ptr->sid.is856, sid_ptr, sizeof(sr_ptr->sid.is856) );
  sr_ptr->prl           = prl;
  sr_ptr->roam          = roam;
  sr_ptr->prl_assn      = prl_assn;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store5_is856 */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store6

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's SID, NID, expected-SID, expected-NID,
  PRL-designation and roaming indication fields per the corresponding input
  parameters.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdsr_ref_type             sdsr_tbl_store6(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        word                      sid,
            /* The system's SID.
            */

        word                      nid,
            /* The system's NID.
            */

        word                      sid_expected,
            /* The system's expected SID.
            */

        word                      nid_expected,
            /* The system's expected NID.
            */

        word                      mcc,
            /* The system's MCC.
            */
        byte                      imsi_11_12,
            /* The system's IMISI_11_12.
            */

        byte                      prl,
            /* The system's PRL designation (as per sdss_prl_e_type).
            */

        sys_roam_status_e_type    roam,
            /* The system's roaming indication (as per sd_roam_e_type).
            */

        sd_sr_prl_assn_e_type     prl_assn
            /* The system's PRL association.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl, SDSS_PRL_MAX );
  SD_ASSERT( roam < SYS_ROAM_STATUS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl_assn, SDSR_PRL_ASSN_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = sdss_map_band_to_band_pref(band);
  sr.sys.chan = chan;

  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID, NID, expected-SID, expected-NID, PRL-designation and
  ** roaming indication fields of the stored system per the corresponding
  ** input parameters.
  */
  sr_ptr        = sdsr_tbl_get_ptr(sr_ref);
  if( sr_ptr == NULL )
  {
    SD_MSG_LOW("sr_ptr NULL",0,0,0);
    return sr_ref;
  }

  sr_ptr->sid.is95.sid  = sid;
  sr_ptr->sid.is95.nid  = nid;

  sr_ptr->sid.is95.true_sid     = sid;
  sr_ptr->sid.is95.true_nid     = nid;

  sr_ptr->sid.is95.mcc          = mcc;
  sr_ptr->sid.is95.imsi_11_12   = imsi_11_12;

  sr_ptr->sid_expected.prm.is95.sid  = sid_expected;
  sr_ptr->sid_expected.prm.is95.nid  = nid_expected;

  sr_ptr->prl           = prl;
  sr_ptr->roam          = roam;
  sr_ptr->prl_assn      = prl_assn;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store6 */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store7

DESCRIPTION
  Store the system that is specified by the input mode, band, chan and sid
  in the system record table.

  Update the stored record's sys_index (i.e., the system table index
  referencing this system) per the input system table index.

  Update the stored record's with expected plmn information.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdsr_ref_type             sdsr_tbl_store7(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        const sd_plmn_s_type      *sd_plmn_ptr,
             /* pointer to plmn data structure
             */

        word                      sys_idx
            /* The system table index referencing this system.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = band_pref;
  sr.sys.chan = chan;

  if( sd_plmn_ptr != NULL ) {
  /* Used for searching a matched GSM/WCDMA record.
  */
    sr.sid_expected.prm.plmn = *sd_plmn_ptr;
  }

  /* Store system field only. sid_expected field does not stored at this time.
  */
  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );
  sr_ptr = sdsr_tbl_get_ptr(sr_ref);
  if(sr_ptr == NULL)
  {
    SD_MSG_ERROR(" sr_ptr NULL",0,0,0);
    return sr_ref;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the input system table index != SD_NO_CHANGE_SYS_IDX, update the
  ** expected system table index field of the stored system per the input
  ** system table index.
  */
  if( sys_idx != SD_NO_CHANGE_SYS_IDX )
  {
    sr_ptr->idx_expected  = sys_idx;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
  /* If the system mode is GSM/WCDMA mode,
  ** update expected system field with plmn information.
  */
  if( mode == SD_MODE_GSM   ||
      mode == SD_MODE_WCDMA ||
      mode == SD_MODE_GW )
  {
    if( sd_plmn_ptr != NULL )
    {
     sr_ptr->sid_expected.prm.plmn = *sd_plmn_ptr;
    }
  }
  #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store7 */



#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)
/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store8

DESCRIPTION
  Store the system record that is pointed to by the sr_ptr parameter in the
  system record table.

  If a matching table entry is already found in the system record table,
  return a reference to this system record entry.

  If a matching table entry is not found and the system record table is not
  full to capacity, a new entry is added to the end of the system record
  table.

  If a matching table entry is not found and the system record table is full
  to capacity, the new system record replaces an existing table entry as per
  the following criteria:

    1. Systems from the PRL's acquisition tables are never deleted from the
       system record table.

    2. Additional systems retained on a most recently used/attempted
       acquisition basis; that is, the system that was least recently
       used/attempted acquisition is deleted from the table first.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the newly added system record.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdsr_ref_type             sdsr_tbl_store8(

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record buffer containing the system to be
            ** added to the system record table.
            */
)
{
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *tbl_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ref = sdsr_tbl_store( sr_ptr );


  /* Update network selection mode.
  */
  tbl_ptr = sdsr_tbl_get_ptr( sr_ref );
  if(tbl_ptr == NULL)
  {
    SD_MSG_ERROR(" tbl_ptr NULL" ,0,0,0);
    return sr_ref;
  }

  tbl_ptr->sid_expected.net_select_mode = sr_ptr->sid_expected.net_select_mode;

  return sr_ref;

} /* sdsr_tbl_store8 */

#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_GW) */

/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store9

DESCRIPTION
  Store the system that is specified by the input mode, band, chan and
  expected sid in the system record table.

  Update the stored record's sys_index (i.e., the system table index
  referencing this system) per the input system table index.

  Update the stored record's with expected plmn information.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdsr_ref_type                   sdsr_tbl_store9(

        sd_mode_e_type                  mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type          band_pref,
            /* The system's band.
            */

        sd_chan_type                    chan,
            /* The system's channel.
            */

        const sd_sid_plmn_wlan_s_type   *sid_exp_ptr,
             /* pointer to Expected SID.
             */

        word                            sys_idx,
            /* The system table index referencing this system.
            */

        sys_profile_id_type             profile_id,
            /* profile id to be used.
            */

        sys_wlan_bss_e_type             wlan_bss_type,
            /* Applicable only for WLAN.
            */

        sys_tech_mask_type              wlan_tech_mask
            /* WLAN tech mask
            */

)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = band_pref;
  sr.sys.chan = chan;

  if( sid_exp_ptr != NULL )
  {
    sr.sid_expected = *sid_exp_ptr;
  }
  sr.nw_profile_id = profile_id;
  sr.wlan_bss_type = wlan_bss_type;
  sr.wlan_tech_mask = wlan_tech_mask;

  /* Store system field only. sid_expected field does not stored at this time.
  */
  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ptr = sdsr_tbl_get_ptr( sr_ref );
  if( sr_ptr == NULL )
  {
    SD_MSG_ERROR(" sr_ptr NULL",0,0,0);
    return sr_ref;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the input system table index != SD_NO_CHANGE_SYS_IDX, update the
  ** expected system table index field of the stored system per the input
  ** system table index.
  */
  if( sys_idx != SD_NO_CHANGE_SYS_IDX )
  {
    sr_ptr->idx_expected  = sys_idx;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update expected system field, profile_id/ wlan_bss_type.
  */
  if( sid_exp_ptr != NULL )
  {
    sr_ptr->sid_expected = *sid_exp_ptr;
  }
  sr_ptr->nw_profile_id = profile_id;
  sr_ptr->wlan_bss_type = wlan_bss_type;
  sr_ptr->wlan_tech_mask = wlan_tech_mask;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store9 */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store10

DESCRIPTION
  Store the system that is specified by the input mode, band, chan and
  expected sid in the system record table.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdsr_ref_type             sdsr_tbl_store10(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        const sd_sid2_u_type      *sid_ptr
            /* System sid
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = sdss_map_band_to_band_pref( band );
  sr.sys.chan = chan;

  /* Store system field only. sid_expected field does not stored at this time.
  */
  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update system sid field.
  */
  sr_ptr = sdsr_tbl_get_ptr( sr_ref );
  SD_ASSERT( sr_ptr != NULL );

  if( sid_ptr != NULL )
  {
    sr_ptr->sid = *sid_ptr;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store9 */





/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================ SYSTEM RECORD LIST =============================
=============================================================================
=============================================================================
===========================================================================*/


/* System record lists are ordered subsets of the system record table. In
** other words, each such list is a list of references to system record table
** entries. A system record item is a system record list of length one.
**
** Note that a system record list shall not contain duplicate elements; that
** is, no two elements on the same list shall reference the same system
** record table entry.
**
** In addition each system record list is of the same length as the system
** record table; or put differently, each system record list is capable of
** referencing all the systems that are stored in the system record table.
*/


/* Type for a system record list.
*/
typedef struct {

  /* Array for holding the system record references.
  */
  sdsr_ref_type     arr[ PRL_TBL_MAX ];

  /* Number of system record references that are stored in list.
  */
  int               num;

} sdsr_list_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Array of all the system record lists.
**
** In this array the sdsr_lists_arr[ SDSR_XXX_LST ] entry represents the
** SDSR_XXX_LST system record list.
*/
static sdsr_list_s_type      sdsr_lists_arr[ SDSR_MAX_LST ];


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Array of all the system record items.
**
** In this array the sdsr_items_arr[ SDSR_XXX_SYS ] entry represents the
** SDSR_XXX_SYS system record item.
*/ /*lint -e656 */ /* Arithmetic operation uses enum */
static sdsr_ref_type         sdsr_items_arr[ SDSR_MAX - SDSR_1ST_SYS ];
   /*lint +e656 */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */


#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================== LIST OPERATORS ==============================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdsr_list_ptr

DESCRIPTION
  Return a pointer to a specified system record list. If no such list exists,
  return NULL.

DEPENDENCIES
  None.

RETURN VALUE
  A pointer to a specified system record list is such a list exists. NULL
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdsr_list_s_type          *sdsr_list_ptr(

        sdsr_e_type               list
            /* List for which to return a pointer.
            */
)
{

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If list is resolved to a system record list, return a pointer to this
  ** list. Else, return NULL.
  */
  if( INRANGE(list, 0, ( SDSR_MAX_LST - 1)) )
  {
    SD_ASSERT( INRANGE(list, 0, (int) (ARR_SIZE(sdsr_lists_arr)-1)) );
    return &sdsr_lists_arr[ list ];
  }

  return NULL;

} /* sdsr_list_ptr */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_item_ptr

DESCRIPTION
  Return a pointer to a specified system record item. If no such item exists,
  return NULL.

DEPENDENCIES
  None.

RETURN VALUE
  A pointer to a specified system record list is such a list exists. NULL
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdsr_ref_type             *sdsr_item_ptr(

        sdsr_e_type               list
            /* Item for which to return a pointer.
            */
)
{
  int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If list is resolved to a system record item, return a pointer to this
  ** item. Else, return NULL.
  */
  if( INRANGE(list, SDSR_1ST_SYS, (SDSR_MAX-1)) )
  { /*lint -e656 */
    i = (int)list - (int)SDSR_1ST_SYS; /*lint +e656 */
    SD_ASSERT( INRANGE(i, 0, (int) (ARR_SIZE(sdsr_items_arr)-1)) );
    return &sdsr_items_arr[i];
  }

  return NULL;

} /* sdsr_item_ptr */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_cnt

DESCRIPTION
  Return the count of a system record list (i.e., the number of items that
  are currently on the list).

DEPENDENCIES
  None.

RETURN VALUE
  The number of items that are on the specified system record list.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  int                       sdsr_list_cnt(

        sdsr_e_type               list
            /* List for which to return the count.
            */
)
{
  sdsr_list_s_type  *list_ptr;
  sdsr_ref_type     *item_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return 0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Based on whether list resolved to a system record list or item, return
  ** the count.
  */
  if( (list_ptr=sdsr_list_ptr(list)) != NULL )
  {
    return list_ptr->num;
  }
  else if( (item_ptr=sdsr_item_ptr(list)) != NULL )
  {
    if( *item_ptr != SDSR_REF_NULL )
    {
      return 1;
    }
  }

  return 0;

} /* sdsr_list_cnt */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_cpy

DESCRIPTION
  Copy the elements from one system record list to another.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_cpy(

        sdsr_e_type               list_to,
            /* List to which to copy the elements.
            */

        sdsr_e_type               list_from
            /* List from which to copy the elements.
            */
)
{
  sdsr_list_s_type  *list_to_ptr    = sdsr_list_ptr( list_to );
  sdsr_list_s_type  *list_from_ptr  = sdsr_list_ptr( list_from );
  sdsr_ref_type     *item_to_ptr    = sdsr_item_ptr( list_to );
  sdsr_ref_type     *item_from_ptr  = sdsr_item_ptr( list_from );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list_to, SDSR_MAX, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list_from, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list_to == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Based on whether list_xx resolved to a system record list or item,
  ** perform the copy operation.
  */

  /* List to list copy.
  */
  if( list_to_ptr    != NULL &&
      list_from_ptr  != NULL )
  {
    *list_to_ptr = *list_from_ptr;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Item to item copy.
  */
  else if( item_to_ptr   != NULL &&
           item_from_ptr != NULL )
  {
    *item_to_ptr = *item_from_ptr;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Item to list copy.
  */
  else if( list_to_ptr   != NULL &&
           item_from_ptr != NULL )
  {
    sdsr_list_clr( list_to );
    sdsr_list_app( list_to, list_from );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* List to item copy.
  */
  else if( item_to_ptr   != NULL &&
           list_from_ptr != NULL )
  {
    sdsr_ref_type  sr_ref = sdsr_list_get_ref( list_from, 0 );

    sdsr_list_clr( list_to );

    if( sr_ref != SDSR_REF_NULL )
    {
      sdsr_list_put_ref( list_to, sr_ref );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else
  {
    SD_ELSE_ERR( "",0,0,0 );
  }

} /* sdsr_list_cpy */




#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_get_ref

DESCRIPTION
  Given a list and a list position return the system record reference of
  the system record that is associated with this list and list position.

DEPENDENCIES
  None.

RETURN VALUE
  System record reference of the system record that is associated with the
  input list and list position if list and pos resolved to an existing system
  record. SDSR_REF_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdsr_ref_type             sdsr_list_get_ref(

        sdsr_e_type               list,
            /* List from which to get the system record reference.
            */

        int                       pos
            /* List position from which to get the system record reference.
            */

)
{
  sdsr_list_s_type  *list_ptr;
  sdsr_ref_type     *item_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT( pos >= 0 );
  //SD_ASSERT( INRANGE(pos, 0, (int) ARR_SIZE(list_ptr->arr)) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return SDSR_REF_NULL;
  }

  if( !INRANGE(pos,0,PRL_TBL_MAX)  )
  {
    SD_MSG_ERROR(" POS OUT-OF-BOUND",0,0,0);
    return SDSR_REF_NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Based on whether list resolved to a system record list or item, get the
  ** system record reference.
  */
  if( (list_ptr=sdsr_list_ptr(list)) != NULL )
  {
    if( !INRANGE(list_ptr->num, 0, (int) ARR_SIZE(list_ptr->arr)) )
    {
      SD_MSG_ERROR( "invalid list_ptr size; %d",list_ptr->num,0,0);
      return SDSR_REF_NULL;
    }
    if( pos < list_ptr->num )
    {
      return list_ptr->arr[ pos ];
    }
  }
  else if( (item_ptr=sdsr_item_ptr(list)) != NULL )
  {
    if( pos == 0 )
    {
      return *item_ptr;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here list and/or pos do not resolved to an existing system
  ** record, return SDSR_REF_NULL.
  */
  return SDSR_REF_NULL;

} /* sdsr_list_get_ref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_get_ptr

DESCRIPTION
  Given a list and a list position return a pointer to system record that is
  referenced by this list and list position.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the system record that is referenced by the input list and list
  position if list and pos resolved to an existing system record. NULL
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  sdsr_s_type               *sdsr_list_get_ptr(

        sdsr_e_type               list,
            /* List from which to get the system record pointer.
            */

        int                       pos
            /* List position from which to get the system record pointer.
            */

)
{
  sdsr_ref_type   sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If list and pos resolved to an existing system record reference,
  ** return a pointer to the corresponding system record. Else, return NULL.
  */
  sr_ref = sdsr_list_get_ref( list, pos );

  if( sr_ref != SDSR_REF_NULL )
  {
    return sdsr_tbl_get_ptr( sr_ref );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return NULL;

} /* sdsr_list_get_ptr */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_get_cpy

DESCRIPTION
  Get a copy of the system record list that is referenced by the list and pos
  parameters into the system record buffer that is pointed to by the sr_ptr
  parameter.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if list and pos resolved to an existing system record. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdsr_list_get_cpy(

        sdsr_e_type               list,
            /* List from which to get the system record information.
            */

        int                       pos,
            /* List position from which to get the system record information.
            */

        sdsr_s_type               *sr_ptr
            /* Pointer to a buffer where to copy the system record
            ** information.
            */
)
{
  sdsr_s_type   *rec_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rec_ptr = sdsr_list_get_ptr( list, pos );

  if( rec_ptr != NULL )
  {
    *sr_ptr = *rec_ptr;
    return TRUE;
  }

  return FALSE;

} /* sdsr_list_get_cpy */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_find

DESCRIPTION
  Given a list and a system record, this function find and return the list
  position of that system record.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  Position of the matching system record if such a system record is found.
  SDSR_POS_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  int                       sdsr_list_find(

        sdsr_e_type               list,
            /* List over which to find the system record reference.
            */

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record for which to search.
            */
)
{
  int           i;
  sdsr_s_type   *list_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return SDSR_POS_NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i=0; (list_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( list_ptr == sr_ptr )
    {
      return i;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDSR_POS_NULL;

} /* sdsr_list_find */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_find_ref

DESCRIPTION
  Given a list and a system record reference, this function find and return
  the list position of that system record reference.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  Position of the matching system record reference if such a system record is
  found. SDSR_POS_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  int                       sdsr_list_find_ref(

        sdsr_e_type               list,
            /* List over which to find the system record reference.
            */

        sdsr_ref_type             sr_ref
            /* System record reference to for which to search.
            */
)
{
  int             i;
  sdsr_ref_type   list_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return SDSR_POS_NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i=0; (list_ref=sdsr_list_get_ref(list, i))!=SDSR_REF_NULL; i++ )
  {
    if( list_ref == sr_ref )
    {
      return i;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDSR_POS_NULL;

} /* sdsr_list_find_ref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_find_sys

DESCRIPTION
  Find a system that matches specific mode band and channel on a system
  record list.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  Position of the 1st matching system record if such a system record is
  found. SDSR_POS_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  int                       sdsr_list_find_sys(

        sdsr_e_type               list,
            /* List over which to find the matching system.
            */

        sd_mode_e_type            mode,
            /* Mode to match.
            */

        sd_band_e_type            band,
            /* Band to match.
            */

        sd_chan_type              chan
            /* Channel to match.
            */
)
{
  int                            i;
  sdsr_s_type                    *list_ptr;
  sd_ss_band_pref_e_type         band_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return SDSR_POS_NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  band_pref = sdss_map_band_to_band_pref(band);

  for( i=0; (list_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( list_ptr->sys.mode == mode &&
        list_ptr->sys.band == band_pref &&
        list_ptr->sys.chan == chan )
    {
      return i;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDSR_POS_NULL;

} /* sdsr_list_find_sys */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_print

DESCRIPTION
  Print the content of a system record list.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_print(

        sdsr_e_type               list,
            /* List to be printed.
            */

        boolean                   is_print_full
            /* Indicate whether the complete system record information
            ** should be printed.
            */
)
{
  byte           i;
  sdsr_s_type   *trav_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_MED( "Print list=%d, count=%d", list, sdsr_list_cnt(list),0 );

  for( i=0; (trav_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    //SD_MSG_HIGH( "Entry=%d",i,0,0 );
    sdsr_print( trav_ptr, is_print_full );
  }

} /* sdsr_list_print */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_is_same

DESCRIPTION
  Compare two system record lists with respect to a specified equality
  category.

  Specifically check whether list1[i] is equal to list2[i] with respect to a
  specified equality category for all i in list1 and list2.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize list1 and
  list2.

RETURN VALUE
  TRUE if the two system record lists are the same with respect to the
  specified equality category. FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdsr_list_is_same(

        sdsr_e_type               list1,
            /* 1st list to be compared.
            */

        sdsr_e_type               list2,
            /* 2nd list to be compared.
            */

        sdsr_equal_e_type         equal_cat
            /* Category according to which to check the lists for equality.
            */
)
{
  int           i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list1, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( list2, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( equal_cat, SDSR_EQUAL_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list1 == SDSR_MAX ||
      list2 == SDSR_MAX )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the two lists do not contain the same number of elements, return
  ** FALSE.
  */
  if( sdsr_list_cnt(list1) != sdsr_list_cnt(list2) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the two lists contain the same number of elements, so
  ** compare the lists' elements one by one with respect to the specified
  ** equality category. Return FALSE if they are not all the same.
  */
  for( i=0; i < sdsr_list_cnt(list1); i++ )
  {
    if( ! sdsr_is_same(sdsr_list_get_ptr(list1, i),
                       sdsr_list_get_ptr(list2, i),
                       equal_cat) )
    {
      return FALSE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the two list are the same with respect to the specified
  ** equality category - return TRUE.
  */
  return TRUE;

} /* sdsr_list_is_same */


/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_sorted_is_same

DESCRIPTION
  Compare two system record lists with respect to a specified equality
  category.

  ** The lists are sorted before comparison. **

  Specifically check whether list1[i] is equal to list2[i] with respect to a
  specified equality category for all i in list1 and list2.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize list1 and
  list2.

RETURN VALUE
  TRUE if the two system record lists are the same with respect to the
  specified equality category. FALSE, otherwise.

  ** The input lists are sorted. **

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdsr_list_sorted_is_same(

        sdsr_e_type               list1,
            /* 1st list to be compared.
            */

        sdsr_e_type               list2,
            /* 2nd list to be compared.
            */

        sdsr_comp_e_type          comp_cat,
            /* Comparison category according to which system records should
            ** be compared.
            */

        sdsr_equal_e_type         equal_cat
            /* Category according to which to check the lists for equality.
            */
)
{

  SD_ASSERT_ENUM_IS_INRANGE_INC( list1, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( list2, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( equal_cat, SDSR_EQUAL_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( comp_cat, SDSR_COMP_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_list_sort(list1, comp_cat);
  sdsr_list_sort(list2, comp_cat);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sdsr_list_is_same(list1, list2, equal_cat);

} /* sdsr_list_sorted_is_same */



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_is_contain

DESCRIPTION
  Check whether list1 contains contain_cond elements of list2,
  where contain_cond can take on one of the following values:

  SDSR_CONTAIN_NONE   - list1 contains no elements of list2
  SDSR_CONTAIN_SOME   - list1 contains some elements of list2
  SDSR_CONTAIN_ALL    - list1 contains all elements of list2
  SDSR_CONTAIN_SAME   - list1 contains the same elements as list2

  Note! If list2 is empty and contain_cond is other than SDSR_CONTAIN_SAME,
  the return value from this function is always TRUE.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize list1 and
  list2.

  List2 must not be empty.

RETURN VALUE
  TRUE if list1 contain contain_cond elements of list2, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdsr_list_is_contain(

        sdsr_e_type               list1,
            /* 1st list to be checked.
            */

        sdsr_contain_e_type       contain_cond,
            /* Contain condition.
            */


        sdsr_e_type               list2
            /* 2nd list to be checked.
            */
)
{
  byte            i;
  sdsr_ref_type   sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list1, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( contain_cond, SDSR_CONTAIN_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( list2, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list1 == SDSR_MAX ||
      list2 == SDSR_MAX )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If contain_cond is other than SDSR_CONTAIN_SAME and list2 is empty,
  ** return TRUE.
  */
  if( contain_cond != SDSR_CONTAIN_SAME &&
      sdsr_list_cnt(list2) == 0 )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( contain_cond )
  {
    case SDSR_CONTAIN_NONE:

      /* If any of the elements of list2 is found on list1, return FALSE.
      ** Else, return TRUE.
      */
      for( i=0; (sr_ref=sdsr_list_get_ref(list2, i)) != SDSR_REF_NULL; i++ )
      {
        if( sdsr_list_find_ref(list1, sr_ref) != SDSR_POS_NULL )
        {
          return FALSE;
        }
      }
      return TRUE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_CONTAIN_SOME:

      /* If any of the elements of list2 is found on list1, return TRUE.
      ** Else, return FALSE.
      */
      for( i=0; (sr_ref=sdsr_list_get_ref(list2, i)) != SDSR_REF_NULL; i++ )
      {
        if( sdsr_list_find_ref(list1, sr_ref) != SDSR_POS_NULL )
        {
          return TRUE;
        }
      }
      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_CONTAIN_ALL:

      /* If any of the elements of list2 is NOT found on list1, return FALSE.
      ** Else, return TRUE.
      */
      for( i=0; (sr_ref=sdsr_list_get_ref(list2, i)) != SDSR_REF_NULL; i++ )
      {
        if( sdsr_list_find_ref(list1, sr_ref) == SDSR_POS_NULL )
        {
          return FALSE;
        }
      }
      return TRUE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_CONTAIN_SAME:

      /* If list2 does not contain the same number of elements as list1 or
      ** any of the elements of list2 is NOT found on list1, return FALSE.
      ** Else, return TRUE.
      */
      if( sdsr_list_cnt(list1) != sdsr_list_cnt(list2) )
      {
        return FALSE;
      }

      for( i=0; (sr_ref=sdsr_list_get_ref(list2, i)) != SDSR_REF_NULL; i++ )
      {
        if( sdsr_list_find_ref(list1, sr_ref) == SDSR_POS_NULL )
        {
          return FALSE;
        }
      }

      return TRUE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_CONTAIN_MAX:
    default:

      SD_CASE_ERR( "contain_cond",0,0,0 );
      return FALSE;

  } /* switch */

} /* sdsr_list_is_contain */




/* <EJECT> */
/*===========================================================================
=============================================================================
============================== LIST MANIPULATORS ============================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdsr_list_clr

DESCRIPTION
  Clear a system record list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_clr(

        sdsr_e_type               list
            /* List to clear.
            */
)
{
  sdsr_list_s_type  *list_ptr;
  sdsr_ref_type     *item_ptr;

  //SD_FUNC_START( "sdsr_list_clr - list=%d",list,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Based on whether list resolved to a system record list or item, perform
  ** the clear operation.
  */
  if( (list_ptr=sdsr_list_ptr(list)) != NULL )
  {
    list_ptr->num = 0;
  }
  else if( (item_ptr=sdsr_item_ptr(list)) != NULL )
  {
    *item_ptr = SDSR_REF_NULL;
  }

} /* sdsr_list_clr */




#if 0
///* <EJECT> */
///*===========================================================================
//
//FUNCTION sdsr_list_clr_if_valid
//
//DESCRIPTION
//  If input system recored list is valid, clear the list.
//
//DEPENDENCIES
//  None.
//
//RETURN VALUE
//  None.
//
//SIDE EFFECTS
//  None.
//
//===========================================================================*/
//MMEXTN  void                      sdsr_list_clr_if_valid(
//
//        sdsr_e_type               list
//            /* List to clear.
//            */
//)
//{
//  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX, SDSR_MAX );
//
//  if( list != SDSR_MAX )
//  {
//    sdsr_list_clr( list );
//  }
//
//} /* sdsr_list_clr_if_valid */
#endif



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_del_pos

DESCRIPTION
  Delete a position from a system record list.

  Deletion position of SDSR_POS_NULL implies deletion from the end of the
  list.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_del_pos(

        sdsr_e_type               list,
            /* List for which to add the system record.
            */

        int                       del_pos
            /* Deletion position. SDSR_POS_NULL implies deletion from the
            ** end of the list.
            */
)
{
  int                 i;
  sdsr_list_s_type    *list_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at list to be deleted.
  */
  list_ptr = sdsr_list_ptr( list );
  if( list_ptr == NULL )
  {
    SD_MSG_ERROR(" list_ptr NULL",0,0,0);
    return;
  }
  SD_ASSERT( list_ptr->num <= (int) ARR_SIZE(list_ptr->arr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If delete position is, SDSR_POS_NULL, delete from the end of list.
  */
  if( del_pos == SDSR_POS_NULL )
  {
    del_pos = list_ptr->num;
  }
  SD_ASSERT( INRANGE(del_pos, 0, list_ptr->num) )

  del_pos = MIN(del_pos, list_ptr->num);
  del_pos = MAX(del_pos, 0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Delete the position from the list.
  */
  list_ptr->num--;
  for( i=del_pos; i < list_ptr->num; i++ )
  {
    list_ptr->arr[i] = list_ptr->arr[i+1];
  }

} /* sdsr_list_del_pos */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_del_ref

DESCRIPTION
  Delete a system record reference from a system record list.

  Note! this operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  Position of system record reference that was deleted from the list.
  SDSR_POS_NULL, if system record reference to be deleted was not found
  on the list.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  int                       sdsr_list_del_ref(

        sdsr_e_type               list,
            /* List for which to add the system record.
            */

        sdsr_ref_type             sr_ref
            /* System record reference to be deleted.
            */
)
{
  int                 del_pos;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return SDSR_POS_NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system record reference to be deleted is not on the list return
  ** now.
  */
  del_pos = sdsr_list_find_ref(list, sr_ref);

  if( del_pos == SDSR_POS_NULL )
  {
    return SDSR_POS_NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Delete the position from the list.
  */
  sdsr_list_del_pos( list, del_pos );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return del_pos;

} /* sdsr_list_del_ref */



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_del_list_cnt

DESCRIPTION
  Deletes 'count' entries from the list, starting from start_pos.

  Note! this operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_del_list_cnt(

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */
        int                      start_pos,
            /* The starting position in the list
            */
        int                      cnt
            /* Number of elements to delete from the list
            */
)
{

  int i=0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX || start_pos >= sdsr_list_cnt(list) )
  {
    return;
  }

  for( i = 0; i<cnt; i++ )
  {
    if( start_pos >= sdsr_list_cnt(list) )
    {
      return;
    }

    sdsr_list_del_pos( list, start_pos );
  }

  return;

} /* sdsr_list_del_list_cnt */


/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_del_list

DESCRIPTION
  Delete the system records that are referenced by list_ref from list.

  Note! this operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_del_list(

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */

        sdsr_e_type               list_ref
            /* Reference list.
            */
)
{
  sdsr_ref_type   sr_ref;
  int             i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list_ref, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through list_ref and delete its elements from list.
  */
  for( i=0; (sr_ref=sdsr_list_get_ref(list_ref, i)) != SDSR_REF_NULL; i++ )
  {
    (void) sdsr_list_del_ref( list, sr_ref );
  }

} /* sdsr_list_del_list */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_del_sys

DESCRIPTION
  Delete system records from list according to the del_cat parameter.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_del_sys(

        sd_ss_e_type              ss,
            /* The SS for which to check the preferences
            */

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */

        sdsr_cat_e_type           sr_cat
            /* System record category.
            */
)
{
  sdsr_s_type                     *sr_ptr;
  int                             i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( sr_cat, SDSR_CAT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through list and delete system records that are called for by
  ** sr_cat.
  */
  for( i = sdsr_list_cnt(list) - 1; i >= 0; i-- )
  {
    sr_ptr = sdsr_list_get_ptr( list, i );
    SD_ASSERT( sr_ptr != NULL );

    switch( sr_cat )
    {
      case SDSR_CAT_NON_AMPS:
        if( sr_ptr->sys.mode != SD_MODE_AMPS )
        {
          sdsr_list_del_pos( list, i );
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_CDMA:
        if( sr_ptr->sys.mode != SD_MODE_CDMA )
        {
          sdsr_list_del_pos( list, i );
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_HDR:
        if( sr_ptr->sys.mode != SD_MODE_HDR )
        {
          sdsr_list_del_pos( list, i );
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_GW:
        if( !sdss_sr_is_mode_pref( sr_ptr, SD_SS_MODE_PREF_GW ) )
        {
          sdsr_list_del_pos( list, i );
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_GW_SYS:
        if( sdss_sr_is_mode_pref( sr_ptr, SD_SS_MODE_PREF_GW ) )
        {
          sdsr_list_del_pos( list, i );
        }
        break;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_CELL:
        if( !sdss_sr_is_band_pref( sr_ptr, SD_SS_BAND_PREF_CELL ) )
        {
          sdsr_list_del_pos( list, i );
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_COMPLY:
        if( !sdss_is_supp_mode_band(sr_ptr->sys.mode, sr_ptr->sys.band) ||
            sdss_sr_is_pref_conf ( ss,
                                   sr_ptr,
                                   SDSS_PREF_CONF_MODE_OR_BAND) )
        {
          sdsr_list_del_pos( list, i );
        }
        break;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_HDR:
        if( sr_ptr->sys.mode == SD_MODE_HDR )
        {
          sdsr_list_del_pos( list, i );
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_WLAN:
        if( sr_ptr->sys.mode != SD_MODE_WLAN )
        {
          sdsr_list_del_pos( list, i );
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_ALL:
        sdsr_list_del_pos( list, i );
        break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_MAX:
      default:
        SD_CASE_ERR( "sr_cat",0,0,0 );
        break;

    } /* switch */

  } /* for */

} /* sdsr_list_del_sys */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_del_band

DESCRIPTION
  Delete system records from list according to the band information in
  the reference system.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_del_band(

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */

        sdsr_e_type               sys_ref
            /* reference sys which contains band information
            */
)
{
  sdsr_s_type                     *sr_ptr;
  sdsr_s_type                     *tmp_sr_ptr;
  int                             i;
  sd_ss_band_pref_e_type          band_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( sys_ref, SDSR_MAX, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (list    == SDSR_MAX)          ||
      (sys_ref == SDSR_MAX)          ||
      (sdsr_list_cnt( list )  == 0)  ||
      (sdsr_list_cnt( sys_ref ) == 0)
    )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read band preference from reference system. */
  tmp_sr_ptr= sdsr_list_get_ptr(sys_ref, 0);
  if(tmp_sr_ptr == NULL)
  {
    SD_MSG_ERROR("tmp_sr_ptr NULL",0,0,0);
    return;
  }
  band_pref = tmp_sr_ptr->sys.band;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through list and delete system records whose band is not same as
  ** that in the reference list.
  */
  for( i = sdsr_list_cnt(list) - 1; i >= 0; i-- )
  {
    sr_ptr = sdsr_list_get_ptr( list, i );

    if(sr_ptr == NULL)
    {
      SD_MSG_ERROR("sr_ptr NULL",0,0,0);
      continue;
    }
    if( sr_ptr->sys.band != band_pref )
    {
      /* delete the record from list */
      sdsr_list_del_pos( list, i );
    }
  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_insert_ref

DESCRIPTION
  Insert a system record reference to a system record list.

  Insertion position of SDSR_POS_NULL implies insertion at the end of the
  list.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_insert_ref(

        sdsr_e_type               list,
            /* List to be inserted.
            */

        int                       insert_pos,
            /* Insertion position. SDSR_POS_NULL implies insertion at the
            ** end of the list.
            */

        sdsr_ref_type             sr_ref,
            /* System record reference which to insert to the list.
            */

        boolean                   is_delete_dup
            /* Indicate whether a duplicate item should be deleted from
            ** the list.
            */
)
{
  /*lint -save -e676 'list_ptr->arr[i]' iPossibly negative subscript -1*/
  int                 i;
  sdsr_list_s_type    *list_ptr;
  int                 dup_pos = SDSR_POS_NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at list to be inserted.
  */
  list_ptr = sdsr_list_ptr( list );
  if(list_ptr == NULL)
  {
    SD_MSG_ERROR("list_ptr NULL",0,0,0);
    return;
  }
  SD_ASSERT( list_ptr->num < (int) ARR_SIZE(list_ptr->arr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If insert position is SDSR_POS_NULL, insert at end of list.
  */
  if( insert_pos == SDSR_POS_NULL )
  {
    insert_pos = list_ptr->num;
  }
  SD_ASSERT( INRANGE(insert_pos, 0, list_ptr->num) )

  insert_pos = MIN(insert_pos, list_ptr->num);
  insert_pos = MAX(insert_pos, 0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are required to eliminate a duplicate item and a duplicate item
  ** is found, set the duplicate position per this item. Else, set the
  ** duplicate position to the end of the list.
  */
  if( is_delete_dup )
  {
    dup_pos = sdsr_list_find_ref( list, sr_ref );
  }

  if( dup_pos == SDSR_POS_NULL )
  {
    dup_pos = list_ptr->num;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If insert position is before duplicate position, shift items between
  ** duplicate position and insert position to the right.
  */
  if( insert_pos < dup_pos )
  {
    for( i=dup_pos; i > insert_pos; i-- )
    {
      list_ptr->arr[i] = list_ptr->arr[i-1];
    }
  }

  /* Else, if insert position is after duplicate position, shift items
  ** between duplicate position and insert position to the left and decrement
  ** insert position by one.
  */
  else if( insert_pos > dup_pos )
  {
    insert_pos--;

    if( insert_pos < 0 )
    {
      SD_MSG_ERROR(" insert_pos = %d",insert_pos,0,0);
      return;
    }

    for( i=dup_pos; i < insert_pos; i++ )
    { 
      list_ptr->arr[i] = list_ptr->arr[i+1]; 
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Insert item at specified position.
  */
  list_ptr->arr[insert_pos] = sr_ref;

  if( dup_pos == list_ptr->num )
  {
    list_ptr->num++;
  }

} /* sdsr_list_insert_ref */
/*lint -restore */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_store_and_app

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table along is a specified SID and append a reference to the
  stored system record to the end of the specified input list.
  list.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_store_and_app(

        sdsr_e_type               list,
            /* List to be inserted.
            */

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        const sd_plmn_s_type      *sd_plmn_ptr,
            /* The system's PLMN
            */

        word                      sys_idx
            /* The system table index referencing this system.
            */
)
{
  sdsr_ref_type sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( list != SDSR_MAX )
  {
    sr_ref = sdsr_tbl_store7( mode, band_pref, chan, sd_plmn_ptr, sys_idx );
    SD_ASSERT( sr_ref != SDSR_REF_NULL );

    sdsr_list_app_ref( list, sr_ref );
  }

} /* sdsr_list_store_and_app */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_store_and_app2

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table along is a specified system index and append a
  reference to the stored system record to the end of the specified input
  lists.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                           sdsr_list_store_and_app2(

        sdsr_e_type                    list1,
        sdsr_e_type                    list2,
        sdsr_e_type                    list3,
            /* Lists to be inserted.
            */

        sd_mode_e_type                 mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type         band_pref,
            /* The system's band.
            */

        sd_chan_type                   chan,
            /* The system's channel.
            */

        const sd_sid_plmn_wlan_s_type  *sid_exp_ptr,
            /* Pointer to sid exp type
            */

        word                           sys_idx
            /* The system table index referencing this system.
            */
)
{
  sdsr_ref_type sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list1, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( list2, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( list3, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( list1 != SDSR_MAX ||
      list2 != SDSR_MAX ||
      list3 != SDSR_MAX )
  {
    sr_ref = sdsr_tbl_store9( mode, band_pref, chan, sid_exp_ptr, sys_idx,
                              SYS_NW_PROFILE_ID_INVALID,
                              SYS_WLAN_BSS_TYPE_NONE,
                              SYS_TECH_MASK_EMPTY
                            );
    SD_ASSERT( sr_ref != SDSR_REF_NULL );

    sdsr_list_app_ref( list1, sr_ref );
    sdsr_list_app_ref( list2, sr_ref );
    sdsr_list_app_ref( list3, sr_ref );
  }

} /* sdsr_list_store_and_app2 */




#if 0
///* <EJECT> */
///*===========================================================================
//
//FUNCTION sdsr_list_insert_list
//
//DESCRIPTION
//  Insert a system record list into another system record list.
//
//  If the reference is already on the list, it is deleted and then inserted
//  at the specified position.
//
//  Note! The add operation is only valid for system record lists (as apposed
//  to system record items).
//
//DEPENDENCIES
//  None.
//
//RETURN VALUE
//  None.
//
//SIDE EFFECTS
//  None.
//
//===========================================================================*/
//MMEXTN  void                      sdsr_list_insert_list(
//
//        sdsr_e_type               list_to,
//            /* List to be inserted.
//            */
//
//        int                       insert_pos,
//            /* Insertion position. SDSR_POS_NULL implies insertion to the
//            ** end of the list.
//            */
//
//        sdsr_e_type               list_from,
//            /* List from which to insert.
//            */
//)
//{
//  int                 i;
//  int                 cnt;
//  sdsr_s_type         *trav_ptr;
//  //sdsr_list_s_type    *list_ptr;
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  SD_ASSERT_ENUM_IS_INRANGE( list_to, SDSR_MAX_LST );
//  SD_ASSERT_ENUM_IS_INRANGE( list_from, SDSR_MAX );
//
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
////  if( insert_pos == SDSR_POS_NULL )
//  {
//    insert_pos = sdsr_list_cnt( list_to );
//  }
//
//  for( i=0; (sr_ref=sdsr_list_get_ref(list_from, i)) != SDSR_REF_NULL; i++ )
//  {
//    sdsr_list_insert_ref( list_to, insert_pos, sr_ref );
//    insert_pos++;
//  }
//
//} /* sdsr_list_insert_ref */
#endif




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_put_ref

DESCRIPTION
  Put a system record reference into a system record item.

  Note! The put operation is only valid for system record items (as apposed
  to system record lists).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_put_ref(

        sdsr_e_type               list,
            /* Item where to put the system record information.
            */

        sdsr_ref_type             sr_ref
            /* Pointer to a system record reference which to put into the
            ** system record item that is referenced by list.
            */
)
{
  sdsr_ref_type   *item_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( INRANGE(list, SDSR_MAX_LST, SDSR_MAX) );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a pointer to the system record item.
  */
  item_ptr = sdsr_item_ptr( list );
  if( item_ptr == NULL )
  {
    SD_MSG_ERROR(" item_ptr NULL",0,0,0);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the system record item as per the input system record reference.
  */
  *item_ptr = sr_ref;

} /* sdsr_list_put_ref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_app_ref

DESCRIPTION
  Append a system record reference to the end of a list. Duplicate reference
  is not appended.

  This operation is only valid to system record lists (as apposed to system
  record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_app_ref(

        sdsr_e_type               list,
            /* List to which to append system records.
            */

        sdsr_ref_type             sr_ref
            /* System record reference which to appended.
            */
)
{
  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system record reference is not already on the list, add the
  ** reference to the end of the list.
  */
  if( sdsr_list_find_ref(list, sr_ref) == SDSR_POS_NULL )
  {
    sdsr_list_insert_ref( list, SDSR_POS_NULL, sr_ref, FALSE );
  }

} /* sdsr_list_app_ref */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_app

DESCRIPTION
  Append the elements from one system record list to the end of another
  system record list. Duplicate items are not appended.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_app(

        sdsr_e_type               list_to,
            /* List to which to append system records.
            */

        sdsr_e_type               list_from
            /* List from which to append system records.
            */
)
{
  int               i;
  sdsr_ref_type     sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list_to, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list_from, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list_to == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list_from and add append items to list_to.
  */
  for( i=0; (sr_ref=sdsr_list_get_ref(list_from, i)) != SDSR_REF_NULL; i++ )
  {
    sdsr_list_app_ref( list_to, sr_ref );
  }

} /* sdsr_list_app */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_order

DESCRIPTION
  Order the elements of a system record list according to their order in a
  reference system record list.

  Note that the ordering algorithm is stable; that is, items that are
  indifference according to the reference list retain their relative order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_order(

        sdsr_e_type               list,
            /* List to be ordered.
            */

        sdsr_e_type               list_ref
            /* Reference list according to which to order the elements of
            ** list
            */
)
{
  int             i;
  sdsr_ref_type   sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list_ref, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the reference list (last to first) and insert at the
  ** beginning of the list that is being ordered any item that is found on
  ** both lists.
  */
  for( i = sdsr_list_cnt(list_ref) - 1; i >= 0; i-- )
  {
    sr_ref = sdsr_list_get_ref( list_ref, i );
    SD_ASSERT( sr_ref != SDSR_REF_NULL );

    if( sdsr_list_find_ref(list, sr_ref) != SDSR_POS_NULL )
    {
      sdsr_list_insert_ref( list,           /* Insert to this list */
                            0,              /* Insert at beginning of list */
                            sr_ref,         /* Insert this item */
                            TRUE );         /* Remove any duplicates */
    }
  }

} /* sdsr_list_order */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_order

DESCRIPTION
  Order the elements of a system record list according to the reference
  system record based on the order category.

  Note that the ordering algorithm is stable; that is, items that are
  indifference according to the reference list retain their relative order.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_order(

        sdsr_e_type               list,
            /* List to be ordered.
            */

        const sdsr_s_type         *sr_ref_ptr,
            /* Pointer to Reference system to be ordered by.
            */

        sdsr_order_e_type         order_cat
            /* Ordering categories, according to which part of system records
            ** can be compared and system record lists can be ordered.
            */
)
{
  int            pos, j, count;
  sdsr_s_type    *sr_ptr;
  sdsr_ref_type  sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ref_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( order_cat )
  {
    case SDSR_ORDER_BAND:

      /* Step through the list in reverse order and find the matching system
      ** from the sytem based on the order categories and inserted to the
      ** list.
      */
      pos = sdsr_list_cnt(list) -1;
      count = pos;

    /*-----paragk, this logic is wrong, it checks only half the times, change it!
    */
      for( j = 0; j <=count; j++ )
      {
        sr_ptr = sdsr_list_get_ptr( list, pos );
        if(sr_ptr == NULL)
        {
          SD_MSG_ERROR(" sr_ptr NULL",0,0,0);
          continue;
        }

        if( SD_BAND_CONTAIN(sr_ptr->sys.band, sr_ref_ptr->sys.band) )
        {
          sr_ref = sdsr_list_get_ref( list, pos );
          sdsr_list_insert_ref( list,           /* Insert to this list */
                                0,              /* Insert at beginning of list */
                                sr_ref,         /* Insert this item */
                                TRUE );         /* Remove any duplicates */
        }
        else
        {
          pos --;
        }
      }

      break;
    case SDSR_ORDER_MAX:
    default:
      break;
  }

} /* sdsr_order */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_order2

DESCRIPTION
  Order the elements of a system record list according to a reference system
  record list based on the order categories.

  Note that the ordering algorithm is stable; that is, items that are
  indifference according to the reference list retain their relative order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_order2(

        sdsr_e_type               list,
            /* List to be ordered.
            */

        sdsr_e_type               list_ref,
            /* Reference list according to which to order the elements of
            ** list
            */

        sdsr_order_e_type         order_cat
            /* Ordering categories, according to which part of system records
            ** can be compared and system record lists can be ordered.
            */
)
{
  int             i;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list_ref, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the reference list (last to first) and insert at the
  ** beginning of the list that is being ordered according to the reference
  ** system based on the order cat.
  */
  for( i = sdsr_list_cnt(list_ref) - 1; i >= 0; i-- )
  {
    sr_ptr = sdsr_list_get_ptr( list_ref, i );
    SD_ASSERT( sr_ptr != NULL );

    sdsr_order( list, sr_ptr, order_cat );
  }

} /* sdsr_list_order2 */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_swap

DESCRIPTION
  Swap two list elements.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdsr_list_swap(

        sdsr_e_type               list,
            /* List over which to swap the elements.
            */

        int                       pos_a,
            /* List position of element to be swapped with pos_b.
            */

        int                       pos_b
            /* List position of element to be swapped with pos_a.
            */
)
{
  sdsr_ref_type     sr_ref;
  sdsr_list_s_type  *list_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  if( !INRANGE(pos_a, 0, sdsr_list_cnt(list)))
  {
    SD_MSG_ERROR(" pos_a out of range",0,0,0);
    return;
  }
    
  if( !INRANGE(pos_b, 0, sdsr_list_cnt(list)))
  {
    SD_MSG_ERROR(" pos_b out of range",0,0,0);
    return;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the list over which the elements are to be swapped.
  */
  list_ptr = sdsr_list_ptr( list );
  if( list_ptr == NULL )
  {
    SD_MSG_ERROR(" list_ptr NULL ",0,0,0);
    return;
  }

  /* Swap the two elements.
  */
  /*lint -save -e676 possitive negative subscript in operator*/
  sr_ref                = list_ptr->arr[pos_a];
  list_ptr->arr[pos_a]  = list_ptr->arr[pos_b];
  list_ptr->arr[pos_b]  = sr_ref;
  /*lint -restore */

} /* sdsr_list_swap */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_sort_shift_up

DESCRIPTION
  Perform the shift-up operation that is required for heap sorting a system
  record list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdsr_list_sort_shift_up(

        sdsr_e_type               list,
            /* List over which to perform the shift-up operation.
            */

        int                       pos,
            /* List position from which to start the shift-up operation.
            */

        sdsr_comp_e_type          comp_cat
            /* Comparison category according to which system records should
            ** be compared.
            */
)
{
  int           parent_pos;
  sdsr_s_type   *sr_ptr;
  sdsr_s_type   *parent_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX_LST );
  SD_ASSERT( pos < sdsr_list_cnt(list) );
  SD_ASSERT_ENUM_IS_INRANGE( comp_cat, SDSR_COMP_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while( pos > 0 )
  {
    parent_pos = (pos+1)/2 - 1;

    sr_ptr      = sdsr_list_get_ptr( list, pos );
    parent_ptr  = sdsr_list_get_ptr( list, parent_pos );

    /* If the parent <= child, we are done shifting. Else, continue up
    ** shifting.
    */
    if( sdsr_comp2(parent_ptr, sr_ptr, comp_cat) <= 0 )
    {
      break;
    }

    sdsr_list_swap( list, parent_pos, pos );

    pos = parent_pos;
  }

} /* sdsr_list_sort_shift_up */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_sort_shift_down

DESCRIPTION
  Perform the shift-down operation that is required for heap sorting a system
  record list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdsr_list_sort_shift_down(

        sdsr_e_type               list,
            /* List over which to perform the shift-up operation.
            */

        int                       end_pos,
            /* List position where to end the shift-down operation.
            */

        sdsr_comp_e_type          comp_cat
            /* Comparison category according to which system records should
            ** be compared.
            */
)
{
  int           pos;
  int           child_pos;
  int           ret_val;
  sdsr_s_type   *sr_ptr;
  sdsr_s_type   *child_ptr;
  sdsr_s_type   *child_left_ptr;
  sdsr_s_type   *child_right_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX_LST );
  SD_ASSERT( end_pos < sdsr_list_cnt(list) );
  SD_ASSERT_ENUM_IS_INRANGE( comp_cat, SDSR_COMP_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( pos=0; (child_pos=(pos+1)*2 - 1) < end_pos;  )
  {
    /* If parent has a right child, find the smaller of the two children.
    */
    if( child_pos+1 < end_pos )
    {
      child_left_ptr  = sdsr_list_get_ptr( list, child_pos );
      child_right_ptr = sdsr_list_get_ptr( list, child_pos+1 );
      ret_val = sdsr_comp2(child_right_ptr, child_left_ptr, comp_cat);
      if( ret_val < 0 )
      {
        if( ret_val == -2 )
        {
          /* This is an error condition when either or both of the system records
          ** under comparison are NULL
          */
          break;
        }
        child_pos++;
      }
    }

    sr_ptr    = sdsr_list_get_ptr( list, pos );
    child_ptr = sdsr_list_get_ptr( list, child_pos );

    /* If the parent <= the smallest of the two children, we are done
    ** shifting. Else, continue down shifting.
    */
    if( sdsr_comp2(sr_ptr, child_ptr, comp_cat) <= 0 )
    {
      break;
    }

    sdsr_list_swap( list, child_pos, pos );

    pos = child_pos;
  }

} /* sdsr_list_sort_shift_down */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_sort

DESCRIPTION
  Sort the elements of a system record list according to an indicated sorting
  order. Note that the sorting algorithm is stable; that is, items that are
  indifference according to the sorting criteria retain their relative order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_sort(

        sdsr_e_type               list,
            /* List to be sorted.
            */

        sdsr_comp_e_type          comp_cat
            /* Comparison category according to which system records should
            ** be compared.
            */
)
{
  int         i;
  sdsr_s_type *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( comp_cat, SDSR_COMP_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the position field of the system records according to their list
  ** positions. This step is necessary in order to achieve stable sorting.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    sr_ptr->list_pos = i; /*lint !e734 */ /* Loss of precision */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Turn the list into a heap by stepping through the list (first to last)
  ** while performing the shift-up operation.
  */
  for( i=1; i < sdsr_list_cnt(list); i++ )
  {
    sdsr_list_sort_shift_up( list, i, comp_cat );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Turn the heap into a sorted list by stepping through the list (last to
  ** first) while swapping the first element and performing the shift-down
  ** operation.
  */
  for( i = sdsr_list_cnt(list)-1; i > 0; i-- )
  {
    sdsr_list_swap( list, 0, i );
    sdsr_list_sort_shift_down( list, i, comp_cat );
  }

} /* sdsr_list_sort */




/*===========================================================================

FUNCTION sdsr_lists_init

DESCRIPTION
  Initializes the system record lists.

  NOTE! this function must be called before any other sdss_list_xxx is called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                  sdsr_lists_init( void )
{
  int i;

  SD_FUNC_START( "sdsr_lists_init",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear all system record lists.
  */
  for( i=0; i < (int)SDSR_MAX_LST; i++ )
  {
    sdsr_list_clr( (sdsr_e_type) i );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear all system record items.
  */
  for( i= (int) SDSR_1ST_SYS; i < (int) SDSR_MAX; i++ )
  {
    sdsr_list_clr( (sdsr_e_type) i );
  }

} /* sdsr_lists_init */


#ifdef FEATURE_HDR
#error code not present
#endif

#if ( defined (FEATURE_HDR) || defined(FEATURE_TECH_IND_SD_STACK_ARCH))
/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_lists_hybr_pref_switch

DESCRIPTION
  Switches the SDSR_HDR_XXX list to reference the appropriate lists according
  to the specified hybrid preference.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                    sdsr_lists_hybr_pref_switch(

        sd_ss_hybr_pref_e_type  hybr_pref
            /* Hybrid preference.
            */
)
{
  static sd_ss_hybr_pref_e_type  prev_hybr_pref = SD_SS_HYBR_PREF_MAX;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( hybr_pref, SD_SS_HYBR_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If hybrid preference did not changed, return now.
  */
  if( prev_hybr_pref == hybr_pref )
  {
    return;
  }

  /* Remember the hybrid preference.
  */
  prev_hybr_pref = hybr_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( hybr_pref )
  {
    case SD_SS_HYBR_PREF_NONE:
    case SD_SS_HYBR_PREF_CDMA__WCDMA:
      /* We are not doing CDMA/HDR hybrid operqation, so switch the HDR
      ** system record lists to reference the normal lists that are used
      ** during CDMA operation.
      */
      #ifdef FEATURE_HDR
#error code not present
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_HYBR_PREF_CDMA__HDR:
    case SD_SS_HYBR_PREF_CDMA__HDR_WCDMA:

      #ifdef FEATURE_TECH_IND_SD_STACK_ARCH

      /* We are doing CDMA/HDR hybrid operqation, so switch the HDR system
      ** record lists to reference their CDMA counterparts.
      */
      #ifdef FEATURE_HDR
#error code not present
      #endif
      break;

      #elif defined FEATURE_HDR_HYBRID
#error code not present
      #endif /* FEATURE_HDR_HYBRID */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_HYBR_PREF_MAX:
    default:
      SD_CASE_ERR( "equal_cat",0,0,0 );
      break;

  } /* switch */



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_HDR
#error code not present
  #endif

} /* sdsr_lists_hybr_pref_switch */
#endif /* FEATURE_HDR */




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_lists_update_gw_sys

DESCRIPTION
  If the network selection type of the reference system is automatic or user
  mode, or limited
  Create a counter part of limited system and store it in SDSR_GW_LTD_LST
  and update SDSR_GW_LAST_LTD_SYS if update is requested.
  Create a counter part of user system and store it in SDSR_USER_LST and
  update SDSR_GW_LAST_USER_SYS if update is requested.
  Create an automatic mode counter part of the system and store it in
  SDSR_GW_LAST_FULL_SYS and if update is requested.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_update_gw_sys(

        sd_ss_e_type              ss,
            /* The ss for which the GW sys is to be updated
            */

        sdsr_ref_type             sr_ref,
            /* A system reference to create the couterpart of limited/user
            ** systems.
            */
        boolean                   update_ltd_sys,
            /* If ture, update the limited list and system with the system
            ** reference.
            */
        boolean                   update_user_sys,
            /* If ture, update the user list and system with the system
            ** reference.
            */
        boolean                   update_full_sys
            /* If ture, update the full system with the system reference.
            */
)
{
  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_GW)

  sdsr_s_type     sr;
  sdsr_e_type     ltd_lst = sdss_sr_list_map2(ss, SDSR_GW_LTD_LST );
  sdsr_e_type     last_ltd_sys = SDSR_MAX;
  sdsr_e_type     manual_lst = sdss_sr_list_map2(ss, SDSR_MANUAL_LST );
  sdsr_e_type     last_user_sys = SDSR_MAX;
  sdsr_e_type     last_full_sys = SDSR_MAX;
  sdsr_ref_type   ltd_sr_ref;
  sdsr_ref_type   user_sr_ref;
  sdsr_ref_type   full_sr_ref;
  boolean         is_sys_ltd = FALSE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sr_ref == SDSR_REF_NULL )
  {
    return;
  }

  if( !sdsr_tbl_get( sr_ref, &sr ) )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the reference system is nor GSM or WCDMA, return now.
  */
  if( sr.sys.mode != SD_MODE_GSM    &&
      sr.sys.mode != SD_MODE_WCDMA  &&
      sr.sys.mode != SD_MODE_GW )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( update_ltd_sys )
  {
    last_ltd_sys = sdss_sr_list_map2( ss, SDSR_GW_LAST_LTD_SYS );
  }

  if( update_user_sys )
  {
    last_user_sys = sdss_sr_list_map2(ss, SDSR_GW_LAST_USER_SYS );
  }

  if( update_full_sys )
  {
    last_full_sys = sdss_sr_list_map2(ss, SDSR_GW_LAST_FULL_SYS );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the network selection mode is not AUTOMATIC or MANUAL, return now.
  */
  if( sr.sid_expected.net_select_mode != SD_NET_SELECT_MODE_AUTO  &&
      sr.sid_expected.net_select_mode != SD_NET_SELECT_MODE_MANUAL_SYS &&
      sr.sid_expected.net_select_mode != SD_NET_SELECT_MODE_LIMITED )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the last user system from the system from user list that contains
  ** the same band group of the reference system.
  */
  if( sr.sid_expected.net_select_mode == SD_NET_SELECT_MODE_LIMITED )
  {
    sdsr_order( manual_lst, &sr, SDSR_ORDER_BAND );
    is_sys_ltd = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the equivalent system record with automatic mode.
  */
  sr.sid_expected.net_select_mode = SD_NET_SELECT_MODE_AUTO;

  /* Store the system record that is an automatic system.
  */
  full_sr_ref = sdsr_tbl_store8( &sr );

  /* Update SDSR_GW_LAST_NON_LTD_SYS with full reference system.
  */
  sdsr_list_put_ref( last_full_sys, full_sr_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system is a limited system, stop here.
  */
  if(is_sys_ltd)
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the equivalent system record with limited mode.
  */
  sr.sid_expected.net_select_mode = SD_NET_SELECT_MODE_LIMITED;

  ltd_sr_ref = sdsr_tbl_store8( &sr );

  /* Append the record in the GW LTD LST.
  */
  sdsr_list_app_ref( ltd_lst, ltd_sr_ref );

  /* Update SDSR_GW_LAST_LTD_SYS with ltd reference system.
  */
  sdsr_list_put_ref( last_ltd_sys, ltd_sr_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the equivalent system record with manual mode.
  */
  sr.sid_expected.net_select_mode = SD_NET_SELECT_MODE_MANUAL_SYS;

  user_sr_ref = sdsr_tbl_store8( &sr );

  /* Append the record in the MANUAL LST.
  */
  sdsr_list_app_ref( manual_lst, user_sr_ref );

  /* Update SDSR_GW_LAST_USER_SYS with user reference system.
  */
  sdsr_list_put_ref( last_user_sys, user_sr_ref );

  #else
  SYS_ARG_NOT_USED(ss);
  SYS_ARG_NOT_USED(sr_ref);
  SYS_ARG_NOT_USED(update_ltd_sys);
  SYS_ARG_NOT_USED(update_user_sys);
  SYS_ARG_NOT_USED(update_full_sys);
  #endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_GW */
}


/*===========================================================================

FUNCTION sdsr_list_undefine_plmn_id

DESCRIPTION
  Undefine the plmn id of a specified list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_list_undefine_plmn_id(

        sdsr_e_type               list
            /* List to undefine plmn id.
            */
)
{
  sdsr_s_type       *sr_ptr;
  int               i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i=0; (sr_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( sr_ptr->sys.mode == SD_MODE_GSM ||
        sr_ptr->sys.mode == SD_MODE_WCDMA ||
        sr_ptr->sys.mode == SD_MODE_GW )
    {
      sys_plmn_undefine_plmn_id( &sr_ptr->sid_expected.prm.plmn.plmn_id );
    }
  }

  return;

} /* sdsr_list_undefined_plmn_id */

#if defined(FEATURE_HDR)
#error code not present
#endif /* (FEATURE_HDR) */



/*===========================================================================

FUNCTION sdsr_list_is_fade

DESCRIPTION
  Check if there are at least CNT number of fades for the reference system.
  Return true if so otherwise return false.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean                   sdsr_list_is_fade(

        sdsr_e_type               list,
            /* List to the reference system.
            */
        byte                      cnt
            /* Number of consecutive fades.
            */
)
{
  sdsr_s_type     *item_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get reference system.
  */
  item_ptr = sdsr_list_get_ptr( list, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( item_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH("IS_FADE LIST = %d, CNT= %d, num_fade = %d", list, cnt, item_ptr->num_fade );

  if ( item_ptr->num_fade >= cnt )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* sdsr_list_is_fade */



/*===========================================================================

FUNCTION sdsr_list_reset_status_mask

DESCRIPTION
  Reset the status bit mask of systems in the reference list.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN void                       sdsr_list_reset_status_mask(
  
        sdsr_e_type               list,
            /* List to the reference system.
            */
        sdsr_status_mask_e_type   status_mask
            /* Status bit mask to be reset
            */
)
{
  sdsr_s_type     *item_ptr;
  int             i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     
  /* If list is empty, return now.
  */
  if( sdsr_list_cnt( list ) == 0 )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset del tmsi bit mask of the system in the list.
  */
  for( i=0; i<sdsr_list_cnt(list); i++ )
  {
    item_ptr = sdsr_list_get_ptr( list, i );

    if( item_ptr == NULL )
    {
      SD_MSG_ERROR(" item_ptr NULL ",0,0,0);
      continue;
    }

    SDSR_STATUS_MASK_OFF( item_ptr->status_mask, status_mask );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;


}/* sdsr_list_reset_tmsi */




/*===========================================================================

FUNCTION sdsr_is_delete_tmsi

DESCRIPTION
  This function indicates that whether tmsi should be delete or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
MMEXTN  boolean                   sdsr_is_del_tmsi(

        sdsr_e_type               list ,
            /* List to the reference system.
            */

        int                       pos
            /* List position from which to get the system record reference.
            */
)
{
  sdsr_s_type       *item_ptr;
  boolean           is_del_tmsi;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the pointer to the system record that is referenced by the list and
  ** list position.
  */
  item_ptr = sdsr_list_get_ptr( list, pos );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The pointer is NULL, return now.
  */ 
  if( item_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     
  /* Return the del tmsi indication.
  */
  is_del_tmsi = SDSR_STATUS_MASK_GET( item_ptr->status_mask, 
                                      SDSR_STATUS_MASK_DEL_TMSI);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH("is_del_tmsi=%d", is_del_tmsi, 0, 0 );

  return is_del_tmsi;

}/* sdsr_is_delete_tmsi */


/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_is_sys

DESCRIPTION
  Indicate if the specified system is GW system.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  True  the system is mode.
  FALSE Otherwise.

  Note: mode GW will return TRUE if the system is either GSM, WCDMA or GW

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN boolean                    sdsr_is_sys(

       sd_mode_e_type             mode,
           /* The mode to check for the system
           */

       sdsr_e_type                list,
           /* List from which to get the specified system record.
           */

       int                        pos
           /* List position from which to get the specified system record.
           */

)
{

  sdsr_s_type                     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get specified system record.
  */
  sr_ptr = sdsr_list_get_ptr( list, pos);

  /* If specified system record is NULL, return now.
  */
  if( sr_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the mode of the system record is GW.
  */

  switch( mode )
  {
    case SD_MODE_AMPS:
    case SD_MODE_CDMA:
    case SD_MODE_HDR:
    case SD_MODE_GPS:
    case SD_MODE_WLAN:
    case SD_MODE_GSM:
    case SD_MODE_WCDMA:

      return( BOOLEAN(( sr_ptr->sys.mode == mode )));
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_GW:
      if( sr_ptr->sys.mode == SD_MODE_GSM || 
          sr_ptr->sys.mode == SD_MODE_WCDMA ||
          sr_ptr->sys.mode == SD_MODE_GW
        )
      {
        return TRUE;
      }

      return FALSE;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_MAX:
    case SD_MODE_NONE:
    case SD_MODE_INACT:
    default:
      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  }
}/* sdsr_is_sys */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
========================== SYSTEM RECORD COMPONENT===========================
=============================================================================
=============================================================================
===========================================================================*/


/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_component_init

DESCRIPTION
  Initialize the System Record Component.

  NOTE! this function must be called before any other sdss_xxx is called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void                      sdsr_component_init( void )
{

  SD_FUNC_START( "sdsr_component_init()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system record table.
  */
  sdsr_tbl_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initializes all the system record lists and items.
  */
  sdsr_lists_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR
#error code not present
  #endif /* FEATURE_HDR */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WLAN
#error code not present
  #endif

} /* sdsr_component_init */




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================= SYSTEM RECORD TEST ============================
=============================================================================
=============================================================================
===========================================================================*/

#if( defined(SD_DEBUG) && TG==T_PC &&!defined(_lint) )
#error code not present
#endif /* defined(SD_DEBUG) && TG==T_PC && !_lint */




