#ifndef ADCI_H
#define ADCI_H
/*===========================================================================


             A D C  ( I n t e r n a l )  H E A D E R    F I L E

DESCRIPTION
  This file contains "internal" definitions required by adc.c and
  the adc_<target>.c files

Copyright (c) 2003, 2004, 2005 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/hwengines/adc/qsc1100/main/latest/src/adci.h#1 $ $DateTime: 2008/08/14 10:53:43 $ $Author: jthielen $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11-16-05   ty      Put HKADC block to sleep when not needed
11-10-05   ty      Add ADC debug messages
06-16-05   jac     Added FEATURE_RF_TX_PWR_LIM_REV for new HDET implementation
04-05-05   ty      prevent sleep when doing ADC reads
06-01-04   Vish    Reverted back to const definifion of 
                   adc_logical_channel_definitions but wrapped under
                   the non-definition of FEATURE_ADC_CALIBRATION.
05-12-04   Vish    Changed the logical channel definition table from being const
                   to non-const so that offset/slope params could be changed
                   dynamically at run-time (for e.g. after the ADC self-calibration
                   procedure is run at system start-up).
05-10-04   aks     Added headers, featurize for T_MSM6000, which uses memory
                   mapped ADIE.
11-11-03   JAC     added include for adie_sbi.h
08-26-03   Vish    Reverted logical channel definition table to be const again
08-15-03   Vish    Replaced full_scale_reading/result with offset/slope params
                   in the logical channel definition structure. Also changed
                   the conversion fn signature to account for the above.
                   Since some conversion fns need to read the same logical
                   channel with different offset/slope for better accuracy,
                   changed the logical channel definition table from being const
                   to non-const so that these 2 params could be changed
                   dynamically at run-time.
03-01-06   tjw     Created.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"
#ifdef T_MSM6000
#error code not present
#else
#include "adie_sbi.h"
#endif

#ifdef FEATURE_ADC_VOTE_FOR_SLEEP
#include "sleep.h"
#include "adie.h"
#endif

typedef enum
  {
  IMMEDIATE,            /* ADC read immediately */
  BUFFERED              /* ADC reading from buffer, invoke a new reading */
  } adc_logical_channel_type_type;

#define NO_READING_IN_BUFFER  (-1)

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

#ifdef FEATURE_ADC_MULTI_RESOLUTION
typedef uint16 adc_data_type;
#else
typedef uint8 adc_data_type;
#endif
/*===========================================================================

  This structure defines a logical ADC channel. The details may vary from target
  to target but the result returned for any channel should always be the same to
  ensure portability of the user code.

===========================================================================*/
typedef struct
  {
  adc_logical_channel_type logical_channel_number;/* to check the table isn't broken */
  adc_error_return_type (*adc_read_fn)(int *reading, void (*adc_completion_cb_fn)(void));
                            /* call-back function on completion of the
                               ADC read process */
  adc_logical_channel_type_type logical_channel_type;
  int   min_reading;        /* Minimum permissible reading */
  int   max_reading;        /* Maximum permissible reading */
  int   default_result;     /* Result returned if no real reading is available */
  /* if ADC reads 'reading' from the hardware, we have to apply the following equation
     to get the result:
     result = offset_result + ((reading * result_vs_reading_slope_x_10000)/10000)
  */
  int32 offset_result;     /* Offset that needs to be added to the read one
                               to get the "actual real" result */
  int32 result_vs_reading_slope_x_10000;  /* result vs. reading slope; since this
                                             will most likely be a small fraction,
                                             we magnify this by multiplying with a
                                             large number (10000) so as to retain some
                                             precision; we need to divide by the same
                                             factor later in our calculation used in
                                             <conversion_fn>
                                          */
  int   (*filter_fn)(int last_reading, int new_reading);
  adc_result_type (*conversion_fn)(int reading, int32 offset, int32 slope_x_10000);
                            /* Function to convert the reading into the units
                               expected by the user code */
  } adc_logical_channel_definition_type;

/*===========================================================================
  The table of channel definitions
===========================================================================*/

extern
#ifndef FEATURE_ADC_CALIBRATION
const
#endif //FEATURE_ADC_CALIBRATION
adc_logical_channel_definition_type adc_logical_channel_definitions[NUMBER_OF_LOGICAL_ADC_CHANNELS];

/*===========================================================================

                        DEFINITIONS

===========================================================================*/

#if defined(FEATURE_SLEEP_TASK) 
#define ADC_SLEEP_ALLOW() \
  do{ \
    INTLOCK(); \
    adie_wakeup( ADIE_DISABLE, ADIE_HKADC_BLK ); \
    SLEEP_ASSERT_OKTS( SLEEP_HKADC_OKTS_SIG ); \
    INTFREE(); \
  }while(0)
#define ADC_SLEEP_FORBID() \
  do{ \
    INTLOCK(); \
    adie_wakeup( ADIE_ENABLE, ADIE_HKADC_BLK ); \
    SLEEP_NEGATE_OKTS( SLEEP_HKADC_OKTS_SIG ); \
    INTFREE(); \
  }while(0)
#else  /* FEATURE_SLEEP_TASK */
#define ADC_SLEEP_ALLOW()
#define ADC_SLEEP_FORBID()
#endif /* FEATURE_SLEEP_TASK */

#ifdef FEATURE_ADC_DEBUG
#define ADC_DEBUG_MSG(fmt,a,b,c) \
    MSG_HIGH((fmt), (a), (b), (c))
#else
#define ADC_DEBUG_MSG(fmt,a,b,c) 
#endif
/*===========================================================================

                        PROTOTYPE DEFINITIONS

===========================================================================*/

/* Standard filter functions included in adc.c
  (target-specific function prototypes can be included in adc_<target>.h)
*/
extern int adc_inc_dec_filter (int last_reading, int new_reading);
extern int adc_null_filter (int last_reading, int new_reading);


/* Standard conversion functions included in adc.c
  (target-specific function prototypes can be included in adc_<target>.h)
*/
extern adc_result_type adc_to_mV(int reading, int32 offset, int32 slope_x_10000);
extern adc_result_type adc_to_percentage(int reading, int32 offset, int32 slope_x_10000);
extern adc_result_type adc_null_conversion(int reading, int32 offset, int32 slope_x_10000);

#ifdef FEATURE_RF_TX_PWR_LIM_REV
/* Functions used internally by adc to set/check control of ADC between ARM and MDSP */
extern boolean adc_check_arm_reserved_adc(void);
extern boolean adc_check_mdsp_reserved_adc(void);
extern void adc_reverse_control_to_arm(void);
#endif

extern void adc_read_complete(void);

#endif /* ADCI_H */

