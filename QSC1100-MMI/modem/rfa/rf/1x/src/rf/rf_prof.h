#ifndef RF_PROF_H
#define RF_PROF_H
/*===========================================================================

                      RF Unit Header File

DESCRIPTION
  This header file contains all the definitions necessary in order to
  interface with the RF Unit.  The RF unit provides interface calls
  for accessing the analog common card and PCN applique synthesizers
  and other miscellaneous items.

Copyright (c) 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_prof.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
10/31/06   ycl     Include the correct rficap file.
05/04/06   dyc     Updated to support timetest.h
08/25/05   dyc     Initial file.

===========================================================================*/

#include "rficap.h"

/*===========================================================================

                       Constants

===========================================================================*/
/* These are optional set depending on level of information */

/* Enable for SW debug port strobe profiling (SURF) */
//#define RF_TIME_PROFILING 

#ifdef RF_HAS_WU_TIMED
/* 
* Un-comment RF_HAS_WU_TIMED_DEBUG to enable for detailed times for 
* specific wake up functions. 
*/
//#define RF_HAS_WU_TIMED_DEBUG 
#endif /* RF_HAS_WU_TIMED */

/* 
* If timing debug is required, setup RF strobes on the SW test header
* (i.e. TIMETEST_PORT)
*
* Access the SW debug port SURF ONLY and may be SURF specific 
* L4 builds re-maps the TIMETEST (SW debug header port)
* FEATURE_L4_KNIGHTRIDER will need to be disabled at the build level since it also
* uses the SW debug header during operation.
*/
#if defined (RF_HAS_WU_TIMED_DEBUG) || \
    defined (RF_TIME_PROFILING)     || \
    defined (INTERPOLATION_PROFILING)
#error code not present
#endif /* RF_HAS_WU_TIMED_DEBUG||RF_TIME_PROFILING||INTERPOLATION_PROFILING */


/* Timetick resolution is 30.5us, factor by 10 for calculations */
#define RF_PROFILE_US_FACTOR  305  

/*===========================================================================

                       Macros

===========================================================================*/

#ifdef TIMETEST_PORT
 /* Use to sent a value to the time test (SW Debug header on the surf) */
 #define RF_SW_DEBUG_OUT(condition, value) if(condition){outpw(TIMETEST_PORT, value);}
#else
 /* No timetest port defined, NULL the macro */
 #define RF_SW_DEBUG_OUT(condition, value);
#endif /* TIMETEST_PORT */

/*===========================================================================

                       Typedefs

===========================================================================*/

/* -------------------------------------------------------
** Structure used to track the RF WU profiling information.
** ------------------------------------------------------- */
typedef struct 
{
  uint32 rf_profile_wu_average;    /* Running aver of wu time */
  uint32 rf_profile_wu_last;       /* Last wu time */
  uint32 wu_count;                 /* Number of wu time sets */
} rf_prof_struct_type;


/* -------------------------------------------------------
** Structure used for RF WU profiling calculations.
** ------------------------------------------------------- */
typedef struct
{
  uint32 wu_start_value;           /* Time tag for start */
  uint32 wu_sum;                   /* Sum of RF WU values for this set */
  uint32 wu_total;                 /* Running total of RF WU values */
  boolean wu_is_first_sleep_cycle; /* flag to determine if 1st sleep cycle */
} rf_prof_calc_type;

/*===========================================================================

                       Shared Data

===========================================================================*/

/* RF WU statistics */
extern rf_prof_struct_type rf_wu_stats[];  

/*===========================================================================

                       Functions Prototypes
                       
===========================================================================*/
/*===========================================================================

FUNCTION RF_PROF_TOGGLE_STROBE_INIT                         INTERNAL FUNCTION

DESCRIPTION
   This function configures a given GPIO signal for RF profiling 
   strobes.   
    
DEPENDENCIES
  None
     
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void rf_prof_toggle_strobe_init(void);

/*===========================================================================

FUNCTION RF_PROF_TOGGLE_STROBE                              INTERNAL FUNCTION

DESCRIPTION
   This function is used to toggle the GPIO strobe signal to the opposite
   level.
    
DEPENDENCIES
  rf_prof_toggle_strobe_init() required to be called once prior to use.
   
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void rf_prof_toggle_strobe(void);

/*===========================================================================

FUNCTION RF_PROF_TIMER_START                                INTERNAL FUNCTION

DESCRIPTION
  This function obtains the value of the tick counter and stores the value
  as the start count reference in usec.
  
DEPENDENCIES
  rf_chain_status structure previously initialized.

RETURN VALUE
  tick counter value in usecs.

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 rf_prof_timer_start(rf_path_enum_type);

/*===========================================================================

FUNCTION RF_PROF_TIMER_STOP                                INTERNAL FUNCTION

DESCRIPTION
  This function obtains the elasped time now and when the last 
  rf_prof_timer_start function was called.  The elasped time is provided 
  in usecs.  
    
DEPENDENCIES
  rf_chain_status structure previously initialized.
  rf_prof_timer_start() called prior to this function

RETURN VALUE
  elapsed time in usec

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 rf_prof_timer_stop(rf_path_enum_type);

/*===========================================================================

FUNCTION RF_PROF_GET_WARMUP                                 INTERNAL FUNCTION

DESCRIPTION
  This function returns the RF WU last and average entries.   
    
DEPENDENCIES
  rf_chain_status structure previously initialized.
  
RETURN VALUE
  Parameters for last WU and average WU in usecs..

SIDE EFFECTS
  None

===========================================================================*/
extern void rf_prof_get_warmup(rf_path_enum_type, uint32 *wu_last, uint32 *wu_average);

/*===========================================================================

FUNCTION RF_PROF_ADD_TO_WARMUP                              INTERNAL FUNCTION

DESCRIPTION
  This function accumulates the warmup values.
  
    
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void rf_prof_add_to_warmup(rf_path_enum_type, uint32);

/*===========================================================================

FUNCTION RF_PROF_WARMUP_DONE                                INTERNAL FUNCTION

DESCRIPTION
  This function processes the current set of accumulated RF WU times.  The
  running average and current WU times are calculated and stored in the
  rf_chain_status structure for reference.   
    
DEPENDENCIES
  rf_chain_status structure previously initialized.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void rf_prof_warmup_done (rf_path_enum_type);

#endif /* RF_PROF_H */

