/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      FTM RF MODE INITILIZATION

GENERAL DESCRIPTION
  Routines for initializing the RF chipsets when switching modes.

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/ftm/ftm_sys_ctl.c#5 $ 
  $DateTime: 2008/12/04 16:23:46 $ 
  $Author: vmahajan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/03/08   vm      Added the Support for Temperature based Device
                   Configration change
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
07/24/08   ems     Corrected compiler warnings
06/13/08   vm      Turning the tx OFF and setting waveform to CDMA before
                   moving out of CDMA Mode
03/12/08   vm      Added Support for supporting Single chain on QSC1100
11/21/07   vm      Added More FTM Support for CGPS mode switching
11/13/07   anr	   Adding ftm_set_calibration_state(), ftm_get_calibration_state(),
					ftm_enter_calibration() and ftm_exit_calibration()	
11/09/07   ans     Removed MDSP writes as MDSP is not available. MDSP is 
                   expected to get initialized when phone switches from FTM
                   to online.
11/08/07   vm      Added support for FTM API supporting CDMA Waveform, 
                   CW Waveform and Offset CW Waveform for Tx
08/24/07   vm      Add the function to return TRUE if current FTM Mode is 
                   RF Cal Mode 
08/01/07   kg     featurization changes for EDGE only support
06/02/07   vm      Added the support for Beta Scaling for Sc2X
02/26/07   vm      Merge for Common archive
01/15/06   vm      Made changes for RX Front Clock Management to be 
                   based on voting mechanism in clock regime
12/18/06   jkl     Fixed featurization for HDR.
11/22/06   vm      Added the support for Firmware TX AGC
10/12/06   vm      Merged RX Front chnages for SC2X into the mainline code
06/14/06   ak/ra   For CDMA2000, set RPC to normal upon exit
03/07/06   go      Fixed RxAGC. Removed enable diversity.
11/29/05   ra      Only execute ftm_rfmode_set() when in FTM_MODE 
11/07/05   vm      Fixed the problem of phone resetting in FTM Mode when 
                   switching from HDR testing to RF CDMA testing
09/09/05   ra      Added call to initialize srch in ftm_rfmode_clk_init
                   for 6800
07/23/05   gco     Created.
===========================================================================*/




/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_sys_ctl.h"
#include "ftm_1x_ctl.h"

#include "ftmicap.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "ftm_task.h"
#include "assert.h"
#include "task.h"


#include "msm_help.h"

#ifdef FTM_HAS_UMTS
#error code not present
#endif /* FTM_HAS_UMTS */

#ifdef FTM_HAS_1X
#include "rfm.h"
#include "mcc.h"
#include "mccdma.h"
#include "rficap.h"
#include "ftm_cmdi.h"
#include "srchmc.h"

#include "srch_mdsp.h"

#include "cagc.h"

#ifdef RF_HAS_MDSP_TX_AGC
#include "rf_mdsp_tx_agc.h"
#endif

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FTM_HAS_CGPS
#error code not present
#endif 

#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */

#endif /* FTM_HAS_1X */

#ifndef CLKRGM_H
#error must define CLKRGM_H
#else
#include CLKRGM_H
#endif

#if defined(FTM_HAS_GSM) && defined(FTM_HAS_1X)
#error code not present
#endif

#ifdef RF_HAS_PDM_VS_POWER_TX_CAL
#include "rfagc_tx_pdm_vs_pwr_cal.h"
#endif /* RF_HAS_PDM_VS_POWER_TX_CAL */


extern void ftm_gsm_rx_disable(void);
extern void ftm_set_tx(rfcom_device_enum_type device, boolean state);
extern void ftm_set_cdma_cw(boolean on_off);

extern ftm_rf_mode_type ftm_current_rf_mode; // ftm.c

#if defined (RF_HAS_MDSP_TX_AGC) || defined(RF_HAS_PDM_VS_POWER_TX_CAL)
extern boolean rf_cdma_load_master_lin_tables;
#endif

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */


#ifdef FTM_HAS_CGPS
#error code not present
#endif

/*===========================================================================
 
                         LOCAL VARIABLES AND FUNCTIONS

===========================================================================*/

/* --- rf modes/states */
typedef enum
{
  FTM_RFMODE_UNKNOWN = -1,   /* unknown rf mode */
  FTM_RFMODE_PARK,           /* rf in park mode */
  FTM_RFMODE_1X,             /* rf in non-signalling cdma 1x mode */
  FTM_RFMODE_GSM,            /* rf in gsm mode */
  FTM_RFMODE_WCDMA,          /* rf in wcdma/umts mode */
  FTM_RFMODE_1X_CAL,         /* rf in cdma 1x cal mode */
  FTM_RFMODE_GSM_CAL,        /* rf in gsm cal mode */
  FTM_RFMODE_WCDMA_CAL,      /* rf in umts/wcdma mode */
  FTM_RFMODE_HDR,            /* rf in hdr mode */
  FTM_RFMODE_MAX
} FTM_RFMODE_T;


/* --- support functions */
static void ftm_rfmode_set(FTM_RFMODE_T mode); 
static void ftm_rfmode_park_enter(void);    /* configure to park mode */
static void ftm_rfmode_1x_enter(void);      /* configure to 1x (from park mode) */
static void ftm_rfmode_1x_exit(void);       /* cleanup for 1x (to park mode) */
static void ftm_rfmode_gsm_enter(void);     /* configure to gsm (from park mode) */
static void ftm_rfmode_gsm_exit(void);      /* cleanup for gsm (to park mode) */
static void ftm_rfmode_wcdma_enter(void);   /* configure to umts (from park mode) */
static void ftm_rfmode_wcdma_exit(void);    /* cleanup for umts (to park mode) */
static void ftm_rfmode_hdr_enter(void);      /* configure to hdr (from park mode) */
static void ftm_rfmode_hdr_exit(void);       /* cleanup for hdr (to park mode) */

static void ftm_enter_calibration(void);	/* Procedure to execute when calibration state is active */
static void ftm_exit_calibration(void);		/* Procedure to execute when calibration state is inactive */
	
#ifdef FTM_HAS_1X
static void ftm_rfmode_clk_init(void);      /* Initialize the clocks and HW for FTM */
#endif //FTM_HAS_1X

static FTM_RFMODE_T  ftm_rfmode_current = FTM_RFMODE_UNKNOWN; /* current RF mode */
static ftm_cal_state_type ftm_current_cal_state = CAL_STATE_INACTIVE; // current calibration state


/*===========================================================================

FUNCTION ftm_rfmode_exit

DESCRIPTION  
   Get out of FTM mode

DEPENDENCIES
   None
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
void ftm_rfmode_exit()
{
  #ifdef FTM_HAS_CGPS
#error code not present
  #endif
  ftm_rfmode_current = FTM_RFMODE_UNKNOWN;

#ifdef FEATURE_HDR  
#error code not present
#endif /*FEATURE_HDR*/
  /* Enable RPC, in case it has been disabled by the FTM param enable_rpc */

  #if defined (RF_HAS_MDSP_TX_AGC) || defined(RF_HAS_PDM_VS_POWER_TX_CAL)
  rf_cdma_load_master_lin_tables =  TRUE;
  #endif

  MSM_LOCK();    /* Lock interrupts during MSM register access */
  
  #ifdef RF_HAS_MDSP_TX_AGC
  #ifdef RF_HAS_BETA_SCALING_TX_AGC
  /* Disable Beta Scaling override (if enabled via ftm_cdma_set_tx_agc()) */
  CDMA_MDSP_SET_TX_BETAP_NORMAL();
  #endif
  /* Disable TX AGC ADJ OVERRIDE (if enabled via ftm_cdma_set_tx_agc()) */
  CDMA_MDSP_SET_TX_AGC_ADJ_NORMAL();
  
  CDMA_MDSP_SET_TX_CLOSED_LOOP_NORMAL();
  #else    
  MSM_SET( TX_AGC_CTL2, PWR_CTL_EN );  /* Enable closed-loop power control */
  #endif /* RF_HAS_MDSP_TX_AGC */              
  
  MSM_UNLOCK();    /* Unlock interrupts */

  ftm_set_tx_waveform(CDMA_WAVEFORM,CW_OFFSET_ZER0_KHZ);
  ftm_set_tx(RFCOM_TRANSCEIVER_0,0);

  #ifdef RF_HAS_SUB_BLOCK_SUPPORT
  rf_clear_block_mask_override(RFCOM_TRANSCEIVER_0);
  rf_clear_block_mask_override(RFCOM_RECEIVER_1);
  #endif

}


/*===========================================================================

FUNCTION ftm_rfmode_1x

DESCRIPTION  
   Sets the RF to CDMA 1X mode. 

DEPENDENCIES
   None
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
void ftm_rfmode_1x()
{
  ftm_rfmode_set(FTM_RFMODE_1X);
}


/*===========================================================================

FUNCTION ftm_rfmode_gsm

DESCRIPTION  
   Sets the RF to GSM mode. 

DEPENDENCIES
   None
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
void ftm_rfmode_gsm()
{
  ftm_rfmode_set(FTM_RFMODE_GSM);
}


/*===========================================================================

FUNCTION ftm_rfmode_wcdma

DESCRIPTION  
   Sets the RF to WCDMA/UMTS mode. 

DEPENDENCIES
   None
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
void ftm_rfmode_wcdma()
{
  ftm_rfmode_set(FTM_RFMODE_WCDMA);
}


/*===========================================================================

FUNCTION ftm_rfmode_1x_cal

DESCRIPTION  
   Sets the RF to CDMA 1X calibration mode. 

DEPENDENCIES
   None
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
void ftm_rfmode_1x_cal()
{
  ftm_rfmode_set(FTM_RFMODE_1X_CAL);
}



/*===========================================================================

FUNCTION ftm_rfmode_gsm_cal

DESCRIPTION  
   Sets the RF to GSM calibration mode. 

DEPENDENCIES
   None
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
void ftm_rfmode_gsm_cal()
{
  ftm_rfmode_set(FTM_RFMODE_GSM_CAL);
}


/*===========================================================================

FUNCTION ftm_rfmode_wcdma_cal

DESCRIPTION  
   Sets the RF to UMTS/WCDMA calibration mode. 

DEPENDENCIES
   None
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
void ftm_rfmode_wcdma_cal()
{
  ftm_rfmode_set(FTM_RFMODE_WCDMA_CAL);
}


/*===========================================================================

FUNCTION ftm_rfmode_hdr

DESCRIPTION  
   Sets the RF to HDR mode. 

DEPENDENCIES
   None
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
void ftm_rfmode_hdr()
{
  ftm_rfmode_set(FTM_RFMODE_HDR);
}

/*===========================================================================

FUNCTION ftm_rfmode_set

DESCRIPTION  
   State machine that actually sets the RF mode

DEPENDENCIES
   None
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
static void ftm_rfmode_set(FTM_RFMODE_T mode)
{
  if(ftm_mode != FTM_MODE)
  {
    return;
  }

  if (ftm_rfmode_current==mode)
  {
    /* do nothing */
    return;
  }
  else if (ftm_rfmode_current==FTM_RFMODE_1X_CAL && mode==FTM_RFMODE_1X)
  {
    FTM_MSG_MED("Transition RF 1x Cal to CDM2K",0,0,0);

    #ifdef FTM_HAS_CGPS  
#error code not present
    #endif

#ifdef FTM_HAS_1X
    /* Restrore LNA range control */
//#ifdef FEATURE_MULTI_RX_CHAIN
    RF_LNA_STATE_MACHINE_NORMAL(RFCOM_TRANSCEIVER_0);
/*#else
    RF_LNA_STATE_MACHINE_NORMAL();
#endif*/

    /* Restore TX AGC control to normal mode */
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
    {
        #ifdef RF_HAS_MDSP_TX_AGC
        CDMA_MDSP_SET_PA_STATE_NORMAL();
        CDMA_MDSP_SET_PA_RANGE_NORMAL();
        #else
        MSM_W_MASKED(TX_AGC_CTL, PA_R_OVERRIDE_N, 1); /* Set PA Range Normal */
        #endif
#ifdef T_MSM6700
        #ifdef RF_HAS_MDSP_TX_AGC
        MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_SEL, 2);//Select the PDM Source to be Firmware
        #else
        MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_SEL, 0);
        #endif        
#else
      MSM_W_MASKED(TX_AGC_CTL, TX_AGC_ADJ_OVERRIDE_N, 1); /* TX AGC ADJ override disable */
#endif

      #ifdef RF_HAS_MDSP_TX_AGC
      #ifdef RF_HAS_BETA_SCALING_TX_AGC
      /* Disable Beta Scaling override (if enabled via ftm_cdma_set_tx_agc()) */
      CDMA_MDSP_SET_TX_BETAP_NORMAL();
      #endif

     /* Disable TX AGC ADJ OVERRIDE (if enabled via ftm_cdma_set_tx_agc()) */
      CDMA_MDSP_SET_TX_AGC_ADJ_NORMAL();     
      #endif

    }
  
    ftm_set_tx(RFCOM_TRANSCEIVER_0,OFF);
    ftm_set_cdma_cw(OFF);    

    rfm_disable_rx( RFCOM_TRANSCEIVER_0 );

#endif  //FTM_HAS_1X
  
    ftm_rfmode_current = mode;
    return;
  }
  else if (ftm_rfmode_current==FTM_RFMODE_1X && mode==FTM_RFMODE_1X_CAL)
  {
#ifdef FTM_HAS_1X
    /* safe transition ... just guarantee enabled rx */
    rfm_enable_rx( RFCOM_TRANSCEIVER_0, NULL, NULL );
 
    rfm_init_digital_band_chan(RFCOM_TRANSCEIVER_0, rf_card.rfc_default_band, 300);
    rfm_complete_sleep_warmup(RFCOM_TRANSCEIVER_0);
#endif  //FTM_HAS_1X
    ftm_rfmode_current = mode;
    return;
  }
   
  /* --- cleanup */
  switch(ftm_rfmode_current)
  {
  case FTM_RFMODE_1X:
  case FTM_RFMODE_1X_CAL:
    ftm_rfmode_1x_exit();
    break;
  
  case FTM_RFMODE_GSM:
  case FTM_RFMODE_GSM_CAL:
    ftm_rfmode_gsm_exit();
    break;
  
  case FTM_RFMODE_WCDMA:
  case FTM_RFMODE_WCDMA_CAL:
    ftm_rfmode_wcdma_exit();
    break;

  case FTM_RFMODE_HDR:
    ftm_rfmode_hdr_exit();
    break;

  default:
    break;
  }

  /* --- transition to park mode */
  if (ftm_rfmode_current != FTM_RFMODE_PARK)
  {
    ftm_rfmode_current = FTM_RFMODE_PARK;
    ftm_rfmode_park_enter();
  }

  /* --- set mode here */    
  switch(mode)
  {
  case FTM_RFMODE_1X:
  case FTM_RFMODE_1X_CAL:
    ftm_rfmode_1x_enter();
    break;
  
  case FTM_RFMODE_GSM:
  case FTM_RFMODE_GSM_CAL:
    ftm_rfmode_gsm_enter();
    break;
  
  case FTM_RFMODE_WCDMA:
  case FTM_RFMODE_WCDMA_CAL:
    ftm_rfmode_wcdma_enter();
    break;
  
  case FTM_RFMODE_HDR:
    ftm_rfmode_hdr_enter();
    break;

  default:
    mode = FTM_RFMODE_PARK;
    break;
  }

  ftm_rfmode_current = mode;
}


/*===========================================================================

FUNCTION ftm_rfmode_park_enter
DESCRIPTION  
   Configure the RF to PARK mode

DEPENDENCIES
   None
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
static void ftm_rfmode_park_enter(void)
{
  FTM_MSG_MED("RF mode idle",0,0,0);
}


/*===========================================================================

FUNCTION ftm_rfmode_1x_enter

DESCRIPTION  
   Configure the RF to CDMA 1X mode

DEPENDENCIES
   RF must be in PARK mode before calling this function.

RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
static void ftm_rfmode_1x_enter(void)
{
#ifdef FTM_HAS_1X
  FTM_MSG_MED("Entering RF mode 1x",0,0,0);

#ifdef FEATURE_HDR  
#error code not present
#endif /*FEATURE_HDR*/
    ftm_rfmode_clk_init(); /* Turn on the clocks for FTM RF Commands */ 

    #ifdef RF_HAS_MULTI_RX_CHAIN
    rfm_enter_mode(RFCOM_TRANSCEIVER_0, RFCOM_1X_MODE, NULL, NULL);
    #else
    rfm_enter_mode(RFCOM_1X_MODE, NULL, NULL);
    #endif

    rfm_enable_rx( RFCOM_TRANSCEIVER_0, NULL, NULL );
    ftm_fwd_curr_state = ftm_rev_curr_state = FTM_ENTER; //FTM_INIT_CDMA;
  
  rfm_init_digital_band_chan(RFCOM_TRANSCEIVER_0, rf_card.rfc_default_band, 300);
  rfm_complete_sleep_warmup(RFCOM_TRANSCEIVER_0);
#endif //FTM_HAS_1X
}


/*===========================================================================

FUNCTION ftm_rfmode_1x_exit

DESCRIPTION  
   Clean-up procedure for CDMA 1X.

DEPENDENCIES
   RF must be in CDMA 1X mode before calling this function.

RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
static void ftm_rfmode_1x_exit(void)
{
#ifdef FTM_HAS_1X
  FTM_MSG_MED("Leaving RF mode 1x",0,0,0);
  
  #ifdef FTM_HAS_CGPS
#error code not present
  #endif

  #ifdef RF_HAS_SUB_BLOCK_SUPPORT  
  rf_clear_block_mask_override(RFCOM_TRANSCEIVER_0);
  rf_clear_block_mask_override(RFCOM_RECEIVER_1);
  #endif

  if (ftm_rfmode_current==FTM_RFMODE_1X)
  {
    // deactivate searcher
    srch_cmd_type cmd;

    cmd.hdr.cmd = SRCH_DEACTIVATE_F;
    cmd.hdr.cmd_hdr.task_ptr = NULL;
    cmd.hdr.cmd_hdr.done_q_ptr = NULL;
    /* Send Command to Search task */
    srch_cmd( &cmd );  
  }

#ifdef RF_HAS_MDSP_TX_AGC
#ifdef RF_HAS_BETA_SCALING_TX_AGC
  /* Disable Beta Scaling override (if enabled via ftm_cdma_set_tx_agc()) */
  CDMA_MDSP_SET_TX_BETAP_NORMAL();
#endif

  /* Disable TX AGC ADJ OVERRIDE (if enabled via ftm_cdma_set_tx_agc()) */
  CDMA_MDSP_SET_TX_AGC_ADJ_NORMAL();
#endif

ftm_set_cdma_cw(OFF);

#ifdef FTM_HAS_GSM  
#error code not present
#endif  //FTM_HAS_GSM

#ifdef FEATURE_FTM_HWTC
#error code not present
#endif 

#ifdef FEATURE_FTM_PARK_MODE_SUPPORTED
  rfm_exit_mode(RFCOM_TRANSCEIVER_0, RFCOM_1X_MODE);
#endif //FEATURE_FTM_PARK_MODE_SUPPORTED

  ftm_current_rf_mode = FTM_DB_RF_IDLE;
#endif //FTM_HAS_1X
}


/*===========================================================================

FUNCTION ftm_rfmode_gsm_enter

DESCRIPTION  
   Configure the RF to GSM mode

DEPENDENCIES
   RF must be in PARK mode before calling this function.
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
static void ftm_rfmode_gsm_enter(void)
{
#ifdef FTM_HAS_GSM
#error code not present
#endif //FTM_HAS_GSM
}


/*===========================================================================

FUNCTION ftm_rfmode_gsm_exit

DESCRIPTION  
   Clean-up procedure for GSM.

DEPENDENCIES
   RF must be in GSM mode before calling this function.
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
static void ftm_rfmode_gsm_exit(void)
{
#ifdef FTM_HAS_GSM
#error code not present
#endif //FTM_HAS_GSM
}


/*===========================================================================

FUNCTION ftm_rfmode_wcdma_enter

DESCRIPTION  
   Configure the RF to UMTS/WCDMA mode

DEPENDENCIES
   RF must be in PARK mode before calling this function.
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
static void ftm_rfmode_wcdma_enter(void)
{
#ifdef FTM_HAS_UMTS
#error code not present
#endif //FTM_HAS_UMTS
}


/*===========================================================================

FUNCTION ftm_rfmode_wcdma_exit

DESCRIPTION  
   Clean-up procedure for WCDMA.

DEPENDENCIES
   RF must be in WCDMA mode before calling this function.
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
static void ftm_rfmode_wcdma_exit(void)
{
#ifdef FTM_HAS_UMTS
#error code not present
#endif // FTM_HAS_UMTS
}



/*===========================================================================

FUNCTION ftm_rfmode_hdr_enter

DESCRIPTION  
   Configure the RF to HDR mode

DEPENDENCIES
   RF must be in PARK mode before calling this function.
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
static void ftm_rfmode_hdr_enter(void)
{
#ifdef FEATURE_HDR
#error code not present
#endif //FEATURE_HDR
}



/*===========================================================================

FUNCTION ftm_rfmode_hdr_exit

DESCRIPTION  
   Clean-up procedure for HDR.

DEPENDENCIES
   RF must be in HDR mode before calling this function.
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
static void ftm_rfmode_hdr_exit(void)
{
#ifdef FEATURE_HDR
#error code not present
#endif // FEATURE_HDR
}



#ifdef FTM_HAS_1X
/*===========================================================================

FUNCTION ftm_rfmode_clk_init

DESCRIPTION  
   Initialize the clocks and HW for FTM.

DEPENDENCIES
   None.
   
RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
static void ftm_rfmode_clk_init()
{

  clk_enable();
  
  #ifdef RF_HAS_RXF_V2
  rxf_enable_rx_front_clocks_sc2x(RF_PATH_0);/* Turn On the RX Front clock for Primary path */
  rxf_enable_rx_front_clocks_sc2x(RF_PATH_1);/* Turn On the RX Front clock for secondary path */
  #else
#error code not present
  #endif  
  
#if defined(T_MSM6800)
  clk_regime_enable( CLK_RGM_MODEM_M);
  srch_mdsp_pre1x();
  /* check to see if the MDSP app is ready and valid */
  if ( !srch_mdsp_demod_running() )
  {
    /* download the mdsp image for searcher4 and demod */
    srch_mdsp_download();
  }
#endif //T_MSM6800
}
#endif //FTM_HAS_1X

/*===========================================================================

FUNCTION FTM_CURRENT_MODE_IS_RF_CAL

DESCRIPTION  
   Determines if current ftm Mode is RF Cal Mode or not.

DEPENDENCIES
   None.

RETURN VALUE
   returns TRUE if FTM Mode is RF Cal Mode and FALSE Otherwise

SIDE EFFECTS
   None
   
===========================================================================*/
boolean ftm_current_mode_is_rf_cal()
{
  if (ftm_rfmode_current == FTM_RFMODE_1X_CAL) 
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*===========================================================================

FUNCTION FTM_SET_CALIBRATION_STATE

DESCRIPTION  
   Function to Activate/Inactivate calibration state.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
   
===========================================================================*/
void ftm_set_calibration_state(ftm_cal_state_type on_off)
{
	
	if (on_off == CAL_STATE_ACTIVE) 
	{
       FTM_MSG_MED("Enabled Calibration State using command FTM_SET_CALIBRATION_STATE",0,0,0);

        ftm_current_cal_state = CAL_STATE_ACTIVE;
        
        ftm_enter_calibration();
	}
	else if (on_off == CAL_STATE_INACTIVE) 
    {
        FTM_MSG_MED("Disabled Calibration State using command FTM_SET_CALIBRATION_STATE",0,0,0);

        ftm_current_cal_state = CAL_STATE_INACTIVE;
        ftm_exit_calibration();
	}
	else
	{
		/*Throw Error*/
	}
}

/*===========================================================================

FUNCTION FTM_GET_CALIBRATION_STATE

DESCRIPTION  
   Function to return the current calibration state.

DEPENDENCIES
   None

RETURN VALUE
   cal_state_type 

SIDE EFFECTS
   None
   
===========================================================================*/

ftm_cal_state_type ftm_get_calibration_state(void)
{
	return ftm_current_cal_state;
}

/*===========================================================================

FUNCTION FTM_ENTER_CALIBRATION

DESCRIPTION  
   Procedure to execute when calibration state is ACTIVE.

DEPENDENCIES
   None

RETURN VALUE
   cal_state_type 

SIDE EFFECTS
   None
   
===========================================================================*/
static void ftm_enter_calibration()
{

   #ifdef RF_HAS_PDM_VS_POWER_TX_CAL
      /* When calibration of the phone is active
      ** reset the TX PDM Vs Power Data for the transmitter */
      rfagc_reset_tx_pdm_vs_pwr_cal();
   #endif   
   rf_device_rf_cal_mode_is_enabled(TRUE);

}

/*===========================================================================

FUNCTION FTM_EXIT_CALIBRATION

DESCRIPTION  
   Procedure to execute when calibration state is INACTIVE.

DEPENDENCIES
   None

RETURN VALUE
   cal_state_type 

SIDE EFFECTS
   None
   
===========================================================================*/
static void ftm_exit_calibration()
{
  rf_device_rf_cal_mode_is_enabled(FALSE);  
}
#endif // FEATURE_FACTORY_TESTMODE

