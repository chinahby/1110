#ifndef PM66XXMPPS_H
#define PM66XXMPPS_H
/*===========================================================================


            P M 6 6 X X M P P S   H E A D E R    F I L E

DESCRIPTION
    This file contains functions prototypes and variable/type/constant 
  declarations to support the different voltage regulators inside the 
  Qualcomm Power Manager ICs.
  
Copyright (c) 2003, 2004, 2005 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2006, 2007       by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
                                                                         
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/device/main/latest/driver/inc/pm66XXmpps.h#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/10   kchou   Add workaround to configure mpp using MSME1  
04/09/09   jtn/bt  Added API pm_mpp_config_dtest_output()
01/26/09   Vk      Introduced following APIs
                   pm_config_secure_mpp_config_digital_input()
                   pm_secure_mpp_config_digital_input()
12/17/08   APU     Introduced the APIs:
                   1. pm_secure_mpp_config_i_sink ()
				   2. pm_make_secure__mpp_config_i_sink ()
08/01/08   jtn     Move all proc comm functions to pm_pcil.c
06/27/08   jtn     Merge changes from QSC1100 branch, consolidated 
                   definition for max. number of MPPs
06/24/08   vk      Provided API that maintains the list of MPPs that have shunt
                   capacitors to ground. In the MPP configuration API for 
                   anaolog output this list is used.
03/01/08   jtn     Corrected number of MPPs for Han
02/27/08   jtn     Added support for Kip PMIC
01/07/08   jtn     Merged from QSC6270 branch
(begin QSC6270 changes) 
10/11/07   jnoblet Added MPPs for Han
(end QSC6270 changes) 
11/13/07   jtn     Added pm_config_secure_mpp_config_digital_output and 
				   pm_secure_mpp_config_digital_output to support RPC access
				   to MPP digital outputs
06/18/07   cng     Added an input parameter to pm_mpp_config_analog_output
                   to allow configurable analog output voltage 
05/18/07   hs      Restored the third parameter in pm_mpp_config_digital_input()
09/08/06   cng     Removed API pm_mpp_config_d_test and enum pm_mpp_dtest_in_dbus_type
06/22/06   hs      Removed the third parameter in pm_mpp_config_digital_input()
06/08/06   hs      Added pm_mpp_config_atest API support
11/09/05   cng     Added PM6620 support
10/24/05   cng     Added Panoramix support; Removed MPP config type enum;
                   Corrected number of MPPs for PM6650 
07/25/05   cng     PM7500 support
07/01/05   cng     Added MPP config type enum
03/15/05   cng     For the analog input function, added the functionality to
                   route the MPP input to one of the 3 analog buses 
01/28/04   rmd     Added initial support for multiple PMIC models/tiers.
11/03/03   rmd     Updated the settings for the BIDIRECTIONAL the pull up 
                   resistor.
09/08/03   rmd     enum "pm_mpp_aout_switch_type" was in the wrong order.
08/11/03   rmd     Created.
===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/* Which MPP needs to be configured */
typedef enum
{
  PM_MPP_1,
  PM_MPP_2,
  PM_MPP_3,
  PM_MPP_4,
  PM_MPP_5,
  PM_MPP_6,
  PM_MPP_7,
  PM_MPP_8,
  PM_MPP_9,
  PM_MPP_10,
  PM_MPP_11,
  PM_MPP_12,
  PM_MPP_13,
  PM_MPP_14,
  PM_MPP_15,
  PM_MPP_16,
  PM_MPP_17,
  PM_MPP_18,
  PM_MPP_19,
  PM_MPP_20,
  PM_MPP_21,
  PM_MPP_22,
  PM_MPP_INVALID,
  PM_NUM_MPP_HAN = PM_MPP_4 + 1,
  PM_NUM_MPP_KIP = PM_MPP_4 + 1,
  PM_NUM_MPP_EPIC = PM_MPP_4 + 1,
  PM_NUM_MPP_PM7500 = PM_MPP_22 + 1,  /* Max number of MPP's for PM7500 */
  PM_NUM_MPP_PM6650 = PM_MPP_12 + 1,  /* Max number of MPP's for PM6650 */
  PM_NUM_MPP_PM6658 = PM_MPP_12 + 1,
  PM_NUM_MPP_PANORAMIX = PM_MPP_2 + 1,/* Max number of MPP's for PANORAMIX and PM6640 */
  PM_NUM_MPP_PM6640 = PM_NUM_MPP_PANORAMIX,
  PM_NUM_MPP_PM6620 = PM_NUM_MPP_PANORAMIX
}pm_mpp_which_type;

/*************************************************************************** 
   The types defined bellow are used to configure the following MPPs modes:
   DIGITAL INPUT, DIGITAL OUTPUT and BIDIRECTIONAL.
 ***************************************************************************/
typedef enum
{
  PM_MPP__DLOGIC__LVL_MSME,
  PM_MPP__DLOGIC__LVL_MSMP,
  PM_MPP__DLOGIC__LVL_RUIM,
  PM_MPP__DLOGIC__LVL_MMC,
  PM_MPP__DLOGIC__LVL_VDD,
  PM_MPP__DLOGIC__LVL_INVALID
}pm_mpp_dlogic_lvl_type;

/* MODE = DIGITAL INPUT. Configures the output logic. */
typedef enum
{
  PM_MPP__DLOGIC_IN__DBUS_NONE,
  PM_MPP__DLOGIC_IN__DBUS1,
  PM_MPP__DLOGIC_IN__DBUS2,
  PM_MPP__DLOGIC_IN__DBUS3,
  PM_MPP__DLOGIC_IN__DBUS_INVALID
}pm_mpp_dlogic_in_dbus_type;

/* MODE = DIGITAL OUT. Configures the output logic. */
typedef enum
{
  PM_MPP__DLOGIC_OUT__CTRL_LOW,    /* MPP OUTPUT= LOGIC LOW         */
  PM_MPP__DLOGIC_OUT__CTRL_HIGH,   /* MPP OUTPUT= LOGIC HIGH        */
  PM_MPP__DLOGIC_OUT__CTRL_MPP,    /* MPP OUTPUT= CORRESPONDING 
                                                  MPP INPUT         */
  PM_MPP__DLOGIC_OUT__CTRL_NOT_MPP,/* MPP OUTPUT= CORRESPONDING
                                                  INVERTED MPP INPUT*/
  PM_MPP__DLOGIC_OUT__CTRL_INVALID
}pm_mpp_dlogic_out_ctrl_type;

/* MODE = BIDIRECTIONAL. Configures the pull up resistor. */
typedef enum
{
  PM_MPP__DLOGIC_INOUT__PUP_1K,  /* PULL UP = 1  K Ohms*/
  PM_MPP__DLOGIC_INOUT__PUP_3K,  /* PULL UP = 3  K Ohms*/
  PM_MPP__DLOGIC_INOUT__PUP_10K, /* PULL UP = 10 K Ohms*/
  PM_MPP__DLOGIC_INOUT__PUP_30K, /* PULL UP = 30 K Ohms*/   
  PM_MPP__DLOGIC_INOUT__PUP_INVALID
}pm_mpp_dlogic_inout_pup_type;

/* MODE = DIGITAL TEST OUTPUT. */
typedef enum
{
  PM_MPP__DLOGIC_OUT__DBUS1,
  PM_MPP__DLOGIC_OUT__DBUS2,
  PM_MPP__DLOGIC_OUT__DBUS3,
  PM_MPP__DLOGIC_OUT__DBUS_INVALID
}pm_mpp_dlogic_out_dbus_type;

/*************************************************************************** 
   The types defined bellow are used to configure the following MPPs modes:
   ANALOG INPUT.
 ***************************************************************************/
typedef enum
{
  PM_MPP__AIN__CH_AMUX5,
  PM_MPP__AIN__CH_AMUX6,
  PM_MPP__AIN__CH_AMUX7,
  PM_MPP__AIN__CH_AMUX8,
  PM_MPP__AIN__CH_AMUX9,
  PM_MPP__AIN__CH_ABUS1,
  PM_MPP__AIN__CH_ABUS2,
  PM_MPP__AIN__CH_ABUS3,
  PM_MPP__AIN__CH_INVALID
}pm_mpp_ain_ch_type;

/*************************************************************************** 
   The types defined bellow are used to configure the following MPPs modes:
   ANALOG OUTPUT.
 ***************************************************************************/
typedef enum
{     
  PM_MPP__AOUT__LEVEL_VREF_1p25_Volts,    
  PM_MPP__AOUT__LEVEL_VREF_0p625_Volts,
  PM_MPP__AOUT__LEVEL_VREF_2p50_Volts,
  PM_MPP__AOUT__LEVEL_INVALID
}pm_mpp_aout_level_type;

typedef enum
{  
  PM_MPP__AOUT__SWITCH_OFF,
  PM_MPP__AOUT__SWITCH_ON,
  PM_MPP__AOUT__SWITCH_ON_IF_MPP_HIGH,
  PM_MPP__AOUT__SWITCH_ON_IF_MPP_LOW,
  PM_MPP__AOUT__SWITCH_INVALID
}pm_mpp_aout_switch_type;

/*************************************************************************** 
   The types defined bellow are used to configure the following MPPs modes:
   CURRENT SINK.
 ***************************************************************************/
typedef enum
{
  PM_MPP__I_SINK__LEVEL_5mA,
  PM_MPP__I_SINK__LEVEL_10mA,
  PM_MPP__I_SINK__LEVEL_15mA,
  PM_MPP__I_SINK__LEVEL_20mA,
  PM_MPP__I_SINK__LEVEL_25mA,
  PM_MPP__I_SINK__LEVEL_30mA,
  PM_MPP__I_SINK__LEVEL_35mA,
  PM_MPP__I_SINK__LEVEL_40mA,
  PM_MPP__I_SINK__LEVEL_INVALID
}pm_mpp_i_sink_level_type;

typedef enum
{
  PM_MPP__I_SINK__SWITCH_DIS,
  PM_MPP__I_SINK__SWITCH_ENA,
  PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_HIGH,
  PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_LOW,
  PM_MPP__I_SINK__SWITCH_INVALID
}pm_mpp_i_sink_switch_type;
/***************************************************************************/

/* ATEST*/
typedef enum
{
  PM_MPP__CH_ATEST1,
  PM_MPP__CH_ATEST2,
  PM_MPP__CH_ATEST3,
  PM_MPP__CH_ATEST_INVALID
}pm_mpp_ch_atest_type;

/*===========================================================================
                          FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION pm_mpp_config_digital_input                    EXTERNAL FUNCTION

DESCRIPTION
  Configure the selected Multi Purpose pin (MPP) to be a digital input pin.
       
  example:
    "Configure MPP5 to be a digital input, set the voltage reference
     to MSMP VREG and drive Data bus 2"
        
    errFlag = pm_mpp_config_digital_input(PM_MPP_5, 
                                           PM_MPP__DLOGIC__LVL_MSMP,
                                            PM_MPP__DLOGIC_IN__DBUS2);

  Note: The MPPs can be paired as MPP(input) -> MPP(output) or  
        MPP(Bidirec) <-> MPP(Bidirec) according to the table bellow.
                                           
        MPP level translator pairs:
            MPP1   <->  MPP2
            MPP3   <->  MPP4
            MPP5   <->  MPP6
            MPP7   <->  MPP8
            MPP9   <->  MPP10
            MPP11  <->  MPP12 
                                                                            
PARAMETERS
  1) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
            PM_MPP_13
            PM_MPP_14
            PM_MPP_15
            PM_MPP_16

  2) Parameter name: level. 
     - The logic level reference that we want to use with this MPP.

     Parameter type: pm_mpp_dlogic_lvl_type (enum).
     - Valid Inputs:   
            PM_MPP__DLOGIC__LVL_MSME
            PM_MPP__DLOGIC__LVL_MSMP
            PM_MPP__DLOGIC__LVL_RUIM
            PM_MPP__DLOGIC__LVL_MMC
            PM_MPP__DLOGIC__LVL_VDD     

  3) Parameter name: dbus. 
     - Which data line will the MPP drive. Please note that only one MPP 
       can be assigned per DBUS.

     Parameter type:  (enum).
     - Valid Inputs:
          PM_MPP__DLOGIC_IN__DBUS_NONE
          PM_MPP__DLOGIC_IN__DBUS1
          PM_MPP__DLOGIC_IN__DBUS2
          PM_MPP__DLOGIC_IN__DBUS3   
               
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter 3 is out of range.
      PM_ERR_FLAG__DBUS_IS_BUSY_MODE    = The DBUS is already in use by
                                          another MPP.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.  
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_mpp_config_digital_input(
                                          pm_mpp_which_type           mpp,
                                          pm_mpp_dlogic_lvl_type      level,
                                          pm_mpp_dlogic_in_dbus_type  dbus);

/*===========================================================================
FUNCTION pm_QSC11x0_mpp_config_digital_input_MSME1              EXTERNAL FUNCTION

DESCRIPTION
  This API is used as a workaround for QSC11x0 only.
  Configure the selected Multi Purpose pin (MPP) to be a digital input pin using MSME1 VREG.
       
  example:
    "Configure MPP5 to be a digital input, set the voltage reference
     to MSMP VREG and drive Data bus 2"
        
    errFlag = pm_mpp_config_digital_input(PM_MPP_5, 
                                            PM_MPP__DLOGIC_IN__DBUS2);

  Note: The MPPs can be paired as MPP(input) -> MPP(output) or  
        MPP(Bidirec) <-> MPP(Bidirec) according to the table bellow.
                                           
        MPP level translator pairs:
            MPP1   <->  MPP2
            MPP3   <->  MPP4
            MPP5   <->  MPP6
            MPP7   <->  MPP8
            MPP9   <->  MPP10
            MPP11  <->  MPP12 
                                                                            
PARAMETERS
  1) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
 

  2) Parameter name: dbus. 
     - Which data line will the MPP drive. Please note that only one MPP 
       can be assigned per DBUS.

     Parameter type:  (enum).
     - Valid Inputs:
          PM_MPP__DLOGIC_IN__DBUS_NONE
          PM_MPP__DLOGIC_IN__DBUS1
          PM_MPP__DLOGIC_IN__DBUS2
          PM_MPP__DLOGIC_IN__DBUS3   
               
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter 3 is out of range.
      PM_ERR_FLAG__DBUS_IS_BUSY_MODE    = The DBUS is already in use by
                                          another MPP.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.  
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  Must QSC11x0 can call this API.
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_QSC11x0_mpp_config_digital_input_MSME1(pm_mpp_which_type           mpp,
                                             pm_mpp_dlogic_in_dbus_type  dbus);


/*===========================================================================
FUNCTION pm_mpp_config_digital_output                      EXTERNAL FUNCTION

DESCRIPTION
  Configure the selected Multi Purpose pin (MPP) to be a digital output pin.
  
  example:
    " Configure MPP10 to be a digital output.
      Set the MPP voltage reference to RUIM VREG.
      The Digital Ouput is equal the input of MPP9(See parameter 3
                                                   description)"
      
    err |= pm_mpp_config_digital_output(PM_MPP_10, 
                                         PM_MPP__DLOGIC__LVL_RUIM, 
                                          PM_MPP__DLOGIC_OUT__CTRL_MPP);                                            

  Note: The MPPs can be paired as MPP(input) -> MPP(output) or  
        MPP(Bidirec) <-> MPP(Bidirec) according to the table bellow.
                                           
        MPP level translator pairs:
            MPP1   <->  MPP2
            MPP3   <->  MPP4
            MPP5   <->  MPP6
            MPP7   <->  MPP8
            MPP9   <->  MPP10
            MPP11  <->  MPP12 
            
PARAMETERS
  1) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
            PM_MPP_13
            PM_MPP_14
            PM_MPP_15
            PM_MPP_16

  2) Parameter name: level. 
     - The logic level reference that we want to use with this MPP pin.

     Parameter type: pm_mpp_dlogic_lvl_type (enum).
     - Valid Inputs:
            PM_MPP__DLOGIC__LVL_MSME
            PM_MPP__DLOGIC__LVL_MSMP
            PM_MPP__DLOGIC__LVL_RUIM
            PM_MPP__DLOGIC__LVL_MMC
            PM_MPP__DLOGIC__LVL_VDD       

  3) Parameter name: output_ctrl. 
     - The output setting of the selected MPP.

     Parameter type: pm_mpp_dlogic_out_ctrl_type (enum).     
     - Valid Inputs:
        PM_MPP__DLOGIC_OUT__CTRL_LOW,    = LOGIC LOW.
        PM_MPP__DLOGIC_OUT__CTRL_HIGH,   = LOGIC HIGH.
        PM_MPP__DLOGIC_OUT__CTRL_MPP,    = CORRESPONDING MPP INPUT 
                                           (see the note regarding MPP pairs
                                            within the decription section).         
        PM_MPP__DLOGIC_OUT__CTRL_NOT_MPP = CORRESPONDING INVERTED MPP INPUT
                                           (see the note regarding MPP pairs
                                            within the decription section).         
     
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter three is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.          
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_mpp_config_digital_output(
                                    pm_mpp_which_type           mpp,
                                    pm_mpp_dlogic_lvl_type      level,
                                    pm_mpp_dlogic_out_ctrl_type output_ctrl);


/*===========================================================================
FUNCTION pm_QSC11x0_mpp_config_digital_output_MSME1                 EXTERNAL FUNCTION

DESCRIPTION
  This API is used as a workaround for QSC11x0 only.
  Configure the selected Multi Purpose pin (MPP) to be a digital output pin using MSME1 VREG.
  
  example:
    " Configure MPP10 to be a digital output.
      Set the MPP voltage reference to MSME1 VREG.
      The Digital Ouput is equal the input of MPP9(See parameter 3
                                                   description)"
      
    err |= pm_mpp_config_digital_output(PM_MPP_10, 
                                          PM_MPP__DLOGIC_OUT__CTRL_MPP);
                                          
  Note: The MPPs can be paired as MPP(input) -> MPP(output) or  
        MPP(Bidirec) <-> MPP(Bidirec) according to the table bellow.
                                           
        MPP level translator pairs:
            MPP1   <->  MPP2
            MPP3   <->  MPP4
            MPP5   <->  MPP6
            MPP7   <->  MPP8
            MPP9   <->  MPP10
            MPP11  <->  MPP12 

PARAMETERS
  1) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
   

  2) Parameter name: output_ctrl. 
     - The output setting of the selected MPP.

     Parameter type: pm_mpp_dlogic_out_ctrl_type (enum).     
     - Valid Inputs:
        PM_MPP__DLOGIC_OUT__CTRL_LOW,    = LOGIC LOW.
        PM_MPP__DLOGIC_OUT__CTRL_HIGH,   = LOGIC HIGH.
        PM_MPP__DLOGIC_OUT__CTRL_MPP,    = CORRESPONDING MPP INPUT 
                                           (see table bellow).
        PM_MPP__DLOGIC_OUT__CTRL_NOT_MPP = CORRESPONDING INVERTED MPP INPUT
                                           (see table bellow).        
                                                
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter three is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.          
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  Must QSC11x0 can call this API.
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_QSC11x0_mpp_config_digital_output_MSME1(
                                             pm_mpp_which_type           mpp,
                                             pm_mpp_dlogic_out_ctrl_type output_ctrl);


/*===========================================================================
FUNCTION pm_mpp_config_digital_inout                    EXTERNAL FUNCTION

DESCRIPTION
     Configure the selected Multi Purpose pin (MPP) to be a digital 
  bidirectional pin.
                                           
  example:
  
      Configure MPP11 and MPP12 to be bidirectionial(see the note bellow).
      Set their logic voltage reference to RUIM VREG.
      Set up the pull up ressitor setting to 30K Ohms.
      
      err =  pm_mpp_config_digital_inout( PM_MPP_11, 
                                          PM_MPP__DLOGIC__LVL_RUIM,
                                          PM_MPP__DLOGIC_INOUT__PUP_30K);

      err |= pm_mpp_config_digital_inout( PM_MPP_12, 
                                          PM_MPP__DLOGIC__LVL_RUIM,
                                          PM_MPP__DLOGIC_INOUT__PUP_30K);

  Note: The MPPs can be paired as MPP(input) -> MPP(output) or  
        MPP(Bidirec) <-> MPP(Bidirec) according to the table bellow.
                                           
        MPP level translator pairs:
            MPP1   <->  MPP2
            MPP3   <->  MPP4
            MPP5   <->  MPP6
            MPP7   <->  MPP8
            MPP9   <->  MPP10
            MPP11  <->  MPP12
            
PARAMETERS
  1) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
            PM_MPP_13
            PM_MPP_14
            PM_MPP_15
            PM_MPP_16
            
  2) Parameter name: level. 
     - The logic level reference that we want to use with this MPP.

     Parameter type: pm_mpp_dlogic_lvl_type (enum).
     - Valid Inputs:
            PM_MPP__DLOGIC__LVL_MSME
            PM_MPP__DLOGIC__LVL_MSMP
            PM_MPP__DLOGIC__LVL_RUIM
            PM_MPP__DLOGIC__LVL_MMC
            PM_MPP__DLOGIC__LVL_VDD         

  3) Parameter name: pull_up. 
     - The pull-up resistor setting of the selected MPP.

     Parameter type: pm_mpp_dlogic_inout_pup_type (enum).
     - Valid Inputs:
            PM_MPP__DLOGIC_INOUT__PUP_30K  = 30K Ohms
            PM_MPP__DLOGIC_INOUT__PUP_10K  = 10K Ohms
            PM_MPP__DLOGIC_INOUT__PUP_3K   = 3K  Ohms
            PM_MPP__DLOGIC_INOUT__PUP_1K   = 1K  Ohms     
          
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
       PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
       PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
       PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter three is out of range.
       PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                           version of the PMIC.         
       PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                           with the PMIC.      
       PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_mpp_config_digital_inout(
                                    pm_mpp_which_type            mpp,
                                    pm_mpp_dlogic_lvl_type       level,
                                    pm_mpp_dlogic_inout_pup_type pull_up);

/*===========================================================================
FUNCTION pm_mpp_config_dtest_output                      EXTERNAL FUNCTION

DESCRIPTION
  Configure the selected Multi Purpose pin (MPP) to be a digital test output pin.
  
  example:
    " Configure MPP10 to be a digital test output.
      Set the MPP voltage reference to RUIM VREG and connect it to DBUS1.
            
    err |= pm_mpp_config_dtest_output(PM_MPP_10, 
                                         PM_MPP__DLOGIC__LVL_RUIM, 
                                          PM_MPP__DLOGIC_IN__DBUS1);
                                          
PARAMETERS
  1) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12

  2) Parameter name: level. 
     - The logic level reference that we want to use with this MPP.

     Parameter type: pm_mpp_dlogic_lvl_type (enum).
     - Valid Inputs:
            PM_MPP__DLOGIC__LVL_MSME
            PM_MPP__DLOGIC__LVL_MSMP
            PM_MPP__DLOGIC__LVL_RUIM
            PM_MPP__DLOGIC__LVL_MMC
            PM_MPP__DLOGIC__LVL_VDD     

  3) Parameter name: dbus. 
     - Which data line will the MPP drive.

     Parameter type: pm_mpp_dlogic_out_dbus_type (enum).
     - Valid Inputs:
           PM_MPP__DLOGIC_OUT__DBUS1,
           PM_MPP__DLOGIC_OUT__DBUS2,
  	       PM_MPP__DLOGIC_OUT__DBUS3,
           PM_MPP__DLOGIC_OUT__DBUS4,
           PM_MPP__DLOGIC_OUT__DBUS_INVALID      
                                                
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter three is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.          
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
pm_err_flag_type pm_mpp_config_dtest_output(
                                             pm_mpp_which_type           mpp,
                                             pm_mpp_dlogic_lvl_type      level,
                                             pm_mpp_dlogic_out_dbus_type  dbus);


/*===========================================================================
FUNCTION pm_mpp_config_analog_input                    EXTERNAL FUNCTION

DESCRIPTION
    This function configures the selected MPP as an Analog Input.
       
    example:
    
        Configure MPP4 to be an analog input and connected it to analog 
        MUX6.
        
        err = pm_mpp_config_analog_input(PM_MPP_6,PM_MPP__AIN__CH_AMUX9);

PARAMETERS
  1) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
            PM_MPP_13
            PM_MPP_14
            PM_MPP_15
            PM_MPP_16

  2) Parameter name: ch_select. 
     - Which analog mux or analog bus will the selected MPP be routed to.

     Parameter type: (enum).
     - Valid Inputs:
            PM_MPP__AIN__CH_AMUX5
            PM_MPP__AIN__CH_AMUX6
            PM_MPP__AIN__CH_AMUX7
            PM_MPP__AIN__CH_AMUX8
            PM_MPP__AIN__CH_AMUX9
            PM_MPP__AIN__CH_ABUS1
            PM_MPP__AIN__CH_ABUS2
            PM_MPP__AIN__CH_ABUS3
     
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.                  
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_mpp_config_analog_input(
                                    pm_mpp_which_type    mpp,
                                    pm_mpp_ain_ch_type   ch_select);


/*===========================================================================
FUNCTION pm_mpp_config_analog_output                    EXTERNAL FUNCTION

DESCRIPTION
    This function configures the selected MPP as an analog output.
        
    example:
    
        Configure MPP14 to be an analog out and turn it on.        
        
        err = pm_mpp_config_analog_output(PM_MPP_14,PM_MPP__AOUT__SWITCH_ON);

PARAMETERS
  1) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
            PM_MPP_13
            PM_MPP_14
            PM_MPP_15
            PM_MPP_16            
            
  2) Parameter name: level. 
     - Specifies the analog output reference voltage level 

     Parameter type: pm_mpp_aout_level_type (enum).
     - Valid Inputs:                        
        PM_MPP__AOUT__LEVEL_VREF_1p25_Volts = 1.25V,
        PM_MPP__AOUT__LEVEL_VREF_0p625_Volts = 0.625V,
        PM_MPP__AOUT__LEVEL_VREF_2p50_Volts = 2.50V,

  3) Parameter name: OnOff. 
     - This option allows the user to enable/disable the MPP output.

     Parameter type: pm_mpp_aout_switch_type (enum).
     - Valid Inputs:                        
        PM_MPP__AOUT__SWITCH_ON,
        PM_MPP__AOUT__SWITCH_OFF
        PM_MPP__AOUT__SWITCH_ON_IF_MPP_HIGH
        PM_MPP__AOUT__SWITCH_ON_IF_MPP_LOW                 
     
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter three is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.        
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_mpp_config_analog_output(
                                    pm_mpp_which_type        mpp,
                                    pm_mpp_aout_level_type   level,
                                    pm_mpp_aout_switch_type  OnOff);

/*===========================================================================
FUNCTION pm_mpp_config_i_sink                           EXTERNAL FUNCTION

DESCRIPTION
  This function configures the selected MPP as a current sink.
  
  example:
    
    Configure MPP7 to be a current sink. Set the current sink to 25mA and
    enable the current sink.
    
    err = pm_mpp_config_i_sink(PM_MPP_15, 
                                    PM_MPP__I_SINK__LEVEL_25mA,
                                        PM_MPP__I_SINK__SWITCH_ENA);
 

PARAMETERS
  1) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12

  2) Parameter name: level. 
     - How much current we want to allow the MPP to sink.

     Parameter type: pm_mpp_i_sink_level_type (enum).
     - Valid Inputs:
            PM_MPP__I_SINK__LEVEL_5mA
            PM_MPP__I_SINK__LEVEL_10mA
            PM_MPP__I_SINK__LEVEL_15mA
            PM_MPP__I_SINK__LEVEL_20mA
            PM_MPP__I_SINK__LEVEL_25mA
            PM_MPP__I_SINK__LEVEL_30mA
            PM_MPP__I_SINK__LEVEL_35mA
            PM_MPP__I_SINK__LEVEL_40mA     

  3) Parameter name: OnOff. 
     - This option allows the user to enable/disable current sink.

     Parameter type: pm_mpp_i_sink_switch_type (enum).
     - Valid Inputs:
            PM_MPP__I_SINK__SWITCH_DIS
            PM_MPP__I_SINK__SWITCH_ENA
            PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_HIGH
            PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_LOW     
                 
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter 3 is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.         
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_mpp_config_i_sink(
                                    pm_mpp_which_type         mpp,
                                    pm_mpp_i_sink_level_type  level,
                                    pm_mpp_i_sink_switch_type OnOff);


/*===========================================================================
FUNCTION pm_mpp_config_atest                           EXTERNAL FUNCTION

DESCRIPTION
    This function configures the selected MPP as ATEST.
       
PARAMETERS
  1) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12
            PM_MPP_13
            PM_MPP_14
            PM_MPP_15
            PM_MPP_16

  2) Parameter name: atest_select. 
     - Which atest will the selected MPP be routed to.

     Parameter type: (enum).
     - Valid Inputs:
            PM_MPP__CH_ATEST1
            PM_MPP__CH_ATEST2
            PM_MPP__CH_ATEST3
     
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.                  
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_mpp_config_atest(
                                    pm_mpp_which_type    mpp,
                                    pm_mpp_ch_atest_type   atest_select);

/*=============================================================================
Function: pm_secure_mpp_config_digital_output
	Configure one of the PMIC MPPs as a digital output.  This function
	is intended to be used by the APPs processor through RPC.
Parameters: 
	pm_mpp_which_type           mpp
		Which MPP to configure
		Valid parameters PM_MPP_1..PM_MPP_16
	pm_mpp_dlogic_lvl_type      level,
		Valid Inputs:
            PM_MPP__DLOGIC__LVL_MSME
            PM_MPP__DLOGIC__LVL_MSMP
            PM_MPP__DLOGIC__LVL_RUIM
            PM_MPP__DLOGIC__LVL_MMC
            PM_MPP__DLOGIC__LVL_VDD       
	pm_mpp_dlogic_out_ctrl_type output_ctrl
		Valid Inputs:
			PM_MPP__DLOGIC_OUT__CTRL_LOW,    = LOGIC LOW.
			PM_MPP__DLOGIC_OUT__CTRL_HIGH,   = LOGIC HIGH.
			PM_MPP__DLOGIC_OUT__CTRL_MPP,    = CORRESPONDING MPP INPUT 
											   (see the note regarding MPP pairs
												within the decription section).         
			PM_MPP__DLOGIC_OUT__CTRL_NOT_MPP = CORRESPONDING INVERTED MPP INPUT
											   (see the note regarding MPP pairs
												within the decription section).         

Returns:
	PM_ERR_FLAG__SUCCESS                0x0000
	PM_ERR_FLAG__PAR1_OUT_OF_RANGE      0x0001
	PM_ERR_FLAG__PAR2_OUT_OF_RANGE      0x0002
	PM_ERR_FLAG__PAR3_OUT_OF_RANGE      0x0004
	PM_ERR_FLAG__SBI_OPT_ERR            0x0080
	PM_ERR_FLAG__FEATURE_NOT_SUPPORTED  0x0100
	PM_ERR_FLAG__INVALID_PMIC_MODEL     0x0200
	PM_ERR_FLAG__RPC_SECURITY_ERR       0x0800

=============================================================================*/
extern pm_err_flag_type pm_secure_mpp_config_digital_output(
                                    pm_mpp_which_type           mpp,
                                    pm_mpp_dlogic_lvl_type      level,
                                    pm_mpp_dlogic_out_ctrl_type output_ctrl);
/*~ FUNCTION pm_secure_mpp_config_digital_output */


/*============================================================================
Function:  pm_config_secure_mpp_config_digital_output
	Configure PMIC MPPs for secure access through RPC.
	Call this function once for each MPP
	Subsequent calls for the same MPP will overwrite old settings.
Parameters:
	pm_mpp_which_type           mpp,
		The MPP that is to be configured as allowed for remote access
	pm_mpp_dlogic_lvl_type*      levels,
		Pointer to an array of levels that are allowed 
	uint numOfLevels,
		Number of levels
	pm_mpp_dlogic_out_ctrl_type* output_ctrl,
		Pointer to an arral of output controls that are allowed
	uint numOfCtrls
		Number of output controls
Example:
	pm_mpp_dlogic_out_ctrl_type out_ctrl[3];
	pm_mpp_dlogic_lvl_type levels[3];

	out_ctrl[0] = xxx;
	out_ctrl[1] = xxx;
	out_ctrl[2] = xxx;
	levels[0] = yyy;
	levels[1] = yyy;
	levels[2] = yyy;
	err = pm_config_secure_mpp_config_digital_output(PM_MPP_1, levels, 3, out_ctrl, 3);
=============================================================================*/
extern pm_err_flag_type pm_config_secure_mpp_config_digital_output(
                                    pm_mpp_which_type            mpp,
                                    pm_mpp_dlogic_lvl_type*      levels,
                                    uint16 numOfLevels,
                                    pm_mpp_dlogic_out_ctrl_type* output_ctrl,
                                    uint16 numOfCtrls);

/*===========================================================================
FUNCTION pm_mpp_set_list_mpp_with_shunt_cap                 EXTERNAL FUNCTION

DESCRIPTION
  This function can be used to create an array of MPPs that have shunt 
  capacitors to ground.
  
  example:
    
    If mpp8 has shunt capacitor to ground.
    
    err = pm_mpp_set_list_mpp_with_shunt_cap(PM_MPP_8);

PARAMETERS
  1) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter 3 is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.         
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/

pm_err_flag_type pm_mpp_set_list_mpp_with_shunt_cap(pm_mpp_which_type mpp);

/*===========================================================================

FUNCTION chg_get_mpp_with_shunt_cap_list_status             EXTERNAL FUNCTION

DESCRIPTION
    This function returns the 32bit status which represents that the 
    corresponding mpp has shunt capacitor to ground.

    e.g if MPP8 and MPP1 has shunt capacitor to ground then status would be
        status = 00000000 00000000 00000000 10000001

INPUT PARAMETER
  None.

RETURN VALUE
  Description:
  - 32 bit status which represents that the corresponding mpp has shunt 
    capacitor to ground.

  Return value type: uint32

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 chg_get_mpp_with_shunt_cap_list_status(void);


/*============================================================================
Function:  pm_make_secure_mpp_config_i_sink
    Configure I[sink] for PMIC MPPs for secure access through RPC.
    Call this function once for each MPP
    called from TMC to configure security level
    Subsequent calls for the same MPP will overwrite old settings.

Note:
    To be called should be called by the root of trust during boot. 

Parameters:
    pm_mpp_which_type           mpp,
        The MPP that is to be configured as allowed for remote access
    boolean                     i_sink_en_dis
        TRUE: Allow the pm_mpp_config_i_sink via RPCed version of 
		      pm_secure_mpp_config_i_sink by Apps processor.
        FALSE: Disallow  pm_mpp_config_i_sink to be called by Apps processor.

Example:
    Disable pm_make_secure__mpp_config_i_sink for MPP1 but enable MPP2 to 
	be called by APPs processor
    err = pm_make_secure_mpp_config_i_sink ( PM_MPP_1 , DISABLE ) ;
	err = pm_make_secure_mpp_config_i_sink ( PM_MPP_2 , ENABLE ) ;
=============================================================================*/
pm_err_flag_type 
pm_config_secure_mpp_config_i_sink ( pm_mpp_which_type  mpp ,
                                     boolean i_sink_en_dis ) ;

/*===========================================================================
FUNCTION pm_secure_mpp_config_i_sink: Secure version of pm_mpp_config_i_sink                           
EXTERNAL FUNCTION

DESCRIPTION
  This secure function configures the selected MPP as a current sink. This is 
  the API that should be remoted to the application processor.
  
Example:
  Configure MPP7 to be a current sink. Set the current sink to 25mA and
  enable the current sink.
    
  err = pm_secure_mpp_config_i_sink ( PM_MPP_15, 
                                      PM_MPP__I_SINK__LEVEL_25mA,
								      PM_MPP__I_SINK__SWITCH_ENA
									 ) ;
 
NOTE:
  1) Do not call this from the modem side, use pm_mpp_config_i_sink instead.
  2) If this mpp is not configured to be used from AP by
     pm_make_secure__mpp_config_i_sink() , the call will fail. 
  3) DO NOT RPC the API from the pm_mpp_config_i_sink()

PARAMETERS
  1) Parameter name: mpp.
     - Which MPP we want to configure.

     Parameter type: pm_mpp_which_type (enum).
     - Valid Inputs:
     
            PM_MPP_1
            PM_MPP_2
            PM_MPP_3
            PM_MPP_4
            PM_MPP_5
            PM_MPP_6
            PM_MPP_7
            PM_MPP_8
            PM_MPP_9
            PM_MPP_10
            PM_MPP_11
            PM_MPP_12

  2) Parameter name: level. 
     - How much current we want to allow the MPP to sink.

     Parameter type: pm_mpp_i_sink_level_type (enum).
     - Valid Inputs:
            PM_MPP__I_SINK__LEVEL_5mA
            PM_MPP__I_SINK__LEVEL_10mA
            PM_MPP__I_SINK__LEVEL_15mA
            PM_MPP__I_SINK__LEVEL_20mA
            PM_MPP__I_SINK__LEVEL_25mA
            PM_MPP__I_SINK__LEVEL_30mA
            PM_MPP__I_SINK__LEVEL_35mA
            PM_MPP__I_SINK__LEVEL_40mA     

  3) Parameter name: OnOff. 
     - This option allows the user to enable/disable current sink.

     Parameter type: pm_mpp_i_sink_switch_type (enum).
     - Valid Inputs:
            PM_MPP__I_SINK__SWITCH_DIS
            PM_MPP__I_SINK__SWITCH_ENA
            PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_HIGH
            PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_LOW     
                 
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
      PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter 3 is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.         
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
pm_err_flag_type 
pm_secure_mpp_config_i_sink (
                                 pm_mpp_which_type         mpp ,
                                 pm_mpp_i_sink_level_type  level ,
                                 pm_mpp_i_sink_switch_type OnOff 
					        ) ;
/*~ FUNCTION pm_secure_mpp_config_i_sink */

/*=============================================================================
Function: pm_secure_mpp_config_digital_input
	Configure one of the PMIC MPPs as a digital input.  This function
	is intended to be used by the APPs processor through RPC.
Parameters: 
	pm_mpp_which_type           mpp
		Which MPP to configure
		Valid parameters PM_MPP_1..PM_MPP_16
	pm_mpp_dlogic_lvl_type      level,
		Valid Inputs:
            PM_MPP__DLOGIC__LVL_MSME
            PM_MPP__DLOGIC__LVL_MSMP
            PM_MPP__DLOGIC__LVL_RUIM
            PM_MPP__DLOGIC__LVL_MMC
            PM_MPP__DLOGIC__LVL_VDD 
	pm_mpp_dlogic_in_dbus_type   dbus
        Which data line will the MPP drive. Please note that only one MPP 
          can be assigned per DBUS.
        Valid Inputs:
            PM_MPP__DLOGIC_IN__DBUS_NONE
            PM_MPP__DLOGIC_IN__DBUS1
            PM_MPP__DLOGIC_IN__DBUS2
            PM_MPP__DLOGIC_IN__DBUS3  
          
Returns:
	PM_ERR_FLAG__SUCCESS                0x0000
	PM_ERR_FLAG__PAR1_OUT_OF_RANGE      0x0001
	PM_ERR_FLAG__PAR2_OUT_OF_RANGE      0x0002
	PM_ERR_FLAG__PAR3_OUT_OF_RANGE      0x0004
	PM_ERR_FLAG__SBI_OPT_ERR            0x0080
	PM_ERR_FLAG__FEATURE_NOT_SUPPORTED  0x0100
	PM_ERR_FLAG__INVALID_PMIC_MODEL     0x0200
	PM_ERR_FLAG__RPC_SECURITY_ERR       0x0800

=============================================================================*/
pm_err_flag_type pm_secure_mpp_config_digital_input(
                                    pm_mpp_which_type           mpp,
                                    pm_mpp_dlogic_lvl_type      level,
                                    pm_mpp_dlogic_in_dbus_type  dbus);
/*~ FUNCTION pm_secure_mpp_config_digital_input */


/*============================================================================
Function:  pm_config_secure_mpp_config_digital_input
	Configure PMIC MPPs for secure access through RPC.
	Call this function once for each MPP
	Subsequent calls for the same MPP will overwrite old settings.
Parameters:
	pm_mpp_which_type           mpp,
		The MPP that is to be configured as allowed for remote access
	pm_mpp_dlogic_lvl_type*      levels,
		Pointer to an array of levels that are allowed 
	uint numOfLevels,
		Number of levels
	pm_mpp_dlogic_in_dbus_type* data_line,
		Pointer to an array of data lines that MPP can drive.
	uint numOfDataLines
		Number of data lines
Example:
	pm_mpp_dlogic_in_dbus_type data_line[3];
	pm_mpp_dlogic_lvl_type levels[3];

	data_line[0] = xxx;
	data_line[1] = xxx;
	data_line[2] = xxx;
	levels[0] = yyy;
	levels[1] = yyy;
	levels[2] = yyy;
	err = pm_config_secure_mpp_config_digital_input(PM_MPP_1, levels, 3, data_line, 3);
=============================================================================*/
pm_err_flag_type pm_config_secure_mpp_config_digital_input(
                                    pm_mpp_which_type            mpp,
                                    pm_mpp_dlogic_lvl_type*      levels,
                                    uint16 numOfLevels,
                                    pm_mpp_dlogic_in_dbus_type*  data_line,
                                    uint16 numOfDataLines);



#endif /* PM66XXMPPS_H */
