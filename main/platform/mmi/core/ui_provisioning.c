/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           U S E R   I N T E R F A C E   M A I N  M O D U L E

GENERAL DESCRIPTION
  This file contains functions related to verifying the provisioning information
  in NVRAM.

EXTERNALIZED FUNCTIONS

  ui_init_provisioning()
    initializes provisioning, including fixing NVRAM if the information is
    inconsistent.

  ui_check_provisioning()
    checks provisioning, once it's been initialized.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  Should be called from UI task after CM task and UI task have initialized.
  Also requires NV.

  Copyright (c) 1999 through 2006 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/ui_provisioning.c#14 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/09   kcp     corruption of nv.esn
11/14/06   cvs     change check_provisioning remoting strategy to prevent
                   deadlock on 7200
11/06/06   cvs     check_provisioning makes sure init_provisioning has finished.
11/06/06   jas     Reverting prior change (doesn't work)
10/02/06   cvs     ui_init_provisioning now runs from ui_check_provisioning
09/08/06   cvs     Fix compile warnings
08/11/06   joe     Default for prl_enabled no longer set on every phone reset.
07/12/06   cvs     Fix compile warnings
03/09/05   cvs     If phone number is not active, set #digits to 0.
02/22/05   jas     Updated featurization of CM calls with WLAN
02/17/05   jas     Updated CM calls for WLAN
01/06/05   cvs     Fix RVCT compiler warning if FEATURE_ACP isn't defined
11/08/04   CVS     Initial revision, moved functionality from coreapp.c
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"

#include "msg.h"
#include "err.h"

#include <string.h>

#include "uiutils.h"

#include "hw.h"

#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#include "cai.h"
#include "prl.h"
#endif


#ifndef FEATURE_UI_DUALPROC_APPS
/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

static boolean provisioning_status = FALSE;
static boolean provisioning_set = FALSE;

/* Provisioning defaults */

/* FM Min is min #0, CDMA Min is min #1 */
#define FMMIN 0
#define CDMAMIN 1

/* Default IMSI values */
#define IMSI_MCC_0 999       /* 000 */
#define IMSI_S1_0  16378855   /* 0000000 */
#define IMSI_S2_0  999       /* 000 */
#define IMSI_CLASS0_ADDR_NUM 0xFF
#define IMSI_11_12_0 99

#ifdef FEATURE_CDMA_800
#ifdef FEATURE_T53_RF
/* T53 Primary Paging Channel Defaults */
#define PCH_A_DEF 76            /* Primary Channel A Default */
#define PCH_B_DEF 76            /* Primary Channel B Default. T53 doesn't
                                   use B-side cellular, so just default it
                                   to be the same as the A-side. */

/* T53 Secondary Paging Channel Defaults */
#define SCH_A_DEF 872           /* Secondary Channel A Default */
#define SCH_B_DEF 872           /* Primary Channel B Default. T53 doesn't
                                   use B-side cellular, so just default it
                                   to be the same as the A-side. */
#else
/* Primary Paging Channel Defaults */
#define PCH_A_DEF 283           /* Primary Channel A Default */
#define PCH_B_DEF 384           /* Primary Channel B Default */

/* Secondary Paging Channel Defaults */
#define SCH_A_DEF 691           /* Secondary Channel A Default */
#define SCH_B_DEF 777           /* Secondary Channel B Default */

#endif /* FEATURE_T53_RF */
#endif /* FEATURE_CDMA_800 */

#if defined FEATURE_SSPR_ENHANCEMENTS
#define NID_DEFAULTS 65535
#endif

#ifdef FEATURE_CDMA_800

#ifdef FEATURE_T53_RF
/* Primary Paging Channel Defaults */
#define PCH_A_DEFAULT 76            /* Primary Channel A Default */
#define PCH_B_DEFAULT 76            /* Primary Channel B Default. T53 doesn't
                                       use B-side cellular, so just default it
                                       to be the same as the A-side. */

/* Secondary Paging Channel Defaults */
#define SCH_A_DEFAULT 872           /* Secondary Channel A Default */
#define SCH_B_DEFAULT 872           /* Secondary Channel B Default. T53 doesn't
                                       use B-side cellular, so just default it
                                       to be the same as the A-side. */
#else
/* Primary Paging Channel Defaults */
#define PCH_A_DEFAULT 283           /* Primary Channel A Default */
#define PCH_B_DEFAULT 384           /* Primary Channel B Default */

/* Secondary Paging Channel Defaults */
#define SCH_A_DEFAULT 691           /* Secondary Channel A Default */
#define SCH_B_DEFAULT 777           /* Secondary Channel B Default */
#endif /* FEATURE_T53_RF */

#endif /* FEATURE_CDMA_800 */





/* <EJECT> */
/*===========================================================================

FUNCTION UI_DEFAULT_IMSI_S1

DESCRIPTION
  This function computes the default IMSI_S1 value as per IS-683 section 3.1.

DEPENDENCIES
  None.

RETURN VALUE
  This function returns the encoded value of IMSI_S1 with the four least
  significant digits set to ESNp, converted directly from binary to decimal,
  modulo 10000.  The other digits are set to zero.

SIDE EFFECTS
  Reading ESN from NV will change structure nvi.

===========================================================================*/
static dword ui_default_imsi_s1 ( void )
{
  nv_item_type nvi;
  word zero;     /* Encoding of three zero digits */
  word fourth;   /* Fourth from last decimal digit of the ESN */
  word last3;    /* Last three decimal digits of the ESN */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ui_get_nv( NV_ESN_I, &nvi );
  /* Encode digits as per JSTD-008 section 2.3.1.1 */
  zero = 1110 - 111;
  last3 = (word)(nvi.esn.esn % 1000);
  last3 += ((last3 / 100) == 0) ? 1000 : 0;
  last3 += (((last3 % 100) / 10) == 0) ? 100 : 0;
  last3 += ((last3 % 10) == 0) ? 10 : 0;
  last3 -= 111;
  fourth = (word)((nvi.esn.esn % 10000) / 1000); /* In range 0-9 */
  if (fourth == 0)
  {
    fourth = 10;
  }

#ifdef FEATURE_UI_IMSI_ALL_0_OK
  return(IMSI_S1_0);
#else
  /* Concatenate results and return 24 bit value for imsi_s1 */
  /* Example: esn = 120406
  **      imsi_s1 = 000  0  406
  **  encodes to -> 999  10 395
  **       in hex = 3e7  a  18b
  **    in binary = 1111100111 1010 0110001011
  */
  return (((dword) zero << 14) | (fourth << 10) | last3);  
#endif

} /* ui_default_imsi_s1() */

/*===========================================================================

FUNCTION UI_INITIALIZE_IMSI_COMPONENTS

DESCRIPTION
  This function computes the default for all IMSI components, and then
  writes them to NV. ACCOLC is also drived from the last digit of the
  IMSI.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ui_initialize_imsi_components ( byte nam )
{
  nv_item_type nvi;
#if !defined(FEATURE_WCDMA) && !defined(FEATURE_GSM)
  byte last_esn_digit;
#endif
  /* Default MCC is 000 in ASCII and 999 in min */
  nvi.imsi_mcc.nam = nam;
  nvi.imsi_mcc.imsi_mcc = IMSI_MCC_0;      /* default to '000' */
  (void) ui_put_nv( NV_IMSI_MCC_I, &nvi );

  /* Default MNC is 00 in ASCII and 99 in min */
  nvi.imsi_11_12.nam = nam;
  nvi.imsi_11_12.imsi_11_12 = IMSI_11_12_0;  /* default to '00' */
  (void) ui_put_nv( NV_IMSI_11_12_I, &nvi );

  nvi.imsi_addr_num.nam = nam;
  nvi.imsi_addr_num.num = IMSI_CLASS0_ADDR_NUM ;
  (void) ui_put_nv( NV_IMSI_ADDR_NUM_I, &nvi ) ;

  nvi.min2.nam = nam;
  nvi.min2.min2[FMMIN] = IMSI_S2_0 ;
  nvi.min2.min2[CDMAMIN] = IMSI_S2_0 ;
  (void) ui_put_nv( NV_MIN2_I, &nvi ) ;
 
  nvi.min1.min1[CDMAMIN] =  ui_default_imsi_s1() ;
  
  /* The following assignment must not be done, before the function above */
  /* because that function will read ESN which will corrupt the min1.nam. */
  
  nvi.min1.nam = nam;
  nvi.min1.min1[FMMIN] = IMSI_S1_0 ;
  (void) ui_put_nv( NV_MIN1_I, &nvi ) ;   


  (void) ui_get_nv( NV_ESN_I, &nvi ) ;

#if !defined(FEATURE_WCDMA) && !defined(FEATURE_GSM)
  last_esn_digit = (byte) (nvi.esn.esn % 10) ;
  nvi.accolc.ACCOLCpClass[CDMAMIN] = last_esn_digit ;
  nvi.accolc.ACCOLCpClass[FMMIN] = last_esn_digit ;
#endif

  nvi.accolc.nam = nam ;
  (void) ui_put_nv( NV_ACCOLC_I, &nvi ) ;
}

#ifdef FEATURE_IS95B_TRUE_IMSI

/* <EJECT> */
/*===========================================================================

FUNCTION UI_DEFAULT_IMSI_T_S1

DESCRIPTION
  This function computes the default IMSI_T_S1 value as per IS-683 section 3.1.

DEPENDENCIES
  None.

RETURN VALUE
  This function returns the encoded value of IMSI_T_S1 with the four least
  significant digits set to ESNp, converted directly from binary to decimal,
  modulo 10000.  The other digits are set to zero.

SIDE EFFECTS
  Reading ESN from NV will change structure nvi.

===========================================================================*/
dword ui_default_imsi_t_s1 ( void )
{
  word zero;     /* Encoding of three zero digits */
  word fourth;   /* Fourth from last decimal digit of the ESN */
  word last3;    /* Last three decimal digits of the ESN */
  nv_item_type nvi;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ui_get_nv( NV_ESN_I, &nvi );
  /* Encode digits as per JSTD-008 section 2.3.1.1 */
  zero = 1110 - 111;
  last3 = (nvi.esn.esn % 1000);
  last3 += ((last3 / 100) == 0) ? 1000 : 0;
  last3 += (((last3 % 100) / 10) == 0) ? 100 : 0;
  last3 += ((last3 % 10) == 0) ? 10 : 0;
  last3 -= 111;
  fourth = ((nvi.esn.esn % 10000) / 1000); /* In range 0-9 */
  if (fourth == 0)
  {
    fourth = 10;
  }

  /* Concatenate results and return 24 bit value for imsi_s1 */
  /* Example: esn = 120406
  **      imsi_s1 = 000  0  406
  **  encodes to -> 999  10 395
  **       in hex = 3e7  a  18b
  **    in binary = 1111100111 1010 0110001011
  */
  return (((dword) zero << 14) | (fourth << 10) | last3);

} /* ui_default_imsi_t_s1() */


/*===========================================================================

FUNCTION UI_INITIALIZE_IMSI_T_COMPONENTS

DESCRIPTION
  This function computes the default for all TRUE IMSI components, and then
  writes them to NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void ui_initialize_imsi_t_components ( byte nam )
{
  nv_item_type nvi;

  /* Default MCC is 000 in ASCII and 999 in min */
  nvi.imsi_t_mcc.nam = nam;
  nvi.imsi_t_mcc.imsi_mcc = IMSI_MCC_0;      /* default to '000' */
  (void) ui_put_nv( NV_IMSI_T_MCC_I, &nvi );

  /* Default MNC is 00 in ASCII and 99 in min */
  nvi.imsi_t_11_12.nam = nam;
  nvi.imsi_t_11_12.imsi_11_12 = IMSI_11_12_0;  /* default to '00' */
  (void) ui_put_nv( NV_IMSI_T_11_12_I, &nvi );

  nvi.imsi_t_addr_num.nam = nam;
  nvi.imsi_t_addr_num.num = IMSI_CLASS0_ADDR_NUM ;
  (void) ui_put_nv( NV_IMSI_T_ADDR_NUM_I, &nvi ) ;

  /* Default S2 is 000 in ASCII and 999 in min */
  nvi.imsi_t_s2.nam = nam;
  nvi.imsi_t_s2.min2[FMMIN] = IMSI_S2_0 ;
  nvi.imsi_t_s2.min2[CDMAMIN] = IMSI_S2_0 ;
  (void) ui_put_nv( NV_IMSI_T_S2_I, &nvi ) ;

#ifdef  FEATURE_IS95B_TRUE_IMSI
  nvi.imsi_t_s1.min1[CDMAMIN] =  ui_default_imsi_t_s1() ;
#endif
  /* The following assignment must not be done, before the function above */
  /* because that function will read ESN which will corrupt the min1.nam. */
  nvi.imsi_t_s1.nam = nam;
  nvi.imsi_t_s1.min1[FMMIN] = IMSI_S1_0 ;
  (void) ui_put_nv( NV_IMSI_T_S1_I, &nvi ) ;

}
#endif //FEATURE_IS95B_TRUE_IMSI

#if defined (FEATURE_SSPR_ENHANCEMENTS) && defined (FEATURE_CDMA_800)
/* <EJECT> */
/*===========================================================================

FUNCTION IS_CELLULAR_SYS_A_CHANNEL

DESCRIPTION
  Check if the CDMA channel is with Cellular Band System A.

DEPENDENCIES
  None

RETURN VALUE
  boolean: TRUE - if the channel is within Cellular Band System A
           FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static boolean is_cellular_sys_a_channel
(
  word cdma_channel       /* The channel to examine */
)
{
#ifdef FEATURE_T53_RF

  /* According to T53 Section 6.1.1.1, the CDMA channel must be even, and
   * be in the range of 51-749, 851-989, or 1091-1149.
   */
  if (((cdma_channel & 0x0001) == 0) &&
     (((cdma_channel > 51)   && (cdma_channel <  749))  ||
      ((cdma_channel > 851)  && (cdma_channel <  989))  ||
      ((cdma_channel > 1091) && (cdma_channel < 1149))))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

#else

    return (PRL_IS_CHAN_IN_CELLULAR_SYS_A(cdma_channel)); /* Sys A */

#endif /* FEATURE_T53_RF */
} /* is_cellular_sys_a_channel */

/* <EJECT> */
/*===========================================================================

FUNCTION IS_CELLULAR_SYS_B_CHANNEL


DESCRIPTION
  Check if the CDMA channel is with Cellular Band System B

DEPENDENCIES
  None

RETURN VALUE
  boolean: TRUE - if the channel is within Cellular Band System B
           FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static boolean is_cellular_sys_b_channel
(
  word cdma_channel       /* The channel to examine */
)
{
#ifdef FEATURE_T53_RF

  /* T53 doesn't use B-side cellular, so return FALSE */
  return FALSE;

#else

    return (PRL_IS_CHAN_IN_CELLULAR_SYS_B(cdma_channel)); /* Sys B */

#endif /* FEATURE_T53_RF */
} /* is_cellular_sys_b_channel */
#endif //defined (FEATURE_SSPR_ENHANCEMENTS) && defined (FEATURE_CDMA_800)


/*=============================================================================
FUNCTION: ui_provisioning_err_cb

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/

void ui_provisioning_err_cb(void *data_block_ptr,
                         cm_ph_cmd_e_type ph_cmd,
                         cm_ph_cmd_err_e_type ph_cmd_err)
{
  if (ph_cmd_err == CM_PH_CMD_ERR_NOERR)
    MSG_MED("ui_provisioning_err_cb received result 0x%X for Phone command %d",
                                    ph_cmd_err, ph_cmd, 0);
  else
    MSG_ERROR("ui_provisioning_err_cb received result 0x%X for Phone command %d",
                                    ph_cmd_err, ph_cmd, 0);
}

/*===========================================================================
FUNCTION ui_init_provisioning

DESCRIPTION
  initialize provisioning

DEPENDENCIES
  the corresponding initialization functions

SIDE EFFECTS
  None
===========================================================================*/
void ui_init_provisioning(cm_client_id_type cm_client_id)
{
  boolean good=TRUE, goodnam=TRUE;
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  nv_item_type nvi;
  int i,j;
  uint8 nam;
  nv_mob_dir_data_type nv_dir_num;

  boolean hascdma[NV_MAX_NAMS];
#ifdef FEATURE_ACP
  hasfm[NV_MAX_NAMS];
#endif
  boolean hasacdma[NV_MAX_NAMS], hasbcdma[NV_MAX_NAMS];

  /* use these flags to treat all IMSI components as one */
  boolean initialize_imsi[NV_MAX_NAMS] ;
  boolean imsi_all_zeros[NV_MAX_NAMS] ;

  #ifdef FEATURE_IS95B_TRUE_IMSI
  boolean initialize_imsi_t[NV_MAX_NAMS];
  boolean imsi_t_all_zeros[NV_MAX_NAMS];
  #endif

  MSG_FATAL("***zzg ui_init_provisioning start***", 0, 0, 0);
  
  for (i=0; i<NV_MAX_NAMS; i++)
  {
    initialize_imsi[i] = FALSE ; /* assume no need */
    imsi_all_zeros[i] = TRUE ;   /* assume true    */
  }

#ifdef FEATURE_IS95B_TRUE_IMSI
  for (i=0; i<NV_MAX_NAMS; i++)
  {
    initialize_imsi_t[i] = FALSE ; /* assume no need */
    imsi_t_all_zeros[i] = TRUE ;   /* assume true    */
  }
#endif // FEATURE_IS95B_TRUE_IMSI

  (void) ui_get_nv( NV_ESN_CHKSUM_I, &nvi );
  if( nvi.esn_chksum.chksum != NV_VALID_ESN_CHKSUM ) {
       MSG_HIGH("BAD ESN CHECKSUM",0,0,0);
       good=FALSE;
  }

  /* Get all the NAM/MIN information that we need */
  (void) ui_get_nv( NV_CURR_NAM_I, &nvi ); /* get current nam */
  /* If the NAM is out of range, default to the first NAM */
  if ( nvi.curr_nam >= NV_MAX_NAMS ) {
     nvi.curr_nam = 0;
    (void) ui_put_nv ( NV_CURR_NAM_I, &nvi );
  }

  nam = nvi.curr_nam;

  for( i=0; i<NV_MAX_NAMS; i++ ) {

     hascdma[i] = TRUE; /* Assume the NAM is good for CDMA */
#ifdef FEATURE_ACP
     hasfm[i] = TRUE;  /* Assume NAM is good for AMPS */
#endif /* FEATURE_ACP */
#ifdef FEATURE_ACP
     /*********************************/
     /* Determine if we have valid FM */
     /*********************************/
     nvi.analog_home_sid.nam = (byte) i; /* has analog if home_sid != 0 */
     /* if inactive or has invalid range ( i.e > 32767 */
     if ( NV_NOTACTIVE_S == ui_get_nv( NV_ANALOG_HOME_SID_I, &nvi ) ||
         nvi.analog_home_sid.sid &0x8000 )
     {
         /* then initialize it */
         nvi.analog_home_sid.sid = 0 ;
         (void) ui_put_nv( NV_ANALOG_HOME_SID_I, &nvi );
      }
#endif /* FEATURE_ACP */

#ifdef FEATURE_CDMA_800
      /***********************************/
      /* Determine if we have valid CDMA */
      /***********************************/
      nvi.pcdmach.nam = (byte) i;  /* has CDMA if any of the 8 channels set */

      {
         boolean need_nv_save = FALSE;
         if ( NV_NOTACTIVE_S == ui_get_nv( NV_PCDMACH_I, &nvi ) )
         {
           nvi.pcdmach.nam = i;
           nvi.pcdmach.channel_a = PCH_A_DEFAULT;
           nvi.pcdmach.channel_b = PCH_B_DEFAULT;
           need_nv_save = TRUE;
         }
#ifdef FEATURE_SSPR_ENHANCEMENTS
         else
         {
           if (!is_cellular_sys_a_channel(nvi.pcdmach.channel_a)) {
             /* Channel A number is invalid, default it */
              nvi.pcdmach.channel_a = PCH_A_DEF;
              need_nv_save = TRUE;
           }

           if (!is_cellular_sys_b_channel(nvi.pcdmach.channel_b)) {
             /* Channel B number is invalid, default it */
             nvi.pcdmach.channel_b = PCH_B_DEF;
             need_nv_save = TRUE;
           }
         }
#endif
         /* If any of the value changed, update NV */
         if (need_nv_save) {
            (void) ui_put_nv( NV_PCDMACH_I, &nvi );
         }
      }

      hasacdma[i] = (boolean) nvi.pcdmach.channel_a;
      hasbcdma[i] = (boolean) nvi.pcdmach.channel_b;

      nvi.scdmach.nam = (byte) i;
      {
          boolean need_nv_save = FALSE;
          if ( NV_NOTACTIVE_S == ui_get_nv( NV_SCDMACH_I, &nvi ) )
          {
             nvi.scdmach.nam = i;
             nvi.scdmach.channel_a = SCH_A_DEFAULT;
             nvi.scdmach.channel_b = SCH_B_DEFAULT;
             need_nv_save = TRUE;
          }
#ifdef FEATURE_SSPR_ENHANCEMENTS
          else {
            if (!is_cellular_sys_a_channel(nvi.scdmach.channel_a)) {
               nvi.scdmach.channel_a = SCH_A_DEF;
               need_nv_save = TRUE;
            }

            if (!is_cellular_sys_b_channel(nvi.scdmach.channel_b)) {
               nvi.scdmach.channel_b = SCH_B_DEF;
               need_nv_save = TRUE;
            }
          }
#endif
          if (need_nv_save) {
            (void) ui_put_nv( NV_SCDMACH_I, &nvi );
          }
      }

      if (hasacdma[i]) {
         hasacdma[i] = (boolean) nvi.scdmach.channel_a;
      }
      if (hasbcdma[i]) {
         hasbcdma[i] = (boolean) nvi.scdmach.channel_b;
      }

      hascdma[i] = hasacdma[i] | hasbcdma[i];

#endif /* FEATURE_CDMA_800 */

#if defined(FEATURE_SSPR_ENHANCEMENTS)

      {
          byte indx;
          boolean need_nv_save = FALSE;

          /**********************************/
          /* Check for a valid SID/NID List */
          /**********************************/
          nvi.home_sid_nid.nam = (byte) i;
          /* Is it active and in valid range */
          if ( NV_NOTACTIVE_S == ui_get_nv( NV_HOME_SID_NID_I, &nvi ) )
          {
             for( indx=0; indx<NV_MAX_HOME_SID_NID; indx++)
             {
                /*  NID default */
                nvi.home_sid_nid.pair[indx].sid = 0;
                nvi.home_sid_nid.pair[indx].nid = NID_DEFAULTS ;
             }
             need_nv_save = TRUE;
          }
          else
          {
            for( indx=0; indx<NV_MAX_HOME_SID_NID; indx++)
            {
               /* Is range valid ? */
               if ( nvi.home_sid_nid.pair[indx].sid & 0x8000 )
               {
                  /*  NID default */
                  nvi.home_sid_nid.pair[indx].sid = 0;
                  nvi.home_sid_nid.pair[indx].nid = NID_DEFAULTS ;
                  need_nv_save = TRUE;
               }
            }
          }
          if (need_nv_save) {
              //(void) ui_put_nv( NV_HOME_SID_NID_I, &nvi );
          }

#if (defined (FEATURE_CDMA_800) && defined (FEATURE_CDMA_1900) )
          /* SID/NID Lockout not allowed for TriMode */
#else

          /******************************************/
          /* Check for a valid Lockout SID/NID List */
          /******************************************/

          nvi.sid_nid_lock.nam = (byte) i;
          need_nv_save = FALSE;
          /* Is it active and in valid range */
          if ( NV_NOTACTIVE_S == ui_get_nv( NV_SID_NID_LOCK_I, &nvi ) )
          {
            for( indx=0; indx<NV_MAX_SID_NID_LOCK; indx++)
            {
                /*  NID default */
                nvi.sid_nid_lock.pair[indx].sid = 0;
                nvi.sid_nid_lock.pair[indx].nid = NID_DEFAULTS ;
            }
            need_nv_save = TRUE;
          }
          else
          {
            for( indx=0; indx<NV_MAX_SID_NID_LOCK; indx++)
            {
               /* Is range valid ? */
               if ( nvi.sid_nid_lock.pair[indx].sid & 0x8000 )
               {
                   /*  NID default */
                   nvi.sid_nid_lock.pair[indx].sid = 0;
                   nvi.sid_nid_lock.pair[indx].nid = NID_DEFAULTS ;
                   need_nv_save = TRUE;
               }
            }
          }
          if (need_nv_save) {
            //(void) ui_put_nv( NV_SID_NID_LOCK_I, &nvi );
          }
#endif /* TriMode */
      }
#else

      {
          int sid, issid;
          boolean need_nv_save = FALSE;

          /*************************/
          /* Check for a valid SID */
          /*************************/
          nvi.sid_nid.nam = (byte) i;
          /* Is it active and in valid range */
          if ( NV_NOTACTIVE_S == ui_get_nv( NV_SID_NID_I, &nvi ) )
          {
            for( sid=0; sid<NV_MAX_SID_NID; sid++)
            {
              /*  NID default */
              nvi.sid_nid.pair[NV_CDMA_MIN_INDEX][sid].sid = 0;
              nvi.sid_nid.pair[NV_CDMA_MIN_INDEX][sid].nid = NID_DEFAULTS ;
            }
            need_nv_save = TRUE;
          }
          else
          {
            for( sid=0; sid<NV_MAX_SID_NID; sid++)
            {
               /* Is range valid ? */
               if ( nvi.sid_nid.pair[NV_CDMA_MIN_INDEX][i].sid & 0x8000 )
               {
                   /*  NID default */
                   nvi.sid_nid.pair[NV_CDMA_MIN_INDEX][sid].sid = 0;
                   nvi.sid_nid.pair[NV_CDMA_MIN_INDEX][sid].nid = NID_DEFAULTS ;
                   need_nv_save = TRUE;
               }
            }
          }
          if (need_nv_save) {
              (void) ui_put_nv( NV_SID_NID_I, &nvi );
          }
          issid = 0;
          for( sid=0; sid<NV_MAX_SID_NID; sid++ ) {
            issid |= (int)nvi.sid_nid.pair[NV_CDMA_MIN_INDEX][sid].sid;
          }
      }

#if defined(FEATURE_CDMA_800)
      /******************************************/
      /* Check for a valid Lockout SID List */
      /******************************************/

      {
           byte indx;
           boolean need_nv_save = FALSE;

           nvi.cdma_sid_lock.nam = (byte) i;
           /* Is it active and in valid range */
           if ( NV_NOTACTIVE_S == ui_get_nv( NV_CDMA_SID_LOCK_I, &nvi ) )
           {
              for( indx=0; indx<NV_MAX_SID_LOCK; indx++)
              {
                /*  default */
                nvi.cdma_sid_lock.sid[indx] = 0;
              }
              need_nv_save = TRUE;
           }
           else
           {
              for( indx=0; indx<NV_MAX_SID_NID; indx++)
              {
                 /* Is range valid ? */
                 if ( nvi.cdma_sid_lock.sid[indx] & 0x8000 )
                 {
                    /*  default */
                    nvi.cdma_sid_lock.sid[indx] = 0;
                    need_nv_save = TRUE;
                 }
              }
           }
           if (need_nv_save) {
              (void) ui_put_nv( NV_CDMA_SID_LOCK_I, &nvi );
           }
      }

#endif /* FEATURE_CDMA_800 */

#endif /* FEATURE_SSPR_ENHANCEMENTS */

#ifdef FEATURE_SSPR_800
       /******************************************/
       /* Check for a valid PRL Enabled          */
       /******************************************/
       nvi.prl_enabled.nam = (byte) i;

       /* Is it active and in valid range */
       if ( NV_NOTACTIVE_S == ui_get_nv( NV_PRL_ENABLED_I, &nvi ) )
       {
#if (defined (FEATURE_CDMA_800) && defined (FEATURE_CDMA_1900) )
         nvi.prl_enabled.enabled = TRUE;
#else
         nvi.prl_enabled.enabled = FALSE;
#endif /* TriMode */
         (void) ui_put_nv( NV_PRL_ENABLED_I, &nvi );
       }

#endif /* FEATURE_SSPR_800 */

#if (defined (FEATURE_CDMA_800) && defined (FEATURE_CDMA_1900) )
       /*--- It is done in ID_S minor state ---*/
#else
#if defined( FEATURE_ACP ) && defined( FEATURE_CDMA_800 )
       if ( !hascdma[i] ) {
          /*
          ** There is NO valid CDMA. In this case we default the mode
          ** to AUTOMATIC.  Note in this, phone goes offline.
          */
         sys_plmn_id_s_type plmn;
         (void) MEMSET(&plmn, 0xFF, sizeof(plmn));


         cm_ph_cmd_sys_sel_pref(
                                ui_provisioning_err_cb,
                                NULL,
                                cm_client_id,
                                (cm_mode_pref_e_type) CM_MODE_PREF_AUTOMATIC,
                                (cm_pref_term_e_type) CM_PREF_TERM_PERMANENT,
                                (dword) 0,
                                (cm_gw_acq_order_pref_e_type) CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                (cm_band_pref_e_type) CM_BAND_PREF_NO_CHANGE,
                                (cm_roam_pref_e_type) CM_ROAM_PREF_NO_CHANGE,
                                OEMCM_HybrPrefToDMSS(CM_HYBR_PREF_NO_CHANGE),
                                (cm_srv_domain_pref_e_type) CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                (cm_network_sel_mode_pref_e_type) CM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                &plmn
#ifdef CM_API_WLAN
                                , NULL
#endif
                                );

        } else {
          /*
          ** There is a valid CDMA. In this case we default the mode to
          ** DIGITAL_ONLY if there is NO valid FM.
          */
          if( !hasfm[i] ) {
            sys_plmn_id_s_type plmn;
            (void) MEMSET(&plmn, 0xFF, sizeof(plmn));

            cm_ph_cmd_sys_sel_pref(
                                   ui_provisioning_err_cb,
                                   NULL,
                                   cm_client_id,
                                   (cm_mode_pref_e_type) CM_MODE_PREF_DIGITAL_ONLY,
                                   (cm_pref_term_e_type) CM_PREF_TERM_PERMANENT,
                                   (dword) 0,
                                   (cm_gw_acq_order_pref_e_type) CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                   (cm_band_pref_e_type) CM_BAND_PREF_NO_CHANGE,
                                   (cm_roam_pref_e_type) CM_ROAM_PREF_NO_CHANGE,
                                   OEMCM_HybrPrefToDMSS(CM_HYBR_PREF_NO_CHANGE),
                                   (cm_srv_domain_pref_e_type) CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                   (cm_network_sel_mode_pref_e_type) CM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                   &plmn
#ifdef CM_API_WLAN
                                   , NULL
#endif
                                   );
          }
        } /* ui.hascdma[i] */


#ifndef FEATURE_SSPR_ENHANCEMENTS
        nvi.cdma_pref_serv.nam = i ;
        /* is it active and in valid range ? */
        if ( NV_NOTACTIVE_S == ui_get_nv( NV_CDMA_PREF_SERV_I, &nvi ) ||
               nvi.cdma_pref_serv.sys > NV_SYS_HOME_PREF )
        {
            /* no, so default it */
            nvi.cdma_pref_serv.nam = i ;
            nvi.cdma_pref_serv.sys = NV_SYS_HOME_PREF;
            (void) ui_put_nv( NV_CDMA_PREF_SERV_I, &nvi );
        }

        nvi.analog_pref_serv.nam = i ;
        /* is it active and in valid range ? */
        if ( NV_NOTACTIVE_S == ui_get_nv( NV_ANALOG_PREF_SERV_I, &nvi ) ||
               nvi.analog_pref_serv.sys > NV_SYS_HOME_PREF )
        {
            /* no, so default it */
            nvi.analog_pref_serv.nam = i ;
            nvi.analog_pref_serv.sys = NV_SYS_HOME_PREF;
            (void) ui_put_nv( NV_ANALOG_PREF_SERV_I, &nvi );
        }
#endif /* FEATURE_SSPR_ENHANCEMENTS */

#endif /* defined( FEATURE_ACP ) && defined( FEATURE_CDMA_800 ) */

#endif /* (defined (FEATURE_CDMA_800) && defined (FEATURE_CDMA_1900) ) */

          /**********************************************/
          /* Make sure MCC and IMSI_11_12 are set       */
          /**********************************************/

		MSG_FATAL("***zzg ui_init_provisioning 1 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);

        nvi.imsi_mcc.nam = i;
        if ( NV_NOTACTIVE_S == ui_get_nv( NV_IMSI_MCC_I, &nvi ) )
        {
           initialize_imsi[i] = TRUE ;
        }
        else
        {
          if ( nvi.imsi_mcc.imsi_mcc > IMSI_MCC_0 )
          {
              initialize_imsi[i] = TRUE ; /* invalid range */
          }
          if ( nvi.imsi_mcc.imsi_mcc != IMSI_MCC_0 )
          {
              imsi_all_zeros[i] = FALSE ;
          }
        }

		MSG_FATAL("***zzg ui_init_provisioning 2 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);

        nvi.imsi_11_12.nam = i;
        if ( NV_NOTACTIVE_S == ui_get_nv( NV_IMSI_11_12_I, &nvi ) )
        {
            initialize_imsi[i] = TRUE ;
        }
        else
        {
            if ( nvi.imsi_11_12.imsi_11_12 > IMSI_11_12_0 )
            {
              initialize_imsi[i] = TRUE ; /* invalid range */
            }
            if ( nvi.imsi_11_12.imsi_11_12 != IMSI_11_12_0 )
            {
              imsi_all_zeros[i] = FALSE ;
            }
        }

		MSG_FATAL("***zzg ui_init_provisioning 3 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);

        nvi.imsi_addr_num.nam = i;
        if ( NV_NOTACTIVE_S == ui_get_nv( NV_IMSI_ADDR_NUM_I, &nvi ) )
        {
            initialize_imsi[i] = TRUE ;
        }

#ifdef FEATURE_IS95B_TRUE_IMSI

        /*  Add five TRUE NV items checking
        **               IMSI_T_S1_I,
        **               IMSI_T_S2_I,
        **               IMSI_T_MCC_I,
        **               IMSI_T_11_12_I,
        **               IMSI_T_ADDR_NUM_I
        **
        */

        MSG_MED ("Reading NV_IMSI_T_S1_I", 0, 0, 0);

        /* Loop through the number of NAMs-1 and read the IMSI_T_S1 for each NAM */
        /* (NOTE:  We are looping to queue NV reads via uinv_access() with
        ** a NULL as the callback, then we queue the last NV read for the
        ** last NAM with this same routine (uicall_strtup_checking ()) as
        ** the callback */

        /*************************/
        /* Read the IMSI_T_S1 from NV */
        /*************************/

        /* Get and validate NV_IMSI_T_S1_I code goes here */

		MSG_FATAL("***zzg ui_init_provisioning 4 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);


        nvi.imsi_t_s1.nam = (byte) i;     /* get the MINs for the NAM */
        if ( NV_NOTACTIVE_S == ui_get_nv( NV_IMSI_T_S1_I, &nvi ) )
        {
            initialize_imsi_t[i] = TRUE ;
        }
        else
        {
              /* Since it is valid for only CDMA, only CDMA MIN is checked. FMMIN is ignored */
              if ( ( ( nvi.imsi_t_s1.min1[CDMAMIN] & 0xFF000000 ) != 0 ) ||
                   ( ( ( nvi.imsi_t_s1.min1[CDMAMIN] & 0x00FFC000 ) >> 14 ) > 999 ) ||
                   ( ( ( nvi.imsi_t_s1.min1[CDMAMIN] & 0x00003C00 ) >> 10 ) > 10 ) ||
                   ( ( ( nvi.imsi_t_s1.min1[CDMAMIN] & 0x00003C00 ) >> 10 ) == 0 ) ||
                   ( ( ( nvi.imsi_t_s1.min1[CDMAMIN] & 0x000003FF ) > 999 ) ) )
              {
                /* Invalid Range See IS-95A section 6.3.1 */
                initialize_imsi_t[i] = TRUE ;
              }
              if ( nvi.imsi_t_s1.min1[CDMAMIN] != IMSI_S1_0 )
              {
                imsi_t_all_zeros[i] = FALSE ;
              }
        }

		MSG_FATAL("***zzg ui_init_provisioning 5 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);


        nvi.imsi_t_s2.nam = (byte) i;     /* get the MINs for the NAM */
        if ( NV_NOTACTIVE_S == ui_get_nv( NV_IMSI_T_S2_I, &nvi ) )
        {
            initialize_imsi_t[i] = TRUE ;
        }
        else
        {
              /* Since it is valid for only CDMA, only CDMA MIN is checked. FMMIN is ignored */
              if ( nvi.imsi_t_s2.min2[CDMAMIN] > IMSI_S2_0 )
              {
                /* invalid range */
                initialize_imsi_t[i] = TRUE ;
              }
              if ( nvi.imsi_t_s2.min2[CDMAMIN] != IMSI_S2_0 )
              {
                imsi_t_all_zeros[i] = FALSE ;
              }
        }

        MSG_FATAL("***zzg ui_init_provisioning 6 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);


        nvi.imsi_t_11_12.nam = (byte) i;     /* get the MINs for the NAM */
        if ( NV_NOTACTIVE_S == ui_get_nv( NV_IMSI_T_11_12_I, &nvi ) )
        {
            initialize_imsi_t[i] = TRUE ;
        }
        else
        {
            if ( nvi.imsi_t_11_12.imsi_11_12 > IMSI_11_12_0 )
            {
              initialize_imsi_t[i] = TRUE ; /* invalid range */
            }
            if ( nvi.imsi_t_11_12.imsi_11_12 != IMSI_11_12_0 )
            {
              imsi_t_all_zeros[i] = FALSE ;
            }
        }

		MSG_FATAL("***zzg ui_init_provisioning 7 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);


        nvi.imsi_t_mcc.nam = (byte) i;     /* get the MINs for the NAM */
        if ( NV_NOTACTIVE_S == ui_get_nv( NV_IMSI_T_MCC_I, &nvi ) )
        {
            initialize_imsi_t[i] = TRUE ;
        }
        else
        {
            if ( nvi.imsi_t_mcc.imsi_mcc > IMSI_MCC_0 )
            {
              initialize_imsi_t[i] = TRUE ; /* invalid range */
            }
            if ( nvi.imsi_t_mcc.imsi_mcc != IMSI_MCC_0 )
            {
              imsi_t_all_zeros[i] = FALSE ;
            }
        }

		MSG_FATAL("***zzg ui_init_provisioning 8 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);

        nvi.imsi_t_addr_num.nam = (byte) i;     /* get the MINs for the NAM */
        if ( NV_NOTACTIVE_S == ui_get_nv( NV_IMSI_T_ADDR_NUM_I, &nvi ) )
        {
            initialize_imsi_t[i] = TRUE ;
        }
        else
        if (!initialize_imsi_t[i])
        {
            /* Check if the IMSI_T_ADDR_NUM is invalid */
            if ((nvi.imsi_t_addr_num.num > 7) &&
               (nvi.imsi_t_addr_num.num != 0xFF))
            {
                ERR ("Invalid IMSI_T_ADDR_NUM for NAM %d", i, 0, 0);

                /* Set the flag that we need to default this NAM's IMSI data */
                initialize_imsi_t[i] = TRUE;
            }
        }

#endif /*** End of FEATURE_IS95B_TRUE_IMSI ***/

		MSG_FATAL("***zzg ui_init_provisioning 9 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);


        /**********************************/
        /* Read out the MINs for each NAM */
        /**********************************/
        nvi.min1.nam = (byte) i;     /* get the MINs for the NAM */
        if ( NV_NOTACTIVE_S == ui_get_nv( NV_MIN1_I, &nvi ) )
        {
        	MSG_FATAL("***zzg ui_init_provisioning 10 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);
            initialize_imsi[i] = TRUE ;
        }
        else
        {
            /* Loop through the number of MINs */
            for (j = 0; j < NV_MAX_MINS; j++)
            {
              if ( ( ( nvi.min1.min1[j] & 0xFF000000 ) != 0 ) ||
                   ( ( ( nvi.min1.min1[j] & 0x00FFC000 ) >> 14 ) > 999 ) ||
                   ( ( ( nvi.min1.min1[j] & 0x00003C00 ) >> 10 ) > 10 ) ||
                   ( ( ( nvi.min1.min1[j] & 0x00003C00 ) >> 10 ) == 0 ) ||
                   ( ( ( nvi.min1.min1[j] & 0x000003FF ) > 999 ) ) )
              {
                /* Invalid Range See IS-95A section 6.3.1 */
				MSG_FATAL("***zzg ui_init_provisioning 11 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);
                initialize_imsi[i] = TRUE ;
              }
              if ( nvi.min1.min1[j] != IMSI_S1_0 )
              {
                imsi_all_zeros[i] = FALSE ;
              }
            } /* end for (j = 0; j < NV_MAX_MINS; j++) */
        }

		MSG_FATAL("***zzg ui_init_provisioning 12 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);

		MSG_FATAL("***zzg ui_init min1[0]=%d, min1[1]=%d***", nvi.min1.min1[0], nvi.min1.min1[1], 0);

        //for( j=0; j<NV_MAX_MINS; j++ ) {
        //    ui.min1[i][j] = nvi.min1.min1[j];
        //}

        nvi.min2.nam = (byte) i;
        if ( NV_NOTACTIVE_S == ui_get_nv( NV_MIN2_I, &nvi ) )
        {
        	MSG_FATAL("***zzg ui_init_provisioning 13 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);
            initialize_imsi[i] = TRUE ;
        }
        else
        {
            /* Loop through each MIN */
            for (j = 0; j < NV_MAX_MINS; j++)
            {
              if ( nvi.min2.min2[j] > IMSI_S2_0 )
              {
                /* invalid range */
				MSG_FATAL("***zzg ui_init_provisioning 14 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);
                initialize_imsi[i] = TRUE ;
              }
              if ( nvi.min2.min2[j] != IMSI_S2_0 )
              {
                imsi_all_zeros[i] = FALSE ;
              }
            } /* end for (j = 0; j < NV_MAX_MINS; j++) */
        }

		MSG_FATAL("***zzg ui_init_provisioning 15 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);

        //for( j=0; j<NV_MAX_MINS; j++ ) {
        //    ui.min2[i][j] = nvi.min2.min2[j];
        //}

    } /* for ( i=0; i<NV_MAX_NAMS; i++ ) */

    /*****************************************************************/
    /* By this point all imsi components are read from NV. The flags */
    /* are set indicating if any one of the components were inactive */
    /* or if the whole IMSI was all zeros. In either case, we must   */
    /* initialize all components of IMSI to zero, except the last    */
    /* four digits of MIN1 will be taken from the ESN                */
    /*****************************************************************/

	MSG_FATAL("***zzg ui_init_provisioning 16 i=%d, initialize_imsi=%d, imsi_all_zeros=%d***", i, initialize_imsi[i], imsi_all_zeros[i]);
	
    for ( i=0; i<NV_MAX_NAMS; i++ )
    {
          if ( initialize_imsi[i] == TRUE || imsi_all_zeros[i] == TRUE )
          {
            ui_initialize_imsi_components ( (byte)i ) ;
          }
    }

#ifdef FEATURE_IS95B_TRUE_IMSI

    /* HHD check all items */
    for ( i=0; i<NV_MAX_NAMS; i++ )
    {
          if ( initialize_imsi_t[i] == TRUE || imsi_t_all_zeros[i] == TRUE )
          {
            ui_initialize_imsi_t_components ( (byte)i ) ;
          }
    }
#endif /* END of FEATURE_IS95B_TRUE_IMSI */

#ifdef FEATURE_CDMA_800
    /***********************************/
    /* Check if CDMA is valid or not ? */
    /***********************************/
    if( !hascdma[nam] ) {
          goodnam = FALSE;
          MSG_HIGH( "Bad NAM!!!", 0, 0, 0 );
    }
#endif /* FEATURE_CDMA_800 */

    /* if phone number is not active, set #digits to 0 */
    if (ui_get_nv ( NV_DIR_NUMBER_PCS_I, (nv_item_type *)&nv_dir_num ) == NV_NOTACTIVE_S)
    {
      nv_dir_num.n_digits =0;
      (void) ui_put_nv ( NV_DIR_NUMBER_PCS_I, (nv_item_type *)&nv_dir_num );
    }

#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

    /******************************************************************/
    /* Tell MC whether UI thinks we can go online or not based on the */
    /* state of the NAMs.                                             */
    /******************************************************************/

    /***************************************************/
    /* Hardware has determined that we must go offline */
    /***************************************************/
    if( hw_force_offline() ) {  /* hardware force offline */
          MSG_HIGH( "Hardware force offline is shorted (on)", 0, 0, 0 );
          good = FALSE;
    }

    provisioning_set = TRUE;
    if (good && goodnam) {
      provisioning_status = TRUE;
    } else {
      provisioning_status = FALSE;
    }

} /* End ui_init_provisioning */


/*===========================================================================
FUNCTION ui_return_provisioning_status

DESCRIPTION
  check provisioning status (success, fail, pending)

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
ui_provisioning_status_type ui_return_provisioning_status(void)
{
  if (provisioning_set)
  {
    if (provisioning_status)
    {
      return UI_PROVISIONING_SUCCESS;
    }
    else
    {
      return UI_PROVISIONING_FAILURE;
    }
  }
  else
  {
    return UI_PROVISIONING_PENDING;
  }
}
#endif /* !FEATURE_UI_DUALPROC_APPS */

/*===========================================================================
FUNCTION ui_check_provisioning

DESCRIPTION
  check provisioning

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean ui_check_provisioning(void)
{
  int i=0;
  int wait_time = 100;
  int sleep_time = 60 * 1000;
  ui_provisioning_status_type provisioning_status;

  // There is a race condition here in multiproc systems between the modem
  // running ui_init_provisioning, and the apps running ui_check_provisioning.
  // This makes sure that the modem has actually checked things out before the
  // apps looks at the value.
  for (i=0; i<(sleep_time / wait_time); i++)
  {
    provisioning_status = ui_return_provisioning_status();

    switch (provisioning_status)
    {
      case UI_PROVISIONING_SUCCESS:
        return TRUE;

      case UI_PROVISIONING_FAILURE:
        return FALSE;

      case UI_PROVISIONING_PENDING:
        rex_sleep(wait_time);
        break;
    }
  }
  MSG_FATAL("ui_init_provisioning has not run within %d seconds.",sleep_time / 1000,0,0);
  return FALSE;
}

