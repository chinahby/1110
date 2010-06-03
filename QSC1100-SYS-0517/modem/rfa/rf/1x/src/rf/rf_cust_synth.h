#ifndef RF_CUST_SYNTH_H
#define RF_CUST_SYNTH_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             C U S T O M E R  E X T E R N A L  S Y NT H  H E A D E R

GENERAL DESCRIPTION
  This file is provided for customers to define a external synth
  not supported by the ZIF library  

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2003   by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_cust_synth.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/25/05   vm	   Added the function cust_synth_is_locked()
                   which returns default TRUE value for the synth 
                   lock of PLL for external synth device
03/30/05   lp      Merged MSM7500 and RUMI emulation changes
07/02/04   dbc     Renamed test_cust_synth_init() to cust_synth_gpio_init()
03/04/04   dbc     Use RF_HAS_CUSTOMER_DEFINED_SYNTH instead of RF_TEST_CUST_SYNTH
02/06/04   dbc     Created
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "db.h"
#include "rfi.h"

#ifdef RF_HAS_CUSTOMER_DEFINED_SYNTH
/* Defines to use when testing the LMX2433 synthesizer as a customer defined
   synthesizer                                                               */

#include "bio.h"

#define CUST_LMX2433_REG                                              0xffffffff
#define CUST_LMX2433_REG_MASK                                         0x01ffffff
#define CUST_LMX2433_PRESCALER_8                                      8
#define CUST_LMX2433_PRESCALER_16                                     16
#define CUST_LMX2433_SHIFT_REGISTER_SIZE 24
#define CUST_LMX2433_MSB_MASK (1<<CUST_LMX2433_SHIFT_REGISTER_SIZE)
#define CUST_LMX2433_NUMBER_ADDRESS_BITS 3      /* aka Control Bits */


#define CUST_LMX2433_R1_RF_B_BIT_SHIFT                                7
#define CUST_LMX2433_R1_RF_A_BIT_SHIFT                                3


#define CUST_LMX2433_R0_RF_R_ADDRESS                                     0
#define CUST_LMX2433_R1_RF_N_ADDRESS                                     1
#define CUST_LMX2433_R2_RF_TOC_ADDRESS                                   2
#define CUST_LMX2433_R3_IF_R_ADDRESS                                     3
#define CUST_LMX2433_R4_IF_N_ADDRESS                                     4
#define CUST_LMX2433_R5_IF_TOC_ADDRESS                                   5

/* Control Register R0 bits */
#define CUST_LMX2433_R0_RF_R_BIT_SHIFT                                3
#define CUST_LMX2433_R0_RF_CPP_PHASE_DETECT_POSITIVE_POLARITY         1
#define CUST_LMX2433_R0_RF_CPP_PHASE_DETECT_NEGATIVE_POLARITY         0
#define CUST_LMX2433_R0_RF_CPP_PHASE_DETECT_BIT_SHIFT                18

#define CUST_LMX2433_R0_RF_CPG_CHARGE_PUMP_GAIN_4MA                   1
#define CUST_LMX2433_R0_RF_CPG_CHARGE_PUMP_GAIN_1MA                   0
#define CUST_LMX2433_R0_RF_CPG_CHARGE_PUMP_GAIN_BIT_SHIFT            19

#define CUST_LMX2433_R0_RF_CPT_CHARGE_PUMP_TRISTATE_HI                1
#define CUST_LMX2433_R0_RF_CPT_CHARGE_PUMP_TRISTATE_NORMAL            0
#define CUST_LMX2433_R0_RF_CPT_CHARGE_PUMP_TRISTATE_BIT_SHIFT        20

#define CUST_LMX2433_R0_RF_RST_SYNTH_COUNTER_RESET_ENABLE             1
#define CUST_LMX2433_R0_RF_RST_SYNTH_COUNTER_RESET_NORMAL             0
#define CUST_LMX2433_R0_RF_RST_SYNTH_COUNTER_RESET_BIT_SHIFT         21


#define CUST_LMX2433_R0_MUX_RF_PLL_DIGITAL_LOCK_DETECT             0x01
#define CUST_LMX2433_R0_MUX_BIT_SHIFT                                22

/* Control Register R1 bits */
#define CUST_LMX2433_R1_RF_B_BIT_SHIFT                                7
#define CUST_LMX2433_R1_RF_A_BIT_SHIFT                                3

#define CUST_LMX2433_R1_RF_SYNTH_PRESCALE_SELECT_8_BY_9               0
#define CUST_LMX2433_R1_RF_SYNTH_PRESCALE_SELECT_16_BY_17             1
#define CUST_LMX2433_R1_RF_SYNTH_PRESCALE_BIT_SHIFT                  22

#define CUST_LMX2433_R1_RF_SYNTH_PD_PLL_ACTIVE                        0
#define CUST_LMX2433_R1_RF_SYNTH_PD_PLL_POWERDOWN                     1
#define CUST_LMX2433_R1_RF_SYNTH_PD_BIT_SHIFT                        23


typedef struct lmx_gpio_config
{
    dword latch_reg;       
    dword latch_m;
    dword latch_hi_v;
    dword latch_lo_v;

    dword clock_reg;       
    dword clock_m;
    dword clock_hi_v;
    dword clock_lo_v;

    dword data_reg;        
    dword data_m;
    dword data_hi_v;
    dword data_lo_v;

} lmx_gpio_config_type;


typedef enum {
    LMX2433_VCO_POSITIVE_POL,
    LMX2433_VCO_NEGATIVE_POL
} lmx2433_vco_polarity_type;


#if defined (RF_HAS_BIO_ENCAPSULATION)
/* HWIO_LOCK/UNLOCK() optimization precludes the need for anything
 * tricky or special being done here for optimization of synchronization
 * of access (eg, INTLOCK/FREE) to gpio registers.
 */
#define LMX2433_BIO_FAST_OUT(io, mask, val) BIO_OUT(io, mask, val)
#else
#define LMX2433_BIO_FAST_OUT(io, mask, val) \
  bio_gpio_image[io].out_buf = (bio_gpio_image[io].out_buf & ~(mask)) \
                                    | ((val)); \
  outpdw(bio_gpio[io].out_addr, bio_gpio_image[io].out_buf); 
#endif /* RF_HAS_BIO_ENCAPSULATION */
  
#endif /* RF_HAS_CUSTOMER_DEFINED_SYNTH */



/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/


/*=========================================================================

FUNCTION CUST_SYNTH_INIT

DESCRIPTION
 Function for customer to initialize the external synthesizer.  This
 function will be invoked when the ZIF library function synth_init() is
 invoked for RF_PATH_1 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cust_synth_init( void );


/*=========================================================================

FUNCTION CUST_SYNTH_SET_MODE

DESCRIPTION
 Function for customer to set the mode of the external synthesizer.  This
 function will be invoked when the ZIF library function synth_set_mode() is
 invoked for RF_PATH_1 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cust_synth_set_mode(rf_card_band_type band, db_rf_mode_type );


/*=========================================================================

FUNCTION CUST_SYNTH_RFRX_PLL_LOAD

DESCRIPTION
 Function for customer to program the PLL in the external synthesizer.  
 This function will be invoked when the ZIF library function 
 synth_rfrx_pll_load() is invoked for RF_PATH_1.  For RFRs on path 1 that
 have internal CDMA VCOs that require coarse tuning the function
 cust_synth_load_rfrx_coarse_tune_val() MUST be invoked from within this
 function.  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cust_synth_rfrx_pll_load(rf_card_band_type band, db_rf_mode_type, dword );

/*=========================================================================

FUNCTION CUST_SYNTH_IS_LOCKED

DESCRIPTION
 This function returns the default TRUE value for the 
 synth lock of PLL for external synth device

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean cust_synth_is_locked( void );

/*=========================================================================

FUNCTION CUST_SYNTH_RFRX_PLL_DISABLE

DESCRIPTION
 Function for customer to disable the RX PLL (if needed) in the external 
 synthesizer.  This function will be invoked when the ZIF library function 
 synth_rfrx_pll_disable() is invoked for RF_PATH_1 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cust_synth_rfrx_pll_disable( void );


/*=========================================================================

FUNCTION CUST_SYNTH_RFRX_PLL_ENABLE

DESCRIPTION
 Function for customer to enable the RX PLL (if needed) in the external 
 synthesizer.  This function will be invoked when the ZIF library function 
 synth_rfrx_pll_enable() is invoked for RF_PATH_1 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cust_synth_rfrx_pll_enable( void );


/*=========================================================================

FUNCTION CUST_SYNTH_RFRX_PLL_LOAD_GIVEN_N

DESCRIPTION
 Function for customer to load the RX PLL in the external synthesizer for
 the given N.  This function is only needed for RFRs on RF_PATH_1 that have
 an internal CDMA VCO that requires tuning.  This function will be invoked 
 from the ZIF library as part of the VCO coarse tuning algorithm.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cust_synth_rfrx_pll_load_given_n( dword );




#ifdef RF_HAS_CUSTOMER_DEFINED_SYNTH

/*===========================================================================
Prototypes for functions/macros used to test the LMX2433 synthesizer as a 
customer defined synthesizer (instead of using the ZIF Library implementation)
==============================================================================*/


/*=========================================================================

FUNCTION CUST_SYNTH_GPIO_INIT

DESCRIPTION
 This function configures the GPIOS used to bit-bang the LMX2433 synthesizer 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cust_synth_gpio_init( void );


/*===========================================================================

FUNCTION LMX2433_GPIO_CONFIG

DESCRIPTION
  This function is configures the GPIOs to be used for the 3-wire serial
  interface to the LMX2433 synthesizer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void lmx2433_gpio_config(gpio_sig_type signal, bio_gpio_type reg, 
                              dword mask, lmx_gpio_config_type* );


/*===========================================================================

FUNCTION LMX2433_STUFF_DATA

DESCRIPTION
  This function is capable of programming an LMX24XX family synthesizer via
  the three-wire serial interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void lmx2433_stuff_data( dword reg, dword mask, dword value, 
                         lmx_gpio_config_type *asd);


/*===========================================================================

FUNCTION LMX2433_ENABLE_GPIOS

DESCRIPTION
  This function enables and initializes the GPIOs that are used for the 3-wire 
  serial interface to the LMX2433 synthesizer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void lmx2433_enable_gpios( lmx_gpio_config_type *asd );


/*===========================================================================

FUNCTION CUST_LMX2433_SYNTH_INIT

DESCRIPTION
  This function initializes the LMX2433 synthesizer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cust_lmx2433_synth_init( lmx_gpio_config_type *lmxdev );


/*===========================================================================

FUNCTION CUST_LMX2433_SYNTH_RFRX_PLL_DISABLE

DESCRIPTION
  This function powers down the PLL in the LMX2433 synthesizer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cust_lmx2433_synth_rfrx_pll_disable( lmx_gpio_config_type *lmxdev );


/*===========================================================================
FUNCTION CUST_LMX2433_SYNTH_RFRX_PLL_ENABLE

DESCRIPTION
  This function enables the PLL in the LMX2433 synthesizer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cust_lmx2433_synth_rfrx_pll_enable( lmx_gpio_config_type *lmxdev );


/*===========================================================================

FUNCTION CUST_LMX2433_SYNTH_SET_MODE

DESCRIPTION
  This function sets the mode of the LMX2433 synthesizer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cust_lmx2433_synth_set_mode(rf_card_band_type band, db_rf_mode_type rf_curr_cdma_band, lmx_gpio_config_type *lmxdev );


/*===========================================================================

FUNCTION CUST_LMX2433_SYNTH_RFRX_PLL_LOAD_GIVEN_N

DESCRIPTION
  This function programs the PLL for a given N value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cust_lmx2433_rfrx_pll_load_given_n( dword N, lmx_gpio_config_type *lmxdev );


/*===========================================================================

FUNCTION CUST_LMX2433_SYNTH_RFRX_PLL_LOAD

DESCRIPTION
  This function programs the PLL in the LMX2433 synthesizer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cust_lmx2433_synth_rfrx_pll_load(rf_card_band_type band, db_rf_mode_type rf_curr_cdma_band, dword channel ,
                                      lmx_gpio_config_type *lmxdev);



/* macro to write a value to a LMX2433 register using the 3-wire serial interface */
#define LMX2433_OUTM( reg, mask, value, serial_dev ) \
    lmx2433_stuff_data(reg, mask, value, serial_dev );


#endif /* RF_HAS_CUSTOMER_DEFINED_SYNTH */

#endif /* RF_CUST_SYNTH_H */

