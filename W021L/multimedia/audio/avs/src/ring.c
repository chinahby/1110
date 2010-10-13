/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         R I N G E R    D R I V E R    

GENERAL DESCRIPTION
  This driver controls the generation of sounds from the ringer.

EXTERNALIZED FUNCTIONS
  ring_init
    Hardware initializers necessary to use ringer.
  ring_click
    Generates clicks through the ringer.
  ring_tone
    Plays the two frequencies at the requested volume (duty cycle),
    a duration of 0ms will stop all tones being played through the ringer.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ring_init() must be called before using any of the ringer functions.

Copyright(c) 1998 - 2003 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/ring.c#6 $ $DateTime: 2009/10/14 07:34:24 $ $Author: c_ypaida $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/14/09   yp      CMI changes for SCMM 
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and 
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
09/24/08   vsud    Added feature guard for Ext Ringer.
06/26/03    aw     Created MSMAUD_MSM6200_RINGER.  Calibrated ringer.
           tjw     RINGER_FREQ_MULTIPLIER = 1200000 for MSM6200
11/06/02    sm     Include msmaud.h and bsp.h
06/11/02    st     Modified code to support new PM6050 revision; modified code
                   to support PM6000; modified code to check for ring counter
                   violations.
03/12/02    st     Added code to compensate for PM6050 ringer circuit.
10/01/01    ro     Update driver to include and use msmaud.h
07/24/01   ymc     Replaced RINGER_OUT macros by MSM_OUT.
                   Changed duty_cycle to duty_cycle[2] and munched_n to
                   munched_n[2].
                   Added T_MSM5200 support.
                   Updated Copyright.
                   Eliminated T_I2 support.
                   Replaced INTLOCK_SAV by INTLOCK and INTFREE_SAV by INTFREE.
11/22/99    jc     Use logical BIO registers
11/15/99    st     Removed MSM2300 and vibrator support.
09/09/99    jc     Added support for vibrator on pm1000 chip.
08/02/99    jc     Removed new gpio_xxx MSM3100 support, featurized.
07/15/99    jc/mk  Added new gpio_xxx MSM3100 support, featurized.
04/20/99    st     Added vibrator support for Digby target.
03/22/99    st     Moved some defines under T_MSM3 target.
03/04/99    st     Creation, transported functions from sndhwg2.c.

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"    /* Customer specific definitions       */
#include "msmaud.h"      /* MSMAudio features                   */
#include "bsp.h"         /* Board support package               */
#include "ring.h"        /* Ringer typedefs and prototypes      */
#include "err.h"         /* Error logging/reporting functions   */
#include "msg.h"         /* Message logging/reporting functions */
#ifdef FEATURE_CMI
#error code not present
#else
#include "clk.h"
#endif

#ifdef FEATURE_CMI_MM
#error code not present
#endif

#ifndef FEATURE_NO_EXT_RINGER

#define RING_D_JAM_HIGH      0x00 /* Duty cycle value to set M/N output high */
#define RING_CLICK_DURATION  150L /* Click pause(us) */

/* Maximum and Minimum frequencies that the ringer can generate.
*/
#ifdef MSMAUD_MSM6200_RINGER
#error code not present
#else
#define RINGER_FREQ_MIN           100       /* minimum ringer freq at 100  Hz */
#endif /* MSMAUD_MSM6200_RINGER */
#define RINGER_FREQ_MAX           4000      /* maximum ringer freq at 4000 Hz */

#define RINGER_LOW_FREQ           534       /* Lower end frequency cutoff     */
#define RINGER_MID_FREQ           1064      /* Middle end frequency cutoff    */
#define RINGER_HIGH_FREQ          2105      /* High end frequency cutoff      */

#if (BSP_OSCILLATOR_IN == BSP_TCXO_19P2_MHZ) /* TCXO = 19.20 */
#ifdef MSMAUD_MSM6200_RINGER
#error code not present
#else
#define RINGER_FREQ_MULTIPLIER    300000L   /* Mulitplier constant: 300000    */
#endif /* MSMAUD_MSM6200_RINGER */
#elif (BSP_OSCILLATOR_IN == BSP_TCXO_19P68_MHZ)
#define RINGER_FREQ_MULTIPLIER    307500L   /* Multiplier constant: 307500    */
#elif (BSP_OSCILLATOR_IN == BSP_TCXO_19P8_MHZ)
#define RINGER_FREQ_MULTIPLIER    309375L   /* Multiplier constant: 309375    */
#else 
#error Unsupport ringer frequency multipler value
#endif

#if (RINGER_FREQ_MULTIPLIER == 1200000L)
  #define RINGER_VERYLOW_M_COUNTER  1  /* M value for ( 100Hz <= freq < 535Hz)*/
  #define RINGER_LOW_M_COUNTER      2  /* M value for ( 535Hz <= freq <1065Hz)*/
  #define RINGER_MID_M_COUNTER      3  /* M value for (1065Hz <= freq <2105Hz)*/
  #define RINGER_HIGH_M_COUNTER     7  /* M value for (freq > 2105 Hz)        */
#else
  #define RINGER_VERYLOW_M_COUNTER  1  /* M value for ( 100Hz <= freq < 535Hz)*/
  #define RINGER_LOW_M_COUNTER      7  /* M value for ( 535Hz <= freq <1065Hz)*/
  #define RINGER_MID_M_COUNTER      14 /* M value for (1065Hz <= freq <2105Hz)*/
  #define RINGER_HIGH_M_COUNTER     28 /* M value for (freq > 2105 Hz)        */
#endif

/* Constant values required for the ringer's (m,n) pairs. The value of
** m is fixed and n is computed by using the following equation:
**                    Frequency (Hz) = RINGER_FREQ_MULTIPLIER * m / n
** where m cannot exceed 4095.
*/
                                   /* Very Low frequency range */
#define RINGER_VERYLOW_FREQ_MULTIPLY \
        (long) (RINGER_FREQ_MULTIPLIER*RINGER_VERYLOW_M_COUNTER)

                                   /* Low Frequency range */
#define RINGER_LOW_FREQ_MULTIPLY  \
        (long) (RINGER_FREQ_MULTIPLIER*RINGER_LOW_M_COUNTER)
                                   /* Mid Frequency range */
#define RINGER_MID_FREQ_MULTIPLY  \
        (long) (RINGER_FREQ_MULTIPLIER*RINGER_MID_M_COUNTER)
                               /* High Frequency range */
#define RINGER_HIGH_FREQ_MULTIPLY  \
        (long) (RINGER_FREQ_MULTIPLIER*RINGER_HIGH_M_COUNTER)

/* Due to specifications of the ring oscillator, in order to support the
** 300Hz - 4kHz range of frequencies, the maximum duty cycle that can be
** used is 98.66%. For values beyond this max, the inverse duty cycle will
** be used.
*/
#define RINGER_MAX_DUTY_CYCLE  9866

/* Structure to contain the values that must be written
** to program one M/N counter.
*/
typedef struct {
  uint16 m;
  uint16 n;
  uint16 d;
} ring_mn_programming_type;

#ifdef FEATURE_PMIC_RINGER
#ifdef MSMAUD_PM6050_RINGER
/* This calculation is necessary to compensate for the PMIC ramp up delay.
** The ramp up delay for the PM6050 is 7.7us. The calculation to adjust for
** duty cycle is (frequency*delay). The calculation below is for frequency in
** Hertz and adjusts for integer math and units used for duty cycle.
*/
#define RING_CALC_PMIC_OFFSET(v)  ((v * 77) / 1000)
#else
/* No compensation is required for the PM6000.
*/
#define RING_CALC_PMIC_OFFSET(v)  (0)
#endif /* MSMAUD_PM6050_RINGER */
#endif /* FEATURE_PMIC_RINGER */

/* <EJECT> */
/**************************** F U N C T I O N S ****************************/
/*===========================================================================

FUNCTION ring_init

DESCRIPTION
  Generic Low Level Hardware Initialization for ringer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void ring_init(void)
{
  /* Set the line that controls power to the ringer enable circuit
  ** to an output. Then turn off the Ringer Circuit.
  */

#ifdef RING_USE_RINGER_ENABLE
  BIO_TRISTATE(BIO_RINGER_REG,BIO_RINGER_EN_M,BIO_OUTPUT_ENA_V);
  BIO_OUT(BIO_RINGER_REG,BIO_RINGER_EN_M,BIO_RINGER_DIS_V);
#endif /* RING_USE_RINGER_ENABLE */

}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION ring_compute_ringer_freq

DESCRIPTION
  This function returns the (m,n) pair for a given frequency to be
  played by the ringer.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the (m,n) pair structure ring_mn_counter_type.

SIDE EFFECTS
  None.
===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void ring_compute_ringer_freq (
  uint16 frequency,
    /* Frequency in hz for which the m,n values are to be computed */
  ring_mn_counter_type *mn_out_val
    /* (m,n) pair for the frequency entered                        */
)
{
   /* Check if the given frequency is out of range for the ringer        */
   if (frequency < RINGER_FREQ_MIN) {
     ERR("Freq:%d - Min Freq is %d Hz",frequency,RINGER_FREQ_MIN,0);
     frequency = RINGER_FREQ_MIN; /* If freq is too low then set to min  */
   } else if (frequency > RINGER_FREQ_MAX) {
     ERR("Freq:%d - Max Freq is %d Hz",frequency,RINGER_FREQ_MAX,0);
     frequency = RINGER_FREQ_MAX; /* If freq is too high then set to max */
   }
   /* For all values, based on what range the frequency lies in
   ** calculate the appropriate (m,n) pair.
   */
   if (frequency <= RINGER_LOW_FREQ) {
     mn_out_val->m = RINGER_VERYLOW_M_COUNTER; /* For very low frequencies    */
     mn_out_val->n = RINGER_VERYLOW_FREQ_MULTIPLY / frequency;
   } else if (frequency <= RINGER_MID_FREQ) {
     mn_out_val->m = RINGER_LOW_M_COUNTER;     /* For low frequencies         */
     mn_out_val->n = RINGER_LOW_FREQ_MULTIPLY / frequency;
   } else if (frequency <= RINGER_HIGH_FREQ) {
     mn_out_val->m = RINGER_MID_M_COUNTER;     /* For mid frequencies         */
     mn_out_val->n = RINGER_MID_FREQ_MULTIPLY / frequency;
   } else {
     mn_out_val->m = RINGER_HIGH_M_COUNTER;    /* For high frequencies        */
     mn_out_val->n = RINGER_HIGH_FREQ_MULTIPLY / frequency;
   }
   MSG_LOW("Freq:%d Hz m:%d n:%d",frequency,mn_out_val->m,mn_out_val->n);
} /* end of ring_compute_ringer_freq */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION ring_stop_ring

DESCRIPTION
  Stop a tone emitting from the ringer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void ring_stop_ring( void )
{
  /* Stop both sides of the Tone Generator.  (Two M/N counters.)        */
  RINGER_STOP();

#ifdef RING_USE_RINGER_ENABLE
  BIO_OUT(BIO_RINGER_REG,BIO_RINGER_EN_M,BIO_RINGER_DIS_V); /* Disable Line */
#endif /* RING_USE_RINGER_ENABLE */

} /* end of ring_stop_ring */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION ring_tone

DESCRIPTION
  Start a DTMF emitting from the ringer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void ring_tone (
  uint16 freq_a,             /* frequency in hz of 1st tone to play */
  uint16 freq_b,             /* frequency in hz of 2nd tone to play */
  int    ring_vol,           /* duty cycle of tone, 5000 = 50% */
  uint16 duration_ms         /* duration of tone, 0 means turn ringer off */
)
{
  /* Variables to remember what tone is being generated */
  static uint16 old_freq_a = 0;
  static uint16 old_freq_b = 0;
  static int old_ring_vol  = 0;
  uint16 duty_cycle_a, duty_cycle_b;

  /* Data to compute M/N pairs from frequency */
  ring_mn_counter_type mn_val;

  /* M/N counter programming values */
  ring_mn_programming_type ring_a_data;
  ring_mn_programming_type ring_b_data;

  MSG_MED( "ring_tone T1=%d, T2=%d, V=%d", freq_a, freq_b, ring_vol);

  /* if duration is 0 then stop sounds from ringer */
  if(duration_ms == 0) {
    ring_stop_ring();
    old_freq_a   = 0;
    old_freq_b   = 0;
    old_ring_vol = 0;
    return;
  }

  /* If both tones are the same as the last call and still running,     */
  /* then don't do anything.    (Leave them alone.)  This prevents a    */
  /* "click" on long duration continuous tones.                         */

  /* Same Tones?  */
  if(old_freq_a == freq_a && old_freq_b == freq_b && ring_vol == old_ring_vol) {
    /* ringer busy at previous volume and scale */
    return;
  }
  /* Volume has changed or ringer circuit is no longer busy */
  old_freq_a   = freq_a;
  old_freq_b   = freq_b;
  old_ring_vol = ring_vol;

  /* Program each side as needed.  The M Register contains an "enable   */
  /* bit" that turns the ringer on/off and the M/N must be turned off   */
  /* to properly reprogram it.                                          */ 

#ifdef FEATURE_PMIC_RINGER
  if (ring_vol != 0) {
    duty_cycle_a = (10000 - ring_vol) + RING_CALC_PMIC_OFFSET(freq_a);
    if(freq_a == freq_b) {
      duty_cycle_b = duty_cycle_a;
    } else {
      duty_cycle_b = (10000 - ring_vol) + RING_CALC_PMIC_OFFSET(freq_b);
    }  
  } else 
#endif /* FEATURE_PMIC_RINGER */
  {
    duty_cycle_a = ring_vol;
    duty_cycle_b = ring_vol;
  }

  /* Duty Cycle correction */
  if (duty_cycle_a >= RINGER_MAX_DUTY_CYCLE) {
    duty_cycle_a = (10000 - duty_cycle_a);
  }
  if (duty_cycle_b >= RINGER_MAX_DUTY_CYCLE) {
    duty_cycle_b = (10000 - duty_cycle_b);
  } 

  ring_compute_ringer_freq(freq_a, &mn_val);
  ring_a_data.d = (uint16) ((mn_val.n * duty_cycle_a) / 10000);
  ring_a_data.m = mn_val.m;
  ring_a_data.n = ~(mn_val.n - mn_val.m);
  
  ring_compute_ringer_freq(freq_b, &mn_val);
  ring_b_data.d = (uint16) ((mn_val.n * duty_cycle_b) / 10000);
  ring_b_data.m = mn_val.m;
  ring_b_data.n = ~(mn_val.n - mn_val.m);

  INTLOCK();
  RINGER_STOP();

  /* Program M/N counter A */
  MSMAUD_RINGER_MN_A_DUTY(ring_a_data.d);
  MSMAUD_RINGER_MN_A_N(ring_a_data.n);
  
  /* Program M/N counter B */
  MSMAUD_RINGER_MN_B_DUTY(ring_b_data.d);
  MSMAUD_RINGER_MN_B_N(ring_b_data.n);
  
  /* Enable both M/N counters */
  MSMAUD_RINGER_MN_A_M(ring_a_data.m);
  MSMAUD_RINGER_MN_B_M(ring_b_data.m);
  INTFREE();

#ifdef RING_USE_RINGER_ENABLE
  BIO_OUT(BIO_RINGER_REG,BIO_RINGER_EN_M,BIO_RINGER_ENA_V);
  BIO_TRISTATE(BIO_RINGER_REG,BIO_RINGER_EN_M,BIO_OUTPUT_ENA_V);
#endif /* RING_USE_RINGER_ENABLE */

} /* end of ring_tone */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/* <EJECT> */
/*===========================================================================

FUNCTION ring_click

DESCRIPTION
  Turn the ringer on and starts counter. 
  
DEPENDENCIES
  Uses ring_msm23_plus_inuse.
  
RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
void ring_click ()
{
  /* Jam in a special M/N Duty Cycle that will force the M/N output to  */
  /* high and keep it there.                                            */ 

  INTLOCK();

  MSMAUD_RINGER_MN_A_DUTY(RING_D_JAM_HIGH);
  MSMAUD_RINGER_MN_B_DUTY(RING_D_JAM_HIGH);
  
  /* Enable both M/N counters */
  MSMAUD_RINGER_MN_A_M(0);
  MSMAUD_RINGER_MN_B_M(0);

  INTFREE();

  #if defined (FEATURE_CMI_MM) || !defined(FEATURE_CMI)
clk_busy_wait(RING_CLICK_DURATION);
    		
 	#else
#error code not present
	#endif

  RINGER_STOP();
} /* end of ring_click */
/* <EJECT> */
#endif/*FEATURE_NO_EXT_RINGER*/
