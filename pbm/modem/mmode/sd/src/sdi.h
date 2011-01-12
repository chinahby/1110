#ifndef SDI_H
#define SDI_H
/*===========================================================================

               S Y S T E M   D E T E R M I N A T I O N

               I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions used internally by the System
  Determination, as well as, providing internal interface to the SD.C file.

  This header file should NOT be included by any source file outside the
  System Determination module.

  Copyright (c) 2000 - 2009 by Qualcomm, Incorporated. All Rights Reserved.
  Qualcomm Confidential and Proprietary.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mmode/sd/rel/08H1/src/sdi.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
09/15/08   pk      Added support for WLAN UOoS
03/19/08   cl      Fixing UOOS Time to support Auto Mode Builds.
03/17/08   vs      Added definitions for additional system info
03/12/08   aj      Add header to file reflecting p4 path and revision
12/19/07   ka      Changes to support 16k PRL
09/04/07   cl      Change the interface that detect HDR Support
08/30/07   cl      Added CM critical section support. Also replaced INTLOCK /
                   INTFREE with rex_enter_crit_sect() / rex_leave_crit_sect()
06/19/07   cl      Added in FEATURE_SD_DETECT_HDR_HW
06/06/07   pk      UOOS enhancements...
03/29/07   pk      Added new GW UOOS wakeup time for GCF fix.
05/02/07   jqi     DO not turn off the redirection afer IHO if the expected
                   SID/NID of the redirected system is WILDCARD.
                   Expanded 1x system reselection timer value from 16-bit to
                   32-bit.
03/28/07   sk      Added new GW UOOS wakeup time for GCF fix.
11/15/06   pk      Added support for HYBR_GW
03/05/06   jqi     Merged WCDMA BC3,4,8 and 9 support.
12/12/05   jqi     Lint fixes
11/08/05   jqi     Added support for 1x acquisition throttling for BCMCS
                   activity.
11/02/05   sk      Fixed all Lint and compiler warnings.
06/26/05   ic      Lint and compiler warnings cleanup
04/18/05   jqi     Lint error 737(Loss of sign in promotion from int to
                   unsigned long long)
04/04/05   jqi     Lint fixes.
03/28/05   RC      Added macros to extract the low/high 32bits of a QWORD
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
06/28/04   sj      Moved macros for mode/band pref to sd.h
03/24/04   jqi     Added band and more bit oprt macro.
03/17/04   jqi     Fixed compiler warning.
12/12/03   RC      Added support for FEATURE_INT_ROAMING
12/08/03   SJ      Added support for multistage full srv reselection.
05/22/03   RC      Added definition for magic value that indicates a new SID.
01/16/03   SJ      Added defines to indicate if SD uses DB or CAI.
01/23/02   RC      Added support for Rel. 3.0 CDMA-HDR hybrid operation.
12/12/01   RC      Added support for CDMA-HDR hybrid operation.
09/07/01   RC      Added support for T53 and JCDMA features.
02/26/01   RC      Changes to support dual-mode targets.
02/01/01   RC      Changes per 8th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
11/27/00   RC      Added sd_ss_act_is_equal().
11/17/00   RC      Initial release.

===========================================================================*/


#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */
#include "sd.h"       /* External header file */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
================================== COMMON ===================================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*---------------------------------------------------------------------------
                         MISCELLANEOUS TYPES AND MACROS
---------------------------------------------------------------------------*/

/* Define to indicate whether SD should use DB services or not.
*/
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) || \
    defined(FEATURE_ACP)      || defined(FEATURE_HDR)
#define SD_USES_DB_SERVICES
#endif


/* Define to indicate whether SD should use CDMA air interface defines
** or not.
*/
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) || \
    defined(FEATURE_ACP)      || defined(FEATURE_HDR)
#define SD_USES_CAI
#endif


/* Define the CAI normal access return if SD does not use CAI.
*/
#ifndef SD_USES_CAI
#define CAI_NORMAL_ACCESS_RETURN     0
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* SD value for a wildcard system table index.
*/
#define SD_WILDCARD_SYS_IDX   ((word) (-1))

/* SD value for no change system table index.
*/
#define SD_NO_CHANGE_SYS_IDX  ((word) (-2))

/* SD magic value that indicates a new SID.
*/
#define SD_NEW_SID            (0xFFFF)

/* SD magic value that indicates no MCC and MAX MCC.
*/
#define SD_MCC_NONE           ((word) (-1))
#define SD_MCC_MAX            1000

/* SD magic value that indicates no IMSI_11_12 and MAX IMSI_11_12.
*/
#define SD_IMSI_11_12_NONE    ((byte) (-1))
#define SD_IMSI_11_12_MAX     100

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to check whether target is dual-mode.
*/
#if( defined(FEATURE_CDMA_800) && !defined(FEATURE_CDMA_1900) )
  #define SD_TARGET_DUALMODE
#else
  #undef SD_TARGET_DUALMODE
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Definition for the additional system info 
*/

#define   SDSS_ASSN_TAG_NONE   (0xFF)
#define   SDSS_GEO_OFFSET_NONE (-1)
#define   SDSS_GEO_COUNT_NONE  (0)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* SD 1X Scan Time Offset for UOOS. This value is the offset 
** between the actual value that writen in NV and the value in real time.
*/
#define SD_UOOS_SCAN_TIME_OFFSET  1

#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING
#error code not present
#endif /* FEATURE_UMTS_UNIFORM_OOS_HANDLING */

/* HDR UOOS scan time, in milliseconds
*/
#define SD_UOOS_ACQ_HYBR_HDR_TIME       850

#define SD_IS683A_DEFAULT_HDR_CHAN      31   /*Modify this default value if required. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to check whether val is between min and max
** (not include min and max).
*/
#define BETWEEN( val, min, max )  ( (val) > (min) && (val) < (max) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to check whether val is in the range of min to max
** (including min and max).
*/
#define INRANGE( val, min, max )  \
/*lint -e568 -e641*/ (((int32)val) >= ((int32)min) && ((int32)val) <= ((int32)max)) \
/*lint +e568 +e641*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to safely assign to a value to a boolean variable.
*/
#ifndef BOOLEAN
#define BOOLEAN( val )  ( (val) ? TRUE : FALSE )
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macros to extract the low/high 32bits of a QWORD (64 bits).
*/
#define QWORD_LOW( qword )  ( (dword) ((uint64) qword & (uint64) 0xFFFFFFFF))
#define QWORD_HIGH( qword ) ( (dword) ((uint64) qword >> 32) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to convert two values into a word.
*/
#define TO_WORD( word_hi, word_lo ) \
             ( (((word) (word_hi))<<8) | \
               (((word) ((byte) (word_lo)))) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to convert two bytes into a word.
** Not as safe as TO_WORD(), but faster.
*/
#define BYTES_TO_WORD( word_hi, word_lo ) \
             ( (((word) (word_hi))<<8) | \
               ((word) (word_lo)) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to convert four bytes into a dword.
** Not as safe as TO_DWORD(), but faster.
*/
#define BYTES_TO_DWORD( b3, b2, b1, b0 ) \
             ( (((dword) (b3))<<24) | \
               (((dword) (b2))<<16) | \
               (((dword) (b1))<<8)  | \
               ((dword) (b0)) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macros to extract the low/high 32bits of a QWORD (64 bits).
*/
#define DWORD_LOW( dword )  ( (word) ((uint32) dword & (uint32) 0xFFFF))
#define DWORD_HIGH( dword ) ( (word) ((uint32) dword >> 16) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to convert two words into a dword.
*/
#define WORD_TO_DWORD( dword_hi, dword_lo ) \
             ( (((dword) (dword_hi))<<16) | \
               (((dword) ((dword) (dword_lo)))) )


/*---------------------------------------------------------------------------
                               Bit operation MACROS
---------------------------------------------------------------------------*/


/* Macro to check if e1 contains e0.
*/
#define SD_IS_BIT_CONTAIN( e1, e0 ) ( (BOOLEAN ((word) e1 & (word) e0)) )
/* Macro to return bits that are set either in e1 or in e0.
*/
#define SD_BIT_ADD( e1, e0 )        ( (word) e1 | (word) e0 )
/* Macro to get common bits in both e1 and e0.
*/
#define SD_BIT_COMMON( e1, e0 )     ( (word) e1 & (word) e0 )
/* Macro to take the bit set in e0 out of e1.
*/
#define SD_BIT_DELETE( e1, e0 )     ( (word) e1 & ~(word) e0 )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to correctly align function definitions (cosmetic).
*/
#define MMEXTN


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* define for invalid SS timer.
*/
#define SD_SS_TIMER_INVALID          (dword)(900)

/* <EJECT> */
/*---------------------------------------------------------------------------
                               STACK MACROS
---------------------------------------------------------------------------*/


/* Macros to operate on a stack of elements that is implemented as a
** contiguous array of elements in memory.
*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to define a stack of a specified type, name and size.
*/
#define STACK_DEF( type, name, size )  struct { unsigned int  cnt;       \
                                                type          arr[size]; \
                                              } name


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to initialize the a stack.
*/
#define STACK_INIT( s )   (s.cnt = 0)


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return the size of a stack.
*/
#define STACK_SIZE( s )   ARR_SIZE( s.arr )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return the count (i.e. the number of elements) of a stack.
*/
#define STACK_CNT( s )    ( s.cnt )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to push an item onto a stack.
** Return TRUE if item is successfully pushed on the stack, else return
** FALSE.
*/
#define STACK_PUSH( s, item )                                               \
 ( s.cnt >= STACK_SIZE(s) ? FALSE : (  s.arr[s.cnt] = item,                 \
                                       s.cnt++,                             \
                                       TRUE ) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return a pointer to the item at the top of a stack.
** If stack is empty a NULL is returned.
*/
#define STACK_TOP( s ) ( s.cnt == 0 ? NULL : &s.arr[s.cnt-1] )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to pop an item of the top of a stack.
** Return TRUE if item is successfully popped, else return FALSE.
*/
#define STACK_POP( s )  ( s.cnt == 0 ? FALSE : (s.cnt--, TRUE) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Helps overcome limitation of pos being word in b_unpackx functions.
** Done by moving pointer to the closest byte and giving offset of bit
** within that byte.
*/
#define SD_B_UNPACKB(buf_ptr, offset, len) b_unpackb(&(buf_ptr[offset/8]), \
                                                     (word) (offset%8), len)

#define SD_B_UNPACKW(buf_ptr, offset, len) b_unpackw(&(buf_ptr[offset/8]), \
                                                     (word) (offset%8), len)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================ SERVICE INDICATORS =============================
=============================================================================
=============================================================================
===========================================================================*/


/* Enumeration of service update criterias.
*/
typedef enum {

  SD_SI_UPDATE_ACQ,   /* Update due to successful acquisition */
  SD_SI_UPDATE_OH,    /* Update due to overhead information */
  SD_SI_UPDATE_REG,   /* Update due to successful registration */

  SD_SI_UPDATE_MAX
} sd_si_update_e_type;


/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_srv_off

DESCRIPTION
  Inform the System Indication component that the service indication should
  be turned off.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_srv_off(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_pwr_save

DESCRIPTION
  Inform the System Indication component that the service indication
  is pwr_save.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_srv_pwr_save(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_srv_off_until

DESCRIPTION
  Inform the System Indication component that the service indication should
  be turned off until the specified service update criteria is met.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_srv_off_until(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sd_si_update_e_type       si_update_criteria
            /* Service update criteria - indicate the minimum criteria for
            ** updating the service indicators.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_acq_fail

DESCRIPTION
  Notify the registered client (i.e., CM) that acquisition over all systems
  has failed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_acq_fail(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);


/*===========================================================================

FUNCTION sd_si_srv_lost_cb

DESCRIPTION
  Notify the registered client (i.e., CM) that service has been lost

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_srv_lost_cb(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_emerg_cb_mode

DESCRIPTION
  Notify the registered client (e.g., CM) that SD entered emergency callback
  mode of operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_emerg_cb_mode( void );




/* <EJECT> */
/*===========================================================================

FUNCTION sd_si_autonam_change

DESCRIPTION
  Notify the registered client (e.g., CM) that auto-NAM change is
  appropriate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_si_autonam_change(

        sd_nam_e_type             nam
          /* The NAM that is proposed by the auto-NAM functionality.
          */
);




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== SYSTEM SELECTION =============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sd_ss_act_is_equal

DESCRIPTION
  Indicate whether most recent measurement request is done being serviced by
  Search.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if most recent measurement request is done being serviced by Search.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean          sd_ss_meas_is_done(

        sd_ss_e_type     ss
          /* System selection - MAIN or HDR.
          */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_is_get_net_done

DESCRIPTION
  Indicate whether network list was received for the last get networks
  SS-Action.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if network list was received
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean          sd_ss_is_get_net_done(

        sd_ss_e_type     ss
          /* System selection - MAIN or HDR.
          */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_update_get_net_done

DESCRIPTION
  Update the get network done flag.
  SS-Action.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if network list was received
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void             sd_ss_update_get_net_done(

        sd_ss_e_type     ss,
          /* System selection - MAIN or HDR.
          */
        boolean          flag

);


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_is_get_net_done

DESCRIPTION
  Indicate whether network list was received for the last WLAN get networks
  SS-Action.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if network list was received
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  boolean          sd_ss_is_wlan_survey_done(

        sd_ss_e_type     ss
          /* System selection - MAIN or HDR.
          */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_update_wlan_survey_done

DESCRIPTION
  Update the WLAN get network done flag.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if network list was received
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  void             sd_ss_update_wlan_survey_done(

        sd_ss_e_type     ss,
          /* System selection - MAIN or HDR.
          */
        boolean          flag

);


/*===========================================================================

FUNCTION sd_ss_get_timer

DESCRIPTION
  Gets the SS timer

EPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
MMEXTN  dword                      sd_ss_get_timer(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);
/* <EJECT> */
/*===========================================================================

FUNCTION sd_rex_enter_crit_sect

DESCRIPTION
  Initialize SD critical section.
  This function must be called before any other part of SD is initialized

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
MMEXTN void sd_rex_enter_crit_sect(void);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_rex_leave_crit_sect

DESCRIPTION
  Initialize SD critical section.
  This function must be called before any other part of SD is initialized

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
MMEXTN void sd_rex_leave_crit_sect(void);


#endif /* SDI_H */


