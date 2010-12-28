/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             C U S T O M E R  E X T E R N A L  S Y NT H  D R I V E R

GENERAL DESCRIPTION
  This file is provided for customers to define a external synth
  not supported by the ZIF library  

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2004   by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_cust_synth.c#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/14/05   sar     Eliminated lint errors.
10/25/05   vm	   Added the function cust_synth_is_locked()
                   which returns default TRUE value for the 
                   synth lock of PLL for external synth device
07/02/04   dbc     Renamed test_cust_synth_init() to cust_synth_gpio_init()
03/12/04   sar     Updated for FEBC.
03/04/04   dbc     Use RF_HAS_CUSTOMER_DEFINED_SYNTH instead of RF_TEST_CUST_SYNTH
02/06/04   dbc     Created
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rf_cust_synth.h"
#include "rfi.h"

/*===========================================================================

                              DATA DEFINITIONS

===========================================================================*/

#ifdef RF_HAS_CUSTOMER_DEFINED_SYNTH
/* test code to show how LMX2433 can be used as a customer defined external
   synthesizer (instead of using the ZIF library implementation            */


static  lmx2433_gps_vco_polarity_type  = LMX2433_VCO_POSITIVE_POL;  

#if defined( T_RF_CZRF6500HT )
#error code not present
#else
static  lmx2433_cdma_vco_polarity_type = LMX2433_VCO_POSITIVE_POL;
#endif

static lmx_gpio_config_type lm2433_gpios;
#endif

/*===========================================================================

                             FUNCTION DEFINITIONS

===========================================================================*/

#ifdef RF_HAS_CUSTOMER_DEFINED_SYNTH
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
void cust_synth_gpio_init( void )
{

   /* enable GPIOs to use for external synthesizer */
#ifndef T_IO_CARD_PANDA
   gpio_tlmm_config(GPIO_OUT_65);
#endif
   gpio_tlmm_config(GPIO_OUT_41);
   gpio_tlmm_config(GPIO_OUT_92);


   /* Serial channel to LMX is GPIO bit-bang */
   lmx2433_gpio_config( GPIO_CLOCK_SIG,
                            BIO_EXT_SYNTH_CLOCK_REG, 
                            BIO_EXT_SYNTH_CLOCK_M,
                            &lm2433_gpios
                       );


   /* data */
   lmx2433_gpio_config( GPIO_DATA_SIG,
                            BIO_EXT_SYNTH_DATA_REG, 
                            BIO_EXT_SYNTH_DATA_M,
                            &lm2433_gpios
                          );

   /* latch */
   lmx2433_gpio_config( GPIO_LATCH_SIG,
                            BIO_EXT_SYNTH_LATCH_REG, 
                            BIO_EXT_SYNTH_LATCH_M,
                            &lm2433_gpios
                          );


   /* enable GPIOs function */
   lmx2433_enable_gpios( &lm2433_gpios );


}

#endif /* RF_HAS_CUSTOMER_DEFINED_SYNTH */



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
void cust_synth_init( void )
{

#ifdef RF_HAS_CUSTOMER_DEFINED_SYNTH
  /* use LMX2433 for testing */
  cust_lmx2433_synth_init( &lm2433_gpios );
#endif

  return;
}


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
void cust_synth_set_mode(rf_card_band_type band, db_rf_mode_type rf_mode )/*lint -esym(715, band) -esym(715, rf_mode)unused */
{

#ifdef RF_HAS_CUSTOMER_DEFINED_SYNTH
  /* use LMX2433 for testing */
  cust_lmx2433_synth_set_mode(band, rf_mode, &lm2433_gpios );
#endif

  return;
}



/*=========================================================================

FUNCTION CUST_SYNTH_RFRX_PLL_DISABLE

DESCRIPTION
 Function for customer to power down the PLL in the external synthesizer.  
 This function will be invoked when the ZIF library function 
 synth_rfrx_pll_disable() is invoked for RF_PATH_1 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cust_synth_rfrx_pll_disable( void )
{

#ifdef RF_HAS_CUSTOMER_DEFINED_SYNTH
  /* use LMX2433 for testing */
  cust_lmx2433_synth_rfrx_pll_disable( &lm2433_gpios );
#endif

  return;
}


/*=========================================================================

FUNCTION CUST_SYNTH_RFRX_PLL_ENABLE

DESCRIPTION
 Function for customer to enable the PLL in the external synthesizer.  
 This function will be invoked when the ZIF library function 
 synth_rfrx_pll_enable() is invoked for RF_PATH_1 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cust_synth_rfrx_pll_enable( void )
{

#ifdef RF_HAS_CUSTOMER_DEFINED_SYNTH
  /* use LMX2433 for testing */
  cust_lmx2433_synth_rfrx_pll_enable( &lm2433_gpios );
#endif

  return;
}



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
void cust_synth_rfrx_pll_load(rf_card_band_type band, db_rf_mode_type rf_mode, dword channel )/*lint -esym(715, band) -esym(715, rf_mode) -esym(715, channel) unused */
{
  /* program synthesizer */

#ifdef RF_HAS_CUSTOMER_DEFINED_SYNTH
  /* use LMX2433 for testing */
  cust_lmx2433_synth_rfrx_pll_load(band, rf_mode, channel, &lm2433_gpios );
#endif


  return;
}

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
boolean cust_synth_is_locked( void )
{
  return TRUE;
}

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
void cust_synth_rfrx_pll_load_given_n( dword N )/*lint -esym(715, N) unused */
{

#ifdef RF_HAS_CUSTOMER_DEFINED_SYNTH
  /* use LMX2433 for testing */
  cust_lmx2433_rfrx_pll_load_given_n( N, &lm2433_gpios );
#endif

  return;
}



#ifdef RF_HAS_CUSTOMER_DEFINED_SYNTH
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
void lmx2433_stuff_data
(
dword reg, 
dword mask,
dword data                   /* Whole 22-bit data value to clock in */,
lmx_gpio_config_type *asd
)
{
    dword reg_mask;          /* MSB of the register to be latched */
    dword synth_data;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* OR in register AND mask */
    synth_data = ( data | reg ) & mask;

    /* --------------------------------------------
    ** Bring EN low (clock in data while EN is low)
    ** -------------------------------------------- */
    LMX2433_BIO_FAST_OUT( asd->latch_reg, asd->latch_m, asd->latch_lo_v );

    /* ----------------------------
    ** Start out with the clock low
    ** ---------------------------- */
    LMX2433_BIO_FAST_OUT( asd->clock_reg, asd->clock_m, asd->clock_lo_v );

    /* -----------------------------------------------------------------
    ** Clock in the whole register.
    ** Initialize reg_mask to the MSB of the register. Shifting the mask
    ** to the right as we clock in the data. Stop after the LSB.
    ** ----------------------------------------------------------------- */

    for ( reg_mask = CUST_LMX2433_MSB_MASK; reg_mask != 0; reg_mask >>= 1 )
    {

        /*
        ** Set data hi/lo depending on masked value.
        */

        if ( (synth_data & reg_mask) != 0 )
        {
            LMX2433_BIO_FAST_OUT( asd->data_reg, asd->data_m, asd->data_hi_v );
        }
        else
        {
            LMX2433_BIO_FAST_OUT( asd->data_reg, asd->data_m, asd->data_lo_v );
        }

        /* ----------------------------------------------------------------
        ** DATA SETUP TIME  50ns min
        ** executing the following code meets this minimum time requirement
        ** Set PLL clock high to latch the data bit.
        ** ---------------------------------------------------------------- */
        LMX2433_BIO_FAST_OUT( asd->clock_reg, asd->clock_m, asd->clock_hi_v );


        /* ----------------------------------------------------------------
        ** CLOCK HIGH TIME  50ns min
        ** executing the following code meets this minimum time requirement
        ** Set PLL clock low so that we can clock in the next data bit.
        ** ---------------------------------------------------------------- */
        LMX2433_BIO_FAST_OUT( asd->clock_reg, asd->clock_m, asd->clock_lo_v );


        /* ----------------------------------------------------------------
        ** CLOCK LOW TIME and 'Clock to Load Enable Set Up Time':  50ns min
        ** Looping back will cover this time requirement.
        ** ---------------------------------------------------------------- */
    }

    /* ----------------------------------------------------------------
    ** 'Clock to Load Enable Setup Time': 50ns min
    ** executing the following code meets this minimum time requirement
    ** Pull LE line high to latch data into the register.
    ** ---------------------------------------------------------------- */
    LMX2433_BIO_FAST_OUT( asd->latch_reg, asd->latch_m, asd->latch_hi_v );
    LMX2433_BIO_FAST_OUT( asd->latch_reg, asd->latch_m, asd->latch_lo_v );
    LMX2433_BIO_FAST_OUT( asd->data_reg, asd->data_m, asd->data_lo_v );


}   /* lmx2433_stuff_data */



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
                              dword mask, lmx_gpio_config_type *asd )
{
    switch(signal)
    {
      case GPIO_CLOCK_SIG:
      {

        asd->clock_reg = reg;
        asd->clock_m = mask;
        asd->clock_hi_v = mask;
        asd->clock_lo_v = 0;

        break;
      }
      case GPIO_DATA_SIG:
      {

        asd->data_reg = reg;
        asd->data_m = mask;
        asd->data_hi_v = mask;
        asd->data_lo_v = 0;

        break;
      }
      case GPIO_LATCH_SIG:
      {

        asd->latch_reg = reg;
        asd->latch_m = mask;
        asd->latch_hi_v = mask;
        asd->latch_lo_v = 0;

        break;
      }
    }

}


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
void lmx2433_enable_gpios( lmx_gpio_config_type *asd )
{
    /* enable the GPIO's used for latch, clock and data */

    BIO_TRISTATE(asd->clock_reg, asd->clock_m,
                 asd->clock_hi_v);
    BIO_TRISTATE(asd->data_reg, asd->data_m,
                 asd->data_hi_v);
    BIO_TRISTATE(asd->latch_reg, asd->latch_m,
                 asd->latch_hi_v);

    /* initialize the lines  */

    BIO_OUT(asd->clock_reg, asd->clock_m,
            asd->clock_hi_v);
    BIO_OUT(asd->data_reg, asd->data_m,
            asd->data_lo_v);
    BIO_OUT(asd->latch_reg, asd->latch_m,
            asd->latch_hi_v);

}


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
void cust_lmx2433_synth_init( lmx_gpio_config_type *lmxdev )
{

  dword general_purpose_disable = 0x00000000;

  /* Registers R0 and R1 are of interest since they program the UHF  */
  LMX2433_OUTM(CUST_LMX2433_R0_RF_R_ADDRESS, CUST_LMX2433_REG_MASK, \
          (CUST_LMX2433_R0_MUX_RF_PLL_DIGITAL_LOCK_DETECT << CUST_LMX2433_R0_MUX_BIT_SHIFT) \
          | (CUST_LMX2433_R0_RF_RST_SYNTH_COUNTER_RESET_NORMAL << CUST_LMX2433_R0_RF_RST_SYNTH_COUNTER_RESET_BIT_SHIFT) \
          | (CUST_LMX2433_R0_RF_CPT_CHARGE_PUMP_TRISTATE_NORMAL << CUST_LMX2433_R0_RF_CPT_CHARGE_PUMP_TRISTATE_BIT_SHIFT) \
          | (CUST_LMX2433_R0_RF_CPG_CHARGE_PUMP_GAIN_1MA << CUST_LMX2433_R0_RF_CPG_CHARGE_PUMP_GAIN_BIT_SHIFT) \
          | (CUST_LMX2433_R0_RF_CPP_PHASE_DETECT_POSITIVE_POLARITY << CUST_LMX2433_R0_RF_CPP_PHASE_DETECT_BIT_SHIFT ) \
          , lmxdev );

  LMX2433_OUTM(CUST_LMX2433_R1_RF_N_ADDRESS, CUST_LMX2433_REG_MASK, \
          (CUST_LMX2433_R1_RF_SYNTH_PD_PLL_POWERDOWN << CUST_LMX2433_R1_RF_SYNTH_PD_BIT_SHIFT ) \
          | (CUST_LMX2433_R1_RF_SYNTH_PRESCALE_SELECT_8_BY_9 << CUST_LMX2433_R1_RF_SYNTH_PRESCALE_BIT_SHIFT ) \
          , lmxdev );

  /* disable RF Synthesizer Time Out Counter(TOC) and FastLock Feature */
  LMX2433_OUTM(CUST_LMX2433_R2_RF_TOC_ADDRESS, CUST_LMX2433_REG_MASK, general_purpose_disable  , lmxdev );

  /* Set the IF R register to all zeros... then powerdown the IF PLL in write to R4 next line */
  LMX2433_OUTM(CUST_LMX2433_R3_IF_R_ADDRESS, CUST_LMX2433_REG_MASK, general_purpose_disable , lmxdev );

  /* Disable the IF PLL */
  LMX2433_OUTM(CUST_LMX2433_R4_IF_N_ADDRESS, CUST_LMX2433_REG_MASK ,general_purpose_disable , lmxdev );


  /* disable IF Synthesizer Time Out Counter(TOC) and FastLock Feature */
  LMX2433_OUTM(CUST_LMX2433_R5_IF_TOC_ADDRESS, CUST_LMX2433_REG_MASK, general_purpose_disable, lmxdev );

}


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
void cust_lmx2433_synth_set_mode(rf_card_band_type band, db_rf_mode_type rf_mode, lmx_gpio_config_type *lmxdev )
{
  dword R = 0;
  int vco_polarity = CUST_LMX2433_R0_RF_CPP_PHASE_DETECT_POSITIVE_POLARITY;

  switch (rf_mode)
  {
  
  case DB_RF_CDMA:

    if ( lmx2433_cdma_vco_polarity_type == LMX2433_VCO_POSITIVE_POL ) {
        vco_polarity = CUST_LMX2433_R0_RF_CPP_PHASE_DETECT_POSITIVE_POLARITY;
    }
    else {
        vco_polarity = CUST_LMX2433_R0_RF_CPP_PHASE_DETECT_NEGATIVE_POLARITY;
    }


    if (band != RF_BC3_BAND)/* US Cellular Band */
    {
       R = 320; 
    } 
    else                    /* JCDMA Band */
    {
       R = 384;
    } 

    break;

  case DB_RF_PCN:

    if ( lmx2433_cdma_vco_polarity_type == LMX2433_VCO_POSITIVE_POL ) {
        vco_polarity = CUST_LMX2433_R0_RF_CPP_PHASE_DETECT_POSITIVE_POLARITY;
    }
    else {
        vco_polarity = CUST_LMX2433_R0_RF_CPP_PHASE_DETECT_NEGATIVE_POLARITY;
    }

    if ( band == RF_BC4_BAND )       /* KPCS Band */
    {
      ; 
    }
    else if ( band == RF_BC6_BAND )   /* IMT Band */
    {
      R = 480;
    }
    else                    /* US PCS Band */
    {
      R = 432; 
    }

    break;

  case DB_RF_GPS:

      if ( lmx2433_gps_vco_polarity_type == LMX2433_VCO_POSITIVE_POL ) {
          vco_polarity = CUST_LMX2433_R0_RF_CPP_PHASE_DETECT_POSITIVE_POLARITY;
      }
      else {
          vco_polarity = CUST_LMX2433_R0_RF_CPP_PHASE_DETECT_NEGATIVE_POLARITY;
      }
    R = 66; 
    break; 
    
  default:
    return;

  }

  /* Registers R0 and R1 are of interest since they program the UHF  */
  LMX2433_OUTM(CUST_LMX2433_R0_RF_R_ADDRESS, CUST_LMX2433_REG_MASK, \
          (CUST_LMX2433_R0_MUX_RF_PLL_DIGITAL_LOCK_DETECT << CUST_LMX2433_R0_MUX_BIT_SHIFT) \
          | (CUST_LMX2433_R0_RF_RST_SYNTH_COUNTER_RESET_NORMAL << CUST_LMX2433_R0_RF_RST_SYNTH_COUNTER_RESET_BIT_SHIFT) \
          | (CUST_LMX2433_R0_RF_CPT_CHARGE_PUMP_TRISTATE_NORMAL << CUST_LMX2433_R0_RF_CPT_CHARGE_PUMP_TRISTATE_BIT_SHIFT) \
          | (CUST_LMX2433_R0_RF_CPG_CHARGE_PUMP_GAIN_1MA << CUST_LMX2433_R0_RF_CPG_CHARGE_PUMP_GAIN_BIT_SHIFT) \
          | ( vco_polarity << CUST_LMX2433_R0_RF_CPP_PHASE_DETECT_BIT_SHIFT ) \
          | (R << CUST_LMX2433_R0_RF_R_BIT_SHIFT  ) \
          , lmxdev );

}


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
void cust_lmx2433_synth_rfrx_pll_load(rf_card_band_type band, db_rf_mode_type rf_mode, dword channel ,
                                      lmx_gpio_config_type *lmxdev)
{
  dword N = 0;
  dword P = CUST_LMX2433_PRESCALER_8;
  dword A = 0;
  dword B = 0;
  switch (rf_mode)
  {
  
  case DB_RF_CDMA:

    if (band != RF_BC3_BAND)   /* US Cellular Band */
    {

      if ( (channel >= 991) && (channel <= 1023) )
      {
        N = 29000 + (channel - 1023);
      }
      else if ( ( channel > 0 ) && ( channel <= 799 ) )
      {
        N = 29000 + channel;
      }
      else
      {
        return;
      }

    } 
    else                 /* JCDMA Band */
    {
      /* only even channels are acceptable, make sure
         channel is even */
      if ( (channel%2) != 0) return;

      /* Compute N */
      if ( (channel >= 1041) && (channel <= 1199) )
      {
        N = 33280 + (( channel - 1040)/2);
      }
      else if ((channel >= 1201) && (channel <= 1600))
      {
        N = 33520 + (( channel - 1200)/2);
      }
      else if ((channel >= 801) && (channel <= 1039))
      {
        N = 33720 + (( channel - 800)/2);
      }
      else if ((channel >=1 ) && (channel <=799))
      {
        N = 34400 + (channel/2);
      }

    } 

    B = N/P;
    A = N - (B * P);

    break;

  case DB_RF_PCN:

    if (band == RF_BC6_BAND )        /* IMT Band */
    {
      if ( channel < 1200 )
      {
        N = 42200 + channel;
      } 
      else return;
    }
    else if ( band == RF_BC4_BAND )  /* KPCS Band */
    {
      if ( channel < 600 )
      {
      } 
      else return;
    }
    else                    /* US PCS Band */
    {
      if ( channel < 1200 )
      {
        N = 38600 + channel;
      } 
      else return;

    } 

    B = N/P;
    A = N - (B * P);

    break;

  case DB_RF_GPS:
    N = 10831;
    B = 1353;
    A = 7;
    break; 
    
  default:
    return;

  }
  LMX2433_OUTM(CUST_LMX2433_R1_RF_N_ADDRESS,CUST_LMX2433_REG_MASK,   \
          (CUST_LMX2433_R1_RF_SYNTH_PD_PLL_ACTIVE << CUST_LMX2433_R1_RF_SYNTH_PD_BIT_SHIFT ) \
          | (CUST_LMX2433_R1_RF_SYNTH_PRESCALE_SELECT_8_BY_9 << CUST_LMX2433_R1_RF_SYNTH_PRESCALE_BIT_SHIFT) \
          | (B << CUST_LMX2433_R1_RF_B_BIT_SHIFT) \
          | (A << CUST_LMX2433_R1_RF_A_BIT_SHIFT), \
          lmxdev );


#if defined( T_RF_CZRF6500HT )
#error code not present
#endif


}


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
void cust_lmx2433_rfrx_pll_load_given_n( dword N, lmx_gpio_config_type *lmxdev )
{
    dword A = 0;
    dword B = 0;
    dword P = CUST_LMX2433_PRESCALER_8;

    B = N/P;
    A = N - (B * P);


    LMX2433_OUTM(CUST_LMX2433_R1_RF_N_ADDRESS,CUST_LMX2433_REG_MASK,   \
            (CUST_LMX2433_R1_RF_SYNTH_PD_PLL_ACTIVE << CUST_LMX2433_R1_RF_SYNTH_PD_BIT_SHIFT ) \
            | (CUST_LMX2433_R1_RF_SYNTH_PRESCALE_SELECT_8_BY_9 << CUST_LMX2433_R1_RF_SYNTH_PRESCALE_BIT_SHIFT) \
            | (B << CUST_LMX2433_R1_RF_B_BIT_SHIFT) \
            | (A << CUST_LMX2433_R1_RF_A_BIT_SHIFT), \
            lmxdev );
}


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
void cust_lmx2433_synth_rfrx_pll_disable( lmx_gpio_config_type *lmxdev )
{
  LMX2433_OUTM(CUST_LMX2433_R1_RF_N_ADDRESS, CUST_LMX2433_REG_MASK, \
          (CUST_LMX2433_R1_RF_SYNTH_PD_PLL_POWERDOWN << CUST_LMX2433_R1_RF_SYNTH_PD_BIT_SHIFT ) \
          , lmxdev );

}



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
void cust_lmx2433_synth_rfrx_pll_enable( lmx_gpio_config_type *lmxdev )
{
  LMX2433_OUTM(CUST_LMX2433_R1_RF_N_ADDRESS, CUST_LMX2433_REG_MASK, \
          (CUST_LMX2433_R1_RF_SYNTH_PD_PLL_ACTIVE << CUST_LMX2433_R1_RF_SYNTH_PD_BIT_SHIFT ) \
          , lmxdev );

}

#endif

