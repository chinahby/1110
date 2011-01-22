 #ifndef FTM_RF_CMD_H
#define FTM_RF_CMD_H
/*===========================================================================

           F T M  R F  C M D    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the FTM Services module.

REFERENCES
  None

Copyright (c) 2003, 2004, 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2007, 2008             by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/ftm/ftm_rf_cmd.h#6 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/08   vm      Changed the name to FTM_SET_BLOCK_MASK
09/30/08   vb/vm   Added Support for Band Class Sub Block Implementation
06/11/08   sl      Added support for UINT16 HDET under new API FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2=144
                   and under feature RF_HAS_HDET_UINT16.
04/18/08   vb      Added FTM_BETA_PRIM_SCALING and FTM_PRIM_CHAIN_SLEEP
11/09/07   anr	   Added FTM_SET_CALIBRATION_STATE		
11/08/07   vm      Added support for FTM API supporting CDMA Waveform, 
                   CW Waveform and Offset CW Waveform for Tx
05/16/07    anr	   Added FTM_DO_XO_DC_CAL and FTM_DO_XO_FT_CURVE_CAL	
05/09/07   lcl     Added FTM_TX_CS_SWEEP
12/08/06   rsr     Added FTM_GSM_DETECTOR_CAL
04/07/06   lcl     Added FTM_DO_DC_CAL for WCDMA interband.
03/28/06   rsr     Added FTM_RX_GAIN_RANGE_CAL
03/15/06   rsr     Added FTM_TX_RSB_DC_SWEEP
02/07/06    ka     Added FTM_DO_INTERNAL_DEVICE_CAL.
12/22/05    ka     Added FTM_SET_TX_SWEEP_STEP_SIZE.
11/17/05    ka     Added FTM_LOAD_RF_NV.
09/12/05   hkk     Changes for TX/RX sweep cal - Added id 130 
08/25/05   dbc     Removed MFLO commands. They are now in ftm_mf_dispatch.h.
08/23/05   jfb     Changed cmd's to reflect current MFLO FTM API document
08/09/05   dbc     Added FTM_MF_AGC_ENABLE and FTM_MF_AGC_DISABLE
07/09/05   dbc     Added some more commands for MFLO
06/22/05   dbc     Added some more commands for MFLO
05/02/05   dbc     Added commands for MFLO.
03/20/05   bmg     Added FTM_SET_RF_POWER_MODE API.
03/11/05   ka      Added FTM_GSM_POLAR_TX_SWEEP_CAL.
03/04/05   rsr     Added support for OPLL transmit and RTR6250 RSB calibration
10/05/04   ka      Added support for FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL.
09/07/04   lcl     Added get IM2 with suppression subcommand.
08/19/04   ra      Merged UMTS with 1x targets. Removed Amps support
08/20/04    ka     Added command to configure tx sweep cal.
04/15/03    xw     Initial revision

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"              /* Target specific definitions            */
#include "comdef.h"              /* Definitions for byte, word, etc.       */

/* <EJECT> */
/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_FACTORY_TESTMODE

/*****************************************************
**
**     s U B - E NU M E R A T I O N
**
**====================================================
**
** This sub-enumeration is used to distinguish between different
** cdmaOne/CDMA2000/WCDMA and some of GSM tests
**
**/ 

typedef enum
{
  FTM_SET_PDM,              
  FTM_SET_PDM_CLOSE,        
  FTM_SET_TX_ON,            
  FTM_SET_TX_OFF,           
  FTM_SET_MODE = 7,             
  FTM_SET_CHAN,             
  FTM_GET_RSSI,             
  FTM_GET_SYNTH_STATE = 16,      
  FTM_SET_CDMA_CW_WAVEFORM = 50,     
  FTM_GET_CAGC_RX_AGC = 53,
  FTM_SET_PA_RANGE = 54,
  FTM_SET_LNA_RANGE = 58,
  FTM_GET_ADC_VAL = 59,         
  FTM_SET_LNA_OFFSET = 81,       
  FTM_SET_DVGA_OFFSET = 111,      
  FTM_GET_CDMA_IM2 = 114,             
  FTM_GSM_IM2 = FTM_GET_CDMA_IM2, /*   114 - FTM GSM IM2 calibration */
  FTM_TX_SWEEP_CAL = 116,       
  FTM_GET_DVGA_OFFSET = 117,
  FTM_GET_LNA_OFFSET=118,
  FTM_GET_HDET_FROM_TX_SWEEP_CAL=119,
  FTM_SET_SECONDARY_CHAIN=121,
  FTM_GET_CTON = 122,
  FTM_SECOND_CHAIN_TEST_CALL=123,
  FTM_SET_HDET_TRACKING = 124,
  FTM_CONFIGURE_TX_SWEEP_CAL = 125,
  FTM_GET_CDMA_IM2_WITH_SUPPRESSION=126,
  FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL=127,
#ifdef RF_HAS_POWER_MODE_LIB
  FTM_SET_RF_POWER_MODE=128,
#endif /* RF_HAS_POWER_MODE_LIB */
  FTM_SET_TX_SWEEP_STEP_SIZE=129,
  FTM_TX_RX_FREQ_CAL_SWEEP=130,
  FTM_LOAD_RF_NV=131,
  FTM_DO_INTERNAL_DEVICE_CAL=132,
  FTM_DO_DC_CAL=133,
  FTM_DO_XO_DC_CAL = 134,
  FTM_DO_XO_FT_CURVE_CAL = 135,
  FTM_SET_TX_WAVEFORM =136,
  FTM_SET_CALIBRATION_STATE = 137,
  FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2=144, // For getting HDET greater than uint8 but up to uint16
  FTM_SET_BETA_PRIME_SCALING = 151,      /*   151  - retrieving the current Beta Prime Value           */
  FTM_SET_RF_PRIMARY_CHAIN_STATE = 152,  /*   152  - for controlling the state of primary chain        */
  FTM_SET_BLOCK_MASK = 153,
  
  /* GSM Command ID */
  FTM_PEEK_DSP_INPORT = 200,      /*   200  - retrieving the content of the QDSP3 MEMB                                */
  FTM_POKE_DSP_OUTPORT,           /*   201  - send user data to the QDSP3 MEMB                                        */
  FTM_SET_GRFC_ALL,               /*   202  - set all GRFCs (configured as output)                                    */
  FTM_GET_GRFC_ALL,               /*   203  - sense all GRFCs (configured as input/output)                            */
  FTM_GRFC_CONFIG_ALL,            /*   204  - configure the GRFC (timing, direction, polarity, mode)                  */
  FTM_SET_GRFC_DATA,              /*   205  - set a specific GRFC to the specified state (HI/LOW)                     */
  FTM_SET_GRFC_OMODE,             /*   206  - configure the output mode (static/timed) for a GRFC                     */
  FTM_SET_GRFC_TRISTATE,          /*   207  - tristate a specific GRFC                                                */
  FTM_SET_GRFC_POLARITY,          /*   208  - set a specific GRFC polarity (active HI/active LOW)                     */
  FTM_SET_SYNTH_ON,               /*   209  - turn RF synthesizer circuitry on                                        */

  FTM_SET_SYNTH_OFF,              /*   210  - turn RF synthesizer circuitry off                                       */
  FTM_SET_ANTENNA_CONFIG,         /*   211  - configure the antenna switch/duplexer for RX/TX operation (band)        */
  FTM_SET_PA_PROFILE,             /*   212  - download GSM PA power profile for specified power level                 */
  FTM_GET_PA_PROFILE,             /*   213  - retrieve GSM PA power profile for specified power level                 */
  FTM_GET_PA_CONFIG,              /*   214  - retrieve GSM PA configuartion                                           */
  FTM_CONF_MODULATE_DATA,         /*   215  - configure GMSK modulator                                                */
  FTM_SET_TRANSMIT_CONT,          /*   216  - transmit a tone (all 1's to GMSK modulator). Causes RF controls setup   */
  FTM_SET_TRANSMIT_BURST,         /*   217  - transmit user defined data pattern. Causes RF controls setup            */
  FTM_SET_VGA_GAIN_VAL,           /*   218  - program a gain value onto the VGA in RF module                          */
  FTM_SET_LNA_ON,                 /*   219  - turn LNA On                                                             */

  FTM_SET_LNA_OFF,                /*   220  - turns LNA off                                                           */
  FTM_SET_RX_ON,                  /*   221  - perform all operations required to turn on RX circuitry                 */
  FTM_SET_RX_OFF,                 /*   222  - perform all operations required to turn off RX circuitry                */
  FTM_GET_RX_DC_OFFSET,           /*   223  - get the residual DC reading from the RX samples                         */
  FTM_GET_RX_IQ_DATA,             /*   224  - receive a specified number of raw RX samples                            */
  FTM_SET_RX_BURST,               /*   225  - set up Rx burst type                                                    */
  FTM_CHFILT_CONFIG,              /*   226  - configure the channel filter                                            */
  FTM_SET_TX_POWER_LEVEL,         /*   227  - select TX power level by choosing a pre-configured PA LUT values        */
  FTM_GET_PHONE_STATE,            /*   228  - query the state of the phone, such as current channel, power level, etc */
  FTM_SET_GRFC_TIMING,            /*   229  - set the timing information for a GRFC (as well as TX/RX/General)        */
  FTM_GET_GSM_RSSI,               /*   230  - get raw RSSI value                                                      */

  FTM_SET_PA_START_DELTA,         /*   231  - move the Pa ramp start time by the specified delta											*/
  FTM_SET_PA_STOP_DELTA,          /*   232	- move the Pa ramp stop time by the specified delta  	  		              */ 
  FTM_SET_PA_DAC_INPUT,           /*   233  - set pa dac input                                                        */           
  FTM_SET_RX_CONTINUOUS,          /*   234  - set Rx continuous                                                       */
  FTM_DO_GSM_TX_CAL_SWEEP,        /*   235  - Tx cal sweep                                                            */
  FTM_DO_GSM_AUTOCAL,             /*  236 - Do autocal																  */      
  FTM_SET_PATH_DELAY,             /*   237 - Set the path delay for AMAM/AMPM					                      */      
  FTM_SET_AMAM_TABLE_GAIN,        /*   238 - Set the gain for the AMAM table 					                      */      
  FTM_SET_AMAM_TABLE_DC,          /*   239 - Set the DC offset for the AMAM table				                      */      
  FTM_SET_TX_FRAME_MATRIX,        /*   240 - Set the TX frame matrix             				                      */      
  FTM_ENABLE_POLAR_REF_CAL,       /*   241 - Enable polar path reference calibration                                  */       
  FTM_SET_TWOTONE_FREQ_OFFSET,    /*   242 - Sets the frequency offset for the two tone transmit                      */
  FTM_DO_CALPATH_RSB,             /*   243 - Measure calpath RSB 			                                      	  */
  FTM_DO_RTR6250_RSB,             /*   244 - Load VI firmware for RSB                                                 */
  FTM_SET_OPLL_BW,                /*   245 - set Chargepump current registers										  */
  FTM_RX_GAIN_RANGE_CAL,          /*   246 - FTM GSM RX GAIN RANGE Calibration                                        */
  FTM_UNUSED_2,               /*   247 - Fill these in... Removed some unnecessary API */
  FTM_UNUSED_3,               /*   248 - Fill these in... Removed some unnecessary API */
  FTM_UNUSED_4,               /*   249 - Fill these in... Removed some unnecessary API */
  FTM_BASEBAND_BW_CAL,            /*   250 - Baseband Bandwidth calibration                                           */       

  /* FTM GSM Extended Command IDs */
  FTM_GSM_POLAR_TX_SWEEP_CAL = 300,    /*   300 - FTM GSM TX Sweep calibration                                        */
  FTM_TX_RSB_DC_SWEEP,                 /*   301 - FTM GSM TX RSB and DC Sweep calibration                             */          
  FTM_GSM_DETECTOR_CAL,                /*   302 - FTM GSM TX Detector calibration                                     */
  FTM_TX_4xFMOD_SWEEP,                 /*   303 - FTM GSM 4xf MOD calibration                                     */
  FTM_TX_CS_SWEEP,                     /*   304 - FTM GSM TX carrier suppression calibration */
  FTM_ID_MAX 
} ftm_id_type;

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FTM_RF_CMD_H */
